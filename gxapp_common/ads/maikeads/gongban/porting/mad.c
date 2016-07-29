/*
 * mad.c
 *
 *  Created on: 2015-4-2
 *      Author: Brant
 */
#include "app_common_porting_stb_api.h"

#include "mad_cfg.h"

#if ENABLE_M_AD
#include "cJSON.h"
#include "mad_interface.h"
#include "mad_util.h"
#include "mad.h"
#include "mad_ts_sec.h"
#include "maike_ads_porting.h"

#define MAD_VERSION_INFO_FILE  ("/home/gx/mad_version.dat")

/***********************************************************************************
 *                              m-ad place configure                               *
 ***********************************************************************************/
/*static*/ MadPlaceInfo mad_place_info[M_AD_PLACE_NUM] =
{
	{(U32)M_AD_PLACE_ID_BOOT_AV,		MAD_TYPE_BOOT,		128000,		720,	576,	TRUE},
	{(U32)M_AD_PLACE_ID_BOOT_PIC,		MAD_TYPE_BOOT,		128000,		720,	576,	TRUE},
	{(U32)M_AD_PLACE_ID_SUBTITLE,		MAD_TYPE_SUBTITLE,	1000,		0,		0,		FALSE},
	{(U32)M_AD_PLACE_ID_CORNER,			MAD_TYPE_CORNER,	50000,		80,		80,		FALSE},
	{(U32)M_AD_PLACE_ID_MAIN_MUNU,		MAD_TYPE_APP,		64000,		245,	110,	FALSE},
	{(U32)M_AD_PLACE_ID_PROG_LIST,		MAD_TYPE_APP,		64000,		245,	110,	FALSE},
	{(U32)M_AD_PLACE_ID_EPG,			MAD_TYPE_APP,		64000,		245,	110,	FALSE},
	{(U32)M_AD_PLACE_ID_PROG_BAR,		MAD_TYPE_APP,		64000,		245,	110,	FALSE},
	{(U32)M_AD_PLACE_ID_VOL_BAR,		MAD_TYPE_APP,		64000,		245,	110,	FALSE},
};

/***********************************************************************************
 *                               m-ad place variable                               *
 ***********************************************************************************/
MadPlace mad_place[M_AD_PLACE_NUM];
/*
 * most frequently used variable.
 */
static MadPlaceBasicInfo mad_place_basic_info[M_AD_PLACE_NUM];
extern MadFilter mad_filters[MAD_FILTER_MAX];
extern MadPlaceInfo mad_place_info[M_AD_PLACE_NUM];
extern mad_file_record mad_file_record_all;

/***********************************************************************************
 *                                     common                                      *
 ***********************************************************************************/
int get_mad_place_id(int place_index)
{
	if (place_index >= 0 && place_index < M_AD_PLACE_NUM)
	{
		return mad_place_info[place_index].ad_place_id;
	}
	return -1;
}

int get_mad_place_index_by_id(int place_id)
{
	int i, index = -1;

	for (i = 0; i < M_AD_PLACE_NUM; i++)
	{
		if (mad_place_info[i].ad_place_id == place_id)
		{
			index = i;
		}
	}
	return index;
}

U32 get_mad_place_limit_size(int place_id)
{
	int place_index;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return 0;
	}
	return mad_place_info[place_index].limit_size;
}


/***********************************************************************************
 *                                 m-ad place                                      *
 ***********************************************************************************/
void destroy_mad_place(MadPlace *mad_place)
{
	int i;

	if (!mad_place)
	{
		return;
	}
	if (mad_place->data_items)
	{
		for (i = 0; i < mad_place->data_item_num; i++)
		{
			if (mad_place->data_items[i].data_addr)
			{
				mad_free(mad_place->data_items[i].data_addr);
			}
			mad_place->data_items[i].data_length = 0;
			mad_place->data_items[i].data_type = 0;
		}
		mad_free(mad_place->data_items);
		mad_place->data_items = NULL;
	}
	mad_place->in_memory = FALSE;
	mad_place->data_item_num =0;
	mad_place->version = 0;
}


static BOOL mad_place_decode(MadPlace *mad_place, U8 *data, U32 length)
{
	const int M_AD_PLACE_DATA_MIN_LENGTH = 12;
	U32 readed_length;
	MadDataInfo *data_infos;
	int i;

	if (data == NULL || length < M_AD_PLACE_DATA_MIN_LENGTH)
	{
		return FALSE;
	}

	mad_memset(mad_place, 0, sizeof(MadPlace));
	readed_length = 0;
	if (mad_memcmp(data + readed_length, M_AD_HEADER, 4) != 0)
	{
		return FALSE;
	}
	readed_length += 4;
	mad_memcpy(&(mad_place->version), data + readed_length, 4);
	readed_length += 4;
	mad_memcpy(&(mad_place->data_item_num), data + readed_length, 4);
	readed_length += 4;
	if (mad_place->data_item_num <= 0)
	{
		mad_place->in_memory = TRUE;
		return TRUE;
	}

	data_infos = (MadDataInfo *)mad_malloc(mad_place->data_item_num * sizeof(MadDataInfo));
	if (data_infos == NULL)
	{
		return FALSE;
	}
	mad_memcpy(data_infos, data + readed_length, mad_place->data_item_num * sizeof(MadDataInfo));

	mad_place->data_items = (MadData *)mad_malloc(mad_place->data_item_num * sizeof(MadData));
	if (mad_place->data_items == NULL)
	{
		mad_free(data_infos);
		return FALSE;
	}
	for (i = 0; i < mad_place->data_item_num; i++)
	{
		mad_place->data_items[i].data_type = data_infos[i].tag;
		mad_place->data_items[i].data_length = data_infos[i].length;
		mad_place->data_items[i].data_addr = (U8 *)mad_malloc(mad_place->data_items[i].data_length);
		if (mad_place->data_items[i].data_addr == NULL)
		{
			mad_free(data_infos);
			destroy_mad_place(mad_place);
			return FALSE;
		}
		mad_memcpy(mad_place->data_items[i].data_addr, data + data_infos[i].offset, mad_place->data_items[i].data_length);
		if (mad_get_crc32(mad_place->data_items[i].data_addr, mad_place->data_items[i].data_length) != data_infos[i].crc32)
		{
			mad_free(data_infos);
			destroy_mad_place(mad_place);
			return FALSE;
		}
	}

	mad_free(data_infos);
	mad_place->in_memory = TRUE;
	return TRUE;
}

static BOOL mad_place_encode(U8 **data, U32 *length, MadPlace *mad_place)
{
	U32 flash_data_header_length;
	U32 flash_data_total_length;
	U32 writed_length;
	MadDataInfo *data_infos;
	U8 *flash_data;
	int i;

	if (data == NULL || length == NULL || mad_place == NULL)
	{
		return FALSE;
	}
	if (mad_place->data_item_num > 0 && mad_place->data_items == NULL)
	{
		return FALSE;
	}

	flash_data_header_length = 12;	/* "M-AD" mark + version + data item num */
	flash_data_total_length = flash_data_header_length;
	data_infos = (MadDataInfo *)mad_malloc(mad_place->data_item_num * sizeof(MadDataInfo));
	if (data_infos == NULL)
	{
		return FALSE;
	}
	flash_data_total_length += mad_place->data_item_num * sizeof(MadDataInfo);
	for (i = 0; i < mad_place->data_item_num; i++)
	{
		data_infos[i].tag = mad_place->data_items[i].data_type;
		data_infos[i].offset = flash_data_total_length;
		data_infos[i].length = mad_place->data_items[i].data_length;
		data_infos[i].crc32 = mad_get_crc32(mad_place->data_items[i].data_addr, mad_place->data_items[i].data_length);
		flash_data_total_length += mad_place->data_items[i].data_length;
	}
	flash_data = (U8 *)mad_malloc(flash_data_total_length);
	if (flash_data == NULL)
	{
		mad_free(data_infos);

		return FALSE;
	}

	writed_length = 0;
	mad_memcpy(flash_data + writed_length, M_AD_HEADER, 4);
	writed_length += 4;
	mad_memcpy(flash_data + writed_length, &(mad_place->version), 4);
	writed_length += 4;
	mad_memcpy(flash_data + writed_length, &(mad_place->data_item_num), 4);
	writed_length += 4;
	mad_memcpy(flash_data + writed_length, data_infos, mad_place->data_item_num * sizeof(MadDataInfo));
	writed_length += mad_place->data_item_num * sizeof(MadDataInfo);
	for (i = 0; i < mad_place->data_item_num; i++)
	{
		mad_memcpy(flash_data + writed_length, mad_place->data_items[i].data_addr, mad_place->data_items[i].data_length);
		writed_length += mad_place->data_items[i].data_length;
	}
	MAD_INF("flash_data_total_length = %d,writed_legth = %d\n",flash_data_total_length,writed_length);
	*data = flash_data;
	*length = flash_data_total_length;
	mad_free(data_infos);

	return TRUE;
}


#define M_AD_PLACE_DATA_MAGIC_NUM		(0x0000ffff)

static BOOL get_item_data_by_tag(U8 **item_data, U32 *item_length, U8 *data, U32 length, U32 tag)
{
	U32 magic_num;
	U32 item_num;
	MadDataInfo data_item;
	U8 *save_data;
	int i;

	save_data = data;
	magic_num = M_AD_MAKE_ULONG(data);
	if (magic_num != M_AD_PLACE_DATA_MAGIC_NUM)
	{
		return FALSE;
	}
	data += 4;
	item_num = M_AD_MAKE_ULONG(data);
	MAD_INF("Item num = %d\n",item_num);
	data += 4;
	for (i = 0; i < item_num; i++)
	{
		data_item.tag = M_AD_MAKE_ULONG(data);
		data += 4;
		data_item.offset = M_AD_MAKE_ULONG(data);
		data += 4;
		data_item.length = M_AD_MAKE_ULONG(data);
		data += 4;
		data_item.crc32 = M_AD_MAKE_ULONG(data);
		data += 4;
		if (data_item.tag == tag)
		{
			if (mad_get_crc32(save_data + data_item.offset, data_item.length) != data_item.crc32)
			{
				MAD_WRN("m-ad place item data check crc32 fail!\n");
				return FALSE;
			}
			*item_data = save_data + data_item.offset;
			*item_length = data_item.length;

			return TRUE;
		}
	}
	return FALSE;
}

static BOOL get_item_data_by_index(U8 **item_data, U32 *item_length, U8 *data, U32 length, int index)
{
	U32 magic_num;
	U32 item_num;
	MadDataInfo data_item;
	U8 *save_data;

	save_data = data;
	magic_num = M_AD_MAKE_ULONG(data);
	if (magic_num != M_AD_PLACE_DATA_MAGIC_NUM)
	{
		return FALSE;
	}
	data += 4;
	item_num = M_AD_MAKE_ULONG(data);
	data += 4;
	if (index < 0 || index >= item_num)
	{
		return FALSE;
	}
	data += (index * sizeof(MadDataInfo));
	data_item.tag = M_AD_MAKE_ULONG(data);
	data += 4;
	data_item.offset = M_AD_MAKE_ULONG(data);
	data += 4;
	data_item.length = M_AD_MAKE_ULONG(data);
	data += 4;
	data_item.crc32 = M_AD_MAKE_ULONG(data);
	data += 4;

	if (mad_get_crc32(save_data + data_item.offset, data_item.length) != data_item.crc32)
	{
		MAD_WRN("m-ad place item data check crc32 fail!\n");
		return FALSE;
	}
	*item_data = save_data + data_item.offset;
	*item_length = data_item.length;

	return TRUE;
}

static BOOL get_ad_control_data(MadData *data, U8 *place_data, U32 length)
{
	U8 *data_addr;
	U32 data_length;

	if (!get_item_data_by_tag(&data_addr, &data_length, place_data, length, MAD_DATA_TYPE_CONTROL))
	{
		return FALSE;
	}
	data->data_addr = (U8 *)mad_malloc(data_length);
	if (data->data_addr == NULL)
	{
		return FALSE;
	}
	data->data_type = MAD_DATA_TYPE_CONTROL;
	mad_memcpy(data->data_addr, data_addr, data_length);
	data->data_length = data_length;
	return TRUE;
}

static cJSON* get_ad_content_form_desc_data(U8 *data, U32 length)
{
	cJSON *root = NULL;
	cJSON *ad_content = NULL;
	cJSON *ret = NULL;

	root = parse_mad_json_data(data, length);
	if (root == NULL)
	{
		goto EXIT;
	}
	ad_content = cJSON_GetObjectItem(root, "ad_content");
	if (ad_content == NULL)
	{
		goto EXIT;
	}
	ret = cJSON_Duplicate(ad_content, TRUE);
EXIT:
	if (root != NULL)
	{
		cJSON_Delete(root);
	}
	return ret;
}

static int get_data_item_num(cJSON *c)
{
	int item_num = 0;

	if (c)
	{
		item_num = cJSON_GetArraySize(c);
	}
	return item_num;
}

static U32 get_data_type_by_string(const char *string)
{
	int i;
	int data_type = MAD_DATA_TYPE_INVALID;
	MadDataStringTyptMap mad_data_string_type_map[] =
	{
		{"txt", MAD_DATA_TYPE_TEXT},
		{"bmp", MAD_DATA_TYPE_BMP},
		{"jpg", MAD_DATA_TYPE_JPG},
		{"m2v", MAD_DATA_TYPE_IFRAME},
		{"gif", MAD_DATA_TYPE_GIF},
	};

	for (i = 0; i < sizeof(mad_data_string_type_map) / sizeof(mad_data_string_type_map[0]); i++)
	{
		if (strcmp(string, mad_data_string_type_map[i].string) == 0)
		{
			data_type = mad_data_string_type_map[i].type;
			break;
		}
	}
	return data_type;
}

static U32 get_data_type(cJSON *c)
{
    char *fmt_string = NULL;
    int data_type = MAD_DATA_TYPE_INVALID;

    if (get_json_item_string_value(&fmt_string, c, "fmt"))
    {
        data_type = get_data_type_by_string(fmt_string);
    }

    return data_type;
}

static int get_data_index(cJSON *c)
{
    int index = -1;
    double value = 0;

    if (get_json_item_num_value(&value, c, "ref"))
    {
        index = (int)(value) - 1;
    }

    return index;
}
extern void mad_ads_dump(U8* str,U8*buffer,U32 length);
static BOOL parse_mad_place_data(MadPlace *place, U8 *data, U32 length)
{
	U8 *desc_data;
	U32 desc_length;
	int i, content_data_num;
	int data_item_index;
	U8 *normal_data;
	U32 normal_data_length;
	cJSON *ad_content = NULL;
	BOOL success = FALSE;
	if (!get_item_data_by_tag(&desc_data, &desc_length, data, length, MAD_DATA_TYPE_DESC))
	{
		return FALSE;
	}
	ad_content = get_ad_content_form_desc_data(desc_data, desc_length);
	if (ad_content == NULL)
	{
		mad_ads_dump("Contend desc data",desc_data, desc_length);
		goto EXIT;
	}
	content_data_num = get_data_item_num(ad_content);
	if (content_data_num <= 0)
	{
		goto EXIT;
	}
	place->data_item_num = content_data_num + 1;	/* content data(s) + control data */
	place->data_items = (MadData *)mad_malloc(place->data_item_num * sizeof(MadData));
	if (place->data_items == NULL)
	{
		goto EXIT;
	}
	/* control data */
	if (!get_ad_control_data(&(place->data_items[0]), data, length))
	{
		goto EXIT;
	}
	/* content data */
	for (i = 1; i < place->data_item_num; i++)
	{
		
		place->data_items[i].data_type = get_data_type(cJSON_GetArrayItem(ad_content, i - 1));
		printf("*******************\n");
		MAD_INF("current place item[%d] data type = 0x%x\n",i,place->data_items[i].data_type);
		data_item_index = get_data_index(cJSON_GetArrayItem(ad_content, i - 1));
		MAD_INF("data_item_index = %d\n",data_item_index);
		if (!get_item_data_by_index(&normal_data, &normal_data_length, data, length, data_item_index))
		{
			goto EXIT;
		}
		place->data_items[i].data_length = normal_data_length;
		MAD_INF("current place item[%d] data length = %d\n",i,place->data_items[i].data_length);
		place->data_items[i].data_addr = (U8 *)mad_malloc(normal_data_length + 1);
		MAD_INF("current place item[%d] data addr = 0x%x\n",i,place->data_items[i].data_addr);
		if (place->data_items[i].data_addr == NULL)
		{
			goto EXIT;
		}
		printf("place->data_items[%d].data_length = %d\n",i,place->data_items[i].data_length);
		mad_memcpy(place->data_items[i].data_addr, normal_data, normal_data_length);
		printf("*******************\n");
	}

	place->in_memory = TRUE;

	success = TRUE;
EXIT:
	if (!success)
	{
		destroy_mad_place(place);
	}
	if (ad_content != NULL)
	{
		cJSON_Delete(ad_content);
	}
	return success;
}

static void save_mad_palce_data_to_file(int place_index,int item_index,char* filename)
{
	int place_id = get_mad_place_id(place_index);
	MadPlace *place_info_temp = mad_place + place_index;
	
	if((filename[0] != '/') || (NULL == filename))
	{
		MAD_INF("%s path err!!! data_type = 0x%x\n",filename,place_info_temp->data_items[item_index].data_type);
		return;
	}
	MAD_INF("place_info_temp->data_items[%d].data_length = %d,place_info_temp->data_items[%d].data_addr = 0x%x\n",item_index,place_info_temp->data_items[item_index].data_length,item_index,place_info_temp->data_items[item_index].data_addr);
	memcpy(mad_file_record_all.mad_file_name_record[place_index][item_index],filename,strlen(filename));
	mad_file_record_all.mad_file_name_record_type[place_index][item_index] = place_info_temp->data_items[item_index].data_type;
	mad_file_record_all.mad_file_name_record_place_id[place_index] = place_id;

	MAD_INF("Save ads data to file [[[[%s]]]\n",filename);
	if(GxCore_FileExists(filename))
		GxCore_FileDelete(filename);
	app_flash_save_ad_data_to_flash_file((char*)place_info_temp->data_items[item_index].data_addr,place_info_temp->data_items[item_index].data_length,filename);
	//mad_ads_dump("MAD-BOOT",place_info_temp->data_items[item_index].data_addr,place_info_temp->data_items[item_index].data_length);

	return;
}

BOOL save_mad_place_data(U32 place_id, BOOL delete_from_mem)
{
	int place_index;
	U8 *mad_place_data;
	U32 length;
	int i = 0;
	MadPlace *place_info_temp = NULL;
	char mad_file_name[256] = {0};
	
	printf("place_id =%d\n",place_id);
	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return FALSE;
	}
	place_info_temp = mad_place + place_index;

	
	switch(place_id)
	{
		case M_AD_PLACE_ID_BOOT_AV:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				memset(mad_file_name,0,256);
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_IFRAME:
						sprintf(mad_file_name,"%s%d%s",MAD_BOOT_FRAM_PATH,i,MAD_FILE_FORMAT_BIN);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
			}
			break;
		case M_AD_PLACE_ID_BOOT_PIC:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				memset(mad_file_name,0,256);
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_BOOT_FRAM_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
			}
			break;
			
		case M_AD_PLACE_ID_CORNER:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_CORNER_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_CORNER_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_CORNER_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-CONER:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;
			
		case M_AD_PLACE_ID_PROG_BAR:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_BAR_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_BAR_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_BAR_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-PROG_BAR:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;

			
		case M_AD_PLACE_ID_VOL_BAR:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_VOL_BAR_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_VOL_BAR_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_VOL_BAR_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-VOL_BAR:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;

		case M_AD_PLACE_ID_MAIN_MUNU:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_MAIN_MENU_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_MAIN_MENU_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_MAIN_MENU_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-MAIN_MENU:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;

		case M_AD_PLACE_ID_PROG_LIST:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_LIST_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_LIST_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_PROG_LIST_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-PROG_LIST:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;

		case M_AD_PLACE_ID_EPG:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("place_info_temp->data_items[%d].data_type = 0x%x\n",i,place_info_temp->data_items[i].data_type);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_JPG:
						sprintf(mad_file_name,"%s%d%s",MAD_EPG_PIC_PATH,i,MAD_FILE_FORMAT_JPG);
						break;
					case MAD_DATA_TYPE_GIF:
						sprintf(mad_file_name,"%s%d%s",MAD_EPG_PIC_PATH,i,MAD_FILE_FORMAT_GIF);
						break;
					case MAD_DATA_TYPE_BMP:
						sprintf(mad_file_name,"%s%d%s",MAD_EPG_PIC_PATH,i,MAD_FILE_FORMAT_PNG);
						break;
					default:
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-EPG:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
			}
			break;
				
		case M_AD_PLACE_ID_SUBTITLE:
			for(i = 0 ; i < place_info_temp->data_item_num;i++)
			{
				printf("--------place_info_temp->data_items[%d].data_length = %d\n",i,place_info_temp->data_items[i].data_length);
				memset(mad_file_name,0,256);
				switch(place_info_temp->data_items[i].data_type)
				{
					case MAD_DATA_TYPE_TEXT:
						sprintf(mad_file_name,"%s%d%s",MAD_OSD_TEXT_PATH,i,MAD_FILE_FORMAT_TEXT);
						break;
					default:
						MAD_INF("not text osd,please check");
						break;
				}
				save_mad_palce_data_to_file(place_index,i,mad_file_name);
				//mad_ads_dump("MAD-SUBTITLE:",place_info_temp->data_items[i].data_addr,place_info_temp->data_items[i].data_length);
				printf("\nsubtitle:%s\n",(char*)place_info_temp->data_items[i].data_addr);
			}
			break;
		default:
			MAD_INF("place id not exit yet!!!\n");
			break;
	}
	return TRUE;

	#if 0
	int cnt = 0;
	for(i = 0 ; i < M_AD_PLACE_NUM;i++)
	{
		for(cnt = 0; cnt < mad_file_record_all.mad_file_name_record_cnt[i];cnt++)
		{
			MAD_INF("mad_file_record_all.mad_file_name_record[%d][%d]%s",i,cnt,mad_file_record_all.mad_file_name_record[i][cnt]);
		}
	}
	#endif
	#if 0
	if (!mad_place_encode(&mad_place_data, &length, mad_place + place_index))
	{
		return FALSE;
	}
	if (length > mad_place_info[place_index].limit_size)
	{
		MAD_WRN("ad size exceed limit!\n");
		return FALSE;
	}
	mad_flash_save_mad_place_data(place_index, mad_place_data, length);
	#endif
	mad_free(mad_place_data);
	if (delete_from_mem)
	{
		destroy_mad_place(mad_place + place_index);
	}

	return TRUE;
}

static BOOL is_mad_content_data_type(int type)
{
	if (type == MAD_DATA_TYPE_DESC || type == MAD_DATA_TYPE_CONTROL)
	{
		return FALSE;
	}
	return TRUE;
}

static int get_mad_place_content_data_item_num(MadPlace *place)
{
	int i, num = 0;

	for (i = 0; i < place->data_item_num; ++i)
	{
		if (is_mad_content_data_type(place->data_items[i].data_type))
		{
			num++;
		}
	}
	return num;
}

static MadData* get_mad_place_data_by_type(MadPlace *place, int type)
{
	int i;

	for (i = 0; i < place->data_item_num; ++i)
	{
		if (place->data_items[i].data_type == type)
		{
			return &(place->data_items[i]);
		}
	}
	return NULL;
}

static MadData* get_mad_place_control_data(MadPlace *place)
{
	return get_mad_place_data_by_type(place, MAD_DATA_TYPE_CONTROL);
}

static int get_interval_from_control_data(MadData *data)
{
	double interval = 0;
	cJSON *root;

	root = parse_mad_json_data(data->data_addr, data->data_length);
	if (root == NULL)
	{
		return 0;
	}
	get_json_item_num_value(&interval, root, "interval");
	cJSON_Delete(root);

	return (int)interval;
}

static int get_mad_place_show_ad_interval(MadPlace *place)
{
	MadData *control_data;

	control_data = get_mad_place_control_data(place);
	if (control_data == NULL)
	{
		return 0;
	}
	return get_interval_from_control_data(control_data);
}

BOOL handle_mad_place_data(U32 place_id, U32 version, U8 *data, U32 length)
{
	int place_index;
	MadPlace *place;
	BOOL result = TRUE;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		MAD_ERR("no such ad place with id: %d\n", place_id);
		return FALSE;
	}
	place = mad_place + place_index;
	if (!parse_mad_place_data(place, data, length))
	{
		return FALSE;
	}

	place->version = version;
	place->in_memory = TRUE;
	//mad_place_basic_info[place_index].version = version;
	mad_place_basic_info[place_index].data_item_num = get_mad_place_content_data_item_num(place);
	mad_place_basic_info[place_index].interval = get_mad_place_show_ad_interval(place);

	if (mad_place_info[place_index].save_to_flash)
	{
		result = save_mad_place_data(place_id, TRUE);
	}
	else
	{
		result = save_mad_place_data(place_id, TRUE);
	}
	return result;
}


static BOOL mad_place_from_flash(int place_index)
{
	U8 *mad_place_data;
	U32 length;
	BOOL result;

	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return FALSE;
	}
	if (!mad_place_info[place_index].save_to_flash)
	{
		return FALSE;
	}
	if (!mad_flash_read_mad_place_data(place_index, &mad_place_data, &length))
	{
		return FALSE;
	}
	result = mad_place_decode(&mad_place[place_index], mad_place_data, length);
	mad_free(mad_place_data);

	return result;
}


/***********************************************************************************
 *                                 basic info                                      *
 ***********************************************************************************/
U32 get_mad_place_content_version(int place_id)
{
	int place_index;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return 0;
	}
	return mad_place_basic_info[place_index].version;
}

int get_mad_place_content_num(int place_id)
{
	int place_index;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return 0;
	}
	return mad_place_basic_info[place_index].data_item_num;
}

int get_mad_place_interval(int place_id)
{
	int place_index;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return 0;
	}
	return mad_place_basic_info[place_index].interval;
}

void save_all_basic_info_to_flash()
{
	int place_index;
	handle_t handle = 0;
	if(1 == GxCore_FileExists(MAD_VERSION_INFO_FILE))
	{
		GxCore_FileDelete(MAD_VERSION_INFO_FILE);
	}
	handle = GxCore_Open((char*)MAD_VERSION_INFO_FILE,"a+");
	if(handle > 0)
	{
		GxCore_Seek(handle, 0, GX_SEEK_SET);
		for (place_index = 0; place_index < M_AD_PLACE_NUM; place_index++)
		{
			printf("mad_palce_basic_info save to flash:");
			printf("\t palce_index = %d: version = %d,item_num =%d\n",place_index,mad_place_basic_info[place_index].version,mad_place_basic_info[place_index].data_item_num);
		}
		GxCore_Write(handle,(void*)mad_place_basic_info,1,M_AD_PLACE_NUM*sizeof(MadPlaceBasicInfo));
		GxCore_Close(handle);
		return;
	}
	
	return;
}
static void read_all_basic_info_from_flash(void)
{
	int place_index;
	handle_t handle = 0;
	handle = GxCore_Open((char*)MAD_VERSION_INFO_FILE,"a+");
	if(handle > 0)
	{
		GxCore_Seek(handle, 0, GX_SEEK_SET);
		GxCore_Read(handle,(void*)mad_place_basic_info,1,M_AD_PLACE_NUM*sizeof(MadPlaceBasicInfo));
		for (place_index = 0; place_index < M_AD_PLACE_NUM; place_index++)
		{
			printf("mad_palce_basic_info read out from flash:");
			printf("\t palce_index = %d: version = %d,item_num =%d interval = %d\n",place_index,mad_place_basic_info[place_index].version,mad_place_basic_info[place_index].data_item_num,mad_place_basic_info[place_index].interval);
		}
		GxCore_Close(handle);
		return;
	}
	printf("mad_verison.dat not exit");
	return;
}

/***********************************************************************************
 *                               show/hide m-ad                                    *
 ***********************************************************************************/
static BOOL show_mad_data(U32 place_id, int x, int y, int w, int h, MadDataType type, U8 *data, U32 length)
{
	BOOL result = FALSE;

	switch (type)
	{
		case MAD_DATA_TYPE_TEXT:
			result = mad_show_text_data(place_id, x, y, w, h, data, length);
			break;
		case MAD_DATA_TYPE_BMP:
			result = mad_show_bitmap_data(place_id, x, y, w, h, data, length);
			break;
		case MAD_DATA_TYPE_JPG:
			result = mad_show_jpeg_data(place_id, x, y, w, h, data, length);
			break;
		case MAD_DATA_TYPE_IFRAME:
			result = mad_show_iframe_data(place_id, x, y, w, h, data, length);
			break;
		case MAD_DATA_TYPE_GIF:
			result = mad_show_gif_data(place_id, x, y, w, h, data, length);
			break;
		default:
			break;
	}
	return result;
}

BOOL show_mad(int x, int y, U32 place_id, U32 content_index)
{
	int place_index;
	BOOL need_read_from_flash;
	MadData *data;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return FALSE;
	}
	need_read_from_flash = !mad_place[place_index].in_memory;
	if (need_read_from_flash)
	{
		mad_place_from_flash(place_index);
	}
	if (content_index >= mad_place[place_index].data_item_num)
	{
		return FALSE;
	}
	if (mad_place[place_index].data_items == NULL || mad_place[place_index].data_items[content_index].data_addr == NULL)
	{
		return FALSE;
	}
	data = &(mad_place[place_index].data_items[content_index]);
	mad_place_basic_info[place_index].x = x;
	mad_place_basic_info[place_index].y = y;
	show_mad_data(place_id, x, y, mad_place_info[place_index].width, mad_place_info[place_index].height,
			data->data_type, data->data_addr, data->data_length);
	if (need_read_from_flash)
	{
		destroy_mad_place(&(mad_place[place_index]));
	}

	return TRUE;
}

BOOL clear_mad(U32 place_id)
{
	int place_index;

	place_index = get_mad_place_index_by_id(place_id);
	if (place_index < 0 || place_index >= M_AD_PLACE_NUM)
	{
		return FALSE;
	}

	return mad_clear_region(place_id, mad_place_basic_info[place_index].x, mad_place_basic_info[place_index].y,
			mad_place_info[place_index].width, mad_place_info[place_index].height);
}


/***********************************************************************************
 *                               initialization                                    *
 ***********************************************************************************/
int mad_init(void)
{
	read_all_basic_info_from_flash();

	return 0;
}



/***********************************************************************************
 *                                     test                                        *
 ***********************************************************************************/
/*
 * test change boot ad
 * use radio logo as boot ad
 */
#if ENABLE_TEST_M_AD
void mad_test_change_boot_ad(void)
{
	MadItem *error_boot_ad = NULL;
	U8 *flash = NULL;
	int total_len;
	int data_max_len;
	BOOL result = TRUE;

	MAD_INF("test change boot ad:\nuse test iframe data as boot ad,M-AD system must change to right ad after next startup.\n");
	total_len = mad_interface_flash_size();
	if (total_len <= sizeof(MadItem))
	{
		result = FALSE;
		goto EXIT;
	}
	if ((flash = mad_malloc(total_len)) == NULL)
	{
		MAD_ERR("no enough memory!!\n");
		result = FALSE;
		goto EXIT;
	}
	if ((error_boot_ad = new_mad_item()) == NULL)
	{
		result = FALSE;
		goto EXIT;
	}
	data_max_len = total_len - mad_item_save_size(error_boot_ad);

	if ((error_boot_ad->content.data_addr = mad_malloc(data_max_len)) == NULL)
	{
		MAD_ERR("no enough memory!!\n");
		result = FALSE;
		goto EXIT;
	}
	error_boot_ad->content.data_length = mad_interface_get_test_iframe_data(error_boot_ad->content.data_addr, data_max_len);
	if (error_boot_ad->content.data_length == 0)
	{
		MAD_ERR("logo test iframe data error!!\n");
		result = FALSE;
		goto EXIT;
	}
	error_boot_ad->type = MAD_TYPE_BOOT;
	error_boot_ad->content.data_type = MAD_DATA_TYPE_IFRAME;
	error_boot_ad->content.crc32 = mad_get_crc32(error_boot_ad->content.data_addr, error_boot_ad->content.data_length);

	memset(flash, 0, mad_interface_flash_size());
	if (mad_item_to_data_array(flash, total_len, error_boot_ad) > 0)
	{
		mad_interface_save_boot_ad(flash);
	}
	MAD_INF("save test logo data as boot ad OK.\n");

EXIT:
	if (flash)
	{
		mad_free(flash);
	}
	if (error_boot_ad)
	{
		delete_mad_item(error_boot_ad);
	}
	MAD_INF("change boot ad result %d.\n", result);
	AutoDisBoxMessage(WAIT_FOR_1_SEC * 3, CENTER_JUST, PROMPT_MSG_X, PROMPT_MSG_Y, MSG_TYPE_INFO, 1,
			result ? "Change Boot Ad OK!" : "Change Boot Ad Fail!");
}

/*
 * test destroy boot ad
 * use mess data as boot ad data
 */
void mad_test_destroy_boot_ad(void)
{
	int total_len;
	U8 *flash = NULL;
	int i;
	BOOL result = TRUE;

	MAD_INF("test destroy boot ad:\nuse mess data as boot ad.\n");
	total_len = mad_interface_flash_size();
	if (total_len <= 0)
	{
		result = FALSE;
		goto EXIT;
	}
	if ((flash = mad_malloc(total_len)) == NULL)
	{
		result = FALSE;
		goto EXIT;
	}

	gsSetRandomSeed(GSOS_TimeNow());
	for (i = 0; i < total_len; i++)
	{
		flash[i] = (U8)(gsGetRandom() % 0x100);	//0~0xff
	}
	mad_interface_save_boot_ad(flash);

EXIT:
	if (flash)
	{
		mad_free(flash);
	}
	MAD_INF("destroy boot ad result %d.\n", result);
	AutoDisBoxMessage(WAIT_FOR_1_SEC * 3, CENTER_JUST, PROMPT_MSG_X, PROMPT_MSG_Y, MSG_TYPE_INFO, 1,
			result ? "Destroy Boot Ad OK!" : "Destroy Boot Ad Fail!");
}
#endif	/* ENABLE_TEST_M_AD */

#endif	/* ENABLE_M_AD */

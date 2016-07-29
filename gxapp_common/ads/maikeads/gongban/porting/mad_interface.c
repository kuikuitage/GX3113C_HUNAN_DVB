/*
 * mad_interface.c
 *
 *  Created on: 2015-4-8
 *      Author: Brant
 */
#include "app_common_porting_stb_api.h"

#include "mad_cfg.h"
//#include "gs_os_interface.h"
//#include "gs_av_interface.h"
#if ENABLE_M_AD_NET
#include "http_common.h"
#endif

#if ENABLE_M_AD

#include "mad.h"
#include "mad_interface.h"
#include "maike_ads_porting.h"

/***********************************************************************************
 *                                      os                                         *
 ***********************************************************************************/
void mad_memcpy(void *dst, void *src, U32 length)
{
	memcpy(dst, src, length);
}

void mad_memset(void *addr, U8 value, U32 length)
{
	memset(addr, value, length);
}

int mad_memcmp(void *addr1, void *addr2, U32 length)
{
	return memcmp(addr1, addr2, length);
}

void *mad_malloc(unsigned int size)
{
	void* addr = malloc(size);
	mad_memset(addr,0x0,size);
	return addr;
}

void mad_free(void *addr)
{
	if (addr != NULL)
		free(addr);
	addr = NULL;
}



int mad_create_task(MadTaskEntry entry, int argc, void *argv, U32 stack_size, void *stack_base, U8 priority, const char *name)
{
	int32_t 		ret = GXCORE_SUCCESS;
	handle_t	handle = -1;
	if((NULL != entry) && (0 != stack_size))
		ret = app_porting_ca_os_create_task((char *)name, &handle,entry, stack_base, stack_size, priority);

	if (ret != GXCORE_SUCCESS)
	{
		printf("task create return err !");
		return MAD_INVALID_TASK;
	}
	return handle;

}

void mad_task_sleep(U32 ms)
{
	GxCore_ThreadDelay(ms);
	return;
}

int mad_create_mutex(int *handle, const char *mutex_name)
{
	return GxCore_MutexCreate((handle_t*)handle);
}

int mad_delete_mutex(int handle)
{
	return GxCore_MutexDelete((handle_t)handle);
}

int mad_mutex_lock(int handle, unsigned int wait_ms)
{
	if(handle == MAD_INVALID_MUTEX)
		return handle;
	return GxCore_MutexLock((handle_t)handle);
}

int mad_mutex_unlock(int handle)
{
	if(handle == MAD_INVALID_MUTEX)
		return handle;
	return GxCore_MutexUnlock((handle_t)handle);
}


/***********************************************************************************
 *                                    device                                       *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
char* mad_device_language(void)
{
	char *lang = NULL;

	switch (GetLanguage())
	{
	#if 0
		case ENGLISH_LANGUAGE:		lang = "English"; 	break;
		case GERMAN_LANGUAGE: 		lang = "German"; 	break;
		case RUSSIAN_LANGUAGE:		lang = "Russian";	break;
		case ARABIC_LANGUAGE:		lang = "Arabic";	break;
		case HUNGARIAN_LANGUAGE:	lang = "Hungarian";	break;
		case TURKISH_LANGUAGE:		lang = "Turkish";	break;
		case ITALIAN_LANGUAGE:		lang = "Italian";	break;
		case FRENCH_LANGUAGE:		lang = "French";	break;
		case SPANISH_LANGUAGE:		lang = "Spanish";	break;
		case FARSI_LANGUAGE:		lang = "Farsi";		break;
		case POLISH_LANGUAGE:		lang = "Polish";	break;
		case GREEK_LANGUAGE:		lang = "Greek";		break;
		case CZECH_LANGUAGE:		lang = "Czech";		break;
		case SLOVAK_LANGUAGE:		lang = "Slovak";	break;
		case UKRAINIAN_LANGUAGE:	lang = "Ukrainian";	break;
		case SWEDISH_LANGUAGE:		lang = "Swedish";	break;
		case DUTCH_LANGUAGE:		lang = "Dutch";		break;
		case THAI_LANGUAGE:			lang = "Thai";		break;
		case SLOVENIA_LANGUAGE:		lang = "Slovenia";	break;
		case BULGARIAN_LANGUAGE:	lang = "Bulgarian";	break;
		case ROMANIAN_LANGUAGE:		lang = "Romanian";	break;
		case PORTUGU_LANGUAGE:		lang = "Portugu";	break;
#if ENABLE_CHINESE_LANGUAGE
		case CHINESE_LANGUAGE:		lang = "Chinese";	break;
#endif
		case DANISH_LANGUAGE:		lang = "Danish";	break;
		case MONGOLIAN_LANGUAGE:	lang = "Mongolian";	break;
		case FINNISH_LANGUAGE:		lang = "Finnish";	break;
		case CROATIAN_LANGUAGE:		lang = "Croatian";	break;
#endif
		default:					lang = "unknown";	break;
	}
	return lang;
}
#endif

int mad_device_pid(void)
{
	return 0;
//	return GetPlatformType();
}

int mad_device_cid(void)
{
	return 0;
//	return get_customer_id();
}

int mad_device_mid(void)
{
	return 0;
//	return get_model_id();
}

BOOL mad_device_sn(char *sn)
{
	//GetDispSNNumber(sn);

	return TRUE;
}

#if ENABLE_M_AD_NET
BOOL mad_device_time(char *time)
{
	int minute;
	int hour;
	int date;
	int month;
	int year;

	//GetRealDate(&year, &month, &date, &hour, &minute, FALSE);
	sprintf(time, "%04d-%02d-%02d %02d:%02d:00", year, month, date, hour, minute);

	return TRUE;
}

U32 mad_device_memory_size(void)
{
	return 0x100000;	//1M
}
#endif




/***********************************************************************************
 *                                     flash                                       *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
U32 mad_flash_total_mad_place_size(void)
{
	return flash_get_mad_size();
}
#endif

extern MadPlaceInfo mad_place_info[M_AD_PLACE_NUM];

BOOL mad_flash_save_mad_place_data(int place_index, U8 *data, U32 length)
{
	int i =0;
	U32 place_id;
	MAD_INF("save ad place data, index: %d, length: %d\n", place_index, length);
	
	place_id = get_mad_place_id(place_index);
	#if 0
	if (place_index != 0)
	{
		return FALSE;
	}
	#endif

#if 0
	switch(place_id)
	{
		case M_AD_PLACE_ID_BOOT:
			for(i = 0;i < M_AD_PLACE_NUM;i++)
			{
				if((mad_place_info[i].ad_place_type == MAD_TYPE_BOOT) && (mad_place_info[i].save_to_flash == TRUE))
				{
					if(1 == GxCore_FileExists(MAD_BOOT_LOGO_PATH))
					{
						GxCore_FileDelete(MAD_BOOT_LOGO_PATH);
					}
					ADS_Dbg("++++++maike ads boot logo start to save to flash file\n");
					//ADS_DUMP("BOOT LOGO",(char*)data,length);
					app_flash_save_ad_data_to_flash_file((char*)data,length,MAD_BOOT_LOGO_PATH);	
				}
			}
			break;
		case M_AD_PLACE_ID_APP:
			for(i = 0;i < M_AD_PLACE_NUM;i++)
			{
				if((mad_place_info[i].ad_place_type == MAD_TYPE_APP) && (mad_place_info[i].save_to_flash == FALSE))
				{
					if(1 == GxCore_FileExists(MAD_APP_PIC_PATH))
					{
						GxCore_FileDelete(MAD_APP_PIC_PATH);
					}
					ADS_Dbg("++++++maike ads app pic  start to save to flash file\n");
					ADS_DUMP("APP PIC",(char*)data,length);
					app_save_data_to_ddram_file((char*)data,length,MAD_APP_PIC_PATH);
				}
			}
			break;
		case M_AD_PLACE_ID_SUBTITLE:
			for(i = 0;i < M_AD_PLACE_NUM;i++)
			{
				if((mad_place_info[i].ad_place_type == MAD_TYPE_SUBTITLE) && (mad_place_info[i].save_to_flash == FALSE))
				{
					if(1 == GxCore_FileExists(MAD_SUBTITLE_PIC_PATH))
					{
						GxCore_FileDelete(MAD_SUBTITLE_PIC_PATH);
					}
					ADS_Dbg("++++++maike ads subtitle pic  start to save to flash file\n");
					ADS_DUMP("SUBTIITLE PIC",(char*)data,length);
					app_save_data_to_ddram_file((char*)data,length,MAD_SUBTITLE_PIC_PATH);
				}
			}
			break;
		case M_AD_PLACE_ID_CORNER:
			for(i = 0;i < M_AD_PLACE_NUM;i++)
			{
				if((mad_place_info[i].ad_place_type == MAD_TYPE_CORNER) && (mad_place_info[i].save_to_flash == FALSE))
				{
					if(1 == GxCore_FileExists(MAD_CORNER_PIC_PATH))
					{
						GxCore_FileDelete(MAD_CORNER_PIC_PATH);
					}
					ADS_Dbg("++++++maike ads corner pic  start to save to flash file\n");
					ADS_DUMP("CORNER PIC",(char*)data,length);
					app_save_data_to_ddram_file((char*)data,length,MAD_CORNER_PIC_PATH);
				}
			}
			break;
		default:
			ADS_Dbg("++++++maike ads WRONG POS\n");
			return;
	}
#endif
	//return flash_update_mad_data(data, length);
	return TRUE;
}

BOOL mad_flash_read_mad_place_data(int place_index, U8 **data, U32 *length)
{
	U8 *mad_data;
	U32 mad_data_length;

	MAD_INF("read flash data! place index: %d\n", place_index);
	if (place_index != 0)
	{
		return FALSE;
	}
	mad_data_length = get_mad_place_limit_size(get_mad_place_id(place_index));
	mad_data = (U8 *)mad_malloc(mad_data_length);
	if (mad_data == NULL)
	{
		return FALSE;
	}
	mad_memset(mad_data,0x0,mad_data_length);
	if (!flash_read_mad_data(mad_data, mad_data_length))
	{
		return FALSE;
	}
	*data = mad_data;
	*length = mad_data_length;

	return TRUE;
}


/***********************************************************************************
 *                                    network                                      *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
BOOL mad_interface_http_download(char *url, U8 **buffer, U32 *data_length)
{
	U8 *data;
	unsigned int length;
	int ret = FALSE;

	http_common_obtain_mutex();

	data = http_download_file(url, &length, NULL, E_HTTP_COMMON_GET);
	if (length > 0)
	{
		*buffer = mad_malloc(length);
		if (*buffer)
		{
			memcpy(*buffer, data + sizeof(U32), length);
			*data_length = length;
			ret = TRUE;
		}
	}
	http_free_download_buffer();
	http_common_release_mutex();

	return ret;
}

char* mad_interface_base64_encode_string(char *src, U32 src_length)
{
	char *dst_string = NULL;

	dst_string = mad_malloc(src_length * 2);
	if (dst_string)
	{
		base64_encode(dst_string, src, src_length);
	}

	return dst_string;
}

char* mad_interface_base64_decode_string(char *src, U32 src_length)
{
	char *src_string;
	char *dst_string = NULL;

	if (src[src_length - 1] == '\0')
	{
		src_string = src;
	}
	else
	{
		src_string = mad_malloc(src_length + 1);
		if (src_string)
		{
			memset(src_string, 0, src_length + 1);
			memcpy(src_string, src, src_length);
		}
	}
	if (src_string)
	{
		dst_string = mad_malloc(src_length);
		if (dst_string)
		{
			base64_decode(dst_string, src_string);
		}
		if (src_string != src)
		{
			mad_free(src_string);
		}
	}

	return dst_string;
}
#endif


/***********************************************************************************
 *                                show/clear m-ad                                  *
 ***********************************************************************************/
BOOL mad_show_text_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length)
{
	U8 *utf8_text;

	MAD_INF("show text ad. place id: %d, data length:%d\n", place_id, length);
	utf8_text = (U8 *)mad_malloc(length + 1);
	if (utf8_text == NULL)
	{
		return FALSE;
	}
	mad_memcpy(utf8_text, data, length);
	utf8_text[length] = 0;
	//draw_utf8_text(x, y, utf8_text, RED_COLOR);
	mad_free(utf8_text);

	return TRUE;
}

BOOL mad_show_bitmap_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length)
{
	MAD_INF("show bitmap ad. place id: %d, data length:%d\n", place_id, length);
	return TRUE;
}

BOOL mad_show_jpeg_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length)
{
	MAD_INF("show jpeg ad. place id: %d, data length:%d\n", place_id, length);
	return TRUE;
}

BOOL mad_show_iframe_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length)
{
	MAD_INF("show iframe ad. place id: %d, data length:%d\n", place_id, length);
	//gsShowIFrame(data, length, FALSE);

	return TRUE;
}

BOOL mad_show_gif_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length)
{
	MAD_INF("show gif ad. place id: %d, data length:%d\n", place_id, length);
	return TRUE;
}

BOOL mad_clear_region(int place_id, int x, int y, int w, int h)
{
	MAD_INF("clear ad region. place id: %d\n", place_id);
	return TRUE;
}

#endif	/* ENABLE_M_AD */

#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_common_porting_ads_flash.h"
#include "app_common_porting_ca_demux.h"
#include "service/gxsearch.h"
#include "app_common_search.h"
#include "cmis_ads_porting.h"
#include "cmis_ads_api.h"
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#endif

#define DATA_SECTION_SET(set,sec_no)  (set[sec_no/8]|=(1<<(sec_no%8)))
#define DATA_SECTION_CLR(set,sec_no)  (set[sec_no/8]&(~(1<<(sec_no%8))))
#define DATA_SECTION_ISSET(set,sec_no)  ((set[sec_no/8]>>(sec_no%8))&0x1)

extern handle_t g_cmis_sem;
uint8_t cmis_task_running = 1;

static handle_t g_private_channel_handle = 0;
static handle_t g_private_filter_handle = 0;
static handle_t g_sdt_channel_handle = 0;
static handle_t g_sdt_filter_handle = 0;
static Cmis_prog_t g_cmis_prog_list[CMIS_PROG_LIST_LEN] = {0};
static uint8_t sdt_cursec_no = 0;
static uint8_t sdt_lastsec_no = 0;
static GxTime g_private_filter_starttime={0};


static void cmis_ad_prog_ad_data_mem_free
	(Cmis_prog_t *ad_prog,Cmis_data_type_t data_type)
{
	if(ad_prog == NULL)
	{
		return;
	}
	if(data_type == CMIS_DATA_TYPE_ALL)
	{
		if(ad_prog->curtain_table)
		{
			CMIS_FREE_PTR_MEM(ad_prog->curtain_table->data);
		}
		CMIS_FREE_PTR_MEM(ad_prog->curtain_table);
		CMIS_FREE_PTR_MEM(ad_prog->curtain_data);

		ad_prog->curtain_data_completed = 0;

		if(ad_prog->epg_table)
		{
			CMIS_FREE_PTR_MEM(ad_prog->epg_table->data);
		}
		CMIS_FREE_PTR_MEM(ad_prog->epg_table);
		CMIS_FREE_PTR_MEM(ad_prog->epg_data);
		ad_prog->epg_data_completed = 0;

	}
	else if(data_type == CMIS_DATA_TYPE_EPG)
	{
		if(ad_prog->epg_table)
		{
			CMIS_FREE_PTR_MEM(ad_prog->epg_table->data);
			memset(ad_prog->epg_table,0,sizeof(Cmis_ad_table_t));
		}
		//CMIS_FREE_PTR_MEM(ad_prog->epg_table);
		//CMIS_FREE_PTR_MEM(ad_prog->epg_data);
		if(ad_prog->epg_data)
		{
			memset(ad_prog->epg_data,0,sizeof(Cmis_epg_data_t));
		}
		ad_prog->epg_data_completed = 0;
	}
	else if(data_type == CMIS_DATA_TYPE_CURTAIN)
	{
		if(ad_prog->curtain_table)
		{
			CMIS_FREE_PTR_MEM(ad_prog->curtain_table->data);
			memset(ad_prog->curtain_table,0,sizeof(Cmis_ad_table_t));
		}
		//CMIS_FREE_PTR_MEM(ad_prog->curtain_table);
		//CMIS_FREE_PTR_MEM(ad_prog->curtain_data);
		if(ad_prog->curtain_data)
		{
			memset(ad_prog->curtain_data,0,sizeof(Cmis_curtain_data_t));
		}
		ad_prog->curtain_data_completed = 0;
	}

}
static void cmis_ad_prog_ad_data_mem_alloc
	(Cmis_prog_t *ad_prog,Cmis_data_type_t data_type)
{
	if(ad_prog == NULL)
	{
		return;
	}

	
	if(data_type == CMIS_DATA_TYPE_ALL)
	{
		/*Malloc curtain data struct*/
		ad_prog->curtain_data = CMIS_MALLOC(sizeof(Cmis_curtain_data_t));
		if(ad_prog->curtain_data == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->curtain_data ,0,sizeof(Cmis_curtain_data_t));
		}
		ad_prog->curtain_table = CMIS_MALLOC(sizeof(Cmis_ad_table_t));
		if(ad_prog->curtain_table == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->curtain_table,0,sizeof(Cmis_ad_table_t));
		}
		ad_prog->curtain_data_completed = 0;
		/*Malloc epg data struct*/
		ad_prog->epg_data = CMIS_MALLOC(sizeof(Cmis_epg_data_t));
		if(ad_prog->epg_data == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->epg_data ,0,sizeof(Cmis_epg_data_t));
		}
		ad_prog->epg_table = CMIS_MALLOC(sizeof(Cmis_ad_table_t));
		if(ad_prog->epg_table == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->epg_table,0,sizeof(Cmis_ad_table_t));
		}
		ad_prog->epg_data_completed = 0;
	}
	else if(data_type == CMIS_DATA_TYPE_CURTAIN)
	{
		/*Malloc curtain data struct*/
		if(ad_prog->curtain_data == NULL)
		{
			ad_prog->curtain_data = CMIS_MALLOC(sizeof(Cmis_curtain_data_t));
		}
		
		if(ad_prog->curtain_data == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->curtain_data ,0,sizeof(Cmis_curtain_data_t));
		}
		/**/
		if(ad_prog->curtain_table == NULL)
		{
			ad_prog->curtain_table = CMIS_MALLOC(sizeof(Cmis_ad_table_t));
		}
		if(ad_prog->curtain_table == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->curtain_table,0,sizeof(Cmis_ad_table_t));
		}
		ad_prog->curtain_data_completed = 0;
	}
	else if(data_type == CMIS_DATA_TYPE_EPG)
	{
		/*Malloc epg data struct*/
		if(ad_prog->epg_data == NULL)
		{
			ad_prog->epg_data = CMIS_MALLOC(sizeof(Cmis_epg_data_t));
		}
		if(ad_prog->epg_data == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->epg_data ,0,sizeof(Cmis_epg_data_t));
		}
		if(ad_prog->epg_table == NULL)
		{
			ad_prog->epg_table = CMIS_MALLOC(sizeof(Cmis_ad_table_t));
		}
		if(ad_prog->epg_table == NULL)
		{
			ADS_ERROR("CMIS_MALLOC ERROR\n");
		}
		else
		{
			memset(ad_prog->epg_table,0,sizeof(Cmis_ad_table_t));
		}
		ad_prog->epg_data_completed = 0;
	}
	
}

Cmis_err_t cmis_ad_prog_list_init(void)
{
	int32_t i = 0;
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		cmis_ad_prog_ad_data_mem_free(&g_cmis_prog_list[i],CMIS_DATA_TYPE_ALL);
		memset(&g_cmis_prog_list[i],0,sizeof(Cmis_prog_t));
	}
	sdt_cursec_no = 0;
	sdt_lastsec_no = 0;
	return CMIS_SUCCESS;
}
Cmis_err_t cmis_ad_prog_list_add_new_prog(Cmis_prog_t * prog_data)
{
	int32_t i = 0;
	if(prog_data == NULL)
	{
		return CMIS_ERROR;
	}
	
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id == prog_data->service_id)
		{
			/*Promram descriptor changed*/
			if((g_cmis_prog_list[i].prog_type != prog_data->prog_type)
				||(g_cmis_prog_list[i].curtain_channel_pid != prog_data->curtain_channel_pid))
			{
				/*Reset the ad data*/
				cmis_ad_prog_ad_data_mem_free(&g_cmis_prog_list[i],CMIS_DATA_TYPE_CURTAIN);
				g_cmis_prog_list[i].prog_type = prog_data->prog_type;
				g_cmis_prog_list[i].curtain_channel_pid = prog_data->curtain_channel_pid;
				cmis_ad_flash_write_ad_info(&g_cmis_prog_list[i],i);
				return CMIS_SUCCESS;
			}
			else if(g_cmis_prog_list[i].epg_channel_pid != prog_data->epg_channel_pid)
			{
				/*Reset the epg data*/
				cmis_ad_prog_ad_data_mem_free(&g_cmis_prog_list[i],CMIS_DATA_TYPE_EPG);
				g_cmis_prog_list[i].epg_channel_pid = prog_data->epg_channel_pid;
				cmis_ad_flash_write_ad_info(&g_cmis_prog_list[i],i);
				return CMIS_SUCCESS;
			}
			else
			{
				/*Promram descriptor NOT changed*/
				return CMIS_ERROR;
			}
		}
	}
	/*Add new item to list*/
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id == 0)
		{
			memcpy(&g_cmis_prog_list[i],prog_data,sizeof(Cmis_prog_t));
			cmis_ad_flash_write_ad_info(&g_cmis_prog_list[i],i);
			return CMIS_SUCCESS;
		}
	}

	return CMIS_ERROR;
}

Cmis_err_t cmis_ad_prog_list_get_prog_by_type
	(Cmis_prog_type_t prog_type,Cmis_prog_t * prog_data)
{
	int32_t i = 0;
	
	if(prog_data == NULL)
	{
		return CMIS_ERROR;
	}
	
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id ==0)
		{
			continue;
		}
		if (g_cmis_prog_list[i].prog_type == prog_type)
		{
			memcpy(prog_data,&g_cmis_prog_list[i],sizeof(Cmis_prog_t));
			return CMIS_SUCCESS;
		}
	}
	return CMIS_ERROR;
}
int32_t cmis_ad_prog_list_get_prog_index_by_service_id(uint16_t service_id)
{
	int32_t i = 0;
	
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id == service_id)
		{
			return i;
		}
	}
	return -1;
}
Cmis_err_t cmis_ad_prog_list_get_prog_by_service_id
	(uint16_t service_id,Cmis_prog_t * prog_data)
{
	int32_t i = 0;
	
	if(prog_data == NULL)
	{
		return CMIS_ERROR;
	}
	
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id == service_id)
		{
			memcpy(prog_data,&g_cmis_prog_list[i],sizeof(Cmis_prog_t));
			return CMIS_SUCCESS;
		}
	}
	return CMIS_ERROR;
}
bool cmis_ad_prog_list_check_ad_data
	(uint16_t service_id,Cmis_data_type_t data_type)
{
	int32_t i = 0;
	
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(g_cmis_prog_list[i].service_id != service_id)
		{
			continue;
		}
		if(data_type == CMIS_DATA_TYPE_ALL)
		{
			if(g_cmis_prog_list[i].curtain_data_completed 
				&& g_cmis_prog_list[i].epg_data_completed)
			{
				return TRUE;
			}
		}
		if(data_type == CMIS_DATA_TYPE_CURTAIN)
		{
			if(g_cmis_prog_list[i].curtain_data_completed)
			{
				return TRUE;
			}
		}
		if(data_type == CMIS_DATA_TYPE_EPG)
		{
			if( g_cmis_prog_list[i].epg_data_completed)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

static int32_t cur_curtain_index = 0;
static int32_t cur_epg_index = 0;
static Cmis_prog_t * cmis_ad_prog_list_get_prog_null_ad_data
	(Cmis_data_type_t data_type)
{
	int32_t i = 0;
	Cmis_prog_t * tempPtr = NULL;
	
	switch (data_type)
	{
		case CMIS_DATA_TYPE_ALL:
			for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
			{
				if(g_cmis_prog_list[i].service_id == 0)
				{
					continue;
				}
				if(g_cmis_prog_list[i].curtain_data_completed 
					&& g_cmis_prog_list[i].epg_data_completed)
				{
					continue;
				}
				tempPtr = &g_cmis_prog_list[i];
				break;
			}		
			break;
		case CMIS_DATA_TYPE_CURTAIN:
			for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
			{
				do
				{
					if(g_cmis_prog_list[cur_curtain_index].service_id == 0)
					{
						break;
					}
					if(g_cmis_prog_list[cur_curtain_index].curtain_channel_pid == 0 ||
						g_cmis_prog_list[cur_curtain_index].curtain_channel_pid == CMIS_INVALID_PID)
					{
						break;
					}
					tempPtr = &g_cmis_prog_list[cur_curtain_index];
				}while(0);
				cur_curtain_index++;
				if(cur_curtain_index >= CMIS_PROG_LIST_LEN)
				{
					cur_curtain_index = 0;
				}
				if(tempPtr != NULL)
				{
					break;
				}
			}		
			break;
		case CMIS_DATA_TYPE_EPG:
			for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
			{
				do
				{
					if(g_cmis_prog_list[cur_epg_index].service_id == 0)
					{
						break;
					}
					//if(g_cmis_prog_list[cur_epg_index].epg_data_completed!= 0)
					{
					//	break;
					}
					if(g_cmis_prog_list[cur_epg_index].epg_channel_pid == 0 ||
						g_cmis_prog_list[cur_epg_index].epg_channel_pid == CMIS_INVALID_PID)
					{
						break;
					}
					tempPtr = &g_cmis_prog_list[cur_epg_index];
				}while(0);
				cur_epg_index++;
				if(cur_epg_index >= CMIS_PROG_LIST_LEN)
				{
					cur_epg_index = 0;
				}
				if(tempPtr != NULL)
				{
					break;
				}
			}
			break;
		default:
			tempPtr = NULL;
	}
	if(tempPtr)
	{
		if(data_type == CMIS_DATA_TYPE_EPG)
		{
			ADS_Dbg("cur_epg_index[%d],type[%d]\n",cur_epg_index-1,data_type);
		}
		else if(data_type == CMIS_DATA_TYPE_CURTAIN)
		{
			ADS_Dbg("cur_curtain_index[%d],type[%d]\n",cur_curtain_index-1,data_type);
		}
	}
	return tempPtr;
}


static void cmis_ad_sdt_parse_descriptor(uint8_t * data,uint16_t service_id)
{
	uint8_t desTag = 0;
	uint8_t desLen = 0;
	Cmis_prog_t temProg = {0};

	if(!data)
	{
		return;
	}
	
	cmis_ad_prog_list_get_prog_by_service_id(service_id,&temProg);
	
	desTag = data[0];
	desLen = data[1];

	switch (desTag)
	{
		case SDT_CMIS_CHANNEL_DES_TAG:
			if (desLen < 1)
			{
				return;
			}
			temProg.service_id = service_id;
			if((data[2] == 0)&& (desLen >= 3))
			{
				temProg.curtain_channel_pid = TODATA16(data[3],data[4]);
				temProg.prog_type = CMIS_PROG_TYPE_CURTAIN;
			}
			else if(data[2] == 1)
			{
				printf("homepage inserted!\n");
				temProg.curtain_channel_pid  = CMIS_INVALID_PID;
				temProg.prog_type = CMIS_PROG_TYPE_HOME_PROG;
			}
			else if(data[2] == 2)
			{
				printf("recharge inserted!\n");
				temProg.curtain_channel_pid  = CMIS_INVALID_PID;
				temProg.prog_type = CMIS_PROG_TYPE_RECHARGE_BACKGROUND;
			}
			else if(data[2] == 3)
			{
				printf("vod inserted!\n");
				temProg.curtain_channel_pid  = CMIS_INVALID_PID;
				temProg.prog_type = CMIS_PROG_TYPE_VOD_BACKGROUND;
			}
			else
			{
				printf("unkown inserted!\n");
				temProg.curtain_channel_pid  = CMIS_INVALID_PID;
				temProg.prog_type = CMIS_PROG_TYPE_UNDEFINE;
			}			
			break;
		case SDT_CMIS_EPG_DES_TAG:
			temProg.service_id = service_id;
			/*App type should be 0x1.others is reserved*/
			if(data[2] != 0x01)
			{
				return;
			}
			temProg.epg_channel_pid = TODATA16(data[3],data[4]);
			break;
		default:
			return;
	}
//printf("inset sid[%d]type[%d]\n",temProg.service_id,temProg.prog_type);
	cmis_ad_prog_list_add_new_prog(&temProg);
	if(temProg.prog_type == CMIS_PROG_TYPE_HOME_PROG 
		||temProg.prog_type == CMIS_PROG_TYPE_RECHARGE_BACKGROUND)
	{
		APP_CMIS_AD_ProgramSave();
	}
}
static void cmis_ad_sdt_parse_services(uint8_t * data)
{
	uint16_t service_id = 0;
	int16_t service_des_len = 0;
	uint8_t desLen = 0;
	uint8_t * pDesLoop = NULL;
	if(!data)
	{
		return;
	}
	service_id = TODATA16(data[0],data[1]);
	service_des_len = TODATA12(data[3],data[4]);
//	service_des_len -= 5;/*Head len*/

	pDesLoop = data+5;
	
	while(service_des_len > 0)
	{
		desLen = pDesLoop[1];
		cmis_ad_sdt_parse_descriptor(pDesLoop,service_id);
		service_des_len -= 2 ;/*Descriptor head, TAG and DES_LEN*/
		service_des_len -= desLen;
		pDesLoop += (2 + desLen);
	}
	
}

private_parse_status cmis_ad_sdt_data_parse
	(uint8_t *p_section_data, uint32_t len)
{
	uint8_t * pServiceLoop = NULL;
	int16_t section_len = 0;
	int16_t des_len = 0;
	uint8_t curSectionNo = 0;
	
	if(!p_section_data)
	{
		return PRIVATE_SECTION_PARSE_ERROR;
	}
	section_len = (TODATA16(p_section_data[1],p_section_data[2]))&0x0fff;
	curSectionNo = p_section_data[6];
	if(curSectionNo >= sdt_cursec_no)
	{
		sdt_cursec_no = curSectionNo;
	}
	sdt_lastsec_no = p_section_data[7];
	pServiceLoop = p_section_data+11;

	section_len -= 11;/*section head len*/
	section_len -= 4; /*CRC*/
	
	while(section_len > 0)
	{
		cmis_ad_sdt_parse_services(pServiceLoop);
		des_len = TODATA12(pServiceLoop[3],pServiceLoop[4]);
		section_len -= 5; /*descriptor head len*/
		section_len -= des_len; /*descriptor content len*/
		pServiceLoop += (5 + des_len);
	}

	if(sdt_lastsec_no <= sdt_cursec_no)
	{
		return PRIVATE_SUBTABLE_OK;
	}
	else
	{
		return PRIVATE_SECTION_OK;
	}
}


static int32_t cmis_ad_data_file_parse(uint8_t *data,uint32_t data_len,
	Cmis_prog_t * cmis_prog,Cmis_data_type_t data_type)
{
	uint16_t pic_offset = 0;
	uint8_t * p = data;
	Cmis_curtain_data_t * curtain_data = NULL;
	Cmis_epg_data_t * epg_data = NULL;
	
	if((p == NULL) || (cmis_prog == NULL) || data_len == 0)
	{
		ADS_ERROR("wrong parameters\n");
		return -1;
	}
	
	switch (data_type)
	{
		case CMIS_DATA_TYPE_CURTAIN:
			curtain_data = cmis_prog->curtain_data ;
			if(curtain_data == NULL)
			{
				ADS_ERROR("cmis_prog->curtain_data is NULL\n");
				return -1;
			}
			curtain_data->magic_no				= TODATA32(data[0],data[1],data[2],data[3]);
			if(curtain_data->magic_no != CMIS_CURTAIN_DATA_MAGIC_NO)
			{
				ADS_ERROR("unkown file MAGIC NO %x!!\n",curtain_data->magic_no );
				return -1;
			}
			curtain_data->protocol_version 			= data[4];
			if(curtain_data->protocol_version != CMIS_AD_CURTAIN_FILE_PROTOCOL_VERSION)
			{
				ADS_ERROR("unkown file protocol [%d]!!\n",
					curtain_data->protocol_version);
				return -1;
			}
			pic_offset 							= TODATA16(data[5],data[6]);
			curtain_data->pic_data.pic_len			= TODATA32(data[7],data[8],data[9],data[10]);
			curtain_data->pic_data.anti_fake 		= TODATA32(data[11],data[12],data[13],data[14]);
			curtain_data->pic_data.monitor_point		= TODATA32(data[15],data[16],data[17],data[18]);
			curtain_data->pic_data.monitor_length   	= TODATA32(data[19],data[20],data[21],data[22]);
			curtain_data->pic_data.x				= TODATA16(data[23],data[24]);
			curtain_data->pic_data.y				= TODATA16(data[25],data[26]);
			curtain_data->action					= data[27];
			curtain_data->pic_data.pic_data 			= data+pic_offset;/*PIC data memory address*/

			p += 28;
			curtain_data->url_data.url_len			= data[28];
			p += 1;
			curtain_data->url_data.url				= p;

			p += curtain_data->url_data.url_len;
			
			curtain_data->tip_data.tip_1_len 		= p[0];
			p += 1;
			curtain_data->tip_data.tip_1_str			= p;
			p += curtain_data->tip_data.tip_1_len;

			curtain_data->tip_data.tip_2_len 		= p[0];
			p += 1;
			curtain_data->tip_data.tip_2_str			= p;
			p += curtain_data->tip_data.tip_2_len;

			curtain_data->tip_data.tip_3_len 		= p[0];
			p += 1;
			curtain_data->tip_data.tip_3_str			= p;
			break;
		case CMIS_DATA_TYPE_EPG:
			epg_data = cmis_prog->epg_data;
			epg_data->magic_no				= TODATA32(data[0],data[1],data[2],data[3]);
			if(epg_data->magic_no != CMIS_EPG_DATA_MAGIC_NO)
			{
				ADS_ERROR("unkown file MAGIC NO %x!!\n",curtain_data->magic_no );
				return -1;
			}
			epg_data->protocol_version 			= data[4];
			if(epg_data->protocol_version != CMIS_AD_EPG_FILE_PROTOCOL_VERSION)
			{
				ADS_ERROR("unkown file protocol [%d]!!\n",
					epg_data->protocol_version);
				return -1;
			}
			epg_data->data_version = data[5];
			epg_data->layout           = data[6];
			epg_data->period		  = TODATA16(data[7],data[8]);
			epg_data->x 			  = TODATA16(data[9],data[10]);
			epg_data->y 			  = TODATA16(data[11],data[12]);
			epg_data->w 			  = TODATA16(data[13],data[14]);
			epg_data->h 			  = TODATA16(data[15],data[16]);
			pic_offset 			  = TODATA32(data[17],data[18],data[19],data[20]);
			epg_data->pic_len  	  = TODATA32(data[21],data[22],data[23],data[24]);
			epg_data->schedule_id   = TODATA32(data[25],data[26],data[27],data[28]);
			epg_data->monitor_point= TODATA32(data[29],data[30],data[31],data[32]);
			epg_data->monitor_len   = TODATA32(data[33],data[34],data[35],data[36]);
			epg_data->action		  = data[37];
			epg_data->url_len		  = TODATA16(data[38],data[39]);
			if(epg_data->url_len)
			{
				epg_data->url_data = &data[40];
			}
			else
			{
				epg_data->url_data = NULL;
			}
			epg_data->pic_data	  = data+pic_offset;
			break;
		default:
			return -1;
	}
	return 0;
	
}
static int32_t cmis_ad_demux_data_callback
	(uint8_t * src_data,uint32_t data_len,Cmis_prog_t * cmis_prog)
{
	uint8_t data_version = 0;
	uint8_t *p = src_data;
	uint16_t des_len = 0;
	uint32_t block_offset = 0;
	uint16_t block_size = 0;
	uint8_t cur_sec_no = 0;
	uint8_t last_sec_no = 0;
	int32_t iRet = 0;
	Cmis_ad_table_t * tem_table = NULL;
	uint8_t table_id = 0;
	Cmis_data_type_t data_type;

	if(src_data == NULL || cmis_prog == NULL)
	{
		return -1;
	}
	table_id = p[0];
	if(table_id == CMIS_CURTAIN_DATA_TABLE_ID)
	{
		tem_table = cmis_prog->curtain_table;
		data_type = CMIS_DATA_TYPE_CURTAIN;
	}
	else if(table_id == CMIS_EPG_DATA_TABLE_ID)
	{
		tem_table = cmis_prog->epg_table;
		data_type = CMIS_DATA_TYPE_EPG;
	}
	else
	{
		ADS_ERROR("unkwon table id [0x%x]\n",table_id);
		return -1;
	}
	
	if(tem_table == NULL)
	{
		ADS_ERROR("table id [0x%x] DATA err \n",table_id);
		return -1;
	}
	/*Data version. renew*/
	data_version = (p[5]>>1)&0x1f;
	if(data_version != tem_table->data_version)
	{
		ADS_Dbg("cmis data version renew.sid[%d].data_type[%d]\n",cmis_prog->service_id,data_type);
		cmis_ad_prog_ad_data_mem_free(cmis_prog,data_type);
		cmis_ad_prog_ad_data_mem_alloc(cmis_prog,data_type);
		if(data_type == CMIS_DATA_TYPE_CURTAIN)
		{
			tem_table = cmis_prog->curtain_table;
		}
		else if(data_type == CMIS_DATA_TYPE_EPG)
		{
			tem_table = cmis_prog->epg_table;
		}
		else
		{
			tem_table = NULL;
		}
	}
	if(tem_table == NULL)
	{
		ADS_ERROR("cmis malloc failed\n");
		return -1;
	}

	des_len = TODATA12(p[8],p[9]);
	cur_sec_no = p[6];
	last_sec_no = p[7];
	/*Check the section is received or not*/
	if(DATA_SECTION_ISSET(tem_table->sec_recv_mask,cur_sec_no))
	{
		ADS_Dbg("Repeat section [%d]\n",cur_sec_no);
		return CMIS_TABLE_OK;
	}
		
	tem_table->data_version = data_version;
	tem_table->channel_code= TODATA16(p[3],p[4]);
	p += 10;
	p += des_len;/*Descriptor loop is not used NOW*/
	
	DATA_SECTION_SET(tem_table->sec_recv_mask,cur_sec_no);
	tem_table->data_size_total = TODATA32(p[0], p[1], p[2], p[3]);
	block_offset = TODATA32(p[4], p[5], p[6], p[7]);
	block_size = TODATA16(p[8], p[9]);
	tem_table->data_size_rcved += block_size;

	if(tem_table->data_size_rcved > tem_table->data_size_total)
	{
		ADS_Dbg("data recved [%d] > total datalen[%d]\n",
			tem_table->data_size_rcved,
			tem_table->data_size_total);
		return -1;
	}
	p += 10;

	if(tem_table->data == NULL)
	{
		tem_table->data = CMIS_MALLOC(tem_table->data_size_total);
	}
	if(tem_table->data != NULL)
	{
		memcpy(tem_table->data+block_offset,p,block_size);
	}
	else
	{
		ADS_ERROR("cmis_prog->curtain_table->data is NULL\n");
		return -1;
	}
	/*debug printf*/
	if(data_type == CMIS_DATA_TYPE_CURTAIN)
	{
		ADS_Dbg("CURTAIN serviceId[%d],pid[0x%x].total[%d]B,recved[%d]B\n",
		cmis_prog->service_id,
		cmis_prog->curtain_channel_pid,
		cmis_prog->curtain_table->data_size_total,
		cmis_prog->curtain_table->data_size_rcved);
	}
	else if(data_type == CMIS_DATA_TYPE_EPG)
	{
		ADS_Dbg("EPG serviceId[%d],pid[0x%x].total[%d]B,recved[%d]B\n",
		cmis_prog->service_id,
		cmis_prog->epg_channel_pid,
		cmis_prog->epg_table->data_size_total,
		cmis_prog->epg_table->data_size_rcved);
	}
	
	/*Data receive completed*/
	if(tem_table->data_size_rcved == tem_table->data_size_total
		&& tem_table->data_size_total != 0)
	{
		iRet = cmis_ad_data_file_parse(tem_table->data,
			tem_table->data_size_rcved, cmis_prog,data_type);
		if(iRet == 0)
		{
			if(data_type == CMIS_DATA_TYPE_CURTAIN)
			{
				cmis_prog->curtain_data_completed = 1;
			}
			else if(data_type == CMIS_DATA_TYPE_EPG)
			{
				cmis_prog->epg_data_completed = 1;
			}
			return CMIS_TABLE_OK;
		}
		else
		{
			/*Reset the AD data*/
			ADS_Dbg("Wrong data.sid[%d],data_type[%d]\n",cmis_prog->service_id,data_type);
			cmis_ad_prog_ad_data_mem_free(cmis_prog,data_type);
			cmis_ad_prog_ad_data_mem_alloc(cmis_prog,data_type);
		}
	}
	return CMIS_SECTION_OK;
}
static Cmis_err_t cmis_ad_demux_private_filter_open
	(Cmis_prog_t * cmis_prog,Cmis_data_type_t data_type)
{
	handle_t channel_handle = 0;
	handle_t filter_handle = 0;
	int32_t iRet = 0;
	bool repeat_flag = 1;
	uint8_t 	match[18] = {0,};
	uint8_t 	mask[18] = {0,};
	uint16_t pid = 0;

	if(cmis_prog == NULL)
	{
		ADS_ERROR("cmis_prog == null \n");
		return CMIS_ERROR;
	}
	if(data_type == CMIS_DATA_TYPE_CURTAIN)
	{
		pid = cmis_prog->curtain_channel_pid;
		match[0] = CMIS_CURTAIN_DATA_TABLE_ID;
	}
	else if(data_type == CMIS_DATA_TYPE_EPG)
	{
		pid = cmis_prog->epg_channel_pid;
		match[0] = CMIS_EPG_DATA_TABLE_ID;
	}
	else
	{
		ADS_FAIL("unkown data type %d \n",data_type);
		return CMIS_ERROR;
	}
	
	mask[0]  = 0xff;
//	GxCore_SemWait(g_cmis_sem);
	do{
		channel_handle = GxDemux_ChannelAllocate(0,pid);
		if(channel_handle == 0)
		{
			ADS_FAIL("GxDemux_ChannelAllocate \n");
			break;
		}

		iRet = GxDemux_ChannelSetPID(channel_handle,pid,repeat_flag);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelSetPID Failed,iRet=%d\n",iRet);
			break;
		}
		filter_handle = GxDemux_FilterAllocate( channel_handle);
		if(filter_handle == 0)
		{
			ADS_FAIL("GxDemux_FilterAllocate \n");
			break;
		}
		
		iRet = GxDemux_FilterSetup(filter_handle,match,mask,	1,1,0,1);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterSetup \n");
			break;
		}
		
		iRet = GxDemux_ChannelEnable(channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelEnable Failed\n");
			break;
		}
		iRet = GxDemux_FilterEnable(filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterEnable Failed\n");
			break;
		}
		g_private_channel_handle = channel_handle;
		g_private_filter_handle = filter_handle;
	//	GxCore_SemPost(g_cmis_sem);
		return CMIS_SUCCESS;
	}while(0);

	if(filter_handle)
	{
		iRet = GxDemux_FilterFree(filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterFree Failed\n");
		}
	}
	if(channel_handle)
	{
		iRet = GxDemux_ChannelFree(channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelFree Failed\n");
		}
	}
//	GxCore_SemPost(g_cmis_sem);
	return CMIS_ERROR;
}


Cmis_err_t cmis_ad_demux_private_filter_close(void)
{
	int32_t iRet = 0;
	Cmis_err_t ret = CMIS_SUCCESS;

//	GxCore_SemWait(g_cmis_sem);

	if(g_private_filter_handle)
	{
		iRet = GxDemux_FilterFree(g_private_filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterFree Failed\n");
			ret = CMIS_ERROR;

		}
		else
		{
			g_private_filter_handle = 0;
		}
	}
	if(g_private_channel_handle)
	{
		iRet = GxDemux_ChannelFree(g_private_channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelFree Failed\n");
			ret = CMIS_ERROR;
		}
		else
		{
			g_private_channel_handle = 0;
		}
	}
	return ret;
//	GxCore_SemPost(g_cmis_sem);
}

Cmis_err_t cmis_ad_demux_sdt_filter_open(uint16_t pid)
{
	handle_t channel_handle = 0;
	handle_t filter_handle = 0;
	int32_t iRet = 0;
	bool repeat_flag = 1;
	uint8_t 	match[18] = {0,};
	uint8_t 	mask[18] = {0,};

	match[0] = CMIS_SDT_TABLE_ID;
	mask[0]  = 0xff;
	//GxCore_SemWait(g_cmis_sem);
	if(g_private_filter_handle || g_private_channel_handle)
	{
		return CMIS_SUCCESS;
	}
	
	do{
		channel_handle = GxDemux_ChannelAllocate(0,pid);
		if(channel_handle == 0)
		{
			ADS_FAIL("GxDemux_ChannelAllocate \n");
			break;
		}

		iRet = GxDemux_ChannelSetPID(channel_handle,pid,repeat_flag);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelSetPID Failed,iRet=%d\n",iRet);
			break;
		}
		filter_handle = GxDemux_FilterAllocate( channel_handle);
		if(filter_handle == 0)
		{
			ADS_FAIL("GxDemux_FilterAllocate \n");
			break;
		}
		
		iRet = GxDemux_FilterSetup(filter_handle,match,mask,	1,1,0,1);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterSetup \n");
			break;
		}
		
		iRet = GxDemux_ChannelEnable(channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelEnable Failed\n");
			break;
		}
		iRet = GxDemux_FilterEnable(filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterEnable Failed\n");
			break;
		}
		g_sdt_channel_handle = channel_handle;
		g_sdt_filter_handle = filter_handle;
		
		//GxCore_SemPost(g_cmis_sem);
		return CMIS_SUCCESS;
	}while(0);

	if(filter_handle)
	{
		iRet = GxDemux_FilterFree(filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterFree Failed\n");
		}
	}
	if(channel_handle)
	{
		iRet = GxDemux_ChannelFree(channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelFree Failed\n");
		}
	}
	//GxCore_SemPost(g_cmis_sem);
	return CMIS_ERROR;
}


Cmis_err_t cmis_ad_demux_sdt_filter_close(void)
{
	int32_t iRet = 0;
	Cmis_err_t ret = CMIS_SUCCESS;
	//GxCore_SemWait(g_cmis_sem);

	if(g_sdt_filter_handle)
	{
		iRet = GxDemux_FilterFree(g_sdt_filter_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_FilterFree Failed\n");
			ret = CMIS_ERROR;
		}
		else
		{
			g_sdt_filter_handle = 0;
		}
	}
	if(g_sdt_channel_handle)
	{
		iRet = GxDemux_ChannelFree(g_sdt_channel_handle);
		if(iRet < 0)
		{
			ADS_FAIL("GxDemux_ChannelFree Failed\n");
			ret = CMIS_ERROR;
		}
		else
		{
			g_sdt_channel_handle = 0;
		}
	}
	return ret;
	//GxCore_SemPost(g_cmis_sem);
}

static void cmis_ad_task(void *arg)
{
	int32_t iRet = 0;
	uint8_t *section = NULL;/* section  buffer */
	uint32_t length = 0;
	Cmis_prog_t * cmis_prog = NULL;
//	uint8_t filter_start_flag = 0;
	GxTime nowtime={0};
	Cmis_data_type_t data_type;
	
	section = GxCore_Malloc(MAX_CMIS_SECTION_BUFFER);
	if(section == NULL)
	{
		ADS_ERROR("GxCore_Malloc Failed\n");
	}
	else
	{
		memset(section,0,sizeof(uint8_t)*MAX_CMIS_SECTION_BUFFER);
	}

	while(1)
	{
		GxCore_SemWait(g_cmis_sem);
		if(cmis_task_running == 0)
		{
			GxCore_ThreadDelay(50);
			GxCore_SemPost(g_cmis_sem);
			continue;
		}
		if(g_private_filter_handle == 0)
		{
			/*Check out the program without AD data*/
			if(data_type == CMIS_DATA_TYPE_EPG)
			{
				data_type = CMIS_DATA_TYPE_CURTAIN;
			}
			else
			{
				data_type = CMIS_DATA_TYPE_EPG;
			}
			
			cmis_prog = cmis_ad_prog_list_get_prog_null_ad_data(data_type);
			
			if(cmis_prog != NULL )
			{
				ADS_Dbg("cmis start get data service_id[%d],type[%d]\n",cmis_prog->service_id,data_type);
				if((data_type == CMIS_DATA_TYPE_CURTAIN && cmis_prog->curtain_data_completed)
					||(data_type == CMIS_DATA_TYPE_EPG && cmis_prog->epg_data_completed))
				{
					cmis_ad_demux_private_filter_open(cmis_prog,data_type);
				}
				else
				{
					cmis_ad_demux_private_filter_open(cmis_prog,data_type);
					cmis_ad_prog_ad_data_mem_free(cmis_prog,data_type);
					cmis_ad_prog_ad_data_mem_alloc(cmis_prog,data_type);
				}
				GxCore_GetTickTime(&g_private_filter_starttime);
			}
			else
			{
				GxCore_ThreadDelay(20);
			}	
		}
		else
		{
				/*Private data receive*/
			iRet = GxDemux_QueryAndGetData(g_private_filter_handle,
						section,MAX_CMIS_SECTION_BUFFER,&length);
			if(iRet > 0)
			{
				iRet = cmis_ad_demux_data_callback(section,length,cmis_prog);
				if(iRet == CMIS_TABLE_OK)
				{
					cmis_ad_demux_private_filter_close();
				}
				else if(iRet == CMIS_SECTION_OK)
				{
					GxCore_GetTickTime(&g_private_filter_starttime);
				}
			}
			else
			{	/*Check time out*/
				GxCore_GetTickTime(&nowtime);
				if((nowtime.seconds - g_private_filter_starttime.seconds) > CMIS_DATA_RECEIVE_TIMEOUT_S)
				{
					ADS_Dbg("cmis data time out\n");
					/*No data get,Free the data received*/
					cmis_ad_prog_ad_data_mem_free(cmis_prog,data_type);
					cmis_ad_demux_private_filter_close();
				}
			}
		}
		
		/*SDT data receive*/
		if(g_sdt_filter_handle != 0)
		{
			iRet = GxDemux_QueryAndGetData(g_sdt_filter_handle,
							section,MAX_CMIS_SECTION_BUFFER,&length);
			if(iRet > 0)
			{
				cmis_ad_sdt_data_parse(section,length);
			}
		}
		
		GxCore_ThreadDelay(10);
		/*Update flash*/
		cmis_ad_flash_update();
		GxCore_SemPost(g_cmis_sem);
	}
}
void cmis_ad_force_search_ad_data(uint16_t service_id,Cmis_data_type_t data_type)
{
	int32_t  ret = -1;
	

	ret = cmis_ad_prog_list_get_prog_index_by_service_id(service_id);
	if(ret <= 0)
	{
		return;
	}
	if(data_type == CMIS_DATA_TYPE_CURTAIN)
	{
		cur_curtain_index = ret;
	}
	else if(data_type == CMIS_DATA_TYPE_EPG)
	{
		cur_epg_index = ret;
	}
	//cmis_ad_demux_private_filter_close();
//	cmis_ad_demux_private_filter_open(&cmis_prog,data_type);
//	GxCore_GetTickTime(&g_private_filter_starttime);
//	cmis_ad_prog_list_add_new_prog(&cmis_prog);
}
void cmis_ad_demux_init(void) 
{
      handle_t task_handle = 0;

	GxCore_ThreadCreate("filter_cmis",&task_handle, cmis_ad_task, 
						NULL, 40 * 1024, GXOS_DEFAULT_PRIORITY);
}

/*
*/

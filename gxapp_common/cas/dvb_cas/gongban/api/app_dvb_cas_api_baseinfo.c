/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.12.02		  zhouhm 	 			creation
*****************************************************************************/
#include "dvbca_interface.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_baseinfo.h"

int32_t app_dvb_cas_api_init_baseinfo_data(void)
{
	return 0;
}

char * app_dvb_cas_api_get_baseinfo_data(ca_get_date_t *data)
{
	uint32_t ID = 0;
	uint32_t pos = 0;
	uint8_t  rating = 0;
	ST_DVBCAWORKTIME st_start_time;
	ST_DVBCAWORKTIME st_end_time;
	DVBCA_BASEINFO_GetStbPair stb_pair;
	static char   s_text_buffer[70]={0};

	if (NULL == data)
		return NULL;

	ID = data->ID;
	pos = data->pos;
	memset(s_text_buffer,0,sizeof(s_text_buffer));

	switch(ID)
	{
		case DVBCAS_BASEINFO_CAS_VERSION:
			if(DVBCA_SUCESS ==  DVBCASTB_GetCaVer((uint8_t *)s_text_buffer))
			{
				return s_text_buffer;
			}
			else
			{
				return NULL;
			}
			break;
		case DVBCAS_BASEINFO_CARD_VERSION:
			if(DVBCA_SUCESS ==  DVBCASTB_GetCardVer((uint8_t *)s_text_buffer))
			{
				return s_text_buffer;
			}
			else
			{
				return NULL;
			}
			break;
		case DVBCAS_BASEINFO_CARD_ID:
			if(DVBCA_SUCESS ==  DVBCASTB_GetCardID((uint8_t *)s_text_buffer))
			{
				return s_text_buffer;
			}
			else
			{
				return NULL;
			}
			break;
		case DVBCAS_BASEINFO_STB_PAIR:
			if(DVBCA_SUCESS ==  DVBCASTB_GetStbPair(stb_pair.stb_id, &stb_pair.stb_num))
			{
				//CAS_Dbg("[debug] %s DVBCAS_BASEINFO_STB_PAIR num = %d\n",__FUNCTION__,stb_pair.stb_num);
				if(pos < stb_pair.stb_num)
				{
					//CAS_Dbg("[debug] pos =  %d stb_id = %08X\n",pos,stb_pair.stb_id[pos]);
					//sprintf(s_text_buffer,"%08X",stb_pair.stb_id[pos]);
					sprintf(s_text_buffer,"%c%c%c%c",((stb_pair.stb_id[pos]>>24)&0xFF),((stb_pair.stb_id[pos]>>16)&0xFF),
							((stb_pair.stb_id[pos]>>8)&0xFF),((stb_pair.stb_id[pos])&0xFF));
					return s_text_buffer;
				}
				else
				{
					return NULL;
				}
			}
			else
			{
				return NULL;
			}
			break;
		case DVBCAS_BASEINFO_WORK_TIME:
			if(DVBCA_SUCESS ==  DVBCASTB_GetWorkTime(&st_start_time, &st_end_time))
			{
				sprintf(s_text_buffer,"%02d:%02d-%02d:%02d",st_start_time.ucHour,st_start_time.ucMinute,st_end_time.ucHour,st_end_time.ucMinute);
				return s_text_buffer;
			}
			else
			{
				return NULL;
			}
			break;
		case DVBCAS_BASEINFO_WATCH_RATING:
			if(DVBCA_SUCESS ==  DVBCASTB_GetRating(&rating))
			{
				char* osd_language=NULL;
				char* chRating[][2]={
					{"不限制","No Limit"},
					{"小于 18 岁","18 Years Old"},
					{"小于 16 岁","16 Years Old"},
					{"小于 14 岁","14 Years Old"},
					{"小于 12 岁","12 Years Old"},
					{"小于 8 岁","8 Years Old"},
					{"小于 6 岁","6 Years Old"},
					{"小于 4 岁","4 Years Old"},};

				osd_language = app_flash_get_config_osd_language();
				if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
				{
					sprintf(s_text_buffer,"%s ",chRating[rating][0]);
				}
				else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
				{
					sprintf(s_text_buffer,"%s ",chRating[rating][1]);
				}
				return s_text_buffer;
			}
			else
			{
				return NULL;
			}
			break;
		default:
			break;
	}
	return NULL;
}





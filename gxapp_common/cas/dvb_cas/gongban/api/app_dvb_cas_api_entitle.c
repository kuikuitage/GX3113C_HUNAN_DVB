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
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_entitle.h"
#include "app_common_epg.h"
#include "dvbca_interface.h"

DVBCA_ENTITLE_GetOperatorID g_dvbca_operatorid;
uint8_t	CaMenu_number = 0;

int32_t app_dvb_cas_api_init_operator_data(void)
{
	return 0;
}

char * app_dvb_cas_api_get_operator_data(ca_get_date_t *data)
{
	uint32_t pos = 0 ;
	uint32_t ID = 0;
	static DVBCA_OPERATOR_GetOperatorInfo dvbca_operator_info;
	static char s_text_operate_name[32] = {0};
	static char s_text_newwork_id[20] = {0};
	static char s_text_operate_id[20] = {0};
	static char s_text_end_time[32] = {0};

	if (NULL == data)
		return NULL;

	pos = data->pos;
	ID = data->ID;

	switch(ID)
	{
		case DVB_OPERATOR_PRODUCT_OPERATOR_ID:
			memset(&dvbca_operator_info,0,sizeof(dvbca_operator_info));
			DVBCASTB_GetOperatorInfo(g_dvbca_operatorid.operator_id[pos],&dvbca_operator_info.operator_info);

			memset(s_text_operate_id, 0, sizeof(s_text_operate_id));
			sprintf(s_text_operate_id, "%d",g_dvbca_operatorid.operator_id[pos]);
			return s_text_operate_id;
		case DVB_OPERATOR_PRODUCT_NETWORK_ID:
			memset(s_text_newwork_id, 0, sizeof(s_text_newwork_id));
			sprintf(s_text_newwork_id, "%d",dvbca_operator_info.operator_info.usNetworkID);
			return s_text_newwork_id;
		case DVB_OPERATOR_PRODUCT_NAME_ID:
			memset(s_text_operate_name, 0, sizeof(s_text_operate_name));
			sprintf(s_text_operate_name, "%s",dvbca_operator_info.operator_info.aucOperatorName);
			return s_text_operate_name;
		case DVB_OPERATOR_PRODUCT_ENDTIME_ID:
			memset(s_text_end_time, 0, sizeof(s_text_end_time));
			sprintf(s_text_end_time, "%04d-%02d-%02d %02d:%02d",dvbca_operator_info.operator_info.stExpiredTime.ucYear+2000,
					dvbca_operator_info.operator_info.stExpiredTime.ucMonth,
					dvbca_operator_info.operator_info.stExpiredTime.ucDay,
					dvbca_operator_info.operator_info.stExpiredTime.ucHour,
					dvbca_operator_info.operator_info.stExpiredTime.ucMonth);
			return s_text_end_time;
		default:
			break;
	}
	return NULL;

}

uint8_t app_dvb_cas_api_get_operator_count(ca_get_count_t *data)
{
	if(DVBCA_SUCESS ==  DVBCASTB_GetOperatorID(g_dvbca_operatorid.operator_id, &g_dvbca_operatorid.operator_num))
	{
		printf("app_dvb_cas_api_get_operator_count =%d\n",g_dvbca_operatorid.operator_num);
		data->totalnum = g_dvbca_operatorid.operator_num;
		return g_dvbca_operatorid.operator_num;
	}
	else
		CAS_Dbg("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return 0;
}


int32_t app_dvb_cas_api_init_entitle_data(void)
{
	return 0;
}

static ST_DVBCA_ENTITLES dvbca_entitle_info;
char * app_dvb_cas_api_get_entitle_data(ca_get_date_t *data)
{
	uint32_t pos = 0 ;
	uint32_t ID = 0;
	static char s_entitle_operate_id[20] = {0};
	static char s_entitle_end_time[32] = {0};
	static char s_entitle_start_time[32] = {0};

	if (NULL == data)
		return NULL;

	pos = data->pos;
	ID = data->ID;

	switch(ID)
	{
		case DVB_ENTITLE_PRODUCT_NUM:
			memset(s_entitle_operate_id, 0, sizeof(s_entitle_operate_id));
			sprintf(s_entitle_operate_id, "%d",pos);
			return s_entitle_operate_id;
		case DVB_ENTITLE_PRODUCT_START_TIME:
			memset(s_entitle_start_time, 0, sizeof(s_entitle_start_time));
			sprintf(s_entitle_start_time, "20%d:%d:%d",dvbca_entitle_info.astEntitles[0].ucStartYear,dvbca_entitle_info.astEntitles[0].ucStartMonth,dvbca_entitle_info.astEntitles[0].ucStartDay);
			return s_entitle_start_time;
		case DVB_ENTITLE_PRODUCT_END_TIME:
			memset(s_entitle_end_time, 0, sizeof(s_entitle_end_time));
			sprintf(s_entitle_end_time, "20%d:%d:%d",dvbca_entitle_info.astEntitles[0].ucExpiredYear,dvbca_entitle_info.astEntitles[0].ucExpiredMonth,dvbca_entitle_info.astEntitles[0].ucExpiredDay);
			return s_entitle_end_time;
		default:
			break;
	}
	return NULL;

}

uint8_t app_dvb_cas_api_get_entitle_count(ca_get_count_t *data)
{
	uint32_t pos = 0 ;

	if (NULL == data)
		return 0;
	pos = data->pos;

	if(DVBCA_SUCESS ==  DVBCASTB_GetServiceEntitles(g_dvbca_operatorid.operator_id[pos],&dvbca_entitle_info))
	{
		printf("app_dvb_cas_api_get_entitle_count =%d\n",dvbca_entitle_info.usProductCount);
		data->totalnum = dvbca_entitle_info.usProductCount;
		return dvbca_entitle_info.usProductCount;
	}
	else
		CAS_Dbg("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return 0;
}
int gEntitleCount = 0;
uint32_t app_dvb_cas_get_entitle_count(int flag)
{
	int i = 0;
	static int count = 0;
	if ((count!=0)&&(count%3==0))
	{
		flag = 1;
	}
	if( flag == 0 )
	{
		count++;
		if(DVBCA_SUCESS ==  DVBCASTB_GetOperatorID(g_dvbca_operatorid.operator_id, &g_dvbca_operatorid.operator_num))
		{
			if ( g_dvbca_operatorid.operator_num!=0)
			{
				for( i=0 ; i<g_dvbca_operatorid.operator_num ; i++ )
				{
					if(DVBCA_SUCESS ==  DVBCASTB_GetServiceEntitles(g_dvbca_operatorid.operator_id[i],&dvbca_entitle_info))
					{
						if (dvbca_entitle_info.usProductCount != 0)
						{
							break;
						}	
					}	
				}
				if( i == g_dvbca_operatorid.operator_num )
				{
					gEntitleCount = 0;
					return 0;
				}
					gEntitleCount = 1;
					return 1;

			}	
			else
			{	
				
				gEntitleCount = 0;
				return 0;
			}	
		}
		else
		{	
		
			gEntitleCount = 0;
			return 0;
		}
	}
	else
	{
		count = 0;
		return gEntitleCount ;
	}	
}



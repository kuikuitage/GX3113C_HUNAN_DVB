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
#include "app_win_interface.h"
#include "app_dvb_cas_api_baseinfo.h"


SIGNAL_HANDLER  int win_dvb_cas_baseinfo_create(const char* widgetname, void *usrdata)
{
	ca_get_date_t get_data = {0};
	//uint8_t  chSmartInfoTxt[40] = {0, };
	char* str = NULL;
	int i;

	get_data.date_type = DVB_CA_BASE_INFO;
	get_data.ID = DVBCAS_BASEINFO_CAS_VERSION;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_baseinfo_cas_version", "string",str);
	}

	GUI_SetProperty("win_dvb_cas_baseinfo_sysspid", "string","DVBCA");

	get_data.ID = DVBCAS_BASEINFO_CARD_VERSION;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_baseinfo_card_version", "string",str);
	}

	get_data.ID = DVBCAS_BASEINFO_CARD_ID;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_baseinfo_cardid", "string",str);
	}

	get_data.ID = DVBCAS_BASEINFO_WORK_TIME;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_baseinfo_work_time", "string",str);
	}

	get_data.ID = DVBCAS_BASEINFO_WATCH_RATING;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_baseinfo_watch_level", "string",str);
	}

	get_data.ID = DVBCAS_BASEINFO_STB_PAIR;
	for(i=0;i<DVBCA_STB_ID_MAX_NUM;i++)
	{
		get_data.pos = i;
		str = app_cas_api_get_data(&get_data);
		if(NULL == str)
			continue;
		switch(i)
		{
			case 0:
				GUI_SetProperty("win_dvb_cas_baseinfo_bind_stb1", "string", str);
				break;
			case 1:
				GUI_SetProperty("win_dvb_cas_baseinfo_bind_stb2", "string", str);
				break;
			case 2:
				GUI_SetProperty("win_dvb_cas_baseinfo_bind_stb3", "string", str);
				break;
			case 3:
				GUI_SetProperty("win_dvb_cas_baseinfo_bind_stb4", "string", str);
				break;
		}
		//sprintf((char*)chSmartInfoTxt, "win_dvb_cas_baseinfo_bind_stb%d", i);
		//GUI_SetProperty((char*)chSmartInfoTxt, "string", str);
	}

	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_baseinfo_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_baseinfo_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
				break;
				return EVENT_TRANSFER_STOP;
			case KEY_RECALL:
			case KEY_MENU:
				GUI_EndDialog("win_dvb_cas_baseinfo");
				return EVENT_TRANSFER_STOP;
			case KEY_OK:
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_STOP;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


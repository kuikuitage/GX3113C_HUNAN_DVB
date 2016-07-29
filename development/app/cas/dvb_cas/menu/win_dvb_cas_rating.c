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
 * Purpose   :	模块头文\u0152?******************************************************************************
 * Release History:
 VERSION	Date			  AUTHOR         Description
 1.0  	2012.12.02		  zhouhm 	 			creation
 *****************************************************************************/
#include "app_common_porting_stb_api.h"
#include "app_win_interface.h"
#include "app_dvb_cas_api_rating.h"

SIGNAL_HANDLER  int win_dvb_cas_rating_create(const char* widgetname, void *usrdata)
{
	uint32_t rating = 0;
	ca_get_date_t get_data = {0};
	char* str = NULL;
	char* osd_language=NULL;

	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
		GUI_SetProperty("win_dvb_cas_watchlevel_boxitem1_value", "content",
				"[不限制,小于 18 岁,小于 16 岁,小于 14 岁,小于 12 岁,小于 8 岁,小于 6 岁,小于 4 岁,]");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		GUI_SetProperty("win_dvb_cas_watchlevel_boxitem1_value", "content",
				"[No Limit,18 Years Old,16 Years Old,14 Years Old,12 Years Old,8 Years Old,6 Years Old,4 Years Old,]");
	}
	get_data.date_type = DVB_CA_RATING_INFO;
	get_data.ID = DVB_RATING_LEVEL_ID;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		rating = atoi(str);
		GUI_SetProperty("win_dvb_cas_watchlevel_boxitem1_value", "select", &rating);
	}

	return 0;

}

SIGNAL_HANDLER  int win_dvb_cas_rating_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_rating_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t ratevalue;
	uint8_t *value;
	char* str = NULL;
	char* osd_language=NULL;
	char pucPin[7] = {0};
	ca_rating_t rating = {0};

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
			case KEY_MENU:
			case KEY_RECALL:
				GUI_EndDialog("win_dvb_cas_watchlevel");
				return EVENT_TRANSFER_STOP;

			case KEY_OK:
				GUI_GetProperty("win_dvb_cas_watchlevel_boxitem2_edit", "string", &value);
				memcpy(pucPin,(char*)value,6);
				pucPin[6]='\0';
				if(strlen(pucPin)!=6)
				{
					osd_language = app_flash_get_config_osd_language();
					if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"PIN码必须为6位",POPMSG_TYPE_OK);
					else
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"PIN length must be six!",POPMSG_TYPE_OK);
					GUI_SetProperty("win_dvb_cas_worktime_boxitem1_edit","clear",NULL);
					return EVENT_TRANSFER_STOP;
				}

				GUI_GetProperty("win_dvb_cas_watchlevel_boxitem1_value", "select", &ratevalue);
				rating.date_type = DVB_CA_RATING_INFO;
				rating.pin = (char*)value;
				rating.rate = ratevalue;
				str = app_cas_api_change_rating(&rating);
				if (NULL != str)
				{
					//printf("str = %s \n",str);
					GUI_SetProperty("win_dvb_cas_watchlevel_boxitem2_edit","clear",NULL);
#ifdef APP_SD
					app_popmsg(210,150,str,POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
					app_popmsg(340, 200,str,POPMSG_TYPE_OK);
#endif
				}
				break;
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_STOP;
		}
	}

	return EVENT_TRANSFER_KEEPON;


}










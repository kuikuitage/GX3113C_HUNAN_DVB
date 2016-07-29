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
#include "app_dvb_cas_api_worktime.h"

SIGNAL_HANDLER  int win_dvb_cas_worktime_create(const char* widgetname, void *usrdata)
{
	ca_get_date_t get_data = {0};
	char* str = NULL;
	char* osd_language=NULL;

	get_data.date_type = DVB_CA_WORK_TIME_INFO;
	get_data.ID = DVB_WORKTIME_START_ID;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_worktime_boxitem2_edit", "string",str);
	}

	get_data.date_type = DVB_CA_WORK_TIME_INFO;
	get_data.ID = DVB_WORKTIME_END_ID;
	str = app_cas_api_get_data(&get_data);
	if (NULL !=str )
	{
		GUI_SetProperty("win_dvb_cas_worktime_boxitem3_edit", "string",str);
	}

	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_worktime_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_worktime_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	char *pin;
	char *starttime;
	char *endtime;
	char* str = NULL;
	char* osd_language=NULL;
	char pucPin[7] = {0};
	ca_work_time_t worktime = {0};

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
				break;
				return EVENT_TRANSFER_STOP;
			case KEY_RECALL:
			case KEY_MENU:
				GUI_EndDialog("win_dvb_cas_worktime");
				return EVENT_TRANSFER_STOP;

			case KEY_LEFT:
			case KEY_RIGHT:
				return EVENT_TRANSFER_STOP;
			case KEY_OK:
				GUI_GetProperty("win_dvb_cas_worktime_boxitem1_edit", "string", &pin);
				memcpy(pucPin,(char*)pin,6);
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

				GUI_GetProperty("win_dvb_cas_worktime_boxitem2_edit", "string", &starttime);
				GUI_GetProperty("win_dvb_cas_worktime_boxitem3_edit", "string", &endtime);

				worktime.date_type = DVB_CA_WORK_TIME_INFO;
				worktime.pin = (char*)pin;
				worktime.starttime = starttime;
				worktime.endtime = endtime;
				str = app_cas_api_change_worktime(&worktime);
				if (str != NULL)
				{
					GUI_SetProperty("win_dvb_cas_worktime_boxitem1_edit","clear",NULL);
#ifdef APP_SD
					app_popmsg(210,150,str,POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
					app_popmsg(340, 200,str,POPMSG_TYPE_OK);
#endif
				}

				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;

}










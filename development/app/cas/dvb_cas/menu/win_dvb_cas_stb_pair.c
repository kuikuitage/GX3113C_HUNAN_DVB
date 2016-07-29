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
#include "app_dvb_cas_api_stb_pair.h"


SIGNAL_HANDLER  int win_dvb_cas_stb_pair_create(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_stb_pair_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int win_dvb_cas_stb_pair_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	ca_pin_t pin={0};
	uint8_t *value;
	char* str = NULL;


	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
				break;
				return EVENT_TRANSFER_STOP;
			case KEY_RECALL:
				GUI_EndDialog("win_dvb_cas_stb_pair");
				return EVENT_TRANSFER_STOP;

			case KEY_OK:
				/*首先匹配原始密码*/
				GUI_GetProperty("win_dvb_cas_stb_pair_boxitem1_edit", "string", &value);
				pin.oldpin = (char*)value;
				pin.errorCode = 1;
				str = app_cas_api_verify_pin(&pin);

				//if ((NULL != str)&&(1 == pin.errorCode))
				{
					GUI_SetProperty("win_dvb_cas_stb_pair_boxitem1_edit","clear",NULL);
#ifdef APP_SD
					app_popmsg(210,150,str,POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
					app_popmsg(340, 200,str,POPMSG_TYPE_OK);
#endif
					return EVENT_TRANSFER_STOP;
				}
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_STOP;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int win_dvb_cas_stb_pair_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_MENU:
				event->key.sym = KEY_EXIT;
				return EVENT_TRANSFER_KEEPON;
				break;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}










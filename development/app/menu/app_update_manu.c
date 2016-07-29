/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_volume.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.12		  lijq 	 			creation
*****************************************************************************/

#include "app.h"

SIGNAL_HANDLER  int app_update_menu_create(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;

	GUI_SetProperty("win_sys_update_time_text", "string", app_win_get_local_date_string());
	/*
	* reset progress zero
	*/
	GUI_SetProperty("update_manual_progbar", "value", (void*)&value);

	return 0;
}

SIGNAL_HANDLER  int app_update_menu_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int app_update_menu_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_MENU:
		case KEY_RECALL:
			GUI_EndDialog("win_update_menu");
			return EVENT_TRANSFER_STOP;
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_update_menu_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int i = 0;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
		case KEY_OK:
			GUI_GetProperty("win_update_menu_box", "select", &i);
			switch(i)
			{
			case 0:	//ota 升级
				GUI_EndDialog("win_update_menu");
				GUI_CreateDialog("win_system_update");
				break;
			case 1:	//usb 升级
				GUI_EndDialog("win_update_menu");
				GUI_CreateDialog("win_update_manual");
				break;
			default:
				break;
			}
			return EVENT_TRANSFER_KEEPON;
		case KEY_UP:
			GUI_GetProperty("win_update_menu_box", "select", &i);
			i--;
			if (i <0)
			{
				i = 1;
			}
			GUI_SetProperty("win_update_menu_box", "select", &i);
			return EVENT_TRANSFER_STOP;
			break;
		case KEY_DOWN:
			GUI_GetProperty("win_update_menu_box", "select", &i);
			i++;
			if (i > 1)
			{
				i = 0;
			}
			GUI_SetProperty("win_update_menu_box", "select", &i);
			return EVENT_TRANSFER_STOP;
			break;
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}
	return EVENT_TRANSFER_KEEPON;
}






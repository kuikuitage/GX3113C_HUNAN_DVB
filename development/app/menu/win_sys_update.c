/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_sys_update.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.15		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"

SIGNAL_HANDLER int win_sys_update_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
	GUI_SetProperty("win_sys_update_time_text", "string", app_win_get_local_date_string());


    if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
    {
#ifdef HD
    	    GUI_SetProperty("win_sys_update_title", "img", "title_update.bmp");
    	GUI_SetProperty("win_sys_update_tip_image_exit", "img", "tips_exit.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
#ifdef HD	    
    	GUI_SetProperty("win_sys_update_title", "img", "title_update_e.bmp");
    	GUI_SetProperty("win_sys_update_tip_image_exit", "img", "tips_exit_e.bmp");
#endif
    }



	
	return 0;
}

SIGNAL_HANDLER  int win_sys_update_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER int win_sys_update_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int32_t item_sel = 0;	


	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_RECALL:
		case KEY_EXIT:
		case KEY_MENU:
			GUI_EndDialog("win_sys_update");
			return EVENT_TRANSFER_STOP;								
		case KEY_OK:
			GUI_GetProperty("win_sys_update_box", "select", (void*)&item_sel);

			switch(item_sel)
			{
				case 1:
					GUI_CreateDialog("win_update_manual");
					break;
				default:
					GUI_EndDialog("win_sys_update");
					break;
			}
			return EVENT_TRANSFER_STOP;								
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


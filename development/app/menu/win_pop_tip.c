/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_pop_tip.c
* Author    : 	zhouhm
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
static event_list* swin_pop_tip_time = NULL;
static int autoClear = 1;

extern char* gs_popmsg_win;

void set_win_pop_tip_autoclear(int clearFlag)
{
    autoClear = clearFlag;
}

int win_pop_tip_show_hotplug_gxmsg(char* msg)
{
	GUI_SetProperty("win_pop_tip_tip", "string", msg);
			
	return 0;
}

static  int timer(void *userdata)
{
 	GUI_EndDialog("win_pop_tip");   
	swin_pop_tip_time = NULL;
			
	return 0;
}

SIGNAL_HANDLER int win_pop_tip_create(const char* widgetname, void *usrdata)
{
	if (app_get_pop_msg_flag_status())
	{
		GUI_EndDialog(gs_popmsg_win);
		GUI_SetInterface("flush", NULL);
		
		app_set_pop_msg_flag_status(FALSE);
	}
	
    if(autoClear)
    {
    	uint32_t duration = 2000;
    	swin_pop_tip_time = create_timer(timer, duration, NULL,  TIMER_ONCE);
    //	GUI_SetInterface("flush", NULL);	
    }
	return 0;
}

SIGNAL_HANDLER  int win_pop_tip_destroy(const char* widgetname, void *usrdata)
{
    if(swin_pop_tip_time)
    {
    	remove_timer(swin_pop_tip_time);
    	swin_pop_tip_time = NULL;
    }
    autoClear = 1;
	return 0;
}

SIGNAL_HANDLER int win_pop_tip_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
        case KEY_OK:
		case KEY_RECALL:
		case KEY_EXIT:
			GUI_EndDialog("win_pop_tip");
			return EVENT_TRANSFER_STOP;								
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


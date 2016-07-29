/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_password_input.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.16		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"

extern void app_manage_play_lock_prog();
static event_list* swin_passowrd_time = NULL;
int rech_end_ok_flag = 0;
static  int timer(void *userdata)
{
	if (1 == rech_end_ok_flag)
		{
			GUI_EndDialog("win_password_input");
		}
	return 0;
}

SIGNAL_HANDLER  int win_password_input_create(const char* widgetname, void *usrdata)
{
	int sApp_list_prog_Sel = 0;
	rech_end_ok_flag = 0;

	if(PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
		{
			GUI_GetProperty("win_tv_prog_listview","select",(void*)&sApp_list_prog_Sel);
			GUI_SetProperty("win_tv_prog_listview","active",&sApp_list_prog_Sel);
		}
	else
		if(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
			{
				GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&sApp_list_prog_Sel);
				GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_list_prog_Sel);
			}
	else
		if(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
			{
				GUI_GetProperty("win_favorite_prog_listview","select",(void*)&sApp_list_prog_Sel);
				GUI_SetProperty("win_favorite_prog_listview","active",&sApp_list_prog_Sel);	
			}					

	swin_passowrd_time = create_timer(timer, 200, NULL,  TIMER_REPEAT);

	return 0;
}

SIGNAL_HANDLER  int win_password_input_destroy(const char* widgetname, void *usrdata)
{
	int sApp_list_prog_Sel = -1;

	if (NULL != swin_passowrd_time )
		{
			remove_timer(swin_passowrd_time);
			swin_passowrd_time = NULL;			
		}
	rech_end_ok_flag = 0;

	if(PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
		{
			GUI_SetProperty("win_tv_prog_listview","active",&sApp_list_prog_Sel);
		}
	else
		if(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
			{
				GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_list_prog_Sel);
			}
	else
		if(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
			{
				GUI_SetProperty("win_favorite_prog_listview","active",&sApp_list_prog_Sel);	
			}
	return 0;
}


SIGNAL_HANDLER  int win_password_input_edit_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_LEFT:
			case KEY_RIGHT:
				return EVENT_TRANSFER_STOP;					
			default:
				break;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;

	
}


SIGNAL_HANDLER  int win_password_input_reach_end(const char* widgetname, void *usrdata)
{
	int32_t rtn =0;
	uint32_t pos;

	rtn = app_win_check_password_valid("win_password_input_edit",MAX_PASSWD_LEN);
	GUI_SetInterface("flush", NULL);
	if(0 == rtn)
	{
		app_play_clear_msg_pop_type(MSG_POP_PROG_LOCK);
		if (TRUE == app_get_win_create_flag(MANAGE_MENU_WIN))
		{
			app_manage_play_lock_prog();
		}
		else
		{
			app_prog_get_playing_pos_in_group(&pos);
			app_play_video_audio(pos);			
		}
		rech_end_ok_flag = 1;
//		GUI_EndDialog("win_password_input");
	}
	else
	{
		rech_end_ok_flag = 0;
		GUI_SetProperty("win_password_input_edit","clear",NULL);
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Error password!",POPMSG_TYPE_OK);


		return EVENT_TRANSFER_STOP;	
	}

	return 0;
}


SIGNAL_HANDLER  int win_password_input_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int32_t rtn =0;
	uint32_t pos;

	event = (GUI_Event *)usrdata;
	
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_EXIT:
				GUI_EndDialog("win_password_input");
				return EVENT_TRANSFER_STOP;	
			case KEY_LEFT:
			case KEY_RIGHT:
				return EVENT_TRANSFER_STOP;					
			case KEY_UP:
			case KEY_DOWN:
				if(PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
					{
							GUI_EndDialog("win_password_input");
							GUI_SetFocusWidget("win_tv_prog_listview");
							GUI_SendEvent("win_tv_prog_listview", event);	
					}
				else
					if(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
						{
							GUI_EndDialog("win_password_input");
							GUI_SetFocusWidget("win_epg_prog_list_listview");
							GUI_SendEvent("win_epg_prog_list_listview", event);	
						}
				else
					if(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
						{
							GUI_EndDialog("win_password_input");
							GUI_SetFocusWidget("win_favorite_prog_listview");
							GUI_SendEvent("win_favorite_prog_listview", event);	
						}					
				else	
					if (FULL_SCREEN_WIN == app_win_get_focus_video_window())
						{
							if (TRUE == app_get_win_create_flag(PROGRAM_BAR_WIN))
								{
									/*
									* 信息条显示状态下，按键发送到信息条
									*/
									GUI_EndDialog("win_password_input");
									GUI_SetInterface("flush", NULL);
									GUI_SendEvent("win_prog_bar", event);
								}
							else
								{
									GUI_EndDialog("win_password_input");
									GUI_SetInterface("flush", NULL);
									GUI_SendEvent("win_full_screen", event);
								}
						}					
				return EVENT_TRANSFER_STOP;	
			case KEY_FAV:
			case KEY_MENU:
			case KEY_RECALL:
			case KEY_EPG:
			case KEY_TV_RADIO:
			case KEY_TV:
			case KEY_RADIO:
			if (FULL_SCREEN_WIN == app_win_get_focus_video_window())
				{
					GUI_EndDialog("win_password_input");
					GUI_EndDialog("win_prog_bar");
					GUI_SendEvent("win_full_screen", event);
				}	
				return EVENT_TRANSFER_STOP;		
			case KEY_OK:

				rtn = app_win_check_password_valid("win_password_input_edit",MAX_PASSWD_LEN);
				if(0 == rtn)
				{
					app_play_clear_msg_pop_type(MSG_POP_PROG_LOCK);
					app_prog_get_playing_pos_in_group(&pos);
					app_play_video_audio(pos);
					GUI_EndDialog("win_password_input");
				}
				else
				{
									GUI_SetProperty("win_password_input_edit","clear",NULL);
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Error password!",POPMSG_TYPE_OK);


					return EVENT_TRANSFER_STOP;
				}
				return EVENT_TRANSFER_STOP;	
			default:
				break;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}


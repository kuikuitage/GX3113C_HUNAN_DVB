/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_password_check.c
* Author    : 	chenth
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2014.6.19		  chenth 	 			creation
*****************************************************************************/
#include "app.h"

static event_list* swin_passowrd_time = NULL;
static int rech_end_ok_flag = 0;
static char win_name[64]={0};
static  int timer(void *userdata)
{
	if (1 == rech_end_ok_flag)
		{
			GUI_EndDialog("win_password_check");
		}
	return 0;
}
char *win_GetName()
{
    if(win_name[0]!='\0')
    {
        return win_name;
    }
    else
    {
        return NULL;
    }
}
void win_SetName(char *win)
{
    if(win)
    {
        strcpy(win_name,win);
    }
    else
    {
        win_name[0] = '\0';
    }
}

int PasswdCheckOK()
{
    return (rech_end_ok_flag>0)?1:0;
}

SIGNAL_HANDLER  int win_password_check_create(const char* widgetname, void *usrdata)
{
	rech_end_ok_flag = 0;
	swin_passowrd_time = create_timer(timer, 200, NULL,  TIMER_REPEAT);

	return 0;
}

SIGNAL_HANDLER  int win_password_check_destroy(const char* widgetname, void *usrdata)
{
	if (NULL != swin_passowrd_time )
	{
		remove_timer(swin_passowrd_time);
		swin_passowrd_time = NULL;			
	}

	return 0;
}


SIGNAL_HANDLER  int win_password_check_edit_keypress(const char* widgetname, void *usrdata)
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


SIGNAL_HANDLER  int win_password_check_reach_end(const char* widgetname, void *usrdata)
{
	int32_t rtn =0;
	rtn = app_win_check_password_valid("win_password_check_edit",MAX_PASSWD_LEN);
	if(0 == rtn)
	{
        char *pwin = NULL;
		rech_end_ok_flag = 1;
        if((pwin = win_GetName()))
        {
           char WinName[128];
           strcpy(WinName,pwin);
           win_SetName(NULL);
           GUI_CreateDialog(WinName);
        }
        
	}
	else
	{
		rech_end_ok_flag = 0;
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Error password!",POPMSG_TYPE_OK);
		GUI_SetProperty("win_password_check_edit","clear",NULL);
		return EVENT_TRANSFER_STOP;	
	}

	return 0;
}


SIGNAL_HANDLER  int win_password_check_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int32_t rtn =0;

	event = (GUI_Event *)usrdata;
	
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_EXIT:
				GUI_EndDialog("win_password_check");
				return EVENT_TRANSFER_STOP;	
			case KEY_LEFT:
			case KEY_RIGHT:
				return EVENT_TRANSFER_STOP;					
			case KEY_UP:
			case KEY_DOWN:				
				return EVENT_TRANSFER_STOP;					
			case KEY_MENU:
			case KEY_RECALL:
			case KEY_EPG:
			case KEY_TV_RADIO:
			case KEY_TV:
			case KEY_RADIO:
				return EVENT_TRANSFER_STOP;		
			case KEY_OK:

				rtn = app_win_check_password_valid("win_password_check_edit",MAX_PASSWD_LEN);
				if(0 == rtn)
				{
                    char *pwin = NULL;
					GUI_EndDialog("win_password_check");
                    rech_end_ok_flag = 1;
                    if((pwin = win_GetName()))
                    {
                       char WinName[128];
                       strcpy(WinName,pwin);
                       win_SetName(NULL);
                       GUI_CreateDialog(WinName);
                    }
				}
				else
				{	
                    rech_end_ok_flag = 0;
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Error password!",POPMSG_TYPE_OK);
					GUI_SetProperty("win_password_check_edit","clear",NULL);
					return EVENT_TRANSFER_STOP;
				}
				return EVENT_TRANSFER_STOP;	
			default:
				break;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}


/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_favorite_prog_list.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.05		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"
#include "win_main_menu.h"

SIGNAL_HANDLER int win_favorite_prog_list_create(const char* widgetname, void *usrdata)
{
	uint32_t pos = 0;

	char* osd_language=NULL;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
#ifdef SD

		//   	GUI_SetProperty("win_favorite_prog_title", "img", "DS_TITLE_PRO_SOLLECT.bmp");
#endif
#ifdef HD
		GUI_SetProperty("win_favorite_prog_title", "img", "title_favlist.bmp");
		GUI_SetProperty("win_fav_list_tip_image_exit", "img", "tips_exit.bmp");
#endif
	}
	else 
	{
#ifdef SD

		//	GUI_SetProperty("win_favorite_prog_title", "img", "DS_TITLE_PRO_SOLLECT_ENG.bmp");
#endif
#ifdef HD
		GUI_SetProperty("win_favorite_prog_title", "img", "title_favlist_e.bmp");
		GUI_SetProperty("win_fav_list_tip_image_exit", "img", "tips_exit_e.bmp");
#endif    
	}
	GUI_SetProperty("win_favorite_prog_list_time_text", "string", app_win_get_local_date_string());



	app_prog_change_group(GROUP_MODE_FAV,app_prog_get_stream_type(), 1);
	if(app_prog_get_num_in_group() > 0)
	{
		app_prog_get_playing_pos_in_group(&pos);
		GUI_SetProperty("win_favorite_prog_listview","select",(void*)&pos);
	}

	app_win_set_focus_video_window(FAVORITE_LIST_WIN);

	if (FALSE == app_play_get_play_status())
	{
		if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
		{
#ifndef DVB_CA_TYPE_DS_CAS50_FLAG
			app_play_switch_prog_clear_msg();
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
		}
		else
		{
			app_play_switch_prog_clear_msg();
		}	
		app_play_reset_play_timer(0);
	}


	return 0;
}

SIGNAL_HANDLER  int win_favorite_prog_list_destroy(const char* widgetname, void *usrdata)
{
	//	app_play_remove_play_tmer();
	return 0;
}

SIGNAL_HANDLER int win_favorite_prog_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_UP:
				return EVENT_TRANSFER_STOP;			
			case KEY_DOWN:
				return EVENT_TRANSFER_STOP;		
			case KEY_LEFT:
			case KEY_RIGHT:
				return EVENT_TRANSFER_STOP;					
			case KEY_OK:
			case KEY_EXIT:
				app_prog_change_group(GROUP_MODE_ALL,app_prog_get_stream_type(), 1);
				if (TRUE == app_get_win_create_flag(MAIN_MENU_WIN))
				{
					app_play_stop();
//					app_play_check_play_timer();
					GUI_EndDialog("win_favorite_prog_list");
					GUI_EndDialog("win_prog_menu");
					GUI_EndDialog("win_main_menu");
					app_play_reset_play_timer(0);
					app_win_set_focus_video_window(FULL_SCREEN_WIN);
				}
				else
				{
					/*
					 *  快捷键进入，直接退出到全屏
					 */
					app_play_stop();
//					app_play_video_window_full();
//					app_play_check_play_timer();
					app_play_set_zoom_para(0,0,VIDEO_WINDOW_W,VIDEO_WINDOW_H);
					GUI_EndDialog("win_favorite_prog_list");
					GUI_EndDialog("win_prog_menu");
					GUI_EndDialog("win_main_menu");
					app_play_reset_play_timer(0);				
					app_win_set_focus_video_window(FULL_SCREEN_WIN);					
				}


				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL:
			case KEY_MENU:
				if (TRUE == app_get_win_create_flag(MAIN_MENU_WIN))
				{
					app_win_set_focus_video_window(MAX_WINDOW_NUM);
					app_play_stop();
#ifdef APP_SD
					app_play_set_zoom_para(340, 40, 324, 242);	
#endif
#ifdef APP_HD
					app_play_video_window_zoom(400, 160, 500, 290);
#endif				
					GUI_EndDialog("win_favorite_prog_list");	
				}
				else
				{

					/*
					 *  快捷键进入，直接退出到全屏
					 */
					app_play_stop();
//					app_play_video_window_full();
//					app_play_check_play_timer();
					app_play_set_zoom_para(0,0,VIDEO_WINDOW_W,VIDEO_WINDOW_H);
					GUI_EndDialog("win_favorite_prog_list");
					GUI_EndDialog("win_prog_menu");
					GUI_EndDialog("win_main_menu");
					app_play_reset_play_timer(0);
					app_win_set_focus_video_window(FULL_SCREEN_WIN);					
				}
				return EVENT_TRANSFER_STOP;	
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int win_favorite_prog_listview_get_total(const char* widgetname, void *usrdata)
{
	return app_prog_get_num_in_group();
}

SIGNAL_HANDLER  int win_favorite_prog_listview_get_data(const char* widgetname, void *usrdata)
{
	static GxBusPmDataProg prog;
	static uint8_t pArry[40] = {0};
	ListItemPara *item_para =NULL;	
	Lcn_State_t lcn_flag=0;

	if(NULL == widgetname || NULL == usrdata)	
	{		
		return (1);	
	}	

	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para) return GXCORE_ERROR;
	memset(pArry,0,5);

	lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_OFF == lcn_flag)
	{
		sprintf((void*)pArry, "%03d",item_para->sel+1);
	}
	else
	{
		GxBus_PmProgGetByPos(item_para->sel,1,&prog);
		sprintf((void*)pArry, "%03d",prog.pos);
	}

	item_para->x_offset = 5;
	item_para->string = (char*)pArry;

	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;
	GxBus_PmProgGetByPos(item_para->sel,1,&prog);
	//	sprintf((void*)pArry1,"%s",prog.prog_name);	
	item_para->string = (char*)prog.prog_name;;

#ifdef APP_SD
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;
	if(TRUE == prog.scramble_flag)
	{
		item_para->image = "DS_Channel_list_Money.bmp";
	}
	else
	{
		item_para->image = NULL;
	}
#endif

	return 0;
}

SIGNAL_HANDLER  int win_favorite_prog_listview_change(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;
	uint32_t pos = 0;
	GUI_GetProperty("win_favorite_prog_listview","select",(void*)&value);

	app_prog_get_playing_pos_in_group(&pos);
	app_prog_save_playing_pos_in_group(value);	
	if (pos != value)
	{
		app_play_switch_prog_clear_msg();	
		app_play_reset_play_timer(PLAY_TIMER_DURATION);					
	}

	return 0;
}
SIGNAL_HANDLER  int win_favorite_prog_list_show(const char* widgetname, void *usrdata)
{
	//GUI_SetInterface("flush", NULL);
#ifdef APP_SD
	//GUI_SetInterface("video_top", NULL);
#endif
	return EVENT_TRANSFER_KEEPON;

}





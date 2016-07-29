/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_menu_tv_list.c
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

#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
#endif

static uint32_t pos = 0;
SIGNAL_HANDLER int win_menu_tv_list_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


	osd_language = app_flash_get_config_osd_language();
	GUI_SetProperty("win_menu_tv_list_time_text", "string", app_win_get_local_date_string());


	if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
	{
#ifdef APP_SD
		if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
		//	GUI_SetProperty("win_tv_prog_image_title", "img", "DS_Title_Prog_list.bmp");
		}
		else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
		{ 
		//	GUI_SetProperty("win_tv_prog_image_title", "img", "DS_Title_Prog_list_Eng.bmp");
		}
#endif
		//GUI_SetProperty("win_tv_prog_image_title", "img", "title_tvprogram.bmp");
#ifdef APP_HD
		if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			GUI_SetProperty("win_tv_prog_image_title", "img", "title_tvprogram.bmp");
		}
		else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
		{
			GUI_SetProperty("win_tv_prog_image_title", "img", "title_tvprogram_e.bmp");
		}

		GUI_SetProperty("win_menu_tv_list_text_title", "string", "TV");
#endif
		GUI_SetProperty("win_tv_prog_image_title", "state", "show");

	}
	else
	{
#ifdef APP_SD
		if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
		//	GUI_SetProperty("win_tv_prog_image_title", "img", "DS_Title_Prog_list.bmp");
		}
		else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
		{ 
		//	GUI_SetProperty("win_tv_prog_image_title", "img", "DS_Title_Prog_list_Eng.bmp");
		}
#endif

#ifdef APP_HD
		if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			GUI_SetProperty("win_tv_prog_image_title", "img", "title_radio.bmp");
		}
		else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
		{
			GUI_SetProperty("win_tv_prog_image_title", "img", "title_radio_e.bmp");
		}
#endif
		//GUI_SetProperty("win_tv_prog_image_title", "img", "title_radio.bmp");
#ifdef APP_SD
		GUI_SetProperty("win_menu_tv_list_text_title", "string", "Radio");
#endif
		GUI_SetProperty("win_tv_prog_image_title", "state", "show");
	}

	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
		//    	GUI_SetProperty("win_tv_list_tip_image_exit", "img", "tips_exit.bmp");
		//  	GUI_SetProperty("win_tv_list_tip_image_ok", "img", "tips_confirm.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		//	GUI_SetProperty("win_tv_list_tip_image_exit", "img", "tips_exit_e.bmp");
		//	GUI_SetProperty("win_tv_list_tip_image_ok", "img", "tips_confirm_e2.bmp");
	}


	if(app_prog_get_num_in_group() > 0)
	{
		app_prog_get_playing_pos_in_group(&pos);
		GUI_SetProperty("win_tv_prog_listview","select",(void*)&pos);
	}

	app_win_set_focus_video_window(PROGRAM_TV_LIST_WIN);

	if (FALSE == app_play_get_play_status())
	{
		app_play_switch_prog_clear_msg();	
		app_play_reset_play_timer(0);
	}
#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_menu_pic_display("win_menu_tv_list_image_ad");
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_show_pic("win_menu_tv_list_image_ad",M_AD_PLACE_ID_PROG_LIST);
#endif
	return 0;
}

SIGNAL_HANDLER  int win_menu_tv_list_destroy(const char* widgetname, void *usrdata)
{
//	app_play_remove_play_tmer();
#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_hide_menu_pic();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_menu_tv_list_image_ad");
#endif
	return 0;
}

SIGNAL_HANDLER int win_menu_tv_list_keypress(const char* widgetname, void *usrdata)
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
			app_play_stop(); 
				app_play_video_window_full();

				GUI_EndDialog("win_menu_tv_list");
				GUI_EndDialog("win_main_menu");
			GUI_SetInterface("flush", NULL);
			app_play_reset_play_timer(0);
				app_win_set_focus_video_window(FULL_SCREEN_WIN);
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL:
			case KEY_MENU:
			app_play_stop();

				GUI_EndDialog("win_menu_tv_list");
			GUI_SetInterface("flush", NULL);

				if (TRUE != app_get_win_create_flag(MAIN_MENU_WIN))
				{
					GUI_CreateDialog("win_main_menu");
				}
				GUI_SetInterface("flush", NULL);
#ifdef APP_SD
				app_play_set_zoom_para(192, 152, 348, 240);
			app_play_reset_play_timer(0);
#endif
#ifdef APP_HD
				app_play_video_window_zoom(400, 160, 500, 290);
#endif
				app_win_set_focus_video_window(MAIN_MENU_WIN);
				return EVENT_TRANSFER_STOP;	
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int win_tv_prog_listview_get_total(const char* widgetname, void *usrdata)
{
	return app_prog_get_num_in_group();
}

SIGNAL_HANDLER  int win_tv_prog_listview_get_data(const char* widgetname, void *usrdata)
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
	item_para->string = (char*)prog.prog_name;

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

SIGNAL_HANDLER  int win_tv_prog_listview_change(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;
	//	uint32_t pos = 0;
	GUI_GetProperty("win_tv_prog_listview","select",(void*)&value);

	//	app_prog_get_playing_pos_in_group(&pos);
	app_prog_save_playing_pos_in_group(value);
	if (pos != value)
	{
		pos = value;
		app_play_switch_prog_clear_msg();	
		app_play_reset_play_timer(PLAY_TIMER_DURATION);
	}


	return 0;
}




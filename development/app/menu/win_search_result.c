/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_search_result.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.12		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"

extern int32_t app_full_screen_timer_stop(void);
extern int32_t app_full_screen_timer_reset(void);
static void win_serach_result_show_save_tip(void)
{
	/*
	* 提示保存，显示搜索电视、广播等节目个数
	*/
	uint8_t buf[50]= {0};
    search_result* searchResult = app_search_get_result_para();

	GUI_CreateDialog("win_search_result_tip");

	if ((searchResult->app_tv_num >0)||(searchResult->app_radio_num>0))
		GUI_SetProperty("win_search_result_tip_save","state","show");		

	memset(buf,0,50);
	sprintf((void*)buf,"%03d",searchResult->app_tv_num);
	GUI_SetProperty("win_search_result_tip_tv_num1","string",buf);
	GUI_SetProperty("win_search_result_tip_tv_num1","state","show");

	memset(buf,0,50);
	sprintf((void*)buf,"%03d",searchResult->app_radio_num);
	GUI_SetProperty("win_search_result_tip_radio_num1","string",buf);
	GUI_SetProperty("win_search_result_tip_radio_num1","state","show");

	/* show process */
	uint32_t provalue = 100;
	uint8_t buf1[10]={0};
	
	GUI_SetProperty("win_search_result_progbar", "value", (void*)&provalue);

	sprintf((char*)buf1, "%d%%", provalue);
	GUI_SetProperty("win_search_result_search_value", "string", buf1);
	
	GUI_SetInterface("flush", NULL);
	return ;	
}

SIGNAL_HANDLER int win_search_result_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;
	uint32_t value = 0;
	app_hide_prompt();

    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))    
   {
#ifdef SD

 //  	   GUI_SetProperty("DS_SEARCHING_TITLE_CH.bmp","img","title_ch_search_result.bmp");	
#endif
#ifdef HD
   	   GUI_SetProperty("win_search_result_title","img","title_ch_search_result.bmp");	
    	GUI_SetProperty("win_search_result_tip_image_exit", "img", "tips_exit.bmp");
#endif 
   }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    { 
#ifdef SD
   //	   GUI_SetProperty("DS_SEARCHING_TITLE_CH.bmp","img","title_ch_search_result.bmp");	
#endif	    
#ifdef HD
    	    GUI_SetProperty("win_search_result_title","img","title_en_search_result.bmp");
    	GUI_SetProperty("win_search_result_tip_image_exit","img","tips_exit_e.bmp");
#endif
     }

	/*
	* reset progress zero
	*/
	GUI_SetProperty("win_search_result_progbar", "value", (void*)&value);
	
	app_full_screen_timer_stop();
	app_set_win_create_flag(SEARCH_RESULT_WIN);
	app_win_set_focus_video_window(SEARCH_RESULT_WIN);
	return 0;
}

SIGNAL_HANDLER  int win_search_result_destroy(const char* widgetname, void *usrdata)
{
	app_full_screen_timer_reset();
	GxFrontend_StartMonitor(0);
	app_set_win_destroy_flag(SEARCH_RESULT_WIN);
	app_win_set_focus_video_window(FULL_SCREEN_WIN);
	return 0;
}

SIGNAL_HANDLER int win_search_result_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int32_t pNitSubtId=0;
	uint32_t pNitRequestId=0;

	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_RECALL:
		case KEY_MENU:
		case KEY_EXIT:
			 app_table_nit_get_search_filter_info(&pNitSubtId,&pNitRequestId);
			if (-1== pNitSubtId)
				{
					/*
					* 必须等待NIT收到或超时退出，否则会出现死机
					*/
					app_search_scan_stop();				
				}
			return EVENT_TRANSFER_STOP;			
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

int app_service(void* usrdata)
{
	GxMessage * msg;
	GxMsgProperty_NewProgGet* params1 = NULL;
	GxMsgProperty_SatTpReply* params2 = NULL;
	GxMsgProperty_StatusReply* params3 = NULL;
	char* osd_language=NULL;
	search_ok_msg ok_msg = {0};
	uint32_t play_pos;
	uint8_t app_popmsgbuf[100]={0};
	search_new_prog_get_msg new_prog_get_msg ={0};
	search_sat_tp_reply_msg sat_tp_reply_msg = {0};
	msg = (GxMessage*)usrdata;

	switch(msg->msg_id)
	{
	case GXMSG_SI_SUBTABLE_OK:
	case GXMSG_SI_SUBTABLE_TIME_OUT:
		app_search_si_subtable_msg(msg);
		return EVENT_TRANSFER_STOP;	
	case GXMSG_SEARCH_NEW_PROG_GET:
		params1 = (GxMsgProperty_NewProgGet*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_NewProgGet);
		
		new_prog_get_msg.params = params1;
		new_prog_get_msg.widget_tv_list_name = "win_search_result_tv_name";
		new_prog_get_msg.widget_tv_num = "win_search_result_tv_num";
		new_prog_get_msg.widget_radio_list_name = "win_search_result_radio_name";
		new_prog_get_msg.widget_radio_num = "win_search_result_radio_num";
		new_prog_get_msg.max_line_num = SEARCH_PROG_LIST_MAX;
		app_search_new_prog_get_msg(new_prog_get_msg);
		GUI_SetInterface("flush", NULL);
		break;
	case GXMSG_SEARCH_SAT_TP_REPLY: 
		params2 = (GxMsgProperty_SatTpReply*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_SatTpReply);
		sat_tp_reply_msg.params = params2;
		
		sat_tp_reply_msg.widget_search_progress_bar = "win_search_result_progbar";
		sat_tp_reply_msg.widget_search_progress_bar_value = "win_search_result_search_value";
		sat_tp_reply_msg.widget_tv_list_name = "win_search_result_tv_name";
		sat_tp_reply_msg.widget_radio_list_name = "win_search_result_radio_name";
		sat_tp_reply_msg.widget_fre_name = "win_search_result_frequency_value";  
			
 		app_search_sat_tp_reply_msg(sat_tp_reply_msg);
		break;

	case GXMSG_SEARCH_STATUS_REPLY:
		params3 = (GxMsgProperty_StatusReply*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_StatusReply);
		osd_language = app_flash_get_config_osd_language();
	    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			if(params3->type == 1)
			   sprintf((void*)app_popmsgbuf,"错误%d:节目达到数据库上限",params3->type);
			
		}
		else
		{
			if(params3->type == 1) 
			  sprintf((void*)app_popmsgbuf,"error %d:database overflow",params3->type);	
		}
		printf("app_popmsgbuf = %s\n",app_popmsgbuf);

		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, (char*)app_popmsgbuf, POPMSG_TYPE_OK);

		break;

	case GXMSG_SEARCH_STOP_OK:
			app_prog_get_playing_pos_in_group(&play_pos);
			ok_msg.play_pos = 0; //play_pos; /*搜索结束默认播放的节目*/
			ok_msg.save_flag = TRUE;
			ok_msg.play_flag = TRUE;
			

#ifdef MOVE_FUNCTION
			ok_msg.taxis_mode = TAXIS_MODE_NON;// TAXIS_MODE_SERVICE_ID;
#else
			ok_msg.taxis_mode = TAXIS_MODE_SERVICE_ID;
#endif
	ok_msg.app_search_ok_pomsg = NULL; //show_search_ok_msg;
			ok_msg.widget_search_bar = "win_search_result_progbar";
			ok_msg.widget_search_bar_value = "win_search_result_search_value";
			win_serach_result_show_save_tip();	//保存结果		
			app_search_stop_ok_msg(ok_msg);
			
			GUI_EndDialog("win_search_result_tip");			
			GUI_EndDialog("win_search_result");
			GUI_EndDialog("win_manual_search");
			GUI_EndDialog("win_all_search");
			GUI_EndDialog("win_search_menu");
			GUI_EndDialog("win_sys_set");
			GUI_EndDialog("win_main_menu");
			app_hide_prompt();
			app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
			app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);
			/*app_hide_prompt();
			app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
			app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);*/

			GUI_SetInterface("flush", NULL);


			app_win_set_focus_video_window(FULL_SCREEN_WIN);

			app_play_video_window_full();
			
			if (TRUE == ok_msg.play_flag)
			{
				if (0 != app_prog_get_num_in_group())
				{
					GUI_SetInterface("flush", NULL);
					if (GXBUS_PM_PROG_TV != app_prog_get_stream_type() && 0 != app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0))
						{
							app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);
						}
//					app_play_reset_play_timer(0);
				}
				else
				{
                  GUI_SetInterface("flush", NULL);
					/*
					* 无节目默认为视频模式
					*/
					if (GXBUS_PM_PROG_TV != app_prog_get_stream_type())
						{
							app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);
						}
				}

				
				if (0 != app_prog_get_num_in_group())
					{
						app_play_reset_play_timer(0);
						GUI_CreateDialog("win_prog_bar");
					}
			}
			#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD)
			if (0 == app_prog_get_num_in_group())
			{
				//if(app_win_check_video_win())
					app_play_show_logo_for_tv();
			}
			#endif
			
		break;
	default:
		break;
	}

	return GXMSG_OK;
}



SIGNAL_HANDLER  int app_search_result_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

	switch(event->type)
	{
	case GUI_SERVICE_MSG:
		app_service(event->msg.service_msg);
		break;
	default:
		break;
	}

	return EVENT_TRANSFER_STOP;
}


/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_prog_info.c
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
static event_list* swin_prog_info_time = NULL;

static  int timer(void *userdata)
{
	strength_xml strengthxml = {0};
	signal_xml singalxml = {0};
	char* focus_Window = (char*)GUI_GetFocusWindow();

	/*如果焦点不在当前界面，不响应TIMER，避免界面错乱*/
	if((NULL != focus_Window)&&( strcasecmp("win_prog_info", focus_Window) != 0))
		{
			return 0;
		}

	if(1 == GxFrontend_QueryStatus(0))
		{
			GUI_SetProperty("win_prog_info_lock_value", "string", "Locked");
			strengthxml.lock_status = 1;	
			singalxml.lock_status =1;
		}
	else
		{
			GUI_SetProperty("win_prog_info_lock_value", "string", "UnLock");	
			strengthxml.lock_status = 0;	
			singalxml.lock_status =0;
		}

	strengthxml.widget_name_strength_bar = "win_prog_info_signal_strength_progbar";
	strengthxml.widget_name_strength_bar_value = "win_prog_info_signal_strength_value";
	app_search_set_strength_progbar(strengthxml);

	singalxml.widget_name_signal_bar = "win_prog_info_signal_quality_progbar";
	singalxml.widget_name_signal_bar_value = "win_prog_info_signal_quality_value";
	singalxml.widget_name_error_rate_bar = NULL;
	singalxml.widget_name_error_rate = "win_prog_info_err_value";
 	app_search_set_signal_progbar(singalxml);
			
	return 0;
}

SIGNAL_HANDLER int win_prog_info_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;
	uint32_t duration = 1000;
	char temp[20];
	GxBusPmDataTP tp = {0};
	uint32_t  App_AudioTrack;
	int8_t   chEntitleBuf[60]= {0};
	GxBusPmDataProg Prog;
    uint32_t pos;
	uint32_t value = 0;



    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
#ifdef SD
 //   	    GUI_SetProperty("win_prog_info_title", "img", "DS_TITLE_PRO_SIGNAL.bmp");
#endif
#ifdef HD
    	    GUI_SetProperty("win_prog_info_title", "img", "title_programinfo.bmp");
    	GUI_SetProperty("win_prog_info_tip_image_exit", "img", "tips_exit.bmp");
#endif     
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
     {
#ifdef SD
    //	    GUI_SetProperty("win_prog_info_title", "img", "DS_TITLE_PRO_SIGNAL_ENG.bmp");
#endif

#ifdef HD	    
    	GUI_SetProperty("win_prog_info_title", "img", "title_programinfo_e.bmp");
    	GUI_SetProperty("win_prog_info_tip_image_exit", "img", "tips_exit_e.bmp");
#endif   
    }

	   
   /*
   * reset progress zero
   */
   GUI_SetProperty("win_prog_info_signal_strength_progbar", "value", (void*)&value);
   GUI_SetProperty("win_prog_info_signal_quality_progbar", "value", (void*)&value);


	swin_prog_info_time = create_timer(timer, duration, NULL,  TIMER_REPEAT);

	app_prog_get_playing_pos_in_group(&pos);
	if (app_prog_get_num_in_group()>0)
	{	
		GxBus_PmProgGetByPos(pos,1,&Prog);
		GUI_SetProperty("win_prog_info_prog_name", "string", Prog.prog_name);
		printf("%s\n",Prog.prog_name);

		memset(temp,0,20);
		sprintf(temp,"%d", Prog.video_pid);
		GUI_SetProperty("win_prog_info_video_pid_value", "string", temp);

		memset(temp,0,20);
		sprintf(temp,"%d", Prog.cur_audio_pid);
		GUI_SetProperty("win_prog_info_audio_pid_value", "string", temp);

		memset(temp,0,20);
		sprintf(temp,"%d", Prog.pcr_pid);
		GUI_SetProperty("win_prog_info_pcr_pid_value", "string", temp);	

		GxBus_PmTpGetById(Prog.tp_id, &tp);
		memset(temp,0,20);

		sprintf(temp,"%d MHz", tp.frequency/1000);
		GUI_SetProperty("win_prog_info_fre_value", "string", temp);	
	}

	App_AudioTrack = app_flash_get_config_audio_track();
	if(AUDIO_TRACK_LEFT == App_AudioTrack)
	{
		GUI_SetProperty("win_prog_info_track_value", "string", "Left");	
	}
	else if(AUDIO_TRACK_RIGHT == App_AudioTrack)
	{
		GUI_SetProperty("win_prog_info_track_value", "string", "Right");		
	}
	else if(AUDIO_TRACK_STEREO == App_AudioTrack)
	{
		GUI_SetProperty("win_prog_info_track_value", "string", "Stereo");		
	}
	
	GxBus_PmProgGetByPos(pos,1,&Prog);

	if(TRUE == Prog.scramble_flag)
	{
		sprintf((char*)chEntitleBuf, "Scramble");
	}
	else
	{
		sprintf((char*)chEntitleBuf, "Free");
	}

	app_search_lock_tp(tp.frequency/1000, tp.tp_c.symbol_rate/1000,
		INVERSION_OFF, tp.tp_c.modulation-1,0);


	GUI_SetProperty("win_prog_info_authorization_info_value", "string", (void*)chEntitleBuf);
	GUI_SetProperty("win_prog_info_time_text", "string", app_win_get_local_date_string());

	return 0;
}

SIGNAL_HANDLER  int win_prog_info_destroy(const char* widgetname, void *usrdata)
{
	remove_timer(swin_prog_info_time);
	swin_prog_info_time = NULL;

	return 0;
}

SIGNAL_HANDLER int win_prog_info_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_RECALL:
		case KEY_EXIT:
		case KEY_OK:
		case KEY_MENU:
			GUI_EndDialog("win_prog_info");
			return EVENT_TRANSFER_STOP;								
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


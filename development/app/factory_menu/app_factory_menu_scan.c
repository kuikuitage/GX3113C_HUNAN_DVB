/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_program_detail.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.13		  lijq 	 			creation
*****************************************************************************/
#include "app.h"
#include "gxprogram_manage_berkeley.h"
static search_dvbc_param previous_param={0};

static event_list* spApp_fatory_time_scan = NULL;
static uint8_t fatorysearchflags = false;

extern float app_float_edit_str_to_value(const char *str);
int fatory_menu_scan_service(void* usrdata)
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
		new_prog_get_msg.widget_tv_list_name = "win_factory_menu_scan_text_list";
		new_prog_get_msg.widget_tv_num =NULL;
		new_prog_get_msg.widget_radio_list_name = "win_factory_menu_scan_text_list";
		new_prog_get_msg.widget_radio_num = NULL;
		new_prog_get_msg.max_line_num = SEARCH_PROG_LIST_MAX;
		app_search_new_prog_get_msg(new_prog_get_msg);
		GUI_SetInterface("flush", NULL);
		break;
	case GXMSG_SEARCH_SAT_TP_REPLY: 
		params2 = (GxMsgProperty_SatTpReply*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_SatTpReply);
		sat_tp_reply_msg.params = params2;
		
		sat_tp_reply_msg.widget_search_progress_bar = NULL;
		sat_tp_reply_msg.widget_search_progress_bar_value = NULL;
		sat_tp_reply_msg.widget_tv_list_name = NULL;
		sat_tp_reply_msg.widget_radio_list_name =NULL;
		sat_tp_reply_msg.widget_fre_name =NULL;  
			
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
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START, (char*)app_popmsgbuf, POPMSG_TYPE_OK);
		break;

	case GXMSG_SEARCH_STOP_OK:
			app_prog_get_playing_pos_in_group(&play_pos);
			ok_msg.play_pos = 0; //play_pos; /*搜索结束默认播放的节目*/
			ok_msg.save_flag = TRUE;
			ok_msg.play_flag = TRUE;
			ok_msg.taxis_mode = TAXIS_MODE_SERVICE_ID;//TAXIS_MODE_NON;
			ok_msg.app_search_ok_pomsg = NULL; //show_search_ok_msg;
			ok_msg.widget_search_bar = "win_search_result_progbar";
			ok_msg.widget_search_bar_value = "win_search_result_search_value";
			//win_serach_result_show_save_tip();			
			app_search_stop_ok_msg(ok_msg);


			GUI_EndDialog("win_factory_menu_scan");
			GUI_CreateDialog("win_factory_menu");

			//if (TRUE == ok_msg.play_flag)
			{
				if (0 != app_prog_get_num_in_group())
				{


					app_play_reset_play_timer(0);
				}
				else
				{
		          
					/*
					* 无节目默认为视频模式
					*/
					if (GXBUS_PM_PROG_TV != app_prog_get_stream_type())
						{
							app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);
						}
				}
			}
			
		break;
	default:
		break;
	}

	return GXMSG_OK;
}





SIGNAL_HANDLER  int app_fatory_menu_scan_service(const char* widgetname, void *usrdata)
{
	
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

	switch(event->type)
	{
	case GUI_SERVICE_MSG:
		fatory_menu_scan_service(event->msg.service_msg);
		break;
	default:
		break;
	}

	return EVENT_TRANSFER_STOP;
}

static  int fatory_menu_scan_time(void *userdata)
{
	strength_xml strengthxml = {0};
	signal_xml singalxml = {0};
	char* focus_Window = (char*)GUI_GetFocusWindow();


	if((NULL !=focus_Window )&&( strcasecmp("win_factory_menu_scan", focus_Window) != 0))
	{
		return 0;
	}
	
	if(previous_param.flag==0)
	{
		return 0;
	}

	if (previous_param.fre < FRE_BEGIN_LOW || previous_param.fre > FRE_BEGIN_HIGH)
	{
		return 0;
	}

	if (previous_param.symbol_rate < 2000 || previous_param.symbol_rate > 7000)
	{
		return 0;
	}


	if(1 == GxFrontend_QueryStatus(0))
	{
		GUI_SetProperty("win_factory_menu_scan_text_lock", "string", "Locked");
		strengthxml.lock_status = 1;	
		singalxml.lock_status =1;
	}
	else
	{
		GUI_SetProperty("win_factory_menu_scan_text_lock", "string", "UnLock"); 
		strengthxml.lock_status = 0;	
		singalxml.lock_status =0;
	}
	strengthxml.widget_name_strength_bar = "win_factory_menu_scan_progbar_lev";
	strengthxml.widget_name_strength_bar_value = "win_factory_menu_scan_text_lev";
	app_search_set_strength_progbar(strengthxml);

	singalxml.widget_name_signal_bar = "win_factory_menu_scan_progbar_qua";
	singalxml.widget_name_signal_bar_value = "win_factory_menu_scan_text_qua";
	singalxml.widget_name_error_rate_bar = NULL;
	singalxml.widget_name_error_rate = NULL;
	app_search_set_signal_progbar(singalxml);

	return 0;
}





SIGNAL_HANDLER  int app_factory_menu_scan_box_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t fre = 0;
	float edit_fre = 0;
	char *value;



	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:

				GUI_GetProperty("win_factory_menu_scan_edit_fre", "string", &value);
				edit_fre = app_float_edit_str_to_value(value);
				fre = 1000* edit_fre;
				previous_param.flag = 1;
				if (fre != previous_param.fre)
				{
						previous_param.fre = fre;
						app_search_lock_tp(previous_param.fre, previous_param.symbol_rate,
									INVERSION_OFF, previous_param.qam,0);
						
				}
			return EVENT_TRANSFER_STOP;	
		case KEY_OK:


			GUI_GetProperty("win_factory_menu_scan_edit_fre", "string", &value);
			edit_fre = app_float_edit_str_to_value(value);
			fre = 1000* edit_fre;

		
			if (FALSE ==  app_win_check_fre_vaild(FRE_MID,fre))
			{
				return EVENT_TRANSFER_STOP;						
			}
	
			
			if(1 != GxFrontend_QueryStatus(0))
			{
				app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Lock Failed",POPMSG_TYPE_OK);
				return EVENT_TRANSFER_STOP;	
			}

			if(fatorysearchflags == true)
			{
				return EVENT_TRANSFER_STOP;
			}
			fatorysearchflags = true;
			app_search_scan_manual_mode(fre,previous_param.symbol_rate,previous_param.qam);
			return EVENT_TRANSFER_STOP;
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_factory_menu_scan_create(const char* widgetname, void *usrdata)
{
	search_dvbc_param tmp_param;
	char App_Fre[7];//频点
	tmp_param.fre= app_flash_get_config_center_freq();
	memset(App_Fre,0,7);
	sprintf(App_Fre,"%03d.%d", tmp_param.fre/1000,(tmp_param.fre%1000)/100);
	GUI_SetProperty("win_factory_menu_scan_edit_fre", "string", App_Fre);

	tmp_param.symbol_rate= app_flash_get_config_manual_search_symbol_rate();
	tmp_param.qam= app_flash_get_config_manual_search_qam();
	app_search_lock_tp(tmp_param.fre, tmp_param.symbol_rate,INVERSION_OFF, tmp_param.qam,0);


	GUI_SetFocusWidget("win_factory_menu_scan_box_fre");
	spApp_fatory_time_scan = create_timer(fatory_menu_scan_time, 1000, NULL,  TIMER_REPEAT);

	previous_param.fre= tmp_param.fre;
	previous_param.symbol_rate= tmp_param.symbol_rate;
	previous_param.qam= tmp_param.qam;
	previous_param.flag = 1;


	return 0;
}

SIGNAL_HANDLER  int app_factory_menu_scan_destroy(const char* widgetname, void *usrdata)
{




	remove_timer(spApp_fatory_time_scan);
	spApp_fatory_time_scan = NULL;
	fatorysearchflags = false;

	return 0;
}

SIGNAL_HANDLER  int app_factory_menu_scan_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(fatorysearchflags == true)
	{
		return EVENT_TRANSFER_STOP;
	}
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_GREEN:
				GUI_EndDialog("win_factory_menu_scan");
				GUI_CreateDialog("win_factory_menu_port");
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL: 
			case KEY_EXIT:
				GUI_EndDialog("win_factory_menu_scan");
				GUI_CreateDialog("win_factory_menu");
				return EVENT_TRANSFER_STOP;	
			case KEY_YELLOW:
			case KEY_MENU:
				GUI_EndDialog("win_factory_menu_scan");
				GUI_CreateDialog("win_factory_menu");
				return EVENT_TRANSFER_STOP;	
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;	
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}

	return EVENT_TRANSFER_KEEPON;
}




/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_main_menu.c
* Author    : 	lijq
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
#include "gxextra.h"
#include "gxapp_sys_config.h"
#include "app_common_panel.h"
#include "app_common_prog.h"
#include "panel_key.h"
#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#if DVB_PVR_FLAG
#include "app_pvr.h"

extern 	void tms_state_change(PlayerStatus state);
extern void tms_speed_change(GxMsgProperty_PlayerSpeedReport *spd);
extern 	void app_pvr_stop(void);
#endif

extern int win_pop_tip_show_hotplug_gxmsg(char* msg);
#ifdef _DEBUG
#define TOP_MSG_DEBUG
#endif
#ifdef TOP_MSG_DEBUG
#define TOP_Msg_Dbg(...)                          do {                                            \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#else
#define TOP_Msg_Dbg(...)                
#endif

int service_msg_to_app(void* usrdata)
{
	GxMessage * msg;
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	GxMsgProperty_ExtraTimeOk *pTimeOk = NULL;
	GxMsgProperty_PlayerResolutionReport *pResolution = NULL;
	GxTime sys_time;
    unsigned char para_usb = 0;
    //static u_int8_t count = 0;
	char* focus_Window = NULL;
	/*
	* 处理gxbus,ca等其他模块发送的消息，消息框显示状态下
	* 此处直接处理消息可能会导致死机
	*/
	
    //TOP_Msg_Dbg("[service_msg_to_app] msg->msg_id=0x%x\n",msg->msg_id);	
   
	switch(msg->msg_id)
	{
		case GXMSG_SEARCH_NEW_PROG_GET:
		case GXMSG_SEARCH_SAT_TP_REPLY:
		case GXMSG_SEARCH_STATUS_REPLY:
		case GXMSG_SEARCH_STOP_OK:
		case GXMSG_SI_SUBTABLE_OK:
		case GXMSG_SI_SUBTABLE_TIME_OUT:
			focus_Window = (char*)GUI_GetFocusWindow();
			if (NULL == focus_Window)
				break;
			if (TRUE == app_get_win_create_flag(SEARCH_RESULT_WIN))
				{
					GUI_EndDialog("win_pop_tip");
					if( 0 != strcasecmp("win_search_result", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_search_result)
							*/
							
							GUI_SendEvent("win_search_result",usrdata);
							return EVENT_TRANSFER_STOP;
						}
				}
			break;	
		case GXMSG_PLAYER_STATUS_REPORT:
			focus_Window = (char*)GUI_GetFocusWindow();
			if (NULL == focus_Window)
				break;	
			
			if (TRUE == app_get_win_create_flag(MEDIA_MOVIE_WIN))	
				{
					if( 0 != strcasecmp("win_movie_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_movie_view)
							*/
							
							GUI_SendEvent("win_movie_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}

			if (TRUE == app_get_win_create_flag(MEDIA_MUSIC_WIN))	
				{
					if( 0 != strcasecmp("win_music_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_music_view)
							*/
							
							GUI_SendEvent("win_music_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}

			if (TRUE == app_get_win_create_flag(MEDIA_PIC_WIN))	
				{
					if( 0 != strcasecmp("win_pic_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_pic_view)
							*/
							
							GUI_SendEvent("win_pic_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}

			if (TRUE == app_get_win_create_flag(MEDIA_TEXT_WIN))	
				{
					if( 0 != strcasecmp("win_text_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_text_view)
							*/
							
							GUI_SendEvent("win_text_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}
#if DVB_PVR_FLAG
			 if(g_AppPvrOps.state != PVR_DUMMY)
			 {
			 	msg = (GxMessage*)event->msg.service_msg;
			 	GxMsgProperty_PlayerStatusReport *status;
				status = GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
				if (strcmp((char*)(status->player), PLAYER_FOR_REC) == 0)
				{
					
					if (status->status == PLAYER_STATUS_RECORD_FULL)
				   {   
				   	   GxBusPmDataProg prog_data;
					   uint32_t rec_prog_id = g_AppPvrOps.env.prog_id;
					   uint32_t pos;
					  				
					   app_pvr_stop();
					   GUI_EndDialog("wnd_pvr_bar");
					   GUI_EndDialog("win_menu_volumes");
					   GUI_EndDialog("win_prog_bar");
			
					   app_prog_get_playing_pos_in_group(&pos);
					   GxBus_PmProgGetByPos(pos, 1, &prog_data);
					   if(prog_data.id == rec_prog_id)
					   {   
						   app_play_video_audio(pos);
					   }
					   app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Enough Space!",POPMSG_TYPE_OK);
				   }
				}
				else if (strcmp((char*)(status->player), PLAYER_FOR_NORMAL) == 0)
				{
					tms_state_change(status->status);
				}
			 }
#endif			
			break;
		case GXMSG_PLAYER_AVCODEC_REPORT:
			focus_Window = (char*)GUI_GetFocusWindow();
			if (NULL == focus_Window)
				break;	
			
			if (TRUE == app_get_win_create_flag(MEDIA_MOVIE_WIN))	
				{
					if( 0 != strcasecmp("win_movie_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_movie_view)
							*/
							
							GUI_SendEvent("win_movie_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}

			if (TRUE == app_get_win_create_flag(MEDIA_MUSIC_WIN))	
				{
					if( 0 != strcasecmp("win_music_view", focus_Window))
						{
							/*
							* 消息转发到指定窗体(win_music_view)
							*/
							
							GUI_SendEvent("win_music_view",usrdata);
							return EVENT_TRANSFER_STOP;
						}					
				}
			break;
		case GXMSG_PLAYER_SPEED_REPORT:
#if DVB_PVR_FLAG
			if(g_AppPvrOps.state != PVR_DUMMY)
			{
				msg = (GxMessage*)event->msg.service_msg;
				GxMsgProperty_PlayerSpeedReport *spd;
					spd = GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerSpeedReport);
					if (strcmp((char*)(spd->player), PLAYER_FOR_NORMAL) == 0)
					{
						tms_speed_change(spd);
					}
			}
				return EVENT_TRANSFER_KEEPON;
#endif
			break;
		case GXMSG_PLAYER_RESOLUTION_REPORT:
			TOP_Msg_Dbg("[service_msg_to_app] GXMSG_PLAYER_RESOLUTION_REPORT\n");
			pResolution = (GxMsgProperty_PlayerResolutionReport*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerResolutionReport);
			if (NULL != pResolution)
				{
				    switch(pResolution->resolution)
						{
							#ifdef APP_HD
							case VIDEO_OUTPUT_HDMI_576I:
							case VIDEO_OUTPUT_HDMI_576P:
							case VIDEO_OUTPUT_HDMI_720P_50HZ:
							case VIDEO_OUTPUT_HDMI_1080I_50HZ:
							case VIDEO_OUTPUT_HDMI_1080P_50HZ:
							#endif
							#ifdef APP_SD
							case VIDEO_OUTPUT_PAL:
							case VIDEO_OUTPUT_NTSC_M:
							#endif
								app_play_update_flash_video_resolution(pResolution->resolution);
							
								break;
							default:
								break;	
						}
				}
			return EVENT_TRANSFER_STOP;				
#ifdef USB_FLAG
		case GXMSG_UPDATE_STATUS:
			break;
        	case GXMSG_HOTPLUG_IN:
			TOP_Msg_Dbg("[service_msg_to_app] GXMSG_HOTPLUG_IN\n");
            g_AppPvrOps.usbstate = USB_IN;
            para_usb = GXLED_USB;
//			app_panel_show(PANEL_STRING, &para_usb);
			
			if (TRUE == app_get_win_create_flag(USB_UPDATE_WIN))
				{
					/*
					* 当前已处于USB升级状态，插入U盘回退到系统升级界面
					*/					
					app_win_exist_to_win_widget("win_sys_update");					
				}
					
			if (TRUE == app_get_win_create_flag(MEDIA_CENTER_MENU_WIN))
				{
					/*
					* 当前已处于media播放状态，插入U盘回退到media主界面
					*/					
					app_win_exist_to_win_widget("win_main_menu");
				}
        
			GUI_EndDialog("win_pop_tip");
			GUI_CreateDialog("win_pop_tip");
			win_pop_tip_show_hotplug_gxmsg("USB Device Insert ");
			return EVENT_TRANSFER_STOP;
		case GXMSG_HOTPLUG_OUT:
			TOP_Msg_Dbg("[service_msg_to_app] GXMSG_HOTPLUG_OUT\n");
			
			if (TRUE == app_get_win_create_flag(USB_UPDATE_WIN))
			{
				/*
				* 当前已处于USB升级状态，插入U盘回退到系统升级界面
				*/
				app_win_exist_to_win_widget("win_sys_update");
			}
			
			if (TRUE == app_get_win_create_flag(MEDIA_CENTER_MENU_WIN))
			{
				/*
				* 当前已处于media播放状态，插入U盘回退到media主界面
				*/
				app_win_exist_to_win_widget("win_main_menu");
			}

            if(TRUE == app_get_win_create_flag(PVR_MEDIA_WIN))
            {//回退到上层
                app_win_exist_to_win_widget("win_sys_set");
            }
            GUI_EndDialog("win_prog_bar");//避免开机以后弹出USB插入信息提示框不能消失
			GUI_EndDialog("win_pop_tip");
			GUI_CreateDialog("win_pop_tip");
			win_pop_tip_show_hotplug_gxmsg("USB Device Remove");			
			TOP_Msg_Dbg("[SERVICE] GXMSG_HOTPLUG_OUT\n");
            para_usb = GXLED_SEAR;
			//app_panel_show(PANEL_STRING, &para_usb);
#if DVB_PVR_FLAG
            if(g_AppPvrOps.state != PVR_DUMMY)
			{
				printf("stop rec/tms \n");
                g_AppPvrOps.usbstate = USB_OUT;
                event->type = GUI_KEYDOWN;
                event->key.sym = KEY_YELLOW;
				GUI_CreateDialog("wnd_pvr_bar");
				GUI_SendEvent("wnd_pvr_bar",event);
			}

#endif
			return EVENT_TRANSFER_STOP;
#endif
		case GXMSG_EXTRA_SYNC_TIME_OK:
			TOP_Msg_Dbg("[service_msg_to_app] GXMSG_EXTRA_SYNC_TIME_OK\n");
			pTimeOk = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_ExtraTimeOk);
			if(pTimeOk->time_fountain == TDT_TIME)
			{
/*begin:modify by cth*/              
#ifdef CA_FLAG
                if(DVB_CA_TYPE_QZ == app_flash_get_config_dvb_ca_flag())
                {
                    //不去设置时间,CA模块中去设置
                }
                else
                {
                    sys_time.seconds = pTimeOk->utc_second;
    				sys_time.microsecs = 0;
    				GxCore_SetLocalTime(&sys_time);
                }
#else
				sys_time.seconds = pTimeOk->utc_second;
				sys_time.microsecs = 0;
				GxCore_SetLocalTime(&sys_time);
#endif
/*end:modify by cth*/
				app_win_update_current_time(0);
				TOP_Msg_Dbg("\nTDT_TIME_OK\n" );
			}
			return EVENT_TRANSFER_STOP;
		case GXMSG_FRONTEND_UNLOCKED:
			{
				TOP_Msg_Dbg("[service_msg_to_app] GXMSG_FRONTEND_UNLOCKED:\n");
//#ifdef ECOS_OS
//				app_play_set_msg_pop_type(MSG_POP_SIGNAL_BAD);
//				app_panel_show(PANEL_UNLOCK,0);

//#else
				//if(++count>2)
				//{
					app_play_set_msg_pop_type(MSG_POP_SIGNAL_BAD);
					app_panel_show(PANEL_UNLOCK,0);
				//	count = 0;
				//}
//#endif
				return EVENT_TRANSFER_STOP;
			}
		case GXMSG_FRONTEND_LOCKED:
			//count=0;
			TOP_Msg_Dbg("[service_msg_to_app] GXMSG_FRONTEND_LOCKED\n");
			app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);
			app_panel_show(PANEL_LOCK,0);
			return EVENT_TRANSFER_STOP;
		case GXMSG_BOOK_TRIGGER:
			{
				GxMsgProperty_BookTrigger* para = NULL;
				para = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_BookTrigger);
				TOP_Msg_Dbg("[service_msg_to_app] GXMSG_BOOK_TRIGGER\n");
				app_book_trigger_start(para);
				if ((NULL != para)&&(para->book_type == BOOK_POWER_OFF))
					{
						int32_t times;
						book_play book1;
					//	lowpower_info lowpower = {0};
						memcpy(&book1, para->struct_buf, sizeof(book_play));
						times = book1.end_time - book1.start_time;
						if (times <=0)
							times = 0;
					//	lowpower.WakeTime = times;
					//	lowpower.GpioMask = 0xffffffff;
					//	lowpower.GpioData = 0xffffffff;
					//	lowpower.key = (PANEL_KEY_POWER&0xff00)>>8;
						app_lower_power_entry((PANEL_KEY_POWER&0xff00)>>8,32,0);													
					}
				return EVENT_TRANSFER_STOP;
			}
		case GXMSG_BOOK_FINISH:
			{
				GxMsgProperty_BookFinish* para = NULL;
				para = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_BookFinish);
				app_book_trigger_end(para);
				TOP_Msg_Dbg("[service_msg_to_app] GXMSG_BOOK_FINISH\n");
				return EVENT_TRANSFER_STOP;
			}
		case GXMSG_CA_ON_EVENT:
			{
				popmsg_ret ret;
				GxMsgProperty0_OnEvent* event;
				event = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty0_OnEvent);
				TOP_Msg_Dbg("[service_msg_to_app] GXMSG_CA_ON_EVENT,key:%d\n",event->key);
				switch(event->key)
					{
#ifdef DVB_AD_TYPE_BY_FLAG
						case DVB_AD_SCROLL_BMP:
						case DVB_AD_BMP:
						case DVB_AD_GIF:
							if(FULL_SCREEN_WIN == app_win_get_focus_video_window()) 
							{
								app_by_ads_gxmsg_ca_on_event_exec(msg);
							}						
							break;
#endif
						case DVB_CA_OTA:
							/*是否升级提示*/
							TOP_Msg_Dbg("有新的版本,需要升级\n");
							ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"有新的版本,是否升级?",POPMSG_TYPE_YESNO);
							if(ret == POPMSG_RET_YES)
							{
								GxCore_ThreadDelay(500);
								restart();
							}
							else
							{
								
							}
							return EVENT_TRANSFER_STOP;
						case DVB_CA_NIT:
							{
								if (0 == app_get_nit_change_for_search_status())
									{
										app_set_nit_change_for_search_status(1);
										TOP_Msg_Dbg("节目更新，是否搜节目?\n");									
									}
								break;
							}
						#if 0
						ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Nit change, Research?",POPMSG_TYPE_YESNO);
						if(ret == POPMSG_RET_YES)
						{								
							app_win_exist_to_full_screen();
							app_prog_delete_all_prog();
							app_search_set_auto_flag(TRUE);
						}
						else
						{
							
						}
						break;

						#endif
							
#if defined(DVB_CA_TYPE_QILIAN_FLAG)
						case DVB_CA_QILIAN_POP_MSG:
							
							if(FULL_SCREEN_WIN == app_win_get_focus_video_window()) 
							{
								extern char buf_osd_msg0[];
								ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf_osd_msg0,POPMSG_TYPE_OK);
								if(ret == POPMSG_RET_YES)
								{								
									//app_win_exist_to_full_screen();
									//app_prog_delete_all_prog();
									//app_search_set_auto_flag(TRUE);
								}
							}
							break;
#endif
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
						case DVB_CA_CONDITION_SEARCH:
							app_win_exist_to_full_screen();
							app_prog_delete_all_prog();
							app_search_set_auto_flag(TRUE);
							break;
#endif
						default:
							app_cas_api_gxmsg_ca_on_event_exec(msg);	
							break;
					}
				return EVENT_TRANSFER_STOP;
			}
		default:
			break;
	}

	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int top(const char* widgetname, void *usrdata)
{


	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

   //TOP_Msg_Dbg("[top] event->type=0x%x\n",event->type);	

	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_SERVICE_MSG:
            //TOP_Msg_Dbg("[top] type=0x%x\n",GUI_SERVICE_MSG);
			return service_msg_to_app(usrdata);

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_POWER:
					/* gpio un mute high*/
//					app_panel_set_gpio_high(37); // mute dvbc gx1001
//					app_panel_set_gpio_high(53); // mute dtmb gx1503
					app_panel_show(PANEL_POWER_OFF,0);
					GxCore_ThreadDelay(200);
 					// gx3113c port32 low for lowpower
					//app_lower_power_entry(event->key.scancode,32,0);
					app_lower_power_entry(PANEL_KEY_POWER,32,0);
					printf("gx_lowpower_entry\n");
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	return EVENT_TRANSFER_KEEPON;
}

#include "app.h"
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
#endif


#define FULLSCREEN_WIN                  "win_full_screen"

#define PROGBAR_WIN                     "win_prog_bar"
#define PROGBAR_IMAGE_TVRADIO           "win_prog_bar_block1_image"
#define PROGBAR_IMAGE_PROG_NAME         "win_prog_bar_prog_name_back"
#define PROGBAR_TEXT_PROG_NAME          "win_prog_bar_prog_name"
#define PROGBAR_TEXT_TIME               "win_prog_bar_time_text"
#define PROGBAR_TEXT_PROG_EVENT1        "win_prog_bar_block2_text1"
#define PROGBAR_TEXT_PROG_EVENT2        "win_prog_bar_block2_text2"

#define PROGBAR_BMP_TV                  "programinfobar_tv.bmp"
#define PROGBAR_BMP_RADIO               "programinfobar_radio.bmp"


static event_list* timer_progbar = NULL;
static event_list* timer_progpfbar = NULL;
static uint32_t pos = 0;


static void show_prog_adapt_info()
{
#ifdef APP_SD
	if(app_flash_get_config_video_auto_adapt())
	{
		if(VIDEO_OUTPUT_PAL == app_flash_get_config_video_hdmi_mode())
		{
			GUI_SetProperty("win_prog_bar_adapt", "string", "PAL"); 
		}
		else if(VIDEO_OUTPUT_NTSC_M == app_flash_get_config_video_hdmi_mode())
		{
			GUI_SetProperty("win_prog_bar_adapt", "string", "NTSC");
		}
	}
#endif
}
static int timer_prog_bar(void *userdata)
{
	timer_progbar = NULL;

#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_hide_epg_pic();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_epg_list_ad_image");
#endif
	GUI_EndDialog(PROGBAR_WIN);

	return 0;
}

static int app_prog_bar_refresh_prog_info()
{
	char buffer[41] = {0};
	Lcn_State_t lcn_flag = 0;
	GxBusPmDataProg prog={0};

	if(0 == app_prog_get_num_in_group())
	{
		return 0;
	}
	else
	{
		//2015-03-04,qm, init the pos
		app_prog_get_playing_pos_in_group(&pos);
	}

	GxBus_PmProgGetByPos(pos,1,&prog);
	lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_OFF == lcn_flag)
	{
		snprintf((void*)buffer, 40, "%03d %s", pos + 1, prog.prog_name);
	}
	else
	{
		snprintf((void*)buffer, 40, "%03d %s", prog.pos, prog.prog_name);
	}
#ifdef APP_SD
	if(TRUE == prog.scramble_flag)
	{
		GUI_SetProperty("win_prog_bar_image_money","state","show");
	}
	else
	{
		GUI_SetProperty("win_prog_bar_image_money","state","hide");
	}
#endif
	//	printf("\n%s\n",buffer);
	GUI_SetProperty(PROGBAR_TEXT_PROG_NAME, "string", buffer);
	GUI_SetProperty("win_prog_bar_block2_text1", "string", " ");
	GUI_SetProperty("win_prog_bar_block2_text2", "string", " ");
	return 0;

}

static int refresh_prog_bar(void *userdata)
{
	GxEpgInfo *epg_info;
	struct tm tm_start;
	struct tm tm_end;

	static uint8_t pArry1[80] = {0};

	// TODO: chage to HH:MM
	//runde
	//GUI_SetProperty(PROGBAR_TEXT_TIME, "string", app_win_get_local_time_string());
#ifdef DVB_AD_TYPE_DS_FLAG
	//app_ds_ad_hide_epg_pic();
	//app_ds_ad_epg_pic_display("win_prog_bar_ad_image");
#endif

	app_epg_free();
	if(0 == app_prog_get_num_in_group())
	{
		return 0;
	}

	epg_info = app_epg_get_pf_event(pos);
	if (NULL == epg_info)
	{
		/*
		 * 获取p/f信息失败
		 */
		GUI_SetProperty("win_prog_bar_block2_text1", "string", "No Present Event Info ");
		GUI_SetProperty("win_prog_bar_block2_text2", "string", "No Follow Event Info ");
		return 0;
	}

	if(epg_info->start_time == 0 && epg_info->duration == 0)
	{
		GUI_SetProperty("win_prog_bar_block2_text1", "string", "No Present Event Info ");
	}
	else
	{
		if(strlen((char*)(epg_info->event_name))>60)
		{
			epg_info->event_name[60]='\0';
		}
		app_epg_get_event_time(epg_info,&tm_start,&tm_end);
		sprintf((void*)pArry1,"%02d:%02d~%02d:%02d ",tm_start.tm_hour, tm_start.tm_min, tm_end.tm_hour, tm_end.tm_min);
		strcat((void*)pArry1,(char*)(epg_info->event_name));
		GUI_SetProperty("win_prog_bar_block2_text1", "string", (void*)pArry1);

	}

	epg_info++;
	if(epg_info->start_time == 0 && epg_info->duration == 0)
	{
		GUI_SetProperty("win_prog_bar_block2_text2", "string", "No Follow Event Info ");
	}
	else
	{
		if(strlen((char*)(epg_info->event_name))>60)
		{
			epg_info->event_name[60]='\0'; 
		}
		app_epg_get_event_time(epg_info,&tm_start,&tm_end);
		sprintf((void*)pArry1,"%02d:%02d~%02d:%02d ",tm_start.tm_hour, tm_start.tm_min, tm_end.tm_hour, tm_end.tm_min);
		strcat((void*)pArry1,(char*)(epg_info->event_name));
		GUI_SetProperty("win_prog_bar_block2_text2", "string", (void*)pArry1);
	}		

	return 0;
}

static status_t key_exit(void)
{

	return 0;
}

static status_t key_updown(unsigned short value)
{
	uint32_t pos1 = pos;
#ifdef _DEBUG
	test_for_show_time_starttime();
#endif
	switch(value)
	{
		case KEY_UP:
			pos = app_play_by_direction(-1);	
			break;
		case KEY_DOWN:
			pos = app_play_by_direction(1);
			break;
	}
	if (pos1 == pos)
	{	
		return GXCORE_SUCCESS;
	}

#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_hide_epg_pic();
	app_ds_ad_epg_pic_display("win_prog_bar_ad_image");
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_epg_list_ad_image");
	app_mad_ad_show_pic("win_prog_bar_ad_image",M_AD_PLACE_ID_PROG_BAR);
#endif
	//app_play_switch_prog_clear_msg();

	if (NULL == timer_progpfbar)
		timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
	else
		reset_timer(timer_progpfbar);

	app_play_reset_play_timer(PLAY_TIMER_DURATION);
	app_prog_bar_refresh_prog_info();
	// show_prog_adapt_info();
	if(timer_progbar)
	{	
		reset_timer(timer_progbar);
	}
#ifdef DVB_AD_TYPE_DS_FLAG
	//app_ds_ad_show_pic(FREE_PIC_DATA,"win_prog_bar_epg_ad_gif",DSAD_STB_EPG);
	//app_ds_ad_show_pic(LOAD_PIC_DATA,"win_prog_bar_epg_ad_gif",DSAD_STB_EPG);
#endif

	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int prog_bar_service(const char* widgetname, void *usrdata)
{

	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int prog_bar_create(const char* widgetname, void *usrdata)
{
	uint32_t duration;
	int32_t config = 0;
	config = app_flash_get_config_bar_time();
	duration = config * 1000;


	//    app_epg_sync_time();
	if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
	{
		//        GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
	}
	else
	{
		//      GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
	}
	//app_prog_get_playing_pos_in_group(&pos);
	app_prog_bar_refresh_prog_info();
	//       refresh_prog_bar(NULL);

	timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
	timer_progbar = create_timer(timer_prog_bar, duration, NULL,  TIMER_ONCE);

	app_set_win_create_flag(PROGRAM_BAR_WIN);

#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_hide_epg_pic();
	app_ds_ad_epg_pic_display("win_prog_bar_ad_image");
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_epg_list_ad_image");
	app_mad_ad_hide_pic("win_full_screen_ad");
	app_mad_ad_show_pic("win_prog_bar_ad_image",M_AD_PLACE_ID_PROG_BAR);
#endif
	return 0;
}

SIGNAL_HANDLER int prog_bar_destroy(const char* widgetname, void *usrdata)
{
	//    app_play_remove_play_tmer();
	printf("prog_bar_destroy\n");
	remove_timer(timer_progbar);
	timer_progbar = NULL;

	remove_timer(timer_progpfbar);
	timer_progpfbar = NULL;

	app_epg_free();
	app_set_win_destroy_flag(PROGRAM_BAR_WIN);
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_prog_bar_ad_image");
#endif
	return 0;
}


SIGNAL_HANDLER int prog_bar_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

#ifdef DVB_AD_TYPE_DS_FLAG
	//	app_ds_ad_hide_full_screen_pic();
	//2015-03-10
	//app_ds_ad_hide_osd();
	app_ds_ad_hide_rolling_osd();
	//	app_ds_ad_hide_unauthorized_pic();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_osd(M_AD_PLACE_ID_SUBTITLE);
#endif

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case KEY_EXIT:
			key_exit();
			app_play_check_play_timer();
			GUI_EndDialog(PROGBAR_WIN);
			return EVENT_TRANSFER_STOP;

		case KEY_UP:
		case KEY_DOWN:
			if (1<app_prog_get_num_in_group())
			{
				key_updown(event->key.sym);			
			}
			break;
			/*
			 * 全屏响应按键，全屏不响应按键不发送
			 *
			 */
		case KEY_TV:
			if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
			{
				if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0))
				{
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);
				}
				else
				{
					app_play_switch_tv_radio();
					app_play_switch_prog_clear_msg();	/*创建信息条、密码框之前，清除上一节目提示，避免闪烁*/	
					app_play_reset_play_timer(PLAY_TIMER_DURATION);
					app_prog_get_playing_pos_in_group(&pos);

					if (NULL == timer_progpfbar)
						timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
					else
						reset_timer(timer_progpfbar);

					app_prog_bar_refresh_prog_info();
					if(timer_progbar)
					{	
						reset_timer(timer_progbar);
					}

					if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
					{
						//     GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
					}
					else
					{
						//    GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
					}

				}
			}
			break;
		case KEY_RADIO:
			if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
			{
				if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0))
				{
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);

				}
				else
				{
					app_play_switch_tv_radio();
					app_play_switch_prog_clear_msg();	
					app_play_reset_play_timer(PLAY_TIMER_DURATION);
					app_prog_get_playing_pos_in_group(&pos);



					if (NULL == timer_progpfbar)
						timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
					else
						reset_timer(timer_progpfbar);
					app_prog_bar_refresh_prog_info();
					if(timer_progbar)
					{	
						reset_timer(timer_progbar);
					}	

					if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
					{
						//    GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
					}
					else
					{
						//   GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
					}						

				}
			}

			break;
		case KEY_TV_RADIO:
			if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
			{
				if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0))
				{
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);
				}
				else
				{
					if(0 != app_play_switch_tv_radio())
						break;
					app_play_switch_prog_clear_msg();	/*创建信息条、密码框之前，清除上一节目提示，避免闪烁*/	
					app_play_reset_play_timer(PLAY_TIMER_DURATION);
					app_prog_get_playing_pos_in_group(&pos);

					if (NULL == timer_progpfbar)
						timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
					else
						reset_timer(timer_progpfbar);

					app_prog_bar_refresh_prog_info();
					if(timer_progbar)
					{	
						reset_timer(timer_progbar);
					}

					if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
					{
						// GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
					}
					else
					{
						//						        GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
					}

				}
			}
			else
				if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
				{
					if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0))
					{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);
					}
					else
					{
						if(0 != app_play_switch_tv_radio())
							break;
						app_play_switch_prog_clear_msg();	
						app_play_reset_play_timer(PLAY_TIMER_DURATION);
						app_prog_get_playing_pos_in_group(&pos);



						if (NULL == timer_progpfbar)
							timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
						else
							reset_timer(timer_progpfbar);
						app_prog_bar_refresh_prog_info();
						if(timer_progbar)
						{	
							reset_timer(timer_progbar);
						}	

						if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
							//  GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
						}
						else
						{
							//				        GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
						}						

					}
				}
			break;
		case KEY_RECALL:
			if (FALSE == app_play_recall_prog())
				break;
			app_play_switch_prog_clear_msg(); /*创建信息条、密码框之前，清除上一节目提示，避免闪烁*/

			if(app_prog_get_num_in_group() >0)
			{
				app_play_reset_play_timer(PLAY_TIMER_DURATION);
				app_prog_get_playing_pos_in_group(&pos);

				if (NULL == timer_progpfbar)
					timer_progpfbar = create_timer(refresh_prog_bar, 300, NULL,  TIMER_REPEAT);
				else
					reset_timer(timer_progpfbar);

				app_prog_bar_refresh_prog_info();
				if(timer_progbar)
				{	
					reset_timer(timer_progbar);
				}

				if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
				{
					//   GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_TV);
				}
				else
				{
					//			        GUI_SetProperty(PROGBAR_IMAGE_TVRADIO, "img", PROGBAR_BMP_RADIO);
				}
			}			
			break;
		case KEY_LEFT:
		case KEY_RIGHT:
			//		case KEY_VOLUME_DOWN_1:
			//		case KEY_VOLUME_UP_1:
			//		case KEY_RECALL:
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
		case KEY_MUTE:
		case KEY_MENU:
		case KEY_FAV:
		case KEY_EPG:
		case KEY_RED:
		case KEY_YELLOW:
		case KEY_GREEN:
			//		case KEY_PROG_INFO:
			//		 case KEY_TV_RADIO:
			app_play_check_play_timer();
			GUI_EndDialog(PROGBAR_WIN);
			GUI_SendEvent(FULLSCREEN_WIN, event);
			return EVENT_TRANSFER_STOP;   
		default:
			break;
	}

	return EVENT_TRANSFER_STOP;
}



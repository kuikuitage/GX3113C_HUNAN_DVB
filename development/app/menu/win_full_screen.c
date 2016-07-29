/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_full_screen.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	Ä£¿éÍ·ÎÄŒþ
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2010.04.12		  lijq 	 			creation
*****************************************************************************/

#include "app.h"
#include "win_main_menu.h"
#include "app_common_panel.h"

#ifdef DVB_AD_TYPE_DVB_FLAG
#include "app_dvb_cas_api_email.h"
#endif

#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif

#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
extern event_list* mad_timer_hide_corner;
extern event_list* mad_timer_show_corner;
extern int mad_corner_is_showing;
extern int mad_corner_need_show ;
extern int mad_timer_hide_corner_timer(void* data);
extern int mad_timer_show_corner_timer(void* data);

#endif

#ifdef DVB_CA_TYPE_MG_FLAG
extern int mg_curprogram_is_ppv(void);
extern void app_mg_cas_rolling_message_hide(void);
#endif

#ifdef DVB_CA_TYPE_MG312_FLAG
extern void app_mg_cas312_rolling_message_hide(void);
extern int mg312_curprogram_is_ppv(void);
#endif

#ifdef DVB_CA_TYPE_TR_FLAG
extern void app_tr_cas_rolling_message_hide(void);
extern void win_trcas_ipp_indicate_clear(void);
#endif

#if DVB_PVR_FLAG
#include "app_pvr.h"
EventState gEventState = {STATE_OFF};
extern void full_pause_exec(void);
extern  void full_pvr_exec(void);
#endif

#if MEDIA_SUBTITLE_SUPPORT
#include "app_ttx_subt.h"
extern TtxSubtOps g_AppTtxSubt;
extern void app_subt_pause(void);
extern void app_ttx_magz_open(void);
#endif

static event_list* spAppFullScreentime = NULL;
#define TIMER_DURATION (200)

#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "app_cd_cas_3.0_api_lock_service.h"
App_CaCurtainNotify cdcasCurtainInfo = {0};

void app_cd_cas_30_curtain_exec(void)
{
	if(cdcasCurtainInfo.wStatus == 1)
	{
		if(cdcasCurtainInfo.wShowStatus == 0)
		{
			if(cdcasCurtainInfo.str != NULL)
			{
				GUI_SetProperty("win_full_screen_text_curtain","string",cdcasCurtainInfo.str);
			}
			GUI_SetProperty("win_full_screen_image_curtain","state","show");
			GUI_SetProperty("win_full_screen_text_curtain","state","show");
			cdcasCurtainInfo.wShowStatus = 1;
		}
		else
		{
		}
	}
	else
	{
		if(cdcasCurtainInfo.wShowStatus == 1)
		{
			GUI_SetProperty("win_full_screen_image_curtain","state","hide");
			GUI_SetProperty("win_full_screen_text_curtain","state","hide");
			cdcasCurtainInfo.wShowStatus = 0;
		}
		else
		{
		}
	}
}
#endif

/*wangjian add on 20141219*/
static void fullscreen_hide_osd_scroll(void)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef DVB_CA_TYPE_MG_FLAG
	app_mg_cas_rolling_message_hide();
#endif

#ifdef DVB_CA_TYPE_MG312_FLAG
	app_mg_cas312_rolling_message_hide();
#endif
	if (dvb_ca_flag == DVB_CA_TYPE_TR  )			
	{
#ifdef DVB_CA_TYPE_TR_FLAG
		app_tr_cas_rolling_message_hide();
#endif
	}
#ifdef DVB_CA_TYPE_QZ_FLAG
	app_qz_cas_rolling_message_hide();
#endif

#ifdef DVB_CA_TYPE_BY_FLAG
	app_by_cas_rolling_message_hide();
#endif

#ifdef DVB_CA_TYPE_DVB_FLAG
	app_dvb_cas_rolling_message_hide();
	app_dvb_cas_email_flag_hide();
#endif
	return;
}

//全屏定时器任务
static  int timer(void *userdata)
{
    #if (defined DVB_CA_TYPE_DS_CAS50_FLAG)||(defined DVB_AD_TYPE_DS_FLAG)
	char* focus_Window = NULL;
    #endif
//	int32_t Config;

#ifdef DVB_CA_TYPE_TR_FLAG
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag == DVB_CA_TYPE_TR  )			
	{
		win_trcas_ipp_indicate_clear();
	}
#endif
	/*开机无节目自动搜索*/
	if (TRUE == app_search_get_auto_flag())
	{
		if (0 == app_prog_get_num_in_group())
		{
			startup_search_para startsearchpara = {0};
#ifdef CUST_TAIKANG		
			startsearchpara.nit_flag = 1;
#else
			startsearchpara.nit_flag = 0; 
#endif
			startsearchpara.fre_low = FRE_BEGIN_LOW_FULL;
			startsearchpara.fre_high = FRE_BEGIN_HIGH_FULL;
			fullscreen_hide_osd_scroll();
			GUI_SetProperty("win_full_screen_img_mute","state","hide");
			app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
			GUI_CreateDialog("win_search_result");	
			GUI_SetInterface("flush", NULL);
			app_search_startup_auto_scan(startsearchpara);
		}

		app_search_set_auto_flag(FALSE);
		return 0;
	}

	app_search_set_auto_flag(FALSE);

	/*静音状态下，显示静音图标*/
	if (app_prog_get_num_in_group() > 0)
	{
		if ((TRUE == app_play_get_mute_flag())
				&& app_win_get_focus_video_window() == FULL_SCREEN_WIN)
		{
			if (TRUE != app_get_win_create_flag(PROG_NUM_WIN))
			{
				GUI_SetProperty("win_full_screen_img_mute","state","show");
			}
		}
		else if (app_win_get_focus_video_window() == FULL_SCREEN_WIN)
		{
			GUI_SetProperty("win_full_screen_img_mute","state","hide");				
		}
	}

	/*预约播放*/
	if (TRUE == app_book_query_play())
	{
		app_play_video_window_full();
	}
	/*提示信息查询、显示*/
	app_win_prompt_msg_exec_call_back();

	/*提示是否有节目更新*/
	app_notice_nit_change_for_search_show();

	/*
	 * 增加升级检测与提示框
	 */

	/*CA滚动消息、指纹、邮件提醒图标等显示*/
#ifdef CA_FLAG
	app_cas_api_osd_exec();	

#ifdef	DVB_CA_TYPE_CD_CAS30_FLAG
	app_cd_cas_30_curtain_exec();
#endif
#endif

	/*Ë¢ÐÂ¡¢ÉèÖÃµ±Ç°Ž°ÌåÀàÐÍ*/
	app_win_update_video_focus_window();

	/*Ë¢ÐÂµ±Ç°œçÃæÏµÍ³Ê±ŒäÏÔÊŸ*/
	app_win_update_current_time(TIMER_DURATION);

#ifdef DVB_AD_TYPE_BY_FLAG
	if (FULL_SCREEN_WIN != app_win_get_focus_video_window())
	{
		AdverPsiStop();		
	}
#endif

#ifdef DVB_AD_TYPE_DS_FLAG
	focus_Window = (char*)GUI_GetFocusWindow();
	/*Show or hide full screen ad*/
	if (0 == strcasecmp("win_full_screen", focus_Window))
	{
		app_ds_ad_full_screen_pic_display("win_full_screen_ad");
		app_ds_ad_osd_display("win_full_screen_text_roll_top","win_full_screen_text_roll_bottom");
	}
	else
	{
	}
	/*Show or hide unauthorized ad*/
	if(  app_ds_ad_unauthorized_pic_is_ready() == 1)
	{
	    if( 0 == strcasecmp("win_full_screen", focus_Window))
        {
    		GUI_EndDialog("win_desai_cas50_ippv_buy");
    		app_ds_ad_unauthorized_pic_display("win_full_screen_unauthorized_ad");
        }
	}
	else
	{
		app_ds_ad_hide_unauthorized_pic();
	}
#endif

#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
    focus_Window = (char*)GUI_GetFocusWindow();
	if( 0 == strcasecmp("win_desai_cas50_ippv_buy", focus_Window))
		app_desai_cas50_rolling_message_hide();
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
	char* focus_win = (char*)GUI_GetFocusWindow();
	static int mad_corner_exit_flag = 0;

	if(0 == strcasecmp("win_full_screen",focus_win))
	{
		app_mad_ad_show_osd();
		if((mad_corner_is_showing == 0) && (mad_corner_need_show == 1))
		{
			app_mad_ad_show_pic("win_full_screen_ad",M_AD_PLACE_ID_CORNER);
		}
		else
		{
			if(mad_timer_hide_corner == NULL)
			{
				printf("full screen mad_timer_hide_corner created!!!\n");
				mad_timer_hide_corner = create_timer(mad_timer_hide_corner_timer, 5000, 0, TIMER_ONCE);
			}
			else
			{
				printf("full screen mad_timer_hide_corner has been created!!!\n");
			}
		}
	}
	else
	{
		app_mad_ad_hide_pic("win_full_screen_ad");
		mad_corner_is_showing = 0;
		mad_corner_need_show = 1;
	}
#endif

	return 0;
}

/*Í£Ö¹È«ÆÁ¶šÊ±Æ÷œÓ¿Ú£¬
  ËÑË÷œÚÄ¿¡¢ÉýŒ¶¡¢mediaµÈÌØÊâ²Ù×÷ÓŠÍ£Ö¹timerÔËÐÐ*/
int32_t app_full_screen_timer_stop(void)
{
	if (NULL != spAppFullScreentime)	
	{
		timer_stop(spAppFullScreentime);
	}

	return 0;
}

/*È«ÆÁ¶šÊ±Æ÷žŽÎ»œÓ¿Ú*/
int32_t app_full_screen_timer_reset(void)
{
	if (NULL == spAppFullScreentime)
	{
		spAppFullScreentime = create_timer(timer, TIMER_DURATION, NULL, TIMER_REPEAT);
	}
	else
	{
		reset_timer(spAppFullScreentime);
	}

	return 0;
}

//È«ÆÁ²¥·Å
SIGNAL_HANDLER  int app_full_screen_create(const char* widgetname, void *usrdata)
{
	startup_play_para playpara = {0};

	/*
	 * ÉèÖÃ¿ª»úµÚÒ»ŽÎ²¥·Å²ÎÊý
	 * ¿ÉÑ¡Ôñ²¥·ÅÖž¶šœÚÄ¿¡¢²¥·Å¶ÏµçŒÇÒäœÚÄ¿
	 *
	 */
	playpara.startupProgFlag = FALSE;
	playpara.startupProgNum = 0;
	if (TRUE == app_play_startup_prog(playpara))
	{
		GUI_CreateDialog("win_prog_bar");
		app_play_reset_play_timer(0);
	}

	/*ÏÔÊŸ¡¢Òþ²ØŸ²ÒôÍŒ±ê*/
	if (TRUE == app_play_get_mute_flag())
	{
		GUI_SetProperty("win_full_screen_img_mute","state","show");			
	}
	else
	{
		GUI_SetProperty("win_full_screen_img_mute","state","hide");			
	}


	if (NULL == spAppFullScreentime)
	{
		spAppFullScreentime = create_timer(timer, TIMER_DURATION, NULL,  TIMER_REPEAT);
	}
	else
	{
		reset_timer(spAppFullScreentime);
	}

	app_win_set_focus_video_window(FULL_SCREEN_WIN);

#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_full_screen_pic_display("win_full_screen_ad");
#endif

#ifdef DVB_CA_TYPE_QILIAN_FLAG
	STBCA_CHECK_MailBox();
#endif

#ifdef DVB_CA_TYPE_DVB_FLAG
	app_dvb_cas_api_init_email_data();
#endif
	return 0;
}

SIGNAL_HANDLER  int full_screen_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

	switch(event->type)
	{
		case GUI_SERVICE_MSG:
			//full_screen_service_msg(event->msg.service_msg);
			break;
		default:
			break;
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_full_screen_destroy(const char* widgetname, void *usrdata)
{
	remove_timer(spAppFullScreentime);
	spAppFullScreentime = NULL;

#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_hide_full_screen_pic();
	//	app_ds_ad_hide_osd();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_full_screen_ad");
	remove_timer(mad_timer_show_corner);
	remove_timer(mad_timer_hide_corner);
	mad_timer_show_corner = NULL;
	mad_timer_hide_corner = NULL;
	mad_corner_is_showing = 0;
	mad_corner_need_show = 1;
#endif
	printf("win_full_screen_destroy+++++++++++++++++++++++\n");
	return 0;
}

SIGNAL_HANDLER  int app_full_screen_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int32_t Config;

	if(event->type == GUI_KEYDOWN)
	{
#ifdef CA_FLAG
	/*
	* ÅÐ¶ÏÊÇ·ñÓŠŒ±¹ã²¥×ŽÌ¬£¬ÈçÓŠŒ±¹ã²¥×ŽÌ¬£¬²»ÔÊÐíÇÐÌš
	*/
	if (TRUE == app_cas_api_get_lock_status())
	{
		return EVENT_TRANSFER_STOP;
	}

	#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	if (TRUE == app_desai_cas50_get_card_update_status())
	{
		return EVENT_TRANSFER_STOP;
	}
	#endif /* DVB_CA_TYPE_DS_CAS50_FLAG */
#endif

		if (0 == app_prog_get_num_in_group())
		{
			/*ÎÞœÚÄ¿×ŽÌ¬ÏÂ£¬×Ô¶¯ËÑË÷±êÖŸŽæÔÚ£¬²»ÔÊÐíÒ£¿ØÆ÷°ŽŒü²Ù×÷*/
			if (TRUE == app_search_get_auto_flag())
			{
				return EVENT_TRANSFER_STOP;
			}
		}

#if MEDIA_SUBTITLE_SUPPORT
		if (g_AppTtxSubt.ttx_magz_opt(&g_AppTtxSubt, 
					find_virtualkey(event->key.sym)) == GXCORE_SUCCESS)
		{
#define IMG_STATE       "img_full_state"
#define IMG_REC         "img_full_rec"
#define IMG_MUTE    "img_full_mute"
#define IMG_PAUSE    "img_full_pause"

			if(event->key.sym == STBK_EXIT)
			{
				GUI_SetProperty(IMG_STATE, "img", "s_icon_ts.bmp");
				GUI_SetProperty(IMG_REC, "img", "s_pvr_rec.bmp");
				GUI_SetProperty(IMG_MUTE, "img", "s_mute.bmp");
				GUI_SetProperty(IMG_PAUSE, "img", "s_pause.bmp");
			}  
			return EVENT_TRANSFER_STOP;
		}
#endif

#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		if (event->key.sym != KEY_DOWN && event->key.sym != KEY_UP)
			app_desai_cas50_rolling_message_hide();
#endif

#ifdef DVB_AD_TYPE_DS_FLAG
		app_ds_ad_hide_full_screen_pic();
        //2015-03-10
		//app_ds_ad_hide_osd();
		app_ds_ad_hide_rolling_osd();
        app_ds_ad_hide_unauthorized_pic();
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_osd(M_AD_PLACE_ID_SUBTITLE);
	app_mad_ad_hide_pic("win_full_screen_ad");
#endif
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
		app_gy_cas_rolling_message_hide();
#endif
		printf("event key =0x%x\n",event->key.scancode);
		switch (event->key.sym)
		{

		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_VOLUME_DOWN_1:
		case KEY_VOLUME_UP_1:
			if(app_prog_get_num_in_group() >0)
			{
                int32_t volume = 0;
                volume = app_flash_get_config_audio_volume();
				/*È¡ÏûŸ²Òô*/
				if (TRUE == app_play_get_mute_flag())
				{
                    if((event->key.sym==KEY_LEFT && volume>0)
                        ||(event->key.sym==KEY_VOLUME_DOWN_1 && volume>0)
                        ||(event->key.sym!=KEY_LEFT && event->key.sym!=KEY_VOLUME_DOWN_1))
                    {
    					Config = 0;
    					app_flash_save_config_mute_flag(Config);
    					app_play_set_mute(Config);
    					GUI_SetProperty("win_full_screen_img_mute","state","hide");
                        GUI_SetInterface("flush",NULL);
                    }

					}

					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");

					GUI_CreateDialog("win_menu_volume");
					GUI_SendEvent("win_menu_volume", event);
				}            
				break;

			case KEY_UP:
			case KEY_DOWN:
				if(app_prog_get_num_in_group() >1)
				{
#ifdef	DVB_CA_TYPE_CD_CAS30_FLAG
					extern void cd_cas30_finger_show(uint8_t number,uint8_t showFlag,char* message);
					cd_cas30_finger_show(0,DVB_CA_FINGER_FLAG_HIDE,(char*)NULL);
#endif
#ifdef DVB_CA_TYPE_TR_FLAG
				GUI_EndDialog("win_tr_cas_parent_lock");
				GUI_EndDialog("win_tr_cas_ipp_buy");				
#endif
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");

					GUI_CreateDialog("win_prog_bar");
					GUI_SendEvent("win_prog_bar", event); 
				}
				break;

			case KEY_RECALL:
				if (FALSE == app_play_recall_prog())
				{
					break;
				}

				/*ŽŽœšÐÅÏ¢Ìõ¡¢ÃÜÂë¿òÖ®Ç°£¬Çå³ýÉÏÒ»œÚÄ¿ÌáÊŸ£¬±ÜÃâÉÁËž*/
				app_play_switch_prog_clear_msg(); 
				GUI_EndDialog("win_menu_volume");
				GUI_EndDialog("win_prog_bar");
				GUI_EndDialog("win_prog_num");

				if (app_prog_get_num_in_group() > 0)
				{
					GUI_CreateDialog("win_prog_bar");
					app_play_reset_play_timer(PLAY_TIMER_DURATION);			 				
				}

				break;

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
				if (TRUE == app_play_get_mute_flag())
				{
					GUI_SetProperty("win_full_screen_img_mute","state","hide");			
				}

				app_play_remove_play_tmer();
#ifdef DVB_CA_TYPE_TR_FLAG
			GUI_EndDialog("win_tr_cas_parent_lock");
			GUI_EndDialog("win_tr_cas_ipp_buy");				
#endif
				GUI_EndDialog("win_menu_volume");
				GUI_EndDialog("win_prog_bar");
				GUI_EndDialog("win_prog_num");

				GUI_CreateDialog("win_prog_num");
				GUI_SendEvent("win_prog_num", event); 
				break;

			case KEY_MUTE:
				if (app_prog_get_num_in_group() > 0)
				{
					Config = app_flash_get_config_mute_flag();

					if (FALSE == app_play_get_mute_flag() 
							|| (0 == app_flash_get_config_audio_volume()))
					{
						Config = 1;
						GUI_SetProperty("win_full_screen_img_mute","state","show");
					}
					else
					{
						/*bug id 13115*/
						Config = 0;
						GUI_SetProperty("win_full_screen_img_mute","state","osd_trans_hide");	
					}

					app_flash_save_config_mute_flag(Config);
					app_play_set_mute(Config);
				}
				break;

			case KEY_MENU:
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
				{
					extern void app_cd_cas30_hide_rolling_message(void);
					app_cd_cas30_hide_rolling_message();
				}
#endif
				fullscreen_hide_osd_scroll();

#if DVB_PVR_FLAG
				if (1 == app_pvr_state_check(0, 0))
				{
					break;
				}
#endif	
#ifdef DVB_AD_TYPE_BY_FLAG
				AdverPsiStop();
#endif
#ifdef DVB_CA_TYPE_TR_FLAG
			GUI_EndDialog("win_tr_cas_parent_lock");
			GUI_EndDialog("win_tr_cas_ipp_buy");				
#endif
				GUI_SetProperty("win_full_screen_img_mute","state","hide");
				GUI_EndDialog("win_menu_volume");
				GUI_EndDialog("win_prog_bar");
				GUI_EndDialog("win_prog_num");

#ifdef APP_SD
			app_play_stop();
				app_play_set_zoom_para(192, 152, 348, 240);
				GUI_CreateDialog("win_main_menu");
			app_play_reset_play_timer(0);
				GUI_SetInterface("flush", NULL);

#endif 
#ifdef APP_HD
				GUI_CreateDialog("win_main_menu");
				GUI_SetInterface("flush", NULL);
				app_play_video_window_zoom(400, 160, 500, 290);
#endif	
				break;

			case KEY_TRACK://ÉùµÀÉèÖÃ


				{
					fullscreen_hide_osd_scroll();
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");
					GUI_CreateDialog("win_audio_set");
				}
				break;

			case KEY_EXIT://ÍË³ö
				event->key.sym = KEY_OK;//ÍË³ö»áEnddialoag,ÉèÖÃÎªÎÞÐ§¡£
				break;

			case KEY_FAV:
#if DVB_PVR_FLAG
				if (1 == app_pvr_state_check(0, 0))
				{
					break;
				}
#endif	

#ifdef DVB_AD_TYPE_BY_FLAG
				AdverPsiStop();
#endif

				if (0 != app_prog_check_group_num(GROUP_MODE_FAV, app_prog_get_stream_type(),1))
				{
					GxBusPmViewInfo sysinfo;
					GxBus_PmViewInfoGet(&sysinfo);

					GxBusPmDataProg Prog;
					uint32_t pos;

					fullscreen_hide_osd_scroll();
					app_play_stop();
					GUI_SetProperty("win_full_screen_img_mute","state","hide");

					app_prog_get_playing_pos_in_group(&pos);
					GxBus_PmProgGetByPos(pos,1,&Prog);
					if (Prog.favorite_flag == 0)
					{
						/*ÅÐ¶Ïµ±Ç°²¥·ÅÊÇ·ñÏ²°®œÚÄ¿£¬·ÇÏ²°®Ä£Êœ£¬Í£Ö¹ÒôÊÓÆµ²¥·Å*/
						app_play_stop_audio();
					}

					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");
						app_play_stop();
						app_play_set_zoom_para(99,140,248,186);

					GUI_CreateDialog("win_favorite_prog_list");
					GUI_SetInterface("flush", NULL);
						if (FALSE == app_play_get_play_status())
							{
#ifndef DVB_CA_TYPE_DS_CAS50_FLAG
								app_play_switch_prog_clear_msg();
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
								app_play_reset_play_timer(0);
							}

#ifdef APP_HD
					app_play_video_window_zoom(630, 160, 480, 300);
#endif
				}
				else
				{
					app_play_clear_msg_pop_type(MSG_POP_INVALID_PROG);

					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Favorite Program!",POPMSG_TYPE_OK);
				}
				break;

			case KEY_EPG:
#if DVB_PVR_FLAG
				if (1 == app_pvr_state_check(0, 0))
				{
					break;
				}
#endif	
#ifdef DVB_AD_TYPE_BY_FLAG
				AdverPsiStop();
#endif

				GUI_SetProperty("win_full_screen_img_mute","state","hide");
				if (0 == app_prog_get_num_in_group())
				{
					if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
							app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);
						}
					else
						{
							app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);
						}
					return EVENT_TRANSFER_STOP;
				}

				fullscreen_hide_osd_scroll();
				GUI_EndDialog("win_menu_volume");
				GUI_EndDialog("win_prog_bar");
				GUI_EndDialog("win_prog_num");

#ifdef APP_SD
				app_play_stop();
				app_play_set_zoom_para(64,116,220,150);
				GUI_CreateDialog("win_epg_list");
				GUI_SetInterface("flush", NULL);
				app_play_video_window_zoom(64,116,224,150);
				GUI_SetInterface("video_top", NULL);
#endif
#ifdef APP_HD
				app_play_set_zoom_para(890, 170, 220, 180);
				GUI_CreateDialog("win_epg_list");
				GUI_SetInterface("flush", NULL);
				app_play_video_window_zoom(890, 170, 220, 180);
#endif
				break;

			case KEY_OK://PROG_LIST
			case KEY_PROG_LIST:
				/*begin:modify by cth*/
#if defined(APP_SD) && !defined(DVB_CA_TYPE_MG_FLAG) && !defined(DVB_CA_TYPE_MG312_FLAG)
				/*È·ÈÏŒüÏÔÊŸÆµµÀÁÐ±í*/
				if (0 != app_prog_get_num_in_group() && (g_AppPvrOps.state == PVR_DUMMY))
				{/*ÂŒÏñÊ±œûÖ¹ÏÔÊŸ*/
				if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
							app_play_stop();
						}

					app_play_set_zoom_para(85,140,248,186);
					fullscreen_hide_osd_scroll();
					GUI_CreateDialog("win_menu_tv_list");
					GUI_SetInterface("flush", NULL);
					app_play_video_window_zoom(85,140,248,186);
					if (FALSE == app_play_get_play_status())
						{
							app_play_switch_prog_clear_msg();	
							app_play_reset_play_timer(0);
						}
					return EVENT_TRANSFER_STOP;	
				}
#endif
				/*end:modify by cth*/

#ifdef CA_FLAG
#if defined(DVB_CA_TYPE_MG_FLAG) || defined(DVB_CA_TYPE_MG312_FLAG)
				if ((DVB_CA_TYPE_MG == app_flash_get_config_dvb_ca_flag()) 
						|| (DVB_CA_TYPE_MG312 == app_flash_get_config_dvb_ca_flag()))
				{	
#ifdef DVB_CA_TYPE_MG_FLAG
					if (mg_curprogram_is_ppv())//µ±Ç°ÎªppvœÚÄ¿
					{
						printf("###This is a PPV@@@\n");
						GUI_CreateDialog("win_mg_cas_ppv");
					}
					else
#elif defined(DVB_CA_TYPE_MG312_FLAG)
					if (mg312_curprogram_is_ppv())//µ±Ç°ÎªppvœÚÄ¿
					{
						printf("###This is a PPV@@@\n");
						GUI_CreateDialog("win_mg_cas312_ppv");
					}
					else
#endif
					{/*wangjian add on 20141219*/
						if ((0 != app_prog_get_num_in_group()) 
								&& (g_AppPvrOps.state == PVR_DUMMY))
						{
							fullscreen_hide_osd_scroll();
			if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
							app_play_stop();
						}
				        app_play_set_zoom_para(85,140,248,186);	
				        app_play_video_window_zoom(85,140,248,186);
							GUI_CreateDialog("win_menu_tv_list");
							GUI_SetInterface("flush", NULL);

							if (FALSE == app_play_get_play_status())
						{
							app_play_switch_prog_clear_msg();	
							app_play_reset_play_timer(0);
						}

							return EVENT_TRANSFER_STOP;	
						}
					}
				}
#endif            
#endif
				break;

			case KEY_PROG_INFO:
#ifdef APP_SD
				/*ÏÔÊŸÐÅºÅÇ¿¶È*/
				if (0 != app_prog_get_num_in_group())
				{
					fullscreen_hide_osd_scroll();
					GUI_CreateDialog("win_prog_info");
					GUI_SetInterface("flush", NULL);
					return EVENT_TRANSFER_STOP;	
				}
#endif
#if 0
				if (0 != app_prog_get_num_in_group())
				{
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");
					GUI_CreateDialog("win_prog_bar");
				}
#endif
				break;
			case KEY_BLUE:
//				app_panel_set_gpio_high(61);
				/*begin:modify by cth*/
#ifdef APP_SD
				/*ÏÔÊŸÐÅºÅÇ¿¶È*/
				if (0 != app_prog_get_num_in_group())
				{
					fullscreen_hide_osd_scroll();
					GUI_CreateDialog("win_prog_info");
					GUI_SetInterface("flush", NULL);
					return EVENT_TRANSFER_STOP;	
				}
#endif
				/*end:modify by cth*/
				break;

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
						app_play_switch_prog_clear_msg();
						GUI_EndDialog("win_menu_volume");
						GUI_EndDialog("win_prog_bar");
						GUI_EndDialog("win_prog_num");

						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(PLAY_TIMER_DURATION);

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
						GUI_EndDialog("win_menu_volume");
						GUI_EndDialog("win_prog_bar");
						GUI_EndDialog("win_prog_num");

						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(PLAY_TIMER_DURATION);
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
						if (0 != app_play_switch_tv_radio())
						{
							break;
						}

						app_play_switch_prog_clear_msg();
						GUI_EndDialog("win_menu_volume");
						GUI_EndDialog("win_prog_bar");
						GUI_EndDialog("win_prog_num");

						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(PLAY_TIMER_DURATION);
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
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");

						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(PLAY_TIMER_DURATION);
					}
				}
				break;

#if 0// DVB_PVR_FLAG
			case KEY_GREEN:
				if (app_prog_get_num_in_group() > 0)
				{
					printf("pvr ops state = %d\n",g_AppPvrOps.state);
					if (g_AppPvrOps.state != PVR_DUMMY)
					{
						GUI_EndDialog("win_prog_bar");
						GUI_CreateDialog("wnd_pvr_bar");
						GUI_SendEvent("wnd_pvr_bar",event);
					}
					else
					{
						full_pause_exec();
					}
				}
				return EVENT_TRANSFER_STOP;

			case KEY_RED:
				if (app_prog_get_num_in_group() > 0)
				{
					printf("pvr red ops state = %d\n",g_AppPvrOps.state);
					if(g_AppPvrOps.state != PVR_DUMMY)
					{
						GUI_EndDialog("win_program_info");
						GUI_CreateDialog("wnd_pvr_bar");
						GUI_SendEvent("wnd_pvr_bar",event);
					}
					else
					{
						full_pvr_exec();
					}
				}
				return EVENT_TRANSFER_STOP;

			case KEY_YELLOW:
				if(g_AppPvrOps.state != PVR_DUMMY)
				{
					printf("full screen yellow button\n");
					//notice
					//app_book_stop_clear();//huangbc del
#if 0
					app_pvr_stop();
					GUI_EndDialog("wnd_pvr_bar");
					GUI_SetProperty("wnd_pvr_bar", "draw_now", NULL);
					GUI_CreateDialog("win_prog_bar");
#else
					GUI_EndDialog("win_prog_bar");
					GUI_CreateDialog("wnd_pvr_bar");
					GUI_SendEvent("wnd_pvr_bar",event);
#endif
				}
				break;
#endif
#if MEDIA_SUBTITLE_SUPPORT
			case KEY_F1:  //ttx
				{
					app_ttx_magz_open();
				}
				break;

			case KEY_F2:   //subt:s
				{
					if((gEventState.pause == STATE_ON))
					{
						break;
					}

#if DVB_PVR_FLAG
                    if((g_AppPvrOps.state == PVR_TIMESHIFT)
                       || (g_AppPvrOps.state == PVR_TMS_AND_REC))
                    {
						 app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Stop PVR first please",POPMSG_TYPE_OK);
                         break;
                    }
#endif
					GUI_SetProperty("txt_full_state", "state", "hide");
					GUI_SetProperty("img_full_state", "state", "hide");

					if (GUI_CheckDialog("win_prog_bar") == GXCORE_SUCCESS)
					{
						GUI_EndDialog("win_prog_bar");
					}

					GUI_CreateDialog("wnd_subtitling");	
				}
				break;
#endif

			case KEY_MAIL: /*wangjian add on 20141219*/
				{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
            GUI_CreateDialog("win_desai_cas50_email");
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
					//if (DVB_CA_TYPE_MG == app_flash_get_config_dvb_ca_flag())
					{
						if (0 != app_prog_get_num_in_group())
						{
							fullscreen_hide_osd_scroll();
							GUI_EndDialog("win_menu_volume");
							GUI_EndDialog("win_prog_bar");
							GUI_EndDialog("win_prog_num");
							GUI_CreateDialog("win_mg_cas312_email");
						}
					}
#endif
#ifdef DVB_CA_TYPE_DVB_FLAG
					//if (DVB_CA_TYPE_MG == app_flash_get_config_dvb_ca_flag())
					{
						if (0 != app_prog_get_num_in_group())
						{
							fullscreen_hide_osd_scroll();
							GUI_EndDialog("win_menu_volume");
							GUI_EndDialog("win_prog_bar");
							GUI_EndDialog("win_prog_num");
							GUI_CreateDialog("win_dvb_cas_email");
						}
					}
#endif

				}
				break;
#ifdef CUST_TAIKANG
			case KEY_RED://自动搜
				if (0 != app_prog_get_num_in_group())
				{
					fullscreen_hide_osd_scroll();
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");
				}
				if(app_play_get_play_status() == TRUE)
					app_play_stop();
				app_win_auto_search();
				break;
			case KEY_YELLOW://手动
				if (0 != app_prog_get_num_in_group())
				{
					fullscreen_hide_osd_scroll();
					GUI_EndDialog("win_menu_volume");
					GUI_EndDialog("win_prog_bar");
					GUI_EndDialog("win_prog_num");
				}
				GUI_CreateDialog("win_manual_search");
				break;
#endif
			default:
				break;
		}

		return EVENT_TRANSFER_KEEPON;
	}

	return EVENT_TRANSFER_STOP;
}



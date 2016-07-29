/**
 *
 * @file        app_common_play.c
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include "app_common_flash.h"
#include <gxtype.h>
 #include "app_common_prog.h"
 #include "app_common_epg.h"
 #include "app_common_table_pmt.h"
 #include "app_common_search.h"
 #include "gui_timer.h"
 #include "gxplayer.h"
 #include "gxfrontend.h"
 #include "gxmsg.h"
 #include "gxbook.h"
 #include "gxsearch.h"
 #include "gxepg.h"
 #include "gui_core.h"
#include "gxapp_sys_config.h"
#include "app_common_panel.h"
 #include <sys/ioctl.h>
 #include "app_common_porting_stb_api.h"
 #include "gxupdate.h"
#include "app_common_porting_stb_api.h"
#include "app_common_ad.h"
#include "osd_msg/app_osd_msg.h"
#include "app_common_play.h"
#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#if DVB_MEDIA_FLAG
#include "play_manage.h"
#endif
#include "gdi_core.h"
#if DVB_PVR_FLAG
#include "app_pvr.h"
#endif

#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#include "desai_ca_api.h"
#endif
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
#include "abv_ca_interface.h"
#endif

#ifdef  DVB_CA_TYPE_TR_FLAG
#include "Tr_Cas.h"

 /*wangjian add on 20150331*/
extern void app_tr_cas_ipp_mark_clear_check(uint16_t serv_id);
#endif

#if MEDIA_SUBTITLE_SUPPORT
extern void app_subt_pause(void);
#endif

uint8_t prog_scramble_flag = 0;


static PlayerWindow rect_video = {0};
static uint32_t zoom_flag = 0;
static event_list* spApp_play_timer = NULL;
static app_msg gApp_msg_list[MAX_MSG_NUM];
/*static */uint8_t gShowAudioFalg = FALSE;
int8_t url[GX_PM_MAX_PROG_URL_SIZE]={0};
/*static*/ play_scramble_para_t play_Para={0};


static uint8_t Msgpoptype[64] = {0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0};
uint8_t oldMsgpoptype = 0xFF;
static uint8_t play_status = FALSE;
static uint8_t running_status = FALSE;
static uint32_t play_count = 0; /*��???л?????*/
static uint8_t video_hide = FALSE;
/*static*/ uint8_t mute_flag= FALSE;
static prompt_show_msg_callback app_play_show_prompt_msg_callback = NULL;
const char* password_widget_win = NULL;
static uint32_t reflush_flag = 0;

void app_play_register_show_prompt_msg_callback(prompt_show_msg_callback promptshowmsgcallback)
{
	if (NULL != promptshowmsgcallback)
	{
		app_play_show_prompt_msg_callback = promptshowmsgcallback;
	}
}

void app_play_register_password_widget_window(const char* password_widget)
{
	if (NULL != password_widget)
		password_widget_win = password_widget;
	return ;
}

void app_play_set_prompt_reflush_flag(uint32_t flag)
{
	reflush_flag = flag;
}

uint32_t app_play_get_prompt_reflush_flag(void)
{
	return reflush_flag;
}



/*
* ??ȡ??ǰ????Ƶ?Ƿ񲥷?״̬
*/
uint8_t app_play_get_play_status(void)
{
	return play_status;
}
/*
* ??ȡ??ǰ????Ƶ?Ƿ??ɹ?????״̬
*/
uint8_t app_play_get_running_status(void)
{
	return running_status;
}

/*
* ???õ?ǰ????Ƶ?Ƿ??ɹ?????״̬
*/
uint8_t app_play_set_running_status(uint8_t status)
{
	 running_status = status;
	 return running_status;
}

/*
* ??Ƶ???ö?
*/
void app_play_set_zoom_video_top(void)
{
		/*
		* ?б???jpgͼƬ?Ĳ˵???????/??????Ƶ????
		* ??Ƶ?????ö???
		*/
#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD)
		GUI_SetInterface("video_top", NULL);
#endif	
	return ;
}



/*
* ֹͣ??/??Ƶ???ţ???Ϊ??Ƶ?б????л???Ŀ???����?ͼƬ
*/
void app_play_stop_audio(void)
{
	if (TRUE == gShowAudioFalg)
	{
		/*
		* ?㲥??????ʾ״̬
		*/
		if (TRUE == play_status)
		{
			/*
			* ֹͣ??Ƶ??????PLAYER3
			*/
			app_send_msg(GXMSG_PLAYER_STOP,PLAYER3);
			play_status = FALSE;
		}
	}
	else
	{
		if (TRUE == play_status)
		{
			/*
			* ֹͣ??(??)Ƶ??????PLAYER0
			*/
			app_send_msg(GXMSG_PLAYER_STOP,PLAYER0);
			video_hide = TRUE;
			gShowAudioFalg = FALSE;
			play_status = FALSE;
		}
	}
	app_play_set_running_status(FALSE);
	return;
}

/*
* ֹͣ????Ƶ???ţ?ͬʱ?ر?PLAYER3??PLAYER0
* ?????жϵ?ǰ??Ŀ????
*/
void app_play_stop(void)
{
#ifdef DVB_AD_TYPE_BY_FLAG
	AdverPsiStop();
#endif

	if (TRUE == gShowAudioFalg)
		{
			/*
			* ?㲥??????ʾ״̬
			*/
			if (TRUE == play_status)
				{
					/*
					* ֹͣ??Ƶ??????PLAYER3
					*/
					app_send_msg(GXMSG_PLAYER_STOP,PLAYER3);
					play_status = FALSE;
				}

				/*
				* ֹͣ?㲥??????????PLAYER0
				*/
				app_send_msg(GXMSG_PLAYER_STOP,PLAYER0);

		}
	else
		{
			if (TRUE == play_status)
			{
				/*
				* ֹͣ??Ƶ??Ŀ/??Ƶ??????????PLAYER0
				*/
				app_send_msg(GXMSG_PLAYER_STOP,PLAYER0);
				play_status = FALSE;
			}
		}

	/*
	 *  ??????һ????/???Ž?Ŀ???????򣬱???ֹͣ????
	 * ??һ????/???Ž?Ŀ??????????δ??????????
	 */
	 if (NULL != password_widget_win)
		GUI_EndDialog(password_widget_win);

	video_hide = TRUE;

	gShowAudioFalg = FALSE;
	
	app_play_set_running_status(FALSE);

	//play_status = FALSE;

	return;
}
void app_play_video_audio_ds(int32_t pos)
{
	static int32_t FirstPowerFlag = TRUE;
	int32_t Config;
	player_t play={0};
#ifdef _DEBUG

		test_for_show_time_ms("app_play_video_audio:200");
#endif
	app_play_set_running_status(FALSE);
	/*
	* ??????????????????0?1??????0?7????0?2??????0?6?????0?0
	*/
	if(TRUE == FirstPowerFlag)
	{
		FirstPowerFlag = FALSE;
		Config = app_flash_get_config_mute_flag();
		if(Config == 1)
		{
			app_play_set_mute(Config);
		}
	}

	if(zoom_flag == 1)
	{
		app_play_video_window_zoom(rect_video.x,rect_video.y,rect_video.width,rect_video.height);
	}

	/*
	 * ?????????0?1
	 */
	play.num = pos;
	play.param = 0;
	play.player = PLAYER0;
	app_send_msg(GXMSG_PLAYER_PLAY,(void*)&play);
	video_hide = FALSE;

	app_epg_sync_time();
#ifdef DVB_AD_TYPE_CMIS_FLAG
	{
		GxBusPmDataProg prog_data = {0};

		GxBus_PmProgGetByPos(pos,1,&prog_data);
		if(0 == mute_flag )
		{
			app_play_set_compsate_and_track(prog_data);
		}
	}
#else
	app_play_set_cur_prog_track();

	if (0 == mute_flag )
	{
		app_play_set_cur_prog_volumn();
	}
#endif



	return ;

}
/*
* ????????Ƶ
*/
void app_play_video_audio(int32_t pos)
{
	static int32_t FirstPowerFlag = TRUE;
	int32_t Config;
	player_t play={0};
#ifdef _DEBUG

		test_for_show_time_ms("app_play_video_audio:200");
#endif

	/*
	* ??????̨֮ǰ????λ?ɹ???????־
	*/
	app_play_set_running_status(FALSE);

	/*
	* ??????һ?β???????Ƶ?????ÿ???ǰ?????ľ???״̬
	*/
	if(TRUE == FirstPowerFlag)
	{
		FirstPowerFlag = FALSE;
		Config = app_flash_get_config_mute_flag();
		if(Config == 1)
		{
			app_play_set_mute(Config);
		}
	}
	if (FALSE == gShowAudioFalg)
	{
		/*
		* ?????л????㲥,??ʾ?㲥????ͼƬ
		*/
		if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
		{
			play.param = 1;
			app_send_msg(GXMSG_PLAYER_PLAY, (void*)&play);
			gShowAudioFalg = TRUE;
			GxCore_ThreadDelay(500);//???ⲥ?Ų???��????ͼƬ
		}
	}

	if(zoom_flag == 1)
	{
		app_play_video_window_zoom(rect_video.x,rect_video.y,rect_video.width,rect_video.height);
	}

//	play_status = TRUE;

	/*
	* ????????Ƶ
	*/
	play.num = pos;
	play.param = 0;
	play.player = PLAYER0;
	app_send_msg(GXMSG_PLAYER_PLAY,(void*)&play);
	video_hide = FALSE;

	app_epg_sync_time();
#ifdef DVB_AD_TYPE_CMIS_FLAG
{
	GxBusPmDataProg prog_data = {0};

	GxBus_PmProgGetByPos(pos,1,&prog_data);
	if(0 == mute_flag )
	{
		app_play_set_compsate_and_track(prog_data);
	}
}
#else
	app_play_set_cur_prog_track();

	if (0 == mute_flag )
	{
		app_play_set_cur_prog_volumn();
	}
#endif

	return ;

}

/*
* ??ʾ??Ƶ????ͼƬ
*/
void app_play_show_logo_for_tv(void)
{
//	if(GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
//		return;
//	gShowAudioFalg = FALSE;
	if (FALSE == gShowAudioFalg)
		{
			gShowAudioFalg = TRUE;
			player_t play={0};
			play.param = 1;
			app_send_msg(GXMSG_PLAYER_PLAY, (void*)&play);
		}
	return ;
}

void app_play_show_logo(void)
{
	if (FALSE == gShowAudioFalg)
		{
			gShowAudioFalg = TRUE;
			player_t play={0};
			play.param = 1;
			app_send_msg(GXMSG_PLAYER_PLAY, (void*)&play);
		}
	return ;
}


uint32_t app_play_i_frame_logo(int32_t VideoResolution,char* path)
{

		//logo
#if 1
		GxBus_ConfigSet(PLAYER_CONFIG_AUTOPLAY_VIDEO_NAME, "player0");
		GxBus_ConfigSet(PLAYER_CONFIG_AUTOPLAY_VIDEO_URL, path);
		printf("I_FRAME_PATH=%s\n",path);
#endif
		GxBus_ConfigSetInt(PLAYER_CONFIG_AUTOPLAY_RADIO_FLAG, 0);
		GxBus_ConfigSetInt(PLAYER_CONFIG_AUTOPLAY_PLAY_FLAG, 1);



		printf("VideoResolution= %d\n",VideoResolution);


		return 0;
}

/*
* ?رտ???????
*/
uint32_t app_play_close_av_logo()
{
	app_send_msg(GXMSG_PLAYER_STOP,PLAYER0);
	return  GxAvdev_CloseLogo();
}

/*
* ?ر???Ƶ??
*/
void app_play_hide_video_layer()
{
	if (TRUE == video_hide)
		return;

	app_send_msg(GXMSG_PLAYER_VIDEO_HIDE,NULL);
	video_hide = TRUE;

	return ;
}

/*
* ??????Ƶ??
*/
void app_play_show_video_layer()
{
	if (FALSE == video_hide)
		return ;

	app_send_msg(GXMSG_PLAYER_VIDEO_SHOW,NULL);
	video_hide = FALSE;
	return ;
}

int32_t  app_play_switch_tv_radio(void)
{
	uint32_t pos=0;
	GxBusPmViewInfo sysinfo_record;
	GxBusPmViewInfo sysinfo;
#if DVB_PVR_FLAG
	if (1 == app_pvr_state_check(0, 0))
	{
		return -1;
	}
#endif

	app_prog_mutex_lock();
	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysinfo_record ,&sysinfo,sizeof(GxBusPmViewInfo));
	sysinfo.group_mode= GROUP_MODE_ALL;
	//sysinfo.taxis_mode=TAXIS_MODE_NON;
	if(sysinfo.stream_type == GXBUS_PM_PROG_TV)
	{
		sysinfo.stream_type = GXBUS_PM_PROG_RADIO;
	}
#if (1 == DVB_HD_LIST)
	else
		if (sysinfo.stream_type == GXBUS_PM_PROG_HD_SERVICE)
			{
				sysinfo.stream_type = GXBUS_PM_PROG_RADIO;
			}
	else
	{
		play_prog *pold;
		app_prog_get_old_play_prog(&pold);
		if ((GXBUS_PM_PROG_HD_SERVICE == pold->SysInfo.stream_type
			&&(0xffffffff != pold->id)
			 ))
			sysinfo.stream_type = GXBUS_PM_PROG_HD_SERVICE;
		else
			sysinfo.stream_type = GXBUS_PM_PROG_TV;

	}
#else
	else
	{
		sysinfo.stream_type = GXBUS_PM_PROG_TV;
	}
#endif
	GxBus_PmViewInfoModify(&sysinfo);
	app_prog_update_num_in_group();
	if(0 == app_prog_get_num_in_group())
	{
#if (1 == DVB_HD_LIST)
		if (GXBUS_PM_PROG_TV == sysinfo.stream_type)
			{
				sysinfo.stream_type = GXBUS_PM_PROG_HD_SERVICE;
				GxBus_PmViewInfoModify(&sysinfo);
			}
		else
			if (GXBUS_PM_PROG_HD_SERVICE == sysinfo.stream_type)
			{
				sysinfo.stream_type = GXBUS_PM_PROG_TV;
				GxBus_PmViewInfoModify(&sysinfo);
			}
		app_prog_update_num_in_group();
		if(0 == app_prog_get_num_in_group())
#endif
			{
				GxBus_PmViewInfoModify(&sysinfo_record);
				app_prog_update_num_in_group();
				app_prog_mutex_unlock();
				return -1;
			}
	}

	app_prog_get_playing_pos_in_group(&pos);	
	app_prog_mutex_unlock();

	app_play_stop();
	app_play_switch_prog_clear_msg();
	app_prog_get_playing_pos_in_group(&pos);
	app_prog_save_playing_pos_in_group(pos);


	return 0;
}

uint32_t app_play_recall_prog(void)
{
	play_prog *pold = NULL;
	GxBusPmViewInfo sys_info;
	uint32_t pos = 0;
	int32_t pos_old;
	uint8_t group_mode = 0;
	uint8_t stream_type =0;
	uint8_t fav_id = 0;

	int32_t ret; 
	uint32_t prog_id=0;

	app_prog_get_old_play_prog(&pold);

	if(0xffffffff == pold->id)
		{
			/*
			* startup ,no pre view prog now
			*/
			return FALSE;
		}
	
	if ( -1 == app_prog_get_pos_in_group_by_id(pold->id,&pos))
		{
			return FALSE;
		}

#if DVB_PVR_FLAG
	if(1 == app_pvr_state_check(0,pos))
	{
		return FALSE;
	}
#endif

	if ( GXBUS_PM_PROG_TV == pold->SysInfo.stream_type)
		{
			prog_id = pold->SysInfo.tv_prog_cur;		
		}
	else
#if (1 == DVB_HD_LIST)
		if ( GXBUS_PM_PROG_HD_SERVICE == pold->SysInfo.stream_type)
			{
				prog_id = pold->SysInfo.hd_prog_cur;				
			}
	else
#endif
	{
		prog_id = pold->SysInfo.radio_prog_cur;				
	}

		ret = app_prog_find_by_id(prog_id,&pos_old,&group_mode,&stream_type,&fav_id);
		if(ret == -1)
		{
			return FALSE;
		}
		else if(4 == ret)//??ĿΪ????
		{
			return ret;
		}
	app_prog_mutex_lock();
	GxBus_PmViewInfoGet(&sys_info);

	/*???ӹ㲥?б?֮???л??ؿ????رջؿ?ǰ??Ƶ*/
	if (sys_info.stream_type != pold->SysInfo.stream_type)
	{
		app_play_stop();
	}

	/*
	* ??ǰ???Ž?Ŀ??????һ???ۿ???Ŀ????һ????
	* ?ؿ??????л?Ϊ????ģʽ
	*/
	if (sys_info.group_mode != pold->SysInfo.group_mode)
		{
			pold->SysInfo.group_mode = GROUP_MODE_ALL;
		}
	
	if (0 != memcmp(&sys_info,&(pold->SysInfo),sizeof(GxBusPmViewInfo)))
		{
			GxBus_PmViewInfoModify(&(pold->SysInfo));
		}
	
	app_prog_record_playing_pos(pos);
	app_prog_update_num_in_group();
	app_prog_mutex_unlock();
	return TRUE;

}

/*
* ÿ????Ŀ??��????????
*/
 void app_play_set_cur_prog_track(void)
{
	if (0 == app_flash_get_config_track_globle_flag())
		{
			/*??ȫ?????��????ŵ?ǰ??Ŀ????*/
			int32_t track = app_flash_get_config_audio_track();
			app_play_set_audio_track(track);
		}

	return;
}

/*
* ÿ????Ŀ??��??????��
*/
void app_play_set_cur_prog_volumn(void)
{

	if (0 == app_flash_get_config_volumn_globle_flag())
		{
			/*??ȫ????��?????ŵ?ǰ??Ŀ??��*/
			int32_t volumn = app_flash_get_config_audio_volume();
			printf("  audio_volume:%d----------\n", volumn);
			if(volumn > 100)
			{
				volumn = 100;
			}
			else if(volumn < 0)
			{
				volumn = 0;
			}

			app_play_set_volumn(volumn);
		}

}

/*
* ȫ???????¼???̨
*/
uint32_t app_play_by_direction(int32_t direction)
{
	uint32_t pos_in_group=0;
	uint32_t pos=0;
	if ( -1 == app_prog_get_playing_pos_in_group(&pos_in_group))
		{
			return 0;
		}
	pos = pos_in_group;
	if(direction == 1)
	{
		if(pos_in_group >  0)
		{
			pos_in_group --;
		}
		else
		{
			pos_in_group = app_prog_get_num_in_group() -1;
		}
#if DVB_PVR_FLAG
		if(1 == app_pvr_state_check(1,pos_in_group))
		{
			return pos;
		}
#endif
	}
	else if(direction == -1)
	{
		if(pos_in_group < app_prog_get_num_in_group() -1)
		{
			pos_in_group ++;
		}
		else
		{
			pos_in_group = 0;
		}
#if DVB_PVR_FLAG
		if(1 == app_pvr_state_check(1,pos_in_group))
		{
			return pos;
		}
#endif
	}

	app_prog_save_playing_pos_in_group(pos_in_group);
	return pos_in_group;
}


/*
* ????????Ƶ???Ų?????С
* app_play_video_audio???Ž?Ŀʱ??app_play_video_window_zoom??????Ƶ??С
*/
void app_play_set_zoom_para(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	if ((VIDEO_WINDOW_W == w)&&(VIDEO_WINDOW_H == h))
	{
		zoom_flag = 0;
		rect_video.x = 0;
		rect_video.y = 0;
		rect_video.width= VIDEO_WINDOW_W;
		rect_video.height= VIDEO_WINDOW_H;
		return;
	}

	zoom_flag = 1;
	rect_video.x = x;
	rect_video.y = y;
	rect_video.width= w;
	rect_video.height= h;
	return;
}


/*
* ??Ƶ????????
*/
void app_play_video_window_zoom(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	GxMsgProperty_PlayerVideoWindow video_window;
	zoom_flag = 1;
	{
		video_window.player = NULL;
		video_window.rect.x = x;
		video_window.rect.y = y;
		video_window.rect.width = w;
		video_window.rect.height = h;
		rect_video.x = x;
		rect_video.y = y;
		rect_video.width= w;
		rect_video.height= h;
//		printf("app_play_video_window_zoom x=%d y=%d w=%d h=%d\n",
//			x,y,w,h);
		app_send_msg(GXMSG_PLAYER_VIDEO_WINDOW,(void*)&video_window);
	}
}

/*
* ??Ƶ???ڷŴ???ȫ??
*/
void app_play_video_window_full()
{
	GxMsgProperty_PlayerVideoWindow video_window;
	if(zoom_flag == 1)
	{
		zoom_flag = 0;
		rect_video.x = VIDEO_WINDOW_X;
		rect_video.y= VIDEO_WINDOW_Y;
		rect_video.width = VIDEO_WINDOW_W;
		rect_video.height = VIDEO_WINDOW_H;
		video_window.player = NULL;
		video_window.rect.x = VIDEO_WINDOW_X;
		video_window.rect.y = VIDEO_WINDOW_Y;
		video_window.rect.width = VIDEO_WINDOW_W;
		video_window.rect.height = VIDEO_WINDOW_H;
		app_send_msg(GXMSG_PLAYER_VIDEO_WINDOW,(void*)&video_window);
	}

	return;
}

/*
* ???ŵ?ǰ??Ŀ
*/
//static int32_t app_play_playingpos_in_group(void)
 int32_t app_play_playingpos_in_group(void)
{

	uint32_t pos;
	GxBusPmDataProg prog_data = {0};
	GxBusPmViewInfo sys_info;
	GxBusPmDataTP tp = {0};
	uint32_t   panel_prog_cur;

	if (0 == app_prog_get_num_in_group())
	{
		/*
		* ??Ŀ????Ϊ0???˳?
		*/
		#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD)
			//if(app_win_check_video_win())
				app_play_show_logo_for_tv();
		#endif
		return 0;
	}
	
	GxBus_PmViewInfoGet(&sys_info);
	if (-1 == app_prog_get_playing_pos_in_group(&pos))
		{
			return 0;			
		}
	GxBus_PmProgGetByPos(pos,1,&prog_data);
	GxBus_PmTpGetById(prog_data.tp_id, &tp);
	/*
	 *  ??????һ????/???Ž?Ŀ???????򣬱????л?????????Ŀ
	 * ??һ????/???Ž?Ŀ??????????δ??????????
	a */
    if (NULL != password_widget_win)
    {
        GUI_EndDialog(password_widget_win);
		GUI_SetInterface("flush", NULL);
    }
    if(prog_data.skip_flag == GXBUS_PM_PROG_BOOL_ENABLE)
    {
        printf("??Ŀ??????????\n");
		app_play_set_msg_pop_type(MSG_POP_PROG_SKIP);
		app_play_stop_audio();
        if(GXBUS_PM_PROG_RADIO == sys_info.stream_type )
        {
          app_play_show_logo();
        }
#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD)
        else
        {
            app_play_show_logo_for_tv();
        }
#endif

#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
        app_search_lock_tp(tp.frequency, tp.tp_c.symbol_rate/1000, INVERSION_OFF, tp.tp_c.modulation-1 ,0);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
        app_search_lock_tp(tp.frequency, tp.tp_dtmb.symbol_rate/1000, INVERSION_OFF, tp.tp_dtmb.modulation-1 ,0);
#endif

    }
	else if(prog_data.lock_flag == GXBUS_PM_PROG_BOOL_ENABLE )
	{
        //modify by cth
		app_play_set_msg_pop_type(MSG_POP_PROG_LOCK);//by remove by xushx for jy_chengdu
		app_play_stop_audio();
		if(GXBUS_PM_PROG_RADIO == sys_info.stream_type )
		{
			// ???Ź㲥????
			app_play_show_logo();
		}
#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD)
		else
		{
			app_play_show_logo_for_tv();
		}
#endif
		if (NULL != password_widget_win)
		{
			GUI_CreateDialog(password_widget_win);
            //modify by cth
			//GUI_EndDialog("win_prog_bar");
			GxBus_PmTpGetById(prog_data.tp_id, &tp);
			/*ǰ??????ʾ??Ŀ?Ż??߼?Ƶ?��?*/
			if(LCN_STATE_OFF == app_flash_get_config_lcn_flag())
			{
				panel_prog_cur = pos+1;
				app_panel_show(PANEL_DATA,&(panel_prog_cur));
			}
			else
			{
				panel_prog_cur = pos;
				app_panel_show(PANEL_DATA,&(panel_prog_cur));
			}
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			app_search_lock_tp(tp.frequency, tp.tp_c.symbol_rate/1000, INVERSION_OFF, tp.tp_c.modulation-1 ,0);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			app_search_lock_tp(tp.frequency, tp.tp_dtmb.symbol_rate/1000, INVERSION_OFF, tp.tp_dtmb.modulation-1 ,0);
#endif
			/* ????tdtͬ?? */
			app_epg_sync_time();
		}
	}
	else
	{		
		app_play_clear_msg_pop_type(MSG_POP_PROG_LOCK);
		app_play_clear_ca_msg();
		app_play_video_audio(pos);
	}
	return 0;

}

void app_play_set_msg_pop_type_flag(MsgPop_e type)
{
	if (FALSE == Msgpoptype[type])
		{
			Msgpoptype[type] = TRUE;
		}
}

/*
* ???ò?????Ϣ????
*/
void app_play_set_msg_pop_type(MsgPop_e type)
{
	if (FALSE == Msgpoptype[type])
		{
			Msgpoptype[type] = TRUE;
			if (NULL != app_play_show_prompt_msg_callback)
				{
					/*
					* CA??Ϣ??TIMER??ѯ??ʾ??????GUI?쳣
					*/
					if (type < MSG_POP_COMMON_END)
						app_play_show_prompt_msg_callback();
				}
		}
}


void app_play_clear_msg_pop_type_flag(MsgPop_e type)
{
	if (TRUE == Msgpoptype[type])
		{
			Msgpoptype[type] = FALSE;
		}
}


/*
* ????ָ?????͵???Ϣ??ʾ
*/
void app_play_clear_msg_pop_type(MsgPop_e type)
{
	if (TRUE == Msgpoptype[type])
		{
			Msgpoptype[type] = FALSE;

			if (NULL != app_play_show_prompt_msg_callback)
			{
				/*
				* CA??Ϣ??TIMER??ѯ??ʾ??????GUI?쳣
				*/
				if (type < MSG_POP_COMMON_END)
					oldMsgpoptype = MESSAGE_MAX_COUNT;

				/*
				* CA??Ϣ??TIMER??ѯ??ʾ??????GUI?쳣
				*/
				if (type < MSG_POP_COMMON_END)
					app_play_show_prompt_msg_callback();
			}
		}
}

/*
* ??ȡָ?????͵???Ϣ??ʾ״̬
*/
uint32_t app_play_get_msg_pop_type_state(MsgPop_e type)
{
	return Msgpoptype[type];
}

/*
* ???õ?ǰ??Ϣ????
*/
void app_play_set_msg_pop_type_record_pos(MsgPop_e type)
{
	oldMsgpoptype = type;
	return ;
}

/*
* ??????????????ʾ
*/
void app_play_switch_prog_clear_msg(void)
{
	uint32_t i = 0;
	for (i = MSG_POP_PROG_LOCK; i< MESSAGE_MAX_COUNT; i++)
		app_play_clear_msg_pop_type(i);
}

/*
* ????????CA??????ʾ
*/
void app_play_clear_ca_msg(void)
{
	uint32_t i = 0;
	for (i = MSG_POP_COMMON_END; i< MESSAGE_MAX_COUNT; i++)
		app_play_clear_msg_pop_type(i);
}

/*
* ????timer??̨????Ŀ?б���ȫ?��?EPG?ȴ???ͬһ
* timer ??̨(ͬʱֻ??һ??????)
*/
static  int play_timer(void *userdata)
{
	if (play_count >0 )
	{
		app_play_playingpos_in_group();
	}
	play_count = 0; /*��????̨????????*/
	spApp_play_timer = NULL;
	return 0;
}

/*
* ???ö?ʱ????̨
* duration ??ʱ??ʱ?䣬??durationΪ0ֱ?Ӳ???
*/
void app_play_reset_play_timer(uint32_t duration)
{
	if (0 == duration)
	{
		/*��????̨??????Ҫ????��????̨*/
		app_play_remove_play_tmer();
		app_play_stop_audio();
		app_play_playingpos_in_group();//???ŵ?ǰ??Ŀ
		return;
	}
#ifdef _DEBUG

	test_for_show_time_ms("play_timer:830");
#endif


	if(!spApp_play_timer)
	{
		/*
		* ??????̨
		*/
		play_count = 0;
		app_play_playingpos_in_group();
		if (0 != duration)
			spApp_play_timer = create_timer(play_timer, duration, NULL,  TIMER_ONCE);
	}
	else
	{

		play_count++; /*��????̨?????ۼ?*/
#if (DVB_THEME_TYPE == DVB_THEME_HD)
		if (play_count >=2)
			{
				/*
				* ????2??��????????ֹͣ??Ƶ???ű?????̨??????????
				*/
				if (TRUE == app_play_get_play_status())
					app_play_stop_audio();
			}
#endif
		reset_timer(spApp_play_timer);
	}

	return ;
}

void app_play_remove_play_tmer(void)
{
	if (NULL !=spApp_play_timer )
		remove_timer(spApp_play_timer);

	 if (NULL != password_widget_win)
		GUI_EndDialog(password_widget_win);

	play_count = 0; /*��????̨????????*/
	spApp_play_timer = NULL;
	return ;
}

/*?˳����?????Ƶ???Ž??棬?????Ƿ????ڵȴ???ʱ???ŵĶ?ʱ??
   ?????ڣ??˳����?????Ƶ???Ž???ǰ???ȴ??���??*/
void app_play_check_play_timer(void)
{
	if (spApp_play_timer)
	{
		play_timer(NULL);
	}
	return ;
}

/*
* ??һ?ο???????ȡ?ϵ???????Ŀ
*/
int32_t app_play_startup_prog(startup_play_para playpara)
{
	uint32_t result = FALSE;
	GxBusPmViewInfo sys;
	uint32_t ppos = 0;
	GxBus_PmViewInfoGet(&sys);
	if (TRUE == playpara.startupProgFlag)
		{
			/*
			* ????Ҫ?󲥷?ָ????Ŀ
			*/

			;
			result =  TRUE;

		}
	else
		{
			/*
			* ???????Ŷϵ???????Ŀ
			*/

		if(sys.group_mode == GROUP_MODE_FAV)
		{
			if (0 == app_prog_change_group(GROUP_MODE_FAV,app_prog_get_stream_type(), 1))
				app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);
		}
		if((sys.stream_type == GXBUS_PM_PROG_NVOD)||(sys.stream_type == GXBUS_PM_PROG_USER2))
		{
			if (0 == app_prog_change_group(GROUP_MODE_ALL,app_prog_get_stream_type(), 1))
				app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);
		}

			if(app_prog_get_num_in_group() >0)
			{
				app_play_switch_prog_clear_msg();
				if (-1 != app_prog_get_playing_pos_in_group(&ppos))
					{
						app_prog_record_playing_pos(ppos);						
					}

				app_epg_open();
				result =  TRUE;
			}
			else
			{

				if(app_play_switch_tv_radio() == 0)
				{
					app_play_switch_prog_clear_msg();
					if (-1 != app_prog_get_playing_pos_in_group(&ppos))
						{
							app_prog_record_playing_pos(ppos);						
						}

					app_epg_open();
					result =  TRUE;
				}
				else
				{

				}
			}

		}

	return result;


}


void app_play_start_ecm_filter(GxBusPmDataProg prog_data)
{
	play_scramble_para_t playPara={0};
	app_play_get_playing_para(&playPara);
	if(prog_data.scramble_flag == GXBUS_PM_PROG_BOOL_ENABLE)
		playPara.program_type = TRUE;
	else
		playPara.program_type = FALSE;

	playPara.program_num = prog_data.service_id;
	playPara.p_video_pid = prog_data.video_pid;
	playPara.p_audio_pid = prog_data.cur_audio_pid;
	
	if ((prog_data.cur_audio_ecm_pid >0)&&(prog_data.cur_audio_ecm_pid <0x1FFF))
		{
			if (prog_data.cur_audio_ecm_pid == prog_data.ecm_pid_v)
				{	// ????ƵECMPID??Ч??ȫһ??
					playPara.p_ecm_pid = prog_data.ecm_pid_v;
					playPara.p_ecm_pid_video = 0x1FFF;
					playPara.p_ecm_pid_audio = 0x1FFF;
					playPara.scramble_type = SCRAMBLE_TS;
				}
			else
				{	// ????ƵECMPID??һ??
					if (0 ==prog_data.ecm_pid_v )
						playPara.p_ecm_pid_video = 0x1FFF;
					else
						playPara.p_ecm_pid_video = prog_data.ecm_pid_v;

						playPara.p_ecm_pid_audio = prog_data.cur_audio_ecm_pid;
						playPara.p_ecm_pid = 0x1FFF;
						playPara.scramble_type = SCRAMBLE_PES;
				}

		}
	else
		if ((prog_data.ecm_pid_v >0)&&(prog_data.ecm_pid_v <0x1FFF))
		{
			if (prog_data.ecm_pid_v == prog_data.cur_audio_ecm_pid)
				{ // ????ƵECMPID??ȫһ??
					playPara.p_ecm_pid = prog_data.ecm_pid_v;;
					playPara.p_ecm_pid_video = 0x1FFF;
					playPara.p_ecm_pid_audio = 0x1FFF;
					playPara.scramble_type = SCRAMBLE_TS;
				}
			else
				{ // ????ƵECMPID??һ??
					if (0 ==prog_data.cur_audio_ecm_pid )
						playPara.p_ecm_pid_audio = 0x1FFF;
					else
						playPara.p_ecm_pid_audio = prog_data.cur_audio_ecm_pid;

					playPara.p_ecm_pid_video = prog_data.ecm_pid_v;
					playPara.p_ecm_pid = 0x1FFF;
					playPara.scramble_type = SCRAMBLE_PES;
				}

		}
		app_play_clear_ca_msg();
//		app_osd_msg_clear();
		app_cas_api_start_ecm_filter(&playPara);
			
}


app_msg* app_get_msg(uint32_t msg_id)
{
	return &gApp_msg_list[msg_id];
}


void app_free_msg(uint32_t msg_type,uint32_t msg_id)
{
	if (NULL == gApp_msg_list[msg_id].msg)
		return ;

	if (msg_id >= MAX_MSG_NUM)
		return;

	switch(msg_type)
	{
		case GXMSG_EPG_NUM_GET:
			GxBus_MessageFree(gApp_msg_list[msg_id].msg);
			gApp_msg_list[msg_id].msg = NULL;
			gApp_msg_list[msg_id].param = NULL;
			break;
		case GXMSG_EPG_GET:
			if (NULL == gApp_msg_list[msg_id].param)
				return;
			if(((GxMsgProperty_EpgGet*)(gApp_msg_list[msg_id].param))->epg_info)
			{
				GxCore_Free(((GxMsgProperty_EpgGet*)(gApp_msg_list[msg_id].param))->epg_info);
				((GxMsgProperty_EpgGet*)(gApp_msg_list[msg_id].param))->epg_info= NULL;
			}
			GxBus_MessageFree(gApp_msg_list[msg_id].msg);
			gApp_msg_list[msg_id].msg = NULL;
			gApp_msg_list[msg_id].param = NULL;
			break;
		case GXMSG_EPG_NVOD_GET:
			if (NULL == gApp_msg_list[msg_id].param)
				return;
			if(((GxMsgProperty_EpgNvodGet*)(gApp_msg_list[msg_id].param))->nvod_info)
			{
				GxCore_Free(((GxMsgProperty_EpgNvodGet*)(gApp_msg_list[msg_id].param))->nvod_info);
				((GxMsgProperty_EpgNvodGet*)(gApp_msg_list[msg_id].param))->nvod_info= NULL;
			}
			GxBus_MessageFree(gApp_msg_list[msg_id].msg);
			gApp_msg_list[msg_id].msg = NULL;
			gApp_msg_list[msg_id].param = NULL;
			break;
		case GXMSG_BOOK_GET:
			if((GxBookGet*)(gApp_msg_list[msg_id].param))
			{
				GxCore_Free((GxBookGet*)(gApp_msg_list[msg_id].param));
			}
			GxBus_MessageFree(gApp_msg_list[msg_id].msg);
			gApp_msg_list[msg_id].msg = NULL;
			gApp_msg_list[msg_id].param = NULL;
			break;
		default:
			break;
	}

}
//??????Ϣ->?????ڵ?,???д???
int32_t app_send_msg(uint32_t msg_type,void *param)
{
	uint32_t i =0;
	static  uint32_t j = 0;
	uint32_t msg_id =MAX_MSG_NUM;
#ifdef DVB_CA_TYPE_TR_FLAG
	static uint16_t oldTpId=0xFFFF;
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	static uint16_t MoldTpId=0xFFFF;
#endif
#ifdef DVB_CA_TYPE_DVB_FLAG
	static bool dvb_firstPlay = FALSE;
#endif

	if (j+1 <MAX_MSG_NUM)
		{
			for(i = j+1;i<MAX_MSG_NUM;i++)
			{
				if(gApp_msg_list[i].msg == NULL)
				{
					msg_id = i;
					break;
				}
			}
		}

	if (MAX_MSG_NUM == msg_id)
		{
			for(i = 1;i<j;i++)
				{
					if(gApp_msg_list[i].msg == NULL)
					{
						msg_id = i;
						break;
					}
				}
		}

	if(msg_id == MAX_MSG_NUM)//??????ǰҪ???͵???Ϣ
	{
		return -1;
	}

	j = msg_id;

	gApp_msg_list[msg_id].msg = GxBus_MessageNew(msg_type);
	if (NULL == gApp_msg_list[msg_id].msg)
		{
			return -1;
		}

	switch(msg_type)
	{
		case GXMSG_SI_SUBTABLE_CREATE:
			{
				GxMsgProperty_SiCreate *params_create = NULL;
				params_create = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_SiCreate);
				*params_create = *(GxMsgProperty_SiCreate*)param;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);

				if ((*params_create)->si_subtable_id == -1)
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					printf("GXMSG_SI_SUBTABLE_CREATE subtable_id failed:%x\n",(*params_create)->si_subtable_id);
					return -1;
				}
				printf("GXMSG_SI_SUBTABLE_CREATE subtable_id:%x\n",(*params_create)->si_subtable_id);

				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;

				return 0;
			}
			break;
		case GXMSG_SI_SUBTABLE_MODIFY:
			{
				GxMsgProperty_SiModify      *params = NULL;
				params = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_SiModify);
				*params =  *(GxMsgProperty_SiModify*)param;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SI_SUBTABLE_RELEASE:
			{
				GxMsgProperty_SiRelease *params = NULL;
				params = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_SiRelease);
				*params = *(GxMsgProperty_SiRelease*)param;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SI_SUBTABLE_START:
			{
				GxMsgProperty_SiStart  *params_start = NULL;
				params_start = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_SiStart);
				printf("GXMSG_SI_SUBTABLE_START :%x\n",*(GxMsgProperty_SiStart*)param);
				*params_start = *(GxMsgProperty_SiStart*)param;
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SI_SUBTABLE_STOP:
			{
				GxMsgProperty_SiStop  *params_stop = NULL;
				params_stop = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_SiStop);
				*params_stop = *(GxMsgProperty_SiStop*)param;
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_EPG_CREATE:
			{
				GxMsgProperty_EpgCreate* cfg_params = NULL;

				cfg_params = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_EpgCreate);
				memcpy(cfg_params,param,sizeof(GxMsgProperty_EpgCreate));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_EPG_NUM_GET:
			{
				GxMsgProperty_EpgNumGet* epg_num_get = NULL;
				int32_t time_zone = 0;
				time_zone = app_flash_get_config_timezone();
				epg_num_get = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_EpgNumGet);
				epg_num_get->time_zone = time_zone;
				epg_num_get->service_id = ((GxMsgProperty_EpgNumGet*) param)->service_id;
				epg_num_get->ts_id = ((GxMsgProperty_EpgNumGet*)param)->ts_id;
				epg_num_get->orig_network_id = ((GxMsgProperty_EpgNumGet*)param)->orig_network_id;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);

				gApp_msg_list[msg_id].param = (void*)epg_num_get;
				return msg_id;
			}
			break;
		case GXMSG_EPG_GET:
			{
				GxMsgProperty_EpgGet* epg_params = NULL;
				epg_params = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_EpgGet);
				epg_params->service_id =((GxMsgProperty_EpgGet*) param)->service_id;
				epg_params->ts_id = ((GxMsgProperty_EpgGet*)param)->ts_id;
				epg_params->get_event_pos = ((GxMsgProperty_EpgGet*)param)->get_event_pos;
				epg_params->epg_info = (GxEpgInfo *)GxCore_Malloc(((GxMsgProperty_EpgGet*) param)->epg_info_size);
				epg_params->epg_info_size =((GxMsgProperty_EpgGet*)  param)->epg_info_size;
				epg_params->want_event_count =((GxMsgProperty_EpgGet*)  param)->want_event_count;

				if (epg_params->epg_info == NULL)
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return -1;
				}
				memset(epg_params->epg_info,0,((GxMsgProperty_EpgGet*) param)->epg_info_size);

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);

				gApp_msg_list[msg_id].param = (void*)epg_params;

				return msg_id;
			}
			break;
		case GXMSG_EPG_NVOD_GET:
			{
				GxMsgProperty_EpgNvodGet   *nvod_get=NULL;
				nvod_get = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_EpgNvodGet);

				nvod_get->service_id = ((GxMsgProperty_EpgNvodGet*) param)->service_id;
				nvod_get->ts_id = ((GxMsgProperty_EpgNvodGet*)param)->ts_id;
				nvod_get->nvod_info = (GxNvodInfo *)GxCore_Malloc(((GxMsgProperty_EpgNvodGet*) param)->nvod_info_size);
				nvod_get->nvod_info_size = ((GxMsgProperty_EpgNvodGet*) param)->nvod_info_size;

				if (nvod_get->nvod_info == NULL)
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return -1;
				}
				memset(nvod_get->nvod_info,0,((GxMsgProperty_EpgNvodGet*) param)->nvod_info_size);

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);

				gApp_msg_list[msg_id].param = (void*)nvod_get;

				return msg_id;
			}
			break;
		case GXMSG_PLAYER_VIDEO_WINDOW:
			{
				GxMsgProperty_PlayerVideoWindow *window_ctrl =NULL;
				window_ctrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoWindow);
				memcpy(window_ctrl,param,sizeof(GxMsgProperty_PlayerVideoWindow));
				window_ctrl->player = PLAYER0;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				//				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_HIDE:
			{
				GxMsgProperty_PlayerVideoHide *VideoHide =NULL;
				VideoHide = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoHide);
				VideoHide->player = PLAYER0;
//				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_SHOW:
			{
				GxMsgProperty_PlayerVideoShow *VideoShow =NULL;
				VideoShow = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoShow);
				VideoShow->player = PLAYER0;
			//	GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;

		case GXMSG_PLAYER_PLAY:
			{
				//printf("#################GXMSG_PLAYER_PLAY\n");
				Lcn_State_t lcn_flag;
				GxBusPmDataProg prog_data = {0};
				GxMsgProperty_PlayerPlay *player_play;
				GxBusPmViewInfo sysinfo;
				player_t *pplay = NULL;
				uint32_t num= 0;
				uint8_t chSyncModeBuffer[20]={0};
				uint32_t   panel_prog_cur;
				int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef _DEBUG

				test_for_show_time_ms("app_send_msg:1264");
#endif


				pplay = (player_t*)(param);
#if DVB_PVR_FLAG
				if(1 == app_pvr_state_check(2,pplay->num))
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return 0;
				}
#endif
				if(pplay->param == 1)
				{
					player_play = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_PlayerPlay);
					memset(player_play->url,0,PLAYER_URL_LONG);//modify by sunil
#if defined(DVB_AD_TYPE_3H_FLAG)
					if (GxCore_FileExists("/home/gx/openpicture.bin") == 1)
					{
						memcpy(player_play->url,"/home/gx/openpicture.bin",PLAYER_URL_LONG);
					}
					else
#endif
					{
						memcpy(player_play->url,LOGO_PATH,PLAYER_URL_LONG);
					}

					player_play->player = PLAYER0;

					if ((0 == rect_video.width)||(0 == rect_video.height))
					{
						player_play->window.x = VIDEO_WINDOW_X;
						player_play->window.y = VIDEO_WINDOW_Y;
						player_play->window.width = VIDEO_WINDOW_W;
						player_play->window.height = VIDEO_WINDOW_H;
					}
					else
					{
						player_play->window.x = rect_video.x;
						player_play->window.y = rect_video.y;
						player_play->window.width = rect_video.width;
						player_play->window.height = rect_video.height;
						printf("x=%d y=%d width=%d height=%d\n",
								rect_video.x,rect_video.y,rect_video.width,rect_video.height);
					}
					printf("url=%s\n",player_play->url);
					GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					//					GxBus_MessageSend(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return 0;
				}

				num = GxBus_PmProgNumGet();

				if((0 == num) ||  (pplay->num >= num))
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return -1;
				}
				if (GXCORE_ERROR == GxBus_PmViewInfoGet(&sysinfo))
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return -1;
				}
				if(sysinfo.stream_type == GXBUS_PM_PROG_RADIO)
				{
					pplay->player = PLAYER3;
				}
				else
				{
					pplay->player = PLAYER0;
				}

				GxBus_PmProgGetByPos(pplay->num,1,&prog_data);

				/*
				 *??̨ǰ??????ECM PID
				 */
#ifdef CA_FLAG

				if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
				{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
					app_play_clear_msg_pop_type(MSG_POP_PROG_NEED_PAY);
#endif
				}	
#ifdef DVB_CA_TYPE_TR_FLAG
				if (dvb_ca_flag == DVB_CA_TYPE_TR  )
				{	// TRCA release by cas lib+++++++++
					app_tr_cas_ipp_mark_clear_check(prog_data.service_id);
				}
				else
				{
					app_cas_api_release_ecm_filter();
				}	
#else
				app_cas_api_release_ecm_filter();
#endif
#endif
				app_table_pmt_filter_close();

#ifdef DVB_AD_TYPE_MAIKE_FLAG
				if ((prog_data.tp_id != MoldTpId) &&(MoldTpId != 0xFFFF))
				{
					MoldTpId = prog_data.tp_id;
					stop_monitor_mad();
				}
#endif
				memset(url, 0, GX_PM_MAX_PROG_URL_SIZE);
				GxBus_PmProgUrlGet(&prog_data, url, GX_PM_MAX_PROG_URL_SIZE);

				//				player_play = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_PlayerPlay);

				sprintf((void*)chSyncModeBuffer, "&sync:%d", SYSC_MODE);
				strcat((void*)url, (void*)chSyncModeBuffer);
				switch(DVB_TS_SRC)
				{
					case 0:
						strcat((void*)url, "&tsid:0&dmxid:0");
						break;
					case 1:
						strcat((void*)url, "&tsid:1&dmxid:0");
						break;
					case 2:
						strcat((void*)url, "&tsid:2&dmxid:0");
						break;
					default:
						strcat((void*)url, "&tsid:0&dmxid:0");
						break;
				}
				printf("url=%s\n", url);
			extern void app_play_free_space(void);
			app_play_free_space();

#if ((DVB_THEME_TYPE == DVB_THEME_DTMB_HD)||(DVB_THEME_TYPE == DVB_THEME_DTMB_MINI_HD))&&(!defined(CA_FLAG))
						if(prog_data.scramble_flag == TRUE )
						{
							GxBusPmDataTP tp = {0};
							if(GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
							{
								// ???Ź㲥????
								app_play_show_logo();
							}
							else
							{
								//app_play_stop();
//								app_send_msg(GXMSG_PLAYER_STOP,PLAYER0);
								app_play_show_logo_for_tv();
							}
							GxBus_PmTpGetById(prog_data.tp_id, &tp);

							#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
									 app_search_lock_tp(tp.frequency, tp.tp_c.symbol_rate/1000, INVERSION_OFF, tp.tp_c.modulation-1, 0);
							#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
								app_search_lock_tp(tp.frequency, tp.tp_dtmb.symbol_rate/1000, INVERSION_OFF, tp.tp_dtmb.modulation-1, 0);
							#endif
						}
						else
						{
							
							play_status = TRUE;
							GxPlayer_MediaPlay(pplay->player,(const char*)url,0,0,NULL);						
							if(sysinfo.stream_type != GXBUS_PM_PROG_RADIO)
								{
									gShowAudioFalg = FALSE;
								}							
						}
#else		
#if 0
						extern uint8_t DIVI_SC_Get_State();

						if ((prog_data.scramble_flag == GXBUS_PM_PROG_BOOL_DISABLE )&& (DIVI_SC_Get_State()==0x00))
						{
							GxBusPmDataTP tp = {0};
							if(GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
							{
								// ???Ź㲥????
								app_play_show_logo();
							}
							GxBus_PmTpGetById(prog_data.tp_id, &tp);
						printf("DIVI_GetProductOneIs()=%d",DIVI_GetProductOneIs());
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
							app_search_lock_tp(tp.frequency, tp.tp_c.symbol_rate/1000, INVERSION_OFF, tp.tp_c.modulation-1, 0);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
							app_search_lock_tp(tp.frequency, tp.tp_dtmb.symbol_rate/1000, INVERSION_OFF, tp.tp_dtmb.modulation-1, 0);
#endif
						}
						else

#endif
						{
							play_status = TRUE;
							GxPlayer_MediaPlay(pplay->player,(const char*)url,0,0,NULL);						
							if(sysinfo.stream_type != GXBUS_PM_PROG_RADIO)
							{
								gShowAudioFalg = FALSE;
							}
						}
#endif


#ifdef _DEBUG

						test_for_show_time_ms("app_send_msg:1392");
#endif

						GxBus_MessageFree(gApp_msg_list[msg_id].msg);


						gApp_msg_list[msg_id].msg = NULL;
						gApp_msg_list[msg_id].param = NULL;

						app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);

#ifndef CA_FLAG
						if(TRUE == prog_data.scramble_flag)
						{
							app_play_set_msg_pop_type(MSG_POP_PROG_SCRAMBLE);
						}
						else
						{
							app_play_clear_msg_pop_type_flag(MSG_POP_PROG_SCRAMBLE);
						}
#endif

						if (TRUE == prog_data.scramble_flag)
						{
							prog_scramble_flag = 1;
						}
						else
						{
							prog_scramble_flag = 0;
						}

#ifdef DVB_CA_TYPE_TR_FLAG
				if (dvb_ca_flag == DVB_CA_TYPE_TR  )
				{
					if (prog_data.tp_id != oldTpId)
					{
						oldTpId = prog_data.tp_id;
						MC_MNGR_PostChanChangeState(prog_data.service_id, 0);
					}
					else
					{
						MC_MNGR_PostChanChangeState(prog_data.service_id, 1);
					}
				}
#endif
				if ((0x1FFF != prog_data.pmt_pid)&&(0!=prog_data.pmt_pid))
				{
#ifdef CA_FLAG

					app_play_start_ecm_filter(prog_data);
#endif

#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
					if(ABV_TRUE == ABV_CASTB_NotifySwitchChannel(prog_data.pmt_pid,
								prog_data.service_id,
								prog_data.cur_audio_pid,
								prog_data.video_pid))
					{
						printf("-------ABV_CASTB_NotifySwitchChannel return error!!!");
					}
#endif

#ifdef SHOW_ECM_GOT_TIME
					extern GxTime starttime;
					GxTime nowtime={0};
					GxCore_GetTickTime(&nowtime);
					printf("\n start filter new program filter pmt current time >>>>>[]->%d MS<<<<<\n\n",(nowtime.seconds*1000+nowtime.microsecs/1000));
					starttime.seconds = nowtime.seconds;
					starttime.microsecs = nowtime.microsecs;
#endif

#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
					app_table_pmt_filter_open(prog_data.pmt_pid,prog_data.service_id,0,TRUE);
#endif
#ifdef DVB_AD_TYPE_DS_FLAG41

							app_ds_ad_sendprogram(prog_data.original_id,prog_data.ts_id,prog_data.service_id);
#endif
					printf("************************sent end***************************\n");
				}

						/*ǰ??????ʾ??Ŀ?Ż??߼?Ƶ?��?*/
						lcn_flag = app_flash_get_config_lcn_flag();
						if(LCN_STATE_OFF == lcn_flag)
						{
							panel_prog_cur = pplay->num+1;
							app_panel_show(PANEL_DATA,&(panel_prog_cur));
						}
						else
						{
							panel_prog_cur = prog_data.pos;
							app_panel_show(PANEL_DATA,&(panel_prog_cur));
						}

#if MEDIA_SUBTITLE_SUPPORT
						app_pmt_ttx_subt_clear();
						app_subt_pause();
#endif
						return 0;
			}
			break;
		case GXMSG_PLAYER_STOP:
			{
				//				GxMsgProperty_PlayerStop *player_stop;
				/*
				 * ֹͣ???Ž?Ŀ??????ECM PID
				 */
				//printf("#################GXMSG_PLAYER_STOP\n");
#ifdef CA_FLAG
				app_cas_api_release_ecm_filter();
#endif
				app_table_pmt_filter_close();
				GxPlayer_MediaStop((char*)param);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
#if 0
				player_stop = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_PlayerStop);
				player_stop->player = (char*)param;
				//				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//				GxBus_MessageFree(gApp_msg_list[msg_id].msg);

				printf("#####################stop %s################\n",player_stop->player);
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
#endif
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_AUTO_ADAPT:
			{
				GxMsgProperty_PlayerVideoAutoAdapt *player_adapt;

				player_adapt = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_PlayerVideoAutoAdapt);

				*player_adapt = *(GxMsgProperty_PlayerVideoAutoAdapt*)param;
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_MUTE://?Ѳ?
			{

				GxMsgProperty_PlayerAudioMute* pAudioMuteCtrl=NULL;
				GxMsgProperty_PlayerAudioMute nMute = 0;

				pAudioMuteCtrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioMute);
				nMute = *(unsigned int*)param;
				memcpy((void*)pAudioMuteCtrl, (void*)&nMute,sizeof(GxMsgProperty_PlayerAudioMute));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);

				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;

				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_AC3_MODE://?Ѳ?
			{

				GxMsgProperty_PlayerAudioAC3Mode* pAudioAc3BypassCtrl=NULL;

				pAudioAc3BypassCtrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioAC3Mode);
				memcpy((void*)pAudioAc3BypassCtrl, param,sizeof(GxMsgProperty_PlayerAudioAC3Mode));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);

				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;

				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_MODE_CONFIG://?Ѳ?
			{
				GxMsgProperty_PlayerAudioModeConfig* p = NULL;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioModeConfig);
				memcpy((void*)p,param,sizeof(GxMsgProperty_PlayerAudioModeConfig));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_INTERFACE://?Ѳ?
			{
				GxMsgProperty_PlayerAudioInterface* p = NULL;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioInterface);
				memcpy((void*)p,param,sizeof(GxMsgProperty_PlayerAudioInterface));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_VOLUME://?Ѳ?
			{
				GxMsgProperty_PlayerAudioVolume* pAudioVolumeCtrl=NULL;
				pAudioVolumeCtrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioVolume);
				memcpy((void*)pAudioVolumeCtrl,param,sizeof(GxMsgProperty_PlayerAudioVolume));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_AUDIO_TRACK://?Ѳ?
			{
				GxMsgProperty_PlayerAudioTrack* pAudioTrack=NULL;
				pAudioTrack = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerAudioTrack);
				memcpy((void*)pAudioTrack,param,sizeof(GxMsgProperty_PlayerAudioTrack));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				//GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_COLOR:
			{
				GxMsgProperty_PlayerVideoColor* pVodeoColor = NULL;
				pVodeoColor = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoColor);
				memcpy((void*)pVodeoColor,param,sizeof(GxMsgProperty_PlayerVideoColor));

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_FREEZE_FRAME_SWITCH:
			{
				GxMsgProperty_PlayerFreezeFrameSwitch* pSwitchEffect = NULL;
				pSwitchEffect = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerFreezeFrameSwitch);
				memcpy((void*)pSwitchEffect,param,sizeof(GxMsgProperty_PlayerFreezeFrameSwitch));

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;

		case GXMSG_PLAYER_VIDEO_INTERFACE:
			{
				GxMsgProperty_PlayerVideoInterface* pPtr = NULL;
				pPtr = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoInterface);
				memcpy((void*)pPtr,param,sizeof(GxMsgProperty_PlayerVideoInterface));

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_DISPLAY_SCREEN:
			{
				GxMsgProperty_PlayerDisplayScreen* pPtr = NULL;
				pPtr = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerDisplayScreen);
				memcpy((void*)pPtr,param,sizeof(GxMsgProperty_PlayerDisplayScreen));

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SCAN_SAT_START:
			{
				GxMsgProperty_ScanSatStart *sat_scan;

				sat_scan = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_ScanSatStart);

				memcpy(sat_scan,(GxMsgProperty_ScanSatStart *)param,sizeof(GxMsgProperty_ScanSatStart));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SCAN_T_START:
			{
				GxMsgProperty_ScanTStart *sat_scan;


				sat_scan = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_ScanTStart);

				memcpy(sat_scan,(GxMsgProperty_ScanTStart *)param,sizeof(GxMsgProperty_ScanTStart));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SCAN_CABLE_START://??ʼ??????Ŀ??Ϣ
			{
				GxMsgProperty_ScanCableStart *c_scan;


				c_scan = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_ScanCableStart);

				memcpy(c_scan,(GxMsgProperty_ScanTStart *)param,sizeof(GxMsgProperty_ScanCableStart));

				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SCAN_DTMB_START:
			{
				GxMsgProperty_ScanDtmbStart *dtmb_scan;
				dtmb_scan = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_ScanDtmbStart);
				memcpy(dtmb_scan,(GxMsgProperty_ScanDtmbStart *)param,sizeof(GxMsgProperty_ScanDtmbStart));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_BACKUP:
			{
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SAVE:
		case GXMSG_EPG_CLEAN:
		case GXMSG_EPG_RELEASE:
			{
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_EXTRA_SYNC_TIME:
			{
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_NOT_SAVE:
			{
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_SEARCH_SCAN_STOP:
			{
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_BOOK_GET:
			{
				GxBookGet* pBookGet;
				GxMsgProperty_BookGet *p;
				pBookGet = (GxBookGet*)GxCore_Malloc(sizeof(GxBookGet));
				if(pBookGet == NULL)
				{
					GxBus_MessageFree(gApp_msg_list[msg_id].msg);
					gApp_msg_list[msg_id].msg = NULL;
					gApp_msg_list[msg_id].param = NULL;
					return  -1;
				}
				memset(pBookGet,0,sizeof(GxBookGet));
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_BookGet);
				*p=pBookGet;
				pBookGet->book_type = *(GxBookType*)param;

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);

				gApp_msg_list[msg_id].param = (void*)pBookGet;
				return msg_id;
			}
			break;

		case GXMSG_BOOK_CREATE:
			{
				GxBook book;
				GxMsgProperty_BookCreate* p;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_BookCreate);
				*p = &book;
				memcpy(&book,(GxBook *)param,sizeof(GxBook));

				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_BOOK_DESTROY:
			{
				static GxBook book;
				GxMsgProperty_BookDestroy* p;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_BookDestroy);
				*p = &book;
				memcpy(&book,(GxBook *)param,sizeof(GxBook));

				//				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				/*˯??100 ms,ȷ???޸?ԤԼ?ɹ???????Ƶ???޸?????*/
				//				GxCore_ThreadDelay(200);

				return 0;
			}
			break;
		case GXMSG_BOOK_MODIFY:
			{
				static GxBook book;
				GxMsgProperty_BookModify* p;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_BookModify);
				*p = &book;
				memcpy(&book,(GxBook *)param,sizeof(GxBook));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				//				GxBus_MessageSend(gApp_msg_list[msg_id].msg);

				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				/*˯??100 ms,ȷ???޸?ԤԼ?ɹ???????Ƶ???޸??쳣*/
				//				GxCore_ThreadDelay(200);

				return 0;
			}
			break;
		case GXMSG_BOOK_START:
		case GXMSG_BOOK_STOP:
			{
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_BOOK_RESET:
			{
				/*ɾ??????ԤԼ*/
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_ASPECT:
			{
				GxMsgProperty_PlayerVideoAspect* Aspect=NULL;
				Aspect = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoAspect);
				memcpy((void*)Aspect,param,sizeof(GxMsgProperty_PlayerVideoAspect));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_PLAYER_VIDEO_MODE_CONFIG:
			{
				GxMsgProperty_PlayerVideoModeConfig* VideoMode=NULL;
				VideoMode = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerVideoModeConfig);
				memcpy((void*)VideoMode,param,sizeof(GxMsgProperty_PlayerVideoModeConfig));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
			//huangbc
#if DVB_PVR_FLAG
		case GXMSG_PLAYER_RECORD:
			{
				GxMsgProperty_PlayerRecord *PlayerRecord = NULL;
				PlayerRecord = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerRecord);
				memcpy((void*)PlayerRecord, param,sizeof(GxMsgProperty_PlayerRecord));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		case GXMSG_PLAYER_TIME_SHIFT:
			{
				GxMsgProperty_PlayerTimeShift *PlayerTimeShift = NULL;
				PlayerTimeShift = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerTimeShift);
				memcpy((void*)PlayerTimeShift, param,sizeof(GxMsgProperty_PlayerTimeShift));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		case GXMSG_PLAYER_PAUSE:
			{
				GxMsgProperty_PlayerPause *PlayerPause = NULL;
				PlayerPause = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerPause);
				memcpy((void*)PlayerPause, param,sizeof(GxMsgProperty_PlayerPause));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		case GXMSG_PLAYER_RESUME:
			{
				GxMsgProperty_PlayerResume *PlayerResume = NULL;
				PlayerResume = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerResume);
				memcpy((void*)PlayerResume, param,sizeof(GxMsgProperty_PlayerResume));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;

		case GXMSG_PLAYER_SEEK:
			{
				GxMsgProperty_PlayerSeek *PlayerSeek = NULL;
				PlayerSeek = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerSeek);
				memcpy((void*)PlayerSeek, param,sizeof(GxMsgProperty_PlayerSeek));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		case GXMSG_PLAYER_SPEED:
			{
				GxMsgProperty_PlayerSpeed *PlayerSpeed = NULL;
				PlayerSpeed = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerSpeed);
				memcpy((void*)PlayerSpeed, param,sizeof(GxMsgProperty_PlayerSpeed));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
#if 0
		case GXMSG_PLAYER_PVR_CONFIG:
			{
				int overlay = 1;
				int32_t size = 0;
				GxMsgProperty_PlayerPVRConfig* Config;
				Config = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_PlayerPVRConfig);

				size = *(int32_t*)param;
				printf("[app pvr setting]pvr size = %d\n", size);
				GxBus_ConfigGetInt(PLAYER_CONFIG_PVR_RECORD_EXIST_OVERLAY, &overlay, 1);

				Config->volume_sizemb = size;
				Config->exist_overlay = overlay;
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
#endif
#endif
#if MEDIA_SUBTITLE_SUPPORT
		case GXMSG_PLAYER_SUBTITLE_LOAD:
			{
				GxMsgProperty_PlayerSubtitleLoad *p_PlaySubt= NULL;
				printf("subt load\n");
				p_PlaySubt = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerSubtitleLoad);
				memcpy((char *)p_PlaySubt,param,sizeof(GxMsgProperty_PlayerSubtitleLoad));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				break;
			}
		case GXMSG_PLAYER_SUBTITLE_UNLOAD:
			{
				GxMsgProperty_PlayerSubtitleUnLoad *p_SubtUnload = NULL;
				printf("subt unload\n");
				p_SubtUnload = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerSubtitleUnLoad);
				memcpy((char *)p_SubtUnload,param,sizeof(GxMsgProperty_PlayerSubtitleUnLoad));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		case GXMSG_PLAYER_SUBTITLE_SWITCH:
			{
				GxMsgProperty_PlayerSubtitleSwitch *p_SubtSwitch = NULL;
				printf("subt switch\n");
				p_SubtSwitch = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_PlayerSubtitleSwitch);
				memcpy((char *)p_SubtSwitch,param,sizeof(GxMsgProperty_PlayerSubtitleSwitch));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
#endif
#ifdef USB_FLAG
		case GXMSG_UPDATE_OPEN:
			{
				GxMsgProperty_UpdateOpen* p;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_UpdateOpen);
				memcpy((char*)p, "gxupdate stream 0",strlen("gxupdate stream 0"));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_UPDATE_PROTOCOL_SELECT:
			{
				GxMsgProperty_UpdateProtocolSelect*  protocol_name;
				protocol_name = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_UpdateProtocolSelect);
				strcpy((char*)protocol_name, (char*)param);
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_UPDATE_PARTITION_SELECT:
			{
				GxMsgProperty_UpdatePartitionSelect*  partition_name;
				partition_name = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty_UpdatePartitionSelect);
				strcpy((char*)partition_name, (char*)param);
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_UPDATE_PROTOCOL_CTRL:
			{
				GxUpdate_IoCtrl*  pctrl;
				pctrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxUpdate_IoCtrl);
				memcpy((void*)pctrl, param,sizeof(GxUpdate_IoCtrl));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_UPDATE_PARTITION_CTRL:
			{
				GxUpdate_IoCtrl*  pctrl;
				pctrl = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxUpdate_IoCtrl);
				memcpy((void*)pctrl, param,sizeof(GxUpdate_IoCtrl));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_UPDATE_START:
		case GXMSG_UPDATE_STOP:
			{
				//			    GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
#endif
		case GXMSG_FRONTEND_STOP_MONITOR:
		case GXMSG_FRONTEND_START_MONITOR:
			{
				GxMsgProperty_FrontendMonitor * p=NULL;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg,GxMsgProperty_FrontendMonitor);
				memcpy((void*)p,param,sizeof(GxMsgProperty_FrontendMonitor));
				GxBus_MessageSendWait(gApp_msg_list[msg_id].msg);
				GxBus_MessageFree(gApp_msg_list[msg_id].msg);
				printf("GXMSG_FRONTEND_STOP_MONITOR ~~~~~~~~~~~~\n");
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
				return 0;
			}
			break;
		case GXMSG_CA_ON_EVENT:
			{
				GxMsgProperty0_OnEvent*	p;
				p = GxBus_GetMsgPropertyPtr(gApp_msg_list[msg_id].msg, GxMsgProperty0_OnEvent);
				memcpy((void*)p,param,sizeof(GxMsgProperty0_OnEvent));
				GxBus_MessageSend(gApp_msg_list[msg_id].msg);
				gApp_msg_list[msg_id].msg = NULL;
				gApp_msg_list[msg_id].param = NULL;
			}
			break;
		default:
			break;

	}
	//GxBus_MessageFree(gApp_msg_list[msg_id].msg);
	return 0;
}


/*
 * ???Ź???ͼƬ?ӿ?
 * ֧?ְ?X??Y????\???в???
 */
status_t app_play_ad_flash_file(ad_play_para *playpara)
{
	handle_t mntjpg;
	handle_t flashhandle;
	size_t   size;
	char* Buf= NULL;

	if((NULL == playpara->file_ddram_path)||(NULL == playpara->file_flash_path))
		return GXCORE_ERROR;


	if (GxCore_FileExists(playpara->file_flash_path) != 1)
		return GXCORE_ERROR;

	Buf = GxCore_Malloc(1024);
	if (NULL == Buf)
	{
		return GXCORE_ERROR;
	}
	memset(Buf,0,1024);

	/*
	 *     ??ȡ?????ļ????ݣ??????????ݶ?ȡ???ڴ???
	 */

	if (GxCore_FileExists(playpara->file_ddram_path) == 1)
	{
		GxCore_FileDelete(playpara->file_ddram_path);
	}

	flashhandle = GxCore_Open(playpara->file_flash_path, "r");
	if (flashhandle > 0)
	{
		GxCore_Seek(flashhandle, 0, GX_SEEK_SET);
		mntjpg = GxCore_Open(playpara->file_ddram_path, "a+");
		GxCore_Seek(mntjpg, 0, GX_SEEK_SET);
		size = GxCore_Read(flashhandle, Buf, 1, 1024);
		while((size>0)&&(GXCORE_ERROR!=size))
		{

			size = GxCore_Write(mntjpg, Buf, 1, size);
			size = GxCore_Read(flashhandle, Buf, 1, 1024);
		}

		GxCore_Close(flashhandle);
		GxCore_Close(mntjpg);
	}
	GxCore_Free(Buf);
	Buf = NULL;

	app_play_ad_ddram_file(playpara);

	return GXCORE_SUCCESS;
}

status_t app_play_ad_ddram_file(ad_play_para *playpara)
{
	GxMessage *msg;
	GxMsgProperty_PlayerPlay *player_play;

	if(NULL == playpara->file_ddram_path)
		return GXCORE_ERROR;


	if (GxCore_FileExists(playpara->file_ddram_path) != 1)
		return GXCORE_ERROR;

	if (1 == playpara->play_mode)
	{
		/*
		 * ??????ʾ
		 */
		msg = GxBus_MessageNew(GXMSG_PLAYER_PLAY);
		if (NULL == msg)
			return GXCORE_ERROR;

		player_play = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerPlay);
		if (NULL == player_play)
			return GXCORE_ERROR;

		player_play->player = "player_pic";//PMP_PLAYER_PIC;
		strcpy(player_play->url, playpara->file_ddram_path);
		player_play->start = 0;

		GxBus_MessageSend(msg);

	}
	else
		if (0 == playpara->play_mode)
		{
			/*
			 * ??X??Yλ????ʾ
			 */
			GUI_SetInterface("image_enable", NULL);
			GUI_SetInterface("image_top", NULL);
			GDI_PlayImage_Cordinate(playpara->file_ddram_path,playpara->rectdest.x,playpara->rectdest.y);
			GUI_SetInterface("flush", NULL);
		}
		else
			if (2 == playpara->play_mode)
			{
				/*
				 * play ad with video
				 */
				if (2 == playpara->type)
				{
					/*
					 * scroll play
					 */
					advertisement_scroll_play(playpara,5,0, TRUE);
				}
				else
				{
					/*
					 * normal play
					 */
					advertisement_play(playpara,5,0, TRUE);
				}
			}

	return GXCORE_SUCCESS;
}

/*
 * ?رչ???ͼƬ?ӿ?
 */
status_t app_play_stop_ad(ad_play_para playpara)
{
	GxMessage *msg;
	GxMsgProperty_PlayerStop *stop;

	if (1 == playpara.play_mode)
	{
		/*
		 * ??????ʾ
		 */
		msg = GxBus_MessageNew(GXMSG_PLAYER_STOP);
		if (NULL == msg)
			return GXCORE_ERROR;

		stop = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerStop);
		if (NULL == stop)
			return GXCORE_ERROR;
		GUI_SetInterface("clear_image", NULL);

		stop->player = "player_pic";//PMP_PLAYER_PIC;

		GxBus_MessageSend(msg);
		//			GUI_SetInterface("clear_image", NULL);
		//			GUI_SetInterface("video_top", NULL);
	}
	else
		if (0 == playpara.play_mode)
		{
			/*
			 * ??X??Yλ????ʾ
			 */
			GDI_StopImage(playpara.file_ddram_path);
			GUI_SetInterface("clear_image", NULL);
			GUI_SetInterface("video_top", NULL);
		}
		else
			if (2 == playpara.play_mode)
			{
				/*
				 * stop ad
				 */
				advertisement_clear_frame(playpara.rectdest);
				//				advertisement_hide();
			}

	return GXCORE_SUCCESS;

}

/*
 * ?????Բ???????Ӧ?ֱ??ʱ仯??????flash?ֱ??ʱ???
 */
int32_t app_play_update_flash_video_resolution(VideoOutputMode video_resution)
{
	int32_t VideoOutPut;
	VideoOutPut = app_flash_get_config_video_hdmi_mode();
	if (VideoOutPut != video_resution)
	{
		app_flash_save_config_video_hdmi_mode(video_resution);
	}

	return 0;
}
/*
 * ????rca1
 */
int32_t app_play_set_rca1_mode(int32_t VideoResolution)
{
	GxMsgProperty_PlayerVideoModeConfig VideoMode;
	VideoMode.mode = VideoResolution;
	switch(DVB_DEFINITION_TYPE)
	{
		case HD_DEFINITION:
			VideoMode.interface = VIDEO_OUTPUT_RCA;
			break;
		case SD_DEFINITION:
			VideoMode.interface = VIDEO_OUTPUT_RCA;		
			break;
	}

	//VideoMode.config.scart.screen = VIDEO_OUTPUT_SCREEN_4X3;
	//VideoMode.config.scart.source = VIDEO_OUTPUT_SOURCE_CVBS;
	app_send_msg(GXMSG_PLAYER_VIDEO_MODE_CONFIG, &VideoMode);
	return 0;
}


/*
 * ????hdmi??Ƶ?ֱ???
 */
int32_t app_play_set_hdmi_mode(int32_t VideoResolution)
{
	GxMsgProperty_PlayerVideoModeConfig VideoMode;
	VideoMode.mode = VideoResolution;
	switch(DVB_DEFINITION_TYPE)
	{
		case HD_DEFINITION:
			VideoMode.interface = VIDEO_OUTPUT_HDMI;
			break;
		case SD_DEFINITION:
			VideoMode.interface = VIDEO_OUTPUT_RCA;		
			break;
	}

	//VideoMode.config.scart.screen = VIDEO_OUTPUT_SCREEN_4X3;
	//VideoMode.config.scart.source = VIDEO_OUTPUT_SOURCE_CVBS;
	app_send_msg(GXMSG_PLAYER_VIDEO_MODE_CONFIG, &VideoMode);

	return 0;
}


/*
 * ???ÿ??߱?4X3\16X9
 */
int32_t app_play_set_video_display_screen(	Video_Display_Screen_t n4To3_16To9)
{
	GxMsgProperty_PlayerDisplayScreen DisplayScreen={0};
	switch(n4To3_16To9)
	{
		case VIDEO_DISPLAY_SCREEN_4X3:
			//DisplayScreen.aspect = DISPLAY_SCREEN_4X3;
			DisplayScreen.aspect = /*DISPLAY_SCREEN_4X3*/VIDEO_DISPLAY_SCREEN_4X3;
			break;
		case VIDEO_DISPLAY_SCREEN_16X9:
			//DisplayScreen.aspect = DISPLAY_SCREEN_16X9;
			DisplayScreen.aspect = /*DISPLAY_SCREEN_16X9*/VIDEO_DISPLAY_SCREEN_16X9;
			break;
		default:
			break;
	}
	DisplayScreen.xres = VIDEO_WINDOW_W;
	DisplayScreen.yres = VIDEO_WINDOW_H;
	app_send_msg(GXMSG_PLAYER_DISPLAY_SCREEN, &DisplayScreen);
	return 0;

}


/*
 * ??????Ƶ???߱?ģʽ
 */
int32_t app_play_set_video_aspect(VideoAspect nAspect)
{
	int32_t videoAspect = nAspect;
	switch(nAspect)
	{
		case 0:
			videoAspect = ASPECT_NORMAL;
			break;
		case 1:
			videoAspect = ASPECT_PAN_SCAN;
			break;
		case 2:
			videoAspect = ASPECT_LETTER_BOX;
			break;
		default:
			videoAspect = ASPECT_NORMAL;
			break;
	}
	app_send_msg(GXMSG_PLAYER_VIDEO_ASPECT, &videoAspect);
	return 0;
}

int32_t app_play_set_channel_switch(int32_t channel_switch)
{
	GxMsgProperty_PlayerFreezeFrameSwitch SwitchEffectPara;

	SwitchEffectPara = channel_switch;
	app_send_msg(GXMSG_PLAYER_FREEZE_FRAME_SWITCH, &SwitchEffectPara);
	return 0;
}

/*
 * ????????
 */
void app_play_set_audio_track(int audio_track)
{
	static int32_t track_pre= 0xff;
	if (track_pre != audio_track)
	{
		track_pre = audio_track;
		app_send_msg(GXMSG_PLAYER_AUDIO_TRACK,(void*)&audio_track);
	}

	return ;
}

/*
 * ??????��
 */
void app_play_set_volumn(int32_t Volume)
{
	/*
	 * if Volume is 0, mute; else unmute
	 */
	static int32_t volume_pre= 0xff;
	int32_t Config;
	int32_t volumn_globle_flag = 0;

	volumn_globle_flag = app_flash_get_config_volumn_globle_flag();

	if (0 == Volume)
	{
		if (1 == volumn_globle_flag)
		{
			Config = app_flash_get_config_mute_flag();
			if(Config == 0)
			{
				Config = 1;
				app_flash_save_config_mute_flag(Config);
			}
		}

		//			if (0 == mute_flag )
		{
			Config = 1;
			app_play_set_mute(Config);
		}

		volume_pre = Volume;
	}
	else
	{
		if (1 == mute_flag )
		{
			Config = 0;
			if (1 == volumn_globle_flag)
			{
				app_flash_save_config_mute_flag(Config);
			}
			app_play_set_mute(Config);
			app_send_msg(GXMSG_PLAYER_AUDIO_VOLUME, &Volume);
			volume_pre = Volume;
		}
		else
			//if (volume_pre != Volume) /*???����?ʱû???????????? for jinya chengdu project*/
		{
			app_send_msg(GXMSG_PLAYER_AUDIO_VOLUME, &Volume);
			volume_pre = Volume;
		}
	}

	return;
}

uint8_t app_play_get_mute_flag(void)
{
	return mute_flag;
}


/*
* ???á?ȡ??????
*/
void app_play_set_mute(int32_t mute)
{
	if (0 == mute)
	{
		mute_flag = FALSE;
	}
	else
	{
		mute_flag = TRUE;
	}
	app_send_msg(GXMSG_PLAYER_AUDIO_MUTE, (void*)&mute);

	return;
}

/*
* AC3 BYPASS??Ƶ???����?????
*/
void app_play_set_audio_ac3_bypass_onoff(int32_t onoff)
{

	GxMsgProperty_PlayerAudioAC3Mode pAudioAc3Bypass ={0};
	pAudioAc3Bypass.mode = onoff;
	app_send_msg(GXMSG_PLAYER_AUDIO_AC3_MODE, (void*)&onoff);

	return;
}

/*
* ??????Ӣ??
*/
void app_play_set_osd_language(char* language)
{
	if (NULL == language)
		return ;

	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		if(strcmp(language,"English") == 0)
		{
			DSCA_SetSysLanguage(CA_LANGUAGE_ENGLISH);
		}
		else if(strcmp(language,"Chinese") == 0)
		{
			DSCA_SetSysLanguage(CA_LANGUAGE_CHINESE);
		}
#endif
	}
	GUI_SetInterface("osd_language", language);
	return;
}

/*
 * ????͸????
 */
void app_play_set_osd_trasn_level(uint32_t nTransLeve)
{
	uint32_t param = 0;
	param = 255*(100-nTransLeve)/100;
	if (param <10)
		param = 10;

	//	param = 30+(225*nTransLeve)/100;
	GUI_SetInterface("osd_alpha_global", (void*)&param);
	return ;
}

void app_play_set_videoColor_level(VideoColor color)
{
	switch(DVB_DEFINITION_TYPE)
		{
			case HD_DEFINITION:
				color.interface = VIDEO_OUTPUT_HDMI;
				app_send_msg(GXMSG_PLAYER_VIDEO_COLOR, (void*)&color);	
				color.interface = VIDEO_OUTPUT_RCA;
				app_send_msg(GXMSG_PLAYER_VIDEO_COLOR, (void*)&color);	

				break;
			case SD_DEFINITION:
				color.interface = VIDEO_OUTPUT_RCA;
				app_send_msg(GXMSG_PLAYER_VIDEO_COLOR, (void*)&color);
#if (DVB_CHIP_TYPE == GX_CHIP_3113C)||(DVB_CHIP_TYPE == GX_CHIP_3115)			
				if ((50 == color.brightness)
					&&(50 == color.saturation)
					&&(50 == color.contrast))
					{
							*(unsigned int*)0xa4804030 = 0x6a963f3f;
							*(unsigned int*)0xa4804028 = 0xa0248cf;
					}
#endif
				break;
		}

	return ;
}

uint8_t app_play_get_playing_para(play_scramble_para_t* playPara)
{
	if (NULL != playPara)
	{
		app_prog_play_para_lock();
		memcpy(playPara,&play_Para,sizeof(play_scramble_para_t));
		app_prog_play_para_unlock();
		return 0;
	}
	return 0;
}

uint8_t app_play_set_playing_para(play_scramble_para_t* playPara)
{
	if (NULL != playPara)
	{
		app_prog_play_para_lock();
		memcpy(&play_Para,playPara,sizeof(play_scramble_para_t));
		app_prog_play_para_unlock();
		return 0;
	}
	return 0;
}

uint8_t app_play_set_gpio_mute(void)
{

#if (GX_CHIP_3115 == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
		{
			case DVB_DEMOD_ATBM886X:
				app_panel_set_gpio_low(62);
				break;
			case DVB_DEMOD_GX1503:
				app_panel_set_gpio_low(62);
				break;
			default:
				break;
		}
#endif
#if (GX_CHIP_3113C == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
		{
			case DVB_DEMOD_ATBM886X:
				if ( 0 == strncmp("runde32m",DVB_CUSTOM,strlen("runde32m")))
					{
						/*
						* ddr1 ?????г???Ӳ??
						*/
						app_panel_set_gpio_low(62);	
					}
				else
					{
						/*
						* ????
						*/
						app_panel_set_gpio_low(62);	
					}
				break;
			case DVB_DEMOD_GX1503:
				break;
			default:
				break;
		}
#endif
#if (GX_CHIP_3201 == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_ATBM886X:
			break;
		case DVB_DEMOD_GX1503:
			app_panel_set_gpio_high(53);
			break;
		case DVB_DEMOD_GX1001:
			app_panel_set_gpio_high(37);
			break;
		default:
			break;
	}
#endif

#if (GX_CHIP_3113H == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_ATBM886X:
			app_panel_set_gpio_high(31);
			break;
		case DVB_DEMOD_GX1801:
			break;
		case DVB_DEMOD_GX1001:
			break;
		default:
			break;
	}	
#endif

#if (GX_CHIP_3201H == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_ATBM886X:
			break;
		case DVB_DEMOD_GX1801:
			app_panel_set_gpio_high(31);
			break;
		case DVB_DEMOD_GX1001:
			break;
		default:
			break;
	}	
#endif

	return 0;
}

/*
*  ??????ʼ?????þ?????·gpio????????
*  ??ͬchip\demodӲ??????gpio??????Ӧһ??(????gx3115+gx1503)
* ?ͻ??ض?Ӳ???޸?,?????ж??Ƿ??ض?Ӳ????????Ĭ?Ϲ???gpio
*/
uint8_t app_play_set_gpio_unmute(void)
{
#if (GX_CHIP_3115 == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
		{
			case DVB_DEMOD_ATBM886X:
				app_panel_set_gpio_high(62);
				break;
			case DVB_DEMOD_GX1503:
				app_panel_set_gpio_high(33);
				break;
			default:
				break;
		}
#endif
#if (GX_CHIP_3113C == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
		{
			case DVB_DEMOD_ATBM886X:
				if ( 0 == strncmp("runde32m",DVB_CUSTOM,strlen("runde32m")))
					{
						/*
						* ddr1 ?????г???Ӳ??
						*/
						app_panel_set_gpio_high(62);	
					}
				else
					{
						/*
						* ????
						*/
						app_panel_set_gpio_high(62);	
					}	
				break;
			case DVB_DEMOD_GX1503:
				break;
			default:
				break;
		}
#endif
#if (GX_CHIP_3201 == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
		{
			case DVB_DEMOD_ATBM886X:		
				break;
			case DVB_DEMOD_GX1503:
				app_panel_set_gpio_low(53);
				break;
			case DVB_DEMOD_GX1001:
				app_panel_set_gpio_low(37);				
				break;
			default:
				break;
		}
#endif


#if (GX_CHIP_3113H == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_ATBM886X:
			app_panel_set_gpio_low(31);
			break;
		case DVB_DEMOD_GX1801:
			break;
		case DVB_DEMOD_GX1001:
			break;
		default:
			break;
	}	
#endif

#if (GX_CHIP_3201H == DVB_CHIP_TYPE)
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_ATBM886X:
			break;
		case DVB_DEMOD_GX1801:
			app_panel_set_gpio_low(31);
			break;
		case DVB_DEMOD_GX1001:
			break;
		default:
			break;
	}	
#endif
	return 0;
}

static int gi_SignalStatus = -1;
int app_play_get_signal_status(void)
{
	return gi_SignalStatus;
}

void app_play_set_signal_status(int ri_SignalStatus)
{
	gi_SignalStatus = ri_SignalStatus;
}

static int gi_PopmsgType = 0;
int app_play_get_popmsg_status(void)
{
	return gi_PopmsgType;
}

void app_play_set_popmsg_status(int ri_Popmsg_Type)
{
	gi_PopmsgType = ri_Popmsg_Type;
}




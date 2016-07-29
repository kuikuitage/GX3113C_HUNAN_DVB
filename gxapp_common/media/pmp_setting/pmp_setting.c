#include "pmp_setting.h"
#include "gui_core.h"
#include "gxbus.h"
#include "gxavdev.h"
#include "gxmsg.h"
#include "play_pic.h"
#include "play_manage.h"
#include "app_common_media.h"
#include "app_common_panel.h"
#include "app_common_flash.h"
#include "app_common_play.h"

#define APP_Printf(...)      {printf("[%s]l-%d",__FUNCTION__,__LINE__); printf( __VA_ARGS__ );}
#define APP_Printf_Blue(...)	{printf("\033[036m");\
					printf(__VA_ARGS__);\
					printf("\033[0m");}

//#include "app.h"
//#include "full_screen.h"

#define PMPSET_ACTIVE					"pmpset_active_flag"
#define DO_NOT_USE_SAME_CONFIG
#define FULL_ASPECT

// 0-40 映射成0-100
#define AUDIO_MAP_40(audio)	(((audio)*5)>>1)

//3: 0-25 映射成0-75 4: 0-25 映射成0-100 
#define VOLUME_NUM (4)
#define AUDIO_MAP_25(audio)	((audio) * VOLUME_NUM)

// 0-50 to 0-75
 #define AUDIO_MAP_50(audio) ((audio)/2 + (audio)) 


#ifdef  DO_NOT_USE_SAME_CONFIG
pmpset_aspect_ratio RecordRATIO =  PMPSET_ASPECT_RATIO_FULL_SCREEN;// PMPSET_ASPECT_RATIO_AUTO;

status_t pmpset_exit(void)
{
	int RecordSetting  = 0;
	GxBus_ConfigGetInt(PMPSET_SAVE_ASPECT_RATIO, &RecordSetting, 0);

#if 0
	if(ASPECT_RAW_RATIO == RecordSetting)
	{
		RecordSetting = PMPSET_ASPECT_RATIO_AUTO;
	}
	else if(ASPECT_NORMAL == RecordSetting)
	{
		RecordSetting = PMPSET_ASPECT_RATIO_FULL_SCREEN;
	}
	else if(ASPECT_RAW_SIZE == RecordSetting)
	{
		RecordSetting = PMPSET_ASPECT_RATIO_ORIGINAL_SIZE;
	}
	else if(ASPECT_4X3_CUT == RecordSetting || ASPECT_4X3_PULL== RecordSetting)
	{
		RecordSetting = PMPSET_ASPECT_RATIO_4_3;
	}
	else if(ASPECT_16X9_CUT == RecordSetting || ASPECT_16X9_PULL == RecordSetting)
	{
		RecordSetting = PMPSET_ASPECT_RATIO_16_9;
	}
	pmpset_set_int(PMPSET_ASPECT_RATIO, RecordSetting);
#endif
	GxMessage *msg_aspect;	
	GxMsgProperty_PlayerVideoAspect *config_aspect ;
	msg_aspect = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_ASPECT);	
	APP_CHECK_P(msg_aspect, GXCORE_ERROR);
	config_aspect = GxBus_GetMsgPropertyPtr(msg_aspect, GxMsgProperty_PlayerVideoAspect);
	APP_CHECK_P(config_aspect, GXCORE_ERROR);

    	*config_aspect = RecordSetting;
	GxBus_MessageSendWait(msg_aspect);
	GxBus_MessageFree(msg_aspect);

	
	
	return 0;
}

#else
status_t pmpset_exit(void)
{
	return 0;
}

#endif
status_t pmpset_init(void)
{
	int32_t value = 0;

#ifdef  DO_NOT_USE_SAME_CONFIG
	//PMPSET_ASPECT_RATIO,
#ifdef FULL_ASPECT
	pmpset_set_int(PMPSET_ASPECT_RATIO, PMPSET_ASPECT_RATIO_FULL_SCREEN);//PMPSET_ASPECT_RATIO_AUTO
#else
	pmpset_set_int(PMPSET_ASPECT_RATIO, PMPSET_ASPECT_RATIO_AUTO);//PMPSET_ASPECT_RATIO_AUTO
#endif
#endif

	GxBus_ConfigGetInt(PMPSET_ACTIVE, &value, 0);
	if(0 == value)
	{
		pmpset_factory_default();
		return GXCORE_SUCCESS;
	}

	APP_Printf_Blue("\n[APP] pmpset\n");
	APP_Printf("---------------------------------------\n");
		
#ifndef PMP_ATTACH_DVB	
	/*GLOBAL*/
	//PMPSET_LANG,
	value = pmpset_get_int(PMPSET_LANG);
	pmpset_set_int(PMPSET_LANG, value);
	APP_Printf("PMPSET_LANG:			%d\n", value);

	//PMPSET_DISPLAY_SCREEN
	value = pmpset_get_int(PMPSET_DISPLAY_SCREEN);
	pmpset_set_int(PMPSET_DISPLAY_SCREEN, value);
	APP_Printf("PMPSET_DISPLAY_SCREEN:		%d\n", value);
	
	/*AV*/
	//PMPSET_VOLUME,
	value = pmpset_get_int(PMPSET_VOLUME);
	pmpset_set_int(PMPSET_VOLUME, value);
	APP_Printf("PMPSET_VOLUME:			%d\n", value);
		
	//PMPSET_OUTPUT_MODE,	
	value = pmpset_get_int(PMPSET_OUTPUT_MODE);
	pmpset_set_int(PMPSET_OUTPUT_MODE, value);
	APP_Printf("PMPSET_OUTPUT_MODE:		%d\n", value);
		
	//PMPSET_ASPECT_RATIO,
	value = pmpset_get_int(PMPSET_ASPECT_RATIO);
	pmpset_set_int(PMPSET_ASPECT_RATIO, value);
	APP_Printf("PMPSET_ASPECT_RATIO:		%d\n", value);
	
	//PMPSET_VIDEO_FORMAT,
	value = pmpset_get_int(PMPSET_VIDEO_FORMAT);
	pmpset_set_int(PMPSET_VIDEO_FORMAT, value);
	APP_Printf("PMPSET_VIDEO_FORMAT:		%d\n", value);
	
	//PMPSET_AUDIO_TRACK,
	value = pmpset_get_int(PMPSET_AUDIO_TRACK);
	pmpset_set_int(PMPSET_AUDIO_TRACK, value);
	APP_Printf("PMPSET_AUDIO_TRACK:		%d\n", value);

	//PMPSET_MUTE,
	value = pmpset_get_int(PMPSET_MUTE);
	pmpset_set_int(PMPSET_MUTE, value);
	APP_Printf("PMPSET_MUTE:			%d\n", value);

	/*MOVIE*/
	
	/*MUSIC*/

	/*PIC*/
		
	/*TEXT*/
	
	/*NETWORK*/


	//PMPSET_POWER_ON_PALY
	//TODO: 
#endif

	APP_Printf("---------------------------------------\n");

	return GXCORE_SUCCESS;	
}
					
status_t pmpset_factory_default(void)
{
	int32_t ret = 0;
	
	printf("[PMPSET] factory default\n");

	/*GLOBAL*/
	//PMPSET_LANG,
#ifndef PMP_ATTACH_DVB	
	pmpset_set_int(PMPSET_LANG, PMPSET_LANG_CHINESE);
#else
	//pmpset_set_int(PMPSET_LANG, PMPSET_LANG_ENGLISH);
#endif

	//PMPSET_DISPLAY_SCREEN

	switch(DVB_DEFINITION_TYPE)
	{
		case HD_DEFINITION:
			pmpset_set_int(PMPSET_DISPLAY_SCREEN, PMPSET_DISPLAY_SCREEN_1280_720);
			break;
		case SD_DEFINITION:
			pmpset_set_int(PMPSET_DISPLAY_SCREEN, PMPSET_DISPLAY_SCREEN_720_576);
			break;
	}

	//PMPSET_POWER_ON_PALY
	pmpset_set_int(PMPSET_POWER_ON_PALY, PMPSET_TONE_OFF);
	pmpset_set_int(PMPSET_SAVE_TAGS, PMPSET_TONE_ON);

	//PMPSET_MUTE
	pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
	
	/*MOVIE*/
	pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE);
	pmpset_set_int(PMPSET_MOVIE_SUBT_VISIBILITY, PMPSET_TONE_ON);
	
	/*MUSIC*/
	pmpset_set_int(PMPSET_MUSIC_PLAY_SEQUENCE, PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE);
	pmpset_set_int(PMPSET_MUSIC_VIEW_MODE, PMPSET_MUSIC_VIEW_MODE_SPECTRUM);

	/*PIC*/
	pmpset_set_int(PMPSET_PIC_SWITCH_DURATION, PMPSET_PIC_SWITCH_DURATION_3s);
	pmpset_set_int(PMPSET_PIC_SWITCH_MODE, PMPSET_PIC_SWITCH_MODE_DEFAULT);
	pmpset_set_int(PMPSET_PIC_PLAY_SEQUENCE, PMPSET_PIC_PLAY_SEQUENCE_SEQUENCE);
		
	/*TEXT*/
	pmpset_set_int(PMPSET_TEXT_ROLL_LINES, 1);
	pmpset_set_int(PMPSET_TEXT_AUTO_ROLL, PMPSET_TONE_OFF);
	/*NETWORK*/

#ifndef PMP_ATTACH_DVB
	/*AV*/
	//PMPSET_VOLUME,
	pmpset_set_int(PMPSET_VOLUME, 20);

	//PMPSET_OUTPUT_MODE,	
	pmpset_set_int(PMPSET_OUTPUT_MODE, PMPSET_OUTPUT_MODE_SCART);	
	
	//PMPSET_VIDEO_FORMAT,
	pmpset_set_int(PMPSET_VIDEO_FORMAT, PMPSET_VIDEO_FORMAT_AUTO);
	
	//PMPSET_ASPECT_RATIO,
	pmpset_set_int(PMPSET_ASPECT_RATIO, PMPSET_ASPECT_RATIO_AUTO);
	
	//PMPSET_AUDIO_TRACK,
	pmpset_set_int(PMPSET_AUDIO_TRACK, PMPSET_AUDIO_TRACK_STEREO);
#endif

	/*pmpset_active_flag*/
	ret = GxBus_ConfigSetInt(PMPSET_ACTIVE, 1);
	if(GXCONFIG_FAILURE == ret) return GXCORE_ERROR;

	pmp_init_tags();

	return GXCORE_SUCCESS;
}

/*value 0-25*/
status_t pmpset_send_volume(int value)
{
    uint32_t audio_vol = 0;
//    int32_t volume_scope = AUDIO_SCOPE;
    // player do sth
//    GxBus_ConfigGetInt(AUDIO_VOLUME_SCOPE, (int32_t*)(&volume_scope), AUDIO_SCOPE);
    if(app_flash_get_config_volumn_default_globle_flag() == app_flash_get_config_volumn_globle_flag())
    {
        audio_vol = AUDIO_MAP_25(value);
		app_flash_save_config_audio_volume(audio_vol);
		app_play_set_volumn(audio_vol);
//        app_system_vol_set(audio_vol);
    }
    return GXCORE_SUCCESS;
}

status_t pmpset_send_mute(int value)
{
	GxMessage *msg;
	GxMsgProperty_PlayerAudioMute *para;
	
	msg = GxBus_MessageNew(GXMSG_PLAYER_AUDIO_MUTE);
	APP_CHECK_P(msg, GXCORE_ERROR);
	para = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerAudioMute);
	APP_CHECK_P(para, GXCORE_ERROR);
	*para = value;			
	GxBus_MessageSend(msg);	
#ifdef PMP_ATTACH_DVB
	app_flash_save_config_mute_flag(value);
#else
	GxBus_ConfigSetInt(PMPSET_SAVE_MUTE, value);
#endif	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_track(int value)
{
	GxMessage *msg;
	GxMsgProperty_PlayerAudioTrack* audio_config;
	
	msg = GxBus_MessageNew(GXMSG_PLAYER_AUDIO_TRACK);
	APP_CHECK_P(msg, GXCORE_ERROR);
	audio_config = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerAudioTrack);
	APP_CHECK_P(audio_config, GXCORE_ERROR);
	
	if(PMPSET_AUDIO_TRACK_STEREO == value)
		*audio_config= AUDIO_TRACK_STEREO;
	else if(PMPSET_AUDIO_TRACK_LEFT == value)
		*audio_config = AUDIO_TRACK_LEFT;
	else if(PMPSET_AUDIO_TRACK_RIGHT == value)
		*audio_config = AUDIO_TRACK_RIGHT;
	GxBus_MessageSend(msg);
#if 1
		app_play_set_audio_track(*audio_config);
#else
	GxBus_ConfigSetInt(PMPSET_SAVE_AUDIO_TRACK, *audio_config);
#endif
	return GXCORE_SUCCESS;
}

status_t pmpset_send_output_mode(int value)
{
	GxMessage *msg_interface;	
	GxMsgProperty_PlayerVideoInterface *config_interface;
	msg_interface = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_INTERFACE);
	APP_CHECK_P(msg_interface, GXCORE_ERROR);
	config_interface = GxBus_GetMsgPropertyPtr(msg_interface, GxMsgProperty_PlayerVideoInterface);
	APP_CHECK_P(config_interface, GXCORE_ERROR);



	GxMessage *msg_video_mode;	
	GxMsgProperty_PlayerVideoModeConfig *config_video_mode;
	msg_video_mode = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_MODE_CONFIG);
	APP_CHECK_P(msg_video_mode, GXCORE_ERROR);
	config_video_mode = GxBus_GetMsgPropertyPtr(msg_video_mode, GxMsgProperty_PlayerVideoModeConfig);
	APP_CHECK_P(config_video_mode, GXCORE_ERROR);

	pmpset_video_format video_format = 0;
	
	if(PMPSET_OUTPUT_MODE_RCA ==value)
	{
		*config_interface = VIDEO_OUTPUT_RCA;
	
		config_video_mode->interface = VIDEO_OUTPUT_RCA;
		video_format = pmpset_get_int(PMPSET_VIDEO_FORMAT);
		if(video_format >= PMPSET_VIDEO_FORMAT_PAL && video_format <= PMPSET_VIDEO_FORMAT_NTSC_443)
		{
			config_video_mode->mode = video_format;
		}
		else
		{
			config_video_mode->mode = VIDEO_OUTPUT_PAL;
		}
	}
#if 0
	else if(PMPSET_OUTPUT_MODE_VGA ==value)
	{
		*config_interface = VIDEO_OUTPUT_VGA;

		config_video_mode->interface = VIDEO_OUTPUT_VGA;
		config_video_mode->mode = VIDEO_OUTPUT_VGA_480P;
	}
#endif
	else if(PMPSET_OUTPUT_MODE_YUV == value)
	{
		*config_interface = VIDEO_OUTPUT_YUV;

		config_video_mode->interface = VIDEO_OUTPUT_YUV;
		config_video_mode->mode = VIDEO_OUTPUT_YCBCR_480I;
	}
#if 0
	else if (PMPSET_OUTPUT_MODE_DVI == value)
	{
		*config_interface = VIDEO_OUTPUT_DVI;

		config_video_mode->interface = VIDEO_OUTPUT_DVI;
		// TODO: config_video_mode->mode = ;	
	}
#endif
	else if (PMPSET_OUTPUT_MODE_HDMI == value)
	{
			switch(DVB_DEFINITION_TYPE)
		{
			case HD_DEFINITION:
				*config_interface = VIDEO_OUTPUT_HDMI;
				config_video_mode->interface = VIDEO_OUTPUT_HDMI;
				break;
			case SD_DEFINITION:
			    *config_interface = VIDEO_OUTPUT_RCA;
				config_video_mode->interface = VIDEO_OUTPUT_RCA;
				break;
		}
#if 0
		config_video_mode->mode = VIDEO_OUTPUT_BT656_YC_8BITS;	
#endif
	}
	else if (PMPSET_OUTPUT_MODE_SCART == value)
	{
		*config_interface = VIDEO_OUTPUT_SCART;

		config_video_mode->interface = VIDEO_OUTPUT_SCART;
		video_format = pmpset_get_int(PMPSET_VIDEO_FORMAT);
		if(video_format >= PMPSET_VIDEO_FORMAT_PAL && video_format <= PMPSET_VIDEO_FORMAT_NTSC_443)
		{
			config_video_mode->mode = video_format;
		}
		else
		{
			config_video_mode->mode = VIDEO_OUTPUT_PAL;
		}
	}
	else if (PMPSET_OUTPUT_MODE_SVIDEO == value)
	{
		*config_interface = VIDEO_OUTPUT_SVIDEO;

		config_video_mode->interface = VIDEO_OUTPUT_SVIDEO;
		video_format = pmpset_get_int(PMPSET_VIDEO_FORMAT);
		if(video_format >= PMPSET_VIDEO_FORMAT_PAL && video_format <= PMPSET_VIDEO_FORMAT_NTSC_443)
		{
			config_video_mode->mode = video_format;
		}
		else
		{
			config_video_mode->mode = VIDEO_OUTPUT_PAL;
		}
	}
#if 0
	else if (PMPSET_OUTPUT_MODE_LCD == value)
	{
		*config_interface = VIDEO_OUTPUT_LCD;

		config_video_mode->interface = VIDEO_OUTPUT_LCD;
		config_video_mode->mode = VIDEO_OUTPUT_DIGITAL_RGB_720x480_0_255;	
	}
#endif

	GxBus_MessageSend(msg_interface);	
	GxBus_ConfigSetInt(PMPSET_SAVE_OUTPUT_MODE, *config_interface);
	
	GxBus_MessageSend(msg_video_mode);	
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_RESOLUTION_RCA, config_video_mode->mode);
	
	return GXCORE_SUCCESS;	
}

status_t pmpset_send_video_format(int value)
{
//	int32_t auto_adapt = 0;
	GxMessage *msg_auto_adapt;	
	GxMsgProperty_PlayerVideoAutoAdapt *config_auto_adapt;
	msg_auto_adapt = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_AUTO_ADAPT);
	APP_CHECK_P(msg_auto_adapt, GXCORE_ERROR);
	config_auto_adapt = GxBus_GetMsgPropertyPtr(msg_auto_adapt, GxMsgProperty_PlayerVideoAutoAdapt);
	APP_CHECK_P(config_auto_adapt, GXCORE_ERROR);

	pmpset_output_mode output_mode = pmpset_get_int(PMPSET_OUTPUT_MODE);
	if(PMPSET_VIDEO_FORMAT_AUTO == value)
	{
		config_auto_adapt->enable = 1;
		config_auto_adapt->pal = VIDEO_OUTPUT_PAL;
		config_auto_adapt->ntsc =VIDEO_OUTPUT_NTSC_M;
	
		GxBus_MessageSend(msg_auto_adapt);
		GxBus_ConfigSetInt(PMPSET_SAVE_VIDEO_AUTO_ADAPT, config_auto_adapt->enable);
	}
	else
	{
		GxMessage *msg_video_mode;	
		GxMsgProperty_PlayerVideoModeConfig *config_video_mode;
		msg_video_mode = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_MODE_CONFIG);
		APP_CHECK_P(msg_video_mode, GXCORE_ERROR);
		config_video_mode = GxBus_GetMsgPropertyPtr(msg_video_mode, GxMsgProperty_PlayerVideoModeConfig);
		APP_CHECK_P(config_video_mode, GXCORE_ERROR);


		config_auto_adapt->enable = 0;
		config_auto_adapt->ntsc =VIDEO_OUTPUT_NTSC_M;
		config_auto_adapt->pal = VIDEO_OUTPUT_PAL;
		GxBus_MessageSend(msg_auto_adapt);
		GxBus_ConfigSetInt(PMPSET_SAVE_VIDEO_AUTO_ADAPT, config_auto_adapt->enable);

		
		if(PMPSET_OUTPUT_MODE_RCA ==output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_RCA;
			config_video_mode->mode = value;	
		}
#if 0
		else if(PMPSET_OUTPUT_MODE_VGA ==output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_VGA;
			config_video_mode->mode = value;	
		}
#endif
		else if(PMPSET_OUTPUT_MODE_YUV == output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_YUV;
			config_video_mode->mode = value;	
		}
#if 0
		else if (PMPSET_OUTPUT_MODE_DVI == output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_DVI;
			config_video_mode->mode = value;	
		}
#endif
		else if (PMPSET_OUTPUT_MODE_HDMI == output_mode)
		{
			switch(DVB_DEFINITION_TYPE)
			{
				case HD_DEFINITION:
					config_video_mode->interface = VIDEO_OUTPUT_HDMI;
					break;
				case SD_DEFINITION:
					config_video_mode->interface = VIDEO_OUTPUT_RCA;
					break;
			}
			
			config_video_mode->mode = value;	
		}
		else if (PMPSET_OUTPUT_MODE_SCART == output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_SCART;
			config_video_mode->mode = value;	
		}
		else if (PMPSET_OUTPUT_MODE_SVIDEO == output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_SVIDEO;
			config_video_mode->mode = value;	
		}
#if 0
		else if (PMPSET_OUTPUT_MODE_LCD == output_mode)
		{
			config_video_mode->interface = VIDEO_OUTPUT_LCD;
			config_video_mode->mode = value;	
		}
#endif

		GxBus_MessageSend(msg_video_mode);
		GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_RESOLUTION_RCA, config_video_mode->mode);
	}

	
	return GXCORE_SUCCESS;	
}


status_t pmpset_send_ratio(int value)
{
#if 0
	GxMessage *msg_video_hide;	
	GxMsgProperty_PlayerVideoHide *config_video_hide;
	msg_video_hide = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_HIDE);	
	APP_CHECK_P(msg_video_hide, GXCORE_ERROR);
	config_video_hide = GxBus_GetMsgPropertyPtr(msg_video_hide, GxMsgProperty_PlayerVideoHide);
	APP_CHECK_P(config_video_hide, GXCORE_ERROR);
	config_video_hide->player = PMP_PLAYER_AV;


	GxMessage *msg_video_show;	
	GxMsgProperty_PlayerVideoShow *config_video_show;
	msg_video_show = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_SHOW);	
	APP_CHECK_P(msg_video_show, GXCORE_ERROR);
	config_video_show = GxBus_GetMsgPropertyPtr(msg_video_show, GxMsgProperty_PlayerVideoShow);
	APP_CHECK_P(config_video_show, GXCORE_ERROR);
	config_video_show->player = PMP_PLAYER_AV;
#endif

	GxMessage *msg_aspect;	
	GxMsgProperty_PlayerVideoAspect *config_aspect;
	msg_aspect = GxBus_MessageNew(GXMSG_PLAYER_VIDEO_ASPECT);	
	APP_CHECK_P(msg_aspect, GXCORE_ERROR);
	config_aspect = GxBus_GetMsgPropertyPtr(msg_aspect, GxMsgProperty_PlayerVideoAspect);
	APP_CHECK_P(config_aspect, GXCORE_ERROR);

	switch (value)
	{
		case PMPSET_ASPECT_RATIO_AUTO://原始比例
			*config_aspect=ASPECT_RAW_RATIO;// 
			break;
		case PMPSET_ASPECT_RATIO_FULL_SCREEN://全屏
			*config_aspect=ASPECT_NORMAL;
			break;
		case PMPSET_ASPECT_RATIO_ORIGINAL_SIZE:
			*config_aspect=ASPECT_RAW_SIZE;
			break;
		case PMPSET_ASPECT_RATIO_4_3:
			*config_aspect=ASPECT_4X3_CUT;
			break;
		case PMPSET_ASPECT_RATIO_16_9:
			*config_aspect=ASPECT_16X9_CUT;			
			break;
		default:
			break;
	}

#if 0
	GxBus_MessageSendWait(msg_video_hide);
	GxBus_MessageFree(msg_video_hide);
#endif

	//GxBus_MessageSendWait(msg_aspect);
	//GxBus_MessageFree(msg_aspect);
	GxBus_MessageSend(msg_aspect);

#if 0
	GxBus_MessageSendWait(msg_video_show);
	GxBus_MessageFree(msg_video_show);
#endif

#ifdef  DO_NOT_USE_SAME_CONFIG
	RecordRATIO = value;
#else
	GxBus_ConfigSetInt(PMPSET_SAVE_ASPECT_RATIO, *config_aspect);
#endif

	return GXCORE_SUCCESS;	
}


status_t pmpset_send_movie_play_sequence(int value)
{
	switch (value)
	{
		case PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE,PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE);
			break;
		
		case PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ONE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE,PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ONE);
			break;
			
		/*case PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ALL:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE,PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ALL);
			break;*/
			
		case PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE,PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE);
			break;
			
		/*case PMPSET_MOVIE_PLAY_SEQUENCE_RANDOM:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE,PMPSET_MOVIE_PLAY_SEQUENCE_RANDOM);
			break;*/
			
		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_music_play_sequence(int value)
{
	switch (value)
	{
		case PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE,PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE);
			break;
		
		case PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE,PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE);
			break;
			
		/*case PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ALL:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE,PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ALL);
			break;
			*/
		case PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE,PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE);
			break;
			
		/*case PMPSET_MUSIC_PLAY_SEQUENCE_RANDOM:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE,PMPSET_MUSIC_PLAY_SEQUENCE_RANDOM);
			break;*/
			
		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_music_view_mode(int value)
{
	switch (value)
	{
		case PMPSET_MUSIC_VIEW_MODE_ID3:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_VIEW_MODE,PMPSET_MUSIC_VIEW_MODE_ID3);
			break;
			
		case PMPSET_MUSIC_VIEW_MODE_LRC:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_VIEW_MODE,PMPSET_MUSIC_VIEW_MODE_LRC);
			break;

		case PMPSET_MUSIC_VIEW_MODE_SPECTRUM:
			GxBus_ConfigSetInt(PMPSET_SAVE_MUSIC_VIEW_MODE,PMPSET_MUSIC_VIEW_MODE_SPECTRUM);
			break;
			
		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_pic_switch_duration(int value)
{
	switch (value)
	{
		case PMPSET_PIC_SWITCH_DURATION_3s:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_DURATION,PMPSET_PIC_SWITCH_DURATION_3s);
			break;
			
		case PMPSET_PIC_SWITCH_DURATION_5s:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_DURATION,PMPSET_PIC_SWITCH_DURATION_5s);
			break;

		case PMPSET_PIC_SWITCH_DURATION_7s:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_DURATION,PMPSET_PIC_SWITCH_DURATION_7s);
			break;

		case PMPSET_PIC_SWITCH_DURATION_10s:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_DURATION,PMPSET_PIC_SWITCH_DURATION_10s);
			break;
			
		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_pic_switch_mode(int value)
{
	switch (value)
	{
		case PMPSET_PIC_SWITCH_MODE_DEFAULT:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_MODE,PMPSET_PIC_SWITCH_MODE_DEFAULT);
			break;
			
		case PMPSET_PIC_SWITCH_MODE_LR:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_MODE,PMPSET_PIC_SWITCH_MODE_LR);
			break;

		case PMPSET_PIC_SWITCH_MODE_RL:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_SWITCH_MODE,PMPSET_PIC_SWITCH_MODE_RL);
			break;
			
		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_pic_play_sequence(int value)
{
	switch (value)
	{
		case PMPSET_PIC_PLAY_SEQUENCE_SEQUENCE:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_PLAY_SEQUENCE,PMPSET_PIC_PLAY_SEQUENCE_SEQUENCE);
			break;
			
		case PMPSET_PIC_PLAY_SEQUENCE_RANDOM:
			GxBus_ConfigSetInt(PMPSET_SAVE_PIC_PLAY_SEQUENCE,PMPSET_PIC_PLAY_SEQUENCE_RANDOM);
			break;

		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_display_screen(int value)
{
	GxMessage *msg;
	GxMsgProperty_PlayerDisplayScreen* display_screen;
	
	msg = GxBus_MessageNew(GXMSG_PLAYER_DISPLAY_SCREEN);
	APP_CHECK_P(msg, GXCORE_ERROR);
	display_screen = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerDisplayScreen);
	APP_CHECK_P(display_screen, GXCORE_ERROR);
	
/*	
	switch (value)
	{
		case PMPSET_DISPLAY_SCREEN_720_576:
			display_screen->aspect = DISPLAY_SCREEN_4X3;
			display_screen->xres = 720;
			display_screen->yres = 576;
			break;
			
		case PMPSET_DISPLAY_SCREEN_1280_720:
			display_screen->aspect = DISPLAY_SCREEN_4X3;
			display_screen->xres = 1280;
			display_screen->yres = 720;
			break;

		default:
			break;
	}	
*/
	display_screen->aspect = DISPLAY_SCREEN_4X3;
	display_screen->xres = APP_XRES;
	display_screen->yres = APP_YRES;
	
	GxBus_MessageSend(msg);

	GxBus_ConfigSetInt(PMPSET_SAVE_DISPLAY_SCREEN,value);
	
	return GXCORE_SUCCESS;
}

status_t pmpset_send_save_tags(int value)
{
	switch (value)
	{
		case PMPSET_TONE_OFF:
			GxBus_ConfigSetInt(PMPSET_SAVE_SAVE_TAGS,PMPSET_TONE_OFF);
			break;
			
		case PMPSET_TONE_ON:
			GxBus_ConfigSetInt(PMPSET_SAVE_SAVE_TAGS,PMPSET_TONE_ON);
			break;

		default:
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t pmpset_set_int(pmpset_property property, int value)
{
	switch(property)
	{
		/*AV*/
		case PMPSET_VOLUME:
			pmpset_send_volume(value);
			break;
		case PMPSET_MUTE://zfz 20101014
			pmpset_send_mute(value);	
			break;
			
	
//#ifndef PMP_ATTACH_DVB
		/*GLOBAL*/
		case PMPSET_LANG:
			if(PMPSET_LANG_ENGLISH == value)
			{
				GxBus_ConfigSetInt(PMPSET_SAVE_LANG, PMPSET_LANG_ENGLISH);
				GUI_SetInterface("osd_language", "English");
			}
			else if(PMPSET_LANG_CHINESE == value)
			{
				GxBus_ConfigSetInt(PMPSET_SAVE_LANG, PMPSET_LANG_CHINESE);
				GUI_SetInterface("osd_language", "Chinese");	
			}
			break;

		case PMPSET_DISPLAY_SCREEN:
			pmpset_send_display_screen(value);
			break;
			
		case PMPSET_POWER_ON_PALY:
			break;
		case PMPSET_LAST_PLAY_PATH:
			break;
		case PMPSET_SAVE_TAGS:
			pmpset_send_save_tags(value);
			break;
		case PMPSET_FACTORY_DEFAULT:
			pmpset_factory_default();
			return GXCORE_SUCCESS;

		case PMPSET_OUTPUT_MODE:
			pmpset_send_output_mode(value);				
			break;
			
		case PMPSET_VIDEO_FORMAT:
			pmpset_send_video_format(value);
			break;

		case PMPSET_ASPECT_RATIO:
			pmpset_send_ratio(value);
			break;
			
		case PMPSET_AUDIO_TRACK:
			pmpset_send_track(value);						
			break;

		/*MOVIE*/
		case PMPSET_MOVIE_PLAY_SEQUENCE:
			pmpset_send_movie_play_sequence(value);
			break;
		case	PMPSET_MOVIE_SUBT_VISIBILITY:
			GxBus_ConfigSetInt(PMPSET_SAVE_MOVIE_SUBT,value);
			
		/*MUSIC*/
		case PMPSET_MUSIC_PLAY_SEQUENCE:
			pmpset_send_music_play_sequence(value);
			break;
		case PMPSET_MUSIC_VIEW_MODE:
			pmpset_send_music_view_mode(value);
			break;

		/*PIC*/
		case PMPSET_PIC_SWITCH_DURATION:
			pmpset_send_pic_switch_duration(value);
			break;
			
		case PMPSET_PIC_SWITCH_MODE:
			pmpset_send_pic_switch_mode(value);
			break;
			
		case PMPSET_PIC_PLAY_SEQUENCE:
			pmpset_send_pic_play_sequence(value);
			break;
		
		/*TEXT*/	
		case PMPSET_TEXT_ROLL_LINES:
			GxBus_ConfigSetInt(PMPSET_SAVE_TEXT_ROLL_LINES,value);
			break;
			
		case PMPSET_TEXT_AUTO_ROLL:
			GxBus_ConfigSetInt(PMPSET_SAVE_TEXT_AUTO_ROLL,value);
			break;
		/*NETWORK*/	
//#endif
		default:
			break;
	}	

	return GXCORE_SUCCESS;	
}

int32_t pmpset_get_int(pmpset_property property)
{
	int value_return = 0;	
	int value_get = 0;

	switch(property)
	{
		/*AV*/
		case PMPSET_VOLUME:
#if 1
		value_get = app_flash_get_config_audio_volume();
		/*if( app_volume_scope_status())
		{
			value_get = app_audio_volume_prog_get_mode();
		}
		else
		{
            value_get = app_system_vol_get();
		}*/
		value_return = value_get / VOLUME_NUM;

#else
			if(app_volume_scope_status())
			{
				value_get = app_audio_volume_prog_get_mode();
			}
			else
			{
				GxBus_ConfigGetInt(PMPSET_SAVE_VOLUME, &value_get, 0);
			}
			value_return = value_get / 3;
#endif			
			break;
		case PMPSET_MUTE://zfz 20101014
#ifdef PMP_ATTACH_DVB
			value_get = app_flash_get_config_audio_volume();
			if(value_get == 0)
			{
				value_return = 0;
			}
			else
			{
				value_return = app_flash_get_config_mute_flag();
			}
#else
			GxBus_ConfigGetInt(PMPSET_SAVE_MUTE, &value_return, 0);
#endif			
			break;
			
		/*GLOBAL*/
		case PMPSET_LANG:
			GxBus_ConfigGetInt(PMPSET_SAVE_LANG, &value_return, 0);
			break;
			
		case PMPSET_DISPLAY_SCREEN:
			GxBus_ConfigGetInt(PMPSET_SAVE_DISPLAY_SCREEN,&value_return, 0);
			break;
			
		case PMPSET_LAST_PLAY_PATH:
			break;		
			
		case PMPSET_SAVE_TAGS:
			GxBus_ConfigGetInt(PMPSET_SAVE_SAVE_TAGS, &value_return, 0);
			break;
			
		case PMPSET_FACTORY_DEFAULT:
			return GXCORE_SUCCESS;

		case PMPSET_OUTPUT_MODE:
			GxBus_ConfigGetInt(PMPSET_SAVE_OUTPUT_MODE, &value_get, 0);
			if(VIDEO_OUTPUT_RCA == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_RCA;
			}
#if 0
			else if(VIDEO_OUTPUT_VGA == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_VGA;
			}
#endif
			else if(VIDEO_OUTPUT_YUV == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_YUV;
			}
#if 0
			else if(VIDEO_OUTPUT_DVI == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_DVI;
			}
#endif
			else if(VIDEO_OUTPUT_HDMI == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_HDMI;
			}
			else if(VIDEO_OUTPUT_SCART == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_SCART;
			}
			else if(VIDEO_OUTPUT_SVIDEO == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_SVIDEO;
			}
#if 0
			else if(VIDEO_OUTPUT_LCD == value_get)
			{
				value_return = PMPSET_OUTPUT_MODE_LCD;
			}
#endif
			break;	

		case PMPSET_VIDEO_FORMAT:	
			GxBus_ConfigGetInt(PMPSET_SAVE_VIDEO_AUTO_ADAPT, &value_get, 0);
			if(1 == value_get)
			{
				value_return = PMPSET_VIDEO_FORMAT_AUTO;
			}			
			else
			{
				GxBus_ConfigGetInt(PLAYER_CONFIG_VIDEO_RESOLUTION_RCA, &value_return, 0);
			}
			break;

		case PMPSET_ASPECT_RATIO:
#ifdef  DO_NOT_USE_SAME_CONFIG
			value_return = RecordRATIO;
#else
			GxBus_ConfigGetInt(PMPSET_SAVE_ASPECT_RATIO, &value_get, 0);
			if(ASPECT_RAW_RATIO == value_get)
			{
				value_return = PMPSET_ASPECT_RATIO_AUTO;
			}
			else if(ASPECT_NORMAL == value_get)
			{
				value_return = PMPSET_ASPECT_RATIO_FULL_SCREEN;
			}
			else if(ASPECT_RAW_SIZE == value_get)
			{
				value_return = PMPSET_ASPECT_RATIO_ORIGINAL_SIZE;
			}
			else if(ASPECT_4X3_CUT == value_get || ASPECT_4X3_PULL== value_get)
			{
				value_return = PMPSET_ASPECT_RATIO_4_3;
			}
			else if(ASPECT_16X9_CUT == value_get || ASPECT_16X9_PULL == value_get)
			{
				value_return = PMPSET_ASPECT_RATIO_16_9;
			}			
#endif
						
			break;			
		case PMPSET_AUDIO_TRACK:
			#if 1
			value_get = app_flash_get_config_audio_track();
			#else
			GxBus_ConfigGetInt(PMPSET_SAVE_AUDIO_TRACK, &value_get, 0);
			#endif
			if(AUDIO_TRACK_STEREO==value_get)
			{
				value_return = PMPSET_AUDIO_TRACK_STEREO;
			}
			else if(AUDIO_TRACK_LEFT==value_get)
			{
				value_return = PMPSET_AUDIO_TRACK_LEFT;
			}
			else if(AUDIO_TRACK_RIGHT ==value_get)
			{
				value_return = PMPSET_AUDIO_TRACK_RIGHT;
			}
			break;

		/*MOVIE*/
		case PMPSET_MOVIE_PLAY_SEQUENCE:
			GxBus_ConfigGetInt(PMPSET_SAVE_MOVIE_PLAY_SEQUENCE, &value_return, 0);
			break;
		case	PMPSET_MOVIE_SUBT_VISIBILITY:
			GxBus_ConfigGetInt(PMPSET_SAVE_MOVIE_SUBT,&value_return, PMPSET_TONE_ON);
			
		/*MUSIC*/
		case PMPSET_MUSIC_PLAY_SEQUENCE:
			GxBus_ConfigGetInt(PMPSET_SAVE_MUSIC_PLAY_SEQUENCE, &value_return, 0);
			break;
			
		case PMPSET_MUSIC_VIEW_MODE:
			GxBus_ConfigGetInt(PMPSET_SAVE_MUSIC_VIEW_MODE, &value_return, 2);
			break;

		/*PIC*/
		case PMPSET_PIC_SWITCH_DURATION:
			GxBus_ConfigGetInt(PMPSET_SAVE_PIC_SWITCH_DURATION, &value_return, 0);
			break;
			
		case PMPSET_PIC_SWITCH_MODE:
			GxBus_ConfigGetInt(PMPSET_SAVE_PIC_SWITCH_MODE, &value_return, 0);			
			break;
			
		case PMPSET_PIC_PLAY_SEQUENCE:
			GxBus_ConfigGetInt(PMPSET_SAVE_PIC_PLAY_SEQUENCE, &value_return, 0);
			break;
		
		/*TEXT*/
		case PMPSET_TEXT_ROLL_LINES:
			GxBus_ConfigGetInt(PMPSET_SAVE_TEXT_ROLL_LINES,&value_return, 0);
			break;
			
		case PMPSET_TEXT_AUTO_ROLL:
			GxBus_ConfigGetInt(PMPSET_SAVE_TEXT_AUTO_ROLL,&value_return, 0);
			break;
		/*NETWORK*/	
		default:
			break;
	}		

	return value_return;
}
status_t pmpset_set_str(pmpset_property property, char* str)
{
	return GXCORE_SUCCESS;
}


status_t pmpset_get_str(const char* key, char** str, int str_len)
{
	return GXCORE_SUCCESS;
}


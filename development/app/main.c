/*****************************************************************************
*                          CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :   main.c
* Author    :   lijq
* Project   :   goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :   模块头文件
******************************************************************************
* Release History:
VERSION   Date              AUTHOR         Description
1.0      2010.04.12        lijq              creation
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui_core.h"
#include "gxcore.h"
#include "gxbus.h"
#include "gxservices.h"
#include "gui_key.h"
#include "app.h"
#include "gdi_core.h"

#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif

#ifdef _DEBUG

//#define SHOW_ECM_GOT_TIME//test the ecm got of each prog time base->pmt set [ecm set/ecm parse/cw set]
#ifdef SHOW_ECM_GOT_TIME
	GxTime starttime={0};
#endif

char show_time_count =0;
static GxTime starttime={0};
void test_for_show_time_starttime(void)
{
	show_time_count =0;
	GxCore_GetTickTime(&starttime);
}
void test_for_show_time_ms(char* msg)
{
	GxTime nowtime={0};
	GxCore_GetTickTime(&nowtime);
	
	if(show_time_count<10)
	{
		printf("\n>>>>>[%s]->%d MS<<<<<\n\n",msg,((nowtime.seconds*1000+nowtime.microsecs/1000) - (starttime.seconds*1000+starttime.microsecs/1000)));
		show_time_count++;
	}
}
#endif

//#define STACK_INFO
#ifdef STACK_INFO
#ifdef ECOS_OS
#include <cyg/kernel/kapi.h>
static void stack_check_thread(void *arg)
{
	struct gx_malloc_info *heap;
	extern struct gx_malloc_info *gx_malloc_info_get(void);
	while(1)
	{
		heap = (struct gx_malloc_info*)gx_malloc_info_get();
		printf("\n------------------------------------\n");
		printf("heap.heap_total = 0x%x\n",heap->heap_total);
		
		printf("heap.heap_blocks = 0x%x\n",heap->heap_blocks);
		printf("heap.heap_allocated = 0x%x\n",heap->heap_allocated);
		printf("heap.heap_free = 0x%x\n",heap->heap_free);
		printf("heap.heap_maxfree = 0x%x\n",heap->heap_maxfree);
		printf("\n------------------------------------\n");
		GxCore_ThreadDelay(1000);
	}
}
#endif
#endif


//主线程
void app_idle_thread(void* arg)
{

	status_t rel = GXCORE_ERROR;
	uint8_t service_num = 0;
	int32_t Config;
	int i;
	char* osd_language=NULL;
	Video_Display_Screen_t n4To3_16To9;
	int32_t nSwitchEffect;
	int32_t videoadapt;
	flash_config_default config_default = {0};
	GuiViewAppCallback app_cb;
	int32_t VideoResolution;
    uint8_t ret = 0;
	/*
	* 开机设置默认参数
	*/
	config_default.default_video_display_screen = VIDEO_DISPLAY_SCREEN_DV;
	config_default.default_video_hdmi_mode = VIDEO_HDMI_MODE_DV;
	config_default.default_video_aspect = VIDEO_ASPECT_DV;
	config_default.default_video_quiet_switch = VIDEO_QUIET_SWITCH_DV;
	config_default.default_video_auto_adapt = VIDEO_AUTO_ADAPT_DV;
	config_default.default_audio_volume = AUDIO_VOLUME_DV;
	config_default.default_audio_track = AUDIO_TRACK_DV;
	config_default.default_audio_audio_track = AUDIO_AUDIO_TRACK_DV;
	config_default.default_audio_ac3_bypass  = AUDIO_AC3_BYPASS_DV;
	config_default.default_osd_lang = OSD_LANG_DV;
	config_default.default_osd_trans = OSD_TRANS_DV;
	config_default.default_videocolor_brightness = VIDEOCOLOR_BRIGHTNESS_DV;
	config_default.default_videocolor_saturation = VIDEOCOLOR_SATURATION_DV;
	config_default.default_videocolor_contrast = VIDEOCOLOR_CONTRAST_DV;
	config_default.default_bar_time = BAR_TIME_DV;
	config_default.default_lcn = LCN_DV;
	config_default.default_password = PASSWORD_DV;
	config_default.default_timezone = TIMEZONE_DV;
	config_default.default_main_fre_nit_version = MAIN_FREQ_NITVERSION_DV;
	config_default.default_manual_fre = MANUSAL_SEARCH_FREQ_DV;
	config_default.default_manual_sym = MANUSAL_SEARCH_SYMBOL_DV;
	config_default.default_manual_qam = MANUSAL_SEARCH_QAM_DV;
	config_default.default_centre_fre = DVB_CENTER_FRE*1000;
	config_default.default_centre_sym = DVB_CENTER_SYMRATE;
	config_default.default_centre_qam = DVB_CENTER_QAM;
	config_default.default_manual_bandwidth= MANUSAL_SEARCH_BANDWIDTH_DV;
	config_default.default_dtmb_dvbc_switch= DTMB_DVBC_SWITCH_DV;
	config_default.default_password_flag = PASSWORD_FLAG_DV;
	config_default.default_mute_flag = MUTE_FLAG_DV;
	config_default.default_dtmb_flag = DTMB_MODE_DV;
	config_default.default_user_list = USER_LIST_DV;
	config_default.default_track_globle_flag = TRACK_GLOBLE_FLAG_DV;
	config_default.default_volumn_globle_flag = VOLUMN_GLOBLE_FLAG_DV;
	config_default.default_teltext_lang = TELTEXT_LANG_DV;
	config_default.default_subtitle_lang = SUBTITLE_LANG_DV;
	config_default.default_sort_prog_by_pat = SORT_PROG_BY_PAT_DV;
#ifdef CA_FLAG
	config_default.default_dvb_ca = DVB_CA_DV;
#endif
    //初始化config配置信息
    app_flash_set_default_config_para(config_default);
	VideoResolution = app_flash_get_config_video_hdmi_mode();


	GxPlayer_ModuleRegisterStreamDVB();
	GxPlayer_ModuleRegisterStreamFILE();
	GxPlayer_ModuleRegisterDemuxerLOGO();
	GxPlayer_ModuleRegisterDemuxerHWTS();
#if DVB_MEDIA_FLAG
	GxPlayer_ModuleRegisterDemuxerSWTS();
	GxPlayer_ModuleRegisterDemuxerMP3();
	GxPlayer_ModuleRegisterDemuxerMP4();
	GxPlayer_ModuleRegisterDemuxerAVI();
	GxPlayer_ModuleRegisterDemuxerMKV();
	GxPlayer_ModuleRegisterDemuxerFLV();
	GxPlayer_ModuleRegisterDemuxerAAC();
#if (!(DVB_THEME_TYPE == DVB_THEME_DTMB_HD))
	GDI_RegisterJPEG();//硬件解码支持
#endif
	GDI_RegisterJPEG_SOFT();
	GDI_RegisterPNG();
	GDI_RegisterGIF();
#endif

#if (0 == DVB_MEDIA_FLAG)
#ifdef AD_FLAG
	//不支持多媒体功能的小内存方案，广告jpg等支持
#if (!(DVB_THEME_TYPE == DVB_THEME_DTMB_HD))
	GDI_RegisterJPEG();//硬件解码支持
#endif
	GDI_RegisterJPEG_SOFT();
//	GDI_RegisterPNG();
//	GDI_RegisterGIF();
#endif
#endif




    /*GUI View服务相关*/
	app_cb.app_init = app_init;
	app_cb.app_msg_init = app_msg_init;
	app_cb.app_msg_destroy = app_msg_destroy;
	GxGuiViewRegisterApp(&app_cb);//gui view服务

/*
*     gx3201不支持gxloader中显示开机画面，应用初始化显示开机画面/广播背景
*/
	GxBus_ConfigSetInt(PLAYER_CONFIG_AUTOPLAY_PLAY_FLAG, 0);

	#if (DVB_DEFINITION_TYPE == SD_DEFINITION)
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_INTERFACE, VIDEO_OUTPUT_RCA);
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_RESOLUTION_RCA, VIDEO_OUTPUT_PAL);
	#endif

#ifdef DVB_AD_TYPE_BY_FLAG
	app_by_ads_show_av_logo(VideoResolution);
#elif defined (DVB_AD_TYPE_DS_FLAG)

    printf("show the ad pic or logo!\n\n\n");
    ret = app_ds_ad_show_av_logo(VideoResolution);
    if (ret == 0)
    {
    	//app_play_i_frame_logo(VideoResolution,LOGO_PATH);
		app_play_i_frame_logo(VideoResolution, LOGO_IFRAME_PATH);
    }
#elif defined DVB_AD_TYPE_MAIKE_FLAG
	app_porting_ads_flash_init(0x21000);
	app_ads_show_logo(VideoResolution);
#else
#if (0 == DVB_JPG_LOGO)
	app_play_i_frame_logo(VideoResolution,I_FRAME_PATH);
#endif
#endif 

	GxBus_PmDbaseInit(SAT_MAX_NUM,TP_MAX_NUM,SERVICE_MAX_NUM,NULL);

	Config = app_flash_get_config_audio_track();
	Config = app_flash_get_config_audio_volume();
	Config = app_flash_get_config_osd_trans();
	osd_language = app_flash_get_config_osd_language();
	n4To3_16To9 = app_flash_get_config_video_display_screen();
	nSwitchEffect = app_flash_get_config_video_quiet_switch();
	videoadapt = app_flash_get_config_video_auto_adapt();

	GxBus_ConfigSetInt(FRONTEND_CONFIG_NUM,1);


	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_XRES,VIDEO_WINDOW_W);
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_YRES,VIDEO_WINDOW_H);
	/*
	* fixed video out quality
	*/
#if 0
	GxBus_ConfigSet   (PLAYER_CONFIG_VIDEO_OUT0_CONFIG ,"0x681a11,0xf0a215,0x26404547,0x1f639102");
	GxBus_ConfigSet	  (PLAYER_CONFIG_VIDEO_OUT1_CONFIG ,"0x681a11,0xf0a215,0x26404547,0x1f639102,0x91,0xce");
#endif

	//播放MSK
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_DEC_MOSAIC_DROP ,0x0); 
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_DEC_MOSAIC_GATE ,50);
#ifdef DVB_32MB
//小内存方案限制帧存 zhangling add
	{
		int32_t fbsize = 6531840;
		GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_FB_MAX_SIZE, fbsize);
	}
#endif
//	app_flash_get_config_audio_ac3_bypass();
#if 0
    //修改同步方式
    GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_DEC_SYNC_FLAG ,2);//2 先播放再同步，能看到同步过程。1 先同步再播放，不能看到同步过程。
#endif

    GxServiceClass serv_list[] =
	{
		player_service,//播放
		si_service,
		epg_service,
		search_service,//节目搜索服务
		book_service,
		extra_service,
		gui_view_service,//gui view服务
		frontend_service,
		hotplug_service,
//		ca0_service,
#ifdef USB_FLAG
        update_service
#endif

	};
	service_num = sizeof(serv_list)/sizeof(GxServiceClass);

#if (0 == DVB_JPG_LOGO)
	rel = GxBus_Init(serv_list, service_num);
	if (rel != GXCORE_SUCCESS)
 	{
		return;
	}
#else
/*----boot logo show control---*/
	rel = GxBus_Init(serv_list, 1);//第一个服务先创建
	GxCore_ThreadDelay(2000);
	for(i = 1; i < service_num; i++)
	{
		rel = GxBus_ServiceCreate(&serv_list[i]);
	}
#endif

#ifdef DVB_SERIAL_SUPPORT
ComData_Process_task_init();
#endif

#ifdef _DEBUG
{
#define gx3201 0
#define gx3113c 1
#ifdef ECOS_OS
#if defined(CHIP) && (CHIP==gx3201) 
    extern int chip_sn_get(unsigned char *SnData, int BufferLen);
    unsigned char buffer[8] = {0};
    printf("[OTP]->Chip SN:");
    if(!chip_sn_get(buffer,8))
    {
        int i;
        for(i = 0;i<8;i++)
        {
            printf("%#x ",buffer[i]);
        }
        printf("\n");
    }
    else
    {
        printf("chip sn get error\n");
    }

    flash_otp_test();
#endif
#if defined(CHIP) && (CHIP==gx3113c)
    extern void chip3113C_otp_test(void);
    chip3113C_otp_test();
    extern void chip3113C_chipId_test(void);
    chip3113C_chipId_test();
#endif
#endif
}
    
#endif

#ifdef STACK_INFO
#ifdef ECOS_OS
   stack_check_thread(NULL);
#endif
#endif 

}




int GxCore_Startup(int argc, char **argv)
{
	//GDI_RegisterJPEG();
	handle_t app_idle;
	
	#if 0
	extern unsigned char _ac3_decoder[];    
	extern unsigned int _ac3_decoder_len;    
	int dev = GxAvdev_CreateDevice(0);    
	GxAVAudioCodecRegister(dev, _ac3_decoder, _ac3_decoder_len, CODEC_AC3);
	#endif

#ifdef ECOS_OS
	app_init_demod_tuner();
	extern void gx3xxx_rtc_timer_init(void);
	gx3xxx_rtc_timer_init();
#endif
	GxCore_ThreadCreate("app_idle",&app_idle, app_idle_thread, NULL, 16 * 1024, GXOS_DEFAULT_PRIORITY);

	GxCore_Loop();
	return 0;
}





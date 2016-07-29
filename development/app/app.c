/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.19		  lijq 	 			creation
*****************************************************************************/
#include "gui_core.h"
#include "app.h"
#include "gxavdev.h"
#if defined(ECOS_OS)
#include "gxcore_hw_bsp.h"
#endif
#include "gxcore_version.h"
//#include "app_key.h"
#include "app_common_porting_ca_flash.h"
//#include "app_common_porting_ca_smart.h"
#include "gxgui_view.h"
#include "development_version.h"
#include "gxapp_sys_config.h"
#include "app_common_panel.h"
#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif
/*wangjian add on 20141208*/

extern GuiWidgetOps file_image_ops;
handle_t g_app_msg_self = 0;
extern 	void app_porting_psi_create_filter_task(void);
extern 	int32_t GxDemux_Init(void);
//GUI服务相关
status_t app_msg_init(handle_t self)
{
/*
	if(s_unlisten_flag == 1)
	{

		// TODO: GxBus_MessageEmptyByOps(&g_service_list[SERVICE_GUI_VIEW]);
		*/
	g_app_msg_self = self;

	/*
	* 监听搜索消息
	*/
	GxBus_MessageListen(self, GXMSG_SEARCH_NEW_PROG_GET);//新节目消息
	GxBus_MessageListen(self, GXMSG_SEARCH_SAT_TP_REPLY);// 当前搜索的卫星和 transport 信
	GxBus_MessageListen(self, GXMSG_SEARCH_STATUS_REPLY);//搜索出错状态消息
	GxBus_MessageListen(self, GXMSG_SEARCH_STOP_OK);
	/*
	* 监听信号监测消息
	*/
	GxBus_MessageListen(self, GXMSG_FRONTEND_LOCKED);
	GxBus_MessageListen(self, GXMSG_FRONTEND_UNLOCKED);
	/*
	* 监听CA消息
	*/
	GxBus_MessageRegister(GXMSG_CA_ON_EVENT, 
							  sizeof(GxMsgProperty0_OnEvent));  
	GxBus_MessageListen(self, GXMSG_CA_ON_EVENT);
	/*
	* 监听预约消息
	*/
	GxBus_MessageListen(self, GXMSG_BOOK_TRIGGER);
	GxBus_MessageListen(self, GXMSG_BOOK_FINISH);
	/*
	* 监听SI表消息
	*/
	GxBus_MessageListen(self, GXMSG_SI_SUBTABLE_OK);
	GxBus_MessageListen(self, GXMSG_SI_SUBTABLE_TIME_OUT);
	/*
	* 监听TDT时间同步消息
	*/
	GxBus_MessageListen(self, GXMSG_EXTRA_SYNC_TIME_OK);
	/*
	* 监听播放消息
	*/
	GxBus_MessageListen(self, GXMSG_PLAYER_STATUS_REPORT);	
	GxBus_MessageListen(self, GXMSG_PLAYER_AVCODEC_REPORT);	
	GxBus_MessageListen(self, GXMSG_PLAYER_SPEED_REPORT);
	GxBus_MessageListen(self, GXMSG_PLAYER_RESOLUTION_REPORT);
	/*
	* 监听USB插拔、升级消息
	*/
#ifdef USB_FLAG
	GxBus_MessageListen(self, GXMSG_UPDATE_STATUS);
    GxBus_MessageListen(self, GXMSG_HOTPLUG_IN);
	GxBus_MessageListen(self, GXMSG_HOTPLUG_OUT);
#endif

	/*
	}
	if(s_unlisten_flag > 0)
	{
		s_unlisten_flag--;
	}*/
	return GXCORE_SUCCESS;
}

status_t app_msg_destroy(handle_t self)
{
	GxBus_MessageUnListen(self, GXMSG_SEARCH_NEW_PROG_GET);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_SAT_TP_REPLY);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_STATUS_REPLY);
	GxBus_MessageUnListen(self, GXMSG_SEARCH_STOP_OK);
	GxBus_MessageUnListen(self, GXMSG_FRONTEND_LOCKED);
	GxBus_MessageUnListen(self, GXMSG_FRONTEND_UNLOCKED);
	GxBus_MessageUnListen(self, GXMSG_CA_ON_EVENT);
	GxBus_MessageUnListen(self, GXMSG_BOOK_TRIGGER);
	GxBus_MessageUnListen(self, GXMSG_BOOK_FINISH);
	GxBus_MessageUnListen(self, GXMSG_SI_SUBTABLE_OK);
	GxBus_MessageUnListen(self, GXMSG_SI_SUBTABLE_TIME_OUT);
	GxBus_MessageUnListen(self, GXMSG_EXTRA_SYNC_TIME_OK);
	GxBus_MessageUnListen(self, GXMSG_PLAYER_STATUS_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_PLAYER_AVCODEC_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_PLAYER_SPEED_REPORT);	
	GxBus_MessageUnListen(self, GXMSG_UPDATE_STATUS);
	GxBus_MessageUnListen(self, GXMSG_PLAYER_RESOLUTION_REPORT);


#ifdef USB_FLAG
	GxBus_MessageUnListen(self, GXMSG_UPDATE_STATUS);
    GxBus_MessageUnListen(self, GXMSG_HOTPLUG_IN);
	GxBus_MessageUnListen(self, GXMSG_HOTPLUG_OUT);
#endif
	return GXCORE_SUCCESS;
}

extern status_t signal_connect_handler(void);

/*wangjian add on 20141208*/
void app_ota_tuner_mainfreq(void)
{
	uint32_t nMainFreq = 0;
	uint32_t nMainFreq1 = 0;
	int32_t symb = 6875;
	int32_t qam = 2;
	status_t lockState = 0;
    uint8_t time = 0;

	nMainFreq = app_flash_get_config_center_freq();
	nMainFreq1 = app_flash_get_config_manual_search_freq(); 
	symb = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	printf("[app_ota_tuner_mainfreq]lock main freq(%d %d %d).\n",\
			nMainFreq, symb, qam);

    GxFrontend_StopMonitor(0);
    while (time++<1)
    {
    	lockState = app_search_lock_tp(nMainFreq, symb, INVERSION_OFF, qam, 2000);
    	if (1 == lockState)
    	{
    		GxCore_ThreadDelay(1000);
			app_panel_show(PANEL_UNLOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) fail!!!\n", nMainFreq);
    	}
    	else
    	{
    		GxCore_ThreadDelay(2000);
			app_panel_show(PANEL_LOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) success@@@\n", nMainFreq);
            break;
    	}
		lockState = app_search_lock_tp(nMainFreq1, symb, INVERSION_OFF, qam, 2000);
    	if (1 == lockState)
    	{
    		GxCore_ThreadDelay(1000);

		app_panel_show(PANEL_UNLOCK,0);
    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) fail!!!\n", nMainFreq);
    	}
    	else
    	{
    		GxCore_ThreadDelay(2000);
		app_panel_show(PANEL_LOCK,0);

    		printf("[app_ota_tuner_mainfreq]lock main freq(%d) success@@@\n", nMainFreq);
            break;
    	}
    }
    GxFrontend_StartMonitor(0);

	return;
}
//应用初始化接口
status_t app_init(void)
{
	VideoColor color;
	status_t ret = GXCORE_SUCCESS;
	struct tm Time = {0,0,0,1,1,2000,0,0,0};	
	int32_t Config;
	char* osd_language=NULL;
    char goxceed_version[20] = {0};

#if (DVB_DEMOD_GX1503 == DVB_DEMOD_TYPE)
	uint32_t bandWidth = app_flash_get_config_center_freq_bandwidth();
	uint32_t dtmb_dvbc = app_flash_get_config_dtmb_dvbc_switch();

	extern void GX1503_Set_BandWidth(unsigned char bandwidth);
	GX1503_Set_BandWidth(bandWidth);
	extern void GX1503_Set_WorkMode(char ChipWorkMode);
	GX1503_Set_WorkMode(dtmb_dvbc);
#endif

    GxBus_ConfigGet(GOXCEED_RELEASE_VERSION, goxceed_version, 20, GOXCEED_VERSON);
	if (0 != strcmp(goxceed_version,GOXCEED_VERSON))
		{
			/*
			* 当前应用驱动版本与之前flash中的版本不同
			* 跨驱动版本升级，初始化删除所有节目、预约数据
			* 恢复系统设置等界面参数默认值
			* 其他可根据具体市场实际需求处理。
			*/
			GxBusPmDataSat sat;
			GxBus_PmLoadDefault(SAT_MAX_NUM,TP_MAX_NUM,SERVICE_MAX_NUM,NULL);
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			sat.type = GXBUS_PM_SAT_C;
			sat.tuner = 0;
			GxBus_PmSatAdd(&sat);
			GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			sat.type = GXBUS_PM_SAT_DTMB;
			sat.tuner = 0;
			sat.sat_dtmb.work_mode = app_flash_get_config_dtmb_dvbc_switch();
			GxBus_PmSatAdd(&sat);
			GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#endif
			app_book_clear_all();
			app_flash_save_default_config_para();
			GxBus_ConfigSet(GOXCEED_RELEASE_VERSION, (const char*)GOXCEED_VERSON);
		}

	app_prog_init_playing();
	app_init_frontend(FRONT_MONITER_DURATION);
	app_init_panel();

	/*
	* 设置无节目是否搜索开关标志
	*/
	app_search_set_auto_flag(TRUE);
	app_sys_init();
    INFO_MSG("Build Time:%s %s\n",__DATE__, __TIME__);

#ifdef DEVELOPMENT_SVN
    INFO_MSG("development svn : %d \n",DEVELOPMENT_SVN);
	INFO_MSG("%s\n\n",DEVELOPMENT_SVN_URL);
#endif

#ifdef DEVELOPMENT_GIT
    INFO_MSG("development git : %d \n",DEVELOPMENT_GIT);
	INFO_MSG("%s\n\n",DEVELOPMENT_GIT_VER);
#endif


#if AUDIO_DOLBY
    app_play_register_dolby();
#endif

	/*signal connect*/
	ret = signal_connect_handler();
	if(GXCORE_SUCCESS != ret)
	{
		printf("[signal_connect_handler] ERROR\n");
	}

#ifndef LOGO_SHOW_DELAY
#if defined(ECOS_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init("/theme/theme.conf");
#elif defined(LINUX_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init(WORK_PATH"theme/theme.conf");
#endif	
	if(GXCORE_SUCCESS != ret)
	{
		printf("=GUI_Init= ERROR\n");
		return GXCORE_ERROR;
	}
#endif

	/*
	  * 开机画面由应用入口显示，延时2s关闭开机画面
	  */
	GxAvdev_SetI2cAckWaitTime(3000);

#if (PANEL_TYPE == PANEL_TYPE_2KEY)
	app_panel_set_gpio_low(34);
#endif
	/*
	  * 开机画面由应用入口显示，延时2s关闭开机画面
	  */
#ifdef DVB_AD_TYPE_BY_FLAG
	app_by_ads_logo_sleep(5000);
#elif defined (DVB_AD_TYPE_DS_FLAG)
    printf("close the ad logo.\n\n");
	app_ds_ad_logo_sleep(5000);
#elif defined DVB_AD_TYPE_MAIKE_FLAG
	app_maike_ads_logo_sleep(5000);
#else
#if (1 == DVB_JPG_LOGO)
//	GxCore_ThreadDelay(5000);
//	advertisement_spp_layer_close();
#else
	GxCore_ThreadDelay(5000);
	app_play_close_av_logo();
#endif
#endif  

	app_epg_set_local_time(&Time,TRUE);
	app_prog_set_view_info_enable();

#ifndef LOGO_SHOW_DELAY
	Config = app_flash_get_config_osd_trans();
	app_play_set_osd_trasn_level(Config);
	#endif

	/*
	* set video color
	*/
	color.brightness = app_flash_get_config_videocolor_brightness();
	color.saturation = app_flash_get_config_videocolor_saturation();
	color.contrast = app_flash_get_config_videocolor_contrast();
	app_play_set_videoColor_level(color);

	//从配置中获取语言设置信息
	#ifndef LOGO_SHOW_DELAY
	osd_language = app_flash_get_config_osd_language();
	app_play_set_osd_language(osd_language);
	#endif
	
	app_book_init();
	GxFrontend_StartMonitor(0);

    
	//app_demodu_monitor_start();

	/*
	* 注册预约出错回调函数(如过期、预约满处理等)
	*/
	app_book_register_add_event_popmsg_callback(app_book_add_event_pop_msg_call_back);
	/*
	* 注册查询预约时间到回调函数(确认、取消预约观看等)
	*/
       app_book_register_play_popmsg_callback(app_book_confirm_play_pop_msg_call_back);
#if DVB_PVR_FLAG
	/*
	* 注册查询预约时间到回调函数(确认、取消预约录制等)
	*/
       app_book_register_record_popmsg_callback(app_book_confirm_record_pop_msg_call_back);
#endif
	/*
	* 注册添加搜索扩展表回调函数(SDT\NIT等扩展表添加)
	*/
	app_search_register_add_extend_table_callback(app_search_add_extend_table_call_back);
	/*
	* 注册提示信息显示回调函数(无信号、加扰节目等提示)
	*/
	app_play_register_show_prompt_msg_callback(app_win_prompt_msg_exec_call_back);
	app_play_register_password_widget_window("win_password_input");
	/*
	* 注册后台监测NIT表分析描述子回调函(OTA升级、LCN动态更新等)
	*/
	app_table_nit_register_parse_descriptor_callback(app_table_nit_parse_descriptor_call_back);

	/*
	* CA filter与后台监测filter(如PMT\CAT\NIT等)资源初始化
	*/
#ifdef CA_FLAG
//OTA强制升级ca部分
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DSCAS50);
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_MG312);
#endif
#ifdef DVB_CA_TYPE_MG_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_MG);
#endif
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DIVITONE);
#endif
#ifdef DVB_CA_TYPE_DVB_FLAG
	app_flash_save_config_dvb_ca_flag(DVB_CA_TYPE_DVB);
#endif


	app_cas_update_dvb_ca_flag();
#endif
	GxDemux_Init();
	app_porting_psi_create_filter_task();
	app_table_nit_monitor_filter_open();
 
	   
#ifdef CA_FLAG
	app_cas_init();//CA模块初始化
#endif
    
	/* gpio un mute low*/
	app_play_set_gpio_unmute();
#ifdef LOGO_SHOW_DELAY
#if defined(ECOS_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init("/theme/theme.conf");
#elif defined(LINUX_OS)
	widget_register("file_image", &file_image_ops);
	ret = GUI_Init(WORK_PATH"theme/theme.conf");
#endif	
	if(GXCORE_SUCCESS != ret)
	{
		printf("=GUI_Init= ERROR\n");
		return GXCORE_ERROR;
	}
    
    Config = app_flash_get_config_osd_trans();
	app_play_set_osd_trasn_level(Config);
    	//从配置中获取语言设置信息
	osd_language = app_flash_get_config_osd_language();
	app_play_set_osd_language(osd_language);
  app_audio_video_init();
#endif
#if (DVB_DEFINITION_TYPE == SD_DEFINITION)
    //app_play_set_videoColor_level(color);



    if(app_flash_get_config_video_hdmi_mode() == VIDEO_OUTPUT_NTSC_M)
    {
        //修复#14186
        app_play_set_hdmi_mode(VIDEO_OUTPUT_PAL);
        app_play_set_hdmi_mode(VIDEO_OUTPUT_NTSC_M);
    }
	app_play_set_videoColor_level(color);

#endif
	/*私有表格OTA升级码流监听
	* 
	*/
	// TODO:  给客户做定制版本时一定要注意这里的PID和table id
#ifdef OTA_PID
	app_table_ota_monitor_filter_open(OTA_PID,OTA_TABLEID);
#else
	app_table_ota_monitor_filter_open(0x1e61,0x88);
	#endif

	//close the log before create full screen.
	 printf("close the startlog.\n\n");
	app_play_close_av_logo();

#if (DVB_CHIP_TYPE == GX_CHIP_3113C)||(DVB_CHIP_TYPE == GX_CHIP_3115)
		*(unsigned int*)0xa48000a8 = 0x20c020;
#endif

#if  FACTORY_SERIALIZATION_SUPPORT
	extern int app_factory_serialization(void);
	app_factory_serialization();
#else
	GUI_CreateDialog("win_full_screen");
#endif

	/*Must be called later than app_cas_init()*/
#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_init();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	extern int mad_init(void);
	mad_init();
	mad_ts_task_init();
	mad_ads_client_init();
	start_monitor_mad();
#endif


	return GXCORE_SUCCESS;
}



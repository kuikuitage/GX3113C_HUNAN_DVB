/**
 *
 * @file        app_flash_init.c
 * @brief
 * @version     1.1.0
 * @date        10/25/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include <gxtype.h>
#include "gxplayer.h"
#include "gxprogram_manage_berkeley.h"
#include "gxbus.h"
#include "gui_event.h"
#include "gxmsg.h"
#include "gxfrontend.h"
#include "app_common_prog.h"
#include "app_common_flash.h"
#include "app_common_play.h"
#include "gxapp_sys_config.h"
#include "gxappcommon_version.h"
#include "app_common_search.h"
#include "app_common_init.h"
#include "panel_key.h"
#include "goxceed_csky.h"
#include "app_common.h"
#include "bsp_dev_name.h"
#include "tuner_enum.h"
#include "gxcore_version.h"

#if defined(ECOS_OS)
#include <cyg/io/gx3110_irr.h>
#endif
#include <sys/ioctl.h>

#if defined(ECOS_OS)
#define WDT_BASE_ADDR  0xa020b000
#define rWDT_CTL      (*(volatile unsigned int *)(WDT_BASE_ADDR + 0X0000))
#define rWDT_MATCH    (*(volatile unsigned int *)(WDT_BASE_ADDR + 0X0004))
#define rWDT_COUNT    (*(volatile unsigned int *)(WDT_BASE_ADDR + 0X0008))
#define rWDT_WSR      (*(volatile unsigned int *)(WDT_BASE_ADDR + 0X000C))
#endif

/*
* 初始化前端
*/
 extern handle_t         sApp_frontend_device_handle;
 extern handle_t         sApp_frontend_demux_handle; 
 extern int GxOem_Init(void);

int panel_fd =  -1;

#if defined(ECOS_OS)
extern void frontend_mod_init_gx1001(unsigned int num, const char *profile);
extern void frontend_mod_init_gx1503(unsigned int num, const char *profile);
extern void frontend_mod_init_atbm888x(unsigned int num ,char *profile);
extern void frontend_mod_init_gx1801(unsigned int num ,char *profile);
/*
* (1,"|0:2:0x18:1:0:0xc0:&0:1")
*  1 	-- tuner(demod) total num
*  0 	-- demod type (0)
*  2	      -- demod i2c id
*  0x18  -- demod i2c chip addr
*  1       -- tuner type
*  0       -- tuner i2c id
*  0xc0  -- tuner i2c chip addr
*  &0:0  -- extra ,扩展，兼容不同客户硬件demod/tuner多种配置可选，
*                              demod驱动统一维护
*   扩展: 0 -- 解调配置(默认)，0 -- tuner配置(默认)
*  公版方案扩展默认为&0:0。客户(或相同解调不同芯片)硬件设计与公版
*  不同，可通过判断客户/市场配置扩展参数。例如&1:2 
*/
 int32_t app_init_demod_tuner(void)
 {
 	uint8_t demod_enum = 0;
 	uint8_t demod_i2c_id = 0;
 	uint8_t demod_i2c_chipaddr = 0;
 	uint8_t tuner_type = TUNER_TYPE;
 	uint8_t tuner_i2c_id = 0;
 	uint8_t tuner_i2c_chipaddr = 0;
 	uint8_t demod_cfg = 0;	/*解调配置，公版默认0,及默认第一种配置*/
 	uint8_t tuner_cfg = 0;	/*tuner配置，公版默认0,及默认第一种配置*/
	char profile[60]={0};
	
	
	
	 if (DVB_DEMOD_GX1001 == DVB_DEMOD_TYPE)
	 {
		/*
		* gx1001
		*/
		demod_enum = 0;
 		demod_i2c_id = 2;
 		demod_i2c_chipaddr = 0x18;
		tuner_i2c_id = 0;
		tuner_i2c_chipaddr = 0xc0;
		demod_cfg = 0;
		tuner_cfg = 0;
		
		 switch(TUNER_TYPE)
		 {
			 case TUNER_ALPSTDAE:
					 break;
			 case TUNER_TDA18250:
					 break;
			 case TUNER_R836:
					 tuner_i2c_chipaddr = 0x74;
					 break;
			 case TUNER_TDA18250A:
					 break;
			 case TUNER_MXL608:
					 break;
			 case TUNER_SONY_CXD2872:
					 break;
#if 0
			 case TUNER_R820C2: 
					 // tuner R820C2
					 frontend_mod_init_gx1001(1,"|0:2:0x18:56:0:0x34:&0:1"); 
					 break; 
#endif
			 default:
					 break;
		 }	
		 
		 sprintf(profile,"|%d:%d:0x%x:%d:%d:0x%x:&%d:%d",
			 demod_enum,demod_i2c_id,demod_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,demod_cfg,tuner_cfg);
		 frontend_mod_init_gx1001(1,profile);
		 
	 }
 
	 if (DVB_DEMOD_GX1503 == DVB_DEMOD_TYPE)
	 {
	   	/*
		   * gx1503
		   */
		demod_enum = 1;
 		demod_i2c_id = 0;
 		demod_i2c_chipaddr = 0x60;
		tuner_i2c_id = 0;
		tuner_i2c_chipaddr = 0xc0;
		if (GX_CHIP_3113H == DVB_CHIP_TYPE)
			{
				demod_cfg = 2; /*外挂串行，其他并行*/
			}
		else if (GX_CHIP_3201 == DVB_CHIP_TYPE)
			{
				demod_cfg = 0; /* gx3201 公版方案gx1503并行配置*/
			}
		else
			{
				demod_cfg = 1; /* gx3113c/gx3115等 公版方案gx1503并行配置*/
			}
		/*
		* 	其他客户硬件配置3~256
		*/
		tuner_cfg = 0;
		 switch(TUNER_TYPE)
		 {
			 case TUNER_TDAC7:
					 break;
			 case TUNER_TDA18273:
					 break;
			 case TUNER_MXL603:
					 break;
	         case TUNER_MXL608:
					 break;
			 default:
					 break;
		 }

		 sprintf(profile,"|%d:%d:0x%x:%d:%d:0x%x:&%d:%d",
			 demod_enum,demod_i2c_id,demod_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,demod_cfg,tuner_cfg);
		 frontend_mod_init_gx1503(1,profile);
		 
	 }
	 if (DVB_DEMOD_ATBM886X == DVB_DEMOD_TYPE)
	 {
	   /*
		   * ATBM886X
		   */
		demod_enum = 7;
 		demod_i2c_id = 0;
 		demod_i2c_chipaddr = 0x80;
		tuner_i2c_id = 0;
		tuner_i2c_chipaddr = 0xc0;

#ifdef DVB_32MB
				demod_cfg = 1; 
#else
				demod_cfg = 0; 
#endif
		/*
		* 	其他客户硬件配置2~256
		*/
		tuner_cfg = 0;		  
		 switch(TUNER_TYPE)
		 {
			 case TUNER_TDAC7:
					 break;
			 case TUNER_TDA18273:					 
					 break;
			 case TUNER_MXL603:
					 break;
             case TUNER_MXL608:
					 break;
			 case TUNER_ATBM2030:
					 break;
			 default:
					 break;
		 }
		 
		 sprintf(profile,"|%d:%d:0x%x:%d:%d:0x%x:&%d:%d",
			 demod_enum,demod_i2c_id,demod_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,demod_cfg,tuner_cfg);
		 frontend_mod_init_atbm888x(1,profile);
		 
	 }
	 
	 if (DVB_DEMOD_GX1801 == DVB_DEMOD_TYPE)
	 {
		/*
		* gx1801
		*/
		demod_enum = 0;
 		demod_i2c_id = 0;
 		demod_i2c_chipaddr = 0xe4;
		tuner_i2c_id = 0;
		tuner_i2c_chipaddr = 0xc0;
		demod_cfg = 0;
		tuner_cfg = 0;
		
		 switch(TUNER_TYPE)
		 {
			 case TUNER_ALPSTDAE:
					 break;
			 case TUNER_TDA18250:
					 break;
			 case TUNER_R836:
					 tuner_i2c_chipaddr = 0x74;
					 break;
			 case TUNER_TDA18250A:
					 break;
			 case TUNER_MXL608:
					 break;
#if 0
			 case TUNER_R820C2: 
					 // tuner R820C2
					 frontend_mod_init_gx1001(1,"|0:2:0x18:56:0:0x34:&0:1"); 
					 break; 
#endif
			 default:
					 break;
		 }	
		 sprintf(profile,"|%d:%d:0x%x:%d:%d:0x%x:&%d:%d",
			 demod_enum,demod_i2c_id,demod_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,demod_cfg,tuner_cfg);
		 
		 //sprintf(profile,"|%d:%d:0x%x:100:41:0:0xC6:%d:%d:0x%x:%d:%d:0x%x:&%d:%d:0:0:0",
		//	 demod_enum,demod_i2c_id,demod_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,tuner_type,tuner_i2c_id,tuner_i2c_chipaddr,demod_cfg,tuner_cfg);
		 frontend_mod_init_gx1801(1,profile);
	 }	

 	return 0;
 }
#endif


int32_t app_init_panel(void)
{
	panel_fd = -1;
#if (PANEL_TYPE != PANEL_TYPE_NONE)
	panel_fd = GxCore_Open(PANEL_NAME, "rw");
#endif
	return 	panel_fd;	
}


/*重启模块*/
int restart(void)
{
	/*
	* restart之前delay 1000ms,避免重启前flash写数据未完成
	*/
	if (TUNER_SI2141 == TUNER_TYPE)
	{
		extern void si2141_tuner_standby(void);
		extern void si2141_tuner_poweron(void);

		si2141_tuner_standby();
		GxCore_ThreadDelay(1000);
		si2141_tuner_poweron();
	}
	
	 GxCore_ThreadDelay(1000);	
#if defined(ECOS_OS)
	unsigned int sys_clk = 27000000;
	rWDT_MATCH = ((sys_clk/1000000 - 1) << 16)|(0x10000 - 10000);  
	rWDT_CTL   = 3; 
	return 0;
#elif defined(LINUX_OS)
	system("reboot"); 
	return 0;
#endif
}

/*
 * sleep_time. (s), power off sleep_time (s) then power on auto
 * if sleep_time is 0, power off always
 */
int32_t app_lower_power_entry(uint32_t key,uint32_t io_num,uint32_t io_status)
{
	char cmd[100];
	lowpower_info lowpower = {0};
	int fd;
	memset(cmd,0,100);
	lowpower.key = 0;
	lowpower.WakeTime = 0;
	lowpower.GpioMask = 0xffffffff;
	lowpower.GpioData = 0xffffffff;
    fd = open("/dev/gxirr0", O_RDWR);

	if(0!= io_num)
	{
		//	sprintf((void*)cmd, "keys=0x%x powercut=%d,%d",key,io_num,io_status);
		sprintf((void*)cmd,
				"keys=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x powercut=%d,%d",
				0xbb44af50,
				0xfb04b748,
				0x50005700,
				0x7F80FD02,
				0xFDCB3FC0,
				PANEL_KEY_POWER,
				io_num,
				io_status);
		lowpower.cmdline = (void*)cmd;
	}
	else
	{
		lowpower.key = key;
	}


	printf("PANEL_KEY_POWER =%x\n",lowpower.key);
	printf("\n %s\n",lowpower.cmdline);
	
	
    ioctl(fd, IRR_LOWPOWER, &lowpower); 
	
#ifdef LINUX_OS
	system("halt"); 
#endif

	return 0;
}

int32_t app_init_frontend(int32_t time)
{
	status_t                ret;
	GxDemuxProperty_ConfigDemux config_demux;

	GxBus_ConfigSetInt(FRONTEND_CONFIG_WATCH_TIME,time);


	sApp_frontend_device_handle = GxAvdev_CreateDevice(0);
	if (sApp_frontend_device_handle < 0) {
		printf("[SEARCH]---create device err!!\n");
		return 1;
	}
	
	sApp_frontend_demux_handle =  GxAvdev_OpenModule(sApp_frontend_device_handle, GXAV_MOD_DEMUX, 0);
	if (sApp_frontend_demux_handle < 0) {
		printf("[SEARCH]---open demux  err!!\n");
		return 2;
	}

	/*
	 * 前端初始化接口中，DEMUX_TS和FRONTEND绑定
	 * 避免出现批点锁定，但DEMUX未锁定的问题
	 */
	config_demux.source = DVB_TS_SRC;
	config_demux.ts_select = FRONTEND;
	config_demux.stream_mode = DEMUX_PARALLEL;
	config_demux.time_gate = 0xf;
	config_demux.byt_cnt_err_gate = 0x03;
	config_demux.sync_loss_gate = 0x03;
	config_demux.sync_lock_gate = 0x03;
	ret = GxAVSetProperty(sApp_frontend_device_handle,
			sApp_frontend_demux_handle,
			GxDemuxPropertyID_Config,
			&config_demux,
			sizeof(GxDemuxProperty_ConfigDemux));

	return 0;
}

void app_audio_video_init()
{
    uint32_t param;
	int32_t Config;
	Video_Display_Screen_t n4To3_16To9;
	int32_t nSwitchEffect;
	int32_t videoadapt;
	int32_t nAspect;
	int32_t VideoOutPut;

	GxMsgProperty_PlayerVideoAutoAdapt	AutoAdapt;
	GxMsgProperty_PlayerFreezeFrameSwitch SwitchEffectPara;

	GxMsgProperty_PlayerVideoInterface VideoInterface;

//	GxBus_ConfigSetInt(FRONTEND_CONFIG_WATCH_TIME,FRONT_MONITER_DURATION);

	Config = app_flash_get_config_audio_track();
	param = Config;
	app_play_set_audio_track(param);

	Config = app_flash_get_config_audio_volume();
	param = Config;
	app_play_set_volumn(param);

	VideoOutPut = app_flash_get_config_video_hdmi_mode();
	n4To3_16To9 = app_flash_get_config_video_display_screen();
	nSwitchEffect = app_flash_get_config_video_quiet_switch();
	videoadapt = app_flash_get_config_video_auto_adapt();
	nAspect = app_flash_get_config_video_aspect();
	
	AutoAdapt.enable= videoadapt;
	AutoAdapt.ntsc =VIDEO_OUTPUT_NTSC_M;
	AutoAdapt.pal = VIDEO_OUTPUT_PAL;
	SwitchEffectPara = nSwitchEffect;
	/*
	 * 支持HDMI与RCA同时输出，
	 *
	 */
 	switch(DVB_DEFINITION_TYPE)
	{
		case HD_DEFINITION:
			VideoInterface = VIDEO_OUTPUT_HDMI|VIDEO_OUTPUT_RCA;
			break;
		case SD_DEFINITION:
			VideoInterface = VIDEO_OUTPUT_RCA;			
			break;
	}
	app_send_msg(GXMSG_PLAYER_VIDEO_INTERFACE, (void*)&VideoInterface);
	app_play_set_video_display_screen(n4To3_16To9);

	/*
	* 高清方案CVBS无输出
	*/
	app_play_set_rca1_mode(VIDEO_OUTPUT_PAL);
	app_play_set_hdmi_mode(VideoOutPut);
	
	app_send_msg(GXMSG_PLAYER_VIDEO_AUTO_ADAPT, (void*)&AutoAdapt);
	app_send_msg(GXMSG_PLAYER_FREEZE_FRAME_SWITCH, &SwitchEffectPara);
	app_play_set_video_aspect(nAspect);


}


void app_sys_init(void)
{
	uint8_t           oemValue[10]={0};
	GxBusPmDataSat sat;
	GxBusPmViewInfo sys={0};

	printf("goxceed version = %s\n\n", GOXCEED_VERSON);	
	
#ifdef DVB_AUTO_TEST_FLAG
	 extern void atuotest_create_serivce(void);
	 app_search_set_auto_flag(FALSE);
	 atuotest_create_serivce();
	 app_demodu_monitor_stop();
	 GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_DEC_MOSAIC_GATE, 80);	 
#endif
	app_enum_search_fre_list_init();

	/* 串口输出gxapp_common 的版本信息*/
	INFO_MSG("Build Time:%s %s\n",__DATE__, __TIME__);
    INFO_MSG(":%s\n",GXAPPCOMMON_BUILD);

#ifdef GXAPPCOMMON_SVN
	INFO_MSG("gxapp_common svn : %d \n",GXAPPCOMMON_SVN);
	INFO_MSG("%s\n\n",GXAPPCOMMON_SVN_URL);
#endif

#ifdef GXAPPCOMMON_GIT
		INFO_MSG("gxapp_common git : %d \n",GXAPPCOMMON_GIT);
		INFO_MSG("version %s\n\n",GXAPPCOMMON_GIT_VER);
#endif


	if(0 == GxBus_PmSatNumGet())
	{
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
	}

#if !defined(LOGO_SHOW_DELAY)
    {
        app_audio_video_init();
    }
#endif

	/*排序方式*///跳过加锁
	GxBus_PmViewInfoGet(&sys);
	if (VIEW_INFO_SKIP_VANISH != sys.skip_view_switch)
	{
		sys.skip_view_switch = VIEW_INFO_SKIP_VANISH;
		GxBus_PmViewInfoModify(&sys);		
	}
	app_prog_userlist_init();

#if defined(ECOS_OS)
	GxOem_Init();
#elif defined(LINUX_OS)
	{
		static uint32_t init_flag = 1;
		if (1 == init_flag)
		{
			init_flag = 0;
			app_flash_linux_partion_init("V_OEM");
			//app_flash_linux_partion_init("I_OEM");
			app_flash_linux_partion_init("SERIAL");		
		}
	}
#endif

#if 1
	/*
	* check & update ota demod \ tuner \ tsrc auto
	*/
	switch(DVB_DEMOD_TYPE)
	{
		case DVB_DEMOD_GX1503:
			if (0 != strcmp("DEMOD_GX1503",app_flash_get_oem_fe_demod_type()))
			{
				printf("DVB_DEMOD_TYPE = %d app_flash_get_oem_fe_demod_type %s\n",
					DVB_DEMOD_TYPE,app_flash_get_oem_fe_demod_type());
				app_flash_set_oem_fe_demod_type("DEMOD_GX1503");
				app_flash_save_oem();
			}
			break;
		case DVB_DEMOD_GX1001:
			if (0 != strcmp("DEMOD_GX1001",app_flash_get_oem_fe_demod_type()))
			{
				printf("DVB_DEMOD_TYPE = %d app_flash_get_oem_fe_demod_type %s\n",
					DVB_DEMOD_TYPE,app_flash_get_oem_fe_demod_type());
				app_flash_set_oem_fe_demod_type("DEMOD_GX1001");
				app_flash_save_oem();
			}
			break;
		case DVB_DEMOD_ATBM886X:
			if (0 != strcmp("DEMOD_ATBM886X",app_flash_get_oem_fe_demod_type()))
			{
				printf("DVB_DEMOD_TYPE = %d app_flash_get_oem_fe_demod_type %s\n",
					DVB_DEMOD_TYPE,app_flash_get_oem_fe_demod_type());
				app_flash_set_oem_fe_demod_type("DEMOD_ATBM886X");
				app_flash_save_oem();
			}	
			break;
		case DVB_DEMOD_GX1801:
			if (0 != strcmp("DEMOD_GX1801",app_flash_get_oem_fe_demod_type()))
			{
				printf("DVB_DEMOD_TYPE = %d app_flash_get_oem_fe_demod_type %s\n",
					DVB_DEMOD_TYPE,app_flash_get_oem_fe_demod_type());
				app_flash_set_oem_fe_demod_type("DEMOD_GX1801");
				app_flash_save_oem();
			}	
			break;
		default:
			break;
	}


	switch(DVB_TS_SRC)
		{
			case 0:
				if (0 != strcmp("DEMUX_TS1",app_flash_get_oem_demux_source()))
					{
						printf("DVB_TS_SRC = %d app_flash_get_oem_demux_source %s\n",
							DVB_TS_SRC,app_flash_get_oem_demux_source());
						app_flash_set_oem_dmx_source("DEMUX_TS1");
						app_flash_save_oem();
					}
				break;
			case 1:
				if (0 != strcmp("DEMUX_TS2",app_flash_get_oem_demux_source()))
					{
						printf("DVB_TS_SRC = %d app_flash_get_oem_demux_source %s\n",
							DVB_TS_SRC,app_flash_get_oem_demux_source());
						app_flash_set_oem_dmx_source("DEMUX_TS2");
						app_flash_save_oem();
					}
				break;
			case 2:
				if (0 != strcmp("DEMUX_TS3",app_flash_get_oem_demux_source()))
					{
						printf("DVB_TS_SRC = %d app_flash_get_oem_demux_source %s\n",
							DVB_TS_SRC,app_flash_get_oem_demux_source());
						app_flash_set_oem_dmx_source("DEMUX_TS3");
						app_flash_save_oem();
					}
				break;
			default:
				break;
		}

	if(TUNER_TYPE != app_flash_get_oem_fe_tuner_type() )
		{
			printf("TUNER_TYPE =%d app_flash_get_oem_fe_tuner_type %d\n",
				TUNER_TYPE,app_flash_get_oem_fe_tuner_type());
			memset(oemValue,0,10);
			sprintf((char*)oemValue, "%d",TUNER_TYPE);
			app_flash_set_oem_fe_tuner_type((char*)oemValue);
			app_flash_save_oem();
		}
#endif
	
}

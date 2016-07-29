#ifndef __SYS_COMMON_CONFIGURE_H__
#define __SYS_COMMON_CONFIGURE_H__

#include <gxtype.h>
#include "gxprogram_manage_berkeley.h"
#include "service/gxplayer.h"
#include "app_common_flash.h"
#include "gxapp_sys_config.h"



/*
* 定义语言xml对应名称
*/
#define LANGUAGE_CHINESE ("Chinese")
#define LANGUAGE_ENGLISH ("English")


/*
* 此头文件中宏定义值提供common模块调用
* 宏定义值可根据不同方案实际修改
* 宏定义变量名切勿随意修改、删除(可增加)，否则可能导致编译、运行异常
*/
#define VIDEO_DISPLAY_SCREEN_DV       (DISPLAY_SCREEN_4X3)
#if ( DVB_DEFINITION_TYPE == SD_DEFINITION)
#define VIDEO_HDMI_MODE_DV            (VIDEO_OUTPUT_PAL)//    (VIDEO_OUTPUT_HDMI_720P_50HZ)
#endif
#if ( DVB_DEFINITION_TYPE == HD_DEFINITION)
#define VIDEO_HDMI_MODE_DV 			 (VIDEO_OUTPUT_HDMI_1080I_50HZ)
#endif
#define VIDEO_ASPECT_DV                       (0)//(ASPECT_NORMAL)
#define VIDEO_QUIET_SWITCH_DV           (VIDEO_SWITCH_STILL)
#define VIDEO_AUTO_ADAPT_DV              (0) //
#define AUDIO_VOLUME_DV                      (48)
#define AUDIO_TRACK_DV                 	  (AUDIO_TRACK_STEREO)
#define AUDIO_AUDIO_TRACK_DV            (AUDIO_TRACK_STEREO)
#define AUDIO_AC3_BYPASS_DV        	  (AUDIO_AC3_DECODE_MODE)
#define OSD_LANG_DV                 		  (LANGUAGE_CHINESE)
#define TELTEXT_LANG_DV                 	  (LANGUAGE_ENGLISH)
#define SUBTITLE_LANG_DV                 	  (LANGUAGE_ENGLISH)


#define OSD_TRANS_DV                            (10)

#define VIDEOCOLOR_BRIGHTNESS_DV                   (50)
#define VIDEOCOLOR_SATURATION_DV                   (50)
#define VIDEOCOLOR_CONTRAST_DV                  	(50)


#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#define BAR_TIME_DV                               (5)
#else
#define BAR_TIME_DV                               (2)
#endif
#define LCN_DV                                   (LCN_STATE_OFF)

#ifdef DVB_CA_TYPE_QZ_FLAG
#define PASSWORD_DV                             ("9527")
#else
#define PASSWORD_DV                             ("0000")
#endif

#define TIMEZONE_DV                              (8)
#define MAIN_FREQ_NITVERSION_DV       (32)
#define MANUSAL_SEARCH_FREQ_DV       (730000)
#define MANUSAL_SEARCH_SYMBOL_DV   (6875)
#define MANUSAL_SEARCH_QAM_DV         (2)
#define MANUSAL_SEARCH_BANDWIDTH_DV  (8)
#define DTMB_DVBC_SWITCH_DV  (0)
#define PASSWORD_FLAG_DV 			   (0)
#define MUTE_FLAG_DV                             (0)
#define DTMB_MODE_DV                GXBUS_PM_SAT_1501_DTMB
#define USER_LIST_DV                              (0)
#define TRACK_GLOBLE_FLAG_DV             (1)
#define VOLUMN_GLOBLE_FLAG_DV          (1)
#define PVR_DURATION_VALUE              (0)
#define PVR_TIMESHIFT_DEFAULT	   			 (0)	
#define SUBTITLE_LANG_VALUE               (0)
#define SUBTITLE_MODE_VALUE               (0)
/*
* 农网等特定市场，节目service_id无序
* 前端码流无逻辑频道描述子。
* 要求按照pat表中的顺序进行排序
* 0 -- 不开启pat表顺序排序
* 1 -- 开启pat表顺序排序
*/
#define SORT_PROG_BY_PAT_DV     (0)


/*
* 定义搜索节目过程中，列表中同时显示搜索到的节目最大个数
*/
#define SEARCH_PROG_LIST_MAX (7)

/*
* 创建timer切台，设置timer启动时间
*/
#define PLAY_TIMER_DURATION (300)

/*
* 密码最大长度
*/
#define MAX_PASSWD_LEN			(4)

/*
* 前端监测时间
*/
#define FRONT_MONITER_DURATION (1000)

/*
* 序列号最大长度
*/
#define MAX_SEARIAL_LEN (20)

/*
* 全品频段搜索开始、结束频率
*/

#if (DVB_DEMOD_MODE == DVB_DEMOD_DVBC)
#define FRE_BEGIN_LOW (115000)
#define FRE_BEGIN_HIGH (858000)
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
#define FRE_BEGIN_LOW (52500)
#define FRE_BEGIN_HIGH (858000)
#endif

#ifdef CUST_TAOYUAN
#define FRE_BEGIN_LOW_FULL (674000)
#define FRE_BEGIN_HIGH_FULL (794000)
#endif
#ifdef CUST_LINLI
#define FRE_BEGIN_LOW_FULL (714000)
#define FRE_BEGIN_HIGH_FULL (796000)
#endif
#ifdef CUST_TAIKANG
#define FRE_BEGIN_LOW_FULL (706000)
#define FRE_BEGIN_HIGH_FULL (770000)
#endif
#ifdef CUST_JINGANGSHAN
#define FRE_BEGIN_LOW_FULL (706000)
#define FRE_BEGIN_HIGH_FULL (770000)
#endif

#ifdef CUST_SHANGSHUI
#define FRE_BEGIN_LOW_FULL (706000)
#define FRE_BEGIN_HIGH_FULL (762000)
#endif

#if (DVB_THEME_TYPE == DVB_THEME_SD)

#define POP_DIALOG_X_START          210
#define POP_DIALOG_Y_START          198 /* 200 */

#define POP_MEDIA_X_START          160
#define POP_MEDIA_Y_START          200 

/*
* main menu video zoom position
*/
#define MAIN_MENU_VIDEO_X  192 
#define MAIN_MENU_VIDEO_Y  152
#define MAIN_MENU_VIDEO_W  348 
#define MAIN_MENU_VIDEO_H  240 

/*
* tv list menu video zoom position
*/

#define TV_MENU_VIDEO_X  85 
#define TV_MENU_VIDEO_Y  140 
#define TV_MENU_VIDEO_W  248 
#define TV_MENU_VIDEO_H  186 

/*
* epg menu video zoom position
*/
#define EPG_MENU_VIDEO_X  64 
#define EPG_MENU_VIDEO_Y  116 
#define EPG_MENU_VIDEO_W  220 
#define EPG_MENU_VIDEO_H  150 

/*
* fav menu video zoom position
*/
#define FAV_MENU_VIDEO_X  85 
#define FAV_MENU_VIDEO_Y  140 
#define FAV_MENU_VIDEO_W  248 
#define FAV_MENU_VIDEO_H  186 

/*
* edit menu video zoom position
*/
#define EDIT_MENU_VIDEO_X  430 
#define EDIT_MENU_VIDEO_Y  40 
#define EDIT_MENU_VIDEO_W  240 
#define EDIT_MENU_VIDEO_H  180 

#endif/*APP_SD*/



#if (DVB_THEME_TYPE == DVB_THEME_HD)

#define POP_DIALOG_X_START          340
#define POP_DIALOG_Y_START          235 /* 220 */

#define POP_MEDIA_X_START          440
#define POP_MEDIA_Y_START          285
/*
* main menu video zoom position
*/
#define MAIN_MENU_VIDEO_X  400 
#define MAIN_MENU_VIDEO_Y  160
#define MAIN_MENU_VIDEO_W  500 
#define MAIN_MENU_VIDEO_H  290 

/*
* tv list menu video zoom position
*/
#define TV_MENU_VIDEO_X  630 
#define TV_MENU_VIDEO_Y  160 
#define TV_MENU_VIDEO_W  480 
#define TV_MENU_VIDEO_H  300 

/*
* epg menu video zoom position
*/
#define EPG_MENU_VIDEO_X  890 
#define EPG_MENU_VIDEO_Y  170 
#define EPG_MENU_VIDEO_W  220 
#define EPG_MENU_VIDEO_H  180 

/*
* fav menu video zoom position
*/
#define FAV_MENU_VIDEO_X  630 
#define FAV_MENU_VIDEO_Y  160 
#define FAV_MENU_VIDEO_W  480 
#define FAV_MENU_VIDEO_H  300 

/*
* edit menu video zoom position
*/
#define EDIT_MENU_VIDEO_X  750 
#define EDIT_MENU_VIDEO_Y  210 
#define EDIT_MENU_VIDEO_W  360 
#define EDIT_MENU_VIDEO_H  220 



#endif /* APP_HD */

#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)

#define POP_DIALOG_X_START          370 //340
#define POP_DIALOG_Y_START          160 /* 220 */

#define POP_MEDIA_X_START          450
#define POP_MEDIA_Y_START          270 

/*
* main menu video zoom position
*/
#define MAIN_MENU_VIDEO_X  624 
#define MAIN_MENU_VIDEO_Y  70
#define MAIN_MENU_VIDEO_W  450 
#define MAIN_MENU_VIDEO_H  300 

/*
* tv list menu video zoom position
*/

#define TV_MENU_VIDEO_X  710 
#define TV_MENU_VIDEO_Y  100 
#define TV_MENU_VIDEO_W  450 
#define TV_MENU_VIDEO_H  300 

/*
* epg menu video zoom position
*/
#define EPG_MENU_VIDEO_X  710 
#define EPG_MENU_VIDEO_Y  100 
#define EPG_MENU_VIDEO_W  450 
#define EPG_MENU_VIDEO_H  300 

/*
* fav menu video zoom position
*/
#define FAV_MENU_VIDEO_X  710 
#define FAV_MENU_VIDEO_Y  100 
#define FAV_MENU_VIDEO_W  450 
#define FAV_MENU_VIDEO_H  300 

/*
* edit menu video zoom position
*/
#define EDIT_MENU_VIDEO_X  710 
#define EDIT_MENU_VIDEO_Y  100 
#define EDIT_MENU_VIDEO_W  450 
#define EDIT_MENU_VIDEO_H  300 



#endif /* APP_DTMB_HD */

#if (DVB_THEME_TYPE == DVB_THEME_SD) 
// 公版标清方案界面，界面默认不开启pal/ntsc切换
// 市场版本根据需求开启
#define TV_STANARD_SUPPORT (0)
#else
#define TV_STANARD_SUPPORT (1)
#endif

#endif

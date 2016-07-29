/**
 *
 * @file        app_common_play.h
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_MEDIA__H__
#define __APP_COMMON_MEDIA__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_media_popmsg.h"

#include "module/player/gxplayer_module.h"
#include "gxservices.h"


//media
#include "media_key.h"
#include "media_string.h"

#include "pmp_setting.h"
#include "pmp_tags.h"
#include "pmp_explorer.h"
#include "pmp_spectrum.h"
#include "pmp_subtitle.h"
#include "pmp_lyrics.h"
#include "pmp_id3.h"

#include "file_view.h"
#include "file_edit.h"
#include "play_manage.h"
#include "play_pic.h"
#include "play_movie.h"
#include "play_music.h"
#include "play_text.h"
#include "app_common_flash.h"
#include "gxapp_sys_config.h"


#ifndef PMP_ATTACH_DVB
#define PMP_ATTACH_DVB
#endif




#define APP_XRES  VIDEO_WINDOW_W
#define APP_YRES  VIDEO_WINDOW_H





#define APP_FREE(x)	if(x){GxCore_Free(x);x=NULL;}

#define APP_CHECK_P(p, r) if(NULL == p){\
							printf("\033[033m");\
							printf("\n[%s: %d] %s\n", __FILE__, __LINE__, __FUNCTION__);\
							printf("%s is NULL\n", #p);\
							printf("\033[0m");\
							return r;}



#define APP_TIMER_ADD(timer, fun, timeout, flag) \
							if(timer){\
								reset_timer(timer);\
							 }else{\
								timer = create_timer(fun, timeout, 0, flag);}

#define APP_TIMER_REMOVE(x)	if(x){remove_timer(x);x=NULL;}


/*string*/
/*#define STR_ID_NO_DEVICE    "No device or not support!"
#define STR_ID_FILE_NOT_SUPPORT    "The file type is not supported!"
#define STR_ID_CONTINUE_VIEW "Continue to watch?"

#define STR_ID_FILE    "File"
#define STR_ID_VIDEO    "Video"
#define STR_ID_MUSIC    "Music"
#define STR_ID_PICTURE    "Picture"
#define STR_ID_TEXT    "Text"
#define STR_ID_WAITING    "Waiting..."
#define STR_ID_COPY    "Copy"
#define STR_ID_CUT    "Cut"
#define STR_ID_PASTE    "Paste"
#define STR_ID_SWITCH_DURA    "Switch Duration"
#define STR_ID_SWITCH_MODE    "Switch Mode"
#define STR_ID_L2R    "Left to Right"
#define STR_ID_R2L    "Right to Left"
#define STR_ID_PLAY_SEQ   "Play Sequence"
#define STR_ID_SEQUENCE    "Sequence"
#define STR_ID_RANDOM    "Random"
#define STR_ID_RESOLUTION    "Resolution"
#define STR_ID_DATE_TIME    "Date Time"
#define STR_ID_CAMERA_MAKE    "Camera Make"
#define STR_ID_CAMERA_MODEL    "Camera Model"
#define STR_ID_FOCAL_LENGTH   "Focal Length"
#define STR_ID_EXPOSURE_TIME  "Exposure Time"
#define STR_ID_QUICK_SEEK    "Quick Seek"
#define STR_ID_AUDIO_CH    "Audio Channel"
#define STR_ID_AUDIO_DELAY    "Audio Delay"
#define STR_ID_LOAD    "Load"
#define STR_ID_VISIBLITY    "Visibility"
#define STR_ID_DELAY    "Delay"
#define STR_ID_FILE_NAME    "File Name"
#define STR_ID_FILE_SIZE    "File Size"
#define STR_ID_CODE_FORMAT    "Code Format"
#define STR_ID_FRAME_RATE    "Frame Rate"
#define STR_ID_TRACK    "TRACK"
#define STR_ID_ONLY_ONCE    "Only once"
#define STR_ID_REPEAT_ONE    "Repeat one"
#define STR_ID_REPEAT_ALL    "Repeat all"
#define STR_ID_VIEW_MODE    "View Mode"
#define STR_ID_LRC    "Lyric"
#define STR_ID_NO_LRC    "No lyric!"
#define STR_ID_SPECTRUM    "Spectrum"
#define STR_ID_ROLL_LINES    "Roll Lines"
#define STR_ID_AUTO_ROLL   "Auto Roll"*/


#define TKGS_SUPPORT 0
#define MINI_256_COLORS_OSD_SUPPORT  0
#define MINI_16_BITS_OSD_SUPPORT  0
#define MINI_16BIT_WIN8_OSD_SUPPORT 0
//#define MENCENT_FREEE_SPACE
#define DLNA_SUPPORT 0
#define EX_SHIFT_SUPPORT 0
#define CA_SUPPORT 0
#define ECM_SUPPORT 0
#define TWO_ECM_SUPPORT 0
#define LOOP_TMS_SUPPORE
#define TTX_PRE_PROCESS_ENABLE 0
#define	PVRBAR_TIMEOUT	8
#define MV_WIN_SUPPORT 1
//#define MEDIA_FILE_EDIT_UNVALID
#define MEDIA_SUBTITLE_SUPPORT /*1*/0

//Don't support this in Ecos
#define FILE_EDIT_VALID
#define MEDIA_FILE_EDIT_UNVALID
#define DISK_FORMAT_UNVALID
#define REDIO_RECODE_SUPPORT	1

#ifdef ECOS_OS
#define FILE_EDIT_VALID
#undef DISK_FORMAT_UNVALID
#endif



typedef enum
{
    GXMSG_PM_OPEN = GXMSG_TOTAL_NUM,
    GXMSG_PM_CLOSE,
    GXMSG_PM_NODE_NUM_GET,
    GXMSG_PM_NODE_BY_POS_GET,
    GXMSG_PM_MULTI_NODE_BY_POS_GET,
    GXMSG_PM_NODE_BY_ID_GET,
    GXMSG_PM_NODE_ADD,
    GXMSG_PM_NODE_MODIFY,
    GXMSG_PM_NODE_DELETE,
    GXMSG_PM_NODE_GET_URL,
    GXMSG_PM_LOAD_DEFAULT,

    GXMSG_PM_PLAY_BY_POS,
    GXMSG_PM_PLAY_BY_ID,
    GXMSG_TUNER_COPY,
    GXMSG_PM_VIEWINFO_SET,
    GXMSG_PM_VIEWINFO_GET,

    GXMSG_INTERFACE_HELP,
    GXMSG_INTERFACE_LOCK_TS,
    GXMSG_CHECK_PM_EXIST,
    GXMSG_EMPTY_SERVICE_MSG,

    GXMSG_GET_MEDIA_TIME,
    GXMSG_AV_OUTPUT_OFF,

    GXMSG_SUBTITLE_INIT,
    GXMSG_SUBTITLE_DESTROY,
    GXMSG_SUBTITLE_HIDE,
    GXMSG_SUBTITLE_SHOW,
    GXMSG_SUBTITLE_DRAW,
    GXMSG_SUBTITLE_CLEAR,

#ifdef LINUX_OS
#if NETWORK_SUPPORT
    GXMSG_NETWORK_START,
    GXMSG_NETWORK_STOP,

    GXMSG_NETWORK_DEV_LIST,

    GXMSG_NETWORK_GET_STATE,
    GXMSG_NETWORK_STATE_REPORT,

    GXMSG_NETWORK_GET_TYPE,

    GXMSG_NETWORK_GET_CUR_DEV,

    GXMSG_NETWORK_PLUG_IN,
    GXMSG_NETWORK_PLUG_OUT,

    GXMSG_NETWORK_SET_MAC,
    GXMSG_NETWORK_GET_MAC,

    GXMSG_NETWORK_SET_IPCFG,
    GXMSG_NETWORK_GET_IPCFG,

    GXMSG_NETWORK_SET_MODE,
    GXMSG_NETWORK_GET_MODE,

    GXMSG_WIFI_SCAN_AP,
    GXMSG_WIFI_SCAN_AP_OVER,

    GXMSG_WIFI_GET_CUR_AP,
    GXMSG_WIFI_SET_CUR_AP,
    GXMSG_WIFI_DEL_CUR_AP,

    GXMSG_3G_CONNECT,
    GXMSG_3G_DISCONNECT,
    GXMSG_3G_SET_PARAM,
    GXMSG_3G_GET_PARAM,

    GXMSG_PPPOE_SET_PARAM,
    GXMSG_PPPOE_GET_PARAM,
#if (DLNA_SUPPORT > 0)
    GXMSG_DLNA_TRIGGER,
#endif
#endif
#endif

#if TKGS_SUPPORT
/*TKGS msg*/
GXMSG_TKGS_CHECK_BY_PMT,//通过检查PMT中是否有TKGS component descriptor，判断该频点是否是包含TKGS数据的频点， 同步消息;参数：PMT表数据
GXMSG_TKGS_CHECK_BY_TP,//对比保存的location list检查该TP是否是包含TKGS数据的频点， 同步消息；参数：TP id
GXMSG_TKGS_START_VERSION_CHECK, //启动TKGS版本检查，看是否有新的升级， 异步消息;参数：GxTkgsUpdateParm
//GXMSG_TKGS_VERSION_CHECK_FINISH,//版本检查结果,APP处理; 参数：GxTkgsUpdateStatus
GXMSG_TKGS_START_UPDATE,//启动升级， 异步消息；参数：GxTkgsUpdateParm
GXMSG_TKGS_STOP,//强制停止TKGS已经启动的版本检查或升级，异步消息；参数:无
GXMSG_TKGS_UPDATE_STATUS, //升级结束，参数:GxTkgsUpdateMsg, 如果有TKGS消息，需要马上展示给用户，APP处理
GXMSG_TKGS_SET_LOCATION,//设置visible location, 同步消息
GXMSG_TKGS_GET_LOCATION,//获取visible location 列表
//GXMSG_TKGS_LOCATION_SET_HIDDEN_LIST,//设置hidden list， 同步消息
GXMSG_TKGS_SET_OPERATING_MODE,	//设置模式：KGS Off，Customizable，Automatic， 同步消息
GXMSG_TKGS_GET_OPERATING_MODE,	//获取设置的模式：KGS Off，Customizable，Automatic， 同步消息
GXMSG_TKGS_GET_PREFERRED_LISTS, //获取prefered list, 从TKGS数据中获取service list, 供用户选择，同步消息；参数：GxTkgsPreferredList
GXMSG_TKGS_SET_PREFERRED_LIST,//设置用户选择的列表，同步消息; 参数：字符串， service list name
GXMSG_TKGS_GET_PREFERRED_LIST,//获取用户选择的列表，同步消息；参数：字符串，service list name
GXMSG_TKGS_BLOCKING_CHECK,//检查节目是否是block节目，同步消息；参数: 节目ID
//测试相关消息
GXMSG_TKGS_TEST_GET_VERNUM, //获取TKGS表的版本号; 同步消息；参数：版本号
GXMSG_TKGS_TEST_RESET_VERNUM,	//清除机顶盒中保存的TKGS表版本号; 同步消息；参数：无
GXMSG_TKGS_TEST_CLEAR_HIDDEN_LOCATIONS_LIST,	//清除hidden location 列表；同步消息；参数：无
GXMSG_TKGS_TEST_CLEAR_VISIBLE_LOCATIONS_LIST,	//清除visible location 列表；同步消息；参数：无
#endif
    GXMAX_MSG_NUM
}AppMsgId;

// pvr control
//#define PVR_TIMESHIFT_FLAG	    0	/*0.time shift off / 1.time shift on*/
//#define PVR_FILE_SIZE_VALUE     2048/*1024 == 1G ,2048 == 2G, 4096 == 4G*/
//#define PVR_DURATION_VALUE      0   /* 0-2Hour ...*/
 

typedef struct {
	PlayerWindow rect;
	int type;
	int handle;
	int num;
	void* data;
}GxMsgProperty_AppSubtitle;



#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_MEDIA__H__*/


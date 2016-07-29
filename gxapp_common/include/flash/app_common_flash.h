/**
 *
 * @file        app_common_flash.h
 * @brief
 * @version     1.1.0
 * @date        10/17/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_FLASH_H__
#define __APP_COMMON_FLASH_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>
#include "service/gxplayer.h"
#include "gxconfig.h"
#include "gxapp_sys_config.h"

#ifdef ECOS_OS
#define PVR_PARTITION          "/dev/usb01"
#endif
#ifdef LINUX_OS
#define PVR_PARTITION          "/dev/sda1"
#define PVR_TRUE_DIR_NAME          "/GxPvr"
#endif


#define LOG_DISK_SELECT   "log>disk_select"
// PVR Control
#define PVR_PLAYER_TMS_KEY  PLAYER_CONFIG_PVR_TIME_SHIFT_FLAG      
#define PVR_TIMESHIFT_KEY   "pvr>tms_flag"
//#define PVR_FILE_SIZE_KEY   PLAYER_CONFIG_PVR_RECORD_VOLUME_SIZE
#define PVR_DURATION_KEY    "pvr>duration" 
#define PVR_PARTITION_KEY   "pvr>partition"
#define PVR_TRUE_DIR_NAME_KEY   "pvr>true_dir_name"

#define PLAYER_PVR_RECORD_EXIST_OVERLAY  PLAYER_CONFIG_PVR_RECORD_EXIST_OVERLAY
/*subtitle*/
#define SUBTITLE_MODE_KEY   "subtitle>mode"
#define SUBTITLE_LANG_KEY    "subtitle>lang"

#define PVR_TIMESHIFT_FLAG	    0	/*0.time shift off / 1.time shift on*/
#define PVR_FILE_SIZE_VALUE     2048/*1024 == 1G ,2048 == 2G, 4096 == 4G*/



#define VIDEO_DISPLAY_SCREEN        PLAYER_CONFIG_VIDEO_DISPLAY_SCREEN
#define VIDEO_INTERFACE             PLAYER_CONFIG_VIDEO_INTERFACE
#define VIDEO_HDMI_MODE             "VideoHdmiMode"//PLAYER_CONFIG_VIDEO_RESOLUTION
#define VIDEO_ASPECT                PLAYER_CONFIG_VIDEO_ASPECT
#define VIDEO_BRIGHTNESS            PLAYER_CONFIG_VIDEO_BRIGHTNESS
#define VIDEO_SATURATION            PLAYER_CONFIG_VIDEO_SATURATION
#define VIDEO_CONTRAST              PLAYER_CONFIG_VIDEO_CONTRAST
#define VIDEO_QUIET_SWITCH          PLAYER_CONFIG_VIDEO_FREEZE_SWITCH
#define VIDEO_AUTO_ADAPT            PLAYER_CONFIG_VIDEO_AUTO_ADAPT
#define AUDIO_INTERFACE             PLAYER_CONFIG_AUDIO_INTERFACE
#define AUDIO_SPDIF_SOURCE          PLAYER_CONFIG_AUDIO_SPDIF_SOURCE
#define AUDIO_VOLUME                PLAYER_CONFIG_AUDIO_VOLUME
#define AUDIO_TRACK                 PLAYER_CONFIG_AUDIO_TRACK
#define AUDIO_AC3_BYPASS       PLAYER_CONFIG_AUDIO_AC3_MODE
#define VIDEO_COVER_FRAME           PLAYER_CONFIG_VIDEO_H264_COVER_FRAME
#define PLAYER_PVR_RECORD_EXIST_OVERLAY  PLAYER_CONFIG_PVR_RECORD_EXIST_OVERLAY

#define VIDEO_RESOLUTION "DisplayOutput"

#define OSD_LANG					        "OSDLang" //系统语言信息
#define OSD_TRANS 					        "OSDTrans"
#define VIDEOCOLOR_BRIGHTNESS 			    "VideoBrightNess"
#define VIDEOCOLOR_SATURATION 			    "VideoSaturation"
#define VIDEOCOLOR_CONTRAST 			    "VideoContrast"
#define BAR_TIME					          "BarTime"
#define LCN							            "LCN"
#define PASSWORD					          "Password"
#define TIMEZONE					          "TimeZone"
#define FIRST_POWER_ON				      "FirstPowerOn"
#define MAIN_FREQ					          "MainFreq"
#define MAIN_FREQ_SYMRATE                     "MainFreqSymRate"
#define MAIN_FREQ_QAM                     	   "MainFreqQAM"
#define MAIN_FREQ1					          "MainFreq1"
#define MAIN_FREQ_SYMRATE1                     "MainFreqSymRate1"
#define MAIN_FREQ_QAM1                     	   "MainFreqQAM1"
#define MAIN_FREQ_BANDWIDTH                     "MainFreqBANDWIDTH"
#define MAIN_FREQ_NITVERSION                  "MainFreNitVersion"
#define MANUAL_SEARCH_FREQ          "ManualSearchFreq"
#define MANUAL_SEARCH_SYMBOL        "ManualSearchSymbol"
#define MANUAL_SEARCH_QAM           "ManualSearchQAM"
#define MANUAL_SEARCH_BANDWIDTH          "ManualSearchBand"
#define DTMB_DVBC_SWITCH          "DtmbDvbcSwitch"
#define AUDIO_AUDIO_TRACK                 "AudioAudioTrack"
#define PASSWORD_FLAG              "PasswordFlag" 
#define MUTE_FLAG              "MuteFlag" 
#define DTMB_MODE              "DTMBMODE"
#define DVB_CA              "DvbCA" 
#define USER_LIST              "UserList" 
#define TRACK_GLOBLE_FLAG              "TrackGlobleFlag" 
#define VOLUMN_GLOBLE_FLAG "VolumnGlobleFlag"
#define TELTEXT_LANG "TTXLang"
#define SUBTITLE_LANG "SUBLang"
//#define PVR_DURATION_KEY    "pvrduration" 
//#define PVR_TIMESHIFT_KEY   "pvrtms_flag"
//#define SUBTITLE_LANG_KEY   "SubtLang"
//#define SUBTITLE_MODE_KEY   "SubMode"
//#define TELTEXT_LANG_KEY    "TtxLang"
#define FM_FREQ				"FMFreq1"
#define MANAULSEARCH		"manSearch"
#define GOXCEED_RELEASE_VERSION "goxceedVer"
#define SORT_PROG_BY_PAT     "sortprogbypat"


typedef struct {
	int32_t default_video_display_screen;
	int32_t default_video_hdmi_mode;
	int32_t default_video_aspect;
	int32_t default_video_quiet_switch;
	int32_t default_video_auto_adapt;
	int32_t default_audio_volume;
	int32_t default_audio_track;
	int32_t default_audio_audio_track;
	int32_t default_audio_ac3_bypass;
	int32_t default_osd_trans;
	int32_t default_videocolor_brightness;
	int32_t default_videocolor_saturation;
	int32_t default_videocolor_contrast;
	int32_t default_bar_time;
	int32_t default_lcn;
	int32_t default_timezone;
	int32_t default_main_fre_nit_version;
	int32_t default_centre_fre1;
	int32_t default_centre_sym1;
	int32_t default_centre_qam1;
	int32_t default_centre_fre;
	int32_t default_centre_sym;
	int32_t default_centre_qam;	
	int32_t default_manual_fre;
	int32_t default_manual_sym;
	int32_t default_manual_qam;
	int32_t default_manual_bandwidth;
	int32_t default_dtmb_dvbc_switch;
	int32_t default_mute_flag;
	int32_t default_dtmb_flag;
	int32_t default_user_list;
	int32_t default_track_globle_flag;
	int32_t default_volumn_globle_flag;
	int32_t default_dvb_ca;
	int32_t default_password_flag;	
	int32_t default_fm_freq;	
	int32_t default_manualsearch_flag;
	int32_t default_pvr_duration;
	int32_t default_sort_prog_by_pat;
	char* default_password;
	char* default_osd_lang;//默认语言信息
	char* default_subtitle_lang;
	char* default_teltext_lang;
} flash_config_default; 


/*
* other config 
*/
#define CARD_UPDATE_FLAG                    "CardUpdateFlag"
#define CARD_UPDATE_TIME                    "CardUpdateTime"

/*0 :无CA 智能卡升级信息1: 智能卡升级成功2: 智能卡升级失败*/
#define CARD_UPDATE_FLAG_DV                    0
#define CARD_UPDATE_TIME_DV                    "2000-01-01 00:00"



typedef enum
{
	VIDEO_FORMAT_AUTO = 0,
	VIDEO_FORMAT_PAL,
	VIDEO_FORMAT_NTSC,
       VIDEO_FORMAT_MAX
}Video_Format_Mode_t;

typedef enum
{
	LCN_STATE_OFF = 0, /*LCN关闭*/
	LCN_STATE_ON,
}Lcn_State_t;

typedef enum
{
	VIDEO_SWITCH_BACK = 0, /* 黑屏切台*/
	VIDEO_SWITCH_STILL       /* 静帧切台*/
}Video_Queit_Switch_t;

typedef enum
{
	VIDEO_DISPLAY_SCREEN_4X3 = 0, /* 4:3*/
	VIDEO_DISPLAY_SCREEN_16X9       /* 16:9*/
}Video_Display_Screen_t;


#define FILE_MAGIC      (0x5A5A)
typedef struct tag_file_info_st{
    uint16_t u16Magic;
    uint16_t u16UnitSize;//结构单元大小
    uint32_t u32Size;//个数
    char filename[32];
    uint8_t  data[0];
}FILE_INFO;



#define VIDEO_ASPERCT_NORMAL          0
//#define VIDEO_ASPERCT_PANSCAN       1
#define VIDEO_ASPERCT_LETTERBOX       1

#define VIDEO_SWITCH_EFFECT_CVBS 0
#define VIDEO_SWITCH_EFFECT_YPBPR 1

#define LANGUAGE_NAME_MAX (20)

#if defined(LINUX_OS)
int32_t app_flash_linux_partion_init(const char* partion_name);
int32_t app_flash_linux_partion_save(const char* partion_name);
char* app_flash_linux_oem_read(const char* partion_name,const char* section, const char* parameter);
int app_flash_linux_oem_write(const char* partion_name,const char* section, const char* parameter, const char* value);
#endif

int32_t app_flash_partion_info_by_name(const char *part_name,uint32_t *paddr,uint32_t *plen,char *pdevName);


/*
*   oem 参数读取
*/
char * app_flash_get_oem_bootversion_str(void);
char * app_flash_get_oem_softversion_str(void);

char* app_flash_get_oem_regionversion_str(void);

uint32_t app_flash_get_oem_platform_id(void);

char* app_flash_get_oem_manufacture_id(void);

char *   app_flash_get_oem_hardware_version_str(void);

uint32_t app_flash_get_oem_serial_number(void);
/*
* get demod type
*/
char *  app_flash_get_oem_fe_demod_type(void);

/*
* get demux src
*/
char * app_flash_get_oem_demux_source(void);

/*
* get fe tuner type
*/
uint32_t app_flash_get_oem_fe_tuner_type(void);


/*
*   oem 参数设置/保存
*/
void       app_flash_save_oem();

uint32_t app_flash_set_oem_soft_updateversion(char* Value);

uint32_t app_flash_set_oem_dmx_pid(char* Value);

uint32_t app_flash_set_oem_dmx_tableid(char* Value);
uint32_t app_flash_set_oem_dmx_frequency(char* Value);
uint32_t app_flash_set_oem_dmx_serviceId(char* Value);
/*
* set demux source 
*/
uint32_t app_flash_set_oem_dmx_source(char* Value);

/*
* set fe demod type 
*/
uint32_t app_flash_set_oem_fe_demod_type(char* Value);

/*
* set tuner type
*/
uint32_t app_flash_set_oem_fe_tuner_type(char* Value);




uint32_t app_flash_set_oem_repeat_times(char* Value);

uint32_t app_flash_set_oem_fe_modulation(char* Value);


uint32_t app_flash_set_oem_fe_symbolrate(char* Value);

uint32_t app_flash_set_oem_fe_workmode(char* Value);

uint32_t app_flash_set_oem_ota_update_type(char* Value);

uint32_t app_flash_set_oem_ota_flag(char* Value);
/*
* 获取是否升级标志
*/
char *  app_flash_get_oem_ota_flag(void);



/*
* config 参数读取/保存
*/

int32_t app_flash_set_default_config_para(flash_config_default config_default);

/*
* 保存默认参数
*/
int32_t app_flash_save_default_config_para(void);


int32_t app_flash_get_config_timezone(void);
int32_t app_flash_save_config_timezone(int32_t config);

Lcn_State_t app_flash_get_config_lcn_flag(void);
int32_t app_flash_save_config_lcn_flag(Lcn_State_t config);

int32_t app_flash_get_user_list_flag(void);

char*  app_flash_get_config_teltext_language(void);
int32_t app_flash_save_config_teltext_language(const char* language);

char*  app_flash_get_config_subtitle_language(void);
int32_t app_flash_save_config_subtitle_language(const char* language);


char*  app_flash_get_config_osd_language(void);
int32_t app_flash_save_config_osd_language(const char* language);

/*
* 获取ac3 bypass 音频输出开关状态
*/
int32_t app_flash_get_config_audio_ac3_bypass(void);

/*
* 设置ac3 bypass 音频输出开关状态
*/
int32_t app_flash_save_config_audio_ac3_bypass(int32_t config);

/*
* 获取全局声道标志
*/
int32_t app_flash_get_config_track_globle_flag(void);

/*
* 设置全局声道标志
*/
int32_t app_flash_save_config_track_globle_flag(int32_t config);

int32_t app_flash_get_config_audio_track(void);
int32_t app_flash_save_config_audio_track(int32_t config);

int32_t app_flash_get_config_audio_audio_track(void);
int32_t app_flash_save_config_audio_audio_track(int32_t config);
int32_t app_flash_get_config_volumn_default_globle_flag(void);

/*
* 获取全局音量标志
*/
int32_t app_flash_get_config_volumn_globle_flag(void);
/*
* 设置全局音量标志
*/
int32_t app_flash_save_config_volumn_globle_flag(int32_t config);


int32_t app_flash_get_config_audio_volume(void);
int32_t app_flash_save_config_audio_volume(int32_t config);

int32_t app_flash_get_config_osd_trans(void);
int32_t app_flash_save_config_osd_trans(int32_t config);

int32_t app_flash_get_config_videocolor_brightness(void);
int32_t app_flash_save_config_videocolor_brightness(int32_t config);

int32_t app_flash_get_config_videocolor_saturation(void);
int32_t app_flash_save_config_videocolor_saturation(int32_t config);

int32_t app_flash_get_config_videocolor_contrast(void);
int32_t app_flash_save_config_videocolor_contrast(int32_t config);


int32_t app_flash_get_config_mute_flag(void);
int32_t app_flash_save_config_mute_flag(int32_t config);


int32_t app_flash_get_config_bar_time(void);
int32_t app_flash_save_config_bar_time(int32_t config);

int32_t app_flash_get_config_center_nit_fre_version(void);
int32_t app_flash_save_config_center_nit_fre_version(int32_t config);

int32_t app_flash_get_config_password_flag(void);
int32_t app_flash_save_config_password_flag(int32_t config);

/*
* 获取视频宽高比模式
*/
int32_t app_flash_get_config_video_aspect(void);

/*
* 保存视频宽高比模式
*/
int32_t app_flash_save_config_video_aspect(int32_t config);



/*
* 获取hdmi视频制式
*/
int32_t app_flash_get_config_video_hdmi_mode(void);

/*
* 保存hdmi视频制式
*/
int32_t app_flash_save_config_video_hdmi_mode(int32_t config);

Video_Display_Screen_t app_flash_get_config_video_display_screen(void);
int32_t app_flash_save_config_video_display_screen(Video_Display_Screen_t config);

int32_t app_flash_get_config_video_auto_adapt(void);
int32_t app_flash_save_config_video_auto_adapt(int32_t config);

int32_t app_flash_get_config_video_quiet_switch(void);
int32_t app_flash_save_config_video_quiet_switch(int32_t config);


uint32_t app_flash_get_config_center_freq(void);
uint32_t app_flash_save_config_center_freq(int32_t nFrequcncy);


int32_t app_flash_get_config_manual_search_freq(void);

uint32_t app_flash_save_config_manual_search_freq(int32_t nFrequcncy);

int32_t app_flash_get_config_manual_search_symbol_rate(void);

uint32_t app_flash_save_config_manual_search_symbol_rate(int32_t nValue);

int32_t app_flash_get_config_manual_search_qam(void);

uint32_t app_flash_save_config_manual_search_qam(int32_t nValue);

int32_t app_flash_get_config_manual_search_bandwidth(void);

uint32_t app_flash_save_config_manual_search_bandwidth(int32_t nValue);

int32_t app_flash_get_config_dtmb_dvbc_switch(void);
uint32_t app_flash_save_config_dtmb_dvbc_switch(int32_t nValue);
int32_t app_flash_save_config_password(const char * password);

int32_t app_flash_get_config_password(char *password,int32_t passwordlen);

/*
* 获取当前CA类型(兼容双CA系统)
*/
int32_t app_flash_get_config_dvb_ca_flag(void);
/*
* 设置当前CA类型(兼容双CA系统)
*/
int32_t app_flash_save_config_dvb_ca_flag(int32_t config);
/*
* flash 读取序列号
*
*/

uint32_t app_flash_get_config_center_freq1(void);
uint32_t app_flash_save_config_center_freq1(int32_t nFrequcncy);
int32_t app_flash_get_config_center_freq_symbol_rate1(void);
uint32_t app_flash_save_config_center_freq_symbol_rate1(int32_t nValue);
int32_t app_flash_get_config_center_freq_qam1(void);
uint32_t app_flash_save_config_center_freq_qam1(int32_t nValue);
int32_t app_flash_get_config_pvrduration(void);
uint32_t app_flash_save_config_pvrduration(int32_t nValue);


int32_t app_flash_get_config_gx150x_mode_flag(void);

int32_t app_flash_save_config_gx150x_mode_flag(int32_t config);

int32_t app_flash_get_config_center_freq_symbol_rate(void);
uint32_t app_flash_save_config_center_freq_symbol_rate(int32_t nValue);
int32_t app_flash_get_config_center_freq_qam(void);
uint32_t app_flash_save_config_center_freq_qam(int32_t nValue);
int32_t app_flash_get_config_center_freq_bandwidth(void);
uint32_t app_flash_save_config_center_freq_bandwidth(int32_t nValue);
int32_t app_flash_get_config_sort_by_pat_flag(void);

int32_t app_flash_get_serial_number(int32_t addr, char * serial,int32_t maxLen);
int32_t app_flash_save_logo_data(const char* data,uint32_t len);
int32_t app_flash_get_ad_data_to_ddram_file(const char* flash_file_name,const char* file_name);
int32_t app_flash_save_ad_data_to_flash_file(const char* data,uint32_t len,const char* flash_file_name);
int32_t app_save_data_to_ddram_file(const char* data,uint32_t len,const char* file_name);

int32_t app_file_init(int ReCreateFlag,const char *file_name,uint16_t UnitSize);
int32_t app_file_read(const char *file_name,uint8_t *buf,uint32_t maxlen,uint32_t *readlen);
int32_t app_file_write(const char *file_name,const uint8_t *buf,uint32_t buflen);
uint32_t app_get_fm_freq(void);
uint32_t app_set_fm_freq(int32_t nFrequcncy);
uint32_t app_get_manual_search_flag(void);
void app_set_manual_search_flag(int32_t flag);

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_FLASH_H__*/


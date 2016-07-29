#ifndef __PMP_SETTING_H__
#define __PMP_SETTING_H__

#include "gxcore.h"
#include "module/config/gxconfig.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	/*GLOBAL*/
	PMPSET_LANG,
	PMPSET_DISPLAY_SCREEN,
	PMPSET_POWER_ON_PALY,
	PMPSET_LAST_PLAY_PATH,
	//PMPSET_POWER_OFF_AUTO,
	PMPSET_SAVE_TAGS,
	PMPSET_FACTORY_DEFAULT,

	/*AV*/
	PMPSET_VOLUME,//0-30
	PMPSET_OUTPUT_MODE,	
	PMPSET_VIDEO_FORMAT,
	PMPSET_ASPECT_RATIO,
	PMPSET_AUDIO_TRACK,
	PMPSET_MUTE,

	/*MOVIE*/
	PMPSET_MOVIE_PLAY_SEQUENCE,
	PMPSET_MOVIE_SUBT_VISIBILITY,

	/*MUSIC*/
	PMPSET_MUSIC_PLAY_SEQUENCE,
	PMPSET_MUSIC_VIEW_MODE,

	/*PIC*/
	PMPSET_PIC_SWITCH_DURATION,
	PMPSET_PIC_SWITCH_MODE,
	PMPSET_PIC_PLAY_SEQUENCE,
	
	/*TEXT*/
	PMPSET_TEXT_ROLL_LINES,
	PMPSET_TEXT_AUTO_ROLL,
	
	/*NETWORK*/
}pmpset_property;



typedef enum
{
	PMPSET_LANG_ENGLISH,
	PMPSET_LANG_CHINESE
}pmpset_lang;

typedef enum
{
	PMPSET_DISPLAY_SCREEN_720_576,
	PMPSET_DISPLAY_SCREEN_1280_720
}pmpset_display;

typedef enum
{
	PMPSET_TONE_OFF,
	PMPSET_TONE_ON
}pmpset_tone;


typedef enum
{
	PMPSET_MUTE_OFF=0,
	PMPSET_MUTE_ON
}pmpset_mute;


typedef enum
{
	PMPSET_OUTPUT_MODE_RCA,
	PMPSET_OUTPUT_MODE_VGA,
	PMPSET_OUTPUT_MODE_YUV,
	PMPSET_OUTPUT_MODE_DVI,
	PMPSET_OUTPUT_MODE_HDMI,
	PMPSET_OUTPUT_MODE_SCART ,
	PMPSET_OUTPUT_MODE_SVIDEO,
	PMPSET_OUTPUT_MODE_LCD  
}pmpset_output_mode;

typedef enum
{
	PMPSET_VIDEO_FORMAT_AUTO, //extern by app

	// syc to gxplayer bus
	
	//RCA,  SCART, SVIDEO
	PMPSET_VIDEO_FORMAT_PAL                       = 1 ,  //composite PAL-BDGHI £­£­£­£­PAL
	PMPSET_VIDEO_FORMAT_PAL_M                     = 2 ,  //composite PAL-M     
	PMPSET_VIDEO_FORMAT_PAL_N                     = 3 ,  //composite PAL-N     
	PMPSET_VIDEO_FORMAT_PAL_NC                    = 4 ,  //composite PAL-Nc        
	PMPSET_VIDEO_FORMAT_NTSC_M                    = 5 ,  //composite NTSC-M  £­£­£­NTSC 
	PMPSET_VIDEO_FORMAT_NTSC_443                  = 6 ,  //composite NTSC-4.43 


	//YUV
	PMPSET_VIDEO_FORMAT_YCBCR_480I                = 7 ,  //component interlaced 525-line system (YCbCr/RGB)
	PMPSET_VIDEO_FORMAT_YCBCR_576I                = 8 ,  //component interlaced 625-line system (YCbCr/RGB)

	PMPSET_VIDEO_FORMAT_YCBCR_1080I_50HZ          = 9 , 
	PMPSET_VIDEO_FORMAT_YCBCR_1080I_60HZ          = 10 , 

	PMPSET_VIDEO_FORMAT_YPBPR_480P                = 11 , //component progressive 525-line system (YPbPr)
	PMPSET_VIDEO_FORMAT_YPBPR_576P                = 12 , //component progressive 625-line system (YPbPr)

	PMPSET_VIDEO_FORMAT_YPBPR_720P_50HZ           = 13 , 
	PMPSET_VIDEO_FORMAT_YPBPR_720P_60HZ           = 14 , 
	PMPSET_VIDEO_FORMAT_YPBPR_1080P_50HZ          = 15 , 
	PMPSET_VIDEO_FORMAT_YPBPR_1080P_60HZ          = 16 , 

	//VGA
	PMPSET_VIDEO_FORMAT_VGA_480P                  = 17 , //component progressive 525-line system (VGA)
	PMPSET_VIDEO_FORMAT_VGA_576P                  = 18 , //component progressive 625-line system (VGA)

	//LCD
	PMPSET_VIDEO_FORMAT_DIGITAL_RGB_720x480_0_255 = 19 ,
	PMPSET_VIDEO_FORMAT_DIGITAL_RGB_320x240_0_255 = 20 ,
	PMPSET_VIDEO_FORMAT_DIGITAL_RGB_16_235        = 21 ,    

	//HDMI
	PMPSET_VIDEO_FORMAT_BT656_YC_8BITS            = 22 , 
	PMPSET_VIDEO_FORMAT_BT656_YC_10BITS           = 23 ,
	PMPSET_VIDEO_FORMAT_MODE_MAX                  = 24
}pmpset_video_format;




typedef enum
{
	PMPSET_ASPECT_RATIO_AUTO,
	PMPSET_ASPECT_RATIO_FULL_SCREEN,
	PMPSET_ASPECT_RATIO_ORIGINAL_SIZE,
	PMPSET_ASPECT_RATIO_4_3,
	PMPSET_ASPECT_RATIO_16_9
}pmpset_aspect_ratio;




typedef enum
{
	PMPSET_AUDIO_TRACK_STEREO,
	PMPSET_AUDIO_TRACK_LEFT,
	PMPSET_AUDIO_TRACK_RIGHT
}pmpset_audio_track;


typedef enum
{
	PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE=0,
	PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ONE,
	//PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ALL,
	PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE,
	//PMPSET_MOVIE_PLAY_SEQUENCE_RANDOM	
}pmpset_movie_play_sequence;


typedef enum
{
	PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE=0,
	PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE,
	//PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ALL,
	PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE,
	//PMPSET_MUSIC_PLAY_SEQUENCE_RANDOM
}pmpset_music_play_sequence;


typedef enum
{
	PMPSET_MUSIC_VIEW_MODE_ID3=0,
	PMPSET_MUSIC_VIEW_MODE_LRC,
	PMPSET_MUSIC_VIEW_MODE_SPECTRUM
}pmpset_music_view_mode;

typedef enum
{
	PMPSET_PIC_SWITCH_DURATION_3s=0,
	PMPSET_PIC_SWITCH_DURATION_5s,
	PMPSET_PIC_SWITCH_DURATION_7s,
	PMPSET_PIC_SWITCH_DURATION_10s
}pmpset_pic_switch_duration;

typedef enum
{
	PMPSET_PIC_SWITCH_MODE_DEFAULT=0,
	PMPSET_PIC_SWITCH_MODE_LR,
	PMPSET_PIC_SWITCH_MODE_RL
}pmpset_pic_switch_mode;

typedef enum
{	
	PMPSET_PIC_PLAY_SEQUENCE_SEQUENCE=0,
	PMPSET_PIC_PLAY_SEQUENCE_RANDOM
}pmpset_pic_play_sequence;



#define PMPSET_SAVE_VOLUME				PLAYER_CONFIG_AUDIO_VOLUME
#define PMPSET_SAVE_OUTPUT_MODE			PLAYER_CONFIG_VIDEO_INTERFACE
#define PMPSET_SAVE_ASPECT_RATIO			PLAYER_CONFIG_VIDEO_ASPECT
//#define PMPSET_SAVE_DISPLAY_SCREEN		PLAYER_CONFIG_VIDEO_DISPLAY_SCREEN
#define PLAYER_CONFIG_VIDEO_RESOLUTION		VIDEO_HDMI_MODE
#define PMPSET_SAVE_VIDEO_FORMAT			PLAYER_CONFIG_VIDEO_RESOLUTION
#define PMPSET_SAVE_VIDEO_AUTO_ADAPT	PLAYER_CONFIG_VIDEO_AUTO_ADAPT
#define PMPSET_SAVE_AUDIO_TRACK			PLAYER_CONFIG_AUDIO_TRACK
#define PMPSET_SAVE_MUTE					"pmp>mute"
#define PMPSET_SAVE_LANG					"pmp>language"
#define PMPSET_SAVE_DISPLAY_SCREEN				"pmp>display_screen"
#define PMPSET_SAVE_POWER_ON_PALY		"pmp>power_on_play"
#define PMPSET_SAVE_LAST_PLAY_PATH		"pmp>last_play_path"	
#define PMPSET_SAVE_SAVE_TAGS				"pmp>tags"
#define PMPSET_SAVE_FACTORY_DEFAULT		"pmp>factory_default"
#define PMPSET_SAVE_MOVIE_PLAY_SEQUENCE	"pmp>movie_sequence"
#define PMPSET_SAVE_MOVIE_SUBT			"pmp>movie_subt"
#define PMPSET_SAVE_MUSIC_PLAY_SEQUENCE	"pmp>music_sequence"
#define PMPSET_SAVE_MUSIC_VIEW_MODE		"pmp>music_view_mode"
#define PMPSET_SAVE_PIC_SWITCH_DURATION  "pmp>pic_switch_duration"
#define PMPSET_SAVE_PIC_SWITCH_MODE		"pmp>pic_switch_mode"
#define PMPSET_SAVE_PIC_PLAY_SEQUENCE      "pmp>pic_play_sequence"
#define PMPSET_SAVE_TEXT_ROLL_LINES		"pmp>text_roll_lines"
#define PMPSET_SAVE_TEXT_AUTO_ROLL      "pmp>text_auto_roll"

status_t pmpset_exit(void);
status_t pmpset_init(void);
status_t pmpset_factory_default(void);

status_t pmpset_set_int(pmpset_property property, int32_t value);
int32_t  pmpset_get_int(pmpset_property property);

status_t pmpset_set_str(pmpset_property property, char* str);
status_t pmpset_get_str(const char* property, char** str, int32_t str_len);






#ifdef __cplusplus
}
#endif

#endif /* __PMP_SETTING_H__ */


/**
 *
 * @file        app_common_play.h
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PLAY__H__
#define __APP_COMMON_PLAY__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>

#include "gxbus.h"
#include "gui_event.h"
#include "gxavdev.h"
#include "service/gxplayer.h"
#include "service/gxsi.h"
#include "app_common_flash.h"
#include "app_common_ad.h"
#include "gxmsg.h"
#include "gxapp_sys_config.h"

#define MAX_MSG_NUM 1000
#define PLAYER0 "player0"
#define PLAYER2 "player2"
#define PLAYER3 "player3"

typedef struct
{
	GxMessage *msg;
	void *param;
}app_msg;


typedef struct player
{
	const char *    player;
	uint8_t    param;
	uint32_t   num;
}player_t;

typedef struct
{
	uint32_t num; /*节目频道号*/
}play_para;

typedef struct
{
	uint8_t startupProgFlag; /*重启是否播放指定节目
	                                        startupProgFlag 为FALSE，播放断电记忆节目*/
	uint8_t  stream_type; /*开机播放指定节目类型*/
	uint16_t startupProgNum; /* 开机播放固定节目号*/
}startup_play_para;

typedef enum MsgPop
{
	MSG_POP_NONE = 0,
	MSG_POP_INVALID_PROG,
	MSG_POP_NO_FAV_PROG,
	MSG_POP_PASSWORD_ERR,
	MSG_POP_NO_PROG,
	MSG_POP_SIGNAL_BAD,
	MSG_POP_PROG_LOCK,
	MSG_POP_PROG_SKIP,
	MSG_POP_PROG_SCRAMBLE,//加扰节目
	
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
	MSG_POP_NO_CARD,
	MSG_POP_INVALID_CARD,
#endif
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	MSG_POP_PROG_NEED_PAY,
#endif
	MSG_POP_COMMON_END   /*此定义未用到，标志普通消息最大个数*/
}MsgPop_e;

typedef enum
{
    PLAY_KEY_DUMMY,
    PLAY_KEY_LOCK,
    PLAY_KEY_UNLOCK
}AppPlayKey;



#define MESSAGE_MAX_COUNT (64)



typedef struct play_scramble_para
{
	u_int16_t program_num;
	bool program_type ;
	uint8_t  scramble_type ;
	u_int16_t p_video_pid ;//解扰
	u_int16_t p_audio_pid ;//解扰
	u_int16_t p_ecm_pid ;//ecm 过滤器相关
	u_int16_t p_ecm_pid_video ;
	u_int16_t p_ecm_pid_audio ;
	u_int16_t p_emm_pid ;//当前节目所在频点emm-pid
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	u_int16_t pmt_pid;
	#endif
} play_scramble_para_t;


int32_t app_send_msg(uint32_t msg_type,void *param);
app_msg * app_get_msg(uint32_t msg_id);

int32_t app_play_startup_prog(startup_play_para playpara);
int32_t  app_play_switch_tv_radio(void);

uint32_t app_play_recall_prog(void);
uint32_t app_play_by_direction(int32_t direction);
 void app_play_set_cur_prog_track(void);
 void app_play_set_cur_prog_volumn(void);
void app_play_reset_play_timer(uint32_t duration);
void app_play_remove_play_tmer(void);
/*退出、进入视频播放界面，检查是否存在等待超时播放的定时器
   如存在，退出、进入视频播放界面前，先处理播放*/
void app_play_check_play_timer(void);


void app_free_msg(uint32_t msg_type,uint32_t msg_id);



/*
* 以下接口同时提供menu调用
*/
typedef void (*prompt_show_msg_callback)(void);
void app_play_register_show_prompt_msg_callback(prompt_show_msg_callback promptshowmsgcallback);
void app_play_register_password_widget_window(const char* password_widget);
/*
* 获取当前音视频是否播放状态
*/
uint8_t app_play_get_play_status(void);

/*
* 获取当前音视频是否成功解码状态
*/
uint8_t app_play_get_running_status(void);

/*
* 设置提示信息刷新标志，菜单变化需要重新提示
*/
void app_play_set_prompt_reflush_flag(uint32_t flag);

/*
* 获取提示信息刷新标志，菜单变化需要重新提示
*/
uint32_t app_play_get_prompt_reflush_flag(void);


/*
* 设置当前音视频是否成功解码状态
*/
uint8_t app_play_set_running_status(uint8_t status);


/*
* 停止音/视频播放，如为音频列表中切换节目保留背景图片
*/
void app_play_stop_audio(void);

/*
* 停止音视频播放，同时关闭PLAYER3，PLAYER0
* 不需判断当前节目类型
*/
void app_play_stop(void);

/*
* 播放音视频
*/
void app_play_video_audio(int32_t pos);

void app_play_video_audio_ds(int32_t pos);
int32_t app_play_playingpos_in_group(void);
/*
* 显示音频背景图片
*/
void app_play_show_logo_for_tv(void);
void app_play_show_logo(void);
uint32_t app_play_i_frame_logo(int32_t VideoResolution,char* path);
/*
* 关闭开机画面
*/
uint32_t app_play_close_av_logo(void);

/*
* 关闭视频层
*/
void app_play_hide_video_layer(void);
/*
* 打开视频层
*/
void app_play_show_video_layer(void);

/*
* 设置视频缩放参数大小
*/
void app_play_set_zoom_para(unsigned int x,unsigned int y,unsigned int w,unsigned int h);

/*
* 视频窗口缩放
*/
void app_play_video_window_zoom(unsigned int x,unsigned int y,unsigned int w,unsigned int h);

/*
* 视频窗口放大到全屏
*/
void app_play_video_window_full(void);

/*
* 播放广告图片接口
* 支持按X，Y播放\居中播放
*/
status_t app_play_ad_flash_file(ad_play_para *playpara);
status_t app_play_ad_ddram_file(ad_play_para *playpara);
/*
* 关闭广告图片接口
*/
status_t app_play_stop_ad(ad_play_para playpara);

/*
* 清除播放相关提示
*/
void app_play_switch_prog_clear_msg(void);

/*
* 清除播放CA相关提示
*/
void app_play_clear_ca_msg(void);
/*
* 设置播放消息类型
*/
void app_play_set_msg_pop_type_flag(MsgPop_e type);

/*
* 设置播放消息类型
*/
void app_play_set_msg_pop_type(MsgPop_e type);

/*
* 设置当前消息类型
*/
void app_play_set_msg_pop_type_record_pos(MsgPop_e type);
/*
* 清除指定类型的消息提示
*/
void app_play_clear_msg_pop_type_flag(MsgPop_e type);


/*
* 清除指定类型的消息提示
*/
void app_play_clear_msg_pop_type(MsgPop_e type);

/*
* 获取指定类型的消息提示状态
*/
uint32_t app_play_get_msg_pop_type_state(MsgPop_e type);

/*
* 设置视频宽高比模式
*/
int32_t app_play_set_video_aspect(VideoAspect nAspect);

/*
* 设置宽高比4X3\16X9
*/
int32_t app_play_set_video_display_screen(	Video_Display_Screen_t n4To3_16To9);

int32_t app_play_set_channel_switch(int32_t channel_switch);

/*
* 设置声道
*/
void app_play_set_audio_track(int audio_track);

/*
* 设置音量
*/
void app_play_set_volumn(int32_t Volume);

/*
* 码流自播放自适应分辨率变化，更新flash分辨率保存
*/
int32_t app_play_update_flash_video_resolution(VideoOutputMode video_resution);
/*
* 设置rca1
*/

int32_t app_play_set_rca1_mode(int32_t VideoResolution);

/*
* 设置hdmi视频分辨率
*/
int32_t app_play_set_hdmi_mode(int32_t VideoResolution);
uint8_t app_play_get_mute_flag(void);
/*
* 设置、取消静音
*/
void app_play_set_mute(int32_t mute);

/*
* AC3 BYPASS音频输出开关设置
*/
void app_play_set_audio_ac3_bypass_onoff(int32_t onoff);

/*
* 设置中英文
*/
void app_play_set_osd_language(char* language);

/*
* 设置透明度
*/
void app_play_set_osd_trasn_level(uint32_t nTransLeve);
void app_play_set_videoColor_level(VideoColor color);

/*
* register dolby support
*/
void app_play_register_dolby(void);
uint8_t app_play_get_playing_para(play_scramble_para_t* playPara);
uint8_t app_play_set_playing_para(play_scramble_para_t* playPara);

/*
*  待机前设置静音电路gpio静音，避免待机爆音
*/
uint8_t app_play_set_gpio_mute(void);

/*
*  开机初始化设置静音电路gpio输出声音
*/
uint8_t app_play_set_gpio_unmute(void);

int app_play_get_signal_status(void);
void app_play_set_signal_status(int ri_SignalStatus);

int app_play_get_popmsg_status();
void app_play_set_popmsg_status(int ri_Popmsg_Type);

/*
* 视频层置顶
*/
void app_play_set_zoom_video_top(void);


#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PLAY__H__*/


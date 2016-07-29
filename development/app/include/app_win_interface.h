#ifndef __MSG_TO_SERVICE_H__
#define __MSG_TO_SERVICE_H__
#include "gui_core.h"
#include "gxgui_view.h"
#include "gui_timer.h"
#include "gui_event.h"
#include "gui_key.h"
#include "gxbus.h"
#include "gxmsg.h"
#include "app_popmsg.h"
#include "gxprogram_manage_berkeley.h"
#include <string.h>
#include "gxtype.h"
#include "gxavdev.h"
#include "service/gxsi.h"
#include "service/gxepg.h"
#include "service/gxplayer.h"
#include "service/gxsearch.h"
#include "service/gxconsole.h"

typedef enum window
{
	FULL_SCREEN_WIN = 1,
	PROGRAM_TV_LIST_WIN,
	FAVORITE_LIST_WIN,
	WEEKLY_EPG_WIN,
	PROGRAM_EDIT_WIN,
	MAIN_MENU_WIN,
	SEARCH_RESULT_WIN,
	PROGRAM_BAR_WIN,
	POP_MSG_WIN,
	MANAGE_MENU_WIN,
	MEDIA_CENTER_MENU_WIN,	
	PROG_NUM_WIN,
	USB_UPDATE_WIN,
	MEDIA_MOVIE_WIN,
	MEDIA_MUSIC_WIN,
	MEDIA_PIC_WIN,
	MEDIA_TEXT_WIN,
	PVR_MEDIA_WIN,
	PROGRAM_MENU_WIN,
	SEARCH_MENU_WIN,
	SYS_SERVICE_MENU_WIN,
	SYS_SET_MENU_WIN,
	SYS_SET_SUB_MENU_WIN,
	SYS_INSTALL_MENU_WIN,
	SYS_CA_MENU_WIN,
	MAX_WINDOW_NUM
}focus_window_e;


typedef enum
{
	FRE_BEGIN = 1,
	FRE_END,
	FRE_MID,
}FRE_TYPE_E;

/*
* xxxxxxxx格式字符串转化成数值
*/
unsigned int htoi(const char *str);

/*
* xx.xx.xx.xx格式字符串转化成数值
*/
unsigned int convert_version_str(const char *str);

uint8_t app_set_win_create_flag(focus_window_e win);
uint8_t app_set_win_destroy_flag(focus_window_e win);
uint8_t app_get_win_create_flag(focus_window_e win);

uint8_t app_set_pop_msg_flag_status(uint8_t flag);
uint8_t app_get_pop_msg_flag_status(void);

uint8_t app_set_nit_change_for_search_status(uint8_t flag);
uint8_t app_get_nit_change_for_search_status(void);

uint8_t app_win_is_full_window(void);

focus_window_e app_win_get_focus_video_window(void);
void app_win_set_focus_video_window(focus_window_e win);
char* app_win_get_local_date_string(void);
char* app_win_get_local_time_string(void);
void app_win_exist_to_full_screen(void);
void app_win_update_video_focus_window(void);

int app_win_update_current_time(uint32_t duration);
int app_win_check_fre_vaild(FRE_TYPE_E type,uint32_t fre);
int app_win_check_sym_vaild(uint32_t sym);
int app_win_check_fre_range_vaild(uint32_t lowfre,uint32_t highfre);

/*
* 退出到指定界面后，响应对应的遥控器或创建指定的窗体
*/
void app_win_exist_to_win_widget(const char* widget_win_name);

/*
* 校验密码是否正确
* widget_password_widget -- 密码EDIT控件名称
* passwordLen -- 密码长度
*/
int32_t app_win_check_password_valid(const char* widget_password_widget,int32_t passwordLen);

/*
* 响应自动搜索OK键
*/
int app_win_auto_search(void);

/*
*  响应恢复出厂设置OK键
*/
int app_win_reset_factory(void);
#endif


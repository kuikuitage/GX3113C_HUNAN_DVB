#ifndef __APP_H__
#define __APP_H__



#include <gxtype.h>
#include "gui_core.h"
#include "gxgui_view.h"
#include "gui_timer.h"
#include "gui_event.h"
#include "gui_key.h"
#include "gxbus.h"
#include "gxmsg.h"
#include "app_key.h"
#include "gxprogram_manage_berkeley.h"
#include "gxbook.h"
#include "gxconfig.h"
#include "gxapp_sys_config.h"
#include "sys_common_config.h"
#include "app_win_interface.h"
#include "app_popmsg.h"
#include "app_call_back.h"
//#include "stb_panel.h"
#include "sys_menu_config.h"
#include "gxfrontend.h"
#include "app_common_flash.h"
#include "app_common_book.h"
#include "app_common_search.h"
#include "app_common_play.h"
#include "app_common_prog.h"
#include "app_common_epg.h"
#include "app_common_init.h"
#include "app_common_nvod.h"
#if DVB_MEDIA_FLAG
#include "app_common_media.h"
#endif
#include "app_common_table_nit.h"
#include "app_common_table_pmt.h"
#include "app_common_table_cat.h"
#include "app_common_table_ota.h"
#include "app_common_table_pat.h"
#include "app_common_lcn.h"
#include "app_common_porting_stb_api.h"
#include "app_file_list.h"
#include "app_common.h"
#include "app_str_id.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PMP_ATTACH_DVB

/*wangjian add on 20141209*/
#define BOOT_PLAY_IFRAME_LOGO

#ifdef BOOT_PLAY_IFRAME_LOGO
#define LOGO_IFRAME_PATH  ("/theme/logo.bin")
#endif
/*wangjian add end.*/

#define __STB_DEBUG__

#ifndef __STB_DEBUG__
#define APP_Printf(...)     {;}
#define APP_Printf_Blue(...) {;}
#else
#define APP_Printf(...)      {printf("[%s]l-%d",__FUNCTION__,__LINE__); printf( __VA_ARGS__ );}
#define APP_Printf_Blue(...)	{printf("\033[036m");\
					printf(__VA_ARGS__);\
					printf("\033[0m");}
#endif

#define KEY_OK              GUIK_RETURN
#define KEY_MENU            GUIK_M
#define KEY_EXIT            GUIK_ESCAPE
#define KEY_RECALL          GUIK_BACKSPACE
#define KEY_TV_RADIO        GUIK_T
#define KEY_TV              GUIK_F11
#define KEY_RADIO           GUIK_F12

#define KEY_MUTE            GUIK_HOME


#define KEY_FAV             GUIK_F
#define KEY_EPG             GUIK_E

#define KEY_LEFT            GUIK_LEFT
#define KEY_RIGHT           GUIK_RIGHT
#define KEY_UP              GUIK_UP
#define KEY_DOWN            GUIK_DOWN

#define KEY_1               GUIK_1
#define KEY_2               GUIK_2
#define KEY_3               GUIK_3
#define KEY_4               GUIK_4
#define KEY_5               GUIK_5
#define KEY_6               GUIK_6
#define KEY_7               GUIK_7
#define KEY_8               GUIK_8
#define KEY_9               GUIK_9
#define KEY_0               GUIK_0

#define KEY_F1              GUIK_F1
#define KEY_F2              GUIK_F2
#define KEY_F3              GUIK_F3
#define KEY_F4              GUIK_F4

#define KEY_RED             GUIK_R
#define KEY_YELLOW          GUIK_Y
#define KEY_BLUE            GUIK_B
#define KEY_GREEN           GUIK_G
#define KEY_POWER           GUIK_P
#define KEY_PROG_INFO	          GUIK_I
#define KEY_MAIL						GUIK_F5
#define KEY_PROG_LIST				GUIK_C

#define KEY_PAGE_UP         GUIK_PAGE_UP
#define KEY_PAGE_DOWN       GUIK_PAGE_DOWN

#define KEY_CHANNEL_UP      GUIK_U
#define KEY_CHANNEL_DOWN    GUIK_D

#define KEY_VOLUME_UP_1       GUIK_PLUS
#define KEY_VOLUME_DOWN_1     GUIK_MINUS

#define KEY_TRACK           GUIK_S

#define KEY_NVOD           GUIK_N
#define KEY_SPEAKER           GUIK_K
//#define KEY_SEARCH         GUIK_Z
#define KEY_ZOOM            GUIK_Z
#define KEY_SWITCH          GUIK_F6

extern handle_t g_app_msg_self;

/** * get PM 's time */
typedef struct
{	
	int8_t *player_name;		
	uint64_t cur_time;
	uint64_t total_time;
	uint64_t seekmin_time;
}GxMsgProperty_GetPlayTime;

status_t app_msg_init(handle_t self);
status_t app_msg_destroy(handle_t self);
status_t app_init(void);

int find_virtualkey_ex(unsigned int scan_code, unsigned int sym);

// subt
extern void app_subt_change(void);
extern void app_subt_pause(void);
extern void app_subt_resume(void);


typedef enum
{
	BG_MENU = 0,
	BG_PLAY_RPOG,
}BackGroundMode;

typedef enum
{
	EXIT_NORMAL,
	EXIT_ABANDON,
}ExitType;



typedef enum
{	
	GIF_HIDE,	
	GIF_SHOW,
}GifDisplayStatus;

typedef enum
{	
	TIP_SHOW,	
	TIP_HIDE,
}TipDisplayStatus;

typedef enum
{
	ITEM_CHOICE,
	ITEM_EDIT,
	ITEM_PUSH
}ItemSettingType;

typedef enum
{
	ITEM_NORMAL,
	ITEM_HIDE,
	ITEM_DISABLE
}ItemDisplayStatus;


typedef struct
{
	char *string;
	char *maxlen;
	char *format;
	char *intaglio;
	char *default_intaglio;
}EditProperty;

typedef struct
{
	char *content;
	int sel;
}CmbProperty;

typedef struct
{
	char *string;
}BtnProperty;



typedef union
{
	CmbProperty itemPropertyCmb;
	EditProperty itemPropertyEdit;
	BtnProperty itemPropertyBtn;
}ItemProPerty;

typedef struct
{
	int (*CmbChange)(int);
	int (*CmbPress)(int);
}CmbCallback;

typedef struct
{
	int (*EditReachEnd)(char *);
	int (*EditPress)(unsigned short);
}EditCallback;

typedef struct
{
	int (*BtnPress)(unsigned short);
}BtnCallback;

typedef union
{
	CmbCallback cmbCallback;
	EditCallback editCallback;
	BtnCallback btnCallback;
}ItemCallback;

typedef struct
{
	char *keyName;
	int (*keyPressFun)(void);
}FuncKey;

typedef struct
{
	FuncKey redKey;
	FuncKey greenKey;
	FuncKey blueKey;
	FuncKey yellowKey;
}MenuFuncKey;


typedef struct
{
	char *itemTitle;
	ItemSettingType itemType;
	ItemProPerty itemProperty;	
	ItemCallback itemCallback;
	ItemDisplayStatus itemStatus;
}SystemSettingItem;

typedef struct
{
	char *menuTitle;
	int itemNum;
	BackGroundMode backGround;
	char *titleImage;
	SystemSettingItem *item;
	MenuFuncKey menuFuncKey;
	int (*exit)(ExitType);
	TipDisplayStatus topTipDisplay;	
	TipDisplayStatus bottmTipDisplay;
	TipDisplayStatus timeDisplay;
	GifDisplayStatus gifDisplay;
}SystemSettingOpt;




#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */


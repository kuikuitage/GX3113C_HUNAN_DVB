/**
 *
 * @file        app_common_table_pmt.h
 * @brief
 * @version     1.1.0
 * @date        10/29/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DS_ADS_H__
#define __APP_DS_ADS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>
#include "gxbus.h"
#include "desai_ad_api.h"
#include "gui_timer.h"

#define DS_AD_WIDGET_NAME_LEN				64
#define DS_AD_OSD_BUFFER_LEN					256

#define DS_AD_FLASH_START_ADDR				0x0
#define DS_AD_FLASH_LEN						0x10000
#define DS_AD_FLASH_FILE 					"/home/gx/ds_flash.bin"

#define DS_AD_LOGO_CONFIG_FILE				"/home/gx/ds_logo_config.conf"
#define DS_AD_LOGO_I_FRAME_FILE 			"/home/gx/ds_logo_i_frame.bin"
#define DS_AD_LOGO_I_FRAME_FILE_LEN			0x10000


/*DS ad pic data file in memory*/
#define DS_AD_EPG_FILE_PATH					"/mnt/ds_ad_epg.gif"
#define DS_AD_MENU_FILE_PATH				"/mnt/ds_ad_menu.gif"
#define DS_AD_VOLUME_FILE_PATH				"/mnt/ds_ad_volume.gif"
#define DS_AD_FULLSCREEN_FILE_PATH			"/mnt/ds_ad_fullscreen.gif"
#define DS_AD_UNAUTHORIZED_FILE_PATH		"/mnt/ds_ad_unauthorized.gif"

/**/
typedef enum
{
	LOAD_PIC_DATA,
	SHOW_PIC_DATA,
	DRAW_PIC_DATA,
	HIDE_PIC_DATA,
	FREE_PIC_DATA,
}DS_AD_PIC_OPT;

typedef struct
{
	char IconName[DS_AD_WIDGET_NAME_LEN];
	char * filePath;
	event_list * ptrTimer;
	uint8_t isShown;
}DS_AD_PIC_t;

void app_ds_ad_init(void);
uint8_t app_ds_ad_show_av_logo(int32_t VideoResolution);
void app_ds_ad_logo_sleep(uint32_t ms);
uint8_t app_ds_ad_flash_read(uint32_t puiStartAddr,  uint8_t *pucData,	uint32_t uiLen);
uint8_t app_ds_ad_flash_write(uint32_t puiStartAddr,  uint8_t *pucData,  uint32_t uiLen);
uint8_t app_ds_ad_flash_erase(uint32_t puiStartAddr,  uint32_t uiLen);
void app_ds_ad_full_screen_pic_display(char * IconeName);
void app_ds_ad_hide_full_screen_pic(void);
void app_ds_ad_unauthorized_pic_display(char * IconeName);
void app_ds_ad_hide_unauthorized_pic(void);
uint8_t app_ds_ad_unauthorized_pic_is_ready(void);
void app_ds_ad_setpid(uint16_t pid);
int  app_ds_ad_osd_display_status(void);
void app_ds_ad_set_unauthorized_status(bool status);
#ifdef __cplusplus
}
#endif
#endif /*__APP_DS_ADS_H__*/


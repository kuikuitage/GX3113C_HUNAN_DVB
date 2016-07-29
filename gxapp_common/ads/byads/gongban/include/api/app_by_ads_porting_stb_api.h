/**
 *
 * @file        app_common_table_pmt.h
 * @brief
 * @version     1.1.0
 * @date        10/29/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_BY_ADS_H__
#define __APP_BY_ADS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "adver.h"
#include "adver_type.h"
#include <gxtype.h>
#include "gxbus.h"



void app_by_ads_init(void);
void AdverDisplayText_Start(SaveAdverScroll_Info *pAdverInfo);
void AdverDisplayCornerBMP_Start(SaveAdverCorner_Info *pAdverInfo);
void AdverGifStart(SaveAdverCorner_Info *pAdverInfo);
handle_t app_by_ads_get_filter_handle(void);
handle_t app_by_ads_get_channel_handle(void);
void app_by_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
void app_by_pmt_Private_AD_Descriptor(char* buf);
int32_t app_by_ads_gxmsg_ca_on_event_exec(GxMessage * msg);
void LoaderCheckPictureCRC32(AdU32 *FlashCRC32);	
void LoaderVideoCheckWrite(AdverOpenPicture_Info *pAdverInfo);
void app_by_ads_show_av_logo(int32_t VideoResolution);
void app_by_ads_logo_sleep(uint32_t ms);
u_int32_t GetAdverCurrentDisplayState(void);
void AdverDisplay_StopType(unsigned char bType);
u_int8_t app_by_ads_corner_bmp_play_state(void);
void app_by_ads_stop_scroll_bmp(void);
void app_by_ads_stop_corner_bmp(void);
void app_by_ads_stop_corner_gif(void);
void app_by_ads_play_scroll_bmp(void);
void app_by_ads_play_corner_bmp(void);
void app_by_ads_play_corner_gif(void);
u_int8_t app_by_ads_corner_gif_play_state(void);
int scroll_GDI_Load_Image(const char *path,
		unsigned int *width,
		unsigned int *height,
		unsigned char *bpp,
		unsigned char **data);


#ifdef __cplusplus
}
#endif
#endif /*__APP_BY_ADS_H__*/


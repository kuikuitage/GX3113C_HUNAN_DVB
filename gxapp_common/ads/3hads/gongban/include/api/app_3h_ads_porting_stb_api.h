/**
 *
 * @file        app_3h_ads_porting_stb_api.h
 * @brief
 * @version     1.1.0
 * @date        11/27/2013 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_3H_ADS_PORTING_API_H__
#define __APP_3H_ADS_PORTING_API_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"

typedef enum AD_3H_SHOW_TYPE
{
	AD_3H_TYPE_NONE=0,
	AD_3H_TYPE_CORNOR ,
	AD_3H_TYPE_MENU,
	AD_3H_TYPE_MAX
}AD_3H_SHOW_TYPE_T;
void app_3h_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
handle_t app_3h_ads_get_filter_handle(void);
int GxAD_ItemFilterClose(void);
int GxAD_LoadData(void) ;
void app_3h_ads_play(uint8_t type);
u_int8_t app_3h_ads_play_state(uint8_t type);
void app_3h_ads_stop(uint8_t type);
void app_3h_ads_logo_sleep(uint32_t ms);
void app_3h_ads_show_av_logo(int32_t VideoResolution);


#ifdef __cplusplus
}
#endif
#endif /*__APP_3H_ADS_PORTING_API_H__*/


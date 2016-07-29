/**
 *
 * @file        3h_ads_porting.h
 * @brief
 * @version     1.1.0
 * @date        11/27/2013 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_3H_ADS_PORTING_H__
#define __APP_3H_ADS_PORTING_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"

void app_3h_ads_init(void);
void app_3h_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
handle_t app_3h_ads_get_filter_handle(void);
int GxAD_ItemFilterClose(void);
int GxAD_LoadData(void) ;

uint8_t advertisement_get_ad_version_flag();
void advertisement_set_ad_version_flag(uint8_t version_flag);
void advertisement_set_ad_version_flag(uint8_t version_flag);
uint8_t advertisement_set_timeout_flag(uint8_t flag);
uint8_t advertisement_get_timeout_flag();
uint8_t advertisement_get_startplayflag(void);
uint8_t advertisement_set_startplayflag(uint8_t flag);


#ifdef __cplusplus
}
#endif
#endif /*__APP_3H_ADS_PORTING_H__*/


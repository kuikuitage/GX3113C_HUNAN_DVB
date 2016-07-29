/**
 *
 * @file        app_common_table_pmt.h
 * @brief
 * @version     1.1.0
 * @date        10/29/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_XINSHIMAO_ADS_H__
#define __APP_XINSHIMAO_ADS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"

typedef enum XINSHIMAO_AD_SHOW_TYPE
{
	AD_TYPE_NONE=0,
	AD_TYPE_ROLL_PIC ,//??????
	AD_TYPE_OPEN_LOGO ,//????
	AD_TYPE_PF_INFO ,//?????
	AD_TYPE_FS_RIGHT ,//????
	AD_TYPE_EPG ,//EPG??
	AD_TYPE_CH_LIST ,//?????
	AD_TYPE_MAIN_MENU ,//????
	AD_TYPE_RADIO_LOGO ,//?????
	AD_TYPE_FAV_LIST ,//?????
	AD_TYPE_NVOD ,//?????
	AD_TYPE_VOLUME ,//?????
	AD_TYPE_ROLLING_PIC, //????????
	AD_TYPE_MAX
	
}E_XINSHIMAO_AD_SHOW_TYPE;


void app_xinshimao_ads_play(uint8_t type);
u_int8_t app_xinshimao_ads_play_state(uint8_t type);
void app_xinshimao_ads_stop(uint8_t type);
void app_xinshimao_ads_logo_sleep(uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif /*__APP_XINSHIMAO_ADS_H__*/


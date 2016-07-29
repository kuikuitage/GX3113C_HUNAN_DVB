/**
 *
 * @file        app_3h_ads_porting_stb_api.h
 * @brief
 * @version     1.1.0
 * @date        11/27/2013 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_JIUZHOU_ADS_PORTING_API_H__
#define __APP_JIUZHOU_ADS_PORTING_API_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"

typedef enum AD_JIUZHOU_SHOW_TYPE
{
	AD_JIUZHOU_TYPE_NONE=0,
	AD_JIUZHOU_TYPE_BANNER,
	AD_JIUZHOU_TYPE_LOGO, 
	AD_JIUZHOU_TYPE_MAX
}AD_JIUZHOU_SHOW_TYPE_T;

//#define ADV_FILES_SAVED_ON_FLASH  /*将广告文件保存起来, 下次开机不必接收*/ 
#define ADV_FILENAME_MAX_LENGTH		256
#define ADV_FILENAME_LOGO				("/home/gx/openpicture.bin")		
#define ADV_FILENAME_BANNER_HEADER			("/home/gx/jiuzhouads_") //("/mnt/jiuzhouads_")


void app_jiuzhou_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
handle_t app_jiuzhou_ads_get_filter_handle(void);
void app_jiuzhou_ads_play(uint8_t type, uint16_t service_id,char * pic_widget,char * default_pic_path);
u_int8_t app_jiuzhou_ads_play_state(uint8_t type);
void app_jiuzhou_ads_stop(uint8_t type);
void app_jiuzhou_ads_logo_sleep(uint32_t ms);
void app_jiuzhou_ads_show_av_logo(int32_t VideoResolution);


#ifdef __cplusplus
}
#endif
#endif /*__APP_JIUZHOU_ADS_PORTING_API_H__*/


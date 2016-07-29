/**
 *
 * @file        jiuzhou_ads_porting.h
 * @brief
 * @version     1.1.0
 * @date        08/28/2014 
 * @author      xujunlin
 *
 */
#ifndef __APP_JIUZHOU_ADS_PORTING_H__
#define __APP_JIUZHOU_ADS_PORTING_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"

/*广告接收初始化*/ 
void app_jiuzhou_ads_init(void);

/*广告数据处理函数 */ 
void app_jiuzhou_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);

/*换台广告数据过滤*/ 
handle_t app_jiuzhou_ads_get_pf_filter_handle(void);

/*开机LOGO数据过滤 */ 
handle_t app_jiuzhou_ads_get_logo_filter_handle(void);

/*根据serviceid获取该频道对应的图片文件名, 全路径*/ 
char *_adv_xw_get_banner_filename(uint16_t s_id,  char *filename); 

#ifdef __cplusplus
}
#endif
#endif /*__APP_JIUZHOU_ADS_PORTING_H__*/


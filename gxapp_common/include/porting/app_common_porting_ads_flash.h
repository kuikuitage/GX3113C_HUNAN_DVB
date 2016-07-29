/**
 *
 * @file        app_common_porting_ads_flash .h
 * @brief
 * @version     1.1.0
 * @date        11/06/2013 09:30:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_ADS_FLASH_H__
#define __APP_COMMON_PORTING_ADS_FLASH_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>

/*
* 初始化ca flash, 分配大小
* type --CA类型，兼容多CA系统
*/
void app_porting_ads_flash_init(uint32_t size);

/*
* 从FLASH中读取CA 数据
* type --CA类型，兼容多CA系统
*/
uint8_t app_porting_ads_flash_read_data(uint32_t Offset,uint8_t *pbyData,uint32_t *pdwLen);

/*
* CA数据写入FLASH
*/
uint8_t app_porting_ads_flash_write_data(uint32_t Offset,uint8_t *pbyData,uint32_t dwLen);

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_ADS_FLASH_H__*/


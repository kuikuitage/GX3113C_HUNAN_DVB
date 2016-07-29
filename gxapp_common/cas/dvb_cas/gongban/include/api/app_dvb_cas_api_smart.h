/**
 *
 * @file        app_dvb_cas_api_smart.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVB_CAS_API_SMART_H__
#define __APP_DVB_CAS_API_SMART_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>

uint8_t app_dvb_cas_api_smart_check_valid(void);
uint8_t app_dvb_cas_api_card_in(char* atr,uint8_t len);
uint8_t app_dvb_cas_api_card_out(void);
void app_dvb_cas_api_smart_init(void);
uint8_t app_dvb_cas_api_smart_reset(uint8_t* pbyATR, uint8_t *pbyLen );

#ifdef __cplusplus
}
#endif
#endif /*__app_dvb_CAS_API_SMART_H__*/


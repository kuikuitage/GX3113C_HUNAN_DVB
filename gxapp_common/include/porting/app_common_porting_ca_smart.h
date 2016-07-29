/**
 *
 * @file        app_common_porting_ca_smart.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:35:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_CA_SMART_H__
#define __APP_COMMON_PORTING_CA_SMART_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
//#include "gxca0_smartcard.h"
#include "gx_smartcard.h"

#define ATR_MAX_LEN                 (256)
void app_porting_ca_smart_card_init(GxSmcParams param,GxSmcTimeParams time);
uint8_t app_porting_ca_smart_reset(GxSmcTimeParams    time,uint8_t* pbyATR, uint8_t *pbyLen );
uint8_t app_porting_ca_smart_status();
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
uint8_t app_porting_ca_smc_status(void);
#endif
void app_porting_ca_smart_card_status_notify(handle_t handle, GxSmcCardStatus status);
uint8_t app_porting_ca_smart_sc_apdu( const uint8_t* pbyCommand,
								  uint16_t       wCommandLen,
								  uint8_t*       pbyReply,
								  uint16_t*      pwReplyLen  );
uint8_t app_porting_ca_smart_sc_apdu61( const uint8_t* pbyCommand,
								  uint16_t       wCommandLen,
								  uint8_t*       pbyReply,
								  uint16_t*      pwReplyLen  );

uint8_t app_porting_ca_smart_sc_apdu61_tr( const uint8_t* pbyCommand,
		uint16_t       wCommandLen,
		uint8_t*       pbyReply,
		uint16_t*      pwReplyLen  );

void app_porting_ca_smart_create_task(void);
void app_porting_ca_smart_config_time(GxSmcTimeParams time);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_CA_SMART_H__*/


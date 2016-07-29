/**
 *
 * @file        app_cd_cas_3.0_api_detitle.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVB_CAS_API_ROLLING_MESSAGE_H__
#define __APP_DVB_CAS_API_ROLLING_MESSAGE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_play.h"

int32_t app_dvb_cas_gxmsg_ca_on_event_rolling(GxMsgProperty0_OnEvent* event);
int32_t app_dvb_cas_rolling_message_exec(void);
void app_dvb_cas_rolling_message_hide(void);

#ifdef __cplusplus
}
#endif
#endif /*__APP_DVB_CAS_API_ROLLING_MESSAGE_H__*/


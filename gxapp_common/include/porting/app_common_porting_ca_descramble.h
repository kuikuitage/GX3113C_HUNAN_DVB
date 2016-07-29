/**
 *
 * @file        app_common_porting_ca_os.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:33:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_CA_DESCRAMBLE_H__
#define __APP_COMMON_PORTING_CA_DESCRAMBLE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>

typedef struct ca_descramble_S
{
	handle_t handle;
	uint8_t      type;
	uint8_t      usedStatus;
}ca_descramble_t;
#define MAX_SCRAMBLE_CHANNEL 8

uint8_t	app_porting_ca_descramble_init(void);
uint8_t	app_porting_ca_descramble_set_pid(uint8_t index,uint16_t pid );
uint8_t	app_porting_ca_descramble_set_cw(uint16_t       wEcmPID,
							  const uint8_t* pbyOddKey,
							  const uint8_t* pbyEvenKey,
							  uint8_t        byKeyLen,
							  bool      bTapingEnabled );
uint8_t	app_porting_ca_descramble_set_cw_for_advanced(uint16_t wEcmPID,
							  int32_t*      pAudioDescramblerId,
							  int32_t*      pVideoDescramblerId);


#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_CA_DESCRAMBLE_H__*/


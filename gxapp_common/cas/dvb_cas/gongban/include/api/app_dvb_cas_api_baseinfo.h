/**
 *
 * @file        app_by_cas_api_baseinfo.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVBCAS_API_BASEINFO_H__
#define __APP_DVBCAS_API_BASEINFO_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_play.h"
#define DVBCA_STB_ID_MAX_NUM  4

typedef enum
{
	DVBCAS_BASEINFO_CAS_VERSION=0,
	DVBCAS_BASEINFO_CARD_VERSION,
	DVBCAS_BASEINFO_CARD_ID,
	DVBCAS_BASEINFO_STB_PAIR,
	DVBCAS_BASEINFO_WORK_TIME,
	DVBCAS_BASEINFO_WATCH_RATING,
}DVBCA_BASEIINFO_ID_E;

typedef struct {
	uint32_t        stb_id[DVBCA_STB_ID_MAX_NUM];
	uint8_t         stb_num;
}DVBCA_BASEINFO_GetStbPair;


int32_t app_dvb_cas_api_init_baseinfo_data(void);
char * app_dvb_cas_api_get_baseinfo_data(ca_get_date_t *data);


#ifdef __cplusplus
}
#endif
#endif /*__APP_BYCAS_API_BASEINFO_H__*/


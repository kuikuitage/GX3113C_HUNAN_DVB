/**
 *
 * @file        app_dvb_cas_api_entitle.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVBCAS_API_ENTITLE_H__
#define __APP_DVBCAS_API_ENTITLE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_play.h"
#include "dvbca_interface.h"
#define DVBCA_OPERATOR_ID_MAX_NUM  4
typedef enum
{
	DVB_OPERATOR_PRODUCT_OPERATOR_ID=0 ,
	DVB_OPERATOR_PRODUCT_NETWORK_ID=1 ,
	DVB_OPERATOR_PRODUCT_NAME_ID =2,
	DVB_OPERATOR_PRODUCT_ENDTIME_ID=3,
}DVBCA_OPERATOR_ID_E;

typedef enum
{
	DVB_ENTITLE_PRODUCT_NUM =0,
	DVB_ENTITLE_PRODUCT_START_TIME=1,
	DVB_ENTITLE_PRODUCT_END_TIME=2,
}DVBCA_ENTITLE_ID_E;

typedef struct {
	uint32_t        operator_id[DVBCA_OPERATOR_ID_MAX_NUM];
	uint32_t        operator_num;
}DVBCA_ENTITLE_GetOperatorID;

typedef struct {
	uint32_t                operator_id;
	ST_DVBCA_OPERATORINFO   operator_info;
}DVBCA_OPERATOR_GetOperatorInfo;

int32_t app_dvb_cas_api_init_entitle_data(void);
char * app_dvb_cas_api_get_entitle_data(ca_get_date_t *data);
uint8_t app_dvb_cas_api_get_entitle_count(ca_get_count_t *data);
char * app_dvb_cas_api_get_operator_data(ca_get_date_t *data);
uint8_t app_dvb_cas_api_get_operator_count(ca_get_count_t *data);

uint32_t app_dvb_cas_get_entitle_count(int flag);

#ifdef __cplusplus
}
#endif
#endif /*__app_dvbCAS_API_ENTITLE_H__*/


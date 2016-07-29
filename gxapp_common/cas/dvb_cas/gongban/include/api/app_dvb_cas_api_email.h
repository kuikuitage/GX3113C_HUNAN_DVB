/**
 *
 * @file        app_dvb_cas_api_entitle.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVBCAS_API_EMAIL_H__
#define __APP_DVBCAS_API_EMAIL_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_play.h"
#include "dvbca_interface.h"

typedef struct {
	uint8_t wStatus; /* ÐèÒªÏÔÊ¾ÓÊ¼þÍ¼±êÀàÐÍ*/
	uint8_t wShowStatus; /*µ±Ç°ÏÔÊ¾×´Ì¬*/
	uint8_t uCount; /* ÏÔÊ¾ÀÛ¼Æ´ÎÊý£¬´¦ÀíÉÁË¸*/
} dvb_cas_email_st; 

typedef enum 
{
	DVB_EMAIL_FLAG_ID =0,
	DVB_EMAIL_TITLE_ID ,
	DVB_EMAIL_SENDTIME_ID ,
	DVB_EMAIL_VALIDTIME_ID,
	DVB_EMAIL_CONTENT_ID
}DVBCA_EMAIL_ID_E;

typedef struct {
	ST_DVBCAEmail   mail_list[DVBCA_MAX_MAIL];
	uint8_t         start_pos;
	uint16_t        get_num;
}dvb_cas_get_mail;

int32_t app_dvb_cas_api_init_email_data(void);
char * app_dvb_cas_api_get_email_data(ca_get_date_t *data);
uint8_t app_dvb_cas_api_get_email_count(ca_get_count_t *data);

bool app_dvb_cas_api_delete_email(ca_delete_data_t *data);
bool app_dvb_cas_api_delete_all_email(ca_delete_data_t *data);

int32_t app_dvb_cas_gxmsg_ca_on_event_email(GxMsgProperty0_OnEvent* event);
int32_t app_dvb_cas_email_flag_exec(void);
void app_dvb_cas_email_flag_check(void);
void app_dvb_cas_email_flag_hide(void);



#ifdef __cplusplus
}
#endif
#endif /*__APP_BYCAS_API_EMAIL_H__*/


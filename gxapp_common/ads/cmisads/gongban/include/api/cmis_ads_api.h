#ifndef __CMIS_ADS_API_H__
#define __CMIS_ADS_API_H__
#include <gxtype.h>

#define CMIS_MAIL_MAX_LEN		100	
#define CMIS_MAIL_TEX_LEN 		100	
#define CMIS_LOCK_SERVICE_NAME_LEN	50


/*In the call back,CMIS API should not be called except APP_CMIS_AD_GetTvNo*/
typedef void (*ad_show_callback_fun)(void *);
typedef enum 
{
	CMIS_SUCCESS,
	CMIS_ERROR
}Cmis_err_t;

typedef enum
{
	CMIS_DATA_TYPE_ALL,
	CMIS_DATA_TYPE_CURTAIN,
	CMIS_DATA_TYPE_EPG,
	CMIS_DATA_TYPE_MAIL,
}Cmis_data_type_t;

typedef enum
{
	CMIS_PROG_TYPE_CURTAIN,
	CMIS_PROG_TYPE_HOME_PROG,
	CMIS_PROG_TYPE_RECHARGE_BACKGROUND,
	CMIS_PROG_TYPE_VOD_BACKGROUND,
	CMIS_PROG_TYPE_UNDEFINE
}Cmis_prog_type_t;

typedef struct
{
	uint32_t pic_len;
	uint32_t anti_fake;
	uint32_t monitor_point;
	uint32_t monitor_length;
	uint16_t x;
	uint16_t y;
	uint8_t * pic_data;
}Cmis_pic_data_t;

typedef struct
{
	uint8_t url_len;
	uint8_t * url;
}Cmis_url_data_t;

typedef struct
{
	uint8_t tip_1_len;
	uint8_t * tip_1_str;
	uint8_t tip_2_len;
	uint8_t * tip_2_str;
	uint8_t tip_3_len;
	uint8_t * tip_3_str;
}Cmis_tip_data_t;

typedef struct
{
	uint32_t magic_no;
	uint8_t protocol_version;
	uint8_t action;
	Cmis_pic_data_t pic_data;
	Cmis_url_data_t url_data;
	Cmis_tip_data_t tip_data;
}Cmis_curtain_data_t;

typedef struct
{
	uint16_t channel_code;
	uint8_t data_version;
	uint8_t sec_recv_mask[32];
	uint32_t data_size_rcved;
	uint32_t data_size_total;
	uint8_t * data;
}Cmis_ad_table_t;

typedef struct
{
	uint32_t magic_no;
	uint8_t protocol_version;
	uint8_t data_version;
	uint8_t layout;
	uint16_t period;
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	uint32_t schedule_id;
	uint32_t monitor_point;
	uint32_t monitor_len;
	uint8_t action;
	uint16_t url_len;
	uint8_t * url_data;
	uint32_t pic_len;
	uint8_t * pic_data;
}Cmis_epg_data_t;


typedef struct
{
	uint16_t service_id;
	Cmis_prog_type_t prog_type;
	uint16_t curtain_channel_pid;	/*Used for prog type CMIS_PROG_TYPE_CURTAIN*/
	uint16_t epg_channel_pid;		/*Used for EPG AD data pid*/
	uint8_t curtain_data_completed;
	uint8_t epg_data_completed;
	Cmis_ad_table_t * curtain_table;
	Cmis_curtain_data_t * curtain_data;
	Cmis_ad_table_t * epg_table;
	Cmis_epg_data_t * epg_data;
}Cmis_prog_t;


typedef enum
{
	CMIS_MAIL_TYPE_UNKOWN,
	CMIS_MAIL_TYPE_LOCK_PLAY,/*lock play designated service*/
	CMIS_MAIL_TYPE_3_LINE,/* common hint,3 line text*/
	CMIS_MAIL_TYPE_RESERVED,
	CMIS_MAIL_TYPE_2_LINE,/* common hint,2 line text*/
	CMIS_MAIL_TYPE_LOW_LEVEL,/*low level recharg hint*/
	CMIS_MAIL_TYPE_MIDDLE_LEVEL,/*middle level recharg hint*/
}
Cmis_mail_type_t;

typedef struct
{
	uint16_t year ;
	uint8_t  month ;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Cmis_time_t;

typedef struct
{
	char service_name[CMIS_LOCK_SERVICE_NAME_LEN];
	uint32_t duation_s;
	uint32_t freq;
	uint32_t sym_rate;
	uint16_t qam;
	uint16_t service_id;
	Cmis_time_t play_start_time;
	uint32_t mail_id;
}Cmis_lock_service_t;

typedef struct
{
	Cmis_mail_type_t mail_type;
	char text_line_1[CMIS_MAIL_TEX_LEN];
	char text_line_2[CMIS_MAIL_TEX_LEN];
	char text_line_3[CMIS_MAIL_TEX_LEN];
	int32_t time_out_s;
	Cmis_lock_service_t lock_play_service;
}Cmis_mail_info_t;
private_parse_status cmis_ad_sdt_data_parse(uint8_t *p_section_data, uint32_t len);

void APP_CMIS_AD_Init(void);
void APP_CMIS_AD_Open(void);
void APP_CMIS_AD_Close(void);
void APP_CMIS_AD_Start(void);
void APP_CMIS_AD_Stop(void);
Cmis_err_t APP_CMIS_AD_PlayHomeService(void);
Cmis_err_t APP_CMIS_AD_PlayRechargeService(void);
Cmis_err_t APP_CMIS_AD_GetTvNo(char * TVNOStr);
void APP_CMIS_AD_TVNoShow(char * widget_name);
void APP_CMIS_AD_TVNoHide(char * widget_name);
Cmis_prog_type_t APP_CMIS_AD_CheckCurProgramType(void);
#endif
/*EOF*/

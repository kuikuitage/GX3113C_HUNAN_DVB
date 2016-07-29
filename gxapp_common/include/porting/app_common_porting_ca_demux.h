/**
 *
 * @file        app_common_porting_ca_demux.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:30:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_CA_DEMUX_H__
#define __APP_COMMON_PORTING_CA_DEMUX_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxcore.h>
//#include <gxca_def.h>
#include <assert.h>
#include <gx_demux.h>
#include "gxapp_sys_config.h"


#define MAX_FILTER_LEN (16)
#define MAX_FILTER_COUNT (18)
typedef void ((*FilterNotify)	(handle_t			Filter, 
								 const uint8_t* 	Section,
								 size_t 			Size));

typedef struct ca_filter_S
{
	handle_t handle;
	uint8_t      usedStatus;
	uint8_t     match[18] ;
	uint8_t     mask[18] ;
	uint8_t      filterLen; 
	handle_t channelhandle;
	uint16_t     pid;
	uint8_t    repeatFlag;
	uint8_t    equalFlag;   /*是否等版本过滤*/
	uint8_t    crcFlag;
	FilterNotify  Callback; /*回调*/
	FilterNotify  timeOutCallback; /*回调*/
	int32_t nWaitSeconds;  /*设置的超时最大时间,单位s*/
	int32_t nms;                 /*未过滤到数据已过时间*/
	uint8_t      byReqID;	/*永新视博CA*/
}ca_filter_t;

uint8_t app_porting_ca_demux_init();

#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
ca_filter_t* app_porting_ca_demux_get_by_filter_id(uint32_t filter_id);
#else
ca_filter_t* app_porting_ca_demux_get_by_filter_id(uint32_t filter_id,ca_filter_t* filter);
#endif
uint8_t app_porting_ca_demux_release_filter( uint32_t   filterId,uint32_t release_channel_flag);
uint8_t app_porting_ca_demux_start_filter(ca_filter_t* filter);
void app_porting_ca_create_filter_task(void);
void app_porting_psi_create_filter_task(void);
void app_porting_disable_query_demux(void);
void app_porting_enable_query_demux(void);
int32_t app_porting_ca_section_filter_crc32_check(uint8_t *pSection);
void app_porting_psi_demux_lock(void);
void app_porting_psi_demux_unlock(void);
void app_porting_ca_demux_enable_all_filter(void);
void app_porting_ca_demux_disable_all_filter(void);
int app_porting_ca_demux_alloc_channel(unsigned int pid,unsigned char repeat,
								unsigned int *p_id);
int app_porting_ca_demux_start_channel(unsigned int pid);
int app_porting_ca_demux_stop_channel(unsigned int pid);
int app_porting_ca_demux_free_channel(unsigned int pid);
int app_porting_ca_demux_check_same_filter(ca_filter_t* filter);
int app_porting_ca_demux_free_filter(unsigned int filterId);
int app_porting_ca_demux_set_filter(ca_filter_t* filter);


#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_CA_DEMUX_H__*/


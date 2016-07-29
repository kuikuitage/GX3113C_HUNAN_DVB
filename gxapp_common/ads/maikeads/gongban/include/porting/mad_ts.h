/*
 * mad_ts.h
 *
 *  Created on: 2015-11-27
 */

#ifndef _M_AD_TS_H_
#define _M_AD_TS_H_


#define MAD_HEADER_VERSION		1

#define MAD_HEADER_PID			18
#define MAD_HEADER_TABLE_ID		64
#define MAD_HEADER_TIMEOUT		10000
#define MAD_DATA_TIMEOUT		10000


typedef enum
{
	MAD_TS_TASK_STATE_IDLE,
	MAD_TS_TASK_STATE_INIT,
	MAD_TS_TASK_STATE_WAIT_HEADER,
	MAD_TS_TASK_STATE_WAIT_DATA,
	MAD_TS_TASK_STATE_STOP,
	MAD_TS_TASK_STATE_NUM
} MadTsTaskState;

typedef enum
{
	MAD_DOWNLOAD_STATE_NONE,
	MAD_DOWNLOAD_STATE_RUNNING,
	MAD_DOWNLOAD_STATE_FINISH,
	MAD_DOWNLOAD_STATE_ERR_FINISH,
	MAD_DOWNLOAD_STATE_NUM
} MadDownloadState;


typedef struct _mad_ts_place_info_
{
	U32 ad_place_id;
	U32 ad_place_ver;
	U16 pid;
	U8  ad_place_type;
	U8  table_id;
} MadTsPlaceInfo;

typedef struct _mad_header_
{
	unsigned short header_version;
	unsigned short platform_id;
	unsigned short customer_id;
	unsigned short model_id;
	unsigned char sn_start[6];
	unsigned char sn_end[6];
	unsigned short mad_ts_place_info_num;
	MadTsPlaceInfo *mad_ts_place_infos;
} MadHeader;


int mad_ts_init(void);
#if M_AD_TS_RUN_IN_BKG
int start_monitor_mad(void);
int stop_monitor_mad(void);
#else
int mad_ts_update(void);
#endif


#endif /* _M_AD_TS_H_ */

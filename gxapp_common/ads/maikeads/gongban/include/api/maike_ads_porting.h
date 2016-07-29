#ifndef __MAIKE_ADS_PORTING_H__
#define __MAIKE_ADS_PORTING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>
#include "gxbus.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "maike_ads_type.h"
#include "mad_cfg.h"
#ifndef u8
#define u8 unsigned char
#endif
#ifndef u32
#define u32 unsigned int
#endif
#if ENABLE_M_AD_TS
//#include "mad_ts.h"
#endif

#define MAD_BOOT_FRAM_PATH		("/home/gx/boot_")
#define MAD_CORNER_PIC_PATH		("/home/gx/mad_corner_")
#define MAD_PROG_BAR_PIC_PATH	("/home/gx/mad_prog_bar_")
#define MAD_VOL_BAR_PIC_PATH	("/home/gx/mad_vol_bar_")
#define MAD_MAIN_MENU_PIC_PATH	("/home/gx/mad_main_menu_")
#define MAD_PROG_LIST_PIC_PATH	("/home/gx/mad_prog_list_")
#define MAD_EPG_PIC_PATH		("/home/gx/mad_epg_")
#define MAD_OSD_TEXT_PATH		("/home/gx/mad_osd_text_")

#define MAD_FILE_FORMAT_JPG		(".jpg")
#define MAD_FILE_FORMAT_GIF		(".gif")
#define MAD_FILE_FORMAT_PNG		(".bmp")
#define MAD_FILE_FORMAT_BIN		(".bin")
#define MAD_FILE_FORMAT_TEXT	(".text")

#define MAD_PLACE_MAX_ITEM		3

#define  DEMUX_INVALID_PID 			(0x1FFF)
#define  SIE_REASON_FILTER_TIMEOUT	(0xFF)

#define RET_CODE 		s32

#define  GENDMX_BAD_CHANNEL (-1)


#define KBYTES								(1024)                                                        
#define  MAD_TASK_STACK_SIZE   				(16 * KBYTES)
#define  MAD_TASK_PRIORITY  				(10)
#define  MAD_FILTER_BUFFER_SIZE 			(64 * (KBYTES))
#define  MADS_DATA_MONTION_TASK_STKSIZE    	(16 * KBYTES)
#define  MAD_FILTER_MAX						(12)
#define GET_MAD_PID_MAX_TIMES   			(5000)
#define MAX_SEARIAL_LEN 					(12)
#define	DMX_SECTION_FILTER_SIZE 			(16)

typedef enum{
	sie_started			= 0x00000001UL, /* section should be activatived */
	sie_stopped			= 0x00000002UL, /* section should be idlized  */
	sie_freed			= 0x00000004UL, /* section should be freed */
}sie_status_t;

typedef enum
{
	AD_MONITOR_TASK_IDLE,
	AD_MONITOR_TASK_INIT,
	AD_MONITOR_TASK_DEINIT,
	AD_MONITOR_TASK_NUM
}AD_MONITOR_TASK_STATE;

typedef struct mad_filter_extend_t
{	
	u8 filter_data[DMX_SECTION_FILTER_SIZE];
	u8 filter_mask[DMX_SECTION_FILTER_SIZE];
	handle_t channel_handle;
	u32 tick;
	u32 time_out;
}mad_filter_extend;


typedef struct file_recored_t
{
	char mad_file_name_record[M_AD_PLACE_NUM][MAD_PLACE_MAX_ITEM][64];//each place id file name;
	uint8_t  mad_file_name_record_type[M_AD_PLACE_NUM][MAD_PLACE_MAX_ITEM];
	uint8_t  mad_file_name_record_cnt[M_AD_PLACE_NUM];					//each place id file cnt;
	uint8_t  mad_file_name_record_place_id[M_AD_PLACE_NUM];
}mad_file_record;


void set_mad_filter_timeout(int mad_filter_index,int timeout);
void set_mad_filter_channel_handle(int mad_filter_index,handle_t handle);


void mad_ts_task_init(void);
void  mad_ads_client_init(void);


#ifdef __cplusplus
}
#endif
#endif //__MAIKE_ADS_PORTING_H__

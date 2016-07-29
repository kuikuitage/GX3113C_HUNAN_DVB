/*
 * mad_ts_sec.h
 *
 *  Created on: 2016-6-14
 *      Author: Brant
 */

#ifndef _M_AD_TS_SEC_H_
#define _M_AD_TS_SEC_H_

#include "mark_map.h"
#include "maike_ads_porting.h"
#include "maike_ads_type.h"

#ifndef u8
#define  u8 unsigned char
#endif
#ifndef u32
#define u32 unsigned int
#endif	

#define ALI_PLATFORM 0
#define MSTAR_PLATFORM 1
#define NewCool_PLATFORM 2
#define GX3113C_PLATFORM 3

#define PLATFORM_SUPPORT  GX3113C_PLATFORM

struct si_filter_t
{
	void* reserved;//to make the right arg list of origin maike ads of func "mad_section_callback"
};



#define MAX_FILTER_NUM		(12)

#define MAX_SECTION_LENGTH		(4096)
#define SECTION_HEADER_LENGTH	(8)
#define CRC32_LENGTH			(4)
#define MAX_DATA_IN_SECTION		(MAX_SECTION_LENGTH - SECTION_HEADER_LENGTH - CRC32_LENGTH)
#define MAX_SECTION_NUM			(0xffff)


typedef enum
{
	MAD_FILTER_STATE_NONE,
	MAD_FILTER_STATE_TIMEOUT,
	MAD_FILTER_STATE_FIND_DATA,
	MAD_FILTER_STATE_FINISH,
	MAD_FILTER_STATE_ERROR,
	MAD_FILTER_STATE_NUM
} MadFilterState;

typedef void (*GetMadDataCallback)(U16 pid, U8 table_id, BOOL success, U8 *data, U32 length);

typedef struct _mad_filter_ext_
{
	u8 filter_data[DMX_SECTION_FILTER_SIZE];
	u8 filter_mask[DMX_SECTION_FILTER_SIZE];
	handle_t channel_handle;
	u32 tick;
	u32 time_out;
}mad_filter_ext;

typedef struct _mad_filter_
{
	BOOL in_use;
	U16  pid;
	U8   table_id;
	int  sys_filter_id;
	U8*  data;
	U32  data_length;
	MarkMap section_got_map;
	GetMadDataCallback cb;
	//gw::extern
	 mad_filter_ext filter_ext;
} MadFilter;


int get_free_mad_filter_num(void);
int request_mad_data(U16 pid, U8 table_id, U32 timeout, GetMadDataCallback cb);
void free_mad_filter(int mad_filter_index);
void free_all_mad_filter(void);
sie_status_t mad_section_callback(U16 pid, struct si_filter_t *filter, U8 reason, U8 *sec_buffer, U32 length);

#endif /* _M_AD_TS_SEC_H_ */

/*
 * mad_ts_sec.c
 *
 *  Created on: 2016-6-14
 *      Author: Brant
 */
#include "app_common_porting_stb_api.h"
#include "mad_cfg.h"
#include "mad_ts_sec.h"
//#include "section.h"
#if (PLATFORM_SUPPORT == ALI_PLATFORM)
#include <api/libtsi/sie.h>
#endif
#include "mark_map.h"
#include "mad_interface.h"
#include "mad_util.h"
#include "mad_ts.h"

#include "maike_ads_type.h"
#include "maike_ads_porting.h"
/***********************************************************************************
 *                               section interface                                 *
 ***********************************************************************************/
sie_status_t mad_section_callback(U16 pid, struct si_filter_t *filter, U8 reason, U8 *sec_buffer, U32 length);

static BOOL is_valid_system_filter(int channel)
{
	return channel >= 0;
}

static void free_system_filter(int channel)
{
	FreeSectionSlot(channel);
}

static int alloc_system_filter(U16 pid, U8 table_id, U32 timeout)
{
	U8 mad_filter_match[16] = {0};
	U8 mad_filter_mask[16] = {0};
	int system_slot_channel;
	handle_t filter_handle;

	memset(mad_filter_match, 0, sizeof(mad_filter_match));
	memset(mad_filter_mask, 0, sizeof(mad_filter_mask));
	mad_filter_match[0] = table_id;
	mad_filter_mask[0] = 0xff;

#if(PLATFORM_SUPPORT == ALI_PLATFORM)
	system_slot_channel = SectionAllocSlot(mad_section_callback, NULL, timeout, GS_FILTER_OTA, pid);
#elif (PLATFORM_SUPPORT == MSTAR_PLATFORM)
	chn_mad_index = SectionAllocSlot(mad_section_callback, OTATimeOut, MAD_TIMEOUT, GS_FILTER_OTA);
#elif (PLATFORM_SUPPORT == GX3113C_PLATFORM)//通过query获取数据解析数据中调用该回调函数即可
	system_slot_channel = SectionAllocSlot(NULL/*mad_section_callback*/,timeout, pid);
#endif
	printf("system_slot_channle = %d\n",system_slot_channel);
	if (!is_valid_system_filter(system_slot_channel))
	{
		printf("[alloc_system_filter]alloc slot invalid\n");
		return -1;
	}
	//use the slot allocated above and mad_filter_match, mad_filter_mask to configure the filter
	filter_handle = SectionAllocFilter(system_slot_channel, mad_filter_match, mad_filter_mask, FALSE/*TRUE*/, TRUE);
	if ((filter_handle == 0) || (filter_handle == -1))
	{
		printf("[alloc_system_filter]alloc filter invalid\n");
		free_system_filter(system_slot_channel);
		return -1;
	}
#if 0
	//enable the filter and begin to get data
	if (SectionSlotSetPid(system_slot_channel, pid) < 0)
	{
		free_system_filter(system_slot_channel);
		return -1;
	}
#endif
	//return system_slot_channel;
	return filter_handle;
}


/***********************************************************************************
 *                                  m-ad filter                                    *
 ***********************************************************************************/
typedef struct _mad_section_header_
{
	U8 table_id;
	U16 section_syntax_indicator: 1;
	U16 zero: 1;
	U16 reserved: 2;
	U16 section_length: 12;
	U16 current_section;
	U16 total_section;
} MadSectionHeader;


MadFilter mad_filters[MAX_FILTER_NUM];


static int get_free_mad_filter(void)
{
	int i;

	for (i = 0; i < MAX_FILTER_NUM; i++)
	{
		if (!mad_filters[i].in_use)
		{
			return i;
		}
	}
	return -1;
}

static void set_mad_filter_pid(int mad_filter_index, U16 pid)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].pid = pid;
	}
}

static void set_mad_filter_table_id(int mad_filter_index, U8 table_id)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].table_id = table_id;
	}
}

static void set_mad_filter_callback(int mad_filter_index, GetMadDataCallback cb)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].cb = cb;
		printf("cb = %p,0x%x\n",cb,cb);
	}
}

static void set_mad_filter_slot_id(int mad_filter_index, int sys_channel)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].sys_filter_id = sys_channel;
	}
}

static void set_mad_filter_in_use_flag(int mad_filter_index, BOOL in_use)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].in_use = in_use;
	}
}

static U8* get_mad_filter_data(int mad_filter_index)
{
	if (mad_filter_index < 0 || mad_filter_index >= MAX_FILTER_NUM)
	{
		return NULL;
	}
	return mad_filters[mad_filter_index].data;
}

static U32 get_mad_filter_data_length(int mad_filter_index)
{
	if (mad_filter_index < 0 || mad_filter_index >= MAX_FILTER_NUM)
	{
		return 0;
	}
	return mad_filters[mad_filter_index].data_length;
}

static int find_mad_filter(U16 pid/*, U8 table_id*/)
{
	int i;

	for (i = 0; i < MAX_FILTER_NUM; i++)
	{
		if (mad_filters[i].in_use && mad_filters[i].pid == pid/* && mad_filters[i].table_id == table_id*/)
		{
			return i;
		}
	}
	return -1;
}



int get_free_mad_filter_num(void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_FILTER_NUM; i++)
	{
		if (!(mad_filters[i].in_use))
		{
			count++;
		}
	}
	return count;
}

void free_mad_filter(int mad_filter_index)
{
	if (mad_filter_index < 0 || mad_filter_index >= MAX_FILTER_NUM)
	{
		return;
	}
	if (!mad_filters[mad_filter_index].in_use)
	{
		return;
	}
	if (mad_filters[mad_filter_index].data)
	{
		mad_free(mad_filters[mad_filter_index].data);
	}
	destroy_mark_map(&(mad_filters[mad_filter_index].section_got_map));
	free_system_filter(mad_filters[mad_filter_index].sys_filter_id);
	memset(mad_filters + mad_filter_index, 0, sizeof(MadFilter));
}

void free_all_mad_filter(void)
{
	int index;

	for (index = 0; index < MAX_FILTER_NUM; index++)
	{
		free_mad_filter(index);
	}
}

static int alloc_mad_filter(U16 pid, U8 table_id, U32 timeout, GetMadDataCallback cb)
{
	int mad_filter_index;
	int system_filter_channel;

	mad_filter_index = get_free_mad_filter();
	printf("available index is %d\n",mad_filter_index);
	if (mad_filter_index < 0)
	{
		return -1;
	}
	system_filter_channel = alloc_system_filter(pid, table_id, timeout);
	printf("filter_handle =  0x%x \n",system_filter_channel);
	if((system_filter_channel != 0)&&(system_filter_channel != -1))
	{
		printf("valid system filter!!@!\n");
		set_mad_filter_in_use_flag(mad_filter_index, TRUE);
		set_mad_filter_slot_id(mad_filter_index, system_filter_channel);
		set_mad_filter_pid(mad_filter_index, pid);
		set_mad_filter_table_id(mad_filter_index, table_id);
		set_mad_filter_timeout(mad_filter_index,timeout);
		set_mad_filter_callback(mad_filter_index, cb);
	}
	else
	{
		printf("valid filter index,but filter handle record not null");
		free_mad_filter(mad_filter_index);
		return -1;
	}
	return mad_filter_index;
}


int request_mad_data(U16 pid, U8 table_id, U32 timeout, GetMadDataCallback cb)
{
	return alloc_mad_filter(pid, table_id, timeout, cb);
}


static BOOL parse_mad_section_header(MadSectionHeader *header, U8 *sec_buffer, U32 length)
{
	if (!sec_buffer || length <= 0)
	{
		return FALSE;
	}
	header->table_id = sec_buffer[0];
	header->section_syntax_indicator = sec_buffer[1] >> 7;
	header->zero = (sec_buffer[1] & 0x40) >> 6;
	header->reserved = (sec_buffer[1] & 0x30) >> 4;
	header->section_length = ((sec_buffer[1] & 0x0F) << 8) | sec_buffer[2];
	header->current_section = (sec_buffer[3] << 8) | sec_buffer[4];
	header->total_section = (sec_buffer[5] << 8) | sec_buffer[6];
	printf("table_id = 0x%x,section_length = %d,current_section = %d,total_section = %d\n"
		,header->table_id,header->section_length,header->current_section,header->total_section);
	if (!mad_check_crc32(sec_buffer, header->section_length + 3))
	{
		return FALSE;
	}

	return TRUE;
}

static int save_mad_section_data(int filter_index, int sec_num, int total_sec, U8 *origin_data, U32 length)
{
	int ret = MAD_FILTER_STATE_NONE;

	if (is_empty_map(&(mad_filters[filter_index].section_got_map)))
	{
		init_mark_map(&(mad_filters[filter_index].section_got_map), total_sec);
		mad_filters[filter_index].data_length = 0;
		mad_filters[filter_index].data = (U8 *)mad_malloc(MAX_DATA_IN_SECTION * total_sec);
		if (!mad_filters[filter_index].data)
		{
			return MAD_FILTER_STATE_ERROR;
		}
	}
	if (!is_item_marked(&(mad_filters[filter_index].section_got_map), sec_num))
	{
		set_item_mark(&(mad_filters[filter_index].section_got_map), sec_num);
		//if(filter_index == 0)//debug boot logo
			//ADS_DUMP("MKADS_DATA",origin_data,length);
		memcpy(mad_filters[filter_index].data + sec_num * MAX_DATA_IN_SECTION, origin_data, length);
		mad_filters[filter_index].data_length += length;
	}
	ret = MAD_FILTER_STATE_FIND_DATA;
	if (is_all_item_marked(&(mad_filters[filter_index].section_got_map)))
	{
		printf("[save_mad_section_data]all hearder sec got!!!\n");
		ret = MAD_FILTER_STATE_FINISH;
	}
	return ret;
}

static BOOL need_stop_mad_filter(int state)
{
	if (state == MAD_FILTER_STATE_TIMEOUT || state == MAD_FILTER_STATE_FINISH || state == MAD_FILTER_STATE_ERROR)
	{
		return TRUE;
	}
	return FALSE;
}

static void do_mad_filter_callback(int filter_index, BOOL success, U8 *data, U32 length)
{
	if (filter_index < 0 || filter_index >= MAX_FILTER_NUM)
	{
		return;
	}
	if (mad_filters[filter_index].cb)
	{
		mad_filters[filter_index].cb(mad_filters[filter_index].pid, mad_filters[filter_index].table_id, success, data, length);
	}
}


sie_status_t mad_section_callback(U16 pid, struct si_filter_t *filter, U8 reason, U8 *sec_buffer, U32 length)
{
	MadSectionHeader header;
	int mad_filter_index = -1;
	int state = MAD_FILTER_STATE_NONE;

	mad_filter_index = find_mad_filter(pid/*, header.table_id*/);
	if (parse_mad_section_header(&header, sec_buffer, length))
	{
		printf("%s,%d , mad_filter_index = %d\n",__FUNCTION__,__LINE__,mad_filter_index);
		state = save_mad_section_data(mad_filter_index, header.current_section, header.total_section,
				sec_buffer + 8,	/* skip section header */
				header.section_length + 3 - 12 /* section_length + 3: the hole section data length; 12: header length & crc32 */);
	}
	if ((reason & SIE_REASON_FILTER_TIMEOUT))
	{
		state = MAD_FILTER_STATE_TIMEOUT;
	}

	if (need_stop_mad_filter(state))
	{
		//printf("%s,%d,  state = %d\n",__FUNCTION__,__LINE__,state);
		//printf("%s,%d , mad_filter_index = %d\n",__FUNCTION__,__LINE__,mad_filter_index);
		if (state == MAD_FILTER_STATE_FINISH)
		{
			do_mad_filter_callback(mad_filter_index, TRUE, get_mad_filter_data(mad_filter_index), get_mad_filter_data_length(mad_filter_index));
		}
		else
		{
			do_mad_filter_callback(mad_filter_index, FALSE, NULL, 0);
		}
		printf("free mad filter!\n");
		free_mad_filter(mad_filter_index);	

		return sie_freed;
	}

	return sie_started;
}

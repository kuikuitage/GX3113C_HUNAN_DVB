#include "maike_ads_type.h"
#include "app_common_porting_stb_api.h"
#include "gxcas_dbg.h"
#include "maike_ads_porting.h"
#include "mad_ts_sec.h"
#include "mad_interface.h"

#ifndef u32
#define u32 	unsigned int
#endif
#ifndef s32
#define s32 	signed int
#endif

#ifndef u16
#define u16 	unsigned short
#endif

#define ADS_MAD_DUMP

#if M_AD_TS_RUN_IN_BKG
int mad_data_monitor_id = MAD_INVALID_TASK;
#endif

mad_file_record mad_file_record_all;
extern MadFilter mad_filters[MAD_FILTER_MAX];
extern MadMutex mad_ts_mutex;
static AD_MONITOR_TASK_STATE cur_state = AD_MONITOR_TASK_IDLE;

void mad_ads_dump(U8* str,U8*buffer,U32 length)
{
#ifdef ADS_MAD_DUMP
	printf("str = %d,length = %d\n",str,length);
	int i = 0; 
	for(i = 0; i < length; i++)
	{
		if(1%20 == 0)
			printf("\n");
		printf("0x%x,",buffer[i]);
	}
#endif
}

void mad_ads_set_clint_state(AD_MONITOR_TASK_STATE state)
{
	cur_state = state;
}

static BOOL ads_filter_end_flag = FALSE;
BOOL get_ads_filter_end_flag(void)
{
	return ads_filter_end_flag;
}

void set_ads_filter_end_flag(BOOL flag)
{
	ads_filter_end_flag = flag;
}

u32 flash_get_mad_size(void)
{
	return 0x20000;
}

BOOL flash_update_mad_data(U8* data,U32 length)
{
	if(data != NULL)
	{
		app_porting_ads_flash_write_data(0,(uint8_t *)data,length);
		MAD_INF("zhumn~~~~~~~~~~flash_update_mad_data \n ");
		return TRUE;
	}
	return FALSE;
}

BOOL flash_read_mad_data(U8* data)
{
	uint32_t length = 0x20000;
	if(NULL != data)
	{
		app_porting_ads_flash_read_data(0,(uint8_t *)data,&length);
		MAD_INF("length = %x\n",length);
		return TRUE;
	}
	return FALSE;
}

void set_mad_filter_timeout(int mad_filter_index,int timeout)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].filter_ext.time_out = timeout;
	}
}
void set_mad_filter_channel_handle(int mad_filter_index,handle_t handle)
{
	if (mad_filter_index >= 0 && mad_filter_index < MAX_FILTER_NUM)
	{
		mad_filters[mad_filter_index].filter_ext.channel_handle = handle;
	}
}

BOOL SlotIndexIsValid(int chn_index)
{
	if((chn_index < MAD_FILTER_MAX)&&(chn_index >= 0))
		return mad_filters[chn_index].in_use;
	return FALSE;
}

void FreeSectionSlot(int chn_index)
{
	int ret = 0;
	MAD_INF("Free IN\n");
	if((chn_index < MAD_FILTER_MAX)&&(chn_index >= 0)&&(mad_filters[chn_index].in_use))
	{
		if(mad_filters[chn_index].sys_filter_id)
		{
			ret = GxDemux_FilterDisable(mad_filters[chn_index].sys_filter_id);
			CHECK_RET(DEMUX,ret);

			ret = GxDemux_FilterFree(mad_filters[chn_index].sys_filter_id);
			CHECK_RET(DEMUX,ret);
		}
		if(mad_filters[chn_index].filter_ext.channel_handle)
		{
			ret = GxDemux_ChannelDisable(mad_filters[chn_index].filter_ext.channel_handle);
			CHECK_RET(DEMUX,ret);

			ret = GxDemux_ChannelFree(mad_filters[chn_index].filter_ext.channel_handle);
			CHECK_RET(DEMUX,ret);
		}
		memset(&mad_filters[chn_index],0,sizeof(MadFilter));
		mad_filters[chn_index].in_use = FALSE;	
	}
	return ;
}

int SectionAllocSlot(GetMadDataCallback callback,u32 time_out, u16 pid)
{
	int index;

	printf("pid=0x%x  time_out=%d\n",pid,time_out);
	app_porting_psi_demux_lock();
	for(index=0; index<MAD_FILTER_MAX;index++)
	{
		if(mad_filters[index].in_use== FALSE)
		{
			mad_filters[index].cb = callback;
			mad_filters[index].pid = pid;
			mad_filters[index].in_use= TRUE;
			mad_filters[index].filter_ext.time_out = time_out;
			mad_filters[index].cb = callback;
			app_porting_psi_demux_unlock();
			printf("index%d\n",index);
			return index;
		}
	}
	app_porting_psi_demux_unlock();
	return -1;
}

int SectionAllocFilter(u8 index,u8 *szFilter,u8 *szMask, BOOL multi_section)
{
	int32_t  ret = 0;

	GxTime nowtime={0};
	memcpy(mad_filters[index].filter_ext.filter_mask, szMask,  DMX_SECTION_FILTER_SIZE);
	memcpy(mad_filters[index].filter_ext.filter_mask,szFilter,DMX_SECTION_FILTER_SIZE);
	ADS_Dbg("index = 	%d \n",index);
	ADS_DUMP("szFilter:",szFilter,DMX_SECTION_FILTER_SIZE);
	ADS_DUMP("szMask:",szMask,DMX_SECTION_FILTER_SIZE);
	app_porting_psi_demux_lock();
	if (0 != mad_filters[index].sys_filter_id)
	{
		ret = GxDemux_FilterFree(mad_filters[index].sys_filter_id);
		CHECK_RET(DEMUX,ret);
		mad_filters[index].sys_filter_id = 0;
	}
	if (0 != mad_filters[index].filter_ext.channel_handle)
	{
		ret = GxDemux_ChannelFree(mad_filters[index].filter_ext.channel_handle);
		CHECK_RET(DEMUX,ret);
		mad_filters[index].filter_ext.channel_handle = 0;
	}
	mad_filters[index].filter_ext.channel_handle  = GxDemux_ChannelAllocate(0, mad_filters[index].pid);
	printf("%d,%s,mad_filters[index].filter_ext.channel_handle = 0x%x\n",__LINE__,__FUNCTION__,mad_filters[index].filter_ext.channel_handle);
	if (( 0 == mad_filters[index].filter_ext.channel_handle)||(-1 == mad_filters[index].filter_ext.channel_handle))
	{
		printf("maike ads alloc channel handle faild 0x%x  \n",mad_filters[index].pid);
		app_porting_psi_demux_unlock();
		return -1;
	}
	mad_filters[index].sys_filter_id = GxDemux_FilterAllocate(mad_filters[index].filter_ext.channel_handle);
	printf("%d,%s,mad_filters[index].sys_filter_id = 0x%x\n",__LINE__,__FUNCTION__,mad_filters[index].sys_filter_id);
	if (( 0 == mad_filters[index].sys_filter_id)||(-1 == mad_filters[index].sys_filter_id))
	{
		ret = GxDemux_ChannelFree(mad_filters[index].filter_ext.channel_handle);
		CHECK_RET(DEMUX,ret);
		mad_filters[index].filter_ext.channel_handle = 0;
		mad_filters[index].sys_filter_id = 0;
		app_porting_psi_demux_unlock();
		return -1;
	}
	ret = GxDemux_FilterSetup(mad_filters[index].sys_filter_id, szFilter, szMask, TRUE, TRUE,0, 1);	
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_ChannelEnable(mad_filters[index].filter_ext.channel_handle);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(mad_filters[index].sys_filter_id);
	CHECK_RET(DEMUX,ret);
	app_porting_psi_demux_unlock();
	GxCore_GetTickTime(&nowtime);
	mad_filters[index].filter_ext.tick= nowtime.seconds;
	printf("mad_filters[index].sys_filter_id = 0x%x\n",mad_filters[index].sys_filter_id);
	return mad_filters[index].sys_filter_id;
}

void mad_disable_filter(index)
{
	handle_t channelhandle;
	uint16_t pid = 0x1fff;
	int32_t ret;

	if (( 0 != mad_filters[index].in_use)&&(0 != mad_filters[index].sys_filter_id)&&(0 != mad_filters[index].filter_ext.channel_handle))
	{
		pid = mad_filters[index].pid;
		channelhandle = mad_filters[index].filter_ext.channel_handle;

		ret = GxDemux_FilterDisable(mad_filters[index].sys_filter_id);
		CHECK_RET(DEMUX,ret);

		return;
	}
	return;	
}
void mad_enable_filter(index)
{
	handle_t channelhandle;
	uint16_t pid = 0x1fff;
	int32_t ret;

	if (( 0 != mad_filters[index].in_use)&&(0 != mad_filters[index].sys_filter_id)&&(0 != mad_filters[index].filter_ext.channel_handle))
	{
		pid = mad_filters[index].pid;
		channelhandle = mad_filters[index].filter_ext.channel_handle;

		ret = GxDemux_FilterEnable(mad_filters[index].sys_filter_id);
		CHECK_RET(DEMUX,ret);

		return ;
	}
	return;
}

static void mad_muti_section_callback(U16 pid, struct si_filter_t *filter, U8 reason, U8 *sec_buffer, U32 length)
{
	U32 sec_len = 0;
	U32 sec_num = 0;
	U32 len = length;
	U8* p_data_bak = sec_buffer;
	printf("len = %d\n",len);
	while(len > 0)
	{
		sec_len = (((uint32_t)p_data_bak[1] & 0x0F) <<8) | p_data_bak[2];
		sec_num = ((uint32_t)p_data_bak[3] << 8) | p_data_bak[4];
		MAD_INF("sec_len = %d,sec_num = %d\n",sec_len,sec_num);
		mad_section_callback(pid,NULL,0,p_data_bak,sec_len + 3);
		p_data_bak +=(sec_len+3);
		len-=(sec_len+3);
		MAD_INF("data length left = %d\n",len);
	}
}

static void mad_data_monitor(unsigned short arg1, unsigned short arg2)
{
	s32 ret = FALSE;
	u8 index = 0;
	u8 *p_data=NULL;
	uint8_t *section;/* section  buffer */
	uint32_t length;
	GxTime nowtime={0};

	section = (uint8_t *)GxCore_Malloc(MAD_FILTER_BUFFER_SIZE);
	if(section == NULL)
		return;
	
	while(1)
	{
		for(index = 0; index < MAD_FILTER_MAX; index++)
		{
			if(mad_filters[index].in_use == FALSE)
			{ 
				GxCore_ThreadDelay(40);
				continue;
			}
			ret = GxDemux_QueryAndGetData(mad_filters[index].sys_filter_id,section,MAD_FILTER_BUFFER_SIZE,&length);
			if(( ret >= 0)&&(length>0))
			{
				p_data = section;
				MAD_INF("index = %d table_id = 0x%x len = %d \n",index,p_data[0],length);
				//MAD_INF("%s %d index = %d mad_filters[index].table_id = 0x%x\n",__FUNCTION__,__LINE__,index,mad_filters[index].table_id);
				mad_muti_section_callback(mad_filters[index].pid,NULL,0,p_data,length);
#if 0
				if(mad_filters[index].cb == NULL)
				{
					MAD_INF("cb == NULL\n");
				}
				
				if(p_data[0] == mad_filters[index].table_id)
				{
					mad_filters[index].cb(mad_filters[index].pid,mad_filters[index].table_id,TRUE,p_data,length);
				}
#endif
			}
		}
		GxCore_ThreadDelay(10);
	}
}


static void mad_data_monitor_process(unsigned short arg1,unsigned short arg2)
{
	mad_data_monitor(0,NULL);
}
void mad_ads_client_init(void)
{
	int index = 0;
	handle_t handle = 0;
	u32 *p_stack = NULL;
	mad_data_monitor_id = mad_create_task(mad_data_monitor_process, 0, NULL, MADS_DATA_MONTION_TASK_STKSIZE, NULL, MAD_TASK_PRIORITY, (char*)"MAD DATA");
	MAD_INF("ads create task id or handle = %d\n",mad_data_monitor_id);
	return;
}

extern void mad_ts_task(unsigned short arg1, unsigned short arg2);
void  mad_ts_task_init(void)
{
	mad_ts_init();
	return;
}


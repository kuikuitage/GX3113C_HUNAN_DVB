/*
 * mad_ts.c
 *
 *  Created on: 2015-11-27
 */
#include "app_common_porting_stb_api.h"

#include "mad_cfg.h"
#if ENABLE_M_AD
#include "mad_interface.h"
#include "mad_util.h"
#include "mad.h"
#endif	/* ENABLE_M_AD */

#if ENABLE_M_AD_TS
#include "mad_ts_sec.h"
#include "mad_ts.h"
#include "maike_ads_porting.h"

#if M_AD_TS_RUN_IN_BKG
int mad_ts_task_id = MAD_INVALID_TASK;
#endif
MadMutex mad_ts_mutex = MAD_INVALID_MUTEX;
static MadTsTaskState mad_ts_task_state = MAD_TS_TASK_STATE_IDLE;

static MadHeader *mad_ts_header = NULL;
static MadDownloadState mad_header_download_state;
static MadDownloadState mad_download_state[M_AD_PLACE_NUM];

extern MadPlaceInfo mad_place_info[M_AD_PLACE_NUM];
extern MadPlace mad_place[M_AD_PLACE_NUM];


/***********************************************************************************
 *                                     common                                      *
 ***********************************************************************************/
static BOOL is_download_finish(int download_state)
{
	return download_state == MAD_DOWNLOAD_STATE_FINISH || download_state == MAD_DOWNLOAD_STATE_ERR_FINISH;
}


/***********************************************************************************
 *                                   get & set                                     *
 ***********************************************************************************/
static MadHeader *get_mad_header(void)
{
	return mad_ts_header;
}


static int get_mad_header_download_state(void)
{
	return mad_header_download_state;
}

static void set_mad_header_download_state(int state)
{
	mad_header_download_state = state;
}


static int get_mad_place_download_state(int place_index)
{
	if (place_index >= 0 && place_index < M_AD_PLACE_NUM)
	{
		return mad_download_state[place_index];
	}
	return MAD_DOWNLOAD_STATE_NUM;
}

static void set_mad_place_download_state(int place_index, int state)
{
	if (place_index >= 0 && place_index < M_AD_PLACE_NUM)
	{
		mad_download_state[place_index] = state;
	}
}


static MadTsPlaceInfo* get_mad_place_ts_info(MadHeader *mad_header, int place_id)
{
	int i;

	if (mad_header)
	{
		for (i = 0; i < mad_header->mad_ts_place_info_num; i++)
		{
			if (mad_header->mad_ts_place_infos[i].ad_place_id == place_id)
			{
				return &(mad_header->mad_ts_place_infos[i]);
			}
		}
	}
	return NULL;
}

static int get_mad_header_pid(int place_id)
{
	MadTsPlaceInfo *info;

	info = get_mad_place_ts_info(get_mad_header(), place_id);
	if (info)
	{
		return info->pid;
	}
	return -1;
}

static int get_mad_header_table_id(int place_id)
{
	MadTsPlaceInfo *info;

	info = get_mad_place_ts_info(get_mad_header(), place_id);
	if (info)
	{
		return info->table_id;
	}
	return -1;
}

static int get_mad_header_ad_place_version(int place_id)
{
	MadTsPlaceInfo *info;

	info = get_mad_place_ts_info(get_mad_header(), place_id);
	if (info)
	{
		return info->ad_place_ver;
	}
	return 0;
}


static int get_mad_place_id_by_pid(int pid)
{
	MadHeader *header = get_mad_header();
	int place_id = -1;
	int i;

	if (header)
	{
		for (i = 0; i < header->mad_ts_place_info_num; i++)
		{
			if (header->mad_ts_place_infos[i].pid == pid)
			{
				place_id = header->mad_ts_place_infos[i].ad_place_id;
			}
		}
	}
	return place_id;
}


/***********************************************************************************
 *                                  m-ad header                                    *
 ***********************************************************************************/
static void delete_mad_header(void)
{
	if (mad_ts_header)
	{
		if (mad_ts_header->mad_ts_place_infos)
		{
			mad_free(mad_ts_header->mad_ts_place_infos);
		}
		mad_free(mad_ts_header);
	}
	mad_ts_header = NULL;
}

static BOOL init_mad_header_with_data_v1(U8 *data, U32 length)
{
	const int HEADER_DATA_MIN_LENGTH = 22;
	int i;

	if (data == NULL || length < HEADER_DATA_MIN_LENGTH)
	{
		return FALSE;
	}
	if (mad_ts_header != NULL)
	{
		delete_mad_header();
	}
	mad_ts_header = (MadHeader *)mad_malloc(sizeof(MadHeader));
	if (mad_ts_header == NULL)
	{
		return FALSE;
	}
	mad_memset(mad_ts_header, 0, sizeof(MadHeader));

	mad_ts_header->header_version = M_AD_MAKE_USHORT(data);
	if (mad_ts_header->header_version != 1)
	{
		delete_mad_header();
		return FALSE;
	}
	data += 2;
	mad_ts_header->platform_id = M_AD_MAKE_USHORT(data);
	data += 2;
	mad_ts_header->customer_id = M_AD_MAKE_USHORT(data);
	data += 2;
	mad_ts_header->model_id = M_AD_MAKE_USHORT(data);
	data += 2;
	mad_memcpy(mad_ts_header->sn_start, data, 6);
	data += 6;
	mad_memcpy(mad_ts_header->sn_end, data, 6);
	data += 6;
	mad_ts_header->mad_ts_place_info_num = M_AD_MAKE_USHORT(data);
	data += 2;

	if ((mad_ts_header->mad_ts_place_info_num <= 0)
		|| (length < (HEADER_DATA_MIN_LENGTH + mad_ts_header->mad_ts_place_info_num * sizeof(MadTsPlaceInfo))))
	{
		delete_mad_header();
		return FALSE;
	}
	mad_ts_header->mad_ts_place_infos = (MadTsPlaceInfo *)mad_malloc(mad_ts_header->mad_ts_place_info_num * sizeof(MadTsPlaceInfo));
	if (mad_ts_header->mad_ts_place_infos == NULL)
	{
		delete_mad_header();
		return FALSE;
	}
	for (i = 0; i < mad_ts_header->mad_ts_place_info_num; i++)
	{
		MAD_INF("place info in ts, index: %d\n", i);
		mad_ts_header->mad_ts_place_infos[i].ad_place_id = M_AD_MAKE_ULONG(data);
		MAD_INF("place id: 0x%x\n", mad_ts_header->mad_ts_place_infos[i].ad_place_id);
		data += 4;
		mad_ts_header->mad_ts_place_infos[i].ad_place_ver = M_AD_MAKE_ULONG(data);
		MAD_INF("place version: 0x%x\n", mad_ts_header->mad_ts_place_infos[i].ad_place_ver);
		data += 4;
		mad_ts_header->mad_ts_place_infos[i].pid = M_AD_MAKE_USHORT(data);
		MAD_INF("place pid: 0x%x\n", mad_ts_header->mad_ts_place_infos[i].pid);
		data += 2;
		mad_ts_header->mad_ts_place_infos[i].ad_place_type = data[0];
		MAD_INF("place type: 0x%x\n", mad_ts_header->mad_ts_place_infos[i].ad_place_type);
		data += 1;
		mad_ts_header->mad_ts_place_infos[i].table_id = data[0];
		MAD_INF("place table_id: 0x%x\n", mad_ts_header->mad_ts_place_infos[i].table_id);
		data += 1;
	}

	return TRUE;
}

static BOOL check_mad_header(void)
{
	MadHeader *header;

	header = get_mad_header();
	if (header == NULL)
	{
		return FALSE;
	}
	if (mad_device_pid() != 0 && mad_device_pid() != header->platform_id)
	{
		MAD_WRN("platform id not match!");
		return FALSE;
	}
	if (mad_device_cid() != 0 && mad_device_cid() != header->customer_id)
	{
		MAD_WRN("customer id not match!");
		return FALSE;
	}
	if (mad_device_mid() != 0 && mad_device_mid() != header->model_id)
	{
		MAD_WRN("model id not match!");
		return FALSE;
	}
	return TRUE;
}

static BOOL init_mad_header_with_data(U8 *data, U32 length)
{
	unsigned short header_version;

	if (data == NULL || length < 2)
	{
		return FALSE;
	}
	/* first 2 bytes must always version data */
	header_version = M_AD_MAKE_USHORT(data);
	if (header_version != MAD_HEADER_VERSION)
	{
		MAD_ERR("m-ad ts header veriosn not match!\n");

		return FALSE;
	}
	if (!init_mad_header_with_data_v1(data, length))
	{
		return FALSE;
	}
	return check_mad_header();
}


static void get_mad_header_data(U16 pid, U8 table_id, BOOL success, U8 *data, U32 length)
{
	MAD_INF("get header data with length: %d\n", length);
	//ADS_DUMP("MKADS_DATA",data,length);
	if (success && init_mad_header_with_data(data, length))
	{
		set_mad_header_download_state(MAD_DOWNLOAD_STATE_FINISH);
	}
	else
	{
		MAD_ERR("download m-ad ts header error!\n");
		set_mad_header_download_state(MAD_DOWNLOAD_STATE_ERR_FINISH);
	}
}

static void start_fetch_mad_header(void)
{
	int mad_filter_index;

	delete_mad_header();
	set_mad_header_download_state(MAD_DOWNLOAD_STATE_NONE);
	mad_filter_index = request_mad_data(MAD_HEADER_PID, MAD_HEADER_TABLE_ID, MAD_HEADER_TIMEOUT, get_mad_header_data);
	MAD_INF("download mad header filter index:%d\n", mad_filter_index);
}

static BOOL is_mad_ts_header_download_finish(void)
{
	return is_download_finish(get_mad_header_download_state());
}

static BOOL is_valid_mad_ts_header(void)
{
	return get_mad_header() != NULL;
}


/***********************************************************************************
 *                                m-ad place data                                  *
 ***********************************************************************************/
/*
 * check which m-ad place data should be downloaded.
 */
static void init_mad_place_data_download_state(void)
{
	int i;
	int place_id;
	U32 version_in_dev, version_in_ts;

	for (i = 0; i < M_AD_PLACE_NUM; i++)
	{
		place_id = get_mad_place_id(i);
		printf("palce_id = 0x%x\n",place_id);
		version_in_dev = get_mad_place_content_version(place_id);
		version_in_ts = get_mad_header_ad_place_version(place_id);
		printf("version_in_dev = %d,version_in_ts = %d\n",version_in_dev,version_in_ts);
		/* no such ad place in ts or version is same, no need download */
		if(place_id == M_AD_PLACE_ID_SUBTITLE)
		{
			set_mad_place_download_state(i, MAD_DOWNLOAD_STATE_NONE);
			continue;
		}
		if ((version_in_ts == 0 || version_in_dev == version_in_ts))
		{
			MAD_INF("place id: %d version is same, no need download!\n", place_id);
			set_mad_place_download_state(i, MAD_DOWNLOAD_STATE_FINISH);
		}
		else
		{
			set_mad_place_download_state(i, MAD_DOWNLOAD_STATE_NONE);
		}
	}
}


static void get_mad_content_data(U16 pid, U8 table_id, BOOL success, U8 *data, U32 length)
{
	int place_id, place_index;
	U32 ad_place_version;
	
	MAD_INF("get content data with pid: %d and length: %d\n", pid, length);
	place_id = get_mad_place_id_by_pid(pid);
	MAD_INF("place id:%d\n", place_id);
	place_index = get_mad_place_index_by_id(place_id);
	ad_place_version = get_mad_header_ad_place_version(place_id);

	if (success && handle_mad_place_data(place_id, ad_place_version, data, length))
	{
		MAD_INF("place id = %d,ad place data download finish!\n",place_id);
		set_mad_place_download_state(place_index, MAD_DOWNLOAD_STATE_FINISH);
	}
	else
	{
		MAD_INF("ad place data download error finish!\n");
		set_mad_place_download_state(place_index, MAD_DOWNLOAD_STATE_ERR_FINISH);
	}
	
}


extern void destroy_mad_place(MadPlace *mad_place);
static void start_fetch_mad_place_content_data(void)
{
	int mad_filter_index;
	int i;

	for (i = 0; i < M_AD_PLACE_NUM; i++)
	{
		if (get_mad_place_download_state(i) == MAD_DOWNLOAD_STATE_NONE && get_free_mad_filter_num() > 0)
		{
			destroy_mad_place(&(mad_place[i]));
			mad_filter_index = request_mad_data(get_mad_header_pid(get_mad_place_id(i)),
					get_mad_header_table_id(get_mad_place_id(i)), MAD_DATA_TIMEOUT, get_mad_content_data);
			MAD_INF("mad filter index:%d\n", mad_filter_index);
			set_mad_place_download_state(i, MAD_DOWNLOAD_STATE_RUNNING);
		}
	}
}

static BOOL is_all_mad_place_data_download_finish(void)
{
	int i;

	for (i = 0; i < M_AD_PLACE_NUM; i++)
	{
		if (!is_download_finish(get_mad_place_download_state(i)))
		{
			return FALSE;
		}
	}
	MAD_INF("all ad place data download finish!!\n");
	return TRUE;
}


/***********************************************************************************
 *                                     cleanup                                     *
 ***********************************************************************************/
static void mad_ts_cleanup(void)
{
	delete_mad_header();
	free_all_mad_filter();
}


/***********************************************************************************
 *                                   mad ts task                                   *
 ***********************************************************************************/
static void set_mad_ts_task_state(int new_state)
{
	if (mad_ts_mutex == MAD_INVALID_MUTEX)
	{
		return;
	}
	mad_mutex_lock(mad_ts_mutex, MAD_MUTEX_WAIT_FOREVER);
	mad_ts_task_state = new_state;
	mad_mutex_unlock(mad_ts_mutex);
}



void mad_ts_task(U32 argc, void *argv)
{
	//MadDownloadState new_state = MAD_TS_TASK_STATE_NUM;
	//gw::MadTsTaskState
	MadTsTaskState new_state = MAD_TS_TASK_STATE_NUM;
	const unsigned int CHECK_UPDATE_INTERVAL = 5*1000;	//30s
	int sleep_time = CHECK_UPDATE_INTERVAL;
	BOOL exit = FALSE;
	BOOL run_in_main_thread;

	run_in_main_thread = TRUE/*argc > 0 ? TRUE : FALSE*/;
	//first run clearnup
	mad_ts_cleanup();
	
	while (!exit)
	{
#if 0
		int x;
		int y;
		for(x = 0; x < M_AD_PLACE_NUM;x++)
		{
			printf("mad_place[%d].data_item_num = %d\n",x,mad_place[x].data_item_num);
			for(y = 0; y < mad_place[x].data_item_num;y++)
			{
				printf("\tmad_place[%d].data_items[%d].data_type = 0x%x\n",x,y,mad_place[x].data_items[y].data_type);
				printf("\tmad_place[%d].data_items[%d].data_length = %u\n",x,y,mad_place[x].data_items[y].data_length);
			}
		}
#endif	
		mad_mutex_lock(mad_ts_mutex, MAD_MUTEX_WAIT_FOREVER);
		switch (mad_ts_task_state)
		{
			case MAD_TS_TASK_STATE_IDLE:
				sleep_time = CHECK_UPDATE_INTERVAL;
				break;
			case MAD_TS_TASK_STATE_INIT:
				printf("-----------start fetch data--------------%d\n",__LINE__);
				start_fetch_mad_header();
				new_state = MAD_TS_TASK_STATE_WAIT_HEADER;
				sleep_time = 100;
				break;
			case MAD_TS_TASK_STATE_WAIT_HEADER:
				if (is_mad_ts_header_download_finish())
				{
					if (is_valid_mad_ts_header())
					{
						init_mad_place_data_download_state();
						new_state = MAD_TS_TASK_STATE_WAIT_DATA;
					}
					else
					{
						new_state = MAD_TS_TASK_STATE_STOP;
					}
				}
				sleep_time = 100;
				break;
			case MAD_TS_TASK_STATE_WAIT_DATA:
				if (!is_all_mad_place_data_download_finish())
				{
					start_fetch_mad_place_content_data();
					sleep_time = 100;
				}
				else
				{
					if (run_in_main_thread)
					{
						new_state = MAD_TS_TASK_STATE_STOP;
					}
					else
					{
						mad_ts_cleanup();
						sleep_time = CHECK_UPDATE_INTERVAL;
						new_state = MAD_TS_TASK_STATE_INIT;
					}
				}
				break;
			case MAD_TS_TASK_STATE_STOP:
				printf("-----------TS_TASK_STATE_STOP--------------%d\n",__LINE__);
				save_all_basic_info_to_flash();
				mad_ts_cleanup();
				new_state = MAD_TS_TASK_STATE_IDLE;
				sleep_time = CHECK_UPDATE_INTERVAL;
				if (run_in_main_thread)
				{
					exit = TRUE;
				}
				break;
			default:
				break;
		}
		mad_mutex_unlock(mad_ts_mutex);

		if (new_state != MAD_TS_TASK_STATE_NUM)
		{
			MAD_INF("mad ts task new state:%d\n", new_state);
			set_mad_ts_task_state(new_state);
		}
		new_state = MAD_TS_TASK_STATE_NUM;
		mad_task_sleep(sleep_time);
	}
	MAD_INF("mad ts task finish\n");
}


#if M_AD_TS_RUN_IN_BKG

int start_monitor_mad(void)
{
	set_mad_ts_task_state(MAD_TS_TASK_STATE_INIT);
	printf("set task state = %d\m",MAD_TS_TASK_STATE_INIT);
	return 0;
}

int stop_monitor_mad(void)
{
	printf("----------stop monitor mad!!!\n");
	if (mad_ts_mutex == MAD_INVALID_MUTEX)
	{
		return -1;
	}
	mad_mutex_lock(mad_ts_mutex, MAD_MUTEX_WAIT_FOREVER);
	mad_ts_cleanup();
	mad_mutex_unlock(mad_ts_mutex);

	set_mad_ts_task_state(MAD_TS_TASK_STATE_STOP);

	return 0;
}

#else

int mad_ts_update(void)
{
	set_mad_ts_task_state(MAD_TS_TASK_STATE_INIT);
	mad_ts_task(1, NULL);

	return 0;
}

#endif	/* M_AD_TS_RUN_IN_BKG */


int mad_ts_init(void)
{
	if (mad_ts_mutex == MAD_INVALID_MUTEX)
	{
		mad_create_mutex(&mad_ts_mutex, "MAD TS");
	}
#if M_AD_TS_RUN_IN_BKG
	if (mad_ts_task_id == MAD_INVALID_TASK)
	{
		mad_ts_task_id = mad_create_task((MadTaskEntry)mad_ts_task, 0, NULL, MAD_TASK_STACK_SIZE, NULL, MAD_TASK_PRIORITY, (char*)"MAD TS");
	}
	if (mad_ts_task_id == MAD_INVALID_TASK)
	{
		MAD_ERR("create mad ts task fail!\n");
	}
#endif
	MAD_INF("mad_ts_init task id = %d\n",mad_ts_task_id);
	return mad_ts_task_id;
}

#endif	/* ENABLE_M_AD_TS */

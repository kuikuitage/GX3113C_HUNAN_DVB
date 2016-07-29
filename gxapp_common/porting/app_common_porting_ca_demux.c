/*
* 此文件实现CA移植DEMUX公共接口
*/
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_stb_api.h"
#include "app_common_table_cat.h"
#include "app_common_table_nit.h"
#include "app_common_table_ota.h"
#include "app_common_table_pmt.h"
#include "gxapp_sys_config.h"
#include "app_common_porting_ca_os.h"
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
#include "app_by_cas_api_demux.h"
#endif
#endif
#ifdef DVB_AD_TYPE_BY_FLAG
#include "app_by_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_3H_FLAG
#include "3h_ads_porting.h"
#endif
#ifdef DVB_AD_TYPE_JIUZHOU_FLAG
#include "jiuzhou_ads_porting.h"
#endif

#ifdef DVB_SERIAL_SUPPORT
#include "app_serial_table_ts.h"
#endif
//#define MAX_SECTION_BUFFER (16*1024)
#define MAX_PSI_SECTION_BUFFER (64*1024)
#define MAX_CA_SECTION_BUFFER (128*1024)


static ca_filter_t cafilter[MAX_FILTER_COUNT];
static handle_t   ca_demux_sem;
static handle_t   psi_demux_lock=0;
extern 	void app_table_pmt_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
extern  void app_table_cat_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
extern 	 void app_table_nit_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
extern 	 void app_table_ota_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
extern 	 void app_table_ts_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
#define PORTING_CA_DEMUX_STOP             0
#define PORTING_CA_DEMUX_ENABLE           1
#define PORTING_CA_DEMUX_DISABLE          2
int running = PORTING_CA_DEMUX_DISABLE;
void printf_cafilter(char * title);


int get_porting_query_demux_runflag(void)
{
	if(running== PORTING_CA_DEMUX_DISABLE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void app_porting_disable_query_demux(void)
{
	app_porting_ca_demux_disable_all_filter();
    running = PORTING_CA_DEMUX_DISABLE;
	//GxCore_ThreadDelay(1000);
	//app_porting_ca_demux_disable_all_filter();
	printf_cafilter("app_porting_disable_query_demux");
}

void app_porting_enable_query_demux(void)
{
    running = PORTING_CA_DEMUX_ENABLE;
	app_porting_ca_demux_enable_all_filter();
}
extern uint32_t dvb_crc32(uint8_t* pBuffer, uint32_t nSize);

void printf_cafilter(char * title)
{
	int i;
	int j;
	printf("\n");
	printf("%s",title);
	for(i=0;i<MAX_FILTER_COUNT;i++){
		printf("\n");
		printf("cafilter[%d].pid = 0x%x \n",i,cafilter[i].pid);
		printf("cafilter[%d].usedStatus = 0x%x \n",i,cafilter[i].usedStatus);
		printf("cafilter[%d].Callback = %p \n",i,cafilter[i].Callback);
		printf("cafilter[%d].timeOutCallback = %p \n",i,cafilter[i].timeOutCallback);
		printf("cafilter[%d].byReqID = 0x%x \n",i,cafilter[i].byReqID);
		printf("cafilter[%d].channelhandle = 0x%x \n",i,cafilter[i].channelhandle);
		printf("cafilter[%d].handle = 0x%x \n",i,cafilter[i].handle);
		printf("cafilter[%d].filterLen = 0x%x \n",i,cafilter[i].filterLen);
		printf("cafilter[%d].match =",i);
		for(j=0;j<cafilter[i].filterLen;j++){
			printf(" 0x%02x",cafilter[i].match[j]);
		}
		printf("\n");
		printf("cafilter[%d].mask =",i);
		for(j=0;j<cafilter[i].filterLen;j++){
			printf(" 0x%02x",cafilter[i].mask[j]);
		}
		printf("\n");
	}
}
int app_porting_ca_demux_check_same_filter(ca_filter_t* filter)
{
	int i;
	for(i=0;i<MAX_FILTER_COUNT;i++){
		if((cafilter[i].pid == filter->pid)
			&&(cafilter[i].Callback == filter->Callback)
			&&(cafilter[i].timeOutCallback == filter->timeOutCallback)
			&&(cafilter[i].filterLen == filter->filterLen)){
				if(memcmp(cafilter[i].match,filter->match,filter->filterLen) == 0
					&&memcmp(cafilter[i].mask,filter->mask,filter->filterLen) == 0)
					return i;
		}
			
	}
	return -1;
}

int32_t app_porting_ca_section_filter_crc32_check(uint8_t *pSection)
{
	uint16_t nlength = 0;
	uint32_t nCrc32 = 0;
	uint32_t nCrc32Result = 0;
	uint8_t *pdata = NULL;
	uint8_t *pndata = NULL;
	uint8_t	chTableId = 0;
	uint8_t	chSectionNumber          = 0;
	//uint8_t	chLastSectionNumber     = 0;

	pdata					= pSection;
	chTableId				= pdata[0];
	nlength					= (uint16_t)((pdata[1]&0x0F)<<8)|pdata[2];
    chSectionNumber			= pdata[6];
	//chLastSectionNumber		= pdata[7];


	 /*TDT表没CRC校验*/
	if(0x70 == chTableId)
		return 0;
	else if ((chTableId >= 0x80) && (chTableId <= 0xFE)){
		if(((pdata[1])&0x80) == 0)
			return 0;
	}

	if(nlength<1)
		return 1;
	nlength = nlength + 3 - 4;
	nCrc32Result = dvb_crc32(pdata, nlength);

	pndata = pdata + nlength;
	nCrc32 = ((pndata[0]<<24)&0xff000000) | ((pndata[1]<<16)&0x00ff0000)
		         | ((pndata[2]<<8)&0x0000ff00) | ((pndata[3]<<0)&0x000000ff);

	if(nCrc32Result == nCrc32)
		return  0;
	else{
		printf("[SI] [table_id]=0x%x [length]=0x%x  [nCrc32Result]=0x%x  [nCrc32]=0x%x \n"
		       ,chTableId, nlength, nCrc32Result, nCrc32);
		printf("\n\nsci   soft crc32 error!!!!!!!!!!!!!!\n\n");
		return  1;
	}
}


void app_porting_psi_demux_task(void* args)
{
    uint8_t *section;/* section  buffer */
    uint8_t *pBuffer;
    uint32_t length;
    int32_t ret;
	handle_t handle;
	
    section = GxCore_Malloc(MAX_PSI_SECTION_BUFFER);

    if( section == NULL )
    {
        DEBUG(DEMUX,ERR,"alloc memory failure!\n");
        return;
    }
    DEBUG(DEMUX,INFO,"app_porting_psi_demux_task running!\n");
	running = PORTING_CA_DEMUX_ENABLE;
    while(1)
    {
		/*
		* check psi pmt ,cat ,nit
		*/
        if (PORTING_CA_DEMUX_ENABLE != running) {
            GxCore_ThreadDelay(500);
            continue;
        }		
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
		handle = app_table_pmt_get_filter_handle();
		if (0 != handle)
			{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
				ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
				if(( ret >= 0)&&(length>0))
            	{
                	pBuffer = section;
					app_table_pmt_filter_notify(handle,pBuffer,length);
				}				
			}
		
		handle = app_table_cat_get_filter_handle();
		if (0 != handle)
			{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
				ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
				if(( ret >= 0)&&(length>0))
            	{
                	pBuffer = section;
					app_table_cat_filter_notify(handle, pBuffer, length);
				}				
			}
#endif

		handle = app_table_nit_get_filter_handle();		
		if (0 != handle)
		{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
			ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
			if(( ret >= 0)&&(length>0))
			{
				pBuffer = section;
				app_table_nit_filter_notify(handle,pBuffer,length);
			}				
		}
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
		handle = app_table_ota_get_filter_handle(); 	
		if (0 != handle)
		{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
			ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
			if(( ret >= 0)&&(length>0))
			{
				pBuffer = section;
				app_table_ota_filter_notify(handle,pBuffer,length);
			}				
		}

#ifdef DVB_SERIAL_SUPPORT
		handle = app_table_ts_get_filter_handle(); 	
		if (0 != handle)
		{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
			ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
			if(( ret >= 0)&&(length>0))
			{
				pBuffer = section;
				app_table_ts_filter_notify(handle,pBuffer,length);
			}				
		}
#endif
#endif
		/*
		* add by ca bat notify
		*/
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
		handle = app_by_cas_api_bat_get_filter_handle(); 	
		if (0 != handle)
			{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
				ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
				if(( ret >= 0)&&(length>0))
				{
					pBuffer = section;
					app_by_cas_api_bat_filter_notify(handle,pBuffer,length);
				}				
			}
#endif
#if 0//def DVB_CA_TYPE_QILIAN_FLAG
		handle = app_qilian_cas_api_bat_get_filter_handle(); 	
		if (0 != handle)
			{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
				ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
				if(( ret >= 0)&&(length>0))
				{
					pBuffer = section;
					app_qilian_cas_api_bat_filter_notify(handle,pBuffer,length);
				}				
			}
#endif
#endif

#ifdef DVB_AD_TYPE_BY_FLAG
		handle = app_by_ads_get_filter_handle(); 	
		if (0 != handle)
			{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
				ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
				if(( ret >= 0)&&(length>0))
				{
					pBuffer = section;
					app_by_ads_filter_notify(handle,pBuffer,length);
				}				
			}
#endif

#ifdef DVB_AD_TYPE_3H_FLAG
				handle = app_3h_ads_get_filter_handle();	
				if (0 != handle)
					{
		//				memset(section,0,MAX_PSI_SECTION_BUFFER);
						ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
						if(( ret >= 0)&&(length>0))
						{
							pBuffer = section;
							app_3h_ads_filter_notify(handle,pBuffer,length);
						}				
					}
#endif

#ifdef DVB_AD_TYPE_JIUZHOU_FLAG
	handle = app_jiuzhou_ads_get_pf_filter_handle();	
	if (0 != handle)
		{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
			ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
			if(( ret >= 0)&&(length>0))
			{
				pBuffer = section;
				app_jiuzhou_ads_filter_notify(handle,pBuffer,length);
			}				
		}
	handle = app_jiuzhou_ads_get_logo_filter_handle();	
	if (0 != handle)
		{
//				memset(section,0,MAX_PSI_SECTION_BUFFER);
			ret = GxDemux_QueryAndGetData(handle,section,MAX_PSI_SECTION_BUFFER,&length);
			if(( ret >= 0)&&(length>0))
			{
				pBuffer = section;
				app_jiuzhou_ads_filter_notify(handle,pBuffer,length);
			}				
		}
#endif


        GxCore_ThreadDelay(50);
    }	
}

static void app_porting_ca_demux_task(void* args)
{
    uint8_t *section;/* section  buffer */
    uint8_t *pBuffer;
    uint32_t length;
    int32_t ret;
	uint8_t filterId;
	handle_t handle;
	ca_filter_t Filter = {0};
	ca_filter_t* sfilter = NULL;

	GxTime nowtime={0};
	uint32_t ends;
	
    section = GxCore_Malloc(MAX_CA_SECTION_BUFFER);

    if( section == NULL )
    {
        DEBUG(DEMUX,ERR,"alloc memory failure!\n");
        return;
    }
    DEBUG(DEMUX,INFO,"app_porting_ca_demux_task running!\n");
	
    while(1)
    {
		/*
		* check ecm ,emm 
		*/
		if (PORTING_CA_DEMUX_ENABLE != running) {
			GxCore_ThreadDelay(500);
			continue;
		}

		
		for (filterId = 0; filterId<MAX_FILTER_COUNT;filterId++)
		{
			
			#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
			sfilter = app_porting_ca_demux_get_by_filter_id(filterId);
			memcpy(&Filter,sfilter,sizeof(ca_filter_t));
			#else
			sfilter = app_porting_ca_demux_get_by_filter_id(filterId,&Filter);
			#endif
			if (( 0 != Filter.usedStatus)&&(0 != Filter.handle)&&
				(0!=Filter.channelhandle))
				{
					handle = Filter.handle;
//					memset(section,0,MAX_CA_SECTION_BUFFER);
					ret = GxDemux_QueryAndGetData(handle,section,MAX_CA_SECTION_BUFFER,&length);
					if(( ret >= 0)&&(length>0))
                	{
                    	pBuffer = section;
						if (Filter.Callback != NULL) 
						{
							if (0 != Filter.nWaitSeconds)
								{
									/*
									* re calcate time counts
									*/
									GxCore_GetTickTime(&nowtime);
									ends = nowtime.seconds;
									app_porting_ca_os_sem_wait(ca_demux_sem);
									sfilter->nms=ends;
									app_porting_ca_os_sem_signal(ca_demux_sem);
								}
                    		Filter.Callback(handle, pBuffer, length);
						}
					}
					else
						{
							/*
							* deal without data 
							*/
							if (0 != Filter.nWaitSeconds)
								{
									/*
									* judge timeout or not 
									*/
									
									GxCore_GetTickTime(&nowtime);
									ends = nowtime.seconds;
									if (ends - Filter.nms >= Filter.nWaitSeconds)
										{
//											printf(" filter timeout pid=0x%x,filterId=%d Filter->nWaitSeconds=%d ends=%d Filter->nms=%d\n",
//												Filter->pid,Filter->byReqID,Filter->nWaitSeconds,ends,Filter->nms);
//											Filter->nWaitSeconds=0;
											app_porting_ca_os_sem_wait(ca_demux_sem);
											sfilter->nms=ends;
											app_porting_ca_os_sem_signal(ca_demux_sem);
											if (Filter.timeOutCallback != NULL) 
											{
					                    		Filter.timeOutCallback(handle, NULL, 0);
											}
																												
										}
								}
						}
				}
		}

		GxCore_ThreadDelay(20);
    }
}

void app_porting_ca_create_filter_task(void)
{
	handle_t    ca_filter;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	uint32_t priority = GXOS_DEFAULT_PRIORITY;

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		priority = GXOS_DEFAULT_PRIORITY-1;
#endif	
	}	
	GxCore_ThreadCreate("filter_ca",&ca_filter, app_porting_ca_demux_task, NULL, 20 * 1024, priority);
	return;	
}

void app_porting_psi_create_filter_task(void)
{
	handle_t    psi_filter;
	GxCore_MutexCreate(&psi_demux_lock);
	GxCore_ThreadCreate("filter_psi",&psi_filter, app_porting_psi_demux_task, NULL, 10 * 1024, GXOS_DEFAULT_PRIORITY);
	return;	
}

void app_porting_psi_demux_lock(void)
{
	GxCore_MutexLock(psi_demux_lock);
	return;	
}

void app_porting_psi_demux_unlock(void)
{
	GxCore_MutexUnlock(psi_demux_lock);
	return;	
}


uint8_t app_porting_ca_demux_init()
{
	app_porting_ca_os_sem_create(&ca_demux_sem,1);
	memset(&cafilter[0],0,MAX_FILTER_COUNT*sizeof(ca_filter_t)) ;
	return 0;
}


#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG

ca_filter_t* app_porting_ca_demux_get_by_filter_id(uint32_t filter_id)
{
	return &cafilter[filter_id];
}


#else


ca_filter_t* app_porting_ca_demux_get_by_filter_id(uint32_t filter_id,ca_filter_t* filter)
{
	ca_filter_t* sfilter = NULL;

	if (NULL == filter)
		return NULL;

	app_porting_ca_os_sem_wait(ca_demux_sem);
	memcpy(filter,&cafilter[filter_id],sizeof(ca_filter_t));
	sfilter = &cafilter[filter_id];
	app_porting_ca_os_sem_signal(ca_demux_sem);
	return sfilter;
}

#endif

uint8_t app_porting_ca_demux_disable_filter( uint32_t   filterId)
{
	handle_t channelhandle;
	uint16_t pid = 0x1fff;
	int32_t ret;

	if (( 0 != cafilter[filterId].usedStatus)&&(0 != cafilter[filterId].handle)&&(0 != cafilter[filterId].channelhandle))
	{
		pid = cafilter[filterId].pid;
		channelhandle = cafilter[filterId].channelhandle;

		ret = GxDemux_FilterDisable(cafilter[filterId].handle);
		CHECK_RET(DEMUX,ret);

		return 0;
	}

	return 0;	
}

uint8_t app_porting_ca_demux_enable_filter( uint32_t   filterId)
{
	handle_t channelhandle;
	uint16_t pid = 0x1fff;
	int32_t ret;

	if (( 0 != cafilter[filterId].usedStatus)&&(0 != cafilter[filterId].handle)&&(0 != cafilter[filterId].channelhandle))
	{
		pid = cafilter[filterId].pid;
		channelhandle = cafilter[filterId].channelhandle;

		ret = GxDemux_FilterEnable(cafilter[filterId].handle);
		CHECK_RET(DEMUX,ret);

		return 0;
	}

	return 0;	
}

uint8_t app_porting_ca_demux_release_filter( uint32_t   filterId,uint32_t release_channel_flag)
{
	uint32_t i =0;
	bool releaseChannel = TRUE;
	handle_t channelhandle;
	uint16_t pid = 0x1fff;
	int32_t ret;
	app_porting_ca_os_sem_wait(ca_demux_sem);

	if (( 0 != cafilter[filterId].usedStatus)&&(0 != cafilter[filterId].handle)&&(0 != cafilter[filterId].channelhandle))
	{
		pid = cafilter[filterId].pid;
		channelhandle = cafilter[filterId].channelhandle;
		/* 释放FILTER*/
		//		ret = GxDemux_FilterDisable(cafilter[filterId].handle);
		//		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(cafilter[filterId].handle);
		CHECK_RET(DEMUX,ret);
		memset(&cafilter[filterId],0,sizeof(ca_filter_t));


		// 查询是否需要释放整个FILTER

		for (i  = 0; i < MAX_FILTER_COUNT;i ++)
		{
			if (( 0 != cafilter[i ].usedStatus)&&(0 != cafilter[i ].handle)
					&&(pid == cafilter[i ].pid)&&(channelhandle == cafilter[i ].channelhandle))
			{
				releaseChannel = FALSE;
				break;
			}
		}

		if ((TRUE == releaseChannel)&&(TRUE == release_channel_flag))
		{
			/*NIT表等CHANNEL不由CA库释放*/
			//			CAS_Dbg("app_porting_ca_demux_release_filter    pid=0x%x \n",
			//				pid);
			ret = GxDemux_ChannelFree(channelhandle);
			CHECK_RET(DEMUX,ret);
		}

		app_porting_ca_os_sem_signal(ca_demux_sem);
		return 0;
	}
	app_porting_ca_os_sem_signal(ca_demux_sem);

	return 0;	
}

uint8_t app_porting_ca_demux_start_filter(ca_filter_t* filter)
{
	int32_t filterId =0;
	handle_t channelhandle=0;
	handle_t filterhandle=0;
	GxTime starttime={0};
	uint32_t starts;
	int32_t ret = 0;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	app_porting_ca_os_sem_wait(ca_demux_sem);

	// 查找相同PID的CHANNEL

	//Find the channel existed.
	for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		if ((TRUE == cafilter[filterId].usedStatus)&&(filter->pid== cafilter[filterId].pid)
				&&(0!=cafilter[filterId].channelhandle))
		{
			channelhandle = cafilter[filterId].channelhandle;
			break;
		}
	}
	filterhandle = 0;

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
	{		
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		//Find the filter existed.
		for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
		{
			if(channelhandle && channelhandle == cafilter[filterId].channelhandle && 0!=cafilter[filterId].handle
					&& filter->byReqID == cafilter[filterId].byReqID)
			{
				filterhandle = cafilter[filterId].handle;
				break;

			}
			if ((FALSE == cafilter[filterId].usedStatus)&&(0 == cafilter[filterId].handle)
					&&(0==cafilter[filterId].channelhandle))
			{
				break;
			}
		}
		//Filter is existed.
		if(channelhandle && filterhandle)
		{
			//break;
		}
		//Find the unused fiter.
#endif
	}
	for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		if ((FALSE == cafilter[filterId].usedStatus)&&(0 == cafilter[filterId].handle)
				&&(0==cafilter[filterId].channelhandle))
		{
			break;
		}
	}

		if (filterId >= MAX_FILTER_COUNT)
		{
			CA_FAIL("app_porting_ca_demux_start_filter Failed,filterId=%d >= %d,pid:0x%x\n",filterId,MAX_FILTER_COUNT,filter->pid);
			printf_cafilter("app_porting_ca_demux_start_filter Failed");
			app_porting_ca_os_sem_signal(ca_demux_sem);
		
			return FALSE;
		}
	
	memcpy(&cafilter[filterId],filter,sizeof(ca_filter_t));

	cafilter[filterId].channelhandle = channelhandle;

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		cafilter[filterId].handle        = filterhandle;
#endif
	}

	// 设置CHANNEL PID
	if (0 == cafilter[filterId].channelhandle)
	{
		cafilter[filterId].channelhandle = GxDemux_ChannelAllocate(0, cafilter[filterId].pid);
	}
	else
	{
		//		GxDemux_ChannelDisable(cafilter[filterId].channelhandle);
		GxDemux_ChannelSetPID(cafilter[filterId].channelhandle,cafilter[filterId].pid,cafilter[filterId].repeatFlag);
	}

	if (( 0 == cafilter[filterId].channelhandle)||(-1 == cafilter[filterId].channelhandle))
	{
		CA_ERROR(" Call CDSTBCA_SetPrivateDataFilter Error cafilter[%d].channelhandle=0\n",filterId);
		memset(&cafilter[filterId],0,sizeof(ca_filter_t));
		app_porting_ca_os_sem_signal(ca_demux_sem);
		return FALSE;
	}


	// 申请FILTER、设置过滤条件

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		if(0 == cafilter[filterId].handle)
		{
			cafilter[filterId].handle = GxDemux_FilterAllocate( cafilter[filterId].channelhandle);
		}
		else
		{
			GxDemux_FilterDisable(cafilter[filterId].handle);
		}
#endif
	}
	else
	{	
		cafilter[filterId].handle = GxDemux_FilterAllocate( cafilter[filterId].channelhandle);
	}

	if (( 0 == cafilter[filterId].handle)||(-1 == cafilter[filterId].handle))
	{
		CA_ERROR(" Call CDSTBCA_SetPrivateDataFilter Error cafilter[%d].handle=0\n",filterId);

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		if(!channelhandle)
		{
			GxDemux_ChannelFree(cafilter[filterId].channelhandle);
		}
#endif
	}
		memset(&cafilter[filterId],0,sizeof(ca_filter_t));
		app_porting_ca_os_sem_signal(ca_demux_sem);
		return FALSE;
	}

	// 设置回调函数、超时回调函数
	//	GxCA_DemuxFilterRigsterNotify(cafilter[filterId].handle, cafilter[filterId].Callback,NULL);


	//  设置过滤条件

	ret = GxDemux_FilterSetup(cafilter[filterId].handle,
			cafilter[filterId].match,
			cafilter[filterId].mask,
			cafilter[filterId].equalFlag,
			cafilter[filterId].crcFlag,
			0,
			cafilter[filterId].filterLen);
	if(ret < 0)
	{
		CA_FAIL("app_porting_ca_demux_start_filter Failed,filterId=%d\n",filterId);
	}
	CHECK_RET(DEMUX,ret);

	// 使能FILTER
	cafilter[filterId].usedStatus = TRUE;
	ret = GxDemux_ChannelEnable(cafilter[filterId].channelhandle);
	if(ret < 0)
	{
		CA_FAIL("app_porting_ca_demux_start_filter Failed,filterId=%d\n",filterId);
	}
	CHECK_RET(DEMUX,ret);

	GxCore_GetTickTime(&starttime);
	starts = starttime.seconds;
	cafilter[filterId].nms= starts;

	memcpy(filter,&cafilter[filterId],sizeof(ca_filter_t));
	//remove by xushx 20131217
	//	filter->byReqID = filterId;
#ifndef DVB_CA_TYPE_CD_CAS30_FLAG //wufei add for CDCA
	filter->byReqID = filterId;
#endif	

	ret = GxDemux_FilterEnable(cafilter[filterId].handle);
	if(ret < 0)
	{
		CA_FAIL("app_porting_ca_demux_start_filter Failed,filterId=%d\n",filterId);
	}
	CHECK_RET(DEMUX,ret);
	app_porting_ca_os_sem_signal(ca_demux_sem);

	//CAS_Dbg("End STBCA_SetPrivateDataFilter...\n");
	return TRUE;
}

void app_porting_ca_demux_disable_all_filter(void)
{
	int i;

	for(i=0;i<MAX_FILTER_COUNT;i++){
		app_porting_ca_demux_disable_filter(i);
	}
}

void app_porting_ca_demux_enable_all_filter(void)
{
	int i;

	for(i=0;i<MAX_FILTER_COUNT;i++){
		app_porting_ca_demux_enable_filter(i);
	}
}
/*
common interface , add for abv 
*/

int app_porting_ca_demux_stop_filter(unsigned int  filterId)
{
    int32_t ret;

	app_porting_ca_os_sem_wait(ca_demux_sem);

	if ( ( 0 != cafilter[filterId].usedStatus)&&(0 != cafilter[filterId].handle)&&
		 ( 0 != cafilter[filterId].channelhandle) )
	{
		ret = GxDemux_FilterDisable(cafilter[filterId].handle);

		ret = 0 ;
	}
	else
	{
		ret = -1 ;
	}

	app_porting_ca_os_sem_signal(ca_demux_sem);

	return ret ;	
}


int app_porting_ca_demux_set_filter(ca_filter_t* filter)
{
	int32_t slotid =0;
	int32_t pid = 0 ;
	int32_t filterId =0;
	handle_t channelhandle=0;
	int32_t ret = 0;

	app_porting_ca_os_sem_wait(ca_demux_sem);

	// 查找相同PID的CHANNEL
	//Find the channel existed.
	
	for (slotid = 0; slotid< MAX_FILTER_COUNT; slotid++)
	{
		if ((TRUE == cafilter[slotid].usedStatus)&&(filter->pid== cafilter[slotid].pid)
				&&(0!=cafilter[slotid].channelhandle))
		{
			channelhandle = cafilter[slotid].channelhandle;
			pid = cafilter[slotid].pid ;

			
		//	CAS_Dbg("APP ABV filter find slot 0x%x, id %d \n",pid,slotid);
			
			break;
		}
	}
	
	if (slotid >= MAX_FILTER_COUNT)
	{
		CA_FAIL("APP ABV set filter find slot Failed, pid:0x%x \n",filter->pid);
		
		for (slotid = 0; slotid< MAX_FILTER_COUNT; slotid++)
		{
			printf("APP ABV set filter failed [%d] pid = 0x%x \n",slotid,cafilter[slotid].pid);	
		}
		app_porting_ca_os_sem_signal(ca_demux_sem);
		
		return -1 ;
	}

	if ( 0 == cafilter[slotid].handle )
	{
		filterId = slotid ;

	//	CAS_Dbg("APP ABV filter handle is zero , 0x%x, id %d \n",pid,slotid);
	}
	else
	{
		for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
		{
			if ((FALSE == cafilter[filterId].usedStatus)&&(0 == cafilter[filterId].handle)
				&&(0==cafilter[filterId].channelhandle))
			{
				break;
			}
		}

		if (filterId >= MAX_FILTER_COUNT)
		{
			CA_FAIL("APP ABV set filter Failed,filterId=%d >= %d,pid:0x%x\n",filterId,MAX_FILTER_COUNT,filter->pid);
			for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
			{
			      printf("APP ABV set filter[%d] pid = 0x%x \n",filterId,cafilter[filterId].pid);
				
			}
			app_porting_ca_os_sem_signal(ca_demux_sem);
		
			return -1;
		}
		else
		{
		//	CAS_Dbg("APP ABV filter handle is already set , 0x%x, new id %d \n",pid,filterId);	
		}
	}

	memcpy(&cafilter[filterId],filter,sizeof(ca_filter_t));
			
	cafilter[filterId].usedStatus = TRUE ;
	cafilter[slotid].pid = pid ;
	cafilter[filterId].channelhandle = channelhandle;

    cafilter[filterId].handle = GxDemux_FilterAllocate(channelhandle);

	if (( 0 == cafilter[filterId].handle)||(-1 == cafilter[filterId].handle))
	{
		CA_ERROR(" Call CDSTBCA_SetPrivateDataFilter Error cafilter[%d].handle=0\n",filterId);
		
		app_porting_ca_os_sem_signal(ca_demux_sem);
		
		return -1;
	}

//  设置过滤条件

	ret = GxDemux_FilterSetup(cafilter[filterId].handle,
					cafilter[filterId].match,
					cafilter[filterId].mask,
					cafilter[filterId].equalFlag,
					cafilter[filterId].crcFlag,
					0,
					cafilter[filterId].filterLen);
	

	cafilter[filterId].byReqID = filterId;

	memcpy(filter,&cafilter[filterId],sizeof(ca_filter_t));

//	CAS_Dbg("APP ABV set filter pid 0x%x, id %d,tableid 0x%x\n",pid,filterId,cafilter[filterId].match[0]);	

	app_porting_ca_os_sem_signal(ca_demux_sem);

	
	return 0 ;
}

int app_porting_ca_demux_free_filter(unsigned int filterId)
{
	int32_t ret;
	handle_t channelhandle;
	uint16_t     pid; 
	int32_t  table_id ;

	if ( filterId >= MAX_FILTER_COUNT )
		return -1 ;
	
	app_porting_ca_os_sem_wait(ca_demux_sem);

	if (( 0 != cafilter[filterId].usedStatus)&&(0 != cafilter[filterId].handle)
		&&(0 != cafilter[filterId].channelhandle))
	{
			
	//	ret = GxDemux_FilterDisable(cafilter[filterId].handle);
	//	ASSERT_RET(ret);
	
		ret = GxDemux_FilterFree(cafilter[filterId].handle);
		//ASSERT_RET(ret);

		channelhandle = cafilter[filterId].channelhandle ;
		pid           = cafilter[filterId].pid ;
		table_id      = cafilter[filterId].match[0];
		
		memset(&cafilter[filterId],0,sizeof(ca_filter_t));

		cafilter[filterId].channelhandle = channelhandle ;
		cafilter[filterId].pid  = pid         ;
		cafilter[filterId].usedStatus = TRUE ;
		
		ret = 0 ;

	//	CAS_Dbg("APP ABV free filter, pid: 0x%x, filterId:%d,table id 0x%x \n",pid,filterId,table_id);
	}
	else
	{
		ret = -1 ;

		CAS_Dbg("APP ABV free filter, ERROR  filterId: %d \n",filterId);
	}
	
	app_porting_ca_os_sem_signal(ca_demux_sem);
	
	return ret ;	

}


int app_porting_ca_demux_alloc_channel(unsigned int pid,unsigned char repeat,
								unsigned int *p_id)
{
	int32_t ret;
	int32_t filterId = 0;
	handle_t channelhandle = 0;

	app_porting_ca_os_sem_wait(ca_demux_sem);
	
	for (filterId = 0; filterId < MAX_FILTER_COUNT; filterId++)
	{
		if ((TRUE == cafilter[filterId].usedStatus) && (pid == cafilter[filterId].pid)
			&&(0 !=cafilter[filterId].channelhandle))
		{
			channelhandle = cafilter[filterId].channelhandle;

			ret = GxDemux_ChannelSetPID(cafilter[filterId].channelhandle,pid,repeat);

			CAS_Dbg("APP ABV alloc channel already 0x%x, id %d \n",pid,filterId);

			app_porting_ca_os_sem_signal(ca_demux_sem);

			return 0 ;
		}
	}

	for (filterId = 0; filterId < MAX_FILTER_COUNT; filterId++)
	{
		if ((FALSE == cafilter[filterId].usedStatus) && (0 ==cafilter[filterId].channelhandle))
		{
			break;
		}
	}

	if (filterId >= MAX_FILTER_COUNT)
	{
		CA_FAIL("alloc channel Failed,filterId=%d >= %d,pid:0x%x\n",filterId,MAX_FILTER_COUNT,pid);
		for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
		{
			printf("alloc channel Failed,[%d] pid = 0x%x \n",filterId,cafilter[filterId].pid);
				
		}
		app_porting_ca_os_sem_signal(ca_demux_sem);
		
		return -1;
	}

	cafilter[filterId].channelhandle = GxDemux_ChannelAllocate(0, pid);
//	ASSERT_RET((cafilter[filterId].channelhandle));
	
	if (( 0 == cafilter[filterId].channelhandle)||(-1 == cafilter[filterId].channelhandle))
	{
		CAS_Dbg("APP ABV alloc channel handle faild 0x%x  \n",pid);
		
		app_porting_ca_os_sem_signal(ca_demux_sem);
		
		return -1;
	}

	cafilter[filterId].pid = pid ;
	cafilter[filterId].repeatFlag = repeat ;

	cafilter[filterId].usedStatus = TRUE;

	*p_id = filterId ;

//	CAS_Dbg("APP ABV alloc channel ok 0x%x ,id %d \n",pid,filterId);

	app_porting_ca_os_sem_signal(ca_demux_sem);

	return 0 ;
}

int app_porting_ca_demux_start_channel(unsigned int pid)
{
	int ret,i =0 ; ;
	int filterId ;
	int first = 1 ;
	
//	GxTime starttime={0};
//	uint32_t starts;

	
	app_porting_ca_os_sem_wait(ca_demux_sem);
	
	for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		if ((TRUE == cafilter[filterId].usedStatus)&&(pid== cafilter[filterId].pid)
				&&(0!=cafilter[filterId].channelhandle))
		{
			if ( first )
			{
				ret = GxDemux_ChannelEnable(cafilter[filterId].channelhandle);
				//ASSERT_RET(ret);

			//	CAS_Dbg("APP ABV start channel 0x%x ,id %d \n",pid,filterId);

				first = 0 ;
			}

		//	GxCore_GetTickTime(&starttime);
		//	starts = starttime.seconds;
		//	cafilter[filterId].nms= starts;

			ret = GxDemux_FilterEnable(cafilter[filterId].handle);
			//ASSERT_RET(ret);

		//	CAS_Dbg("APP ABV start filter 0x%x ,id %d,0x%x \n",pid,filterId,cafilter[filterId].handle);

			i++ ;
		}
	}

//	CAS_Dbg("APP ABV start channel 0x%x, do %d \n",pid,i);

	app_porting_ca_os_sem_signal(ca_demux_sem);

	return 0 ;
}

int app_porting_ca_demux_stop_channel(unsigned int pid)
{
	int ret = 0 ,i =0 ; 
	int filterId ;
	handle_t channelhandle=0;
	
	app_porting_ca_os_sem_wait(ca_demux_sem);

	
	for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		if ((TRUE == cafilter[filterId].usedStatus)&&(pid== cafilter[filterId].pid)
				&&(0!=cafilter[filterId].channelhandle))
		{
			channelhandle = cafilter[filterId].channelhandle ;
			
			if (0 != cafilter[filterId].handle)
			{
				ret = GxDemux_FilterDisable(cafilter[filterId].handle);
			}
			//ASSERT_RET(ret);


		//	CAS_Dbg("APP ABV stop filter 0x%x ,id %d, slot handle 0x%x \n",pid,filterId,channelhandle);

			i++ ;
		}
	}

	if (channelhandle)
	{
		ret = GxDemux_ChannelDisable(channelhandle);
		//ASSERT_RET(ret);

	//	CAS_Dbg("APP ABV stop channel handle 0x%x  \n",channelhandle);

		ret = 0 ;
	}
	else
	{
		ret = -1 ;
	}

//	CAS_Dbg("APP ABV stop channel 0x%x, do %d \n",pid,i);

	app_porting_ca_os_sem_signal(ca_demux_sem);

	return ret ;
}	

int app_porting_ca_demux_free_channel(unsigned int pid)
{
	int ret,i= 0 ;
	int filterId ;
	int first = 1 ;
	handle_t channelhandle = 0;
	
	app_porting_ca_os_sem_wait(ca_demux_sem);
	
	for (filterId = 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		if ((TRUE == cafilter[filterId].usedStatus)&&(pid== cafilter[filterId].pid)
				&&(0!=cafilter[filterId].channelhandle))
		{
			if (0 != cafilter[filterId].handle )
			{
				ret = GxDemux_FilterFree(cafilter[filterId].handle);
				//ASSERT_RET(ret);

			//	CAS_Dbg("APP ABV free channel ,but not free filter, pid: 0x%x, filterId: %d \n",pid,filterId);
			}
			
			if (first)
			{
				channelhandle = cafilter[filterId].channelhandle ;
				
				first = 0 ;	
			}

			memset(&cafilter[filterId],0,sizeof(ca_filter_t));

			i++ ;
		}
	}
printf("\n----->[%s]-channelh = %d.\n",__FUNCTION__,channelhandle);
	if ( channelhandle )
	{
		ret = GxDemux_ChannelFree(channelhandle);
		//ASSERT_RET(ret);

	//	CAS_Dbg("APP ABV free channel ,pid 0x%x ,handle 0x%x \n",pid,channelhandle);
	}

	app_porting_ca_os_sem_signal(ca_demux_sem);

	return 0 ;
}		



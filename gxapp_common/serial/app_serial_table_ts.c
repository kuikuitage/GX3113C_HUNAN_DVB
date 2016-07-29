#include <gxtype.h>
#include "service/gxsi.h"
#include "gxapp_sys_config.h"
#include "app_common_table_ota.h"
#include "app_common_flash.h"
#include "app_common_search.h"
#include "app_common_init.h"
#include "gxfrontend.h"
#include "gx_demux.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "app_serial_landian.h"
static handle_t ts_filter =0;
static handle_t ts_channel=0;
uint8_t ts_flag = 0;
uint16_t ts_pid = 0x1fff;
uint8_t ts_table_id = 0x0;
static char* ota_qam[]={"QAM_16","QAM_32","QAM_64","QAM_128","QAM_256"};
extern unsigned int htoi(const char *str);
extern unsigned int convert_version_str(const char *str);

GXSerialSection serialSection = {0,};

handle_t app_table_ts_get_filter_handle(void)
{ 
	return ts_filter;
}

handle_t app_table_ts_get_channel_handle(void)
{ 
	return ts_channel;
}


int app_table_ts_monitor_filter_close(void)
{ 
	int32_t ret = 0;
	app_porting_psi_demux_lock();

	if (0 != ts_channel)
		{ 
			printf("!!!!!!!!!!app_table_ts_monitor_filter_close!!!!!!!!!!!!!!!!!\n");

			if(ts_filter != 0)
			{ 
//				ret = GxDemux_FilterDisable(ota_filter);
//				CHECK_RET(DEMUX,ret);
				ret = GxDemux_FilterFree(ts_filter);
				CHECK_RET(DEMUX,ret);				
				ts_filter = 0;
			}

			ret = GxDemux_ChannelFree(ts_channel);
			CHECK_RET(DEMUX,ret);
			ts_channel = 0;
		}
	app_porting_psi_demux_unlock();

	return 0;
}


 void app_table_ts_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{ 
	
    int16_t            pid;
    uint16_t            section_length;
    uint8_t*            data = (uint8_t*)Section;
    uint8_t version = 0;
    uint8_t match[8] = {0,};
    uint8_t mask[8] = {0,};
    int32_t ret;
	if (NULL == Section)
		return;

//    ASSERT(Section != NULL);
	app_porting_psi_demux_lock();

    ret = GxDemux_FilterGetPID(Filter,&pid);
    if (ts_pid != pid)
    	{ 
             printf("app_table_ts_filter_notify pid error  pid:0x%x\n", pid);
			 app_porting_psi_demux_unlock();
    		return ;
    	}
	app_porting_psi_demux_unlock();
	serialSection.pid = pid;
    printf("app_table_ts_filter_notify total size:%d\n", Size);
 
	/*应用分析NIT表，先调用应用分析NIT表
	   之后永新CA库中设置NIT过滤分析*/

	section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
	version = data[5] & 0x3E;
	app_table_ts_monitor_section_parse(data, section_length);
	{ 
			app_porting_psi_demux_lock();
			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterSetup(Filter, match, mask, TRUE, false,0, 0);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_ChannelEnable(ts_channel);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterEnable(Filter);
			CHECK_RET(DEMUX,ret);
			app_porting_psi_demux_unlock();

	}
			
        return;
		
}

int app_table_ts_monitor_filter_open(uint16_t pid)
{ 
	handle_t filter;
	uint8_t match[8] = {0,};
	uint8_t mask[8] = {0,};
	int32_t ret = 0;

	CAS_Dbg("app_table_ts_monitor_filter_open\n");
	app_porting_psi_demux_lock();
	
	if (0x1fff == pid)
	{ 
		app_porting_psi_demux_unlock();
		return 0;
	}
	if (0 != ts_filter)
	{ 
		//				ret = GxDemux_FilterDisable(ota_filter);
		//				CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(ts_filter);
		CHECK_RET(DEMUX,ret);
		ts_filter = 0;
	}
	if (0 != ts_channel)
	{
		ret = GxDemux_ChannelFree(ts_channel);
		CHECK_RET(DEMUX,ret);
		ts_channel = 0;
	} 


	printf("!!!!!!!!!!app_table_ts_monitor_filter_open!!!!!!!!!!!!!!!!!\n");
	ts_channel = GxDemux_ChannelAllocate(0, pid);
	if ((0 == ts_channel)||(-1 == ts_channel))
	{
		CA_ERROR("  ts_channel=0x%x\n",ts_channel);
		ts_channel = 0;
		app_porting_psi_demux_unlock();

		return 0;
	} 


	GxDemux_ChannelSetPID(ts_channel,pid,FALSE);
	filter = GxDemux_FilterAllocate(ts_channel);
	if (( 0 == filter)||(-1 == filter))
	{ 
		ret = GxDemux_ChannelFree(ts_channel);
		CHECK_RET(DEMUX,ret);
		ts_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();

		return 0;
	}

	ts_filter = filter;
	ret = GxDemux_FilterSetup(filter, match, mask, TRUE, FALSE,0, 0);
	CHECK_RET(DEMUX,ret);
	//	GxCA_DemuxFilterRigsterNotify(filter, app_table_nit_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(ts_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	ts_pid = pid;
	app_porting_psi_demux_unlock();
	return 0;
}

int app_table_ts_monitor_filter_restart(void)
{	 
	handle_t filter;
	uint8_t match[8] = {0,};
	uint8_t mask[8] = {0,};

	int32_t ret = 0;
	app_porting_psi_demux_lock();

	if (0x1fff == ts_pid)
	{ 
		/*
		 * ota pid invalid 
		 */
		app_porting_psi_demux_unlock();
		return 0;
	}


	CAS_Dbg("app_table_ota_monitor_filter_restart\n");


	if (0 != ts_filter)
	{ 
		//				ret = GxDemux_FilterDisable(ota_filter);
		//				CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(ts_filter);
		CHECK_RET(DEMUX,ret);
		ts_filter = 0;
	}
	if (0 != ts_channel)
	{
		ret = GxDemux_ChannelFree(ts_channel);
		CHECK_RET(DEMUX,ret);
		ts_channel = 0;
	} 

	ts_channel = GxDemux_ChannelAllocate(0, ts_pid);
	if ((0 == ts_channel)||(-1 == ts_channel))
	{ 
		CA_ERROR(" ts_channel=0x%x\n",ts_channel);
		ts_channel = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}
	filter = GxDemux_FilterAllocate(ts_channel);
	if (( 0 == filter)||(-1 == filter))
	{ 
		ret = GxDemux_ChannelFree(ts_channel);
		CHECK_RET(DEMUX,ret);
		ts_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}


	ret = GxDemux_FilterSetup(filter, match, mask, TRUE, FALSE,0, 0);
	CHECK_RET(DEMUX,ret);
	//	GxCA_DemuxFilterRigsterNotify(filter, app_table_cat_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(ts_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	ts_filter = filter;
	app_porting_psi_demux_unlock();

	return 0;
}
//cth
//
void app_table_ts_monitor_section_parse(uint8_t* pSectionData,size_t Size)
{ 

	uint16_t wSectionLength = 0;
	uint16_t wSectionNumber = 0;
	uint8_t  chFileHeadLength = 0;
	uint8_t  chTableId = 0;
	uint16_t wLastSectionNumber = 0;
	uint8_t *pData = pSectionData;
	char oemValue[20]={0};
	GxFrontend frontendpara[FRONTEND_MAX];
	char*          softwareVer = NULL;
	char*          hardwareVer = NULL;
	uint32_t       m_Frequency=0;
	uint32_t       m_PlatformID = 0;
	uint8_t        m_Modulation=0;
	uint32_t       m_SymbolRate=0;
	char*          PlatformIDStr = NULL;


	if (NULL == pSectionData)
		return;

	printf("app_table_ts_monitor_section_parse=%d\n",Size);

	chTableId	   = pData[0];
	wSectionLength = (((uint16_t)pData[1] << 8) | pData[2]) & 0x0fff;
	wSectionNumber = ((uint16_t)pData[6] << 8 )| pData[7];
	wLastSectionNumber = ((uint16_t)pData[8] << 8 )| pData[9];
	memset(serialSection.buffer,0,1024*sizeof(uint8_t));
	if( wSectionLength+3 > 1024 )
	{
		wSectionLength = 1024-3;
	}
	memcpy(serialSection.buffer,pData,wSectionLength+3);
	serialSection.length =  wSectionLength+3;
//	SendDataToPC(SEND_TS_ID,pData,wSectionLength+3);
	return ;
}



/*
 * =====================================================================================
 *
 *       Filename:  autotest_tsfilter.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2014 04:55:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Z.Z.R 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#include "../include/common_autotest.h"

static handle_t	sg_TESTThreadHandle = -1;
static int sg_StartFlag = 0;


GxDemuxProperty_Slot sg_MuxSlot = {0};
GxDemuxProperty_Slot sg_Slot = {0};
GxDemuxProperty_Filter sg_Filter = {0}; 

static TsFilterPara_t sg_TsPara;
static unsigned int sg_ReceivePackageCount = 0;
static unsigned int sg_ErrorPackageCount = 0;


int _parameter_init(TsFilterPara_t *pTsFilterPara)
{
//#define TEST_TS_SOURCE		1
//#define TEST_DEMUX_ID		1
//#define DATA_PID			0x0// TS PID
//#define TS_PACKET_LEN		188

	if(pTsFilterPara == NULL)
		return -1;
	memcpy(&sg_TsPara, pTsFilterPara, sizeof(TsFilterPara_t));
	sg_TsPara.DemuxPara.DemuxDev = -1;
	sg_TsPara.DemuxPara.DemuxModule = -1;
    sg_ReceivePackageCount = 0;
    sg_ErrorPackageCount = 0;
	return 0;
}

static int _set_demux(void)
{
	GxDemuxProperty_ConfigDemux demux;
	int32_t ret;
	
	if(0 >= sg_TsPara.DemuxPara.DemuxDev)
	{
	 	sg_TsPara.DemuxPara.DemuxDev = GxAvdev_CreateDevice(0);
		if(sg_TsPara.DemuxPara.DemuxDev <= 0)
		{
			log_printf("\nTEST, create device failed, %s,%d\n",__FILE__,__LINE__);
			return -1;
		}
		if(sg_TsPara.DemuxPara.DemuxModule <= 0)
		{
	 		sg_TsPara.DemuxPara.DemuxModule = GxAvdev_OpenModule(sg_TsPara.DemuxPara.DemuxDev, GXAV_MOD_DEMUX, sg_TsPara.DemuxPara.DemuxId);
			if(sg_TsPara.DemuxPara.DemuxModule <= 0)
			{
				GxAvdev_DestroyDevice(sg_TsPara.DemuxPara.DemuxDev);
				sg_TsPara.DemuxPara.DemuxDev = -1;
				log_printf("\nTEST, open module failed, %s,%d\n",__FILE__,__LINE__);
				return -1;
			}
		}
	}

    // demux config
    demux.source = sg_TsPara.DemuxPara.TsSource;
    demux.ts_select = FRONTEND;
    demux.stream_mode = sg_TsPara.DemuxPara.TsMode;
    demux.time_gate = 0xf;
    demux.byt_cnt_err_gate = 0x03;
    demux.sync_loss_gate = 0x03;
    demux.sync_lock_gate = 0x03;
    ret = GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_Config, &demux, \
            sizeof(GxDemuxProperty_ConfigDemux));
	if(ret < 0)
	{
		log_printf("\nTEST, config demux error, %x\n", ret);
		return -1;
	}
	return 0;
}

int _setup_slot(void)
{
#define TS_DEMUX_IN_SIZE  (sg_TsPara.TsPackageLen*1024)
#define TS_DEMUX_IN_GATE  (sg_TsPara.TsPackageLen*30)
    int ret = 0;
    
	memset(&sg_MuxSlot, 0, sizeof(GxDemuxProperty_Slot));
	memset(&sg_Slot, 0, sizeof(GxDemuxProperty_Slot));
	memset(&sg_Filter, 0, sizeof(GxDemuxProperty_Filter));
    
    // sepical slot
    sg_MuxSlot.type = DEMUX_SLOT_MUXTS;
    sg_MuxSlot.flags = (DMX_TSOUT_EN | DMX_CRC_DISABLE|DMX_REPEAT_MODE);
    ret = GxAVGetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotAlloc,
					&sg_MuxSlot, sizeof(GxDemuxProperty_Slot));
	if (ret < 0) 
	{
		return -1;
	}
    // special filter
    sg_Filter.slot_id = sg_MuxSlot.slot_id;
    sg_Filter.filter_fifo_size = TS_DEMUX_IN_SIZE;
    sg_Filter.filter_buffer_al_gate = TS_DEMUX_IN_GATE;
    ret = GxAVGetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_FilterAlloc,
					&sg_Filter, sizeof(GxDemuxProperty_Filter));
	if (ret < 0) 
	{
		return -1;
	}
    // normal slot
    sg_Slot.type = DEMUX_SLOT_TS;
	sg_Slot.flags = (DMX_REPEAT_MODE | DMX_TSOUT_EN | DMX_CRC_DISABLE);
	sg_Slot.ts_out_pin = sg_MuxSlot.slot_id;
	sg_Slot.pid = sg_TsPara.TsPID;

	ret = GxAVGetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotAlloc,
					&sg_Slot, sizeof(GxDemuxProperty_Slot));
	if (ret < 0) 
	{
		return -1;
	}

	ret = GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotConfig,
					&sg_Slot, sizeof(GxDemuxProperty_Slot));
	if (ret < 0)
	{
		return -1;
	}
    
    ret = GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev,sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotEnable,
					&sg_Slot, sizeof(GxDemuxProperty_Slot));
	if (ret < 0) 
	{
		return -1;
	}

	sg_StartFlag = 1;
    return 0;
}

static int _ts_package_read(char *pBuffer, unsigned MaxBufferLen)
{
    int ret = 0;
    unsigned int EventRet = 0;
    if(pBuffer == NULL)
    {
        return -1;
    }
    GxDemuxProperty_FilterRead DmxFilterRead; 
    //GxDemuxProperty_FilterFifoQuery Status = {0};
    // query it is have data or not
    ret = GxAVWaitEvents(sg_TsPara.DemuxPara.DemuxDev,sg_TsPara.DemuxPara.DemuxModule,EVENT_DEMUX0_FILTRATE_TS_END, 1000000, &EventRet);
    // read
    DmxFilterRead.filter_id = sg_Filter.filter_id;
    DmxFilterRead.buffer    = pBuffer;
    DmxFilterRead.max_size  = MaxBufferLen;
    ret = GxAVGetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_FilterRead,
                         (void*)&DmxFilterRead, sizeof(GxDemuxProperty_FilterRead));
    if(ret < 0)
    {
		log_printf("\nTEST, read, failed \n");
        return -1;
    }
    if(DmxFilterRead.read_size < 0)
        DmxFilterRead.read_size = 0;

    return DmxFilterRead.read_size; 
}


#define __TEST_TEST__
int g_TestDebugFlag = 0;
int _deal_with_one_packet(char *pData)
{
	int i = 0;
	if(NULL == pData)
	{
		log_printf("\nTEST, parameter error\n");
		return -1;
	}
	if((pData[0] != 0x47)
		|| (((pData[1]&0x1f)<<8)|(pData[2])) != sg_TsPara.TsPID)
	{
		log_printf("\nTEST, wrong data, pid = 0x%x\n", sg_TsPara.TsPID);
	    sg_ReceivePackageCount++;
		sg_ErrorPackageCount++;
#if 1
//#ifdef __TEST_TEST__
if(g_TestDebugFlag)
{
                char *p = NULL;
                int i = 0;
				log_printf("\n************************************************\n");
				p = pData;
				for(i = 0; i < sg_TsPara.TsPackageLen; i++)
				{
					log_printf("0x%02x ",p[i]);
					if(((i + 1)%16) == 0)
						log_printf("\n");
				}
				log_printf("\n************************************************\n");
}
#endif

		return -1;
	}
	 
	// TODO: do yourself
if((pData[1]&0x40) == 0)
{// not start preload data
	for(i=12; i < sg_TsPara.TsPackageLen; i++)
	{
		if(pData[i] != (i - 4))
			break;
	}
	
	if(i != sg_TsPara.TsPackageLen)
	{
			sg_ErrorPackageCount++;
#if 1
	//#ifdef __TEST_TEST__
	if(g_TestDebugFlag)
	{
	                char *p = NULL;
	                int i = 0;
					log_printf("\n************************************************\n");
					p = pData;
					for(i = 0; i < sg_TsPara.TsPackageLen; i++)
					{
						log_printf("0x%02x ",p[i]);
						if(((i + 1)%16) == 0)
							log_printf("\n");
					}
					log_printf("\n************************************************\n");
	}
#endif

	}
	sg_ReceivePackageCount++;
}
else
{
	if(g_TestDebugFlag)
	{
	                char *p = NULL;
	                int i = 0;
					log_printf("\n**************start preload*********************\n");
					p = pData;
					for(i = 0; i < sg_TsPara.TsPackageLen; i++)
					{
						log_printf("0x%02x ",p[i]);
						if(((i + 1)%16) == 0)
							log_printf("\n");
					}
					log_printf("\n************************************************\n");
	}
    
	sg_ReceivePackageCount++;
}
	
	return 0;
}


static void _test_process(void *arg)
{
#define READ_DATA_SIZE		sg_TsPara.TsPackageLen*100
	int ret = -1;
	int i = 0;
	char* pDataBuffer = calloc(1,READ_DATA_SIZE);
	char* p = NULL;
	if(NULL == pDataBuffer)
	{
		log_printf("\nTEST, calloc failed, %s,%d\n",__FILE__,__LINE__);
		while(1);
	}
	while(1)
	{
		if(sg_StartFlag > 0)
		{
			memset(pDataBuffer,0,READ_DATA_SIZE);
            ret =  _ts_package_read(pDataBuffer,READ_DATA_SIZE);
			if((ret >= sg_TsPara.TsPackageLen) && (sg_StartFlag == 1))
			{// get the ts data
				//log_printf("\nTEST, read data\n");
#if 0
//#ifdef __TEST_TEST__
if(g_TestDebugFlag)
{
				log_printf("\n************************************************\n");
				p = pDataBuffer;
				for(i = 0; i < sg_TsPara.TsPackageLen; i++)
				{
					log_printf("0x%02x ",p[i]);
					if(((i + 1)%16) == 0)
						log_printf("\n");
				}
				log_printf("\n************************************************\n");
}
#endif
// do TS packet
				p = pDataBuffer;
				for(i = 0; i < (ret/sg_TsPara.TsPackageLen); i++)// check the data is right or not
				{
					_deal_with_one_packet(p);
					p += sg_TsPara.TsPackageLen;
				}
			}
			else if(ret < 0)
			{
				log_printf("\nTEST, read data error\n");
			}
			
		}
		else
		{
	       // if(pDataBuffer)
           //     free(pDataBuffer);
            if(sg_StartFlag == 0)
        	{
				log_printf("\nTEST, EXIT THE TS THREAD\n");
				sg_StartFlag = -1;
        	}
		    GxCore_ThreadDelay(100);
		}
		//GxCore_ThreadDelay(60);

	}
}

static int _ts_start(void)
{
	int ret = 0;
	sg_Slot.slot_id = -1;
	
	ret = _set_demux();
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	ret = _setup_slot();
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(sg_TESTThreadHandle <= 0)
		GxCore_ThreadCreate("test_console", &sg_TESTThreadHandle, (void*)_test_process, NULL,\
							100*1024,GXOS_DEFAULT_PRIORITY-3);
    return 0;
}

int test_ts_destroy(void)
{
	if((sg_TsPara.DemuxPara.DemuxModule> 0) && (sg_TsPara.DemuxPara.DemuxDev > 0))
	{
		sg_StartFlag = 0;
		
		if(sg_Slot.slot_id > 0)
		{
            // disable slot
            GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotDisable,
								(void*)&sg_Slot, sizeof(GxDemuxProperty_Slot));
            // special filter
        	GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_FilterFree,
								(void*)&sg_Filter, sizeof(GxDemuxProperty_Filter));
           
            GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotFree,
								(void*)&sg_MuxSlot, sizeof(GxDemuxProperty_Slot));
			GxAVSetProperty(sg_TsPara.DemuxPara.DemuxDev, sg_TsPara.DemuxPara.DemuxModule, GxDemuxPropertyID_SlotFree,
								(void*)&sg_Slot, sizeof(GxDemuxProperty_Slot));

           	memset(&sg_MuxSlot, 0, sizeof(GxDemuxProperty_Slot));
	        memset(&sg_Slot, 0, sizeof(GxDemuxProperty_Slot));
	        memset(&sg_Filter, 0, sizeof(GxDemuxProperty_Filter));
			sg_Slot.slot_id = -1;
			sg_MuxSlot.slot_id = -1;
		}
		//wait for thread exit
		while(sg_StartFlag != -1)GxCore_ThreadDelay(10);
			
		GxAvdev_CloseModule(sg_TsPara.DemuxPara.DemuxDev,sg_TsPara.DemuxPara.DemuxModule);
		GxAvdev_DestroyDevice(sg_TsPara.DemuxPara.DemuxDev);
		sg_TsPara.DemuxPara.DemuxDev = -1;
		sg_TsPara.DemuxPara.DemuxModule = -1;
		
	}
	return 0;
}

int start_ts_compare(void *pTsFilterPara)
{
	int ret = 0;

	if(pTsFilterPara == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	if(sg_StartFlag > 0)
	{
		log_printf("\nTEST, stop first\n");
		stop_ts_compare();
	}
	
	ret = _parameter_init((TsFilterPara_t*)pTsFilterPara);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	ret = _ts_start();
	return ret;
}
int stop_ts_compare(void)
{
	int ret = 0;
	ret = test_ts_destroy();
	memset(&sg_TsPara, 0, sizeof(TsFilterPara_t));
	return ret;
}

int get_ts_compare_result(int *ReceivePackageCount, int *ErrorPackageCount)
{
	if(sg_StartFlag > 0)
	{
		*ErrorPackageCount = sg_ErrorPackageCount;
		*ReceivePackageCount = sg_ReceivePackageCount;
	}
	else
	{
		*ErrorPackageCount = 0;
		*ReceivePackageCount = 0;
		return -1;
	}
    return 0;
}



/*
 * =====================================================================================
 *
 *       Filename:  autotest_frontend.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2014 05:04:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Z.Z.R 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#include "../include/common_autotest.h"
#include "../include/module_command.h"

//frontend module
//#define USER_SELF_MODE

static FrontendInfo_t sg_FrontendInfo;

static int __fronendend_open(int id) 
{
#ifdef USER_SELF_MODE
	int fd = -1, i;
	char devname[128] = {0};

	if(id >= FRONTEND_MAX)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	for (i = 0; i < 4; i++) {
		sprintf(devname, "/dev/dvb/adapter%d/frontend%d", i, id);
		fd = open(devname, O_RDWR);
		if (fd > 0)
			goto out;

		sprintf(devname, "/dev/dvb%d.frontend%d", i, id);
		fd = open(devname, O_RDWR|O_NONBLOCK);
		if (fd > 0)
			goto out;
		break;
	}
out:
	return fd;
#else
	return GxFrontend_IdToHandle(id);
#endif
}

static int _frontend_init(int *pFrontendFd, int tuner)
{
	if(pFrontendFd == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(*pFrontendFd < 0)
		*pFrontendFd = __fronendend_open(tuner);
	//log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
	return *pFrontendFd;
}

int _frontend_destroy(int *pFrontendFd,int tuner)
{
	int ret = 0;
	if((pFrontendFd == NULL) || (tuner >= FRONTEND_MAX))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(*pFrontendFd > 0)
	{
#ifdef USER_SELF_MODE
		ret = close(*pFrontendFd);
#else
		GxFrontend_CleanRecord(tuner);
#endif
		*pFrontendFd = -1;
	}
	return ret;
}

//
int frontend_voltage(int *pFrontendFd, unsigned int voltage)
{
	int ret = 0;
	if((voltage > SEC_VOLTAGE_OFF) || (pFrontendFd == NULL) || (*pFrontendFd <= 0))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	ret = ioctl (*pFrontendFd, FE_SET_VOLTAGE, voltage);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
	}
	return ret;
}

static int _check_tp_lock(int FrontendFd)
{
	fe_status_t festatus;
	if(ioctl(FrontendFd, FE_READ_STATUS, &festatus) >= 0)
	{
		if(festatus & FE_HAS_LOCK)
		{
			return 1;// TP LOCK
		}
		else
		{
			return 0;// TP UNLOCK
		}
	}
	else
	{
		return -1;
	}
}

// demux
static int _demux_config(handle_t *pTESTDev, handle_t *pTESTModule, DemuxConfigPara_t DemuxPara)
{
	GxDemuxProperty_ConfigDemux demux;

	int32_t ret;

	if((pTESTDev == NULL) || (pTESTModule == NULL))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
	if(-1 == *pTESTDev)
	{
	 	*pTESTDev = GxAvdev_CreateDevice(0);
		if(*pTESTDev <= 0)
		{
			log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
			return -1;
		}
		if(*pTESTModule < 0)
		{
	 		*pTESTModule = GxAvdev_OpenModule(*pTESTDev, GXAV_MOD_DEMUX, DemuxPara.DemuxId);
			if(*pTESTModule <= 0)
			{
				GxAvdev_DestroyDevice(*pTESTDev);
				*pTESTDev = -1;
				log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
				return -1;
			}
		}
	}

    // demux config
    demux.source = DemuxPara.TsSource;
    demux.ts_select = FRONTEND;
    demux.stream_mode = DemuxPara.TsMode;
    demux.time_gate = 0xf;
    demux.byt_cnt_err_gate = 0x03;
    demux.sync_loss_gate = 0x03;
    demux.sync_lock_gate = 0x03;
    ret = GxAVSetProperty(*pTESTDev, *pTESTModule, GxDemuxPropertyID_Config, &demux, \
            sizeof(GxDemuxProperty_ConfigDemux));
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	return 0;
}

static int _check_ts_lock(int DemuxDev,int DemuxModul)
{
	GxDemuxProperty_TSLockQuery ts_lock_status = {TS_SYNC_UNLOCKED};
	int32_t ret = -1;

	ret = GxAVGetProperty(DemuxDev,
			DemuxModul,
			GxDemuxPropertyID_TSLockQuery,
			&ts_lock_status,
			sizeof(GxDemuxProperty_TSLockQuery));
	if(ret < 0)
		return -1;

	return (ts_lock_status.ts_lock==TS_SYNC_LOCKED?1:0);
}

static int _demux_destroy(handle_t *pDemuxDev,handle_t *pDemuxModul)
{
	if((pDemuxDev == NULL) || (pDemuxModul == NULL))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if((*pDemuxModul > 0) && (*pDemuxDev > 0))
	{
		GxAvdev_CloseModule(*pDemuxDev,*pDemuxModul);
		GxAvdev_DestroyDevice(*pDemuxDev);
	}
	return 0;
}

static int _lock_check(int FrontendFd,int DemuxDev,int DemuxModul)
{
	if((_check_ts_lock(DemuxDev,DemuxModul) == 1)
		&& (_check_tp_lock(FrontendFd)==1))
	{
		// LOCKED
		log_printf("\nTEST, LOCKED\n");
		return 1;
	}
	else
	{
		return 0;
	}
}

static int _init_demod_demux(FrontendInfo_t* pTpInfo)
{
	int ret = 0;
	if(pTpInfo == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if((pTpInfo->TunerSelect >= TUNER_NUM_TOTAL) 
		|| (pTpInfo->DemuxPara.DemuxId >= DEMUX_ID_TOTAL))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	pTpInfo->FrontendDev = -1;
	ret = _frontend_init(&pTpInfo->FrontendDev,pTpInfo->TunerSelect);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	pTpInfo->DemuxPara.DemuxDev = -1;
	pTpInfo->DemuxPara.DemuxModule = -1;
	ret = _demux_config(&pTpInfo->DemuxPara.DemuxDev,&pTpInfo->DemuxPara.DemuxModule,pTpInfo->DemuxPara);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		_frontend_destroy(&pTpInfo->FrontendDev,pTpInfo->TunerSelect);
		return -1;
	}
	return 0;
}

//output
static int _lock_status_get(int *pLockStatus)
{
	if((pLockStatus == NULL)
		|| (sg_FrontendInfo.FrontendDev <= 0)
		|| (sg_FrontendInfo.DemuxPara.DemuxDev <= 0)
		|| (sg_FrontendInfo.DemuxPara.DemuxModule <= 0))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	*pLockStatus = _lock_check(sg_FrontendInfo.FrontendDev, sg_FrontendInfo.DemuxPara.DemuxDev, sg_FrontendInfo.DemuxPara.DemuxModule);//((sg_LockStatus == 0)?0:1);
	return 0;
}


int destroy_frontend(void)
{
	int ret = 0;
#if 0
	ret = frontend_voltage(&sg_FrontendInfo.FrontendDev, SEC_VOLTAGE_OFF);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
#endif
	ret = _frontend_destroy(&sg_FrontendInfo.FrontendDev, sg_FrontendInfo.TunerSelect);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	ret = _demux_destroy(&sg_FrontendInfo.DemuxPara.DemuxDev,&sg_FrontendInfo.DemuxPara.DemuxModule);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	
	memset(&sg_FrontendInfo, 0, sizeof(FrontendInfo_t));
	// TODO: reply, success
	return 0;
err:
	// TODO: reply failed
	 return -1;
}

int init_frontend(void* pFrontendInfo)
{
	int ret = 0;

	if(pFrontendInfo == NULL)
    {
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}	
	if(sg_FrontendInfo.FrontendDev > 0)
	{
		ret = destroy_frontend();
		if(ret < 0)
		{
			log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
			goto err;
		}
	}
	memcpy(&sg_FrontendInfo, (FrontendInfo_t*)pFrontendInfo, sizeof(FrontendInfo_t));
	
	ret = _init_demod_demux(&sg_FrontendInfo);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}	
	memcpy(pFrontendInfo, &sg_FrontendInfo, sizeof(FrontendInfo_t));
	// TODO: reply success
	return 0;
err:
	// TODO: reply failed
	return -1;
}

int get_lock_status(void)
{
	int Status = 0;
	int ret = 0;
	ret = _lock_status_get(&Status);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	log_printf("\nTEST, Lock status, %d\n",Status);
	// TODO: reply success
	return ((Status > 0)?1:0);
err:
	// TODO: reply failed
	return -1;
}


// init program parameters
int init_play(void* ProgInfo)
{
	ProgInfo_t *pProgInfo = NULL;
	
	if(ProgInfo == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	pProgInfo = (ProgInfo_t*)ProgInfo;
	
	if(pProgInfo->DemuxId == 0xff)
		pProgInfo->DemuxId = sg_FrontendInfo.DemuxPara.DemuxId;
	if(pProgInfo->TsMode == 0xff)
		pProgInfo->TsMode = sg_FrontendInfo.DemuxPara.TsMode;
	if(pProgInfo->TsSource == 0xff)
		pProgInfo->TsSource = sg_FrontendInfo.DemuxPara.TsSource;
	if(pProgInfo->TunerSelect == 0xff)
		pProgInfo->TunerSelect = sg_FrontendInfo.TunerSelect;

	// TODO: reply success
	return 0;
err:
	// TODO: reply failed
	return -1;

}

int stop_play(void)
{
	GxPlayer_MediaStop(PLAYER_NAME);
	// TODO: reply success
	return 0;
}

int stop_all(void)
{
	int ret = 0;
	log_printf("\nTEST, test, %s, %d\n",__FUNCTION__,__LINE__);
	// stop ts filter
	ret = stop_ts_compare();
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	// stop tp
	ret = _frontend_destroy(&sg_FrontendInfo.FrontendDev, sg_FrontendInfo.TunerSelect);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	ret = _demux_destroy(&sg_FrontendInfo.DemuxPara.DemuxDev,&sg_FrontendInfo.DemuxPara.DemuxModule);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		goto err;
	}
	memset(&sg_FrontendInfo, 0, sizeof(FrontendInfo_t));

	// stop play
	GxPlayer_MediaStop("player1");
	// TODO: reply success	
	return 0;
err:
	// TODO: reply failed
	return -1;	
}

int compare_ts_data(void *TsPara)
{
    int ret = 0;
    TsFilterPara_t *pFilterPare = NULL;

    if(TsPara == NULL)
    {
        log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
        goto err;
    }
    pFilterPare = (TsFilterPara_t*)TsPara;

    if(pFilterPare->DemuxPara.DemuxId == 0xff)
        pFilterPare->DemuxPara.DemuxId = sg_FrontendInfo.DemuxPara.DemuxId;
    if(pFilterPare->DemuxPara.TsMode == 0xff)
        pFilterPare->DemuxPara.TsMode = sg_FrontendInfo.DemuxPara.TsMode;
    if(pFilterPare->DemuxPara.TsSource == 0xff)
        pFilterPare->DemuxPara.TsSource = sg_FrontendInfo.DemuxPara.TsSource;
    //if(pFilterPare->TsPackageLen == 0xff)
    pFilterPare->TsPackageLen = 188;
    ret = start_ts_compare((void*)pFilterPare);
    if(ret < 0)
    {
        log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
        goto err;
    }
    // TODO: reply success
    return 0;
err:
    // TODO: reply failed
    return -1;
}

void TestTSPrint(void)
{
    extern int g_TestDebugFlag;
    if(g_TestDebugFlag == 1)
        g_TestDebugFlag = 0;
    else
        g_TestDebugFlag = 1;
	log_printf("\nTEST, test, %s, %d, Flag = %d\n",__FUNCTION__,__LINE__,g_TestDebugFlag);
}	



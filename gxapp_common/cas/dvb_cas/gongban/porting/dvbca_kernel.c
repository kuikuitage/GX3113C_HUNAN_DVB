#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_card.h"
#include "dvbca_msg.h"
#include "dvbca_kernel.h"
#include "dvbca_db.h"
#include "dvbca_urgent.h"
#include "gxcore.h"
#ifdef ENABLE_INTERATOR_COMMON
#define	ENABLE_OPERATOR_NUM		1
static DVBCA_UINT32 g_auiEnableOperator[ENABLE_OPERATOR_NUM] = 
{
	0xFFFFFFFF,
};
#endif

static ST_CAKERL *g_pstCaKernel = NULL;
static ST_EMM *g_pstEmm = NULL;
static ST_ECM *g_astEcm = NULL;
static DVBCA_UINT8 g_ucEcmNum = 0;
static const char g_acDVBCasVer[] = "DVBCA1.0";
static DVBCA_UINT8 g_ucDisableCa = 0;
static ST_CA_MSG g_astSubCache[2];
static DVBCA_UINT32 g_uiSubLastVer[2];

static void DVBCA_ParseEcm(ST_ECM *pstEcm, DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen);
static void DVBCA_ParseEmm(DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen);
static void DVBCA_ParseEmmEmm(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);
static void DVBCA_ParseEmmEvent(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);
static void DVBCA_ParseEmmCmd(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);
static void DVBCA_ParseNetPara(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);
static void DVBCA_ParseAdvertize(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);

int DVBCA_InitKernel(DVBCA_UINT8 *pucStbId)
{
	int i = 0;
	//init ca kernel para
	g_pstCaKernel = (ST_CAKERL *)DVBSTBCA_Malloc(sizeof(ST_CAKERL));
	if(g_pstCaKernel == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_Memset(g_pstCaKernel, 0x0,  sizeof(ST_CAKERL));
	
	g_pstCaKernel->usCasID = DVBCA_CAS_ID;
	g_pstCaKernel->usNetworkID = 0xFFFF;
	DVBSTBCA_Memcpy(g_pstCaKernel->aucStbID, pucStbId, 4 * sizeof(DVBCA_UINT8));
	
	//init ecm para
	g_ucEcmNum = 0;
	g_astEcm = (ST_ECM *)DVBSTBCA_Malloc(DVBCA_MAX_ECM_NUM * sizeof(ST_ECM));
	if(g_astEcm == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_Memset(g_astEcm, 0x0, DVBCA_MAX_ECM_NUM * sizeof(ST_ECM));
	
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
	{
		g_astEcm[i].usPid = 0x1FFF;

		g_astEcm[i].pEcmSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
		if(g_astEcm[i].pEcmSemaphore == NULL)
			return DVBCA_FAILED;
		DVBSTBCA_SemaphoreInit(g_astEcm[i].pEcmSemaphore, 0);
		DVBSTBCA_SemaphoreSignal(g_astEcm[i].pEcmSemaphore);
	}

	//init emm para
	g_pstEmm = (ST_EMM *)DVBSTBCA_Malloc(sizeof(ST_EMM));
	if(g_pstEmm == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_Memset(g_pstEmm, 0x0, sizeof(ST_EMM));
	
	g_pstEmm->usPid = 0x1FFF;
	g_pstEmm->pEmmSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_pstEmm->pEmmSemaphore == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreInit(g_pstEmm->pEmmSemaphore, 0);
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
	
#ifndef SUPPORT_CAM
	g_uiSubLastVer[0] = 0xFFFFFFFF;
	g_uiSubLastVer[1] = 0xFFFFFFFF;
#endif
	return DVBCA_SUCESS;
}

void DVBCA_Kernel_UnInit(void)
{
	int i = 0;
	
	if(g_pstEmm)
	{
		if(g_pstEmm->pEmmSemaphore)
		{
			DVBSTBCA_SemaphoreDestory(g_pstEmm->pEmmSemaphore);
			DVBSTBCA_Free(g_pstEmm->pEmmSemaphore);
			g_pstEmm->pEmmSemaphore = NULL;		
		}
		DVBSTBCA_Free(g_pstEmm);
		g_pstEmm = NULL;		
	}

	g_ucEcmNum = 0;
	if(g_astEcm)
	{
		for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
		{
			if(g_astEcm[i].pEcmSemaphore)
			{
				DVBSTBCA_SemaphoreDestory(g_astEcm[i].pEcmSemaphore);
				DVBSTBCA_Free(g_astEcm[i].pEcmSemaphore);
				g_astEcm[i].pEcmSemaphore = NULL;		
			}
		}
		DVBSTBCA_Free(g_astEcm);
		g_astEcm = NULL;
	}
	if(g_pstCaKernel)
	{
		DVBSTBCA_Free(g_pstCaKernel);
		g_pstCaKernel = NULL;		
	}
}

int DVBCA_CheckCasID(DVBCA_UINT16 usCasID)
{
	if(g_pstCaKernel->usCasID == usCasID)
		return DVBCA_SUCESS;
	else
		return DVBCA_FAILED;	
}

void DVBCA_SetAreaNo(DVBCA_UINT32 uiAreaNo)
{
	g_pstCaKernel->uiAreaID = uiAreaNo;
}

DVBCA_UINT32 DVBCA_GetAreaNo(void)
{
	return g_pstCaKernel->uiAreaID;
}

void DVBCA_GetCAVer(DVBCA_UINT8 *pucCaVer)
{	
	DVBSTBCA_Memcpy(pucCaVer, (DVBCA_UINT8 *)g_acDVBCasVer, DVBSTBCA_Strlen((char *)g_acDVBCasVer));
}

void DVBCA_GetStbID(DVBCA_UINT8 *pucStbID)
{
	DVBSTBCA_Memcpy(pucStbID, g_pstCaKernel->aucStbID, 4 * sizeof(DVBCA_UINT8));
}

void DVBCA_SetCardID(DVBCA_UINT8 *pucCardID)
{
	DVBSTBCA_Memcpy(g_pstCaKernel->aucCardID, pucCardID, 8 * sizeof(DVBCA_UINT8));
}

void DVBCA_GetCardID(DVBCA_UINT8 *pucCardID)
{
	DVBSTBCA_Memcpy(pucCardID, g_pstCaKernel->aucCardID, 8 * sizeof(DVBCA_UINT8));
}

void DVBCA_GetRealCardID(DVBCA_UINT32 *puiCardID)
{
	DVBCA_UINT32 uiCardID = 0;
	uiCardID = (g_pstCaKernel->aucCardID[4] << 24) | (g_pstCaKernel->aucCardID[5] << 16) 
		| (g_pstCaKernel->aucCardID[6] << 8) | g_pstCaKernel->aucCardID[7];
	*puiCardID = (uiCardID ^ 0x57b87c2d) - 0X5F1D6B;
}

void DVBCA_SetNetworkID(DVBCA_UINT16 usNetworkID)
{
	if(g_pstCaKernel->usNetworkID != usNetworkID && usNetworkID != 0xFFFF)
	{
		g_pstCaKernel->usNetworkID = usNetworkID;
		DVBCA_ReSendEcm();
	}
}

void DVBCA_GetNetworkID(DVBCA_UINT8 *pucBuffer)
{
	pucBuffer[0] = (g_pstCaKernel->usNetworkID >> 8) & 0xFF;
	pucBuffer[1] = g_pstCaKernel->usNetworkID & 0xFF;
}

void DVBCA_ClearEmmCrc(void)
{
	DVBSTBCA_SemaphoreWait(g_pstEmm->pEmmSemaphore);
	g_pstEmm->uiEmmCrc= 0;
	g_pstEmm->uiEventCrc = 0;
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
}

void DVBCA_SetEmmPid(DVBCA_UINT16 usEmmPid)
{
	DVBCA_UINT32 uiEmmCrc = 0;
	DVBCA_UINT32 uiEventCrc = 0;
	DVBCA_SEMAPHORE *pEmmSemaphore = g_pstEmm->pEmmSemaphore;
	DVBSTBCA_SemaphoreWait(g_pstEmm->pEmmSemaphore);

	uiEmmCrc = g_pstEmm->uiEmmCrc;
	uiEventCrc = g_pstEmm->uiEventCrc;
	DVBSTBCA_Memset(g_pstEmm, 0x0, sizeof(ST_EMM));
	g_pstEmm->pEmmSemaphore = pEmmSemaphore;
	g_pstEmm->usPid = usEmmPid;
	g_pstEmm->uiEmmCrc = uiEmmCrc; //100617 dmwang modify, because only one emm in system, only clear it by card insert
	g_pstEmm->uiEventCrc = uiEventCrc;
	g_pstEmm->stFilter[0].aucFilter[0] = 0x80;
	g_pstEmm->stFilter[0].aucFilter[3] = 0x00;
	g_pstEmm->stFilter[0].aucFilter[6] = 0x00;
	g_pstEmm->stFilter[0].aucFilter[7] = 0x00;
	g_pstEmm->stFilter[0].aucFilter[8] = 0x00;
	g_pstEmm->stFilter[0].aucFilter[9] = 0x00;

	g_pstEmm->stFilter[0].aucMask[0] = 0xF0;
	g_pstEmm->stFilter[0].aucMask[3] = 0xFF;
	g_pstEmm->stFilter[0].aucMask[6] = 0xFF;
	g_pstEmm->stFilter[0].aucMask[7] = 0xFF;
	g_pstEmm->stFilter[0].aucMask[8] = 0xFF;
	g_pstEmm->stFilter[0].aucMask[9] = 0xFF;	
	
	g_pstEmm->stFilter[0].ucLen = 10;
	
	g_pstEmm->stFilter[1].aucFilter[0] = 0x80;
	g_pstEmm->stFilter[1].aucFilter[3] = 0x00;
	g_pstEmm->stFilter[1].aucFilter[6] = g_pstCaKernel->aucCardID[4];
	g_pstEmm->stFilter[1].aucFilter[7] = g_pstCaKernel->aucCardID[5];
	g_pstEmm->stFilter[1].aucFilter[8] = g_pstCaKernel->aucCardID[6];
	g_pstEmm->stFilter[1].aucFilter[9] = g_pstCaKernel->aucCardID[7];

	g_pstEmm->stFilter[1].aucMask[0] = 0xF0;
	g_pstEmm->stFilter[1].aucMask[3] = 0xFF;
	g_pstEmm->stFilter[1].aucMask[6] = 0xFF;
	g_pstEmm->stFilter[1].aucMask[7] = 0xFF;
	g_pstEmm->stFilter[1].aucMask[8] = 0xFF;
	g_pstEmm->stFilter[1].aucMask[9] = 0xFF;	
	
	g_pstEmm->stFilter[1].ucLen = 10;

	g_pstEmm->stFilter[2].aucFilter[0] = 0x80;
	g_pstEmm->stFilter[2].aucFilter[3] = 0x01;

	g_pstEmm->stFilter[2].aucMask[0] = 0xF0;
	g_pstEmm->stFilter[2].aucMask[3] = 0xFF;	
	
	g_pstEmm->stFilter[2].ucLen = 4;

	g_pstEmm->stFilter[3].aucFilter[0] = 0x80;
	g_pstEmm->stFilter[3].aucFilter[3] = 0x02;

	g_pstEmm->stFilter[3].aucMask[0] = 0xF0;
	g_pstEmm->stFilter[3].aucMask[3] = 0xFF;	
	
	g_pstEmm->stFilter[3].ucLen = 4;

	g_pstEmm->stFilter[4].aucFilter[0] = 0x80;
	g_pstEmm->stFilter[4].aucFilter[3] = 0x03;

	g_pstEmm->stFilter[4].aucMask[0] = 0xF0;
	g_pstEmm->stFilter[4].aucMask[3] = 0xFF;	
	
	g_pstEmm->stFilter[4].ucLen = 4;
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
	
}


void DVBCA_SetEcmPid(DVBCA_UINT16 *pusEcmPid, DVBCA_UINT8 ucEcmNum)
{	
	int i = 0;
	if(ucEcmNum > DVBCA_MAX_ECM_NUM)
		ucEcmNum = DVBCA_MAX_ECM_NUM;
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
	{
		DVBSTBCA_SemaphoreWait(g_astEcm[i].pEcmSemaphore);
		g_astEcm[i].ucStatus = 0;
	#ifndef SUPPORT_CAM
		DVBSTBCA_Memset(g_astEcm[i].aucBuffer, 0x0, 1024);
	#else
		DVBSTBCA_Memset(g_astEcm[i].aucBuffer, 0x0, 256);
	#endif
		g_astEcm[i].ucTabledID = 0;
		g_astEcm[i].ucCtrl = 0xFF;
		g_astEcm[i].ucErrorType = 0;
		g_astEcm[i].ucCwNum = 0;
		DVBSTBCA_Memset(g_astEcm[i].aucCw, 0x0, 3*8);
		g_astEcm[i].usPid = 0x1FFF;
		g_astEcm[i].uiCurTime = 0;
		g_astEcm[i].uiExpiredTime = 0;
		g_astEcm[i].uiCrc = 0;		
		g_astEcm[i].ucSetFilter = 0;
	}

	g_ucEcmNum = ucEcmNum;
	for(i = 0; i < g_ucEcmNum; i++)
	{
		g_astEcm[i].usPid = pusEcmPid[i];
		DVBSTBCA_Memset(g_astEcm[i].stFilter.aucFilter, 0x0, 12 * sizeof(DVBCA_UINT8));
		DVBSTBCA_Memset(g_astEcm[i].stFilter.aucMask, 0x0, 12 * sizeof(DVBCA_UINT8));		
		g_astEcm[i].stFilter.aucFilter[0] = 0x80;
		g_astEcm[i].stFilter.aucMask[0] = 0xFE;		
		g_astEcm[i].stFilter.ucLen = 1;
	}
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
	{
		DVBSTBCA_SemaphoreSignal(g_astEcm[i].pEcmSemaphore);
	}
}

void DVBCA_GetEmmPid(DVBCA_UINT16 *pusPid)
{
	*pusPid = g_pstEmm->usPid;
}

void DVBCA_GetEcmPid(DVBCA_UINT16 *pusPid, DVBCA_UINT8 *pucPidNum)
{
	int i = 0;
	*pucPidNum = g_ucEcmNum;
	for(i = 0; i < g_ucEcmNum; i++)
	{
		pusPid[i] = g_astEcm[i].usPid;
	}
}

int DVBCA_GetEcmFilter(DVBCA_UINT16 usEcmPid, ST_DVBCAFilter *pstFilter)
{
	int i = 0;
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
	{
		
		if(g_astEcm[i].usPid == usEcmPid)
			break;
	}
	if(i == DVBCA_MAX_ECM_NUM)
		return DVBCA_ECM_PID_ERROR;
	
	g_astEcm[i].ucSetFilter = 1;
	DVBSTBCA_Memcpy(pstFilter , &(g_astEcm[i].stFilter), sizeof(ST_DVBCAFilter));
	return DVBCA_SUCESS;
}
int DVBCA_GetEmmFilter(DVBCA_UINT16 usEmmPid, ST_DVBCAFilter *pstFilter)
{
	if(usEmmPid != g_pstEmm->usPid)
		return DVBCA_EMM_PID_ERROR;
		
	DVBSTBCA_Memcpy(pstFilter ,g_pstEmm->stFilter, 5*sizeof(ST_DVBCAFilter));
	return DVBCA_SUCESS;
}

void DVBCA_ParseData(DVBCA_UINT16 usPid, DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen)
{	
	int i = 0;
	if(usPid == 0x1FFF)
		return;
	if(usPid == g_pstEmm->usPid)
	{
		if(pucReceiveData[0] == 0x82)
			DVBCA_ParseEmm(pucReceiveData, usLen);		
	}
	else
	{
		for(i = 0; i < g_ucEcmNum; i++)
		{
			if(usPid == g_astEcm[i].usPid)
			{
				DVBCA_ParseEcm(&(g_astEcm[i]), pucReceiveData, usLen);
				break;
			}
		}
		if(i == g_ucEcmNum)
		{
			for(i = 0; i < g_pstCaKernel->ucBCFreqSum; i++)
			{
				if(usPid == g_pstCaKernel->ausBCPid[i] && pucReceiveData[0] == g_pstCaKernel->aucBCTableid[i])
					DVBCA_ParseEmm(pucReceiveData, usLen);					
			}
}
	}
}

void DVBCA_ResetEcmFilter(void)
{
	int i = 0;
	
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
		DVBSTBCA_SemaphoreWait(g_astEcm[i].pEcmSemaphore);

	for(i = 0; i < g_ucEcmNum; i++)
	{
		DVBSTBCA_Memset(g_astEcm[i].stFilter.aucFilter, 0x0, 12 * sizeof(DVBCA_UINT8));
		DVBSTBCA_Memset(g_astEcm[i].stFilter.aucMask, 0x0, 12 * sizeof(DVBCA_UINT8));		
		g_astEcm[i].stFilter.aucFilter[0] = 0x80;
		g_astEcm[i].stFilter.aucMask[0] = 0xFE;		
		g_astEcm[i].stFilter.ucLen = 1;
	}
	
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
		DVBSTBCA_SemaphoreSignal(g_astEcm[i].pEcmSemaphore);
	for(i = 0; i < g_ucEcmNum; i++)
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, g_astEcm[i].usPid);
}

void DVBCA_ReSendEcm(void)
{
	int i = 0;
	for(i = 0; i < g_ucEcmNum; i++)
	{
		if(g_astEcm[i].uiCrc != 0)
		{
			DVBSTBCA_SemaphoreWait(g_astEcm[i].pEcmSemaphore);
			g_astEcm[i].ucStatus = DVBCA_ECM_DATA_INPUT;
			DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_SEND_ECM, (DVBCA_UINT32)&(g_astEcm[i]));
			DVBSTBCA_SemaphoreSignal(g_astEcm[i].pEcmSemaphore);
		}
	}
}


static void DVBCA_ParseEcm(ST_ECM *pstEcm, DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen)
{
	DVBCA_UINT32 uiCheckSum = 0;
	DVBCA_UINT32 uiCrc = 0;

	if(pstEcm->ucSetFilter == 0)
		return;
	#ifndef SUPPORT_CAM
	if((pucReceiveData[0] & pstEcm->stFilter.aucMask[0]) != 
		(pstEcm->stFilter.aucFilter[0] & pstEcm->stFilter.aucMask[0]))
	{
		DVBSTBCA_Printf("\nEcm Filter Data Error:%02x:%02x\n",pucReceiveData[0], pstEcm->stFilter.aucFilter[0]);
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, pstEcm->usPid);	
		return;
	}
	#endif

	uiCheckSum = (pucReceiveData[usLen - 3] << 16)|(pucReceiveData[usLen - 2] << 8)|(pucReceiveData[usLen - 1]);

	if(DVBCA_CheckSum(pucReceiveData + 3, usLen - 6) != uiCheckSum)
	{		
		DVBSTBCA_Printf("CheckSum Error\n");
		#ifndef SUPPORT_CAM
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, pstEcm->usPid);	
		#endif
		return;
	}
	#ifndef SUPPORT_CAM
	if(usLen > 1024)
	{
		DVBSTBCA_Printf("usLen Error:%d\n",usLen);
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, pstEcm->usPid);	
		return;
	}
	#else
	if(usLen > 256)
	{
		DVBSTBCA_Printf("usLen Error:%d\n",usLen);
		//DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, pstEcm->usPid);	
		return;
	}
	#endif
	
	DVBSTBCA_SemaphoreWait(pstEcm->pEcmSemaphore);
	uiCrc = DVBCA_Crc32(pucReceiveData, usLen);
	if(uiCrc != pstEcm->uiCrc && g_ucDisableCa == 0)
	{
		pstEcm->ucStatus = DVBCA_ECM_DATA_INPUT;
		DVBSTBCA_Memcpy(pstEcm->aucBuffer, pucReceiveData, usLen);
		pstEcm->uiCrc = uiCrc;
		pstEcm->ucTabledID = pucReceiveData[0];
		DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_SEND_ECM, (DVBCA_UINT32)pstEcm);
#ifdef SHOW_ECM_GOT_TIME
		extern GxTime starttime;
		GxTime nowtime={0};
		GxCore_GetTickTime(&nowtime);
		printf("\n Re start filter send ecm to card >>>>>[DVBCA_ParseEcm]->%d MS<<<<<\n\n",((nowtime.seconds*1000+nowtime.microsecs/1000) - (starttime.seconds*1000+starttime.microsecs/1000)));
		starttime.seconds = nowtime.seconds;
		starttime.microsecs = nowtime.microsecs;
#endif			
	}
	if(pucReceiveData[0] ==0x80)
	{
		pstEcm->stFilter.aucFilter[0] = 0x81;
		pstEcm->stFilter.aucMask[0] = 0xFF;
	}
	else
	{
		pstEcm->stFilter.aucFilter[0] = 0x80;
		pstEcm->stFilter.aucMask[0] = 0xFF;
	}
	DVBSTBCA_SemaphoreSignal(pstEcm->pEcmSemaphore);
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, pstEcm->usPid);	
}

static void DVBCA_ParseEmm(DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiCrc = 0;
	DVBCA_UINT32 uiRealLen = 0;

	if(usLen > 1024)
	{
		DVBSTBCA_Printf("[DVBCA_ParseEmm]usLen Error:%d\n",usLen);
		return;
	}
	if(pucReceiveData[3] > pucReceiveData[4] ) //section error
	{

		DVBSTBCA_Printf("[DVBCA_ParseEmm]pucReceiveData[3] > pucReceiveData[4]\n");
		return;
	}
	if(pucReceiveData[4] != 0)
	{
		if((pucReceiveData[4] + 1)!= g_pstEmm->ucMaxSec
		 	|| pucReceiveData[5] != g_pstEmm->ucEmmType) //new section is arrived, Discarded old section
		{
			g_pstEmm->ucMaxSec = 0;
			g_pstEmm->ucEmmType = 0;
			g_pstEmm->ucRecvSec = 0;
			g_pstEmm->usRecvLen = 0;
		}
		if(pucReceiveData[3] != g_pstEmm->ucRecvSec) //section id is not matched
		{
			DVBSTBCA_Printf("[DVBCA_ParseEmm]pucReceiveData[3] != g_pstEmm->ucRecvSec\n");
			return;
		}

		g_pstEmm->ucEmmType = pucReceiveData[5];
		if(pucReceiveData[3] == 0)
		{
			DVBSTBCA_Memcpy(g_pstEmm->aucDataBuffer, pucReceiveData, usLen - 4);
			g_pstEmm->usRecvLen += usLen - 4;
			g_pstEmm->ucMaxSec = pucReceiveData[4] + 1;
		}
		else
		{
			DVBSTBCA_Memcpy(g_pstEmm->aucDataBuffer + g_pstEmm->usRecvLen, pucReceiveData + 6, usLen - 10);
			g_pstEmm->usRecvLen += usLen - 10;
		}
		g_pstEmm->ucRecvSec++;
		if(g_pstEmm->ucRecvSec == g_pstEmm->ucMaxSec)
		{
			uiRealLen = g_pstEmm->usRecvLen - 3 + 4;
			g_pstEmm->aucDataBuffer[1]  = (uiRealLen >> 8);
			g_pstEmm->aucDataBuffer[2]  = (uiRealLen & 0xFF);
			uiCrc = DVBCA_Crc32(g_pstEmm->aucDataBuffer, g_pstEmm->usRecvLen);
			g_pstEmm->aucDataBuffer[g_pstEmm->usRecvLen] = (uiCrc>> 24) & 0xFF;
			g_pstEmm->aucDataBuffer[g_pstEmm->usRecvLen + 1] = (uiCrc>> 16) & 0xFF;
			g_pstEmm->aucDataBuffer[g_pstEmm->usRecvLen + 2] = (uiCrc>> 8) & 0xFF;
			g_pstEmm->aucDataBuffer[g_pstEmm->usRecvLen + 3] = (uiCrc) & 0xFF;
			g_pstEmm->ucMaxSec = 0;
			g_pstEmm->ucEmmType = 0;
			g_pstEmm->ucRecvSec = 0;
			g_pstEmm->usRecvLen = 0;
			pucBuffer = g_pstEmm->aucDataBuffer;
			uiRealLen += 3;
		}			
	}
	else
	{
		pucBuffer = pucReceiveData;
		uiRealLen = usLen;
	}

	if(pucBuffer == NULL)
	{

		DVBSTBCA_Printf("[DVBCA_ParseEmm] pucBuffer == NULL\n");
		return;
	}
	//DVBSTBCA_Printf("[DVBCA_ParseEmm] pucBuffer[5]=%d\n",pucBuffer[5]);
	switch(pucBuffer[5])
	{
		case DVBCA_EMM_NETPARA:
			DVBCA_ParseNetPara(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_EMM:
			DVBCA_ParseEmmEmm(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_MAIL:
			if(DVBCA_DB_AddMail(pucBuffer, uiRealLen) == CA_DB_SUCCESS)
			{
				DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_NEW_MAIL, (pucBuffer[14] << 6) | (pucBuffer[15] >> 2));				
			}
			break;
		case DVBCA_EMM_URGENT_NOTIFY:
			DVBCA_UrgentParse(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_ADVERTIZE:
			DVBCA_ParseAdvertize(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_PROMPT:
			DVBCA_DB_AddPromptMsg(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_EVENT:
			if(DVBCA_CardGetCardVer() >= 0x140)
				DVBCA_ParseEmmEvent(pucBuffer, uiRealLen);
			break;
		case DVBCA_EMM_CMD:
			if(DVBCA_CardGetCardVer() >= 0x140)
				DVBCA_ParseEmmCmd(pucBuffer, uiRealLen);
			break;
		default:
			break;
	}
}

static void DVBCA_ParseEmmEmm(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	DVBCA_UINT32 uiCrc;
	DVBCA_UINT32 uiNetID;
		
	uiCrc = (pucBuffer[usLen - 4] << 24) | (pucBuffer[usLen - 3] << 16) |
		(pucBuffer[usLen - 2] << 8) | pucBuffer[usLen - 1];		
	
	if(uiCrc == g_pstEmm->uiEmmCrc)
		return;
	if(usLen > 1100)
		return;
	uiNetID = (pucBuffer[11] << 16) | (pucBuffer[12] << 8) | pucBuffer[13];
	if(DVBCA_CheckOpertor(uiNetID) == 0)
		g_ucDisableCa = 1;
	else
		g_ucDisableCa = 0;
	
	if(g_ucDisableCa == 1)
		return;
	DVBSTBCA_SemaphoreWait(g_pstEmm->pEmmSemaphore);			
	DVBSTBCA_Memcpy(g_pstEmm->aucEmmBuffer, pucBuffer, usLen );
	g_pstEmm->uiEmmCrc = uiCrc;
	g_pstEmm->ucEmmStatus = DVBCA_EMM_DATA_INPUT;
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_SEND_EMM, (DVBCA_UINT32)g_pstEmm);
	DVBCA_ReSendEcm();
}

static void DVBCA_ParseEmmEvent(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	DVBCA_UINT32 uiCrc;

	if(g_ucDisableCa == 1)
		return;
	if(DVBCA_CardGetCardVer() < 140)
		return;
	uiCrc = (pucBuffer[usLen - 4] << 24) | (pucBuffer[usLen - 3] << 16) |
		(pucBuffer[usLen - 2] << 8) | pucBuffer[usLen - 1];
	if(uiCrc == g_pstEmm->uiEventCrc)
		return;
	if(usLen > 1100)
		return;
	DVBSTBCA_SemaphoreWait(g_pstEmm->pEmmSemaphore);			
	DVBSTBCA_Memcpy(g_pstEmm->aucEventBuffer, pucBuffer, usLen );
	g_pstEmm->uiEventCrc = uiCrc;
	g_pstEmm->ucEventStatus = DVBCA_EMM_DATA_INPUT;
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_SEND_EVENT, (DVBCA_UINT32)g_pstEmm);
	DVBCA_ReSendEcm();
}

static void DVBCA_ParseEmmCmd(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
//	DVBCA_UINT32 uiCrc;

	if(g_ucDisableCa == 1)
		return;
	if(usLen > 256)
		return;
	DVBSTBCA_SemaphoreWait(g_pstEmm->pEmmSemaphore);			
	DVBSTBCA_Memcpy(g_pstEmm->aucCmdBuffer, pucBuffer, usLen );
	g_pstEmm->ucCmdStatus = DVBCA_EMM_DATA_INPUT;
	DVBSTBCA_SemaphoreSignal(g_pstEmm->pEmmSemaphore);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_SEND_CMD, (DVBCA_UINT32)g_pstEmm);
}

static void DVBCA_ParseNetPara(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	ST_DVBCA_UPGRADEPROG *pstFreq = NULL;
	DVBCA_UINT8 *pucTempPtr = pucBuffer + 10;
	DVBCA_UINT8 ucAreaDisablesum = 0;
	int i = 0;
	DVBCA_UINT8 ucNetName[9];
	DVBCA_UINT32 uiNetID = 0xFFFFFFFF;
	
	DVBSTBCA_Memset(ucNetName, 0x0, 9 * sizeof(DVBCA_UINT8));

	pstFreq = (ST_DVBCA_UPGRADEPROG *)DVBSTBCA_Malloc(sizeof(ST_DVBCA_UPGRADEPROG));
	if(NULL==pstFreq)
	{
		DVBSTBCA_Printf("%s, %d. malloc error!!!\n", __FUNCTION__, __LINE__);
		return;
	}
	uiNetID = (pucTempPtr[0] << 16) | (pucTempPtr[1] << 8) | pucTempPtr[2];
	pucTempPtr += 3; //net_number
	DVBSTBCA_Memcpy(ucNetName, pucTempPtr,8);
	DVBCA_DB_SetNetName(ucNetName);
	pucTempPtr += 8; //net_name
	//(pucTempPtr[0] >> 7) & 0x01 stb card match sign
	ucAreaDisablesum = pucTempPtr[0] &0x7F;
	pucTempPtr += 1; //stb card match sign and area disable num
	pucTempPtr += 4*ucAreaDisablesum;//area disable sum
	DVBCA_DB_SetFaceTime(pucTempPtr[0]);
	pucTempPtr++;//LOGO time	
	pstFreq->ucFreqCount = pucTempPtr[0] >> 4;
	pstFreq->ucVer = pucTempPtr[0] & 0x0F;
	pstFreq->pstFreq = NULL;
	pucTempPtr++;
	if(pstFreq->ucVer != DVBCA_DB_GetFreqVer() || uiNetID != DVBCA_DB_GetNetwork())
	{
		if(pstFreq->ucFreqCount > 0)
		{
			pstFreq->pstFreq = (ST_DVBCA_FREQ *)DVBSTBCA_Malloc(pstFreq->ucFreqCount * sizeof(ST_DVBCA_FREQ));
			if(pstFreq->pstFreq == NULL)
				goto NET_PARA_RTN;
			DVBSTBCA_Memset(pstFreq->pstFreq, 0x0, pstFreq->ucFreqCount * sizeof(ST_DVBCA_FREQ));
			for(i = 0; i < pstFreq->ucFreqCount; i++)
			{
				pstFreq->pstFreq[i].uiFrequency = (pucTempPtr[i*7] << 16) |(pucTempPtr[i*7 + 1] << 8) | pucTempPtr[i*7 + 2];
				pstFreq->pstFreq[i].ucModunation = pucTempPtr[i*7 + 3];
				pstFreq->pstFreq[i].uiSymbolrate = (pucTempPtr[i*7 + 4] << 16) |(pucTempPtr[i*7 + 5] << 8) | pucTempPtr[i*7 + 6];
			}
			DVBSTBCA_UpgradeFreq(pstFreq->pstFreq,pstFreq->ucFreqCount);
		}
		DVBCA_DB_SetNetwork(uiNetID);
		DVBCA_DB_SetFreqVer(pstFreq->ucVer);
	}
	if(DVBCA_CheckOpertor(uiNetID) == 0)
		g_ucDisableCa = 1;
	else
		g_ucDisableCa = 0;
NET_PARA_RTN:
	if(pstFreq)
	{
		if(pstFreq->pstFreq)
			DVBSTBCA_Free(pstFreq->pstFreq); 
		DVBSTBCA_Free(pstFreq);			
	}
}

static void DVBCA_ParseAdvertize(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	ST_CA_MSG *pstSub = NULL;
	DVBCA_UINT32 uiCardID = 0;
	DVBCA_UINT32 uiVer = 0;
	DVBCA_UINT32 uiContentLen = 0;
	DVBCA_UINT32 uiAreaNo = 0;
	DVBCA_UINT16 usPos = 0;
	DVBCA_UINT16 usSubDescLen = 0;
	DVBCA_UINT8 ucSubTag = 0;
	DVBCA_UINT8 ucFindColor = 0;
	
	if(pucBuffer == NULL || usLen <= 27)
		return;
	uiAreaNo = (pucBuffer[10] << 24) | (pucBuffer[11] << 16)  | (pucBuffer[12] << 8) | pucBuffer[13];
	
	if(DVBCA_ChechAreaNo(g_pstCaKernel->uiAreaID, uiAreaNo) == 0x00)
	//if(g_pstCaKernel->uiAreaID != 0xFFFFFFFF && uiAreaNo != 0 && uiAreaNo != g_pstCaKernel->uiAreaID)		
		return;	
	uiVer = (DVBCA_UINT32)(pucBuffer[14] << 24) | (pucBuffer[15] << 16) | (pucBuffer[16] << 8) | pucBuffer[17];
	if(g_uiSubLastVer[0] == uiVer)
		return;	
	if(g_uiSubLastVer[1] == uiVer)
		return;	
	if(g_astSubCache[0].ucUsed && g_astSubCache[0].uiVer == uiVer)
		return;	
	if(g_astSubCache[1].ucUsed && g_astSubCache[1].uiVer == uiVer)
		return;
	if(pucBuffer[18] != 1)
		return;
	uiCardID = (DVBCA_UINT32)(pucBuffer[6] << 24) | (pucBuffer[7] << 16) | (pucBuffer[8] << 8) | pucBuffer[9];
	if(uiCardID == 0)
		pstSub = &(g_astSubCache[0]);
	else
		pstSub = &(g_astSubCache[1]);
	
	DVBSTBCA_Memset(pstSub, 0x0, sizeof(ST_CA_MSG));
	pstSub->ucUsed = 1;
	pstSub->uiVer = uiVer;
	pstSub->uiCardID = uiCardID;
	pstSub->ucDispPos = (pucBuffer[21] >> 6) & 0x03;
	pstSub->ucDispWay = (pucBuffer[21] >> 5) & 0x01;
	pstSub->ucDispCount = ((pucBuffer[21] >> 2) & 0x07) + 1;
	pstSub->ucDispDirection = (pucBuffer[21] >> 1) & 0x01;
	pstSub->ucIntervalTime = pucBuffer[22] | ((pucBuffer[21] & 0x01) << 8);
	pstSub->ucBgColor = pucBuffer[23];
	pstSub->ucFontColor= pucBuffer[24];
	uiContentLen = (pucBuffer[25]<<8) | pucBuffer[26];
	if(uiContentLen > DVBCA_MAXLEN_MSG_TEXT)
		DVBSTBCA_Memcpy(pstSub->aucText, pucBuffer + 27,  DVBCA_MAXLEN_MSG_TEXT);
	else
		DVBSTBCA_Memcpy(pstSub->aucText, pucBuffer + 27,  uiContentLen);

	usPos = uiContentLen + 27;
	while(usPos + 4 < usLen)
	{
		ucSubTag = pucBuffer[usPos];
		usSubDescLen = ((pucBuffer[usPos + 1] << 8) | (pucBuffer[usPos + 2])) & 0x7FF;
		switch(ucSubTag)
		{
			case 0x02:
				if(usSubDescLen == 6)
				{
					pstSub->ucBgRedColor = pucBuffer[usPos + 3];
					pstSub->ucBgGreenColor = pucBuffer[usPos + 4];
					pstSub->ucBgBlueColor = pucBuffer[usPos + 5];
					pstSub->ucFontRedColor = pucBuffer[usPos + 6];
					pstSub->ucFontGreenColor = pucBuffer[usPos + 7];
					pstSub->ucFontBlueColor = pucBuffer[usPos + 8];
					ucFindColor = 1;
				}
				break;
			default:
				break;
		}
		usPos += (usSubDescLen + 3);
	}
	if(ucFindColor == 0)
	{
		pstSub->ucBgRedColor = 0xe9;
		pstSub->ucBgGreenColor = 0xca;
		pstSub->ucBgBlueColor = 0x33;
		pstSub->ucFontRedColor = 0x0a;
		pstSub->ucFontGreenColor = 0x22;
		pstSub->ucFontBlueColor = 0x2a;
	}
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_CHECKNEWMSG, 0);
}

int DVBCA_GetAdvertieze(ST_DVBCA_MSG *pstMsg)
{
	int i = 0;
	if(pstMsg == NULL)
		return DVBCA_FAILED;
	for(i = 0; i < 2; i++)
	{
		if(g_astSubCache[i].ucUsed && g_uiSubLastVer[i] != g_astSubCache[i].uiVer)
		{
			g_uiSubLastVer[i] = g_astSubCache[i].uiVer;
			pstMsg->ucDispPos = g_astSubCache[i].ucDispPos;
			pstMsg->ucDispWay = g_astSubCache[i].ucDispWay;
			pstMsg->ucDispCount = g_astSubCache[i].ucDispCount;
			pstMsg->ucDispDirection = g_astSubCache[i].ucDispDirection;
			pstMsg->ucIntervalTime = g_astSubCache[i].ucIntervalTime;
			pstMsg->ucBgColor = g_astSubCache[i].ucBgColor;
			pstMsg->ucFontColor = g_astSubCache[i].ucFontColor;
			
			pstMsg->stBgColor.ucRed = g_astSubCache[i].ucBgRedColor ;
			pstMsg->stBgColor.ucGreen = g_astSubCache[i].ucBgGreenColor;
			pstMsg->stBgColor.ucBlue = g_astSubCache[i].ucBgBlueColor;
			pstMsg->stBgColor.ucAlpha = 0xFF;
			
			pstMsg->stFontColor.ucRed = g_astSubCache[i].ucFontRedColor ;
			pstMsg->stFontColor.ucGreen = g_astSubCache[i].ucFontGreenColor;
			pstMsg->stFontColor.ucBlue = g_astSubCache[i].ucFontBlueColor;
			pstMsg->stFontColor.ucAlpha = 0xFF;
			
			DVBSTBCA_Memcpy(pstMsg->aucText , g_astSubCache[i].aucText, DVBCA_MAXLEN_MSG_TEXT);
			g_astSubCache[i].ucUsed = 0;
			return DVBCA_SUCESS;
		}
	}
	return DVBCA_FAILED;
}

int DVBCA_GetCurProgExpiredTime(DVBCA_UINT16 usEcmPid, ST_DVBCATIME *pstTime)
{
	DVBCA_UINT32 uiTime = 0;
	int i = 0;
	
	for(i = 0; i < DVBCA_MAX_ECM_NUM; i++)
	{
		if(usEcmPid == g_astEcm[i].usPid)
		{
			uiTime = g_astEcm[i].uiExpiredTime;
			break;
		}
	}
	if(i == DVBCA_MAX_ECM_NUM)
		return DVBCA_ECM_PID_ERROR;
	
	if(uiTime == 0)
		return DVBCA_NOENTITLE;
	pstTime->ucYear = (uiTime >> 25) & 0x7F;
	pstTime->ucMonth = (uiTime >> 21) & 0x0F;
	pstTime->ucDay = (uiTime >> 16) & 0x1F;
	return DVBCA_SUCESS;
}

int DVBCA_CheckOpertor(DVBCA_UINT32 uiOpertorID)
{
	int i = 0;

	for(i = 0; i < ENABLE_OPERATOR_NUM; i++)
	{
		if(g_auiEnableOperator[i] == 0xFFFFFFFF)
			break;
		if(g_auiEnableOperator[i] == uiOpertorID)
			break;
	}
	if(i == ENABLE_OPERATOR_NUM)
		return 0;
	else
		return 1;
}

void DVBCA_SetOpertor(DVBCA_UINT8 *pucBuffer, DVBCA_UINT32 uiLen)
{
	int i = 0;
	DVBCA_UINT32 uiNetID;
	
	if(uiLen == 0 || uiLen > 12)
		return;
	
	g_ucDisableCa = 1;
	
	for(i = 0; i < uiLen; i+=3)
	{		
		uiNetID = (pucBuffer[i] << 24) | (pucBuffer[i+1] << 16) | pucBuffer[i+2];
		if(DVBCA_CheckOpertor(uiNetID) == 1)
		{
			g_ucDisableCa = 0;
			break;
		}
	}
}

int DVBCA_ChechAreaNo(DVBCA_UINT32 uiCardAreaNo, DVBCA_UINT32 uiAreaNo)
{
	int i = 0;
	DVBCA_UINT32 auiMask[4] = {0xFFFFFFFF, 0xFFFFFF00, 0xFFFF0000, 0xFF000000};
	
	if(uiCardAreaNo == 0xFFFFFFFF)
		return 1;
	if(uiAreaNo == 0)
		return 1;

	for(i = 0; i < 4; i++)
	{
		if(((uiAreaNo >> (i *8)) & 0xFF) != 0x00)
			break;
	}

	if((uiCardAreaNo & auiMask[i]) == uiAreaNo)
		return 1;
	else
		return 0;
}


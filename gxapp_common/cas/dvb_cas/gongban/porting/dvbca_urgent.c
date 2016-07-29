#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_card.h"
#include "dvbca_msg.h"
#include "dvbca_kernel.h"
#include "dvbca_db.h"
#include "dvbca_urgent.h"

static ST_DVBCA_URGENT *g_pstUrgent = NULL;
static DVBCA_SEMAPHORE *g_puiUrgentSemaphore = NULL;
static DVBCA_SEMAPHORE *g_puiUrgentExitSemaphore = NULL;

static DVBCA_UINT16 g_usLastDate = 0;
static DVBCA_UINT16 g_usLastTime = 0;

static DVBCA_UINT32 g_uiRealTime = 0;
static DVBCA_UINT32 g_uiLocalTime = 0;
static DVBCA_UINT32 g_uiUrgentRun = 0;

static void DVBCA_UrgentTask(void);
static void DVBCA_UpdateStatus(void);

int DVBCA_Urgent_Init(DVBCA_UINT8 ucThreadPriority)
{
	g_pstUrgent = (ST_DVBCA_URGENT *)DVBSTBCA_Malloc(sizeof(ST_DVBCA_URGENT));
	if(g_pstUrgent == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_Memset(g_pstUrgent, 0x0, sizeof(ST_DVBCA_URGENT));
	
	g_puiUrgentSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiUrgentSemaphore == NULL)
		return DVBCA_FAILED;
	
	DVBSTBCA_SemaphoreInit(g_puiUrgentSemaphore, 0);
	DVBSTBCA_SemaphoreSignal(g_puiUrgentSemaphore);
	
	g_puiUrgentExitSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiUrgentExitSemaphore == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreInit(g_puiUrgentExitSemaphore, 0);

	g_uiUrgentRun = 1;
	DVBSTBCA_RegisterTask("UrgentTask",ucThreadPriority, DVBCA_UrgentTask, NULL, 8*1024);
	return DVBCA_SUCESS;
}

void DVBCA_Urgent_UnInit(void)
{
	if(g_uiUrgentRun == 1)
	{
		g_uiUrgentRun = 0;
		if(g_puiUrgentExitSemaphore != NULL)
			DVBSTBCA_SemaphoreWait(g_puiUrgentExitSemaphore);
	}
	if(g_pstUrgent != NULL)
	{
		DVBSTBCA_Free(g_pstUrgent);
		g_pstUrgent = NULL;
	}
	if(g_puiUrgentSemaphore != NULL)
	{
		DVBSTBCA_SemaphoreDestory(g_puiUrgentSemaphore);
		DVBSTBCA_Free(g_puiUrgentSemaphore);
		g_puiUrgentSemaphore = NULL;		
	}
	if(g_puiUrgentExitSemaphore != NULL)
	{
		DVBSTBCA_SemaphoreDestory(g_puiUrgentExitSemaphore);
		DVBSTBCA_Free(g_puiUrgentExitSemaphore);
		g_puiUrgentExitSemaphore = NULL;		
	}
}


void DVBCA_UrgentParse(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	DVBCA_UINT32 uiTime = 0;
	DVBCA_UINT32 uiTitleLen = 0;
	DVBCA_UINT32 uiContentLen = 0;
	DVBCA_UINT32 uiCurDay = 0;
	DVBCA_UINT32 uiAreaNo = 0;
	DVBCA_UINT32 uiCardAreaNo = 0;
	
	if(g_pstUrgent == NULL || g_puiUrgentSemaphore == NULL)
		return;	
	if(g_pstUrgent->ucVer == (pucBuffer[18] >> 2))
		return;
	
	uiCardAreaNo = DVBCA_GetAreaNo();
	uiAreaNo = (pucBuffer[10] << 24) | (pucBuffer[11] << 16)  | (pucBuffer[12] << 8) | pucBuffer[13];
	if(DVBCA_ChechAreaNo(uiCardAreaNo, uiAreaNo) == 0x00)
	{
		return;
	}

	DVBSTBCA_SemaphoreWait(g_puiUrgentSemaphore);
	
	uiTime = (pucBuffer[14] << 24) | (pucBuffer[15] << 16) | (pucBuffer[16] << 8) | pucBuffer[17];
	uiCurDay = DVBCA_TransYMD((uiTime >> 25) & 0x3F, (uiTime >> 21) & 0x0F, (uiTime >> 16) & 0x1F);
	g_pstUrgent->uiStartTime = uiCurDay * 86400 + ((uiTime >> 11) & 0x1F) * 3600 + ((uiTime >> 5) & 0x3F) * 60;
	

	g_pstUrgent->ucVer = pucBuffer[18] >> 2;

	if(pucBuffer[18] & 0x03)
	{
		DVBCA_ParsePSW(pucBuffer + 19, usLen - 23, pucBuffer + 6, 13);
	}

	g_pstUrgent->ucConCycle = pucBuffer[19];
	g_pstUrgent->usIntervalCycle = ((pucBuffer[20] << 2) | ((pucBuffer[21] & 0xc0) >> 6)) & 0x3FF;
	g_pstUrgent->ucShowTimes = pucBuffer[21] & 0x3F;
	uiTitleLen = pucBuffer[24];
	if(uiTitleLen > DVBCA_NOTIFY_TITLE_SIZE)
		uiTitleLen = DVBCA_NOTIFY_TITLE_SIZE;
	DVBSTBCA_Memset(g_pstUrgent->aucTitle, 0x00, DVBCA_NOTIFY_TITLE_SIZE + 1);
	DVBSTBCA_Memcpy(g_pstUrgent->aucTitle, pucBuffer + 25, uiTitleLen);
	uiContentLen = (pucBuffer[25 + uiTitleLen] << 8) | pucBuffer[26 + uiTitleLen];
	if(uiContentLen > DVBCA_NOTIFY_CONTENT_SIZE)
		uiContentLen = DVBCA_NOTIFY_CONTENT_SIZE;
	DVBSTBCA_Memset(g_pstUrgent->aucContent, 0x00, DVBCA_NOTIFY_CONTENT_SIZE + 1);
	DVBSTBCA_Memcpy(g_pstUrgent->aucContent, pucBuffer + 27 + uiTitleLen, uiContentLen);
	if(g_pstUrgent->ucDisplay)
	{
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_HIDEURGENT, 0);
		g_pstUrgent->ucDisplay = 0;
	}
	g_pstUrgent->ucNewUrgent = 1;
	g_uiLocalTime = 0;
	DVBSTBCA_SemaphoreSignal(g_puiUrgentSemaphore);
}

static void DVBCA_UrgentTask(void)
{
	DVBCA_UINT16 usDate = 0;
	DVBCA_UINT16 usTime = 0;
	
	while(g_uiUrgentRun)
	{
		DVBSTBCA_SemaphoreWait(g_puiUrgentSemaphore);
		
		DVBCA_UpdateStatus();
		
		DVBCA_DB_Get(CA_DB_CMD_DATE, &usDate);
		DVBCA_DB_Get(CA_DB_CMD_TIME, &usTime);
		if(usDate != g_usLastDate || usTime != g_usLastTime)
		{
			g_usLastDate = usDate;
			g_usLastTime = usTime;
			g_uiRealTime = g_usLastDate * 86400 + (g_usLastTime >> 8) * 3600 + (g_usLastTime & 0xFF) * 60;
		}
		else
			g_uiRealTime++;
		DVBSTBCA_SemaphoreSignal(g_puiUrgentSemaphore);
		DVBSTBCA_Sleep(1000);
	}
	DVBSTBCA_SemaphoreSignal(g_puiUrgentExitSemaphore);
}

static void DVBCA_UpdateStatus(void)
{
	DVBCA_UINT32 uiDuraTime = 0;
	DVBCA_UINT32 uiTime = 0;
	
	uiDuraTime = g_pstUrgent->ucConCycle + g_pstUrgent->usIntervalCycle;

	if(g_uiLocalTime >= g_pstUrgent->ucShowTimes * uiDuraTime)
	{
		if(g_pstUrgent->ucDisplay == 1)
		{
			g_pstUrgent->ucDisplay = 0;
			DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_HIDEURGENT, 0);
		}
		return;
	}
	
	if(g_pstUrgent->ucNewUrgent == 1)
	{
		if(g_uiRealTime < g_pstUrgent->uiStartTime)
			return;
		g_uiLocalTime = g_uiRealTime - g_pstUrgent->uiStartTime;
		g_pstUrgent->ucNewUrgent = 0;
	}
	
	uiTime = g_uiLocalTime % uiDuraTime;
	if(uiTime < g_pstUrgent->ucConCycle)
	{
		if(g_pstUrgent->ucDisplay == 0)
		{
			g_pstUrgent->ucDisplay = 1;
			DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_DISPLAYURGENT, (DVBCA_UINT32)g_pstUrgent);
		}
	}
	else if(g_pstUrgent->ucDisplay == 1)
	{
		g_pstUrgent->ucDisplay = 0;
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_HIDEURGENT, 0);
	}
	g_uiLocalTime++;
}


#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_db.h"
#include "dvbca_card.h"
#include "dvbca_msg.h"
#include "dvbca_kernel.h"
#include "dvbca_urgent.h"

static DVBCA_UINT8 g_aucErrorCW[8] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
static DVBCA_UINT8 g_ucDisplayMsgType = DVBCA_MESSAGE_MAX_NUM;
static DVBCA_UINT8 g_ucMsgType = DVBCA_MESSAGE_MAX_NUM;
static DVBCA_UINT8 g_ucOSDCtrl = OSD_HIDE; // 0:display  1:hide
static DVBCA_UINT8 g_ucFingerCtrl = FINGER_HIDE; //0:hide 1:display
static DVBCA_UINT8 g_ucOSDRunStatus = OSD_STOP;
static DVBCA_UINT16 g_usOSDEcmPid = 0x1FFF;
static DVBCA_UINT16 g_usFingerEcmPid = 0x1FFF;
static DVBCA_UINT16 g_usCurMsgEcmPid = 0x1FFF;
static int g_iPPCDisplayDay = -1;
static ST_DVBCA_MSG g_stMsg;
static DVBCA_UINT32 g_uiDvbcaMsgRun = 0;
static DVBCA_SEMAPHORE *g_puiDvbcaMsgExitSemaphore = NULL;


static void DVBCA_MsgAuthorizeFailed(void);
static void DVBCA_MsgInsert(void);
static void DVBCA_MsgRemove(void);
static void DVBCA_MsgSetEcmFilter(DVBCA_UINT16 usEcmPid);
static void DVBCA_MsgSetEmmFilter(DVBCA_UINT16 usEmmPid);
static void DVBCA_MsgRecvEcm(ST_ECM *pstEcm);
static void DVBCA_MsgShowFinger(DVBCA_UINT16 usPid);
static void DVBCA_MsgCheckMsgStatus(void);
static void DVBCA_MsgOSDStatusUpdate(void);
static void DVBCA_MsgShowUrgent(ST_DVBCA_URGENT *pstUrgent);
static void DVBCA_MsgHideUrgent(void);
static void DVBCA_MsgCheckNewMsg(void);


int DVBCA_MsgTaskInit(DVBCA_UINT8 ucThreadPriority)
{
	g_uiDvbcaMsgRun = 1;
	
	g_puiDvbcaMsgExitSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiDvbcaMsgExitSemaphore == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreInit(g_puiDvbcaMsgExitSemaphore, 0);	

	DVBSTBCA_RegisterTask("MsgTask",ucThreadPriority, DVBCA_MsgTask, NULL, 16*1024);
	return DVBCA_SUCESS;
}

void DVBCA_MsgTaskUnInit(void)
{
	if(g_uiDvbcaMsgRun == 1)
	{
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_EXIT, 0);
		if(g_puiDvbcaMsgExitSemaphore != NULL)
			DVBSTBCA_SemaphoreWait(g_puiDvbcaMsgExitSemaphore);
	}
	if(g_puiDvbcaMsgExitSemaphore != NULL)
	{
		DVBSTBCA_SemaphoreDestory(g_puiDvbcaMsgExitSemaphore);
		DVBSTBCA_Free(g_puiDvbcaMsgExitSemaphore);
		g_puiDvbcaMsgExitSemaphore = NULL;		
	}
}

void DVBCA_MsgTask(void)
{
	ST_MSG_QUEUE stMsg;
	
	while(g_uiDvbcaMsgRun)
	{
		DVBSTBCA_Memset(&stMsg, 0x0, sizeof(ST_MSG_QUEUE));
		if(DVBCA_MsgQueueQuery(MSG_MSGTASK, &stMsg) != MSG_SUCESS)
			continue;
		switch(stMsg.ucMsgType)
		{
			case MSG_CARD_AUTHORIZE_FAILED:
				DVBCA_MsgAuthorizeFailed();
				break;
			case MSG_CARD_REMOVE:
				DVBCA_MsgRemove();
				break;
			case MSG_CARD_INSERT:
				DVBCA_MsgInsert();
				break;
			case MSG_CARD_SET_ECM_FILTER:
				DVBCA_MsgSetEcmFilter((DVBCA_UINT16)stMsg.uiData);
				break;
			case MSG_CARD_SET_EMM_FILTER:
				DVBCA_MsgSetEmmFilter((DVBCA_UINT16)stMsg.uiData);
				break;
			case MSG_CARD_RECV_ECM:
				DVBCA_MsgRecvEcm((ST_ECM *)stMsg.uiData);
				break;
			case MSG_CARD_CHECK_MSG_STATUS:
				DVBCA_MsgCheckMsgStatus();
				break;
			case MSG_CARD_NEW_MAIL:
				DVBSTBCA_NewEmailNotify((DVBCA_UINT16 )stMsg.uiData);
				break;
			case MSG_CARD_HIDEURGENT:
				DVBCA_MsgHideUrgent();
				break;
			case MSG_CARD_DISPLAYURGENT:
				DVBCA_MsgShowUrgent((ST_DVBCA_URGENT *)stMsg.uiData);
				break;
			case MSG_CARD_CHECKNEWMSG:
				DVBCA_MsgCheckNewMsg();
				break;
			case MSG_CARD_EXIT:
				g_uiDvbcaMsgRun = 0;
				break;
			default:
				break;
		}
		DVBSTBCA_Sleep(10);
	}

	DVBSTBCA_SemaphoreSignal(g_puiDvbcaMsgExitSemaphore);


}

void DVBCA_MsgShowOsdMessageEnd(void)
{
	if(g_ucOSDRunStatus != OSD_STOP)
		g_ucOSDRunStatus = OSD_STOP;	
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_CHECKNEWMSG, 0);
}

static void DVBCA_MsgHideUrgent(void)
{
	DVBSTBCA_HideUrgent();
}

static void DVBCA_MsgShowUrgent(ST_DVBCA_URGENT *pstUrgent)
{
	DVBSTBCA_ShowUrgent(pstUrgent->aucTitle, pstUrgent->aucContent);
}

static void DVBCA_MsgAuthorizeFailed(void)
{
	DVBCA_UINT8 i = 0;
	DVBCA_UINT8 ucEcmNum = 0;
	DVBCA_UINT16 ausEcmPid[DVBCA_MAX_ECM_NUM];
	DVBCA_UINT16 usEcmPid = 0x1FFF;

	DVBCA_GetEcmPid(ausEcmPid, &ucEcmNum);
	for(i = 0; i < ucEcmNum; i++)
	{
		if(ausEcmPid[i] != 0x1FFF)
		{
			usEcmPid = ausEcmPid[i];
			break;
		}
	}
		
	if(usEcmPid != 0x1FFF)
	{
		DVBSTBCA_ShowMessage(usEcmPid, DVBCA_MESSAGE_BADCARD);
		g_ucDisplayMsgType = DVBCA_MESSAGE_BADCARD;
	}
	g_usCurMsgEcmPid = 0x1FFF;
	g_ucMsgType = DVBCA_MESSAGE_BADCARD;
}

static void DVBCA_MsgInsert(void)
{
	if(g_ucDisplayMsgType == DVBCA_MESSAGE_INSERTCARD)
	{
		DVBSTBCA_HideMessage(0x1FFF, DVBCA_MESSAGE_INSERTCARD);
	}
	g_usCurMsgEcmPid = 0x1FFF;
	g_ucMsgType = DVBCA_MESSAGE_MAX_NUM;
	g_ucDisplayMsgType = DVBCA_MESSAGE_MAX_NUM;
}

static void DVBCA_MsgRemove(void)
{
	DVBCA_UINT8 ucEcmNum = 0;
	DVBCA_UINT16 ausEcmPid[DVBCA_MAX_ECM_NUM];
	DVBCA_UINT16 usEcmPid = 0x1FFF;
	int i = 0;

	DVBCA_GetEcmPid(ausEcmPid, &ucEcmNum);
	for(i = 0; i < ucEcmNum; i++)
	{
		if(ausEcmPid[i] != 0x1FFF)
		{
			usEcmPid = ausEcmPid[i];
			break;
		}
	}

	if(usEcmPid != 0x1FFF && g_ucDisplayMsgType != DVBCA_MESSAGE_INSERTCARD)
	{
		DVBSTBCA_ShowMessage(usEcmPid, DVBCA_MESSAGE_INSERTCARD);
		g_ucDisplayMsgType = DVBCA_MESSAGE_INSERTCARD;
	}
	else if(usEcmPid == 0x1FFF && g_ucDisplayMsgType == DVBCA_MESSAGE_INSERTCARD)
	{
		DVBSTBCA_HideMessage(0x1FFF, DVBCA_MESSAGE_INSERTCARD);
		g_ucDisplayMsgType = DVBCA_MESSAGE_MAX_NUM;
	}
	for(i = 0; i < ucEcmNum; i++)
	{
		if(ausEcmPid[i] != 0x1FFF)
			DVBSTBCA_ScrSetCW(ausEcmPid[i], g_aucErrorCW, g_aucErrorCW, 8);	
	}
	g_usCurMsgEcmPid = 0x1FFF;
	g_ucMsgType = DVBCA_MESSAGE_INSERTCARD;
	
	if(g_ucFingerCtrl != FINGER_HIDE)
	{
		g_ucFingerCtrl = FINGER_HIDE;
		DVBCA_MsgShowFinger(g_usFingerEcmPid);
	}
	if(g_ucOSDCtrl != OSD_HIDE)
	{
		g_ucOSDCtrl = OSD_HIDE;
		DVBCA_MsgOSDStatusUpdate();
	}
}

static void DVBCA_MsgSetEcmFilter(DVBCA_UINT16 usEcmPid)
{
	ST_DVBCAFilter stFilter;

	if(DVBCA_CardCheckInsert() != CARD_INSERT)
		return;
	if(DVBCA_GetEcmFilter(usEcmPid, &stFilter) != DVBCA_SUCESS)
		return;
	DVBSTBCA_SetEcmFilter(&stFilter, 1, usEcmPid);
}

static void DVBCA_MsgSetEmmFilter(DVBCA_UINT16 usEmmPid)
{
	ST_DVBCAFilter astFilter[5];

	if(DVBCA_CardCheckInsert() != CARD_INSERT)
		return;
	if(DVBCA_GetEmmFilter(usEmmPid, astFilter) != DVBCA_SUCESS)
		return;
	DVBSTBCA_SetEmmFilter(astFilter, 5, usEmmPid);
}
static void DVBCA_MsgRecvEcm(ST_ECM *pstEcm)
{
	DVBCA_UINT8 ucErrorMsg = 0xFF;
	DVBCA_UINT8 ucYear = 0;
	DVBCA_UINT8 ucMonth = 0;
	DVBCA_UINT8 ucDay = 0;
	DVBCA_UINT8 ucHour =  0;
	DVBCA_UINT8 ucMinute = 0;
	DVBCA_UINT8 ucOSDCtrl = 1;
	DVBCA_UINT8 ucFingerCtrl = 0;
	DVBCA_UINT16 usTime = 0;
	DVBCA_UINT32 uiCurDay = 0;
	DVBCA_UINT32 uiExpiredDay = 0;
	int iPPCDay = -1;
	
	DVBSTBCA_SemaphoreWait(pstEcm->pEcmSemaphore);
	if(pstEcm->ucStatus != DVBCA_ECM_DATA_SEND)
		goto RECV_ECM_END;
	pstEcm->ucStatus = DVBCA_ECM_DATA_SET;
	
	ucYear = (pstEcm->uiCurTime >> 25) & 0x7F;
	ucMonth = (pstEcm->uiCurTime >> 21) & 0x0F;
	ucDay = (pstEcm->uiCurTime >> 16) & 0x1F;
	uiCurDay = DVBCA_TransYMD(ucYear, ucMonth, ucDay);
	
	ucHour = (pstEcm->uiCurTime >> 11) & 0x1F;
	ucMinute = (pstEcm->uiCurTime >> 5) & 0x3F;
	usTime = (ucHour << 8) | ucMinute;

	if(CA_LIMIT_TIME < pstEcm->uiCurTime)
	{
		DVBSTBCA_ScrSetCW(pstEcm->usPid, g_aucErrorCW, g_aucErrorCW, 8);	
	}
	else if(pstEcm->ucCwNum > 0)
	{
		if(pstEcm->ucCwNum < 2)
		{
			if(pstEcm->ucTabledID & 0x01)
				DVBSTBCA_ScrSetCW(pstEcm->usPid, pstEcm->aucCw[0], NULL, 8);
			else
				DVBSTBCA_ScrSetCW(pstEcm->usPid, NULL, pstEcm->aucCw[0], 8);	
		}
		else
		{
			if(pstEcm->ucTabledID & 0x01)
				DVBSTBCA_ScrSetCW(pstEcm->usPid, pstEcm->aucCw[pstEcm->ucCwNum - 2], pstEcm->aucCw[pstEcm->ucCwNum - 1], 8);
			else
				DVBSTBCA_ScrSetCW(pstEcm->usPid, pstEcm->aucCw[pstEcm->ucCwNum - 1], pstEcm->aucCw[pstEcm->ucCwNum - 2], 8);
		}
		if(g_ucMsgType != DVBCA_MESSAGE_MAX_NUM && g_usCurMsgEcmPid == pstEcm->usPid)
		{
			g_ucMsgType = DVBCA_MESSAGE_MAX_NUM;
		}
		if(g_ucDisplayMsgType != DVBCA_MESSAGE_MAX_NUM && g_usCurMsgEcmPid == pstEcm->usPid)
		{
			DVBSTBCA_HideMessage(pstEcm->usPid, g_ucDisplayMsgType);
			g_ucDisplayMsgType = DVBCA_MESSAGE_MAX_NUM;
		}
		ucYear = (pstEcm->uiExpiredTime >> 25) & 0x7F;
		ucMonth = (pstEcm->uiExpiredTime >> 21) & 0x0F;
		ucDay = (pstEcm->uiExpiredTime >> 16) & 0x1F;
		ucHour = (pstEcm->uiExpiredTime >> 11) & 0x1F;
		ucMinute = (pstEcm->uiExpiredTime >> 5) & 0x3F;
		uiExpiredDay = DVBCA_TransYMD(ucYear, ucMonth, ucDay);
		if(uiCurDay + DVBCA_PPC_DAY >=  uiExpiredDay)
		{
			if(uiCurDay >= uiExpiredDay)
				iPPCDay = 0;
			else
				iPPCDay = uiExpiredDay - uiCurDay;
			if(iPPCDay != g_iPPCDisplayDay)
			{
				g_iPPCDisplayDay = iPPCDay;
				DVBSTBCA_ShowPPCMessage(pstEcm->usPid, g_iPPCDisplayDay);
			}
		}
	}
	else if(pstEcm->ucErrorType != 0)
	{
		switch(pstEcm->ucErrorType)
		{
			case NO_ENTITLE:
				ucErrorMsg = DVBCA_MESSAGE_NOENTITLE;
				break;
			case ILLEGL_OPERATOR:
				ucErrorMsg = DVBCA_MESSAGE_NOOPER_TYPE;
				break;
			case BLOCK_AREO:
				ucErrorMsg = DVBCA_MESSAGE_BLOCK_AREA;
				break;
			case EXPIRED:
				ucErrorMsg = DVBCA_MESSAGE_EXPIRED;
				break;
			case PASSWORD_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_PASSWORD_ERROR;
				break;
			case NETWORK_ID_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_ERRREGION;
				break;			
			case ADULT_LEVEL_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_WATCHLEVEL;
				break;
			case STR_PAIR_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_PAIRING_ERROR;
				break;
			case EXPIRE_CARD_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_EXPIRED_CARD;
				break;
			case WORK_TIME_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_WORKTIME;
				break;
			case DECRYPT_ERROR:
				ucErrorMsg = DVBCA_MESSAGE_DECRYPTFAIL;
				break;
			default:
				break;
		}

		if(ucErrorMsg != 0xFF)
		{
			if(ucErrorMsg != g_ucMsgType)
			{
				g_ucMsgType = ucErrorMsg;
				g_ucDisplayMsgType = ucErrorMsg;
				DVBSTBCA_ShowMessage(pstEcm->usPid, ucErrorMsg);	
				g_usCurMsgEcmPid = pstEcm->usPid;			
			}
			DVBSTBCA_ScrSetCW(pstEcm->usPid, g_aucErrorCW, g_aucErrorCW, 8);	
		}
	}

	if(pstEcm->ucCtrl != 0xFF)
	{
		ucOSDCtrl = (pstEcm->ucCtrl >> 2) & 0x01;
		ucFingerCtrl = (pstEcm->ucCtrl >> 1) & 0x01;
		
		if(g_usFingerEcmPid == 0x1FFF && g_ucFingerCtrl == FINGER_DISPLAY)
			g_usFingerEcmPid = pstEcm->usPid;
		if(g_usOSDEcmPid == 0x1FFF && g_ucOSDCtrl == OSD_DISPLAY)
			g_usOSDEcmPid = pstEcm->usPid;
		if(ucOSDCtrl != g_ucOSDCtrl)
		{
			if(!(ucOSDCtrl == OSD_HIDE && g_usOSDEcmPid != pstEcm->usPid))
			{
				g_ucOSDCtrl = ucOSDCtrl;
				g_usOSDEcmPid = pstEcm->usPid;
				DVBCA_MsgOSDStatusUpdate();
			}
		}
		if(ucFingerCtrl != g_ucFingerCtrl)
		{
			if(!(ucFingerCtrl == FINGER_HIDE && g_usFingerEcmPid != pstEcm->usPid))
			{
				g_ucFingerCtrl = ucFingerCtrl;
				DVBCA_MsgShowFinger(pstEcm->usPid);		
			}
		}
	}

	if(pstEcm->ucErrorType != DECRYPT_ERROR)
	{
		DVBCA_DB_Set(CA_DB_CMD_DATE, uiCurDay);
		DVBCA_DB_Set(CA_DB_CMD_TIME, usTime);		
	}
	if(g_ucOSDCtrl == OSD_DISPLAY && g_ucOSDRunStatus==OSD_STOP&&DVBCA_GetAdvertieze(&g_stMsg) == DVBCA_SUCESS)
	{
		DVBSTBCA_ShowOSDMessage(&g_stMsg);	
		g_ucOSDRunStatus = OSD_RUN;			
	}
RECV_ECM_END:
	DVBSTBCA_SemaphoreSignal(pstEcm->pEcmSemaphore);
}

static void DVBCA_MsgShowFinger(DVBCA_UINT16 usPid)
{
	DVBCA_UINT32 uiCardID = 0;
	DVBCA_GetRealCardID(&uiCardID);
	if(g_ucFingerCtrl == FINGER_DISPLAY)
		DVBSTBCA_ShowFingerMessage(usPid, uiCardID);
	else
		DVBSTBCA_HideFingerMessage();
	g_usFingerEcmPid = usPid;
}

static void DVBCA_MsgCheckMsgStatus(void)
{
	DVBCA_UINT8 ucEcmNum = 0;
	DVBCA_UINT16 ausEcmPid[DVBCA_MAX_ECM_NUM];
	DVBCA_UINT16 usEcmPid = 0x1FFF;
	int i = 0;

	DVBCA_GetEcmPid(ausEcmPid, &ucEcmNum);
	
	if(ucEcmNum == 0)
	{
		if(g_ucFingerCtrl != FINGER_HIDE)
		{
			g_ucFingerCtrl = FINGER_HIDE;
			DVBCA_MsgShowFinger(0x1FFF);
		}
		if(g_ucOSDCtrl != OSD_HIDE)
		{
			g_ucOSDCtrl = OSD_HIDE;
			DVBCA_MsgOSDStatusUpdate();
		}
	}
	else
	{
		for(i = 0; i < ucEcmNum; i++)
		{
			if(ausEcmPid[i] != 0x1FFF)
			{
				usEcmPid = ausEcmPid[i];
				break;
			}
		}
	}	
	g_iPPCDisplayDay = -1;
	g_usFingerEcmPid = 0x1FFF;
	g_usOSDEcmPid = 0x1FFF;

	if(usEcmPid!= 0x1FFF && (g_ucMsgType == DVBCA_MESSAGE_INSERTCARD || g_ucMsgType == DVBCA_MESSAGE_BADCARD))
	{
		DVBSTBCA_ShowMessage(usEcmPid, g_ucMsgType);
		g_ucDisplayMsgType = g_ucMsgType;
		return;
	}
	if(g_ucDisplayMsgType != DVBCA_MESSAGE_MAX_NUM)
	{
		DVBSTBCA_HideMessage(g_usCurMsgEcmPid, g_ucDisplayMsgType);	
		g_ucDisplayMsgType = DVBCA_MESSAGE_MAX_NUM;	
	}
	g_ucMsgType = DVBCA_MESSAGE_MAX_NUM;
	g_usCurMsgEcmPid = 0x1FFF;

}

static void DVBCA_MsgOSDStatusUpdate(void)
{
	if(g_ucOSDCtrl == OSD_DISPLAY && g_ucOSDRunStatus == OSD_PAUSE)
	{
		g_ucOSDRunStatus = OSD_RUN;
		DVBSTBCA_ResumeOSDMessage();
	}
	else if(g_ucOSDCtrl == OSD_HIDE && g_ucOSDRunStatus == OSD_RUN)
	{
		g_ucOSDRunStatus = OSD_PAUSE;
		DVBSTBCA_HideOSDMessage();
	}
}
static void DVBCA_MsgCheckNewMsg(void)
{		
	if(g_ucOSDCtrl == OSD_DISPLAY)
	{
		if(g_ucOSDRunStatus==OSD_STOP&&DVBCA_GetAdvertieze(&g_stMsg) == DVBCA_SUCESS)
		{
			DVBSTBCA_ShowOSDMessage(&g_stMsg);
			g_ucOSDRunStatus = OSD_RUN;			
		}
	}
}


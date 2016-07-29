#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_db.h"
#include "dvbca_card.h"
#include "dvbca_msg.h"
#include "dvbca_kernel.h"
#include "dvbca_urgent.h"
#include "gxcore.h"
typedef enum
{
	DVBCA_INIT_FAILED,
	DVBCA_INIT_SUCESS,
}DVBCA_STATUS;
static DVBCA_UINT8 g_aucSendBuffer[256];
static DVBCA_UINT8 g_ucCaStatus = DVBCA_INIT_FAILED;

int DVBCASTB_Init(DVBCA_UINT8 ucThreadPriority, DVBCA_UINT8 *pucStbId)
{
	if(DVBCA_DB_Init() != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(DVBCA_MsgQueueInit() != MSG_SUCESS)
		return DVBCA_FAILED;
	if(DVBCA_InitKernel(pucStbId) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	
	if(DVBCA_CardTaskInit(ucThreadPriority) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(DVBCA_MsgTaskInit(ucThreadPriority) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(DVBCA_Urgent_Init(ucThreadPriority) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	g_ucCaStatus = DVBCA_INIT_SUCESS;
	return DVBCA_SUCESS;
}


void DVBCASTB_UnInit(void)
{
	g_ucCaStatus = DVBCA_INIT_FAILED;
	
	DVBCA_Urgent_UnInit();
	DVBCA_MsgTaskUnInit();
	DVBCA_CardTaskUnInit();
	DVBCA_DB_UnInit();
	DVBCA_Kernel_UnInit();
	DVBCA_MsgQueueUnInit();
	
}

int DVBCASTB_IsDVBCA(DVBCA_UINT16 usCaSystemID)
{	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	if(DVBCA_CheckCasID(usCaSystemID) == DVBCA_SUCESS)
		return DVBCA_SUCESS;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_SCInsert(void)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	if(DVBCA_CardGetStatus() != CARD_REMOVE)
		return DVBCA_FAILED;
	DVBCA_ClearEmmCrc();
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_AUTHORIZE, 0);
	DVBCA_CardWaitCard();
	if(DVBCA_CardGetStatus() == CARD_INSERT)
		return DVBCA_SUCESS;
	else
		return DVBCA_FAILED;
}

void DVBCASTB_SCRemove(void)
{	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	if(DVBCA_CardGetStatus() == CARD_REMOVE)
		return;
	DVBCA_CardRemove();
}

void DVBCASTB_SetEmmPid(DVBCA_UINT16 usEmmPid)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	DVBCA_CardCheckInsert();
	if(usEmmPid > 0x1FFF)
		usEmmPid = 0x1FFF;
	DVBCA_SetEmmPid(usEmmPid);
	if(usEmmPid < 0x1FFF && DVBCA_CardCheckInsert() != CARD_REMOVE)
	{
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_EMM_FILTER, usEmmPid);
	}
}

void DVBCASTB_SetEcmPid(IN DVBCA_UINT16 *pusEcmPid,
					   DVBCA_UINT8 ucEcmPidNum)
{
	int i = 0;
	int j = 0;
	DVBCA_UINT8 ucRealEcmNum = 0;
	DVBCA_UINT16 ausRealEcmPid[DVBCA_MAX_ECM_NUM];
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	for(i = 0; i < ucEcmPidNum; i++)
	{
		if(pusEcmPid[i] < 0x1FFF)
		{
			for(j = 0; j < ucRealEcmNum; j++)
			{
				if(ausRealEcmPid[ucRealEcmNum] == pusEcmPid[i])
					break;
			}
			if(j == ucRealEcmNum)
			{
				ausRealEcmPid[ucRealEcmNum] = pusEcmPid[i];
				ucRealEcmNum++;
			}
		}
	}
	DVBCA_SetEcmPid(ausRealEcmPid, ucRealEcmNum);
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_CHECK_MSG_STATUS, 0);
	if(DVBCA_CardCheckInsert() == CARD_INSERT)
	{
		for(i = 0; i < ucRealEcmNum; i++)
		{
			DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_SET_ECM_FILTER, ausRealEcmPid[i]);
		}
	}
#ifdef SHOW_ECM_GOT_TIME
	extern GxTime starttime;
	GxTime nowtime={0};
	GxCore_GetTickTime(&nowtime);
	printf("\n start filter set ecm pid end >>>>>[DVBCASTB_SetEcmPid]->%d MS<<<<<\n\n",((nowtime.seconds*1000+nowtime.microsecs/1000) - (starttime.seconds*1000+starttime.microsecs/1000)));
	starttime.seconds = nowtime.seconds;
	starttime.microsecs = nowtime.microsecs;
#endif
}

void DVBCASTB_PrivateDataGot(DVBCA_UINT16 usPid, 
							DVBCA_UINT8 *pucReceiveData,
							DVBCA_UINT16 usLen)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	if(DVBCA_CardCheckInsert() != CARD_INSERT)
		return;
	DVBCA_ParseData(usPid, pucReceiveData, usLen);
}

void DVBCASTB_NitDataGot(DVBCA_UINT8 *pucReceiveData,
							DVBCA_UINT16 usLen)
{
	if(DVBCA_CardCheckInsert() != CARD_INSERT)
		return;
	DVBCA_SetNetworkID((pucReceiveData[3] << 8) | pucReceiveData[4]);
}


int DVBCASTB_GetCaVer(OUT DVBCA_UINT8 *pucCaVer)
{
	int iCardStatus = 0;
	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	if(pucCaVer == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;

	DVBCA_GetCAVer(pucCaVer);
	return DVBCA_SUCESS;
}

int DVBCASTB_GetCardVer(OUT DVBCA_UINT8 *pucCardVer)
{
	int iCardStatus = 0;
	int iPos = 0;
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucCardVer == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;

	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_CAVER, 0);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != 2)
		return DVBCA_FAILED;
	pucCardVer[iPos++] = 'V';
	if(pucBuffer[0] > 0x09)
		pucCardVer[iPos++] =  ((pucBuffer[0] >> 4) & 0x0F) + 0x30;
	pucCardVer[iPos++] =  (pucBuffer[0]& 0x0F) + 0x30;
	pucCardVer[iPos++] = '.';
	pucCardVer[iPos++] =   ((pucBuffer[1] >> 4) & 0x0F) + 0x30;
	pucCardVer[iPos++] =  (pucBuffer[1]& 0x0F) + 0x30;
	
	return DVBCA_SUCESS;
}

int DVBCASTB_GetCardID(OUT DVBCA_UINT8 *pucCardID)
{
	int iCardStatus = 0;
	DVBCA_UINT32 uiCardID = 0;
	int i = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucCardID == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;

	DVBCA_GetRealCardID(&uiCardID);
	for(i = 7; i >= 0; i--)
	{
		pucCardID[i] = (uiCardID & 0x0F) + 0x30;
		uiCardID = uiCardID >> 4;
	}
	return DVBCA_SUCESS;	
}

int DVBCASTB_SetStbPair(IN DVBCA_UINT8* pucPin)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int iRet = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucPin == NULL)
		return DVBCA_FAILED;
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBSTBCA_Memcpy(g_aucSendBuffer, pucPin, 6);
	DVBCA_GetStbID(g_aucSendBuffer + 6);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_WRITE_STBPAIR, (DVBCA_UINT32)g_aucSendBuffer);
	
	iRet = DVBCA_CardGetReadValue(&pucBuffer, &uiLen);
	if(iRet == DVBCA_SUCESS)
	{
		DVBCA_ReSendEcm();
		return DVBCA_SUCESS;
	}
	else if(iRet == DVBCA_PIN_ERROR)
		return DVBCA_PIN_ERROR;
	else if(iRet == DVBCA_STB_PAIR_FAILED)
		return DVBCA_STB_PAIR_FAILED;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_GetStbPair(OUT DVBCA_UINT32 *puiStbID, 
						OUT DVBCA_UINT8 *pucStbNum)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int i = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(puiStbID == NULL || pucStbNum == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_STBBAND, 0);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if((uiLen & 0x03) != 0)
		return DVBCA_FAILED;
	*pucStbNum = (uiLen >> 2) & 0xFF;
	for(i = 0; i < (*pucStbNum); i++)
	{
		puiStbID[i] = (pucBuffer[ i * 4] << 24) | (pucBuffer[ i * 4 + 1] << 16) | (pucBuffer[ i * 4 + 2] << 8) | pucBuffer[ i * 4 + 3];

	}
	
	return DVBCA_SUCESS;	
}

int DVBCASTB_GetOperatorID(OUT DVBCA_UINT32 *puiOperatorID, 
							OUT DVBCA_UINT32 *puiOperatorNum)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int i = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(puiOperatorID == NULL || puiOperatorNum == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_PROVIDER, 0);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if((uiLen %3) != 0 || uiLen > 12)
		return DVBCA_FAILED;
	*puiOperatorNum = uiLen /3;
	for(i = 0; i < (*puiOperatorNum); i++)
	{
		puiOperatorID[i] = (pucBuffer[ i * 3 ] << 16) | (pucBuffer[ i * 3 + 1] << 8) | pucBuffer[ i * 3 + 2];

	}	
	return DVBCA_SUCESS;	
}

int DVBCASTB_GetOperatorInfo(DVBCA_UINT32 uiOperatorID, 
						OUT ST_DVBCA_OPERATORINFO *pstOperatorInfo)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	DVBCA_UINT32 uiTime = 0;
	int iCardStatus = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pstOperatorInfo == NULL)
		return DVBCA_FAILED;
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_PROVIDERINFO, uiOperatorID);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != 33)
		return DVBCA_FAILED;
	pstOperatorInfo->uiOperatorID = (pucBuffer[0] << 16) | (pucBuffer[1] << 8) | pucBuffer[2];
	pstOperatorInfo->usNetworkID = (pucBuffer[3] << 8) | pucBuffer[4];
	uiTime = (pucBuffer[9] << 24) | (pucBuffer[10] << 16) | (pucBuffer[11] << 8) | pucBuffer[12];
	pstOperatorInfo->stExpiredTime.ucYear = (uiTime >> 25) & 0x7F;
	pstOperatorInfo->stExpiredTime.ucMonth= (uiTime >> 21) & 0x0F;
	pstOperatorInfo->stExpiredTime.ucDay= (uiTime >> 16) & 0x1F;
	pstOperatorInfo->stExpiredTime.ucHour = 23;
	pstOperatorInfo->stExpiredTime.ucMinute = 59;
	pstOperatorInfo->stExpiredTime.ucSecond = 59;
	DVBSTBCA_Memcpy(pstOperatorInfo->aucOperatorName, pucBuffer + 13, 20);
	
	return DVBCA_SUCESS;	
}

int DVBCASTB_GetServiceEntitles(DVBCA_UINT32 uiOperatorID, 
						OUT ST_DVBCA_ENTITLES *pstEntitles)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	DVBCA_UINT32 uiDate = 0;
	int iCardStatus = 0;
	int i = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pstEntitles == NULL)
		return DVBCA_FAILED;
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_AUTHORIZE, uiOperatorID);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != (pucBuffer[0] * 9 + 1))
		return DVBCA_FAILED;
	pstEntitles->usProductCount = pucBuffer[0];
	if(pstEntitles->usProductCount > DVBCA_MAXLEN_ENTITL)
		pstEntitles->usProductCount = DVBCA_MAXLEN_ENTITL;
	for(i = 0; i < pstEntitles->usProductCount; i++)
	{
		pstEntitles->astEntitles[i].usProductID = pucBuffer[i*9 + 1];
		uiDate = (pucBuffer[i*9 + 2] << 24) | (pucBuffer[i*9 + 3] << 16) | (pucBuffer[i*9 + 4] << 8) | pucBuffer[i*9 + 5];
		pstEntitles->astEntitles[i].ucStartYear = (uiDate >> 25) & 0x7F;
		pstEntitles->astEntitles[i].ucStartMonth= (uiDate >> 21) & 0x0F;
		pstEntitles->astEntitles[i].ucStartDay = (uiDate >> 16) & 0x1F;
		uiDate = (pucBuffer[i*9 + 6] << 24) | (pucBuffer[i*9 + 7] << 16) | (pucBuffer[i*9 + 8] << 8) | pucBuffer[i*9 + 9];
		pstEntitles->astEntitles[i].ucExpiredYear = (uiDate >> 25) & 0x7F;
		pstEntitles->astEntitles[i].ucExpiredMonth = (uiDate >> 21) & 0x0F;
		pstEntitles->astEntitles[i].ucExpiredDay = (uiDate >> 16) & 0x1F;
	}
	
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_EVENT_INFO, uiOperatorID);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != 4)
		return DVBCA_FAILED;
	pstEntitles->ucProductVersion = pucBuffer[0];
	pstEntitles->usEventCount = (pucBuffer[1] << 8)|pucBuffer[2];
	pstEntitles->ucEventVersion = pucBuffer[3];
	return DVBCA_SUCESS;	
}

int DVBCASTB_SetRating(IN DVBCA_UINT8* pucPin, 
					DVBCA_UINT8 ucRating)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int iRet = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucPin == NULL)
		return DVBCA_FAILED;
	
	DVBSTBCA_Memcpy(g_aucSendBuffer, pucPin, 6);
	g_aucSendBuffer[6] = ucRating;
	
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_WRITE_RATING, (DVBCA_UINT32)g_aucSendBuffer);
	iRet = DVBCA_CardGetReadValue(&pucBuffer, &uiLen);
	if(iRet == DVBCA_SUCESS)
	{		
		DVBCA_ReSendEcm();
		return DVBCA_SUCESS;
	}
	else if(iRet == DVBCA_PIN_ERROR)
		return DVBCA_PIN_ERROR;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_SetWorkTime(IN DVBCA_UINT8* pucPin, 
					IN ST_DVBCAWORKTIME *pstStartTime,
					IN ST_DVBCAWORKTIME *pstEndTime)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT16 usStartTime = 0;
	DVBCA_UINT16 usEndTime = 0;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int iRet = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucPin == NULL)
		return DVBCA_FAILED;
	if(pstStartTime == NULL)
		return DVBCA_FAILED;
	if(pstEndTime == NULL)
		return DVBCA_FAILED;
	if(pstStartTime->ucHour >= 24 
		|| pstStartTime->ucMinute >= 60
		|| pstEndTime->ucHour >= 24 
		|| pstEndTime->ucMinute >= 60)
		return DVBCA_FAILED;
	
	DVBSTBCA_Memcpy(g_aucSendBuffer, pucPin, 6);
	usStartTime = pstStartTime->ucHour * 60 + pstStartTime->ucMinute;
	usEndTime = pstEndTime->ucHour * 60 + pstEndTime->ucMinute;
	g_aucSendBuffer[6] = (usStartTime >> 8) & 0xFF;
	g_aucSendBuffer[7] = (usStartTime) & 0xFF;
	g_aucSendBuffer[8] = (usEndTime >> 8) & 0xFF;
	g_aucSendBuffer[9] = (usEndTime) & 0xFF;
	
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	if(DVBCA_CardGetVer() < 0x130)
		return DVBCA_CARD_VER_ERROR;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_WRITE_WORKTIME, (DVBCA_UINT32)g_aucSendBuffer);
	iRet = DVBCA_CardGetReadValue(&pucBuffer, &uiLen);
	if(iRet == DVBCA_SUCESS)
	{		
		DVBCA_ReSendEcm();
		return DVBCA_SUCESS;
	}
	else if(iRet == DVBCA_PIN_ERROR)
		return DVBCA_PIN_ERROR;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_GetWorkTime(OUT ST_DVBCAWORKTIME *pstStartTime,
					OUT ST_DVBCAWORKTIME *pstEndTime)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	DVBCA_UINT16 usStartTime = 0;
	DVBCA_UINT16 usEndTime = 0;
	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pstStartTime == NULL || pstEndTime == NULL)
		return DVBCA_FAILED;
	pstStartTime->ucHour = 0;
	pstStartTime->ucMinute = 0;
	pstEndTime->ucHour = 0;
	pstEndTime->ucMinute = 0;
	
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;

	if(DVBCA_CardGetVer() < 0x130)
		return DVBCA_CARD_VER_ERROR;
	
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_WORKTIME, 0);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != 4)
		return DVBCA_FAILED;

	usStartTime = ((pucBuffer[0] & 0xFF) << 8) | (pucBuffer[1] & 0xFF);
	usEndTime = ((pucBuffer[2] & 0xFF) << 8) | (pucBuffer[3] & 0xFF);
	if(usStartTime >= 1440)
		usStartTime = 0;
	if(usEndTime >= 1440)
		usEndTime = 0;
	pstStartTime->ucHour = usStartTime/60;
	pstStartTime->ucMinute = usStartTime%60;
	pstEndTime->ucHour = usEndTime/60;
	pstEndTime->ucMinute = usEndTime%60;
	return DVBCA_SUCESS;
}

int DVBCASTB_GetRating(OUT DVBCA_UINT8 *pucRating)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucRating == NULL)
		return DVBCA_FAILED;
		
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_RATING, 0);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(uiLen != 1)
		return DVBCA_FAILED;
	if(pucBuffer[0] > 7)
		pucBuffer[0] = 0;
	*pucRating = pucBuffer[0];
	return DVBCA_SUCESS;
}

int DVBCASTB_CheckPin(IN DVBCA_UINT8* pucPin)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int iRet = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucPin == NULL)
		return DVBCA_FAILED;
		
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	
	DVBSTBCA_Memcpy(g_aucSendBuffer, pucPin, 6);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_CHECK_PIN, (DVBCA_UINT32)g_aucSendBuffer);
	
	iRet = DVBCA_CardGetReadValue(&pucBuffer, &uiLen);
	if(iRet == DVBCA_SUCESS)
		return DVBCA_SUCESS;
	else if(iRet == DVBCA_PIN_ERROR)
		return DVBCA_PIN_ERROR;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_ChangePin(IN DVBCA_UINT8 *pucOldPin, 
					DVBCA_UINT8 *pucNewPin)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int iRet = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pucOldPin == NULL || pucNewPin == NULL)
		return DVBCA_FAILED;
		
	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	
	DVBSTBCA_Memcpy(g_aucSendBuffer, pucOldPin, 6);
	DVBSTBCA_Memcpy(g_aucSendBuffer + 6, pucNewPin, 6);
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_WRITE_PIN, (DVBCA_UINT32)g_aucSendBuffer);
	
	iRet = DVBCA_CardGetReadValue(&pucBuffer, &uiLen);
	if(iRet == DVBCA_SUCESS)
		return DVBCA_SUCESS;
	else if(iRet == DVBCA_PIN_ERROR)
		return DVBCA_PIN_ERROR;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_GetCurProgExpiredTime(DVBCA_UINT16 usEcmPid, OUT ST_DVBCATIME *pstTime)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	return DVBCA_GetCurProgExpiredTime(usEcmPid, pstTime);
}

DVBCA_UINT16 DVBCASTB_GetMailNum(EN_CA_DB_MAIL_CMD enCmd)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return 0;
	
	return DVBCA_DB_GetMailNum(enCmd);
}

DVBCA_UINT16 DVBCASTB_GetMail(OUT ST_DVBCAEmail *pstMail, 
								DVBCA_UINT8 ucStartPos, 
								DVBCA_UINT16 usNum)
{	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return 0;
	
	return DVBCA_DB_GetMail(pstMail, ucStartPos, usNum);
}

int DVBCASTB_SetMailRead(DVBCA_UINT16 usMailID)
{		
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(DVBCA_DB_SetMailRead(usMailID) == CA_DB_SUCCESS)
		return DVBCA_SUCESS;
	else
		return DVBCA_FAILED;
}

int DVBCASTB_DelMail(DVBCA_UINT16 usMailID)
{	
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;

	if(DVBCA_DB_DelMail(usMailID) == CA_DB_SUCCESS)
		return DVBCA_SUCESS;
	else
		return DVBCA_FAILED;
}

void DVBCASTB_GetNetName(DVBCA_UINT8 *pucNetName)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	DVBCA_DB_GetNetName(pucNetName);
}

void DVBCASTB_ShowOsdMessageEnd(void)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return;
	DVBCA_MsgShowOsdMessageEnd();
}

int DVBCASTB_GetACList(
		DVBCA_UINT32 uiOperatorID,
		OUT DVBCA_UINT16 *pusACArray,
		OUT DVBCA_UINT32 *puiACNum)
{
	DVBCA_UINT8 *pucBuffer = NULL;
	DVBCA_UINT32 uiLen = 0;
	int iCardStatus = 0;
	int i = 0;

	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	
	if(pusACArray == NULL)
		return DVBCA_FAILED;

	iCardStatus = DVBCA_CardCheckInsert();
	if(iCardStatus == CARD_REMOVE)
		return DVBCA_INSERTCARD;
	
	if(iCardStatus == CARD_AUTHORIZE_FAILED)
		return DVBCA_CARD_INVALID;
	DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_READ_AC_LIST, uiOperatorID);
	if(DVBCA_CardGetReadValue(&pucBuffer, &uiLen) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if((uiLen %2) != 0 || uiLen > 10)
		return DVBCA_FAILED;
	*puiACNum = uiLen /2;
	for(i = 0; i < (*puiACNum); i++)
	{
		pusACArray[i] = (pucBuffer[ i * 2 ] << 8) | pucBuffer[ i * 2 + 1];

	}	
	return DVBCA_SUCESS;	
}

int DVBCASTB_GetPromptMsg(
		DVBCA_UINT8 ucPromptID,
		DVBCA_UINT8 ucLanguage,
		OUT DVBCA_UINT8 *pucText)
{
	if(g_ucCaStatus != DVBCA_INIT_SUCESS)
		return DVBCA_INIT_ERROR;
	if(DVBCA_DB_GetPromptMsg(ucPromptID, ucLanguage, pucText) != CA_DB_SUCCESS)
		return DVBCA_FAILED;
	return DVBCA_SUCESS;
}

void DVBCASTB_RestoreDB(void)
{
	DVBCA_DB_Default();
}



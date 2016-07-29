#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_card.h"
#include "dvbca_msg.h"
#include "dvbca_kernel.h"

static DVBCA_UINT8 g_ucCardInsert = CARD_REMOVE;
static DVBCA_UINT8 *g_pucCommond = NULL;
static DVBCA_UINT8 *g_pucReply = NULL;
static DVBCA_UINT16 g_usCommondLen = 0;
static DVBCA_UINT16 g_usReplyLen = 0;
static DVBCA_UINT8 *g_pucCommondKey = NULL;

static const DVBCA_UINT8 g_aucStbIDCmd[] = 		{0x5A, 0x14, 0x00, 0x00, 0x04};
static const DVBCA_UINT8 g_aucKeyCmd[] = 			{0x5A, 0x16, 0x00, 0x00, 0x08};
static const DVBCA_UINT8 g_aucCardIDCmd[] = 		{0x5A, 0x0c, 0x00, 0x00, 0x08};
static const DVBCA_UINT8 g_aucCardVerCmd[] = 		{0x5A, 0x22, 0x00, 0x00, 0x02};
static const DVBCA_UINT8 g_aucGetStbBandCmd[] = 	{0x5A, 0x26, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucProviderCmd[] = 		{0x5A, 0x06, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucProviderInfoCmd[] = 	{0x5A, 0x08, 0x00, 0x00, 0x03};
static const DVBCA_UINT8 g_aucAuthorizeCmd[] = 	{0x5A, 0x12, 0x00, 0x00, 0x03};
static const DVBCA_UINT8 g_aucAuthorizeInfoCmd[] = {0x5A, 0x18, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucECMCmd[] = 			{0x5A, 0x10, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucEMMCmd[] = 			{0x5A, 0x0E, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucEventCmd[] = 		{0x5A, 0x3C, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucCmdCmd[] = 			{0x5A, 0x44, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucStbPare[] = 			{0x5A, 0x24, 0x00, 0x00, 0x0A};
static const DVBCA_UINT8 g_aucCheckPin[] = 			{0x5A, 0x1A, 0x00, 0x00, 0x06};
static const DVBCA_UINT8 g_aucSetPin[] = 			{0x5A, 0x1C, 0x00, 0x00, 0x0C};
static const DVBCA_UINT8 g_aucGetRating[] = 		{0x5A, 0x1E, 0x00, 0x00, 0x01};
static const DVBCA_UINT8 g_aucSetRating[] = 		{0x5A, 0x20, 0x00, 0x00, 0x07};
static const DVBCA_UINT8 g_aucResponseCmd[] = 	{0x5A, 0xc0, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucClearPin[] = 			{0x5A, 0x04, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucGetAreaNo[] = 		{0x5A, 0x30, 0x00, 0x00, 0x04};
static const DVBCA_UINT8 g_aucGetACList[] =  {0x5A, 0x34, 0x00, 0x00, 0x00};
static const DVBCA_UINT8 g_aucSetWorkTime[] = 		{0x5A, 0x36, 0x00, 0x00, 0x0A};
static const DVBCA_UINT8 g_aucGetWorkTime[] = 		{0x5A, 0x38, 0x00, 0x00, 0x04};
static const DVBCA_UINT8 g_aucSetStbPair[] = 		{0x5A, 0x3A, 0x00, 0x00, 0x01};
static const DVBCA_UINT8 g_aucEventInfoCmd[] = 	{0x5A, 0x3E, 0x00, 0x00, 0x03};
static const DVBCA_UINT8 g_aucSetCaModFlag[] = 	{0x5A, 0x40, 0x00, 0x00, 0x01};

static DVBCA_SEMAPHORE *g_puiCardSemaphore = NULL;
static DVBCA_UINT8 g_aucReadBuffer[1100];
static DVBCA_UINT32 g_uiReadLen = 0;
static int g_iResult = 0;
static DVBCA_UINT16 g_usCardVer = 0;
static DVBCA_UINT16 g_usDvbcaCardRun = 0;
static DVBCA_SEMAPHORE *g_puiCardExitSemaphore = NULL;

static void DVBCA_CardTask(void);
static void DVBCA_CardAuthorize(void);
static void DVBCA_CardSendEcm(ST_ECM *pstEcm);
static void DVBCA_CardSendEmm(ST_EMM *pstEmm);
static void DVBCA_CardSendEvent(ST_EMM *pstEmm);
static void DVBCA_CardSendCmd(ST_EMM *pstEmm);
static void DVBCA_CardSendClearPin(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen, DVBCA_UINT8 *pucOperationID);
static void DVBCA_CardParseCw(ST_ECM *pstEcm, DVBCA_UINT8 *pucBuffer);
static void DVBCA_CardGetCW(DVBCA_UINT8 *pucCWBuf);
static void DVBCA_CardReset(void);
static int DVBCA_CardSendInitPara(void);
static int DVBCA_CardSendKeyToCard(void);
static int DVBCA_CardReadCardID(void);
static void DVBCA_CardReadCardVer(void);
static void DVBCA_CardReadStbBand(void);
static void DVBCA_CardReadProvider(void);
static void DVBCA_CardReadProviderInfo(DVBCA_UINT32 uiProviderID);
static void DVBCA_CardReadAuthorize(DVBCA_UINT32 uiProviderID);
static void DVBCA_CardReadACList(DVBCA_UINT32 uiProviderID);
static void DVBCA_CardCheckPin(DVBCA_UINT8 *pucPin);
static void DVBCA_CardSetPin(DVBCA_UINT8 *pucBuffer);
static void DVBCA_CardGetRating(void);
static void DVBCA_CardSetRating(DVBCA_UINT8 *pucBuffer);
static void DVBCA_CardSetWorkTime(DVBCA_UINT8 *pucBuffer);
static void DVBCA_CardGetWorkTime(void);
static void DVBCA_CardSetStbPair(DVBCA_UINT8 *pucBuffer);
static void DVBCA_CardReadEventInfo(DVBCA_UINT32 uiProviderID);
static void DVBCA_CardGetResponse(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord);
static int DVBCA_CardSendToCard(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord);
static int DVBCA_CardSendToCardNotReset(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord);

DVBCA_UINT16  DVBCA_CardGetVer(void)
{
	return g_usCardVer;
}
int DVBCA_CardTaskInit(DVBCA_UINT8 ucThreadPriority)
{	
	g_puiCardSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiCardSemaphore == NULL)
		return DVBCA_FAILED;

	g_pucCommondKey = (DVBCA_UINT8 *)DVBSTBCA_Malloc(8*sizeof(DVBCA_UINT8));
	if(g_pucCommondKey == NULL)
		return DVBCA_FAILED;

	g_pucReply = (DVBCA_UINT8 *)DVBSTBCA_Malloc(512*sizeof(DVBCA_UINT8));
	if(g_pucReply == NULL)
		return DVBCA_FAILED;

	g_pucCommond = (DVBCA_UINT8 *)DVBSTBCA_Malloc(512*sizeof(DVBCA_UINT8));
	if(g_pucCommond == NULL)
		return DVBCA_FAILED;
   
	DVBSTBCA_SemaphoreInit(g_puiCardSemaphore, 0);

	g_puiCardExitSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiCardExitSemaphore == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreInit(g_puiCardExitSemaphore, 0);	

	g_usDvbcaCardRun = 1;

	DVBSTBCA_RegisterTask("CardTask",ucThreadPriority, DVBCA_CardTask, NULL, 8*1024);

	return DVBCA_SUCESS;
}

void DVBCA_CardTaskUnInit(void)
{
	if(g_usDvbcaCardRun == 1)
	{
		DVBCA_MsgQueueAdd(MSG_CARDTASK, CARD_EXIT_MSG, 0);
		if(g_puiCardExitSemaphore != NULL)
			DVBSTBCA_SemaphoreWait(g_puiCardExitSemaphore);
	}
	if(g_puiCardExitSemaphore != NULL)
	{
		DVBSTBCA_SemaphoreDestory(g_puiCardExitSemaphore);
		DVBSTBCA_Free(g_puiCardExitSemaphore);
		g_puiCardExitSemaphore = NULL;		
	}
	if(g_puiCardSemaphore != NULL)
	{
		DVBSTBCA_SemaphoreDestory(g_puiCardSemaphore);
		DVBSTBCA_Free(g_puiCardSemaphore);
		g_puiCardSemaphore = NULL;		
	}
	if(g_pucCommondKey != NULL)
	{
		DVBSTBCA_Free(g_pucCommondKey);
		g_pucCommondKey = NULL;
	}
	if(g_pucReply != NULL)
	{
		DVBSTBCA_Free(g_pucReply);
		g_pucReply = NULL;
	}
	if(g_pucCommond != NULL)
	{
		DVBSTBCA_Free(g_pucCommond);
		g_pucCommond = NULL;
	}
}

int DVBCA_CardCheckInsert(void)
{
	if(g_ucCardInsert == CARD_REMOVE)
	{		
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_REMOVE, 0);
	}
	else if(g_ucCardInsert == CARD_AUTHORIZE_FAILED)
	{		
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_AUTHORIZE_FAILED, 0);
	}
	return g_ucCardInsert;
}

void DVBCA_CardRemove(void)
{
	DVBSTBCA_Printf("DVBCA_CardRemove\n");
	g_ucCardInsert = CARD_REMOVE;
	g_usCardVer = 0;
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_REMOVE, 0);
}

void DVBCA_CardWaitCard(void)
{
	DVBSTBCA_SemaphoreWait(g_puiCardSemaphore);
}

int DVBCA_CardGetStatus(void)
{
	return g_ucCardInsert;
}

int  DVBCA_CardGetReadValue(DVBCA_UINT8 **pucBuffer, DVBCA_UINT32 *puiLen)
{
	if(pucBuffer == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreWait(g_puiCardSemaphore);
	*pucBuffer = g_aucReadBuffer;
	*puiLen = g_uiReadLen;
	return g_iResult;
}

DVBCA_UINT16 DVBCA_CardGetCardVer(void)
{
	return g_usCardVer;
}

static void DVBCA_CardTask(void)
{
	ST_MSG_QUEUE stMsg; 
	while(g_usDvbcaCardRun)
	{
		DVBSTBCA_Memset(&stMsg, 0x0, sizeof(ST_MSG_QUEUE));
		if(DVBCA_MsgQueueQuery(MSG_CARDTASK, &stMsg) != MSG_SUCESS)
			continue;
		switch(stMsg.ucMsgType)
		{
			case CARD_AUTHORIZE:
				DVBCA_CardAuthorize();
				break;
			case CARD_SEND_ECM:
				DVBCA_CardSendEcm((ST_ECM *)stMsg.uiData);
				break;
			case CARD_SEND_EMM:
				DVBCA_CardSendEmm((ST_EMM *)stMsg.uiData);
				break;
			case CARD_READ_CAVER:
				DVBCA_CardReadCardVer();
				break;
			case CARD_READ_STBBAND:
				DVBCA_CardReadStbBand();
				break;
			case CARD_READ_PROVIDER:
				DVBCA_CardReadProvider();
				break;
			case CARD_READ_PROVIDERINFO:
				DVBCA_CardReadProviderInfo(stMsg.uiData);
				break;
			case CARD_READ_AUTHORIZE:
				DVBCA_CardReadAuthorize(stMsg.uiData);
				break;
			case CARD_WRITE_STBPAIR:
				DVBCA_CardSetStbPair((DVBCA_UINT8 *)stMsg.uiData);
				break;
			case CARD_WRITE_RATING:
				DVBCA_CardSetRating((DVBCA_UINT8 *)stMsg.uiData);
				break;
			case CARD_READ_RATING:
				DVBCA_CardGetRating();
				break;
			case CARD_WRITE_PIN:
				DVBCA_CardSetPin((DVBCA_UINT8 *)stMsg.uiData);
				break;
			case CARD_CHECK_PIN:
				DVBCA_CardCheckPin((DVBCA_UINT8 *)stMsg.uiData);
				break;
			case CARD_READ_AC_LIST:
				DVBCA_CardReadACList(stMsg.uiData);
				break;
			case CARD_WRITE_WORKTIME:
				DVBCA_CardSetWorkTime((DVBCA_UINT8 *)stMsg.uiData);
				break;
			case CARD_READ_WORKTIME:
				DVBCA_CardGetWorkTime();
				break;
			case CARD_SEND_EVENT:
				DVBCA_CardSendEvent((ST_EMM *)stMsg.uiData);
				break;
			case CARD_SEND_CMD:
				DVBCA_CardSendCmd((ST_EMM *)stMsg.uiData);
				break;
			case CARD_READ_EVENT_INFO:
				DVBCA_CardReadEventInfo(stMsg.uiData);
				break;
			case CARD_EXIT_MSG:
				g_usDvbcaCardRun = 0;
				break;
			default:
				break;
		}
		DVBSTBCA_Sleep(10);
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardExitSemaphore);
}

static void DVBCA_CardAuthorize(void)
{
	if(DVBCA_CardSendInitPara() == DVBCA_SUCESS
		&& DVBCA_CardSendKeyToCard() == DVBCA_SUCESS
		&& DVBCA_CardReadCardID() == DVBCA_SUCESS)
	{
		g_ucCardInsert = CARD_INSERT;
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_INSERT, 0);	
	}	
	else
	{
		g_ucCardInsert = CARD_AUTHORIZE_FAILED;
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_AUTHORIZE_FAILED, 0);
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardSendEcm(ST_ECM *pstEcm)
{
	DVBCA_UINT16 usLen = 0;
	DVBCA_UINT8 aucStausWord[2];
	int iRet = 0;
	int iRetry = 0;


	DVBSTBCA_SemaphoreWait(pstEcm->pEcmSemaphore);
	if(pstEcm->ucStatus != DVBCA_ECM_DATA_INPUT)
		goto SEND_ECM_END;
	if(pstEcm->aucBuffer[0] != 0x80 && pstEcm->aucBuffer[0] != 0x81)
		goto SEND_ECM_END;
	usLen = (pstEcm->aucBuffer[1] << 8) | pstEcm->aucBuffer[2];
	usLen &= 0xFFF;
	if(usLen > 0xFF || usLen == 0)
		goto SEND_ECM_END;

	for(iRetry = 0; iRetry < 2; iRetry++)
	{
		DVBSTBCA_Memcpy(g_pucCommond + 5, pstEcm->aucBuffer + 3, usLen);
		DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucECMCmd, 5);
		
		DVBCA_GetNetworkID(g_pucCommond + 2);
		
		g_pucCommond[4] = usLen;
		g_usCommondLen = 5 + usLen;

		iRet = DVBCA_CardSendToCard(g_pucCommond, g_usCommondLen, g_pucReply, 
			&g_usReplyLen, aucStausWord);
		if(iRet != DVBCA_SUCESS)
			continue;
		
		if(aucStausWord[0] != 0x61)
			continue;
		
		DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucResponseCmd, 5);
		g_pucCommond[4] = aucStausWord[1];
		g_usCommondLen = 5;

		iRet = DVBCA_CardSendToCardNotReset(g_pucCommond, g_usCommondLen, g_pucReply, 
			&g_usReplyLen, aucStausWord);
		if(iRet != DVBCA_SUCESS)
			continue;
		else
			break;
	}

	if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
	{
		DVBCA_CardParseCw(pstEcm, g_pucReply); 
	}
	else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x03)
	{
		DVBSTBCA_Printf("\n[DVBCA_CradSendEcm] error\n");
		pstEcm->ucCtrl = 0xFF;
		pstEcm->ucCwNum = 0;
		pstEcm->ucErrorType = DECRYPT_ERROR;
		pstEcm->uiCurTime = 0;
		pstEcm->uiExpiredTime = 0;
		
		pstEcm->ucStatus = DVBCA_ECM_DATA_SEND;
		DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_RECV_ECM, (DVBCA_UINT32)pstEcm);
	}
SEND_ECM_END:
	DVBSTBCA_SemaphoreSignal(pstEcm->pEcmSemaphore);
}

static void DVBCA_CardSendEmm(ST_EMM *pstEmm)
{
	DVBCA_UINT8 aucStausWord[2];
	DVBCA_UINT16 usLen = 0;
	DVBCA_UINT16 usMainDataLen = 0;
	DVBCA_UINT16 usPatchDataLen = 0;
	DVBCA_UINT16 usCopyLen = 0;
	DVBCA_UINT16 usBlockPos = 0;
	DVBCA_UINT16 usBlockSum = 0;

	usLen = (pstEmm->aucEmmBuffer[1] << 8) | pstEmm->aucEmmBuffer[2];
	usLen &= 0xFFF;
	
	usMainDataLen = (pstEmm->aucEmmBuffer[15] << 8) | pstEmm->aucEmmBuffer[16];
	usMainDataLen += 11;

	usPatchDataLen = (pstEmm->aucEmmBuffer[6 + usMainDataLen] << 8) | pstEmm->aucEmmBuffer[6 + usMainDataLen + 1];
	
	DVBSTBCA_SemaphoreWait(pstEmm->pEmmSemaphore);	
	if(pstEmm->ucEmmStatus != DVBCA_EMM_DATA_INPUT)
	{
		pstEmm->uiEmmCrc = 0;
		goto SEND_EMM_OVER;
	}
	
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucEMMCmd, 5);
	usBlockSum = (usMainDataLen + 254) /255;
	if(usBlockSum > 4)
	{
		//pstEmm->uiCardCrc = 0;
		goto SEND_EMM_OVER;
	}
	for(usBlockPos = 0; usBlockPos < usBlockSum; usBlockPos++)
	{
		if((usBlockPos +1) * 255 > usMainDataLen)
			usCopyLen = usMainDataLen - usBlockPos * 255;
		else
			usCopyLen = 255;
		
		if(usBlockSum == 1)
			g_pucCommond[2] = 0;
		else
			g_pucCommond[2] = usBlockSum;
		
		g_pucCommond[3] = usBlockPos;
		g_pucCommond[4] = usCopyLen & 0xFF;
		
		DVBSTBCA_Memcpy(g_pucCommond + 5, pstEmm->aucEmmBuffer+ 6 + usBlockPos * 255, usCopyLen);
		g_usCommondLen = 5 + usCopyLen;

		
		if(DVBCA_CardSendToCard(g_pucCommond, g_usCommondLen, 
			g_pucReply, &g_usReplyLen, aucStausWord) != DVBCA_SUCESS)
			continue;
			
		DVBSTBCA_Printf("status:%02x:%02x:%d\n",aucStausWord[0],aucStausWord[1],usBlockPos);
		if(aucStausWord[0] != 0x90)
			break;
		if(aucStausWord[1] == 0x00)
			break;
	}
	if(aucStausWord[0] != 0x90 || aucStausWord[1] != 0x00)
	{
		pstEmm->uiEmmCrc= 0;
	}
	pstEmm->ucEmmStatus = DVBCA_EMM_DATA_SEND;
	if(usPatchDataLen + usMainDataLen + 8 < usLen)
		DVBCA_CardSendClearPin(pstEmm->aucEmmBuffer+ 6 + usMainDataLen + 2, 
					usPatchDataLen, pstEmm->aucEmmBuffer + 11);

	DVBCA_CardSendToCard((DVBCA_UINT8 *)g_aucGetAreaNo, 5, g_pucReply, &g_usReplyLen, aucStausWord);	
	if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00 && g_usReplyLen == 4)
		DVBCA_SetAreaNo((g_pucReply[0] << 24) | (g_pucReply[1] << 16) | (g_pucReply[2] << 8) |g_pucReply[3]);
SEND_EMM_OVER:
	DVBSTBCA_SemaphoreSignal(pstEmm->pEmmSemaphore);
}

static void DVBCA_CardSendEvent(ST_EMM *pstEmm)
{
	DVBCA_UINT8 aucStausWord[2];
	DVBCA_UINT16 usLen = 0;
	DVBCA_UINT16 usMainDataLen = 0;
	DVBCA_UINT16 usCopyLen = 0;
	DVBCA_UINT16 usBlockPos = 0;
	DVBCA_UINT16 usBlockSum = 0;


	usLen = (pstEmm->aucEventBuffer[1] << 8) | pstEmm->aucEventBuffer[2];
	usLen &= 0xFFF;
	
	usMainDataLen = (pstEmm->aucEventBuffer[15] << 8) | pstEmm->aucEventBuffer[16];
	usMainDataLen += 11;
	
	DVBSTBCA_SemaphoreWait(pstEmm->pEmmSemaphore);	
	if(pstEmm->ucEventStatus != DVBCA_EMM_DATA_INPUT)
	{
		pstEmm->uiEventCrc = 0;
		goto SEND_EMM_OVER;
	}
	
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucEventCmd, 5);
	usBlockSum = (usMainDataLen + 254) /255;
	if(usBlockSum > 4)
	{
		//pstEmm->uiCardCrc = 0;
		goto SEND_EMM_OVER;
	}
	for(usBlockPos = 0; usBlockPos < usBlockSum; usBlockPos++)
	{
		if((usBlockPos +1) * 255 > usMainDataLen)
			usCopyLen = usMainDataLen - usBlockPos * 255;
		else
			usCopyLen = 255;
		
		if(usBlockSum == 1)
			g_pucCommond[2] = 0;
		else
			g_pucCommond[2] = usBlockSum;
		
		g_pucCommond[3] = usBlockPos;
		g_pucCommond[4] = usCopyLen & 0xFF;
		
		DVBSTBCA_Memcpy(g_pucCommond + 5, pstEmm->aucEventBuffer+ 6 + usBlockPos * 255, usCopyLen);
		g_usCommondLen = 5 + usCopyLen;
		
		if(DVBCA_CardSendToCard(g_pucCommond, g_usCommondLen, 
			g_pucReply, &g_usReplyLen, aucStausWord) != DVBCA_SUCESS)
			continue;
		if(aucStausWord[0] != 0x90)
			break;
		if(aucStausWord[1] == 0x00)
			break;
	}
	if(aucStausWord[0] != 0x90 || aucStausWord[1] != 0x00)
	{
		pstEmm->uiEventCrc = 0;
	}
	pstEmm->ucEventStatus = DVBCA_EMM_DATA_SEND;
SEND_EMM_OVER:
	DVBSTBCA_SemaphoreSignal(pstEmm->pEmmSemaphore);
}

static void DVBCA_CardSendCmd(ST_EMM *pstEmm)
{
	DVBCA_UINT8 aucStausWord[2];
	DVBCA_UINT16 usLen = 0;
	DVBCA_UINT16 usSendLen = 0;

	usLen = (pstEmm->aucCmdBuffer[1] << 8) | pstEmm->aucCmdBuffer[2];
	usLen = (usLen & 0xFFF) + 3;

	if(usLen > 256)
		return;
	DVBSTBCA_SemaphoreWait(pstEmm->pEmmSemaphore);	
	if(pstEmm->ucCmdStatus != DVBCA_EMM_DATA_INPUT)
	{
		goto SEND_EMM_OVER;
	}
	usSendLen = usLen - 6 - 4;
	
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucCmdCmd, 5);
	g_pucCommond[4] = usSendLen & 0xFF;
	DVBSTBCA_Memcpy(g_pucCommond + 5, pstEmm->aucCmdBuffer + 6 , usSendLen);
	g_usCommondLen = 5 + usSendLen;
	DVBCA_CardSendToCard(g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	pstEmm->ucEventStatus = DVBCA_EMM_DATA_SEND;
SEND_EMM_OVER:
	DVBSTBCA_SemaphoreSignal(pstEmm->pEmmSemaphore);
}

static void DVBCA_CardSendClearPin(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen, DVBCA_UINT8 *pucOperationID)
{
	DVBCA_UINT16 usPos = 0;
	DVBCA_UINT8 ucDataType = 0;
	DVBCA_UINT8 ucDataLen = 0;
	DVBCA_UINT8 aucStausWord[2];
	while(usPos < usLen)
	{
		ucDataType = pucBuffer[usPos];
		ucDataLen = pucBuffer[usPos + 1];
		if(ucDataType == 0)
		{
			DVBSTBCA_Memcpy(g_pucCommond , (DVBCA_UINT8 *)g_aucClearPin , 5);
			DVBSTBCA_Memcpy(g_pucCommond +5, pucOperationID , 3);	
			DVBSTBCA_Memcpy(g_pucCommond +8, pucBuffer + usPos + 2 , 1);	
			g_pucCommond[4] = 4;	
			if(pucBuffer[usPos + 3] != 0xFF ||pucBuffer[usPos + 4] != 0xFF 
				|| pucBuffer[usPos + 5] != 0xFF ||pucBuffer[usPos + 6] != 0xFF 
				|| pucBuffer[usPos + 7] != 0xFF ||pucBuffer[usPos + 8] != 0xFF )
			{		
				g_pucCommond[4] += 6;	
				DVBSTBCA_Memcpy(g_pucCommond +9, pucBuffer + usPos + 3 , 6);
				
			}
			g_usCommondLen = 5 + g_pucCommond[4];
			DVBCA_CardSendToCard(g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
		}
		usPos += (ucDataLen + 2);
	}
}

static void DVBCA_CardParseCw(ST_ECM *pstEcm, DVBCA_UINT8 *pucBuffer)
{
	DVBCA_UINT8 ucLoopSum = 0;
	DVBCA_UINT8 ucLoop = 0;
	DVBCA_UINT8 ucPos = 0;

	pstEcm->ucCtrl = 0xFF;
	pstEcm->ucCwNum = 0;
	pstEcm->ucErrorType = 0;
	pstEcm->uiCurTime = 0;
	pstEcm->uiExpiredTime = 0;
	
	ucLoopSum = pucBuffer[0];
	ucPos = 1;
	for(ucLoop = 0; ucLoop < ucLoopSum; ucLoop++)
	{
		switch(pucBuffer[ucPos])
		{
			case ECM_TYPE_CTRL_INFO:
				pstEcm->ucCtrl = pucBuffer[ucPos + 2];
				pstEcm->uiCurTime = (pucBuffer[ucPos + 3] << 24) | (pucBuffer[ucPos + 4] << 16) | (pucBuffer[ucPos + 5] << 8) | (pucBuffer[ucPos + 6] );
				break;
			case ECM_TYPE_CW_INFO:
				{
					switch(pucBuffer[ucPos + 1])
					{
						case 8:
							DVBSTBCA_Memcpy(pstEcm->aucCw[0], pucBuffer + ucPos + 2, 8);
							DVBCA_CardGetCW(pstEcm->aucCw[0]);
							pstEcm->ucCwNum = 1;
							break;
						case 16:
							DVBSTBCA_Memcpy(pstEcm->aucCw[0], pucBuffer + ucPos + 2, 8);
							DVBSTBCA_Memcpy(pstEcm->aucCw[1], pucBuffer + ucPos + 10, 8);
							DVBCA_CardGetCW(pstEcm->aucCw[0]);
							DVBCA_CardGetCW(pstEcm->aucCw[1]);
							pstEcm->ucCwNum = 2;
							break;
						case 24:
							DVBSTBCA_Memcpy(pstEcm->aucCw[0], pucBuffer + ucPos + 2, 8);
							DVBSTBCA_Memcpy(pstEcm->aucCw[1], pucBuffer + ucPos + 10, 8);
							DVBSTBCA_Memcpy(pstEcm->aucCw[2], pucBuffer + ucPos + 18, 8);
							DVBCA_CardGetCW(pstEcm->aucCw[0]);
							DVBCA_CardGetCW(pstEcm->aucCw[1]);
							DVBCA_CardGetCW(pstEcm->aucCw[2]);
							pstEcm->ucCwNum = 3;
							break;
						default:
							break;
					}
				}
				break;
			case ECM_TYPE_EERO_TYPE:
				pstEcm->ucErrorType = pucBuffer[ucPos + 2];
				break;
			case ECM_TYPE_DATE:
				pstEcm->uiExpiredTime = (pucBuffer[ucPos + 2] << 24) | (pucBuffer[ucPos + 3] << 16) | (pucBuffer[ucPos + 4] << 8) | (pucBuffer[ucPos + 5] );
				break;
				break;
			default:
				break;
		}
		ucPos += (2 + pucBuffer[ucPos + 1]);
	}
	pstEcm->ucStatus = DVBCA_ECM_DATA_SEND;
	//DVBSTBCA_Printf("\n[DVBCA_CardParseCw]:%04x\n",pstEcm->usPid);
	DVBCA_MsgQueueAdd(MSG_MSGTASK, MSG_CARD_RECV_ECM, (DVBCA_UINT32)pstEcm);
}

static void DVBCA_CardGetCW(DVBCA_UINT8 *pucCWBuf)
{
	DVBCA_DesPro(pucCWBuf, g_pucCommondKey, 1);
}

static void DVBCA_CardReset(void)
{
	if(DVBCA_CardCheckInsert() == CARD_REMOVE)
		return;
	if(DVBSTBCA_SCReset() != DVBCA_SUCESS)
		return;
	if(DVBCA_CardSendInitPara() != DVBCA_SUCESS)
		return;
	if(DVBCA_CardSendKeyToCard() != DVBCA_SUCESS)
		return;
	if(DVBCA_CardReadCardID() != DVBCA_SUCESS)
		return;
}

static int DVBCA_CardSendInitPara(void)
{
	DVBCA_UINT8 aucTempCmd[6];
	DVBCA_UINT8 aucStausWord[2];

	g_usCardVer = 0;
	
 	if(DVBCA_CardSendToCardNotReset((DVBCA_UINT8 *)g_aucCardVerCmd, 5, g_pucReply, 
		&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00 && g_usReplyLen == 2)
		{
			g_usCardVer = (g_pucReply[0] << 8) | (g_pucReply[1] & 0xFF);
				
			DVBSTBCA_Memcpy(aucTempCmd, (DVBCA_UINT8 *)g_aucGetAreaNo, 5);
			DVBCA_CardSendToCardNotReset((DVBCA_UINT8 *)aucTempCmd, 5, g_pucReply, &g_usReplyLen, aucStausWord);	
			if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00 && g_usReplyLen == 4)
				DVBCA_SetAreaNo((g_pucReply[0] << 24) | (g_pucReply[1] << 16) | (g_pucReply[2] << 8) |g_pucReply[3]);
			else
				DVBCA_SetAreaNo(0);
			DVBSTBCA_Memcpy(aucTempCmd, (DVBCA_UINT8 *)g_aucSetCaModFlag, 5);
			aucTempCmd[5] = CA_MOD_COMMON;
			
			DVBCA_CardSendToCardNotReset((DVBCA_UINT8 *)aucTempCmd, 6, g_pucReply, &g_usReplyLen, aucStausWord);	
				
			g_uiReadLen = 0;
			g_iResult = DVBCA_FAILED; 
			DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucProviderCmd, 5);
			g_usCommondLen = 5;
			DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
			DVBCA_SetOpertor(g_aucReadBuffer, g_uiReadLen);	
		}
		return DVBCA_SUCESS;
	}
	else
		return DVBCA_FAILED;
}

static int DVBCA_CardSendKeyToCard(void)
{
	DVBCA_UINT8 aucStausWord[2];
	DVBCA_UINT8 aucStbID[4];
	DVBCA_UINT8 aucTempCmd[13];
	DVBCA_UINT16 usTempCmdLen = 0;
	
	DVBCA_GetStbID(aucStbID);
	
	DVBSTBCA_Memcpy(aucTempCmd, (DVBCA_UINT8 *)g_aucStbIDCmd, 5);
	DVBSTBCA_Memcpy(aucTempCmd + 5, aucStbID, 4);
	usTempCmdLen = 9;
	if(DVBSTBCA_SCPBRun(aucTempCmd, usTempCmdLen, g_pucReply, &g_usReplyLen, aucStausWord) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(aucStausWord[0] != 0x90 || aucStausWord[1] != 0x00)
	{
		DVBSTBCA_Printf("GetStbID:%02x:%02x\n",aucStausWord[0],aucStausWord[1]);
		return DVBCA_FAILED;
	}
	DVBSTBCA_Memcpy(aucTempCmd, (DVBCA_UINT8 *)g_aucKeyCmd, 5);
	aucTempCmd[4] = 0x09;
	usTempCmdLen = 5;
	
	if(DVBSTBCA_SCPBRun(aucTempCmd, usTempCmdLen, g_pucReply, &g_usReplyLen, aucStausWord) != DVBCA_SUCESS)
		return DVBCA_FAILED;
	if(aucStausWord[0] != 0x90 || aucStausWord[1] != 0x00)
	{
		DVBSTBCA_Printf("SendKey:%02x:%02x\n",aucStausWord[0],aucStausWord[1]);
		return DVBCA_FAILED;
	}

	if(g_usReplyLen != 9)
	{
		DVBSTBCA_Printf("SendKey:%d\n",g_usReplyLen);
		return DVBCA_FAILED;
	}	
	DVBSTBCA_Memcpy(g_pucCommondKey, g_pucReply + 1, 8);
	return DVBCA_SUCESS;
}

static int DVBCA_CardReadCardID(void)
{
	DVBCA_UINT8 aucStausWord[2];
	int iRet = DVBCA_SUCESS;

	iRet = DVBCA_CardSendToCardNotReset((DVBCA_UINT8 *)g_aucCardIDCmd, 5, g_pucReply, &g_usReplyLen, aucStausWord);
	
	if(iRet == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00 && g_usReplyLen == 8)
		{
			DVBCA_SetCardID(g_pucReply);
			return DVBCA_SUCESS;
		}
		else
		{
			DVBSTBCA_Printf("ReadCardID:%02x:%02x\n",aucStausWord[0],aucStausWord[1]);
			return DVBCA_FAILED;
		}
	}
	else
	{
		return DVBCA_FAILED;
	}
}

static void DVBCA_CardReadCardVer(void)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	
 	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_aucCardVerCmd, 5, g_pucReply, 
		&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00 && g_usReplyLen == 2)
		{
			DVBSTBCA_Memcpy(g_aucReadBuffer, g_pucReply, 2);
			g_uiReadLen = 2;
			g_iResult = DVBCA_SUCESS;
		}
	}
	
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadStbBand(void)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucGetStbBandCmd, 5);
	g_usCommondLen = 5;
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadProvider(void)
{
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucProviderCmd, 5);
	g_usCommondLen = 5;
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadProviderInfo(DVBCA_UINT32 uiProviderID)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucProviderInfoCmd, 5);
	g_pucCommond[5] = (uiProviderID >> 16 ) &0xFF;
	g_pucCommond[6] = (uiProviderID >> 8 ) &0xFF;
	g_pucCommond[7] = (uiProviderID ) &0xFF;
	g_usCommondLen = 8;
	
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadAuthorize(DVBCA_UINT32 uiProviderID)
{
	DVBCA_UINT8 aucStausWord[2];
	DVBCA_UINT8 ucBlockSum = 0;
	DVBCA_UINT16 usTotalLen = 0;
	int i = 0;
	
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucAuthorizeCmd, 5);
	g_pucCommond[5] = (uiProviderID >> 16 ) &0xFF;
	g_pucCommond[6] = (uiProviderID >> 8 ) &0xFF;
	g_pucCommond[7] = (uiProviderID ) &0xFF;
	g_usCommondLen = 8;
	
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	if(g_iResult != DVBCA_SUCESS || g_uiReadLen != 3)
	{
		if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x00)
		{
			g_aucReadBuffer[0] = 0;
			g_uiReadLen = 1;
			g_iResult = DVBCA_SUCESS; 
		}
		goto READ_AUTHORIZE_RTN;
	}
	g_iResult = DVBCA_FAILED; 
	g_uiReadLen = 0;
	usTotalLen = (g_aucReadBuffer[0] << 8) | g_aucReadBuffer[1];
	ucBlockSum = g_aucReadBuffer[2];
	if(ucBlockSum == 0)
		ucBlockSum = 1;
	for(i = 0; i < ucBlockSum && usTotalLen > 0; i++)
	{
		DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucAuthorizeInfoCmd, 5);
		if(ucBlockSum == 1)
			g_pucCommond[2] = 0;
		else
			g_pucCommond[2] = ucBlockSum;
		g_pucCommond[3] = i;
		if(usTotalLen >= 0xFF)
			g_pucCommond[4] = 0xFF;
		else
			g_pucCommond[4] = usTotalLen;
		if(usTotalLen >=255)
			usTotalLen -= 255;
		else
			usTotalLen = 0;
		if(DVBCA_CardSendToCard(g_pucCommond, 5, g_pucReply, 
					&g_usReplyLen, aucStausWord) != DVBCA_SUCESS)
			break;
		if(aucStausWord[0] != 0x90)
			break;
		if(g_uiReadLen + g_usReplyLen > 1100)
		{
			aucStausWord[0] = 0;
			break;
		}
		DVBSTBCA_Memcpy(g_aucReadBuffer + g_uiReadLen, g_pucReply, g_usReplyLen);
		g_uiReadLen += g_usReplyLen;
		if(aucStausWord[0] == 0x00)
			break;
	}
	if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		g_iResult = DVBCA_SUCESS;
	else
		g_uiReadLen = 0;
READ_AUTHORIZE_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadACList(DVBCA_UINT32 uiProviderID)
{
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 

	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucGetACList, 5);
	g_pucCommond[4] = 0x03;
	g_pucCommond[5] = (uiProviderID >> 16 ) &0xFF;
	g_pucCommond[6] = (uiProviderID >> 8 ) &0xFF;
	g_pucCommond[7] = (uiProviderID ) &0xFF;
	g_usCommondLen = 8;
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
		
	if(g_iResult == DVBCA_SUCESS && g_uiReadLen == 0)
	{
		g_uiReadLen = 2;
		DVBSTBCA_Memset(g_aucReadBuffer, 0x0, 2);
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardCheckPin(DVBCA_UINT8 *pucPin)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucCheckPin, 5);
	DVBSTBCA_Memcpy(g_pucCommond + 5, (DVBCA_UINT8 *)pucPin, 6);
	g_usCommondLen = 11;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x05)
		{
			g_iResult = DVBCA_PIN_ERROR;			
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardSetPin(DVBCA_UINT8 *pucBuffer)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucSetPin, 5);
	DVBSTBCA_Memcpy(g_pucCommond + 5, (DVBCA_UINT8 *)pucBuffer, 12);
	g_usCommondLen = 17;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x05)
		{
			g_iResult = DVBCA_PIN_ERROR;			
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardGetRating(void)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucGetRating, 5);
	g_usCommondLen = 5;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			DVBSTBCA_Memcpy(g_aucReadBuffer, g_pucReply, 1);
			g_uiReadLen = 1;
			g_iResult = DVBCA_SUCESS;
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardSetRating(DVBCA_UINT8 *pucBuffer)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucSetRating, 5);
	DVBSTBCA_Memcpy(g_pucCommond + 5, (DVBCA_UINT8 *)pucBuffer, 7);
	g_usCommondLen = 12;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x05)
		{
			g_iResult = DVBCA_PIN_ERROR;			
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardSetWorkTime(DVBCA_UINT8 *pucBuffer)
{	
	DVBCA_UINT8 aucStausWord[2];

	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucSetWorkTime, 5);
	DVBSTBCA_Memcpy(g_pucCommond + 5, (DVBCA_UINT8 *)pucBuffer, 10);
	g_usCommondLen = 15;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x05)
		{
			g_iResult = DVBCA_PIN_ERROR;			
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardGetWorkTime(void)
{	
	DVBCA_UINT8 aucStausWord[2];

	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucGetWorkTime, 5);
	g_usCommondLen = 5;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			DVBSTBCA_Memcpy(g_aucReadBuffer, g_pucReply, 4);
			g_uiReadLen = 4;
			g_iResult = DVBCA_SUCESS;
		}
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardSetStbPair(DVBCA_UINT8 *pucBuffer)
{	
	DVBCA_UINT8 aucStausWord[2];
	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucStbPare, 5);
	DVBSTBCA_Memcpy(g_pucCommond + 5, (DVBCA_UINT8 *)pucBuffer, 10);
	g_usCommondLen = 15;
	if(DVBCA_CardSendToCard((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen,
		(DVBCA_UINT8 *)g_pucReply,&g_usReplyLen, aucStausWord) == DVBCA_SUCESS)
	{
		if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x05)
		{
			g_iResult = DVBCA_PIN_ERROR;			
		}
		else if(aucStausWord[0] == 0x68 && aucStausWord[1] == 0x07)
		{
			g_iResult = DVBCA_STB_PAIR_FAILED;			
		}
		else
		{
			g_iResult = DVBCA_STB_PAIR_FAILED;	
		}
		
	}
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardReadEventInfo(DVBCA_UINT32 uiProviderID)
{
	DVBCA_UINT8 aucStausWord[2];

	g_uiReadLen = 0;
	g_iResult = DVBCA_FAILED; 
	DVBSTBCA_Memcpy(g_pucCommond, (DVBCA_UINT8 *)g_aucEventInfoCmd, 5);
	g_pucCommond[5] = (uiProviderID >> 16 ) &0xFF;
	g_pucCommond[6] = (uiProviderID >> 8 ) &0xFF;
	g_pucCommond[7] = (uiProviderID ) &0xFF;
	g_usCommondLen = 8;
	
	DVBCA_CardGetResponse((DVBCA_UINT8 *)g_pucCommond, g_usCommondLen, g_pucReply, &g_usReplyLen, aucStausWord);
	if(g_iResult != DVBCA_SUCESS || g_uiReadLen != 4)
	{
		goto READ_AUTHORIZE_RTN;
	}
	if(aucStausWord[0] == 0x90 && aucStausWord[1] == 0x00)
	{
		DVBSTBCA_Memcpy(g_aucReadBuffer, g_pucReply,g_uiReadLen);
		g_iResult = DVBCA_SUCESS;
	}
	else
		g_uiReadLen = 0;
	
READ_AUTHORIZE_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiCardSemaphore);
}

static void DVBCA_CardGetResponse(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord)
{
	int iRet = 0;
	if(DVBCA_CardSendToCardNotReset((DVBCA_UINT8 *)pucCommand, usCommandLen, pucReply,
		pusReplyLen, pucStatusWord) == DVBCA_SUCESS)
	{
		if(pucStatusWord[0] == 0x90 && pucStatusWord[1] == 0x00)
		{
			g_iResult = DVBCA_SUCESS;
		}
		else if(pucStatusWord[0] == 0x61)
		{
			DVBSTBCA_Memcpy(pucCommand, (DVBCA_UINT8 *)g_aucResponseCmd, 5);
			pucCommand[4] = pucStatusWord[1];
			usCommandLen = 5;
			
			iRet = DVBCA_CardSendToCardNotReset(pucCommand, usCommandLen, pucReply, 
				pusReplyLen, pucStatusWord);
			if(iRet == DVBCA_SUCESS)
			{
				if( *pusReplyLen < 1100)
				{
					DVBSTBCA_Memcpy(g_aucReadBuffer, pucReply, *pusReplyLen);
					g_uiReadLen = *pusReplyLen;
					g_iResult = DVBCA_SUCESS;
				}
			}
		}
	}
}

static int DVBCA_CardSendToCard(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord)
{
	int i = 0; 
	int iRet = 0;
	
	for(i = 0; i < 3; i++)
	{
		iRet = DVBSTBCA_SCPBRun(pucCommand, usCommandLen, pucReply, pusReplyLen, pucStatusWord);
		if(iRet == DVBCA_SUCESS)
			break;
		if(i == 1)
			DVBCA_CardReset();
	}
	if(i == 3)
		return DVBCA_FAILED;
	else
		return DVBCA_SUCESS;
}

static int DVBCA_CardSendToCardNotReset(DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					DVBCA_UINT8 *pucReply,
					DVBCA_UINT16 *pusReplyLen,
					DVBCA_UINT8 *pucStatusWord)
{
	int i = 0; 
	int iRet = 0;
	
	for(i = 0; i < 3; i++)
	{
		iRet = DVBSTBCA_SCPBRun(pucCommand, usCommandLen, pucReply, pusReplyLen, pucStatusWord);
		if(iRet == DVBCA_SUCESS)
			break;
	}
	if(i == 3)
		return DVBCA_FAILED;
	else
		return DVBCA_SUCESS;
}


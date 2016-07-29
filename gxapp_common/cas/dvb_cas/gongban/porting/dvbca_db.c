#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_kernel.h"
#include "dvbca_db.h"

#define	DVBCA_DB_MAIGC_CODE	0xdead
static ST_CA_DB g_stCaDb;

static void DVBCA_DB_Save(void);
static int DVBCA_DB_Load(void);

static int DVBCA_DB_AddDelMailID(DVBCA_UINT16 usMailID);
static int DVBCA_DB_LoadDelMailID(void);
static int DVBCA_DB_LoadMail(void);
static int DVBCA_DB_LoadNetName(void);
static int DVBCA_DB_LoadFreqVer(void);
static int DVBCA_DB_LoadFaceTime(void);
static int DVBCA_DB_LoadNetwork(void);
static int DVBCA_DB_LoadPromptMsg(void);
static void DVBCA_DB_SavePromptMsg(void);

static DVBCA_SEMAPHORE*g_puiDBSemaphore = NULL;

int  DVBCA_DB_Init(void)
{	
	g_puiDBSemaphore = (DVBCA_SEMAPHORE *)DVBSTBCA_Malloc(sizeof(DVBCA_SEMAPHORE));
	if(g_puiDBSemaphore == NULL)
		return DVBCA_FAILED;
	DVBSTBCA_SemaphoreInit(g_puiDBSemaphore, 0);
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);
	
	if(DVBCA_DB_Load() == CA_DB_REFRESH)
	{
		DVBCA_DB_Save();
	}	
	g_stCaDb.usDate = 0;
	g_stCaDb.usTime = 0;

	return DVBCA_SUCESS;
}

void DVBCA_DB_UnInit(void)
{
	if(g_puiDBSemaphore)
	{
		DVBSTBCA_SemaphoreDestory(g_puiDBSemaphore);
		DVBSTBCA_Free(g_puiDBSemaphore);
		g_puiDBSemaphore = NULL;		
	}
}

void DVBCA_DB_Default(void)
{
	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	DVBSTBCA_FormatBuffer(0, DVBCA_DB_FLASH_SIZE);
	if(DVBCA_DB_Load() == CA_DB_REFRESH)
	{
		DVBCA_DB_Save();
	}	
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);	
}

DVBCA_UINT16 DVBCA_DB_GetMailNum(EN_CA_DB_MAIL_CMD enCmd)
{
	DVBCA_UINT16 i;
	DVBCA_UINT16 usNum = 0;
	
	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	switch(enCmd)
	{
		case CA_DB_MAIL_TOTAL_NUM:
			usNum = g_stCaDb.usMailNum;
			break;
		case CA_DB_MAIL_READ_NUM:
			for(i = 0; i <MAIL_REPEAT_TIMES; i++)
			{
				if(g_stCaDb.astMail[i].usUsedFlag == 0x01)
					usNum++;
			}
			break;
		case CA_DB_MAIL_NOT_READ_NUM:
			for(i = 0; i <MAIL_REPEAT_TIMES; i++)
			{
				if(g_stCaDb.astMail[i].usUsedFlag == 0x03)
					usNum++;
			}
			break;
		default:
			break;
	}
	if(usNum > DVBCA_MAX_MAIL)
		usNum = DVBCA_MAX_MAIL;
	
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);
	return usNum;
}

DVBCA_UINT16 DVBCA_DB_GetMail(OUT ST_DVBCAEmail *pstMail, 
								DVBCA_UINT8 ucStartPos, 
								DVBCA_UINT16 usNum)
{
	DVBCA_UINT16 usRealNum = 0;
	DVBCA_UINT16 usPos = 0;
	int iRepeatTimes;
	

	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	if(pstMail == NULL )
		goto GET_MAIL_RTN;

	if(ucStartPos >= DVBCA_MAX_MAIL)
		goto GET_MAIL_RTN;

	if((ucStartPos + usNum) > DVBCA_MAX_MAIL)
		usRealNum = DVBCA_MAX_MAIL - ucStartPos;
	else
		usRealNum = usNum;

	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES && usPos < (ucStartPos + usRealNum); iRepeatTimes++)
	{
		if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0x00 && g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0xFF)
		{			
			if(usPos >= ucStartPos)
			{				
				DVBSTBCA_Memset((DVBCA_UINT8 *)(&pstMail[usPos - ucStartPos]),0x0, sizeof(ST_DVBCAEmail));
				pstMail[usPos - ucStartPos].usEmailID = g_stCaDb.astMail[iRepeatTimes].usMailID;
				pstMail[usPos - ucStartPos].ucNewEmail = (g_stCaDb.astMail[iRepeatTimes].usUsedFlag >> 1) & 0x01;
				DVBSTBCA_Memcpy(pstMail[usPos - ucStartPos].aucTitle, g_stCaDb.astMail[iRepeatTimes].aucTitle, DVBCA_MAIL_TITLE_SIZE);
				DVBSTBCA_Memcpy(pstMail[usPos - ucStartPos].aucTerm, g_stCaDb.astMail[iRepeatTimes].aucTerm, DVBCA_MAIL_TERM_SIZE);
				DVBSTBCA_Memcpy(pstMail[usPos - ucStartPos].aucContext, g_stCaDb.astMail[iRepeatTimes].aucContext, DVBCA_MAIL_CONTEXT_SIZE);
			}
			usPos++;
		}
	}
GET_MAIL_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);

	return usRealNum;
}

int DVBCA_DB_SetMailRead(DVBCA_UINT16 usMailID)
{	
	int iRepeatTimes;
	int iMailPos = 0;
	int iRet = 0;

	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
	{
		if(usMailID == g_stCaDb.astMail[iRepeatTimes].usMailID && g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0x00
			&& g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0xFF)
			break;
	}
	if(iRepeatTimes == MAIL_REPEAT_TIMES)
	{
		iRet = CA_DB_NOT_EXIST;
		goto SET_MAIL_RTN;
	}

	iMailPos = iRepeatTimes;
	if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0x01)
	{
		//printf("id:0x%x iRepeatTimesus:%d UsedFlag:0x%x in %s\n",g_stCaDb.astMail[iRepeatTimes].usMailID,iRepeatTimes,g_stCaDb.astMail[iRepeatTimes].usUsedFlag,__func__);
		g_stCaDb.astMail[iRepeatTimes].usUsedFlag = 0x01;
		
		DVBSTBCA_WriteBuffer(DVBCA_DB_MAIL_OFFSET + iRepeatTimes * sizeof(ST_CA_MAIL), 
					(DVBCA_UINT8 *)(&(g_stCaDb.astMail[iRepeatTimes])), sizeof(ST_CA_MAIL));
	}	
	iRet = CA_DB_SUCCESS;
SET_MAIL_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);

	return iRet;	
}

int DVBCA_DB_DelMail(DVBCA_UINT16 usMailID)
{	
	int iRepeatTimes;
	int iRet = 0;

	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
	{
		if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0 
			&& g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0xFF
			&& usMailID == g_stCaDb.astMail[iRepeatTimes].usMailID)
			break;
	}
	if(iRepeatTimes == MAIL_REPEAT_TIMES)
	{
		iRet = CA_DB_NOT_EXIST;
		goto DEL_MAIL_RTN;
	}

	if(DVBCA_DB_AddDelMailID(usMailID) >= 0)
	{
		//printf("id:0x%x iRepeatTimesus:%d UsedFlag:0x%x in %s\n",g_stCaDb.astMail[iRepeatTimes].usMailID,iRepeatTimes,g_stCaDb.astMail[iRepeatTimes].usUsedFlag,__func__);
		g_stCaDb.astMail[iRepeatTimes].usUsedFlag = 0;
		
		DVBSTBCA_WriteBuffer(DVBCA_DB_MAIL_OFFSET + iRepeatTimes * sizeof(ST_CA_MAIL), 
					(DVBCA_UINT8 *)(&(g_stCaDb.astMail[iRepeatTimes])), sizeof(ST_CA_MAIL));

		g_stCaDb.usMailNum--;
		

		iRet = CA_DB_SUCCESS;
	}
	else
	{
		//printf("CA_DB_FULL in %s!\n",__func__);
		iRet = CA_DB_FULL;
	}
DEL_MAIL_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);

	return iRet;	
}

int DVBCA_DB_AddMail(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	DVBCA_UINT16 usMailID = 0;
	DVBCA_UINT16 usConLen = 0;
	DVBCA_UINT32 uiAreaNo = 0;
	DVBCA_UINT32 uiCardAreaNo = 0;
	int iRepeatTimes;
	int iNewPos = -1;
	int iRet = 0;

	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	
	if(pucBuffer == NULL)
	{
		iRet = CA_DB_NULL_PTR;
		goto ADD_MAIL_RTN;
	}

	uiCardAreaNo = DVBCA_GetAreaNo();
	uiAreaNo = (pucBuffer[10] << 24) | (pucBuffer[11] << 16)  | (pucBuffer[12] << 8) | pucBuffer[13];
	//if(uiCardAreaNo != 0xFFFFFFFF && uiAreaNo != 0 && uiAreaNo != uiCardAreaNo)
	if(DVBCA_ChechAreaNo(uiCardAreaNo, uiAreaNo) == 0x00)
	{
		iRet = CA_DB_FAILED;
		goto ADD_MAIL_RTN;
	}
	usMailID = (pucBuffer[14] << 6) | (pucBuffer[15] >> 2);
	//printf("get mail ID:0x%x in %s\n",usMailID,__func__);
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
	{
		if(usMailID == g_stCaDb.astMail[iRepeatTimes].usMailID)
		{
			iRet = CA_DB_EXIST;
			goto ADD_MAIL_RTN;
		}
	}
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES * 2; iRepeatTimes++)
	{
		if(usMailID == g_stCaDb.ausDelMailID[iRepeatTimes])
		{
			iRet = CA_DB_EXIST;
			goto ADD_MAIL_RTN;
		}
	}
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
	{
		if(0xFFFF == g_stCaDb.astMail[iRepeatTimes].usMailID)
		{
			iNewPos = iRepeatTimes;
			break;
		}
	}
	if(iNewPos < 0)
	{
		iRet = CA_DB_FULL;
		goto ADD_MAIL_RTN;
	}
	if(g_stCaDb.usMailNum >= DVBCA_MAX_MAIL)
	{
		int iUnReadPos = -1;
		int iReadPos = -1;
		iRet = -1;

		for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
		{
			if(iReadPos == -1 && g_stCaDb.astMail[iRepeatTimes].usUsedFlag == 0x01)
			{
				iReadPos = iRepeatTimes;
				break;
			}
			else if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0 && 
				g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0xFF && iUnReadPos == -1)
				iUnReadPos = iRepeatTimes;			
		}
		DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);
		if(iReadPos >= 0)
			iRet = DVBCASTB_DelMail(g_stCaDb.astMail[iReadPos].usMailID);
		else if(iUnReadPos >= 0)
			iRet = DVBCASTB_DelMail(g_stCaDb.astMail[iUnReadPos].usMailID);
		if(iRet < 0)
			return iRet;
	}
	if(pucBuffer[15] & 0x03)
	{
		DVBCA_ParsePSW(pucBuffer + 16, usLen - 20, pucBuffer + 6, 10);
	}
	DVBSTBCA_Memset((DVBCA_UINT8 *)&(g_stCaDb.astMail[iNewPos]), 0x0, sizeof(ST_CA_MAIL));
	g_stCaDb.astMail[iNewPos].usUsedFlag = 0x03;
	g_stCaDb.astMail[iNewPos].usMailID = usMailID;
	DVBSTBCA_Memcpy(g_stCaDb.astMail[iNewPos].aucTitle, pucBuffer + 16, DVBCA_MAIL_TITLE_SIZE);
	DVBSTBCA_Memcpy(g_stCaDb.astMail[iNewPos].aucTerm, pucBuffer + 16 + DVBCA_MAIL_TITLE_SIZE, DVBCA_MAIL_TERM_SIZE);
	usConLen = usLen - 20 - DVBCA_MAIL_TITLE_SIZE - DVBCA_MAIL_TERM_SIZE;
	if(usConLen < DVBCA_MAIL_CONTEXT_SIZE)
		DVBSTBCA_Memcpy(g_stCaDb.astMail[iNewPos].aucContext, pucBuffer + 16 + DVBCA_MAIL_TITLE_SIZE + DVBCA_MAIL_TERM_SIZE, usConLen);
	DVBSTBCA_WriteBuffer(DVBCA_DB_MAIL_OFFSET + iNewPos * sizeof(ST_CA_MAIL), 
				(DVBCA_UINT8 *)(&(g_stCaDb.astMail[iNewPos])), sizeof(ST_CA_MAIL));
	g_stCaDb.usMailNum++;
	//printf("add mail ID:0x%x in %s usMailNum:%d iNewPos:%d\n",usMailID,__func__,g_stCaDb.usMailNum,iNewPos);
	iRet = CA_DB_SUCCESS;

ADD_MAIL_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);

	return iRet;	
}

int DVBCA_DB_AddPromptMsg(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen)
{
	int iRet = 0;
	DVBCA_UINT8 ucVersion = 0;
	DVBCA_UINT8 usTag = 0;
	DVBCA_UINT16 usPromptLen = 0;
	DVBCA_UINT16 usOffset = 0;
	

	DVBSTBCA_SemaphoreWait(g_puiDBSemaphore);
	
	if(pucBuffer == NULL || usLen == 0)
	{
		iRet = CA_DB_NULL_PTR;
		goto ADD_PROMPT_RTN;
	}

	ucVersion = (pucBuffer[10] >> 2);
	if(g_stCaDb.stPrompt.ucVersion == ucVersion)
	{
		iRet = CA_DB_EXIST;
		goto ADD_PROMPT_RTN;
	}
		
	if(pucBuffer[10] &0x3)
	{
		DVBCA_ParsePSW(pucBuffer + 11, usLen-15, pucBuffer + 6, 5);
	}

	usOffset = 11;
	while(usOffset + 4 < usLen)
	{
		usTag = pucBuffer[usOffset];
		usPromptLen = (pucBuffer[usOffset + 1] << 8) | pucBuffer[usOffset + 2];
		if(usTag == DVBCA_DB_PROMPT_TAG)
		{
			break;
		}
		usOffset += (usPromptLen + 3);
	}
	if(usOffset + 4 >= usLen)
	{
		iRet = CA_DB_NOT_EXIST;
		goto ADD_PROMPT_RTN;
	}
	if((g_stCaDb.stPrompt.usOffset + (usPromptLen + 3)) > DVBCA_DB_PROMPT_SIZE)
	{
		iRet = CA_DB_FULL;
		goto ADD_PROMPT_RTN;
	}
	g_stCaDb.stPrompt.ucVersion = ucVersion;
	if(g_stCaDb.stPrompt.usOffset == 0 && g_stCaDb.stPrompt.usLength == 0)
		g_stCaDb.stPrompt.usOffset = 0;
	else
		g_stCaDb.stPrompt.usOffset = g_stCaDb.stPrompt.usOffset + g_stCaDb.stPrompt.usLength + 3;
	g_stCaDb.stPrompt.usLength = usPromptLen;

	g_stCaDb.aucPromptData[g_stCaDb.stPrompt.usOffset] = g_stCaDb.stPrompt.ucVersion;
	g_stCaDb.aucPromptData[g_stCaDb.stPrompt.usOffset + 1] = (g_stCaDb.stPrompt.usLength >> 8) & 0xFF;
	g_stCaDb.aucPromptData[g_stCaDb.stPrompt.usOffset + 2] = g_stCaDb.stPrompt.usLength & 0xFF;

	DVBSTBCA_Memcpy(g_stCaDb.aucPromptData + g_stCaDb.stPrompt.usOffset + 3,
			pucBuffer + usOffset + 3,
			g_stCaDb.stPrompt.usLength);
	DVBSTBCA_WriteBuffer(DVBCA_DB_PROMPT_OFFSET + g_stCaDb.stPrompt.usOffset, 
			g_stCaDb.aucPromptData + g_stCaDb.stPrompt.usOffset,
			g_stCaDb.stPrompt.usLength + 3);
	iRet = CA_DB_SUCCESS;

ADD_PROMPT_RTN:
	DVBSTBCA_SemaphoreSignal(g_puiDBSemaphore);

	return iRet;
}

int DVBCA_DB_GetPromptMsg(DVBCA_UINT8 ucPromptID, DVBCA_UINT8 ucLanguage, DVBCA_UINT8 *pucText)
{

	DVBCA_UINT16 usOffset = 0;
	DVBCA_UINT8 *pucBuffer = NULL;

	if(g_stCaDb.stPrompt.ucVersion == 0xFF)
	{
		return CA_DB_NOT_EXIST;
	}
	pucBuffer = g_stCaDb.aucPromptData + g_stCaDb.stPrompt.usOffset + 3;
	while(usOffset < g_stCaDb.stPrompt.usLength)
	{
		if(ucPromptID == ((pucBuffer[usOffset] >> 1) & 0x7F)
				&& ucLanguage == (pucBuffer[usOffset] & 0x01))
			break;
		usOffset += (pucBuffer[usOffset + 1] + 2);
	}
	if(usOffset >= g_stCaDb.stPrompt.usLength)
	{
		return CA_DB_NOT_EXIST;
	}
	
	DVBSTBCA_Memcpy(pucText, pucBuffer + usOffset + 2, pucBuffer[usOffset + 1]);
	pucText[ pucBuffer[usOffset + 1]] = 0;

	return CA_DB_SUCCESS;
}

int DVBCA_DB_SetFreqVer(DVBCA_UINT8 ucFreqVer)
{
	int iResult = 0;

	DVBCA_UINT8 ucLastFreqVer = 0xFF;
	int iPos;
	if(ucFreqVer == 0xFF || ucFreqVer == g_stCaDb.ucFreqVer)
		return iResult;

	
	g_stCaDb.ucFreqVer = ucFreqVer;
	for(iPos = 0; iPos < DVBCA_DB_FREQVER_TIMES; iPos++)
	{
		DVBCA_UINT8 ucTempFreqVer = 0xFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_FREQVER_OFFSET + iPos *sizeof(DVBCA_UINT8) ,&ucTempFreqVer, sizeof(DVBCA_UINT8));
		if(ucTempFreqVer == 0xFF)
			break;
		ucLastFreqVer = ucTempFreqVer;
	}

	if(iPos < DVBCA_DB_FREQVER_TIMES)
	{
		if(iPos > 0 && ((ucLastFreqVer & ucFreqVer) == ucFreqVer))
			iPos--;
		DVBSTBCA_WriteBuffer(DVBCA_DB_FREQVER_OFFSET + iPos *sizeof(DVBCA_UINT8) ,(DVBCA_UINT8 *)&ucFreqVer, sizeof(DVBCA_UINT8));
	}
	
	iResult = 1;

	return iResult;
}
DVBCA_UINT8 DVBCA_DB_GetFreqVer(void)
{

	return g_stCaDb.ucFreqVer;

}
void DVBCA_DB_SetFaceTime(DVBCA_UINT8 ucFaceTime)
{

	DVBCA_UINT8 ucLastFaceTime = 0xFF;
	int iPos;
	
	if(ucFaceTime == 0xFF || ucFaceTime == g_stCaDb.ucFaceTime)
		return;

	g_stCaDb.ucFaceTime = ucFaceTime;
	
	for(iPos = 0; iPos < DVBCA_DB_FACETIME_TIMES; iPos++)
	{
		DVBCA_UINT8 ucTempFaceTime = 0xFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_FACETIME_OFFSET+ iPos *sizeof(DVBCA_UINT8) ,&ucTempFaceTime, sizeof(DVBCA_UINT8));
		if(ucTempFaceTime == 0xFF)
			break;
		ucLastFaceTime = ucTempFaceTime;
	}

	if(iPos < DVBCA_DB_FACETIME_TIMES)
	{
		if(iPos > 0 && ((ucLastFaceTime & ucFaceTime) == ucFaceTime))
			iPos--;
		DVBSTBCA_WriteBuffer(DVBCA_DB_FACETIME_OFFSET + iPos *sizeof(DVBCA_UINT8) ,(DVBCA_UINT8 *)&ucFaceTime, sizeof(DVBCA_UINT8));
	}

}

DVBCA_UINT8 DVBCA_DB_GetFaceTime(void)
{

	return g_stCaDb.ucFaceTime;

}

void DVBCA_DB_SetNetName(DVBCA_UINT8 *pucNetName)
{

	int iPos;
	for(iPos = 0; iPos < DVBCA_NET_NAME_LEN;iPos++)
	{
		if(g_stCaDb.aucNetName[iPos] != pucNetName[iPos])
			break;
	}
	if(iPos == DVBCA_NET_NAME_LEN)
		return;

	DVBSTBCA_Memset(g_stCaDb.aucNetName, 0x0, DVBCA_NET_NAME_LEN + 1);
	DVBSTBCA_Memcpy(g_stCaDb.aucNetName, pucNetName, DVBCA_NET_NAME_LEN);
	
	for(iPos = 0; iPos < DVBCA_DB_NETNAME_TIMES; iPos++)
	{
		DVBCA_UINT8 aucNetName[DVBCA_NET_NAME_LEN];
		DVBSTBCA_ReadBuffer(DVBCA_DB_NETNAME_OFFSET+ iPos * DVBCA_NET_NAME_LEN,aucNetName, DVBCA_NET_NAME_LEN);
		if(aucNetName[0] == 0xFF)
			break;
	}

	if(iPos < DVBCA_DB_NETNAME_TIMES)
	{
		DVBSTBCA_WriteBuffer(DVBCA_DB_NETNAME_OFFSET+ iPos * DVBCA_NET_NAME_LEN ,g_stCaDb.aucNetName, DVBCA_NET_NAME_LEN);
	}

}

void DVBCA_DB_GetNetName(DVBCA_UINT8 *pucNetName)
{

	DVBSTBCA_Memset(pucNetName, 0x0, DVBCA_NET_NAME_LEN);
	DVBSTBCA_Memcpy(pucNetName, g_stCaDb.aucNetName,DVBCA_NET_NAME_LEN);

}

DVBCA_UINT32 DVBCA_DB_GetNetwork(void)
{
	return g_stCaDb.uiNetworkID;
}

int DVBCA_DB_SetNetwork(DVBCA_UINT32 uiNetworkID)
{
	int iResult = 0;
	int iPos;
	DVBCA_UINT32 uiLastNetworkID = 0xFFFFFFFF;
	if(uiNetworkID == 0xFFFFFFFF || uiNetworkID == g_stCaDb.uiNetworkID)
		return iResult;

	g_stCaDb.uiNetworkID = uiNetworkID;
	
	for(iPos = 0; iPos < DVBCA_DB_NETWORK_TIMES; iPos++)
	{
		DVBCA_UINT32 uiTempNetworkID = 0xFFFFFFFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_NETWORK_OFFSET + iPos *sizeof(DVBCA_UINT32) ,(DVBCA_UINT8 *)&uiTempNetworkID, sizeof(DVBCA_UINT32));
		if(uiTempNetworkID == 0xFFFFFFFF)
			break;
		uiLastNetworkID = uiTempNetworkID;
	}

	if(iPos < DVBCA_DB_NETWORK_TIMES)
	{
		if(iPos > 0 && ((uiLastNetworkID & uiNetworkID) == uiNetworkID))
			iPos--;
		DVBSTBCA_WriteBuffer(DVBCA_DB_NETWORK_OFFSET + iPos *sizeof(DVBCA_UINT32) ,(DVBCA_UINT8 *)&uiNetworkID, sizeof(DVBCA_UINT32));
	}
	
	return iResult;
}

int DVBCA_DB_Get(EN_CA_DB_CMD enCmd, void *pValue)
{
	if(pValue == NULL)
		return CA_DB_NULL_PTR;

	switch(enCmd)
	{
		case CA_DB_CMD_DATE:
			*(DVBCA_UINT16 *)(pValue) = g_stCaDb.usDate;
			break;
		case CA_DB_CMD_TIME:
			*(DVBCA_UINT16 *)(pValue) = g_stCaDb.usTime;
			break;
		default:
			return CA_DB_UNKNOW_CMD;
	}
	
	return CA_DB_SUCCESS;
}

int DVBCA_DB_Set(EN_CA_DB_CMD enCmd, DVBCA_UINT32 uiValue)
{

	switch(enCmd)
	{
		case CA_DB_CMD_DATE:
			if(g_stCaDb.usDate == uiValue)
				break;
			g_stCaDb.usDate = uiValue;
			break;
		case CA_DB_CMD_TIME:
			if(g_stCaDb.usTime == uiValue)
				break;
			g_stCaDb.usTime = uiValue;
			break;
		default:
			return CA_DB_UNKNOW_CMD;
	}
	
	return CA_DB_SUCCESS;
}

static void DVBCA_DB_Save(void)
{
	int iRepeatTimes;
	int iPos = 0;

	DVBSTBCA_FormatBuffer(0, DVBCA_DB_FLASH_SIZE);

	DVBSTBCA_WriteBuffer(DVBCA_DB_HEAD_OFFSET, (DVBCA_UINT8 *)(&g_stCaDb.uiMagic), DVBCA_DB_HEAD_SIZE);


	iPos = 0;
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES; iRepeatTimes++)
	{
		if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag == 0x03 || g_stCaDb.astMail[iRepeatTimes].usUsedFlag == 0x01)
		{
			DVBSTBCA_WriteBuffer(DVBCA_DB_MAIL_OFFSET + iPos * sizeof(ST_CA_MAIL),
				(DVBCA_UINT8 *)(&(g_stCaDb.astMail[iRepeatTimes])), sizeof(ST_CA_MAIL));
			iPos++;
		}
	}
	DVBCA_DB_LoadMail();
	//DVBSTBCA_Printf("in %s DVBCA_DB_LoadMail:%x\n",__func__,g_stCaDb.usMailNum);
	iPos = 0;
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES*2; iRepeatTimes++)
	{
		if(g_stCaDb.ausDelMailID[iRepeatTimes] != 0xFFFF)
		{
			DVBSTBCA_WriteBuffer(DVBCA_DB_DELMAIL_OFFSET + iPos * sizeof(DVBCA_UINT16),
				(DVBCA_UINT8 *)(&(g_stCaDb.ausDelMailID[iRepeatTimes])), sizeof(DVBCA_UINT16));
			iPos++;
		}
	}
	DVBCA_DB_LoadDelMailID();
	//DVBSTBCA_Printf("in %s DVBCA_DB_LoadDelMailID:%x\n",__func__,g_stCaDb.usDelMailNum);
	DVBSTBCA_WriteBuffer(DVBCA_DB_FACETIME_OFFSET,&g_stCaDb.ucFaceTime, 1);
	DVBSTBCA_WriteBuffer(DVBCA_DB_FREQVER_OFFSET,&g_stCaDb.ucFreqVer, 1);
	DVBSTBCA_WriteBuffer(DVBCA_DB_NETNAME_OFFSET,g_stCaDb.aucNetName, DVBCA_NET_NAME_LEN);

	if(g_stCaDb.uiNetworkID != 0xFFFFFFFF)
		DVBSTBCA_WriteBuffer(DVBCA_DB_NETWORK_OFFSET,(DVBCA_UINT8 *)&g_stCaDb.uiNetworkID, sizeof(DVBCA_UINT32));


	DVBCA_DB_SavePromptMsg();

}


static int DVBCA_DB_AddDelMailID(DVBCA_UINT16 usMailID)
{
	int iRepeatTimes;
	for(iRepeatTimes = 0; iRepeatTimes < MAIL_REPEAT_TIMES * 2; iRepeatTimes++)
	{
		if(g_stCaDb.ausDelMailID[iRepeatTimes] == 0xFFFF)
			break;		
	}
	if(iRepeatTimes == MAIL_REPEAT_TIMES* 2)
	{
		return CA_DB_FULL;
	}
	else
	{
		g_stCaDb.ausDelMailID[iRepeatTimes] = usMailID;
		DVBSTBCA_WriteBuffer(DVBCA_DB_DELMAIL_OFFSET + iRepeatTimes * sizeof(DVBCA_UINT16), (DVBCA_UINT8 *)(&usMailID), sizeof(DVBCA_UINT16));
		return CA_DB_SUCCESS;
	}
}

static int DVBCA_DB_Load(void)
{
	int iResult = CA_DB_SUCCESS;
	int iReturn;
	
	DVBSTBCA_Memset((DVBCA_UINT8 *)(&g_stCaDb), 0x0, sizeof(ST_CA_DB));

	//read magic code
	DVBSTBCA_ReadBuffer(DVBCA_DB_HEAD_OFFSET, (DVBCA_UINT8 *)(&g_stCaDb.uiMagic), DVBCA_DB_HEAD_SIZE);

	//magic code is error
	if(g_stCaDb.uiMagic != DVBCA_DB_MAIGC_CODE)
	{
		DVBSTBCA_Printf("Magic code ERRO!\n");
		DVBSTBCA_FormatBuffer(0, DVBCA_DB_FLASH_SIZE);
		g_stCaDb.uiMagic = DVBCA_DB_MAIGC_CODE;
		DVBSTBCA_WriteBuffer(DVBCA_DB_HEAD_OFFSET, (DVBCA_UINT8 *)(&g_stCaDb.uiMagic), DVBCA_DB_HEAD_SIZE);
	}


	iReturn = DVBCA_DB_LoadMail();
	DVBSTBCA_Printf("DVBCA_DB_LoadMail:%d:%x\n",iReturn,g_stCaDb.usMailNum);
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;
	iReturn = DVBCA_DB_LoadDelMailID();
	DVBSTBCA_Printf("DVBCA_DB_LoadDelMailID:%d:%x\n",iReturn,g_stCaDb.usDelMailNum);
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;
	iReturn = DVBCA_DB_LoadFreqVer();
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;
	iReturn = DVBCA_DB_LoadFaceTime();
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;
	iReturn = DVBCA_DB_LoadNetName();
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;

	iReturn = DVBCA_DB_LoadNetwork();
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;


	iReturn = DVBCA_DB_LoadPromptMsg();
	if(iReturn != CA_DB_SUCCESS)
		iResult = iReturn;

	DVBSTBCA_Printf("iResult:%d\n",iResult);
	return iResult;
}


static int DVBCA_DB_LoadDelMailID(void)
{
	int iRepeatTimes;
	int iEndPos = -1;
	int iResult = CA_DB_SUCCESS;
	
	DVBSTBCA_Memset((DVBCA_UINT8 *)(g_stCaDb.ausDelMailID), 0xFF, MAIL_REPEAT_TIMES*2* sizeof(DVBCA_UINT16));
	g_stCaDb.usDelMailNum = 0;

	DVBSTBCA_ReadBuffer(DVBCA_DB_DELMAIL_OFFSET, 
		(DVBCA_UINT8 *)((g_stCaDb.ausDelMailID)), sizeof(DVBCA_UINT16) * MAIL_REPEAT_TIMES * 2);

	for(iRepeatTimes = (MAIL_REPEAT_TIMES*2 - 1); iRepeatTimes >= 0; iRepeatTimes--)
	{
		if(g_stCaDb.ausDelMailID[iRepeatTimes] != 0xFFFF)
		{
			if(iEndPos == -1)
				iEndPos = iRepeatTimes;
			if(g_stCaDb.usDelMailNum < MAIL_REPEAT_TIMES)
				g_stCaDb.usDelMailNum++;
			else
			{
				g_stCaDb.ausDelMailID[iRepeatTimes] = 0xFFFF;
				iResult = CA_DB_REFRESH;
			}
		}
	}

	if(iEndPos >= 0 && iEndPos > (((MAIL_REPEAT_TIMES) >> 1) + MAIL_REPEAT_TIMES))
		iResult = CA_DB_REFRESH;

	return iResult;
}

static int DVBCA_DB_LoadMail(void)
{
	int iRepeatTimes;
	int iEndPos = -1;
	int iResult = CA_DB_SUCCESS;
	
	DVBSTBCA_Memset((DVBCA_UINT8 *)(g_stCaDb.astMail), 0xFF, MAIL_REPEAT_TIMES * sizeof(ST_CA_MAIL));
	g_stCaDb.usMailNum = 0;

	DVBSTBCA_ReadBuffer(DVBCA_DB_MAIL_OFFSET , 
			(DVBCA_UINT8 *)(&(g_stCaDb.astMail)), sizeof(ST_CA_MAIL) * MAIL_REPEAT_TIMES);
	for(iRepeatTimes = MAIL_REPEAT_TIMES - 1; iRepeatTimes >= 0; iRepeatTimes--)
	{
		if(g_stCaDb.astMail[iRepeatTimes].usMailID != 0xFFFF)
		{
			if(iEndPos == -1)
				iEndPos = iRepeatTimes;
			if(g_stCaDb.astMail[iRepeatTimes].usUsedFlag != 0)
			{
				if(g_stCaDb.usMailNum < DVBCA_MAX_MAIL)
					g_stCaDb.usMailNum++;
				else
				{
					g_stCaDb.astMail[iRepeatTimes].usUsedFlag = 0;
					iResult = CA_DB_REFRESH;
				}
			}
		}
	}
	if(iEndPos >= 0 && iEndPos > (((MAIL_REPEAT_TIMES - DVBCA_MAX_MAIL) >> 1) + DVBCA_MAX_MAIL))
		iResult = CA_DB_REFRESH;

	return iResult;
}


static int DVBCA_DB_LoadNetName(void)
{
	int iPos = 0;
	int iResult = 0;

	DVBSTBCA_Memset(g_stCaDb.aucNetName, 0x0, DVBCA_NET_NAME_LEN + 1);

	for(iPos = 0 ; iPos < DVBCA_DB_NETNAME_TIMES; iPos++)
	{
		DVBCA_UINT8 aucNetName[DVBCA_NET_NAME_LEN];
		DVBSTBCA_ReadBuffer(DVBCA_DB_NETNAME_OFFSET+ iPos * DVBCA_NET_NAME_LEN,aucNetName, DVBCA_NET_NAME_LEN);
		if(aucNetName[0] == 0xFF)
			break;
		DVBSTBCA_Memset(g_stCaDb.aucNetName, 0x0, DVBCA_NET_NAME_LEN + 1);
		DVBSTBCA_Memcpy(g_stCaDb.aucNetName, aucNetName, DVBCA_NET_NAME_LEN);
	}
	
	if(iPos >= ((DVBCA_DB_NETNAME_TIMES * 2) / 3))
	{
		iResult = CA_DB_REFRESH;
	}
	return iResult;
}

static int DVBCA_DB_LoadFreqVer(void)
{
	int iPos = 0;
	int iResult = 0;

	g_stCaDb.ucFreqVer= 0xFF;

	for(iPos = 0 ; iPos < DVBCA_DB_FREQVER_TIMES; iPos++)
	{
		DVBCA_UINT8 ucFreqVer = 0xFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_FREQVER_OFFSET+ iPos,&ucFreqVer, 1);
		if(ucFreqVer == 0xFF)
			break;
		g_stCaDb.ucFreqVer = ucFreqVer;
	}
	
	if(iPos >= ((DVBCA_DB_FREQVER_TIMES * 2) / 3))
	{
		iResult = CA_DB_REFRESH;
	}
	return iResult;
}

static int DVBCA_DB_LoadFaceTime(void)
{
	int iPos = 0;
	int iResult = 0;

	g_stCaDb.ucFaceTime= 0x00;

	for(iPos = 0 ; iPos < DVBCA_DB_FACETIME_TIMES; iPos++)
	{
		DVBCA_UINT8 ucFaceTime = 0xFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_FACETIME_OFFSET + iPos,&ucFaceTime, 1);
		if(ucFaceTime == 0xFF)
			break;
		g_stCaDb.ucFaceTime = ucFaceTime;
	}
	
	if(iPos >= ((DVBCA_DB_FACETIME_TIMES * 2) / 3))
	{
		iResult = CA_DB_REFRESH;
	}
	return iResult;
}


static int DVBCA_DB_LoadNetwork(void)
{
	int iPos = 0;
	int iResult = 0;

	g_stCaDb.uiNetworkID = 0xFFFFFFFF;

	for(iPos = 0 ; iPos < DVBCA_DB_NETWORK_TIMES; iPos++)
	{
		DVBCA_UINT32 uiNetworkID = 0xFFFFFFFF;
		DVBSTBCA_ReadBuffer(DVBCA_DB_NETWORK_OFFSET + iPos *sizeof(DVBCA_UINT32) ,(DVBCA_UINT8 *)&uiNetworkID, sizeof(DVBCA_UINT32));
		if(uiNetworkID == 0xFFFFFFFF)
			break;
		g_stCaDb.uiNetworkID = uiNetworkID;
	}
	
	if(iPos >= ((DVBCA_DB_NETWORK_TIMES * 2) / 3))
	{
		iResult = CA_DB_REFRESH;
	}

	return iResult;
}


static int DVBCA_DB_LoadPromptMsg(void)
{
	int iResult = 0;
	DVBCA_UINT8 ucVersion;
	DVBCA_UINT16 usOffset = 0;

	DVBSTBCA_ReadBuffer(DVBCA_DB_PROMPT_OFFSET, g_stCaDb.aucPromptData, DVBCA_DB_PROMPT_SIZE);

	g_stCaDb.stPrompt.ucVersion = 0xFF;
	g_stCaDb.stPrompt.usLength = 0;
	g_stCaDb.stPrompt.usOffset = 0;
	do
	{
		ucVersion = g_stCaDb.aucPromptData[usOffset];
		if(ucVersion == 0xFF)
			break;
		g_stCaDb.stPrompt.usLength = (g_stCaDb.aucPromptData[usOffset + 1] << 8) | g_stCaDb.aucPromptData[usOffset + 2];
		g_stCaDb.stPrompt.ucVersion = ucVersion;
		g_stCaDb.stPrompt.usOffset = usOffset;
		usOffset += (g_stCaDb.stPrompt.usLength + 3);
	}while(usOffset < DVBCA_DB_PROMPT_SIZE);

	if(g_stCaDb.stPrompt.usOffset != 0 || usOffset >= DVBCA_DB_PROMPT_SIZE)
	{
		if(usOffset >= DVBCA_DB_PROMPT_SIZE)
		{
			DVBSTBCA_Memset(g_stCaDb.aucPromptData, 0xFF, DVBCA_DB_PROMPT_SIZE);
			g_stCaDb.stPrompt.ucVersion = 0xFF;
			g_stCaDb.stPrompt.usLength = 0;
			g_stCaDb.stPrompt.usOffset = 0;
		}
		iResult = CA_DB_REFRESH;
	}
	return iResult;
}

static void DVBCA_DB_SavePromptMsg(void)
{
	if(g_stCaDb.stPrompt.ucVersion == 0xFF)
		return;
	
	DVBSTBCA_WriteBuffer(DVBCA_DB_PROMPT_OFFSET, g_stCaDb.aucPromptData + g_stCaDb.stPrompt.usOffset, g_stCaDb.stPrompt.usLength + 3);
	if(g_stCaDb.stPrompt.usOffset != 0)
	{
		DVBCA_DB_LoadPromptMsg();
	}
}



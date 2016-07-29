#ifndef _DVBCA_KERNEL_H_
#define _DVBCA_KERNEL_H_

typedef enum 
{
	CA_MOD_COMMON = 20,	
}EN_CA_INTEGRATOR;

#define ENABLE_INTERATOR_COMMON

#define CA_LIMIT_TIME	0xFFFFFFFF

#define DVBCA_CAS_ID		0x1703  //0x4b07

#define ECM_TYPE_CTRL_INFO (0x01)
#define ECM_TYPE_CW_INFO (0x02)
#define ECM_TYPE_EERO_TYPE (0x03)
#define ECM_TYPE_DATE	(0x4)
typedef enum
{
	DVBCA_ECM_DATA_INIT,
	DVBCA_ECM_DATA_INPUT,
	DVBCA_ECM_DATA_SEND,
	DVBCA_ECM_DATA_SET,
}DVBCA_ECM_DATA_STATUS;

typedef enum
{
	DVBCA_EMM_DATA_INIT,
	DVBCA_EMM_DATA_INPUT,
	DVBCA_EMM_DATA_SEND,
}DVBCA_EMM_DATA_STATUS;

typedef enum
{
	DVBCA_EMM_NETPARA,
	DVBCA_EMM_EMM,
	DVBCA_EMM_MAIL,
	DVBCA_EMM_MSG,
	DVBCA_EMM_PRODUCT,
	DVBCA_EMM_URGENT_NOTIFY = 0x0d,
	DVBCA_EMM_ADVERTIZE  = 0x0c,
	DVBCA_EMM_PROMPT = 0x11,
	DVBCA_EMM_EVENT = 0x12,
	DVBCA_EMM_CMD = 0x13,
}DVBCA_EMM_TYPE;

typedef struct _ST_ECM
{
	DVBCA_UINT8 aucBuffer[1024];
	DVBCA_UINT8 ucSetFilter;
	DVBCA_UINT8 ucStatus;
	DVBCA_UINT8 ucTabledID;
	DVBCA_UINT8 ucCtrl;
	DVBCA_UINT8 ucErrorType;
	DVBCA_UINT8 ucCwNum;
	DVBCA_UINT8 aucCw[3][8];
	DVBCA_UINT16 usPid;
	DVBCA_UINT32 uiCurTime;
	DVBCA_UINT32 uiExpiredTime;
	DVBCA_UINT32 uiCrc;
	ST_DVBCAFilter stFilter;
	DVBCA_SEMAPHORE *pEcmSemaphore;
}ST_ECM;

typedef struct _ST_EMM
{
	DVBCA_UINT8 ucEmmStatus;
	DVBCA_UINT8 ucEventStatus;
	DVBCA_UINT8 ucCmdStatus;
	DVBCA_UINT8 ucMaxSec;
	DVBCA_UINT8 ucRecvSec;
	DVBCA_UINT8 ucEmmType;
	DVBCA_UINT8 aucEmmBuffer[1100];
	DVBCA_UINT8 aucEventBuffer[1100];
	DVBCA_UINT8 aucCmdBuffer[256];
	DVBCA_UINT8 aucDataBuffer[10*1024];
	DVBCA_UINT16 usPid;
	DVBCA_UINT16 usRecvLen;
	DVBCA_UINT32 uiEmmCrc;
	DVBCA_UINT32 uiEventCrc;
	ST_DVBCAFilter stFilter[5];	
	DVBCA_SEMAPHORE *pEmmSemaphore;
}ST_EMM;

typedef struct _ST_CAKERL
{
	DVBCA_UINT8 aucCardID[8];
	DVBCA_UINT8 aucStbID[4];
	DVBCA_UINT8 aucBCTableid[20];
	DVBCA_UINT8 ucBCFreqSum;
	DVBCA_UINT16 usCasID;
	DVBCA_UINT16 usNetworkID;
	DVBCA_UINT16 ausBCPid[20];
	DVBCA_UINT32 uiAreaID;
}ST_CAKERL;

typedef struct _ST_CA_MSG
{
	DVBCA_UINT8 ucUsed;
	DVBCA_UINT8 ucDispPos;
	DVBCA_UINT8 ucDispWay;
	DVBCA_UINT8 ucDispCount;
	DVBCA_UINT8 ucDispDirection;
	DVBCA_UINT8 ucIntervalTime;
	DVBCA_UINT8 ucBgColor;
	DVBCA_UINT8 ucFontColor;
	DVBCA_UINT8 aucText[DVBCA_MAXLEN_MSG_TEXT];
	DVBCA_UINT32 uiVer;
	DVBCA_UINT32 uiCardID;
	DVBCA_UINT8 ucBgRedColor;
	DVBCA_UINT8 ucBgGreenColor;
	DVBCA_UINT8 ucBgBlueColor;
	DVBCA_UINT8 ucFontRedColor;
	DVBCA_UINT8 ucFontGreenColor;
	DVBCA_UINT8 ucFontBlueColor;
}ST_CA_MSG;

int 	DVBCA_InitKernel(DVBCA_UINT8 *pucStbId); 
int 	DVBCA_CheckCasID(DVBCA_UINT16 usCasID);
void DVBCA_SetAreaNo(DVBCA_UINT32 uiAreaNo);
DVBCA_UINT32 DVBCA_GetAreaNo(void);
void DVBCA_GetCAVer(DVBCA_UINT8 *pucCaVer);
void DVBCA_GetStbID(DVBCA_UINT8 *pucStbID);
void DVBCA_SetCardID(DVBCA_UINT8 *pucCardID);
void DVBCA_GetCardID(DVBCA_UINT8 *pucCardID);
void DVBCA_GetRealCardID(DVBCA_UINT32 *puiCardID);
void DVBCA_SetNetworkID(DVBCA_UINT16 usNetworkID);
void DVBCA_GetNetworkID(DVBCA_UINT8 *pucBuffer);
void DVBCA_ResetEcmFilter(void);
void DVBCA_SetEmmPid(DVBCA_UINT16 usEmmPid);
void DVBCA_ClearEmmCrc(void);
void DVBCA_SetEcmPid(DVBCA_UINT16 *pusEcmPid, DVBCA_UINT8 ucEcmNum);
void DVBCA_GetEmmPid(DVBCA_UINT16 *pusPid);
void DVBCA_GetEcmPid(DVBCA_UINT16 *pusPid, DVBCA_UINT8 *pucPidNum);
int 	DVBCA_GetEcmFilter(DVBCA_UINT16 usEcmPid, ST_DVBCAFilter *pstFilter);
int 	DVBCA_GetEmmFilter(DVBCA_UINT16 usEmmPid, ST_DVBCAFilter *pstFilter);
void DVBCA_ParseData(DVBCA_UINT16 usPid, DVBCA_UINT8 *pucReceiveData, DVBCA_UINT16 usLen);
void DVBCA_ReSendEcm(void);
int DVBCA_GetAdvertieze(ST_DVBCA_MSG *pstAdvertize);

int DVBCA_GetCurProgExpiredTime(DVBCA_UINT16 usEcmPid, ST_DVBCATIME *pstTime);
int DVBCA_CheckOpertor(DVBCA_UINT32 uiOpertorID);
void DVBCA_SetOpertor(DVBCA_UINT8 *pucBuffer, DVBCA_UINT32 uiLen);
int DVBCA_ChechAreaNo(DVBCA_UINT32 uiCardAreaNo, DVBCA_UINT32 uiAreaNo);

void DVBCA_Kernel_UnInit(void);
#endif





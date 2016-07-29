#ifndef __dvbca_base_h__
#define __dvbca_base_h__

#define MAX_MSG_QUEUE_NODE_NUM	200

typedef enum
{
	MSG_SUCESS = 0,
	MSG_MALLOC_ERROR = -1,
	MSG_QUEUE_FULL = -2,
	MSG_QUEUE_EMPTY = -3,
	MSG_CMD_ERROR = -4,
}MSG_ERROR_CODE;

typedef enum
{
	MSG_CARDTASK,
	MSG_MSGTASK,
	MSG_MAXTYPE,
}MSG_TYPE;

typedef struct _ST_MSG_QUEUE
{
	DVBCA_UINT8 ucMsgType;
	DVBCA_UINT32 uiData;
}ST_MSG_QUEUE;


DVBCA_INT32 DVBCA_MsgQueueInit(void);
void DVBCA_MsgQueueUnInit(void);
DVBCA_INT32 DVBCA_MsgQueueAdd(DVBCA_UINT8 ucModule, DVBCA_UINT8 ucType, DVBCA_UINT32 uiData);
DVBCA_INT32 DVBCA_MsgQueueQuery(DVBCA_UINT8 ucModule, ST_MSG_QUEUE *pstMsg);

DVBCA_UINT32 DVBCA_Crc32(DVBCA_UINT8 *data, DVBCA_UINT32  len);
DVBCA_UINT32 DVBCA_CheckSum(DVBCA_UINT8 *data, DVBCA_UINT32  len);
DVBCA_UINT16 DVBCA_TransYMD(DVBCA_UINT8 ucYear, DVBCA_UINT8 ucMon, DVBCA_UINT8 ucDay);
void DVBCA_Trans2YMD(DVBCA_UINT16 usDays, DVBCA_UINT8 *pucYear, DVBCA_UINT8 *pucMon, DVBCA_UINT8 *pucDay);
void DVBCA_ParsePSW(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen,
	DVBCA_UINT8 *pucKey, DVBCA_UINT8 ucKeyLen);

void DVBCA_TimeToBytes(ST_DVBCATIME *time,DVBCA_UINT8 *bytes2);
void DVBCA_DesPro(DVBCA_UINT8 * pucBuffer, DVBCA_UINT8 * pucKey, DVBCA_UINT8 IfDecrypt);

#endif

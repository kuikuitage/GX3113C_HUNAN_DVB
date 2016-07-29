#include "dvbca_interface.h"
#include "dvbca_base.h"
#include "dvbca_kernel.h"

static const DVBCA_UINT8 des_table1[4][16] =
{
	{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
	{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
	{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
	{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
};
static const DVBCA_UINT8 des_table2[4][16]= 
{
	{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
	{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
	{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
	{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }	
};
  
static const DVBCA_UINT8 des_table3[4][16]=
{
	{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
	{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
	{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
	{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
};

static const DVBCA_UINT8 des_table4[4][16]= 
{
	{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
	{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
	{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
	{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
};

static const DVBCA_UINT8 des_table5[4][16]=
{
   	{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
	{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
	{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
	{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
};

static const DVBCA_UINT8 des_table6[4][16]= 
{
	{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
	{10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
	{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
	{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13} 
};

static const DVBCA_UINT8 des_table7[4][16]= 
{
	{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
	{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
	{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
	{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12} 
};

static const DVBCA_UINT8 des_table8[4][16] = 
{
	{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
	{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
	{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
	{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11} 
};

static const DVBCA_UINT8 shift[16] = 
{
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 
};

static const DVBCA_UINT8 binary[64] = 
{
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,
	0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
	1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 
};

static DVBCA_UINT32 g_uiCrcTable[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 
	0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7, 
	0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75, 
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 
	0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef, 
	0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 
	0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0, 
	0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072, 
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 
	0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 
	0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba, 
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 
	0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050, 
	0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 
	0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1, 
	0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53, 
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 
	0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9, 
	0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b, 
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 
	0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71, 
	0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3, 
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 
	0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 
	0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec, 
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 
	0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676, 
	0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 
	0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

static DVBCA_UINT8 g_aucKey[16] = 
{
	0x09, 0x44, 0x16, 0xEE, 0x69, 0x15, 0xDD, 0x41, 0x31, 0xC4, 0xFF, 0xB1, 0x0E, 0x91, 0x0F, 0xCE
};

static DVBCA_UINT8 g_aucKeyData[100];
static ST_MSG_QUEUE **g_pstMsgQueue = NULL;
static DVBCA_UINT32 *g_puiMsgQueuePos = NULL;
static DVBCA_UINT32 *g_puiMsgQueueNodeNum = NULL;
static DVBCA_SEMAPHORE *g_puiMsgSemaphore = NULL;
static DVBCA_SEMAPHORE *g_puiMsgWaitSemaphore = NULL;
static DVBCA_UINT8 g_ucMsgCacheState = 0;

int DVBCA_MsgQueueInit(void)
{
	int i = 0;

	g_pstMsgQueue = (ST_MSG_QUEUE **)DVBSTBCA_Malloc(MSG_MAXTYPE * sizeof(ST_MSG_QUEUE *));
	if(g_pstMsgQueue == NULL)
		return MSG_MALLOC_ERROR;
	DVBSTBCA_Memset(g_pstMsgQueue, 0x0, MSG_MAXTYPE * sizeof(ST_MSG_QUEUE *));

	g_puiMsgQueuePos = (DVBCA_UINT32 *)DVBSTBCA_Malloc(MSG_MAXTYPE * sizeof(DVBCA_UINT32));
	if(g_puiMsgQueuePos == NULL)
		return MSG_MALLOC_ERROR;
	DVBSTBCA_Memset(g_puiMsgQueuePos, 0x0, MSG_MAXTYPE * sizeof(DVBCA_UINT32));

	g_puiMsgQueueNodeNum = (DVBCA_UINT32 *)DVBSTBCA_Malloc(MSG_MAXTYPE * sizeof(DVBCA_UINT32));
	if(g_puiMsgQueueNodeNum == NULL)
		return MSG_MALLOC_ERROR;
	DVBSTBCA_Memset(g_puiMsgQueueNodeNum, 0x0, MSG_MAXTYPE * sizeof(DVBCA_UINT32));

	g_puiMsgSemaphore = (DVBCA_UINT32 *)DVBSTBCA_Malloc(MSG_MAXTYPE * sizeof(DVBCA_SEMAPHORE));
	if(g_puiMsgSemaphore == NULL)
		return MSG_MALLOC_ERROR;
	DVBSTBCA_Memset(g_puiMsgSemaphore, 0x0, MSG_MAXTYPE * sizeof(DVBCA_SEMAPHORE));

	g_puiMsgWaitSemaphore = (DVBCA_UINT32 *)DVBSTBCA_Malloc(MSG_MAXTYPE * sizeof(DVBCA_SEMAPHORE));
	if(g_puiMsgWaitSemaphore == NULL)
		return MSG_MALLOC_ERROR;
	DVBSTBCA_Memset(g_puiMsgWaitSemaphore, 0x0, MSG_MAXTYPE * sizeof(DVBCA_SEMAPHORE));

	for(i = 0; i < MSG_MAXTYPE; i++)
	{
		g_pstMsgQueue[i] = (ST_MSG_QUEUE *)DVBSTBCA_Malloc(MAX_MSG_QUEUE_NODE_NUM * sizeof(ST_MSG_QUEUE));
		if(g_pstMsgQueue[i] == NULL)
			return MSG_MALLOC_ERROR;
		DVBSTBCA_Memset(g_pstMsgQueue[i], 0x0, MAX_MSG_QUEUE_NODE_NUM * sizeof(ST_MSG_QUEUE));

		DVBSTBCA_SemaphoreInit(&(g_puiMsgSemaphore[i]), 0);
		DVBSTBCA_SemaphoreInit(&(g_puiMsgWaitSemaphore[i]), 0);
		DVBSTBCA_SemaphoreSignal(&(g_puiMsgSemaphore[i]));
	}
	g_ucMsgCacheState = 1;
	return MSG_SUCESS;
}

void DVBCA_MsgQueueUnInit(void)
{
	int i = 0;

	if(g_pstMsgQueue)
	{
		for(i = 0; i < MSG_MAXTYPE; i++)
		{
			if(g_pstMsgQueue[i])
			{
				DVBSTBCA_Free(g_pstMsgQueue[i]);
				g_pstMsgQueue[i] = NULL;
			}
		}
		DVBSTBCA_Free(g_pstMsgQueue);
		g_pstMsgQueue = NULL;
	}
	DVBSTBCA_Free(g_puiMsgQueuePos);
	g_puiMsgQueuePos = NULL;
	DVBSTBCA_Free(g_puiMsgQueueNodeNum);
	g_puiMsgQueueNodeNum = NULL;
	DVBSTBCA_Free(g_puiMsgSemaphore);
	g_puiMsgSemaphore = NULL;
	DVBSTBCA_Free(g_puiMsgWaitSemaphore);
	g_puiMsgWaitSemaphore = NULL;
}

int DVBCA_MsgQueueAdd(DVBCA_UINT8 ucModule, DVBCA_UINT8 ucType, DVBCA_UINT32 uiData)
{
	ST_MSG_QUEUE *pstMsgQueue = NULL;
	DVBCA_UINT32 uiPos = 0;

	if(ucModule >= MSG_MAXTYPE)
		return MSG_CMD_ERROR;

	if(g_puiMsgQueueNodeNum[ucModule] >= MAX_MSG_QUEUE_NODE_NUM)
	{
		DVBSTBCA_Printf("[DVBCA_MsgQueueAdd] FULL! module:%d ucType:%d uiData:%08x\n",ucModule, ucType, uiData);
		return MSG_QUEUE_FULL;
	}
	
	DVBSTBCA_SemaphoreWait(&(g_puiMsgSemaphore[ucModule]));

	pstMsgQueue = g_pstMsgQueue[ucModule];
	uiPos = g_puiMsgQueuePos[ucModule];
	pstMsgQueue[uiPos].ucMsgType = ucType;
	pstMsgQueue[uiPos].uiData = uiData;

	g_puiMsgQueuePos[ucModule] = (g_puiMsgQueuePos[ucModule] + 1) % MAX_MSG_QUEUE_NODE_NUM;
	g_puiMsgQueueNodeNum[ucModule]++;
	DVBSTBCA_SemaphoreSignal(&(g_puiMsgWaitSemaphore[ucModule]));
	DVBSTBCA_SemaphoreSignal(&(g_puiMsgSemaphore[ucModule]));	
	return MSG_SUCESS;
}

int DVBCA_MsgQueueQuery(DVBCA_UINT8 ucModule, ST_MSG_QUEUE *pstMsg)
{
	ST_MSG_QUEUE *pstMsgQueue = NULL;
	int iPos = 0;
	if(pstMsg == NULL)
		return MSG_CMD_ERROR;

	DVBSTBCA_SemaphoreWait(&(g_puiMsgWaitSemaphore[ucModule]));
	if(g_puiMsgQueueNodeNum[ucModule] <= 0)
	{
		DVBSTBCA_Printf("[DVBCA_MsgQueueQuery] Empty! module:%d\n",ucModule);
		return MSG_QUEUE_EMPTY;
	}
	DVBSTBCA_SemaphoreWait(&(g_puiMsgSemaphore[ucModule]));

	pstMsgQueue = g_pstMsgQueue[ucModule];
	iPos = (g_puiMsgQueuePos[ucModule] + MAX_MSG_QUEUE_NODE_NUM - g_puiMsgQueueNodeNum[ucModule])% MAX_MSG_QUEUE_NODE_NUM;
	DVBSTBCA_Memcpy(pstMsg, &(pstMsgQueue[iPos]), sizeof(ST_MSG_QUEUE));
	g_puiMsgQueueNodeNum[ucModule]--;
	DVBSTBCA_SemaphoreSignal(&(g_puiMsgSemaphore[ucModule]));	
	return MSG_SUCESS;
}


DVBCA_UINT32 DVBCA_Crc32(DVBCA_UINT8 *data, DVBCA_UINT32  len)
{
	DVBCA_UINT32 i;
	DVBCA_UINT32 crc = 0xffffffff;
	for (i = 0; i < len; i++)
		crc = (crc << 8) ^ g_uiCrcTable[((crc >> 24) ^ *data++) & 0xff];
	return crc;
}

DVBCA_UINT32 DVBCA_CheckSum(DVBCA_UINT8 *data, DVBCA_UINT32  len)
{
	DVBCA_UINT32  i = 0;
	DVBCA_UINT32  u32CheckSum_Value = 0;
	for (i = 0; i < len; i++) 
	{
		u32CheckSum_Value += data[i];
	}
	return u32CheckSum_Value;

}

DVBCA_UINT16 DVBCA_TransYMD(DVBCA_UINT8 ucYear, DVBCA_UINT8 ucMon, DVBCA_UINT8 ucDay)
{
	DVBCA_UINT16 usDay = 0;
	DVBCA_UINT8 ucMonth = 0;
	DVBCA_UINT8 aucMonthDay[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	
	usDay = 365 * ucYear + ((ucYear+3) >> 2);

	if((ucYear & 0x03) == 0)
		aucMonthDay[1] = 29;
	else
		aucMonthDay[1] = 28;

	for(ucMonth  = 0; ucMonth < (ucMon -1); ucMonth++)
		usDay += aucMonthDay[ucMonth];

	usDay += (ucDay - 1);
	return usDay;
}

void DVBCA_Trans2YMD(DVBCA_UINT16 usDays, DVBCA_UINT8 *pucYear, DVBCA_UINT8 *pucMon, DVBCA_UINT8 *pucDay)
{
	DVBCA_UINT16 usYearDay = 366;
	DVBCA_UINT8 ucMonth = 0;
	DVBCA_UINT8 aucMonthDay[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	*pucYear = 0;
	*pucMon = 0;
	*pucDay = 0;

	
	while(usDays >= usYearDay)
	{
		(*pucYear) += 1;
		usDays -= usYearDay;
		if(((*pucYear) % 4) == 0)
			usYearDay = 366;
		else
			usYearDay = 365;
	}
	if(usYearDay == 366)
		aucMonthDay[1] = 29;
	
	while(usDays >= aucMonthDay[ucMonth])
	{
		usDays -= aucMonthDay[ucMonth];
		ucMonth ++;
	}

	*pucMon = ucMonth + 1;
	*pucDay = usDays + 1;
}

void DVBCA_ParsePSW(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen,
	DVBCA_UINT8 *pucKey, DVBCA_UINT8 ucKeyLen)
{
	DVBCA_UINT8 aucRealKey[16];
	int i = 0;

	for(i = 0; i < 2*ucKeyLen; i++)
	{
		if((i & 0x01) == 0)
			g_aucKeyData[i] = (pucKey[i >> 1] >> 4) & 0x0F;
		else
			g_aucKeyData[i] = pucKey[i >> 1] & 0x0F;
		
		if(g_aucKeyData[i] > 9)
			g_aucKeyData[i] += 0x57;
		else
			g_aucKeyData[i] += 0x30;
	}

	for(i = 0; i < 16; i++)
	{
		aucRealKey[i] = g_aucKey[i] ^ g_aucKeyData[i % (2*ucKeyLen)];
	}

	for(i = 0; i < usLen; i++)
	{
		pucBuffer[i] = pucBuffer[i] ^ aucRealKey[i & 0x0F];
	}
}


void DVBCA_TimeToBytes(ST_DVBCATIME *time,DVBCA_UINT8 *bytes2)
{
	DVBCA_UINT8 temp;
	bytes2[0] = time->ucYear;
	bytes2[0] <<= 1;
	temp = (time->ucMonth)>>3;
	bytes2[0] |= temp;
	bytes2[1] = (time->ucMonth) << 5;
	bytes2[1] |= (time->ucDay)&0x1F;
}


void DVBCA_DesPro(DVBCA_UINT8 * pucBuffer, DVBCA_UINT8 * pucKey, DVBCA_UINT8 IfDecrypt)
{
	DVBCA_UINT8 bufout[64];
	DVBCA_UINT8 kwork[56];
	DVBCA_UINT8 worka[48];
	DVBCA_UINT8 kn[48];
	DVBCA_UINT8 buffer[64];
	DVBCA_UINT8 key[64];
	DVBCA_UINT8 nbrofshift, temp1, temp2;
	int valindex;
	int i;
	int j;
	int k;
	int iter;

	/* MAIN PROCESS */
	/* Convert from 64-bit key into 64-DVBCA_UINT8 key */
	for (i = 0; i < 8; i++) 
	{
		j =pucKey[i];
		key[8*i] = (DVBCA_UINT8)((j /128) %2);//MSB
		key[8*i+1] =(DVBCA_UINT8)( (j / 64) % 2);
		key[8*i+2] =(DVBCA_UINT8)( (j / 32) % 2);
		key[8*i+3] =(DVBCA_UINT8)( (j / 16) % 2);
		key[8*i+4] =(DVBCA_UINT8) ((j / 8) % 2);
		key[8*i+5] =(DVBCA_UINT8)( (j / 4) % 2);
		key[8*i+6] = (DVBCA_UINT8)((j / 2) % 2);
		key[8*i+7] =(DVBCA_UINT8)(( j % 2));//LSB
	}

	/* Convert from 64-bit data into 64-DVBCA_UINT8 data */
	for (i = 0; i < 8; i++) 
	{
		j=pucBuffer[i];
		buffer[8*i]= (DVBCA_UINT8)((j /128) %2);
		buffer[8*i+1] = (DVBCA_UINT8)((j / 64) % 2);
		buffer[8*i+2] = (DVBCA_UINT8)((j / 32) % 2);
		buffer[8*i+3] =(DVBCA_UINT8)( (j / 16) % 2);
		buffer[8*i+4] =(DVBCA_UINT8)( (j / 8) % 2);
		buffer[8*i+5] =(DVBCA_UINT8)( (j / 4) % 2);
		buffer[8*i+6] =(DVBCA_UINT8)( (j / 2) % 2);
		buffer[8*i+7] =(DVBCA_UINT8)( j % 2);
	}

	/* Initial Permutation of Data */
	bufout[ 0] = buffer[57];
	bufout[ 1] = buffer[49];
	bufout[ 2] = buffer[41];
	bufout[ 3] = buffer[33];
	bufout[ 4] = buffer[25];
	bufout[ 5] = buffer[17];
	bufout[ 6] = buffer[ 9];
	bufout[ 7] = buffer[ 1];
	bufout[ 8] = buffer[59];
	bufout[ 9] = buffer[51];
	bufout[10] = buffer[43];
	bufout[11] = buffer[35];
	bufout[12] = buffer[27];
	bufout[13] = buffer[19];
	bufout[14] = buffer[11];
	bufout[15] = buffer[ 3];
	bufout[16] = buffer[61];
	bufout[17] = buffer[53];
	bufout[18] = buffer[45];
	bufout[19] = buffer[37];
	bufout[20] = buffer[29];
	bufout[21] = buffer[21];
	bufout[22] = buffer[13];
	bufout[23] = buffer[ 5];
	bufout[24] = buffer[63];
	bufout[25] = buffer[55];
	bufout[26] = buffer[47];
	bufout[27] = buffer[39];
	bufout[28] = buffer[31];
	bufout[29] = buffer[23];
	bufout[30] = buffer[15];
	bufout[31] = buffer[ 7];
	bufout[32] = buffer[56];
	bufout[33] = buffer[48];
	bufout[34] = buffer[40];
	bufout[35] = buffer[32];
	bufout[36] = buffer[24];
	bufout[37] = buffer[16];
	bufout[38] = buffer[ 8];
	bufout[39] = buffer[ 0];
	bufout[40] = buffer[58];
	bufout[41] = buffer[50];
	bufout[42] = buffer[42];
	bufout[43] = buffer[34];
	bufout[44] = buffer[26];
	bufout[45] = buffer[18];
	bufout[46] = buffer[10];
	bufout[47] = buffer[ 2];
	bufout[48] = buffer[60];
	bufout[49] = buffer[52];
	bufout[50] = buffer[44];
	bufout[51] = buffer[36];
	bufout[52] = buffer[28];
	bufout[53] = buffer[20];
	bufout[54] = buffer[12];
	bufout[55] = buffer[ 4];
	bufout[56] = buffer[62];
	bufout[57] = buffer[54];
	bufout[58] = buffer[46];
	bufout[59] = buffer[38];
	bufout[60] = buffer[30];
	bufout[61] = buffer[22];
	bufout[62] = buffer[14];
	bufout[63] = buffer[ 6];

	/* Initial Permutation of Key  64 to 56 */
	kwork[ 0] = key[56];
	kwork[ 1] = key[48];
	kwork[ 2] = key[40];
	kwork[ 3] = key[32];
	kwork[ 4] = key[24];
	kwork[ 5] = key[16];
	kwork[ 6] = key[ 8];
	kwork[ 7] = key[ 0];
	kwork[ 8] = key[57];
	kwork[ 9] = key[49];
	kwork[10] = key[41];
	kwork[11] = key[33];
	kwork[12] = key[25];
	kwork[13] = key[17];
	kwork[14] = key[ 9];
	kwork[15] = key[ 1];
	kwork[16] = key[58];
	kwork[17] = key[50];
	kwork[18] = key[42];
	kwork[19] = key[34];
	kwork[20] = key[26];
	kwork[21] = key[18];
	kwork[22] = key[10];
	kwork[23] = key[ 2];
	kwork[24] = key[59];
	kwork[25] = key[51];
	kwork[26] = key[43];
	kwork[27] = key[35];
	kwork[28] = key[62];
	kwork[29] = key[54];
	kwork[30] = key[46];
	kwork[31] = key[38];
	kwork[32] = key[30];
	kwork[33] = key[22];
	kwork[34] = key[14];
	kwork[35] = key[ 6];
	kwork[36] = key[61];
	kwork[37] = key[53];
	kwork[38] = key[45];
	kwork[39] = key[37];
	kwork[40] = key[29];
	kwork[41] = key[21];
	kwork[42] = key[13];
	kwork[43] = key[ 5];
	kwork[44] = key[60];
	kwork[45] = key[52];
	kwork[46] = key[44];
	kwork[47] = key[36];
	kwork[48] = key[28];
	kwork[49] = key[20];
	kwork[50] = key[12];
	kwork[51] = key[ 4];
	kwork[52] = key[27];
	kwork[53] = key[19];
	kwork[54] = key[11];
	kwork[55] = key[ 3];

	/* 16 Iterations */
	for (iter = 1; iter < 17; iter++) 
	{
		for (i = 0; i < 32; i++)
			buffer[i] = bufout[32+i];//R->L

		/* Calculation of F(R, K) */
		/* Permute - E    32 to 48 */
		worka[ 0] = buffer[31];
		worka[ 1] = buffer[ 0];
		worka[ 2] = buffer[ 1];
		worka[ 3] = buffer[ 2];
		worka[ 4] = buffer[ 3];
		worka[ 5] = buffer[ 4];
		worka[ 6] = buffer[ 3];
		worka[ 7] = buffer[ 4];
		worka[ 8] = buffer[ 5];
		worka[ 9] = buffer[ 6];
		worka[10] = buffer[ 7];
		worka[11] = buffer[ 8];
		worka[12] = buffer[ 7];
		worka[13] = buffer[ 8];
		worka[14] = buffer[ 9];
		worka[15] = buffer[10];
		worka[16] = buffer[11];
		worka[17] = buffer[12];
		worka[18] = buffer[11];
		worka[19] = buffer[12];
		worka[20] = buffer[13];
		worka[21] = buffer[14];
		worka[22] = buffer[15];
		worka[23] = buffer[16];
		worka[24] = buffer[15];
		worka[25] = buffer[16];
		worka[26] = buffer[17];
		worka[27] = buffer[18];
		worka[28] = buffer[19];
		worka[29] = buffer[20];
		worka[30] = buffer[19];
		worka[31] = buffer[20];
		worka[32] = buffer[21];
		worka[33] = buffer[22];
		worka[34] = buffer[23];
		worka[35] = buffer[24];
		worka[36] = buffer[23];
		worka[37] = buffer[24];
		worka[38] = buffer[25];
		worka[39] = buffer[26];
		worka[40] = buffer[27];
		worka[41] = buffer[28];
		worka[42] = buffer[27];
		worka[43] = buffer[28];
		worka[44] = buffer[29];
		worka[45] = buffer[30];
		worka[46] = buffer[31];
		worka[47] = buffer[0];

		/* KS Function Begin */
		if (IfDecrypt==0) 
		{
			nbrofshift = shift[iter-1];
			for (i = 0; i < (int) nbrofshift; i++) //shitft times
			{
				temp1 = kwork[0];
				temp2 = kwork[28];
				for (j = 0; j < 27; j++) 
				{
					kwork[j] = kwork[j+1];
					kwork[j+28] = kwork[j+29];
				}
				kwork[27] = temp1;
				kwork[55] = temp2;//key shift left 
			}
		} 
		else if (iter > 1) 
		{
			nbrofshift = shift[17-iter];//最右边
			for (i = 0; i < (int) nbrofshift; i++) //shitft times
			{
				temp1 = kwork[27];
				temp2 = kwork[55];
				for (j = 27; j > 0; j--) 
				{
					kwork[j] = kwork[j-1];
					kwork[j+28] = kwork[j+27];//key shift right
					}
				kwork[0] = temp1;
				kwork[28] = temp2;
			}
		}

		/* Permute kwork - PC2 /56  to 48 */
		kn[ 0] = kwork[13];
		kn[ 1] = kwork[16];
		kn[ 2] = kwork[10];
		kn[ 3] = kwork[23];
		kn[ 4] = kwork[ 0];
		kn[ 5] = kwork[ 4];
		kn[ 6] = kwork[ 2];
		kn[ 7] = kwork[27];
		kn[ 8] = kwork[14];
		kn[ 9] = kwork[ 5];
		kn[10] = kwork[20];
		kn[11] = kwork[ 9];
		kn[12] = kwork[22];
		kn[13] = kwork[18];
		kn[14] = kwork[11];
		kn[15] = kwork[ 3];
		kn[16] = kwork[25];
		kn[17] = kwork[ 7];
		kn[18] = kwork[15];
		kn[19] = kwork[ 6];
		kn[20] = kwork[26];
		kn[21] = kwork[19];
		kn[22] = kwork[12];
		kn[23] = kwork[ 1];
		kn[24] = kwork[40];
		kn[25] = kwork[51];
		kn[26] = kwork[30];
		kn[27] = kwork[36];
		kn[28] = kwork[46];
		kn[29] = kwork[54];
		kn[30] = kwork[29];
		kn[31] = kwork[39];
		kn[32] = kwork[50];
		kn[33] = kwork[44];
		kn[34] = kwork[32];
		kn[35] = kwork[47];
		kn[36] = kwork[43];
		kn[37] = kwork[48];
		kn[38] = kwork[38];
		kn[39] = kwork[55];
		kn[40] = kwork[33];
		kn[41] = kwork[52];
		kn[42] = kwork[45];
		kn[43] = kwork[41];
		kn[44] = kwork[49];
		kn[45] = kwork[35];
		kn[46] = kwork[28];
		kn[47] = kwork[31];
		/* KS Function End */

		/* worka XOR kn */
		for (i = 0; i < 48; i++)
			worka[i] = (DVBCA_UINT8)(worka[i] ^ kn[i]);

		/* 8 s-functions */
		valindex = des_table1[2*worka[0]+worka[5]][2*(2*(2*worka[1]+worka[2])+worka[3])+worka[4]];//
		valindex = valindex * 4;//<<2
		kn[ 0] = binary[0+valindex];//查表转换为4
		kn[ 1] = binary[1+valindex];
		kn[ 2] = binary[2+valindex];
		kn[ 3] = binary[3+valindex];
		valindex = des_table2[2*worka[ 6]+worka[11]][2*(2*(2*worka[ 7]+worka[ 8])+worka[ 9])+worka[10]];
		valindex = valindex * 4;
		kn[ 4] = binary[0+valindex];
		kn[ 5] = binary[1+valindex];
		kn[ 6] = binary[2+valindex];
		kn[ 7] = binary[3+valindex];
		valindex = (int)des_table3[2*worka[12]+worka[17]][2*(2*(2*worka[13]+worka[14])+worka[15])+worka[16]];
		valindex = valindex * 4;
		kn[ 8] = binary[0+valindex];
		kn[ 9] = binary[1+valindex];
		kn[10] = binary[2+valindex];
		kn[11] = binary[3+valindex];
		valindex = (int)des_table4[2*worka[18]+worka[23]][2*(2*(2*worka[19]+worka[20])+worka[21])+worka[22]];
		valindex = valindex * 4;
		kn[12] = binary[0+valindex];
		kn[13] = binary[1+valindex];
		kn[14] = binary[2+valindex];
		kn[15] = binary[3+valindex];
		valindex = (int)des_table5[2*worka[24]+worka[29]][2*(2*(2*worka[25]+worka[26])+worka[27])+worka[28]];
		valindex = valindex * 4;
		kn[16] = binary[0+valindex];
		kn[17] = binary[1+valindex];
		kn[18] = binary[2+valindex];
		kn[19] = binary[3+valindex];
		valindex = (int)des_table6[2*worka[30]+worka[35]][2*(2*(2*worka[31]+worka[32])+worka[33])+worka[34]];
		valindex = valindex * 4;
		kn[20] = binary[0+valindex];
		kn[21] = binary[1+valindex];
		kn[22] = binary[2+valindex];
		kn[23] = binary[3+valindex];
		valindex = (int)des_table7[2*worka[36]+worka[41]][2*(2*(2*worka[37]+worka[38])+worka[39])+worka[40]];
		valindex = valindex * 4;
		kn[24] = binary[0+valindex];
		kn[25] = binary[1+valindex];
		kn[26] = binary[2+valindex];
		kn[27] = binary[3+valindex];
		valindex = (int)des_table8[2*worka[42]+worka[47]][2*(2*(2*worka[43]+worka[44])+worka[45])+worka[46]];
		valindex = valindex * 4;
		kn[28] = binary[0+valindex];
		kn[29] = binary[1+valindex];
		kn[30] = binary[2+valindex];
		kn[31] = binary[3+valindex];

		/* Permute - P   p表*/
		worka[ 0] = kn[15];
		worka[ 1] = kn[ 6];
		worka[ 2] = kn[19];
		worka[ 3] = kn[20];
		worka[ 4] = kn[28];
		worka[ 5] = kn[11];
		worka[ 6] = kn[27];
		worka[ 7] = kn[16];
		worka[ 8] = kn[ 0];
		worka[ 9] = kn[14];
		worka[10] = kn[22];
		worka[11] = kn[25];
		worka[12] = kn[ 4];
		worka[13] = kn[17];
		worka[14] = kn[30];
		worka[15] = kn[ 9];
		worka[16] = kn[ 1];
		worka[17] = kn[ 7];
		worka[18] = kn[23];
		worka[19] = kn[13];
		worka[20] = kn[31];
		worka[21] = kn[26];
		worka[22] = kn[ 2];
		worka[23] = kn[ 8];
		worka[24] = kn[18];
		worka[25] = kn[12];
		worka[26] = kn[29];
		worka[27] = kn[ 5];
		worka[28] = kn[21];
		worka[29] = kn[10];
		worka[30] = kn[ 3];
		worka[31] = kn[24];

		/* bufout XOR worka */
		for (i = 0; i < 32; i++) 
		{
			bufout[i+32] =(DVBCA_UINT8)( bufout[i] ^ worka[i]);//^L[i-1]
			bufout[i] = buffer[i];//L[i]=R[i-1]
		}
	} /* End of Iter */

	/* Prepare Output R */   // OUT:  R[]L[]
	for (i = 0; i < 32; i++) 
	{
		j = bufout[i];
		bufout[i] = bufout[32+i];
		bufout[32+i] = (DVBCA_UINT8)j;
	}

	/* Inverse Initial Permutation */
	buffer[ 0] = bufout[39];
	buffer[ 1] = bufout[ 7];
	buffer[ 2] = bufout[47];
	buffer[ 3] = bufout[15];
	buffer[ 4] = bufout[55];
	buffer[ 5] = bufout[23];
	buffer[ 6] = bufout[63];
	buffer[ 7] = bufout[31];
	buffer[ 8] = bufout[38];
	buffer[ 9] = bufout[ 6];
	buffer[10] = bufout[46];
	buffer[11] = bufout[14];
	buffer[12] = bufout[54];
	buffer[13] = bufout[22];
	buffer[14] = bufout[62];
	buffer[15] = bufout[30];
	buffer[16] = bufout[37];
	buffer[17] = bufout[ 5];
	buffer[18] = bufout[45];
	buffer[19] = bufout[13];
	buffer[20] = bufout[53];
	buffer[21] = bufout[21];
	buffer[22] = bufout[61];
	buffer[23] = bufout[29];
	buffer[24] = bufout[36];
	buffer[25] = bufout[ 4];
	buffer[26] = bufout[44];
	buffer[27] = bufout[12];
	buffer[28] = bufout[52];
	buffer[29] = bufout[20];
	buffer[30] = bufout[60];
	buffer[31] = bufout[28];
	buffer[32] = bufout[35];
	buffer[33] = bufout[ 3];
	buffer[34] = bufout[43];
	buffer[35] = bufout[11];
	buffer[36] = bufout[51];
	buffer[37] = bufout[19];
	buffer[38] = bufout[59];
	buffer[39] = bufout[27];
	buffer[40] = bufout[34];
	buffer[41] = bufout[ 2];
	buffer[42] = bufout[42];
	buffer[43] = bufout[10];
	buffer[44] = bufout[50];
	buffer[45] = bufout[18];
	buffer[46] = bufout[58];
	buffer[47] = bufout[26];
	buffer[48] = bufout[33];
	buffer[49] = bufout[ 1];
	buffer[50] = bufout[41];
	buffer[51] = bufout[ 9];
	buffer[52] = bufout[49];
	buffer[53] = bufout[17];
	buffer[54] = bufout[57];
	buffer[55] = bufout[25];
	buffer[56] = bufout[32];
	buffer[57] = bufout[ 0];
	buffer[58] = bufout[40];
	buffer[59] = bufout[ 8];
	buffer[60] = bufout[48];
	buffer[61] = bufout[16];
	buffer[62] = bufout[56];
	buffer[63] = bufout[24];

	for (i = 0,j = 0; i < 8; i++,j+=8) 
	{
		pucBuffer[i]=0x00;
		for (k = 0; k < 7; k++)
			pucBuffer[i] =(DVBCA_UINT8)( (pucBuffer[i] + buffer[j+k]) * 2);
		pucBuffer[i] =(DVBCA_UINT8)( pucBuffer[ i] + buffer[j+7]);
	}
} 



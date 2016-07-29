#ifndef __dvbca_interface_h__
#define __dvbca_interface_h__


#define		OUT				/*被调用填充内容*/
#define 	IN				/*调用者填充内容*/
#define 	INOUT			/*调用者有输入,被调用者有输出*/

#include "dvbca_types.h"

#define DVBCA_PIN_MAX_LENGTH						(6)			/*最大PIN长度*/
#define DVBCA_NET_NAME_LEN						(8)			/*网络名称长度*/
#define DVBCA_MAX_MAIL							(50)		/*最大邮件数量*/

#define DVBCA_MAIL_TITLE_SIZE					(10)		/*邮件标题最大长度*/
#define DVBCA_MAIL_TERM_SIZE					(20)		/*邮件发送者最大长度*/
#define DVBCA_MAIL_CONTEXT_SIZE					(512)		/*邮件内容最大长度*/

#define DVBCA_OPERATOR_NAME_SIZE				(20)		/*运营商名称最大长度*/

#define DVBCA_NOTIFY_TITLE_SIZE					(20)		/*应急通告标题最大长度*/
#define DVBCA_NOTIFY_CONTENT_SIZE				(512)		/*应急通告内容最大长度*/


#define DVBCA_MAX_FILTER_SIZE					(12)		/*过滤器最大长度*/
#define DVBCA_MAXLEN_ENTITL						(100)		/*单运营商智能卡保存最多授权产品的个数*/
#define DVBCA_MAXLEN_ACLIST      				(5)      	/*智能卡内保存每运营商的AC个数*/
#define DVBCA_MAX_ECM_NUM						(2)			/*最大支持ECM pid 个数*/
#define DVBCA_MAXLEN_MSG_TEXT					(4096)

/*节目无法播放的提示*/
typedef enum
{
	DVBCA_MESSAGE_INSERTCARD = 0x01,		/*加扰节目，请插入智能卡*/
	DVBCA_MESSAGE_BADCARD = 0x02,			/*无法识别卡，不能使用*/
	DVBCA_MESSAGE_NOENTITLE = 0x03,		/*没有授权*/
	DVBCA_MESSAGE_NOOPER_TYPE = 0x04,		/*卡中不存在节目运营商*/
	DVBCA_MESSAGE_BLOCK_AREA = 0x05,		/*区域禁播*/
	DVBCA_MESSAGE_EXPIRED = 0x06,			/*授权过期*/
	DVBCA_MESSAGE_PASSWORD_ERROR = 0x07,	/*运营商密码错误*/
	DVBCA_MESSAGE_ERRREGION = 0x08,		/*区域不正确*/
	DVBCA_MESSAGE_PAIRING_ERROR = 0x09,	/*机卡不匹配*/
	DVBCA_MESSAGE_WATCHLEVEL = 0x10,		/*节目级别高于设定观看级别*/
	DVBCA_MESSAGE_EXPIRED_CARD = 0x011,	/*智能卡过期*/
	DVBCA_MESSAGE_DECRYPTFAIL = 0x12,		/*节目解密失败*/
	DVBCA_MESSAGE_WORKTIME = 0x13,		/*不在工作时段内*/

	DVBCA_MESSAGE_FREQ_UPDATE = 0x21,		/*发现新节目,即将进行自动扫描*/
	DVBCA_MESSAGE_MAX_NUM = 0xFF,
}DVBCA_MESSAGE;

typedef enum
{
	DVBCA_SUCESS = 0,			/*执行成功*/
	DVBCA_FAILED = -1,			/*执行失败*/
	DVBCA_INSERTCARD = -2,		/*智能卡未插入*/
	DVBCA_CARD_INVALID = -3, 	/*智能卡无效*/
	DVBCA_ECM_PID_ERROR = -4, 	/*ECM Pid 错误*/
	DVBCA_EMM_PID_ERROR = -5,	/*EMM Pid 错误*/
	DVBCA_NOENTITLE = -6, 		/*没有授权*/
	DVBCA_PIN_ERROR = -7, 		/*Pin码错误*/
	DVBCA_STB_PAIR_FAILED = -8,	/*机卡配对失败*/
	DVBCA_INIT_ERROR = -9,		/*CA初始化失败*/
	DVBCA_CARD_VER_ERROR = -10,		/*该版本卡不支持该命令*/
}DVBCA_ERROR_CODE;

typedef enum
{
	MSG_BOX_INSERTCARD = 0x01,		/*加扰节目，请插入智能卡*/
	MSG_BOX_BADCARD = 0x02,			/*无法识别卡，不能使用*/
	MSG_BOX_NOENTITLE = 0x03,		/*没有授权*/
	MSG_BOX_NOOPER_TYPE = 0x04,		/*卡中不存在节目运营商*/
	MSG_BOX_BLOCK_AREA = 0x05,		/*区域禁播*/
	MSG_BOX_EXPIRED = 0x06,			/*授权过期*/
	MSG_BOX_PASSWORD_ERROR = 0x07,		/*运营商密码错误*/
	MSG_BOX_ERRREGION = 0x08,		/*区域不正确*/
	MSG_BOX_PAIRING_ERROR = 0x09,		/*机卡不匹配*/
	MSG_BOX_WATCHLEVEL = 0x10,		/*节目级别高于设定观看级别*/
	MSG_BOX_EXPIRED_CARD = 0x011,		/*智能卡过期*/
	MSG_BOX_DECRYPTFAIL = 0x12,		/*节目解密失败*/
	MSG_BOX_WORKTIME = 0x14,			/*不在工作时段内*/

	MSG_BOX_PPC_MSG = 0x20,			/*节目%d天后到期*/
	MSG_BOX_FREQ_UPDATE = 0x21,		/*发现新节目,即将进行自动扫描*/
	MSG_BOX_SOFTWARE_UPGRADE = 0x22,		/*软件升级中, 请稍候...*/
	MSG_BOX_BAD_SIGNAL = 0x24,		/*信号中断,请拨打服务电话*/
}EN_MSG_BOX_TYPE;

typedef enum
{
	UNDEF,
	DVBCA_QAM16,
	DVBCA_QAM32,
	DVBCA_QAM64,
	DVBCA_QAM128,
	DVBCA_QAM256
}DVBCA_MODUNATION_TYPE;/*调制类型变量*/

typedef enum _EN_CA_DB_MAIL_CMD
{
	CA_DB_MAIL_TOTAL_NUM,
	CA_DB_MAIL_READ_NUM,
	CA_DB_MAIL_NOT_READ_NUM,
}EN_CA_DB_MAIL_CMD;

typedef struct _ST_DVBCAWORKTIME
{
	 DVBCA_UINT8 ucHour;			/*小时*/
	 DVBCA_UINT8 ucMinute;		/*分钟*/
}ST_DVBCAWORKTIME;/*时间结构*/

typedef struct _ST_DVBCATIME
{
	 DVBCA_UINT8 ucYear;			/*年*/
	 DVBCA_UINT8 ucMonth;		/*月*/
	 DVBCA_UINT8 ucDay;			/*日*/
	 DVBCA_UINT8 ucHour;			/*小时*/
	 DVBCA_UINT8 ucMinute;		/*分钟*/
	 DVBCA_UINT8 ucSecond;		/*秒*/
}ST_DVBCATIME;/*时间结构*/


typedef struct _ST_DVBCAFilter{
	DVBCA_UINT8 aucFilter[DVBCA_MAX_FILTER_SIZE];		/*当前过滤器的值*/
	DVBCA_UINT8 aucMask[DVBCA_MAX_FILTER_SIZE];		/*当前过滤器的掩码*/
	DVBCA_UINT8 ucLen;								/*当前过滤器的长度*/
}ST_DVBCAFilter;

typedef struct _ST_DVBCAEmail{
	DVBCA_UINT16 usEmailID;								/*邮件ID*/
	DVBCA_UINT8 ucNewEmail;								/*是否新邮件*/
	DVBCA_UINT8 aucTitle[DVBCA_MAIL_TITLE_SIZE + 1];		/*邮件标题*/
	DVBCA_UINT8 aucTerm[DVBCA_MAIL_TERM_SIZE + 1];		/*邮件发送者*/
	DVBCA_UINT8 aucContext[DVBCA_MAIL_CONTEXT_SIZE + 1];	/*邮件内容*/
}ST_DVBCAEmail;


typedef struct _ST_DVBCA_OPERATORINFO
{
	DVBCA_UINT8 aucOperatorName[DVBCA_OPERATOR_NAME_SIZE];	/*运营商名称*/
	DVBCA_UINT16 usNetworkID;									/*网络编号*/
	DVBCA_UINT32 uiOperatorID;										/*运营商编号*/
	ST_DVBCATIME	stExpiredTime;									/*智能卡到期时间*/
}ST_DVBCA_OPERATORINFO;

typedef struct _ST_DVBCA_ENTITLE{
	DVBCA_UINT8  ucStartYear;									/*授权开始时间*/
	DVBCA_UINT8  ucStartMonth;
	DVBCA_UINT8  ucStartDay;
	DVBCA_UINT8  ucExpiredYear;									/*授权结束时间*/
	DVBCA_UINT8  ucExpiredMonth;
	DVBCA_UINT8  ucExpiredDay;
	DVBCA_UINT16 usProductID;									/*产品编号*/
}ST_DVBCA_ENTITLE;

typedef struct _ST_DVBCA_ENTITLES
{
	DVBCA_UINT16 	usProductCount;								/*产品数量*/
	DVBCA_UINT16	usEventCount;
	DVBCA_UINT8 	ucProductVersion;
	DVBCA_UINT8 	ucEventVersion;
	ST_DVBCA_ENTITLE	astEntitles[DVBCA_MAXLEN_ENTITL];
}ST_DVBCA_ENTITLES;

typedef struct _ST_DVBCA_RGB
{
	DVBCA_UINT8 	ucRed;
	DVBCA_UINT8 	ucGreen;
	DVBCA_UINT8 	ucBlue;
	DVBCA_UINT8 	ucAlpha;
}ST_DVBCA_RGB;

typedef struct _ST_DVBCA_MSG
{
	DVBCA_UINT8 ucDispPos;
	DVBCA_UINT8 ucDispWay;
	DVBCA_UINT8 ucDispCount;
	DVBCA_UINT8 ucDispDirection;
	DVBCA_UINT8 ucIntervalTime;
	DVBCA_UINT8 ucBgColor;
	DVBCA_UINT8 ucFontColor;
	DVBCA_UINT8 aucText[DVBCA_MAXLEN_MSG_TEXT];
	ST_DVBCA_RGB stBgColor;
	ST_DVBCA_RGB stFontColor;
}ST_DVBCA_MSG;

typedef struct _ST_DVBCA_FREQ
{
	DVBCA_MODUNATION_TYPE ucModunation;
	unsigned int uiFrequency;
	unsigned int uiSymbolrate;
}ST_DVBCA_FREQ;

typedef struct _ST_DVBCA_UPGRADEPROG
{
	unsigned char ucFreqCount;  /*频点变化数量*/
	unsigned char ucVer;/*频点变化版本号*/
	ST_DVBCA_FREQ *pstFreq;
}ST_DVBCA_UPGRADEPROG;

/*机顶盒实现函数*/
int DVBSTBCA_SCReset(void);
int DVBSTBCA_SetEcmFilter(IN ST_DVBCAFilter *pucFilter,
						   DVBCA_UINT8 ucFilterNum,
						   DVBCA_UINT16 usPid);
int DVBSTBCA_SetEmmFilter(IN ST_DVBCAFilter *pucFilter,
						   DVBCA_UINT8 ucFilterNum,
						   DVBCA_UINT16 usPid);
void DVBSTBCA_ScrSetCW(DVBCA_UINT16 usEcmPid,
					  IN DVBCA_UINT8 *pucOddKey,
					  IN DVBCA_UINT8 *pucEvenKey,
					  DVBCA_UINT8 ucKeyLen);
int DVBSTBCA_SCPBRun(IN DVBCA_UINT8 *pucCommand,
					DVBCA_UINT16 usCommandLen,
					OUT DVBCA_UINT8 *pucReply,
					OUT DVBCA_UINT16 *pusReplyLen,
					OUT DVBCA_UINT8 *pucStatusWord);

void DVBSTBCA_ReadBuffer(DVBCA_UINT32 uiAddr,
						OUT DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen);
void DVBSTBCA_WriteBuffer(DVBCA_UINT32 uiAddr,
						IN DVBCA_UINT8 *pucData,
						DVBCA_UINT32 uiLen);
void DVBSTBCA_FormatBuffer(DVBCA_UINT32 uiAddr,
						DVBCA_UINT32 uiLen);

void DVBSTBCA_ShowPPCMessage(DVBCA_UINT16 usEcmPid,
						 DVBCA_UINT8 ucDay);
void DVBSTBCA_ShowMessage(DVBCA_UINT16 usEcmPid,
						 DVBCA_UINT8 ucMessageType);
void DVBSTBCA_HideMessage(DVBCA_UINT16 usEcmPid,
						 DVBCA_UINT8 ucMessageType);
void DVBSTBCA_ShowFingerMessage(DVBCA_UINT16 usEcmPid,
						 DVBCA_UINT32 uiCardID);
void DVBSTBCA_HideFingerMessage(void);
void DVBSTBCA_NewEmailNotify(DVBCA_UINT16 usEmailID);

void DVBSTBCA_HideUrgent(void);
void DVBSTBCA_ShowUrgent(IN DVBCA_UINT8 *pucTitle,
						IN DVBCA_UINT8 *pucContent);

void DVBSTBCA_ShowOSDMessage(IN ST_DVBCA_MSG *pstAdvertize);

void DVBSTBCA_HideOSDMessage(void);
void DVBSTBCA_ResumeOSDMessage(void);

void DVBSTBCA_UpgradeFreq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum);

int DVBSTBCA_RegisterTask(IN const char *szName,
						 DVBCA_UINT8 ucPriority,
						 IN void *pTaskFun,
						 IN void *pParam,
						 DVBCA_UINT16 usStackSize);
void DVBSTBCA_Sleep(DVBCA_UINT16 usMilliSeconds);
void DVBSTBCA_SemaphoreInit(OUT DVBCA_SEMAPHORE *pSemaphore,
						   DVBCA_UINT32 uiInitValue);
void DVBSTBCA_SemaphoreSignal(INOUT DVBCA_SEMAPHORE *pSemaphore);
void DVBSTBCA_SemaphoreWait(INOUT DVBCA_SEMAPHORE *pSemaphore);
void DVBSTBCA_SemaphoreDestory(INOUT DVBCA_SEMAPHORE *pSemaphore);
void *DVBSTBCA_Malloc(DVBCA_UINT32 uiBufSize);
void DVBSTBCA_Free(void *pBuf);
void DVBSTBCA_Memset(void *pDestBuf, DVBCA_UINT8 ucValue, DVBCA_UINT32 uiSize);
void DVBSTBCA_Memcpy(void *pDestBuf, void *pSrcBuf, DVBCA_UINT32 uiSize);
DVBCA_UINT32 DVBSTBCA_Strlen(char *pString);
void DVBSTBCA_Printf(IN const char * fmt,...);


/*CAS接口函数*/
int DVBCASTB_Init(unsigned char ucThreadPriority, unsigned char *pucStbId);
int DVBCASTB_IsDVBCA(unsigned short usCaSystemID);
int DVBCASTB_SCInsert(void);
void DVBCASTB_SCRemove(void);
void DVBCASTB_SetEmmPid(unsigned short usEmmPid);
void DVBCASTB_SetEcmPid(IN unsigned short *pusEcmPid,
					   unsigned char ucEcmPidNum);
void DVBCASTB_PrivateDataGot(unsigned short usPid,
							unsigned char *pucReceiveData,
							unsigned short usLen);
void DVBCASTB_NitDataGot(unsigned char *pucReceiveData,
							unsigned short usLen);
int DVBCASTB_GetCaVer(OUT unsigned char *pucCaVer);
int DVBCASTB_GetCardVer(OUT unsigned char *pucCardVer);
int DVBCASTB_GetCardID(OUT unsigned char *pucCardID);
int DVBCASTB_SetStbPair(IN unsigned char* pucPin);
int DVBCASTB_GetStbPair(OUT unsigned int *puiStbID,
					OUT unsigned char *pucStbNum);
int DVBCASTB_GetOperatorID(OUT unsigned int *puiOperatorID,
							OUT unsigned int *puiOperatorNum);
int DVBCASTB_GetOperatorInfo(unsigned int uiOperatorID,
						OUT ST_DVBCA_OPERATORINFO *pstOperatorInfo);
int DVBCASTB_GetServiceEntitles(unsigned int uiOperatorID,
						OUT ST_DVBCA_ENTITLES *pstEntitles);
int DVBCASTB_SetRating(IN unsigned char* pucPin,
					unsigned char ucRating);
int DVBCASTB_GetRating(OUT unsigned char *pucRating);
int DVBCASTB_CheckPin(IN unsigned char* pucPin);
int DVBCASTB_ChangePin(IN unsigned char *pucOldPin,
					unsigned char *pucNewPin);

unsigned short DVBCASTB_GetMailNum(EN_CA_DB_MAIL_CMD enCmd);
unsigned short DVBCASTB_GetMail(OUT ST_DVBCAEmail *pstMail,
								unsigned char ucStartPos,
								unsigned short usNum);
int DVBCASTB_SetMailRead(unsigned short usMailID);
int DVBCASTB_DelMail(unsigned short usMailID);

void DVBCASTB_ShowOsdMessageEnd(void);

void DVBCASTB_GetNetName(unsigned char *pucNetName);
int DVBCASTB_GetCurProgExpiredTime(unsigned short usEcmPid,
						OUT ST_DVBCATIME *pstTime);
void DVBCASTB_RestoreDB(void);
int DVBCASTB_GetWorkTime(OUT ST_DVBCAWORKTIME *pstStartTime,
					OUT ST_DVBCAWORKTIME *pstEndTime);
int DVBCASTB_SetWorkTime(IN unsigned char* pucPin,
					IN ST_DVBCAWORKTIME *pstStartTime,
					IN ST_DVBCAWORKTIME *pstEndTime);
int DVBCASTB_GetACList(unsigned int uiOperatorID,
							OUT unsigned short *pusACArray,
							OUT unsigned int *puiACNum);
int DVBCASTB_GetPromptMsg(unsigned char ucPromptID,
							unsigned char ucLanguage,
							OUT unsigned char *pucText);

void DVBCASTB_UnInit(void);

#endif


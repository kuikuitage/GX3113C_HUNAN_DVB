#ifndef _DS_AD_API
#define _DS_AD_API

//提示消息
#define DSAD_MESSAGE_UPDATE_LOG_TYPE			(0X03)
#define DSAD_MSEEAGE_SHOWFULLSRCEEN_TYPE	(0X04)
#define DSAD_MSEEAGE_HIDEFULLSRCEEN_TYPE	(0X05)


#define	DSAD_FILTER_MAX_LEN		(15)
#define  IN                         /*函数参数(传入数据)*/
#define	 OUT                      /*函数参数(回传数据)*/

#ifndef		NULL
#define		NULL		0
#endif

#ifndef TRUE
#define TRUE				(1)
#endif

#ifndef FALSE
#define FALSE			(0)
#endif

#ifndef BOOL
#define BOOL			int
#endif

#ifndef INT8
#define INT8			char
#endif

#ifndef UINT8
#define UINT8			unsigned char
#endif

#ifndef INT16
#define INT16			short
#endif

#ifndef UINT16
#define UINT16			unsigned short
#endif

#ifndef INT32
#define INT32			int
#endif

#ifndef UINT32
#define UINT32			unsigned int
#endif

#ifndef DSAD_Semaphore
#define DSAD_Semaphore	unsigned int
#endif

#ifndef DSAD_MsgQueue
#define DSAD_MsgQueue	unsigned int
#endif



//1.机顶盒平台
typedef enum _EN_DSAD_STB_DECODE_TYPE
{
	DSAD_STB_SD=1,
	DSAD_STB_HD=2,
	DSAD_STB_UNKNOWN,
}EN_DSAD_STB_DECODE_TYPE;

//2.支持的广告类型
typedef enum _EN_DSAD_POS_TYPE
{
	DSAD_STB_LOG = 1,       //开机广告
	DSAD_STB_EPG = 2,          
	DSAD_STB_FULLSCREEN = 3,//推送广告
	DSAD_STB_VOLUME = 4,
	DSAD_STB_MENU = 5,
	DSAD_STB_UNAUTHORIZED = 6,/*未授权节目广告*/
	DSAD_STB_POS_UNKNOWN,
}EN_DSAD_POS_TYPE;

//3.支持的图片格式
typedef enum _EN_DSAD_PIC_TYPE
{
	DSAD_PIC_GIF=3,
	DSAD_PIC_UNKNOWN,
}EN_DSAD_PIC_TYPE;

//4.支持的音视频格式
typedef enum _EN_DSAD_AV_TYPE
{
	DSAD_AV_VIDEO_M2V=4,
	DSAD_AV_UNKNOWN,
}EN_DSAD_AV_TYPE;

//5、调用广告系统函数返回值类型定义
typedef enum _EN_DSAD_FUNCTION_TYPE
{
	DSAD_FUN_ERR_PARA,
	DSAD_FUN_NO_ELEMENT,
	DSAD_FUN_NOT_SUPPORT,
	DSAD_FUN_FLASH_NOT_READY,
	DSAD_FUN_OK,
}EN_DSAD_FUNCTION_TYPE;

//6、时间格式
typedef struct _ST_DSAD_TIME_INFO
{
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DSAD_TIME_INFO;

//7、日期时间格式
typedef struct _ST_DSAD_DATE_TIME_INFO
{
	UINT16	usYear;
	UINT8	ucMonth;
	UINT8	ucDay;
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DSAD_DATE_TIME_INFO;

//8、OSD显示位置
typedef enum _EN_DSAD_OSD_SHOW_POS
{
	SHOW_TOP_AD = 0,					//在顶部显示
	SHOW_BOTTOM_AD,					//在底部显示
	SHOW_TOP_BOTTOM_AD,				//在顶部和底部同时显示
	SHOW_LEFT_AD,						//在左边显示
	SHOW_RIGHT_AD,					//在右边显示	
	SHOW_LEFT_RIGHT_DA,				//在左边和右边同时显示
	SHOW_UNKNOWN_TYPE_AD,
}EN_DSAD_OSD_SHOW_POS;

//9、OSD显示信息
typedef struct _ST_DSAD_OSD_SHOW_INFO
{
	EN_DSAD_OSD_SHOW_POS		enDisplayPos; //osd显示位置
	UINT8				aucBackgroundColor[4];//背景颜色
	UINT8				aucDisplayColor[4];   //字体颜色
	UINT16				usContentSize;        //osd内容长度
	UINT16				usGetBufferSize;
	UINT8* 				pucDisplayBuffer;     //osd内容
	UINT16				ucFontType;           //字体类型
	UINT16				ucFontSize;           //字体大小
	UINT8				ucShowTimes;          //显示次数
}ST_DSAD_OSD_SHOW_INFO;

//10、图片显示信息
typedef struct _ST_DSAD_PICTURE_SHOW_INFO
{
	EN_DSAD_POS_TYPE	enPosType;
	EN_DSAD_PIC_TYPE	enPicType;
	UINT16				usStartX;
	UINT16				usStartY;
	UINT16				usWidth;
	UINT16				usHeigth;
	UINT32				uiDataLen;
	UINT8				aucElementId[4];
	UINT8* 				pucPicData;
}ST_DSAD_PICTURE_SHOW_INFO;

//11、音视频显示信息
typedef struct _ST_DSAD_AV_SHOW_INFO
{
	EN_DSAD_POS_TYPE	enPosType;
	EN_DSAD_AV_TYPE		enAvType;
	UINT16				usStartX;
	UINT16				usStartY;
	UINT16				usWidth;
	UINT16				usHeigth;
	UINT32				uiDataLen;
	UINT8				aucElementId[4];
	UINT8* 				pucAvData;
}ST_DSAD_AV_SHOW_INFO;

//12、图片视频广告显示信息
typedef struct _ST_DSAD_PROGRAM_SHOW_INFO
{
	ST_DSAD_PICTURE_SHOW_INFO stPictureShow;
	ST_DSAD_AV_SHOW_INFO stAvPlay;
	UINT8 aucFlag[2];
}ST_DSAD_PROGRAM_SHOW_INFO;

//13、OSD显示信息//2013-4-7 wsq 增
typedef struct _ST_DSAD_OSD_PROGRAM_SHOW_INFO
{
	ST_DSAD_OSD_SHOW_INFO stTextShow[15];	
	UINT8 aucFlag[15];
	UINT8 ucOsdNum;
}ST_DSAD_OSD_PROGRAM_SHOW_INFO;

//14开机广告信息
typedef struct _ST_DSAD_LOG_INFO
{
	UINT8 ucLogType;//1图片；2视频
	union
	{
		ST_DSAD_AV_SHOW_INFO stAvType;
		ST_DSAD_PICTURE_SHOW_INFO stPictureType;
	}Element;
}ST_DSAD_LOG_INFO;

//15过滤数据的类型
typedef enum _EN_DSAD_DATA_ID
{
	DSAD_ELEMENT_DATA = 1,
	DSAD_DELECMD_DATA = 2,
	DSAD_CONFIG_DATA = 3,
    DSAD_ELEMENT2_DATA = 4,
    DSAD_ELEMENT3_DATA = 5, 
    DSAD_ELEMENT4_DATA = 6,
    DSAD_ELEMENT5_DATA = 7,
    DSAD_ELEMENT6_DATA = 8,
	DSAD_TOTAL_TYPE,
}EN_DSAD_DATA_ID;

//16、过滤器回传函数
typedef void (* FilterNotifyFunction)(UINT16 usPid, IN UINT8* pucDataBuffer, UINT16 usDataLen);

//17、过滤器过滤条件
typedef struct _ST_DSAD_FILTER_INFO
{
	EN_DSAD_DATA_ID enDataId;//8
	UINT16 usChannelPid;//16
	UINT8 ucFilterLen;//8
	UINT8 aucFilter[15];//15*8
	UINT8 aucMask[15];//15*8
	UINT8 ucWaitSeconds;
	FilterNotifyFunction pfDataNotifyFunction;
}ST_DSAD_FILTER_INFO;

//18、节目参数
typedef struct _ST_DSAD_PROGRAM_PARA
{
	UINT16 usNetworkId;
	UINT16 usTsId;
	UINT16 usServiceId;
}ST_DSAD_PROGRAM_PARA;

//19、消息队列参数
typedef struct _ST_DSAD_MSG_QUEUE
{
	UINT32 uiFirstPara;
	UINT32 uiSecondPara;
	UINT32 uiThirdPara;
	UINT32 uiFourthPara;
}ST_DSAD_MSG_QUEUE;

//20、消息队列等待模式
typedef enum _EN_DSAD_MSG_QUEUE_MODE
{
	DSAD_MSG_QUEUE_WAIT,
	DSAD_MSG_QUEUE_NOWAIT,
	DSAD_MSG_QUEUE_UNKNOW,
}EN_DSAD_MSG_QUEUE_MODE;

/***************************************stb提供给ad的函数******************************************/
extern void DSAD_Printf(OUT const INT8 *string,...);

extern BOOL DS_AD_Flash_Read(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen);
extern BOOL DS_AD_Flash_Write(UINT32 puiStartAddr,  UINT8 *pucData,  UINT32 uiLen);
extern BOOL DS_AD_Flash_Erase(UINT32 puiStartAddr,  UINT32 uiLen);

extern void* DSAD_Malloc(UINT32 uiBufferSize);
extern void DSAD_Free(IN void* pucBuffer);
extern void DSAD_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize);
extern void DSAD_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSAD_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSAD_StrLen(IN const UINT8* pucFormatBuffer);
extern INT32 DSAD_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...);

extern void DSAD_Sleep(UINT16 usMilliSeconds);	
extern BOOL DSAD_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun );
extern void DSAD_SemaphoreInit(IN DSAD_Semaphore* puiSemaphore , UINT8 ucInitVal);
extern void DSAD_ReleaseSemaphore(IN DSAD_Semaphore* puiSemaphore);
extern void DSAD_WaitSemaphore(IN DSAD_Semaphore* puiSemaphore);
extern BOOL DSAD_MsgQueueInit(IN INT8* pucName, UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_DSAD_MSG_QUEUE_MODE enMode);
extern BOOL DSAD_MsgQueueGetMsg(UINT32 uiMsgHandle, ST_DSAD_MSG_QUEUE* pstMsg, EN_DSAD_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond);
extern BOOL DSAD_MsgQueueSendMsg(UINT32 uiMsgHandle, ST_DSAD_MSG_QUEUE* pstMsg);

extern EN_DSAD_FUNCTION_TYPE DS_AD_SetFilter(ST_DSAD_FILTER_INFO *pstFilterInfo);
extern EN_DSAD_FUNCTION_TYPE DS_AD_StopFilter(EN_DSAD_DATA_ID enDataID, UINT16 usPID);


extern void DSAD_ShowMessage(UINT8 ucMessageType, IN UINT8 * pucMessage);

/**********************************************************************************************/

/******************************************广告模块提供给stb调用的函数****************************************************/
void DSAD_SetDataPid(UINT16 usADPid);
BOOL DSAD_Init(UINT8 ucPriority, UINT32 uiFlashStartAddress, EN_DSAD_STB_DECODE_TYPE enStbDecodeType);
EN_DSAD_FUNCTION_TYPE DSAD_ProgramGetElement(ST_DSAD_PROGRAM_PARA stProgramPara, ST_DSAD_PROGRAM_SHOW_INFO *pstProgramShow,EN_DSAD_POS_TYPE enAdPos);
EN_DSAD_FUNCTION_TYPE DSAD_NoProgramGetElement(ST_DSAD_PROGRAM_SHOW_INFO *pstProgramShow,EN_DSAD_POS_TYPE enAdPos);
EN_DSAD_FUNCTION_TYPE DSAD_ProgramGetOSD(ST_DSAD_PROGRAM_PARA stProgramPara,ST_DSAD_OSD_PROGRAM_SHOW_INFO *pstOsdProgramShow);//2013-4-7 wsq 增
EN_DSAD_FUNCTION_TYPE DSAD_GetADVersion(UINT8 * pucADVersion);
/**********************************************************************************************/
#endif
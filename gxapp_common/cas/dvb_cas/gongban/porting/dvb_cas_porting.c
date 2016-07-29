/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0	2015.4.15		  fuxl		creation
*****************************************************************************/
#include <gxcore.h>
#include "app_common_porting_ca_flash.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_ca_os.h"
#include "app_common_porting_ca_smart.h"
#include "app_common_table_nit.h"
#include "app_common_search.h"
#include "app_common_play.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_smart.h"
#include "app_common_porting_ca_descramble.h"
#include "app_common_prog.h"
#include "app_common_lcn.h"
#include "app_dvb_cas_api_demux.h"

#include "gxapp_sys_config.h"
#include "app_common_search.h"
#include "dvbca_interface.h"
#include "app_dvb_cas_api_pop.h"
#include "app_dvb_cas_api_finger.h"
#include "app_dvb_cas_api_email.h"

static dvb_ca_rolling_message_st  rollingmsg = {0};
static dvb_ca_finger_data_st  fingerdata = {0};
static dvb_cas_email_st emailflag = {0};
static ST_DVBCA_UPGRADEPROG upgradeprog = {0};
int DVBSTBCA_SCReset(void)
{

	uint8_t   len;
	uint8_t   atr[256] = {0};
	int nRetValue = FALSE;

	CAS_Dbg("[SC] DVBCASTB_SCReset!\n");

	nRetValue = app_dvb_cas_api_smart_reset(atr,&len);

#if 0
	{
		int i;
		for( i=0; i<len; i++)
		{
			if( i%4 == 0)
			{
				CAS_Dbg("\n[debug] ");
			}
			CAS_Dbg("%x ",atr[i]);
		}
	}
#endif
	if (TRUE == nRetValue)
	{
		//CAS_Dbg("[debug]DVBSTBCA_SCReset success \n");
		return DVBCA_SUCESS;
	}
	else
	{
		//CAS_Dbg("[debug]DVBSTBCA_SCReset failure \n");
		return DVBCA_FAILED;
	}
}

int DVBSTBCA_SCPBRun(unsigned char*      pucCommand,
	unsigned short      usCommandLen,
	unsigned char*      pucReply,
	unsigned short*     pusReplyLen,
	unsigned char*      pucStatusWord)
{
	uint8_t reply[1024] = {0};
	uint8_t sc_result;
	uint16_t ReceiveLen;
	uint8_t i = 1;
	//DSCAS_Dbg("%s>>sendLen=%d\n",__FUNCTION__,*pucSendLen);
	while(i--)
	{
		sc_result =  app_porting_ca_smart_sc_apdu(pucCommand,usCommandLen,reply,&ReceiveLen);
		if (TRUE == sc_result)
		{
			memcpy(pucReply, reply, ReceiveLen - 2);
			*pusReplyLen = ReceiveLen - 2;
			*pucStatusWord = reply[ReceiveLen - 2];
			*(pucStatusWord + 1) = reply[ReceiveLen - 1];
			//	CAS_DUMP("[SC] pucSend", pucSend, *pucSendLen);
			//	CAS_DUMP("[SC] pucReceive", pucReceive, *pucReceiveLen);
			//	CAS_DUMP("[SC] pucStatusWord", pucStatusWord, 2);
			return DVBCA_SUCESS;
		}
	}

	return DVBCA_FAILED;
}
/* 释放私有数据过滤器 */
void DVBCA_ReleasePrivateDataFilter( uint16_t wPid )
{
	uint32_t filterId = 0;
    bool releaseChannel = TRUE;
	ca_filter_t sfilter = {0};

	/*
	* 查找对应channel id , filter id
	*/

	for (filterId = 0; filterId< MAX_FILTER_COUNT;filterId++)
	{
		app_porting_ca_demux_get_by_filter_id(filterId,&sfilter);
		if (( 0 != sfilter.usedStatus)&&(0 != sfilter.handle)&&(0 != sfilter.channelhandle)
			&&(wPid == sfilter.pid))
		{

				CAS_Dbg("release  FilterId=%d usPid = 0x%x\n",filterId,wPid);
		            app_porting_ca_demux_release_filter( filterId, releaseChannel);

		}
	}

	return ;
}

int DVBSTBCA_SetEcmFilter(ST_DVBCAFilter* pucFilter, unsigned char ucFilterNum, unsigned short usPid)
{
	ca_filter_t filter = {0};
	bool repeatFlag = TRUE;
	//ca_filter_t* sfilter = NULL;
	//uint32_t filterId = 0;
	uint8_t     match[18] = {0,};
	uint8_t     mask[18] = {0,};
	int i = 0;
	uint8_t usLen = 0;
	if(pucFilter == NULL  )
	{
		return DVBCA_FAILED;
	}

	//DUMP(DEMUX,INFO,usLen,match,"0x%x");
	//DUMP(DEMUX,INFO,usLen,mask,"0x%x");
	CAS_Dbg("ECM ucFilterNum=%d usPid = 0x%x\n",ucFilterNum,usPid);

	if( usPid != 0 &&	usPid != 0x1FFF )
	{
		DVBCA_ReleasePrivateDataFilter( usPid );
		for (i=0;i<ucFilterNum;i++)
		{
			memset(match,0,sizeof(match));
			memset(mask,0,sizeof(mask));
			memcpy(match,(pucFilter+i)->aucFilter,(pucFilter+i)->ucLen);
			memcpy(mask,(pucFilter+i)->aucMask,(pucFilter+i)->ucLen);
			usLen = (pucFilter+i)->ucLen;


			filter.crcFlag = FALSE;
			filter.filterLen = usLen;
			memcpy(filter.match,match,usLen);
			memcpy(filter.mask,mask,usLen);
			filter.nWaitSeconds = 100000;
			filter.pid = usPid;
			filter.equalFlag = TRUE;
			filter.repeatFlag = repeatFlag;
			filter.Callback = app_dvb_cas_filter_notify;//过滤器数据回调处理
			filter.timeOutCallback = app_dvb_cas_filter_timeout_notify;//超时回调
			app_porting_ca_demux_start_filter(&filter);//设置CA过滤器
		}
	}
	return DVBCA_SUCESS;
}

int DVBSTBCA_SetEmmFilter(ST_DVBCAFilter *pucFilter, unsigned char ucFilterNum, unsigned short usPid)
{
	ca_filter_t filter = {0};
	bool repeatFlag = TRUE;
	//ca_filter_t* sfilter = NULL;
	//uint32_t filterId = 0;
	uint8_t     match[18] = {0,};
	uint8_t     mask[18] = {0,};
	int i = 0;
	uint8_t usLen;
	if(pucFilter == NULL  )
	{
		return DVBCA_FAILED;
	}

	//	DUMP(DEMUX,INFO,byLen,pbyFilter,"%x");
	//	DUMP(DEMUX,INFO,byLen,pbyMask,"%x");

	CAS_Dbg("EMM ucFilterNum=%d usPid = 0x%x\n",ucFilterNum,usPid);
	if (usPid !=0 && usPid !=0x1FFF)
	{
		DVBCA_ReleasePrivateDataFilter( usPid );
		for (i=0;i<ucFilterNum;i++)
		{
			memset(match,0,sizeof(match));
			memset(mask,0,sizeof(mask));
			memcpy(match,(pucFilter+i)->aucFilter,(pucFilter+i)->ucLen);
			memcpy(mask,(pucFilter+i)->aucMask,(pucFilter+i)->ucLen);
			usLen = (pucFilter+i)->ucLen;

			filter.crcFlag = FALSE;
			filter.filterLen = usLen;
			memcpy(filter.match,match,usLen);
			memcpy(filter.mask,mask,usLen);
			filter.nWaitSeconds = 100000;
			filter.pid = usPid;
			filter.equalFlag = TRUE;
			filter.repeatFlag = repeatFlag;
			filter.Callback = app_dvb_cas_filter_notify;//过滤器数据回调处理
			filter.timeOutCallback = app_dvb_cas_filter_timeout_notify;//超时回调
			app_porting_ca_demux_start_filter(&filter);//设置CA过滤器
		}
	}
	return DVBCA_SUCESS;
}

void DVBSTBCA_ScrSetCW(unsigned short usEcmPid, IN unsigned char* pucOddKey, IN unsigned char* pucEvenKey, unsigned char ucKeyLen)
{

	//CAS_DUMP("[CW OldCw]->", pucOddKey, 8);
	//CAS_DUMP("[CW EvenCw]->", pucEvenKey, 8);
	//CAS_Dbg("[debug] %s %d usECmPid = 0x%x ucKeyLen = %d\n",__FUNCTION__,__LINE__,usEcmPid,ucKeyLen);
	app_porting_ca_descramble_set_cw(usEcmPid,pucEvenKey,pucOddKey,ucKeyLen,0);
	return;
}

/*Something about NVRAM*/
void DVBSTBCA_FormatBuffer(unsigned int uiAddr, unsigned int uiLen)
{
	app_porting_ca_flash_erase(uiAddr,(uint32_t)uiLen);
}

void DVBSTBCA_ReadBuffer(unsigned int uiAddr, OUT unsigned char*  pucData, unsigned int uiLen)
{
	if (NULL == pucData)
	{
		CAS_Dbg("DVBSTBCA_ReadDataBuffer plDataLen=0x%x pData=0x%x\n",uiLen,(unsigned int)pucData);
		return;
	}

	app_porting_ca_flash_read_data(uiAddr,(uint8_t *)pucData,(uint32_t *)&uiLen);
}

void DVBSTBCA_WriteBuffer(unsigned int uiAddr, IN unsigned char* pucData, unsigned int uiLen)
{
	if (NULL == pucData)
	{
		CAS_Dbg("DVBSTBCA_WriteDataBuffer pData=0x%x \n",(unsigned int)pucData);
		return;
	}

	app_porting_ca_flash_write_data(uiAddr,(uint8_t *)pucData,uiLen);
}

/*Something about OS*/
int DVBSTBCA_RegisterTask(IN const char* szName, unsigned char  ucPriority,IN void* pTaskFun,IN void* pParam,unsigned short usStackSize)
{

    	int         ret;
    	handle_t    handle;
	uint32_t priority = ucPriority;

	if ((NULL == szName)||(NULL == pTaskFun))
	{
		CAS_Dbg("DVBSTBCA_RegisterTask szName=%s  pTaskFun=0x%x\n",szName,(unsigned int)pTaskFun);
		return DVBCA_FAILED;
	}
	CAS_Dbg("DVBSTBCA_RegisterTask szName=%s	ucPriority=%d\n",szName,priority);

      ret = app_porting_ca_os_create_task(szName, &handle, (void*)pTaskFun,
                  pParam, usStackSize, priority);
		return DVBCA_SUCESS;
}

void DVBSTBCA_Sleep(unsigned short usMilliSeconds)
{
	app_porting_ca_os_sleep(usMilliSeconds);
}

void DVBSTBCA_SemaphoreInit(DVBCA_SEMAPHORE* pSemaphore, unsigned int uiInitValue)
{
	handle_t semaphore = 0;
	if (NULL == pSemaphore)
	{
		CAS_Dbg("DVBSTBCA_SemaphoreInit pSemaphore=0x%x \n",(unsigned int)pSemaphore);
		return;
	}

	app_porting_ca_os_sem_create(&semaphore,uiInitValue);
	*pSemaphore = (DVBCA_SEMAPHORE)semaphore;
}

void DVBSTBCA_SemaphoreSignal(DVBCA_SEMAPHORE* pSemphore)
{
	if (NULL == pSemphore)
	{
		CAS_Dbg("DVBSTBCA_SemaphoreSignal pSemaphore=0x%x \n",(unsigned int)pSemphore);
		return;
	}
	app_porting_ca_os_sem_signal((handle_t)*pSemphore);
}

void DVBSTBCA_SemaphoreWait(DVBCA_SEMAPHORE* pSemaphore)
{
	if (NULL == pSemaphore)
	{
		CAS_Dbg("DVBSTBCA_SemaphoreWait pSemaphore=0x%x \n",(unsigned int)pSemaphore);
		return;
	}

	app_porting_ca_os_sem_wait((handle_t)*pSemaphore);
}

void DVBSTBCA_SemaphoreDestory(INOUT DVBCA_SEMAPHORE *pSemaphore)
{
	if (NULL == pSemaphore)
	{
		CAS_Dbg("DVBSTBCA_SemaphoreDestory pSemaphore=0x%x \n",(unsigned int)pSemaphore);
	}
	app_porting_ca_os_sem_delete((handle_t)*pSemaphore);
}

void* DVBSTBCA_Malloc(unsigned int uiBufSize)
{
	return app_porting_ca_os_malloc(uiBufSize);
}

void DVBSTBCA_Free(void* pBuf)
{
	app_porting_ca_os_free(pBuf);
}

void DVBSTBCA_Memset(void* pDestBuf, unsigned char ucValue, unsigned int uiSize)
{
	if(pDestBuf == NULL)
	{
		CA_FAIL(("DVBSTBCA_Memset err\n"));
		return;
	}
	memset(pDestBuf, ucValue, uiSize);
}

void DVBSTBCA_Memcpy(void* pDestBuf, void* pSrcBuf, unsigned int uiSize)
{
	if(pDestBuf == NULL||pSrcBuf == NULL)
	{
		CA_FAIL(("DVBSTBCA_Memcpy err\n"));
		return;
	}

	memcpy(pDestBuf, pSrcBuf, uiSize);
}

unsigned int DVBSTBCA_Strlen(char* pString)
{
	return strlen(pString);
}

void DVBSTBCA_Printf(IN const char* fmt, ...)
{
	va_list     ap;
	//extern int  print_enable;

	//if (print_enable)
	{
		va_start(ap, fmt);
		vfnprintf(stdout, INT_MAX, fmt, ap);
		va_end(ap);
	}
	return;
}

void DVBSTBCA_ShowOSDMessage(ST_DVBCA_MSG *pstAdvertize)
{
	int i = 0;
	int j=0;

	memset(&rollingmsg,0,sizeof(dvb_ca_rolling_message_st))	;
	for (i = 0; i< strlen((char *)pstAdvertize->aucText);i++)
	{
		if (i >= ROLL_TITLE_MAX)
			break;
		if ((0x0d != pstAdvertize->aucText[i] )&&(0x0a != pstAdvertize->aucText[i]))
		{
			rollingmsg.content[j++] = pstAdvertize->aucText[i];
		}
		else
		{
			/*回车或换行会被当作结束符*/
			CAS_Dbg("aucText[%d]=0x%x\n",i,pstAdvertize->aucText[i]);
		}
	}
	CAS_Dbg("[debug] osd type = %d time = %d \n",pstAdvertize->ucDispCount,pstAdvertize->ucIntervalTime);
	CAS_Dbg("[debug] osd pos = %d way= %d direct=%d \n",pstAdvertize->ucDispPos,pstAdvertize->ucDispWay,pstAdvertize->ucDispDirection);
	rollingmsg.wStatus = TRUE;
	rollingmsg.allTimes = pstAdvertize->ucDispCount;
	rollingmsg.wDuration= pstAdvertize->ucIntervalTime*10;
	app_cas_api_on_event(DVB_CA_ROLLING_INFO,CAS_NAME, &rollingmsg, sizeof(dvb_ca_rolling_message_st));
}

void DVBSTBCA_HideOSDMessage(void)
{
	CAS_Dbg("%s\n", __FUNCTION__);
	rollingmsg.wStatus = FALSE;
	app_cas_api_on_event(DVB_CA_ROLLING_INFO,CAS_NAME, &rollingmsg, sizeof(dvb_ca_rolling_message_st));
}

void DVBSTBCA_ResumeOSDMessage(void)
{
	CAS_Dbg("%s\n", __FUNCTION__);
	rollingmsg.wStatus = TRUE;
	app_cas_api_on_event(DVB_CA_ROLLING_INFO,CAS_NAME, &rollingmsg, sizeof(dvb_ca_rolling_message_st));
}

void DVBSTBCA_ShowFingerMessage(unsigned short usEcmPid, unsigned int uiCardID)
{
	fingerdata.dwCardID = uiCardID;
	fingerdata.wEcmPid = usEcmPid;
	if (0 == fingerdata.dwCardID)
	{
		fingerdata.wStatus = 0;
		/*通知关闭指纹窗体*/
	}
	else
	{
		fingerdata.wStatus = TRUE;
	}
	CAS_Dbg("%s %d uiCardID = %d usEcmPid = %d\n",__FUNCTION__,__LINE__,uiCardID,usEcmPid);
	app_cas_api_on_event(DVB_CA_FINGER_INFO,CAS_NAME, &fingerdata, sizeof(dvb_ca_finger_data_st));
	return;
}

void DVBSTBCA_HideFingerMessage(void)
{
	fingerdata.wStatus = 0;
	app_cas_api_on_event(DVB_CA_FINGER_INFO,CAS_NAME, &fingerdata, sizeof(dvb_ca_finger_data_st));
}

void DVBSTBCA_NewEmailNotify(unsigned short usEmailID)
{
	emailflag.wStatus = DVB_CA_EMAIL_NEW;
	app_cas_api_on_event(DVB_CA_EMAIL_INFO,CAS_NAME, &emailflag, sizeof(dvb_cas_email_st));
	CAS_Dbg("%s\t EmailID:%d\n", __FUNCTION__, usEmailID);
}


void DVBSTBCA_ShowMessage(unsigned short usEcmPid, unsigned char ucMessageType)
{
	if ( TRUE != app_play_get_msg_pop_type_state(MSG_POP_COMMON_END+ucMessageType+1))
	{
		app_play_clear_ca_msg();
		app_play_set_msg_pop_type(MSG_POP_COMMON_END+ucMessageType+1);
	}
	CAS_Dbg("%s\t类型:%d Ecmpid:0x%x\n", __FUNCTION__, ucMessageType, usEcmPid);
}

void DVBSTBCA_HideMessage(unsigned short usEcmPid, unsigned char ucMessageType)
{
	app_play_clear_ca_msg();
}

extern void win_dvb_cas_showDialog(unsigned char *title,unsigned char *content);
void DVBSTBCA_ShowUrgent(unsigned char* pucTitle, unsigned char* pucContent)
{
	win_dvb_cas_showDialog(pucTitle,pucContent);
	return;
}

extern void win_dvb_cas_close_dialog();
void DVBSTBCA_HideUrgent(void)
{
	win_dvb_cas_close_dialog();
	CAS_Dbg("应急通告隐藏HLSTBCA_HideUrgent running\n");
}

#include "app_common_search.h"
ST_DVBCA_FREQ stFreq[TP_MAX_NUM] = {{0,},};
void DVBSTBCA_UpgradeFreq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum)
{
	unsigned char freqSum;

	if((ucFreqSum == 0) || pstFreq == NULL)
		return;
	else if(ucFreqSum > TP_MAX_NUM)
		freqSum = TP_MAX_NUM;
	else
		freqSum = ucFreqSum;
	memset(stFreq,0,TP_MAX_NUM*sizeof(ST_DVBCA_FREQ));
	memcpy(&stFreq[0],pstFreq,freqSum*sizeof(ST_DVBCA_FREQ));
	upgradeprog.ucFreqCount = freqSum;
	upgradeprog.pstFreq = stFreq;
	app_cas_api_on_event(DVB_CA_UPGRADE_FRE,CAS_NAME, &upgradeprog, sizeof(ST_DVBCA_UPGRADEPROG));
	CAS_Dbg("[debug] %s %d sum=%d \n",__FUNCTION__,__LINE__,ucFreqSum);
}

void DVBSTBCA_ShowPPCMessage(unsigned short usEcmPid, unsigned char ucDay)
{
	char* osd_language=NULL;
	char text_buffer[100] = {0,};
	osd_language = app_flash_get_config_osd_language();
	if(ucDay < 10)
	{
		if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			sprintf(text_buffer,"%s","节目授权小于10天");
		}
		else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
		{
			sprintf(text_buffer,"%s","Entitlement expire in 10 days");
		}
	
		win_dvb_cas_showDialog(NULL,(unsigned char *)text_buffer);
		app_porting_ca_os_sleep(5000);
		win_dvb_cas_close_dialog();
	}

	CAS_Dbg("%s usEcmPid = 0x%x\n", __FUNCTION__,usEcmPid);
	return;
}


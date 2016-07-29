/*
* 此文件接口实现CA移植智能卡相关公共接口
*/
#include <gxtype.h>

#include "gxapp_sys_config.h"
#include "app_common_porting_ca_smart.h"
#include "app_common_porting_stb_api.h"
#include "gx_smartcard.h"
#include "app_common_porting_stb_api.h"
#include "app_common_panel.h"
//----------------------------------------

#define SMARTCARD_DEV_NAME                  ("/dev/gxsmartcard0")

static handle_t smart_handle=0;//智能卡设备句柄

//static uint8_t smart_card_art_flag = FALSE;
//qilian ca   init open after no card promt
/*static*/ GxSmcCardStatus smart_card_in_status =GXSMC_CARD_INIT;// GXSMC_CARD_OUT;
//static GxSmcCardStatus smart_card_in_status = GXSMC_CARD_OUT;
static GxSmcTimeParams smart_time;
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
static uint8_t card_state  = 0;//经过卡复位后的卡状态.0，无卡；1，复位后卡正常；2，卡复位出错
#endif
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
extern u_int8_t g_byMark;
#endif

static handle_t g_SCCardSem;

void app_porting_ca_smart_config_time(GxSmcTimeParams time)
{
	 GxSmc_Config(smart_handle, &time);
}
//extern 	int GxSmc_ConfigAll(handle_t handle, IN const GxSmcConfigs *config);
//智能卡复位
uint8_t app_porting_ca_smart_reset(GxSmcTimeParams    time,uint8_t* pbyATR, uint8_t *pbyLen )
{
	size_t                     len;
	GxSmcCardStatus status;
	uint8_t               atr[ATR_MAX_LEN] = {0,};
	int ret = -1;
	uint8_t i = 0;

	//	GxSmcConfigs cofigs = {0};

	if ((NULL == pbyATR)||(NULL == pbyLen))
		return FALSE;

	CAS_Dbg("[SC] app_porting_ca_smart_reset handle=0x%x pbyATR=0x%x,pbyLen=0x%x\n",
			smart_handle,(unsigned int)pbyATR,(unsigned int)pbyLen);

	if (0 == smart_handle)
		return FALSE;

   for (i = 0; i< 5; i++)
   	{
		ret = GxSmc_GetStatus(smart_handle,&status);//智能卡状态
		CHECK_RET(SMC,ret);
	    if (GXSMC_CARD_IN == status)
	    	{
				break;
	    	}
		else
			{
				GxCore_ThreadDelay(200);
			}
   	}

   if (GXSMC_CARD_IN != status)
   	{
		return FALSE;
   	}
   	
	ret = GxSmc_Reset(smart_handle,atr,ATR_MAX_LEN,&len);
	if ((-1 == ret)||(len >ATR_MAX_LEN))
	{
		CAS_Dbg("[SC] app_porting_ca_smart_reset failed ret=%d len=%d\n",
				ret,len);
		return FALSE;
	}

	if (len > 0) 
	{
		CAS_DUMP("[SC] ATR", atr, len);
		//		smart_card_art_flag = TRUE;
		if ((NULL != pbyATR)&&(NULL != pbyLen))
		{
			memcpy(pbyATR,atr,len);
			*pbyLen = len;
		}
		CAS_Dbg("[SC] Reset OK\n");

		/*		cofigs.flags = SMCC_TIME;
				time.flags = SMCT_WDT|SMCT_EGT|SMCT_TWDT|SMCT_TGT;
				memcpy(&cofigs.time,&time,sizeof(GxSmcTimeParams));
				ret = GxSmc_ConfigAll(smart_handle, &cofigs);*/

		ret = GxSmc_Config(smart_handle, &time);

		CHECK_RET(SMC,ret);


		return TRUE;
	}
	else
	{
		CAS_Dbg("[SC] Reset Failed len == 0\n");
	}
	CAS_Dbg("[SC] Reset Failed\n");
	return FALSE;
}

uint8_t app_porting_ca_smart_status()
{
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
	if ((GXSMC_CARD_IN == smart_card_in_status)/*&&(TRUE == smart_card_art_flag)*/)
	{
		return TRUE;
	}
	else if(GXSMC_CARD_INIT == smart_card_in_status)
	{
		return 0xff;
	}
	return FALSE;
#else
	if ((GXSMC_CARD_IN == smart_card_in_status)/*&&(TRUE == smart_card_art_flag)*/)
	{
		return TRUE;
	}
	return FALSE;
	
#endif
}
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
uint8_t app_porting_ca_smc_status(void)
{
	return card_state;
}
#endif

void app_porting_ca_smart_check_status(void)
{
	GxSmcCardStatus status;
	int32_t ret = 0;
	if (0 == smart_handle)
		return;
	ret = GxSmc_GetStatus(smart_handle,&status);//智能卡状态
	CHECK_RET(SMC,ret);
    if (status == GXSMC_CARD_IN) 
	{
		if(GXSMC_CARD_IN != smart_card_in_status)
		{
			app_porting_ca_smart_card_status_notify(smart_handle,status);
		}
	}
	else if (status == GXSMC_CARD_OUT)
	{
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
		card_state = 0;
#endif
    	if(GXSMC_CARD_OUT != smart_card_in_status)
    	{
			app_porting_ca_smart_card_status_notify(smart_handle,status);
    	}
    }

}

//智能卡消息
void app_porting_ca_smart_card_status_notify(handle_t handle, GxSmcCardStatus status)
{
       CAS_Dbg("[SC] app_porting_ca_smart_card_status_notify\n");
	int nRetValue = FALSE;
	    uint8_t                     len;
	    uint8_t               atr[ATR_MAX_LEN] = {0,};

	if (0 == smart_handle )
		smart_handle = handle;

    if (status == GXSMC_CARD_IN)
	{
		if(GXSMC_CARD_IN != smart_card_in_status)
		{
	        	CAS_Dbg("[SC] insert\n");
	        	nRetValue = app_porting_ca_smart_reset(smart_time,atr,&len);

			if(FALSE == nRetValue)
			{
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
				card_state = 2;
#endif
				CAS_Dbg("\n\n[SC] Reset Failed\n\n");
			}
#ifndef DVB_CA_TYPE_CD_CAS30_FLAG			
			if (TRUE == nRetValue)
				{				
			#ifdef DVB_CA_TYPE_DIVITONE_FLAG			
				card_state = 1;
			#endif
			     	smart_card_in_status = GXSMC_CARD_IN;
					app_cas_api_card_in((char*)atr,len);
				}
#else
			if (TRUE == nRetValue)
			{				
				smart_card_in_status = GXSMC_CARD_IN;

			}
			smart_card_in_status = GXSMC_CARD_IN;
			app_cas_api_card_in((char*)atr,len);
			CAS_Dbg("[SC] insert End\n");
		//	CAS_Dbg("[SC] insert End**22\n");
#endif

		       

		}
    }
	else if (status == GXSMC_CARD_OUT)
	{
    	if(GXSMC_CARD_OUT != smart_card_in_status)
    	{
	        CAS_Dbg("[SC] remove\n");
		    smart_card_in_status = GXSMC_CARD_OUT;
		    app_cas_api_card_out();
	        CAS_Dbg("[SC] remove End\n");
    	}
    }
}

//智能卡初始化(打开智能卡设备)
void app_porting_ca_smart_card_init(GxSmcParams param,GxSmcTimeParams time)
{
    handle_t handle=0;
	/*配置GPIO管脚*/
	GxCore_SemCreate(&g_SCCardSem, 1);	// 智能卡读写信号量保护
#ifdef DVB_CA_TYPE_BY_FLAG
        app_panel_set_gpio_high(33);
#else 
        app_panel_set_gpio_low(33);
#endif
	memset(&smart_time,0,sizeof(GxSmcTimeParams));
	memcpy(&smart_time,&time,sizeof(GxSmcTimeParams));

	handle = GxSmc_Open(SMARTCARD_DEV_NAME, &param);

	smart_handle = handle;
}


//修改在0x61的时候不能正常通信。可能这个0x61原来加那个是为了某个CA。不过并不适用所有卡。如果其他卡有特殊要求，在其他部分来处理。tony-xiong
int GxSmc_SendReceiveData_Patch(handle_t handle,
							size_t CmdLen,
							IN const uint8_t *pCmd,
							OUT uint8_t *pResponse,
							OUT size_t *RespLen,
							OUT uint8_t *pSW1,
							OUT uint8_t *pSW2)
{
	int  len = 0;
#define SMC_MAX_REPLY			(1024)
#define	SMC_MAX_CMD_LEN			(512)/*最大到命令长度限定 */
    uint8_t cmd[SMC_MAX_CMD_LEN];
	static uint8_t reply[SMC_MAX_REPLY];
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	ASSERT(pCmd != NULL);
	ASSERT(pResponse != NULL);
	ASSERT(RespLen != NULL);
	ASSERT(pSW1 != NULL);
	ASSERT(pSW2 != NULL);
	//GXSMC_CHECK_H(handle);

	memset(cmd,0,sizeof(cmd));
	memcpy(cmd,pCmd,CmdLen);
	DEBUG(SMC,INFO,"Send Command \n");
	DEBUG(SMC,INFO,"Command len = %d:\n",CmdLen);
	DEBUG(SMC,INFO,"Command info:\n");
	DUMP(SMC,INFO,CmdLen,cmd,"%x");

    *RespLen = 0;
    *pSW1 = *pSW2 = 0;
	if(CmdLen <=5 )
	{
        DEBUG(SMC,INFO,"Cmdlen <= 5!\n");
		len = GxSmc_SendCmd(handle, cmd, CmdLen);
		if (len == CmdLen)
		{
			DEBUG(SMC,INFO,"Send Command OK\n");
			len = GxSmc_GetReply(handle, reply, SMC_MAX_REPLY);
			DEBUG(SMC,INFO,"Reply len = %d\n",len);

			if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
			{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
				if(len>=2)
				{
					DEBUG(SMC,INFO,"sw1 = %#x, sw2 = %#x\n",reply[len - 2],reply[len - 1]);
				}
				else if(len == 1)
				{
					DEBUG(SMC,INFO,"reply[0]=0x%x\n",reply[0]);
					reply[1] = 0x90;
					reply[2] = 0x00;
					len = 3;
				}
#endif
			}
			else
			{	
				DEBUG(SMC,INFO,"sw1 = %#x, sw2 = %#x\n",reply[len - 2],reply[len - 1]);
			}

			*pSW1 = reply[len - 2];
			*pSW2 = reply[len - 1];
			if (len == 2)
			{
				/*ACK*/
				//if (reply[0] == 0x61)
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
				if (reply[0] == 0x61)
#else
					if(0)
#endif			
					{
						cmd[0] = 0x00;
						cmd[1] = 0xc0;
						cmd[2] = 0x00;
						cmd[3] = 0x00;
						cmd[4] = *pSW2;
						CmdLen = 5;
						DEBUG(SMC,INFO,"Recv sw1 0x61, send cmd to get response!\n");
						len = GxSmc_SendCmd(handle, cmd, CmdLen);
						if(len != CmdLen)
							DEBUG(SMC,ERR,"Send cmd again failure\n");

						len = GxSmc_GetReply(handle, reply, SMC_MAX_REPLY);
						if(len == 2)
						{
							*RespLen = 0;
							*pSW1 = reply[len - 2];
							*pSW2 = reply[len - 1];
							return 0;
						}
						else if (len > 2)
						{
							if (reply[0] == cmd[1])
							{
								/*INS*/
								DEBUG(SMC,INFO,"Sw1 is the INS\n");
								*RespLen = (unsigned short)len - 3;
								memcpy(pResponse, reply + 1, *RespLen);
							}
							else
							{
								*RespLen = (unsigned short)len - 2;
								memcpy(pResponse, reply, *RespLen);
							}
							DUMP(SMC,INFO,*RespLen,pResponse,"%x");
							return 0;
						}
						else
						{
							DEBUG(SMC,ERR,"Get reply failure\n");
							return -1;
						}
					}
			}
			else if (len > 2)
			{
				if (reply[0] == cmd[1])
				{
					/*INS*/
					DEBUG(SMC,INFO,"Sw1 is the INS\n");
					*RespLen = (unsigned short)len - 3;
					memcpy(pResponse, reply + 1, *RespLen);
				}
				else
				{
					*RespLen = (unsigned short)len - 2;
					memcpy(pResponse, reply, *RespLen);
				}
			}
			else
			{
				DEBUG(SMC,ERR,"Get reply failure\n");
				return -1;
			}
			DEBUG(SMC,INFO,"SW1=%#x,SW2=%#x\n", *pSW1, *pSW2);
			DEBUG(SMC,INFO,"Reply info = %d:\n",*RespLen);
			DUMP(SMC,INFO,*RespLen,pResponse,"%x");
			return 0;
		}
		DEBUG(SMC,ERR,"Send cmd failure\n");
		return -1;
	}
	else
	{
		DEBUG(SMC,INFO,"Cmdlen > 5!send 5 bytes first!\n");
		len = GxSmc_SendCmd(handle, cmd, 5);
		if (len == 5)
		{
			DEBUG(SMC,INFO,"Send Command OK\n");
			len = GxSmc_GetReply(handle, reply, SMC_MAX_REPLY);
			DUMP(SMC,INFO,len,reply,"%x");
			if(cmd[1] == reply[0])
			{
				DEBUG(SMC,INFO,"Sw1 is the INS\n");
				len = GxSmc_SendCmd(handle, cmd  + 5, CmdLen - 5);
				if(len != CmdLen - 5)
					DEBUG(SMC,ERR,"Send cmd again failure\n");

				len = GxSmc_GetReply(handle, reply, SMC_MAX_REPLY);
				DEBUG(SMC,INFO,"Get reply len = %d\n",len);
				if(len > 0)
				{
					if(2 == len)
					{
						/*ACK*/
						//if (reply[0] == 0x61)
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
						if (reply[0] == 0x61)
#else
							if(0)
#endif			
							{
								cmd[0] = 0x00;
								cmd[1] = 0xc0;
								cmd[2] = 0x00;
								cmd[3] = 0x00;
								cmd[4] = reply[1];
								CmdLen = 5;
								DEBUG(SMC,INFO,"Recv sw1 0x61, send cmd to get response\n");

								len = GxSmc_SendCmd(handle, cmd, CmdLen);
								if(len != CmdLen)
									DEBUG(SMC,ERR,"Send cmd again failure\n");

								len = GxSmc_GetReply(handle, reply, SMC_MAX_REPLY);
								if(len == 2)
								{
									*RespLen = 0;
									*pSW1 = reply[len - 2];
									*pSW2 = reply[len - 1];
									return 0;
								}
								else if (len > 2)
								{
									*pSW1 = reply[len - 2];
									*pSW2 = reply[len - 1];
									if (reply[0] == cmd[1])
									{
										/*INS*/
										DEBUG(SMC,INFO,"Sw1 is the INS\n");
										*RespLen = (unsigned short)len - 3;
										memcpy(pResponse, reply + 1, *RespLen);
									}
									else
									{
										*RespLen = (unsigned short)len - 2;
										memcpy(pResponse, reply, *RespLen);
									}
									DEBUG(SMC,INFO,"SW1=%#x,SW2=%#x\n", *pSW1, *pSW2);
									DEBUG(SMC,INFO,"Reply info = %d:\n",*RespLen);
									DUMP(SMC,INFO,*RespLen,pResponse,"%x");
									return 0;
								}
								else
								{
									DEBUG(SMC,ERR,"Get reply failure\n");
									return -1;
								}
							}
							else
							{
								*pSW1 = reply[0];
								*pSW2 = reply[1];
								DEBUG(SMC,INFO,"SW1=%#x,SW2=%#x\n", *pSW1, *pSW2);
								return 0;
							}
					}
					else if(len > 2)
					{
						*pSW1 = reply[len - 2];
						*pSW2 = reply[len - 1];
						memcpy(pResponse, reply, len - 2);
						*RespLen = len - 2;
						DEBUG(SMC,INFO,"SW1=%#x,SW2=%#x\n", *pSW1, *pSW2);
						return 0;
					}
					else
					{
						DEBUG(SMC,ERR,"Get reply failure\n");
						return -1;
					}
				}
				DEBUG(SMC,ERR,"Get reply failure\n");
				return -1;
			}
			return 0;
		}
	}
	DEBUG(SMC,INFO,"Send Command err\n");
	return -1;
}

uint8_t app_porting_ca_smart_sc_apdu( const uint8_t* pbyCommand,
		uint16_t       wCommandLen,
		uint8_t*       pbyReply,
		uint16_t*      pwReplyLen  )
{
	uint8_t sw1,sw2;
	size_t len;
	int count = 2;
	size_t cmd_len = wCommandLen;
	if (0 == smart_handle)
	{
		*pwReplyLen = 0;
		return FALSE;		
	}

	GxCore_SemWait(g_SCCardSem);
	while(1)
	{
		if (count -- < 0)
			break;	
		if (GxSmc_SendReceiveData_Patch(smart_handle,cmd_len,pbyCommand,pbyReply,&len,&sw1,&sw2) >= 0)
		{
			if (sw1<0x60)
			{
				//CAS_Dbg("\n app_porting_ca_smart_sc_apdu sw1=0x%x\n",sw1);
				continue;
			}

			*pwReplyLen = (uint16_t)len;
			pbyReply[len]=sw1;
			pbyReply[len+1]=sw2;
			*pwReplyLen +=2;
			GxCore_SemPost(g_SCCardSem);
			return TRUE;
		}
		else
		{
			CAS_Dbg("\n app_porting_ca_smart_sc_apdu err\n");

		}

	}

	CAS_Dbg("\n app_porting_ca_smart_sc_apdu Failed\n\n\n\n");
	*pwReplyLen = 0;
	//	*pbyReply = 0;
	GxCore_SemPost(g_SCCardSem);
	return FALSE;

}

uint8_t app_porting_ca_smart_sc_apdu61_tr( const uint8_t* pbyCommand,
		uint16_t       wCommandLen,
		uint8_t*       pbyReply,
		uint16_t*      pwReplyLen  )
{
	uint8_t sw1,sw2;
	size_t len;
	int count = 2;
	size_t cmd_len = wCommandLen;
	int i = 0;
	static uint8_t reply[1024];
	if (0 == smart_handle)
	{
		*pwReplyLen = 0;
		return FALSE;		
	}

	GxCore_SemWait(g_SCCardSem);
	while(1)
	{
		if (count -- < 0)
			break;	

		if (GxSmc_SendReceiveData(smart_handle,cmd_len,pbyCommand,reply,&len,&sw1,&sw2) >= 0)
		{
			if (sw1<0x60)
			{
				//CAS_Dbg("\n app_porting_ca_smart_sc_apdu sw1=0x%x\n",sw1);
				continue;
			}
			if ((cmd_len == 5)&&(pbyCommand[4]==len))	
			{
				
			
			}
			else
			{
				if( len>0 )	
				{
					for(i=0;i<len;i++ )
					{
						if (reply[i] != 0x60)
							break;
					}
				}
			}	
			
			len = len -i;
			*pwReplyLen = (uint16_t)len;
			memcpy(pbyReply,reply+i,len);
			pbyReply[len]=sw1;
			pbyReply[len+1]=sw2;
			*pwReplyLen +=2;
			GxCore_SemPost(g_SCCardSem);
			return TRUE;
		}

	}

	CAS_Dbg("\n app_porting_ca_smart_sc_apdu Failed\n\n\n\n");
	*pwReplyLen = 0;
	//	*pbyReply = 0;
	GxCore_SemPost(g_SCCardSem);
	return FALSE;

}

uint8_t app_porting_ca_smart_sc_apdu61( const uint8_t* pbyCommand,
								  uint16_t       wCommandLen,
								  uint8_t*       pbyReply,
								  uint16_t*      pwReplyLen  )
{
	uint8_t sw1,sw2;
	size_t len;
	int count = 2;
	size_t cmd_len = wCommandLen;
	if (0 == smart_handle)
	{
		*pwReplyLen = 0;
		return FALSE;		
	}

	GxCore_SemWait(g_SCCardSem);
	while(1)
	{
		if (count -- < 0)
			break;	

		if (GxSmc_SendReceiveData(smart_handle,cmd_len,pbyCommand,pbyReply,&len,&sw1,&sw2) >= 0)
		{
			if (sw1<0x60)
			{
				//CAS_Dbg("\n app_porting_ca_smart_sc_apdu sw1=0x%x\n",sw1);
				continue;
			}
		
			*pwReplyLen = (uint16_t)len;
            pbyReply[len]=sw1;
			pbyReply[len+1]=sw2;
            *pwReplyLen +=2;
			GxCore_SemPost(g_SCCardSem);
			return TRUE;
		}

	}
	
	CAS_Dbg("\n app_porting_ca_smart_sc_apdu Failed\n\n\n\n");
	*pwReplyLen = 0;
//	*pbyReply = 0;
	GxCore_SemPost(g_SCCardSem);
	return FALSE;

}
//智能卡监测任务
static void app_porting_ca_smart_task(void* args)
{
	while(1)
	{
		/*
		 * check card
		 */
		app_porting_ca_smart_check_status();//状态监测
#ifdef DVB_CA_TYPE_QILIAN_FLAG
		if(app_porting_ca_smart_status() !=1)
		{
			STBCA_ShowOSDMessage(3);
		}
#endif
		GxCore_ThreadDelay(200);
	}
}

void app_porting_ca_smart_create_task(void)
{
	handle_t    smart;
	GxCore_ThreadCreate("smart",&smart, app_porting_ca_smart_task, NULL, 10 * 1024, GXOS_DEFAULT_PRIORITY);
	return;	
}




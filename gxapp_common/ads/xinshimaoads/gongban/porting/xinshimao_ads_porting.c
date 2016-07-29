/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	xinshimao_ads_porting.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	Ã„Â£Â¿Ã©ÃÂ·ÃŽÃ„Â¼Ã¾
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.11.05		  zhouhm 	 			creation
*****************************************************************************/
//#include "xinshimao_ca.h"
#include "ads_api_xsm_2.0.h"
#include "app_common_porting_ads_flash.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_ca_os.h"
//#include "app_common_porting_ca_smart.h"
//#include "app_common_porting_ca_descramble.h"
#include "app_common_prog.h"
#include "app_common_play.h"
#include "app_common_epg.h"
//#include "app_xinshimao_ads_api_demux.h"
#include "app_common_porting_stb_api.h"
#include "gxprogram_manage_berkeley.h"
#include "gxplayer_url.h"
#include "app_xinshimao_ads_porting_stb_api.h"
//#define ONE_SECTOR_SIZE (64*1024)
#define XINSHIMAO_ADS_STARTADDR (64*1024)
#define XINSHIMAO_ADS_SIZE (2*64*1024)



char buf1[1024]={0};
uint8_t chAdFlashTmp[XINSHIMAO_ADS_SIZE] = {0, };
extern u_int8_t pic_data_buffer[MAX_PIC_AD_SIZE];
extern void app_xinshimao_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
extern void app_xinshimao_ads_item_reset(uint16_t PID, uint8_t TableId,uint8_t  ProgNo);

void* YTDY_AD_Malloc(YTDY_U32 byBufSize)//????
{
	return app_porting_ca_os_malloc(byBufSize);
	
}


void YTDY_AD_Free(void* pBuf)//ÊÍ·ÅÄÚ´æ
{
	app_porting_ca_os_free(pBuf);
	return;	
}

void YTDY_AD_Memset(void* pDestBuf,YTDY_U8 c,YTDY_U32 wSize)//ÄÚ´æÉèÖÃ
{
	memset(pDestBuf,c,wSize);
	return ;
}

void YTDY_AD_Memcpy(void* pDestBuf,const void* pSrcBuf,YTDY_U32 wSize)//ÄÚ´æ¿½±´
{
	memcpy(pDestBuf,pSrcBuf,wSize);
	return;
}

void YTDY_AD_Get_local_time( YTDY_AD_TIME_T *  ad_time)//»ñÈ¡µ±Ç°TDT±íÖÐ´«ÊäµÄÊ±¼ä
{
	struct tm LocalTime;
	app_epg_get_local_time(&LocalTime,TRUE);
	ad_time->day = LocalTime.tm_mday;
	ad_time->hour = LocalTime.tm_hour;
	ad_time->minute = LocalTime.tm_min;
	ad_time->month = LocalTime.tm_mon;
	ad_time->second = LocalTime.tm_sec;
	ad_time->year = LocalTime.tm_year;
	return ;
}

void mtos_printk(const char *fmt, ...)//´òÓ¡
{
	return;
	va_list args;
	int i;
	char buf[1024]={0};


	va_start(args,fmt);
	i = Vsnprintf(buf,sizeof(buf), fmt, args);
	va_end(args);

	printf("%s", buf);
	return;	
}


void YTDY_AD_Printf(const char *fmt, ...)//´òÓ¡
{
	return;

	va_list args;
	int i;

	va_start(args,fmt);
	i = Vsnprintf(buf1,sizeof(buf1), fmt, args);
	va_end(args);

	printf("%s", buf1);
	return;
}

YTDY_AD_ERR_E YTDY_AD_RegisterTask(YTDY_S8* szName, YTDY_U8 byPriority,
	    void* pTaskFun,void* pParam, YTDY_U16 nStackSize)//ÈÎÎñ×¢²á
{


    int         ret;
    handle_t    handle;
    uint32_t priority = GXOS_DEFAULT_PRIORITY;

	if ((NULL == szName)||(NULL == pTaskFun))
	{
		return YTDY_AD_ERR_UNKNOWN;
	}

	printf("YTDY_AD_RegisterTask szName=%s\n",szName);
	priority = GXOS_DEFAULT_PRIORITY;

	ret = app_porting_ca_os_create_task((char*)szName, &handle, pTaskFun,
                          pParam, nStackSize, priority);
	if (ret != GXCORE_SUCCESS)
	{
		CA_FAIL("YTDY_AD_RegisterTask return error = 0x%x\n", ret);
		return YTDY_AD_ERR_UNKNOWN;
	}
	
	return YTDY_AD_SUCCESS;
}
void YTDY_AD_TaskSleep( unsigned long ms )
{
	app_porting_ca_os_sleep(ms);
	return;
}

extern YTDY_U32 YTDY_AD_section_cancel(YTDY_U32 LogicSlot)//È¡Ïû¹ýÂË LogicSlot ¹ýÂËÆ÷µÄÂß¼­ºÅ
{
	ca_filter_t sfilter = {0};

	/*
	* ²éÕÒ¶ÔÓ¦channel id , filter id
	*/
	
	if (LogicSlot >= MAX_FILTER_COUNT)
		{
			printf("%s LogicSlot=%d error\n",__FUNCTION__,(unsigned int)LogicSlot);
			return 1;
		}

	if (LogicSlot < 0)
		{
			printf("%s LogicSlot=%d error\n",__FUNCTION__,(unsigned int)LogicSlot);
			return 1;
		}
	printf("%s LogicSlot=%d\n",__FUNCTION__,(unsigned int)LogicSlot);

	app_porting_ca_demux_get_by_filter_id(LogicSlot,&sfilter);
	if (( 0 != sfilter.usedStatus)&&(0 != sfilter.handle)&&(0 != sfilter.channelhandle)
		/*&&(LogicSlot == sfilter->byReqID)*/)
	{
		  app_porting_ca_demux_release_filter( LogicSlot, TRUE);
		  return 0;
	}

	return 1;
}

YTDY_S32  YTDY_AD_Section_req(YTDY_U16 PID, YTDY_U8 TableId,YTDY_U16  ProgNo)
{
	if ( PID > 0 && PID < 0x1fff )
		{
			ca_filter_t filter = {0};
			uint8_t 	match[18] = {0};
			uint8_t 	mask[18] = {0};
			match[0]= TableId;
			mask[0]= 0xff;
			match[3] = (ProgNo>>8);
			mask[3] = 0xff;
			match[4] = ProgNo&0xff;
			mask[4] = 0xff;
			uint32_t filterId = 0;
			ca_filter_t* sfilter = NULL;


			printf("%s PID=0x%x TableId=0x%x ProgNo=0x%x\n",
				__FUNCTION__,PID,TableId,ProgNo);
			
			app_xinshimao_ads_item_reset(PID, TableId,ProgNo);

			/*
			* check same filter exist or not
			*/
			for (filterId = 0; filterId< MAX_FILTER_COUNT;filterId++)
			{
				sfilter = app_porting_ca_demux_get_by_filter_id(filterId,&filter);
				if (( 0 != filter.usedStatus)&&(0 != filter.handle)&&(0 != filter.channelhandle)
					&&(PID == filter.pid)&&(TableId == filter.match[0])
					&&((ProgNo>>8) == filter.match[3])&&((ProgNo&0xff) == filter.match[4]))
				{
					printf("%s PID = 0x%x TableId=0x%x ProgNo=0x%x exist already\n",
						__FUNCTION__,PID,TableId,ProgNo);
					  return filter.byReqID;
				}
			}

			memset(&filter,0,sizeof(ca_filter_t));

			filter.byReqID = 0;
			filter.crcFlag = TRUE;
			filter.filterLen = 5;
			memcpy(filter.match,match,5);
			memcpy(filter.mask,mask,5);
			filter.nWaitSeconds = 0;	
			filter.pid = PID;
			filter.equalFlag = TRUE;
			filter.repeatFlag = TRUE;
			filter.Callback = app_xinshimao_ads_filter_notify;
			filter.timeOutCallback = NULL;
			if (TRUE == app_porting_ca_demux_start_filter(&filter))
				{
					CAS_Dbg("%s byReqID=%d\n",__FUNCTION__,filter.byReqID);		
					return 	filter.byReqID;
				}
			else
				{
					return -1;
				}
			
		}

	return -1;
}


void YTDY_AD_InitBuffer(YTDY_U32 *Start_adr, YTDY_U32 *Size)//FLASH ?????  ??FLASH ?? 128K
{
	if ((NULL == Start_adr)||(NULL ==Size ))
		{
		  	CAS_Dbg("YTDY_AD_InitBuffer Size=0x%x Start_adr=0x%x\n",(unsigned int)Size,(unsigned int)Start_adr);		
			return ;
		}

	
    *Start_adr = XINSHIMAO_ADS_STARTADDR;
	*Size = XINSHIMAO_ADS_SIZE;
	return ;	
}


void YTDY_AD_ReadBuffer(YTDY_U8* pAdvData,YTDY_U32 *DataLen,YTDY_U32 *DataCRC)//FLASH ???
{
	uint32_t  Offset = 0;
	uint32_t len = XINSHIMAO_ADS_SIZE;


   if ((NULL ==pAdvData )||(NULL == DataLen)||(NULL == DataCRC))
   	{
	  	CAS_Dbg("%s pAdvData=0x%x DataLen=0x%x DataCRC=0x%x error\n",__FUNCTION__,(unsigned int)pAdvData,(unsigned int)DataLen,(unsigned int)DataCRC);		       	
		return ;
   	}

//   memset((uint8_t *)chAdFlashTmp,0,XINSHIMAO_ADS_SIZE);
   app_porting_ads_flash_read_data(Offset,(uint8_t *)chAdFlashTmp,&len);
	memcpy(DataLen ,chAdFlashTmp, 4);
	printf("%s chAdFlashTmp[0]=0x%x chAdFlashTmp[1]=0x%x chAdFlashTmp[2]=0x%x chAdFlashTmp[3]=0x%x \n",
		__FUNCTION__,chAdFlashTmp[0], chAdFlashTmp[1], chAdFlashTmp[2],chAdFlashTmp[3] );
	if((*DataLen !=0xFFFFFFFF) && (*DataLen < XINSHIMAO_ADS_SIZE) && (*DataLen>0))
	{
		if (*DataLen > XINSHIMAO_ADS_SIZE)
			{
//				pAdvData = NULL;
				*DataLen = 0; 
				*DataCRC = 0;			
			}
		else
			{
				memcpy(DataCRC ,chAdFlashTmp+4, 4);
				memcpy(pAdvData ,chAdFlashTmp+8, *DataLen);		
			}
	}
	else
	{
//		pAdvData = NULL;
		*DataLen = 0; 
		*DataCRC = 0;
	}

	CAS_Dbg("YTDY_AD_ReadBuffer OK~~~ DataLen = 0x%x DataCRC = %d\n",(unsigned int)*DataLen , (unsigned int)*DataCRC);			


	return;

	
}

void YTDY_AD_WriteBuffer(YTDY_U8* pAdvData,YTDY_U32 DataLen,YTDY_U32 DataCRC)//¾ÉµÄÊý¾Ý±»²Á³ýÁËå?å?
{
	uint32_t len = XINSHIMAO_ADS_SIZE;

	if (DataLen > XINSHIMAO_ADS_SIZE)
		{
			printf("%s DataLen=0x%x DataCRC=0x%x\n",__FUNCTION__,(unsigned int)DataLen,(unsigned int)DataCRC);
			return;
		}

	CAS_Dbg("YTDY_AD_WriteBuffer~~~ DataLen = 0x%x DataCRC = %d  adr = %x \n", (unsigned int)DataLen , (unsigned int)DataCRC, XINSHIMAO_ADS_STARTADDR);

	if (NULL ==pAdvData )
	 {
		 CAS_Dbg("YTDY_AD_WriteBuffer pAdvData=0x%x\n",(unsigned int)pAdvData);			 
		 return ;
	 }

//	memset((uint8_t *)chAdFlashTmp,0,XINSHIMAO_ADS_SIZE);
	memcpy(chAdFlashTmp, &DataLen, 4);
	printf("%s chAdFlashTmp[0]=0x%x chAdFlashTmp[1]=0x%x chAdFlashTmp[2]=0x%x chAdFlashTmp[3]=0x%x \n",
		__FUNCTION__,chAdFlashTmp[0], chAdFlashTmp[1], chAdFlashTmp[2],chAdFlashTmp[3] );
	memcpy(chAdFlashTmp+4, &DataCRC, 4);
	memcpy(chAdFlashTmp+8, pAdvData, DataLen);

	app_porting_ads_flash_write_data(0,(uint8_t *)chAdFlashTmp,len);

	CAS_Dbg("YTDY_AD_WriteBuffer OK~~~ \n");
	app_xinshimao_ads_play(AD_TYPE_OPEN_LOGO);
	return;
	
}

void  YTDY_AD_Upcall_Back(YTDY_U16  ad_id, YTDY_U8 show_or_hide, YTDY_U32 res2) //???????????
{
	uint16_t ts_id = 0;
	uint16_t ser_id = 0;

	YTDY_AD_ERR_E nRet = 0;
	GxBusPmDataProg Prog;
	uint32_t pos;
	S_YTDY_AD_SHOW_DATA_T s_ad_data;//È«¾Ö±äÁ¿
	printf("%s ad_id=0x%x\n",__FUNCTION__,ad_id);
	E_YTDY_AD_SHOW_TYPE show_type = ad_id & 0xf000;

	printf("%s show_type=0x%x\n",__FUNCTION__,show_type);
	switch(show_type)
		{
			case AD_NONE:
				break;
			case AD_ROLL_PIC:
				break;
			case AD_OPEN_LOGO:
				/*
				* save open logo ad to flash
				*/
				{
					if (app_prog_get_num_in_group()>0)
						{
							app_prog_get_playing_pos_in_group(&pos);
							GxBus_PmProgGetByPos(pos,1,&Prog);
							ser_id = Prog.service_id;
							ts_id = Prog.ts_id; 						
						}
					s_ad_data.p_pic_data = pic_data_buffer;

					nRet = YTDY_AD_GetPicture(show_type, ser_id, ts_id, &s_ad_data);
					if(nRet == YTDY_AD_SUCCESS )
						{
							app_flash_save_logo_data((char*)s_ad_data.p_pic_data,s_ad_data.size);						
						}
		
				}
				break;
			case AD_PF_INFO:
				break;
			case AD_FS_RIGHT:	
				break;
			case AD_EPG:					
				break;
			case AD_CH_LIST:
				break;
			case AD_MAIN_MENU:
				break;
			case AD_RADIO_LOGO:
			case AD_FAV_LIST:
			case AD_NVOD:
			case AD_VOLUME:
			case AD_ROLLING_PIC:
				break;
			default:
				break;
		}
	
}








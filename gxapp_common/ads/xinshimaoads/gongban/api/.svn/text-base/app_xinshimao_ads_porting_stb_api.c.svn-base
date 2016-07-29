/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_xinshimao_ads_porting_stb_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.11.06		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_common_porting_ads_flash.h"
#include "app_xinshimao_ads_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "ads_api_xsm_2.0.h"
handle_t ads_handle={0};

ad_play_para xinshimaoadspara[AD_TYPE_MAX];

//#define MAX_ITEMS_COUNT (3)
#define MAX_ITEMS_COUNT (20)

typedef struct ad_xinshimao_section_s
{
	uint8_t      recievedFlag;
	uint8_t*     addata;
	int32_t         len;
}ad_xinshimao_section_t;

#define SECTION_MAX_NUM (/*60*/128)
typedef struct ad_xinshimao_items_s
{
	uint16_t  pid;
	uint8_t   tableId;
	uint16_t  progNo;
	uint8_t	  lastSectionNum;
	uint8_t	  recieveNum;
	ad_xinshimao_section_t sectionData[SECTION_MAX_NUM];
}ad_xinshimao_items_t;
ad_xinshimao_items_t xinshimao_adItems[MAX_ITEMS_COUNT];



void app_xinshimao_ads_init(void)
{
	app_porting_ads_flash_init(2*64*1024);
	memset(&xinshimaoadspara[0],0,AD_TYPE_MAX*sizeof(ad_play_para));
	memset(&xinshimao_adItems[0],0,MAX_ITEMS_COUNT*sizeof(ad_xinshimao_items_t));
	advertisement_play_init();
	YTDY_AD_init();
	YTDY_AD_Open();	
	return;
}

 void app_xinshimao_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
 {
	 uint16_t			 filterID;
	 uint16_t			 i;
	 int16_t			pid;
	 uint8_t			 secnum= 0;
	 uint8_t			 lastsecnum= 0;
	 uint8_t			 cur= 0;
	 uint16_t			 section_length;
	 uint16_t			 progNum=0;
	 bool bFlag = FALSE;
	 uint8_t byReqID = 0;
	 uint8_t*			 data = (uint8_t*)Section;
	 int				 len = Size;
	 ca_filter_t sfilter = {0};
	 int32_t ret;
 
 //    ASSERT(Section != NULL);
	ret = GxDemux_FilterGetPID(Filter,&pid);
 
			 while(len > 0) {
				 bFlag = FALSE;
				 section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
			 for (filterID = 0; filterID< MAX_FILTER_COUNT; filterID++)
			 {
				 app_porting_ca_demux_get_by_filter_id(filterID,&sfilter);
				 if (( 0 !=sfilter.usedStatus)&&(pid == sfilter.pid)
					 &&(0 !=sfilter.channelhandle))
				 {
					 if (Filter == sfilter.handle)
						 {
							 /*NIT表或相应FILTER数据*/
							 for (i = 0; i<sfilter.filterLen; i++)
							 {
								 if ((data[i]&sfilter.mask[i]) != (sfilter.match[i]&sfilter.mask[i]))
									 {
										 CA_FAIL(" data[%d]=0x%x cafilter[filterID].match[%d]=0x%x \n",
											 i,data[i],i,sfilter.match[i]);
										 return;
									 }

								 
							 }
 
 // 						 sfilter->nms = 0;
							 byReqID = sfilter.byReqID;
							 bFlag = TRUE;
							 break;
						 }
				 }
			 }
 
			 if (FALSE == bFlag)
			 {
				 CA_FAIL(" FALSE == bFlag\n");
				 return;
			 }

 
			 
//			if(( data[0] == sfilter->match[0])&&(data[3] == sfilter->match[3])&&(data[4] == sfilter->match[4]))
//			 {
//				 YTDY_AD_PrivateDataGot(pid, data, section_length);
//			 }
				progNum = (data[3]<<8)|data[4];
				secnum = data[6];
				lastsecnum = data[7];

				for (cur = 0;cur<MAX_ITEMS_COUNT;cur++)
					{
						if ((xinshimao_adItems[cur].pid == pid)
							&&(xinshimao_adItems[cur].tableId== data[0])
							&&(xinshimao_adItems[cur].progNo == progNum))
							{
								break;
							}
					}
				
				if (cur>=MAX_ITEMS_COUNT)
					{
						for (cur = 0;cur<MAX_ITEMS_COUNT;cur++)
							{
								if ((0 == xinshimao_adItems[cur].pid)
									&&(0 == xinshimao_adItems[cur].tableId)
									&&(0 == xinshimao_adItems[cur].progNo))
									{
										xinshimao_adItems[cur].pid= pid;
										xinshimao_adItems[cur].tableId=data[0];
										xinshimao_adItems[cur].progNo=progNum;
										break;
									}
							}
					}

				
				if (cur<MAX_ITEMS_COUNT)
					{	
						if (lastsecnum > SECTION_MAX_NUM)
							{
								printf("ad_data_got  lastSectionNum=%d \n",lastsecnum);
								return ;
							}
						
						xinshimao_adItems[cur].lastSectionNum = lastsecnum;
						if (0 == xinshimao_adItems[cur].sectionData[secnum].recievedFlag)
							{
								xinshimao_adItems[cur].sectionData[secnum].recievedFlag=1;
								xinshimao_adItems[cur].recieveNum++;


								if (NULL != xinshimao_adItems[cur].sectionData[secnum].addata)
									{
										GxCore_Free(xinshimao_adItems[cur].sectionData[secnum].addata);
										xinshimao_adItems[cur].sectionData[secnum].addata = NULL;
									}
								xinshimao_adItems[cur].sectionData[secnum].addata= GxCore_Malloc(section_length);
								if (NULL == xinshimao_adItems[cur].sectionData[secnum].addata)
									{
										printf("g_adItems[%d].sectionData[%d].data =NULL\n",cur,secnum);
										return ;			
									}
								
								memcpy(xinshimao_adItems[cur].sectionData[secnum].addata,data,section_length);
								
								xinshimao_adItems[cur].sectionData[secnum].len = section_length; 

	
							}
					
					}			
 
				 data += section_length;
				 len -= section_length;
			 }

		if ((cur < MAX_ITEMS_COUNT)&&(TRUE == bFlag))
			{
//				printf("cur = %d recieveNum=%d lastSectionNum=%d\n",
//				cur,xinshimao_adItems[cur].recieveNum,xinshimao_adItems[cur].lastSectionNum);
				 if (xinshimao_adItems[cur].recieveNum == (xinshimao_adItems[cur].lastSectionNum+1))
					 {
//						printf("cur = %d recieveNum=%d lastSectionNum=%d\n",
//						cur,xinshimao_adItems[cur].recieveNum,xinshimao_adItems[cur].lastSectionNum);					 	
						 for (secnum=0;secnum<=xinshimao_adItems[cur].lastSectionNum;secnum++)
						  {
//							  YTDY_AD_PrivateDataGot(pid, data, section_length);
							  section_length = ((xinshimao_adItems[cur].sectionData[secnum].addata[1] & 0x0F) << 8) + xinshimao_adItems[cur].sectionData[secnum].addata[2] + 3;
//						  	  printf("section_length=%d\n",section_length);
							  YTDY_AD_PrivateDataGot(pid, xinshimao_adItems[cur].sectionData[secnum].addata, section_length);

						  }
						 
						 for (secnum = 0; secnum<xinshimao_adItems[cur].lastSectionNum+1;secnum++ )
						 {
								 if (NULL !=xinshimao_adItems[cur].sectionData[secnum].addata )
									 {
										 GxCore_Free(xinshimao_adItems[cur].sectionData[secnum].addata);
										 xinshimao_adItems[cur].sectionData[secnum].addata = NULL;
									 }
						 }
						 xinshimao_adItems[cur].lastSectionNum =0;
						 xinshimao_adItems[cur].recieveNum =0;
						 xinshimao_adItems[cur].pid =0;
						 xinshimao_adItems[cur].tableId =0;
						 xinshimao_adItems[cur].progNo =0;
						 memset(&xinshimao_adItems[cur].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_xinshimao_section_t));
		 
					 }				
			}

		 return;
 
		 
 }

 void app_xinshimao_ads_item_reset(uint16_t PID, uint8_t TableId,uint8_t  ProgNo)
{
	uint16_t i = 0;
	uint16_t secnum = 0;
	
	for (i = 0; i<MAX_ITEMS_COUNT;i++ )
		{
			if ((PID == xinshimao_adItems[i].pid)
				&&(TableId == xinshimao_adItems[i].tableId)
				&&(ProgNo == xinshimao_adItems[i].progNo))
				{
					for (secnum = 0; secnum<xinshimao_adItems[i].lastSectionNum+1;secnum++ )
					{
						if (NULL !=xinshimao_adItems[i].sectionData[secnum].addata )
							{
								GxCore_Free(xinshimao_adItems[i].sectionData[secnum].addata);
								xinshimao_adItems[i].sectionData[secnum].addata = NULL;
							}
					}
					xinshimao_adItems[i].lastSectionNum =0;
					xinshimao_adItems[i].recieveNum =0;
					xinshimao_adItems[i].pid =0;
					xinshimao_adItems[i].tableId =0;
					xinshimao_adItems[i].progNo =0;
					memset(&xinshimao_adItems[i].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_xinshimao_section_t));
					break;
				}
		}
	return;
}









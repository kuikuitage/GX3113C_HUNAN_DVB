/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.06.26		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "adver.h"
#include "adverparse.h"
#include "bmpparse.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_by_ads_porting_stb_api.h"

unsigned int gAdverProgramId = 0;
u_int16_t adverPID = 0x1fff;
static handle_t by_ads_filter =0;
static handle_t by_ads_channel=0;

SaveAdverCorner_Info gSaveAdverCorner_Info;
SaveAdverScroll_Info gSaveAdverScroll_Info;

void AdverDisplay_StopType(unsigned char bType)
{
	switch(bType)
		{
			case FONT_MOVE_PICTURE:
				app_by_ads_stop_scroll_bmp();
				break;
			case CORNER_PICTURE:
				app_by_ads_stop_corner_bmp();
				app_by_ads_stop_corner_gif();
				break;
			default:
				break;
		}
	printf("AdverDisplay_StopType bType=%d\n",bType);

	return;
}

void app_by_ads_init(void)
{
	advertisement_play_init();
	memset(&gSaveAdverCorner_Info,0,sizeof(SaveAdverCorner_Info));
	memset(&gSaveAdverScroll_Info,0,sizeof(SaveAdverScroll_Info));	
	return;
}

handle_t app_by_ads_get_filter_handle(void)
{
	return by_ads_filter;
}

handle_t app_by_ads_get_channel_handle(void)
{
	return by_ads_channel;
}


int app_by_ads_filter_open(u_int16_t pid,unsigned int ProgId)
{
	handle_t filter;
	uint8_t  match = ADVER_TABKE_ID;
	uint8_t  mask  = 0xFF;
	int32_t ret = 0;
	CAS_Dbg("app_by_ads_filter_open\n");
	app_porting_psi_demux_lock();

	if(by_ads_channel != 0)
	{
		printf("by_ads_channel = 0x%x\n",by_ads_channel);
		app_porting_psi_demux_unlock();
		return 0;
	}

	by_ads_channel = GxDemux_ChannelAllocate(0, pid);
	if ((0 == by_ads_channel)||(-1 == by_ads_channel))
		{
			CA_ERROR("  by_ads_channel=0x%x\n",by_ads_channel);
			by_ads_channel = 0;
			app_porting_psi_demux_unlock();
			return 0;
		}

	filter = GxDemux_FilterAllocate(by_ads_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(by_ads_channel);
		CHECK_RET(DEMUX,ret);
		by_ads_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}

	by_ads_filter = filter;
	ret = GxDemux_FilterSetup(filter, &match, &mask, TRUE, TRUE,0, 1);
	CHECK_RET(DEMUX,ret);

	ret = GxDemux_ChannelEnable(by_ads_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);

	app_porting_psi_demux_unlock();
	return 0;
}

int app_by_ads_filter_close(void)
{
	/*
	* 关闭后台监测NIT表FILTER
	*/

	int32_t ret = 0;
	app_porting_psi_demux_lock();
	if (0 != by_ads_channel)
		{
			if(by_ads_filter != 0)
			{
//				ret = GxDemux_FilterDisable(by_ads_filter);
//				CHECK_RET(DEMUX,ret);
				ret = GxDemux_FilterFree(by_ads_filter);
				CHECK_RET(DEMUX,ret);				
				by_ads_filter = 0;
			}

			ret = GxDemux_ChannelFree(by_ads_channel);
			CHECK_RET(DEMUX,ret);
			by_ads_channel = 0;
		}
	
	app_porting_psi_demux_unlock();
	return 0;
}

 void app_by_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
 {
	 int16_t			 pid;
	 uint16_t			 section_length;
	 uint8_t*			 data = (uint8_t*)Section;
	 int				 len = Size;
	 int32_t ret;
 
 
 
 //    ASSERT(Section != NULL);
 	app_porting_psi_demux_lock();
	ret = GxDemux_FilterGetPID(Filter,&pid);

     if (by_ads_filter != Filter)
    	{
    	
			app_porting_psi_demux_unlock();
             CAS_Dbg("app_by_ads_filter_notify by_ads_filter=0x%x  Filter=0x%xerror  \n", by_ads_filter,Filter);
    		return ;
    	}
	 app_porting_psi_demux_unlock();

  
		 while(len > 0) {
			 section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
			 	if (ADVER_TABKE_ID == data[0])
			 		{
						AdverPsiParse(data,section_length);
			 		}
			 data += section_length;
			 len -= section_length;
		 }
	 return;
 
		 
 }

 void AdverPsiStart(u_int32_t AdverPid,unsigned int ProgId)
{
	app_by_ads_filter_open(adverPID,ProgId);
	return;
}

void AdverPsiStop(void)
{
	app_by_ads_filter_close();
	AdverDisplay_StopType(FONT_MOVE_PICTURE);
	AdverDisplay_StopType(CORNER_PICTURE);

	return;
}

void app_by_pmt_Private_AD_Descriptor(char* buf)
{
	gAdverProgramId = buf[2];
	adverPID = (u_int16_t)((buf[3]<<8)|buf[4]);
	printf("Private_AD_Descriptor gAdverProgramId=0x%x,adverPID=0x%x\n",
		gAdverProgramId,adverPID);
	gSaveAdverCorner_Info.mad_CRC32 = 0;
	gSaveAdverScroll_Info.mad_CRC32 = 0;
	AdverPsiStart(adverPID,gAdverProgramId);
	return ;
}

int32_t app_by_ads_gxmsg_ca_on_event_exec(GxMessage * msg)
{
	switch(msg->msg_id)
		{
			case GXMSG_CA_ON_EVENT:
			{
				GxMsgProperty0_OnEvent* event;
				event = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty0_OnEvent);
				printf("app_by_ads_gxmsg_ca_on_event_exec GXMSG_CA_ON_EVENT event->key=%d\n",event->key);
				switch (event->key) {

					case DVB_AD_SCROLL_BMP:
						app_by_ads_play_scroll_bmp();
						break;
					case DVB_AD_BMP:
						app_by_ads_play_corner_bmp();						
						break;
					case DVB_AD_GIF:
						app_by_ads_play_corner_gif();
						break;
					default:
						break;
					}
				}
			default:
				break;
		}
	return 0;
}










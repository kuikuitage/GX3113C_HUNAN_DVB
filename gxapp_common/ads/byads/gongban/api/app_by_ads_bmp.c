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


static event_list* ad_bmp_time = NULL;
ad_play_para playbmpara = {0};
static int timer_corner_bmp(void *userdata);

#define PLAY_BMP_TEST
#ifdef PLAY_BMP_TEST
void app_by_ads_test_bmp(void)
{		
	playbmpara.file_flash_path = "/theme/1.bmp";
	playbmpara.file_ddram_path = "/mnt/1.bmp";
	playbmpara.play_mode = 2;
	playbmpara.rectdest.x = 200;
	playbmpara.rectdest.y =100;
	playbmpara.rectdest.width= 0;
	playbmpara.rectdest.height= 0;
	app_play_ad_flash_file(&playbmpara);
	ad_bmp_time = create_timer(timer_corner_bmp, 10*1000, NULL,  TIMER_ONCE);												
}
#endif


void app_by_ads_stop_corner_bmp(void)
{
	if (NULL != ad_bmp_time)
		{
			advertisement_clear_frame(playbmpara.rectdest);
			memset(&playbmpara,0,sizeof(playbmpara));
			remove_timer(ad_bmp_time);
			ad_bmp_time = NULL;	
		}
}


static int timer_corner_bmp(void *userdata)
{
	AdverDisplay_StopType(CORNER_PICTURE);
    ad_bmp_time = NULL;	
	return 0;
}


void app_by_ads_play_corner_bmp(void)
{
	handle_t flash_ad_file;
	size_t	 size;
	u_int32_t duration = 0;
	u_int8_t hour,minute,second;

	if (GxCore_FileExists("/mnt/corner.inf") == 1) 
		{
			SaveAdverCorner_Info *pAdverInfo = NULL;
			pAdverInfo = GxCore_Malloc(sizeof(SaveAdverCorner_Info));
			flash_ad_file = GxCore_Open("/mnt/corner.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdverInfo, 1, sizeof(SaveAdverCorner_Info));
			GxCore_Close(flash_ad_file);
			
			playbmpara.file_flash_path = NULL;
			playbmpara.play_mode = 2;
			playbmpara.rectdest.x = pAdverInfo->mad_CornerInfo.mad_StartxPos;
			playbmpara.rectdest.y = pAdverInfo->mad_CornerInfo.mad_StartyPos;
			printf("pAdverInfo->mad_CornerInfo.mad_StartyPos=%d\n",pAdverInfo->mad_CornerInfo.mad_StartyPos);
			printf("pAdverInfo->mad_CornerInfo.mad_StartxPos=%d\n",pAdverInfo->mad_CornerInfo.mad_StartxPos);
			printf("pAdverInfo->mad_CornerInfo.mad_Width=%d\n",pAdverInfo->mad_CornerInfo.mad_Width);
			printf("pAdverInfo->mad_CornerInfo.mad_Height=%d\n",pAdverInfo->mad_CornerInfo.mad_Height);
//			printf("pAdverInfo->mad_TotleLen=%d\n",pAdverInfo->mad_TotleLen);
			
	
	
			hour = ((pAdverInfo->mad_CornerInfo.mad_Duration[0]&0xF0)>>4)*10
				+(pAdverInfo->mad_CornerInfo.mad_Duration[0]&0x0F);
			minute = ((pAdverInfo->mad_CornerInfo.mad_Duration[1]&0xF0)>>4)*10
												+(pAdverInfo->mad_CornerInfo.mad_Duration[1]&0x0F);
			second = ((pAdverInfo->mad_CornerInfo.mad_Duration[2]&0xF0)>>4)*10
												+(pAdverInfo->mad_CornerInfo.mad_Duration[2]&0x0F);
	
			duration = hour*60*60+minute*60+second;
			duration = duration*1000;
			printf("duration=%d\n",duration);
			
			if(pAdverInfo->mad_CornerInfo.mad_PicType== 1)
			{
				playbmpara.file_ddram_path = "/mnt/corner.bmp";
	
			}
			else  if(pAdverInfo->mad_CornerInfo.mad_PicType== 2)
			{
				playbmpara.file_ddram_path = "/mnt/corner.gif";
			}
								
			app_play_ad_ddram_file(&playbmpara);
			ad_bmp_time = create_timer(timer_corner_bmp, duration, NULL,  TIMER_ONCE);												
			GxCore_Free(pAdverInfo);
			pAdverInfo = NULL;

	}

}


u_int8_t app_by_ads_corner_bmp_play_state(void)
{
	if (NULL == ad_bmp_time)
		return FALSE;
	return TRUE;
}



void AdverDisplayCornerBMP_Start(SaveAdverCorner_Info *pAdverInfo)
{

	if ((TRUE == app_by_ads_corner_gif_play_state())||(TRUE == app_by_ads_corner_bmp_play_state()))
		return;

	
	printf("AdverDisplayCornerBMP_Start\n");
	app_save_data_to_ddram_file((const char*)pAdverInfo,sizeof(SaveAdverCorner_Info),"/mnt/corner.inf");	
	app_save_data_to_ddram_file((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen,"/mnt/corner.bmp");	
//	app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo,sizeof(SaveAdverCorner_Info),"/home/gx/corner.inf");	
//	app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen,"/home/gx/corner.bmp");	

	app_cas_api_on_event(DVB_AD_BMP,CAS_NAME, pAdverInfo, sizeof(SaveAdverCorner_Info));
	return;
}









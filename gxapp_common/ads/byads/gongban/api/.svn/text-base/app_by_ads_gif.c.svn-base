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

static event_list* ad_gif_time = NULL;
static ad_play_para gifplaypara = {0};
GxAvRect rectgif = {0};
static int timer_gif_play(void *userdata);
#define PLAY_GIF_TEST
#ifdef PLAY_GIF_TEST
void app_by_ads_test_gif(void)
{		
	gifplaypara.file_flash_path = "/theme/firework.gif";
	gifplaypara.file_ddram_path = "/mnt/firework.gif";
	gifplaypara.play_mode = 2;
	gifplaypara.rectdest.x = 200;
	gifplaypara.rectdest.y = 200;
	gifplaypara.rectdest.width= 0;
	gifplaypara.rectdest.height= 0;
	app_play_ad_flash_file(&gifplaypara);
	rectgif.width = gifplaypara.rectdest.width;
	rectgif.height = gifplaypara.rectdest.height;			
	ad_gif_time = create_timer(timer_gif_play, 500, NULL,  TIMER_REPEAT);													
}
#endif



static int timer_gif_play(void *userdata)
{
		gifplaypara.slice_num++;
		printf("gifplaypara.slice_num=%d gifplaypara.slice_count=%d\n",
			gifplaypara.slice_num,gifplaypara.slice_count);
		if (gifplaypara.slice_num <= gifplaypara.slice_count)
		{
			gifplaypara.rectdest.width= 0;
			gifplaypara.rectdest.height= 0;					
			app_play_ad_ddram_file(&gifplaypara);
			rectgif.x = gifplaypara.rectdest.x;
			rectgif.y = gifplaypara.rectdest.y;
			if (rectgif.width < gifplaypara.rectdest.width)
				rectgif.width = gifplaypara.rectdest.width;
			if (rectgif.height < gifplaypara.rectdest.height)
				rectgif.height = gifplaypara.rectdest.height;
		}
	else
		{
			advertisement_clear_frame(rectgif);
			memset(&gifplaypara,0,sizeof(ad_play_para));
			memset(&rectgif,0,sizeof(GxAvRect));
			remove_timer(ad_gif_time);
			ad_gif_time = NULL; 
		}
	return 0;
}

void app_by_ads_stop_corner_gif(void)
{
	if (NULL != ad_gif_time)
		{
			advertisement_clear_frame(rectgif);
			memset(&gifplaypara,0,sizeof(ad_play_para));
			memset(&rectgif,0,sizeof(GxAvRect));
			remove_timer(ad_gif_time);
			ad_gif_time = NULL;	
		}
}

u_int8_t app_by_ads_corner_gif_play_state(void)
{
	if (NULL == ad_gif_time)
		return FALSE;
	return TRUE;
}

void app_by_ads_play_corner_gif(void)
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
			
			gifplaypara.file_flash_path = NULL;
			gifplaypara.play_mode = 2;
			gifplaypara.rectdest.x = pAdverInfo->mad_CornerInfo.mad_StartxPos;
			gifplaypara.rectdest.y = pAdverInfo->mad_CornerInfo.mad_StartyPos;
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
				gifplaypara.file_ddram_path = "/mnt/corner.bmp";
	
			}
			else  if(pAdverInfo->mad_CornerInfo.mad_PicType== 2)
			{
				gifplaypara.file_ddram_path = "/mnt/corner.gif";
			}
								
			app_play_ad_ddram_file(&gifplaypara);
			if (rectgif.width < gifplaypara.rectdest.width)
				rectgif.width = gifplaypara.rectdest.width;
			if (rectgif.height < gifplaypara.rectdest.height)
				rectgif.height = gifplaypara.rectdest.height;			
//			ad_corner_time = create_timer(timer_corner_bmp, duration, NULL,  TIMER_ONCE);												
			if(pAdverInfo->mad_CornerInfo.mad_PicType== 2)	
				ad_gif_time = create_timer(timer_gif_play, 500, NULL,  TIMER_REPEAT);													
				
			GxCore_Free(pAdverInfo);
			pAdverInfo = NULL;

	}

}

void AdverGifStart(SaveAdverCorner_Info *pAdverInfo)
{
	if ((TRUE == app_by_ads_corner_gif_play_state())||(TRUE == app_by_ads_corner_bmp_play_state()))
		return;

	printf("AdverDisplayGIF_Start\n");
	app_save_data_to_ddram_file((const char*)pAdverInfo,sizeof(SaveAdverCorner_Info),"/mnt/corner.inf");	
	app_save_data_to_ddram_file((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen,"/corner.gif");	
	app_cas_api_on_event(DVB_AD_GIF,CAS_NAME, pAdverInfo, sizeof(SaveAdverCorner_Info));
	
	return;
}








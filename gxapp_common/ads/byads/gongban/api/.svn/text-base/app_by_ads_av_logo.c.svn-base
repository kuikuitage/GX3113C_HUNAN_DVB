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

typedef enum 
{
	MPEGIFRAME=1,
	JPEG	
}ADVERTFormat;

ad_play_para playlogopara = {0};

void LoaderCheckPictureCRC32(AdU32 *FlashCRC32)
{
	if (GxCore_FileExists("/home/gx/openpicture.inf") == 1)
		{
			AdverOpenPicture_Info *pAdverInfo = NULL;
			handle_t flash_ad_file;
			size_t	 size;	
			pAdverInfo = GxCore_Malloc(sizeof(AdverOpenPicture_Info));
			flash_ad_file = GxCore_Open("/home/gx/openpicture.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdverInfo, 1, sizeof(AdverOpenPicture_Info));
			GxCore_Close(flash_ad_file);
			*FlashCRC32 = pAdverInfo->mad_CRC32;
			GxCore_Free(pAdverInfo);
			pAdverInfo = NULL;
			return ;
		}
	
	*FlashCRC32 = 0xFFFFFFFF;
	return ;
}


void LoaderVideoCheckWrite(AdverOpenPicture_Info *pAdverInfo)
{

	if(pAdverInfo->mad_PicFormat== MPEGIFRAME)
	{
		app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen,"/home/gx/openpicture.bin");
		app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo,sizeof(AdverOpenPicture_Info),"/home/gx/openpicture.inf");
//		printf("\n LoaderVideoCheckWrite MPEG OPENLOG pAdverInfo->mad_TotleLen=0x%x\n",pAdverInfo->mad_TotleLen);
	}
	else  if(pAdverInfo->mad_PicFormat== JPEG)
	{
		if (pAdverInfo->mad_TotleLen < 0x10000)
			{
				app_flash_save_logo_data((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen);
		//		app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen,"/home/gx/openpicture.jpg");
				app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo,sizeof(AdverOpenPicture_Info),"/home/gx/openpicture.inf");
			
			}
		else 
			{
				printf("\n LoaderVideoCheckWrite pAdverInfo->mad_TotleLen=0x%lx\n",(unsigned long)pAdverInfo->mad_TotleLen);
			}

//		printf("\n LoaderVideoCheckWrite JPEG OPENLOG pAdverInfo->mad_TotleLen=0x%x\n",pAdverInfo->mad_TotleLen);
	}
	else
	{
		printf("\n RECEIVE	OPENLOG UNKNOW FORMAT \n");
		
	}

	return;
		
}

void app_by_ads_show_av_logo(int32_t VideoResolution)
{
	if (GxCore_FileExists("/home/gx/openpicture.inf") == 1)
		{
			AdverOpenPicture_Info *pAdverInfo = NULL;
			handle_t flash_ad_file;
			size_t	 size;	
			pAdverInfo = GxCore_Malloc(sizeof(AdverOpenPicture_Info));
			flash_ad_file = GxCore_Open("/home/gx/openpicture.inf", "r");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdverInfo, 1, sizeof(AdverOpenPicture_Info));
			GxCore_Close(flash_ad_file);
			
			if(pAdverInfo->mad_PicFormat== MPEGIFRAME)
			{
				app_play_i_frame_logo(VideoResolution,"/home/gx/openpicture.bin");
				printf("\n RECEIVE MPEG OPENLOG \n");
			}
			else  if(pAdverInfo->mad_PicFormat== JPEG)
			{
				printf("\n RECEIVE JPEG OPENLOG \n");
			}
			else
			{
				printf("\n RECEIVE	OPENLOG UNKNOW FORMAT \n");
			}		
				
			GxCore_Free(pAdverInfo);
			pAdverInfo = NULL;
		}
	else
		{
#if (0 == DVB_JPG_LOGO)
			app_play_i_frame_logo(VideoResolution,I_FRAME_PATH);
#endif		
		}
	
	return ;
}



void app_by_ads_logo_sleep(uint32_t ms)
{
	if (GxCore_FileExists("/home/gx/openpicture.inf") == 1)
		{
			AdverOpenPicture_Info *pAdverInfo = NULL;
			handle_t flash_ad_file;
			size_t	 size;	
			pAdverInfo = GxCore_Malloc(sizeof(AdverOpenPicture_Info));
			flash_ad_file = GxCore_Open("/home/gx/openpicture.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdverInfo, 1, sizeof(AdverOpenPicture_Info));
			GxCore_Close(flash_ad_file);
			printf("pAdverInfo->mad_Duration=%d\n",pAdverInfo->mad_Duration);
			GxCore_ThreadDelay(pAdverInfo->mad_Duration*1000);
			GxCore_Free(pAdverInfo);

			if(pAdverInfo->mad_PicFormat== MPEGIFRAME)
			{
				app_play_close_av_logo();
			}
			else  if(pAdverInfo->mad_PicFormat== JPEG)
			{
//				advertisement_spp_layer_close();
				//app_play_stop_ad(playlogopara);
				printf("\n RECEIVE JPEG OPENLOG \n");
			}

			pAdverInfo = NULL;
		}
	else
		{
			
#if (1 == DVB_JPG_LOGO)
//				advertisement_spp_layer_close();		
#else
			GxCore_ThreadDelay(ms);			
			app_play_close_av_logo();
#endif
		}

}








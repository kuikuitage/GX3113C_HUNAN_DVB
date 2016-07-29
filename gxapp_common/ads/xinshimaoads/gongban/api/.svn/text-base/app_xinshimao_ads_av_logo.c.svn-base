/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_xinshimao_ads_av_logo.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.11.05		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "ads_api_xsm_2.0.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_xinshimao_ads_porting_stb_api.h"

void app_xinshimao_ads_logo_sleep(uint32_t ms)
{
	if (GxCore_FileExists("/home/gx/openpicture.inf") == 1)
		{
			S_YTDY_AD_SHOW_DATA_T *pAdShowData = NULL;
			handle_t flash_ad_file;
			size_t	 size;	
			pAdShowData = GxCore_Malloc(sizeof(S_YTDY_AD_SHOW_DATA_T));
			flash_ad_file = GxCore_Open("/home/gx/openpicture.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdShowData, 1, sizeof(S_YTDY_AD_SHOW_DATA_T));
			GxCore_Close(flash_ad_file);
			printf("pAdShowData->showing_time=%d\n",pAdShowData->showing_time);
//			GxCore_ThreadDelay(pAdShowData->showing_time*1000);
			GxCore_ThreadDelay(pAdShowData->showing_time);
			GxCore_Free(pAdShowData);
			pAdShowData = NULL;
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








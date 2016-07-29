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
//#include "ads_api_xsm_2.0.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_3h_ads_porting_stb_api.h"

ad_play_para playlogopara = {0};


void app_3h_ads_show_av_logo(int32_t VideoResolution)
{
	if (GxCore_FileExists("/home/gx/openpicture.bin") == 1)
		{
			app_play_i_frame_logo(VideoResolution,"/home/gx/openpicture.bin");
			printf("\n RECEIVE MPEG OPENLOG \n");
		}
	else
		{
#if (0 == DVB_JPG_LOGO)
			app_play_i_frame_logo(VideoResolution,I_FRAME_PATH);
#endif		
		}
	
	return ;
}



void app_3h_ads_logo_sleep(uint32_t ms)
{
		
#if (1 == DVB_JPG_LOGO)
//				advertisement_spp_layer_close();		
#else
			GxCore_ThreadDelay(ms);			
			app_play_close_av_logo();
#endif
}









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
#include "app_jiuzhou_ads_porting_stb_api.h"

ad_play_para playlogopara = {0};


void app_jiuzhou_ads_show_av_logo(int32_t VideoResolution)
{
	printf("%s ==================\n", __FUNCTION__); 
	#if 0
		if (1)
		{
			app_play_i_frame_logo(VideoResolution,I_FRAME_PATH);
			printf("\n RECEIVE MPEG OPENLOG \n");
		}
	#else
	if (GxCore_FileExists(ADV_FILENAME_LOGO) == 1)
		{
			app_play_i_frame_logo(VideoResolution,ADV_FILENAME_LOGO);
			printf("\n RECEIVE MPEG OPENLOG \n");
		}
	#endif
	else
		{
#if (0 == DVB_JPG_LOGO)
			//app_play_i_frame_logo(VideoResolution,I_FRAME_PATH);
#endif		
		}
	
	return ;
}



void app_jiuzhou_ads_logo_sleep(uint32_t ms)
{
	printf("%s ==================\n", __FUNCTION__); 		
#if (1 == DVB_JPG_LOGO)
			GxCore_ThreadDelay(ms);	
			advertisement_spp_layer_close();		
#else
//add by xushx
	if (GxCore_FileExists(ADV_FILENAME_LOGO) == 1)
//	if (1)
	{
			GxCore_ThreadDelay(ms);			
			app_play_close_av_logo();
			printf("app_jiuzhou_ads_logo_sleep ms=%d end\n",ms); 		
	}
	else
	{
			GxCore_ThreadDelay(ms);			
			//app_play_close_av_logo();
	}
#endif
}









/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_xinshimao_ads_bmp.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.11.27		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
//#include "ads_api_xsm_2.0.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_3h_ads_porting_stb_api.h"
#include "3h_ads_porting.h"
#include "app_common_prog.h"





extern ad_play_para ads3h_para[AD_3H_TYPE_MAX];


u_int8_t app_3h_ads_get_play_type(void)
{
	u_int8_t i;
	for (i = 0; i<AD_3H_TYPE_MAX; i++ )
		{
			if (TRUE == app_3h_ads_play_state(i))
				return i;
		}
	return AD_3H_TYPE_MAX;
}

void app_3h_ads_stop(uint8_t type)
{
	GxAvRect rect;	

	if(type >= AD_3H_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}

	
//	printf("%s file_ddram_path=%s\n",__FUNCTION__,ads3h_para[type].file_ddram_path);	
	
	if (NULL != ads3h_para[type].ad_timer)
		{
			remove_timer(ads3h_para[type].ad_timer);
			ads3h_para[type].ad_timer = NULL;
		}

		if (NULL != ads3h_para[type].file_ddram_path)
		{
			rect.x = 0;
			rect.y = 0;
			rect.width = VIDEO_WINDOW_W;
			rect.height = VIDEO_WINDOW_H;
			advertisement_clear_frame(rect);			
//			advertisement_clear_frame(ads3h_para[type].rectdest);
			memset(&ads3h_para[type],0,sizeof(ad_play_para));
//			advertisement_hide();
		}
	return ;
}

void app_3h_ads_play(uint8_t type)
{
	if(type >= AD_3H_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}

	if (FALSE == advertisement_get_startplayflag())
		{
			return ;
		}


	switch(type)
		{
			case AD_3H_TYPE_NONE:
				return;
			case AD_3H_TYPE_CORNOR:
				if (GxCore_FileExists("/mnt/banner.jpg") == 1)
					{
						ads3h_para[type].file_flash_path = NULL;
						ads3h_para[type].play_mode = 2;
//						ads3h_para[type].rectdest.x = 915;//843;
//						ads3h_para[type].rectdest.y = 500;
						ads3h_para[type].rectdest.x = 720;//843;
						ads3h_para[type].rectdest.y = 370;
						ads3h_para[type].file_ddram_path = "/mnt/banner.jpg";
						app_play_ad_ddram_file(&ads3h_para[type]);							
						break;
					}
				else
					{
						return ;
					}
				break;
			case AD_3H_TYPE_MENU:
				if (GxCore_FileExists("/mnt/menu.jpg") == 1)
					{
						ads3h_para[type].file_flash_path = NULL;
						ads3h_para[type].play_mode = 2;
						
						ads3h_para[type].rectdest.x = 100;//843;
						ads3h_para[type].rectdest.y = 100;
						ads3h_para[type].file_ddram_path = "/mnt/menu.jpg";

						app_play_ad_ddram_file(&ads3h_para[type]);							
						break;
					}
				else
					{
						return ;
					}
				break;
			default:
				return;
		}

}


u_int8_t app_3h_ads_play_state(uint8_t type)
{
	if(type >= AD_3H_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return FALSE;
		}

	if (NULL != ads3h_para[type].file_ddram_path)
		{
			return TRUE;
			printf("%s ddram file paht = %s\n",__FUNCTION__,ads3h_para[type].file_ddram_path);
		}

	return FALSE;
}







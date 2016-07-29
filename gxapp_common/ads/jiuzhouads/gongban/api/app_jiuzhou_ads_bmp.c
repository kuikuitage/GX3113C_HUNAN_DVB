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
#include "app_jiuzhou_ads_porting_stb_api.h"
#include "jiuzhou_ads_porting.h"
#include "app_common_prog.h"

//#define DDRAM_FILE_ENABLED	/*使用SPP层显示*/ 
#define DEFAULT_BANNER_FILENAME		("/theme/banner.jpg")
#define DDRAM_BANNER_FILE_PATH		("/mnt/")


extern ad_play_para ads_jiuzhou_para[AD_JIUZHOU_TYPE_MAX];

#ifdef DDRAM_FILE_ENABLED
static char banner_filename[256]; 
#endif 


u_int8_t app_jiuzhou_ads_get_play_type(void)
{
	u_int8_t i;
	for (i = 0; i<AD_JIUZHOU_TYPE_MAX; i++ )
	{
			if (TRUE == app_jiuzhou_ads_play_state(i))
				return i;
	}
	return AD_JIUZHOU_TYPE_MAX;
}

void app_jiuzhou_ads_stop(uint8_t type)
{
		GxAvRect rect;	

		if(type >= AD_JIUZHOU_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}
	
		printf("%s file_ddram_path=%s\n",__FUNCTION__,ads_jiuzhou_para[type].file_ddram_path);	
	
		if (NULL != ads_jiuzhou_para[type].ad_timer)
		{
			remove_timer(ads_jiuzhou_para[type].ad_timer);
			ads_jiuzhou_para[type].ad_timer = NULL;
		}

		if (NULL != ads_jiuzhou_para[type].file_ddram_path)
		{
			rect.x = 0;
			rect.y = 0;
			rect.width = VIDEO_WINDOW_W;
			rect.height = VIDEO_WINDOW_H;
			advertisement_clear_frame(rect);			
//			advertisement_clear_frame(ads_jiuzhou_para[type].rectdest);
			memset(&ads_jiuzhou_para[type],0,sizeof(ad_play_para));
//			advertisement_hide();
		}
		
		return ;
}


int32_t app_jiuzhou_ads_get_ddram_name(char *src_filename, char *dst_filename)
{
	uint8_t buf[1024]={0};
    	handle_t fd_src;
	size_t   size;					
	handle_t fd_dst;
	char *p = NULL; 

	if ((strlen(src_filename) == 0) || (GxCore_FileExists(src_filename) == 0))
	{
		if (GxCore_FileExists(DEFAULT_BANNER_FILENAME) == 1)
			sprintf(src_filename, DEFAULT_BANNER_FILENAME); 
	}

	if (strlen(src_filename) == 0)
		return -1; 

	/*将需要显示的文件COPY到/mnt/目录下*/ 
	/*生成新的DDR RAM文件名*/
	printf("src_filename is: %s\n", src_filename); 
	p = src_filename + strlen(src_filename) - 1; /*取文件名, 去掉路径*/ 
	while (p > src_filename)
	{
		if (*p-- == '/')
		{
			p += 2; 
			sprintf(dst_filename, "%s%s", DDRAM_BANNER_FILE_PATH, p); 
			printf("ddram filename is: %s\n", dst_filename); 
			break; 
		}
	}

	/*将原来的FLASH文件复制到DDRAM*/ 
	if (GxCore_FileExists(src_filename) == 1)
	{
	 		printf("%s %s exist\n", __FUNCTION__, src_filename); 
			if (GxCore_FileExists(dst_filename) == 1)
				GxCore_FileDelete(dst_filename); 
				
			fd_src = GxCore_Open(src_filename, "r");
			GxCore_Seek(fd_src, 0, GX_SEEK_SET);
			fd_dst = GxCore_Open(dst_filename, "a+");
			GxCore_Seek(fd_dst, 0, GX_SEEK_SET);
				
			size = GxCore_Read(fd_src, buf, 1, 1024);
			while(size>0)
			{
			      	size = GxCore_Write(fd_dst, buf, 1, size);	
				size = GxCore_Read(fd_src, buf, 1, 1024);
			}
			GxCore_Close(fd_src);	
			GxCore_Close(fd_dst);	

			printf("%s %s copyed to %s completed.\n", __FUNCTION__, src_filename, dst_filename);
	}

	return 0; 
}

void app_jiuzhou_ads_play(uint8_t type, uint16_t service_id,char * pic_widget,char * default_pic_path)
{
		char name[256]; 
		
		if(type >= AD_JIUZHOU_TYPE_MAX || pic_widget == NULL)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}

		switch(type)
		{
			case AD_JIUZHOU_TYPE_NONE:
				return;
			case AD_JIUZHOU_TYPE_BANNER:
				if(_adv_xw_get_banner_filename(service_id, name) == NULL)
				{
					printf("%s======prog.service_id: 0x%X, ad_file is NULL \n", __FUNCTION__, service_id); 
					if(default_pic_path)
					{
						GUI_SetProperty(pic_widget,"load_img",default_pic_path);
						GUI_SetProperty(pic_widget,"state","show");
						return;
					}
					else
					{
						GUI_SetProperty(pic_widget,"state","hide");
						return ;
					}
				}
				printf("%s======prog.service_id: 0x%X, ad_file: %s\n", __FUNCTION__, service_id, name); 

#ifdef DDRAM_FILE_ENABLED
				memset(banner_filename, 0x00, sizeof(banner_filename)); 
				app_jiuzhou_ads_get_ddram_name(name, banner_filename); 

				printf("%s======ddram banner_filename: %s\n", __FUNCTION__, banner_filename); 
				if (GxCore_FileExists(banner_filename) == 1)
				{
						ads_jiuzhou_para[type].file_flash_path = NULL;
						ads_jiuzhou_para[type].play_mode = 2;
						//ads_jiuzhou_para[type].rectdest.x = 720;//843;
						//ads_jiuzhou_para[type].rectdest.y = 370;
						ads_jiuzhou_para[type].rectdest.x = 720;//843;
						ads_jiuzhou_para[type].rectdest.y = 300;
						ads_jiuzhou_para[type].file_ddram_path = banner_filename;
						app_play_ad_ddram_file(&ads_jiuzhou_para[type]);	
						printf("%s OK, %s!\n", __FUNCTION__, banner_filename); 
						break;
				}
#else
				printf("%s======banner_filename: %s\n", __FUNCTION__, name); 
				if (GxCore_FileExists(name) == 1)
				{
						GUI_SetProperty(pic_widget,"load_img",name);
						GUI_SetProperty(pic_widget,"state","show");

						printf("%s OK, %s!\n", __FUNCTION__, name); 
						break;
				}
#endif 
				break;
			default:
				return;
		}

}


u_int8_t app_jiuzhou_ads_play_state(uint8_t type)
{
		if(type >= AD_JIUZHOU_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return FALSE;
		}

		if (NULL != ads_jiuzhou_para[type].file_ddram_path)
		{
			printf("%s ddram file paht = %s\n",__FUNCTION__,ads_jiuzhou_para[type].file_ddram_path);
			return TRUE;
		}

		return FALSE;
}







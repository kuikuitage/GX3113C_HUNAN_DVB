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
#include "IMG.h"

#define ADVER_DIR_RIGHT_LEFT			0
#define ADVER_DIR_LEFT_RIGHT			1
#define ADVER_DIR_DOWN_UP				2
#define ADVER_DIR_UP_DOWN				3


unsigned char mBuffer[256*1024]={0};
DisplayAdverScroll_Info 	madInfo;


static event_list* ad_scroll_time = NULL;

ad_play_para playscrollpara = {0};
//#define SCROLL_STEP (20)
//#define SCROLL_DURATION (500)


unsigned char PegCustomPalette[256*3] = {
0x02,0x02,0x02,
0x80,0x00,0x00,
0x00,0x80,0x00,
0x80,0x80,0x00,
0x00,0x00,0x80,
0x80,0x00,0x80,
0x00,0x80,0x80,
0x80,0x80,0x80,
0xC0,0xC0,0xC0,
0xFF,0x00,0x00,
0x00,0xFF,0x00,
0xFF,0xFF,0x00,
0x00,0x00,0xFF,
0xFF,0x00,0xFF,
0x00,0xFF,0xFF,
0xFF,0xFF,0xFF,
0xE2,0xB6,0x45,
0x8C,0xE9,0x7D,
0xF7,0xAE,0x68,
0x40,0x21,0x6D,
0xE1,0x09,0x2F,
0xE9,0x69,0x4B,
0x23,0x88,0x0E,
0xE2,0x52,0xF6,
0x07,0x83,0x7C,
0xCD,0x30,0xD6,
0xA6,0xB7,0x16,
0x57,0x78,0xFE,
0x99,0x6F,0xE2,
0xAE,0xE8,0xB6,
0xF8,0xF5,0x03,
0xA6,0x58,0xCF,
0xAA,0xAB,0x2A,
0x16,0x22,0x4C,
0xB1,0xFF,0x64,
0x4F,0x85,0x65,
0x00,0x2A,0x7E,
0x91,0x9A,0x69,
0x56,0x89,0x91,
0x31,0x4F,0xF9,
0x68,0x75,0x9F,
0x00,0x00,0x33,
0x00,0x00,0x66,
0x00,0x00,0x99,
0x00,0x00,0xCC,
0x58,0xEA,0x5C,
0x00,0x33,0x00,
0x00,0x33,0x33,
0x00,0x33,0x66,
0x00,0x33,0x99,
0x00,0x33,0xCC,
0x00,0x33,0xFF,
0x00,0x66,0x00,
0x00,0x66,0x33,
0x00,0x66,0x66,
0x00,0x66,0x99,
0x00,0x66,0xCC,
0x00,0x66,0xFF,
0x00,0x99,0x00,
0x00,0x99,0x33,
0x00,0x99,0x66,
0x00,0x99,0x99,
0x00,0x99,0xCC,
0x00,0x99,0xFF,
0x00,0xCC,0x00,
0x00,0xCC,0x33,
0x00,0xCC,0x66,
0x00,0xCC,0x99,
0x00,0xCC,0xCC,
0x00,0xCC,0xFF,
0xF3,0x60,0x2B,
0x00,0xFF,0x33,
0x00,0xFF,0x66,
0x00,0xFF,0x99,
0x00,0xFF,0xCC,
0xAC,0x23,0x85,
0x33,0x00,0x00,
0x33,0x00,0x33,
0x33,0x00,0x66,
0x33,0x00,0x99,
0x33,0x00,0xCC,
0x33,0x00,0xFF,
0x33,0x33,0x00,
0x33,0x33,0x33,
0x33,0x33,0x66,
0x33,0x33,0x99,
0x33,0x33,0xCC,
0x33,0x33,0xFF,
0x33,0x66,0x00,
0x33,0x66,0x33,
0x33,0x66,0x66,
0x33,0x66,0x99,
0x33,0x66,0xCC,
0x33,0x66,0xFF,
0x33,0x99,0x00,
0x33,0x99,0x33,
0x33,0x99,0x66,
0x33,0x99,0x99,
0x33,0x99,0xCC,
0x33,0x99,0xFF,
0x33,0xCC,0x00,
0x33,0xCC,0x33,
0x33,0xCC,0x66,
0x33,0xCC,0x99,
0x33,0xCC,0xCC,
0x33,0xCC,0xFF,
0x33,0xFF,0x00,
0x33,0xFF,0x33,
0x33,0xFF,0x66,
0x33,0xFF,0x99,
0x33,0xFF,0xCC,
0x33,0xFF,0xFF,
0x66,0x00,0x00,
0x66,0x00,0x33,
0x66,0x00,0x66,
0x66,0x00,0x99,
0x66,0x00,0xCC,
0x66,0x00,0xFF,
0x66,0x33,0x00,
0x66,0x33,0x33,
0x66,0x33,0x66,
0x66,0x33,0x99,
0x66,0x33,0xCC,
0x66,0x33,0xFF,
0x66,0x66,0x00,
0x66,0x66,0x33,
0x66,0x66,0x66,
0x66,0x66,0x99,
0x66,0x66,0xCC,
0x66,0x66,0xFF,
0x66,0x99,0x00,
0x66,0x99,0x33,
0x66,0x99,0x66,
0x66,0x99,0x99,
0x66,0x99,0xCC,
0x66,0x99,0xFF,
0x66,0xCC,0x00,
0x66,0xCC,0x33,
0x66,0xCC,0x66,
0x66,0xCC,0x99,
0x66,0xCC,0xCC,
0x66,0xCC,0xFF,
0x66,0xFF,0x00,
0x66,0xFF,0x33,
0x66,0xFF,0x66,
0x66,0xFF,0x99,
0x66,0xFF,0xCC,
0x66,0xFF,0xFF,
0x99,0x00,0x00,
0x99,0x00,0x33,
0x99,0x00,0x66,
0x99,0x00,0x99,
0x99,0x00,0xCC,
0x99,0x00,0xFF,
0x99,0x33,0x00,
0x99,0x33,0x33,
0x99,0x33,0x66,
0x99,0x33,0x99,
0x99,0x33,0xCC,
0x99,0x33,0xFF,
0x99,0x66,0x00,
0x99,0x66,0x33,
0x99,0x66,0x66,
0x99,0x66,0x99,
0x99,0x66,0xCC,
0x99,0x66,0xFF,
0x99,0x99,0x00,
0x99,0x99,0x33,
0x99,0x99,0x66,
0x99,0x99,0x99,
0x99,0x99,0xCC,
0x99,0x99,0xFF,
0x99,0xCC,0x00,
0x99,0xCC,0x33,
0x99,0xCC,0x66,
0x99,0xCC,0x99,
0x99,0xCC,0xCC,
0x99,0xCC,0xFF,
0x99,0xFF,0x00,
0x99,0xFF,0x33,
0x99,0xFF,0x66,
0x99,0xFF,0x99,
0x99,0xFF,0xCC,
0x99,0xFF,0xFF,
0xCC,0x00,0x00,
0xCC,0x00,0x33,
0xCC,0x00,0x66,
0xCC,0x00,0x99,
0xCC,0x00,0xCC,
0xCC,0x00,0xFF,
0xCC,0x33,0x00,
0xCC,0x33,0x33,
0xCC,0x33,0x66,
0xCC,0x33,0x99,
0xCC,0x33,0xCC,
0xCC,0x33,0xFF,
0xCC,0x66,0x00,
0xCC,0x66,0x33,
0xCC,0x66,0x66,
0xCC,0x66,0x99,
0xCC,0x66,0xCC,
0xCC,0x66,0xFF,
0xCC,0x99,0x00,
0xCC,0x99,0x33,
0xCC,0x99,0x66,
0xCC,0x99,0x99,
0xCC,0x99,0xCC,
0xCC,0x99,0xFF,
0xCC,0xCC,0x00,
0xCC,0xCC,0x33,
0xCC,0xCC,0x66,
0xCC,0xCC,0x99,
0xCC,0xCC,0xCC,
0xCC,0xCC,0xFF,
0xCC,0xFF,0x00,
0xCC,0xFF,0x33,
0xCC,0xFF,0x66,
0xCC,0xFF,0x99,
0xCC,0xFF,0xCC,
0xCC,0xFF,0xFF,
0xAC,0x90,0x95,
0xFF,0x00,0x33,
0xFF,0x00,0x66,
0xFF,0x00,0x99,
0xFF,0x00,0xCC,
0xC9,0xA6,0x78,
0xFF,0x33,0x00,
0xFF,0x33,0x33,
0xFF,0x33,0x66,
0xFF,0x33,0x99,
0xFF,0x33,0xCC,
0xFF,0x33,0xFF,
0xFF,0x66,0x00,
0xFF,0x66,0x33,
0xFF,0x66,0x66,
0xFF,0x66,0x99,
0xFF,0x66,0xCC,
0xFF,0x66,0xFF,
0xFF,0x99,0x00,
0xFF,0x99,0x33,
0xFF,0x99,0x66,
0xFF,0x99,0x99,
0xFF,0x99,0xCC,
0xFF,0x99,0xFF,
0xFF,0xCC,0x00,
0xFF,0xCC,0x33,
0xFF,0xCC,0x66,
0xFF,0xCC,0x99,
0xFF,0xCC,0xCC,
0xFF,0xCC,0xFF,
0x99,0x4A,0xEA,
0xFF,0xFF,0x33,
0xFF,0xFF,0x66,
0xFF,0xFF,0x99,
0xFF,0xFF,0xCC,
0x00,0x00,0x00
};


void app_by_ads_stop_scroll_bmp(void)
{
	if (NULL != ad_scroll_time)
		{
			advertisement_clear_frame(playscrollpara.rectdest);
			memset(&playscrollpara,0,sizeof(playscrollpara));
			remove_timer(ad_scroll_time);
			ad_scroll_time = NULL;		
		}
	return ;
}

static int timer_scroll_bmp(void *userdata)
{
	GxAvRect dest_old = {0};
	u_int32_t offset_start = 0;

	if (GxCore_FileExists("/mnt/scroll.inf") == 1) 
		{
			handle_t flash_ad_file;
			size_t	 size;
			SaveAdverScroll_Info *pAdverInfo = NULL;
			pAdverInfo = GxCore_Malloc(sizeof(SaveAdverScroll_Info));
			flash_ad_file = GxCore_Open("/mnt/scroll.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdverInfo, 1, sizeof(SaveAdverScroll_Info));
			GxCore_Close(flash_ad_file);
			
			playscrollpara.file_flash_path = NULL;
			playscrollpara.play_mode = 2;
			playscrollpara.times_count= pAdverInfo->mad_ScrollInfo.mad_Times;

			memcpy(&dest_old,&playscrollpara.rectdest,sizeof(GxAvRect));
			playscrollpara.scroll_len +=SCROLL_STEP;
			if ( playscrollpara.scroll_len <= pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos )
				{
					playscrollpara.rectdest.x = pAdverInfo->mad_ScrollInfo.mad_EndxPos-playscrollpara.scroll_len;
				}
			else
				{
					playscrollpara.rectdest.x = pAdverInfo->mad_ScrollInfo.mad_StartxPos;
				}
			playscrollpara.rectdest.y = pAdverInfo->mad_ScrollInfo.mad_StartyPos;
			playscrollpara.rectsrc.y = 0;


			if (playscrollpara.rectdest.x == pAdverInfo->mad_ScrollInfo.mad_StartxPos)
				{
					offset_start = playscrollpara.scroll_len - (pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos);
					playscrollpara.rectsrc.x = offset_start;
					if (playscrollpara.scroll_len >= pAdverInfo->mad_ScrollInfo.mad_width + pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos)
						{
							advertisement_clear_frame(playscrollpara.rectdest);
							playscrollpara.times_num++;
							if (playscrollpara.times_num >= playscrollpara.times_count)
								{
									AdverDisplay_StopType(FONT_MOVE_PICTURE);
									memset(&playscrollpara,0,sizeof(playscrollpara));
									remove_timer(ad_scroll_time);
									ad_scroll_time = NULL;
									return 0;								
								}
							else
								{
									memset(&playscrollpara.rectdest,0,sizeof(GxAvRect));
									memset(&playscrollpara.rectsrc,0,sizeof(GxAvRect));
									playscrollpara.scroll_len = 0; 
									return 0;
								}
							
						}
					if (pAdverInfo->mad_ScrollInfo.mad_width-offset_start >= pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos)
						{
							playscrollpara.rectdest.width= pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos;				
							playscrollpara.rectsrc.width= pAdverInfo->mad_ScrollInfo.mad_EndxPos - pAdverInfo->mad_ScrollInfo.mad_StartxPos;				

						}
					else
						{
							playscrollpara.rectdest.width= pAdverInfo->mad_ScrollInfo.mad_width-offset_start;
							playscrollpara.rectsrc.width= pAdverInfo->mad_ScrollInfo.mad_width-offset_start;				
						}
				}
			else
				{
					playscrollpara.rectsrc.x = 0;
					if (playscrollpara.scroll_len>=pAdverInfo->mad_ScrollInfo.mad_width)
						{
							playscrollpara.rectsrc.width= pAdverInfo->mad_ScrollInfo.mad_width;
							playscrollpara.rectdest.width= pAdverInfo->mad_ScrollInfo.mad_width;
						}
					else
						{
							playscrollpara.rectsrc.width= playscrollpara.scroll_len;
							playscrollpara.rectdest.width= playscrollpara.scroll_len;						
						}
				}
			playscrollpara.rectdest.height= pAdverInfo->mad_ScrollInfo.mad_height;
			playscrollpara.rectsrc.height= pAdverInfo->mad_ScrollInfo.mad_height;

			printf("playscrollpara.rectdest.width = %d\n",playscrollpara.rectdest.width);
			if (playscrollpara.rectdest.width== 0)
				{
					advertisement_clear_frame(playscrollpara.rectdest);
					playscrollpara.times_num++;
					if (playscrollpara.times_num >= playscrollpara.times_count)
						{
							AdverDisplay_StopType(FONT_MOVE_PICTURE);
							memset(&playscrollpara,0,sizeof(playscrollpara));
							remove_timer(ad_scroll_time);
							ad_scroll_time = NULL;
							return 0;								
						}
					else
						{
							memset(&playscrollpara.rectdest,0,sizeof(GxAvRect));
							memset(&playscrollpara.rectsrc,0,sizeof(GxAvRect));
							playscrollpara.scroll_len = 0; 
							return 0;
						}
				}
			
			playscrollpara.file_ddram_path = "/mnt/scroll.bmp";
			
			GxCore_Free(pAdverInfo);
			pAdverInfo = NULL;	
			if (playscrollpara.rectdest.width < dest_old.width)
				{
					dest_old.width = dest_old.width - playscrollpara.rectdest.width;
					dest_old.x = playscrollpara.rectdest.x + playscrollpara.rectdest.width;
					advertisement_clear_frame(dest_old);			
				}
				
			app_play_ad_ddram_file(&playscrollpara);
		}
	
	return 0;
}

void app_by_ads_play_scroll_bmp()
{
	if (GxCore_FileExists("/mnt/scroll.inf") == 1) 
		{
			memset(&playscrollpara,0,sizeof(playscrollpara));
			playscrollpara.file_flash_path = NULL;
			playscrollpara.play_mode = 2;
			playscrollpara.file_ddram_path = "/mnt/scroll.bmp";
			if (NULL == ad_scroll_time)
				ad_scroll_time = create_timer(timer_scroll_bmp, SCROLL_DURATION, NULL,  TIMER_REPEAT);												
		}	
}

u_int32_t GetAdverCurrentDisplayState(void)
{
	if (NULL == ad_scroll_time)
		return FALSE;
	return TRUE;
}



void AdverDisplayText_Start(SaveAdverScroll_Info *pAdverInfo)
{
	  unsigned char  *pBuffer=NULL;

	  unsigned int iLen = 0;
	  unsigned short usColorPalatteLen = 0;
	
	  memset(&madInfo,0,sizeof(DisplayAdverScroll_Info));	
	  memset(mBuffer,0,256*1024);
	  if(pAdverInfo == NULL)
	  return;
	  
	  AdverDisplay_StopType(FONT_MOVE_PICTURE);
	
	  memset((void *)&madInfo,0,sizeof(DisplayAdverScroll_Info));
	  memcpy((unsigned char *)&madInfo.mad_ScrollInfo,(unsigned char *)&pAdverInfo->mad_ScrollInfo,sizeof(AdverScroll_Info));   
	
	  madInfo.mad_Type = pAdverInfo->mad_Type;
	  madInfo.mad_CRC32 = pAdverInfo->mad_CRC32;
	  madInfo.mad_TotleLen = pAdverInfo->mad_TotleLen;
	  memcpy((unsigned char *)madInfo.mad_Data,(unsigned char *)pAdverInfo->mad_Data,pAdverInfo->mad_TotleLen);
		  
	
	pAdverInfo->mad_ScrollInfo.mad_height =	((unsigned short)madInfo.mad_Data[4]*256)+madInfo.mad_Data[5];
	pAdverInfo->mad_ScrollInfo.mad_width =	((unsigned short)madInfo.mad_Data[2]*256)+madInfo.mad_Data[3];
	printf("wHeight=%d wWidth=%d\n",pAdverInfo->mad_ScrollInfo.mad_height,pAdverInfo->mad_ScrollInfo.mad_width);
	usColorPalatteLen 			  = ((unsigned short)madInfo.mad_Data[7]<<8)+madInfo.mad_Data[8];

	  if( madInfo.mad_ScrollInfo.mad_MoveDir > 3)
	  {
		  printf(" unknow direction \n");
		  ASSERT(0);
		  return;
	  }

	  if((pAdverInfo->mad_ScrollInfo.mad_width*pAdverInfo->mad_ScrollInfo.mad_height) > 1024*1024)
	  {
		  printf(" file too large \n");
		  ASSERT(0);
		  return;
	  }
	  
	  if(pAdverInfo->mad_ScrollInfo.mad_Times== 0)
		  pAdverInfo->mad_ScrollInfo.mad_Times = 1;
	
	  DeComBitmapData(mBuffer,(madInfo.mad_Data+usColorPalatteLen*4+9),&iLen,(madInfo.mad_TotleLen-9-usColorPalatteLen*4));
	  if(pBuffer != NULL)
	  {
		  free(pBuffer);
	  }
	  pBuffer = (unsigned char *)malloc(iLen+1);
	  memset(pBuffer,0x00,iLen);
	  memcpy(pBuffer,mBuffer,iLen);
		 
	printf("AdverDisplayText_Start\n");
//	printf("AdverDisplayText_Start pAdverInfo->mad_TotleLen=%d\n",pAdverInfo->mad_TotleLen);

	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_StartxPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_StartxPos);
	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_StartyPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_StartyPos);
	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_EndxPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_EndxPos);
	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_EndyPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_EndyPos);
	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_Times=%d\n",pAdverInfo->mad_ScrollInfo.mad_Times);


	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_StartxPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_StartxPos);
	printf("AdverDisplayText_Start pAdverInfo->mad_ScrollInfo.mad_StartyPos=%d\n",pAdverInfo->mad_ScrollInfo.mad_StartyPos);

	app_save_data_to_ddram_file((const char*)pAdverInfo,sizeof(SaveAdverScroll_Info),"/mnt/scroll.inf");	
	app_save_data_to_ddram_file((const char*)pBuffer,iLen,"/mnt/scroll.bmp");	
//	app_flash_save_ad_data_to_flash_file((const char*)pAdverInfo,sizeof(SaveAdverScroll_Info),"/home/gx/scroll.inf");	
//	app_flash_save_ad_data_to_flash_file((const char*)pBuffer,iLen,"/home/gx/scroll.bmp");	
	free(pBuffer);
	pBuffer = NULL;

	app_cas_api_on_event(DVB_AD_SCROLL_BMP,CAS_NAME, pAdverInfo, sizeof(SaveAdverScroll_Info));

	return;
}



int scroll_GDI_Load_Image(const char *path,
		unsigned int *width,
		unsigned int *height,
		unsigned char *bpp,
		unsigned char **data)
{
//	static u_int8_t i = 0,gif_slice_num = 0;
//	GIF_Image *gif_slice = NULL;
//	image_desc *image = NULL;
	unsigned char *pout = NULL;
	unsigned char *data1 = NULL;
	handle_t file;
	size_t	 size;	
	SaveAdverScroll_Info *pAdverInfo = NULL;


//	GIF_Para *gif_image = NULL;


	if((NULL == path) || (NULL == width) || (NULL == height) || (NULL == bpp))
		return 0;

//	image = gal_img_load(NULL, path);
//	if(NULL == image)
//		return 0;

//	*width = image->width;
//	*height = image->height;

	{

		pAdverInfo = GxCore_Malloc(sizeof(SaveAdverScroll_Info));
		file = GxCore_Open("/mnt/scroll.inf", "r+");
		GxCore_Seek(file, 0, GX_SEEK_SET);
		size = GxCore_Read(file, pAdverInfo, 1, sizeof(SaveAdverScroll_Info));
		GxCore_Close(file);
		
		*width = pAdverInfo->mad_ScrollInfo.mad_width;
		*height = pAdverInfo->mad_ScrollInfo.mad_height;

	
		data1 = (unsigned char *)GxCore_Malloc(pAdverInfo->mad_ScrollInfo.mad_width * pAdverInfo->mad_ScrollInfo.mad_height * 2);
		memset(data1, 0, pAdverInfo->mad_ScrollInfo.mad_width * pAdverInfo->mad_ScrollInfo.mad_height * 2);

		file = GxCore_Open(path, "r");
		GxCore_Seek(file, 0, GX_SEEK_SET);
		size = GxCore_Read(file, data1, 1, pAdverInfo->mad_ScrollInfo.mad_width * pAdverInfo->mad_ScrollInfo.mad_height);
		GxCore_Close(file);

		pout = (unsigned char *)GxCore_Malloc(pAdverInfo->mad_ScrollInfo.mad_width * pAdverInfo->mad_ScrollInfo.mad_height * 2);
		if(NULL == pout)
			return 1;

		memset(pout, 0, pAdverInfo->mad_ScrollInfo.mad_width * pAdverInfo->mad_ScrollInfo.mad_height * 2);

		convert_rgb_2_yuv422((void*)(data1), pout, pAdverInfo->mad_ScrollInfo.mad_width, pAdverInfo->mad_ScrollInfo.mad_height, 8, PegCustomPalette);
//		gal_img_free_memory(image);
//		image->data = pout;
	}

	free(pAdverInfo);
	pAdverInfo = NULL;
	free(data1);
	data1 = NULL;

	*data = pout;

//	*data = (unsigned char *)image->data;

//	GUI_FREE(image->filename);
//	GUI_FREE(image->pal);
//	GUI_FREE(image);

	return 0;

}









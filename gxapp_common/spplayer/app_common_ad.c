/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ad.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2011.06.09		  zhouhm 	 			creation
*****************************************************************************/
#include <stdlib.h>
#include <stdarg.h>
#include <gxcore.h>
#include <assert.h>
#include "gxavdev.h"
#include "gui_core.h"
#include "IMG.h"

#include "app_common_ad.h"
#include "gxapp_sys_config.h"
#include "app_common_play.h"
#ifdef DVB_AD_TYPE_BY_FLAG
#include "adverparse.h"
#include "app_by_ads_porting_stb_api.h"
#endif



GxVpuProperty_LayerMainSurface GuiSppSurface;
GxVpuProperty_LayerMainSurface AdSppSurface;
GxVpuProperty_LayerMainSurface CurSppSurface;

 
int device_ad_handle = 0;
int vpu_ad_handle = 0;
static int zoom_flag = 0;


int ads_GDI_Load_Image(ad_play_para* playpara,
		unsigned int *img_top,
		unsigned int *img_left,
		unsigned int *width,
		unsigned int *height,
		unsigned char *bpp,
		unsigned char **data)
{
	u_int8_t i = 0;
	GIF_Image *gif_slice = NULL;
	image_desc *image = NULL;
	unsigned char *pout = NULL;
	GIF_Para *gif_image = NULL;

	const char *path = playpara->file_ddram_path;
	if((NULL == path) || (NULL == width) || (NULL == height) || (NULL == bpp))
		return 0;

	image = gal_img_load(NULL, path);
	if(NULL == image)
		return 0;

	*width = image->width;
	*height = image->height;
	*img_top = 0;
	*img_left = 0;

	if(JPEG_TYPE == image->type)
		*bpp = 16;
	else if((BMP_TYPE == image->type) || (PNG_TYPE == image->type)) {
		pout = (unsigned char *)GxCore_Malloc(image->width * image->height * 2);
		if(NULL == pout)
		{
			
			return 1;
		}

		memset(pout, 0, image->width * image->height * 2);

		convert_rgb_2_yuv422((void*)(image->data), pout, image->width, image->height, image->bpp, image->pal);
//		gal_img_free_memory(image);
//		image->data = pout;
	}
	else if(GIF_TYPE == image->type) {
		gif_image = (GIF_Para *)(image->data);
		if(NULL == gif_image)
			return 1;
		gif_image->bpp = 8;

		if (0 == playpara->slice_num)
			{
				pout = (unsigned char *)GxCore_Malloc(image->width * image->height * 2);
				if(NULL == pout)
					return 1;

				memset(pout, 0, image->width * image->height * 2);

				gif_slice = gif_image->each_image;

				*width = gif_slice->img_width;
				*height = gif_slice->img_height;
				*img_top = gif_slice->img_top;
				*img_left = gif_slice->img_left;

				printf("gif_slice->img_top = %d\n",gif_slice->img_top);
				printf("gif_slice->img_left = %d\n",gif_slice->img_left);
				printf("gif_slice->img_width = %d\n",gif_slice->img_width);
				printf("gif_slice->img_height = %d\n",gif_slice->img_height);


				
				convert_rgb_2_yuv422((void*)(gif_slice->img_data),
									pout,
									image->width,
									image->height,
									gif_image->bpp,
									gif_image->pal);
				gif_slice = gif_image->each_image;
				while (NULL != gif_slice)
					{
						playpara->slice_count++;
						gif_slice = gif_slice->next;
					}
				printf("playpara->slice_count = %d\n",playpara->slice_count);
				gif_slice = gif_image->each_image;
			}
		else
			{
			
				gif_slice = gif_image->each_image;
				for (i = 0; i< playpara->slice_num;i++)
					{
						gif_slice = gif_slice->next;
						if (NULL == gif_slice)
							{
//								playpara->slice_num = 0;
								return 1;
							}
					}
				pout = (unsigned char *)GxCore_Malloc(gif_slice->img_width * gif_slice->img_height * 2);
				if(NULL == pout)
					return 1;

				*width = gif_slice->img_width;
				*height = gif_slice->img_height;
				*img_top = gif_slice->img_top;
				*img_left = gif_slice->img_left;

				printf("gif_slice->img_top = %d\n",gif_slice->img_top);
				printf("gif_slice->img_left = %d\n",gif_slice->img_left);
				printf("gif_slice->img_width = %d\n",gif_slice->img_width);
				printf("gif_slice->img_height = %d\n",gif_slice->img_height);				
				memset(pout, 0, gif_slice->img_width * gif_slice->img_height * 2);
				
//				gif_slice = gif_image->each_image;
				
				convert_rgb_2_yuv422((void*)(gif_slice->img_data),
									pout,
									gif_slice->img_width,
									gif_slice->img_height,
									gif_image->bpp,
									gif_image->pal);		
				
			}


//		gal_img_free_memory(image);
//		image->data = pout;
	}
	else
		*bpp = image->bpp;

	*bpp = 16;
	if (NULL != pout)
		{
			*data = (unsigned char *)pout;
		}
	else
		{
			pout = (unsigned char *)GxCore_Malloc(image->width * image->height * image->bpp/8);
			if(NULL == pout)
				return 1;
			
			memset(pout, 0, image->width * image->height * image->bpp/8);
			memcpy(pout,image->data ,image->width * image->height * image->bpp/8);
			*data = (unsigned char *)pout;	
		}
//	*data = (unsigned char *)image->data;
//	GUI_FREE(image->filename);
//	GUI_FREE(image->pal);
//	GUI_FREE(image);
	
	gal_img_release(image);

	return 0;

}


int advertisement_play_init(void)
{
	static GxVpuProperty_CreateSurface create_surface;
	static int init_flag = 1;

	if(init_flag)
	{
		GxVpuProperty_FillRect tRect;
		memset(&create_surface,0,sizeof(create_surface));

		device_ad_handle = GxAvdev_CreateDevice(0);
		vpu_ad_handle = GxAvdev_OpenModule(device_ad_handle, GXAV_MOD_VPU, 0);
		
		GuiSppSurface.layer = GX_LAYER_SPP;
		GxAVGetProperty(device_ad_handle,
			            vpu_ad_handle,
			            GxVpuPropertyID_LayerMainSurface,
			            &GuiSppSurface,
			            sizeof(GxVpuProperty_LayerMainSurface));

		create_surface.width = VIDEO_WINDOW_W;
		create_surface.height = VIDEO_WINDOW_H;
		create_surface.mode = GX_SURFACE_MODE_IMAGE;
		create_surface.format = GX_COLOR_FMT_YCBCRA6442;//GX_COLOR_FMT_YCBCR422;//;
		create_surface.buffer = NULL;
		if(0 > GxAVGetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_CreateSurface,
			&create_surface, sizeof(GxVpuProperty_CreateSurface)))
		{
			printf("[AD] Creat surface failure\n");
			return -1;
		}
		//绘制透明窗口
		memset(&tRect,0,sizeof(tRect));
		tRect.surface = create_surface.surface;
		tRect.is_ksurface = 1;
		tRect.rect.x = 0;
		tRect.rect.y = 0;
		tRect.rect.width = VIDEO_WINDOW_W;
		tRect.rect.height = VIDEO_WINDOW_H;
		if(GxAVSetProperty(device_ad_handle, 
			vpu_ad_handle, 
			GxVpuPropertyID_FillRect,
			&tRect, 
			sizeof(GxVpuProperty_FillRect)) < 0)
			printf("[AD] Fill rect  failure\n");
		
		AdSppSurface.layer = GX_LAYER_SPP;
		AdSppSurface.surface = create_surface.surface;

		CurSppSurface = GuiSppSurface;
#ifdef DVB_AD_TYPE_3H_FLAG
		advertisement_zoom(VIDEO_WINDOW_W*3/8,VIDEO_WINDOW_H*3/8,VIDEO_WINDOW_W*5/8,VIDEO_WINDOW_H*5/8);
#endif
		init_flag = 0;
	}
	return 0;
}

/*转换矩阵,把ycbcr422 数据转到ycbcr6422数据
注意,为了节省内存, data中的数据将会改变成新
的数据*/
int ycbcr422toycbcra6422(unsigned int img_width,unsigned int img_height,
							unsigned char img_bpp, unsigned char* data)
{
	/*转换ycbcr422  to ycbcra6442*/
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;

/*	
	char* pmem = GxCore_Malloc( img_width * img_height * img_bpp / 8);
	if(pmem == NULL)
	{
		printf("[AD] GxCore_Malloc failure\n");
		return -1;
	}
	memcpy(pmem,data,( img_width * img_height * img_bpp / 8));
	*/

	

	unsigned char y,u,v;
	unsigned short Yuv6442, *pData = (unsigned short *)data;
	unsigned char *Srcy = NULL,*Srcu = NULL,*Srcv = NULL;

	//malloc 动态分配比较好，不然可以设置一个数组，大小为
	//最大图片的大小；
	Srcy = GxCore_Malloc(img_width*img_height);
	if(Srcy == NULL)return -1;
	Srcu = GxCore_Malloc(img_width*img_height);
	if(Srcu == NULL){GxCore_Free(Srcy);return -1;}
	Srcv = GxCore_Malloc(img_width*img_height);
	if(Srcv == NULL){GxCore_Free(Srcy);GxCore_Free(Srcu);return -1;}
	//malloc出来的内存，不能以以下方式使用，需要转换成
	//指针。g_menudata  解码完后数据membuf 输出数据
	//这边是按列处理的
	for(i=0;i<img_height;i++)
	{
		for(j=0;j<img_width;j++)
		{
			k=((i*img_width)+j);
			*(Srcy+i*img_width + j)=data[k*2+1];
			if(k%2)
			{
				*(Srcu+i*img_width + j) = data[k*2-2];
				*(Srcv+i*img_width + j) = data[k*2];
				
			}
			else
			{
				*(Srcu+i*img_width + j)  = data[k*2];
				*(Srcv+i*img_width + j)  = data[k*2+2];
				
			}
		}
	}
	for(i=0;i<img_height;i++)
	{
		for(j=0;j<img_width;j++)
		{
			y = *(Srcy+i*img_width + j)>>2;
			u = *(Srcu+i*img_width + j)>>4;
			v = *(Srcv+i*img_width + j)>>4;
			Yuv6442 = (y<<10)|(u<<6)|(v<<2)|0x3;
			/*回写*/
			*(pData++) = (Yuv6442>>8)|(Yuv6442<<8);
		}
	}
	GxCore_Free(Srcy);
	GxCore_Free(Srcu);
	GxCore_Free(Srcv);
	//free(pmem);
	return 0;
}

/*position  参照广告协议解析,这边用于内部记录图片解码以及
选择surface*/
int advertisement_play(ad_play_para* playpara, unsigned short position,unsigned short pos2, bool bShow)
{
	unsigned int ret = 0, img_width = 0, img_height = 0;
	unsigned int img_top =0,img_left =0;
	unsigned char img_bpp = 0;
	unsigned char *data = NULL, *membuf = NULL;
	int result = 0;
	char *path = playpara->file_ddram_path;
	GxAvRect *psrcrect = &playpara->rectsrc;
	GxAvRect *pdestrect = &playpara->rectdest;
	
	GxVpuProperty_Blit Blit;
	GxVpuProperty_CreateSurface surface_tpm = {0};

	
	GxVpuProperty_LayerEnable SetLayerEnable={0};
	GxVpuProperty_LayerOnTop OnTop={0};
	/*用于缓存12张图片解码数据,记录解码的名称
	黑龙江广告系统只有12份图片,暂定12,名字不会超过64*/
	static char first_run = 1;
	static char cmp_name[13][64];
	static struct 
	{	
		unsigned int width;
		unsigned int hight;		
		unsigned int bpp;
		void* pdata;
	}decode_data[13];
	unsigned short index=0;

	advertisement_play_init();
	if(first_run)
	{
		memset(cmp_name,0,sizeof(cmp_name));
		memset(decode_data,0,sizeof(decode_data));
		first_run = 0;
	}
	if(position >=12 )
		return -1;

	index = position;
	if(pos2 == 1&&position == 3)
	{
		index = 12;
	}
	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(path))
	{
		printf("\n[AD] Resource not exist .name = %s\n",path);
		return -1;
	}

	/*这部分用于测试default数据，最终需要去掉*/
#ifdef DVB_AD_TYPE_3H_FLAG
	if((0 == strcmp(cmp_name[index], path)) && (decode_data[index].width != 0 ) && ( decode_data[index].hight != 0)
		&&(NULL != decode_data[index].pdata))
	{
//		printf("[AD]  pic named %s has decode before!\n",path);
		img_width = decode_data[index].width;
		img_height = decode_data[index].hight;
		img_bpp = decode_data[index].bpp;
	}
	else
#endif
	{
#ifdef DVB_AD_TYPE_BY_FLAG
		if (0 == strcmp("/mnt/scroll.bmp",playpara->file_ddram_path))
			{
				result = scroll_GDI_Load_Image((const char*)path,&img_width,&img_height,&img_bpp, &data);				
			}
		else
#endif
			{
				result = ads_GDI_Load_Image(playpara, &img_top,&img_left,&img_width, &img_height, &img_bpp, &data);				
			}
		if(0 == result)
		{
//			img_width = img_width;
//			printf("[AD]  decode pic  named %s ! img_width=%d img_height=%d img_bpp=%d\n",path,img_width,img_height,img_bpp);
			
			memcpy((char*)cmp_name[index],path,strlen(path));
			decode_data[index].width = img_width;
			decode_data[index].hight = img_height;
			decode_data[index].bpp = img_bpp;
			/*下面data是需要释放掉的,等换图片的时候释放*/
			if(decode_data[index].pdata)
			{
				GxCore_Free(decode_data[index].pdata);
				decode_data[index].pdata = NULL;
			}
			decode_data[index].pdata = (void*)data;
			if(position != 3)
				ycbcr422toycbcra6422(img_width,img_height, img_bpp,data);
		}
		else
		{
			printf("[AD]  decode pic  named %s failure !!!\n",path);			
		}
			
	}		
	if(img_width == 0 || img_height == 0)
	{
		printf("[AD]@@@@@(img_width == 0 || img_height == 0)@@@@@\n");
		if(decode_data[index].pdata)
		{
			GxCore_Free(decode_data[index].pdata);
			decode_data[index].pdata = NULL;
		}

		return -1;
	}

	if(FALSE == bShow)
	{
		if(decode_data[index].pdata)
		{
			GxCore_Free(decode_data[index].pdata);
			decode_data[index].pdata = NULL;
		}

		return 0;
	}
	
	//锁定spp层资源
	GxAvdev_SppLock();
	/*选择surface*/
	if(position == 3)
	{
		if(CurSppSurface.surface != GuiSppSurface.surface)
		{
			if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerMainSurface,
						&GuiSppSurface, sizeof(GxVpuProperty_LayerMainSurface)) == 0)
			{
				printf("[AD] Change to  gui surface !");
				CurSppSurface = GuiSppSurface;
			}
			else
			{
				printf("[AD] Change surface failure!");
			}
			CurSppSurface = GuiSppSurface;
			
		} 
	}
	else
	{
		if(CurSppSurface.surface != AdSppSurface.surface)
		{
			if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerMainSurface,
						&AdSppSurface, sizeof(GxVpuProperty_LayerMainSurface)) == 0)
			{
				CurSppSurface = AdSppSurface;
			}
			else
			{
				printf("[AD] Change surface failure!");
			}
			CurSppSurface = AdSppSurface;
			
		} 	
	}
#ifdef DVB_AD_TYPE_3H_FLAG
	advertisement_zoom(VIDEO_WINDOW_W*3/8,VIDEO_WINDOW_H*3/8,VIDEO_WINDOW_W*5/8,VIDEO_WINDOW_H*5/8);
#else
{
	GxVpuProperty_LayerViewport LayerViewport = {0};
	LayerViewport.layer = GX_LAYER_SPP;
	LayerViewport.rect.x = 0;
	LayerViewport.rect.y = 0;
	LayerViewport.rect.width = VIDEO_WINDOW_W;
	LayerViewport.rect.height = VIDEO_WINDOW_H;
	if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerViewport,
						&LayerViewport, sizeof(GxVpuProperty_LayerViewport))!= 0) {
		}
}
#endif
		

	surface_tpm.width = img_width;
	surface_tpm.height = img_height;
	surface_tpm.mode = GX_SURFACE_MODE_IMAGE;
/*	if(CurSppSurface.surface != AdSppSurface.surface)	
	{
		surface_tpm.format = GX_COLOR_FMT_YCBCR422;
	}
	else*/
	{
		surface_tpm.format = GX_COLOR_FMT_YCBCRA6442;
	}
	surface_tpm.buffer = NULL;
	
	ret |= GxAVGetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_CreateSurface,&surface_tpm, sizeof(GxVpuProperty_CreateSurface));
	if (0 != ret)
	{
 		printf("[AD] creat tmp surface failure\n");	
		GxAvdev_SppUnlock();	
		if(decode_data[index].pdata)
		{
			GxCore_Free(decode_data[index].pdata);
			decode_data[index].pdata = NULL;
		}

 		return 0;			
	}
	
	membuf = (unsigned char *)surface_tpm.buffer;
	memcpy(membuf, decode_data[index].pdata, img_width * img_height * img_bpp / 8); 
#ifdef DVB_AD_TYPE_3H_FLAG
#else
	if(decode_data[index].pdata)
	{
		GxCore_Free(decode_data[index].pdata);
		decode_data[index].pdata = NULL;
	}
#endif	
	
	/*Blit to main surface*/
//	Blit.has_clut_conversion = 0;
	memset(&Blit,0,sizeof(GxVpuProperty_Blit));
	Blit.mode = GX_ALU_ROP_COPY;
	Blit.srca.surface = surface_tpm.surface;
	Blit.srca.rect.x = psrcrect->x;
	Blit.srca.rect.y = psrcrect->y;
	if(0 != psrcrect->width)
		Blit.srca.rect.width = psrcrect->width;
	else
		Blit.srca.rect.width = img_width;
	
	if (0 != psrcrect->height)
		Blit.srca.rect.height = psrcrect->height;
	else
		Blit.srca.rect.height = img_height;
	
	Blit.srca.is_ksurface = 1;
	Blit.srcb.surface = CurSppSurface.surface;
	//Blit.srcb.rect = Blit.srca.rect;
	Blit.srcb.rect.x = pdestrect->x+img_left;
	Blit.srcb.rect.y = pdestrect->y+img_top;
	if(0 != pdestrect->width)
		Blit.srcb.rect.width = pdestrect->width;
	else
		Blit.srcb.rect.width = img_width;
	Blit.srcb.rect.height = img_height;
	Blit.srcb.is_ksurface = 1;

	Blit.dst = Blit.srcb;
	Blit.dst.rect.x = pdestrect->x+img_left;
	Blit.dst.rect.y = pdestrect->y+img_top;
	if(0 != pdestrect->width)
		Blit.dst.rect.width = pdestrect->width;
	else
		Blit.dst.rect.width = img_width;	
	Blit.dst.rect.height = img_height;
	Blit.dst.is_ksurface = 1;

	if (0 == pdestrect->width)
		{
			pdestrect->width= img_width;
			pdestrect->height= img_height;		
		}


	//Blit.dst = Blit.srcb;
	ret = GxAVSetProperty(device_ad_handle,  vpu_ad_handle, GxVpuPropertyID_Blit, &Blit, sizeof(GxVpuProperty_Blit));
	if (0 != ret)
	{
 		printf("[AD] Blit error\n");	
		GxAvdev_SppUnlock();		
 		return 0;			
	}	


	//记得画完后提交给vpu，因为是多任务的，可能多????
	// 硬件才提交一次。手动提交可以尽快完成并释放资源
	GxVpuProperty_EndUpdate update_draw;
	GxAVSetProperty(device_ad_handle, 
		vpu_ad_handle, 
		GxVpuPropertyID_EndUpdate, 
		&update_draw, 
		sizeof(GxVpuProperty_EndUpdate));

	//里面记得destroy 每次临时穿件的surface。不然会导致内存泄露的
	GxVpuProperty_DestroySurface destroy;
	destroy.surface = surface_tpm.surface;
	ret = GxAVSetProperty(device_ad_handle, 
		vpu_ad_handle, 
		GxVpuPropertyID_DestroySurface, 
		&destroy, 
		sizeof(GxVpuProperty_DestroySurface));
	if (0 != ret)
	{
 		printf("[AD] destroy surface failure!!\n");
		GxAvdev_SppUnlock();
 		return 0;			
	}	

	//释放spp层资源
	GxAvdev_SppUnlock();
	//hd_add_blit_element(CreateSurface.surface);
	
	SetLayerEnable.layer = GX_LAYER_SPP;
	SetLayerEnable.enable = 1;
	ret |= GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerEnable, &SetLayerEnable, sizeof(GxVpuProperty_LayerEnable));
	if (0 != ret)
	{
 		printf("advertisement_play GxAVSetProperty  GxVpuPropertyID_LayerEnable error\n");		
 		return 0;			
	}	
	OnTop.layer  = GX_LAYER_SPP;
	OnTop.enable = 1;
	GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerOnTop, (void*)&OnTop, sizeof(GxVpuProperty_LayerOnTop));
	if (0 != ret)
	{
 		printf("advertisement_play GxAVSetProperty  GxVpuProperty_LayerOnTop error\n");		
 		return 0;			
	}			

	return (ret);
}	



int advertisement_hide(void)
{
	unsigned int ret = 0;
	GxVpuProperty_LayerOnTop  OnTop = { 0 };
	GxVpuProperty_LayerEnable SetLayerEnable;
	GxVpuProperty_LayerViewport   p_Viewport;


	SetLayerEnable.layer = GX_LAYER_SPP;
	SetLayerEnable.enable = 0;
	if (GxAVSetProperty(device_ad_handle,vpu_ad_handle,GxVpuPropertyID_LayerEnable,
								&SetLayerEnable, sizeof(GxVpuProperty_LayerEnable)) <0 )
		printf("[Ad] set property err! line %d\n",__LINE__);

	OnTop.layer  = GX_LAYER_SPP;
	OnTop.enable = 0;
	if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerOnTop, 
							(void*)&OnTop, sizeof(GxVpuProperty_LayerOnTop))<0)
		printf("[Ad] set property err! line %d\n",__LINE__);

	
	if(CurSppSurface.surface != GuiSppSurface.surface)
	{
		/*Revert*/
		if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerMainSurface,
							&GuiSppSurface, sizeof(GxVpuProperty_LayerMainSurface)) < 0 )
			printf("[Ad] set property err! line %d\n",__LINE__);
		CurSppSurface = GuiSppSurface;		
	}

	p_Viewport.layer = GX_LAYER_SPP;
	p_Viewport.rect.x = 0;
	p_Viewport.rect.y = 0;
	p_Viewport.rect.width = VIDEO_WINDOW_W;
	p_Viewport.rect.height = VIDEO_WINDOW_H;
	
	
	printf("Viewport.rect = [.x=%-3d, .y=%-3d, .w=%-3d, .h=%-3d ]", 
				p_Viewport.rect.x, p_Viewport.rect.y, p_Viewport.rect.width, p_Viewport.rect.height);
	
	ret = GxAVSetProperty(device_ad_handle,
						 vpu_ad_handle,
						 GxVpuPropertyID_LayerViewport,
						 &p_Viewport,
						 sizeof(GxVpuProperty_LayerViewport));
	if (0 != ret)
	{
		printf("[AD] Layer Viewport error\n");		
		return 0;	
	}


	
	return (ret);
}

int advertisement_clear_frame(GxAvRect rect)
{
	GxVpuProperty_FillRect tRect;
	//绘制透明窗口
	memset(&tRect,0,sizeof(tRect));
	tRect.surface = AdSppSurface.surface ;
	tRect.is_ksurface = 1;
//	tRect.rect.x = 0;
//	tRect.rect.y = 0;
//	tRect.rect.width = VIDEO_WINDOW_W;
//	tRect.rect.height = VIDEO_WINDOW_H;
	tRect.rect.x = rect.x;
	tRect.rect.y = rect.y;
	tRect.rect.width = rect.width;
	tRect.rect.height = rect.height;

//	printf("advertisement_clear_frame x=%d y=%d width=%d height=%d\n",
//		rect.x,rect.y,rect.width,rect.height);

	if(GxAVSetProperty(device_ad_handle, 
		vpu_ad_handle, 
		GxVpuPropertyID_FillRect,
		&tRect, 
		sizeof(GxVpuProperty_FillRect)) < 0)
	{
		printf("[AD] Fill rect  failure\n");
		return -1;
	}
	return 0;
}

void advertisement_spp_layer_close(void)
{
	int device_ad_handle = 0;
	int vpu_ad_handle = 0;
	GxVpuProperty_LayerOnTop  OnTop = { 0 };
	GxVpuProperty_LayerEnable SetLayerEnable;

	device_ad_handle = GxAvdev_CreateDevice(0);
	vpu_ad_handle = GxAvdev_OpenModule(device_ad_handle, GXAV_MOD_VPU, 0);

	SetLayerEnable.layer = GX_LAYER_SPP;
	SetLayerEnable.enable = 0;
	if (GxAVSetProperty(device_ad_handle,vpu_ad_handle,GxVpuPropertyID_LayerEnable,
								&SetLayerEnable, sizeof(GxVpuProperty_LayerEnable)) <0 )
		{
			printf("[Ad] set property err! line %d\n",__LINE__);
			return;
		}

		OnTop.layer  = GX_LAYER_SPP;
		OnTop.enable = 0;
	if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerOnTop, 
							(void*)&OnTop, sizeof(GxVpuProperty_LayerOnTop))<0)
		{
			printf("[Ad] set property err! line %d\n",__LINE__);
			return;
		}
	return;
}

int advertisement_zoom(int x,int y,int w,int h)
{
	unsigned int ret = 0;
	
	if(CurSppSurface.surface != AdSppSurface.surface)
	{
		if(GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerMainSurface,
					&AdSppSurface, sizeof(GxVpuProperty_LayerMainSurface)) == 0)
		{
			CurSppSurface = AdSppSurface;
		}
		else
		{
			printf("[AD] Change surface failure!");
		}
		CurSppSurface = AdSppSurface;
		
	} 


	if ((VIDEO_WINDOW_W == w)&&(VIDEO_WINDOW_H == h))
		{
			
			zoom_flag = 0;
			GxVpuProperty_LayerViewport   p_Viewport;
			p_Viewport.layer = GX_LAYER_SPP;
			p_Viewport.rect.x = x;
			p_Viewport.rect.y = y;
			p_Viewport.rect.width = w;
			p_Viewport.rect.height = h;


			printf("Viewport.rect = [.x=%-3d, .y=%-3d, .w=%-3d, .h=%-3d ]", 
						p_Viewport.rect.x, p_Viewport.rect.y, p_Viewport.rect.width, p_Viewport.rect.height);
	
			ret = GxAVSetProperty(device_ad_handle,
								 vpu_ad_handle,
								 GxVpuPropertyID_LayerViewport,
								 &p_Viewport,
								 sizeof(GxVpuProperty_LayerViewport));
			if (0 != ret)
			{
				printf("[AD] Layer Viewport error\n");		
				return 0;	
			}
			
		}
	else
		{
			if (0 == zoom_flag)
			{
					GxVpuProperty_LayerViewport   p_Viewport;
					zoom_flag = 1;
					p_Viewport.layer = GX_LAYER_SPP;
					p_Viewport.rect.x = x;
					p_Viewport.rect.y = y;
					p_Viewport.rect.width = w;
					p_Viewport.rect.height = h;


					printf("Viewport.rect = [.x=%-3d, .y=%-3d, .w=%-3d, .h=%-3d ]", 
								p_Viewport.rect.x, p_Viewport.rect.y, p_Viewport.rect.width, p_Viewport.rect.height);
			
					ret = GxAVSetProperty(device_ad_handle,
										 vpu_ad_handle,
										 GxVpuPropertyID_LayerViewport,
										 &p_Viewport,
										 sizeof(GxVpuProperty_LayerViewport));
					if (0 != ret)
					{
						printf("[AD] Layer Viewport error\n");		
						return 0;	
					}
				}				
		}
	return 0;


}




/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2014, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_common_ad_scroll_load.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2014.05.23		  zhouhm 	 			creation
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

#include <setjmp.h>
#include "jpeglib.h"



extern GxVpuProperty_LayerMainSurface GuiSppSurface;
extern GxVpuProperty_LayerMainSurface AdSppSurface;
extern GxVpuProperty_LayerMainSurface CurSppSurface;

 
extern int device_ad_handle;
extern int vpu_ad_handle;
static GxVpuProperty_CreateSurface surface_scroll_tpm = {0};

extern int ycbcr422toycbcra6422(unsigned int img_width,unsigned int img_height,
								unsigned char img_bpp, unsigned char* data);
typedef struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
} *my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 0);
}


image_desc *advertisement_jpeg_scroll_soft_load(image_desc *img_desc, const char *pFile)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct my_error_mgr jerr;
	/* More stuff */
	FILE *infile;		/* source file */
	JSAMPARRAY buffer;	/* Output row buffer */
	int row_stride, size = 0;		/* physical row width in output buffer */
	char *sample_buf = NULL;

	/* In this example we want to open the input file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */
	if(pFile == NULL)
		return (NULL);

	if ((infile = fopen((char *)pFile, "rb")) == NULL) {
		fprintf(stderr, "[GUI]can't open %s\n", pFile);
		return (NULL);
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return (NULL);
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void)jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.doc for more info.
	 */

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	 * jpeg_read_header(), so we do nothing here.
	 */

	/* Step 5: Start decompressor */
	// zhouhm delete at 2013.03.24
#if 0
	if((cinfo.image_width > gui.config.width) || (cinfo.image_height > gui.config.height))
		image_zoom(&cinfo);
#endif

	(void)jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * In this example, we need to make an output work buffer of the right size.
	 */
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) & cinfo, JPOOL_IMAGE, row_stride, 1);

	size = cinfo.output_height * cinfo.output_width * cinfo.output_components;
	
	if (img_desc == NULL) {
		img_desc = (image_desc*)GxCore_Malloc(sizeof(image_desc));
		if(NULL == img_desc)
			return (NULL);

		memset(img_desc, 0, sizeof(image_desc));

		img_desc->filename = GxCore_Strdup(pFile);
	}

	img_desc->type     = JPEG_TYPE;
	img_desc->width    = cinfo.output_width;
	img_desc->height   = cinfo.output_height;
	img_desc->bpp      = 16;
	img_desc->size     = size;
	img_desc->effect   = EFFECT_COPY;
	sample_buf = (char *)gal_img_alloc_memory(img_desc);

	if(NULL == sample_buf)
		return (NULL);
	img_desc->data = sample_buf;

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */

		memcpy(&(sample_buf[(cinfo.output_scanline - 1) * cinfo.output_width * 2]),
				*buffer,
				cinfo.output_width * 2);

	}

	/* Step 7: Finish decompression */

	(void)jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	fclose(infile);

	return img_desc;
}

int ads_scroll_GDI_Load_Image(ad_play_para* playpara,
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
		return 1;

	image = advertisement_jpeg_scroll_soft_load(NULL, path);


	if(NULL == image)
	{	
		printf(" ads_scroll_GDI_Load_Image image==NULL \n");
		return 1;
	}
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
			gal_img_release(image);
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
		{
			gal_img_release(image);
			return 1;
		}
		printf("image bpp =%d gif_image->bpp = %d \n",image->bpp,gif_image->bpp);
		gif_image->bpp = 8;

		if (0 == playpara->slice_num)
		{
			pout = (unsigned char *)GxCore_Malloc(image->width * image->height * 2);
			if(NULL == pout)
			{
				gal_img_release(image);
				return 1;
			}
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
					image->bpp,
					gif_slice->img_pal);
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
						gal_img_release(image);
						return 1;
					}
				}
				pout = (unsigned char *)GxCore_Malloc(gif_slice->img_width * gif_slice->img_height * 2);
				if(NULL == pout)
				{	

					gal_img_release(image);
					return 1;
				}
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



/*position  参照广告协议解析,这边用于内部记录图片解码以及
选择surface*/
int advertisement_scroll_play(ad_play_para* playpara, unsigned short position,unsigned short pos2, bool bShow)
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

	if (SCROLL_STEP == playpara->scroll_len)
		{
			if(decode_data[index].pdata)
			{
				GxCore_Free(decode_data[index].pdata);
				decode_data[index].pdata = NULL;

				GxAvdev_SppLock();
				//里面记得destroy 每次临时穿件的surface。不然会导致内存泄露的
				GxVpuProperty_DestroySurface destroy;
				destroy.surface = surface_scroll_tpm.surface;
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
				GxAvdev_SppUnlock();

			}			
		}

	/*这部分用于测试default数据，最终需要去掉*/
	if((0 == strcmp(cmp_name[index], path)) && (decode_data[index].width != 0 ) && ( decode_data[index].hight != 0)
		&&(NULL != decode_data[index].pdata))
	{
//		printf("[AD]  pic named %s has decode before!\n",path);
		img_width = decode_data[index].width;
		img_height = decode_data[index].hight;
		img_bpp = decode_data[index].bpp;
	}
	else
	{
#ifdef DVB_AD_TYPE_BY_FLAG
		if (0 == strcmp("/mnt/scroll.bmp",playpara->file_ddram_path))
			{
				result = scroll_GDI_Load_Image((const char*)path,&img_width,&img_height,&img_bpp, &data);				
			}
		else
#endif
			{
				result = ads_scroll_GDI_Load_Image(playpara, &img_top,&img_left,&img_width, &img_height, &img_bpp, &data);				
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
			{
				ycbcr422toycbcra6422(img_width,img_height, img_bpp,data);
			}
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

	if (SCROLL_STEP == playpara->scroll_len)
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

			printf("img_width=%d\n",img_width);
			printf("img_height=%d\n",img_height);
			surface_scroll_tpm.width = img_width;
			surface_scroll_tpm.height = img_height;
			surface_scroll_tpm.mode = GX_SURFACE_MODE_IMAGE;
		/*	if(CurSppSurface.surface != AdSppSurface.surface)	
			{
				surface_tpm.format = GX_COLOR_FMT_YCBCR422;
			}
			else*/
			{
				surface_scroll_tpm.format = GX_COLOR_FMT_YCBCRA6442;
			}
			surface_scroll_tpm.buffer = NULL;
			
			ret |= GxAVGetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_CreateSurface,&surface_scroll_tpm, sizeof(GxVpuProperty_CreateSurface));
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
			
			membuf = (unsigned char *)surface_scroll_tpm.buffer;
			memcpy(membuf, decode_data[index].pdata, img_width * img_height * img_bpp / 8); 			
		}

	
	/*Blit to main surface*/
//	Blit.has_clut_conversion = 0;
	memset(&Blit,0,sizeof(GxVpuProperty_Blit));
	Blit.mode = GX_ALU_ROP_COPY;
	Blit.srca.surface = surface_scroll_tpm.surface;
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

	//释放spp层资源
	GxAvdev_SppUnlock();
	//hd_add_blit_element(CreateSurface.surface);

	if (SCROLL_STEP == playpara->scroll_len)
		{
			SetLayerEnable.layer = GX_LAYER_SPP;
			SetLayerEnable.enable = 1;
			ret |= GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerEnable, &SetLayerEnable, sizeof(GxVpuProperty_LayerEnable));
			if (0 != ret)
			{
		 		printf("advertisement_scroll_play GxAVSetProperty  GxVpuPropertyID_LayerEnable error\n");		
		 		return 0;			
			}	
			OnTop.layer  = GX_LAYER_SPP;
			OnTop.enable = 1;
			GxAVSetProperty(device_ad_handle, vpu_ad_handle, GxVpuPropertyID_LayerOnTop, (void*)&OnTop, sizeof(GxVpuProperty_LayerOnTop));
			if (0 != ret)
			{
		 		printf("advertisement_scroll_play GxAVSetProperty  GxVpuProperty_LayerOnTop error\n");		
		 		return 0;			
			}		
		}
	
	return (ret);
}	

/*
* return : 0 - stop , 1 - scrolling
*/
int advertisement_scrolling_by_step(ad_play_para* playPara,uint16_t start_y,uint16_t width,uint16_t height)
{
	GxAvRect dest_old = {0};
	GxAvRect dest_clear = {0};
	u_int32_t offset_start = 0;	

	if (NULL == playPara)
		{
			printf(" %s %d playPara error \n",__FILE__,__LINE__);
			return 0;
		}

	memcpy(&dest_old,&playPara->rectdest,sizeof(GxAvRect));			
	memset(&dest_clear,0,sizeof(GxAvRect));
	playPara->scroll_len +=SCROLL_STEP;
	if ( playPara->scroll_len <= VIDEO_WINDOW_W)
		{
			playPara->rectdest.x = VIDEO_WINDOW_W -playPara->scroll_len;
		}
	else
		{
			playPara->rectdest.x = 0;
		}
	playPara->rectdest.y = start_y;
	playPara->rectsrc.y = 0;

	if (playPara->rectdest.x == 0)
		{
			offset_start = playPara->scroll_len - (VIDEO_WINDOW_W);
			playPara->rectsrc.x = offset_start;
			if (playPara->scroll_len >= width+ VIDEO_WINDOW_W)
				{
					playPara->times_num++;
					if (playPara->times_num >= playPara->times_count)
						{ 						
							return 0;							
						}
					else
						{
							advertisement_clear_frame(playPara->rectdest);
							memset(&playPara->rectdest,0,sizeof(GxAvRect));
							memset(&playPara->rectsrc,0,sizeof(GxAvRect));
							playPara->scroll_len = 0; 
							return 1;
						}
					
				}
			if (width-offset_start >= VIDEO_WINDOW_W)
				{
					playPara->rectdest.width= VIDEO_WINDOW_W;				
					playPara->rectsrc.width= VIDEO_WINDOW_W;				

				}
			else
				{
					playPara->rectdest.width= width-offset_start;
					playPara->rectsrc.width= width-offset_start;				
				}
		}
	else
		{
			playPara->rectsrc.x = 0;
			if (playPara->scroll_len>=width)
				{									
					playPara->rectsrc.width= width;
					playPara->rectdest.width= width;
				}
			else
				{
					playPara->rectsrc.width= playPara->scroll_len;
					playPara->rectdest.width= playPara->scroll_len;						
				}
		}
	playPara->rectdest.height= height;
	playPara->rectsrc.height= height;

	if (playPara->rectdest.width== 0)
		{
			playPara->times_num++;
			if (playPara->times_num >= playPara->times_count)
				{							
					return 0;								
				}
			else
				{
					advertisement_clear_frame(playPara->rectdest);
					memset(&playPara->rectdest,0,sizeof(GxAvRect));
					memset(&playPara->rectsrc,0,sizeof(GxAvRect));
					playPara->scroll_len = 0; 	
					return 1;
				}
		}
	
	if (playPara->rectdest.width < dest_old.width)
		{
			dest_old.width = dest_old.width - playPara->rectdest.width;
			dest_old.x = playPara->rectdest.x + playPara->rectdest.width;
			memcpy(&dest_clear,&dest_old,sizeof(GxAvRect));
		}
	else
		if ((playPara->rectdest.width == dest_old.width)
			&&(VIDEO_WINDOW_W != dest_old.width))
			{
				dest_old.width = SCROLL_STEP;
				dest_old.x = playPara->rectdest.x + playPara->rectdest.width;
				memcpy(&dest_clear,&dest_old,sizeof(GxAvRect));
			}
		
	app_play_ad_ddram_file(playPara);
	if (dest_clear.width >0)
		{
			advertisement_clear_frame(dest_clear);						
		}

	return 1;
	
}










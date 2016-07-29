/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	jiuzhou_ads_porting.c
* Author    : 	xujunlin
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2014.08.28		  xujunlin 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_common_porting_ads_flash.h"
#include "jiuzhou_ads_porting.h"
#include "app_jiuzhou_ads_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "service/gxsearch.h"
#include "app_common_search.h"


//#define __ADV_JIUZHOU_DEBUG__

#ifndef __ADV_JIUZHOU_DEBUG__
#define PRINTF(...)     {;}
#else
#define PRINTF(...)      {printf("[ADV]"); printf( __VA_ARGS__ );}
#endif


static handle_t ad_channel_jiuzhou_pf=0; 
static handle_t ad_filter_jiuzhou_pf=0; 
static handle_t ad_channel_jiuzhou_logo=0; 
static handle_t ad_filter_jiuzhou_logo=0; 
static handle_t ad_filter_timeout = 0; 



extern void app_porting_psi_demux_lock(void); 
extern void app_porting_psi_demux_unlock(void); 
#ifdef ECOS_OS
extern void cyg_scheduler_lock(void);
extern void cyg_scheduler_unlock(void);
#endif



ad_play_para ads_jiuzhou_para[AD_JIUZHOU_TYPE_MAX];

/*如果大于三秒没有收到数据, 则重启过滤器*/ 
#define 	AD_FILTER_TIMEOUT_JIUZHOU (3)
uint32_t      g_filter_nms_pf = 0;
uint32_t      g_filter_nms_logo = 0;

int GxAD_JiuZhou_BannerFilterOpen(uint8_t version);
int GxAD_JiuZhou_BannerFilterClose(); 
int GxAD_JiuZhou_LogoFilterOpen(void);
int GxAD_JiuZhou_LogoFilterClose(void);


/****************************************************************************
* 相关定义
*****************************************************************************/
	
/****************************************************************************
*  广告数据将以TS流文件的形式在主频点315MHZ播出
* 开机画面PID为0X1BA0
* EPG换台广告PID为0X1BAB
*****************************************************************************/
#define AD_JIUZHOU_BANNER_PID 	(0x1BAB)
#define AD_JIUZHOU_LOGO_PID 		(0x1BA0)

#define ERR_FAILURE		(-1)
#define SUCCESS			(0)

#define AD_FILENAME_PF_INFO	("/home/gx/pf_info.cfg")

#define MAX_ADV_INFO_SEC_NUM 		256
#define SECTION_NEW 	1
#define SECTION_REPEAT 1 << 1
#define SECTION_ERROR  1 << 2
#define MAX_SEC_NUM 		128

// default
#define XW_INFO_MAX_NUM			128 
#define XW_LINKAGE_MAX_NUM		32 

#define UINT8	unsigned char
#define UINT16    unsigned short
#define BOOL 	unsigned char 
#define INT32	int 
#define UINT32  	unsigned int 

#define MEMSET	memset
#define MEMCPY 	memcpy

#define DATA8_16(data)      (*(data) << 8 | *(data +1))   
#define DATA8_24(data)      (*(data) << 16 | *(data +1) << 8 |*(data +2))       
#define DATA8_32(data)      (*(data) << 24 | *(data + 1) << 16 | *(data +2) << 8 | *(data +3))   

static UINT16 AD_SECTION_EXTID(UINT8 *data)
{
	return DATA8_16(&data[3]); 
}

static UINT16 AD_SECTION_NUMBER(UINT8 *data)
{
	return data[6]; 
}

static UINT16 AD_SECTION_MAXNO(UINT8 *data)
{
	return data[7]+1; 
}

typedef struct
{
	UINT8  max_sec;
	UINT8  sec_num;
}xw_pf_info_sec_t;

typedef struct
{
	UINT16 ext_id;
	UINT8  max_sec;
	UINT8  sec_num;
}xw_pf_sec_t;

typedef struct 
{
	UINT16 pic_identify; 
	UINT32 pic_size; 
	UINT16 pic_width; 
	UINT16 pic_height; 
	UINT8 pic_palette; 
	UINT8 pic_type; 
	UINT8 pic_iscompress; 
	UINT32 pic_compress_length; 
}xw_pic_des_info; 

typedef struct 
{
	UINT16 ts_id; 
	UINT16 on_id; 
	UINT16 s_id; 
}xw_linkage_des_info; 

typedef struct 
{
	UINT8 dis_mode; 
	UINT8 row_number; 
	UINT8 font_color; 
	UINT16 dis_time; 
	UINT8 content_length; 
	UINT8 content[256]; 
}xw_content_des_info; 

typedef struct 
{
	UINT8 cus_fun; 
}xw_custom_des_info; 


typedef struct 
{
	UINT16 ad_id; 
	UINT8 ad_type; 
	UINT16 ad_des_loop_len; 
	UINT16 linkage_num; 
	xw_pic_des_info ad_pic_des; 
	xw_linkage_des_info ad_linkage_des[XW_LINKAGE_MAX_NUM]; 
	xw_content_des_info ad_content_des; 
	xw_custom_des_info ad_custom_des; 
	UINT8 *p_data[MAX_SEC_NUM]; // file data
	UINT16 p_data_len[MAX_SEC_NUM]; //
	UINT32 recved_len; //data size
}banner_info_t; 

typedef struct
{
	UINT16 ad_id; 
	UINT32 pic_size; 
	UINT8 *p_data[MAX_SEC_NUM]; // file data
	UINT16 p_data_len[MAX_SEC_NUM]; //
	UINT32 recved_len; //data size
} logo_info_t; 

static BOOL ad_filter_stopped = FALSE; 

/*广告信息*/ 
static BOOL bInfoStart = FALSE;
xw_pf_info_sec_t ad_info_sec_tag[MAX_ADV_INFO_SEC_NUM];
static UINT16 ad_info_sec_count = 0;

/*开机LOGO*/ 
static BOOL bLogoStart = FALSE; 
logo_info_t ad_logo_info; 
xw_pf_info_sec_t ad_logo_sec_tag[MAX_SEC_NUM];
static UINT16 ad_logo_sec_count = 0;

/*换台广告*/ 
static BOOL bBannerStart = FALSE;
static banner_info_t ad_xw_banner_info[XW_INFO_MAX_NUM];
static UINT8 ad_xw_ad_count = 0;
xw_pf_sec_t ad_xw_banner_sec_tag[MAX_SEC_NUM];
static UINT16 ad_xw_rsc_count = 0,ad_xw_all_banner_sec_num = 0;


static INT32 _adv_pf_parser(UINT8 *data, UINT32 len); 
static INT32 _adv_pf_data_parser(UINT8 *data, UINT32 len); 
static INT32 _adv_pf_info_parser(UINT8 *data, UINT32 len); 
static INT32 _adv_xw_get_info_pos(UINT16 s_id); 
static INT32 _adv_pf_info_sec_check(UINT8 sec_num,UINT8 max_sec); 
static INT32 _adv_pf_sec_check(UINT16 extend_id,UINT8 sec_num,UINT8 max_sec); 
static INT32 _adv_pf_section_parser(UINT8 *data, UINT32 len); 
static INT32 _adv_xw_get_info_pos_by_pic_identify(UINT16 pic_identify); 
static char *_adv_get_file_name(xw_pic_des_info *pic_info, char *filename); 

static INT32 _adv_logo_sec_check(UINT8 sec_num,UINT8 max_sec); 
static INT32 _adv_logo_data_parser(UINT8 *data, UINT32 len); 
static INT32 _adv_logo_section_parser(UINT8 *data, UINT32 len); 

char *_adv_xw_get_banner_filename(UINT16 s_id, char *filename); 
static void _adv_clean_ad_files(void); 
static BOOL _adv_xw_get_complete_flag(void); 
static BOOL _adv_xw_check_file_recved_status(AD_JIUZHOU_SHOW_TYPE_T type, UINT16 pic_id); 
static void _adv_xw_update_file_recved_status(AD_JIUZHOU_SHOW_TYPE_T type, banner_info_t *p_info); 
/****************************************************************************
* 定义结束
*****************************************************************************/


handle_t app_jiuzhou_ads_get_pf_filter_handle(void)
{
	return ad_filter_jiuzhou_pf;
}

handle_t app_jiuzhou_ads_get_logo_filter_handle(void)
{
	return ad_filter_jiuzhou_logo;
}

void app_jiuzhou_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
    	int16_t            pid;
    	uint16_t            section_length;
    	uint8_t*            data = (uint8_t*)Section;
    	int                 len = Size;
	int32_t ret;

	ret = GxDemux_FilterGetPID(Filter,&pid);

    	PRINTF("%s Size=%d pid=0x%x\n",__FUNCTION__,Size,pid);
    	switch(pid) 
    	{
    		case AD_JIUZHOU_BANNER_PID: 
			{
				g_filter_nms_pf = 0; 
		        	while(len > 0) {
					if(data[0] != 0xc0) return;
					
					section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
					
					PRINTF("%s section_length=%d\n",__FUNCTION__,section_length);
					_adv_pf_parser(data, section_length); 
					
					data += section_length;
		           		len -= section_length;
		        	}

		 	}
	        	break; 
    		case AD_JIUZHOU_LOGO_PID: 
			{
				g_filter_nms_logo = 0; 
		        	while(len > 0) {
					if(data[0] != 0xca) return;
					
					section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
					
					PRINTF("%s section_length=%d\n",__FUNCTION__,section_length);
					_adv_logo_data_parser(data, section_length); 
					
					data += section_length;
		           		len -= section_length;
		        	}
		 	}
	        	break; 
	    	default:
	        	break;
    	}
}


void _adv_filter_timeout_task(void* arg)
{
		while(TRUE)
		{
					GxCore_ThreadDelay(100); 
					if (ad_filter_stopped)
						return; 
						
					if (( 0 != ad_filter_jiuzhou_pf)&&(0 != ad_channel_jiuzhou_pf))
					{
						g_filter_nms_pf+=100;	
						if (g_filter_nms_pf>=1000*AD_FILTER_TIMEOUT_JIUZHOU)
						{
							PRINTF("%s  timeout A\n", __FUNCTION__); 
															
							/*time out*/
							GxAD_JiuZhou_BannerFilterClose();	
							GxAD_JiuZhou_BannerFilterOpen(32);
						}
					}

					if (( 0 != ad_filter_jiuzhou_logo)&&(0 != ad_channel_jiuzhou_logo))
					{
						g_filter_nms_logo+=100;	
						if (g_filter_nms_logo>=1000*AD_FILTER_TIMEOUT_JIUZHOU)
						{
							PRINTF("%s  timeout B\n", __FUNCTION__); 
															
							/*time out*/
							GxAD_JiuZhou_LogoFilterClose();	
							GxAD_JiuZhou_LogoFilterOpen();
						}
					}

		}
}


int GxAD_JiuZhou_BannerFilterClose(void)
{
	int32_t  ret = 0;

	PRINTF("GxAD_JiuZhou_BannerFilterClose\n");
	app_porting_psi_demux_lock();

	if (0 != ad_filter_jiuzhou_pf)
		{
//			ret = GxDemux_FilterDisable(ad_filter_jiuzhou_pf);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter_jiuzhou_pf);
			CHECK_RET(DEMUX,ret);
			ad_filter_jiuzhou_pf = 0;
		}
	if (0 != ad_channel_jiuzhou_pf)
		{
			ret = GxDemux_ChannelFree(ad_channel_jiuzhou_pf);
			CHECK_RET(DEMUX,ret);
			ad_channel_jiuzhou_pf = 0;
		}
	g_filter_nms_pf=0;
	app_porting_psi_demux_unlock();

	return 0;
}

int GxAD_JiuZhou_LogoFilterClose(void)
{
	int32_t  ret = 0;

	PRINTF("GxAD_JiuZhou_LogoFilterClose\n");
	app_porting_psi_demux_lock();

	if (0 != ad_filter_jiuzhou_logo)
		{
//			ret = GxDemux_FilterDisable(ad_filter_jiuzhou_pf);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter_jiuzhou_logo);
			CHECK_RET(DEMUX,ret);
			ad_filter_jiuzhou_logo = 0;
		}
	if (0 != ad_channel_jiuzhou_logo)
		{
			ret = GxDemux_ChannelFree(ad_channel_jiuzhou_logo);
			CHECK_RET(DEMUX,ret);
			ad_channel_jiuzhou_logo = 0;
		}
	g_filter_nms_logo=0;
	app_porting_psi_demux_unlock();

	return 0;
}


int GxAD_JiuZhou_LogoFilterOpen(void)
{
	uint16_t pid = AD_JIUZHOU_LOGO_PID;
	handle_t filter;
	uint8_t     match[18] = {0};
	uint8_t     mask[18] = {0};
	int32_t  ret = 0;

	PRINTF("GxAD_JiuZhou_LogoFilterOpen\n");

	match[0] = 0xCA;
	mask[0] = 0xff;

	app_porting_psi_demux_lock();

	if (0 != ad_filter_jiuzhou_logo)
	{
//			ret = GxDemux_FilterDisable(ad_filter_jiuzhou_pf);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter_jiuzhou_logo);
			CHECK_RET(DEMUX,ret);

			ad_filter_jiuzhou_logo = 0;
	}
	if (0 != ad_channel_jiuzhou_logo)
	{
			ret = GxDemux_ChannelFree(ad_channel_jiuzhou_logo);
			CHECK_RET(DEMUX,ret);
			ad_channel_jiuzhou_logo= 0;
	}

	ad_channel_jiuzhou_logo = GxDemux_ChannelAllocate(0, pid);
	if ((0 == ad_channel_jiuzhou_logo)||(-1 == ad_channel_jiuzhou_logo))
	{
			CA_ERROR(" ad_channel_jiuzhou_pf=0x%x\n",ad_channel_jiuzhou_logo);
			ad_channel_jiuzhou_logo = 0;
			app_porting_psi_demux_unlock();
			return 0;
	}


	filter = GxDemux_FilterAllocate(ad_channel_jiuzhou_logo);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(ad_channel_jiuzhou_logo);
		CHECK_RET(DEMUX,ret);
		ad_channel_jiuzhou_logo = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}

	ad_filter_jiuzhou_logo = filter;

	ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 1);	
	CHECK_RET(DEMUX,ret);

	ret = GxDemux_ChannelEnable(ad_channel_jiuzhou_logo);
	CHECK_RET(DEMUX,ret);


//	GxCA_DemuxFilterRigsterNotify(filter, adfilter_notify,NULL);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);

	g_filter_nms_logo=0;
	
	app_porting_psi_demux_unlock();
	
	return 0;
}


int GxAD_JiuZhou_BannerFilterOpen(uint8_t version)
{
	uint16_t pid = AD_JIUZHOU_BANNER_PID;
	handle_t filter;
	uint8_t     match[18] = {0};
	uint8_t     mask[18] = {0};
	int32_t  ret = 0;

	PRINTF("GxAD_JiuZhou_BannerFilterOpen\n");

	match[0] = 0xC0;
	mask[0] = 0xff;

    	match[5] = (version<<1)&0x3E;
    	mask[5]  = 0x3E;
	app_porting_psi_demux_lock();
	if (0 != ad_filter_jiuzhou_pf)
		{
//			ret = GxDemux_FilterDisable(ad_filter_jiuzhou_pf);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter_jiuzhou_pf);
			CHECK_RET(DEMUX,ret);
			ad_filter_jiuzhou_pf = 0;
		}
		if (0 != ad_channel_jiuzhou_pf)
		{
			ret = GxDemux_ChannelFree(ad_channel_jiuzhou_pf);
			CHECK_RET(DEMUX,ret);
			ad_channel_jiuzhou_pf = 0;
		}

		ad_channel_jiuzhou_pf = GxDemux_ChannelAllocate(0, pid);
		if ((0 == ad_channel_jiuzhou_pf)||(-1 == ad_channel_jiuzhou_pf))
		{
				CA_ERROR("	ad_channel_jiuzhou_pf=0x%x\n",ad_channel_jiuzhou_pf);
				ad_channel_jiuzhou_pf = 0;
				app_porting_psi_demux_unlock();
				return 0;
		}
	
		filter = GxDemux_FilterAllocate(ad_channel_jiuzhou_pf);
		if (( 0 == filter)||(-1 == filter))
		{
			ret = GxDemux_ChannelFree(ad_channel_jiuzhou_pf);
			CHECK_RET(DEMUX,ret);
			ad_channel_jiuzhou_pf = 0;
	
			CA_ERROR(" filter=0x%x\n",filter);
			filter = 0;
			app_porting_psi_demux_unlock();
			return 0;
		}
	
		ad_filter_jiuzhou_pf = filter;

		if (32 == version)
		{
			ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 1);	
			CHECK_RET(DEMUX,ret);
		}
		else
		{
			ret = GxDemux_FilterSetup(filter, match, mask, FALSE, TRUE,0, 6);
			CHECK_RET(DEMUX,ret);
		}
	
		//GxCA_DemuxFilterRigsterNotify(filter, app_jiuzhou_ads_filter_notify,NULL);
		ret = GxDemux_ChannelEnable(ad_channel_jiuzhou_pf);
		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterEnable(filter);
		CHECK_RET(DEMUX,ret);

		g_filter_nms_pf=0;
		
		app_porting_psi_demux_unlock();

		return 0;
}


static void _adv_xw_save_pf_info()
{
	char filename[ADV_FILENAME_MAX_LENGTH]; 
	handle_t handle; 
	INT32 writed_len = -1; 

	sprintf(filename, AD_FILENAME_PF_INFO); 
	
	PRINTF("filename: %s\n", filename); 
	if (GxCore_FileExists(filename) == 1)
	{
		PRINTF("file existed. %s\n", filename); 
		GxCore_FileDelete(filename);
	}
		
  	handle = GxCore_Open(filename, "w+");
	GxCore_Seek(handle, 0, GX_SEEK_SET);

	writed_len = GxCore_Write(handle, &ad_xw_banner_info, 1, sizeof(banner_info_t)*XW_INFO_MAX_NUM);

	PRINTF("writed_len=%d\n", writed_len); 

	GxCore_Close(handle);	
} 

static void _adv_xw_load_pf_info()
{
	char filename[ADV_FILENAME_MAX_LENGTH]; 
	handle_t handle; 
	INT32 readed_len = -1; 
	UINT16 i; 

	sprintf(filename, AD_FILENAME_PF_INFO); 
	
	PRINTF("filename: %s\n", filename); 
	if (GxCore_FileExists(filename) == 0)
	{
		PRINTF("file not existed. %s\n", filename); 
		return; 
	}
		
  	handle = GxCore_Open(filename, "r");
	GxCore_Seek(handle, 0, GX_SEEK_SET);

	/*数据清空*/ 
	ad_xw_ad_count = 0; 
	memset(&ad_xw_banner_info, 0x00, sizeof(banner_info_t)*XW_INFO_MAX_NUM); 
	readed_len = GxCore_Read(handle, &ad_xw_banner_info, 1, sizeof(banner_info_t)*XW_INFO_MAX_NUM);

	PRINTF("readed_len=%d\n", readed_len); 

	GxCore_Close(handle);	

	for (i = 0; i<XW_INFO_MAX_NUM; i++)
	{
		if (ad_xw_banner_info[i].ad_id != 0)
			ad_xw_ad_count++; 
	}

	PRINTF("ad_xw_ad_count=%d\n", ad_xw_ad_count); 
}

void app_jiuzhou_ads_init(void)
{
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint16_t center_fre = 0;
	uint16_t i=0; 
	
	memset(&ads_jiuzhou_para[0],0,AD_JIUZHOU_TYPE_MAX*sizeof(ad_play_para));
	advertisement_play_init();

	PRINTF("%s=============\n", __FUNCTION__); 
	symbol_rate = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	center_fre = app_flash_get_config_center_freq();
	
	if (0 == app_search_lock_tp(center_fre, symbol_rate, INVERSION_OFF, qam,2000))
	{	
		PRINTF("%s============= lock tp ok.\n", __FUNCTION__); 
		GxAD_JiuZhou_BannerFilterOpen(32);
		GxAD_JiuZhou_LogoFilterOpen(); 
		GxCore_ThreadCreate("ad_filter_timeout",&ad_filter_timeout, _adv_filter_timeout_task, NULL, 10 * 1024, /*30*/10);

		/*无论如何先等候5秒*/ 
		GxCore_ThreadDelay(5000); 
		
		/*20秒接收超时*/ 
		while (i++ < 100)
		{
			if (_adv_xw_get_complete_flag())
			{
				/*接收完成, 关闭过滤通道*/ 
				GxAD_JiuZhou_BannerFilterClose();
				GxAD_JiuZhou_LogoFilterClose(); 
				PRINTF("%s============= recv completed!!!!!!!!!!!!!!!!!!\n", __FUNCTION__); 

				/*保存频道对应关系*/
				_adv_xw_save_pf_info(); 
				return; 
			} else
				GxCore_ThreadDelay(200); 
		}

		/*接收超时, 关闭过滤通道*/ 
		GxAD_JiuZhou_BannerFilterClose();
		GxAD_JiuZhou_LogoFilterClose(); 
		PRINTF("%s============= recv over time!!!!!!!!!!!!!!!!!!\n", __FUNCTION__); 

		ad_filter_stopped = TRUE; 
		
		return ;
	}
	else
	{	  
            	// 锁频失败
            	PRINTF("%s============= lock tp failed!!!!!!!!!!!!!!!!!!\n", __FUNCTION__); 

		/*加载频道对应关系*/
		_adv_xw_load_pf_info(); 
	}
}



static INT32 _adv_pf_info_parser(UINT8 *data, UINT32 len)
{
	UINT8 des_tag,des_len,link_des;
	UINT16 j= 0;
	UINT16 des_loop_len,sec_len;
	UINT8 max_sec = 0,sec_num = 0;
	INT32 ext_status;
	UINT8 i = ad_xw_ad_count;
	
	if(data[0] != 0xc0) return ERR_FAILURE;
	sec_len = ((data[1] & 0x0f) << 8) | data[2];

	sec_num = AD_SECTION_NUMBER(data);
	max_sec = AD_SECTION_MAXNO(data);

	PRINTF("%s, %d, %d\n", __FUNCTION__, sec_num, max_sec); 
	if((sec_num == 0) && !bInfoStart)
	{
		ad_info_sec_count = 0;
		MEMSET(ad_info_sec_tag,0,MAX_ADV_INFO_SEC_NUM);
		MEMSET(&ad_xw_banner_info,0,sizeof(banner_info_t) * XW_INFO_MAX_NUM);
		bInfoStart = TRUE;
	}

	PRINTF("bInfoStart=%d, 0x%02X, 0x%02X\n", bInfoStart, data[3], data[4]);
	if(bInfoStart && ((data[3] << 8 | data[4] ) == 0))
	{
		ext_status = _adv_pf_info_sec_check(sec_num,max_sec);
		data += 10;
		sec_len -= 11;		
		PRINTF("ext_status=%d, ad_xw_ad_count=%d, sec_len=%d\n", ext_status, ad_xw_ad_count, sec_len);
		while((ext_status == SECTION_NEW) && (ad_xw_ad_count < XW_INFO_MAX_NUM) && (sec_len > 0))
		{
			ad_xw_banner_info[ad_xw_ad_count].ad_id = data[0] << 8 | data[1];
			ad_xw_banner_info[ad_xw_ad_count].ad_type = data[2];
			ad_xw_banner_info[ad_xw_ad_count].ad_des_loop_len = data[3] << 8 | data[4];
			des_loop_len = ad_xw_banner_info[ad_xw_ad_count].ad_des_loop_len;
			ad_xw_banner_info[ad_xw_ad_count].linkage_num = 0;

			PRINTF("ad_xw_banner_info[%d], ad_id=%d\n", ad_xw_ad_count, data[0] << 8 | data[1]); 

			if(des_loop_len > sec_len) return ERR_FAILURE;
			data += 5;

			while(des_loop_len > 0)
			{
				des_tag = data[0];
				des_len = data[1];
				data += 2;
				PRINTF("%s, des_tag=%x\n", __FUNCTION__, des_tag); 
				switch(des_tag){
					case 0x84:
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_identify = DATA8_16(&data[0]);
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_size = DATA8_32(&data[2]);
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_width = DATA8_16(&data[6]);
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_height= DATA8_16(&data[8]);
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_palette = data[10];
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_type = data[11];
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_iscompress = data[13] & 0x01;
						ad_xw_banner_info[ad_xw_ad_count].ad_pic_des.pic_compress_length = DATA8_32(&data[14]);
#ifdef ADV_FILES_SAVED_ON_FLASH
						/*检查此文件是否已经存在. 如果存在, 则修改相关字段, 以备完整性检查*/
						/*这些字段主要与_adv_xw_get_complete_flag有关即recved_len*/
						_adv_xw_update_file_recved_status(AD_JIUZHOU_TYPE_BANNER, &ad_xw_banner_info[ad_xw_ad_count]); 
#endif 
						PRINTF("ad_xw_banner_info[%d], file_id: %d, file_length: %d", ad_xw_ad_count, DATA8_16(&data[0]), DATA8_32(&data[2])); 
						data += des_len;
						break;
					case 0x83:
						link_des = des_len;
						if(i != ad_xw_ad_count)
						{
							j = 0;
							i = ad_xw_ad_count;
						}
						while(link_des > 0)
						{
							if(j < XW_LINKAGE_MAX_NUM)
							{
								ad_xw_banner_info[ad_xw_ad_count].ad_linkage_des[j].ts_id = DATA8_16(&data[0]);
								ad_xw_banner_info[ad_xw_ad_count].ad_linkage_des[j].on_id = DATA8_16(&data[2]);
								ad_xw_banner_info[ad_xw_ad_count].ad_linkage_des[j].s_id = DATA8_16(&data[4]);
								PRINTF("linkage info: %X, %X, %X\n", DATA8_16(&data[0]), DATA8_16(&data[2]), DATA8_16(&data[4])); 
							}
							data += 6;
							link_des -= 6;
							j++;
						}
						if(j <= XW_LINKAGE_MAX_NUM)
						{
							ad_xw_banner_info[ad_xw_ad_count].linkage_num = j;
							PRINTF("======ad_xw_banner_info[%d].linkage_num: %d \n",ad_xw_ad_count,ad_xw_banner_info[ad_xw_ad_count].linkage_num);
						}
						else
						{
							ad_xw_banner_info[ad_xw_ad_count].linkage_num = XW_LINKAGE_MAX_NUM;
							PRINTF("\n======WARN: ad_xw_banner_info[%d] linkage num: %d > XW_LINKAGE_MAX_NUM!!!!!!!!!!!\n",ad_xw_ad_count,j);
						}
						break;
					case 0x80:
						ad_xw_banner_info[ad_xw_ad_count].ad_content_des.dis_mode = data[0];
						ad_xw_banner_info[ad_xw_ad_count].ad_content_des.row_number = data[1];
						ad_xw_banner_info[ad_xw_ad_count].ad_content_des.font_color = data[2];
						ad_xw_banner_info[ad_xw_ad_count].ad_content_des.dis_time = DATA8_16(&data[3]);
						ad_xw_banner_info[ad_xw_ad_count].ad_content_des.content_length = data[5];
						MEMCPY(ad_xw_banner_info[ad_xw_ad_count].ad_content_des.content,&data[6],data[5]);
						data += des_len;
						break;
					case 0x89:
						ad_xw_banner_info[ad_xw_ad_count].ad_custom_des.cus_fun = data[0];
						data += des_len;
						break;
					default:
						data += des_len;
						break;
					}
				
				des_loop_len -= (des_len + 2);
			}
			sec_len -= (ad_xw_banner_info[ad_xw_ad_count].ad_des_loop_len + 5);
			ad_xw_ad_count++;
			//j = 0;
		}
//		ad_xw_ad_count = i;
	}

	_adv_clean_ad_files(); 

	return SUCCESS; 
}



static INT32 _adv_pf_data_parser(UINT8 *data, UINT32 len)
{
	UINT16 extend_id = 0;
	
	INT32 ext_status;
	UINT8 max_sec = 0,sec_num = 0;
	
	extend_id = AD_SECTION_EXTID(data);
	sec_num = AD_SECTION_NUMBER(data);
	max_sec = ( data[7] + 1);

	PRINTF("%s, extend_id=%X, sec_num=%d, max_sec=%d, bBannerStart=%d\n", __FUNCTION__, extend_id, sec_num, max_sec, bBannerStart); 
	if (TRUE == _adv_xw_check_file_recved_status(AD_JIUZHOU_TYPE_BANNER, extend_id))
	{
		PRINTF("%s, this pf file already recved completed!\n", __FUNCTION__); 
		return SUCCESS; 
	}
	
	if((sec_num == 0) && !bBannerStart)
	{
		bBannerStart = TRUE;
		ad_xw_rsc_count = 0;
		ad_xw_all_banner_sec_num = 0;
		MEMSET(ad_xw_banner_sec_tag,0,MAX_SEC_NUM);
	}
	if(bBannerStart)
	{
		ext_status = _adv_pf_sec_check(extend_id,sec_num,max_sec);
		PRINTF("EXT_ID = 0x%x, sec_num =%d, max_sec = %d,ad_xw_rsc_count = %d, ext_status=%d\n",extend_id, sec_num,max_sec,ad_xw_rsc_count, ext_status);
		if(ext_status == SECTION_NEW )
		{	
			PRINTF("SECTION_NEW, len = %d\n", len); 
			_adv_pf_section_parser(data, len); 
		}
		else if(ext_status == SECTION_ERROR)
		{
			//do nothing!
		}
		else
		{
			//xw_filter_abort(xw_ctx,XW_ADV_EVENT_COMPLETE);
		}
	}

	return SUCCESS;	
}

static INT32 _adv_pf_parser(UINT8 *data, UINT32 len)
{
	if(data[0] != 0xc0) return ERR_FAILURE;
#if 0
	PRINTF("%s\n", __FUNCTION__); 
	UINT32 i = 0; 
	for (i=0; i<32; i++)
		PRINTF("%X ", data[i]);
	PRINTF("\n"); 
#endif 
	
	if((data[3] << 8 | data[4] ) == 0)
	{
		_adv_pf_info_parser(data, len);
	}
	else
	{
		_adv_pf_data_parser(data,len);
	}
	return SUCCESS;

}

static INT32 _adv_logo_data_parser(UINT8 *data, UINT32 len)
{
	PRINTF("%s\n", __FUNCTION__);  

	UINT16 extend_id = 0;
	
	INT32 ext_status;
	UINT8 max_sec = 0,sec_num = 0;
	
	
	extend_id = AD_SECTION_EXTID(data);
	sec_num = AD_SECTION_NUMBER(data);
	max_sec = ( data[7] + 1);

	PRINTF("%s, extend_id=%X, sec_num=%d, max_sec=%d, bLogoStart=%d\n", __FUNCTION__, extend_id, sec_num, max_sec, bLogoStart); 
	if (TRUE == _adv_xw_check_file_recved_status(AD_JIUZHOU_TYPE_LOGO, extend_id))
	{
		PRINTF("%s, this pf file already recved completed!\n", __FUNCTION__); 
		return SUCCESS; 
	}
	
	if((sec_num == 0) && !bLogoStart)
	{
		bLogoStart = TRUE;

		MEMSET(ad_logo_sec_tag,0,MAX_SEC_NUM);
		ad_logo_sec_count = 0; 
		MEMSET(&ad_logo_info,0,sizeof(logo_info_t));
	}
	if(bLogoStart)
	{
		ext_status = _adv_logo_sec_check(sec_num,max_sec);
		PRINTF("EXT_ID = 0x%x, sec_num =%d, max_sec = %d,ext_status=%d\n",extend_id, sec_num,max_sec,ext_status);
		if(ext_status == SECTION_NEW )
		{	
			PRINTF("SECTION_NEW, len = %d\n", len); 

			_adv_logo_section_parser(data, len);
		}
		else if(ext_status == SECTION_ERROR)
		{
			//do nothing!
		}
		else
		{
			//xw_filter_abort(xw_ctx,XW_ADV_EVENT_COMPLETE);
		}
	}
	
	return SUCCESS;
}

static INT32 _adv_logo_section_parser(UINT8 *data, UINT32 len)
{
	UINT8 extend_id = 0, max_sec = 0, sec_num = 0;
	UINT32 sec_len = 0, data_len = 0; 
	UINT32 pic_size = 0; 

	// new data arrived. 
	extend_id = AD_SECTION_EXTID(data);
	sec_num = AD_SECTION_NUMBER(data);
	max_sec = AD_SECTION_MAXNO(data); 
	pic_size = DATA8_32(&data[10]); 
	
	PRINTF("%s, extend_id=%X, sec_num=%d, max_sec=%d\n", __FUNCTION__, extend_id, sec_num, max_sec); 

	/*得到实际数据长度*/ 
	sec_len = ((data[1] & 0x0f) << 8) | data[2];
	data_len = sec_len - 19; 

	/* 数据是否已经处理*/
	if (ad_logo_info.p_data[sec_num] != NULL)
	{
		PRINTF("sec_num=%d already recved!\n", sec_num); 
		return ERR_FAILURE; 
	}

	ad_logo_info.pic_size = pic_size; 
#ifdef ADV_FILES_SAVED_ON_FLASH
	_adv_xw_update_file_recved_status(AD_JIUZHOU_TYPE_LOGO, NULL); 
#endif 
	/* 将数据内容保存到内存中*/
	ad_logo_info.p_data[sec_num] = GxCore_Malloc(data_len); 
	data = data + 18; 
	memcpy(ad_logo_info.p_data[sec_num], data, data_len); 
	ad_logo_info.p_data_len[sec_num] = data_len; 
	
	/* 全部已接收的数据大小 */
	ad_logo_info.recved_len += data_len; 
	
	/* 判断是否接收完成 */
	PRINTF("recved: %d, size=%d\n", ad_logo_info.recved_len, ad_logo_info.pic_size); 
	if (ad_logo_info.recved_len >=ad_logo_info.pic_size)
	{
		char filename[32]; 
		handle_t handle; 
		UINT16 i ; 

		/*数据接收完成, 停止这个过滤器*/
		GxAD_JiuZhou_LogoFilterClose(); 
		
		PRINTF("%s logo recv completed!file_length=%d\n", __FUNCTION__, ad_logo_info.pic_size); 

		sprintf(filename, ADV_FILENAME_LOGO); 
		
		if (GxCore_FileExists(filename) == 1)
		{
			PRINTF("file existed. %s\n", filename); 
			GxCore_FileDelete(filename);
		}
			
  		handle = GxCore_Open(filename, "w+");
		GxCore_Seek(handle, 0, GX_SEEK_SET);

		for (i=0; i<MAX_SEC_NUM; i++)
		{
			if ((ad_logo_info.p_data_len[i] != 0) && (ad_logo_info.p_data[i] != NULL))
			{
				INT32 writed_len = 0; 
				
				writed_len = GxCore_Write(handle, ad_logo_info.p_data[i], 1, ad_logo_info.p_data_len[i]);

				PRINTF("i=%d, writed_len=%d\n", i, writed_len); 

				if (writed_len > 0)
					GxCore_Free(ad_logo_info.p_data[i]); 
			}
		}
		GxCore_Close(handle);	
	}

	return SUCCESS; 
}

static INT32 _adv_pf_section_parser(UINT8 *data, UINT32 len)
{
	UINT8 extend_id = 0, max_sec = 0, sec_num = 0;
	INT32 pos = 0; 
	UINT32 sec_len = 0, data_len = 0; 

	// new data arrived. 
	extend_id = AD_SECTION_EXTID(data);
	sec_num = AD_SECTION_NUMBER(data);
	max_sec = AD_SECTION_MAXNO(data); 
	PRINTF("%s, extend_id=%X, sec_num=%d, max_sec=%d\n", __FUNCTION__, extend_id, sec_num, max_sec); 
	
	// if table_id extension invalid 
	if(extend_id == 0)
		return ERR_FAILURE; 

	pos = _adv_xw_get_info_pos_by_pic_identify(extend_id); 
	if (pos < 0)
		return ERR_FAILURE;  

	sec_len = ((data[1] & 0x0f) << 8) | data[2];
	data_len = sec_len - 19; 

	/* 数据是否已经处理*/
	if (ad_xw_banner_info[pos].p_data[sec_num] != NULL)
	{
		PRINTF("sec_num=%d already recved!\n", sec_num); 
		return ERR_FAILURE; 
	}

	/* 将数据内容保存到内存中*/
	ad_xw_banner_info[pos].p_data[sec_num] = GxCore_Malloc(data_len); 
	data = data + 18; 
	memcpy(ad_xw_banner_info[pos].p_data[sec_num], data, data_len); 
	ad_xw_banner_info[pos].p_data_len[sec_num] = data_len; 
	
	/* 全部已接收的数据大小 */
	ad_xw_banner_info[pos].recved_len += data_len; 

	//PRINTF("data = %X, %X, %X, %X\n", data[0], data[1], data[2], data[3]); 

	/* 判断是否接收完成 */
	PRINTF("recved: %d, size=%d\n", ad_xw_banner_info[pos].recved_len, ad_xw_banner_info[pos].ad_pic_des.pic_size); 
	if (ad_xw_banner_info[pos].recved_len >=ad_xw_banner_info[pos].ad_pic_des.pic_size)
	{
		char filename[32]; 
		handle_t handle; 
		UINT16 i ; 
		
		PRINTF("%s recv completed!file_id=%X, file_length=%d, pic_type=%d\n", __FUNCTION__
			, ad_xw_banner_info[pos].ad_pic_des.pic_identify, ad_xw_banner_info[pos].ad_pic_des.pic_size
			, ad_xw_banner_info[pos].ad_pic_des.pic_type); 

		if (NULL == _adv_get_file_name(&ad_xw_banner_info[pos].ad_pic_des, filename))
			return ERR_FAILURE; 

		PRINTF("filename: %s\n", filename); 
		if (GxCore_FileExists(filename) == 1)
		{
			PRINTF("file existed. %s\n", filename); 
			GxCore_FileDelete(filename);
		}
			
  		handle = GxCore_Open(filename, "w+");
		GxCore_Seek(handle, 0, GX_SEEK_SET);

		for (i=0; i<MAX_SEC_NUM; i++)
		{
			if ((ad_xw_banner_info[pos].p_data_len[i] != 0) && (ad_xw_banner_info[pos].p_data[i] != NULL))
			{
				INT32 writed_len = 0; 

				if (i == 0)
				{
					UINT8 *p = ad_xw_banner_info[pos].p_data[i]; 
					PRINTF("header: %02X %02X %02X %02X\n", p[0], p[1], p[2], p[3]); 
				}
				
				writed_len = GxCore_Write(handle, ad_xw_banner_info[pos].p_data[i], 1, ad_xw_banner_info[pos].p_data_len[i]);

				PRINTF("i=%d, writed_len=%d\n", i, writed_len); 

				if (writed_len > 0)
					GxCore_Free(ad_xw_banner_info[pos].p_data[i]); 
			}
		}
		GxCore_Close(handle);	
	}
	
	return SUCCESS; 
}


char *_adv_xw_get_banner_filename(UINT16 s_id, char *filename)
{
	UINT8 i = 0;
	UINT8 j = 0;
	
	for(i = 0; i< ad_xw_ad_count; i++){
		for(j =0; j< ad_xw_banner_info[i].linkage_num; j++){
			if(s_id == ad_xw_banner_info[i].ad_linkage_des[j].s_id){
				
				_adv_get_file_name(&ad_xw_banner_info[i].ad_pic_des, filename);
				
				PRINTF("%s, s_id = %02X, filename=%s\n", __FUNCTION__, s_id, filename); 
				
				return filename; 
			}
		}
	}

	filename[0] = '\0'; 
	
	return NULL; 
}

static INT32 _adv_xw_get_info_pos_by_pic_identify(UINT16 pic_identify)
{
	UINT8 i = 0;

	for(i = 0; i< ad_xw_ad_count; i++){
		if (ad_xw_banner_info[i].ad_pic_des.pic_identify == pic_identify){
			return i; 
		}
	}

	return -1; 
}

static char *_adv_get_file_name(xw_pic_des_info *pic_info, char *filename)
{
	if (pic_info->pic_type == 0x02)
		sprintf(filename, "%s%d.jpg", ADV_FILENAME_BANNER_HEADER, pic_info->pic_identify); 
	else
		sprintf(filename, "%s%d.unknown", ADV_FILENAME_BANNER_HEADER, pic_info->pic_identify); 

	return filename; 
}

static BOOL _adv_xw_check_file_recved_status(AD_JIUZHOU_SHOW_TYPE_T type, UINT16 pic_id)
{
	INT32 pos = -1; 

	switch (type)
	{
		case AD_JIUZHOU_TYPE_BANNER: 
			pos = _adv_xw_get_info_pos_by_pic_identify(pic_id); 
			if (pos < 0)
				return FALSE; 
				
			if ((ad_xw_banner_info[pos].ad_pic_des.pic_size > 0)
				&&(ad_xw_banner_info[pos].recved_len >= ad_xw_banner_info[pos].ad_pic_des.pic_size))
				return TRUE; 
			break; 
		case AD_JIUZHOU_TYPE_LOGO: 
			if ((ad_logo_info.pic_size > 0) &&(ad_logo_info.recved_len >= ad_logo_info.pic_size))
				return TRUE; 
			break; 
		default:
			break; 
	}

	return FALSE; 
}

static void _adv_xw_update_file_recved_status(AD_JIUZHOU_SHOW_TYPE_T type, banner_info_t *p_info)
{
	char filename[ADV_FILENAME_MAX_LENGTH]; 
	GxFileInfo file_info; 

	switch (type)
	{
		case AD_JIUZHOU_TYPE_BANNER: 
			if ((p_info->ad_pic_des.pic_size > 0) && (p_info->recved_len >= p_info->ad_pic_des.pic_size))
				return; 

			_adv_get_file_name(&p_info->ad_pic_des, filename); 

			if (GxCore_FileExists(filename) == 1)
			{
				/*如果已存在这个文件, 则取文件大小, 写入banner_info_t中*/ 
				GxCore_GetFileInfo(filename, &file_info); 
				if (file_info.size_by_bytes == p_info->ad_pic_des.pic_size)
				{
					/*这个就表明文件已接收完成, 因为_adv_xw_get_complete_flag会用到这个变量*/ 
					p_info->recved_len = p_info->ad_pic_des.pic_size; 
					PRINTF("%s file existed! not need to recved again. size=%d\n", filename, file_info.size_by_bytes); 
				}
			}
			break; 
		case AD_JIUZHOU_TYPE_LOGO:
			if ((ad_logo_info.pic_size > 0) && (ad_logo_info.recved_len >= ad_logo_info.pic_size))
				return; 

			sprintf(filename, ADV_FILENAME_LOGO); 
			if (GxCore_FileExists(filename) == 1)
			{
				/*如果已存在这个文件, 则取文件大小, 写入banner_info_t中*/ 
				GxCore_GetFileInfo(filename, &file_info); 
				if (file_info.size_by_bytes == ad_logo_info.pic_size)
				{
					/*这个就表明文件已接收完成, 因为_adv_xw_get_complete_flag会用到这个变量*/ 
					ad_logo_info.recved_len = ad_logo_info.pic_size; 
					PRINTF("%s file existed! not need to recved again. size=%d\n", filename, file_info.size_by_bytes); 
				}
			}
			break; 
		default:
			break; 
	}
}

/*判断广告数据是否接收完成*/ 
static BOOL _adv_xw_get_complete_flag(void)
{
	UINT8 i = 0;

	/*先判断开机LOGO数据*/
	if ((ad_logo_info.pic_size > 0) && (ad_logo_info.recved_len < ad_logo_info.pic_size)) 
		return FALSE; 

	/*再判断换台广告数据*/ 
	if (ad_xw_ad_count == 0)
		return FALSE; 
	
	for(i = 0; i< ad_xw_ad_count; i++){
		if (ad_xw_banner_info[i].recved_len< ad_xw_banner_info[i].ad_pic_des.pic_size){
			return FALSE; 
		}
	}

	return TRUE; 
}

/*整理文件. 如果不是本广告系统中的文件, 则删除掉.*/ 
static void _adv_clean_ad_files(void)
{
	/*根据ad_xw_banner_info中的文件ID所关联的文件名进行整理*/
	/*注意!!!==>openpicture.bin需要保留*/ 
	
	PRINTF("%s=========\n", __FUNCTION__); 
}

static INT32 _adv_pf_info_sec_check(UINT8 sec_num,UINT8 max_sec)
{
	UINT16 i = 0;
	if(ad_info_sec_count >= MAX_ADV_INFO_SEC_NUM)
	{
		return SECTION_ERROR;
	}
	if((sec_num >= MAX_ADV_INFO_SEC_NUM-1) || (max_sec >= MAX_ADV_INFO_SEC_NUM))
	{
		return SECTION_ERROR;
	}
	do{
		if((ad_info_sec_tag[i].max_sec == max_sec) && (ad_info_sec_tag[i].sec_num == sec_num))
		{
			return SECTION_REPEAT;
		}
	}while( i++ <= ad_info_sec_count);
	ad_info_sec_tag[ad_info_sec_count].max_sec = max_sec;
	ad_info_sec_tag[ad_info_sec_count].sec_num = sec_num;
	ad_info_sec_count ++;

	return SECTION_NEW;
	
}


static INT32 _adv_logo_sec_check(UINT8 sec_num,UINT8 max_sec)
{
	UINT16 i = 0;
	if(ad_logo_sec_count >= MAX_SEC_NUM)
	{
		return SECTION_ERROR;
	}
	if((sec_num >= MAX_SEC_NUM-1) || (max_sec >= MAX_SEC_NUM))
	{
		return SECTION_ERROR;
	}
	do{
		if((ad_logo_sec_tag[i].max_sec == max_sec) && (ad_logo_sec_tag[i].sec_num == sec_num))
		{
			return SECTION_REPEAT;
		}
	}while( i++ <= ad_logo_sec_count);
	ad_logo_sec_tag[ad_logo_sec_count].max_sec = max_sec;
	ad_logo_sec_tag[ad_logo_sec_count].sec_num = sec_num;
	ad_logo_sec_count ++;

	return SECTION_NEW;
	
}

INT32 _adv_pf_sec_check(UINT16 extend_id,UINT8 sec_num,UINT8 max_sec)
{
	UINT16 i = 0;
	//static UINT16 ad_xw_all_banner_sec_num = 0;
	BOOL  NEW_EXTEND_ID = TRUE;
	do{
		if
		(	(ad_xw_banner_sec_tag[i].ext_id == extend_id) &&
			(ad_xw_banner_sec_tag[i].max_sec == max_sec) &&
			(ad_xw_banner_sec_tag[i].sec_num == sec_num))
			{
				if(ad_xw_rsc_count >= ad_xw_all_banner_sec_num)
					return SECTION_REPEAT;
				else
					return SECTION_ERROR;
			}
		if((ad_xw_banner_sec_tag[i].ext_id == extend_id) &&(ad_xw_banner_sec_tag[i].max_sec == max_sec))
		{
			NEW_EXTEND_ID = FALSE;
		}
	}while( i++ <= ad_xw_rsc_count);
	if(NEW_EXTEND_ID)
	{
		ad_xw_all_banner_sec_num += max_sec;
	}
	ad_xw_banner_sec_tag[ad_xw_rsc_count].ext_id = extend_id;
	ad_xw_banner_sec_tag[ad_xw_rsc_count].max_sec = max_sec;
	ad_xw_banner_sec_tag[ad_xw_rsc_count].sec_num = sec_num;
	ad_xw_rsc_count ++;

	return SECTION_NEW;
	
}

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
1.0  	2013.11.06		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "ads_api_xsm_2.0.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_xinshimao_ads_porting_stb_api.h"
#include "app_common_prog.h"





extern ad_play_para xinshimaoadspara[AD_TYPE_MAX];
static S_YTDY_AD_SHOW_DATA_T s_ad_data={0};//全局变量
u_int8_t pic_data_buffer[MAX_PIC_AD_SIZE]={0};

static GxAvRect rectgif = {0};

//#define SCROLL_STEP (2)
//#define SCROLL_DURATION (20)
#define GIF_DURATION (200)
#define TEST_SCROLL_JEPG (0)
#if  TEST_SCROLL_JEPG
u_int8_t app_xinshimao_ads_get_play_type(void);
/*static*/ int timer_scroll_bmp_test(void *userdata)
{
	int rtn = 0;
	u_int32_t start_y = 50;
	u_int32_t width = 3044;//2056;
	u_int32_t height = 30;

	u_int8_t type = app_xinshimao_ads_get_play_type();
	if ((type < AD_TYPE_MAX)&&(2 == xinshimaoadspara[type].type))
		{
			xinshimaoadspara[type].file_flash_path = NULL;
			xinshimaoadspara[type].play_mode = 2;
			xinshimaoadspara[type].times_count= 1;

			rtn = advertisement_scrolling_by_step(&xinshimaoadspara[type],start_y,width,height);
			if (0 == rtn)
				{
					app_xinshimao_ads_stop(type);
				}

			return 0;
		}
	
	return 0;
}


void app_xinshimao_ads_play_test(uint8_t type)
{

	app_flash_get_ad_data_to_ddram_file("/theme/2-2.jpg","/mnt/ad_roll_pic.jpeg");	
	xinshimaoadspara[type].file_flash_path = NULL;
	xinshimaoadspara[type].file_ddram_path = "/mnt/ad_roll_pic.jpeg";
	xinshimaoadspara[type].play_mode = 2;

	xinshimaoadspara[type].rectdest.x = 0;//pAdShowData->start_x;
	xinshimaoadspara[type].rectdest.y = 50;//pAdShowData->start_y;
	xinshimaoadspara[type].type = 2;
	xinshimaoadspara[type].times_count = 1;
	if (2 == xinshimaoadspara[type].type)
		{
			xinshimaoadspara[type].ad_timer = create_timer(timer_scroll_bmp_test, SCROLL_DURATION, NULL, TIMER_REPEAT);												
		}
	
}
#endif

u_int8_t app_xinshimao_ads_get_play_type(void)
{
	u_int8_t i;
	for (i = 0; i<AD_TYPE_MAX; i++ )
		{
			if (TRUE == app_xinshimao_ads_play_state(i))
				return i;
		}
	return AD_TYPE_MAX;
}



static int timer_scroll_bmp(void *userdata)
{
	int rtn = 0;
	u_int8_t type = app_xinshimao_ads_get_play_type();
	if ((type < AD_TYPE_MAX)&&(2 == xinshimaoadspara[type].type))
		{
			handle_t flash_ad_file;
			size_t	 size;

			S_YTDY_AD_SHOW_DATA_T *pAdShowData = NULL;
			pAdShowData = GxCore_Malloc(sizeof(S_YTDY_AD_SHOW_DATA_T));
			if (NULL == pAdShowData)
				{
					return 0;
				}
			flash_ad_file = GxCore_Open("/mnt/ad_roll_pic.inf", "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdShowData, 1, sizeof(S_YTDY_AD_SHOW_DATA_T));
			GxCore_Close(flash_ad_file);
			
			xinshimaoadspara[type].file_flash_path = NULL;
			xinshimaoadspara[type].play_mode = 2;
			xinshimaoadspara[type].type = 2;
			xinshimaoadspara[type].times_count= 1;


			rtn = advertisement_scrolling_by_step(&xinshimaoadspara[type],pAdShowData->start_y,pAdShowData->width,pAdShowData->height);
			if (0 == rtn)
				{
					app_xinshimao_ads_stop(type);
				}

			GxCore_Free(pAdShowData);
			pAdShowData = NULL; 
			return 0;

		}
	
	return 0;
}




static int timer_gif_play(void *userdata)
{
	u_int8_t type = app_xinshimao_ads_get_play_type();
	if ((type < AD_TYPE_MAX)&&(1 == xinshimaoadspara[type].type))
		{
			xinshimaoadspara[type].slice_num++;
			printf("type = %d gifplaypara.slice_num=%d gifplaypara.slice_count=%d\n",
				type,xinshimaoadspara[type].slice_num,xinshimaoadspara[type].slice_count);
			if (xinshimaoadspara[type].slice_num < xinshimaoadspara[type].slice_count)
			{
				xinshimaoadspara[type].rectdest.width= 0;
				xinshimaoadspara[type].rectdest.height= 0;
				app_play_ad_ddram_file(&xinshimaoadspara[type]);
				rectgif.x = xinshimaoadspara[type].rectdest.x;
				rectgif.y = xinshimaoadspara[type].rectdest.y;
				if (rectgif.width < xinshimaoadspara[type].rectdest.width)
					rectgif.width = xinshimaoadspara[type].rectdest.width;
				if (rectgif.height < xinshimaoadspara[type].rectdest.height)
					rectgif.height = xinshimaoadspara[type].rectdest.height;
			}
		else
			{
				app_xinshimao_ads_stop(type);
			}			
		}
	
	return 0;
}


void app_xinshimao_ads_stop(uint8_t type)
{
	if(type >= AD_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}

	
//	printf("%s file_ddram_path=%s\n",__FUNCTION__,xinshimaoadspara[type].file_ddram_path);	
	
	if (NULL != xinshimaoadspara[type].ad_timer)
		{
			remove_timer(xinshimaoadspara[type].ad_timer);
			xinshimaoadspara[type].ad_timer = NULL;
		}

		if (NULL != xinshimaoadspara[type].file_ddram_path)
		{
			if (1 == xinshimaoadspara[type].type)
				{
					/*
					* gif
					*/
					memcpy(&xinshimaoadspara[type].rectdest,&rectgif,sizeof(GxAvRect));
					memset(&rectgif,0,sizeof(GxAvRect));					
				}			
//			printf("%s %d advertisement_clear_frame\n",__FILE__,__LINE__);
			advertisement_clear_frame(xinshimaoadspara[type].rectdest);
			memset(&xinshimaoadspara[type],0,sizeof(ad_play_para));	
			memset(&s_ad_data,0,sizeof(S_YTDY_AD_SHOW_DATA_T));
		}
	return ;
}

void app_xinshimao_ads_play(uint8_t type)
{
	handle_t flash_ad_file;
	size_t	 size;

	u_int16_t ts_id = 0;
	u_int16_t ser_id = 0;

	char inf_name[50]={0};
	u_int8_t cur_type = app_xinshimao_ads_get_play_type();



	YTDY_AD_ERR_E nRet = 0;
	GxBusPmDataProg Prog;
	uint32_t pos;

	if (type == AD_TYPE_ROLL_PIC)
		{
			if (( AD_TYPE_PF_INFO == cur_type)||(AD_TYPE_EPG == cur_type)
				||(AD_TYPE_MAIN_MENU == cur_type)||(AD_TYPE_ROLL_PIC == cur_type))
				{
//					printf("%s cur_type=%d\n",__FUNCTION__,cur_type);	
					return;
				}
		}

	if (type == AD_TYPE_FS_RIGHT)
		{
			if (( AD_TYPE_PF_INFO == cur_type)||(AD_TYPE_EPG == cur_type)
				||(AD_TYPE_MAIN_MENU == cur_type)||(AD_TYPE_ROLL_PIC == cur_type)
				||(AD_TYPE_FS_RIGHT == cur_type ))
				{
//					printf("%s cur_type=%d\n",__FUNCTION__,cur_type);	
					return;
				}
		}

	if (app_prog_get_num_in_group()>0)
		{
			app_prog_get_playing_pos_in_group(&pos);
			GxBus_PmProgGetByPos(pos,1,&Prog);
			ser_id = Prog.service_id;
			ts_id = Prog.ts_id; 						
		}


	if(type >= AD_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return ;
		}

	
	s_ad_data.p_pic_data = pic_data_buffer;
	

	switch(type)
		{
			case AD_TYPE_NONE:
				break;
			case AD_TYPE_ROLL_PIC:
				/*
				* save roll pic ad to ddram file
				*/				
				nRet = YTDY_AD_GetPicture(AD_ROLL_PIC, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_xinshimao_ads_stop(AD_TYPE_FS_RIGHT);
						app_save_data_to_ddram_file((const char*)&s_ad_data,sizeof(S_YTDY_AD_SHOW_DATA_T),"/mnt/ad_roll_pic.inf");
						strcpy(inf_name,"/mnt/ad_roll_pic.inf");
						switch(s_ad_data.pic_format)
							{
								case 'G':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_roll_pic.gif");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_roll_pic.gif";
									break;
								case 'B':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_roll_pic.bmp");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_roll_pic.bmp";
									break;
								case 'J':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_roll_pic.jpeg");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_roll_pic.jpeg";
									break;
								default:
									return;
							}
					}
				else
					{
						return ;
					}
				break;
			case AD_TYPE_OPEN_LOGO:
				nRet = YTDY_AD_GetPicture(AD_OPEN_LOGO, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_flash_save_logo_data((char*)s_ad_data.p_pic_data,s_ad_data.size);						
					}				
				return;
			case AD_TYPE_PF_INFO:				
				/*
				* save pf info ad to ddram file
				*/
				app_xinshimao_ads_stop(AD_TYPE_FS_RIGHT);
				app_xinshimao_ads_stop(AD_TYPE_ROLL_PIC);
				nRet = YTDY_AD_GetPicture(AD_PF_INFO, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_save_data_to_ddram_file((const char*)&s_ad_data,sizeof(S_YTDY_AD_SHOW_DATA_T),"/mnt/ad_pf_info.inf");	
						strcpy(inf_name,"/mnt/ad_pf_info.inf");
						switch(s_ad_data.pic_format)
							{
								case 'G':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_pf_info.gif");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_pf_info.gif";
									break;
								case 'B':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_pf_info.bmp");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_pf_info.bmp";
									break;
								case 'J':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_pf_info.jpeg");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_pf_info.jpeg";
									break;
								default:
									return;
							}

					}
				else
					{
						
						return ;
					}

				break;
			case AD_TYPE_FS_RIGHT:
				/*
				* save fs right ad to ddram file
				*/
				nRet = YTDY_AD_GetPicture(AD_FS_RIGHT, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_save_data_to_ddram_file((const char*)&s_ad_data,sizeof(S_YTDY_AD_SHOW_DATA_T),"/mnt/ad_fs_right.inf");	
						strcpy(inf_name,"/mnt/ad_fs_right.inf");
						switch(s_ad_data.pic_format)
							{
								case 'G':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_fs_right.gif");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_fs_right.gif";
									break;
								case 'B':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_fs_right.bmp");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_fs_right.bmp";
									break;
								case 'J':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_fs_right.jpeg");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_fs_right.jpeg";
									break;
								default:
									return;
							}
					}
				else
					{
						return;
					}
				break;
			case AD_TYPE_EPG:
				/*
				* save epg ad to ddram file
				*/
				app_xinshimao_ads_stop(AD_TYPE_ROLL_PIC);
				app_xinshimao_ads_stop(AD_TYPE_FS_RIGHT);
				nRet = YTDY_AD_GetPicture(AD_EPG, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_save_data_to_ddram_file((const char*)&s_ad_data,sizeof(S_YTDY_AD_SHOW_DATA_T),"/mnt/ad_epg.inf");	
						strcpy(inf_name,"/mnt/ad_epg.inf");
						switch(s_ad_data.pic_format)
							{
								case 'G':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_epg.gif");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_epg.gif";
									break;
								case 'B':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_epg.bmp");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_epg.bmp";
									break;
								case 'J':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_epg.jpeg");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_epg.jpeg";
									break;
								default:
									return;
							}
					}
				else
					{
						return ;
					}

				break;
			case AD_TYPE_CH_LIST:
				break;
			case AD_TYPE_MAIN_MENU:
				/*
				* save epg ad to ddram file
				*/
				app_xinshimao_ads_stop(AD_TYPE_ROLL_PIC);
				app_xinshimao_ads_stop(AD_TYPE_FS_RIGHT);
				nRet = YTDY_AD_GetPicture(AD_MAIN_MENU, ser_id, ts_id, &s_ad_data);
				if(nRet == YTDY_AD_SUCCESS )
					{
						app_save_data_to_ddram_file((const char*)&s_ad_data,sizeof(S_YTDY_AD_SHOW_DATA_T),"/mnt/ad_main_menu.inf");	
						strcpy(inf_name,"/mnt/ad_main_menu.inf");
						switch(s_ad_data.pic_format)
							{
								case 'G':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_main_menu.gif");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_main_menu.gif";
									break;
								case 'B':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_main_menu.bmp");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_main_menu.bmp";
									break;
								case 'J':
									app_save_data_to_ddram_file((const char*)s_ad_data.p_pic_data,s_ad_data.size,"/mnt/ad_main_menu.jpeg");									
									xinshimaoadspara[type].file_ddram_path = "/mnt/ad_main_menu.jpeg";
									break;
								default:
									return;
							}
					}
				else
					{
						return ;
					}
				break;
			case AD_TYPE_RADIO_LOGO:
			case AD_TYPE_FAV_LIST:
			case AD_TYPE_NVOD:
			case AD_TYPE_VOLUME:
			case AD_TYPE_ROLLING_PIC:
			default:
				return;
		}



	if (GxCore_FileExists(inf_name) == 1) 
		{
			S_YTDY_AD_SHOW_DATA_T *pAdShowData = NULL;
			pAdShowData = GxCore_Malloc(sizeof(S_YTDY_AD_SHOW_DATA_T));
			flash_ad_file = GxCore_Open(inf_name, "r+");
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, pAdShowData, 1, sizeof(S_YTDY_AD_SHOW_DATA_T));
			GxCore_Close(flash_ad_file);

			switch(s_ad_data.pic_format)
				{
					case 'G':
						xinshimaoadspara[type].type = 1;
						break;
					default:
						break;
				}

			
			xinshimaoadspara[type].file_flash_path = NULL;
			xinshimaoadspara[type].play_mode = 2;

			switch(type)
				{
					case AD_TYPE_NONE:
						GxCore_Free(pAdShowData);
						pAdShowData = NULL;						
						return;
					case AD_TYPE_ROLL_PIC:
						xinshimaoadspara[type].rectdest.x = 0;//pAdShowData->start_x;
						xinshimaoadspara[type].rectdest.y = pAdShowData->start_y;
						xinshimaoadspara[type].type = 2;
						xinshimaoadspara[type].times_count = 1;
						break;
					case AD_TYPE_OPEN_LOGO:
						GxCore_Free(pAdShowData);
						pAdShowData = NULL;						
						return;
					case AD_TYPE_PF_INFO:
						/*
						* save pf info ad to ddram file
						*/
						xinshimaoadspara[type].rectdest.x = 950;//843;
						xinshimaoadspara[type].rectdest.y = 523;
						break;
					case AD_TYPE_FS_RIGHT:
						xinshimaoadspara[type].rectdest.x = 1000;
						xinshimaoadspara[type].rectdest.y = 523;
						break;
					case AD_TYPE_EPG:
						xinshimaoadspara[type].rectdest.x = 450;//pAdShowData->start_x;
						xinshimaoadspara[type].rectdest.y = 415;//415;//pAdShowData->start_y;
						break;
					case AD_TYPE_CH_LIST:
						GxCore_Free(pAdShowData);
						pAdShowData = NULL;						
						return;
					case AD_TYPE_MAIN_MENU:
						xinshimaoadspara[type].rectdest.x = 530;
						xinshimaoadspara[type].rectdest.y = 486;
						break;
					case AD_TYPE_RADIO_LOGO:
					case AD_TYPE_FAV_LIST:
					case AD_TYPE_NVOD:
					case AD_TYPE_VOLUME:
					case AD_TYPE_ROLLING_PIC:
					default:
						GxCore_Free(pAdShowData);
						pAdShowData = NULL;						
						return;
				}

			printf("pAdShowData->start_x=%d\n",pAdShowData->start_x);
			printf("pAdShowData->start_y=%d\n",pAdShowData->start_y);
			printf("pAdShowData->width=%d\n",pAdShowData->width);
			printf("pAdShowData->height=%d\n",pAdShowData->height);
//			printf("pAdShowData->size=%d\n",pAdShowData->size);

			
//			duration = pAdShowData->showing_time;
//			printf("duration=%d\n",duration);
	
//			xinshimaoadspara[type].file_ddram_path = pic_name;	
			if (AD_TYPE_ROLL_PIC != type)
				{
					app_play_ad_ddram_file(&xinshimaoadspara[type]);				
				}

			memset(&rectgif,0,sizeof(GxAvRect));
			if (1 == xinshimaoadspara[type].type)
				{
					/*
					* gif
					*/
					if (rectgif.width < xinshimaoadspara[type].rectdest.width)
						rectgif.width = xinshimaoadspara[type].rectdest.width;
					if (rectgif.height < xinshimaoadspara[type].rectdest.height)
						rectgif.height = xinshimaoadspara[type].rectdest.height;			
					xinshimaoadspara[type].ad_timer = create_timer(timer_gif_play, GIF_DURATION, NULL,  TIMER_REPEAT);						
				}

			if (2 == xinshimaoadspara[type].type)
				{
					/*
					* scroll
					*/
					xinshimaoadspara[type].ad_timer = create_timer(timer_scroll_bmp, SCROLL_DURATION, NULL,	TIMER_REPEAT);												
				}


			GxCore_Free(pAdShowData);
			pAdShowData = NULL;

	}

}


u_int8_t app_xinshimao_ads_play_state(uint8_t type)
{
	if(type >= AD_TYPE_MAX)
		{
			printf("%s type=%d\n",__FUNCTION__,type);	
			return FALSE;
		}

	if (NULL != xinshimaoadspara[type].file_ddram_path)
		{
			return TRUE;
			printf("%s ddram file paht = %s\n",__FUNCTION__,xinshimaoadspara[type].file_ddram_path);
		}

	return FALSE;
}







/*****************************************************************************
 * 						   CONFIDENTIAL
 *        Hangzhou GuoXin Science and Technology Co., Ltd.
 *                      (C)2010, All right reserved
 ******************************************************************************

 ******************************************************************************
 * File Name :	app_win_interface.c
 * Author    : 	zhouhm
 * Project   :	goxceed dvbc
 * Type      :
 ******************************************************************************
 * Purpose   :
 ******************************************************************************
 * Release History:
 VERSION	Date			  AUTHOR         Description
 1.0  	2012.11.15		  zhouhm 	 			creation
 *****************************************************************************/

#include "app.h"

/*
* 该文件实现提供其他菜单/包括CA菜单可能调用接口
* 
*/

focus_window_e focus_win = MAX_WINDOW_NUM;
static uint8_t win_create_flag[MAX_WINDOW_NUM] = {0};
static uint8_t pop_msg_flag = FALSE;
static uint8_t nit_change_for_search = FALSE;


/*
* xxxxxxxx格式字符串转化成数值
*/
unsigned int htoi(const char *str)
{
	const char *cp;
	unsigned int data, bdata;

	for (cp = str, data = 0; *cp != 0; ++cp) {
		if (*cp >= '0' && *cp <= '9')
			bdata = *cp - '0';
		else if (*cp >= 'A' && *cp <= 'F')
			bdata = *cp - 'A' + 10;
		else if (*cp >= 'a' && *cp <= 'f')
			bdata = *cp - 'a' + 10;
		else
			break;
		data = (data << 4) | bdata;
	}

	return data;
}


/*
* xx.xx.xx.xx格式字符串转化成数值
*/
unsigned int convert_version_str(const char *str)
{
	const char *cp;
	unsigned int data, bdata;

	for (cp = str, data = 0; *cp != 0; ++cp) {
		if (*cp >= '0' && *cp <= '9')
			bdata = *cp - '0';
		else if (*cp >= 'A' && *cp <= 'F')
			bdata = *cp - 'A' + 10;
		else if (*cp >= 'a' && *cp <= 'f')
			bdata = *cp - 'a' + 10;
		else if ('.' == *cp)
			continue;
		else
			break;
		data = (data << 4) | bdata;
	}

	return data;
}

/*
* 在app_top下使用阻塞消息会引起引起app_top
*下的其他消息都不搜不到。
*/
uint8_t app_set_nit_change_for_search_status(uint8_t flag)
{
	printf("%s %d app_nit_change_for_search_status = %d\n",__FILE__,__LINE__,flag);

	nit_change_for_search = flag;
	return nit_change_for_search;
}

uint8_t app_get_nit_change_for_search_status(void)
{

	return nit_change_for_search;
}




uint8_t app_set_win_create_flag(focus_window_e win)
{
	if (FALSE  == win_create_flag[win])
		{
			win_create_flag[win] = TRUE;
		}
	return 1;
}

uint8_t app_set_win_destroy_flag(focus_window_e win)
{
	if (TRUE  == win_create_flag[win])
		{
			win_create_flag[win] = FALSE;
		}
	return 1;
}

uint8_t app_get_win_create_flag(focus_window_e win)
{
	return win_create_flag[win];
}

/*
* 设置模态对话框标志，同时只允许创建
* 一个模态对话框
(创建多个模态对话框会导致嵌套死循环)
*/
uint8_t app_set_pop_msg_flag_status(uint8_t flag)
{
	printf("%s %d app_set_pop_msg_flag_status = %d\n",__FILE__,__LINE__,flag);

	pop_msg_flag = flag;
	return pop_msg_flag;
}

uint8_t app_get_pop_msg_flag_status(void)
{
	//if (TRUE == pop_msg_flag)
		//printf("%s %d app_get_pop_msg_flag_status = %d\n",__FILE__,__LINE__,pop_msg_flag);

	return pop_msg_flag;
}



//设置菜单页面标记
void app_win_set_focus_video_window(focus_window_e win)
{
	app_play_set_msg_pop_type_record_pos(MESSAGE_MAX_COUNT);
	focus_win = win;
	/*返回到上一菜单之前，刷新上一菜单提示信息显示
	* 避免上衣菜单信息提示错误
	*/
	app_play_set_prompt_reflush_flag(1);
	if (FULL_SCREEN_WIN != win)
		app_win_prompt_msg_exec_call_back();

}
//获取页面id号
focus_window_e app_win_get_focus_video_window(void)
{
	return focus_win;
}
uint8_t app_win_entered_full_window(void)
{
	if (MAX_WINDOW_NUM == app_win_get_focus_video_window())
		{
			return 1;
		}
	return 0;
} 

uint8_t app_win_is_full_window(void)
{
	if (FULL_SCREEN_WIN == app_win_get_focus_video_window())
		{
			return 1;
		}
	return 0;
} 

void app_win_update_video_focus_window(void)
{
	/*
	* 视频窗口发生变化，在新视频窗口显示提示信息
	*/
	char* focus_Window = (char*)GUI_GetFocusWindow();
	if (NULL == focus_Window)
		return;

	if( 0 == strcasecmp("win_menu_tv_list", focus_Window))
		{
			if (PROGRAM_TV_LIST_WIN != app_win_get_focus_video_window())
				{
					app_win_set_focus_video_window(PROGRAM_TV_LIST_WIN);
				}
		}

		if( 0 == strcasecmp("win_main_menu", focus_Window))
		{
			if (MAIN_MENU_WIN != app_win_get_focus_video_window())
				{
					app_win_set_focus_video_window(MAIN_MENU_WIN);
				}
		}

		if( 0 == strcasecmp("win_prog_manage", focus_Window))
		{
			if (PROGRAM_EDIT_WIN != app_win_get_focus_video_window())
				{
					app_win_set_focus_video_window(PROGRAM_EDIT_WIN);					
				}
		}
		
		if( 0 == strcasecmp("win_epg_list", focus_Window))
		{
			if (WEEKLY_EPG_WIN != app_win_get_focus_video_window())
				{
					app_win_set_focus_video_window(WEEKLY_EPG_WIN);						
				}
		}

		if( 0 == strcasecmp("win_favorite_prog_list", focus_Window))
		{
			if (FAVORITE_LIST_WIN != app_win_get_focus_video_window())
				{
					app_win_set_focus_video_window(FAVORITE_LIST_WIN);
				}
		}	
}

char* app_win_get_local_time_string(void)
{
	struct tm LocalTime;
	static char buf[50] = {0};

	app_epg_get_local_time(&LocalTime,TRUE);

/*	sprintf((void*)buf,
			"%04d-%02d-%02d %02d:%02d",
			LocalTime.tm_year,
			LocalTime.tm_mon,
			LocalTime.tm_mday,
			LocalTime.tm_hour,
			LocalTime.tm_min);
*/
	sprintf((void*)buf,
			"%02d:%02d",
			LocalTime.tm_hour,
			LocalTime.tm_min);

	return buf;

}

int app_win_update_current_time(uint32_t duration)
{
	static uint32_t count = 0;
	char widget_text_date[100] = {0};
	char* focus_Window = (char*)GUI_GetFocusWindow();
	if (NULL == focus_Window)
		return 0;


	if(( 0 == strcasecmp("win_book_manage", focus_Window))
		||( 0 == strcasecmp("win_sys_update", focus_Window))
		||( 0 == strcasecmp("win_passwd_set", focus_Window))
		||( 0 == strcasecmp("win_osd_set", focus_Window))
		||( 0 == strcasecmp("win_prog_info", focus_Window))
		||( 0 == strcasecmp("win_audio_set", focus_Window))
		||( 0 == strcasecmp("win_auto_search", focus_Window))
		||( 0 == strcasecmp("win_manual_search", focus_Window))
		||( 0 == strcasecmp("win_all_search", focus_Window))
		||( 0 == strcasecmp("win_stb_info", focus_Window))
		||( 0 == strcasecmp("win_main_frequecy_set", focus_Window))
		||( 0 == strcasecmp("win_sys_set", focus_Window))
		||( 0 == strcasecmp("win_epg_list", focus_Window))
		)
		{
			count++;
			if ((duration*count >= 1000)||(0 == duration))
				{
					count = 0;
					/*
					* 日期/时间控件每分钟刷新一次
					*/
					strcpy(widget_text_date,focus_Window);
					strcat(widget_text_date,"_time_text");
					GUI_SetProperty(widget_text_date, "string", app_win_get_local_date_string());					
				}
		}
	return 0;
}

//获取当前日期信息
char* app_win_get_local_date_string(void)
{
	struct tm LocalTime;
	static char buf[50]={0};
    
	app_epg_get_local_time(&LocalTime,TRUE);
	sprintf(buf,"%04d-%02d-%02d",LocalTime.tm_year,LocalTime.tm_mon,LocalTime.tm_mday);
    
    return buf;
}

/*
* 退出到指定界面后，响应对应的遥控器或创建指定的窗体
*/
void app_win_exist_to_win_widget(const char* widget_win_name)
{
	GUI_Event keyEvent = {0};
	int rtn = 1;
	if (TRUE == app_get_pop_msg_flag_status())
		return;

	if (NULL != widget_win_name)
	{
		char* focus_Window = (char*)GUI_GetFocusWindow();
		printf("%s %d app_win_exist_to_win_widget focus_Window=%s\n",__FILE__,__LINE__,focus_Window);
		if (NULL != focus_Window)
		{
			rtn = strcasecmp(widget_win_name, focus_Window);
			if (0 != rtn)
			{
				app_set_pop_msg_flag_status(TRUE);
			}
			else
			{
				app_set_pop_msg_flag_status(FALSE);
				return;
			}
			while(rtn != 0)
			{
				rtn = 1;
				focus_Window = (char*)GUI_GetFocusWindow();
				
				if (NULL != focus_Window)
				{
					printf("%s %d app_win_exist_to_win_widget widget_win_name=%s focus_Window=%s\n",__FILE__,__LINE__,widget_win_name,focus_Window);

					if ((0 == strcasecmp("win_full_screen" ,widget_win_name))&&
						((0 == strcasecmp("win_sys_set" ,focus_Window))||(0 == strcasecmp("win_media_centre" ,focus_Window)) || (0 == strcasecmp("win_main_menu" ,focus_Window))))
					{
						/*
						* 系统设置、网络多媒体退出时重新播放节目，直接关闭窗体
						* 避免退出到全屏前，播放上一节目
						*/
						GUI_EndDialog(focus_Window);
					}
					else if ((0 == strcasecmp("win_pic_view" ,focus_Window))
								||(0 == strcasecmp("win_music_view" ,focus_Window)) ||
								(0 == strcasecmp("win_epg_list", focus_Window)))
					{
						/*
						* 对于会引起死锁界面，直接GUI_EndDialog。
						*/
						GUI_EndDialog(focus_Window);												
					}
					else
					{
						rtn = strcasecmp(widget_win_name, focus_Window);
						if (0 != rtn)
						{
							keyEvent.type = GUI_KEYDOWN;
							keyEvent.key.sym = KEY_EXIT;	
							GUI_SendEvent(focus_Window,&keyEvent);
						}
					}
				}
//							GxCore_ThreadDelay(50);
				//GUI_LoopEvent();
			}
				app_set_pop_msg_flag_status(FALSE);
		}
	}

	printf("%s %d app_win_exist_to_win_widget end\n",__FILE__,__LINE__); 
	
	return ;
}


void app_win_exist_to_full_screen(void)
{
//	if (FALSE == app_get_win_create_flag(FULL_SCREEN_WIN))
//		return ;
	if (TRUE == app_get_pop_msg_flag_status())
		return;
	app_win_exist_to_win_widget("win_full_screen");
	
	app_set_win_destroy_flag(POP_MSG_WIN);
	app_set_win_destroy_flag(MANAGE_MENU_WIN);
	app_set_win_destroy_flag(MAIN_MENU_WIN);
	app_set_win_destroy_flag(PROGRAM_BAR_WIN);
	app_play_video_window_full();
	app_play_set_zoom_video_top();
	app_win_set_focus_video_window(FULL_SCREEN_WIN);

	return ;
}

int32_t app_win_check_password_valid(const char* widget_password_widget,int32_t passwordLen)
{
	char *value;
	uint32_t nPwCmpResult;

	char curPassword[MAX_PASSWD_LEN+1] = {0};
	char password[MAX_PASSWD_LEN+1] = {0};
	
	app_flash_get_config_password(password,passwordLen);
	
	if (NULL != widget_password_widget)
		GUI_GetProperty(widget_password_widget, "string", &value);
	
	memcpy(curPassword,(char*)value,passwordLen);
	if(strlen((char*)curPassword)!=passwordLen)
		{
			return 1; // 密码长度错误
		}
	
	nPwCmpResult = strcasecmp((char*)curPassword,(char*)password);
	if(nPwCmpResult != 0)
		{
			return 2; // 密码校验错误
		}
       
	return 0 ; // 密码校验成功
}

int app_win_check_fre_vaild(FRE_TYPE_E type,uint32_t fre)
{
	int retResult = POPMSG_RET_YES ;
	char buf[256];
	char* osd_language=NULL;
    
    if(FRE_MID == type)
    {
	    if (fre < FRE_BEGIN_LOW)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，频点应大于等于 %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not less than %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, buf, POPMSG_TYPE_OK);

			return FALSE;
		}

		if (fre > FRE_BEGIN_HIGH)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，频点应小于等于 %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not larger than %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);

			return FALSE;
		}
    }
    else if(FRE_BEGIN == type)
    {
	    if (fre < FRE_BEGIN_LOW)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，开始频点应大于等于 %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not less than %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);

			return FALSE;
		}

		if (fre > FRE_BEGIN_HIGH)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，开始频点应小于 %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not larger than %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);
			return FALSE;
		}
        
    }
    else if(FRE_END == type)
    {
	    if (fre < FRE_BEGIN_LOW)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，结束频点应大于 %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not less than %03d.%d", 
						FRE_BEGIN_LOW/1000,(FRE_BEGIN_LOW%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);
			return FALSE;
		}

		if (fre > FRE_BEGIN_HIGH)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，结束频点应小于等于 %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			else
			{
				sprintf((void*)buf,"Fre para error , should not larger than %03d.%d", 
						FRE_BEGIN_HIGH/1000,(FRE_BEGIN_HIGH%1000)/100);
			}
			retResult = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);
			return FALSE;
		}
        
    }
	return TRUE;

	
}

int app_win_check_sym_vaild(uint32_t sym)
{
	int retResult = POPMSG_RET_YES ;
	char buf[256];
	char* osd_language=NULL;

	if (sym < 2000)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"符号率参数错误，符号率不能小于 %03d", 
						2000);
			}
			else
			{
				sprintf((void*)buf,"Sym para error , sym should not less than %03d", 
						2000);
			}

			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);

			return FALSE;
		}

		if (sym > 7000)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"符号率参数错误，符号率不能大于 %03d", 
						7000);
			}
			else
			{
				sprintf((void*)buf,"Sym para error , sym should not larger \n than %03d", 
						7000);
			}

			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);

			return FALSE;
		}
	return TRUE;
}

int app_win_check_fre_range_vaild(uint32_t lowfre,uint32_t highfre)
{
	int retResult = POPMSG_RET_YES ;
	char buf[256];
	char* osd_language=NULL;

	if (FALSE == app_win_check_fre_vaild(FRE_BEGIN,lowfre))
		{
			return FALSE;
		}

	if (FALSE == app_win_check_fre_vaild(FRE_END,highfre))
		{
			return FALSE;
		}

	if (lowfre + 8000> highfre)
		{
			osd_language = app_flash_get_config_osd_language();
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				sprintf((void*)buf,"错误频点，结束频点应比开始频点大，且大于等于8M");
			}
			else
			{
				sprintf((void*)buf,"Fre para error , the end fre should larger than begin fre 8M or more");
			}
			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_OK);

			return FALSE;
		}
	return TRUE;
}

int app_win_auto_search(void)
{
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint32_t fre = 0;
	uint32_t fre1 = 0;
	symbol_rate = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	fre = app_flash_get_config_center_freq();
	fre1 = app_flash_get_config_manual_search_freq(); 

#ifdef DVB_CA_TYPE_DVB_FLAG
	GUI_CreateDialog("win_search_result");
	GUI_SetInterface("flush", NULL);
#ifdef CUST_TAOYUAN
	app_win_hunan_taoyuan_search();		
#endif
#ifdef CUST_LINLI
	app_win_hunan_linli_search();		
#endif
#ifdef CUST_TAIKANG
	app_win_hunan_taikang_search();		
#endif
#ifdef CUST_JINGANGSHAN
	app_win_hunan_taikang_search();		
#endif
#ifdef CUST_SHANGSHUI
	app_win_shangshui_search();		
#endif

#else
	GxFrontend_StopMonitor(0);
	if (0 == app_search_lock_tp(fre, symbol_rate,
				INVERSION_OFF, qam,1000))
	{
		GUI_CreateDialog("win_search_result");
		GUI_SetInterface("flush", NULL);
		app_search_scan_nit_mode();
	}
	else
	{
		if (0 == app_search_lock_tp(fre1, symbol_rate,
					INVERSION_OFF, qam,100))
		{
			GUI_CreateDialog("win_search_result");
			GUI_SetInterface("flush", NULL);
			app_search_scan_nit_mode();
		}
		else
		{	
			GxFrontend_StartMonitor(0);

			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Lock Failed",POPMSG_TYPE_OK);
		}
		return EVENT_TRANSFER_STOP;
	}
#endif
	return EVENT_TRANSFER_KEEPON;	
}

int app_win_reset_factory(void)
{
	popmsg_ret ret_pop = POPMSG_RET_YES;
	ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, "Do you want to reset the STB to factory state?", POPMSG_TYPE_YESNO);

	if(ret_pop == POPMSG_RET_YES)
	{
		GUI_SetProperty("win_sys_set_factory_text", "string", "Initalizing");
		GUI_SetProperty("win_sys_set_factory_text","state","show");
		GUI_SetProperty("win_advanced_set_menu_image_bg","state","show");
		GUI_SetProperty("win_advanced_set_menu_image_warning","state","show");
		GUI_SetProperty("win_sys_set_factory_text", "draw_now", NULL);
		GUI_SetProperty("win_advanced_set_menu_image_bg","draw_now",NULL);
		GUI_SetProperty("win_advanced_set_menu_image_warning","draw_now",NULL);

		app_prog_set_default();
		/*
		 * 恢复出厂设置后，恢复初始化差异设置
		 */
		GxBus_ConfigSetInt(FRONTEND_CONFIG_NUM,1);
			GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_XRES,VIDEO_WINDOW_W);
			GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_YRES,VIDEO_WINDOW_H);
			
			GUI_SetProperty("win_sys_set_factory_text", "string", "Init succeed");
	    }
		else
		{	
			return 1;
		}	
		return 0;	
}











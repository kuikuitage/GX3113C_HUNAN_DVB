/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_call_back.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2012.10.30		  zhouhm 	 			creation
*****************************************************************************/

#include "app.h"
#include "gxapp_sys_config.h"
#include "sys_common_config.h"
#if DVB_PVR_FLAG
#include "app_pvr.h"
#endif
#if defined(DVB_CA_TYPE_DS_CAS50_FLAG) || defined(DVB_CA_TYPE_WF_CA_FLAG)
//#include "app_desai_cas_5.0_api_demux.h"

uint8_t Show_time_short = 0;		//huangyp		add

#endif

#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif

#ifdef DVB_CA_TYPE_WF_CA_FLAG
extern void wf_cas_rolling_message_show(uint8_t byStyle,uint8_t showFlag,char* message);
extern void WFSTB_Set_roll_status(uint8_t flag);
#endif


extern void app_pvr_stop(void);
extern  uint8_t oldMsgpoptype ;
extern char* gs_popmsg_win;
#if DVB_MEDIA_FLAG
extern char* gc_MediaPopmsg_win;
#endif

extern 	int8_t win_epg_book_sync(void);
extern int8_t win_book_manage_sync(void);

int app_book_add_event_pop_msg_call_back(int ret, uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet ,GxBook* pbooktmp)
{
	int retResult = POPMSG_RET_YES ;
	switch(ret)
	{
		case BOOK_STATUS_OVERDUE:
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Time passed",POPMSG_TYPE_OK);

			break;
		case BOOK_STATUS_EXIST:
			break;
		case BOOK_STATUS_FULL:
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Book Full",POPMSG_TYPE_OK);

			break;
		case BOOK_STATUS_CONFILCT:
		{
			/*
			* 相同时间已存在其他预约,差异化提示
			*/
			char buf[256];
			char* osd_language=NULL;
			GxBusPmDataProg book_prog;
			book_play book;
			osd_language = app_flash_get_config_osd_language();
			memcpy(&book, pbooktmp->struct_buf, MAX_BOOK_STRUCT_LEN);
			GxBus_PmProgGetById(book.prog_id,&book_prog);

			if (0== strcmp(osd_language,LANGUAGE_CHINESE))
			{
				if(book_prog.service_type == GXBUS_PM_PROG_NVOD)
				{
					sprintf((void*)buf,"此时段已预约 %s 是否替换?",
							book.event_name);
				}
				else
				{
					sprintf((void*)buf,"此时段已预约 %s 是否替换?",
							book_prog.prog_name);
				}
			}
			else
			{
				if(book_prog.service_type == GXBUS_PM_PROG_NVOD)
				{
					sprintf((void*)buf,"This Time Booked %s Replace?",
							book.event_name);
				}
				else
				{
					sprintf((void*)buf,"This Time Booked %s Replace?",
							book_prog.prog_name);
				}
			}
			retResult = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,buf, POPMSG_TYPE_YESNO);

		}
		case BOOK_STATUS_OK:
		default:
			break;

	}
	if (POPMSG_RET_YES == retResult)
		return TRUE;
	else
		return FALSE;
}

//预约回调接口
int gi_BookPopMsg = 0;
int app_book_confirm_play_pop_msg_call_back(book_play book,GxBusPmDataProg   prog)
{
	int ret;
	popmsg_ret ret_pop = POPMSG_RET_NO; 
	char buf[100]={0};
	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	/*
	* 选择是否播放差异化提示
	*/

	/* modify:fy*/
	GUI_EndDialog("win_prog_bar");
	GUI_EndDialog("win_menu_volume");
	/* modify:fy:"无广播节目"框一直不消失 */
#if DVB_MEDIA_FLAG
	if (NULL != gc_MediaPopmsg_win)
	{
		GUI_EndDialog(gc_MediaPopmsg_win);
		gc_MediaPopmsg_win = NULL;
	}
#endif

	if (app_get_pop_msg_flag_status())
	{
		GUI_EndDialog(gs_popmsg_win);
		GUI_SetInterface("flush", NULL);
		
		app_set_pop_msg_flag_status(FALSE);
	}
	
	if(prog.service_type == GXBUS_PM_PROG_NVOD)
	{
		if (0== strcmp(osd_language,LANGUAGE_CHINESE))
		{
			sprintf((void*)buf,"是否播放:%s\n超时30秒自动跳转",book.event_name);
		}
		else
		{
			sprintf((void*)buf,"Want Play:[%s]",book.event_name);
		}
		
	}
	else
	{
		if (0== strcmp(osd_language,LANGUAGE_CHINESE))
		{
			sprintf((void*)buf,"是否播放:%s\n%s\n 超时30秒自动播放", 
            							prog.prog_name, book.event_name);
		}
		else
		{
			sprintf((void*)buf,"Want Play:%s\n%s\n timeout 30s,play auto", 
            							prog.prog_name, book.event_name);
		}
	}

	app_set_popmsg_yes_timeout(30);
	gi_BookPopMsg = 1;
	if (0 == GUI_CheckDialog("win_epg_list"))
	{
		win_epg_book_sync();
	}

	if (0 == GUI_CheckDialog("win_book_manage"))
	{
		win_book_manage_sync();
		GUI_SetProperty("win_book_manage", "update_all", NULL);
	}
	
	ret_pop = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START, (const char*)buf, POPMSG_TYPE_YESNO);
	ret = ret_pop;
	if (POPMSG_RET_YES == ret_pop)
	{	

 #if DVB_PVR_FLAG
        if(g_AppPvrOps.state != PVR_DUMMY)
    	{
    		printf("stop rec/tms \n");
            g_AppPvrOps.stoppvr_flag = 1;
            extern void app_pvr_stop(void) ;
            app_pvr_stop();
    	}
#endif    
       
		/*
		* 退出到全屏播放，根据不同方案实际需求实现
		*/
		app_play_stop();
		app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);
		app_play_switch_prog_clear_msg();
		app_win_exist_to_full_screen();
		/*
		* 退出到全屏播放，某些界面退出会重启播放
		*/
		app_play_stop();
		app_play_switch_prog_clear_msg();
		ret = TRUE;
	}
	else
	{
		char* focus_Window = (char*)GUI_GetFocusWindow();

		/*刷新预约界面*/
		if (NULL != focus_Window)
			{
				if( strcasecmp("win_book_manage", focus_Window) == 0)
				{
					win_book_manage_sync();
					GUI_SetProperty("win_book_manage", "update_all", NULL);//update
				}	

				if( strcasecmp("win_epg_list", focus_Window) == 0)
					{
						win_epg_book_sync();
						GUI_SetProperty("win_epg_schedule_listview", "update_all", NULL);//update
					}
			}
		ret = FALSE;
	}

	gi_BookPopMsg = 0;

	return ret;

}
//录制回调接口

 #if DVB_PVR_FLAG
int app_book_confirm_record_pop_msg_call_back(book_play book,GxBusPmDataProg   prog)
{
	int ret = FALSE;
	popmsg_ret ret_pop = POPMSG_RET_NO; 
	char buf[100]={0};
	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();

	HotplugPartitionList* partition_list = NULL;
	usb_check_state usb_state = USB_OK;
	
	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
	usb_state = g_AppPvrOps.usb_check(&g_AppPvrOps);
	if((usb_state == USB_ERROR)||(usb_state == USB_NO_SPACE)||(NULL == partition_list)||(0 == partition_list->partition_num))
	{
		if(usb_state == USB_ERROR)
		{
		    //notic usb error
			if (0== strcmp(osd_language,LANGUAGE_CHINESE))
			{
				app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"USB 错误!",POPMSG_TYPE_OK);
			}
			else
			{
				app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"USB Error!",POPMSG_TYPE_OK);
			}
		}

		if(usb_state == USB_NO_SPACE)
		{
		    //notice usb no space
		    if (0== strcmp(osd_language,LANGUAGE_CHINESE))
			{
				app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"硬盘空间已满!",POPMSG_TYPE_OK);
			}
			else
			{
				app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Space!",POPMSG_TYPE_OK);
			}
		}
		return ret;	
	}
	if(g_AppPvrOps.state != PVR_DUMMY)
	{
		 if (0== strcmp(osd_language,LANGUAGE_CHINESE))
		{
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"已有节目在录制!",POPMSG_TYPE_OK);
		}
		else
		{
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Schedule time,Device Busy!",POPMSG_TYPE_OK);
		}
		return ret;	
	}
	/*
	* 选择是否播放差异化提示
	*/
	if(prog.service_type == GXBUS_PM_PROG_NVOD)
	{
		if (0== strcmp(osd_language,LANGUAGE_CHINESE))
		{
			sprintf((void*)buf,"是否录制:%s\n超时30秒自动跳转",book.event_name);
		}
		else
		{
			sprintf((void*)buf,"Want Record:[%s]",book.event_name);
		}
		
	}
	else
	{
		if (0== strcmp(osd_language,LANGUAGE_CHINESE))
		{
			sprintf((void*)buf,"是否录制:%s\n%s\n 超时30秒自动录制", 
            							prog.prog_name, book.event_name);
		}
		else
		{
			sprintf((void*)buf,"Want Record:%s\n%s\n timeout 30s,do it auto", 
            							prog.prog_name, book.event_name);
		}
	}
	
	app_play_switch_prog_clear_msg();
	app_set_popmsg_yes_timeout(30);
	ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, (const char*)buf, POPMSG_TYPE_YESNO);
	ret = ret_pop;
	if (POPMSG_RET_YES == ret_pop)
		{	
			/*
			* 退出到全屏播放，根据不同方案实际需求实现
			*/
			app_play_stop();
			app_win_exist_to_full_screen();
			/*
			* 退出到全屏播放，某些界面退出会重启播放
			*/
			app_play_stop();
			app_play_switch_prog_clear_msg();
			ret = TRUE;
		}
	else
		{
			char* focus_Window = (char*)GUI_GetFocusWindow();

			/*刷新预约界面*/
			if (NULL != focus_Window)
				{
					if( strcasecmp("win_book_manage", focus_Window) == 0)
						{
							GUI_SetProperty("win_book_manage", "update_all", NULL);//update
						}	

					if( strcasecmp("win_epg_schedule", focus_Window) == 0)
						{
							win_epg_book_sync();
							GUI_SetProperty("win_epg_schedule_listview", "update_all", NULL);//update
						}
				}
			ret = FALSE;
		}

	return ret;

}
#endif
int app_search_add_extend_table_call_back(search_extend* searchExtendList )
{
	if (NULL == searchExtendList)
		return 0;
	memset(searchExtendList,0,sizeof(search_extend));



		/*
		* 扩展过滤sdt表
		*/
		
		if (0 != app_flash_get_config_sort_by_pat_flag())
		{
			if (searchExtendList->extendnum >=SEARCH_EXTEND_MAX)
				{
					printf(" extend num over SEARCH_EXTEND_MAX error\n");
					return 0;
				}

			searchExtendList->searchExtend[searchExtendList->extendnum].pid = PAT_PID;
			searchExtendList->searchExtend[searchExtendList->extendnum].match_depth = 1;
			searchExtendList->searchExtend[searchExtendList->extendnum].match[0] = PAT_TID;
			searchExtendList->searchExtend[searchExtendList->extendnum].mask[0] = 0xff;
			searchExtendList->searchExtend[searchExtendList->extendnum].table_parse_fun = app_table_pat_section_parse;	
			searchExtendList->extendnum ++;			
		}

		/*
		* 陕西项目扩展过滤BAT表
		*/

	
/*		if (searchExtendList->extendnum >=SEARCH_EXTEND_MAX)
			{
				printf(" extend num over SEARCH_EXTEND_MAX error\n");
				return 0;
			}


		searchExtendList->searchExtend[searchExtendList.extendnum].pid = BAT_PID;
		searchExtendList->searchExtend[searchExtendList.extendnum].match_depth = 5;
		searchExtendList->searchExtend[searchExtendList.extendnum].match[0]  = BAT_TID;
		searchExtendList->searchExtend[searchExtendList.extendnum].mask[0]  = 0xff;
		searchExtendList->searchExtend[searchExtendList.extendnum].match[3] = 0x61;
		searchExtendList->searchExtend[searchExtendList.extendnum].mask[3] = 0xff;
		searchExtendList->searchExtend[searchExtendList.extendnum].match[4] = 0x00;
		searchExtendList->searchExtend[searchExtendList.extendnum].mask[4] = 0xff;
		searchExtendList->searchExtend[searchExtendList.extendnum].table_parse_fun = bat_descriptor_parse;
		searchExtendList->extendnum ++;*/

		
		
		return 0;
}


void app_play_free_space(void)
{
#ifdef MENCENT_FREEE_SPACE
		if(FULL_SCREEN_WIN == app_win_get_focus_video_window())
		{
			GUI_SetInterface("free_space", "fragment|spp");
		   // GxCore_HwCleanCache();
		}
		else
		{
			GUI_SetInterface("free_space", "fragment");
		}
		GxCore_HwCleanCache();
#endif
	return ;
}
//在焦点窗体上显示消息提示框
void app_show_prompt(char* context)
{
	if (TRUE == app_get_pop_msg_flag_status())
	{
		if(FULL_SCREEN_WIN != app_win_get_focus_video_window()) 
		{
			oldMsgpoptype =MESSAGE_MAX_COUNT;
			return;
		}
	}
    //根据当前焦点位置(菜单界面)提示相应的消息框
	if (PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
	{
		GUI_SetProperty("win_menu_tv_list_text_msg", "string", context);
		GUI_SetProperty("win_menu_tv_list_text_msg","state","show");
		GUI_SetProperty("win_menu_tv_list_image574","state","show");
		GUI_SetProperty("win_menu_tv_list_image575","state","show");
	}
	else if(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
	{
		GUI_SetProperty("win_favorite_prog_list_text_msg", "string", context);
		GUI_SetProperty("win_favorite_prog_list_text_msg","state","show");					
	}
	else if(MAIN_MENU_WIN == app_win_get_focus_video_window()) 
	{
		GUI_SetProperty("win_main_menu_image572","state","show");					
		GUI_SetProperty("win_main_menu_image571","state","show");					
		GUI_SetProperty("win_main_menu_text_msg", "string", context);
		GUI_SetProperty("win_main_menu_text_msg","state","show");					

	}
	else if(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
	{
		GUI_SetProperty("win_epg_list_text_msg", "string", context);
		GUI_SetProperty("win_epg_list_text_msg","state","show");					
	}
	else if(PROGRAM_EDIT_WIN == app_win_get_focus_video_window()) 
	{
		if (strcasecmp(context, "No Tv program,please search!") == 0)
		{
			return;
		}
		GUI_SetProperty("win_prog_manage_text_msg", "string", context);
		GUI_SetProperty("win_prog_manage_text_msg","state","show");					
	}
	else if(FULL_SCREEN_WIN == app_win_get_focus_video_window()) 			
	{
		GUI_SetProperty("win_full_screen_text_msg", "string", context);
//		GUI_SetProperty("win_full_screen_image562","state","show");					
//		GUI_SetProperty("win_full_screen_image563","state","show");					
		GUI_SetProperty("win_full_screen_text_msg","state","show");					
	}
	else
	{
		/*
		 * 其他窗体不提示，设置类型为MESSAGE_POP_COUNT。返回视频窗口后提示对应消息
		 */
		oldMsgpoptype = MESSAGE_MAX_COUNT;
	}

	return;		
}

//隐藏消息提示框
void app_hide_prompt(void)
{
	char* hide_mode = "hide";
#ifdef DVB_CA_TYPE_WF_CA_FLAG
#ifdef DVB_CA_TYPE_WF
	wf_cas_rolling_message_show(0,3,NULL);
	WFSTB_Set_roll_status(0);
#endif	
#endif
	if (TRUE == app_get_pop_msg_flag_status())
	{
		if(FULL_SCREEN_WIN != app_win_get_focus_video_window()) 
			return;
		/**/
		hide_mode = "osd_trans_hide";
	}

	if(PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
	{
		GUI_SetProperty("win_menu_tv_list_text_msg","state",hide_mode);
		GUI_SetProperty("win_menu_tv_list_image574","state",hide_mode);
		GUI_SetProperty("win_menu_tv_list_image575","state",hide_mode);
	}
	else
		if(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
		{
			GUI_SetProperty("win_favorite_prog_list_text_msg","state",hide_mode);
		}
		else
			if(MAIN_MENU_WIN == app_win_get_focus_video_window()) 
			{
				GUI_SetProperty("win_main_menu_text_msg","state",hide_mode);
				GUI_SetProperty("win_main_menu_image572","state",hide_mode);					
				GUI_SetProperty("win_main_menu_image571","state",hide_mode);					

			}
			else
				if(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
				{
					GUI_SetProperty("win_epg_list_text_msg","state",hide_mode);
				}
		else
			if(FULL_SCREEN_WIN == app_win_get_focus_video_window()) 
				{
	
					GUI_SetProperty("win_full_screen_text_msg","state",hide_mode);
				}
		else
			if(PROGRAM_EDIT_WIN == app_win_get_focus_video_window()) 
				{
					GUI_SetProperty("win_prog_manage_text_msg","state",hide_mode);
				}
	return;			
}

uint32_t app_win_check_video_win(void)
{
    //根据当前焦点位置(菜单界面)提示相应的消息框
	if ((PROGRAM_TV_LIST_WIN == app_win_get_focus_video_window() ) 
	||(FAVORITE_LIST_WIN == app_win_get_focus_video_window()) 
	||(MAIN_MENU_WIN == app_win_get_focus_video_window()) 
	||(WEEKLY_EPG_WIN == app_win_get_focus_video_window()) 
	||(PROGRAM_EDIT_WIN == app_win_get_focus_video_window()) 
	||(FULL_SCREEN_WIN == app_win_get_focus_video_window())) 			
	{
		return 1;					
	}
	else
	{
		return 0;
	}

}
static int show_delay_ms = 0;
void app_win_set_prompt_msg_time(int time)
{
    show_delay_ms = time;
}
void app_win_prompt_msg_exec_call_back(void)
{
	uint8_t i = 0;
	static  uint8_t count0 = 0;
	uint32_t reflushFlag = 0;
	
	if (app_prog_get_num_in_group() > 0)
	{
		/*
		* 存在节目，清除无节目提示
		*/
		app_play_clear_msg_pop_type_flag(MSG_POP_NO_PROG);
	}
	else
	{
		/*
		* 提示无节目
		*/
		if(PROGRAM_EDIT_WIN != app_win_get_focus_video_window())
		{
			app_play_set_msg_pop_type_flag(MSG_POP_NO_PROG);
		}
	}

	if ( (app_flash_get_config_dvb_ca_flag() == DVB_CA_TYPE_DSCAS50)||(app_flash_get_config_dvb_ca_flag() == DVB_CA_TYPE_DVB)  )
	{
		for(i = MESSAGE_MAX_COUNT-1;i > 0;i--)
		//for(i = 0;i < MESSAGE_MAX_COUNT;i++)
		{
		/*
		 * 查找需要显示的提示类型
		 */
			if(TRUE == app_play_get_msg_pop_type_state(i))
			{
				break;
			}
		}
	}
	else
	{
		for(i = 0;i < MESSAGE_MAX_COUNT;i++)
		{
			/*
			 * 查找需要显示的提示类型
			 */
			if(TRUE == app_play_get_msg_pop_type_state(i))
			{
				break;
			}
		}
	}	
		

#ifdef CA_FLAG
	if (TRUE == app_cas_api_is_ca_pop_msg_type(i))/*显示对应类型的消息*/
	{
		/*
		 * 当前需要显示CA提示
		 */
		if(oldMsgpoptype != i)
		{
#ifdef _DEBUG
			printf("#show msgid:%d\n",i);
#endif
			oldMsgpoptype = i;
			app_cas_api_pop_msg_exec(i);//CA消息显示
		}

#if defined(DVB_CA_TYPE_DS_CAS50_FLAG) || defined(DVB_CA_TYPE_WF_CA_FLAG)
		//短时间显示的消息
		if(Show_time_short == 1)
			if(count0++ >= 10)
			{
				printf("\n==================Message Hide=====================\n");
				count0 = 0;
				Show_time_short = 0;
				//2014-11-26, qm
				//app_play_set_msg_pop_type_flag(MSG_POP_COMMON_END+0+1);
				app_play_clear_msg_pop_type(i);
			}
#endif	
		return ;
	}
#endif

	

	reflushFlag = app_play_get_prompt_reflush_flag();
	/*
	 * 以下为普通提示
	 */
	switch(i)
	{
		case MSG_POP_NONE:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				printf("app_hid_msg\n");
				oldMsgpoptype = i;
				app_hide_prompt();
			}
			break;
		case MSG_POP_INVALID_PROG:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("Invalid program id");
			}
			if(count0++ >= 5)
			{
				count0 = 0;
				app_play_clear_msg_pop_type_flag(MSG_POP_INVALID_PROG);
			}
			break;
		case MSG_POP_NO_FAV_PROG:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("No Favorite Program!");
			}
			break;
		case MSG_POP_NO_PROG:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("No Tv program,please search!");
			}
			break;
		case MSG_POP_SIGNAL_BAD:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("Bad signal");
			}
			break;
		case MSG_POP_PROG_LOCK:
			if (0 == app_play_get_signal_status())
			{
				return;
			}
			
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("program Locked");
			}
			break;
		case MSG_POP_PROG_SKIP:
			if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("Skip the program");
			}
			break;
		case MSG_POP_PROG_SCRAMBLE:
		if((oldMsgpoptype != i)||(1 == reflushFlag))
			{
				oldMsgpoptype = i;
				app_show_prompt("Scramble program");
			}
			break;		
#ifdef DVB_CA_TYPE_DIVITONE_FLAG	
		case MSG_POP_NO_CARD:
			if(oldMsgpoptype != i)
			{
				oldMsgpoptype = i;
				app_show_prompt("ERROR NO CARD");
			}
			break;		
		case MSG_POP_INVALID_CARD:
			if(oldMsgpoptype != i)
			{
				oldMsgpoptype = i;
				app_show_prompt("Sc Invalid Card");
			}
			break;
#endif
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		case MSG_POP_PROG_NEED_PAY:
			if(oldMsgpoptype != i)
			{
				oldMsgpoptype = i;
				app_show_prompt("Program need buy");
			}
			break;
#endif
		default:
			oldMsgpoptype =MESSAGE_MAX_COUNT;
			app_hide_prompt();

			break;
	}		

#ifdef DVB_CA_TYPE_DIVITONE_FLAG
	extern uint8_t prog_scramble_flag;
	extern uint8_t app_porting_ca_smart_status();
	if((1 == app_porting_ca_smart_status()) || (0 == prog_scramble_flag))
	{
		if(oldMsgpoptype == MSG_POP_NO_CARD)
			app_play_clear_msg_pop_type_flag(MSG_POP_NO_CARD);
		else if(oldMsgpoptype == MSG_POP_INVALID_CARD)
			app_play_clear_msg_pop_type_flag(MSG_POP_INVALID_CARD);
		if((oldMsgpoptype<MESSAGE_MAX_COUNT) &&(oldMsgpoptype>MSG_POP_COMMON_END))
		{
			app_play_clear_msg_pop_type_flag(oldMsgpoptype); 
		}
	}
	if((1 == prog_scramble_flag)&&(oldMsgpoptype != MSG_POP_NO_CARD)&&(0 == app_porting_ca_smart_status()))
	{
		if(app_porting_ca_smc_status() != 2)
		{
			if(oldMsgpoptype == MSG_POP_INVALID_CARD)
				app_play_clear_msg_pop_type_flag(MSG_POP_INVALID_CARD);
			app_play_set_msg_pop_type_flag(MSG_POP_NO_CARD); 	
		}
	}
	if((1 == prog_scramble_flag)&&(oldMsgpoptype != MSG_POP_INVALID_CARD))
	{
		if(app_porting_ca_smc_status() == 2)
		{
			if(oldMsgpoptype == MSG_POP_NO_CARD)
				app_play_clear_msg_pop_type_flag(MSG_POP_NO_CARD);
			app_play_set_msg_pop_type_flag(MSG_POP_INVALID_CARD); 		
		}
	}
#endif

}
#ifdef GX_NIT_OTA_FLAG
#define OTA_UPDATE_MANUAL			0x01
#define OTA_UPDATE_FORCE				0x02
#define OTA_UPDATE_SERIAL				0x03
#define OTA_UPDATE_SEQUENCE_NUMBER		0x04
/*
 * 永新OTA升级描述子，默认为gx3201方案通用OTA升级描述子
 */
#define OTA_TFCAS_DOWNLOAD_DESC		0xA1
uint8_t ota_manual_mode = 0;
static char* set_qam[]={"QAM_16","QAM_32","QAM_64","QAM_128","QAM_256"};
static uint8_t app_parse_cdcas_ota_descriptor(uint8_t* pData)
{
	uint8_t*		puSectionData = NULL;
	uint8_t		uDownloadType = 0;
	uint8_t           oemValue[20]={0};
	uint16_t		USTBManufacturerID = 0;
	uint16_t		m_PlatformID = 0;
	int32_t		iLength = 0;
	uint32_t		uHWVer=0;
	uint32_t		uSWVer=0;
	uint16_t         m_PID=0;
	uint32_t         m_Frequency=0;
	uint8_t         m_Modulation=0;
	uint32_t         m_SymbolRate=0;
	uint8_t         m_TableID=0;
	uint8_t         m_UpdateTimes=0;
	uint8_t         m_checkOtaFlag = FALSE;
	char serialNumber[20]={0};
	int m_manufacture_serial=0;
	int m_serial=0;
	char caUser[MAX_SEARIAL_LEN]={0};
	uint32_t		m_sequenceNumber = 0;
	uint32_t		m_sequenceStart = 0;
	uint32_t		m_sequenceEnd = 0;
	#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	GxFrontend frontendpara[FRONTEND_MAX];
	#endif
	char*          softwareVer = NULL;
	char*          hardwareVer = NULL;
	//uint32_t		m_len = 0;

	puSectionData = pData;

	iLength = puSectionData[1];

	USTBManufacturerID = (puSectionData[2]<<8)|puSectionData[3];

	m_PlatformID = convert_version_str(app_flash_get_oem_manufacture_id());

	if (USTBManufacturerID != m_PlatformID)
	{
		printf("USTBManufacturerID=0x%x m_PlatformID=0x%x\n",USTBManufacturerID,m_PlatformID);	
		return FALSE;
	}


	iLength = puSectionData[1];

	puSectionData += 4;

	while(iLength > 0)
	{	
		if( 0x44 == puSectionData[0] ||0x5A == puSectionData[0])
		{
			uint32_t  uNoOfDataBytes = 0;

			uNoOfDataBytes = ((puSectionData[2]	>> 4) &	0xF) * 10 + (puSectionData[2] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100	+ ((puSectionData[3] >> 4) & 0xF) * 10	+ (puSectionData[3] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100	+ ((puSectionData[4] >> 4) & 0xF) * 10	+ (puSectionData[4] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100	+ ((puSectionData[5] >> 4) & 0xF) * 10	+ (puSectionData[5] & 0xF);
			m_Frequency = uNoOfDataBytes ;
			m_Frequency = m_Frequency/10;

			m_Modulation = puSectionData[8];

			uNoOfDataBytes = ((puSectionData[9]	>> 4) &	0xF) * 10 + (puSectionData[9] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100	+ ((puSectionData[10] >> 4) & 0xF) * 10	+ (puSectionData[10] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100	+ ((puSectionData[11] >> 4) & 0xF) * 10	+ (puSectionData[11] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 10 + ((puSectionData[12] >>	4) & 0xF);
			m_SymbolRate = uNoOfDataBytes;
			m_SymbolRate = m_SymbolRate/10;

			iLength -= ( puSectionData[1] + 2 );
			puSectionData += ( puSectionData[1] + 2 );

			printf("[update]>>Freq=%d, Symbol=%d, QAM=%d \n",
					m_Frequency,
					m_SymbolRate,
					m_Modulation);
		}
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
		else if (0x5A == puSectionData[0])
		{
			m_Frequency = (uint32_t)((puSectionData[2]<<24 | puSectionData[3]<<16 | puSectionData[4]<<8 | puSectionData[5])*10/1000); // khz

			iLength -= ( puSectionData[1] + 2 );
			puSectionData += ( puSectionData[1] + 2 );
			printf("[update]>>Freq=%d\n",m_Frequency);
		}
#endif
		else			
		{
			/*不是有线传输系统描述,跳过*/
			iLength -= ( puSectionData[1] + 2 );
			puSectionData += ( puSectionData[1] + 2 );

			/*跳过私有数据*/
			iLength -= ( puSectionData[2] + 3 );
			puSectionData += ( puSectionData[2] + 3 );

			continue;
		}

		m_PID = (puSectionData[0]<<8)|puSectionData[1];
		m_PID >>= 3;

		uDownloadType = puSectionData[1]&0x07;

		m_TableID = puSectionData[3];
		m_UpdateTimes = puSectionData[4];

		printf("[update]>>PID = 0x%x \n", m_PID);
		printf("[update]>>DownType = %d \n", uDownloadType);
		printf("[update]>>TableID = 0x%x \n", m_TableID);
		printf("[update]>>Times = %d \n", m_UpdateTimes);

		/*HardWare version*/
		uHWVer = (puSectionData[5] << 24)|(puSectionData[6] << 16)
			|(puSectionData[7] << 8)|(puSectionData[8]);
		//uHWVer = (puSectionData[7] << 8)|(puSectionData[8]);

		hardwareVer = app_flash_get_oem_hardware_version_str();
		if (uHWVer  != convert_version_str(hardwareVer))
		{
			printf("uHWVer=0x%x != hardwareVer=0x%x\n",uHWVer,convert_version_str(hardwareVer));	
			iLength -= ( puSectionData[2] + 3 );
			puSectionData += ( puSectionData[2] + 3 );
			continue;
		}

		if (TRUE == ota_manual_mode)
		{
			/*菜单中强制OTA升级，不比对软件版本*/
			m_checkOtaFlag = TRUE;
		}


		/*SoftWare version*/
		uSWVer =  (puSectionData[9] << 24)|(puSectionData[10] << 16)
				|(puSectionData[11] << 8)|(puSectionData[12]);

		printf("[update]>>HW=V%x, SoftVer=V%x \n",uHWVer, uSWVer);
		softwareVer = app_flash_get_oem_softversion_str();
		if(uSWVer > convert_version_str(softwareVer)
				// TODO: For test in tongfang. COMMENT it for product
				/**/|| ((convert_version_str(softwareVer) == 3) && (1== uSWVer))
		  )
		{
			switch(uDownloadType)
			{
				case OTA_UPDATE_MANUAL:
					m_checkOtaFlag = TRUE;
					break;
				case OTA_UPDATE_FORCE:
					m_checkOtaFlag = TRUE;
					break;
				case OTA_UPDATE_SERIAL:
					app_flash_get_serial_number(0,(char *)caUser,MAX_SEARIAL_LEN);
					memset(serialNumber,0,20);
					memcpy(serialNumber,caUser+12,8);
					m_manufacture_serial = htoi(serialNumber);
					m_serial = ((puSectionData[13]&0x0F) <<16 ) |(puSectionData[14]<<8 )|(puSectionData[15]);
					printf(" 按批次升级批次m_manufacture_serial=%d  描述子m_serial=%d \n",
							m_manufacture_serial,m_serial);

					if (m_manufacture_serial == m_serial)
					{
						/*按批次升级*/
						m_checkOtaFlag = TRUE;
					}
					break;
				case OTA_UPDATE_SEQUENCE_NUMBER:
					memset(serialNumber,0,20);
					memcpy(serialNumber,caUser+6,5);
					m_manufacture_serial = htoi(serialNumber);
					m_serial = ((puSectionData[13]&0x0F) <<16 ) |(puSectionData[14]<<8 )|(puSectionData[15]);
					printf(" 按序列号序列号m_manufacture_serial=%d  描述子m_serial=%d \n",
							m_manufacture_serial,m_serial);

					if (m_manufacture_serial == m_serial)
					{
						/*检查批次是否匹配*/
						memset(serialNumber,0,20);
						memcpy(serialNumber,caUser,19);
						m_sequenceNumber = (serialNumber[11]-0x30)*10000;
						m_sequenceNumber +=(serialNumber[12]-0x30)*1000;
						m_sequenceNumber +=(serialNumber[13]-0x30)*100;
						m_sequenceNumber +=(serialNumber[14]-0x30)*10;
						m_sequenceNumber +=(serialNumber[15]-0x30);
						m_sequenceStart =   (puSectionData[16] & 0xF);
						m_sequenceStart = m_sequenceStart * 100	+ ((puSectionData[17] >> 4) & 0xF) * 10	+ (puSectionData[17] & 0xF);
						m_sequenceStart = m_sequenceStart * 100	+ ((puSectionData[18] >> 4) & 0xF) * 10	+ (puSectionData[18] & 0xF);
						m_sequenceEnd =   (puSectionData[19] & 0xF);
						m_sequenceEnd = m_sequenceEnd * 100	+ ((puSectionData[20] >> 4) & 0xF) * 10	+ (puSectionData[20] & 0xF);
						m_sequenceEnd = m_sequenceEnd * 100	+ ((puSectionData[21] >> 4) & 0xF) * 10	+ (puSectionData[21] & 0xF);

						printf(" 按序列号升级m_sequenceNumber=%d  m_sequenceStart=%d m_sequenceEnd=%d\n",
								m_sequenceNumber,m_sequenceStart,m_sequenceEnd);
						if ((m_sequenceNumber >= m_sequenceStart)&&(m_sequenceNumber <= m_sequenceEnd))
						{
							/*按序列号升级*/
							m_checkOtaFlag = TRUE;
						}
					}
					break;
				default:
					break;
			}
		}

		if (TRUE == m_checkOtaFlag)
		{
			/*升级类型1=手动下载2=强制升级*/
			/*发送消息至APP，需要进行更新提示*/
			printf("[update]>>需要升级，界面提示\n");
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "%02x.%02x.%02x.%02x",
					puSectionData[9],puSectionData[10],puSectionData[11],puSectionData[12]);
			app_flash_set_oem_soft_updateversion((char*)oemValue);

			memset(oemValue,0,20);
			sprintf((char*)oemValue, "%d",m_PID);
			app_flash_set_oem_dmx_pid((char*)oemValue);
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "%d",m_TableID);
			app_flash_set_oem_dmx_tableid((char*)oemValue);
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "%d",m_UpdateTimes);
			app_flash_set_oem_repeat_times((char*)oemValue);
			app_flash_set_oem_fe_modulation((char*)set_qam[m_Modulation-1]);
			memset(oemValue,0,20);
			/*
			   升级私有表和升级流在同一个频点中下发，检测到升级描述子记录
			   当前频点信息，OTA锁当前频点过滤升级流*/

			//GxFrontend_GetCurFre(0, &frontendpara[0]);
			//m_Frequency = frontendpara[0].fre;
			m_Frequency = m_Frequency*1000;
			sprintf((char*)oemValue, "%d",m_Frequency);
			app_flash_set_oem_dmx_frequency((char*)oemValue);
			memset(oemValue,0,20);
			m_SymbolRate = m_SymbolRate*1000;
			sprintf((char*)oemValue, "%d",m_SymbolRate);
			app_flash_set_oem_fe_symbolrate((char*)oemValue);
			memset(oemValue,0,20);
			//				GxFrontend_GetCurFre(0, &frontendpara[0]);
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			if(frontendpara[0].type_1501 == GXBUS_PM_SAT_1501_DTMB)  //DTMB
			{
				sprintf((char*)oemValue,"%s","DTMB");
			}
			else
			{
				sprintf((char*)oemValue,"%s","DVB_C");
			}
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			sprintf((char*)oemValue,"%s","DVB_C");
#endif
			app_flash_set_oem_fe_workmode((char*)oemValue);

				if (0 != strcasecmp("yes",app_flash_get_oem_ota_flag()))
				{
					/*
					* 无升级标志。检测升级描述子存在，立即触发升级。
					*/
					app_flash_set_oem_ota_update_type("ts");
					app_flash_set_oem_ota_flag("yes");
					app_flash_save_oem();

					/*if ((uDownloadType == OTA_UPDATE_FORCE)||(TRUE == ota_manual_mode))
					{
						GxCore_ThreadDelay(1000);
	                   			 // 重启
						restart();
					}
					else*/
					{
						/*
						 * 强制升级外的其他升级模式，都需要弹出消息框
						 * 用户可选择立即升级或下次重启升级
						 */
						 app_cas_api_on_event(DVB_CA_OTA,CAS_NAME, NULL, 0);
					}
					
				}
				else
					{
						/*
						* 如果升级标志已保存(之前选择取消)，不触发
						*/
				//app_flash_save_oem();
			}
			return TRUE;

		}


		iLength -= ( puSectionData[2] + 3 );
		puSectionData += ( puSectionData[2] + 3 );
	}
	return FALSE;

}

#endif

uint8_t app_table_nit_parse_descriptor_call_back(uint8_t tag,uint8_t* pData)
{
#ifdef DVB_AD_TYPE_DS_FLAG	
	uint16_t adPid = 0;
#endif
	switch(tag)
	{
#ifdef GX_NIT_OTA_FLAG
		case OTA_TFCAS_DOWNLOAD_DESC:/*同方升级方式*/
			app_parse_cdcas_ota_descriptor(pData);
			return 1;
#endif
		case 0xA6:
#ifdef DVB_AD_TYPE_DS_FLAG	
			adPid = pData[2];
			adPid = adPid<<8;
			adPid |= pData[3];

			printf("[NIT]get dsads PID=0x%x\n",adPid);
			DSAD_SetDataPid(adPid);
#endif
			return 1;
		default:
			return 1;
	}

}

void app_notice_nit_change_for_search_show(void)
{
	popmsg_ret ret;

	if(1 == app_get_nit_change_for_search_status())
	{
		if(TRUE == app_get_pop_msg_flag_status())
		{
			return;
		}
		app_set_nit_change_for_search_status(2);
		app_win_exist_to_full_screen();
		ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Nit change, Research?",POPMSG_TYPE_YESNO);
		if(ret == POPMSG_RET_YES)
		{	
#if DVB_PVR_FLAG
			if(g_AppPvrOps.state != PVR_DUMMY)
			{
				GUI_SetProperty("full_screen_tms_stop_image", "state", "hide");
				app_pvr_stop();
			}
#endif
			app_win_exist_to_full_screen();
			app_prog_delete_all_prog();
			app_search_set_auto_flag(TRUE);
		}
		else
		{
		}
	}
}




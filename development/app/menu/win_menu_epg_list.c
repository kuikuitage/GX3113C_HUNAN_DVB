/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_menu_epg.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.05		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"
#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
#endif

static event_list* spApp_Epgtime = NULL;
static int sApp_list_prog_Sel;
static int sApp_list_schedule_Sel=-1;
static int sApp_listAct = -1;
static GxEpgInfo *epg_info;
static GxBookGet EpgBookGet;
int epg_flush_time = 500;
static uint32_t epg_count = 0;
const char* WIDGET_DAY[7] = {
														"win_epg_list_text_day1",
														"win_epg_list_text_day2",
														"win_epg_list_text_day3",
														"win_epg_list_text_day4",
														"win_epg_list_text_day5",
														"win_epg_list_text_day6",
														"win_epg_list_text_day7"
									};

int8_t win_epg_book_sync(void)
{
	app_book_init();
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
	return 0;
}

static void win_menu_epg_list_set_day_widget_string(void)
{
	uint32_t i;
	struct tm LocalTime;
	app_epg_get_local_time(&LocalTime,TRUE);


	for(i = 0; i < 7; i++)
	{	
		switch((LocalTime.tm_wday + i)%7)
			{
				case 0:
					GUI_SetProperty(WIDGET_DAY[i],"string","SUN");
					break;
				case 1:
					GUI_SetProperty(WIDGET_DAY[i],"string","MON");
					break;
				case 2:
					GUI_SetProperty(WIDGET_DAY[i],"string","TUE");
					break;
				case 3:
					GUI_SetProperty(WIDGET_DAY[i],"string","WED");
					break;
				case 4:
					GUI_SetProperty(WIDGET_DAY[i],"string","THU");
					break;
				case 5:
					GUI_SetProperty(WIDGET_DAY[i],"string","FRI");
					break;
				case 6:
					GUI_SetProperty(WIDGET_DAY[i],"string","SAT");
					break;
				default:
					break;
	
			}

	}
}

static void win_menu_epg_list_update_day_widget_focus(void)
{
	uint32_t i;
	epg_get_para* epg_para= NULL;
	epg_para = app_epg_get_para();

	for(i = 0; i < 7; i++)
	{
		if(i == epg_para->sEpg_day)
		{
			#ifdef APP_SD
			GUI_SetProperty(WIDGET_DAY[i],"backcolor", "[#40FF1E,#40FF1E,#40FF1E]");
			GUI_SetProperty(WIDGET_DAY[i],"forecolor", "[#212021,#212021,#212021]");
			#endif
			#ifdef APP_HD
			GUI_SetProperty(WIDGET_DAY[i],"backcolor", "[#408DD2,#408DD2,#408DD2]");
			GUI_SetProperty(WIDGET_DAY[i],"forecolor", "[#FFFFFF,#FFFFFF,#FFFFFF]");

			#endif

		}
		else
		{
			#ifdef APP_SD
			GUI_SetProperty(WIDGET_DAY[i],"backcolor", "[#33508C,#33508c,#33508C]");
			GUI_SetProperty(WIDGET_DAY[i],"forecolor", "[#CEE8FF,#CEE8FF,#CEE8FF]");
			#endif
			#ifdef APP_HD
			GUI_SetProperty(WIDGET_DAY[i],"backcolor", "[#408DD2,#408DD2,#408DD2]");
			GUI_SetProperty(WIDGET_DAY[i],"forecolor", "[#3A4E4F,#3A4EFF,#3A4E4F]");
			#endif
		}
	}
	return;
	
}
static void update_event(uint32_t value)
{
	int32_t nListSel = -1;
	GUI_GetProperty("win_epg_schedule_listview","select",(void*)&nListSel);
	epg_info = app_epg_update_event_by_day(value,nListSel,TRUE);	
	win_menu_epg_list_set_day_widget_string();
	win_menu_epg_list_update_day_widget_focus();
}


static  int epg_timer(void *userdata)
{
	uint32_t value;
	int32_t nListSel=-1;

	/*如果焦点不在当前界面，不响应TIMER，避免界面错乱*/
	char* focus_Window = (char*)GUI_GetFocusWindow();

	
	if((NULL != focus_Window)&&( strcasecmp("win_epg_list", focus_Window) != 0))
		{
			return 0;
		}

	if (0 == app_prog_get_num_in_group())
		{
			/*
			* 节目个数为0，退出
			*/
			return 0;
		}
	
	GUI_GetProperty("win_epg_schedule_listview","select",(void*)&nListSel);
	GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);
	update_event(value);
	epg_count = app_epg_get_event_count();
	//GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);
	if(strcasecmp("win_epg_schedule_listview" ,GUI_GetFocusWidget())==0)
	{
		if ((nListSel >= epg_count)||(-1 == nListSel))
			{
				nListSel = 0;
			}
		GUI_SetProperty("win_epg_schedule_listview","select",(void*)&nListSel);

		if ( 0 == epg_count)
			GUI_SetProperty("win_epg_schedule_listview","active",&nListSel);

	}
	else
	{
		if ((nListSel >= epg_count)||(-1 == nListSel))
			{
				nListSel = 0;
			}
		GUI_SetProperty("win_epg_schedule_listview","active",&nListSel);
	}

	if (epg_count >0 )
	{
		/*刷新epg列表后，递增重置epg timer,避免过于频繁刷新epg列表出现的闪烁*/
		if (epg_flush_time < 1500)
			{
				remove_timer(spApp_Epgtime);
				epg_flush_time += 500;
				spApp_Epgtime = create_timer(epg_timer, epg_flush_time, NULL,  TIMER_REPEAT);						
			}		
	}

	return 0;
}


SIGNAL_HANDLER int win_epg_list_create(const char* widgetname, void *usrdata)
{
	uint32_t pos = 0;

	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {

#ifdef APP_SD
//    	    GUI_SetProperty("win_epg_list_title", "img", "DS_TITLE_EPG.bmp");
#endif
    	    
#ifdef APP_HD
        GUI_SetProperty("win_epg_list_title", "img", "title_epg.bmp");
    	GUI_SetProperty("win_epg_list_image_exit", "img", "tips_exit.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
     { 
#ifdef APP_SD
   // 	    GUI_SetProperty("win_epg_list_title", "img", "DS_TITLE_EPG_ENG.bmp");
#endif
 
#ifdef APP_HD
    	    GUI_SetProperty("win_epg_list_title", "img", "title_epg_e.bmp");
    	GUI_SetProperty("win_epg_list_image_exit", "img", "tips_exit_e.bmp");
#endif
    }
    GUI_SetProperty("win_epg_list_time_text", "string", app_win_get_local_date_string());



	/*
	* EPG界面通用
	*/

	app_epg_reset_get_para();
	epg_count = 0;

	epg_flush_time = 500;
	if(!spApp_Epgtime)
	{
		spApp_Epgtime = create_timer(epg_timer, epg_flush_time, NULL,  TIMER_REPEAT);
	}
	else
	{	
		reset_timer(spApp_Epgtime);
	}
	app_prog_get_playing_pos_in_group(&pos);
	app_book_init();
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);

	GUI_SetProperty("win_epg_prog_list_listview","select",(void*)&pos);
	update_event(pos);
	epg_count = app_epg_get_event_count();
	sApp_list_schedule_Sel = 0;	
	GUI_SetProperty("win_epg_schedule_listview","select",&sApp_list_schedule_Sel);
	GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);	

	GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);

#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_menu_pic_display("win_epg_list_ad_image");
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_show_pic("win_epg_list_ad_image",M_AD_PLACE_ID_EPG);
#endif
	win_menu_epg_list_update_day_widget_focus();
	win_menu_epg_list_set_day_widget_string();

	app_win_set_focus_video_window(WEEKLY_EPG_WIN);	

	if (FALSE == app_play_get_play_status())
	{
		app_play_switch_prog_clear_msg();	
		app_play_reset_play_timer(0);
	}
	return 0;


}

SIGNAL_HANDLER  int win_epg_list_lost_focus(const char* widgetname, void *usrdata)
{
	GUI_GetProperty("win_epg_schedule_listview","select",(void*)&sApp_list_schedule_Sel);
	GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER  int win_epg_list_destroy(const char* widgetname, void *usrdata)
{
	app_epg_free();
	app_epg_reset_get_para();
	remove_timer(spApp_Epgtime);
	spApp_Epgtime = NULL;
	epg_count = 0;
	epg_flush_time = 500;

#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_hide_menu_pic();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_epg_list_ad_image");
#endif
	return 0;
}

SIGNAL_HANDLER int win_epg_list_show(const char* widgetname, void *usrdata)
{
//GUI_SetInterface("flush", NULL);
	#ifdef APP_SD
	//GUI_SetInterface("video_top", NULL);
	#endif
	return EVENT_TRANSFER_KEEPON;

}

SIGNAL_HANDLER int win_epg_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t value = 0;

	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_LEFT:
		case KEY_RIGHT:
			if(strcasecmp("win_epg_schedule_listview" ,GUI_GetFocusWidget())==0)
			{
				GUI_GetProperty("win_epg_schedule_listview","select",(void*)&sApp_list_schedule_Sel);
				GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_listAct);
				GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);
				GUI_SetFocusWidget("win_epg_prog_list_listview");
				return EVENT_TRANSFER_STOP;
			}
			
			if((strcasecmp("win_epg_prog_list_listview" ,GUI_GetFocusWidget())==0) && (epg_count != 0))
			{
				GUI_GetProperty("win_epg_schedule_listview","select",(void*)&sApp_list_schedule_Sel);
				if(sApp_list_schedule_Sel == -1)
				{
					sApp_list_schedule_Sel = 0;
					GUI_SetProperty("win_epg_schedule_listview","select",(void*)&sApp_list_schedule_Sel);
				}
				GUI_SetProperty("win_epg_schedule_listview","active",&sApp_listAct); // 激活无效
				GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&sApp_list_prog_Sel);
				GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_list_prog_Sel);
				GUI_SetFocusWidget("win_epg_schedule_listview");
				return EVENT_TRANSFER_STOP;
			}
			return EVENT_TRANSFER_STOP;		
			case KEY_RED:
				{
					uint32_t value;
					uint32_t dayoffset = 0;
					dayoffset = app_epg_switch_day_offset();
					win_menu_epg_list_update_day_widget_focus();
					GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);
					update_event(value);

					if(strcasecmp("win_epg_schedule_listview" ,GUI_GetFocusWidget())==0)
						{
							/*
							* 当前焦点处于EPG列表
							*/
							epg_count = 0;
							sApp_list_schedule_Sel = 0;
							GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);
							GUI_SetInterface("flush", NULL);

							GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_listAct);
							GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);
							GUI_SetFocusWidget("win_epg_prog_list_listview");

							epg_count = app_epg_get_event_count();
							GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);


							GUI_SetProperty("win_epg_schedule_listview","active",&sApp_listAct); // 激活无效
							GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&sApp_list_prog_Sel);
							GUI_SetProperty("win_epg_prog_list_listview","active",&sApp_list_prog_Sel);
							GUI_SetProperty("win_epg_schedule_listview","select",&sApp_list_schedule_Sel);
							GUI_SetFocusWidget("win_epg_schedule_listview");
							GUI_SetInterface("flush", NULL);
							
						}
					else
						{
							/*
							* 当前焦点处于节目列表
							*/
							epg_count = app_epg_get_event_count();
							sApp_list_schedule_Sel = 0;
							GUI_SetProperty("win_epg_schedule_listview","select",&sApp_list_schedule_Sel);
							GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);		
							GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);															
						}
				}
				return EVENT_TRANSFER_STOP;
			case KEY_BLUE:
				return EVENT_TRANSFER_STOP;
			case KEY_YELLOW:
					if(strcasecmp("win_epg_schedule_listview" ,GUI_GetFocusWidget())==0)
					{					
						if(epg_count && epg_info)
						{
							int32_t ret;
							GxBusPmDataProg prog;
							event_book_para bookpara = {0};
							GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);
							GxBus_PmProgGetByPos(value,1,&prog);

							bookpara.book_type=BOOK_PROGRAM_PLAY;
							bookpara.prog_id = prog.id;
							bookpara.pEpgBookGet = &EpgBookGet;
							bookpara.event_id = epg_info->event_id;
							bookpara.start_time = epg_info->start_time;
							bookpara.end_time = epg_info->start_time+epg_info->duration;
							bookpara.event_name = epg_info->event_name;
//							bookpara.epg_info = epg_info;
							ret = app_book_add_event(bookpara); 
							switch(ret)
							{
								case BOOK_STATUS_OVERDUE:
									GUI_GetProperty("win_epg_schedule_listview","select",(void*)&value);
									GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);
									GUI_SetProperty("win_epg_schedule_listview","select",(void*)&value);
									break;
                                case BOOK_STATUS_CONFILCT:
								case BOOK_STATUS_EXIST:
								case BOOK_STATUS_OK:
									GUI_GetProperty("win_epg_schedule_listview","select",(void*)&value);
									GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);
                                    GUI_SetProperty("win_epg_schedule_listview","select",(void*)&value);	
								default:
									break;
									
							}
						}
					}
					return EVENT_TRANSFER_STOP;

				return EVENT_TRANSFER_STOP;
	case KEY_GREEN:
		if(strcasecmp("win_epg_schedule_listview" ,GUI_GetFocusWidget())==0)
			{					
				//todo for book record
				if(epg_count && epg_info)
				{
					int32_t ret;
					GxBusPmDataProg prog;
					event_book_para bookpara = {0};
					GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);
					GxBus_PmProgGetByPos(value,1,&prog);

					bookpara.book_type=BOOK_TYPE_1;
					bookpara.prog_id = prog.id;
					bookpara.pEpgBookGet = &EpgBookGet;
					bookpara.event_id = epg_info->event_id;
					bookpara.start_time = epg_info->start_time;
					bookpara.end_time = epg_info->start_time+epg_info->duration;
					bookpara.event_name = epg_info->event_name;
//							bookpara.epg_info = epg_info;
					ret = app_book_add_event(bookpara); 
					switch(ret)
						{
							case BOOK_STATUS_OVERDUE:
								GUI_GetProperty("win_epg_schedule_listview","select",(void*)&value);
								GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);
								GUI_SetProperty("win_epg_schedule_listview","select",(void*)&value);
								break;
							case BOOK_STATUS_EXIST:
							case BOOK_STATUS_OK:
								GUI_GetProperty("win_epg_schedule_listview","select",(void*)&value);
								GUI_SetProperty("win_epg_schedule_listview","update_row",(void*)&value);		
							default:
								break;
									
						}
				}
			}
			
			return EVENT_TRANSFER_STOP;				
		case KEY_OK:
		case KEY_EXIT:
			app_play_stop();
			app_play_video_window_full();
			app_play_set_zoom_para(0,0,VIDEO_WINDOW_W,VIDEO_WINDOW_H);
			GUI_EndDialog("win_epg_list");
			GUI_EndDialog("win_main_menu");
			GUI_SetInterface("flush", NULL);
			app_play_reset_play_timer(0);

			app_win_set_focus_video_window(FULL_SCREEN_WIN);
			return EVENT_TRANSFER_STOP;	
		case KEY_RECALL:
		case KEY_MENU:
			if (TRUE == app_get_win_create_flag(MAIN_MENU_WIN))
				{
					
					app_play_stop();
//					app_play_check_play_timer();
					#ifdef APP_SD
					app_play_set_zoom_para(192, 152, 348, 240);
					#endif
					#ifdef APP_HD
					app_play_set_zoom_para(400, 160, 500, 290);
					#endif
					GUI_EndDialog("win_epg_list");
					GUI_SetInterface("flush", NULL);
					app_play_reset_play_timer(0);


					app_win_set_focus_video_window(MAIN_MENU_WIN);					
				}
			else
				{
					/*
					* 快捷方式进入，直接退出到全屏
					*/
			        app_play_stop();
					app_play_video_window_full();
					app_play_set_zoom_para(0,0,VIDEO_WINDOW_W,VIDEO_WINDOW_H);					
					GUI_EndDialog("win_epg_list");
					GUI_EndDialog("win_main_menu");
					GUI_SetInterface("flush", NULL);
					app_play_reset_play_timer(0);
					app_win_set_focus_video_window(FULL_SCREEN_WIN);				
				}
			return EVENT_TRANSFER_STOP;	
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_epg_prog_list_get_total(const char* widgetname, void *usrdata)
{
	return app_prog_get_num_in_group();
}
SIGNAL_HANDLER  int app_epg_prog_list_get_data(const char* widgetname, void *usrdata)
{
	static GxBusPmDataProg prog;
	static uint8_t pArry[5] = {0};
	ListItemPara *item_para =NULL;
	Lcn_State_t lcn_flag=0;
	
	if(NULL == widgetname || NULL == usrdata)	
	{		
		return (1);	
	}	
	
	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para) return GXCORE_ERROR;
	
	lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_OFF == lcn_flag)
	{
		sprintf((void*)pArry, "%03d",item_para->sel+1);
	}
	else
	{
		GxBus_PmProgGetByPos(item_para->sel,1,&prog);
		sprintf((void*)pArry, "%03d",prog.pos);
	}
	item_para->string = (char*)pArry;
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;



	GxBus_PmProgGetByPos(item_para->sel,1,&prog);
	item_para->string = (char*)prog.prog_name;

#ifdef APP_SD
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;
	if(TRUE == prog.scramble_flag)
	{
		item_para->image = "DS_Channel_list_Money.bmp";
	}
	else
	{
		item_para->image = NULL;
	}
#endif

	
	return 0;
}

SIGNAL_HANDLER  int app_epg_prog_list_change(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;
	uint32_t pos = 0;

	GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);

	/*播放节目*/
	app_prog_get_playing_pos_in_group(&pos);
	app_prog_save_playing_pos_in_group(value);
	if (pos != value)
	{
		app_play_switch_prog_clear_msg();	
		app_play_reset_play_timer(PLAY_TIMER_DURATION);
	}


	/*切台直接清除EPG列表信息，避免快速、连按卡顿*/
	epg_count = 0;
	sApp_list_schedule_Sel = 0;
	GUI_SetProperty("win_epg_schedule_listview","select",&sApp_list_schedule_Sel);
	GUI_SetProperty("win_epg_schedule_listview","active",&sApp_list_schedule_Sel);		
	GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);	
	
	/*切台重置500ms 刷新EPG timer*/
	if(!spApp_Epgtime)
	{
		epg_flush_time = 500;
		spApp_Epgtime = create_timer(epg_timer, epg_flush_time, NULL,  TIMER_REPEAT);
	}
	else
	{
		if (500 == epg_flush_time)
			reset_timer(spApp_Epgtime);
		else
			{
				remove_timer(spApp_Epgtime);
				epg_flush_time = 500;
				spApp_Epgtime = create_timer(epg_timer, epg_flush_time, NULL,  TIMER_REPEAT);				
			}
		
	}

	return 0;
}


SIGNAL_HANDLER  int app_epg_schedule_listview_change(const char* widgetname, void *usrdata)
{

	/*如不需重新刷新，直接读取当前选中行的EVENT信息
	   避免快速、连按频繁刷新卡顿、闪烁*/


	int32_t value;

	GUI_GetProperty("win_epg_schedule_listview","select",(void*)&value);
	if (-1 != value)
		epg_info = app_epg_get_event_info_by_sel(value);
	else
		epg_info = NULL;


    GUI_SetProperty("win_epg_schedule_listview","update_all",NULL);															

    
	return 0;
}


SIGNAL_HANDLER  int app_epg_schedule_listview_get_total(const char* widgetname, void *usrdata)
{
	int select = 0;

	GUI_GetProperty(widgetname, "select", &select);
	if((select < 0) && (epg_count > 0))
	{
		select = 0;
		GUI_SetProperty(widgetname, "select", &select);
	}
	return epg_count;
}

SIGNAL_HANDLER  int app_epg_schedule_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara *item_para =NULL;	
	struct tm tm_start;
	struct tm tm_end;
	uint8_t pArry_time[40] = {0};
    uint8_t Event_Name[80] = {0};
	static uint8_t pArry[120] = {0};

	uint32_t value;
	static GxBusPmDataProg prog;	
	GxEpgInfo *epg_info_temp = NULL;
	time_t time_start;
	int32_t TimeZone;
	
	if(NULL == widgetname || NULL == usrdata)	
	{		
		return (1);	
	}	
	
	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para) return GXCORE_ERROR;

	epg_info_temp = app_epg_get_event_info_by_sel(item_para->sel);
	if (NULL == epg_info_temp)
	{		
		return (1);	
	}

	TimeZone = app_flash_get_config_timezone();
	time_start = epg_info_temp->start_time+TimeZone*3600;
	
	app_epg_get_event_time(epg_info_temp,&tm_start,&tm_end);
	memset(pArry_time,0,sizeof(pArry_time));
	sprintf((void*)pArry_time,"%02d:%02d-%02d:%02d",tm_start.tm_hour,tm_start.tm_min,tm_end.tm_hour,tm_end.tm_min);	
	memcpy((void*)Event_Name,epg_info_temp->event_name,66);

    memset(pArry,0,sizeof(pArry));
	sprintf((char *)pArry,"%s %s",pArry_time,Event_Name);
	//printf("%s \n",pArry);
	item_para->x_offset = 5;
	item_para->string = (char*)pArry;	
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;
	
	GUI_GetProperty("win_epg_prog_list_listview","select",(void*)&value);

    /*是否预约*/
	GxBus_PmProgGetByPos(value,1,&prog);
	if(app_book_channel_check_exist(BOOK_PROGRAM_PLAY,prog.id, time_start-TimeZone*3600, &EpgBookGet))
	{
		item_para->image = "DS_ICON_TIME.bmp";
	}
	else
	{
		item_para->image =NULL;
	}
	
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;

    
	if(app_book_channel_check_exist(BOOK_TYPE_1,prog.id, time_start-TimeZone*3600, &EpgBookGet))
	{
		item_para->image = "DS_ICON_GREEN.bmp";
	}
	else
	{
		item_para->image =NULL;
	}	
	return 0;

}



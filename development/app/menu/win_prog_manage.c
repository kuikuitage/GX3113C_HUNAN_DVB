/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_prog_manage.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.15		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"
static GxBusPmViewInfo sysinfo_old;

static uint8_t manage_stream_type = GXBUS_PM_PROG_TV;
void app_set_prog_manage_stream_type(uint8_t stream_type)
{
	manage_stream_type = stream_type;
}

void app_manage_play_lock_prog()
{
	uint32_t pos;

	if (manage_stream_type != sysinfo_old.stream_type)
		{
			GxBusPmViewInfo sysinfo;

			/*
			* 进入节目编辑前，节目类型与当前编辑节目类型不一致
			*/
			sysinfo_old.skip_view_switch = VIEW_INFO_SKIP_VANISH;
			GxBus_PmViewInfoModify(&sysinfo_old);	
			app_prog_change_group(GROUP_MODE_ALL, app_prog_get_stream_type(), 0);
			app_prog_get_playing_pos_in_group(&pos);
			app_play_video_audio(pos);	

			GxBus_PmViewInfoGet(&sysinfo);
			sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
			GxBus_PmViewInfoModify(&sysinfo);		
			app_prog_change_group(GROUP_MODE_ALL, manage_stream_type, 0);
			

			
		}
	else
		{
			app_prog_get_playing_pos_in_group(&pos);
			app_play_video_audio(pos);			
		}
	return ;
}

SIGNAL_HANDLER int win_prog_manage_create(const char* widgetname, void *usrdata)
{
	uint32_t program_num=0;
	uint32_t sel = 0;
	uint32_t pos = 0;
	GxBusPmDataProg prog={0};
	GxBusPmViewInfo sysinfo={0};
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef APP_HD
	char* osd_language=NULL;


	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
		GUI_SetProperty("win_prog_prog_manage_title", "img", "title_manager.bmp");
		GUI_SetProperty("win_prog_manage_hint_exit", "img", "tips_exit.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		GUI_SetProperty("win_prog_prog_manage_title", "img", "title_manager_e.bmp");
		GUI_SetProperty("win_prog_manage_hint_exit", "img", "tips_exit_e.bmp");
	}
#endif
	GUI_SetProperty("win_prog_manage_time_text", "string", app_win_get_local_date_string());

	/*
	 * 显示当前类型所有节目,包括设置为跳过的节目
	 */
	app_win_set_focus_video_window(PROGRAM_EDIT_WIN);
	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	{
#ifndef DVB_CA_TYPE_DS_CAS50_FLAG
		app_play_switch_prog_clear_msg();
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
	}
	else
	{
		app_play_switch_prog_clear_msg();
	}	
	//	app_play_reset_play_timer(0);
	app_prog_get_playing_pos_in_group(&sel);
	GxBus_PmProgGetByPos(sel,1,&prog);



	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));
	sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
	GxBus_PmViewInfoModify(&sysinfo);		
	program_num = app_prog_change_group(GROUP_MODE_ALL, manage_stream_type, 0);
	app_prog_edit_init_attr();

#ifdef MOVE_FUNCTION
	GUI_SetProperty("win_prog_manage_skip_hint", "string", "Move");
	GUI_SetProperty("win_prog_manage_lock_flag","string","Move");
#else
	GUI_SetProperty("win_prog_manage_skip_hint", "string", "Skip");
	GUI_SetProperty("win_prog_manage_lock_flag","string","Skip");
#endif
	GUI_SetProperty("win_prog_manage_listview","update_all",NULL);
	app_prog_get_pos_in_group_by_id(prog.id,&pos);
	GUI_SetProperty("win_prog_manage_listview","select",(void*)&pos);
	GUI_SetProperty("win_prog_manage_listview","active",(void*)&pos);
	GUI_SetProperty("win_prog_manage_time_text", "string", app_win_get_local_date_string());

	app_set_win_create_flag(MANAGE_MENU_WIN);


	return 0;
}

SIGNAL_HANDLER  int win_prog_manage_destroy(const char* widgetname, void *usrdata)
{
	app_set_win_destroy_flag(MANAGE_MENU_WIN);

	app_win_set_focus_video_window(MAX_WINDOW_NUM);
	return 0;
}


SIGNAL_HANDLER  int win_prog_manage_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t value = 0;
	uint32_t pos = 0;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

	GUI_GetProperty("win_prog_manage_listview","select",(void*)&value);

	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_MENU:
				case KEY_EXIT:
				case KEY_RECALL:
					if (TRUE == app_prog_edit_get_attr_change_flag())
					{
						if(POPMSG_RET_YES == app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_CHK_SAVE,POPMSG_TYPE_YESNO))

						{
							app_prog_eidt_save_attr("win_prog_manage_listview");
						}
					}

					sysinfo_old.skip_view_switch = VIEW_INFO_SKIP_VANISH;
					GxBus_PmViewInfoModify(&sysinfo_old);	
					if (0 == app_prog_change_group(GROUP_MODE_ALL, app_prog_get_stream_type(), 0))
					{
						if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
						{
							//if (0 !=  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0))
							{
								app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0);

							}
						}
						else
							if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
							{
								if (0 !=  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0))
								{
									app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_RADIO, 0);
								}
							}					

					}
					else
					{
						app_prog_get_playing_pos_in_group(&pos);
						app_prog_save_playing_pos_in_group(pos);
					}
					app_play_stop();
					if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
					{
#ifndef DVB_CA_TYPE_DS_CAS50_FLAG
						app_play_switch_prog_clear_msg();
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
					}
					else
					{
						app_play_switch_prog_clear_msg();
					}
					GUI_EndDialog("win_prog_manage");
					return EVENT_TRANSFER_STOP;
					break;					
				case KEY_GREEN://加锁
					app_prog_edit_set_attr("win_prog_manage_listview",PROG_EDIT_LOCK);
					return EVENT_TRANSFER_STOP;
				case KEY_RED://删除
					app_prog_edit_set_attr("win_prog_manage_listview",PROG_EDIT_DEL);	
					break;
				case KEY_YELLOW://喜爱
					app_prog_edit_set_attr("win_prog_manage_listview",PROG_EDIT_FAV);
					break;
#ifndef MOVE_FUNCTION
				case KEY_BLUE://跳过
					app_prog_edit_set_attr("win_prog_manage_listview",PROG_EDIT_SKIP);
					break;
#else
					/*move function must set taxis_mode = TAXIS_MODE_NON;//TAXIS_MODE_SERVICE_ID;*/
				case KEY_BLUE: //移动
					app_prog_edit_set_attr("win_prog_manage_listview",PROG_EDIT_MOVE);		
					break;
#endif
					//#define RENAME_FUNCTION
#ifdef RENAME_FUNCTION
				case KEY_1:// 重命名
					app_prog_edit_rename_attr("win_prog_manage_listview","测试0001");
					break;
#endif
				default:
					break;
			}
			break;
		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int win_prog_manage_listview_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t value = 0;

	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_UP:
					GUI_GetProperty("win_prog_manage_listview","select",(void*)&value);

					//GUI_SetProperty("win_prog_manage_listview","active",(void*)&value);
					app_prog_edit_move_attr("win_prog_manage_listview",-1);
					break;
				case KEY_DOWN:
					GUI_GetProperty("win_prog_manage_listview","select",(void*)&value);
					//GUI_SetProperty("win_prog_manage_listview","active",(void*)&value);
					app_prog_edit_move_attr("win_prog_manage_listview",1);
					break;
				default:
					return EVENT_TRANSFER_KEEPON;
					break;
			}
			break;
		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int win_prog_manage_listview_get_total(const char* widgetname, void *usrdata)
{
	return app_prog_get_num_in_group();
}

SIGNAL_HANDLER  int win_prog_manage_listview_get_data(const char* widgetname, void *usrdata)
{
	static GxBusPmDataProg prog;
	static uint8_t pArry[5] = {0};
	ListItemPara *item_para =NULL;	
	Lcn_State_t lcn_flag;
	App_proedit_arr* pedit_arr=NULL;

	if(NULL == widgetname || NULL == usrdata)	
	{		
		return (1); 
	}	

	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para) return GXCORE_ERROR;

	memset(pArry,0,5);
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
	item_para->x_offset = 5;
	item_para->string = (char*)pArry;
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;

	pedit_arr = app_prog_edit_get_attr(item_para->sel);
	if (NULL ==pedit_arr )
	{		
		return (1); 
	}

	//		GxBus_PmProgGetByPos(pedit_arr->prog_pos,1,&prog);
	//		item_para->string = (char*)prog.prog_name;
	item_para->string = pedit_arr->prog_name;
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;



	if(pedit_arr->del_flag!=0)
	{
		item_para->image = "DS_PG_del.bmp";
	}
	else
	{
		item_para->image = NULL;
	}

	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;

	if(pedit_arr->lock_flag!=0)
	{	
		item_para->image = "DS_PM_lock.bmp";
	}
	else
	{
		item_para->image = NULL;
	}


	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;


	if(pedit_arr->fav_flag!=0)
	{
		item_para->image = "DS_PM_fav.bmp";
	}
	else
	{
		item_para->image = NULL;
	}
	item_para= item_para->next;
	if(NULL == item_para) return GXCORE_ERROR;
#ifdef MOVE_FUNCTION
	if(pedit_arr->mov_flag!=0)
	{
		item_para->image = "DS_PM_move.bmp";
	}
	else
	{
		item_para->image = NULL;
	}
#else
	if(pedit_arr->skip_flag!=0)
	{
		item_para->image = "DS_ICON_SELECT.bmp";
	}
	else
	{
		item_para->image = NULL;
	}
#endif


	return 0;
}

SIGNAL_HANDLER  int win_prog_manage_listview_change(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;

	GUI_GetProperty("win_prog_manage_listview","select",(void*)&value);
	GUI_SetProperty("win_prog_manage_listview","active",(void*)&value);
	return 0;
}


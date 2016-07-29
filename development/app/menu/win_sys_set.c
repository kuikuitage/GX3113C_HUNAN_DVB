#include "app.h"
#include "win_main_menu.h"
#if DVB_NETWORK_FLAG
#include "youtube_tools.h"
#endif
//resourse
#define SYSSET_LISTVIEW1        "win_sys_set_listview1"
#define SYSSET_LISTVIEW2        "win_sys_set_listview2"
#define SYSSET_TEXT_TIME        "win_sys_set_time_text"

static int systemSetButtonNum= 0;

#if DVB_NETWORK_FLAG
static int net_app_redtube = 0;
extern 	void app_set_wnd_type(WND_TYPE type); /* 0:youtube  1:iptv */
extern 	void app_weather_map_set_wnd(uint8_t wnd);
#endif

#if MEDIA_SUBTITLE_SUPPORT
extern void app_subt_pause(void);
#endif

extern void app_set_prog_manage_stream_type(uint8_t stream_type);

//widget

//string

#define SYSSET_LISTVIEW2_ITEM_MAX      20

typedef struct sys_set_list2_item_t 
{
	char* list2_content;//子菜单
	char* list2_window; //窗体名称(组件)
}sys_set_list2_item;

typedef struct sys_set_list1_t
{
	char* list1_content;//菜单
	sys_set_list2_item list2_item[SYSSET_LISTVIEW2_ITEM_MAX];
}sys_set_list1;

//主界面菜单
sys_set_list1 sys_set_list[] = 
{
	{"Program Search",
		{
			{"Auto Search", "win_auto_search"},
			{"Manual Search", "win_manual_search"},
			{"Full Frequency Search", "win_all_search"},
			{"Main Frequency Set", "win_main_frequecy_set"},
#ifdef APP_SD   
			{"Channel Set", "win_channel_set"}
#endif

		}}, 

	{"Program Manage",
		{
			{"TV Service", "win_prog_manage"},
			{"Radio Service", "win_prog_manage"},
			{"Favorite Program", "win_favorite_prog_list"},
			{"Book Manage", "win_book_manage"}
		}},

	{"AV Setting",
		{
			{"Audio Setting", "win_audio_set"},
			{"Video Setting", "win_video_set"},
			{"Color Setting", "win_videocolor_set"},
		}},

	{"Menu Setting",
		{
			{"Language", "win_lang_set"},
			{"Transparency", "win_trans_set"},
		}},

	{"System Information",
		{
			{"STB Information", "win_stb_info"},
			{"Program Information", "win_prog_info"},
		}},
#ifdef CA_FLAG  
	{"CA Information",
		{
			{"ca info", "win_ca_info"},
		}},
#endif

	{"Advance Setting",
		{
			{"System Update", "win_sys_update"},
			{"Password Setting", "win_passwd_set"},
			//        {"Network Setting", "win_network_set"},
			{"Reset To Factory", "win_reset_to_factory"},
		}},

#if DVB_PVR_FLAG
	{"PVR Manage",
		{
			{"Record Files", "wnd_pvr_media_list"},
			{"USB DISK Info", "win_disk_information"}, 
			{"Record Time", "win_duration_set"}, 
		}},
#endif

#if DVB_NETWORK_FLAG
	{
		"NetWork Setting",
		{
			{"Wireless AP list","win_wifi"},
			{"NetWork Setting","wnd_network"},
		}
	},
	{
		"NetWork APP",
		{
			{"youtube","wnd_youtube"},
			{"iptv","wnd_youtube"},
			{"photo","wnd_picasa"},
			{"rss","wnd_rss"},
			{"weather","wnd_weather_map"},
			{"map","wnd_weather_map"},
			{"redtube","wnd_youtube"},
			{"youporn","wnd_youtube"},
		}
	},
#endif
};


char osd_language[LANGUAGE_NAME_MAX+1]={0};


SIGNAL_HANDLER  int sys_set_service(const char* widgetname, void *usrdata)
{
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int sys_set_create(const char* widgetname, void *usrdata)
{
	memset(osd_language,0,LANGUAGE_NAME_MAX+1);
	strcpy(osd_language,app_flash_get_config_osd_language());
#if 0
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
	{
		GUI_SetProperty("win_system_menu_image230","img", "DS_TITLE_SYS_SET.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		GUI_SetProperty("win_system_menu_image230","img", "DS_TITLE_SYS_SET_ENG.bmp");
	}
#endif
#ifdef HD
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
	{
		GUI_SetProperty("win_sys_set_title", "img", "title_set.bmp");
		GUI_SetProperty("win_sys_set_tip_image_exit", "img", "tips_exit.bmp");
		GUI_SetProperty("win_sys_set_tip_image_ok", "img", "tips_confirm.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
		GUI_SetProperty("win_sys_set_title", "img", "title_set_e.bmp");
		GUI_SetProperty("win_sys_set_tip_image_exit", "img", "tips_exit_e.bmp");
		GUI_SetProperty("win_sys_set_tip_image_ok", "img", "tips_confirm_e2.bmp");
	}

	GUI_SetProperty(SYSSET_TEXT_TIME, "string", app_win_get_local_date_string());
#endif

	app_set_win_create_flag(SYS_SET_MENU_WIN);
	app_win_set_focus_video_window(SYS_SET_MENU_WIN);
	return 0;
}

SIGNAL_HANDLER int sys_set_destroy(const char* widgetname, void *usrdata)
{

#ifdef APP_HD
	char* language=NULL;
	language = app_flash_get_config_osd_language();
	if (0 != strcmp(osd_language,language))
	{
		strcpy(osd_language,language);
		if (0 == strcmp(language,LANGUAGE_CHINESE))
		{
			GUI_SetProperty("window_main_menu_image_title", "img", "title_menu.bmp");//title
			GUI_SetProperty("win_main_menu_tip_image_exit", "img", "tips_exit.bmp");
			GUI_SetProperty("win_main_menu_tip_image_ok", "img", "tips_confirm.bmp");

			GUI_SetProperty("win_main_menu_tv_list_button", "img", "main_menu_tv_unfocus_button.bmp");//title
			GUI_SetProperty("win_main_menu_radio_list_button", "img", "main_menu_radio_unfocus_button.bmp");//title
			GUI_SetProperty("win_main_menu_week_epg_list_button", "img", "main_menu_week_epg_unfocus_button.bmp");//title
			GUI_SetProperty("win_main_menu_nvod_button", "img", "main_menu_nvod_unfocus_button.bmp");//title
			GUI_SetProperty("win_main_menu_media_button", "img", "main_menu_media_unfocus_button.bmp");//title
			GUI_SetProperty("win_main_menu_system_set_button", "img", "main_menu_system_focus_button.bmp");//title


		}
		else if(0 == strcmp(language,LANGUAGE_ENGLISH))
		{
			GUI_SetProperty("window_main_menu_image_title", "img", "title_menu_e.bmp");
			GUI_SetProperty("win_main_menu_tip_image_exit", "img", "tips_exit_e.bmp");
			GUI_SetProperty("win_main_menu_tip_image_ok", "img", "tips_confirm_e2.bmp");

			GUI_SetProperty("win_main_menu_tv_list_button", "img", "menu_tvlist_unfocus_e.bmp");//title
			GUI_SetProperty("win_main_menu_radio_list_button", "img", "menu_radiolist_unfocus_e.bmp");//title
			GUI_SetProperty("win_main_menu_week_epg_list_button", "img", "menu_epg_unfocus_e.bmp");//title
			GUI_SetProperty("win_main_menu_nvod_button", "img", "menu_video_unfocus_e.bmp");//title
			GUI_SetProperty("win_main_menu_media_button", "img", "menu_media_unfocus_e.bmp");//title
			GUI_SetProperty("win_main_menu_system_set_button", "img", "menu_set_focus_e.bmp");//title
		}
		GUI_SetInterface("flush", NULL);

		win_main_menu_set_img_list();

	}
#endif	
	app_set_win_destroy_flag(SYS_SET_MENU_WIN);
	app_win_set_focus_video_window(MAIN_MENU_WIN);
#ifdef APP_SD
	app_play_video_window_zoom(192, 152, 348, 240);
#endif
#ifdef APP_HD
	app_play_video_window_zoom(400, 160, 500, 290);
#endif

	return 0;
}
#if DVB_NETWORK_FLAG
static int step1=0;
static int step2=0;
static int step3=0;
#endif

SIGNAL_HANDLER int sys_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
#if DVB_NETWORK_FLAG
		case KEY_RED:
			step1 = 1;
			step2 = 0;
			step3 = 0;
			break;
		case KEY_YELLOW:
			if(step1 == 1)
			{
				step2 = 1;
			}
			else
			{
				step1 = 0;
				step2 = 0;
				step3 = 0;
			}
			break;
		case KEY_BLUE:
			if((step1 == 1)&&(step2 == 1))
			{
				step3 = 1;
			}
			else
			{
				step1=0;
				step2=0;
				step3=0;
			}
			break;
		case KEY_GREEN:
			if((step1 == 1)&&(step2 == 1) && (step3==1))
			{
				int sel=0;
				net_app_redtube = !net_app_redtube;
				GUI_GetProperty(SYSSET_LISTVIEW1, "select", &sel);
				if(strcmp(sys_set_list[sel].list1_content,"NetWork APP") == 0)
				{
					GUI_SetProperty(SYSSET_LISTVIEW2, "update", NULL);
				}
			}
			step1=0;
			step2=0;
			step3=0;
#endif
		default:
			break;
	}	

	return EVENT_TRANSFER_KEEPON;//消息传递处理
}

SIGNAL_HANDLER int sys_set_listview1_get_count(const char* widgetname, void *usrdata)
{
	//printf("sys_set_listview1_get_count list1: %d\n", sizeof(sys_set_list)/sizeof(sys_set_list[0]));

	return (sizeof(sys_set_list)/sizeof(sys_set_list[0]));
}

SIGNAL_HANDLER int sys_set_listview1_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	uint32_t index = 0;

	item = (ListItemPara*)usrdata;
	if(NULL == item) return GXCORE_ERROR;
	index = (uint32_t)item->sel;
#ifdef APP_SD
	item->x_offset = 10;
#endif
#ifdef APP_HD
	item->x_offset = 30;
#endif
	item->image = NULL;	
	item->string = sys_set_list[index].list1_content;

    printf("sys_set_listview1_get_data: %s\n",item->string);
	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int sys_set_listview1_change(const char* widgetname, void *usrdata)
{
	int list2_sel = 0;
	GUI_SetProperty(SYSSET_LISTVIEW2, "select", (void*)&list2_sel);
	GUI_SetProperty(SYSSET_LISTVIEW2, "update", NULL);

	return GXCORE_SUCCESS;	
}

SIGNAL_HANDLER int sys_set_listview1_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_RECALL:
			case KEY_EXIT:
			case KEY_MENU:
				/*
				 * 返回主菜单，播放视频
				 */
				app_win_set_focus_video_window(MAIN_MENU_WIN);
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
  
				GUI_EndDialog("win_sys_set");
				app_play_reset_play_timer(0);
				return EVENT_TRANSFER_STOP;

			case KEY_RIGHT:
			case KEY_LEFT:
			case KEY_OK:
				{  
					int list1_sel = 0;
					GUI_GetProperty(SYSSET_LISTVIEW1, "select", &list1_sel);//当前选中的项
					GUI_SetProperty(SYSSET_LISTVIEW1, "active", (void*)&list1_sel);
					GUI_SetFocusWidget(SYSSET_LISTVIEW2);//设置焦点组件到listveiw2上
				}
				break;

			default:
				return EVENT_TRANSFER_KEEPON;
				break;
		}
	}

	return EVENT_TRANSFER_STOP;
}




SIGNAL_HANDLER int sys_set_listview2_get_count(const char* widgetname, void *usrdata)
{
	int list1_sel = 0;
	int i = 0;

	//listview1选中的第几栏
	GUI_GetProperty(SYSSET_LISTVIEW1, "select", &list1_sel);
#ifdef CA_FLAG
	if (5 == list1_sel)
	{
		int32_t dvb_ca_flag = app_flash_get_config_dvb_ca_flag();

		switch(dvb_ca_flag)
		{
			case DVB_CA_TYPE_CDCAS30:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base Info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_cd_cas30_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_cd_cas30_entitle";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Paired Info";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_cd_cas30_paried";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Modify Pin";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_cd_cas30_pin";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Watch Level";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_cd_cas30_rating";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Work Time";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_cd_cas30_worktime";
				sys_set_list[list1_sel].list2_item[6].list2_content = "Features Info";
				sys_set_list[list1_sel].list2_item[6].list2_window = "win_cd_cas30_features";
				sys_set_list[list1_sel].list2_item[7].list2_content = "Ippv Slot";
				sys_set_list[list1_sel].list2_item[7].list2_window = "win_cd_cas30_ippv_slot";
				sys_set_list[list1_sel].list2_item[8].list2_content = "Ippv Info";
				sys_set_list[list1_sel].list2_item[8].list2_window = "win_cd_cas30_ippv_info";
				sys_set_list[list1_sel].list2_item[9].list2_content = "SubMainCard";
				sys_set_list[list1_sel].list2_item[9].list2_window = "win_cd_cas30_feed";
				sys_set_list[list1_sel].list2_item[10].list2_content = "Detitle Info";
				sys_set_list[list1_sel].list2_item[10].list2_window = "win_cd_cas30_detitle";
				sys_set_list[list1_sel].list2_item[11].list2_content = "Card Update Info";
				sys_set_list[list1_sel].list2_item[11].list2_window = "win_cd_cas30_cardupdate";
				sys_set_list[list1_sel].list2_item[12].list2_content = "Email List";
				sys_set_list[list1_sel].list2_item[12].list2_window = "win_cd_cas30_email";
				return 13;											
			case DVB_CA_TYPE_BY:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_by_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_by_cas_entitle";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_by_cas_email";
				return 3;						
			case DVB_CA_TYPE_KN:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_kn_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_kn_cas_entitle";
				return 2;						
			case DVB_CA_TYPE_XINSHIMAO:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_xinshimao_cas_baseinfo";
				return 1;	
			case DVB_CA_TYPE_DIVITONE:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Card Info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_divitone_cas_cardinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_divitone_cas_email";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Work Time";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_divitone_cas_worktime";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Watch Level";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_divitone_cas_watchlevel";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Set Pin";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_divitone_cas_pin";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Active Child";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_divitone_cas_activechild";							
				sys_set_list[list1_sel].list2_item[6].list2_content = "Entitle Info";
				sys_set_list[list1_sel].list2_item[6].list2_window = "win_divitone_cas_entitleinfo";							
				return 7;							
			case DVB_CA_TYPE_DSCAS50:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_desai_cas50_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Operator Info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_desai_cas50_operatorinfo";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Card Set Watchlevel";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_desai_cas50_card_setwatchlevel";						
				sys_set_list[list1_sel].list2_item[3].list2_content = "Card Set Worktime";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_desai_cas50_card_setworktime";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Modify Pin";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_desai_cas50_changepin";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Purser Info";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_desai_cas50_purser_info";
				sys_set_list[list1_sel].list2_item[6].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[6].list2_window = "win_desai_cas50_entitle_info";
				sys_set_list[list1_sel].list2_item[7].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[7].list2_window = "win_desai_cas50_email";							
				return 8;
			case DVB_CA_TYPE_QILIAN:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_qilian_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_qilian_cas_entitle";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_qilian_cas_email";
				return 3;	
				/*begin:add by chenth,移植迅驰CA*/
			case DVB_CA_TYPE_MG:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_mg_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Features Code";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_mg_cas_featurescode";
				sys_set_list[list1_sel].list2_item[2].list2_content = "SubMainCard";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_mg_cas_submaincard";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_mg_cas_email";
				return 4;
				/*end:add by chenth,移植迅驰CA*/
			case DVB_CA_TYPE_QZ:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_qz_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Set Host Slave";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_qz_cas_masterslave";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_qz_cas_email";
				return 3;
			case DVB_CA_TYPE_GOS:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_gos_cas_baseinfo";
				return 1;
			case DVB_CA_TYPE_GY:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_gy_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_gy_cas_entitle";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Paired Info";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_gy_cas_paried";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Modify Pin";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_gy_cas_pin";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Watch Level";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_gy_cas_rating";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Email List";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_gy_cas_email";
				return 6;
			case DVB_CA_TYPE_WF:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_wf_cas_baseinfo";					
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_wf_cas_entitle";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_wf_cas_email";
				return 3;
			case DVB_CA_TYPE_DVT:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Authorization Information";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_dvt_cas_authorize";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Smartcard Information";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_dvt_cas_smart";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Password Setting";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_dvt_cas_pin";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Lock Setting";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_dvt_cas_rating";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Period Setting";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_dvt_cas_worktime";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Mail Manage";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_dvt_cas_email";							
				return 6;
			case DVB_CA_TYPE_KP:/*天柏CA*/
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base Info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_kp_cas_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Authorization Info";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_kp_cas_entitle_info";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Modify Pin";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_kp_cas_pin";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Watch Level";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_kp_cas_watchlevel";
				sys_set_list[list1_sel].list2_item[4].list2_content = "Work Time";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_kp_cas_worktime";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_kp_cas_email";
				//sys_set_list[list1_sel].list2_item[6].list2_content = "IPPV Slot";
				//sys_set_list[list1_sel].list2_item[6].list2_window = "win_kp_cas_ippv_slot";

				return 6;
				/*wangjian add on 20141217 for MGCAS3.1.2*/
			case DVB_CA_TYPE_MG312:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_mg_cas312_baseinfo";
				sys_set_list[list1_sel].list2_item[1].list2_content = "Features Code";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_mg_cas312_featurescode";
				sys_set_list[list1_sel].list2_item[2].list2_content = "SubMainCard";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_mg_cas312_submaincard";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_mg_cas312_email";
				return 4;
				/*wangjian add end.*/
			case DVB_CA_TYPE_TR:
				sys_set_list[list1_sel].list2_item[0].list2_content = "Base info";
				sys_set_list[list1_sel].list2_item[0].list2_window = "win_tr_cas_baseinfo";	
				sys_set_list[list1_sel].list2_item[1].list2_content = "Modify Pin";
				sys_set_list[list1_sel].list2_item[1].list2_window = "win_tr_cas_pin";
				sys_set_list[list1_sel].list2_item[2].list2_content = "Watch Level";
				sys_set_list[list1_sel].list2_item[2].list2_window = "win_tr_cas_rating";
				sys_set_list[list1_sel].list2_item[3].list2_content = "Authorization info";
				sys_set_list[list1_sel].list2_item[3].list2_window = "win_tr_cas_entitle";
				sys_set_list[list1_sel].list2_item[4].list2_content = "IPP Info";
				sys_set_list[list1_sel].list2_item[4].list2_window = "win_tr_cas_ippv_order";
				sys_set_list[list1_sel].list2_item[5].list2_content = "Slot Info";
				sys_set_list[list1_sel].list2_item[5].list2_window = "win_tr_cas_ippv_slot";
				sys_set_list[list1_sel].list2_item[6].list2_content = "Ipp Bill";
				sys_set_list[list1_sel].list2_item[6].list2_window = "win_tr_cas_ippv_bill";
				sys_set_list[list1_sel].list2_item[7].list2_content = "MC-Manage";
				sys_set_list[list1_sel].list2_item[7].list2_window = "win_tr_cas_mc_mgr";
				sys_set_list[list1_sel].list2_item[8].list2_content = "Email";
				sys_set_list[list1_sel].list2_item[8].list2_window = "win_tr_cas_email";
						sys_set_list[list1_sel].list2_item[9].list2_content = "Announce";
						sys_set_list[list1_sel].list2_item[9].list2_window = "win_tr_cas_announce_list";
						return 10;	
				break;
			default:
				break;
		}

	}
#endif
#if DVB_NETWORK_FLAG
	if(strcmp(sys_set_list[list1_sel].list1_content,"NetWork APP") == 0)
	{
		if(net_app_redtube == 0)
		{
			sys_set_list[list1_sel].list2_item[6].list2_content = NULL;
			sys_set_list[list1_sel].list2_item[7].list2_content = NULL;
		}
		else
		{
			sys_set_list[list1_sel].list2_item[6].list2_content = "redtube";
			sys_set_list[list1_sel].list2_item[7].list2_content = "youporn";
		}
	}
#endif

	for(i = 0; i < SYSSET_LISTVIEW2_ITEM_MAX; i++)
	{
		if (NULL == sys_set_list[list1_sel].list2_item[i].list2_content)
			break;
	}

	return i;
}
//获取数据
SIGNAL_HANDLER int sys_set_listview2_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	uint32_t index = 0;
	int list1_sel = 0;

	//listview当前选中项(第n行)
	GUI_GetProperty(SYSSET_LISTVIEW1, "select", &list1_sel);

	item = (ListItemPara*)usrdata;
	if(NULL == item) return GXCORE_ERROR;
	index = (uint32_t)item->sel;

	item->x_offset = 50;
	item->image = NULL;	
	item->string = sys_set_list[list1_sel].list2_item[index].list2_content;

	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int sys_set_listview2_change(const char* widgetname, void *usrdata)
{
	return GXCORE_SUCCESS;	
}

SIGNAL_HANDLER int sys_set_listview2_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int list1_sel = 0;
	int list2_sel = 0;
	int ret = 0;
	char language[LANGUAGE_NAME_MAX+1]={0};
#ifdef CA_FLAG
	int capopmsg_flag = 0;
	int check_passwdFailed = 0;
#endif
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
				/*
				 * 返回主菜单，播放视频
				 */
				app_win_set_focus_video_window(MAIN_MENU_WIN);
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
  
  
				GUI_EndDialog("win_sys_set");
				GUI_SetInterface("flush", NULL);
				app_play_reset_play_timer(0);
				return EVENT_TRANSFER_STOP;

			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_RECALL:
			case KEY_MENU:
				{   
					int active_no = -1;
					GUI_SetProperty(SYSSET_LISTVIEW1, "active", (void*)&active_no);
					GUI_SetFocusWidget(SYSSET_LISTVIEW1);//切换焦点到listveiw1上
				}
				break;

			case KEY_OK://进入子菜单功能 (上下键已经在组件内部处理)
				GUI_GetProperty(SYSSET_LISTVIEW1, "select", &list1_sel);
				GUI_GetProperty(SYSSET_LISTVIEW2, "select", &list2_sel);
#ifdef CA_FLAG
				int32_t dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
				switch(dvb_ca_flag)
				{
					case DVB_CA_TYPE_CDCAS30:
						if (strcasecmp("win_cd_cas30_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_cd_cas30_paried", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_PAIRED_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_pin", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_rating", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_worktime", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_features", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_FETURE_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_ippv_slot", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_ippv_info", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_PROG_INFO);		
						}
						else if (strcasecmp("win_cd_cas30_feed", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_MOTHER_CARD_INFO); 	
						}
						else if (strcasecmp("win_cd_cas30_detitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_DETITLE_INFO); 	
						}
						else if (strcasecmp("win_cd_cas30_update", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CARD_UPDATE_INFO); 	
						}
						else if (strcasecmp("win_cd_cas30_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO); 	
						}
						break;
					case DVB_CA_TYPE_BY:
						if (strcasecmp("win_by_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_by_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_by_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						break;
					case DVB_CA_TYPE_KN:
							if (strcasecmp("win_by_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
							{
								app_cas_api_init_data(DVB_CA_BASE_INFO);
							}
							else if (strcasecmp("win_by_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
							{
								app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
							}
							else if (strcasecmp("win_by_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
							{
								app_cas_api_init_data(DVB_CA_EMAIL_INFO);
							}
						break;
					case DVB_CA_TYPE_XINSHIMAO:
						if (strcasecmp("win_xinshimao_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						break;
					case DVB_CA_TYPE_DIVITONE:
						if (strcasecmp("win_divitone_cas_cardinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_divitone_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						else if (strcasecmp("win_divitone_cas_worktime", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);		
						}							
						else if (strcasecmp("win_divitone_cas_entitleinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);		
						}
						break;							
					case DVB_CA_TYPE_QILIAN:
						printf("list1_sel=%d,list2_sel=%d\n",list1_sel,list2_sel);
						if(sys_set_list[list1_sel].list2_item[list2_sel].list2_window)
						{
							//printf("list2_windowl=%s\n",sys_set_list[list1_sel].list2_item[list2_sel].list2_windowl);
							printf("list2_windowl=%s\n",sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
						}
						else
							printf("windows null\n");
						if (strcasecmp("win_qilian_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_qilian_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							printf("**win_qilian_cas_entitle**\n");
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_qilian_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						break;
					case DVB_CA_TYPE_MG:
						if (strcasecmp("Email", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						else
						{
							if(app_cas_api_init_data(DVB_CA_BASE_INFO)<0)
							{
								if(!strcasecmp("Features Code", sys_set_list[list1_sel].list2_item[list2_sel].list2_content)
										|| !strcasecmp("SubMainCard", sys_set_list[list1_sel].list2_item[list2_sel].list2_content))
								{
									capopmsg_flag = 1;
								}
							}
						}
						break;

					case DVB_CA_TYPE_QZ:
						{
							if(strcasecmp("Set Host Slave", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
							{
								extern int PasswdCheckOK();
								extern void win_SetName(char *win);
								win_SetName(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
								GUI_CreateDialog("win_password_check");
								if(!PasswdCheckOK())
								{
									check_passwdFailed = 1;
								}
							}

						}
						break;


					case DVB_CA_TYPE_GY:
						if (strcasecmp("win_gy_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_gy_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_gy_cas_paried", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_PAIRED_INFO);		
						}
						else if (strcasecmp("win_gy_cas_pin", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);		
						}
						else if (strcasecmp("win_gy_cas_rating", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);		
						}
						else if (strcasecmp("win_gy_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);	
						}
						break;
					case DVB_CA_TYPE_WF:
						if (strcasecmp("win_wf_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_wf_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_wf_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						break;
					case DVB_CA_TYPE_MG312:
						if (strcasecmp("Email", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);		
						}
						else
						{
							if(app_cas_api_init_data(DVB_CA_BASE_INFO)<0)
							{
								if(!strcasecmp("Features Code", sys_set_list[list1_sel].list2_item[list2_sel].list2_content)
										|| !strcasecmp("SubMainCard", sys_set_list[list1_sel].list2_item[list2_sel].list2_content))
								{
									capopmsg_flag = 1;
								}
							}
						}
						break;	
					case DVB_CA_TYPE_TR:
						if (strcasecmp("win_tr_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if(strcasecmp("win_tr_cas_rating", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if(strcasecmp("win_tr_cas_entitle", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_order", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_PROG_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_slot", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_bill", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_DETITLE_INFO);
						}
						else if(strcasecmp("win_tr_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							//app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
							else if(strcasecmp("win_tr_cas_announce_list", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
							{
								//app_cas_api_init_data(DVB_CA_CURTAIN_INFO);
							}
						break;
					case DVB_CA_TYPE_DVT:
						if (strcasecmp("win_dvt_cas_authorize", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_OPERATOR_INFO))
							{
						            ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Access to information error",POPMSG_TYPE_OK);

					
								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_smart", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_dvt_cas_rating", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_RATING_INFO))
							{
						            ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Access to information error",POPMSG_TYPE_OK);


								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_worktime", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_WORK_TIME_INFO))
							{
						            ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Access to information error",POPMSG_TYPE_OK);


								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_EMAIL_INFO))
							{
						            ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Access to information error",POPMSG_TYPE_OK);

									return EVENT_TRANSFER_STOP;

							}
						}
						break;
					case DVB_CA_TYPE_KP:
						if (strcasecmp("win_kp_cas_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);		
						}
						else if (strcasecmp("win_kp_cas_entitle_info", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO); 	
						}
						else if (strcasecmp("win_kp_cas_ippv_slot", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO); 	
						}
						break;
	case DVB_CA_TYPE_DSCAS50:
                    {
                        printf("item name is %s.\n", sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
						if (strcasecmp("win_desai_cas50_baseinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_desai_cas50_operatorinfo", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_OPERATOR_INFO);
						}
						else if (strcasecmp("win_desai_cas50_card_setwatchlevel", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if (strcasecmp("win_desai_cas50_card_setworktime", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);
						}
						else if (strcasecmp("win_desai_cas50_changepin", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);
						}
						else if (strcasecmp("win_desai_cas50_purser_info", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_PROG_INFO);
						}
						else if (strcasecmp("win_desai_cas50_entitle_info", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_desai_cas50_email", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}

						break;
                    }
					default:
						break;
				}
#endif
				if (strcasecmp("win_reset_to_factory", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
				{
					memcpy(language,app_flash_get_config_osd_language(),LANGUAGE_NAME_MAX);
#if MEDIA_SUBTITLE_SUPPORT

					app_subt_pause();
#endif
					ret =app_win_reset_factory();
					if(ret == 1)
						return EVENT_TRANSFER_STOP;
					if (0 == strcmp(language,LANGUAGE_ENGLISH))
					{
						GUI_SetProperty("win_sys_set_title", "img", "title_set.bmp");
						GUI_SetProperty("win_sys_set_title", "img", "title_set.bmp");
						GUI_SetProperty("win_sys_set_tip_image_exit", "img", "tips_exit.bmp");
						GUI_SetProperty("win_sys_set_tip_image_ok", "img", "tips_confirm.bmp");
						GUI_SetProperty("win_sys_set", "update", NULL);//update
						GUI_SetProperty("win_sys_set_factory_text", "draw_now", NULL);
						GxCore_ThreadDelay(1000);	
						GUI_SetProperty("win_sys_set_factory_text","state","hide");
					}
					else
					{
						GUI_SetProperty("win_sys_set", "update", NULL);//update
						GUI_SetProperty("win_sys_set_factory_text", "draw_now", NULL);
						GxCore_ThreadDelay(1000);	
						GUI_SetProperty("win_sys_set_factory_text","state","hide");
					}
				}
				else if (strcasecmp("win_favorite_prog_list", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
				{
					if (0 !=  app_prog_check_group_num(GROUP_MODE_FAV,app_prog_get_stream_type(),1))
					{
#ifdef APP_SD
						app_play_video_window_zoom(340, 40, 320, 240);
#endif
#ifdef APP_HD
						app_play_video_window_zoom(630, 160, 480, 300);
#endif
						GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
						GUI_SetInterface("flush", NULL);
					}
					else
					{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Favorite Program!",POPMSG_TYPE_OK);
					}
				}
				else if (strcasecmp("TV Service", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					GxBusPmViewInfo sysinfo;
					GxBusPmViewInfo sysinfo_old;
					GxBus_PmViewInfoGet(&sysinfo);
					memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));
					sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
					GxBus_PmViewInfoModify(&sysinfo);		

					if (0 !=  app_prog_check_group_num(GROUP_MODE_ALL, GXBUS_PM_PROG_TV, 0))
					{
						GxBus_PmViewInfoModify(&sysinfo_old);
						app_prog_update_num_in_group();
						app_set_prog_manage_stream_type(GXBUS_PM_PROG_TV);
#ifdef APP_SD
						app_play_video_window_zoom(430, 40, 240, 180);
#endif
#ifdef APP_HD
						app_play_video_window_zoom(750, 210, 360, 220);
#endif
						GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
						app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
						GUI_SetInterface("flush", NULL);
					}
					else
					{
						GxBus_PmViewInfoModify(&sysinfo_old);
						app_prog_update_num_in_group();

						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);
					}
				}
				else if (strcasecmp("Radio Service", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					GxBusPmViewInfo sysinfo;
					GxBusPmViewInfo sysinfo_old;
					GxBus_PmViewInfoGet(&sysinfo);
					memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));
					sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
					GxBus_PmViewInfoModify(&sysinfo);		

					if (0 !=  app_prog_check_group_num(GROUP_MODE_ALL, GXBUS_PM_PROG_RADIO, 0))
					{
						GxBus_PmViewInfoModify(&sysinfo_old);
						app_prog_update_num_in_group();
						app_set_prog_manage_stream_type(GXBUS_PM_PROG_RADIO);
#ifdef APP_SD
						app_play_video_window_zoom(430, 40, 240, 180);
#endif
#ifdef APP_HD
						app_play_video_window_zoom(750, 210, 360, 220);
#endif
						GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);
						app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
						GUI_SetInterface("flush", NULL);
					}
					else
						{
							GxBus_PmViewInfoModify(&sysinfo_old);
							app_prog_update_num_in_group();
							app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);
						}
			 	}
			 	else if (strcasecmp("win_auto_search", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
			 	{

					app_win_auto_search();

				}
				else if (strcasecmp("win_prog_info", sys_set_list[list1_sel].list2_item[list2_sel].list2_window) == 0)
				{
					if (0 == app_prog_get_num_in_group())
					{
						if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
							app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);
						}
						else
						{
							app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);
						}
						return EVENT_TRANSFER_STOP;	
					}
					else
					{
						GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);							
					}
				}
#if DVB_PVR_FLAG
				else if(strcasecmp("Record Files", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					extern 	void app_create_pvr_media_menu(void);
					app_create_pvr_media_menu();
				}
				else if(strcasecmp("USB DISK Info", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					extern 	void app_create_disk_information_menu(void);
					app_create_disk_information_menu();
				}
#endif

#if DVB_NETWORK_FLAG
				else if(strcasecmp("iptv", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					app_set_wnd_type(WND_TYPE_IPTV);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				else if(strcasecmp("youtube", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					app_set_wnd_type(WND_TYPE_YOUTUBE);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				else if(strcasecmp("weather", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)
				{
					app_weather_map_set_wnd(0);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				else if(strcasecmp("map", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)	
				{
					app_weather_map_set_wnd(1);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				else if(strcasecmp("redtube", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)	
				{
					app_set_wnd_type(WND_TYPE_REDTUBE);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				else if(strcasecmp("youporn", sys_set_list[list1_sel].list2_item[list2_sel].list2_content) == 0)	
				{
					app_set_wnd_type(WND_TYPE_YOUPORN);
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
#endif

#ifdef CA_FLAG
				else if(capopmsg_flag)	
				{
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Card not Ready!",POPMSG_TYPE_OK);			

				}
				else if(check_passwdFailed)
				{
				}
#endif
				else
				{
					//进入子窗体
					GUI_CreateDialog(sys_set_list[list1_sel].list2_item[list2_sel].list2_window);		  	
				}
				break;

			default:
				return EVENT_TRANSFER_KEEPON;
				break;
		}
	}

	return EVENT_TRANSFER_STOP;
}
 
SIGNAL_HANDLER int app_system_menu_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
//	GxBusPmViewInfo sys;
	event = (GUI_Event *)usrdata;

	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;
		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_UP:
					GUI_GetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					systemSetButtonNum--;
					if (systemSetButtonNum <0)
					{
						systemSetButtonNum = 3;
						GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
						GUI_SetFocusWidget("win_system_menu_box32");
						return EVENT_TRANSFER_STOP;
					}
					GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					systemSetButtonNum++;
					if (systemSetButtonNum > 3)
					{
						systemSetButtonNum = 0;
						GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
						GUI_SetFocusWidget("win_system_menu_box32");
						return EVENT_TRANSFER_STOP;
					}
					GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					return EVENT_TRANSFER_STOP;
				case KEY_LEFT:
					GUI_GetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					systemSetButtonNum--;
					if (systemSetButtonNum < 0)
					{
						systemSetButtonNum = 3;
					}
					GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					GUI_SetFocusWidget("win_system_menu_box32");
					return EVENT_TRANSFER_STOP;
				case KEY_RIGHT:
					GUI_GetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					GUI_SetFocusWidget("win_system_menu_box32");
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					switch(systemSetButtonNum)
					{
						case 0:
							GUI_CreateDialog("win_search_menu");
							break;
						case 1:
							GUI_CreateDialog("win_trans_set");
							break;
						case 2:	
#ifdef CA_FLAG
						GUI_CreateDialog("win_ca_menu");
#else
						app_popmsg(210,180,"NO CA!",POPMSG_TYPE_OK);	
#endif
		break;
						case 3:
							GUI_CreateDialog("win_stb_info");
							break;
					}
					return EVENT_TRANSFER_KEEPON;
				case KEY_MENU:
				case KEY_EXIT:
				case KEY_RECALL:
					app_win_set_focus_video_window(MAIN_MENU_WIN);
					systemSetButtonNum = 0;
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

					GUI_EndDialog("win_sys_set");
					app_play_reset_play_timer(0);
					return EVENT_TRANSFER_STOP;
				default:
					return EVENT_TRANSFER_KEEPON;
			}

		default:
			return EVENT_TRANSFER_KEEPON;
  	}
 	return EVENT_TRANSFER_KEEPON;
 } 
SIGNAL_HANDLER int app_system_menu_box1_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
//	GxBusPmViewInfo sys;
	event = (GUI_Event *)usrdata;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;
		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_UP:
					GUI_GetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					systemSetButtonNum--;
					if (systemSetButtonNum <0)
					{
						systemSetButtonNum = 3;
						GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
						GUI_SetFocusWidget("win_system_menu_box1");
						return EVENT_TRANSFER_STOP;
					}
					GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					systemSetButtonNum++;
					if (systemSetButtonNum > 3)
					{ 
						systemSetButtonNum = 0;
						GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
						GUI_SetFocusWidget("win_system_menu_box1");
						return EVENT_TRANSFER_STOP;
					}
					GUI_SetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					return EVENT_TRANSFER_STOP;
				case KEY_RIGHT:
					GUI_GetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					systemSetButtonNum++;
					if (systemSetButtonNum > 3)
					{
						systemSetButtonNum = 0;
					}
					GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					GUI_SetFocusWidget("win_system_menu_box1");
					return EVENT_TRANSFER_STOP;
				case KEY_LEFT:
					GUI_GetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					GUI_SetProperty("win_system_menu_box1", "select", &systemSetButtonNum);
					GUI_SetFocusWidget("win_system_menu_box1");
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_system_menu_box32", "select", &systemSetButtonNum);
					switch(systemSetButtonNum)
					{
						case 0:
							GUI_CreateDialog("win_video_menu");
							break;
						case 1:
							GUI_CreateDialog("win_lang_set");
							break;
	
						case 2:
							{	
								extern int PasswdCheckOK();
								extern void win_SetName(char *win);
								win_SetName("win_advanced_set_menu");
								GUI_CreateDialog("win_password_check");
								if(!PasswdCheckOK())
								{
								}
							}
							break;
						case 3:
						if (0 == app_prog_get_num_in_group())
						{
						if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
						{
#ifdef APP_SD
							app_popmsg(210,180,"No Tv Program!",POPMSG_TYPE_OK);	
#endif
#ifdef APP_HD
							app_popmsg(340,200,"No Tv Program!",POPMSG_TYPE_OK);
#endif								
						}
						else
						{
#ifdef APP_SD
							app_popmsg(210,180,"No Radio Program!",POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
							app_popmsg(340,200,"No Radio Program!",POPMSG_TYPE_OK);
#endif
						}
						return EVENT_TRANSFER_STOP;	
					}
					else
					{
						GUI_CreateDialog("win_prog_info");							
					}
							break;
					}
					return EVENT_TRANSFER_KEEPON;
				case KEY_MENU:
				case KEY_EXIT:
				case KEY_RECALL:
					app_win_set_focus_video_window(MAIN_MENU_WIN);
					systemSetButtonNum = 0;
       
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
					GUI_EndDialog("win_sys_set");
					app_play_reset_play_timer(0);
					return EVENT_TRANSFER_STOP;

				default:
					return EVENT_TRANSFER_KEEPON;
			}

		default:
			return EVENT_TRANSFER_KEEPON;
	}
	return EVENT_TRANSFER_KEEPON;
}


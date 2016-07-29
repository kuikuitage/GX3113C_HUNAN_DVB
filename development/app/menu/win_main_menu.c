/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_main_menu.c
* Author    : 	lijq
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
#include "win_main_menu.h"
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
#endif

static int g_mainmenubutton = BUTTON_TV;
int MainMenuIndex = 0;

#ifdef APP_HD
char* WIDGET_MAIN_BUTTON[BUTTON_MAIN_END] = {
														"win_main_menu_tv_list_button",
														"win_main_menu_radio_list_button",
														"win_main_menu_week_epg_list_button",
														"win_main_menu_nvod_button",
														"win_main_menu_media_button",
														"win_main_menu_system_set_button"
									};

char* WIDGET_MAIN_UNFOCUS_IMG_CH[BUTTON_MAIN_END] = {
														"main_menu_tv_unfocus_button.bmp",
														"main_menu_radio_unfocus_button.bmp",
														"main_menu_week_epg_unfocus_button.bmp",
														"main_menu_nvod_unfocus_button.bmp",
														"main_menu_media_unfocus_button.bmp",
														"main_menu_system_unfocus_button.bmp"
									};

char* WIDGET_MAIN_UNFOCUS_IMG_EN[BUTTON_MAIN_END] = {
														"menu_tvlist_unfocus_e.bmp",
														"menu_radiolist_unfocus_e.bmp",
														"menu_epg_unfocus_e.bmp",
														"menu_video_unfocus_e.bmp",
														"menu_media_unfocus_e.bmp",
														"menu_set_unfocus_e.bmp"
									};


char* WIDGET_MAIN_FOCUS_IMG_CH[BUTTON_MAIN_END] = {
														"main_menu_tv_focus_button.bmp",
														"main_menu_radio_focus_button.bmp",
														"main_menu_week_epg_focus_button.bmp",
														"main_menu_nvod_focus_button.bmp",
														"main_menu_media_focus_button.bmp",
														"main_menu_system_focus_button.bmp"
									};

char* WIDGET_MAIN_FOCUS_IMG_EN[BUTTON_MAIN_END] = {
														"menu_tvlist_focus_e.bmp",
														"menu_radiolist_focus_e.bmp",
														"menu_epg_focus_e.bmp",
														"menu_video_focus_e.bmp",
														"menu_media_focus_e.bmp",
														"menu_set_focus_e.bmp"
									};

char** WIDGET_MAIN_FOCUS_IMG = WIDGET_MAIN_FOCUS_IMG_CH;
char** WIDGET_MAIN_UNFOCUS_IMG = WIDGET_MAIN_UNFOCUS_IMG_CH;

int win_main_menu_set_img_list(void)
{
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
        WIDGET_MAIN_FOCUS_IMG = WIDGET_MAIN_FOCUS_IMG_CH;
        WIDGET_MAIN_UNFOCUS_IMG = WIDGET_MAIN_UNFOCUS_IMG_CH;        
    }
    else 
    {        
        WIDGET_MAIN_FOCUS_IMG = WIDGET_MAIN_FOCUS_IMG_EN;
        WIDGET_MAIN_UNFOCUS_IMG = WIDGET_MAIN_UNFOCUS_IMG_EN;        
    }	
	return 0;
}
#endif





int win_main_menu_change_button(int oldbuttonindex , int newbuttonindex)
{
	/*
	* 主菜单未使用控件，控制选中button实现，改变选中、未选中图标
	*/
	#ifdef APP_HD
	GUI_SetProperty(WIDGET_MAIN_BUTTON[oldbuttonindex], "img", WIDGET_MAIN_UNFOCUS_IMG[oldbuttonindex]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[oldbuttonindex], "state", "show");

	GUI_SetProperty(WIDGET_MAIN_BUTTON[newbuttonindex], "img", WIDGET_MAIN_FOCUS_IMG[newbuttonindex]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[newbuttonindex], "state", "show");
	#endif

	g_mainmenubutton = newbuttonindex;
	return 0;
	
}

SIGNAL_HANDLER int win_main_menu_create(const char* widgetname, void *usrdata)
{
#ifdef APP_SD
	//app_play_reset_play_timer(0);
	MainMenuIndex = 0;
	GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
    	GUI_SetProperty("win_main_menu_title", "img", "DS_Title_Menu.bmp");

    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
    	GUI_SetProperty("win_main_menu_title", "img", "DS_Title_Menu_Eng.bmp");
    }


	//	GUI_SetProperty(WIDGET_MAIN_FOCUS[BUTTON_TV], "state", "focus");

//	GUI_SetProperty(WIDGET_MAIN_IMAGE_FOCUS[BUTTON_TV],"img",WIDGET_MAIN_FOCUS_IMG[BUTTON_TV]);
//	GUI_SetProperty(WIDGET_MAIN_IMAGE_FOCUS[BUTTON_TV], "state", "show");
#endif
#ifdef APP_HD
		char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
    	GUI_SetProperty("window_main_menu_image_title", "img", "title_menu.bmp");
    	GUI_SetProperty("win_main_menu_tip_image_exit", "img", "tips_exit.bmp");
    	GUI_SetProperty("win_main_menu_tip_image_ok", "img", "tips_confirm.bmp");

        WIDGET_MAIN_FOCUS_IMG = WIDGET_MAIN_FOCUS_IMG_CH;
        WIDGET_MAIN_UNFOCUS_IMG = WIDGET_MAIN_UNFOCUS_IMG_CH;        
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
    	GUI_SetProperty("window_main_menu_image_title", "img", "title_menu_e.bmp");
    	GUI_SetProperty("win_main_menu_tip_image_exit", "img", "tips_exit_e.bmp");
    	GUI_SetProperty("win_main_menu_tip_image_ok", "img", "tips_confirm_e2.bmp");
        
        WIDGET_MAIN_FOCUS_IMG = WIDGET_MAIN_FOCUS_IMG_EN;
        WIDGET_MAIN_UNFOCUS_IMG = WIDGET_MAIN_UNFOCUS_IMG_EN;        
    }

	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_TV], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_TV]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_RADIO], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_RADIO]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_EPG], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_EPG]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_NVOD], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_NVOD]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_MEDIA], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_MEDIA]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_SYSTEM_SET], "img", WIDGET_MAIN_UNFOCUS_IMG[BUTTON_SYSTEM_SET]);



	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_TV], "img", WIDGET_MAIN_FOCUS_IMG[BUTTON_TV]);
	GUI_SetProperty(WIDGET_MAIN_BUTTON[BUTTON_TV], "state", "show");
	GUI_SetProperty("win_main_menu_time_text", "string", app_win_get_local_date_string());

#endif
	g_mainmenubutton = BUTTON_TV;
	app_win_set_focus_video_window(MAIN_MENU_WIN);
	
	app_set_win_create_flag(MAIN_MENU_WIN);
#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_menu_pic_display("win_main_menu_image_ad");
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_show_pic("win_main_menu_image_ad",M_AD_PLACE_ID_MAIN_MUNU);
#endif
	return 0;
}
int speakerStateFlag = 0;
SIGNAL_HANDLER  int win_main_menu_destroy(const char* widgetname, void *usrdata)
{
	
	app_win_set_focus_video_window(FULL_SCREEN_WIN);
	app_set_win_destroy_flag(MAIN_MENU_WIN);
#ifdef DVB_AD_TYPE_DS_FLAG
    app_ds_ad_hide_menu_pic();
#endif

#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_main_menu_ad_gif");
#endif
	GUI_SetInterface("flush", NULL);

	app_play_video_window_full();
	speakerStateFlag = 0;
	return 0;
}

SIGNAL_HANDLER int win_main_menu_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_0:
				if(2 == speakerStateFlag  )
				{
					speakerStateFlag = 3;
				}else if( 1 == speakerStateFlag )
				{
					speakerStateFlag = 2;
				}else{
					speakerStateFlag = 0;
				}
				break;
			case KEY_1:
				if( 0 == speakerStateFlag )
				{
					speakerStateFlag = 1;
				}
				else if( 3 == speakerStateFlag )
				{
					app_panel_set_gpio_low(61);
					speakerStateFlag = 0;
					printf("[speaker] close\n");
				}
				else
				{
					speakerStateFlag = 0;
				}	

				break;
			case KEY_3:
				if(3 == speakerStateFlag )
				{
					app_panel_set_gpio_high(61);
					speakerStateFlag = 0;
					printf("[speaker] open\n");
				}
				else if( 0 == speakerStateFlag )
				{
					speakerStateFlag = 1;
				}
				else
				{
					speakerStateFlag = 0;
				}
				break;
			case KEY_7:
				if( 1 == speakerStateFlag )
				{
					speakerStateFlag = 2;
				}
				else if( 2== speakerStateFlag)
				{
					speakerStateFlag = 3;
				}
				else
				{
					speakerStateFlag = 0;
				}
				break;	
			case KEY_UP:
				speakerStateFlag = 0;
				if (BUTTON_TV == g_mainmenubutton)
				{
					win_main_menu_change_button(BUTTON_TV,BUTTON_SYSTEM_SET);
				}
				else
				{
					win_main_menu_change_button(g_mainmenubutton,g_mainmenubutton-1);
				}
				return EVENT_TRANSFER_STOP;			
			case KEY_DOWN:
				speakerStateFlag = 0;
				if (BUTTON_SYSTEM_SET == g_mainmenubutton)
				{
					win_main_menu_change_button(BUTTON_SYSTEM_SET,BUTTON_TV);

				}
				else
				{
					win_main_menu_change_button(g_mainmenubutton,g_mainmenubutton+1);
				}
				return EVENT_TRANSFER_STOP;		
			case KEY_LEFT:
				speakerStateFlag = 0;
				switch(g_mainmenubutton)
				{
					case BUTTON_TV:
						win_main_menu_change_button(BUTTON_TV,BUTTON_SYSTEM_SET);
						break;
					case BUTTON_RADIO:
						win_main_menu_change_button(BUTTON_RADIO,BUTTON_NVOD);
						break;
					case BUTTON_EPG:
						win_main_menu_change_button(BUTTON_EPG,BUTTON_MEDIA);
						break;
					case BUTTON_NVOD:
						win_main_menu_change_button(BUTTON_NVOD,BUTTON_TV);
						break;
					case BUTTON_MEDIA:
						win_main_menu_change_button(BUTTON_MEDIA,BUTTON_RADIO);
						break;
					case BUTTON_SYSTEM_SET:
						win_main_menu_change_button(BUTTON_SYSTEM_SET,BUTTON_EPG);
						break;
					default:
						break;

				}
				return EVENT_TRANSFER_STOP;					
			case KEY_RIGHT:
			speakerStateFlag = 0;
				switch(g_mainmenubutton)
				{
					case BUTTON_TV:
						win_main_menu_change_button(BUTTON_TV,BUTTON_NVOD);
						break;
					case BUTTON_RADIO:
						win_main_menu_change_button(BUTTON_RADIO,BUTTON_MEDIA);
						break;
					case BUTTON_EPG:
						win_main_menu_change_button(BUTTON_EPG,BUTTON_SYSTEM_SET);
						break;
					case BUTTON_NVOD:
						win_main_menu_change_button(BUTTON_NVOD,BUTTON_RADIO);
						break;
					case BUTTON_MEDIA:
						win_main_menu_change_button(BUTTON_MEDIA,BUTTON_EPG);
						break;
					case BUTTON_SYSTEM_SET:
						win_main_menu_change_button(BUTTON_SYSTEM_SET,BUTTON_TV);
						break;
					default:
						break;

				}
				return EVENT_TRANSFER_STOP;					

			case KEY_OK:
				speakerStateFlag = 0;
#ifdef DVB_AD_TYPE_DS_FLAG
			//app_ds_ad_hide_menu_pic();
#endif
				switch(g_mainmenubutton)
				{
					case BUTTON_TV:
						if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0))
						{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Tv Program!",POPMSG_TYPE_OK);

						}
						else
						{
							if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
							{
								app_play_switch_tv_radio();
							}

							app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0);

								app_play_stop();
							app_play_set_zoom_para(84, 140, 250, 186);
							GUI_CreateDialog("win_menu_tv_list");
							GUI_SetInterface("flush", NULL);


								if (FALSE == app_play_get_play_status())
								{
									app_play_switch_prog_clear_msg();	
									app_play_reset_play_timer(0);
								}
						}
						return EVENT_TRANSFER_STOP;		
					case BUTTON_RADIO:
						if (0 ==  app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0))
						{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Radio Program!",POPMSG_TYPE_OK);

						}
						else
						{
							if (GXBUS_PM_PROG_TV == app_prog_get_stream_type())
							{
								app_play_switch_tv_radio();
								/*
								 * 电视广播切换，进入广播节目前，先设置视频缩放大小
								 */
								app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);
								app_play_stop();

#ifdef APP_SD
								app_play_set_zoom_para(84, 140, 250, 186);
#endif

								GUI_CreateDialog("win_menu_tv_list");
								GUI_SetInterface("flush", NULL);
							}
							else
							{
								app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);	
								app_play_stop();
								app_play_set_zoom_para(84, 140, 250, 186);

								GUI_CreateDialog("win_menu_tv_list");
								GUI_SetInterface("flush", NULL);
							}
								if (FALSE == app_play_get_play_status())
								{
									app_play_switch_prog_clear_msg();	
									app_play_reset_play_timer(0);
								}

						}

						return EVENT_TRANSFER_STOP;	
					case BUTTON_EPG:
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
#ifdef APP_SD
						app_play_stop();
						app_play_set_zoom_para(64, 116, 220, 150);
#endif

						GUI_CreateDialog("win_epg_list");
						GUI_SetInterface("flush", NULL);

						if (FALSE == app_play_get_play_status())
						{
							app_play_switch_prog_clear_msg();	
							app_play_reset_play_timer(0);
						}
						return EVENT_TRANSFER_STOP;	
					case BUTTON_NVOD:
#ifdef APP_SD
						//		app_popmsg(210,150,"Can't Support!",POPMSG_TYPE_OK);
						app_play_stop();
						app_win_set_focus_video_window(MAX_WINDOW_NUM);
						GUI_CreateDialog("win_prog_menu");
#endif
#ifdef APP_HD
						app_popmsg(340,235,"Can't Support!",POPMSG_TYPE_OK);
#endif		
						//						GUI_CreateDialog("win_menu_nvod");
						return EVENT_TRANSFER_STOP;	
					case BUTTON_MEDIA:
						/*
						 * 进入media界面，停止音视频播放
						 */
						{
				//	app_popmsg(210,180,"Can't Support!",POPMSG_TYPE_OK);
					extern void app_create_media_centre_menu(void);
							app_create_media_centre_menu();
						}
						return EVENT_TRANSFER_STOP;	
					case BUTTON_SYSTEM_SET:
						/*
						 * 进入系统信息界面，停止音视频播放
						 */

						app_play_stop();
						app_win_set_focus_video_window(MAX_WINDOW_NUM);
						GUI_CreateDialog("win_sys_set");
						return EVENT_TRANSFER_STOP;	
					default:
						break;

				}			
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL:
			case KEY_EXIT:
			case KEY_MENU:			
				GUI_EndDialog("win_main_menu");
			app_play_stop();
			GUI_SetInterface("flush", NULL);
			app_play_video_window_full();
			
			app_play_reset_play_timer(0);
				return EVENT_TRANSFER_STOP;			
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int win_main_menu_show(const char* widgetname, void *usrdata)
{
	//GUI_SetInterface("flush", NULL);
#ifdef APP_SD
	//GUI_SetInterface("video_top", NULL);
#endif

	return EVENT_TRANSFER_KEEPON;

}

SIGNAL_HANDLER int app_main_menu_boxr_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	static GxBusPmDataProg prog;

	event = (GUI_Event *)usrdata;

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
					return EVENT_TRANSFER_KEEPON;
				case KEY_RIGHT:
					MainMenuIndex++;
					if (MainMenuIndex > 2)
					{
						MainMenuIndex = 0;
					}
					g_mainmenubutton = MainMenuIndex;
					GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
					GUI_SetFocusWidget("win_main_menu_box");
					return EVENT_TRANSFER_STOP;
				case KEY_LEFT:
					g_mainmenubutton = MainMenuIndex;
					GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
					GUI_SetFocusWidget("win_main_menu_box");
					return EVENT_TRANSFER_STOP;
				case KEY_UP:
					MainMenuIndex--;
					if (MainMenuIndex < 0)
					{
						MainMenuIndex = 2;
						g_mainmenubutton = MainMenuIndex;
						GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
						GUI_SetFocusWidget("win_main_menu_box");
					}else
					{	
						g_mainmenubutton = MainMenuIndex + 3;
						GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
					}	
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					MainMenuIndex++;
					if (MainMenuIndex > 2)
					{
						MainMenuIndex = 0;
						g_mainmenubutton = MainMenuIndex;
						GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
						GUI_SetFocusWidget("win_main_menu_box");
					}else
					{	
						GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
						g_mainmenubutton = MainMenuIndex+3;
					}

					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					return EVENT_TRANSFER_KEEPON;
				default:
					return EVENT_TRANSFER_KEEPON;
			}

		default:
			return EVENT_TRANSFER_KEEPON;
	}
	return EVENT_TRANSFER_KEEPON;
}
SIGNAL_HANDLER int app_main_menu_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	static GxBusPmDataProg prog;

	event = (GUI_Event *)usrdata;

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
					return EVENT_TRANSFER_KEEPON;
				case KEY_LEFT:
					MainMenuIndex--;
					if (MainMenuIndex <0)
					{
						MainMenuIndex = 2;
					}
					g_mainmenubutton = MainMenuIndex+3;
					GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
					GUI_SetFocusWidget("win_main_menu_boxr");
					return EVENT_TRANSFER_STOP;
				case KEY_RIGHT:
					g_mainmenubutton = MainMenuIndex+3;
					GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
					GUI_SetFocusWidget("win_main_menu_boxr");
					return EVENT_TRANSFER_STOP;
				case KEY_UP:
					MainMenuIndex--;
					if (MainMenuIndex <0)
					{
						MainMenuIndex = 2;
						g_mainmenubutton = MainMenuIndex+3;
						GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
						GUI_SetFocusWidget("win_main_menu_boxr");
					}else
					{
						GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
						g_mainmenubutton = MainMenuIndex;
					}
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					MainMenuIndex++;
					if (MainMenuIndex > 2)
					{
						MainMenuIndex = 0;
						g_mainmenubutton = MainMenuIndex+3;
						GUI_SetProperty("win_main_menu_boxr", "select", &MainMenuIndex);
						GUI_SetFocusWidget("win_main_menu_boxr");
					}else{
						GUI_SetProperty("win_main_menu_box", "select", &MainMenuIndex);
						g_mainmenubutton = MainMenuIndex;
					}
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					return EVENT_TRANSFER_KEEPON;
				default:
					return EVENT_TRANSFER_KEEPON;
			}
		default:
			return EVENT_TRANSFER_KEEPON;
	}
	return EVENT_TRANSFER_KEEPON;
}




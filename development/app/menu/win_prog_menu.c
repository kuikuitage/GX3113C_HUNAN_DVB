#include "app.h"
	
SIGNAL_HANDLER int app_prog_menu_create(const char* widgetname, void *usrdata)
{

	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
	{ 
	//	GUI_SetProperty("win_prog_menu_image_title","img", "DS_TITLE_PROGRAM_MANAGE.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{  
	//	GUI_SetProperty("win_prog_menu_image_title","img", "DS_TITLE_PROGRAM_MANAGE_ENG.bmp ");
	}
	return 0;
}
SIGNAL_HANDLER int app_prog_menu_keypress(const char* widgetname, void *usrdata)
{ 
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
 	{
		switch(event->key.sym)
 		{
			case KEY_RECALL:
			case KEY_MENU:	
			case KEY_EXIT:
				app_win_set_focus_video_window(MAIN_MENU_WIN);
				app_play_switch_prog_clear_msg();
				GUI_EndDialog("win_prog_menu");
				app_play_reset_play_timer(0);
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}
SIGNAL_HANDLER  int app_prog_menu_destroy(const char* widgetname, void *usrdata)
{ 
	app_win_set_focus_video_window(MAIN_MENU_WIN);
#ifdef APP_SD
	app_play_video_window_zoom(192, 152, 348, 240);
#endif

	return 0;	
}
SIGNAL_HANDLER int app_prog_menu_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int i =0;
	popmsg_ret ret;
	event = (GUI_Event *)usrdata;
	char language[LANGUAGE_NAME_MAX+1]={0};
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_UP:
					GUI_GetProperty("win_prog_menu_box", "select", &i);
					i--;
					if (i <0)
					{ 
						i = 3;
					}
					GUI_SetProperty("win_prog_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_prog_menu_box", "select", &i);
					i++;
					if (i > 3)
					{
						i = 0;
					}  
					GUI_SetProperty("win_prog_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_prog_menu_box", "select", &i);
					switch(i)
					{  
						case 0:
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
								//	app_play_video_window_zoom(430, 40, 240, 180);
#endif
									GUI_CreateDialog("win_prog_manage");
									app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
									GUI_SetInterface("flush", NULL);
								}
								else
								{
									GxBus_PmViewInfoModify(&sysinfo_old);
									app_prog_update_num_in_group();	
#ifdef APP_SD	
									app_popmsg(210,180,"No Tv Program!",POPMSG_TYPE_OK);
#endif
								}
							}
							break;
						case 1:
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
								//	app_play_video_window_zoom(430, 40, 240, 180);
#endif
									GUI_CreateDialog("win_prog_manage");
									app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
									GUI_SetInterface("flush", NULL);
								}
								else
								{
									GxBus_PmViewInfoModify(&sysinfo_old);
									app_prog_update_num_in_group();		
#ifdef APP_SD
									app_popmsg(210,180,"No Radio Program!",POPMSG_TYPE_OK);
#endif
								}
							}
							break;
						case 2:
							{
								if (0 !=  app_prog_check_group_num(GROUP_MODE_FAV,app_prog_get_stream_type(),1))
								{
#ifdef APP_SD
									app_play_video_window_zoom(99,140,248,186);
#endif
									GUI_CreateDialog("win_favorite_prog_list");
									GUI_SetInterface("flush", NULL);
								}
								else
								{
#ifdef APP_SD
									app_popmsg(210,180,"No Favorite Program!",POPMSG_TYPE_OK);
#endif
								}
							}
							break;
						case 3:
							GUI_CreateDialog("win_book_manage");
							break;
	
					}
					return EVENT_TRANSFER_KEEPON;
				default:
					return EVENT_TRANSFER_KEEPON;
			}

		default:
			return EVENT_TRANSFER_KEEPON;
	}
	return EVENT_TRANSFER_KEEPON;
} 



#include "app.h"
#include "win_main_menu.h"

SIGNAL_HANDLER int app_advanced_menu_create(const char* widgetname, void *usrdata)
{

	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
	{
//		GUI_SetProperty("win_system_set_image229","img", "DS_TITLE_ADV_SET.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
//		GUI_SetProperty("win_system_set_image229","img", "DS_TITLE_ADV_SET_ENG.bmp");
	}
	return 0;
}
SIGNAL_HANDLER int app_advanced_menu_keypress(const char* widgetname, void *usrdata)
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
				GUI_EndDialog("win_advanced_set_menu");
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}
SIGNAL_HANDLER  int app_advanced_menu_destroy(const char* widgetname, void *usrdata)
{
	app_win_set_focus_video_window(SYS_SET_MENU_WIN);
	app_set_win_destroy_flag(SYS_SET_MENU_WIN);
	return 0;	
}
SIGNAL_HANDLER int app_advanced_menu_box_keypress(const char* widgetname, void *usrdata)
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
					GUI_GetProperty("win_system_set_box", "select", &i);
					i--;
					if (i <0)
					{
						i = 2;
					}
					GUI_SetProperty("win_system_set_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_system_set_box", "select", &i);
					i++;
					if (i > 2)
					{
						i = 0;
					}
					GUI_SetProperty("win_system_set_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_system_set_box", "select", &i);
					switch(i)
					{
						case 0:
							GUI_CreateDialog("win_sys_update");
							break;
						case 1:
							GUI_CreateDialog("win_passwd_set");
							break;
						case 2:
							memcpy(language,app_flash_get_config_osd_language(),LANGUAGE_NAME_MAX);
#if MEDIA_SUBTITLE_SUPPORT
							app_subt_pause();
#endif
							ret =app_win_reset_factory();
							if(ret == 1)
								return EVENT_TRANSFER_STOP;
							if (0 == strcmp(language,LANGUAGE_ENGLISH))
							{
								GUI_SetProperty("win_system_set_image229","img", "DS_TITLE_ADV_SET.bmp");
								GUI_SetProperty("win_sys_set_factory_text", "draw_now", NULL);
								GUI_SetProperty("win_advanced_set_menu_image_bg","state","hide");
								GUI_SetProperty("win_advanced_set_menu_image_warning","state","hide");
								GUI_SetProperty("win_advanced_set_menu", "update", NULL);//update
								GxCore_ThreadDelay(1000);	
								GUI_SetProperty("win_advanced_set_menu_image_bg","state","hide");
								GUI_SetProperty("win_advanced_set_menu_image_warning","state","hide");
								GUI_SetProperty("win_sys_set_factory_text","state","hide");
							}
							else
							{
								GUI_SetProperty("win_sys_set_factory_text", "draw_now", NULL);
								GUI_SetProperty("win_advanced_set_menu_image_warning","state","hide");
								GUI_SetProperty("win_advanced_set_menu_image_bg","state","hide");
								GUI_SetProperty("win_advanced_set_menu", "update", NULL);//update
								GxCore_ThreadDelay(1000);	
								GUI_SetProperty("win_advanced_set_menu_image_bg","state","hide");
								GUI_SetProperty("win_advanced_set_menu_image_warning","state","hide");
								GUI_SetProperty("win_sys_set_factory_text","state","hide");
															}
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



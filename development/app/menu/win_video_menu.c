#include "app.h"
#include "win_main_menu.h"

SIGNAL_HANDLER int app_video_menu_create(const char* widgetname, void *usrdata)
{ 

	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
 	{ 
		GUI_SetProperty("win_video_menu_image_title","img", "DS_TITLE_DISPLAY_SET.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{ 
		GUI_SetProperty("win_video_menu_image_title","img", "DS_TITLE_DISPLAY_SET_ENG.bmp");
	}
	return 0;
}
SIGNAL_HANDLER int app_video_menu_keypress(const char* widgetname, void *usrdata)
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
				GUI_EndDialog("win_video_menu");
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}
SIGNAL_HANDLER  int app_video_menu_destroy(const char* widgetname, void *usrdata)
{  
	return 0;	
}
SIGNAL_HANDLER int app_video_menu_box_keypress(const char* widgetname, void *usrdata)
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
					GUI_GetProperty("win_video_menu_box", "select", &i);
					i--;
					if (i <0)
					{  
						i = 2;
					}
					GUI_SetProperty("win_video_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_video_menu_box", "select", &i);
					i++;
					if (i > 2)
					{ 
						i = 0;
					} 
					GUI_SetProperty("win_video_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_video_menu_box", "select", &i);
					switch(i)
					{  
						case 0:
							GUI_CreateDialog("win_audio_set");
							break;
						case 1:
							GUI_CreateDialog("win_video_set");
							break;
						case 2:
							GUI_CreateDialog("win_videocolor_set");
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



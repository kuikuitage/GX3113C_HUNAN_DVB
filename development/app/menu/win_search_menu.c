#include "app.h"
#include "win_main_menu.h"

SIGNAL_HANDLER int app_search_menu_create(const char* widgetname, void *usrdata)
{

	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE)) 
	{ 
	//	GUI_SetProperty("win_search_menu_image229","img", "DS_Title_Prog_Search.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{  
	//	GUI_SetProperty("win_search_menu_image229","img", "DS_Title_Prog_Search_ENG.bmp");
	}
	return 0;
}
SIGNAL_HANDLER int app_search_menu_keypress(const char* widgetname, void *usrdata)
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
				GUI_EndDialog("win_search_menu");
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}
SIGNAL_HANDLER  int app_search_menu_destroy(const char* widgetname, void *usrdata)
{ 
	return 0;	
}
SIGNAL_HANDLER int app_search_menu_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int i =0;
	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case KEY_UP:
					GUI_GetProperty("win_search_menu_box", "select", &i);
					i--;
					if (i <0)
					{ 
						i = 3;
					}
					GUI_SetProperty("win_search_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_DOWN:
					GUI_GetProperty("win_search_menu_box", "select", &i);
					i++;
					if (i > 3)
					{
						i = 0;
					} 
					GUI_SetProperty("win_search_menu_box", "select", &i);
					return EVENT_TRANSFER_STOP;
				case KEY_OK:
					GUI_GetProperty("win_search_menu_box", "select", &i);
					switch(i)
					{ 
						case 0:
							app_win_auto_search();
							break;
						case 1:
							GUI_CreateDialog("win_manual_search");
							break;
						case 2:
							GUI_CreateDialog("win_all_search");
							break;
						case 3:
							GUI_CreateDialog("win_main_frequecy_set");
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



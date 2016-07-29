#include "app.h"
#include "down_view.h"
#include "down_sitecfg.h"
#include "down_prog.h"
#include "play_manage.h"

//resourse
#define IMG_CHANNEL				"MP_ICON_FILE.bmp"
#define IMG_PROG				"MP_ICON_MOVICE.bmp"

//widget
#define WIN_DOWN_VIEW			"win_down_view"
#define TEXT_NAME				"down_view_text_name"
#define IMAGE_COVER				"down_view_image_cover"
#define TEXT_DESC				"down_view_text_desc"
#define TEXT_LINK				"down_view_text_link"
#define TEXT_VIEW_CLASS			"down_view_text_class"
#define LISTVIEW_DOWN_VIEW		"down_view_listview"



typedef enum
{
	DOWN_VIEW_MODE_SITE,
	DOWN_VIEW_MODE_CHANNEL,
	DOWN_VIEW_MODE_PROGRAM
}down_view_mode;

static down_view_mode down_view_mode_visit = DOWN_VIEW_MODE_SITE;


SIGNAL_HANDLER int down_view_listview_init(const char* widgetname, void *usrdata)
{
	down_view_mode_visit = DOWN_VIEW_MODE_SITE;
	return 0;
}


SIGNAL_HANDLER int down_view_listview_get_count(const char* widgetname, void *usrdata)
{
	size_t count = 0;
	
	if(DOWN_VIEW_MODE_SITE == down_view_mode_visit)
	{
		down_get_count("sitecfg", "site", &count);
	}
	else if(DOWN_VIEW_MODE_CHANNEL == down_view_mode_visit)
	{
		down_get_count("sitecfg", "channel", &count);
	}
	else  if(DOWN_VIEW_MODE_PROGRAM == down_view_mode_visit)
	{
		down_get_count("program", NULL, &count);
	}
	
	return (int)count;
}

SIGNAL_HANDLER int down_view_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	int32_t index = 0;
	site_view* browser_site = NULL;
	channel_view* browser_channel = NULL;
	down_item_program* browser_program = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item)
	{
		return GXCORE_ERROR;
	}


	index = (int32_t)item->sel;
	if(DOWN_VIEW_MODE_SITE == down_view_mode_visit)
	{
		/*get site*/
		down_get_item("sitecfg", "site", index, &browser_site);
		if(NULL == browser_site)
		{
			return GXCORE_ERROR;
		}

		/*col-0*/
		item->image = (char*)browser_site->logo;
		item->string = NULL;

		/*col-1*/
		item = item->next;
		if(NULL == item)
		{
			return GXCORE_ERROR;
		}
		item->x_offset = 0;
		item->image = NULL;	
		item->string = (char*)browser_site->name;
		
	}
	else if(DOWN_VIEW_MODE_CHANNEL == down_view_mode_visit)
	{
		/*get channel*/
		down_get_item("sitecfg", "channel", index, &browser_channel);
		if(NULL == browser_channel)
		{
			return GXCORE_ERROR;
		}


		/*col-0*/
		item->image = IMG_CHANNEL;
		item->string = NULL;

		/*col-1*/
		item = item->next;
		if(NULL == item)
		{
			return GXCORE_ERROR;
		}
		item->x_offset = 0;
		item->image = NULL;	
		item->string = (char*)browser_channel->title;	
	}
	else if(DOWN_VIEW_MODE_PROGRAM == down_view_mode_visit)
	{
		static char test[] = {"program"};
		
		/*get channel*/
		down_get_item("program", NULL, index, &browser_program);
		if(NULL == browser_program)
		{
			// TODO: test
			item->next->string = (char*)test;
			return GXCORE_ERROR;
		}
			
		/*col-0*/
		item->image = IMG_PROG;
		item->string = NULL;

		/*col-1*/
		item = item->next;
		if(NULL == item)
		{
			return GXCORE_ERROR;
		}
		item->x_offset = 0;
		item->image = NULL;	
		item->string = (char*)browser_program->title;	
	
	}

	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int down_view_listview_change(const char* widgetname, void *usrdata)
{
	int32_t index = 0;
	
	site_view* browser_site = NULL;
	channel_view* browser_channel = NULL;
	down_item_program* browser_program = NULL;
	//int visit_site_no = 0;
	
	char* title = NULL;
	char* link = NULL;
	char* descriptor = NULL;
	char* empty = {" "};


	GUI_GetProperty(LISTVIEW_DOWN_VIEW, "select", &index);
	
	if(DOWN_VIEW_MODE_SITE == down_view_mode_visit)
	{
		/*get site*/
		down_get_item("sitecfg", "site", index, &browser_site);
		if(NULL == browser_site)
		{
			return GXCORE_ERROR;
		}

		title = (char*)browser_site->name;
		link = (char*)empty;
		descriptor = (char*)empty;
		
	}
	else if(DOWN_VIEW_MODE_CHANNEL == down_view_mode_visit)
	{		
		/*get channel*/
		down_get_item("sitecfg", "channel", index, &browser_channel);
		if(NULL == browser_channel)
		{
			return GXCORE_ERROR;
		}
		
		title = (char*)browser_channel->title;
		link = (char*)browser_channel->link;
		descriptor = (char*)empty;	
	}
	else if(DOWN_VIEW_MODE_PROGRAM == down_view_mode_visit)
	{		
		/*get channel*/
		down_get_item("program", NULL, index, &browser_program);
		if(NULL == browser_program)
		{
			return GXCORE_ERROR;
		}
		
		title = (char*)browser_program->title;
		link = (char*)browser_program->link;
		descriptor = (char*)browser_program->descriptor;	
	}	
	
	GUI_SetProperty(TEXT_NAME, "string", (void*)title);
	GUI_SetProperty(TEXT_LINK, "string", (void*)link);
	GUI_SetProperty(TEXT_DESC, "string", (void*)descriptor);

	return GXCORE_SUCCESS;	
}

static int down_view_ok_key(void)
{
	int sel_no = 0;
	int visit_site_no = 0;
	int visit_channel_no = 0;
	site_view* browser_site = NULL;
	channel_view* browser_channel = NULL;
	down_item_program* browser_program = NULL;

	GUI_GetProperty(LISTVIEW_DOWN_VIEW, "select", &sel_no);

	if(DOWN_VIEW_MODE_SITE == down_view_mode_visit)
	{
		down_view_mode_visit = DOWN_VIEW_MODE_CHANNEL;
		down_set_property("sitecfg", "visit_site_no", &sel_no);

		GUI_SetProperty(LISTVIEW_DOWN_VIEW, "update_all", NULL);
		GUI_SetProperty(TEXT_VIEW_CLASS, "string", (void*)"channel:");
	}
	
	else if(DOWN_VIEW_MODE_CHANNEL == down_view_mode_visit)
	{
		down_view_mode_visit = DOWN_VIEW_MODE_PROGRAM;
		down_set_property("sitecfg", "visit_channel_no", &sel_no);

		down_get_property("sitecfg", "visit_site_no", &visit_site_no);
		visit_channel_no = sel_no;
		
		down_get_item("sitecfg", "site", visit_site_no, &browser_site);
		down_get_item("sitecfg", "channel", visit_channel_no, &browser_channel);
		if(NULL == browser_channel)
		{
			return EVENT_TRANSFER_STOP;
		}
		
		down_view_start("program", 
						(const char*)browser_site->class, 
						(const char*)browser_channel->link);
		
		GUI_SetProperty(LISTVIEW_DOWN_VIEW, "update_all", NULL);
		GUI_SetProperty(TEXT_VIEW_CLASS, "string", (void*)"program:");
	}
	
	else if(DOWN_VIEW_MODE_PROGRAM == down_view_mode_visit)
	{
		down_get_item("program", NULL, sel_no, &browser_program);
		if(NULL == browser_program)
		{
			return GXCORE_ERROR;
		}
		
		//printf("[DOWN] start play program: %s, url: %s\n", browser_program->title, browser_program->link);
		play_av_by_url((const char*)browser_program->link, 0);
	}
		
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int down_view_listview_keypress(const char* widgetname, void *usrdata)
{
	int ret = EVENT_TRANSFER_KEEPON;
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_BACK:
			//down_set_property("program", "release", NULL);
			return EVENT_TRANSFER_KEEPON;
			
		case APPK_LEFT:
			if(DOWN_VIEW_MODE_PROGRAM == down_view_mode_visit)
			{
				down_view_mode_visit = DOWN_VIEW_MODE_CHANNEL;
				GUI_SetProperty(LISTVIEW_DOWN_VIEW, "update_all", NULL);
				GUI_SetProperty(TEXT_VIEW_CLASS, "string", (void*)"channel:");
			}
			else if(DOWN_VIEW_MODE_CHANNEL == down_view_mode_visit)
			{
				down_view_mode_visit = DOWN_VIEW_MODE_SITE;
				GUI_SetProperty(LISTVIEW_DOWN_VIEW, "update_all", NULL);
				GUI_SetProperty(TEXT_VIEW_CLASS, "string", (void*)"site:");
			}
			else if(DOWN_VIEW_MODE_SITE == down_view_mode_visit)
			{
				GUI_EndDialog(WIN_DOWN_VIEW);;
			}
			return EVENT_TRANSFER_STOP;
			break;
			
		case APPK_RIGHT:
		case APPK_OK:
			ret = down_view_ok_key();
			return EVENT_TRANSFER_STOP;
			
			
		default:
			return EVENT_TRANSFER_KEEPON;
	}
	
	return EVENT_TRANSFER_KEEPON;
}





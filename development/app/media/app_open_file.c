#include "app.h"

//resourse
#define IMG_DEV_UNSELECT		"MP_ICON_ARROE_UNSELECT.bmp"
#define IMG_DEV_SELECT			"MP_ICON_ARROE_SELECT.bmp"
#define IMG_DIR					"MP_ICON_FILE.bmp"
#define IMG_MOVIC				"MP_ICON_MOVICE.bmp"
#define IMG_PIC					"MP_ICON_PIC.bmp"
#define IMG_MUSIC				"MP_ICON_MUSIC.bmp"
#define IMG_TEXT				"MP_ICON_TXT.bmp"
#define IMG_UNKNOW				"MP_ICON_UNKNOW.bmp"

//widget
#define WIN_OPEN_FILE			"win_open_file"
#define TEXT_TITLE				"open_file_text_title"
#define TEXT_PATH				"open_file_text_path"
#define LISTVIEW_OPEN_FILE		"open_file_listview"

#define OPEN_FILE_NOT_SELECT	"open file not select"

explorer_para* explorer_open = NULL;
char* open_file_select = NULL;



SIGNAL_HANDLER  int open_file_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	event = (GUI_Event *)usrdata;
	GUI_SendEvent("win_movie_view", event);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int open_file_init(const char* widgetname, void *usrdata)
{
	open_file_select = NULL;
	return 0;
}


SIGNAL_HANDLER int open_file_keypress(const char* widgetname, void *usrdata)
{
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int open_file_listview_get_count(const char* widgetname, void *usrdata)
{
	if(explorer_open)
		return explorer_open->nents;
	
	return 0;
}

SIGNAL_HANDLER int open_file_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	uint32_t index = 0;
	GxDirent* ent = NULL;

	if(NULL == explorer_open) return GXCORE_ERROR;

	item = (ListItemPara*)usrdata;
	if(NULL == item) return GXCORE_ERROR;
	
	index = (uint32_t)item->sel;
	if(index >= explorer_open->nents) return GXCORE_ERROR;

	ent = explorer_open->ents + index;
	if(NULL == ent) return GXCORE_ERROR;

	/*col-0: img*/
	item->x_offset = 0;
	item->string = NULL;
	
	if(GX_FILE_DIRECTORY == ent->ftype)
	{
		item->image = IMG_DIR;
	}
	else// if(GX_FILE_REGULAR == ent->ftype)
	{	
		item->image = IMG_UNKNOW;

	}
		
	/*col-1: file name*/
	item = item->next;
	if(NULL == item) return GXCORE_ERROR;

	item->x_offset = 0;
	item->image = NULL;	
	item->string = ent->fname;

	return GXCORE_SUCCESS;
}

static int open_file_listview_okkey(void)
{
	int32_t index = 0;
	GxDirent* ent = NULL;

	if(NULL == explorer_open) return GXCORE_ERROR;
	if(0 == explorer_open->nents) return GXCORE_ERROR;
	
	GUI_GetProperty(LISTVIEW_OPEN_FILE, "select", &index);
	if(index >= explorer_open->nents) return GXCORE_ERROR;
	
	ent = explorer_open->ents + index;
	if(NULL == ent) return GXCORE_ERROR;
	
	if(GX_FILE_DIRECTORY == ent->ftype)
	{
		explorer_open = explorer_chdir(explorer_open, index);
		GUI_SetProperty(LISTVIEW_OPEN_FILE, "update_all", NULL);
		GUI_SetProperty(TEXT_PATH, "string", explorer_open->path);
	}
	else
	{
		open_file_select = explorer_static_path_strcat(explorer_open->path, ent->fname);
	}

	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int open_file_listview_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;
	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
			
		case APPK_BACK:
		case APPK_MENU:
			open_file_select = OPEN_FILE_NOT_SELECT;
			return EVENT_TRANSFER_STOP;

		/*page down*/
		case APPK_PREVIOUS:
			event->key.sym = GUIK_PAGE_UP;
			return EVENT_TRANSFER_KEEPON;
		/*page up*/
		case APPK_NEXT:
			event->key.sym = GUIK_PAGE_DOWN;
			return EVENT_TRANSFER_KEEPON;

			
		case APPK_LEFT:
			explorer_open = explorer_backdir(explorer_open);
			GUI_SetProperty(LISTVIEW_OPEN_FILE, "update_all", NULL);
			GUI_SetProperty(TEXT_PATH, "string", explorer_open->path);
			return EVENT_TRANSFER_STOP;
			
		case APPK_RIGHT:
			open_file_listview_okkey();
			return EVENT_TRANSFER_STOP;			

		case APPK_OK:
			open_file_listview_okkey();
			return EVENT_TRANSFER_STOP;
						
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}




char* app_open_file(const char* path, const char* suffix)
{	
      explorer_closedir(explorer_open);
	explorer_open = explorer_opendir(path, suffix);
	
	GUI_CreateDialog(WIN_OPEN_FILE);
	GUI_SetProperty(TEXT_PATH, "string", (void*)path);

	app_msg_destroy(g_app_msg_self);
#if 0
	while(NULL == open_file_select)
	{
		GUI_Exec();
		GxCore_ThreadDelay(50);
	}
#endif
	while(NULL == open_file_select)
    {
        GUI_Loop();
        GxCore_ThreadDelay(50);
    }
	GUI_StartSchedule();

	GUI_EndDialog(WIN_OPEN_FILE);
	//GUI_Exec();
	GUI_SetInterface("flush",NULL);
	app_msg_init(g_app_msg_self);


	if(0 == strcmp(OPEN_FILE_NOT_SELECT, open_file_select))
	{
		return NULL;
	}
	
	return open_file_select;
}



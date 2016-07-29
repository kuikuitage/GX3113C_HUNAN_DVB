#include "app.h"

//widget
#define BOOK_WIN                "win_book_manage"        
#define BOOK_LISTVIEW           "win_book_manage_listview"
#define BOOK_TEXT_TIME          "win_book_manage_time_text"


//resourse
#define BOOK_IMAGE_DELETE       "delete.bmp"

//string
#define STR_DEL_CHK         "Do you want to delete this book?"
#define STR_DEL_ALL_CHK     "Do you want to delete all book?"




static GxBookGet EpgBookGet;
static event_list* spApp_BookTimer = NULL;
int gi_Sel = 0;
bool gb_ManualDel = false;

int app_book_GetDelFlag(void)
{
	return gb_ManualDel;
}

int8_t win_book_manage_sync(void)
{
	app_book_init();
	app_book_sync(&EpgBookGet, BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
	return 0;
}

static int timer_book(void *userdata)
{
	char* focus_Window = (char*)GUI_GetFocusWindow();

	if((NULL != focus_Window)&&( strcasecmp(BOOK_WIN, focus_Window) != 0))
	{
		return 0;
	}
	    
	uint32_t sel = 0;
	app_book_sync(&EpgBookGet, BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
	GUI_GetProperty(BOOK_LISTVIEW, "select", &sel);			
	GUI_SetProperty(BOOK_LISTVIEW, "update_all", NULL);		
	GUI_SetProperty(BOOK_LISTVIEW, "select", &sel);
    
	return 0;
}


SIGNAL_HANDLER  int book_manage_service(const char* widgetname, void *usrdata)
{
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int book_manage_create(const char* widgetname, void *usrdata)
{


	gi_Sel = 0;

	GUI_SetProperty(BOOK_TEXT_TIME,"string",app_win_get_local_date_string());
	
	spApp_BookTimer = create_timer(timer_book, 3000, NULL,  TIMER_REPEAT);
	app_book_init();
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
	return 0;
}

SIGNAL_HANDLER int book_manage_destroy(const char* widgetname, void *usrdata)
{
	remove_timer(spApp_BookTimer);
	spApp_BookTimer = NULL;

	gb_ManualDel = false;
	
	return 0;
}

SIGNAL_HANDLER int book_manage_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t sel;
	uint32_t book_id;
	popmsg_ret ret_pop = POPMSG_RET_NO; 	
	GxBookGet pEpgBookGet;
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;
	GUI_GetProperty(BOOK_LISTVIEW, "select", &sel);	
	book_id = EpgBookGet.book[sel].id;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
    	case KEY_EXIT:
    	case KEY_RECALL:
        case KEY_MENU:
	     GUI_EndDialog(BOOK_WIN);
    		return EVENT_TRANSFER_STOP;
            
		case KEY_RED:
				{
					int tempsel = 0;
					
					if(0 == EpgBookGet.book_number)
					{
		                app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Book Program!",POPMSG_TYPE_OK);
						break;
					}				
            		ret_pop = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START, STR_DEL_CHK, POPMSG_TYPE_YESNO);
					if(ret_pop == POPMSG_RET_YES)
					{
						if(-1 == app_book_check_exist(&pEpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1, book_id, &tempsel))
						{
							return EVENT_TRANSFER_STOP;
						}
						
						app_book_delete(&EpgBookGet.book[tempsel]);
						app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
						if ((sel == EpgBookGet.book_number-1)&&(sel >0))
						{
							sel -=1;
						}
						GUI_SetProperty(BOOK_LISTVIEW, "update_all", NULL);		
						GUI_SetProperty(BOOK_LISTVIEW, "select", &sel);	

						gb_ManualDel = true;
					}	
				}
				return EVENT_TRANSFER_STOP;
            break;

		case KEY_YELLOW:
			if(0 == EpgBookGet.book_number)
			{
                app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"No Book Program!",POPMSG_TYPE_OK);
				break;
			}

            ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_DEL_ALL_CHK, POPMSG_TYPE_YESNO);

			if(ret_pop == POPMSG_RET_YES)
			{
				app_book_clear_all();
				app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
				sel = 0;
				GUI_SetProperty(BOOK_LISTVIEW, "update_all", NULL);		
				GUI_SetProperty(BOOK_LISTVIEW, "select", &sel);		

				gb_ManualDel = true;
			}	
			return EVENT_TRANSFER_STOP;
            break;

            
		default:
			break;
	}	

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int book_manage_listview_get_count(const char* widgetname, void *usrdata)
{
	
	return EpgBookGet.book_number;
}

SIGNAL_HANDLER int book_manage_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	uint32_t index = 0;
	static uint8_t buffer_no[40] = {0};
	static uint8_t buffer_start[40] = {0};

	item = (ListItemPara*)usrdata;
	if(NULL == item) return GXCORE_ERROR;
	index = (uint32_t)item->sel;


	book_play *book;
	static GxBusPmDataProg prog;
	book = (book_play*)(EpgBookGet.book[item->sel].struct_buf);
	GxBus_PmProgGetById(book->prog_id, &prog);


    /*prog no*/
	sprintf((void*)buffer_no, "%03d",item->sel+1);
	item->x_offset = 3;
	item->image = NULL;	
	item->string = (char*)buffer_no; 

    /*prog name*/
    item = item->next;
	if(NULL == item) return GXCORE_ERROR;
	item->x_offset = 0;
	item->image = NULL;	
	item->string = (char*)prog.prog_name;

    /*event name*/
    item = item->next;
	if(NULL == item) return GXCORE_ERROR;
	item->x_offset = 0;
	item->image = NULL;	
	item->string = (char*)book->event_name;


    /*start time*/
	int32_t TimeZone;
	struct tm tm_start;
	time_t start_time;
    
	start_time = EpgBookGet.book[item->sel].trigger_time_start;
    	TimeZone = app_flash_get_config_timezone();
	start_time += (TimeZone)*3600;
	memcpy(&tm_start, localtime(&start_time), sizeof(struct tm));
	sprintf((void*)buffer_start,
		"%02d-%02d %02d:%02d",
		tm_start.tm_mon+1,
		tm_start.tm_mday,
		tm_start.tm_hour,
		tm_start.tm_min);


    item = item->next;
	if(NULL == item) return GXCORE_ERROR;
	item->x_offset = 0;
	item->image = NULL;	
	item->string = (char*)buffer_start;

	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int book_manage_listview_change(const char* widgetname, void *usrdata)
{
	uint32_t value = 0;
	
	GUI_GetProperty("win_book_manage_listview","select",(void*)&value);

	gi_Sel = value;
	
	return GXCORE_SUCCESS;	
}

SIGNAL_HANDLER int book_manage_listview_draw(const char* widgetname, void *usrdata)
{
	if (0 == EpgBookGet.book_number)
	{
		return GXCORE_ERROR;
	}
	
	if (gi_Sel == EpgBookGet.book_number)
	{
		gi_Sel = gi_Sel - 1;
	}
	printf("\n------------book_manage_listview_draw----->[%d].\n",gi_Sel);

	GUI_SetProperty("win_book_manage_listview","select",(void*)&gi_Sel);
	return 0;
}

SIGNAL_HANDLER int book_manage_listview_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
    		case KEY_RECALL:
    		case KEY_EXIT:
		case KEY_MENU:
		     GUI_EndDialog(BOOK_WIN);
	    		return EVENT_TRANSFER_STOP;
			
		default:
			return EVENT_TRANSFER_KEEPON;
    			break;
		}
	}

	return EVENT_TRANSFER_STOP;
}








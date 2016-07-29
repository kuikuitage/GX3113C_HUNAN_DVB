#include "app.h"

typedef enum
{
	TMS_OFF,
	TMS_ON
}PvrTmsFlag;

typedef enum
{
	FILE_SIZE_1G    =   1024,
	FILE_SIZE_2G    =   2048,
	FILE_SIZE_4G    =   4096,
}PvrFileSize;

typedef struct
{
	PvrTmsFlag      tms_flag;
    PvrFileSize     file_size;
    uint32_t        duration;   // seconds
}PvrSetPara;

typedef enum
{	
	PVR_MENU_TIMESHIFT_TITLE,
	PVR_MENU_TIMESHIFT_SWITCH,
	PVR_MENU_TS_FILE_SIZE,
	PVR_MENU_RECORD_TITLE,
	PVR_MENU_RECORD_DURATION,
	PVR_MENU_DISK_INFO,
	PVR_MENU_END	
}pvr_menu_setting;

//static PvrSetPara s_PvrSetPara;
#define PVR_FILE_SIZE_KEY "PVRFileSize"
#define PVR_FILE_SIZE 1024


static int app_pvr_set_tms_flag(int32_t tms)
{
    GxBus_ConfigSetInt(PVR_TIMESHIFT_KEY, tms);
    return 0;
}

static int app_pvr_set_file_size(int32_t size)
{

    printf("[app pvr setting]pvr size = %d\n", size);
    GxBus_ConfigSetInt(PVR_FILE_SIZE_KEY, size);

    app_send_msg(GXMSG_PLAYER_PVR_CONFIG, (void*)&size);
    //app_send_msg_exec(GXMSG_PLAYER_PVR_CONFIG,&pvr_cfg);

	return 0;
}

static int app_pvr_set_duration_flag(uint32_t duration)
{
    GxBus_ConfigSetInt(PVR_DURATION_KEY, duration);

    // TODO: re-calculate pvr stop time

    return 0;
}

static void pvr_management_init(void)
{
	int32_t sel1 = 0;
	int32_t sel2 = 0;
	int32_t sel3 = 0;

	GxBus_ConfigGetInt(PVR_TIMESHIFT_KEY, &sel1,PVR_TIMESHIFT_DEFAULT);
	GxBus_ConfigGetInt(PVR_FILE_SIZE_KEY, &sel2,PVR_FILE_SIZE);
	if(sel2 == FILE_SIZE_1G)
	{
		sel2 = 0;
	}
	else if(sel2 == FILE_SIZE_2G)
	{
		sel2 = 1;
	}
	else 
	{
		sel2 = 2;
	}
	GxBus_ConfigGetInt(PVR_DURATION_KEY, &sel3,PVR_DURATION_VALUE);

	GUI_SetProperty("pvr_management_boxitem1_combox", "select", &sel1);
	GUI_SetProperty("pvr_management_boxitem2_combox", "select", &sel2);
	GUI_SetProperty("pvr_management_boxitem3_combox", "select", &sel3);
}

SIGNAL_HANDLER  int pvr_management_create(const char* widgetname, void *usrdata)
{
	pvr_management_init();
	return 0;

}

SIGNAL_HANDLER  int pvr_management_destroy(const char* widgetname, void *usrdata)
{
	return 0;	

}



SIGNAL_HANDLER  int pvr_management_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{	
			case KEY_EXIT:
				GUI_EndDialog("win_pvr_management");
				GUI_EndDialog("win_media_center");
				GUI_EndDialog("win_main_menu");
				app_win_exist_to_full_screen();
//				app_exit_to_full_window();
				return EVENT_TRANSFER_STOP;	
				
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;				
			default:
				return EVENT_TRANSFER_STOP;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int pvr_management_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t selItem = 0;
	int32_t value;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{	
			case KEY_LEFT:
			case KEY_RIGHT:
				GUI_GetProperty("pvr_management_box", "select", &selItem);
				switch(selItem)
				{
					case PVR_MENU_TIMESHIFT_SWITCH:
						GUI_GetProperty("pvr_management_boxitem1_combox", "select", &value);
						app_pvr_set_tms_flag(value);
						return EVENT_TRANSFER_STOP;	

					case PVR_MENU_TS_FILE_SIZE:
						GUI_GetProperty("pvr_management_boxitem2_combox", "select", &value);
						if(value == 0)
							value = FILE_SIZE_1G;
						else if(value == 1)
							value = FILE_SIZE_2G;
						else 
							value = FILE_SIZE_4G;
					   	app_pvr_set_file_size(value);
						return EVENT_TRANSFER_STOP;	

					case PVR_MENU_RECORD_DURATION:
						GUI_GetProperty("pvr_management_boxitem3_combox", "select", &value);
						app_pvr_set_duration_flag(value);
						return EVENT_TRANSFER_STOP;	
					default :
						break;
				}
				return EVENT_TRANSFER_STOP;	
		
			case KEY_EXIT:
				GUI_EndDialog("win_pvr_management");
				GUI_EndDialog("win_media_center");
				GUI_EndDialog("win_main_menu");
				app_win_exist_to_full_screen();
//				app_exit_to_full_window();
				return EVENT_TRANSFER_STOP;	
				
			case KEY_MENU:
				GUI_SetProperty("media_center_boxitem2_text", "state", "enable");
//				app_menu_help_display(MENU_HELP_MAIN_MENU);
				GUI_SetFocusWidget("media_center_box");
				return EVENT_TRANSFER_STOP;

			case KEY_UP:
				GUI_GetProperty("pvr_management_box", "select", &selItem);
				if(selItem == PVR_MENU_TIMESHIFT_SWITCH)
				{
					selItem = PVR_MENU_DISK_INFO;
				}
				else
				{
					selItem--;
					if(selItem == PVR_MENU_RECORD_TITLE)
						selItem--;
				}
				GUI_SetProperty("pvr_management_box", "select", &selItem);
				return EVENT_TRANSFER_STOP;		
				
			case KEY_DOWN:
				GUI_GetProperty("pvr_management_box", "select", &selItem);
				selItem++;
				if(selItem == PVR_MENU_RECORD_TITLE)
					selItem++;
				if (selItem > PVR_MENU_DISK_INFO)
				{
					selItem = PVR_MENU_TIMESHIFT_SWITCH;
				}
				GUI_SetProperty("pvr_management_box", "select", &selItem);
				return EVENT_TRANSFER_STOP;		

			case KEY_OK:
				GUI_GetProperty("pvr_management_box", "select", &selItem);
				if(selItem == PVR_MENU_DISK_INFO)
				{
					GUI_CreateDialog("win_disk_information");
				}
				else
				{
					GUI_SetProperty("media_center_boxitem2_text", "state", "enable");
//					app_menu_help_display(MENU_HELP_MAIN_MENU);
					GUI_SetFocusWidget("media_center_box");
				}
				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}


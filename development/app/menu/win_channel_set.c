#include "app.h"
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#include "desai_ca_api.h"
#endif

#define OSDSET_TEXT_TIME                            "win_channel_set_time_text"
#define OSDSET_BOX                                  "win_channel_set_box"
#define OSDSET_COMBOBOX_MENU_CHANNEL               "win_channel_set_boxitem1_combobox"

static int32_t widget_init_menu_channel = 0;


static int init_menu_channel(void)
{

	int32_t widget_value = 0;
    
    widget_value = app_flash_get_config_gx150x_mode_flag();
        
    GUI_SetProperty(OSDSET_COMBOBOX_MENU_CHANNEL, "select", (void*)&widget_value);

    widget_init_menu_channel = widget_value;
	return 0;
}


static status_t key_exit(void)
{
    int32_t widget_menu_language = 0;    
    GxBusPmDataSat Sat = {0,};
    /*check modified?*/
    GUI_GetProperty(OSDSET_COMBOBOX_MENU_CHANNEL, "select", (void*)&widget_menu_language);

    /*not modified, exit directly*/
    if(widget_menu_language == widget_init_menu_channel)
    {
        GUI_EndDialog("win_channel_set");
	    return GXCORE_SUCCESS;
    }
    
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;
	#ifdef APP_SD
    ret_pop = app_popmsg(210,150, STR_CHK_SAVE, POPMSG_TYPE_YESNO);
	#endif
	#ifdef APP_HD
	ret_pop = app_popmsg(340, 200, STR_CHK_SAVE, POPMSG_TYPE_YESNO);
	#endif
    
    /*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
        app_flash_save_config_gx150x_mode_flag(widget_menu_language);
		GxBus_PmSatsGetByPos(0,1,&Sat);
		Sat.sat_dtmb.work_mode = widget_menu_language;
		GxBus_PmSatModify(&Sat);
		GxBus_PmSync(GXBUS_PM_SYNC_SAT);
    }	
    
    /*not save, reset param*/
    else
    {
        app_flash_save_config_gx150x_mode_flag(widget_init_menu_channel);
    }
	
	return GXCORE_SUCCESS;
}



SIGNAL_HANDLER int win_channel_set_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
    	GUI_SetProperty("win_channel_set_title", "img", "title_language.bmp");
    	GUI_SetProperty("win_channel_set_tip_image_exit", "img", "tips_exit.bmp");
    }
    else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
    	GUI_SetProperty("win_channel_set_title", "img", "title_language_e.bmp");
    	GUI_SetProperty("win_channel_set_tip_image_exit", "img", "tips_exit_e.bmp");
    }


	GUI_SetProperty(OSDSET_TEXT_TIME, "string", app_win_get_local_date_string());
    
	init_menu_channel();

	return 0;
}

SIGNAL_HANDLER int win_channel_set_destroy(const char* widgetname, void *usrdata)
{

	return 0;
}


SIGNAL_HANDLER int win_channel_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
    	case KEY_EXIT:
    	case KEY_RECALL:
        case KEY_MENU:
            key_exit();
	     	GUI_EndDialog("win_channel_set");
    		return EVENT_TRANSFER_STOP;                      
        			
		default:
			break;
	}
	return EVENT_TRANSFER_STOP;
}



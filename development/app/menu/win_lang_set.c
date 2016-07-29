#include "app.h"
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#include "desai_ca_api.h"
#endif

#define OSDSET_TEXT_TIME                            "win_lang_set_time_text"
#define OSDSET_BOX                                  "win_lang_set_box"
#define OSDSET_COMBOBOX_MENU_LANGUAGE               "win_lang_set_boxitem1_combobox"

static int32_t widget_init_menu_language = 0;


static int init_menu_language(void)
{
	char* osd_language=NULL;
	int32_t widget_value = 0;
    
    osd_language = app_flash_get_config_osd_language();


    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
		widget_value = 0;
    else
		widget_value = 1;
        
    GUI_SetProperty(OSDSET_COMBOBOX_MENU_LANGUAGE, "select", (void*)&widget_value);

    widget_init_menu_language = widget_value;
	return 0;
}


static int set_menu_language(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 2;
    int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

    flash_value = widget_value;

    if(flag_set)
    {
	    if(0 == flash_value)
	    {
		    //        	GUI_SetProperty("win_lang_set_title", "img", "title_language.bmp");
		    //      	GUI_SetProperty("win_lang_set_tip_image_exit", "img", "tips_exit.bmp");
	    }
	    else if(1 == flash_value)
	    {
		    //        	GUI_SetProperty("win_lang_set_title", "img", "title_language_e.bmp");
		    //      	GUI_SetProperty("win_lang_set_tip_image_exit", "img", "tips_exit_e.bmp");
	    }

	    GUI_SetProperty("win_lang_set", "update", NULL);//update
    }
    if(flag_save)
    {
	    if (0 == flash_value)
	    {
		    app_flash_save_config_osd_language(LANGUAGE_CHINESE);
	    }
	    else
	    {
		    app_flash_save_config_osd_language(LANGUAGE_ENGLISH);				
	    }
    }

    if (0 == flash_value)
    {

	    if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	    {
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		    DSCA_SetSysLanguage(CA_LANGUAGE_CHINESE);
#endif
	    }
	    app_play_set_osd_language(LANGUAGE_CHINESE);
    }
    else
    {

	    if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	    {
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		    DSCA_SetSysLanguage(CA_LANGUAGE_ENGLISH);
#endif
	    }
	    app_play_set_osd_language(LANGUAGE_ENGLISH);				
    }

    return 0;
}


static status_t key_exit(void)
{
	int32_t widget_menu_language = 0;    

	/*check modified?*/
	GUI_GetProperty(OSDSET_COMBOBOX_MENU_LANGUAGE, "select", (void*)&widget_menu_language);

	/*not modified, exit directly*/
	if(widget_menu_language == widget_init_menu_language)
	{
		GUI_EndDialog("win_lang_set");
		return GXCORE_SUCCESS;
	}

	/*popmsg to save?*/
	popmsg_ret ret_pop = POPMSG_RET_YES;

	ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_CHK_SAVE, POPMSG_TYPE_YESNO);

	/*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
		set_menu_language(widget_menu_language, 0, 1);
	}	

	/*not save, reset param*/
	else
	{
		set_menu_language(widget_init_menu_language, 0, 1);
	}

	return GXCORE_SUCCESS;
}

static status_t key_lr(unsigned short value)
{
	uint32_t item_sel = 0;	
	uint32_t value_sel = 0;

	GUI_GetProperty(OSDSET_BOX, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:{
			       GUI_GetProperty(OSDSET_COMBOBOX_MENU_LANGUAGE, "select", (void*)&value_sel);
			       set_menu_language(value_sel, 1, 0);
			       break;
		       }

		default:
		       break;
	}

	return GXCORE_SUCCESS;
}

static status_t key_ok(void)
{
	uint32_t item_sel = 0;
	//uint32_t value_sel = 0;	

	GUI_GetProperty(OSDSET_BOX, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:{
			       break;
		       }

		case 1:{
			       break;
		       }

		default:
		       break;
	}

	return GXCORE_SUCCESS;
}



SIGNAL_HANDLER  int win_lang_set_service(const char* widgetname, void *usrdata)
{

	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_lang_set_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
		//   	GUI_SetProperty("win_lang_set_title", "img", "title_language.bmp");
		// 	GUI_SetProperty("win_lang_set_tip_image_exit", "img", "tips_exit.bmp");
	}
	else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{ 
		//	GUI_SetProperty("win_lang_set_title", "img", "title_language_e.bmp");
		//	GUI_SetProperty("win_lang_set_tip_image_exit", "img", "tips_exit_e.bmp");
	}


	GUI_SetProperty(OSDSET_TEXT_TIME, "string", app_win_get_local_date_string());

	init_menu_language();

	return 0;
}

SIGNAL_HANDLER int win_lang_set_destroy(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
//		GUI_SetProperty("win_sys_set_title", "img", "title_set.bmp");
//		GUI_SetProperty("win_sys_set_tip_image_exit", "img", "tips_exit.bmp");
//		GUI_SetProperty("win_sys_set_tip_image_ok", "img", "tips_confirm.bmp");
		GUI_SetProperty("win_sys_set", "update", NULL);//update
	}
	else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
//		GUI_SetProperty("win_sys_set_title", "img", "title_set_e.bmp");
//		GUI_SetProperty("win_sys_set_tip_image_exit", "img", "tips_exit_e.bmp");
//		GUI_SetProperty("win_sys_set_tip_image_ok", "img", "tips_confirm_e2.bmp");
		GUI_SetProperty("win_sys_set", "update", NULL);//update
	}
	return 0;
}


SIGNAL_HANDLER int win_lang_set_keypress(const char* widgetname, void *usrdata)
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
			GUI_EndDialog("win_lang_set");
			return EVENT_TRANSFER_STOP;

		case APPK_LEFT:
		case APPK_RIGHT:
			key_lr(event->key.sym);
			break;				

		case APPK_OK:
			key_ok();
			break;


		default:
			break;
	}


	return EVENT_TRANSFER_STOP;
}



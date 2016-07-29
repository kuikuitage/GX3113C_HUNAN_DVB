#include "app.h"


#define OSDSET_TEXT_TIME                            "win_trans_set_time_text"
#define OSDSET_BOX                                  "win_trans_set_box"
#define OSDSET_COMBOBOX_MENU_TRANSPARENCY           "win_trans_set_boxitem2_combobox"

static int32_t widget_init_menu_transparency = 0;
//static uint32_t TransLevelSel[]={0,10,20,30,40,50,60,70,80,90};
static uint32_t TransLevelSel[]={90,80,70,60,50,40,30,20,10,0};



static int init_menu_transparency(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;
    uint32_t i;

    
    flash_value = app_flash_get_config_osd_trans();
	for(i = 0;i<sizeof(TransLevelSel)/sizeof(TransLevelSel[0]);i++)
	{
		if(flash_value == TransLevelSel[i])
		{
			widget_value = i;
		}
	}
    
    GUI_SetProperty(OSDSET_COMBOBOX_MENU_TRANSPARENCY, "select", (void*)&widget_value);

    widget_init_menu_transparency = widget_value;
	return 0;
}


static int set_menu_transparency(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 2;

    flash_value = widget_value;
   
    if(flag_set)
        ;
    if(flag_save)
        app_flash_save_config_osd_trans(TransLevelSel[flash_value]);
    app_play_set_osd_trasn_level(TransLevelSel[flash_value]);
	return 0;
}

static status_t key_exit(void)
{
    int32_t widget_menu_transparency = 0;   
    
    /*check modified?*/
    GUI_GetProperty(OSDSET_COMBOBOX_MENU_TRANSPARENCY, "select", (void*)&widget_menu_transparency);

    /*not modified, exit directly*/
    if(widget_menu_transparency == widget_init_menu_transparency)
    {
	    return GXCORE_SUCCESS;
    }
    
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;

    ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_CHK_SAVE, POPMSG_TYPE_YESNO);

    /*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
        set_menu_transparency(widget_menu_transparency, 0, 1);
	}	
    
    /*not save, reset param*/
    else
    {
        set_menu_transparency(widget_init_menu_transparency, 0, 1);
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
			GUI_GetProperty(OSDSET_COMBOBOX_MENU_TRANSPARENCY, "select", (void*)&value_sel);
            set_menu_transparency(value_sel, 1, 0);
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



SIGNAL_HANDLER  int win_trans_set_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_trans_set_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
   {
 //   	GUI_SetProperty("win_trans_set_title", "img", "title_transparent.bmp");
   // 	GUI_SetProperty("win_trans_set_tip_image_exit", "img", "tips_exit.bmp");
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    { 
    //	GUI_SetProperty("win_trans_set_title", "img", "title_transparent_e.bmp");
    //	GUI_SetProperty("win_trans_set_tip_image_exit", "img", "tips_exit_e.bmp");
    }


	//GUI_SetProperty(OSDSET_TEXT_TIME, "string", app_win_get_local_date_string());
    
	init_menu_transparency();

	return 0;
}

SIGNAL_HANDLER int win_trans_set_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER int win_trans_set_keypress(const char* widgetname, void *usrdata)
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
	     		GUI_EndDialog("win_trans_set");
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



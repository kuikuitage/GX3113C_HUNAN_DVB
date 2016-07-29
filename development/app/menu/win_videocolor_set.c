#include "app.h"


#define VIDEOCOLORSET_BOX			            "win_videocolor_set_box"
#define VIDEOCOLORSET_COMBOBOX_BRIGHTRNESS		"win_videocolor_set_boxitem1_combobox"
#define VIDEOCOLORSET_COMBOBOX_SATURATION       "win_videocolor_set_boxitem3_combobox"
#define VIDEOCOLORSET_COMBOBOX_CONTRAST      	"win_videocolor_set_boxitem4_combobox"
#define VIDEOCOLORSET_TEXT_TIME                  "win_videocolor_set_time_text"

static int32_t widget_init_brightness = 0;
static int32_t widget_init_saturation = 0;
static int32_t widget_init_contrast = 0;
static uint32_t LevelSel[]={10,20,30,40,50,60,70,80,90,100};

static int init_videocolor_brightness(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;
    uint32_t i;

    
    flash_value = app_flash_get_config_videocolor_brightness();
	for(i = 0;i<sizeof(LevelSel)/sizeof(LevelSel[0]);i++)
	{
		if(flash_value == LevelSel[i])
		{
			widget_value = i;
		}
	}
    
    GUI_SetProperty(VIDEOCOLORSET_COMBOBOX_BRIGHTRNESS, "select", (void*)&widget_value);

    widget_init_brightness = widget_value;
	return 0;

}

static int init_videocolor_saturation(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;
    uint32_t i;

    
    flash_value = app_flash_get_config_videocolor_saturation();
	for(i = 0;i<sizeof(LevelSel)/sizeof(LevelSel[0]);i++)
	{
		if(flash_value == LevelSel[i])
		{
			widget_value = i;
		}
	}
    
    GUI_SetProperty(VIDEOCOLORSET_COMBOBOX_SATURATION, "select", (void*)&widget_value);

    widget_init_saturation = widget_value;

	return 0;
}

static int init_videocolor_contrast(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;
    uint32_t i;

    
    flash_value = app_flash_get_config_videocolor_contrast();
	for(i = 0;i<sizeof(LevelSel)/sizeof(LevelSel[0]);i++)
	{
		if(flash_value == LevelSel[i])
		{
			widget_value = i;
		}
	}
    
    GUI_SetProperty(VIDEOCOLORSET_COMBOBOX_CONTRAST, "select", (void*)&widget_value);

    widget_init_contrast = widget_value;

	return 0;
}

static int set_videocolor(VideoColor color, int flag_set, int flag_save)
{
    if(flag_save)
	{
    	app_flash_save_config_videocolor_brightness(color.brightness);
    	app_flash_save_config_videocolor_saturation(color.saturation);
    	app_flash_save_config_videocolor_contrast(color.contrast);

	}

	if (flag_set)
	{
		app_play_set_videoColor_level(color);
	}
	return 0;
}

static status_t key_exit(void)
{
    int32_t widget_brightness = 0;
    int32_t widget_saturation = 0;
    int32_t widget_contrast = 0;
	VideoColor color;

    
    /*check modified?*/
    GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_BRIGHTRNESS, "select",  (void*)&widget_brightness);
    GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_SATURATION, "select",  (void*)&widget_saturation);
    GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_CONTRAST, "select",  (void*)&widget_contrast);

    /*not modified, exit directly*/
    if(widget_brightness == widget_init_brightness
    && widget_saturation == widget_init_saturation
    && widget_contrast == widget_init_contrast)
   {
        return GXCORE_SUCCESS;
    }
    
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;
    ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_CHK_SAVE, POPMSG_TYPE_YESNO);

    /*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
		color.brightness = LevelSel[widget_brightness];
		color.saturation = LevelSel[widget_saturation];
		color.contrast = LevelSel[widget_contrast];
		set_videocolor(color, 0, 1);
	}	
    
    /*not save, reset param*/
    else
    {
		color.brightness = LevelSel[widget_init_brightness];
		color.saturation = LevelSel[widget_init_saturation];
		color.contrast = LevelSel[widget_init_contrast];
		set_videocolor(color, 1, 0);
    }
    
	return GXCORE_SUCCESS;
}

static status_t key_lr(unsigned short value)
{
    int32_t widget_brightness = 0;
    int32_t widget_saturation = 0;
    int32_t widget_contrast = 0;
	VideoColor color;

	GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_BRIGHTRNESS, "select",  (void*)&widget_brightness);
	GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_SATURATION, "select",  (void*)&widget_saturation);
	GUI_GetProperty(VIDEOCOLORSET_COMBOBOX_CONTRAST, "select",  (void*)&widget_contrast);



	color.brightness = LevelSel[widget_brightness];
	color.saturation = LevelSel[widget_saturation];
	color.contrast = LevelSel[widget_contrast];
	set_videocolor(color, 1, 0);
		
	return GXCORE_SUCCESS;
}

static status_t key_ok(void)
{
	uint32_t item_sel = 0;
	//uint32_t value_sel = 0;	
	
	GUI_GetProperty(VIDEOCOLORSET_BOX, "select", (void*)&item_sel);

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



SIGNAL_HANDLER  int win_videocolor_set_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_videocolor_set_create(const char* widgetname, void *usrdata)
{	
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
#ifdef HD 
    	    GUI_SetProperty("win_videocolor_set_title", "img", "title_colorset.bmp");
    	GUI_SetProperty("win_videocolor_set_tip_image_exit", "img", "tips_exit.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
#ifdef HD
    	    GUI_SetProperty("win_videocolor_set_title", "img", "title_colorset_e.bmp");
    	GUI_SetProperty("win_videocolor_set_tip_image_exit", "img", "tips_exit_e.bmp");
#endif
    }


	GUI_SetProperty(VIDEOCOLORSET_TEXT_TIME, "string", app_win_get_local_date_string());

    init_videocolor_brightness();
    init_videocolor_saturation();
    init_videocolor_contrast();

	return 0;
}

SIGNAL_HANDLER int win_videocolor_set_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER int win_videocolor_set_keypress(const char* widgetname, void *usrdata)
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
	     		GUI_EndDialog("win_videocolor_set");
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



#include "app.h"


#define VIDEOSET_BOX			            "win_video_set_box"
#define VIDEOSET_COMBOBOX_TV_STANDARD		"win_video_set_boxitem1_combobox"
#define VIDEOSET_COMBOBOX_SCREEN_RATIO      "win_video_set_boxitem3_combobox"
#define VIDEOSET_COMBOBOX_ASPECT_MODE      	"win_video_set_boxitem4_combobox"
#define VIDEOSET_TEXT_TIME                  "win_video_set_time_text"

static int32_t widget_init_tv_standard = 0;
static int32_t widget_init_screen_ratio = 0;
static int32_t widget_init_aspect_mode = 0;

static int init_tv_standard(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;

    flash_value = app_flash_get_config_video_hdmi_mode();
    switch(flash_value)
		{
			#ifdef APP_SD
			case VIDEO_OUTPUT_PAL:
				flash_value = 0;
				break;
			case VIDEO_OUTPUT_NTSC_M:
				flash_value = 1;
				break;
			default:
				break;
			#endif
			#ifdef APP_HD
			case VIDEO_OUTPUT_HDMI_576I:
				flash_value = 0;
				break;
			case VIDEO_OUTPUT_HDMI_576P:
				flash_value = 1;
				break;
			case VIDEO_OUTPUT_HDMI_720P_50HZ:
				flash_value = 2;
				break;
			case VIDEO_OUTPUT_HDMI_1080I_50HZ:
				flash_value = 3;
				break;
			case VIDEO_OUTPUT_HDMI_1080P_50HZ:
				flash_value = 4;
				break;
			default:
				break;

			#endif
		}

    widget_value = flash_value;
    
    GUI_SetProperty(VIDEOSET_COMBOBOX_TV_STANDARD, "select", (void*)&widget_value);

    widget_init_tv_standard = widget_value;
	return 0;
}

static int init_screen_ratio(void)
{
    Video_Display_Screen_t flash_value= 0;
    int32_t widget_value = 0;
    
    flash_value = app_flash_get_config_video_display_screen();
    widget_value = flash_value;
    
    GUI_SetProperty(VIDEOSET_COMBOBOX_SCREEN_RATIO, "select", (void*)&widget_value);

    widget_init_screen_ratio = widget_value;
	return 0;
}

static int init_aspect_mode(void)
{
    int32_t flash_value= 0;
    int32_t widget_value = 0;
    
    flash_value = app_flash_get_config_video_aspect();
    widget_value = flash_value;

    GUI_SetProperty(VIDEOSET_COMBOBOX_ASPECT_MODE, "select", (void*)&widget_value);

    widget_init_aspect_mode = widget_value;
	return 0;
}

static int set_tv_standard(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 2;

    flash_value = widget_value;
    switch(flash_value)
		{
			#ifdef APP_SD
			case 0:
				flash_value = VIDEO_OUTPUT_PAL;
				break;
			case 1:
				flash_value = VIDEO_OUTPUT_NTSC_M;
				break;

			default:
				break;
			#endif
			#ifdef APP_HD
						case 0:
				flash_value = VIDEO_OUTPUT_HDMI_576I;
				break;
			case 1:
				flash_value = VIDEO_OUTPUT_HDMI_576P;
				break;
			case 2:
				flash_value = VIDEO_OUTPUT_HDMI_720P_50HZ;
				break;
			case 3:
				flash_value = VIDEO_OUTPUT_HDMI_1080I_50HZ;
				break;
			case 4:
				flash_value = VIDEO_OUTPUT_HDMI_1080P_50HZ;
				break;
			default:
				break;
			#endif
		}

    if(flag_set)
	{
    	app_play_set_hdmi_mode(flash_value);
#ifdef APP_SD
		//*(unsigned int*)0xa4804030 = 0x6a963f3f;
		//*(unsigned int*)0xa4804028 = 0xa0248cf;
#endif
	}
    if(flag_save)
        app_flash_save_config_video_hdmi_mode(flash_value);

	return 0;
}

static int set_screen_ratio(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 2;

    flash_value = widget_value;
   
    if(flag_set)
        app_play_set_video_display_screen(flash_value);
    if(flag_save)
        app_flash_save_config_video_display_screen(flash_value);
    
	return 0;
}

static int set_aspect_mode(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 0;
    
    flash_value = widget_value;
    
    if(flag_set)
        app_play_set_video_aspect(flash_value);
    if(flag_save)
        app_flash_save_config_video_aspect(flash_value);
    
	return 0;
}

static status_t key_exit(void)
{
    int32_t widget_tv_standard = 0;
    int32_t widget_screen_ratio = 0;
    int32_t widget_aspect_mode = 0;
    
    /*check modified?*/
    GUI_GetProperty(VIDEOSET_COMBOBOX_TV_STANDARD, "select",  (void*)&widget_tv_standard);
    GUI_GetProperty(VIDEOSET_COMBOBOX_SCREEN_RATIO, "select",  (void*)&widget_screen_ratio);
    GUI_GetProperty(VIDEOSET_COMBOBOX_ASPECT_MODE, "select",  (void*)&widget_aspect_mode);

    /*not modified, exit directly*/
    if(widget_tv_standard == widget_init_tv_standard
    && widget_screen_ratio == widget_init_screen_ratio
    && widget_aspect_mode == widget_init_aspect_mode)
    {
        return GXCORE_SUCCESS;
    }
    
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;

    ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_CHK_SAVE, POPMSG_TYPE_YESNO);

    /*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
    	set_tv_standard(widget_tv_standard, 0, 1);
        set_screen_ratio(widget_screen_ratio, 0, 1);
        set_aspect_mode(widget_aspect_mode, 0, 1);
	}	
    
    /*not save, reset param*/
    else
    {
        set_tv_standard(widget_init_tv_standard, 1, 0);
        set_screen_ratio(widget_init_screen_ratio, 1, 0);
        set_aspect_mode(widget_init_aspect_mode, 1, 0);
    }
    
	return GXCORE_SUCCESS;
}

static status_t key_lr(unsigned short value)
{
	uint32_t item_sel = 0;	
	uint32_t value_sel = 0;
    
	GUI_GetProperty(VIDEOSET_BOX, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:{
			GUI_GetProperty(VIDEOSET_COMBOBOX_TV_STANDARD, "select", (void*)&value_sel);
            set_tv_standard(value_sel, 1, 0);
			break;
			}
		
		case 1:{
			GUI_GetProperty(VIDEOSET_COMBOBOX_SCREEN_RATIO, "select", (void*)&value_sel);
            set_screen_ratio(value_sel, 1, 0);
			break;
			}

		case 2:{
			GUI_GetProperty(VIDEOSET_COMBOBOX_ASPECT_MODE, "select", (void*)&value_sel);
            set_aspect_mode(value_sel, 1, 0);
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
	
	GUI_GetProperty(VIDEOSET_BOX, "select", (void*)&item_sel);

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



SIGNAL_HANDLER  int win_video_set_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_video_set_create(const char* widgetname, void *usrdata)
{	
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
#ifdef HD
    	    GUI_SetProperty("win_video_set_title", "img", "title_videoset.bmp");
    	GUI_SetProperty("win_video_set_tip_image_exit", "img", "tips_exit.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
#ifdef HD 
    	GUI_SetProperty("win_video_set_title", "img", "title_videoset_e.bmp");
    	GUI_SetProperty("win_video_set_tip_image_exit", "img", "tips_exit_e.bmp");
#endif
    } 


	GUI_SetProperty(VIDEOSET_TEXT_TIME, "string", app_win_get_local_date_string());

    init_tv_standard();
    init_screen_ratio();
    init_aspect_mode();

	return 0;
}

SIGNAL_HANDLER int win_video_set_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER int win_video_set_keypress(const char* widgetname, void *usrdata)
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
	     		GUI_EndDialog("win_video_set");
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



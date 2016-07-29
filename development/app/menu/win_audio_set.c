#include "app.h"


#define AUDIOSET_BOX					    "win_audio_set_box"
#define AUDIOSET_COMBOBOX_AUDIO_TRACK		"win_audio_set_boxitem1_combobox"
#define AUDIOSET_COMBOBOX_SPDIF      		"win_audio_set_boxitem2_combobox"
#define AUDIOSET_TEXT_TIME                  "win_audio_set_time_text"


static int32_t widget_init_audio_track = 0;
static int32_t widget_init_spdif = 0;

static int init_audio_track(void)
{
    int32_t flash_value = 0;
    int32_t widget_value = 0;
    
    flash_value = app_flash_get_config_audio_track();
    if(AUDIO_TRACK_LEFT == flash_value)
    {
        widget_value = 0;
    }
    else if(AUDIO_TRACK_RIGHT == flash_value)
    {
        widget_value = 1;
    }
    else if(AUDIO_TRACK_STEREO == flash_value)
    {
        widget_value = 2;
    }
    GUI_SetProperty(AUDIOSET_COMBOBOX_AUDIO_TRACK, "select", (void*)&widget_value);

    widget_init_audio_track = widget_value;
	return 0;
}

static int init_spdif(void)
{
    int32_t flash_value = 0;
    int32_t widget_value = 0;
	flash_value = app_flash_get_config_audio_ac3_bypass();
	if(flash_value == AUDIO_AC3_BYPASS_MODE)
	{
		widget_value = 1;
	}
	else if(flash_value == AUDIO_AC3_DECODE_MODE)
	{
		widget_value = 0;
	}
    GUI_SetProperty(AUDIOSET_COMBOBOX_SPDIF, "select", (void*)&widget_value);


    // TODO: later
    
    widget_init_spdif = widget_value;
	return 0;
}

static int set_audio_track(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = AUDIO_TRACK_LEFT;
    
    if(0 == widget_value)
    {
        flash_value = AUDIO_TRACK_LEFT;
    }
    else if(1 == widget_value)
    {
        flash_value = AUDIO_TRACK_RIGHT;
    }
    else if(2 == widget_value)
    {
        flash_value = AUDIO_TRACK_STEREO;
    }
    
    if(flag_set)
        app_play_set_audio_track(flash_value);
    if(flag_save)
        app_flash_save_config_audio_track(flash_value);
    
	return 0;
}

static int set_spdif(int widget_value, int flag_set, int flag_save)
{
    int32_t flash_value = 0;
   flash_value = widget_value;
   if(widget_value == 0)
   {
	   flash_value = AUDIO_AC3_DECODE_MODE;
   }
   else if(widget_value == 1)
   {
	   flash_value = AUDIO_AC3_BYPASS_MODE;
   }

    if(flag_set)
        app_play_set_audio_ac3_bypass_onoff(flash_value);
    if(flag_save)
        app_flash_save_config_audio_ac3_bypass(flash_value);


    // TODO: later
    
	return 0;
}



static status_t key_exit(void)
{
    int32_t widget_audio_track = 0;    
    int32_t widget_spdif = 0;   
    
    /*check modified?*/
    GUI_GetProperty(AUDIOSET_COMBOBOX_AUDIO_TRACK, "select", (void*)&widget_audio_track);
    GUI_GetProperty(AUDIOSET_COMBOBOX_SPDIF, "select", (void*)&widget_spdif);

    /*not modified, exit directly*/
    if(widget_audio_track == widget_init_audio_track
        && widget_spdif == widget_init_spdif)
    {
        return GXCORE_SUCCESS;
    }
    
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;

    ret_pop = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_CHK_SAVE, POPMSG_TYPE_YESNO);

    /*save*/
	if(ret_pop == POPMSG_RET_YES)
	{
        set_audio_track(widget_audio_track, 0, 1);
        set_spdif(widget_spdif, 0, 1);
	}	
    
    /*not save, reset param*/
    else
    {
        set_audio_track(widget_init_audio_track, 1, 0);
        set_spdif(widget_init_spdif, 1, 0);
    }
    
	return GXCORE_SUCCESS;
}

static status_t key_lr(unsigned short key)
{
	uint32_t item_sel = 0;
    uint32_t widget_value = 0;
    
	GUI_GetProperty(AUDIOSET_BOX, "select", (void*)&item_sel);
	switch(item_sel)
	{
		case 0:{
            GUI_GetProperty(AUDIOSET_COMBOBOX_AUDIO_TRACK, "select",  (void*)&widget_value);
            set_audio_track(widget_value, 1, 0);
			break;
			}

		case 1:{
            GUI_GetProperty(AUDIOSET_COMBOBOX_SPDIF, "select", (void*)&widget_value);
            set_spdif(widget_value, 1, 0);
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
	
	GUI_GetProperty(AUDIOSET_BOX, "select", (void*)&item_sel);
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



SIGNAL_HANDLER  int win_audio_set_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_audio_set_create(const char* widgetname, void *usrdata)
{	



	GUI_SetProperty(AUDIOSET_TEXT_TIME, "string", app_win_get_local_date_string());

	init_audio_track();
	init_spdif();

	return 0;
}

SIGNAL_HANDLER int win_audio_set_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER int win_audio_set_keypress(const char* widgetname, void *usrdata)
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
	     GUI_EndDialog("win_audio_set");
    		return EVENT_TRANSFER_STOP;
                        
		case KEY_LEFT:
		case KEY_RIGHT:

			key_lr(event->key.sym);
			break;				

		case KEY_OK:
			key_ok();
			break;

        			
		default:
			break;
	}

	
	return EVENT_TRANSFER_STOP;
}



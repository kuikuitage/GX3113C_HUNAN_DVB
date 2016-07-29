#include "app.h"
#ifdef DVB_AD_TYPE_DS_FLAG
#include "app_ds_ads_porting_stb_api.h"
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
#include "mad.h"
#endif


#define FULLSCREEN_WIN                  "win_full_screen"

#define VOLUME_WIN                      "win_menu_volume"
#define VOLUME_PROGRESS                 "win_volume_progbar"
#define VOLUME_VALUE                      "win_volume_text_value"


static event_list* s_timer_volume = NULL;
static int timer_volume(void *userdata)
{
    s_timer_volume = NULL;
	
    GUI_EndDialog(VOLUME_WIN);    
	return 0;
}

static status_t key_lr(unsigned short value)
{
    int32_t volume = 0;
    int32_t volumeBar = 0;

    uint8_t volume_value[3] = {0};

    volume = app_flash_get_config_audio_volume();

#ifndef CUST_TAIKANG

    if(KEY_LEFT == value || KEY_VOLUME_DOWN_1 == value)
    {
        if(volume == 0)
        {
            if (TRUE == app_play_get_mute_flag())
        	{
        		GUI_SetProperty("win_full_screen_img_mute","state","show");
        	}
            return GXCORE_SUCCESS;
        }
        if(3 >= volume)
            volume = 0;
        else
            volume -= 3;

	volumeBar = volume;
    }
    else if(KEY_RIGHT == value || KEY_VOLUME_UP_1 == value)
    {
    	volume +=3;
        if(96 <= volume)
        	{
        		volume = 96;
	             volumeBar = 100;
        	}
        else
        	{
//	             volume += 3;  
			volumeBar = volume;
        	}
    }

	volumeBar = volumeBar*100/96;
    sprintf((char*)volume_value,"%d",volume/3);
#else
    if(KEY_LEFT == value || KEY_VOLUME_DOWN_1 == value)
    {
        if(volume == 0)
        {
            if (TRUE == app_play_get_mute_flag())
        	{
        		GUI_SetProperty("win_full_screen_img_mute","state","show");
        	}
            return GXCORE_SUCCESS;
        }
        if(5 >= volume)
            volume = 0;
        else
            volume -= 5;

		volumeBar = volume;
    }
    else if(KEY_RIGHT == value || KEY_VOLUME_UP_1 == value)
    {
        if(100 <= volume)
    	{
    		volume = 100;
    	}
        else
    	{
            volume += 5;  
    	}
		volumeBar = volume;
    }
	sprintf((char*)volume_value,"%d",volume/5);
#endif


    GUI_SetProperty(VOLUME_VALUE, "string", volume_value);
    GUI_SetProperty(VOLUME_PROGRESS, "value", (void*)&volumeBar);
    app_flash_save_config_audio_volume(volume);
    
	// set stb
    app_play_set_volumn(volume);
	if (TRUE == app_play_get_mute_flag())
	{
		GUI_SetProperty("win_full_screen_img_mute","state","show");
	}
	else
	{
		GUI_SetProperty("win_full_screen_img_mute","state","hide");
	}
    
    return GXCORE_SUCCESS;
}



SIGNAL_HANDLER  int win_volume_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_volume_create(const char* widgetname, void *usrdata)
{	
    int32_t volume = 0;
    int32_t volumeBar = 0;
    uint8_t volume_value[3] = {0};
    
    volume = app_flash_get_config_audio_volume();
#ifndef CUST_TAIKANG
    if (volume<32*3)
    	sprintf((char*)volume_value,"%d",volume/3);
    else
    	sprintf((char*)volume_value,"%d",32);

    volumeBar = volume;

	volumeBar = volumeBar*100/96;
#else
    sprintf((char*)volume_value,"%d",volume/5);
	volumeBar = volume;
#endif

    GUI_SetProperty(VOLUME_VALUE, "string", volume_value);
    GUI_SetProperty(VOLUME_PROGRESS, "value", (void*)&volumeBar);
    
#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_vol_pic_display("win_menu_volume_ad");
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	GUI_SetInterface("flush", NULL);
	app_mad_ad_show_pic("win_menu_volume_ad",M_AD_PLACE_ID_VOL_BAR);
#endif
	s_timer_volume = create_timer(timer_volume, 3000, NULL,  TIMER_ONCE);
	return 0;
}

SIGNAL_HANDLER int win_volume_destroy(const char* widgetname, void *usrdata)
{
    remove_timer(s_timer_volume);
    s_timer_volume = NULL;
#ifdef DVB_AD_TYPE_DS_FLAG
	app_ds_ad_hide_vol_pic();
#endif
#ifdef DVB_AD_TYPE_MAIKE_FLAG
	app_mad_ad_hide_pic("win_menu_volume_ad");
#endif
	return 0;
}


SIGNAL_HANDLER int win_volume_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
    	case KEY_EXIT:
            GUI_EndDialog(VOLUME_WIN);
    		return EVENT_TRANSFER_STOP;
                        
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_VOLUME_UP_1:
		case KEY_VOLUME_DOWN_1:
			key_lr(event->key.sym);
			if(s_timer_volume)
			{	
				reset_timer(s_timer_volume);
			}			
			break;				
		

		default:
            GUI_EndDialog(VOLUME_WIN);
            GUI_SendEvent(FULLSCREEN_WIN, event);
            return EVENT_TRANSFER_STOP;
			break;
	}

	
	return EVENT_TRANSFER_STOP;
}



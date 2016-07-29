/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_program_detail.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.13		  lijq 	 			creation
*****************************************************************************/
#include "app.h"
#include "gxprogram_manage_berkeley.h"
static uint32_t pos = 0;

static event_list* spApp_fatory_time_dplay = NULL;

static  int fatory_menu_dplay_time(void *userdata)
{

	char* focus_Window = (char*)GUI_GetFocusWindow();
	strength_xml strengthxml = {0};
	signal_xml singalxml = {0};


	if((NULL !=focus_Window )&&( strcasecmp("win_factory_menu_dplay", focus_Window) != 0))
	{
		return 0;
	}

	if(1 == GxFrontend_QueryStatus(0))
	{
		GUI_SetProperty("win_factory_menu_dplay_text_lock", "string", "Locked");
		strengthxml.lock_status = 1;	
		singalxml.lock_status =1;
	}
	else
	{
		GUI_SetProperty("win_factory_menu_dplay_text_lock", "string", "UnLock");	
		strengthxml.lock_status = 0;	
		singalxml.lock_status =0;
	}
	strengthxml.widget_name_strength_bar = "win_factory_menu_dplay_progbar_lev";
	strengthxml.widget_name_strength_bar_value = "win_factory_menu_dplay_text_lev";
	app_search_set_strength_progbar(strengthxml);

	singalxml.widget_name_signal_bar = "win_factory_menu_dplay_progbar_qua";
	singalxml.widget_name_signal_bar_value = "win_factory_menu_dplay_text_qua";
	singalxml.widget_name_error_rate_bar = NULL;
	singalxml.widget_name_error_rate = NULL;
	app_search_set_signal_progbar(singalxml);


	return 0;
}


static void factory_update_cur_program_info(void)
{

	GxBusPmDataProg prog={0};
	char buffer[41] = {0};
	int8_t volume;
	uint8_t volume_value[3] = {0};
	int32_t volumeBar = 0;
    int32_t flash_value = 0;
	Lcn_State_t lcn_flag;

	if(0 == app_prog_get_num_in_group())
	{
		return ;
	}

    GxBus_PmProgGetByPos(pos,1,&prog);


	if(strlen((char*)(prog.prog_name))>15)
	{
		prog.prog_name[15]='\0';
	}

    lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_OFF == lcn_flag)
	{
		snprintf((void*)buffer, 40, "%03d %s", pos + 1, prog.prog_name);
	}
	else
	{
		snprintf((void*)buffer, 40, "%03d %s", prog.pos, prog.prog_name);
	}

	GUI_SetProperty("win_factory_menu_dplay_text_prog_name", "string", (void*)buffer);

	volume = app_flash_get_config_audio_volume();
	if (volume<32*3)
	   sprintf((char*)volume_value,"%d",volume/3);
	else
	   sprintf((char*)volume_value,"%d",32);

	volumeBar = volume;

	volumeBar = volumeBar*100/96;

	GUI_SetProperty("win_factory_menu_dplay_text_vol", "string", volume_value);
	GUI_SetProperty("win_factory_menu_dplay_progbar_vol", "value", (void*)&volumeBar);
	
    flash_value = app_flash_get_config_audio_track();
    if(AUDIO_TRACK_LEFT == flash_value)
    {
        
		GUI_SetProperty("win_factory_menu_dplay_text_sound", "string", "Left");	
        
    }
    else if(AUDIO_TRACK_RIGHT == flash_value)
    {
        
		GUI_SetProperty("win_factory_menu_dplay_text_sound", "string", "Right");	
        
    }
    else if(AUDIO_TRACK_STEREO == flash_value)
    {
        
		GUI_SetProperty("win_factory_menu_dplay_text_sound", "string", "Stereo");	
        
    }


}


SIGNAL_HANDLER  int app_factory_menu_dplay_create(const char* widgetname, void *usrdata)
{

	factory_update_cur_program_info();
	spApp_fatory_time_dplay = create_timer(fatory_menu_dplay_time, 500, NULL,  TIMER_REPEAT);


	return 0;

}

SIGNAL_HANDLER  int app_factory_menu_dplay_destroy(const char* widgetname, void *usrdata)
{	

	remove_timer(spApp_fatory_time_dplay);
	spApp_fatory_time_dplay = NULL;


	return 0;
}
static status_t key_lr(unsigned short value)
{
    int32_t volume = 0;
    int32_t volumeBar = 0;

    uint8_t volume_value[3] = {0};

    volume = app_flash_get_config_audio_volume();

    if(KEY_LEFT == value || KEY_VOLUME_DOWN_1 == value)
    {
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


    GUI_SetProperty("win_factory_menu_dplay_text_vol", "string", volume_value);
    GUI_SetProperty("win_factory_menu_dplay_progbar_vol", "value", (void*)&volumeBar);
    app_flash_save_config_audio_volume(volume);
    
	// set stb
    app_play_set_volumn(volume);

    
    return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int app_factory_menu_dplay_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_GREEN:
				GUI_EndDialog("win_factory_menu_dplay");
				GUI_CreateDialog("win_factory_menu");
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL: 
			case KEY_EXIT:
				GUI_EndDialog("win_factory_menu_dplay");
				GUI_CreateDialog("win_factory_menu");
				return EVENT_TRANSFER_STOP;	
			case KEY_YELLOW:
			case KEY_MENU:
				GUI_EndDialog("win_factory_menu_dplay");
				GUI_CreateDialog("win_factory_menu_port");
				return EVENT_TRANSFER_STOP;	
			case KEY_LEFT:
			case KEY_RIGHT:
				key_lr(event->key.sym);
				return EVENT_TRANSFER_STOP;

			case KEY_UP:
			case KEY_CHANNEL_UP:
				//gApp_lock_right = 0;
				pos = app_play_by_direction(-1);	
				factory_update_cur_program_info();
				app_play_reset_play_timer(PLAY_TIMER_DURATION);

				return EVENT_TRANSFER_STOP;	
				
			case KEY_DOWN:
			case KEY_CHANNEL_DOWN:
				//gApp_lock_right = 0;
				pos = app_play_by_direction(1);
				factory_update_cur_program_info();
				app_play_reset_play_timer(PLAY_TIMER_DURATION);

				return EVENT_TRANSFER_STOP;	
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;	
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}
	return EVENT_TRANSFER_KEEPON;
}



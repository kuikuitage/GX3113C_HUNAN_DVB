#include "app.h"
#include "app_common_media.h" 

#define WIN_VOLUME					"win_volume"
#define PROGBAR						"volume_progbar"
#define COMBOBOX_SWITCH_MODE		"pic_set_combo_switch_mode"
#define COMBOBOX_SWITCH_SEQUENCE	"pic_set_combo_play_sequence"

#define IMAGE_MUTE_MOVIE                      "movie_view_imag_mute"
#define IMAGE_MUTE_MUSIC                               "music_view_imag_mute"

event_list* timer_volume_destory = NULL;
static uint32_t volume_value = 0;

// 0-40 映射成0-100
#define AUDIO_MAP_40(audio)	(((audio)*5)>>1)

//3: 0-25 映射成0-75 4: 0-25 映射成0-100 
#define VOLUME_NUM (4)
#define AUDIO_MAP_25(audio)	((audio) * VOLUME_NUM)

// 0-50 to 0-75
 #define AUDIO_MAP_50(audio) ((audio)/2 + (audio)) 

extern status_t app_volume_scope_status(void);
extern int app_audio_prog_track_set_mode(int value,uint8_t mode);
extern status_t app_system_vol_save(GxMsgProperty_PlayerAudioVolume val);

static void volume_show_mute(void)
{
	if(volume_value == 0)
	{
		if(GUI_CheckDialog("win_movie_view") == GXCORE_SUCCESS)
		{
			GUI_SetProperty("movie_view_imag_mute", "state", "show");
		}
		else if(GUI_CheckDialog("win_music_view") == GXCORE_SUCCESS)
		{
			GUI_SetProperty("music_view_imag_mute", "state", "show");
		}
	}
}

static int _volume_auto_destory(void* usrdata)
{
	GUI_EndDialog(WIN_VOLUME);	
	volume_show_mute();
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int volume_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	char *PreviousWindow=NULL;
	event = (GUI_Event *)usrdata;

	PreviousWindow = (char*)GUI_GetPreviousWindow(WIN_VOLUME);
	GUI_SendEvent(PreviousWindow, event);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int volume_init(const char* widgetname, void *usrdata)
{
	volume_value = pmpset_get_int(PMPSET_VOLUME);
	GUI_SetProperty(PROGBAR, "value", &volume_value);
	
	timer_volume_destory = create_timer(_volume_auto_destory, 5000, 0, TIMER_ONCE);
	return 0;
}

SIGNAL_HANDLER int volume_destroy(const char* widgetname, void *usrdata)
{
	if(timer_volume_destory)
	{
		remove_timer(timer_volume_destory);
		timer_volume_destory = NULL;
	}
	
    app_flash_save_config_audio_volume(AUDIO_MAP_25(volume_value));
	app_play_set_volumn(AUDIO_MAP_25(volume_value));
	return 0;
}

SIGNAL_HANDLER int volume_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t value = 0;
	const char *Previous=NULL;
	int key;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	key = find_virtualkey_ex(event->key.scancode,event->key.sym);
	switch(key)
	{
		case APPK_BACK:
		case APPK_MENU:
			//remove_timer(timer_volume_destory);
			GUI_EndDialog(WIN_VOLUME);
			volume_show_mute();
			break;
			
		case APPK_VOLDOWN:
		case APPK_LEFT://movie menu
		case APPK_GREEN:
			reset_timer(timer_volume_destory);
			if(0 < volume_value)
			{
				volume_value--;
			}
			else
			{
				volume_value = 0;
			}

			pmpset_set_int(PMPSET_VOLUME, volume_value);
			GUI_SetProperty(PROGBAR, "value", &volume_value);	

			if (0 == volume_value)
			{
				if (GUI_CheckDialog(WIN_VOLUME) == GXCORE_SUCCESS)
				{
					GUI_EndDialog(WIN_VOLUME);
				}
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_ON);
				GUI_SetProperty(IMAGE_MUTE_MOVIE, "state", "show");
				GUI_SetProperty(IMAGE_MUTE_MUSIC, "state", "show");

			}

			{
				value = pmpset_get_int(PMPSET_MUTE);	
				if (value==PMPSET_MUTE_ON)
				{		
					pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
					GUI_SetProperty(IMAGE_MUTE_MOVIE, "state", "osd_trans_hide");
					GUI_SetProperty(IMAGE_MUTE_MUSIC, "state", "hide");				
				}	
			}
			break;
			
		case APPK_VOLUP:
		case APPK_RIGHT://movie menu
		case APPK_YELLOW:
			reset_timer(timer_volume_destory);
			if(volume_value == 0)
			{		
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE_MOVIE, "state", "osd_trans_hide");	
				GUI_SetProperty(IMAGE_MUTE_MUSIC, "state", "osd_trans_hide");	
			}
			if(25 > volume_value)
			{
				volume_value++;
			}
			else
			{
				volume_value = 25;
			}

			value = pmpset_get_int(PMPSET_MUTE);	
			if (value == PMPSET_MUTE_ON)
			{		
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE_MOVIE, "state", "osd_trans_hide");	
				GUI_SetProperty(IMAGE_MUTE_MUSIC, "state", "hide");	
			}
			
			pmpset_set_int(PMPSET_VOLUME, volume_value);
			GUI_SetProperty(PROGBAR, "value", &volume_value);
		
			break;
			
		default:
			GUI_EndDialog(WIN_VOLUME);
			
#if 0  //delete by zhangmq 20140505, 
	   //SendEvent to box is unreasonable, 
	   //and too many Event to the next wnd, will make problem of flush
			//GUI_SetProperty(WIN_VOLUME, "draw_now", "NULL");
			GUI_SetInterface("flush",NULL);
			Previous = GUI_GetFocusWidget();//GUI_GetFocusWindow();
			if(Previous != NULL)
			{
				GUI_SendEvent(Previous, event);
				
				#if 1
				//patch for the bug543,  pause and play in the wnd volume.  20121112 by liuhy
				if(0 == strcmp("music_view_box", Previous))
				{
					GUI_SendEvent("win_music_view", event);
					GUI_SetFocusWidget("music_view_box");
				}
				#endif
			}
#endif
			//and by zhangmq 20140505
			Previous = GUI_GetFocusWindow();
			if(Previous != NULL)
			{
				GUI_SendEvent(Previous, event);
			}
			break;
	}
	
	return EVENT_TRANSFER_STOP;
}


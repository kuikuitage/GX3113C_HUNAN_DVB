#include "app.h"

#define WIN_MUSIC_VIEW              			"win_music_view"
#define WIN_MUSIC_SET              			"win_music_set"
#define BOX_MUSIC_SET					"music_set_box"
#define COMBOBOX_SWITCH_SEQUENCE	 	"music_set_combo_play_sequence"
#define COMBOBOX_VIEW_MODE			"music_set_combo_view_mode"

#define CANVAS_SPECTRUM				"music_view_canvas_spectrum"
#define NOTPAD_LRC                				"music_view_notepad_lrc"

static uint32_t s_view_mode_bak = PMPSET_MUSIC_VIEW_MODE_SPECTRUM;

#if (DVB_DEFINITION_TYPE == SD_DEFINITION)
#define SPECTRUM_X_POINTER	30
#define SPECTRUM_Y_POINTER  50
#define SPECTRUM_X_WIDTH	350
#define SPECTRUM_Y_LENGTH	250
#endif
#if (DVB_DEFINITION_TYPE == HD_DEFINITION)
#define SPECTRUM_X_POINTER	0
#define SPECTRUM_Y_POINTER  50
#define SPECTRUM_X_WIDTH	700
#define SPECTRUM_Y_LENGTH	300
#endif

static status_t key_left_right(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;
	
	GUI_GetProperty(BOX_MUSIC_SET, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:
			GUI_GetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_MUSIC_PLAY_SEQUENCE, value_sel);
			break;
		case 1:
			GUI_GetProperty(COMBOBOX_VIEW_MODE, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_MUSIC_VIEW_MODE, value_sel);
			break;

		default:
			break;
	}

	printf("[MUSIC] keypress_music_set_ok item:%d, value:%d\n", item_sel, value_sel);
		
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int music_set_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	event = (GUI_Event *)usrdata;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;

	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			if(PLAYER_STATUS_PLAY_END == player_status->status)
				GUI_EndDialog(WIN_MUSIC_SET);
			break;
		default:
			break;
	}
	GUI_SendEvent(WIN_MUSIC_VIEW, event);
	GUI_SetProperty(WIN_MUSIC_SET, "update", NULL);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int music_set_init(const char* widgetname, void *usrdata)
{
	int32_t value = 0;
    char* osd_language=NULL;    
         
    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
        GUI_SetProperty("music_set_image_logo", "img", "MP_SETTING_ch.bmp");
    }
    else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
        GUI_SetProperty("music_set_image_logo", "img", "MP_SETTING.bmp");
    }

	value = pmpset_get_int(PMPSET_MUSIC_PLAY_SEQUENCE);
	GUI_SetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value);

	value = pmpset_get_int(PMPSET_MUSIC_VIEW_MODE);
	GUI_SetProperty(COMBOBOX_VIEW_MODE, "select", (void*)&value);
	s_view_mode_bak = value;
	
	return 0;
}

SIGNAL_HANDLER int music_set_destroy(const char* widgetname, void *usrdata)
{
	uint32_t value_sel = 0;
	
	value_sel = pmpset_get_int(PMPSET_MUSIC_VIEW_MODE);
	if(PMPSET_MUSIC_VIEW_MODE_ID3 == value_sel)
	{
		if(s_view_mode_bak != PMPSET_MUSIC_VIEW_MODE_ID3)
		{
			if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_SPECTRUM)
			{
				spectrum_destroy();
			}	
			else if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_LRC)	
			{
				lyrics_destroy();
			}
			
			id3_create();
		}
	}
	else if(PMPSET_MUSIC_VIEW_MODE_LRC == value_sel)
	{
		if(s_view_mode_bak != PMPSET_MUSIC_VIEW_MODE_LRC)
		{
			if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_SPECTRUM)
			{
		spectrum_destroy();
			}
			else if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_ID3)
			{
		id3_destroy();
			}
		
		lyrics_create(NOTPAD_LRC);
		}
	}
	else if(PMPSET_MUSIC_VIEW_MODE_SPECTRUM == value_sel)
	{
		if(s_view_mode_bak != PMPSET_MUSIC_VIEW_MODE_SPECTRUM)
		{
			if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_LRC)
			{
		lyrics_destroy();
			}
			else if(s_view_mode_bak == PMPSET_MUSIC_VIEW_MODE_ID3)
			{
		id3_destroy();
			}
		
		spectrum_create(CANVAS_SPECTRUM, SPECTRUM_X_POINTER, SPECTRUM_Y_POINTER, SPECTRUM_X_WIDTH, SPECTRUM_Y_LENGTH);
		spectrum_start();
		}
	}	
	return 0;
}


SIGNAL_HANDLER int music_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
	{
		case APPK_OK:
			//key_ok();
			break;
		case APPK_RIGHT:
		case APPK_LEFT:
			key_left_right();
			break;

		case APPK_VOLUP:
		case APPK_VOLDOWN:
		case APPK_GREEN:
		case APPK_YELLOW:
			GUI_CreateDialog("win_volume");
			GUI_SendEvent("win_volume", event);
			return EVENT_TRANSFER_STOP;

		case APPK_REPEAT:
		{
			int value;
	
			value = pmpset_get_int(PMPSET_MUSIC_PLAY_SEQUENCE);
			if(value == PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE)
			{
				value = PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE;
			}
			else
			{
				value++;
			}
				pmpset_set_int(PMPSET_MUSIC_PLAY_SEQUENCE, value);

			GUI_SetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value);
			break;
		}
		default:
			break;
	}
	
	return EVENT_TRANSFER_KEEPON;
}


#include "app.h"

#define BOX_PIC_SET						"pic_set_box"
#define COMBOBOX_SWITCH_DURATION	"pic_set_combo_switch_duration"
#define COMBOBOX_SWITCH_MODE		"pic_set_combo_switch_mode"
#define COMBOBOX_SWITCH_SEQUENCE	"pic_set_combo_play_sequence"
#define WIN_PIC_VIEW              			"win_pic_view"

static status_t key_left_right(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;
	
	GUI_GetProperty(BOX_PIC_SET, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:
			GUI_GetProperty(COMBOBOX_SWITCH_DURATION, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_PIC_SWITCH_DURATION, value_sel);
			break;
		/*case 1:
			GUI_GetProperty(COMBOBOX_SWITCH_MODE, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_PIC_SWITCH_MODE, value_sel);
			break;*/
		case 1:
			GUI_GetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_PIC_PLAY_SEQUENCE, value_sel);
			break;

		default:
			break;
	}

	printf("[PIC] keypress_pic_view_set_ok=item:%d, value:%d\n", item_sel, value_sel);
		
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int pic_set_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	event = (GUI_Event *)usrdata;
	GUI_SendEvent(WIN_PIC_VIEW, event);
	
	return EVENT_TRANSFER_STOP;
}
void recover_waitshow(void);
void hide_waitshow(void);

SIGNAL_HANDLER int pic_set_init(const char* widgetname, void *usrdata)
{	
	int32_t value = 0;
    char* osd_language=NULL;

    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {          
        GUI_SetProperty("pic_set_image_logo", "img", "MP_SETTING_ch.bmp");
    }    
    else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {         
        GUI_SetProperty("pic_set_image_logo", "img", "MP_SETTING.bmp");
    }   
             
	hide_waitshow();

	value = pmpset_get_int(PMPSET_PIC_SWITCH_DURATION);
	GUI_SetProperty(COMBOBOX_SWITCH_DURATION, "select", (void*)&value);
	
	//value = pmpset_get_int(PMPSET_PIC_SWITCH_MODE);
	//GUI_SetProperty(COMBOBOX_SWITCH_MODE, "select", (void*)&value);
	
	value = pmpset_get_int(PMPSET_PIC_PLAY_SEQUENCE);
	GUI_SetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value);
	
	return 0;
}

SIGNAL_HANDLER int pic_set_destroy(const char* widgetname, void *usrdata)
{
	GUI_SetProperty(BOX_PIC_SET, "state", "hide");
	recover_waitshow();
	return 0;
}


SIGNAL_HANDLER int pic_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case APPK_LEFT:
				case APPK_RIGHT:
					key_left_right();	
					break;				

				case APPK_OK:
					
					break;
					
				default:
					break;
			}
			break;
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}




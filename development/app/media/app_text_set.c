#include "app.h"

//widget
#define NOTEPAD					"text_view_notepad"
#define BOX_TEXT_SET 	               "text_set_box"
#define COMBOBOX_TEXT_ROLL_LINES 	 "text_set_combo_roll_lines"
#define COMBOBOX_TEXT_AUTO_ROLL 	 "text_set_combo_auto_roll"
#define WIN_TEXT_VIEW             "win_text_view"

event_list* text_auto_roll_timer = NULL;
extern uint32_t text_roll_line_num;


 int text_roll_start(void* usrdata)
{

	GUI_SetProperty(NOTEPAD, "line_down", &text_roll_line_num);	
	return 0;
}

SIGNAL_HANDLER  int text_set_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	event = (GUI_Event *)usrdata;
	GUI_SendEvent(WIN_TEXT_VIEW, event);
	
	return EVENT_TRANSFER_STOP;
}

static status_t key_left_right(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;	
	char rows[]={1,3,5};
	
	GUI_GetProperty(BOX_TEXT_SET, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:
			GUI_GetProperty(COMBOBOX_TEXT_ROLL_LINES, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_TEXT_ROLL_LINES, value_sel);			
			text_roll_line_num=rows[value_sel];
			break;
			
		case 1:	
			GUI_GetProperty(COMBOBOX_TEXT_AUTO_ROLL, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_TEXT_AUTO_ROLL, value_sel);
			break;
		
		default:
			break;
	}

	printf("[TEXT] keypress_text_view_set_ok item:%d, value:%d\n", item_sel, value_sel);
		
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int text_set_init(const char* widgetname, void *usrdata)
{	
	int32_t value = 0;	
    char* osd_language=NULL;

    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {          
        GUI_SetProperty("text_set_image_logo", "img", "MP_SETTING_ch.bmp");
    }    
    else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {         
        GUI_SetProperty("text_set_image_logo", "img", "MP_SETTING.bmp");
    }   

	value = pmpset_get_int(PMPSET_TEXT_ROLL_LINES);
	GUI_SetProperty(COMBOBOX_TEXT_ROLL_LINES, "select", (void*)&value);

	
       value = pmpset_get_int(PMPSET_TEXT_AUTO_ROLL);
	GUI_SetProperty(COMBOBOX_TEXT_AUTO_ROLL, "select", (void*)&value);

	if(text_auto_roll_timer)
	{
		remove_timer(text_auto_roll_timer);
		text_auto_roll_timer = NULL;
	}
	return 0;
}

SIGNAL_HANDLER int text_set_destroy(const char* widgetname, void *usrdata)
{	
	uint32_t value=0;
	
	value= pmpset_get_int(PMPSET_TEXT_AUTO_ROLL);
	if(value==PMPSET_TONE_ON)
	{
		text_auto_roll_timer = create_timer(text_roll_start, 5000, 0, TIMER_REPEAT);
	}
      return 0;
}


SIGNAL_HANDLER int text_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_LEFT:			
		case APPK_RIGHT:
			key_left_right();
			break;				

		case APPK_OK:
			//key_ok();
			break;
			
		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}




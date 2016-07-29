#include "app.h"
#if FACTORY_SERIALIZATION_SUPPORT

#define WND_FACTORY_SERIALIZATION               "wnd_factory_serialization"
#define PATH_XML                                "factory_serialization/wnd_factory_serialization.xml"

#define TXT_ITEM1                               "txt_factory_serialization_item1"
#define TXT_ITEM2                               "txt_factory_serialization_item2"
#define TXT_ITEM3                               "txt_factory_serialization_item3"
#define TXT_ITEM4                               "txt_factory_serialization_item4"
#define TXT_ITEM5                               "txt_factory_serialization_item5"
#define TXT_ITEM6                               "txt_factory_serialization_item6"
#define TXT_ITEM7                               "txt_factory_serialization_item7"
#define TXT_ITEM8                               "txt_factory_serialization_item8"
#define TXT_ITEM9                               "txt_factory_serialization_item9"
#define TXT_ITEM10                              "txt_factory_serialization_item10"
#define TXT_ITEM11                              "txt_factory_serialization_item11"

#define ITEM_COUNTS                             11
static char *thiz_widgets[ITEM_COUNTS] = {
     TXT_ITEM1,
     TXT_ITEM2,
     TXT_ITEM3,
     TXT_ITEM4,
     TXT_ITEM5,
     TXT_ITEM6,
     TXT_ITEM7,
     TXT_ITEM8,
     TXT_ITEM9,
     TXT_ITEM10,
     TXT_ITEM11,
};


extern int factory_serialization_get_message(char* string, unsigned int data_bytes);
static unsigned int thiz_current_item = 0;
static unsigned int _get_display_strings(char *buffer, unsigned int in_size)
{
    unsigned int real_bytes = 0;
    if((NULL == buffer) || (0 == in_size))
    {
        return 0;// 0 bytes
    }
    // TODO
    real_bytes = factory_serialization_get_message(buffer, in_size);
    return real_bytes;
}

#define MESSAGE_LEN  512
char messages[MESSAGE_LEN] = {0};

static int _display_messages(void)
{
    unsigned int data_len = 0;
    unsigned int current_item = 0;
    data_len = _get_display_strings(messages, MESSAGE_LEN);
    if(data_len)
    {// need to display
        current_item = thiz_current_item%ITEM_COUNTS;
        if((0 == current_item) && (0 != thiz_current_item))
        {// need clean the display
            int i = 0;
            for(i = 0; i < ITEM_COUNTS; i++)
            {
                GUI_SetProperty(thiz_widgets[i], "string", " ");
            }
        }
        GUI_SetProperty(thiz_widgets[current_item], "string", messages);
        thiz_current_item++;
    }
    return 0;
}
// timer
static event_list  *thiz_display_timer = NULL;
static int _display_timer(void *usrdata)
{
    _display_messages();
    return 0;
}
static int _create_display_timer(void)
{
    if (reset_timer(thiz_display_timer) != 0) 
    {
        thiz_display_timer = create_timer(_display_timer, 100,  NULL, TIMER_REPEAT);
    }
    return 0;
}

extern int factory_serialization_enter(void);
extern void factory_serialization_exit(void);
extern int get_factory_serialization_flag(void);

int app_factory_serialization(void)
{
    //static int init_flag = 0;
    if(factory_serialization_enter())
    {// need serialization
        thiz_current_item = 0;
	printf("wnd_factory_serialization\n");
	GUI_CreateDialog(WND_FACTORY_SERIALIZATION);
        _create_display_timer();
    }
    else
    {// need not
	GUI_CreateDialog("win_full_screen");
    }
    return 0;
}

// for ui
SIGNAL_HANDLER int On_wnd_factory_serialization_destroy(GuiWidget *widget, void *usrdata)
{
    factory_serialization_exit();
    if(thiz_display_timer)
    {
        remove_timer(thiz_display_timer);
        thiz_display_timer = NULL;
    }
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int On_wnd_factory_serialization_keypress(GuiWidget *widget, void *usrdata)
{
    int ret = EVENT_TRANSFER_STOP;
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_KEYDOWN:
		//	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
			switch(event->key.sym)
			{
                case KEY_EXIT:
                case KEY_MENU:
                    if(0 == get_factory_serialization_flag())
                    {
                        GUI_EndDialog(WND_FACTORY_SERIALIZATION);
			GUI_CreateDialog("win_full_screen");
                        GUI_SetInterface("flush", NULL);
                    }
                    break;
				default:
					break;
			}
		default:
			break;
	}
	return ret;
}

#endif

#include "app.h"
//#include "app_module.h"
//#include "app_msg.h"
//#include "app_send_msg.h"


static int popmsg_result;
popmsg_ret pop_rss_msg(char *context,int YesorNo)
{

	GUI_CreateDialog("wnd_rss_dialog");
	if(YesorNo)
		GUI_SetFocusWidget("wnd_button_yes");
	else
		GUI_SetFocusWidget("wnd_button_no");
	GUI_SetProperty("wnd_text", "string", (void*)context);
	app_msg_destroy(g_app_msg_self);
#if 0
	while(POPMSG_RET_NONE == popmsg_result)
	{
		GUI_Exec();
		GxCore_ThreadDelay(50);
	}
#endif
	while(POPMSG_RET_NONE == popmsg_result)
	{
	   GUI_Loop();
	   GxCore_ThreadDelay(50);
	}
	GUI_StartSchedule();
	GUI_EndDialog("wnd_rss_dialog");
	//GUI_Exec();
	GUI_SetInterface("flush",NULL);
	app_msg_init(g_app_msg_self);
	return popmsg_result;
}

SIGNAL_HANDLER int app_rss_dialog_init(const char* widgetname, void *usrdata)
{
	popmsg_result = POPMSG_RET_NONE;
	printf("dialog init \n");
	return 0;
}

SIGNAL_HANDLER int app_rss_dialog_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	char* name = NULL;
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(find_virtualkey(event->key.sym))
		{
			case STBK_OK:	
				name = (char*)GUI_GetFocusWidget();
				if(0==strcasecmp(name, "wnd_button_yes"))
					popmsg_result=POPMSG_RET_YES;
				else
					popmsg_result=POPMSG_RET_NO;
				
				break;

			case STBK_LEFT:
			case STBK_RIGHT:
				name = (char*)GUI_GetFocusWidget();
				if(0==strcasecmp(name, "wnd_button_yes"))
					GUI_SetFocusWidget("wnd_button_no");
				else
					GUI_SetFocusWidget("wnd_button_yes");		
				break;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_STOP;
}



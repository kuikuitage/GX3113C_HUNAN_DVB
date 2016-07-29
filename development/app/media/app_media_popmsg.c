#include "app_media_popmsg.h"
#include "app_pop.h"
#include "gui_core.h"
#include "media_key.h"
#include "app_win_interface.h"



#define TEXT_WIN_POPMSG_YESNO		"win_media_popmsg_yesno"
#define TEXT_TEXT_POPMSG_YESNO		"popmsg_text_yesno"
#define TEXT_BUTTON_POPMSG_YES		"popmsg_button_yes"
#define TEXT_BUTTON_POPMSG_NO		"popmsg_button_no"

#define TEXT_WIN_POPMSG_OK			"win_media_popmsg_ok"
#define TEXT_TEXT_POPMSG_OK			"popmsg_text_ok"
#define TEXT_BUTTON_POPMSG_OK		"popmsg_button_ok"


static media_popmsg_ret popmsg_result = MEDIA_POPMSG_RET_NONE;
static media_popmsg_type popmsg_type_sel = MEDIA_POPMSG_TYPE_YESNO;
static int32_t media_popmsg_yes_timeout = 0;

char* gc_MediaPopmsg_win = NULL;
extern char* gs_popmsg_win;

PopDlgRet popdlg_create(PopDlg* dlg)
{
	// pop.format
	// pop.mode
	// pop.timeout_sec
	// pop.exit_cb
	// pop.title
	media_popmsg_ret msgret = MEDIA_POPMSG_RET_NO;
	PopDlgRet ret = POP_VAL_CANCEL;
	
	if (NULL == dlg)
		return POP_VAL_CANCEL;
	
	if (0 != dlg->timeout_sec)
		media_popmsg_yes_timeout = dlg->timeout_sec;
	switch(dlg->type)
	{
		case POP_TYPE_YES_NO:
			msgret = media_popmsg(dlg->pos.x, dlg->pos.y,dlg->str, MEDIA_POPMSG_TYPE_YESNO);
			break;
		case POP_TYPE_OK:
		default:
			ret = media_popmsg(dlg->pos.x, dlg->pos.y,dlg->str, MEDIA_POPMSG_TYPE_OK);	
			break;
	}

	switch(msgret)
	{
		case MEDIA_POPMSG_RET_YES:
			ret = POP_VAL_OK;
			break;
		case MEDIA_POPMSG_RET_NO:
			ret = POP_VAL_CANCEL;
			break;
		case MEDIA_POPMSG_RET_OK:
			ret = POP_VAL_OK;
			break;
		default:
			break;
	}
	return ret;
}

static status_t popmsg_ok_key(void)
{
	char* name = NULL;

	name = (char*)GUI_GetFocusWidget();
	
	switch(popmsg_type_sel)
	{
		case MEDIA_POPMSG_TYPE_YESNO:
			if(0 == strcasecmp(name, TEXT_BUTTON_POPMSG_YES))
			{
				popmsg_result = MEDIA_POPMSG_RET_YES;
			}
			else if(0 == strcasecmp(name, TEXT_BUTTON_POPMSG_NO))
			{
				popmsg_result = MEDIA_POPMSG_RET_NO;
			}
			break;
		case MEDIA_POPMSG_TYPE_OK:
			popmsg_result = MEDIA_POPMSG_RET_OK;
			break;

		default:
			return GXCORE_ERROR;
			break;
	}
	return GXCORE_SUCCESS;
}

static status_t popmsg_esc_key(void)
{
	switch(popmsg_type_sel)
	{
		case MEDIA_POPMSG_TYPE_YESNO:
			popmsg_result = MEDIA_POPMSG_RET_NO;
			break;
			
		case MEDIA_POPMSG_TYPE_OK:
			popmsg_result = MEDIA_POPMSG_RET_OK;
			break;

		default:
			return GXCORE_ERROR;
			break;
	}
	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int popmsg_init(const char* widgetname, void *usrdata)
{
	popmsg_result = MEDIA_POPMSG_RET_NONE;

	if(MEDIA_POPMSG_TYPE_YESNO == popmsg_type_sel)
	{
		GUI_SetFocusWidget(TEXT_BUTTON_POPMSG_NO);
	}
	
	return 0;
}

SIGNAL_HANDLER int popmsg_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{						
		case APPK_BACK:
		case APPK_MENU:
			popmsg_esc_key();
			break;
	
		case APPK_OK:
			popmsg_ok_key();
			break;
			
		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}



media_popmsg_ret media_popmsg(uint32_t x, uint32_t y, const char* context, media_popmsg_type type)
{
	char* xml_popmsg_win = NULL;
	char* xml_popmsg_text = NULL;
	char popmsg_win_yesno[] = {TEXT_WIN_POPMSG_YESNO};
	char popmsg_text_yesno[] = {TEXT_TEXT_POPMSG_YESNO};
	char popmsg_win_ok[] = {TEXT_WIN_POPMSG_OK};
	char popmsg_text_ok[] = {TEXT_TEXT_POPMSG_OK};

	GxTime nowtime={0};
	GxTime starttime={0};
//	uint8_t time[10] = {0};

	/*
	* 如果当前提示框状态，新提示框消息不创建直接返回
	*避免多个提示框死机
	*/
	if (TRUE == app_get_pop_msg_flag_status())
		{
			media_popmsg_yes_timeout = 0;
			switch(type)
			{
				case MEDIA_POPMSG_TYPE_YESNO:
					return MEDIA_POPMSG_RET_NO;
				case MEDIA_POPMSG_TYPE_OK:
					return MEDIA_POPMSG_RET_OK;
				default:
					return MEDIA_POPMSG_RET_NONE;
			}			
		}

	app_set_pop_msg_flag_status(TRUE);

	popmsg_type_sel = type;
	
	switch(type)
	{
		case MEDIA_POPMSG_TYPE_YESNO:
			xml_popmsg_win = popmsg_win_yesno;
			xml_popmsg_text = popmsg_text_yesno;
			break;
		case MEDIA_POPMSG_TYPE_OK:
			xml_popmsg_win = popmsg_win_ok;
			xml_popmsg_text = popmsg_text_ok;
			break;

		default:
			return MEDIA_POPMSG_RET_NONE;
			break;
	}

	GUI_EndDialog(xml_popmsg_win);//fix create two and more
	GUI_CreateDialog(xml_popmsg_win);
	GUI_SetProperty(xml_popmsg_win, "move_window_x", &x);
	GUI_SetProperty(xml_popmsg_win, "move_window_y", &y);
	GUI_SetProperty(xml_popmsg_text, "string", (void*)context);

	gc_MediaPopmsg_win = xml_popmsg_win;
	
	app_set_pop_msg_flag_status(TRUE);

	//app_msg_destroy(g_app_msg_self);
	GxCore_GetTickTime(&starttime);
	GUI_SetInterface("flush", NULL);
	while(MEDIA_POPMSG_RET_NONE == popmsg_result)
	{
		GUI_LoopEvent();
		GxCore_ThreadDelay(50);
		if (media_popmsg_yes_timeout >0 )
		{
			GxCore_GetTickTime(&nowtime);	
			if (nowtime.seconds - starttime.seconds >= media_popmsg_yes_timeout)
				popmsg_result = MEDIA_POPMSG_RET_YES;
			else
				{
/*					char* focus_Window = (char*)GUI_GetFocusWindow();
					if ((NULL != focus_Window)&&(0 == strcasecmp(xml_popmsg_win, focus_Window)))
						{
							sprintf((void*)time,"%02d",popmsg_yes_timeout - (nowtime.seconds - starttime.seconds));
							GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT,"state","show");
							GUI_SetProperty(TEXT_TEXT_POPMSG_TIME_OUT, "string", (void*)time);								
						} */
				}
		}
	}

	media_popmsg_yes_timeout = 0;
	GUI_EndDialog(xml_popmsg_win);
	app_set_pop_msg_flag_status(FALSE);

//	GUI_LoopEvent();
//    GUI_StartSchedule();
	//app_msg_init(g_app_msg_self);
	return popmsg_result;
}


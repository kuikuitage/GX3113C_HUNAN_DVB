#include "app_media_popmsg.h"
//#include "app_utility.h"

#if 0

#define WIN_POPMSG_TIMEOUT		"win_media_popmsg_timeout"
#define TEXT_POPMSG_TIMEOUT_INFO		"text_popmsg_timeout_info"
#define TEXT_POPMSG_TIMEOUT_TIME		"text_popmsg_timeout_time"
#define BUTTON_POPMSG_TIMEOUT_YES	"button_popmsg_timeout_yes"
#define BUTTON_POPMSG_TIMEOUT_NO	    "button_popmsg_timeout_no"

static popmsg_ret s_pop_timeout_result = MEDIA_POPMSG_RET_NONE;
static time_t s_timeout_sec = 0;
static popmsg_ret s_default_result = MEDIA_POPMSG_RET_NONE;
int iiii =0;
static bool s_show_time = false;
int iiissi =0;

static PopMsgExitCb s_exit_cb = NULL;
static char *s_time_str = NULL;
static event_list  *s_popmsg_timer = NULL;



static int popmsg_timeout_cb(void *usrdata)
{
	s_timeout_sec--;

	if(s_show_time == true)
	{
		if(s_time_str != NULL)
		{
			GxCore_Free(s_time_str);
			s_time_str = NULL;
		}
		s_time_str = app_time_to_hms_str(s_timeout_sec);
		GUI_SetProperty(TEXT_POPMSG_TIMEOUT_TIME, "string", (void*)s_time_str);
		GUI_SetProperty(TEXT_POPMSG_TIMEOUT_TIME, "draw_now", NULL);
	}

	if(s_timeout_sec == 0)
	{
		timer_stop(s_popmsg_timer);
		s_pop_timeout_result = s_default_result;
		GUI_EndDialog(WIN_POPMSG_TIMEOUT);
	}

	return 0;
}

popmsg_ret popmsg_timeout(const char *info_str, time_t sec, popmsg_ret default_ret, bool show_time, PopMsgExitCb exit_cb)
{
#define SEC_TO_MILLISEC (1000)

	uint32_t x = POP_MEDIA_X_START;
	uint32_t y = POP_MEDIA_Y_START;

	s_pop_timeout_result = MEDIA_POPMSG_RET_NONE;
	s_timeout_sec = sec;
	s_default_result = default_ret;
	s_show_time = show_time;
	s_exit_cb = exit_cb;

	if(s_time_str != NULL)
	{
		GxCore_Free(s_time_str);
		s_time_str = NULL;
	}
	
	GUI_CreateDialog(WIN_POPMSG_TIMEOUT);
	GUI_SetProperty(WIN_POPMSG_TIMEOUT, "move_window_x", &x);
	GUI_SetProperty(WIN_POPMSG_TIMEOUT, "move_window_y", &y);
	GUI_SetProperty(TEXT_POPMSG_TIMEOUT_INFO, "string", (void*)info_str);

	if(MEDIA_POPMSG_RET_NO == s_default_result)
	{
		GUI_SetFocusWidget(BUTTON_POPMSG_TIMEOUT_NO);
	}
	else
	{
		GUI_SetFocusWidget(BUTTON_POPMSG_TIMEOUT_YES);
	}

	if(s_timeout_sec > 0)
	{
		if(show_time == true)
		{
			s_time_str = app_time_to_hms_str(s_timeout_sec);
			GUI_SetProperty(TEXT_POPMSG_TIMEOUT_TIME, "string", (void*)s_time_str);
			s_popmsg_timer = create_timer(popmsg_timeout_cb, SEC_TO_MILLISEC,  NULL, TIMER_REPEAT);
		}
	}
	
	return s_pop_timeout_result;
}

#endif


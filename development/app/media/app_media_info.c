#include "app.h"
#include "media_info.h"
#include "play_music.h"

#define WIN_MEDIA_INFO     "win_media_info"
#define WIN_PIC_VIEW       "win_pic_view"
#define WIN_MOVIE_VIEW     "win_movie_view"
#define WIN_MUSIC_VIEW     "win_music_view"

#define MEDIA_TXT         "media_info_text"
#define MEDIA_INFO        "media_info_text_info"
#define MEDIA_TITLE       "media_info_text_title"

static MediaInfo s_info;

SIGNAL_HANDLER  int media_info_service(const char* widgetname, void *usrdata)
{
	MediaInfo info;
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
				GUI_EndDialog(WIN_MEDIA_INFO);
			break;
		default:
			break;
	}

	memset(&info, 0, sizeof(info));
	
	if(NULL != s_info.change_cb)
	{
		s_info.change_cb(&info);
		media_info_create(&info);
	}
	
	event = (GUI_Event *)usrdata;
	if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_PIC_VIEW))
		GUI_SendEvent(WIN_PIC_VIEW, event);
	else if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MOVIE_VIEW))
		GUI_SendEvent(WIN_MOVIE_VIEW, event);
	else if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MUSIC_VIEW))
		GUI_SendEvent(WIN_MUSIC_VIEW, event);

	GUI_SetProperty(WIN_MEDIA_INFO, "update", NULL);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_info_init(const char* widgetname, void *usrdata)
{
#if MEDIA_SUBTITLE_SUPPORT	
	if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MOVIE_VIEW))
		subtitle_pause();
#endif
	
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_info_destroy(const char* widgetname, void *usrdata)
{
#if MEDIA_SUBTITLE_SUPPORT	
	if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MOVIE_VIEW))
		subtitle_resume();
#endif

	if(s_info.destroy_cb)
		s_info.destroy_cb();
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_info_keypress(const char* widgetname, void *usrdata)
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
				case APPK_OK:
				case APPK_BACK:
				case APPK_MENU:
					GUI_EndDialog(WIN_MEDIA_INFO);
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

status_t media_info_create(MediaInfo *info)
{
	status_t ret = GXCORE_ERROR;
	int i = 0;
	char text_buf[20] = {0};
	char info_buf[20] = {0};

	if(info == NULL) return ret;
	memset(&s_info, 0, sizeof(MediaInfo));
    memcpy(&s_info, info, sizeof(MediaInfo));

    if(GUI_CheckDialog(WIN_MEDIA_INFO) != GXCORE_SUCCESS)
        GUI_CreateDialog(WIN_MEDIA_INFO);

	for(i=0; i<MEDIA_INFO_LINE; i++)
	{
		sprintf(text_buf, "%s%d", MEDIA_TXT, i);
		sprintf(info_buf, "%s%d", MEDIA_INFO, i);

		if(s_info.line[i].subt)
			GUI_SetProperty(text_buf, "string", (void *)(s_info.line[i].subt));
		else
			GUI_SetProperty(text_buf, "string", (void *)(" "));

		if(s_info.line[i].info)		
			GUI_SetProperty(info_buf, "string", (void *)(s_info.line[i].info));
		else
			GUI_SetProperty(info_buf, "string", (void *)(" "));
		printf("[MEIDA][%d]: subt= %s, info= %s\n",i, s_info.line[i].subt, s_info.line[i].info);
	}
	GUI_SetProperty(WIN_MEDIA_INFO, "update", NULL);
	return GXCORE_SUCCESS;
}


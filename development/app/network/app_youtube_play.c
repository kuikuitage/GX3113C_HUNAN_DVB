#include "app.h"
//#include "app_module.h"
//#include "app_msg.h"
//#include "app_send_msg.h"
//#include "full_screen.h"
//#include "app_pop.h"
//#include "app_book.h"
//#include "app_default_params.h"
#include "app_youtube_playlist.h"
#include "youtube_tools.h"

//huangbc added

#define PLAYER_FOR_NORMAL       "player1"
#define PLAYER_FOR_PIP          "player2"
#define PLAYER_FOR_REC          "player3"
#define PLAYER_FOR_JPEG         "player4"
#define PLAYER_FOR_IPTV         "player5"
#define STR_ID_SERVER_FAIL		"Server connecting failed, please retry!"



static struct youtube_play_item* sp_play_item = NULL;
static int s_total_media_dur = 0;
static event_list* sp_YoutubePlaySliderbarTimer = NULL;
static event_list* sp_YoutubePlayUpdateTimer = NULL;

GxMsgProperty_PlayerPlay g_youtube_playParm = {0};

void youtube_play_set_cur_play_item(struct youtube_play_item* pitem)
{
	sp_play_item = pitem;
	s_total_media_dur = sp_play_item->media_duration;
}

//-------------------------------------------------------------------------
#define YOUTUBE_PLAY_GIF
#define GIF_PATH WORK_PATH"theme/image/youtube/loading.gif"
static event_list* sp_YoutubePlayGifTimer = NULL;

void app_youtube_play_hide_gif(void)
{
	GUI_SetProperty("redtube_loading_gif", "state", "hide");
	//GUI_SetInterface("flush", NULL);
}

void app_youtube_play_show_gif(void)
{
	GUI_SetProperty("redtube_loading_gif", "state", "show");
	//GUI_SetInterface("flush", NULL);
}

void app_youtube_play_load_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("redtube_loading_gif", "load_img", GIF_PATH);
	GUI_SetProperty("redtube_loading_gif", "init_gif_alu_mode", &alu);
}

void app_youtube_play_free_gif(void)
{
	GUI_SetProperty("redtube_loading_gif", "load_img", NULL);
}


static int youtube_play_draw_gif_proc(void* userdata)
{
	static int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("redtube_loading_gif", "draw_gif", &alu);
	return 0;
}

void gif_start(void)
{
	app_youtube_play_show_gif();
	
	if (reset_timer(sp_YoutubePlayGifTimer) != 0)
	{
		sp_YoutubePlayGifTimer = create_timer(youtube_play_draw_gif_proc, 50, NULL, TIMER_REPEAT);
	}
}

void gif_stop(void)
{
	if(sp_YoutubePlayGifTimer)
	{
		remove_timer(sp_YoutubePlayGifTimer);
		sp_YoutubePlayGifTimer = NULL;
	}

	app_youtube_play_hide_gif();
}


//-------------------------------------------------------------------------
static event_list* sp_YoutubePlayPopupTimer = NULL;

void hide_popup_msg_youtube_play(void)
{
	if(sp_YoutubePlayPopupTimer)
	{
		remove_timer(sp_YoutubePlayPopupTimer);
		sp_YoutubePlayPopupTimer = NULL;
	}
	
	GUI_SetProperty("img_youtube_play_popup", "state", "hide");
	GUI_SetProperty("txt_youtube_play_popup", "state", "hide");
}

static int youtube_play_popup_timer_timeout(void *userdata)
{
	hide_popup_msg_youtube_play();
	return 0;
}

void show_popup_msg_youtube_play(char* pMsg, int time_out)
{
	GUI_SetProperty("txt_youtube_play_popup", "string", pMsg);
	
	GUI_SetProperty("img_youtube_play_popup", "state", "show");
	GUI_SetProperty("txt_youtube_play_popup", "state", "show");

	if(time_out > 0)
	{
		if (reset_timer(sp_YoutubePlayPopupTimer) != 0)
		{
			sp_YoutubePlayPopupTimer = create_timer(youtube_play_popup_timer_timeout, time_out, NULL, TIMER_ONCE);
		}
	}
}


//-------------------------------------------------------------------------
static void update_sliderbar(void)
{
	uint64_t cur_time_ms = 0;
	uint64_t total_time_ms = 0;
	PlayTimeInfo time = {};
	uint64_t cur_time_s = 0;
	uint64_t total_time_s = 0;
	int cur_sliderbar = 0;
	struct tm *ptm = NULL;
	char buf_tmp[20];	
	status_t ret = GXCORE_ERROR;

	ret = GxPlayer_MediaGetTime(PLAYER_FOR_IPTV, &time);
	if(GXCORE_SUCCESS == ret)
	{
		s_total_media_dur = time.totle;
		cur_time_ms = time.current;
		if(0 >= s_total_media_dur)
		{
			s_total_media_dur = 0;
			cur_time_ms = 0;
			cur_sliderbar = 0;
		}
		else if(cur_time_ms > s_total_media_dur)
		{
			cur_time_ms = s_total_media_dur;
			cur_sliderbar = 100;
		}
		else
			cur_sliderbar = (cur_time_ms * 100)/ s_total_media_dur ;
	}
	else
	{
		cur_sliderbar = 0;
	}

	/*sliderbar*/
	GUI_SetProperty("youtube_play_sliderbar", "value", &cur_sliderbar);
	//GUI_SetProperty("youtube_play_sliderbar_cursor", "value", &cur_sliderbar);

	#if 1
	/*cur time*/
	cur_time_s = cur_time_ms / 1000;
	cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
	ptm = localtime((const time_t*)  &cur_time_s);
	sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	GUI_SetProperty("youtube_play_text_start_time", "string", buf_tmp);

	/*total time*/
	total_time_s = s_total_media_dur / 1000;
	total_time_s += ((s_total_media_dur % 1000) == 0 ? 0 : 1);
	ptm = localtime((const time_t*)  &total_time_s);
	sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	GUI_SetProperty("youtube_play_text_stop_time", "string", buf_tmp);
	#endif
}

static int youtube_play_update(void* userdata)
{
	update_sliderbar();
	return 0;
}

void youtube_play_update_timer_stop(void)
{
	//timer_stop(sp_YoutubePlayUpdateTimer);
	remove_timer(sp_YoutubePlayUpdateTimer);
	sp_YoutubePlayUpdateTimer = NULL;
}

void youtube_play_update_timer_reset(void)
{
	if (reset_timer(sp_YoutubePlayUpdateTimer) != 0)
	{
		sp_YoutubePlayUpdateTimer = create_timer(youtube_play_update, 1000, NULL, TIMER_REPEAT);
	}
}


//-------------------------------------------------------------------------
static void youtube_play_hide_sliderbar(void)
{
	GUI_SetProperty("youtube_play_image_boxback", "state", "hide");
	GUI_SetProperty("img_youtube_play_sliderback", "state", "hide");
	GUI_SetProperty("youtube_play_sliderbar", "state", "hide");
	//GUI_SetProperty("youtube_play_sliderbar_cursor", "state", "hide");
	GUI_SetProperty("youtube_play_text_start_time", "state", "hide");
	GUI_SetProperty("youtube_play_text_stop_time", "state", "hide");
	GUI_SetProperty("youtube_play_text_title", "state", "hide");
	
	//timer_stop(sp_YoutubePlaySliderbarTimer);
	remove_timer(sp_YoutubePlaySliderbarTimer);
	sp_YoutubePlaySliderbarTimer = NULL;
}

static int youtube_play_sliderbar_timeout(void *userdata)
{
	youtube_play_update_timer_stop();
	
	youtube_play_hide_sliderbar();
	return 0;
}

void youtube_play_show_sliderbar(int timeout)
{
	update_sliderbar();
	GUI_SetProperty("youtube_play_image_boxback", "state", "show");
	GUI_SetProperty("img_youtube_play_sliderback", "state", "show");
	GUI_SetProperty("youtube_play_sliderbar", "state", "show");
	//GUI_SetProperty("youtube_play_sliderbar_cursor", "state", "show");
	GUI_SetProperty("youtube_play_text_start_time", "state", "show");
	GUI_SetProperty("youtube_play_text_stop_time", "state", "show");
	GUI_SetProperty("youtube_play_text_title", "state", "show");

	youtube_play_update_timer_reset();
	
	if (reset_timer(sp_YoutubePlaySliderbarTimer) != 0)
	{
		sp_YoutubePlaySliderbarTimer = create_timer(youtube_play_sliderbar_timeout, timeout, NULL, TIMER_ONCE);
	}
}


//-------------------------------------------------------------------------
static int youtube_play_service_status(GxMsgProperty_PlayerStatusReport* player_status)
{
	//char err_player[64];
	GxMsgProperty_PlayerStop player_stop;
	
	if(strcmp((char*)(player_status->player), PLAYER_FOR_IPTV) != 0)
	{
		return EVENT_TRANSFER_STOP;
	}
	
	switch(player_status->status)
	{
		case PLAYER_STATUS_ERROR:
			printf("\n[Youtube Player] PLAYER_STATUS_ERROR, %d\n", player_status->error);

			gif_stop();
			
			//sprintf(err_player, "Player error,code:%d!", player_status->error);
			show_popup_msg_youtube_play(STR_ID_SERVER_FAIL, 0);
			
			break;

		case PLAYER_STATUS_PLAY_END:
			printf("\n[Youtube Player] PLAYER_STATUS_PLAY_END\n");

			
			app_send_msg(GXMSG_PLAYER_STOP,PLAYER_FOR_IPTV);
			//app_send_msg_exec(GXMSG_PLAYER_STOP, &player_stop);
			//app_player_close(PLAYER_FOR_NORMAL);
			GUI_EndDialog("wnd_youtube_play");
			break;

		case PLAYER_STATUS_PLAY_RUNNING:
			gif_stop();
			printf("\n[Youtube Player] PLAYER_STATUS_PLAY_RUNNING\n");
			break;
			
		case PLAYER_STATUS_PLAY_START:
			printf("\n[Youtube Player] PLAYER_STATUS_PLAY_START\n");
			break;

		default:
			break;
	}

	return 0;
}

SIGNAL_HANDLER  int youtube_play_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;
	GxMsgProperty_PlayerAVCodecReport* avcodec_status = NULL;
	GxMsgProperty_PlayerStatusReport player_status_fromavcodec;
	
	event = (GUI_Event *)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			youtube_play_service_status(player_status);
			break;
			
		case GXMSG_PLAYER_AVCODEC_REPORT:
			avcodec_status = (GxMsgProperty_PlayerAVCodecReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerAVCodecReport);
			if(AVCODEC_ERROR == avcodec_status->acodec || AVCODEC_ERROR == avcodec_status->vcodec)
			{
				player_status_fromavcodec.status = PLAYER_STATUS_ERROR;
				youtube_play_service_status(&player_status_fromavcodec);
			}
			break;
			
		default:
			break;
	}
	
	
	return EVENT_TRANSFER_STOP;
}


//-------------------------------------------------------------------------
static event_list* sp_YoutubePlay_FirstIn_Timer = NULL;

static  status_t app_send_play(uint32_t msg_id ,  void* params)                      \
{                                                                       \
    GxMessage *msg = NULL;                                              \
    status_t ret = GXCORE_ERROR;                                            \
    GxMsgProperty_PlayerPlay *data = NULL;                                              \
    GxMsgProperty_PlayerPlay *out_param = NULL;                                         \
                                                                        \
                                                                        \
    if((NULL == params) ||(GXMSG_PLAYER_PLAY !=  msg_id))                                  \
    {                                                                   \
        APP_PRINT("params or msg id is incorrect!msg_id = %d\n",msg_id);        \
        return GXCORE_ERROR;                                            \
    }                                                                   \
                                                                        \
    msg = GxBus_MessageNew(GXMSG_PLAYER_PLAY);                                     \
    if(NULL == msg)                                                     \
    {                                                                   \
        return GXCORE_ERROR;                                            \
    }                                                                   \
                                                                        \
    data = (GxMsgProperty_PlayerPlay*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerPlay);           \
    if(NULL == data)                                                        \
    {                                                                   \
		GxBus_MessageFree(msg);											\
        return GXCORE_ERROR;                                            \
    }                                                                   \
    out_param = (GxMsgProperty_PlayerPlay*)params;                                      \
                                                                        \
    memcpy(data,out_param,sizeof(GxMsgProperty_PlayerPlay));                                \
                                                                        \
    ret = GxBus_MessageSend(msg);                                       \
    if(GXCORE_SUCCESS != ret)                                             \
    {                                                                   \
        return GXCORE_ERROR;                                            \
    }                                                                   \
    return GXCORE_SUCCESS;                                              \
}


static int youtube_play_first_in_timeout(void *userdata)
{
	remove_timer(sp_YoutubePlay_FirstIn_Timer);
	sp_YoutubePlay_FirstIn_Timer = NULL;
	
	youtube_play_show_sliderbar(9000);

	gif_start();

	app_send_play(GXMSG_PLAYER_PLAY, (void *)(&g_youtube_playParm));

	return 0;
}

SIGNAL_HANDLER int youtube_play_create(const char* widgetname, void *usrdata)
{
	int progress = 2;
	GUI_SetProperty("youtube_play_sliderbar", "value", &progress);
	//GUI_SetProperty("youtube_play_sliderbar_cursor", "value", &progress);

	GUI_SetProperty("youtube_play_text_title", "string", sp_play_item->media_title);
	
	//GUI_SetInterface("video_enable", NULL);
	
	app_youtube_play_load_gif();
	

	sp_YoutubePlay_FirstIn_Timer = create_timer(youtube_play_first_in_timeout, 500, NULL, TIMER_ONCE);
	
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int youtube_play_destroy(const char* widgetname, void *usrdata)
{
	if(sp_YoutubePlay_FirstIn_Timer)
	{
		remove_timer(sp_YoutubePlay_FirstIn_Timer);
		sp_YoutubePlay_FirstIn_Timer = NULL;
	}

	gif_stop();

	app_youtube_play_free_gif();
	
	youtube_play_hide_sliderbar();
	youtube_play_update_timer_stop();

	//GUI_SetInterface("video_disable", NULL);
	return GXCORE_SUCCESS;
}

static void youtube_mute_exec(void)
{
	int32_t Config;

	Config = app_flash_get_config_mute_flag();
    if (Config == 1) 
    {
        Config = 0;
    }
    else 
    {
        Config = 1;
    }
	app_flash_save_config_mute_flag(Config);
    app_send_msg(GXMSG_PLAYER_AUDIO_MUTE, (void *)(&Config));
}


static void youtube_mute_draw(void)
{
#define IMG_MUTE    "img_youtube_mute"
	int32_t Config;
	Config = app_flash_get_config_mute_flag();
    if (Config == 1)
    {
        GUI_SetProperty(IMG_MUTE, "state", "show");
    }
    else
    {
        GUI_SetProperty(IMG_MUTE, "state", "osd_trans_hide");
    }
    GUI_SetProperty(IMG_MUTE, "draw_now", NULL);
}


SIGNAL_HANDLER int youtube_play_keypress(const char* widgetname, void *usrdata)
{
	int ret = EVENT_TRANSFER_KEEPON;
	GUI_Event *event = NULL;
	GxMsgProperty_PlayerStop player_stop;
	
	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_SERVICE_MSG:
			youtube_play_service(widgetname, usrdata);
			break;

		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case STBK_EXIT:
					app_send_msg(GXMSG_PLAYER_STOP, PLAYER_FOR_IPTV);
					//app_player_close(PLAYER_FOR_IPTV);
					GUI_EndDialog("wnd_youtube_play");
					return EVENT_TRANSFER_STOP;
				case KEY_MUTE:
            		youtube_mute_exec();
            		youtube_mute_draw();
                    return EVENT_TRANSFER_STOP;
				case KEY_VOLUME_DOWN_1:
				case STBK_LEFT:
					event->key.sym = APPK_VOLDOWN;
					GUI_CreateDialog("win_volume");
					GUI_SendEvent("win_volume", event);
					return EVENT_TRANSFER_STOP;

				case KEY_VOLUME_UP_1:
				case STBK_RIGHT:
					event->key.sym = APPK_VOLUP;
					GUI_CreateDialog("win_volume");
					GUI_SendEvent("win_volume", event);
					return EVENT_TRANSFER_STOP;
				default:
					youtube_play_show_sliderbar(6000);
					break;
			}
			break;
		default:
			break;
	}

	return ret;
}



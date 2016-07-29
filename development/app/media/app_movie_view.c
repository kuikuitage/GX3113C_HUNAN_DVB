#include "app.h"
#include "app_pop.h"
//#include "app_epg.h"
//#include "module/app_nim.h"
#include "media_info.h"
//#define MEDIA_PLAY_DISPLAY_MODE
//resourse
#define IMG_POP_ZOOM_1					""
#define IMG_POP_ZOOM_2					""
#define IMG_POP_ZOOM_4					""
#define IMG_POP_ZOOM_8					""
#define IMG_POP_ZOOM_16					""
#define IMG_POP_BSPEED_1				""
#define IMG_POP_BSPEED_2				""
#define IMG_POP_BSPEED_4				""
#define IMG_POP_BSPEED_8				""
#define IMG_POP_BSPEED_16				""
#define IMG_POP_BSPEED_32				""
#define IMG_POP_FSPEED_1				""
#define IMG_POP_FSPEED_2				""
#define IMG_POP_FSPEED_4				""
#define IMG_POP_FSPEED_8				""
#define IMG_POP_FSPEED_16				""
#define IMG_POP_FSPEED_32				""
#if MINI_16_BITS_OSD_SUPPORT
#define IMG_POP_STOP					"s_pvr_stop.bmp"
#define IMG_BUTTON2_PLAY_FOCUS		    "s_mpicon_play_focus.bmp"
#define IMG_BUTTON2_PLAY_UNFOCUS		"s_mpicon_play_unfocus.bmp"
#define IMG_BUTTON2_PAUSE_FOCUS		    "s_mpicon_pause_focus.bmp"
#define IMG_BUTTON2_PAUSE_UNFOCUS	    "s_mpicon_pause_unfocus.bmp"

#define IMAGE_MOVIE_TITLE               "img_movie_view_back"

#else
#define IMG_POP_STOP					"MP_BUTTON_STOP.bmp"

#define IMG_BUTTON2_PLAY_FOCUS			"MP_ICON_PLAY_YELLOW.bmp"
#define IMG_BUTTON2_PLAY_UNFOCUS		"MP_ICON_PLAY.bmp"
#define IMG_BUTTON2_PAUSE_FOCUS			"MP_ICON_PAUSE_YELLOW.bmp"
#define IMG_BUTTON2_PAUSE_UNFOCUS		"MP_ICON_PAUSE.bmp"
#endif

#define IMG_POP_PREVIOUS				"MP_BUTTON_FRONT.bmp"
#define IMG_POP_PLAY					"MP_BUTTON_PLAY.bmp"
#define IMG_POP_NEXT					"MP_BUTTON_NEXT.bmp"
#define IMG_FF                          "s_pvr_ff.bmp"
#define IMG_FB                          "s_pvr_fb.bmp"
#define IMG_POP_PAUSE					"MP_BUTTON_PAUSE.bmp"

//widget
#define WIN_VOLUME						"win_volume"
#define WIN_MOVIE_VIEW                  "win_movie_view"
#define WIN_MEDIA_BAR                   "win_media_bar"
#define TEXT_SUBT                       "movie_view_text_subt"
//#define TEXT_SUBT                 		"movie_view_canvas_subt"
#define IMAGE_POPMSG					"movie_view_image_popmsg"
//#define TEXT_POPMSG					    "movie_view_text_popmsg"
#define IMAGE_BOX_BACK					"movie_view_image_boxback"
#define BOX_MOVIE_CTROL					"movie_view_box"
#if (TRICK_PLAY_SUPPORT > 0)
#define BUTTON_PLAY_STOP				"movie_view_boxitem3_button"
#else
#define BUTTON_PLAY_STOP				"movie_view_boxitem2_button"
#endif
#define TEXT_MOVIE_NAME1				"movie_view_text_name1"
#define TEXT_START_TIME					"movie_view_text_start_time"
#define TEXT_STOP_TIME					"movie_view_text_stop_time"
#define SLIDERBAR_MOVICE				"movie_view_sliderbar"
#define SLIDER_CURSOR_MOVICE			"movie_view_sliderbar_cursor"
#define IMG_SLIDER_BACK			        "movie_view_img_sliderback"
#define COMBOBOX_POPMSG                 "movie_view_combo_popmsg"
#if (MINI_16_BITS_OSD_SUPPORT == 1)
#define IMAGE_MUTE_1                      "movie_view_imag_mute1"
#endif
#define IMAGE_MUTE                      "movie_view_imag_mute"
#define TEXT_MOVIE_STATUS				"movie_view_text_status"

static event_list* box_timer_hide = NULL;
static event_list* box_sliderbar_update_timer = NULL;
static event_list* box_sliderbar_active_timer = NULL;
static event_list* popmsg_topright_timer_hide = NULL;
static event_list* popmsg_centre_timer_hide = NULL;
static event_list* delay_play_timer = NULL;
static bool s_movie_video_ok = true;//mark of video decode, if err, can't support play_movie_speed

static play_movie_speed_state movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
static play_movie_ctrol_state movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
static play_movie_speed_state movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
static play_movie_zoom_state movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
static int64_t s_movie_total_time=0;
static bool s_movie_manual_stop = false;
static pmpset_movie_play_sequence s_movie_play_sequence;
static PlayerProgInfo  s_player_info;
static enum
{
    MOVIE_SEEK_NONE = 0,
    MOVIE_SEEKING,
    MOVIE_SEEKED
}s_seek_flag = MOVIE_SEEK_NONE;
static int s_start_time_ms = 0;
static int s_start_play_no = -1;
static bool s_ctl_box_show = false;

//#if (MINI_256_COLORS_OSD_SUPPORT == 0)
// add in 20121123 for radio background display
static bool RadioBackgroundDisplayFlag = false;
//#endif

extern bool music_play_state(void);
extern void music_status_init(void);
static status_t movie_sliderbar_timer_stop(void);
static int movie_box_show(void);
typedef enum
{
	MOVIE_BOX_BUTTON_PREVIOUS,
#if (TRICK_PLAY_SUPPORT > 0)	
	MOVIE_BOX_BUTTON_SPEED_BACKWARD,
#endif	
	MOVIE_BOX_BUTTON_PLAY_PAUSE,
#if (TRICK_PLAY_SUPPORT > 0) 	
	MOVIE_BOX_BUTTON_SPEED_FORWARD,
#endif	
	MOVIE_BOX_BUTTON_NEXT,
	MOVIE_BOX_BUTTON_STOP,
	MOVIE_BOX_BUTTON_SET,
	MOVIE_BOX_BUTTON_INFO,
       #if MINI_16_BITS_OSD_SUPPORT
	MOVIE_BOX_BUTTON_VOL,
       #endif
	MOVIE_BOX_BUTTON_ZOOM,				// TODO: later
	MOVIE_BOX_BUTTON_LRC,					// TODO: later
	MOVIE_BOX_BUTTON_PAUSE,
	MOVIE_BOX_BUTTON_PLAY,
}movie_box_button;

typedef enum
{
	MOVIE_COMBO_TRACK,
	MOVIE_COMBO_RATIO	
}movie_combo;

typedef enum
{
	MOVE_A_DECODE_WORKING = 0,
	MOVE_V_DECODE_WORKING ,
	MOVE_A_V_DECODE_WORKING,
	MOVE_A_V_DECODE_NOT_WORKING
}avdecode_status;

int sg_AVDecodeStatus = 0;;

#ifdef MEDIA_PLAY_DISPLAY_MODE
//win_movie_view
static void app_movie_view_create(void)
{
	
	if(GUI_CheckDialog(WIN_MEDIA_BAR) != GXCORE_SUCCESS)
	{
		GUI_CreateDialog(WIN_MEDIA_BAR);
		if(box_timer_hide != NULL)
		{
			reset_timer(box_timer_hide);
		}
	}	
	return ;
}

static void app_movie_view_end_dialog(void)
{
	
	if(GUI_CheckDialog(WIN_MEDIA_BAR) == GXCORE_SUCCESS)
	{
		GUI_SetProperty(WIN_MEDIA_BAR, "back_ground", "null");
		GUI_EndDialog(WIN_MEDIA_BAR);
	}

	return ;
}
#endif

void movie_view_box_reset(void)
{
	int sliderbar_init = 0;
	char buf_tmp[20]= "00:00:00";
	
	GUI_SetProperty(SLIDERBAR_MOVICE, "value", &sliderbar_init);
	GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
	GUI_SetProperty(TEXT_STOP_TIME, "string", buf_tmp);

	return ;
	
}
static int popmsg_topright_hide(void *userdata)
{
#if 0
	char* focus_win = NULL;
	focus_win = (char*)GUI_GetFocusWindow();
	if(strcasecmp(focus_win, WIN_MOVIE_VIEW) == 0)
	{
		GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
	}
	else
	{
#endif
//	}
	GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
	APP_TIMER_REMOVE(popmsg_topright_timer_hide);
	return 0;
}

static status_t popmsg_topright_ctrl(play_movie_ctrol_state button, uint32_t value)
{
	int timeout = 1000;

	if(popmsg_topright_timer_hide)
	{	
		printf("[MOVIE] popmsg_topright_timer_hide remove it\n");
		APP_TIMER_REMOVE(popmsg_topright_timer_hide);
	}	

	char* focus_win = NULL;
	focus_win = (char*)GUI_GetFocusWindow();
	if(NULL == focus_win) return GXCORE_ERROR;
	if(strcasecmp(focus_win, WIN_MOVIE_VIEW))
	{
		//return GXCORE_ERROR;
		GUI_SetProperty(focus_win, "update", NULL);
	}

	GUI_SetProperty(IMAGE_POPMSG, "state", "show");
//    GUI_SetProperty(TEXT_POPMSG, "state", "hide");
	
	switch(button)
	{
		case PLAY_MOVIE_CTROL_PREVIOUS:
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PREVIOUS);
			break;

#if (TRICK_PLAY_SUPPORT > 0)
		case  PLAY_MOVIE_CTROL_BACKWARD:
			if(PLAY_MOVIE_SPEED_X1== value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PLAY);
				break;
			}

			timeout = 0;
			if(PLAY_MOVIE_SPEED_X2 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X2");
			}
			else if(PLAY_MOVIE_SPEED_X4 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X4");
			}
			else if(PLAY_MOVIE_SPEED_X8 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X8");
			}
			else if(PLAY_MOVIE_SPEED_X16 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X16");
			}	
			else if(PLAY_MOVIE_SPEED_X32 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X32");
			}
            GUI_SetProperty(IMAGE_POPMSG, "img", IMG_FB);
            GUI_SetProperty(TEXT_POPMSG, "state", "show");
			break;		
#endif

		case PLAY_MOVIE_CTROL_PLAY:
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PLAY);
			break;


		case PLAY_MOVIE_CTROL_PAUSE:
			timeout = 0;
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PAUSE);
			break;

#if (TRICK_PLAY_SUPPORT > 0)			
		case PLAY_MOVIE_CTROL_FORWARD:
			if(PLAY_MOVIE_SPEED_X1== value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PLAY);
				break;
			}

			timeout = 0;
			if(PLAY_MOVIE_SPEED_X2 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X2");
			}
			else if(PLAY_MOVIE_SPEED_X4 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X4");
			}
			else if(PLAY_MOVIE_SPEED_X8 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X8");
			}
			else if(PLAY_MOVIE_SPEED_X16 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X16");
			}
			else if(PLAY_MOVIE_SPEED_X32 == value)
			{
				GUI_SetProperty(TEXT_POPMSG, "string", "X32");
			}
            GUI_SetProperty(IMAGE_POPMSG, "img", IMG_FF);
            GUI_SetProperty(TEXT_POPMSG, "state", "show");
			break;
#endif
			
		case PLAY_MOVIE_CTROL_NEXT:
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_NEXT);
			break;

			
		case PLAY_MOVIE_CTROL_STOP:
			timeout = 0;
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_STOP);
			break;


		case  PLAY_MOVIE_CTROL_ZOOM:
			if(PLAY_MOVIE_ZOOM_X1 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_1);
				break;
			}

			timeout = 0;
			if(PLAY_MOVIE_ZOOM_X2== value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_2);
			}
			else if(PLAY_MOVIE_ZOOM_X4 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_4);
			}
			else if(PLAY_MOVIE_ZOOM_X8 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_8);
			}
			else if(PLAY_MOVIE_ZOOM_X16 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_16);
			}
			break;

		default:
			break;
	}
// add in 20121023
	GUI_SetInterface("flush", NULL);
	if(timeout)
	{
		if(popmsg_topright_timer_hide)
		{	
			printf("[MOVIE] popmsg_topright_timer_hide is already created\n");
			return GXCORE_ERROR;
		}
		
		popmsg_topright_timer_hide = create_timer(popmsg_topright_hide, timeout, 0, TIMER_REPEAT);
	}
	
	return GXCORE_SUCCESS;
}

static int popmsg_centre_hide(void* usrdata)
{
	GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");
	APP_TIMER_REMOVE(popmsg_centre_timer_hide);
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_resume();
#endif
	return 0;
}

status_t popmsg_centre_ctrl(movie_combo value_sel)
{
	uint32_t value = 0;

	switch (value_sel)
	{
		case MOVIE_COMBO_TRACK:
			value = pmpset_get_int(PMPSET_AUDIO_TRACK);	
			if(value==PMPSET_AUDIO_TRACK_RIGHT)
			{
				value=PMPSET_AUDIO_TRACK_STEREO;								      
			}
			else 
			{
				value+=1;								 
			}	
			pmpset_set_int(PMPSET_AUDIO_TRACK, value);
			GUI_SetProperty(COMBOBOX_POPMSG, "select", (void*)&value);
			break;
			
		case MOVIE_COMBO_RATIO:		
			value = pmpset_get_int(PMPSET_ASPECT_RATIO);	
			if(value==PMPSET_ASPECT_RATIO_16_9)
			{
				value=PMPSET_ASPECT_RATIO_AUTO;			 
			}
			else 
			{
				value+=1;			
			}	
			pmpset_set_int(PMPSET_ASPECT_RATIO, value);	 
			value+=3;
			GUI_SetProperty(COMBOBOX_POPMSG, "select", (void*)&value);
			break;
			
		default:
			return GXCORE_ERROR;
			break;
	}

#if MEDIA_SUBTITLE_SUPPORT
		subtitle_pause();
#endif

	GUI_SetProperty(COMBOBOX_POPMSG, "state", "show");	

	/*timer*/
	if(popmsg_centre_timer_hide)
	{
		reset_timer(popmsg_centre_timer_hide);
	}
	else
	{
		popmsg_centre_timer_hide = create_timer(popmsg_centre_hide, 3000, 0, TIMER_REPEAT);
	}
	return GXCORE_SUCCESS;
}

status_t popmsg_mute_init(void)
{
	uint32_t value = 0;
    int volume = 0;	
	value=pmpset_get_int(PMPSET_MUTE);
	volume = pmpset_get_int(PMPSET_VOLUME);
    if (volume == 0)
    {
        value = PMPSET_MUTE_ON;
    }
	
	switch (value)
	{
		case PMPSET_MUTE_ON:
            app_play_set_mute(value);
#if (MINI_16_BITS_OSD_SUPPORT == 1 || MINI_16BIT_WIN8_OSD_SUPPORT == 1)
			GUI_SetProperty(IMAGE_MUTE_1, "state", "show");
#endif
			GUI_SetProperty(IMAGE_MUTE, "state", "show");
			break;
			
		case PMPSET_MUTE_OFF:
#if (MINI_16_BITS_OSD_SUPPORT == 1 || MINI_16BIT_WIN8_OSD_SUPPORT == 1)
			GUI_SetProperty(IMAGE_MUTE_1, "state", "hide");
#endif
			GUI_SetProperty(IMAGE_MUTE, "state", "hide");
			break;
		
		default:
			return GXCORE_ERROR;
			break;
	}	
	return GXCORE_SUCCESS;
}

status_t popmsg_mute_ctrl(void)
{
	uint32_t value = 0;
	int Config = 0;
	value=pmpset_get_int(PMPSET_MUTE);
	
	switch (value)
	{
		case PMPSET_MUTE_ON:
            Config = 0;
			pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
#if (MINI_16_BITS_OSD_SUPPORT == 1 || MINI_16BIT_WIN8_OSD_SUPPORT == 1)
			GUI_SetProperty(IMAGE_MUTE_1, "state", "hide");
#endif
			GUI_SetProperty(IMAGE_MUTE, "state", "hide");
			break;
			
		case PMPSET_MUTE_OFF:
            Config = 1;
			pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_ON);
#if (MINI_16_BITS_OSD_SUPPORT == 1 || MINI_16BIT_WIN8_OSD_SUPPORT == 1)
			GUI_SetProperty(IMAGE_MUTE_1, "state", "show");
#endif
			GUI_SetProperty(IMAGE_MUTE, "state", "show");
			break;
		
		default:
			return GXCORE_ERROR;
			break;
	}	
    app_flash_save_config_mute_flag(Config);
    app_play_set_mute(Config);
	return GXCORE_SUCCESS;
}


static status_t movie_box_setinfo(play_movie_info* info)
{
	uint64_t cur_time_ms = 0;
	uint64_t total_time_ms = 0;
	PlayTimeInfo time = {};
	uint64_t cur_time_s = 0;
	uint64_t total_time_s = 0;
	int cur_sliderbar = 0;
	struct tm *ptm = NULL;
	char buf_tmp[20] = {0};
	status_t ret = GXCORE_ERROR;
	
#ifdef MEDIA_PLAY_DISPLAY_MODE
	if(GXCORE_ERROR == GUI_CheckDialog("WIN_MEDIA_BAR"))
		return GXCORE_ERROR;
#endif
	/*file name*/
	if(NULL == info)return GXCORE_ERROR;
	GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info->name);	

//	ret = GxPlayer_MediaGetTime(PMP_PLAYER_AV, &cur_time_ms, &total_time_ms);
	ret = GxPlayer_MediaGetTime(PMP_PLAYER_AV, &time);
	if(GXCORE_SUCCESS == ret)
	{
		total_time_ms = time.totle;
		cur_time_ms = time.current;
		s_movie_total_time = total_time_ms;
		if(0 >= s_movie_total_time)
		{
			s_movie_total_time = 0;
			cur_time_ms = 0;
			cur_sliderbar = 0;
		}
		else if(cur_time_ms > s_movie_total_time)
		{
			cur_time_ms = s_movie_total_time;
			cur_sliderbar = 100;
		}
		else
			cur_sliderbar = (cur_time_ms * 100)/ s_movie_total_time ;
	}
	else
	{
		//return GXCORE_SUCCESS;//避免进度条播放过程中回到开始位置
		cur_sliderbar = 0;
		if ((PLAY_MOVIE_CTROL_STOP != movie_box_button2_ctrol) && (true != s_movie_manual_stop))
		{
			return GXCORE_SUCCESS;
		}
	}
	//printf("cur_time_ms=%lld\n",cur_time_ms);
	/*sliderbar*/
	if ((MOVIE_SEEK_NONE == s_seek_flag) || (MOVIE_SEEKED == s_seek_flag))
    {
        GUI_SetProperty(SLIDERBAR_MOVICE, "value", &cur_sliderbar);

		if (MOVIE_SEEK_NONE == s_seek_flag)
        {
            GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &cur_sliderbar);
        }
        else
        {
            int seeked_val = 0;
			
            GUI_GetProperty(SLIDER_CURSOR_MOVICE, "value", &seeked_val);
            GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &seeked_val);
        }
    }

	/*cur time*/
	cur_time_s = cur_time_ms / 1000;
	cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
	ptm = localtime((const time_t*)  &cur_time_s);
	sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);

	/* total time */	
	total_time_s = s_movie_total_time / 1000;	
	total_time_s += ((s_movie_total_time % 1000) == 0 ? 0 : 1);	
	ptm = localtime((const time_t*)  &total_time_s);	
	sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);	
	GUI_SetProperty(TEXT_STOP_TIME, "string", buf_tmp);

	return GXCORE_SUCCESS;
}


static status_t movie_box_button2_setimg(play_movie_ctrol_state state)
{
	if((PLAY_MOVIE_CTROL_PAUSE == state)||(PLAY_MOVIE_CTROL_STOP == state))
	{
		GUI_SetProperty(BUTTON_PLAY_STOP, "unfocus_img", IMG_BUTTON2_PLAY_UNFOCUS);
		GUI_SetProperty(BUTTON_PLAY_STOP, "focus_img", IMG_BUTTON2_PLAY_FOCUS);
	}
	else if(PLAY_MOVIE_CTROL_PLAY== state)
	{
		GUI_SetProperty(BUTTON_PLAY_STOP, "unfocus_img", IMG_BUTTON2_PAUSE_UNFOCUS);
		GUI_SetProperty(BUTTON_PLAY_STOP, "focus_img", IMG_BUTTON2_PAUSE_FOCUS);
	}
	else
	{
		return GXCORE_ERROR;
	}

	GUI_SetProperty("movie_view_boxitem2", "update", NULL);

	return GXCORE_SUCCESS;
}

static int movie_box_hide(void* userdata)
{
	int ret_value = 0;
#ifdef MEDIA_PLAY_DISPLAY_MODE
	app_movie_view_end_dialog();
    ret_value = EVENT_TRANSFER_KEEPON;
#else

	GUI_SetProperty(IMAGE_BOX_BACK, "state", "hide");
	#if MINI_16_BITS_OSD_SUPPORT
	GUI_SetProperty(IMAGE_MOVIE_TITLE, "state", "hide");
	#endif
	GUI_SetProperty(TEXT_MOVIE_NAME1, "rolling_stop", NULL);
	GUI_SetProperty(TEXT_MOVIE_NAME1, "state", "hide");
	GUI_SetProperty(BOX_MOVIE_CTROL, "state", "hide");

	GUI_SetProperty(TEXT_START_TIME, "state", "hide");
	GUI_SetProperty(TEXT_STOP_TIME, "state", "hide");
	GUI_SetProperty(SLIDER_CURSOR_MOVICE, "state", "hide");
	GUI_SetProperty(SLIDERBAR_MOVICE, "state", "hide");
	GUI_SetProperty(IMG_SLIDER_BACK, "state", "hide");
	ret_value = EVENT_TRANSFER_STOP;
#endif

	s_seek_flag = MOVIE_SEEK_NONE;
	//box_timer_hide = NULL;
	APP_TIMER_REMOVE(box_sliderbar_update_timer);
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_resume();
#endif
	APP_TIMER_REMOVE(box_timer_hide);
    s_ctl_box_show = false;

	return ret_value;
}

static void movie_view_info_get(MediaInfo *media_info)
{
	PlayerProgInfo  play_info;
	float size_orignal = 0;
	float size_human = 0;

	static play_movie_info info;
	static char str_size[20] = {0};
	static char str_res[20] = {0};
	static char str_bit[20] = {0};
	static char str_fps[20] = {0};
	
	memset(str_size, 0, sizeof(str_size));
	memset(str_res, 0, sizeof(str_res));
	memset(str_bit, 0, sizeof(str_bit));
	memset(str_fps, 0, sizeof(str_fps));
	memset(&info, 0, sizeof(play_movie_info));
	play_movie_get_info(&info);

	memset(&play_info, 0, sizeof(MediaInfo));
	GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &play_info);

	media_info->line[0].subt = "File name:";
	media_info->line[0].info = info.name;


	media_info->line[1].subt = "File Size:";
	if(play_info.file_size)	
	{		
		size_orignal = play_info.file_size;

		if(size_orignal >= (1024*1024*1024))
		{
			size_human = size_orignal / (1024*1024*1024);
			sprintf(str_size, "%3.2f GB", size_human);
		}
		else if(size_orignal >= (1024*1024))
		{
			size_human = size_orignal / (1024*1024);
			sprintf(str_size, "%3.2f MB", size_human);
		}
		else if(size_orignal >= (1024))
		{
			size_human = size_orignal / (1024);
			sprintf(str_size, "%3.2f KB", size_human);
		}
		else
		{
			size_human = size_orignal;
			sprintf(str_size, "%3.2f B", size_human);
		}
		media_info->line[1].info = str_size;
	}	

	media_info->line[2].subt = "Resolution:";
	if(play_info.video.width)	
	{		
		sprintf(str_res, "%d x %d", play_info.video.width,play_info.video.height);		
		media_info->line[2].info = str_res;
	}

	media_info->line[3].subt = "Code format:";
	if(play_info.video.codec_id)	
		media_info->line[3].info = play_info.video.codec_id;

	media_info->line[4].subt = "Bit rate:";
	if(play_info.video.bitrate)	
	{		
		sprintf(str_bit, "%d bps",play_info.video.bitrate);	
		media_info->line[4].info = str_bit;
	}

	media_info->line[5].subt = "Frame rate:";
	if(play_info.video.fps)	
	{		
		sprintf(str_fps, "%3.1f fps", play_info.video.fps);	
		media_info->line[5].info = str_fps;
	}
}

status_t movie_box_ctrl(movie_box_button button)
{
	status_t ret = GXCORE_SUCCESS;
	play_movie_info info;

	switch(button)
	{
		case MOVIE_BOX_BUTTON_PREVIOUS:
            {
            int slider_value = 0;
            GUI_SetProperty(SLIDERBAR_MOVICE, "value", &slider_value);
            GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &slider_value);

			/*reset speed state*/
			movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
			movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
			/*reset play state*/
			//if(PLAY_MOVIE_CTROL_PLAY != movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				movie_box_button2_setimg(movie_box_button2_ctrol);
			}
			/*reset zoom state*/
			if(PLAY_MOVIE_ZOOM_X1 != movie_box_button6_zoom)
			{
				movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
				//TODO: if gxplayer not reset, reset it
				play_movie_zoom(movie_box_button6_zoom);
			}			


			/*previous*/
			s_movie_total_time=0;
			ret =play_movie_ctrol(PLAY_MOVIE_CTROL_PREVIOUS);
			popmsg_topright_ctrl(PLAY_MOVIE_CTROL_PREVIOUS, 0);
			
			play_movie_get_info(&info);
			movie_box_setinfo(&info);
            GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info.name); 
			break;
            }
#if (TRICK_PLAY_SUPPORT > 0)
		case MOVIE_BOX_BUTTON_SPEED_BACKWARD:
			if(s_player_info.is_radio == 1 || s_movie_video_ok == false)// radio not support
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = STR_ID_FILE_NOT_SUPPORT;
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;						
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_OK);
				ret =  GXCORE_ERROR;
				break;
			}
			
			/*turn to playing, then speed*/
			if(PLAY_MOVIE_CTROL_PAUSE == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
		#if 0
				//before pause, it is in fast play mode,we should clear the status. 
				movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
				movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
				play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);

				movie_box_button2_setimg(movie_box_button2_ctrol);
				popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
				play_movie_ctrol(PLAY_MOVIE_CTROL_RESUME);
		#endif
				goto BACKSPEED;
			}
			else if(PLAY_MOVIE_CTROL_STOP == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				movie_box_button2_setimg(movie_box_button2_ctrol);
				popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
				app_movie_view_end_dialog();
				play_movie_ctrol(PLAY_MOVIE_CTROL_PLAY);
				movie_box_show();
			}
			else
			{
				/*speed*/
BACKSPEED:		movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
				if(PLAY_MOVIE_SPEED_X32 <= movie_box_button1_bspeed)
				{
					movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button2_setimg(PLAY_MOVIE_CTROL_PLAY);
				}
				else
				{
                    movie_box_button1_bspeed *= 2;
                    movie_box_button2_setimg(PLAY_MOVIE_CTROL_PAUSE);
				}
				popmsg_topright_ctrl(PLAY_MOVIE_CTROL_BACKWARD, movie_box_button1_bspeed);

				play_movie_speed(PLAY_MOVIE_CTROL_BACKWARD, movie_box_button1_bspeed);
			}
			break;
#endif

		case MOVIE_BOX_BUTTON_PLAY_PAUSE:
			if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
			{
#if (TRICK_PLAY_SUPPORT > 0)
				if((movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
					|| (movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1))
				{
					movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;

					popmsg_topright_ctrl(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);
					play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);
				}
				else
#endif
				{
					movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PAUSE;
					play_movie_ctrol(PLAY_MOVIE_CTROL_PAUSE);
				}
			}
			else if(PLAY_MOVIE_CTROL_PAUSE == movie_box_button2_ctrol)
			{
#if (TRICK_PLAY_SUPPORT > 0)
				if((movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
					|| (movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1))
				{
					movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
					play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);
				}
#endif				
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				play_movie_ctrol(PLAY_MOVIE_CTROL_RESUME);
			}
			else if(PLAY_MOVIE_CTROL_STOP == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				play_movie_ctrol(PLAY_MOVIE_CTROL_PLAY);
			}
			movie_box_button2_setimg(movie_box_button2_ctrol);
			popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
			break;
			
		// add the pause mode for pause key	
		case MOVIE_BOX_BUTTON_PAUSE:
			if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PAUSE;
				play_movie_ctrol(PLAY_MOVIE_CTROL_PAUSE);
			}
			else if(PLAY_MOVIE_CTROL_PAUSE == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				play_movie_ctrol(PLAY_MOVIE_CTROL_RESUME);
			}
			else 
			{
				break;
			}
			movie_box_button2_setimg(movie_box_button2_ctrol);
			popmsg_topright_ctrl(movie_box_button2_ctrol, 0);

			//recover the fast play mode display
			if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
			{
#if (TRICK_PLAY_SUPPORT > 0)
				if(movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
					popmsg_topright_ctrl(PLAY_MUSIC_CTROL_BACKWARD, movie_box_button1_bspeed);
				else if(movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1)
					popmsg_topright_ctrl(PLAY_MOVIE_CTROL_FORWARD, movie_box_button3_fspeed);
#endif		
			}
			break;

		case MOVIE_BOX_BUTTON_PLAY:
			if(PLAY_MOVIE_CTROL_PAUSE== movie_box_button2_ctrol)
			{
#if (TRICK_PLAY_SUPPORT > 0)
				if((movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
					|| (movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1))
				{
					movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
					play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);
				}
#endif				
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				movie_box_button2_setimg(movie_box_button2_ctrol);
				popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
				play_movie_ctrol(PLAY_MOVIE_CTROL_RESUME);		
			}
			else if(PLAY_MOVIE_CTROL_STOP== movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				play_movie_ctrol(PLAY_MOVIE_CTROL_PLAY);
			}
#if (TRICK_PLAY_SUPPORT > 0)			
			else if((PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
					&& ((movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
					|| (movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1)))
			{
					movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
					play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, movie_box_button3_fspeed);		
			}
#endif			
			else
			{
				break;
			}
			movie_box_button2_setimg(movie_box_button2_ctrol);
			popmsg_topright_ctrl(PLAY_MOVIE_CTROL_PLAY, 0);
			
			break;	

#if (TRICK_PLAY_SUPPORT > 0)
		case MOVIE_BOX_BUTTON_SPEED_FORWARD:
			if(s_player_info.is_radio == 1  || s_movie_video_ok == false)// radio not support
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = STR_ID_FILE_NOT_SUPPORT;
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
                popdlg_create(&pop);
				ret =  GXCORE_ERROR;
				break;
			}
			/*turn to playing, then speed*/
			if(PLAY_MOVIE_CTROL_PAUSE == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
		#if 0
				
				
				//before pause, it is in fast play mode,we should clear the status.
				movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
				movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
				play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, PLAY_MOVIE_SPEED_X1);
				
				movie_box_button2_setimg(movie_box_button2_ctrol);
				popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
				play_movie_ctrol(PLAY_MOVIE_CTROL_RESUME);
		#endif
				goto FORWARDSPEED;
			}
			else if(PLAY_MOVIE_CTROL_STOP == movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				movie_box_button2_setimg(movie_box_button2_ctrol);
				popmsg_topright_ctrl(movie_box_button2_ctrol, 0);
				app_movie_view_end_dialog();
				play_movie_ctrol(PLAY_MOVIE_CTROL_PLAY);
				movie_box_show();
			}
			else
			{
				/*speed*/
FORWARDSPEED:	movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
				if(PLAY_MOVIE_SPEED_X32 <= movie_box_button3_fspeed)
				{
					movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
					movie_box_button2_setimg(PLAY_MOVIE_CTROL_PLAY);
				}
				else
				{
					movie_box_button3_fspeed *= 2;
					movie_box_button2_setimg(PLAY_MOVIE_CTROL_PAUSE);
				}
				popmsg_topright_ctrl(PLAY_MOVIE_CTROL_FORWARD, movie_box_button3_fspeed);
				play_movie_speed(PLAY_MOVIE_CTROL_FORWARD, movie_box_button3_fspeed);
			}
            play_movie_get_info(&info);
            movie_box_setinfo(&info);
            GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info.name);

			break;
#endif
		case MOVIE_BOX_BUTTON_NEXT:
            {
            int slider_value = 0;
            GUI_SetProperty(SLIDERBAR_MOVICE, "value", &slider_value);       
            GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &slider_value);       
			/*reset speed state*/
			movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
			movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
			/*reset play state*/
			//if(PLAY_MOVIE_CTROL_PLAY != movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
				movie_box_button2_setimg(movie_box_button2_ctrol);
			}
			/*reset zoom state*/
			if(PLAY_MOVIE_ZOOM_X1 != movie_box_button6_zoom)
			{
				movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
				//TODO: if gxplayer not reset, reset it
				play_movie_zoom(movie_box_button6_zoom);
			}	
			

			/*next*/
			s_movie_total_time=0;
			ret =play_movie_ctrol(PLAY_MOVIE_CTROL_NEXT);
			popmsg_topright_ctrl(PLAY_MOVIE_CTROL_NEXT, 0);
			if(NULL != delay_play_timer)
			{
				APP_TIMER_REMOVE(delay_play_timer);
			}
			if (GXCORE_SUCCESS == GUI_CheckDialog("win_movie_set"))
			{
				GUI_EndDialog("win_movie_set");
			}

            play_movie_get_info(&info);
            movie_box_setinfo(&info);
            GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info.name);

			GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "hide");
			GUI_SetInterface("flush", NULL);
			break;
            }
		case MOVIE_BOX_BUTTON_STOP:
			/*reset speed state*/
			movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
			movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;

#if (MINI_16_BITS_OSD_SUPPORT || MINI_16BIT_WIN8_OSD_SUPPORT)
			if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MEDIA_BAR))
#endif
			{
				//#error 10559	
				int sliderbar_init = 0;
				char buf_tmp[20]= "00:00:00";

				s_movie_total_time = 0;
				
				GUI_SetProperty(SLIDERBAR_MOVICE, "value", &sliderbar_init);
				GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
				GUI_SetProperty(TEXT_STOP_TIME, "string", buf_tmp);	
			}
			/*reset zoom state*/
			if(PLAY_MOVIE_ZOOM_X1 != movie_box_button6_zoom)
			{
				movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
				//TODO: if gxplayer not reset, reset it
				play_movie_zoom(movie_box_button6_zoom);
			}			

			/*stop*/
			if(PLAY_MOVIE_CTROL_STOP != movie_box_button2_ctrol)
			{
				movie_box_button2_ctrol = PLAY_MOVIE_CTROL_STOP;
				movie_box_button2_setimg(movie_box_button2_ctrol);
			}
			popmsg_topright_ctrl(PLAY_MOVIE_CTROL_STOP, 0);

			play_movie_ctrol(PLAY_MOVIE_CTROL_STOP);
			break;
			
		case MOVIE_BOX_BUTTON_ZOOM:			
			if(PLAY_MOVIE_ZOOM_X16 <= movie_box_button6_zoom)
			{
				movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
			}
			else
			{
				movie_box_button6_zoom++;
			}
			popmsg_topright_ctrl(PLAY_MOVIE_CTROL_ZOOM, movie_box_button6_zoom);

			play_movie_zoom(movie_box_button6_zoom);
			break;
#if MEDIA_SUBTITLE_SUPPORT
		case MOVIE_BOX_BUTTON_LRC:
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);
			if(popmsg_centre_timer_hide)
			{
				APP_TIMER_REMOVE(popmsg_centre_timer_hide);
				GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");
				GUI_SetProperty(COMBOBOX_POPMSG, "draw_now", NULL);
			}
			GUI_CreateDialog("win_movie_subt");
			break;
#endif			
		case MOVIE_BOX_BUTTON_SET:
			if(movie_box_button1_bspeed == PLAY_MOVIE_SPEED_X1
				&&movie_box_button3_fspeed == PLAY_MOVIE_SPEED_X1//Normal Play
				&&movie_box_button2_ctrol != PLAY_MOVIE_CTROL_STOP)
			{
				movie_sliderbar_timer_stop();
				movie_box_hide(NULL);
				if(popmsg_centre_timer_hide)
				{
					APP_TIMER_REMOVE(popmsg_centre_timer_hide);
					GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");
					GUI_SetProperty(COMBOBOX_POPMSG, "draw_now", NULL);
				}
				GUI_CreateDialog("win_movie_set");
			}
			else
			{
				//Notice for can't support setting!
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "Can't Support!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.timeout_sec = 6;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);											
                popdlg_create(&pop);
			}
			break;
			
		case MOVIE_BOX_BUTTON_INFO:
			if(movie_box_button2_ctrol != PLAY_MOVIE_CTROL_STOP)
			{
				movie_sliderbar_timer_stop();
				movie_box_hide(NULL);
				if(popmsg_centre_timer_hide)
				{
					APP_TIMER_REMOVE(popmsg_centre_timer_hide);
					GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");
					GUI_SetProperty(COMBOBOX_POPMSG, "draw_now", NULL);
				}

				MediaInfo media_info;
				memset(&media_info, 0, sizeof(media_info));

				movie_view_info_get(&media_info);
				media_info.change_cb = movie_view_info_get;
				media_info.destroy_cb = NULL;
				media_info_create(&media_info);
			}
			else
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "Can't Support!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.timeout_sec = 6;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);											
                popdlg_create(&pop);
			}
			break;

#if MINI_16_BITS_OSD_SUPPORT
		case MOVIE_BOX_BUTTON_VOL:
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);
			if(popmsg_centre_timer_hide)
			{
				APP_TIMER_REMOVE(popmsg_centre_timer_hide);
				GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");
				GUI_SetProperty(COMBOBOX_POPMSG, "draw_now", NULL);
			}
			uint32_t value;
			value=pmpset_get_int(PMPSET_MUTE); 
			if(value==PMPSET_MUTE_ON)
			{  
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE_1, "state", "osd_trans_hide"); 
				GUI_SetProperty(IMAGE_MUTE, "state", "osd_trans_hide");
			}
			GUI_CreateDialog("win_volume");
			break;
#endif

		default:
			ret =  GXCORE_ERROR;
			break;
	}

		
	return ret;
}

static status_t movie_box_timer_start(void)
{
	APP_TIMER_ADD(box_timer_hide, movie_box_hide, 10000, TIMER_REPEAT);
	return GXCORE_SUCCESS;
}

static status_t movie_box_timer_restart(void)
{
	reset_timer(box_timer_hide);	
	return GXCORE_SUCCESS;
}

static int movie_sliderbar_update(void* userdata)
{
	play_movie_info cur_info;

	/*set info*/
	play_movie_get_info(&cur_info);
	movie_box_setinfo(&cur_info);
	return 0;
}
static status_t movie_sliderbar_timer_start(void)
{
	APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);
	return GXCORE_SUCCESS;
}

static status_t movie_sliderbar_timer_stop(void)
{
	APP_TIMER_REMOVE(box_sliderbar_update_timer);
	return GXCORE_SUCCESS;
}


static status_t movie_sliderbar_okkey(void)
{
	long cur_time_ms = 0;
	int value=0;
	GxMessage *msg;	
	GxMsgProperty_PlayerSeek *seek;

	GUI_GetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
	if(s_movie_total_time>0){
		cur_time_ms=s_movie_total_time*value/100;
		if((cur_time_ms > s_movie_total_time)||(0 == s_movie_total_time))
			return GXCORE_ERROR;
	}


	if(movie_box_button2_ctrol == PLAY_MOVIE_CTROL_STOP)
	{
		play_list* list = NULL;
		char* path = NULL;
		
		/*play*/
		list = play_list_get(PLAY_LIST_TYPE_MOVIE);
		if(NULL == list) return GXCORE_ERROR;
		
		path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
		if(NULL == path) return GXCORE_ERROR;
		
		printf("[PLAY] movie: %s\n", path);
		play_av_by_url(path, cur_time_ms);

		GUI_SetProperty(TEXT_MOVIE_STATUS, "string", "Seeking...");
#ifdef MEDIA_PLAY_DISPLAY_MODE
		app_movie_view_end_dialog();			
#endif
		GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "show");
		GUI_SetProperty(TEXT_MOVIE_STATUS, "draw_now", NULL);

		GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
//		GUI_SetProperty(TEXT_POPMSG, "state", "hide");
		
		movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;				
		movie_box_button2_setimg(movie_box_button2_ctrol);
	}
	else
	{						
		msg = GxBus_MessageNew(GXMSG_PLAYER_SEEK);
		APP_CHECK_P(msg, 1);
		seek = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerSeek);
		APP_CHECK_P(seek, 1);
		seek->player = PMP_PLAYER_AV;
		seek->time = cur_time_ms;
		seek->flag = SEEK_ORIGIN_SET;
		
		GUI_SetProperty(TEXT_MOVIE_STATUS, "string", "Seeking...");
		GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "show");
		GUI_SetProperty(TEXT_MOVIE_STATUS, "draw_now", NULL);
		
		GxBus_MessageSendWait(msg);
		GxBus_MessageFree(msg);
		
		//movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
		GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "osd_trans_hide");
		GUI_SetProperty(TEXT_MOVIE_STATUS, "draw_now", NULL);

		movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
	
		//GxCore_ThreadDelay(2000);//解决进度条回退问题，以后需要驱动解决
	}

	s_seek_flag = MOVIE_SEEK_NONE;
	APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);
	
	return GXCORE_SUCCESS;
}


static status_t movie_sliderbar_active(void* usrdata)
{
//	movie_sliderbar_okkey();

    APP_TIMER_REMOVE(box_sliderbar_active_timer);
    s_seek_flag = MOVIE_SEEKED;
	APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);

	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int  movie_view_sliderbar_keypress(const char* widgetname, void *usrdata)
{
#if 0
	GUI_Event *event = NULL;
	int64_t cur_time_ms = 0;
	int32_t cur_time_s = 0;
	int value=0,step=0;
	struct tm *ptm = NULL;
	char buf_tmp[20];

	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	movie_box_timer_restart();
	
	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_BACK:
		case APPK_MENU:
			movie_box_timer_stop();
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);
			return EVENT_TRANSFER_STOP;

		case APPK_VOLDOWN:
		case APPK_LEFT:
		//case APPK_REW://tmp
			if(0 == s_movie_total_time) break;

			GUI_GetProperty(SLIDERBAR_MOVICE, "value", &value);
			GUI_GetProperty(SLIDERBAR_MOVICE, "step", &step);
			if(value-step<0)
			{
				value=0;
			}
			else
			{
				value-=step;
			}
			cur_time_ms=s_movie_total_time*value/100;
			cur_time_s = cur_time_ms / 1000;
			cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
			ptm = localtime((const time_t*)  &cur_time_s);
			sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
			GUI_SetProperty(SLIDERBAR_MOVICE, "value", &value);

			APP_TIMER_REMOVE(box_sliderbar_update_timer);
			APP_TIMER_ADD(box_sliderbar_active_timer, movie_sliderbar_active, 2000, TIMER_ONCE);
			return EVENT_TRANSFER_STOP;

		case APPK_VOLUP:
		case APPK_RIGHT:
		//case APPK_FF://tmp
			if(0 == s_movie_total_time) break;

			GUI_GetProperty(SLIDERBAR_MOVICE, "value", &value);
			GUI_GetProperty(SLIDERBAR_MOVICE, "step", &step);
			if(value+step>100)
			{
				value=100;
			}
			else
			{
				value+=step;
			}
			cur_time_ms=s_movie_total_time*value/100;
			cur_time_s = cur_time_ms / 1000;
			cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
			ptm = localtime((const time_t*)  &cur_time_s);
			sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
			GUI_SetProperty(SLIDERBAR_MOVICE, "value", &value);

			APP_TIMER_REMOVE(box_sliderbar_update_timer);
			APP_TIMER_ADD(box_sliderbar_active_timer, movie_sliderbar_active, 2000, TIMER_ONCE);
			return EVENT_TRANSFER_STOP;
			
		case APPK_OK:
			APP_TIMER_REMOVE(box_sliderbar_active_timer);
			movie_sliderbar_okkey();
			return EVENT_TRANSFER_STOP;

		case APPK_UP:
			return EVENT_TRANSFER_STOP;
		case APPK_DOWN:
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			APP_TIMER_REMOVE(box_sliderbar_active_timer);
			APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 1000, TIMER_REPEAT);
			return EVENT_TRANSFER_STOP;
			
		default:
			APP_TIMER_REMOVE(box_sliderbar_active_timer);
			movie_box_timer_stop();
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);			
			return EVENT_TRANSFER_KEEPON;
	}
#endif	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int  movie_view_cursor_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int64_t cur_time_ms = 0;
	int32_t cur_time_s = 0;
	int value=0,step=0;
	struct tm *ptm = NULL;
	char buf_tmp[20];
	play_movie_info info;

	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	movie_box_timer_restart();
	
	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_BACK:
		case APPK_MENU:
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);
			return EVENT_TRANSFER_STOP;

		case APPK_LEFT:
			if(0 == s_movie_total_time)
				return EVENT_TRANSFER_STOP;

          APP_TIMER_REMOVE(box_sliderbar_update_timer);
			s_seek_flag = MOVIE_SEEKING;

			GUI_GetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
			GUI_GetProperty(SLIDER_CURSOR_MOVICE, "step", &step);
			if(value-step<0)
			{
				value=0;
			}
			else
			{
				value-=step;
			}
			cur_time_ms=s_movie_total_time*value/100;
			cur_time_s = cur_time_ms / 1000;
			cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
			ptm = localtime((const time_t*)  &cur_time_s);
			sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
			GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);

			APP_TIMER_ADD(box_sliderbar_active_timer, movie_sliderbar_active, 100, TIMER_REPEAT);
			return EVENT_TRANSFER_STOP;

		case APPK_RIGHT:
			if(0 == s_movie_total_time)
				return EVENT_TRANSFER_STOP;

          APP_TIMER_REMOVE(box_sliderbar_update_timer);
			s_seek_flag = MOVIE_SEEKING;

			GUI_GetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
			GUI_GetProperty(SLIDER_CURSOR_MOVICE, "step", &step);
			if(value+step>100)
			{
				value=100;
			}
			else
			{
				value+=step;
			}
			cur_time_ms=s_movie_total_time*value/100;
			cur_time_s = cur_time_ms / 1000;
			cur_time_s += ((cur_time_ms % 1000) == 0 ? 0 : 1);
			ptm = localtime((const time_t*)  &cur_time_s);
			sprintf(buf_tmp, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);
			GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);

			APP_TIMER_ADD(box_sliderbar_active_timer, movie_sliderbar_active, 100, TIMER_REPEAT);
			return EVENT_TRANSFER_STOP;

		case APPK_OK:
            if(s_seek_flag == MOVIE_SEEKED)
            {
			    APP_TIMER_REMOVE(box_sliderbar_active_timer);
			    movie_sliderbar_okkey();
            }
			return EVENT_TRANSFER_STOP;

		case APPK_UP:
			return EVENT_TRANSFER_STOP;

		case APPK_DOWN:
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			s_seek_flag = MOVIE_SEEK_NONE;
			play_movie_get_info(&info);
			movie_box_setinfo(&info);
			APP_TIMER_REMOVE(box_sliderbar_active_timer);
			APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);
			return EVENT_TRANSFER_STOP;
			
		default:
#ifndef MINI_16_BITS_OSD_SUPPORT
			APP_TIMER_REMOVE(box_sliderbar_active_timer);
			movie_sliderbar_timer_stop();
			movie_box_hide(NULL);			
#endif
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}

static int movie_box_show(void)
{
	play_movie_info info;

	/*show*/
#ifdef MEDIA_PLAY_DISPLAY_MODE
	app_movie_view_create();
#else
	GUI_SetProperty(IMAGE_BOX_BACK, "state", "show");
#if MINI_16_BITS_OSD_SUPPORT
	GUI_SetProperty(IMAGE_MOVIE_TITLE, "state", "show");
#endif
	GUI_SetProperty(TEXT_MOVIE_NAME1, "state", "show");
	GUI_SetProperty(BOX_MOVIE_CTROL, "state", "show");

	GUI_SetProperty(TEXT_START_TIME, "state", "show");
	GUI_SetProperty(TEXT_STOP_TIME, "state", "show");
	GUI_SetProperty(IMG_SLIDER_BACK, "state", "show");
	GUI_SetProperty(SLIDER_CURSOR_MOVICE, "state", "show");
	GUI_SetProperty(SLIDERBAR_MOVICE, "state", "show");
#endif
	
	/*set info*/
	play_movie_get_info(&info);
	movie_box_setinfo(&info);
	GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info.name);	
	/*timer start*/
	movie_box_timer_start();
	movie_sliderbar_timer_start();
	s_seek_flag = MOVIE_SEEK_NONE;

	movie_box_button2_setimg(movie_box_button2_ctrol);

	int item_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
	GUI_SetProperty(BOX_MOVIE_CTROL, "select", (void*)&item_sel);
	GUI_SetFocusWidget(SLIDER_CURSOR_MOVICE);
#ifdef MEDIA_PLAY_DISPLAY_MODE
	GUI_SetProperty("win_media_bar", "update", NULL);
#endif
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_pause();
#endif
    s_ctl_box_show = true;

	return 0;
}

SIGNAL_HANDLER int movie_view_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t item_sel = 0;
	int ret_value = 0;

	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			movie_box_timer_restart();
			
			switch(event->key.sym)
			{
				case APPK_BACK:
				case APPK_MENU:
					movie_sliderbar_timer_stop();
					ret_value = movie_box_hide(NULL);
					return ret_value;
					
				case APPK_OK:
					GUI_GetProperty(BOX_MOVIE_CTROL, "select", (void*)&item_sel);
					if(MOVIE_BOX_BUTTON_STOP == item_sel)
					{
						APP_TIMER_REMOVE(box_sliderbar_update_timer);

						s_movie_manual_stop = TRUE;
					}
					else
					{
						if (MOVIE_BOX_BUTTON_SET != item_sel && MOVIE_BOX_BUTTON_INFO != item_sel)
						{
							APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);
						}
					}

					if (MOVIE_BOX_BUTTON_PREVIOUS == item_sel || MOVIE_BOX_BUTTON_NEXT == item_sel)
					{
						movie_view_box_reset();
					}
						
					movie_box_ctrl(item_sel);
					return EVENT_TRANSFER_STOP;
					break;

				case APPK_SEEK:
					GUI_SetFocusWidget(SLIDER_CURSOR_MOVICE);
					return EVENT_TRANSFER_STOP;
				case APPK_DOWN:
					return EVENT_TRANSFER_STOP;
				case APPK_UP:
					GUI_SetFocusWidget(SLIDER_CURSOR_MOVICE);
					return EVENT_TRANSFER_STOP;	
				default:
					break;
			}
			break;
		default:
			break;
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int movie_view_got_focus(const char* widgetname, void *usrdata)
{
#if MEDIA_SUBTITLE_SUPPORT
    if(s_ctl_box_show == false)
        subtitle_resume();
#endif
	movie_box_timer_start();
//	popmsg_topright_hide(NULL);


	PlayerStatusInfo info;
	if(GXCORE_SUCCESS == GxPlayer_MediaGetStatus(PMP_PLAYER_AV, &info))
	{
		if(info.status == PLAYER_STATUS_PLAY_PAUSE)
		{
			GUI_SetProperty(IMAGE_POPMSG, "state", "show");
//			GUI_SetProperty(TEXT_POPMSG, "state", "hide");
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PAUSE);
		}
	}

	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER  int movie_view_lost_focus(const char* widgetname, void *usrdata)
{
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_pause();
#endif

	return EVENT_TRANSFER_STOP;
}

static int _onlyonce_play(void* userdata)
{
    pmpset_movie_play_sequence sequence = 0;

    sequence = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
    if(PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE == sequence)
    {
        APP_Printf("play only once\n");
        movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
        GUI_EndDialog("after win_movie_view");
        GUI_EndDialog(WIN_MOVIE_VIEW);
#ifdef MEDIA_PLAY_DISPLAY_MODE
        app_movie_view_end_dialog();
#endif
    }
    APP_TIMER_REMOVE(delay_play_timer);
    return 0;
}

static int _sequence_play(void* userdata)
{
	play_list* list = NULL;
	list = play_list_get(PLAY_LIST_TYPE_MOVIE);

	if(NULL == list) return GXCORE_ERROR;

	APP_Printf("play sequence\n");
	if(list->play_no >= list->nents -1)
	{
		movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
	}
	else
	{
		movie_box_ctrl(MOVIE_BOX_BUTTON_NEXT);
	}
	APP_TIMER_REMOVE(delay_play_timer);
	return 0;
}

static int movie_play_status_prompt_info(GxMsgProperty_PlayerStatusReport* player_status)
{
	if(player_status == NULL)
		return -1;
	if(player_status->error == PLAYER_ERROR_AUDIO_DECODER_ERROR)
	{
        GUI_EndDialog("win_volume");
	    PopDlg pop;
	    memset(&pop, 0, sizeof(PopDlg));
	    pop.type = POP_TYPE_OK;
	    pop.format = POP_FORMAT_DLG;
	    pop.str = "Can't play audio.";
	    pop.mode = POP_MODE_UNBLOCK;
	    pop.timeout_sec = 5;
		pop.pos.x = POP_MEDIA_X_START;
		pop.pos.y = POP_MEDIA_Y_START;
//		media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);		
	    popdlg_create(&pop);
	    
	}
	else if(player_status->error == PLAYER_ERROR_VIDEO_DECODER_ERROR)
	{
        GUI_EndDialog("win_volume");
		s_movie_video_ok = false; //video err, can't support play_movie_speed 
	    PopDlg pop;
	    memset(&pop, 0, sizeof(PopDlg));
	    pop.type = POP_TYPE_OK;
	    pop.format = POP_FORMAT_DLG;
	    pop.str = "Can't play video.";
	    pop.mode = POP_MODE_UNBLOCK;
	    pop.timeout_sec = 5;
		pop.pos.x = POP_MEDIA_X_START;
		pop.pos.y = POP_MEDIA_Y_START;
//		media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);	
	    popdlg_create(&pop);
	}
	return 0;
}

static int movie_service_status(GxMsgProperty_PlayerStatusReport* player_status)
{
	play_list* list = NULL;
	char* path = NULL;
	play_movie_info inf;
	pmpset_movie_play_sequence sequence = 0;
	APP_CHECK_P(player_status, 1);

	/*check this msg owner*/
	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	if(NULL == list) return 1;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	if(strcmp(path, player_status->url) != 0)
	{
		if(music_play_state() == TRUE)
		//((player_status->status == PLAYER_STATUS_PLAY_START) && (music_play_state() == TRUE))
		{
			music_status_init();
		}
		return 0;
	}

	switch(player_status->status)
	{
		case PLAYER_STATUS_ERROR:
			APP_Printf("\n[SERVICE] PLAYER_STATUS_ERROR, %d\n", player_status->error);

			GUI_SetProperty(TEXT_MOVIE_STATUS, "string", "Can not play the file!");
			GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "show");
			GUI_SetProperty(TEXT_MOVIE_STATUS, "draw_now", NULL);

			play_movie_ctrol(PLAY_MOVIE_CTROL_STOP);
			int sliderbar_init = 0;
			GUI_SetProperty(SLIDERBAR_MOVICE, "value", &sliderbar_init);
			char buf_tmp[20]= "00:00:00";
			GUI_SetProperty(TEXT_START_TIME, "string", buf_tmp);

			GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &sliderbar_init);

			s_movie_total_time=0;
			sequence = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
			if(PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE == sequence)
			{
				APP_TIMER_ADD(box_sliderbar_update_timer, movie_sliderbar_update, 500, TIMER_REPEAT);
				APP_TIMER_ADD(delay_play_timer, _sequence_play, 3000, TIMER_REPEAT);
			}
            if(PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE == sequence)
            {
                APP_TIMER_ADD(delay_play_timer, _onlyonce_play, 3000, TIMER_REPEAT);
            }
			break;

		case PLAYER_STATUS_PLAY_END:
			APP_TIMER_REMOVE(delay_play_timer);
			APP_Printf("\n[SERVICE] PLAYER_STATUS_PLAY_END\n");

			int value = 0;
			
			GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "hide");
			GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
			
			s_seek_flag = MOVIE_SEEK_NONE; 
			s_movie_total_time=0;
			s_start_time_ms = 0;
			pmp_save_tag(path, s_start_time_ms);
			if(TRUE == app_get_pop_msg_flag_status())
			{
				GUI_EndDialog("win_media_popmsg_ok");
				GUI_SetInterface("flush", NULL);
			}
			/*sequence*/
			sequence = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
			if(PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE == sequence)
			{
				APP_Printf("play only once\n");
				movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
				GUI_EndDialog("after win_movie_view");
				GUI_EndDialog(WIN_MOVIE_VIEW);
#ifdef MEDIA_PLAY_DISPLAY_MODE
				app_movie_view_end_dialog();
#endif
				break;
			}
			else if(PMPSET_MOVIE_PLAY_SEQUENCE_REPEAT_ONE == sequence)
			{
				int value = 0;
				GUI_SetProperty(SLIDERBAR_MOVICE, "value", &value);
				GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
				GUI_SetInterface("flush", NULL);
				if(s_movie_manual_stop == TRUE)
				{
					movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
					GUI_EndDialog("after win_movie_view");
					GUI_EndDialog(WIN_MOVIE_VIEW);	
				}
				else
				{
					APP_Printf("play repeat one\n");
					if((movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
							||(movie_box_button3_fspeed != PLAY_MOVIE_SPEED_X1))
					{
						movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
						movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
#ifdef MEDIA_PLAY_DISPLAY_MODE
						app_movie_view_end_dialog();
#endif
					}
					else
					{
						GUI_EndDialog("after win_movie_view");
						play_movie_ctrol(PLAY_MOVIE_CTROL_PLAY);
					}
				}
				break;
			}
			/*else if(PMPSET_MOVIE_PLAY_SEQUENCE_RANDOM == sequence)
			  {
			  APP_Printf("play random\n");

			//rand cur play_no, then play next
			list = play_list_get(PLAY_LIST_TYPE_MOVIE);
			if(NULL == list) return GXCORE_ERROR;	
			int random_no = rand();
			random_no = random_no%list->nents;
			if(0 == list->play_no)
			{
			if(random_no == list->nents - 1)
			list->play_no =  0;
			else
			list->play_no = random_no;
			}
			else
			{
			if(random_no != list->play_no - 1)
			{
			list->play_no = random_no;
			}
			}

			movie_box_ctrl(MOVIE_BOX_BUTTON_NEXT);
			break;
			}*/
			else if(PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE == sequence)
			{
				list = play_list_get(PLAY_LIST_TYPE_MOVIE);
				if(NULL == list) return GXCORE_ERROR;					

				APP_Printf("play sequence\n");
				if(GUI_CheckDialog("win_media_info") == GXCORE_SUCCESS)
				{
					GUI_EndDialog("win_media_info");
				}
				if(movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1)
				{
					movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
					movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
				}
				else
				{
					if(list->play_no >= list->nents -1)
					{
						GUI_EndDialog("after win_movie_view");
						GUI_EndDialog(WIN_MOVIE_VIEW);			   
//#ifdef MEDIA_PLAY_DISPLAY_MODE
//						app_movie_view_end_dialog();
//#endif
					}
					else if(s_movie_manual_stop == TRUE)
					{
						movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
					}
					else
					{
						movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
						movie_box_ctrl(MOVIE_BOX_BUTTON_NEXT);
					}
				}
#ifdef MEDIA_PLAY_DISPLAY_MODE
						app_movie_view_end_dialog();
#endif
				break;
			}
			else
			{
				APP_Printf("play none\n");
				movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
				break;
			}
			break;

        case PLAYER_STATUS_PLAY_RUNNING:
            movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;                                                                               
            popmsg_topright_ctrl(movie_box_button2_ctrol, 0); 
            movie_box_button2_setimg(movie_box_button2_ctrol);

			s_movie_video_ok = true;
			movie_box_timer_restart();
#if (MINI_256_COLORS_OSD_SUPPORT == 0)
			movie_play_status_prompt_info(player_status);
#endif
			APP_TIMER_REMOVE(delay_play_timer);
			APP_Printf("\n[SERVICE] PLAYER_STATUS_PLAY_RUNNING\n");	
			s_movie_total_time=0;

#if 1//(MINI_256_COLORS_OSD_SUPPORT == 0)
			{
				PlayerProgInfo  player_info;
				if(GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &player_info) == GXCORE_SUCCESS) //success
				{
					memcpy(&s_player_info, &player_info, sizeof(PlayerProgInfo));

					// add 20121112, for background display
					if(s_player_info.is_radio == 1)
					{
						if(false == RadioBackgroundDisplayFlag)
						{
							GUI_SetProperty(WIN_MOVIE_VIEW, "back_ground", (void*)"mediacenter.jpg");
							GUI_SetInterface("image_enable", NULL);
							RadioBackgroundDisplayFlag = true;
						}
					}
					else
					{
						if(true == RadioBackgroundDisplayFlag)
						{
							GUI_SetInterface("image_disable", NULL);
							RadioBackgroundDisplayFlag = false;
						}
					}
				}
				else
				{
					printf("get prog info err !!!\n");
				}
			}
#endif
			/*set info*/
			play_movie_get_info(&inf);
			movie_box_setinfo(&inf);
			s_movie_manual_stop = FALSE;
#if MEDIA_SUBTITLE_SUPPORT
			subtitle_start(NULL, PMP_SUBT_LOAD_INIT);
#endif
			break;

		case PLAYER_STATUS_PLAY_START:
			APP_Printf("\n[SERVICE] PLAYER_STATUS_PLAY_START\n");
			// add in 20121128--for audio delay status recover
			// driver will clear too, so app must do following code.
			{
				extern int audio_delay_ms;
				audio_delay_ms = 0;

				GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "hide");
				if(GXCORE_SUCCESS == GUI_CheckDialog("win_movie_set"))
					GUI_SetProperty("movie_set_button_delay", "string", "0ms");
			}
			break;

		default:
			break;
	}

	return 0;
}

void player_speed_change(GxMsgProperty_PlayerSpeedReport *spd)
{
#if (TRICK_PLAY_SUPPORT > 0)
	if(spd->speed == 1 && (movie_box_button1_bspeed != PLAY_MOVIE_SPEED_X1))
	{
		GUI_SetProperty(IMAGE_POPMSG, "state", "hide");
		GUI_SetProperty(TEXT_POPMSG, "state", "hide");
		GUI_SetInterface("flush", NULL);
		movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
	}
	if(GUI_CheckDialog("wnd_pop_tip") == GXCORE_SUCCESS)
		GUI_EndDialog("wnd_pop_tip");
#endif
}

SIGNAL_HANDLER  int movie_view_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;
	GxMsgProperty_PlayerAVCodecReport* avcodec_status = NULL;
	//GxMsgProperty_PlayerStatusReport player_status_fromavcodec;
	GxMsgProperty_PlayerSpeedReport *player_spd = NULL;

	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	event = (GUI_Event *)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			movie_service_status(player_status);

			break;

		case GXMSG_PLAYER_AVCODEC_REPORT:
			avcodec_status = (GxMsgProperty_PlayerAVCodecReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerAVCodecReport);
			if(AVCODEC_ERROR == avcodec_status->acodec.state || AVCODEC_ERROR == avcodec_status->vcodec.state
                    || AVCODEC_UNSUPPORT == avcodec_status->acodec.state || AVCODEC_UNSUPPORT == avcodec_status->vcodec.state)
			{
				player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
				player_status->status = PLAYER_STATUS_ERROR;
				movie_service_status(player_status);
				//player_status_fromavcodec.status = PLAYER_STATUS_ERROR;
				//movie_service_status(&player_status_fromavcodec);
			}
			#if MINI_16_BITS_OSD_SUPPORT
			movie_play_status_prompt_info(player_status);
			#endif
			break;
        case GXMSG_PLAYER_SPEED_REPORT:
			{
				player_spd = GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerSpeedReport);
				player_speed_change(player_spd);
			}
            break;
		case GXMSG_SUBTITLE_INIT:
			{
				GxMsgProperty_AppSubtitle* subt;
				subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
				app_subt_init(&subt->rect, subt->type);
			}
			break;
		case GXMSG_SUBTITLE_DESTROY:
			{
				GxMsgProperty_AppSubtitle* subt;
				subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
				//app_subt_destroy(subt->handle);
				app_subt_destroy(subt->handle, subt->type);
			}
			break;
		case GXMSG_SUBTITLE_HIDE:
			{
				GxMsgProperty_AppSubtitle* subt;
				subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
				app_subt_hide(subt->handle);
			}
			break;
		case GXMSG_SUBTITLE_SHOW:
			{
				GxMsgProperty_AppSubtitle* subt;
				subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
				app_subt_show(subt->handle);
			}
			break;
		case GXMSG_SUBTITLE_DRAW:
            {
                GxMsgProperty_AppSubtitle* subt;
                PlayerSubTextPage* subpg = NULL;
                int  i = 0;

                subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
                if(subt != NULL)
                {
                    app_subt_draw(subt->data, subt->num, subt->type);

                    subpg = (PlayerSubTextPage*)(subt->data);
                    if(subpg != NULL)
                    {
                        for(i = 0; i < subpg->lines; i++)
                        {
                            if(subpg->text[i])
                            {
                                GxCore_Free(subpg->text[i]);
                                subpg->text[i] = NULL;
                            }
                        }
                        GxCore_Free(subpg);
                        subpg = NULL;
                        subt->data = NULL;
                    }
                }
            }
			break;
		case GXMSG_SUBTITLE_CLEAR:
			{
				GxMsgProperty_AppSubtitle* subt;
				subt = (GxMsgProperty_AppSubtitle*)GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_AppSubtitle);
				app_subt_clear(subt->handle);
			}
			break;

		default:
			break;
	}

	return EVENT_TRANSFER_STOP;
}

static void movie_pvr_media_init(void)
{
#if 0
    int osd_alpha_global;

    //app_epg_disable();

    //GUI_SetProperty("wnd_pvr_media_list", "draw_now", NULL);

    GxBus_ConfigGetInt("osd>trans_level", &osd_alpha_global, 255);
    GUI_SetInterface("osd_alpha_global", &osd_alpha_global);
    //GUI_SetInterface("logic_clut", "MP_ICON_INFO.bmp");
    //GUI_SetInterface("logic_clut", "MP_ICON_MEDIAPLAYER.bmp");
#endif
}

static int _play_cb(PopDlgRet ret)
{
	HotplugPartitionList* partition_list = NULL;
	
	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
	if (partition_list->partition_num < 1)
	{
		return 1;
	}
	
    if(ret != POP_VAL_OK)
    {
        s_start_time_ms = 0;

        /*list get*/
        play_list* list = NULL;
        char* path = NULL;
        list = play_list_get(PLAY_LIST_TYPE_MOVIE);
        APP_CHECK_P(list, GXCORE_ERROR);
        if(list->play_no >= list->nents) return GXCORE_ERROR;
        path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
        APP_CHECK_P(path, GXCORE_ERROR);

        /*save tag*/
        pmp_save_tag(path, s_start_time_ms);
    }

    /*subt create*/
#if MEDIA_SUBTITLE_SUPPORT
    subtitle_create(TEXT_SUBT);
#endif

#ifdef MENCENT_FREEE_SPACE
	//GUI_SetInterface("flush", NULL);// ??????????,????Player???????????
	//GUI_SetInterface("free_space", "fragment|spp|osd|back_osd");
#endif
    /*play start*/
    play_movie(s_start_play_no, s_start_time_ms);
	movie_box_button2_setimg(movie_box_button2_ctrol);
    GUI_SetInterface("flush",NULL);//pp enable together with osd
    return 0;
}

SIGNAL_HANDLER int movie_view_init(const char* widgetname, void *usrdata)
{
	int iDialogRet = -1;
	uint32_t value = 0;
	status_t ret = GXCORE_SUCCESS;
    play_music_ctrol(PLAY_MUSIC_CTROL_STOP);

    play_movie_info info;
    play_movie_get_info(&info);
    movie_box_setinfo(&info);
    GUI_SetProperty(TEXT_MOVIE_NAME1, "string", info.name);

#ifndef MEDIA_PLAY_DISPLAY_MODE
	GUI_SetProperty(SLIDERBAR_MOVICE, "value", &value);
	GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
#endif

	// clear mp3 file name
	clean_music_path_bak();
	// clean music play tag .
	clean_music_file_no_bak();

	/*list get*/
	play_list* list = NULL;
	char* path = NULL;
	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	if(list == NULL){
		ret = GXCORE_ERROR;
		goto TO_MOVIE_INIT_ERR;
	}
	if(list->play_no >= list->nents){
		ret = GXCORE_ERROR;
		goto TO_MOVIE_INIT_ERR;
	}
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	if(path == NULL){
		ret = GXCORE_ERROR;
		goto TO_MOVIE_INIT_ERR;
	}

#if 0//(MINI_256_COLORS_OSD_SUPPORT == 1)
	GUI_SetProperty(WIN_MOVIE_VIEW, "back_ground", "null");
	GUI_SetInterface("flush", NULL);
	GUI_SetInterface("video_enable", NULL);
#else
	RadioBackgroundDisplayFlag  = false;
	GUI_SetInterface("image_disable", NULL);
	GUI_SetProperty(WIN_MOVIE_VIEW, "back_ground", (void*)"null");
	GUI_SetFocusWidget("movie_view_sliderbar_cursor");
	GUI_SetInterface("flush", NULL);

	memset(&s_player_info, 0, sizeof(PlayerProgInfo));
	// for SPP display control
	if(GxPlayer_MediaGetProgInfoByUrl(path, &s_player_info) == 0) //success
	{
		// add 20121112, for background display
		if(s_player_info.is_radio == 1)
		{
			GUI_SetInterface("image_enable", NULL);
			RadioBackgroundDisplayFlag  = true;
			//GUI_SetProperty(WIN_MOVIE_VIEW, "back_ground", (void*)"bg.jpg");
		}
	}
#endif
	movie_box_button1_bspeed = PLAY_MOVIE_SPEED_X1;
	movie_box_button2_ctrol = PLAY_MOVIE_CTROL_PLAY;
	movie_box_button3_fspeed = PLAY_MOVIE_SPEED_X1;
	movie_box_button6_zoom = PLAY_MOVIE_ZOOM_X1;
	s_movie_total_time=0;
	s_movie_manual_stop= false;
	s_seek_flag = MOVIE_SEEK_NONE;
	s_start_play_no = list->play_no;

	if(GUI_CheckDialog("wnd_pvr_media_list") == GXCORE_SUCCESS)
	{
		movie_pvr_media_init();
		s_movie_play_sequence = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
		pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, s_movie_play_sequence);
	}
	/*popmsg topright*/
	GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
//	GUI_SetProperty(TEXT_POPMSG, "state", "hide");

	/*popmsg centre*/
	GUI_SetProperty(COMBOBOX_POPMSG, "state", "osd_trans_hide");

	/*popmsg status*/
	GUI_SetProperty(TEXT_MOVIE_STATUS, "state", "osd_trans_hide");


#ifdef MEDIA_PLAY_DISPLAY_MODE
	app_movie_view_create();
	GUI_SetProperty(SLIDERBAR_MOVICE, "value", &value);
	GUI_SetProperty(SLIDER_CURSOR_MOVICE, "value", &value);
#endif

	/*box*/
	movie_box_timer_start();
	movie_sliderbar_timer_start();
	
	if(GUI_CheckDialog("wnd_pvr_media_list") == GXCORE_SUCCESS)
	{
		//popmsg_topright_ctrl(PLAY_MOVIE_CTROL_PLAY, 0);
		// s_start_time_ms = 0;
		
		s_start_time_ms = pmp_load_tag(path);
		if(0 < s_start_time_ms)
		{
			PopDlg  pop;
			memset(&pop, 0, sizeof(PopDlg));
			pop.type = POP_TYPE_YES_NO;
			pop.mode = POP_MODE_UNBLOCK;
			pop.format = POP_FORMAT_DLG;
			pop.str = STR_ID_CONTINUE_VIEW;
			pop.title = NULL;
			pop.exit_cb = _play_cb;
			pop.timeout_sec = 6;
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;
//			media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);	
			iDialogRet = _play_cb(popdlg_create(&pop));
		}
		else
		{
			iDialogRet = _play_cb(POP_VAL_CANCEL);
		}
	}
	else
	{
		s_start_time_ms = pmp_load_tag(path);
		if(0 < s_start_time_ms)
		{
			PopDlg  pop;
			memset(&pop, 0, sizeof(PopDlg));
			pop.type = POP_TYPE_YES_NO;
			pop.mode = POP_MODE_UNBLOCK;
			pop.format = POP_FORMAT_DLG;
			pop.str = STR_ID_CONTINUE_VIEW;
			pop.title = NULL;
			pop.exit_cb = _play_cb;
			pop.timeout_sec = 6;
			//pop.default_ret = POP_VAL_CANCEL;
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;
//			media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);	
			iDialogRet = _play_cb(popdlg_create(&pop));
		}
		else
		{
			iDialogRet = _play_cb(POP_VAL_CANCEL);
		}
	}

	if (0 != iDialogRet)
	{
		return GXCORE_ERROR;
	}
	/*popmsg mute*/
	popmsg_mute_init();

	/*popmsg aspect*/
	value = pmpset_get_int(PMPSET_ASPECT_RATIO);	
	pmpset_set_int(PMPSET_ASPECT_RATIO, value);


TO_MOVIE_INIT_ERR:
	if(ret == GXCORE_ERROR)
	{
		PopDlg  pop;
		memset(&pop, 0, sizeof(PopDlg));
		pop.type = POP_TYPE_OK;
		pop.mode = POP_MODE_BLOCK;
		pop.format = POP_FORMAT_DLG;
		pop.str = "Can not play the file!";
		pop.title = NULL;
		pop.timeout_sec = 6;
		pop.pos.x = POP_MEDIA_X_START;
		pop.pos.y = POP_MEDIA_Y_START;
//		media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);	
		popdlg_create(&pop);
		GUI_EndDialog("win_movie_view");
		return GXCORE_ERROR;
	}

	app_set_win_create_flag(MEDIA_MOVIE_WIN);
	return GXCORE_SUCCESS;
}

static void movie_pvr_media_exit(void)
{
    //GUI_SetInterface("logic_clut", "s_bar_money.bmp");
    //GxBus_ConfigGetInt("osd>trans_level", &init_value, 255);
    //GUI_SetInterface("osd_alpha_global", &init_value);

    //app_epg_enable(0,TS_2_DMX);
}

SIGNAL_HANDLER int movie_view_destroy(const char* widgetname, void *usrdata)
{
	/*popmsg topright*/
	APP_TIMER_REMOVE(popmsg_topright_timer_hide);

	/*popmsg centre*/
	APP_TIMER_REMOVE(popmsg_centre_timer_hide);

	/*box*/
//	APP_TIMER_REMOVE(box_timer_hide);
	APP_TIMER_REMOVE(box_sliderbar_update_timer);
	APP_TIMER_REMOVE(box_sliderbar_active_timer);
	//movie_box_hide(NULL);

	s_seek_flag = MOVIE_SEEK_NONE;

	APP_TIMER_REMOVE(delay_play_timer);

	/*list get*/
	play_list* list = NULL;
	char* path = NULL;
	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	APP_CHECK_P(list, GXCORE_ERROR);
	if(list->play_no >= list->nents) return GXCORE_ERROR;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	APP_CHECK_P(path, GXCORE_ERROR);
	
	/*save tag*/
	uint64_t cur_time_ms = 0;
	PlayTimeInfo  time = {};
	status_t ret = GXCORE_ERROR;
	ret = GxPlayer_MediaGetTime(PMP_PLAYER_AV, &time);
	cur_time_ms = time.current;
	if(GXCORE_SUCCESS == ret)
	{
		if((0 != cur_time_ms) && (0 != s_movie_total_time) 
			&&(cur_time_ms != s_movie_total_time))
			pmp_save_tag(path, cur_time_ms);
	}

	/*subt destroy*/
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_destroy();
	//pmpset_set_int(PMPSET_MOVIE_SUBT_VISIBILITY, PMPSET_TONE_ON);
#endif

	/*play stop*/
	play_movie_ctrol(PLAY_MOVIE_CTROL_STOP);

	if(GUI_CheckDialog("wnd_pvr_media_list") == GXCORE_SUCCESS)
	{
        s_movie_play_sequence = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);          
	    pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, s_movie_play_sequence);
	    movie_pvr_media_exit();
	}
	app_set_win_destroy_flag(MEDIA_MOVIE_WIN);
	
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int movie_media_view_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MOVIE_VIEW))
		GUI_SendEvent(WIN_MOVIE_VIEW, event);

#if 0//def MEDIA_PLAY_DISPLAY_MODE
	int box_sel;
#if MINI_16_BITS_OSD_SUPPORT
	uint32_t value;
#endif
	GUI_Event *event = NULL;
	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	event = (GUI_Event *)usrdata; 
	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
	{	
		case APPK_BACK:
		case APPK_MENU: 	
#ifdef MEDIA_PLAY_DISPLAY_MODE
			app_movie_view_end_dialog();
#else
			GUI_EndDialog(WIN_MOVIE_VIEW);
#endif			
			return EVENT_TRANSFER_STOP;
			
		case APPK_OK:
			popmsg_centre_hide(NULL);
			movie_box_show();
			return EVENT_TRANSFER_STOP;

		case APPK_VOLDOWN:
		case APPK_LEFT:
			popmsg_centre_hide(NULL);
			event->key.sym = APPK_VOLDOWN;
#if MINI_16_BITS_OSD_SUPPORT
			value=pmpset_get_int(PMPSET_MUTE); 
			if(value==PMPSET_MUTE_ON)
			{  
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE, "state", "osd_trans_hide");
			}

			GUI_CreateDialog("win_volume");
#else
			GUI_CreateDialog(WIN_VOLUME);
			GUI_SendEvent(WIN_VOLUME, event);
#endif
			return EVENT_TRANSFER_STOP;
		case APPK_VOLUP:
		case APPK_RIGHT:
			popmsg_centre_hide(NULL);
			event->key.sym = APPK_VOLUP;
#if MINI_16_BITS_OSD_SUPPORT
			value=pmpset_get_int(PMPSET_MUTE); 
			if(value==PMPSET_MUTE_ON)
			{  
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE, "state", "osd_trans_hide");
			}

			GUI_CreateDialog("win_volume");
#else
			GUI_CreateDialog(WIN_VOLUME);
			GUI_SendEvent(WIN_VOLUME, event);
#endif
			return EVENT_TRANSFER_STOP;

		/*shortcut keys
*/
		case APPK_PREVIOUS:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PREVIOUS;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PREVIOUS);
			return EVENT_TRANSFER_STOP;
			
		case APPK_REW:
#if (TRICK_PLAY_SUPPORT > 0)	
			movie_box_ctrl(MOVIE_BOX_BUTTON_SPEED_BACKWARD);
#endif		
			return EVENT_TRANSFER_STOP;
			
		case APPK_PAUSE_PLAY:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY_PAUSE);
			return EVENT_TRANSFER_STOP; 

		// add in 20120206, for single key for pause
		case APPK_PAUSE:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PAUSE);
			return EVENT_TRANSFER_STOP;

		case APPK_PLAY:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
			return EVENT_TRANSFER_STOP; 	
			
		case APPK_FF:
#if (TRICK_PLAY_SUPPORT > 0)	
			movie_box_ctrl(MOVIE_BOX_BUTTON_SPEED_FORWARD);
#endif		
			return EVENT_TRANSFER_STOP;
			
		case APPK_NEXT:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_NEXT;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_NEXT);
			return EVENT_TRANSFER_STOP; 
			
		case APPK_STOP:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_STOP;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			s_movie_manual_stop = TRUE;
			movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
			return EVENT_TRANSFER_STOP; 
			
		/*case APPK_ZOOM:
			movie_box_ctrl(MOVIE_BOX_BUTTON_ZOOM);
			return EVENT_TRANSFER_STOP; */
#if MEDIA_SUBTITLE_SUPPORT			
		case APPK_SUBT:
			movie_box_ctrl(MOVIE_BOX_BUTTON_LRC);
			return EVENT_TRANSFER_STOP; 
#endif			
		case APPK_SET:
			//if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
			//{
				movie_box_ctrl(MOVIE_BOX_BUTTON_SET);
			//}
			return EVENT_TRANSFER_STOP;
			
		case APPK_AUDIO:
			popmsg_centre_ctrl(MOVIE_COMBO_TRACK);		
			return EVENT_TRANSFER_STOP;
			
		case APPK_RATIO:
			if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol
				&& s_player_info.is_radio == 0)
			{
				popmsg_centre_ctrl(MOVIE_COMBO_RATIO);
			}
			return EVENT_TRANSFER_STOP;
			
		case APPK_MUTE:
			popmsg_mute_ctrl();
			return EVENT_TRANSFER_STOP;

		case APPK_SEEK:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(SLIDERBAR_MOVICE);
			return EVENT_TRANSFER_STOP;

		case APPK_REPEAT:
		{
			int value;
	
			value = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
			if(value == PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE)
			{
				value = PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE;
			}
			else
			{
				value++;
			}
			pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, value);

			GUI_CreateDialog("win_movie_set");
			
			return EVENT_TRANSFER_STOP; 
		}	
			
		default:
			break;
	}
#endif	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER  int movie_media_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	event = (GUI_Event *)usrdata;

	if(GXCORE_SUCCESS == GUI_CheckDialog(WIN_MOVIE_VIEW))
		GUI_SendEvent(WIN_MOVIE_VIEW, event);
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int movie_view_keypress(const char* widgetname, void *usrdata)
{
	int box_sel;

//	uint32_t value;
	GUI_Event *event = NULL;
	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);

	event = (GUI_Event *)usrdata; 
	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
	{	
		case APPK_BACK:
		case APPK_MENU:		
#ifdef MEDIA_PLAY_DISPLAY_MODE
			app_movie_view_end_dialog();
#endif
			GUI_EndDialog(WIN_MOVIE_VIEW);		
			return EVENT_TRANSFER_STOP;

		case APPK_OK:
			popmsg_centre_hide(NULL);
			movie_box_show();
			return EVENT_TRANSFER_STOP;

		case APPK_VOLDOWN:
		case APPK_LEFT:
			popmsg_centre_hide(NULL);
			event->key.sym = APPK_VOLDOWN;
#if MINI_16_BITS_OSD_SUPPORT
			value=pmpset_get_int(PMPSET_MUTE); 
			if(value==PMPSET_MUTE_ON)
			{  
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE_1, "state", "osd_trans_hide"); 
				GUI_SetProperty(IMAGE_MUTE, "state", "osd_trans_hide");
			}
	
			GUI_CreateDialog("wnd_volume_value");
#else
			GUI_CreateDialog(WIN_VOLUME);
			GUI_SendEvent(WIN_VOLUME, event);
#endif
			return EVENT_TRANSFER_STOP;

		case APPK_VOLUP:
		case APPK_RIGHT:
			popmsg_centre_hide(NULL);
			event->key.sym = APPK_VOLUP;
#if MINI_16_BITS_OSD_SUPPORT
			value=pmpset_get_int(PMPSET_MUTE); 
			if(value==PMPSET_MUTE_ON)
			{  
				pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);
				GUI_SetProperty(IMAGE_MUTE_1, "state", "osd_trans_hide"); 
				GUI_SetProperty(IMAGE_MUTE, "state", "osd_trans_hide");
			}
			GUI_CreateDialog("win_volume");
#else
			GUI_CreateDialog(WIN_VOLUME);
			GUI_SendEvent(WIN_VOLUME, event);
#endif
			return EVENT_TRANSFER_STOP;

			/*shortcut keys
			*/
		case APPK_PREVIOUS:
			popmsg_centre_hide(NULL);
#if MINI_16_BITS_OSD_SUPPORT
			app_movie_view_end_dialog();
#endif
			movie_view_box_reset();

			movie_box_ctrl(MOVIE_BOX_BUTTON_PREVIOUS);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PREVIOUS;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			return EVENT_TRANSFER_STOP;

		case APPK_REW:
#if (TRICK_PLAY_SUPPORT > 0)	
			movie_box_ctrl(MOVIE_BOX_BUTTON_SPEED_BACKWARD);
#endif		
			return EVENT_TRANSFER_STOP;

		case APPK_PAUSE_PLAY:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY_PAUSE);
			return EVENT_TRANSFER_STOP;	

			// add in 20120206, for single key for pause
		case APPK_PAUSE:
			if(movie_box_button2_ctrol == PLAY_MOVIE_CTROL_STOP)
				return EVENT_TRANSFER_STOP;
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PAUSE);
			return EVENT_TRANSFER_STOP;

		case APPK_PLAY:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			movie_box_ctrl(MOVIE_BOX_BUTTON_PLAY);
			return EVENT_TRANSFER_STOP;		

		case APPK_FF:
#if (TRICK_PLAY_SUPPORT > 0)	
			movie_box_ctrl(MOVIE_BOX_BUTTON_SPEED_FORWARD);
#endif		
			return EVENT_TRANSFER_STOP;

		case APPK_NEXT:
			popmsg_centre_hide(NULL);
#if MINI_16_BITS_OSD_SUPPORT
			app_movie_view_end_dialog();
#endif
			movie_view_box_reset();
			
			movie_box_ctrl(MOVIE_BOX_BUTTON_NEXT);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_NEXT;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			return EVENT_TRANSFER_STOP;	

		case APPK_STOP:
			popmsg_centre_hide(NULL);
			movie_box_show();
			GUI_SetFocusWidget(BOX_MOVIE_CTROL);
			box_sel = MOVIE_BOX_BUTTON_STOP;
			GUI_SetProperty(BOX_MOVIE_CTROL, "select", &box_sel);
			s_movie_manual_stop = TRUE;
			movie_box_ctrl(MOVIE_BOX_BUTTON_STOP);
			return EVENT_TRANSFER_STOP;	

			/*case APPK_ZOOM:
			  movie_box_ctrl(MOVIE_BOX_BUTTON_ZOOM);
			  return EVENT_TRANSFER_STOP;	*/
#if MEDIA_SUBTITLE_SUPPORT			
		case APPK_SUBT:
			movie_box_ctrl(MOVIE_BOX_BUTTON_LRC);
			return EVENT_TRANSFER_STOP;	
#endif			
		case APPK_SET:
			//if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol)
			//{
				movie_box_ctrl(MOVIE_BOX_BUTTON_SET);
			//}
			return EVENT_TRANSFER_STOP;

		case APPK_AUDIO:
			popmsg_centre_ctrl(MOVIE_COMBO_TRACK);		
			return EVENT_TRANSFER_STOP;

		case APPK_RATIO:
			if(PLAY_MOVIE_CTROL_PLAY == movie_box_button2_ctrol
					&& s_player_info.is_radio == 0)
			{
				popmsg_centre_ctrl(MOVIE_COMBO_RATIO);
			}
			return EVENT_TRANSFER_STOP;

		case APPK_MUTE:
			popmsg_mute_ctrl();
			return EVENT_TRANSFER_STOP;

		case APPK_SEEK:
			popmsg_centre_hide(NULL);
			movie_box_show();
			//GUI_SetFocusWidget(SLIDERBAR_MOVICE);
			return EVENT_TRANSFER_STOP;

		case APPK_REPEAT:
			{
				int value;

				value = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
				if(value == PMPSET_MOVIE_PLAY_SEQUENCE_SEQUENCE)
				{
					value = PMPSET_MOVIE_PLAY_SEQUENCE_ONLY_ONCE;
				}
				else
				{
					value++;
				}
				pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, value);

				GUI_CreateDialog("win_movie_set");

				return EVENT_TRANSFER_STOP;	
			}	

		default:
			break;
	}

	return EVENT_TRANSFER_KEEPON;
}


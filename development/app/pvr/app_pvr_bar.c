/*
 * =====================================================================================
 *
 *       Filename:  app_pvr_bar.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年09月21日 11时16分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#include "app.h"
#include "app_pvr.h"

#ifdef CA_FLAG
#include "app_common_porting_ca_smart.h"
#endif
#if MEDIA_SUBTITLE_SUPPORT
extern void app_subt_pause(void);
#endif

extern uint32_t app_play_pos_set(int32_t pos);
extern bool app_check_av_running(void);
static bool iSliderMove;
#define KK_PATCH_E0003 0

typedef enum
{
    PVR_SHOW,
    PVR_HIDE
}pvr_ui;

typedef enum
{
    PVR_PAUSE_ENABLE,
    PVR_PAUSE_DISABLE
}pvr_pause;

typedef enum
{
    PVR_TIMER_TICK,
    PVR_TIMER_SLIDER,
    PVR_TIMER_REC_END,
    PVR_TIMER_TOTAL
}pvr_timer_mode;

//added by huangbc

//#define STR_ID_STOP_REC_INFO    "Stop recording?"
//#define STR_ID_STOP_TMS_INFO    "Stop Timeshifting?"

//end huangbc

typedef struct _pvr_timer pvr_timer;
struct _pvr_timer
{
    event_list      *timer[PVR_TIMER_TOTAL];
    uint32_t        interval[PVR_TIMER_TOTAL];
    TIMER_FLAG      flag[PVR_TIMER_TOTAL];
    int             (*cb[PVR_TIMER_TOTAL])(void*);
    void    (*init)(pvr_timer*);
    void    (*release)(pvr_timer*);
    void    (*start)(pvr_timer*, pvr_timer_mode);
};

typedef struct
{
    pvr_ui          ui;
    //pvr_pause       pause;
    uint32_t        slider_refresh; // 0-stop refresh  1-timer refresh
    bool              shift_end;
    uint32_t        tick_refresh_delay;
    void            (*taxis)(uint32_t);
    void            (*state_change)(pvr_state);// ff fb pause play dummy
    pvr_timer       timer;
    bool            shift_start;
#define TAXIS_NONE  (0)
#define TAXIS_TP    (1)
}AppPvrUi;

typedef struct
{	
	int8_t *player_name;		
	uint64_t cur_time;
	uint64_t total_time;
}MediaPlayTime;

extern AppPvrUi g_AppPvrUi;
#define STR_ID_PLAY    "Play"
static void _get_pvr_duration_from_event(void);

static time_t pvr_duration = 0;

static time_t set_pvr_duration = 0;

static int gi_KeyUpFlag = -1;

#define WND_PVR_BAR     "wnd_pvr_bar"
#define TXT_NAME        "txt_pvr_prog_name"
#define TXT_PERCENT     "txt_pvr_percent"
#define IMG_PERCENT     "img_percent_"
#define IMG_STATE       "img_pvr_state"
#define TXT_STATE       "txt_pvr_state"
#define TXT_CUR_TICK    "txt_pvr_cur_time"
#define TXT_TOTAL_TICK  "txt_pvr_total_time"
#define SLIDER_BAR      "slider_pvr_process"
#define SLIDER_BAR_SEEK      "slider_pvr_seek"
#define BMP_PAUSE       "s_pvr_pause.bmp"
#define BMP_PLAY        "s_pvr_play.bmp"
#define BMP_FF          "s_pvr_fspeed.bmp"
#define BMP_FB          "s_pvr_bspeed.bmp"
#define EDIT_DURATION      "edit_duration_set"


#define TICK_DELAY (1)
#define SEC_TO_MILLISEC (1000)

extern EventState gEventState;




#if DVB_PVR_FLAG
void event_to_pvr(uint8_t flag) //flag: 0 is rec; 1 is pvr;2 is stop;
{
	GUI_Event event = {0};

    event.type = GUI_KEYDOWN;
	 if(flag == 0)
	 {
		event.key.sym = KEY_RED;//rec
	 }
	 else if(flag == 1)
	 {
	 	event.key.sym = KEY_GREEN;//tms
	 }
	 else if(flag == 2)
	 {
	 	event.key.sym = KEY_YELLOW;//stop rec or tms
	 }
	GUI_EndDialog("win_program_info");
	GUI_CreateDialog("wnd_pvr_bar");
	GUI_SetProperty("wnd_pvr_bar", "draw_now", NULL);
	GUI_SendEvent("wnd_pvr_bar", &event);
}

/*static*/ void full_pause_exec(void)
{
	GxMsgProperty_PlayerPause player_pause;
	GxMsgProperty_PlayerResume player_resume;
	uint32_t pos = 0;
	GxBusPmDataProg prog_data;
	int32_t tms_flag = 0;
	GxBusPmViewInfo view_info;

	app_prog_get_playing_pos_in_group(&pos);
	if(GxBus_PmProgGetByPos(pos, 1, &prog_data)==-1)
	{
		return ;
	}
	GxBus_PmViewInfoGet(&view_info);
	GxBus_ConfigGetInt(PVR_TIMESHIFT_KEY, &tms_flag, PVR_TIMESHIFT_DEFAULT);
	tms_flag = STATE_ON;
    if(g_AppPvrOps.state != PVR_DUMMY)
    {
        if((prog_data.id == g_AppPvrOps.env.prog_id)
            && (view_info.stream_type == GXBUS_PM_PROG_TV))
        {
            event_to_pvr(1);
        }
        else
        {
       		return;
            //goto NORMAL_PAUSE;
        }
    }
    else if ((tms_flag == STATE_ON)
        && (app_prog_get_num_in_group() > 0)
        && (g_AppPvrOps.usb_check(&g_AppPvrOps) == USB_OK)
        && ((TRUE == app_play_get_running_status() ))
        && (FALSE == app_play_get_msg_pop_type_state(MSG_POP_SIGNAL_BAD))
        && (gEventState.pause == STATE_OFF))
    {
        //tms only start
        if(view_info.stream_type == GXBUS_PM_PROG_TV)
        {
        	printf("event_to pvr 1\n");
            event_to_pvr(1);
        }
        else
        {
        	return;
            //goto NORMAL_PAUSE;
        }
    }
    else if(g_AppPvrOps.state == PVR_DUMMY && ((FALSE == app_play_get_running_status())
		||(TRUE == app_play_get_msg_pop_type_state(MSG_POP_SIGNAL_BAD))))
    {
        printf("##2233\n");
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Can't Support!",POPMSG_TYPE_OK);
        return;
    }
    else if(g_AppPvrOps.usb_check(&g_AppPvrOps) == USB_ERROR)
    {
            //notic usb error
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Insert the device please",POPMSG_TYPE_OK);
        return;
    }
    else if(g_AppPvrOps.usb_check(&g_AppPvrOps) == USB_NO_SPACE)
    {
            //notice usb no space
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"No space!",POPMSG_TYPE_OK);
        return;
    }
    else
    {
    	//return;
//NORMAL_PAUSE:
        if (gEventState.pause == STATE_ON)
        {
            player_resume.player = PLAYER_FOR_NORMAL;
            app_send_msg(GXMSG_PLAYER_RESUME, (void *)(&player_resume));
            gEventState.pause = STATE_OFF;
        }
        else
        {
            GxMsgProperty_PlayerTimeShift time_shift;
            memset(&time_shift,0,sizeof(GxMsgProperty_PlayerTimeShift));
            time_shift.enable = 0;
            app_send_msg(GXMSG_PLAYER_TIME_SHIFT, (void*)(&time_shift));
#if MEDIA_SUBTITLE_SUPPORT
            app_subt_pause();
#endif
            player_pause.player = PLAYER_FOR_NORMAL;
            app_send_msg(GXMSG_PLAYER_PAUSE, (void *)(&player_pause));
            gEventState.pause = STATE_ON;

        }
    }

}

/*static*/ void full_pvr_exec(void)
{
    usb_check_state usb_state = USB_OK;
	 GxBusPmViewInfo view_info;
	 GxBusPmDataProg prog_data;
	 uint32_t pos = 0;

	 app_prog_get_playing_pos_in_group(&pos);
	  if(GxBus_PmProgGetByPos(pos, 1, &prog_data)==-1)
	  {
	  		return ;
	  }
	 GxBus_PmViewInfoGet(&view_info);
    if (view_info.stream_type != GXBUS_PM_PROG_TV)
    {
    	app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Can't Support!",POPMSG_TYPE_OK);
        return;
    }

    if (app_prog_get_num_in_group() == 0)
        return;

    if(g_AppPvrOps.state == PVR_DUMMY)
    {
        //if(app_check_av_running() == false)
        if((FALSE == app_play_get_running_status())||(TRUE == app_play_get_msg_pop_type_state(MSG_POP_SIGNAL_BAD)))
        {
		   app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Can't Support!",POPMSG_TYPE_OK);            
           return;
        }
        // check usb state
        usb_state = g_AppPvrOps.usb_check(&g_AppPvrOps);
        if(usb_state == USB_ERROR)
        {
            //notic usb error
			app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Insert the device please",POPMSG_TYPE_OK);
            return;
        }

        if(usb_state == USB_NO_SPACE)
        {
            //notice usb no space
			app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"No space!",POPMSG_TYPE_OK);
            return;
        }
    }

	if (1 == prog_data.scramble_flag)
	{
		#ifdef CA_FLAG
			if (app_porting_ca_smart_status() == FALSE)
			{
				app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START, "Please insert card and record!",POPMSG_TYPE_OK); 
				return;
			}
		#else
			app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Can't Support!",POPMSG_TYPE_OK);   
			return;
		#endif
	}

    event_to_pvr(0);
    return;
}
#endif


static int  pvr_cur_total_tick_get(void)
{
    MediaPlayTime time = {0};
    AppPvrOps *pvr = &g_AppPvrOps;
	PlayTimeInfo t_PlayTimeInfo = {0};
//    AppPvrUi *pvr_ui = &g_AppPvrUi;
    //pvr->time.total_tick = 0;
    //pvr->time.cur_tick = 0;
    
    if (pvr->state == PVR_DUMMY)
    {
        return GXCORE_ERROR;
    }
    else if (pvr->state == PVR_RECORD)
    {
        time.player_name = (int8_t*)PVR_PLAYER;
    }
    else
    {
        // pvr || pvr&tms
        time.player_name = (int8_t*)PLAYER_FOR_TIMESHIFT;
    }
	//printf("get player name = %s\n",time.player_name);
	if(GXCORE_SUCCESS  == GxPlayer_MediaGetTime((char*)(time.player_name), &t_PlayTimeInfo))
    {
        //printf("________________________total_time_ms_____________%llu\n", time.total_time);
        pvr->time.total_tick = (uint32_t)(t_PlayTimeInfo.totle/SEC_TO_MILLISEC);
        if(gEventState.pause == STATE_OFF)
        {
            pvr->time.cur_tick = (uint32_t)(t_PlayTimeInfo.current/SEC_TO_MILLISEC);
        }
    }
    else
    {
        return GXCORE_ERROR;
    }
    if((pvr->time.total_tick > 0) && (pvr->time.total_tick > pvr->time.cur_tick))
    {
        pvr->time.remaining_tick = pvr->time.total_tick - pvr->time.cur_tick;
    }
    else
    {
		pvr->time.remaining_tick= 0;
    }
    return GXCORE_SUCCESS;
}   

static void pvr_time_show(void)
{
    AppPvrOps *pvr = &g_AppPvrOps;
    char cur_tick[16]= {0};
    char total_tick[16]= {0};
    struct tm *tm;

    if (pvr->time.cur_tick == 0)
    {
        if(pvr->spd <= 0)
        {
        	if (pvr->state == PVR_RECORD)
        	{
        		GUI_SetProperty(TXT_CUR_TICK, "string", " ");
        	}
			else
			{
            	GUI_SetProperty(TXT_CUR_TICK, "string", "00:00:00");
			}
        }
    }
    else
    {
        if(pvr->state == PVR_RECORD)
        {
            GUI_SetProperty(TXT_CUR_TICK, "string", " ");
        }
        else
        {
            //printf("________________________cur_tick_____________%d\n", pvr->time.cur_tick);
            tm = localtime((time_t*)(&(pvr->time.cur_tick)));
            sprintf(cur_tick, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
            GUI_SetProperty(TXT_CUR_TICK, "string", cur_tick);
        }
    }

    //printf("________________________total_tick_____________%d\n", pvr->time.total_tick);
    tm = localtime((time_t*)(&(pvr->time.total_tick)));
    sprintf(total_tick, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    GUI_SetProperty(TXT_TOTAL_TICK, "string", total_tick);
    
}

static void pvr_slider_show(char *slider)
{
	uint32_t bar_value = 0;
	AppPvrOps *pvr = &g_AppPvrOps;

	if ((pvr->time.cur_tick == 0) && (pvr->spd > 0))
		return;
		
	if (pvr->state == PVR_RECORD)
	{
		if(pvr->time.rec_duration != 0)
		{
			bar_value = (pvr->time.total_tick*100)/(pvr->time.rec_duration); 
		}
    }
	else if (pvr->state == PVR_TIMESHIFT 
		|| pvr->state == PVR_TMS_AND_REC)
	{
		if(pvr->time.total_tick != 0)
		{
			bar_value = (pvr->time.cur_tick*100)/(pvr->time.total_tick); 
		}
   }
    
	// bar show here
	GUI_SetProperty(slider, "value", &bar_value);
}

static void pvr_percent_show(void)
{
#define PER_STR_LEN     8
#define PER_IMG_LEN     16
    uint32_t i;
    int32_t percent;
    char per_str[PER_STR_LEN] = {0};
    char per_img[PER_IMG_LEN];

    AppPvrOps *pvr = &g_AppPvrOps; 
    percent = pvr->percent(pvr);//获取磁盘剩余空间百分比
    if(percent < 0)
    {
    	for (i=0; i<5; i++)
    	{
    		memset(per_img, 0, PER_IMG_LEN);
        	sprintf(per_img, "%s%d", IMG_PERCENT, i);

			GUI_SetProperty(per_img, "img", "s_pvr_progress_grey.bmp");
    	}
        return;
    }

    sprintf(per_str, "%d%%", percent);
    GUI_SetProperty(TXT_PERCENT, "string", per_str);

    // 5 dot to show the percent
    for (i=0; i<5; i++)
    {
        memset(per_img, 0, PER_IMG_LEN);
        sprintf(per_img, "%s%d", IMG_PERCENT, i);

        if (i*20 < percent)
        {
            if (i < 3)
            {
                GUI_SetProperty(per_img, "img", "s_pvr_progress_blue.bmp");
            }
            else if (i == 3)
            {
                GUI_SetProperty(per_img, "img", "s_pvr_progress_yellow.bmp");
            }
            else
            {
                GUI_SetProperty(per_img, "img", "s_pvr_progress_red.bmp");
            }
        }
        else
        {
            GUI_SetProperty(per_img, "img", "s_pvr_progress_grey.bmp");
        }
    }
}

static int pvr_timer_tick(void *usrdata)
{
    static int32_t percent_show = 0;
    AppPvrUi *pvr_ui = &g_AppPvrUi;
    int ret = 0;
     // get tick
    ret = pvr_cur_total_tick_get();
    
    if (GUI_CheckDialog(WND_PVR_BAR) != GXCORE_SUCCESS)
    {
        return 0;
    }

    if(pvr_ui->tick_refresh_delay == 0)
    {
        pvr_time_show();
    }
    
    if(pvr_ui->tick_refresh_delay > 0)
    {
        pvr_ui->tick_refresh_delay--;
    }
    if(GXCORE_ERROR == ret)
    {
        return 0;
    }
    if(pvr_ui->slider_refresh <= 1)
    {
	    pvr_slider_show(SLIDER_BAR);
        if(pvr_ui->slider_refresh == 1)
        {
            pvr_slider_show(SLIDER_BAR_SEEK);
        }
    }

    if(pvr_ui->slider_refresh>1)
    {
    	pvr_slider_show(SLIDER_BAR);
        pvr_ui->slider_refresh--;
    }

    // 10s , refresh usb percent
    if (percent_show++ == 10)
    {
        percent_show = 0;
        pvr_percent_show();
    }

    return 0;
}

static int pvr_timer_slider(void *usrdata)
{
    return 0;
}

static void pvr_timer_init(pvr_timer *timer)
{
    timer->cb[PVR_TIMER_TICK]         = pvr_timer_tick;
    timer->interval[PVR_TIMER_TICK]   = 1*SEC_TO_MILLISEC;
    timer->flag[PVR_TIMER_TICK]       = TIMER_REPEAT;

    timer->cb[PVR_TIMER_SLIDER]       = pvr_timer_slider;
    timer->interval[PVR_TIMER_SLIDER] = 1*SEC_TO_MILLISEC;
    timer->flag[PVR_TIMER_SLIDER]     = TIMER_ONCE;

}

static void pvr_timer_release(pvr_timer *timer)
{
    uint32_t i;

    for (i=0; i<PVR_TIMER_SLIDER; i++)
    {
        if (timer->timer[i] != NULL)
        {
            remove_timer(timer->timer[i]);
            timer->timer[i] = NULL;
        }
    }
}

static void pvr_timer_start(pvr_timer *timer, pvr_timer_mode mode)
{
    if(0 != reset_timer(timer->timer[mode]))
    {
        timer->timer[mode] = create_timer(timer->cb[mode], 
        timer->interval[mode], NULL, timer->flag[mode]); 
    }
}

static void pvr_name_show(void)
{
    char prog_name[80];
    uint32_t pos = 0;
    GxBusPmDataProg prog_data;
    GxBusPmDataProg prog_data_pvr;
    static int pos_bk = 0;
	app_prog_get_playing_pos_in_group(&pos);
	GxBus_PmProgGetByPos(pos, 1, &prog_data);

    //printf("#%d,%d,%d,%d\n",pos,g_AppPvrOps.env.prog_id,prog_data.id,prog_data.pos);

	if(LCN_STATE_OFF == app_flash_get_config_lcn_flag())
	{
        prog_data.pos = pos + 1;
	}

	if((g_AppPvrOps.env.prog_id == prog_data.id)||(g_AppPvrOps.env.prog_id == 0))
	{
		sprintf(prog_name,"%d-%d %s", (prog_data.pos & 0xFF00)>>8, (prog_data.pos & 0xFF),prog_data.prog_name);
		GUI_SetProperty(TXT_NAME, "string", prog_name);
		//GUI_SetProperty(TXT_NAME, "forecolor", "[#f4f4f4,#f4f4f4,#f4f4f4]");
		pos_bk = prog_data.pos;
	}
	else
	{
		GxBus_PmProgGetById(g_AppPvrOps.env.prog_id , &prog_data_pvr);

        if(LCN_STATE_OFF == app_flash_get_config_lcn_flag())
    	{
            prog_data_pvr.pos = pos_bk;
    	}


        if (0 == strcmp(app_flash_get_config_osd_language(),LANGUAGE_CHINESE))
		{
			sprintf(prog_name,"%d-%d %s  %d-%d %s (正在录制...)", 
             (prog_data.pos & 0xFF00)>>8, (prog_data.pos & 0xFF),prog_data.prog_name,
			(prog_data_pvr.pos & 0xFF00)>>8,(prog_data_pvr.pos & 0xFF),prog_data_pvr.prog_name);
		}
		else
		{
			sprintf(prog_name,"%d-%d %s  %d-%d %s (Recording...)", 
             (prog_data.pos & 0xFF00)>>8, (prog_data.pos & 0xFF),prog_data.prog_name,
			(prog_data_pvr.pos & 0xFF00)>>8,(prog_data_pvr.pos & 0xFF),prog_data_pvr.prog_name);
		}

        GUI_SetProperty(TXT_NAME, "string", prog_name);
		//GUI_SetProperty(TXT_NAME, "forecolor", "[#ff0000,#ff0000,#ff0000]");
	}

}

// bk-0. use bakc   1. use force
static void pvr_slider_bk(uint32_t bk)
{
    char *bk_img[2][3] = {{"s_pvr_progress_l.bmp", "s_pvr_progress_m.bmp", "s_pvr_progress_r.bmp"}, 
                    {"s_pvr_progress_l2.bmp", "s_pvr_progress_m2.bmp", "s_pvr_progress_r2.bmp"}};

    GUI_SetProperty(SLIDER_BAR, "back_image_l", bk_img[bk][0]);
    GUI_SetProperty(SLIDER_BAR, "back_image_m", bk_img[bk][1]);
    GUI_SetProperty(SLIDER_BAR, "back_image_r", bk_img[bk][2]);
}

static int app_pvr_state_show(void)
{
	if(gEventState.rec == STATE_ON)
	{
		GUI_SetProperty("full_screen_rec_image", "state", "show");
	}
	else
	{
		GUI_SetProperty("full_screen_rec_image", "state", "hide");
	}
	if(gEventState.tms == STATE_ON)
	{
		GUI_SetProperty("full_screen_tms_image", "state", "show");
	}
	else
	{
		GUI_SetProperty("full_screen_tms_image", "state", "hide");
		GUI_SetProperty("full_screen_tms_stop_image", "state", "hide");
	}
	return 0;
}


SIGNAL_HANDLER int app_pvr_bar_create(GuiWidget *widget, void *usrdata)
{
    AppPvrUi *pvr_ui = &g_AppPvrUi;
    AppPvrOps *pvr = &g_AppPvrOps;
    char *spd_str[] = {"X1","X2","X4","X8","X16","X24"};

    pvr_name_show();
    
    if((pvr->state == PVR_RECORD) || (pvr->state == PVR_DUMMY))
    {
        pvr_slider_bk(0);
        GUI_SetProperty(SLIDER_BAR_SEEK, "state", "hide");
        pvr_slider_show(SLIDER_BAR_SEEK);
        pvr_slider_show(SLIDER_BAR);
        GUI_SetFocusWidget(SLIDER_BAR);
    }
    else
    {
        pvr_slider_bk(1);
        if (pvr->spd != 0)
        {
        	GUI_SetFocusWidget(SLIDER_BAR);
        }
        else
        {
        	GUI_SetProperty(SLIDER_BAR_SEEK, "state", "show");
        	GUI_SetFocusWidget(SLIDER_BAR_SEEK);
        }
    }
    
    if(pvr->state != PVR_DUMMY)
    {
        // get tick
        pvr_cur_total_tick_get();
        pvr_time_show();
        pvr_slider_show(SLIDER_BAR);
        pvr_slider_show(SLIDER_BAR_SEEK);
    }
    
    pvr_percent_show();
    pvr_ui->timer.init(&(pvr_ui->timer));
    pvr_ui->timer.start(&(pvr_ui->timer), PVR_TIMER_TICK);
    pvr_ui->ui = PVR_SHOW;

    if(gEventState.pause == STATE_OFF)
    {
        if(pvr->spd > 0)
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_FF);
            GUI_SetProperty(TXT_STATE, "string", spd_str[pvr->spd]);
        }
        else if(pvr->spd < 0)
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_FB);
            GUI_SetProperty(TXT_STATE, "string", spd_str[pvr->spd*(-1)]);
        }
        else
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
            GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
        }
    }
    else
    {
        GUI_SetProperty(IMG_STATE, "img", BMP_PAUSE);
        GUI_SetProperty(TXT_STATE, "string", "Pause");
    }

	g_AppPvrOps.usbstate = USB_IN;
    
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_bar_destroy(GuiWidget *widget, void *usrdata)
{
    AppPvrUi *pvr_ui = &g_AppPvrUi;

    pvr_ui->timer.release(&(pvr_ui->timer));
    pvr_ui->ui = PVR_HIDE;
    pvr_ui->slider_refresh = 1;
    pvr_ui->tick_refresh_delay = 0;
    
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_bar_got_focus(GuiWidget *widget, void *usrdata)
{
    return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int app_pvr_bar_lost_focus(GuiWidget *widget, void *usrdata)
{
    return EVENT_TRANSFER_KEEPON;
}

void app_pvr_rec_stop(void)
{
    AppPvrOps *pvr = &g_AppPvrOps;
    //AppPvrUi *pvr_ui = &g_AppPvrUi;

     if((pvr->state != PVR_RECORD)
        && (pvr->state != PVR_TMS_AND_REC))
        return;
        
    gEventState.rec = STATE_OFF;
	GUI_SetProperty("full_screen_rec_image", "state", "hide");
    pvr->rec_stop(pvr);
    //pvr_ui->taxis(TAXIS_NONE);
    printf("app_pvr_rec_stop\n");

}

void app_pvr_tms_stop(void)
{
	uint32_t pos = 0;
    AppPvrOps *pvr = &g_AppPvrOps;

    if((pvr->state != PVR_TIMESHIFT)
        && (pvr->state != PVR_TMS_AND_REC))
        return;
    pvr->spd = 0;    
    gEventState.pause = STATE_OFF;//停止时移
	GUI_SetProperty("full_screen_tms_image", "state", "hide");
    //arb->draw[EVENT_PAUSE](arb);
	app_pvr_state_show();
    pvr->tms_stop(pvr);
	
	if(0 != app_prog_get_num_in_group())
		{
			app_prog_get_playing_pos_in_group(&pos);
			app_play_video_audio(pos);			
		}

	pvr->tms_delete(pvr);/* delete failed */
	
    printf("app_pvr_tms_stop\n");
}

void app_pvr_stop(void)
{
//	 int32_t Config = 0;
    AppPvrOps *pvr = &g_AppPvrOps;
    AppPvrUi *pvr_ui = &g_AppPvrUi;

     if(pvr->state == PVR_DUMMY)
        return;

    printf("app_pvr_stop\n");

	if (1 == gi_KeyUpFlag)
	{
		if (PVR_TMS_AND_REC != pvr->state)
		{
	    	app_pvr_rec_stop();
			gEventState.rec = STATE_OFF;
		}
	}
	else
	{
		app_pvr_rec_stop();
		gEventState.rec = STATE_OFF;
	}

	app_pvr_tms_stop();
#if 0
	 GxBus_ConfigGetInt(STANDBY_BOOK_KEY,&Config,STANDBY_BOOK_STATE);
	 if(Config)
	 {
	 	Config = 1;
	 	GxBus_ConfigSetInt(STANDBY_BOOK_KEY, Config);
	 }
#endif
    if (pvr_ui->timer.timer[PVR_TIMER_REC_END] != NULL)
    {
        remove_timer(pvr_ui->timer.timer[PVR_TIMER_REC_END]);
        pvr_ui->timer.timer[PVR_TIMER_REC_END] = NULL;
    }
	//#if KK_PATCH_03098
	//app_subt_resume();
	//#endif
#if KK_PATCH_E0003
	app_date_states_send(RS232_RECEIVE_POWER_CONTINUE);
#endif
	gEventState.tms=STATE_OFF;

	app_pvr_state_show();

	gi_KeyUpFlag = -1;
}

uint8_t app_pvr_state_check(uint8_t flag, uint32_t pos)
{
	AppPvrOps *pvr = &g_AppPvrOps;
	uint32_t prog_pos = 0;
	GxBusPmDataProg prog_data = {0};
	GxBusPmDataProg prog_now = {0};

	if (app_prog_get_num_in_group() == 0)
	{
		return 0;
	}

	if(flag == 0)
	{
		app_prog_get_playing_pos_in_group(&prog_pos);
		GxBus_PmProgGetByPos(prog_pos,1,&prog_data);
		
		if(pvr->state == PVR_TIMESHIFT||pvr->state == PVR_TMS_AND_REC)
		{
			//notice
			app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Timeshift...",POPMSG_TYPE_OK);

			if (GXBUS_PM_PROG_BOOL_ENABLE == prog_data.lock_flag && (0 == app_play_get_running_status()))
			{
				app_play_reset_play_timer(20);
			}
			
			return 1;
		}
		if(pvr->state == PVR_RECORD)
		{
			app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Recording...",POPMSG_TYPE_OK);

			if (GXBUS_PM_PROG_BOOL_ENABLE == prog_data.lock_flag && (0 == app_play_get_running_status()))
			{
				app_play_reset_play_timer(20);
			}
			
			return 1;
		}
	}
	else if(flag == 1)
	{
		if(pvr->state == PVR_TIMESHIFT||pvr->state == PVR_TMS_AND_REC)
		{
			//notice
			if(POPMSG_RET_YES == app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Stop Timeshifting?",POPMSG_TYPE_YESNO))
			{
				printf("\n-------------->[99999999999999999].\n");
				app_pvr_stop();
				return 0;
			}
			else
			{
				return 1;
			}
		}
		if(pvr->state == PVR_RECORD)
		{
			app_prog_get_playing_pos_in_group(&prog_pos);
			GxBus_PmProgGetByPos(prog_pos,1,&prog_data);
			GxBus_PmProgGetByPos(pos,1,&prog_now);
			if(prog_now.tp_id == prog_data.tp_id)
			{
				return 0;
			}
			else
			{
				//notice
				if(POPMSG_RET_YES == app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Stop recording?",POPMSG_TYPE_YESNO))
				{
					app_pvr_stop();
					return 0;
				}
				else
				{
					return 1;
				}
			}
		}
	}
	else
	{
		if(pvr->state == PVR_TIMESHIFT||pvr->state == PVR_TMS_AND_REC)
		{
			//notice
			return 1;
		}		
	}
	return 0;
	
}


//extern void app_pvr_rec_stop(void);
static int pvr_timer_rec_duration(void *usrdata)
{
    app_pvr_stop();
    GUI_EndDialog(WND_PVR_BAR);

    GUI_CreateDialog("win_program_info");
    return 0;
}


// dura_sec : 0-use system setting time
void app_pvr_rec_start(uint32_t dura_sec)
{
#define HALF_HOUR_SEC       (60*30)
#define PVR_DURATION_OFFSET (5) //sec
    //int32_t     duration;
    AppPvrOps   *pvr        = &g_AppPvrOps;
    AppPvrUi    *pvr_ui     = &g_AppPvrUi;
    pvr_timer_mode mode     = PVR_TIMER_REC_END;

    if (pvr->state == PVR_RECORD
            || pvr->state == PVR_TMS_AND_REC)
    {
        return;
    }
    //if(app_check_av_running() == false)
   _get_pvr_duration_from_event();
	
    if (pvr->state == PVR_DUMMY)
    {
        pvr_slider_bk(0);
    }
    
    //pvr_ui->taxis(TAXIS_TP);

    if (dura_sec == 0)
    {
       // GxBus_ConfigGetInt(PVR_DURATION_KEY, &duration, PVR_DURATION_VALUE);
       // pvr->time.rec_duration = (duration + 4) * HALF_HOUR_SEC + PVR_DURATION_OFFSET; // duration 0, means 2hours
	   pvr->time.rec_duration = pvr_duration;
	   GUI_SetProperty("full_screen_rec_image", "state", "show");
    }
    else
    {
        pvr->time.rec_duration = dura_sec + PVR_DURATION_OFFSET;
    }
    pvr->rec_start(pvr);
    gEventState.rec = STATE_ON;
    app_pvr_state_show();
    pvr_percent_show();

    // set timer
    pvr_ui->timer.cb[PVR_TIMER_REC_END]         = pvr_timer_rec_duration;
    pvr_ui->timer.interval[PVR_TIMER_REC_END]   = pvr->time.rec_duration*SEC_TO_MILLISEC;
    pvr_ui->timer.flag[PVR_TIMER_REC_END]       = TIMER_ONCE;

    if(0 != reset_timer(pvr_ui->timer.timer[mode]))
    {
       pvr_ui->timer.timer[mode] = create_timer(pvr_ui->timer.cb[mode], 
       pvr_ui->timer.interval[mode], NULL, pvr_ui->timer.flag[mode]); 
    }
}

void pvr_taxis_mode_clear(void)
{
    //g_AppPvrUi.taxis(TAXIS_NONE);
}

void pvr_clear_speed_display(void)
{
    AppPvrOps *pvr = &g_AppPvrOps;

    if (GUI_CheckDialog(WND_PVR_BAR) == GXCORE_SUCCESS)
    {  
        if(gEventState.pause == STATE_ON)
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_PAUSE);
            GUI_SetProperty(TXT_STATE, "string", "Pause");
        }
        else
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
            GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
        }
    }

    pvr->spd = 0;
}
void tms_speed_change(GxMsgProperty_PlayerSpeedReport *spd)
{
#if (TRICK_PLAY_SUPPORT > 0)
    AppPvrOps *pvr = &g_AppPvrOps;

    if (pvr->state != PVR_TIMESHIFT
        && pvr->state != PVR_TMS_AND_REC)
    {
        return;
    }

    AppPvrUi  *pvr_ui = &g_AppPvrUi;
    
    if(spd->speed == PVR_SPD_1)
    {
        GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
        GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
        GUI_SetProperty(SLIDER_BAR_SEEK, "state", "show");
        GUI_SetFocusWidget(SLIDER_BAR_SEEK);
        pvr->spd = 0;
    }
    else
    {
        pvr->spd = 1;
        if(spd->speed < 0) 
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_FB);
        }
        else if(spd->speed > 0)
        {
            GUI_SetProperty(IMG_STATE, "img", BMP_FF);
        }

        if(spd->speed < 0)
        {
            spd->speed *= -1;
            pvr->spd *= -1;
        }    

        switch((int)spd->speed)
        {
            case PVR_SPD_4:
                GUI_SetProperty(TXT_STATE, "string", "X2");
                break;

            case PVR_SPD_8:
                GUI_SetProperty(TXT_STATE, "string", "X4");
                pvr->spd *= 2;
                break;

            case PVR_SPD_16:
                GUI_SetProperty(TXT_STATE, "string", "X8");
                pvr->spd *= 3;
                break;

            case PVR_SPD_24:
                GUI_SetProperty(TXT_STATE, "string", "X16");
                pvr->spd *= 4;
                break;

            default:
                GUI_SetProperty(TXT_STATE, "string", " ");
                break;
        }
        pvr_ui->slider_refresh = 1;
        pvr_ui->tick_refresh_delay = 0;     
        GUI_SetProperty(SLIDER_BAR_SEEK, "state", "hide");
        GUI_SetInterface("flush",NULL);
        GUI_SetFocusWidget(SLIDER_BAR);
    }
#endif  
}

void tms_state_change(PlayerStatus state)
{
    AppPvrOps *pvr = &g_AppPvrOps;
    
    if (pvr->state != PVR_TIMESHIFT
        && pvr->state != PVR_TMS_AND_REC)
    {
        return;
    }
  
    AppPvrUi  *pvr_ui = &g_AppPvrUi;
    
    switch(state)
    {
        case PLAYER_STATUS_RECORD_FULL:
		case PLAYER_STATUS_ERROR:
		case PLAYER_STATUS_SHIFT_END:
		case PLAYER_STATUS_RECORD_END:
        {
		   	printf("1\n");
           	GxBusPmDataProg prog_data;
//		   	uint32_t rec_prog_id = g_AppPvrOps.env.prog_id;
		   	uint32_t pos;
			void app_popmsg_SetReslut(int ri_Result);
			
		   app_pvr_stop();
		   app_popmsg_SetReslut(POPMSG_RET_NO);
		   if(TRUE == app_get_pop_msg_flag_status())
		   	{
				GUI_EndDialog("win_popmsg_yesno");
				app_set_pop_msg_flag_status(FALSE);
		   	}
//		   if((state == PLAYER_STATUS_SHIFT_END) && (TRUE == app_get_pop_msg_flag_status()))
		 
		   GUI_EndDialog("wnd_pvr_bar");
		   GUI_EndDialog("win_menu_volumes");
		   GUI_EndDialog("win_prog_bar");

		   app_prog_get_playing_pos_in_group(&pos);
		   GxBus_PmProgGetByPos(pos, 1, &prog_data);
		  /* if(prog_data.id == rec_prog_id)
		   {   
			   app_play_video_audio(pos);
		   }*/
           
            extern int win_pop_tip_show_hotplug_gxmsg(char* msg);
            if (PLAYER_STATUS_RECORD_FULL == state)
            {
				if (USB_IN == g_AppPvrOps.usbstate)
				{
		            GUI_EndDialog("win_pop_tip");
		            GUI_CreateDialog("win_pop_tip");
		            win_pop_tip_show_hotplug_gxmsg("No Enough Space!");				
				}
            }

            break;
        }
        
        case PLAYER_STATUS_SHIFT_START:
        {//时移开始，解复用相关配置完成
        	printf("2\n");
            pvr_ui->shift_start = true;
            pvr->spd = 0; 
            GUI_SetProperty(IMG_STATE, "img", BMP_PAUSE);
            GUI_SetProperty(TXT_STATE, "string", "Pause"); 
            GUI_SetProperty(SLIDER_BAR_SEEK, "state", "show");
            GUI_SetFocusWidget(SLIDER_BAR_SEEK);
            break;
        }
        
        case PLAYER_STATUS_SHIFT_RUNNING:
        {
			printf("3\n");
            pvr_ui->shift_start = false;
            pvr_ui->slider_refresh = 1;
            pvr_ui->tick_refresh_delay = 0;        

            GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
            GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
            GUI_SetProperty(SLIDER_BAR_SEEK, "state", "show");
            GUI_SetFocusWidget(SLIDER_BAR_SEEK);
            break;
        }

        case PLAYER_STATUS_SHIFT_HOLD_RUNNING:
        {
			printf("4\n");
            pvr_ui->shift_start = false;
            pvr_ui->slider_refresh = 1;
            pvr_ui->tick_refresh_delay = 0;
            pvr->spd = 0; 
            GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
            GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY); 
            GUI_SetProperty(SLIDER_BAR_SEEK, "state", "show");
            GUI_SetFocusWidget(SLIDER_BAR_SEEK);
            gEventState.pause = STATE_OFF;
         //   arb->draw[EVENT_PAUSE](arb);
            break;
        }
        case PLAYER_STATUS_SHIFT_PAUSE:
        {
			printf("5\n");
            GUI_SetProperty(IMG_STATE, "img", BMP_PAUSE);
            GUI_SetProperty(TXT_STATE, "string", "Pause");
            break;
        }    
        default:
            break;
    }
}

//Duration Set Start!

#define SET_RECORD_TIME  "pic_set_pro_time_duration"
#define SETED_TIMER_UNIT  60 
#define PIC_SET_TEXT   "pic_set_text" 

static void _get_pvr_duration_from_event(void)
{
#define HALF_HOUR_SEC       (60*30)
    time_t system_sec = 0;
    int32_t     duration;
    bool cur_flag = false;
    bool next_flag = false;
	uint32_t prog_pos = 0;
	GxTime time;

    GxEpgInfo *cur_epg_info = NULL;
    GxEpgInfo *next_epg_info = NULL;
   
    GxBus_ConfigGetInt(PVR_DURATION_KEY, &duration, PVR_DURATION_VALUE);
    pvr_duration = (duration+4)*HALF_HOUR_SEC;//2H

	app_prog_get_playing_pos_in_group(&prog_pos);
	app_epg_reset_get_para();
	app_epg_update_event_by_day(prog_pos, 0, TRUE);//init epg_para
	
    if(app_epg_get_event_count() > 0)
    {
        if(cur_epg_info == NULL)
        {
        	cur_epg_info = app_epg_update_event_by_day(prog_pos, 0, TRUE);
            if(cur_epg_info)
            {
                if(cur_epg_info->event_name_len > 0)
                {
                    cur_flag = true;
                }
            }
        }

        if(next_epg_info == NULL)
        {
        	next_epg_info = app_epg_update_event_by_day(prog_pos, 1, TRUE);
            if(next_epg_info)
            {
                if(next_epg_info->event_name_len > 0)
                {
                    next_flag = true;
                }
            }
        }
    }

    if(cur_flag)
    {
    	GxCore_GetLocalTime(&time);
        system_sec = time.seconds;
        if(system_sec > cur_epg_info->start_time)
        {
            pvr_duration = cur_epg_info->duration - (system_sec - cur_epg_info->start_time);
            if(pvr_duration < 3*60)//3 MIN
            {
                if(next_flag)
                {
                    pvr_duration += next_epg_info->duration;
                    if(pvr_duration < 3*60)
                    {
                        pvr_duration = 3*60;
                    }
                }
                else
                {
                    pvr_duration = (duration+4)*HALF_HOUR_SEC;//2H
                }
            }
        }
    }

    if(cur_epg_info != NULL)
    {
        cur_epg_info = NULL;
    }

    if(next_epg_info != NULL)
    {
        next_epg_info = NULL;
    }
}

static int  app_create_new_timer(void)
{
        time_t time = 0;
        AppPvrOps   *pvr		  = &g_AppPvrOps;
        AppPvrUi	  *pvr_ui	  = &g_AppPvrUi;
        pvr_timer_mode mode	  = PVR_TIMER_REC_END;

        if(set_pvr_duration == 0)
        {
            int32_t value = 0;
            value = app_flash_get_config_pvrduration();
            set_pvr_duration =(value +1)*60*60; 
        }
         
		time = set_pvr_duration;
		printf("@@@@@in app_create_new_timer set_pvr_duration = %d,pvr->time.rec_duration = %d\n",
			(int)set_pvr_duration,pvr->time.rec_duration);
		
		 if(time == 0 || time == pvr->time.rec_duration)
		 {
			 return 0;//nothing to do! 
		 }
		 else if(time < pvr->time.total_tick)
		 {
			 return 0;//nothing to do! 
		 }
		 else
		 {
			 pvr->time.rec_duration = time;
		 }

		 //remove old timer
		 if (pvr_ui->timer.timer[mode] != NULL)
		 {
			 remove_timer(pvr_ui->timer.timer[mode]);
			 pvr_ui->timer.timer[mode] = NULL;
		 }

		 //create new timer
		 pvr_ui->timer.cb[mode] 		= pvr_timer_rec_duration;
		 pvr_ui->timer.interval[mode]	= (pvr->time.rec_duration - pvr->time.total_tick)*SEC_TO_MILLISEC;
		 pvr_ui->timer.flag[mode]		= TIMER_ONCE;

		 if(0 != reset_timer(pvr_ui->timer.timer[mode]))
		 {
			 pvr_ui->timer.timer[mode] = create_timer(pvr_ui->timer.cb[mode], 
			 pvr_ui->timer.interval[mode], NULL, pvr_ui->timer.flag[mode]); 
		 }
return 0;
}
static int32_t widget_init_menu_duration = 0;
static status_t key_exit(void)
{
    time_t time = 0;
    uint32_t item_sel = 0;	
    //   AppPvrOps   *pvr        = &g_AppPvrOps;
    //  AppPvrUi    *pvr_ui     = &g_AppPvrUi;
    //	 pvr_timer_mode mode	 = PVR_TIMER_REC_END;

	 GUI_GetProperty(SET_RECORD_TIME, "select", (void*)&item_sel);
    if(item_sel == widget_init_menu_duration)
    {
	    return GXCORE_SUCCESS;
    }
    /*popmsg to save?*/
    popmsg_ret ret_pop = POPMSG_RET_YES;
    ret_pop = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START, STR_CHK_SAVE, POPMSG_TYPE_YESNO);
    
	if(ret_pop != POPMSG_RET_YES)
	{
        app_flash_save_config_pvrduration(widget_init_menu_duration);
        return GXCORE_SUCCESS;
    }	
    
	 switch(item_sel)
	 {
		case 0:{
			time = SETED_TIMER_UNIT*60;    //1xiaoshi
			break;
			}
		case 1:{
			time = SETED_TIMER_UNIT*2*60; //2xiaoshi
			break;
			}
		case 2:{
			time = SETED_TIMER_UNIT*3*60; //3xiaoshi
			break;
		}
		case 3:{
			time = SETED_TIMER_UNIT*4*60; //4xiaoshi
			break;
		}
		default:
			break;
	}
    printf("--------set time = %d,\n",(int)time);
    app_flash_save_config_pvrduration(item_sel);
    set_pvr_duration = time;
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int app_duration_set_keypress(GuiWidget *widget, void *usrdata)
{
    GUI_Event *event = NULL;
    event = (GUI_Event *)usrdata;

    switch(event->key.sym)
    {
    case KEY_EXIT:
    case KEY_RECALL:
    case KEY_MENU:
        key_exit();
        GUI_EndDialog("win_duration_set");
        return EVENT_TRANSFER_STOP;
    case KEY_LEFT:
    case KEY_RIGHT:
        break;
    case KEY_OK:
        break;
    default:
        break;
    }

    return EVENT_TRANSFER_STOP;
}



SIGNAL_HANDLER int app_duration_set_create(GuiWidget *widget, void *usrdata)
{
	int32_t value = 0;
    value = app_flash_get_config_pvrduration();
	GUI_SetProperty(SET_RECORD_TIME, "select", (void*)&value);
    widget_init_menu_duration = value;
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_duration_set_destroy(GuiWidget *widget, void *usrdata)
{
    return EVENT_TRANSFER_STOP;
}
//Duration Set End!

SIGNAL_HANDLER int app_pvr_bar_keypress(GuiWidget *widget, void *usrdata)
{
    GUI_Event *event = NULL;
    AppPvrOps *pvr = &g_AppPvrOps;
    AppPvrUi  *pvr_ui = &g_AppPvrUi;
    GxBusPmDataProg prog_data;
	 uint32_t pos;

    event = (GUI_Event *)usrdata;
    switch(event->type)
    {
        case GUI_SERVICE_MSG:
            break;

        case GUI_MOUSEBUTTONDOWN:
            break;

        case GUI_KEYDOWN:
            switch(event->key.sym)
            {
                case STBK_EXIT:
                case STBK_MENU:
					gi_KeyUpFlag = 1;
                    GUI_EndDialog(WND_PVR_BAR);
                    break;

                case KEY_GREEN://pause
                {

                    printf("state:%d,%d\n",pvr->state,gEventState.pause);
#if MEDIA_SUBTITLE_SUPPORT		
			        app_subt_pause();
#endif
        			app_prog_get_playing_pos_in_group(&pos);
        			GxBus_PmProgGetByPos(pos, 1, &prog_data);
                    if((pvr->state == PVR_DUMMY)
                        || (prog_data.id == g_AppPvrOps.env.prog_id))
                    {
                        pvr_slider_bk(1);
                        if(gEventState.pause == STATE_OFF)
                        {
                            GUI_SetProperty(IMG_STATE, "img", BMP_PAUSE);
                            GUI_SetProperty(TXT_STATE, "string", "Pause");
                            GUI_SetInterface("flush", NULL);
                            if(pvr->state == PVR_RECORD)
                            {
                                pvr->time.cur_tick = pvr->time.total_tick;
                                if (pvr_ui->timer.timer[PVR_TIMER_REC_END] != NULL)
                                {
	                                remove_timer(pvr_ui->timer.timer[PVR_TIMER_REC_END]);
	                                pvr_ui->timer.timer[PVR_TIMER_REC_END] = NULL;
                                }
                            }
                            pvr->pause(pvr);
                            gEventState.pause = STATE_ON;
                            gEventState.tms = STATE_ON;
                            pvr_percent_show();

							GUI_SetProperty("full_screen_tms_stop_image", "state", "show");
                        }
                        else
                        {
                            GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
                            GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
                            GUI_SetInterface("flush", NULL);
                            if(pvr_ui->shift_start == true)
                            {
                                pvr_ui->tick_refresh_delay = TICK_DELAY * 2;
                                pvr_ui->slider_refresh = 4; 
                            }

                            if (pvr->spd != 0)
                            {
                                // if fb or ff, set normal speed
                                pvr->spd = 0;
                                pvr->speed(pvr,PVR_SPD_1);
                            }
                            else
                            {
                                pvr->resume(pvr);
                            }
                            gEventState.pause = STATE_OFF;

							GUI_SetProperty("full_screen_tms_stop_image", "state", "hide");
                        }
                    }
                    //arb->draw[EVENT_PAUSE](arb);
					app_pvr_state_show();
                    break;
                }
              case KEY_RED:  //record
#if 1
              {
          		app_create_new_timer();
      			#if MEDIA_SUBTITLE_SUPPORT		   
    			 app_subt_pause();
    			#endif
    			 app_pvr_rec_start(0);
              }
			
#else
		else
		{
		#if MEDIA_SUBTITLE_SUPPORT			
		 		app_subt_pause();
	        #endif
				app_pvr_rec_start(0);
		}
              #if 0
                	 if(pvr->state == PVR_DUMMY)
                	 {
				#if KK_PATCH_03098
				app_subt_pause();
				#endif
            	 		app_pvr_rec_start(0);
                	 }
			 else
                    	GUI_EndDialog(WND_PVR_BAR);
			 #else
#if MEDIA_SUBTITLE_SUPPORT			
		 		app_subt_pause();
#endif
				if(set_pvr_duration >0)
				{
					printf("@@@@ set_pvr_duration = %d,\n",set_pvr_duration);
					app_pvr_rec_start(set_pvr_duration);
				}
				else
				{
					app_pvr_rec_start(0);
				}

			 #endif
#endif
                    break;
					
                case KEY_YELLOW://stop
                    {
                        char* str=NULL;
                        int skip_popmsg = 0;
                        
                        if(GUI_CheckDialog(WND_PVR_BAR) == GXCORE_SUCCESS)
                        {
                            GUI_EndDialog(WND_PVR_BAR);
                            GUI_SetProperty(WND_PVR_BAR, "draw_now", NULL);
                        }
                        if(pvr->state == PVR_TIMESHIFT)
                        {
                            str = STR_ID_STOP_TMS_INFO;
                        }
                        else
                        {
                            str = STR_ID_STOP_REC_INFO;
                        }
#if DVB_PVR_FLAG

                        if(g_AppPvrOps.usbstate != USB_IN)
                        {
                            skip_popmsg = 1;
                        }
                        if(g_AppPvrOps.stoppvr_flag)
                        {
                            g_AppPvrOps.stoppvr_flag = 0;
                            skip_popmsg = 1;
                        }
#endif
                        if(skip_popmsg || POPMSG_RET_YES == app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,str,POPMSG_TYPE_YESNO))
//						if (skip_popmsg || MEDIA_POPMSG_RET_YES == media_popmsg(POP_MEDIA_X_START,POP_MEDIA_Y_START,str,MEDIA_POPMSG_TYPE_YESNO))
                        {
							if(gEventState.pause ==  STATE_ON)
							{
                               pvr->resume(pvr);
                               gEventState.pause = STATE_OFF;
							}

							gi_KeyUpFlag = -1;
							
//                        	uint32_t pos;
//							int pvr_rec_id;
//							GxBusPmDataProg prog_data;
//							pvr_rec_id = g_AppPvrOps.env.prog_id ;
                            app_pvr_stop();
							if(GXCORE_SUCCESS == GUI_CheckDialog("win_password_input"))
							{
								app_play_clear_msg_pop_type(MSG_POP_PROG_LOCK);
								GUI_EndDialog("win_password_input");
							}
							
/*							app_prog_get_playing_pos_in_group(&pos);
							GxBus_PmProgGetByPos(pos, 1, &prog_data);
							printf("porg id = %d env id = %d\n",prog_data.id,pvr_rec_id);
							if (GXBUS_PM_PROG_BOOL_ENABLE == prog_data.lock_flag && (0 == app_play_get_running_status()))
							{
								app_play_reset_play_timer(1);
							}*/

							/*if((pvr_rec_id == prog_data.id))
							{
								printf("play pos = %d\n",pos);
								app_play_video_audio(pos);
							}*/

							//GUI_SetProperty("full_screen_tms_stop_image", "state", "hide");
                        }
                    }
                    break;
                case KEY_MUTE:
                    //arb->exec[EVENT_MUTE](arb);
                    //arb->draw[EVENT_MUTE](arb);
                    GUI_SendEvent("win_full_screen", event);
		    break;
					
			case STBK_UP:
			case STBK_DOWN:
				if(GUI_CheckDialog(WND_PVR_BAR) == GXCORE_SUCCESS)
				{
					GUI_EndDialog(WND_PVR_BAR);
                	GUI_SetInterface("flush", NULL);
				}

				gi_KeyUpFlag = 1;
				
				GUI_SendEvent(GUI_GetFocusWindow(), event);
				break;

			case STBK_OK:
				if(GUI_CheckDialog(WND_PVR_BAR) == GXCORE_SUCCESS)
				{
					GUI_EndDialog(WND_PVR_BAR);
                GUI_SetInterface("flush", NULL);
				}
				GUI_SendEvent(GUI_GetFocusWindow(), event);
				break;

		    case KEY_VOLUME_DOWN_1:
		    case KEY_VOLUME_UP_1:
			    if(GUI_CheckDialog(WND_PVR_BAR) == GXCORE_SUCCESS)
			    {
				    GUI_EndDialog(WND_PVR_BAR);
                 GUI_SetInterface("flush", NULL);
			    }
              GUI_CreateDialog("win_menu_volume");
              GUI_SendEvent("win_menu_volume", event);
              break;

                default:
                    break;
            }
    }

	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_sliderbar_keypress(GuiWidget *widget, void *usrdata)
{
    status_t ret = EVENT_TRANSFER_STOP;
    GUI_Event *event = (GUI_Event *)usrdata;
#if (TRICK_PLAY_SUPPORT > 0)      
    AppPvrOps *pvr = &g_AppPvrOps;  
//    pvr_speed spd_val[] = {PVR_SPD_1,PVR_SPD_4,PVR_SPD_8,PVR_SPD_16,PVR_SPD_24};//PVR_SPD_2,
//    FullArb *arb = &g_AppFullArb;
#endif    

    switch(event->type)
    {
        case GUI_SERVICE_MSG:
            break;

        case GUI_MOUSEBUTTONDOWN:
            break;

        case GUI_KEYDOWN:
        switch(event->key.sym)
        {
            case STBK_LEFT:
            case STBK_RIGHT:
            break;
#if 0
            case STBK_FB:
#if (TRICK_PLAY_SUPPORT > 0)            
            if (pvr->spd > 0)   
                pvr->spd = 0;

            (pvr->spd<=-4)?(pvr->spd=0):(pvr->spd--);
            if(pvr->spd != 0)
            {
                pvr->speed(pvr, spd_val[pvr->spd*(-1)]*(-1));
            }
            else
            {   
                pvr->speed(pvr, spd_val[pvr->spd]);
            }
            gEventState.pause.pause = STATE_OFF;
          //  arb->draw[EVENT_PAUSE](arb);
 #endif           
            break;

        case STBK_FF:
#if (TRICK_PLAY_SUPPORT > 0)
            if (pvr->spd < 0)   
                pvr->spd = 0;

            (pvr->spd>=4)?(pvr->spd=0):(pvr->spd++);
            pvr->speed(pvr, spd_val[pvr->spd]);
            gEventState.pause.pause = STATE_OFF;
       //     arb->draw[EVENT_PAUSE](arb);
#endif         
            break;
#endif
        case STBK_OK: 
#if (TRICK_PLAY_SUPPORT > 0)         
            if (pvr->spd != 0)
            {
                pvr->spd = 0;
                pvr->speed(pvr, PVR_SPD_1);
            }
#endif            
            break;

        default:
            ret = EVENT_TRANSFER_KEEPON;    
            break;
        }
    }
    return ret;
}

SIGNAL_HANDLER int app_pvr_seek_slider_keypress(GuiWidget *widget, void *usrdata)
{
    status_t ret = EVENT_TRANSFER_STOP;
    uint32_t slider = 0;
	GUI_Event *event = NULL;
    AppPvrOps *pvr = &g_AppPvrOps;
    AppPvrUi  *pvr_ui = &g_AppPvrUi;
    //pvr_speed spd_val[] = {PVR_SPD_1,PVR_SPD_2,PVR_SPD_4,PVR_SPD_8,PVR_SPD_16,PVR_SPD_24};
    //char *spd_str[] = {"X1","X2","X4","X8","X16","X24"};

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_SERVICE_MSG:
			break;

		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
                case STBK_LEFT:
                case STBK_RIGHT:
            if((pvr->time.remaining_tick == 0) && (pvr_ui->slider_refresh != 0) && (event->key.sym == STBK_RIGHT))
                        break;
                        
                    // fb or ff can't control slider bar
            if ((pvr->spd != 0) && (gEventState.pause == STATE_OFF))
                        break;
					// rec can't control slider bar
						if ((pvr->state != PVR_TIMESHIFT) && (pvr->state != PVR_TMS_AND_REC))
						    break;

                    pvr_ui->slider_refresh = 0;
					iSliderMove = TRUE;
                    ret = EVENT_TRANSFER_KEEPON; 
                    break;
                case STBK_OK:
                	if(pvr_ui->slider_refresh == 0)
                	{
	                    if ((pvr->state == PVR_TIMESHIFT) || (pvr->state == PVR_TMS_AND_REC))
	                    {
                        	pvr_ui->slider_refresh = 3;
	                        pvr_ui->tick_refresh_delay = TICK_DELAY;
                        	GUI_GetProperty(SLIDER_BAR_SEEK, "value", &slider);
			      	if(gEventState.pause == STATE_ON)
	                        {
                             if (pvr->spd != 0)
                            {
                                // if fb or ff, set normal speed
                                pvr->spd = 0;
                                pvr->speed(pvr,PVR_SPD_1);
                            }
                            else
                            {
                                GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
                                GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);
                            }
	                            gEventState.pause = STATE_OFF;
	                            //arb->draw[EVENT_PAUSE](arb);
	                        }
	                        pvr->seek(pvr, pvr->time.total_tick*slider/100);
                                pvr->resume(pvr); 

				GUI_SetProperty("full_screen_tms_stop_image", "state", "hide");
	    			    iSliderMove = FALSE;
	                    }
	                    else
	                    {}
			}		
                    break;
		/*
                case KEY_RED://FB
                    if ((pvr->state!=PVR_TIMESHIFT)&&(pvr->state!=PVR_TMS_AND_REC))
			   			 	break;

						 if(pvr->time.cur_tick == 0)
						    break;

						 if(gEventState.pause == STATE_ON)
						 {
						    pvr->resume(pvr);
						 }
						 pvr_ui->slider_refresh = 2;
                    if (pvr->spd > 0)   pvr->spd=-5;

                    (pvr->spd==-5)?(pvr->spd=-1):(pvr->spd--);
                    GUI_SetProperty(IMG_STATE, "img", BMP_FB);
                    GUI_SetProperty(TXT_STATE, "string", spd_str[pvr->spd*(-1)]);
                    pvr->speed(pvr, spd_val[pvr->spd*(-1)]*(-1));
                    gEventState.pause = STATE_OFF;
                    //arb->draw[EVENT_PAUSE](arb);
                   break;

                case KEY_BLUE://FF
                    if ((pvr->state!=PVR_TIMESHIFT)&&(pvr->state!=PVR_TMS_AND_REC))
			   				break;

						 if(pvr_ui->shift_end == true)
						    break;

						 if(gEventState.pause == STATE_ON)
						 {
						    pvr->resume(pvr);
						 }    
						 pvr_ui->slider_refresh = 2;
                    if (pvr->spd < 0)   pvr->spd=5;

                    (pvr->spd==5)?(pvr->spd=1):(pvr->spd++);
                    GUI_SetProperty(IMG_STATE, "img", BMP_FF);
                    GUI_SetProperty(TXT_STATE, "string", spd_str[pvr->spd]);
                    pvr->speed(pvr, spd_val[pvr->spd]);
                    gEventState.pause = STATE_OFF;
                    //arb->draw[EVENT_PAUSE](arb);
                    break;
			*/
           #if 0         
                case KEY_GREEN:
                    GUI_SetProperty(IMG_STATE, "img", BMP_PLAY);
                    GUI_SetProperty(TXT_STATE, "string", STR_ID_PLAY);

                    if(gEventState.pause == STATE_ON)
                    {
                        pvr->resume(pvr);
                        gEventState.pause = STATE_OFF;
                        //arb->draw[EVENT_PAUSE](arb);
                    }

                    if (pvr->spd != 0)
                    {
                        pvr->spd = 0;
                        pvr->speed(pvr, PVR_SPD_1);
                        pvr_ui->slider_refresh = 4;
                        pvr_ui->tick_refresh_delay = TICK_DELAY*2;
                    }
                    break;
				#endif
                default:
                    ret = EVENT_TRANSFER_KEEPON; 
                    break;
            }
    }
	return ret;
}

AppPvrUi g_AppPvrUi = 
{
    .ui             = PVR_HIDE,
    .slider_refresh = 1,
    .taxis          = NULL,
    .timer  = {{NULL,},{0,},{0,},{NULL,},pvr_timer_init,pvr_timer_release,pvr_timer_start},
};



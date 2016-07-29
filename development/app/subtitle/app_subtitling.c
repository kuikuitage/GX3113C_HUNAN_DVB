/*
 * =====================================================================================
 *
 *       Filename:  app_subtitling.c
 *
 *    Description:  select dvb subtitle or ttx subtitle
 *
 *        Version:  1.0
 *        Created:  2011年10月13日 10时14分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#include "app.h"
#include "app_ttx_subt.h"
#include "app_pvr.h"
#include "app_pop.h"


#define SUBT_OFF_MODE_NUM    1
#define WND_SUBT    "wnd_subtitling"
#define LV_SUBT    "listview_subtitling"

typedef struct subtitling Subtitling;
struct subtitling
{
    int32_t     sel;    
    uint32_t    mode;
    char        (*lang)[8];
    char        (*lang_list)[8];
    void        (*magz_open)(void);
    status_t    (*open)(Subtitling*, uint32_t);
    void        (*close)(Subtitling*);
    void        (*change)(Subtitling*);
    void        (*pause)(Subtitling*);
    void        (*resume)(Subtitling*);

#define SUBT_MODE_OFF   (0)
#define SUBT_MODE_TTX   (1)
#define SUBT_MODE_DVB   (2)
};

extern Subtitling g_AppSubtitling;
static event_list* timer_refresh_list = NULL;
static char s_ch_setfocus = 0;//0:Don't do set sel for LV_SUBT 1:do

static int _check_char_validity(uint8_t ch_str)//language code from ISO-8859-1
{
    uint8_t tmp = 0;

    tmp = (ch_str&0xf0)>>4;
    if((tmp == 0) || (tmp == 1) || (tmp == 8) || (tmp == 9))
        return 1;
    else
        return 0;
}

static int _refresh_list(void* usrdata)
{
    uint32_t            i=0;
    TtxSubtOps          *p = &g_AppTtxSubt;
    Subtitling          *p_sub = &g_AppSubtitling;
    uint32_t            size = sizeof(char *[8])*(p->ttx_num+p->subt_num+SUBT_OFF_MODE_NUM);
    static uint32_t     last_lang = 0;
    uint32_t            lang = 0;
    int32_t             sel = 0;

    p_sub->lang = GxCore_Malloc(size);
    if (p_sub->lang == NULL)    return EVENT_TRANSFER_STOP;

    memset(p_sub->lang, 0, size);
    lang++;

    strncpy((char*)p_sub->lang, "OFF", 8);

    for (i=0; i<p->ttx_num; i++)
    {
        if(_check_char_validity(p->content[lang-1].lang[0]))
            p->content[lang-1].lang[0] = 0x20;
        if(_check_char_validity(p->content[lang-1].lang[1]))
            p->content[lang-1].lang[1] = 0x20;
        if(_check_char_validity(p->content[lang-1].lang[2]))
            p->content[lang-1].lang[2] = 0x20;

        sprintf((char*)(p_sub->lang+lang), "%c%c%c(T)", p->content[lang-1].lang[0],
                p->content[lang-1].lang[1],p->content[lang-1].lang[2]);           
        lang++;
    }
    for (i=0; i<p->subt_num; i++)
    {
        if(_check_char_validity(p->content[lang-1].lang[0]))
            p->content[lang-1].lang[0] = 0x20;
        if(_check_char_validity(p->content[lang-1].lang[1]))
            p->content[lang-1].lang[1] = 0x20;
        if(_check_char_validity(p->content[lang-1].lang[2]))
            p->content[lang-1].lang[2] = 0x20;

        sprintf((char*)(p_sub->lang+lang), "%c%c%c(D)", p->content[lang-1].lang[0],
                p->content[lang-1].lang[1],p->content[lang-1].lang[2]);           
        lang++;
    }

    if(s_ch_setfocus == 1 || last_lang != lang)
    {
        if (lang == 1)
        {
            sel = 0;
            strncpy((char*)p_sub->lang, STR_ID_NO_DATA, 8);
        }
        else
        {
            GxBus_ConfigGetInt(SUBTITLE_LANG_KEY, &sel, SUBTITLE_LANG_VALUE);
            if (lang <= sel)
            {
                sel = 1;
            }
        }
        GUI_SetProperty(LV_SUBT, "select", &sel);
        GUI_SetProperty(LV_SUBT, "update_all", NULL);
    }

    s_ch_setfocus = 0;
    last_lang = lang;

	if((timer_refresh_list != NULL) && (p->sync_flag == 1))
	{
		remove_timer(timer_refresh_list);
		timer_refresh_list = NULL;
	}

	return GXCORE_SUCCESS;
}

static void app_subt_ini_save(Subtitling *p_sub)
{
    int32_t init_value;

    init_value = p_sub->mode;
    GxBus_ConfigSetInt(SUBTITLE_MODE_KEY, init_value);
    init_value = p_sub->sel;
    GxBus_ConfigSetInt(SUBTITLE_LANG_KEY, init_value);
}

void app_ttx_magz_open(void)
{
    GxBusPmDataProg prog_data;
    TtxSubtOps *p_ttx = &g_AppTtxSubt;
	uint32_t pos;

    if(app_prog_get_num_in_group() > 0)
    {            
        app_prog_get_playing_pos_in_group(&pos);
        GxBus_PmProgGetByPos(pos,1,&prog_data);

        if (prog_data.ttx_flag == GXBUS_PM_PROG_BOOL_ENABLE)
        {   
             if ((g_AppPvrOps.state == PVR_TIMESHIFT)
                    || (g_AppPvrOps.state == PVR_TMS_AND_REC))
             {
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_NO_BTN;
                pop.format = POP_FORMAT_DLG;
                pop.str = STR_ID_STOP_PVR_FIRST;
                pop.mode = POP_MODE_UNBLOCK;
                pop.timeout_sec = 3;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
                popdlg_create(&pop);
             }
            else
            {
                app_subt_pause();

            GUI_EndDialog("win_prog_bar");
            GUI_EndDialog("win_volume");
            //GUI_Exec();
            GUI_SetInterface("flush",NULL); 
            if(p_ttx->ttx_magz_open(p_ttx) != GXCORE_SUCCESS)
            {
                    memset(&pop, 0, sizeof(PopDlg));
                    pop.type = POP_TYPE_OK;
                    pop.format = POP_FORMAT_DLG;
                    pop.str = STR_ID_NO_TELTEXT;
                    pop.mode = POP_MODE_UNBLOCK;
					pop.pos.x = POP_MEDIA_X_START;
					pop.pos.y = POP_MEDIA_Y_START;
                    popdlg_create(&pop);
                    app_subt_resume();
            }
            else
            {
                    g_AppFullArb.timer_stop();
                    GUI_SetProperty("txt_full_state", "state", "hide");
                    GUI_SetProperty("img_full_state", "state", "hide");
                }
            }
        }
        else
        {
            memset(&pop, 0, sizeof(PopDlg));
            pop.type = POP_TYPE_OK;
            pop.str = STR_ID_NO_TELTEXT;
            pop.mode = POP_MODE_UNBLOCK;
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;
            popdlg_create(&pop);
        }    
    }            
}

void app_subt_change(void)
{
    if (GUI_CheckDialog("win_main_menu") == GXCORE_SUCCESS
        || (GUI_CheckDialog("win_channel_edit") == GXCORE_SUCCESS)
        || (GUI_CheckDialog("win_epg") == GXCORE_SUCCESS)
        || g_AppFullArb.state.tv == STATE_OFF)
        return;

    g_AppSubtitling.change(&g_AppSubtitling);
}

void app_subt_pause(void)
{
        g_AppSubtitling.pause(&g_AppSubtitling);
}

void app_subt_resume(void)
{
    TtxSubtOps *p = &g_AppTtxSubt;
    
    if(p->ttx_num + p->subt_num > 0)
    {
        //GxCore_ThreadDelay(500);
        g_AppSubtitling.resume(&g_AppSubtitling);
    }
}

static status_t app_subtitling_open(Subtitling *p_sub,uint32_t sel)
{
    status_t ret = GXCORE_ERROR;
    TtxSubtOps *p = &g_AppTtxSubt;
	
#if DVB_PVR_FLAG
    if(g_AppFullArb.state.pause == STATE_ON)
    {
        return GXCORE_ERROR;
    }
#endif
	printf("p_sub mode=%d ,sel = %d\n",p_sub->mode,sel);
	printf("ttx+subt num = %d\n",p->ttx_num+p->subt_num);
    if (sel > p->ttx_num+p->subt_num
            || sel == 0)
    {
        // sel = 0 mean MODE_OFF 
        return GXCORE_ERROR;
    }

//exit the pre-process for release the TTX memory for subt
	 g_AppTtxSubt.ttx_pre_process_close(&g_AppTtxSubt);


    if (p_sub->mode == SUBT_MODE_TTX)
    {
        p->ttx_subt_close(p);
    }

    if (p_sub->mode == SUBT_MODE_DVB)
    {
        p->dvb_subt_close(p);
    }

    if (sel > p->ttx_num)
    {
        // dvb_subt
        ret = p->dvb_subt_open(p, sel-SUBT_OFF_MODE_NUM-p->ttx_num);
        if (ret == GXCORE_SUCCESS)
        {
            p_sub->mode = SUBT_MODE_DVB;
        }
    }
    else
    {
        // ttx_subt
        ret = p->ttx_subt_open(p, sel-SUBT_OFF_MODE_NUM);
        if (ret == GXCORE_SUCCESS)
        {
            p_sub->mode = SUBT_MODE_TTX;
        }
    }
    
    if (ret == GXCORE_SUCCESS)
    {
#if DVB_PVR_FLAG
        g_AppFullArb.state.subt = STATE_ON;
#endif

        p_sub->sel = sel;
        app_subt_ini_save(p_sub);
    }

    return ret;
}

static void app_subtitling_close(Subtitling *p_sub)
{
    TtxSubtOps *p = &g_AppTtxSubt;

    if (p_sub->mode == SUBT_MODE_TTX)
    {
        p->ttx_subt_close(p);
    }

    if (p_sub->mode == SUBT_MODE_DVB)
    {
        p->dvb_subt_close(p);
    }

    p_sub->sel = 0;
    p_sub->mode = SUBT_MODE_OFF;
    //p->subt_num = 0;
    //p->ttx_num= 0;
    app_subt_ini_save(p_sub);
#if DVB_PVR_FLAG
    g_AppFullArb.state.subt = STATE_OFF;
#endif
//restart the pre-process, if APP support the function.
	g_AppTtxSubt.ttx_pre_process_init(&g_AppTtxSubt);
}

static void app_subtitling_change(Subtitling *p_sub)
{
    int32_t mode = 0;
    int32_t lang = 0;
    TtxSubtOps *p = &g_AppTtxSubt;
    uint32_t lang_total = p->ttx_num + p->subt_num;

    GxBus_ConfigGetInt(SUBTITLE_MODE_KEY, &mode, SUBTITLE_MODE_VALUE);
    if (mode != SUBTITLE_MODE_VALUE)
    {
        if (lang_total > 0)
        {
            // subtitling  on
            GxBus_ConfigGetInt(SUBTITLE_LANG_KEY, &lang, SUBTITLE_LANG_VALUE);

            if (lang_total < lang)
            {
                lang = 1;
                GxBus_ConfigSetInt(SUBTITLE_LANG_KEY, lang);
            }
            
            p_sub->open(p_sub, lang);
        }
    }
}

static void app_subtitling_pause(Subtitling *p_sub)
{
    TtxSubtOps *p = &g_AppTtxSubt;

    if (p_sub->mode == SUBT_MODE_TTX)
    {
        p->ttx_subt_close(p);
    }

    if (p_sub->mode == SUBT_MODE_DVB)
    {
        p->dvb_subt_close(p);
    }
}

static void app_subtitling_resume(Subtitling *p_sub)
{
    int32_t lang = 0;
    TtxSubtOps *p = &g_AppTtxSubt;

    if (p_sub->mode != SUBT_MODE_OFF
            && p->ttx_num+p->subt_num>lang)
    {
        GxBus_ConfigGetInt(SUBTITLE_LANG_KEY, &lang, SUBTITLE_LANG_VALUE);
        p_sub->open(p_sub, lang);
    }
}

SIGNAL_HANDLER int app_subtitling_list_get_total(GuiWidget *widget, void *usrdata)
{
    TtxSubtOps *p = &g_AppTtxSubt;

    return p->ttx_num+p->subt_num+SUBT_OFF_MODE_NUM;
}

SIGNAL_HANDLER int app_subtitling_list_get_data(GuiWidget *widget, void *usrdata)
{
    ListItemPara* item = NULL;
    Subtitling  *p_sub = &g_AppSubtitling;

    item = (ListItemPara*)usrdata;
    if(NULL == item)
        return GXCORE_ERROR;
    if(0 > item->sel)
        return GXCORE_ERROR;

    // col-0: num
    item->x_offset = 0;
    item->image = NULL;
    item->string = NULL;

    //col-1: channel name
    item = item->next;
    item->image = NULL;
    item->string = p_sub->lang[item->sel];

    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_subtitling_list_create(GuiWidget *widget, void *usrdata)
{
    TtxSubtOps          *p = &g_AppTtxSubt;
	
    s_ch_setfocus = 1;
    _refresh_list(NULL);

    if(p->sync_flag == 0)
        timer_refresh_list = create_timer(_refresh_list, 5000, 0, TIMER_REPEAT);

    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_subtitling_list_destroy(GuiWidget *widget, void *usrdata)
{
    Subtitling  *p_sub = &g_AppSubtitling;

    GxCore_Free(p_sub->lang);

	if(timer_refresh_list)
	{
		remove_timer(timer_refresh_list);
		timer_refresh_list = NULL;
	}
	
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_subtitling_list_keypress(GuiWidget *widget, void *usrdata)
{
    GUI_Event *event = NULL;
    Subtitling  *p_sub = &g_AppSubtitling;
    int ret = EVENT_TRANSFER_KEEPON;
    event = (GUI_Event *)usrdata;
    switch(event->type)
    {
        case GUI_SERVICE_MSG:
            break;

        case GUI_KEYDOWN:
            switch(event->key.sym)
            {
#if DLNA_SUPPORT
                case VK_DLNA_TRIGGER:
                    GUI_EndDialog(WND_SUBT);
                    GUI_SendEvent("win_full_screen", event);
                    break;
#endif
                case VK_BOOK_TRIGGER:
                    GUI_EndDialog(WND_SUBT);
                    break;

                case STBK_EXIT:
                case STBK_MENU:
                    GUI_EndDialog(WND_SUBT);
                    GUI_CreateDialog("wnd_channel_info");
				#if (MINI_256_COLORS_OSD_SUPPORT || MINI_16_BITS_OSD_SUPPORT)
			ret = EVENT_TRANSFER_STOP;
			#endif
                    break;

                case STBK_OK:
                    {
                        int32_t sel;
                        TtxSubtOps *p = &g_AppTtxSubt;
                        uint32_t total = p->ttx_num+p->subt_num;

                        GUI_GetProperty(LV_SUBT, "select", &sel);

                        if (total != 0)
                        {
                            if (sel == 0)
                            {
                                p_sub->close(p_sub);
                            }
                            else
                            {
                                p_sub->open(p_sub, sel);
                            }
                        }

                        GUI_EndDialog(WND_SUBT);
                    }
                    break;
                default:
                    break;
            }
    }

    return ret;
}

SIGNAL_HANDLER int app_subtitling_create(GuiWidget *widget, void *usrdata)
{
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_subtitling_destroy(GuiWidget *widget, void *usrdata)
{
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_subtitling_keypress(GuiWidget *widget, void *usrdata)
{
    GUI_Event *event = NULL;

    event = (GUI_Event *)usrdata;
    switch(event->type)
    {
        case GUI_KEYDOWN:
            switch(event->key.sym)
            {
#if DLNA_SUPPORT
                case VK_DLNA_TRIGGER:
                    GUI_EndDialog(WND_SUBT);
                    GUI_SendEvent("win_full_screen", event);
                    break;
#endif
                case VK_BOOK_TRIGGER:
                    GUI_EndDialog(WND_SUBT);
                    break;

                default:
                    break;
            }
        default:
            break;
    }

    return EVENT_TRANSFER_STOP;
}

Subtitling g_AppSubtitling = 
{
    .magz_open          = app_ttx_magz_open,
    .open    = app_subtitling_open,
    .close   = app_subtitling_close,
    .change  = app_subtitling_change,
    .pause   = app_subtitling_pause,
    .resume  = app_subtitling_resume,

};



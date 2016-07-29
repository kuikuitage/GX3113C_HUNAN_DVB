/*
 * =====================================================================================
 *
 *       Filename:  app_ttx_subt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年10月11日 14时49分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#include <gxtype.h>
#include "app_common_play.h"
#include "app_common_flash.h"

#include "module/ttx/gxttx.h"
#include "app_ttx_subt.h"
#include "gxmsg.h"
#include "gui_key.h"
#include "app_key.h"

#define EX_SHIFT_SUPPORT 0




#define STR_ID_ENGLISH    "English"
#define STR_ID_ARABIC    "Arabic"
#define STR_ID_FRENCH    "French"
#define STR_ID_PORTUGUESE    "Portuguese"
#define STR_ID_TURKISH    "Turkish"
#define STR_ID_SPAIN    "Spain"
#define STR_ID_FARSI    "Farsi"
#define STR_ID_CHINESES    "Chinese"
#define STR_ID_VIETNAMESE    "Vietnamese"
#define STR_ID_GERMAN    "German"
#define STR_ID_RUSSIAN    "Russian"
#define STR_ID_ITALIAN    "Italian"
#define STR_ID_POLSKI    "Polski"

#define LANG_MAX 2

#define TTX_LANG LANG_MAX

#define PLAYER_FOR_SUBT  "player0"
extern 	void app_subt_resume(void);
static uint32_t ttx_change_lang_to_charset(void)
{
	int lang_value = 0;
    
    GxBus_ConfigGetInt(TELTEXT_LANG_KEY, &lang_value, TTX_LANG);

#if 0
    if(lang_value >= LANG_MAX)
        return 0xFF;
    else if(strcmp(g_LangName[lang_value], STR_ID_ENGLISH) == 0)
        return English;
    else if(strcmp(g_LangName[lang_value], STR_ID_ARABIC) == 0)
        return Arabic;
    else if(strcmp(g_LangName[lang_value], STR_ID_RUSSIAN) == 0)
        return Russian;
    else if(strcmp(g_LangName[lang_value], STR_ID_FRENCH) == 0)
        return French;
    else if(strcmp(g_LangName[lang_value], STR_ID_PORTUGUESE) == 0)
        return Portuguese;
    else if(strcmp(g_LangName[lang_value], STR_ID_TURKISH) == 0)
        return Turkish;
    else if(strcmp(g_LangName[lang_value], STR_ID_SPAIN) == 0)
        return Portuguese;
    else if(strcmp(g_LangName[lang_value], STR_ID_GERMAN) == 0)
        return German;
    else if(strcmp(g_LangName[lang_value], STR_ID_FARSI) == 0)
        return Arabic;
    else
        return 0xFF;
#endif
	return English;
}



static void ttx_info_get(ttx_subt *p, Teletext *p_ttx, uint32_t elem_pid)
{
    p->elem_pid = elem_pid;
    memcpy(p->lang, p_ttx->iso639, 3);
    p->type= p_ttx->type;

    p->ttx.magazine = p_ttx->magazine;
    p->ttx.page = p_ttx->page;
}

static void subt_info_get(ttx_subt *p, Subtiling *p_subt, uint32_t elem_pid)
{
    p->elem_pid = elem_pid;
    memcpy(p->lang, p_subt->iso639, 3);
    p->type= p_subt->type;

    p->subt.composite_page_id = p_subt->composite_page_id;
    p->subt.ancillary_page_id = p_subt->ancillary_page_id;
}

static void ttx_subt_sync(TtxSubtOps* p, PmtInfo* pmt)
{                                                      
    uint32_t i=0;
    uint32_t j=0;
    uint32_t subt_num=0;
    TeletextDescriptor *p_ttx = NULL;
    SubtDescriptor     *p_subt = NULL;
    uint32_t magz_flag = 0;
    static uint32_t ttx_init_flag = 0;

    if (pmt==NULL || p==NULL)    return; 

    // ttx init here, after init ,then ttx can use
    if (ttx_init_flag == 0)
    {
        ttx_init_flag = 1;
        GxTtx_Init();
    }

    p->ttx_num = 0;
    p->subt_num = 0;

    // check total won't overflow
    for (i=0; i<pmt->ttx_count; i++)
    {
        p_ttx = (TeletextDescriptor*)pmt->ttx_info[i];
		p->ttx_num += p_ttx->ttx_num;
    }
    for (i=0; i<pmt->subt_count; i++)
    {
        p_subt = (SubtDescriptor*)pmt->subt_info[i];
        p->subt_num += p_subt->subt_num;
    }

    if (p->ttx_num+p->subt_num > TTX_SUBT_TOTAL)
    {
        printf("[ttx_subt] need lager memory!\n");
        return;
    }

    memset(&p->magazine, 0, sizeof(ttx_subt));
    memset(p->content, 0, sizeof(ttx_subt)*TTX_SUBT_TOTAL);
    
    // info get
    for (i=0; i<pmt->ttx_count; i++)
    {
        p_ttx = (TeletextDescriptor*)pmt->ttx_info[i];

        for (j=0; j<p_ttx->ttx_num; j++)
        {
            if (j==0 && magz_flag==0)
            {
                ttx_info_get(&p->magazine, &p_ttx->ttx[j], p_ttx->elem_pid);
                magz_flag = 1;
            }
            if ((p_ttx->ttx[j].type == 2)
                    || (p_ttx->ttx[j].type == 5))  /*type 2,5 for ttx subtitle*/
            {
                ttx_info_get(&p->content[subt_num], &p_ttx->ttx[j], p_ttx->elem_pid);
                subt_num++;
            }
        }
    }

    p->ttx_num = subt_num;
    for (i=0; i<pmt->subt_count; i++)
    {
        p_subt = (SubtDescriptor*)pmt->subt_info[i];

        for (j=0; j<p_subt->subt_num; j++)
        {
            subt_info_get(&p->content[subt_num], &p_subt->subt[j], p_subt->elem_pid);
            subt_num++;
        }
     }

}                                                      

static status_t ttx_magazine_open(TtxSubtOps* p)
{
    if (p == NULL)  return GXCORE_ERROR;

    if (p->magazine.elem_pid != 0)
    {
#if EX_SHIFT_SUPPORT
{
	int32_t Flag = 0;
	app_ioctl(0, APP_EXSHIFT_CHECK, &Flag);
	if(Flag)
	{
		int pidnum = 1;
		int PidData = p->magazine.elem_pid;

		{
			EXShiftPidControl_t params = {0};
			params.Num = pidnum;
			params.pData = &PidData;
			app_ioctl(0,APP_EXSHIFT_PID_CONTROL, (void *)(&params));
		}
	}
}
#endif
        GxTtx_TtxStart(p->magazine.elem_pid);
        GxTtx_SetLang(ttx_change_lang_to_charset());
        GxTtx_SetPid(p->magazine.elem_pid,p->magazine.ttx.magazine,p->magazine.ttx.page);
        GxTtx_Enable();

        return GXCORE_SUCCESS;
    }

    return GXCORE_ERROR;
}

static status_t ttx_magazine_operat(TtxSubtOps* p, uint16_t key_value)
{
    if(GxTtx_IsnotEnable() && !GxTtx_CcIsnotEnable())
    {
        if(GxTtx_IsnotWorking())
        {
            switch(key_value)
            {

                case STBK_UP:
                    GxTtx_SetKey(UpKey);
                    break;

                case STBK_DOWN:	
                    GxTtx_SetKey(DownKey);
                    break;

                case STBK_LEFT:
                    GxTtx_SetKey(LeftKey);
                    break;

                case STBK_RIGHT:	
                    GxTtx_SetKey(RightKey);
                    break;	
                case STBK_0:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey0);
                    break;
                case STBK_1:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey1);
                    break;
                case STBK_2:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey2);
                    break;
                case STBK_3:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey3);
                    break;
                case STBK_4:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey4);
                    break;
                case STBK_5:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey5);
                    break;
                case STBK_6:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey6);
                    break;
                case STBK_7:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey7);
                    break;
                case STBK_8:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey8);
                    break;
                case STBK_9:
                    GxTtx_QuickSearchEnable();
                    GxTtx_SetKey(NumKey9);
                    break;

                case APP_KEY_RED:	
                    GxTtx_SetKey(RedKey);
                    break;

                case APP_KEY_GREEN:	
                    GxTtx_SetKey(GreenKey);
                    break;

                case APP_KEY_YELLOW: 
                    GxTtx_SetKey(YellowKey);
                    break;

                case APP_KEY_BLUE:	
                    GxTtx_SetKey(BlueKey);
                    break;

                case STBK_OK:	
                    GxTtx_SetKey(OkKey);
                    break;

                case STBK_EXIT: 
                    GxTtx_SetKey(ExitKey);
                    while(!GxTtx_ExitFlag());
                   // g_AppFullArb.timer_start();
                    app_subt_resume();
                    break;

                default:
                    break;

            }            
            return GXCORE_SUCCESS;
        }
    }

    return GXCORE_ERROR;
}

static status_t ttx_subt_open(TtxSubtOps *p, uint32_t sel)
{
    if (p==NULL) return GXCORE_ERROR;
	

    if((sel<p->ttx_num) && (!GxTtx_IsnotEnable()))
    {
        GxTtx_TtxStop();
        GxTtx_TtxStart(p->content[sel].elem_pid);
#if EX_SHIFT_SUPPORT
{
	int32_t Flag = 0;
	app_ioctl(0, APP_EXSHIFT_CHECK, &Flag);
	if(Flag)
	{
		int pidnum = 1;
		int PidData = p->content[sel].elem_pid;
		{
			EXShiftPidControl_t params = {0};
			params.Num = pidnum;
			params.pData = &PidData;
			app_ioctl(0,APP_EXSHIFT_PID_CONTROL, (void *)(&params));
		}

	}
}
#endif
        GxTtx_SubtEnable(p->content[sel].elem_pid,
                p->content[sel].ttx.magazine,
                p->content[sel].ttx.page);

        return GXCORE_SUCCESS;
    }

    return GXCORE_ERROR;
}

static void ttx_subt_close(TtxSubtOps *p)
{
    if (p==NULL) return;

    if(GxTtx_CcIsnotEnable())
    {
        GxTtx_SubtDisable();
        GxTtx_TtxStop();
    }
}
    
static status_t dvb_subt_open(TtxSubtOps *p, uint32_t sel)
{
    GxMsgProperty_PlayerSubtitleLoad subt = {0};
    GxMsgProperty_PlayerSubtitleSwitch subt_switch = {0};

	printf("dvb_subt_open open sel = %d\n",sel);

    if(p->subt_num > sel)
    {
        // offset ttx subt
        sel += p->ttx_num;
// add temp
#if EX_SHIFT_SUPPORT
		{
			int32_t Flag = 0;
			app_ioctl(0, APP_EXSHIFT_CHECK, &Flag);
			if(Flag)
			{
				int pidnum = 1;
				int PidData = p->content[sel].elem_pid;
				{
					EXShiftPidControl_t params = {0};
					params.Num = pidnum;
					params.pData = &PidData;
					app_ioctl(0,APP_EXSHIFT_PID_CONTROL, (void *)(&params));
				}

			}
		}
#endif
        if(NULL == p->player_subt)
        {
            // first open subt
            memset(&subt,0,sizeof(GxMsgProperty_PlayerSubtitleLoad));
            subt.player = PLAYER_FOR_SUBT;
            subt.para.pid.pid =  p->content[sel].elem_pid;
            
            subt.para.pid.major = p->content[sel].subt.composite_page_id;
            subt.para.pid.minor = p->content[sel].subt.ancillary_page_id;
           
            subt.para.render = PLAYER_SUB_RENDER_SPP;
            subt.para.type = PLAYER_SUB_TYPE_DVB;
			printf("subt pid = %d\n",subt.para.pid.pid);

            app_send_msg(GXMSG_PLAYER_SUBTITLE_LOAD, (void*)&subt);
            p->player_subt =  (PlayerSubtitle*)subt.sub;
        }
        else
        {
            // subt switch
            subt_switch.player = PLAYER_FOR_SUBT;
            subt_switch.sub = p->player_subt;
            subt_switch.pid.pid =p->content[sel].elem_pid;

            subt_switch.pid.major = p->content[sel].subt.composite_page_id;
            subt_switch.pid.minor = p->content[sel].subt.ancillary_page_id;

            app_send_msg(GXMSG_PLAYER_SUBTITLE_SWITCH, (void*)&subt_switch);
        }

        return GXCORE_SUCCESS;
    }

    return GXCORE_ERROR;
}

static void dvb_subt_close(TtxSubtOps *p)
{
    GxMsgProperty_PlayerSubtitleUnLoad unload;

    if (p==NULL)    return;

    if(p->player_subt != NULL)
    {
        unload.player = PLAYER_FOR_SUBT;
        unload.sub = p->player_subt;

        app_send_msg(GXMSG_PLAYER_SUBTITLE_UNLOAD, (void*)&unload);
        p->player_subt = NULL;
    }
}

TtxSubtOps g_AppTtxSubt =
{
    .sync   = ttx_subt_sync,

    .ttx_magz_open  = ttx_magazine_open,
    .ttx_magz_opt   = ttx_magazine_operat,
    
    .ttx_subt_open  = ttx_subt_open,
    .ttx_subt_close  = ttx_subt_close,
    
    .dvb_subt_open  = dvb_subt_open,
    .dvb_subt_close  = dvb_subt_close
};


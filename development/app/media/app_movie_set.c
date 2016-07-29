#include "app.h"
#include "app_pop.h"

#define AUDIO_DELAY_SUPPORT 0

#define WIN_MOVIE_VIEW              "win_movie_view"
#define BOX_MOVIE_SET				"movie_set_box"
#define EDIT_SEEK_TIME				"movie_set_edit_seek"
#define COMBOBOX_AUDIO_CHANNEL		"movie_set_combo_audio_channel"
#define COMBOBOX_SWITCH_SEQUENCE	"movie_set_combo_play_sequence"
#define BUTTON_SET_DELAY            "movie_set_button_delay"
#define COMBOBOX_DISPLAY_MODE		"movie_set_combo_display_mode"

enum
{
    MOV_SEEK = 0,
    MOV_AUDIO_CH,
    MOV_PLAY_MODE,
#if AUDIO_DELAY_SUPPORT > 0
    MOV_AUDIO_DELAY,
#endif
    MOV_VIEW_MODE
};

int audio_delay_ms = 0;
static int s_movie_set_total_time = 0;
static int edit_set_seek(int seek_ms)
{
	GxMessage *msg;

	GxMsgProperty_PlayerSeek *seek;
	msg = GxBus_MessageNew(GXMSG_PLAYER_SEEK);
	APP_CHECK_P(msg, 1);
	seek = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerSeek);
	APP_CHECK_P(seek, 1);
	seek->player = PMP_PLAYER_AV;
	seek->time = seek_ms;
	seek->flag = SEEK_ORIGIN_SET;
	GxBus_MessageSendWait(msg);
	GxBus_MessageFree(msg);

	return 0;
}

static int combobox_init_audio_channel(void)
{
	PlayerProgInfo info;

	memset(&info, 0, sizeof(info));
	GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &info);
	if((0 >= info.audio_num)||(PLAYER_MAX_TRACK_AUDIO < info.audio_num))
	{
		GUI_SetProperty(COMBOBOX_AUDIO_CHANNEL, "content", (void*)"[none]");
		return 1;
	}

	char* combobox_buf = NULL;
	char* combobox_node = NULL;
	char* default_buf_data = "aud";
	char default_buf_index[20] = {0};
	int i = 0;

	combobox_buf = GxCore_Malloc(PLAYER_MAX_TRACK_AUDIO*PLAYER_TARCK_NAME_LONG + PLAYER_MAX_TRACK_AUDIO + 100);
	if(NULL == combobox_buf)
        return 1;
	memset(combobox_buf, 0, sizeof(combobox_buf));

	/*combobox context*/
	strcpy(combobox_buf,  "[");
	for(i = 0; i < info.audio_num; i++)
	{
		/*add ,*/
		if(0 != i) strcat(combobox_buf, ",");

		/*add index*/
		memset(default_buf_index, 0, sizeof(default_buf_index));
		sprintf(default_buf_index, "%d.", i);
		strcat(combobox_buf, default_buf_index);

		/*add node*/
		if('\0' != info.audio[i].track_name[0])
			combobox_node = info.audio[i].track_name;
		else
			combobox_node = info.audio[i].lang;

		//default combobox_node
		if((NULL == combobox_node) || (0 == strlen(combobox_node)))
		{
			combobox_node = default_buf_data;
		}

		strcat(combobox_buf, combobox_node);
	}
	strcat(combobox_buf,  "]");

	printf("AUD: combobox_buf %s\n", combobox_buf);
	GUI_SetProperty(COMBOBOX_AUDIO_CHANNEL, "content", (void*)combobox_buf);
	GUI_SetProperty(COMBOBOX_AUDIO_CHANNEL, "select", &info.cur_audio_id);
	APP_FREE(combobox_buf);

	return 0;
}


static int combobox_set_audio_channel(int index)
{
	PlayerProgInfo info;
	GxMessage *msg;

	memset(&info, 0, sizeof(info));
	GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &info);
	if((index >= info.audio_num) || (index == info.cur_audio_id))
        return 1;

	GxMsgProperty_PlayerAudioSwitch *audio_switch;
	msg = GxBus_MessageNew(GXMSG_PLAYER_AUDIO_SWITCH);
	APP_CHECK_P(msg, 1);
	audio_switch = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerAudioSwitch);
	APP_CHECK_P(audio_switch, 1);
	audio_switch->player = PMP_PLAYER_AV;
	audio_switch->pid = index;
	GxBus_MessageSendWait(msg);
	GxBus_MessageFree(msg);
	return 0;
}

#if AUDIO_DELAY_SUPPORT > 0
static int button_set_audio_delay(int delay_ms)
{
	GxMessage *msg;

	GxMsgProperty_PlayerAudioSync *audio_delay;
	msg = GxBus_MessageNew(GXMSG_PLAYER_AUDIO_SYNC);
	APP_CHECK_P(msg, 1);
	audio_delay = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerAudioSync);
	APP_CHECK_P(audio_delay, 1);
	audio_delay->player = PMP_PLAYER_AV;
	audio_delay->timems = delay_ms;
	GxBus_MessageSendWait(msg);
	GxBus_MessageFree(msg);

	return 0;
}
#endif

static status_t key_ok(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;	

	GUI_GetProperty(BOX_MOVIE_SET, "select", (void*)&item_sel);

	switch(item_sel)
	{
		/*seek*/
		case 0:
			{
				/*get*/
				char* str_seek = NULL;
				int ms_seek = 0;
				int hour = 0;
				int minite = 0;
				int second = 0;
				PlayerProgInfo  info;

				memset(&info, 0, sizeof(info));
				GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &info);	
				s_movie_set_total_time = info.duration;

				GUI_GetProperty(EDIT_SEEK_TIME, "string", (void*)&str_seek);
				if(NULL == str_seek) break;
				sscanf(str_seek, "%d:%d:%d", &hour, &minite, &second);
				printf("[MOVIE] seek %d:%d:%d\n", hour, minite, second);
				ms_seek = (hour*60*60 + minite*60 + second) * 1000;

				/*check*/

				/*dealy with ms*/
				if(ms_seek == (s_movie_set_total_time - s_movie_set_total_time%1000))
				{
					ms_seek = s_movie_set_total_time;
				}

				if(ms_seek > s_movie_set_total_time)
				{
					int sel = 0;
				
					PopDlg pop;
					memset(&pop, 0, sizeof(PopDlg));
					pop.type = POP_TYPE_OK;
					pop.format = POP_FORMAT_DLG;
					pop.str = "Seek time error";
					pop.mode = POP_MODE_UNBLOCK;
					pop.timeout_sec = 6;
					pop.pos.x = POP_MEDIA_X_START;
					pop.pos.y = POP_MEDIA_Y_START;			
					popdlg_create(&pop);

					GUI_SetProperty(EDIT_SEEK_TIME, "string", "00:00:00");
					GUI_SetProperty(EDIT_SEEK_TIME, "select", &sel);
					GUI_SetProperty(EDIT_SEEK_TIME, "draw_now", NULL);
						
//					media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);
					break;
				}

				/*set*/
				edit_set_seek(ms_seek);
				GUI_EndDialog("win_movie_set");
				break;
			}

		default:
			break;
	}
	printf("[MOVIE] keypress_movie_view_set_ok item:%d, value:%d\n", item_sel, value_sel);

	return GXCORE_SUCCESS;
}

static status_t key_left_right(unsigned short value)
{
    uint32_t item_sel = 0;
    uint32_t value_sel = 0;
    GUI_GetProperty(BOX_MOVIE_SET, "select", (void*)&item_sel);

    switch(item_sel)
    {
        /*audio channel*/
        case MOV_AUDIO_CH:
            {
                /*get*/
                GUI_GetProperty(COMBOBOX_AUDIO_CHANNEL, "select", (void*)&value_sel);
                /*set*/
                combobox_set_audio_channel(value_sel);
                break;
            }

            /*sequence*/
        case MOV_PLAY_MODE:
            {
                GUI_GetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value_sel);
                pmpset_set_int(PMPSET_MOVIE_PLAY_SEQUENCE, value_sel);
                break;
            }

#if AUDIO_DELAY_SUPPORT > 0
            /*audio delay*/
        case MOV_AUDIO_DELAY:
            {
                char string[15] = {0};
                if(APPK_LEFT==value)
                {
                    if(audio_delay_ms >= -4800)
                        audio_delay_ms -= 200;
                }
                else
                {
                    if(audio_delay_ms < 5000)
                        audio_delay_ms += 200;
                }
                memset(string,0,sizeof(string));
                sprintf(string, "%dms", audio_delay_ms);
                GUI_SetProperty(BUTTON_SET_DELAY, "string", (void*)string);

                button_set_audio_delay(audio_delay_ms);
            }
            break;
#endif
            // for display mode
        case MOV_VIEW_MODE:
            GUI_GetProperty(COMBOBOX_DISPLAY_MODE, "select", (void*)&value_sel);
            pmpset_set_int(PMPSET_ASPECT_RATIO, value_sel);	 
            break;

        default:
            break;
    }

    return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int movie_set_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	event = (GUI_Event *)usrdata;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;
	//error#4252
	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			//gx_printf("dgw----> movie set  %d  %d\n",player_status->status,PLAYER_STATUS_PLAY_END);
			if(PLAYER_STATUS_PLAY_END == player_status->status)
				GUI_EndDialog("win_movie_set");
			break;
		default:
			break;
	}
	GUI_SendEvent(WIN_MOVIE_VIEW, event);

	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int movie_set_init(const char* widgetname, void *usrdata)
{
    int32_t value = 0;
    PlayerProgInfo info;
    char* osd_language=NULL;

    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {          
        GUI_SetProperty("movie_set_image_logo", "img", "MP_SETTING_ch.bmp");
    }    
    else if (0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {         
        GUI_SetProperty("movie_set_image_logo", "img", "MP_SETTING.bmp");
    }   

#if MEDIA_SUBTITLE_SUPPORT
    subtitle_pause();
#endif

    memset(&info, 0, sizeof(info));
    GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &info);

    /*seek*/

    /*audio channel*/
    combobox_init_audio_channel();

    /*sequence*/
    value = pmpset_get_int(PMPSET_MOVIE_PLAY_SEQUENCE);
    GUI_SetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value);

#if AUDIO_DELAY_SUPPORT > 0
    /*audio delay*/
    //TODO: get audio_delay_ms from player
    char string[10] = {0};
    sprintf(string, "%dms", audio_delay_ms);
    GUI_SetProperty(BUTTON_SET_DELAY, "string", (void*)string);
#endif

    /*display mode*/
    value = pmpset_get_int(PMPSET_ASPECT_RATIO);
    GUI_SetProperty(COMBOBOX_DISPLAY_MODE, "select", (void*)&value);
    if(info.is_radio == 1)//Radio
    {
        //GUI_SetProperty("movie_set_boxitem3","state","disable");
#if AUDIO_DELAY_SUPPORT > 0
		GUI_SetProperty("movie_set_boxitem3","state","disable");
#endif
        GUI_SetProperty("movie_set_boxitem4","state","disable");
    }
    else
    {
        //GUI_SetProperty("movie_set_boxitem3","state","enable");
#if AUDIO_DELAY_SUPPORT > 0
		GUI_SetProperty("movie_set_boxitem3", "enable", NULL);
        GUI_SetProperty("movie_set_boxitem3","state","enable");
#endif
		GUI_SetProperty("movie_set_boxitem4", "enable", NULL);
        GUI_SetProperty("movie_set_boxitem4","state","enable");
    }
    return 0;
}

SIGNAL_HANDLER int movie_set_destroy(const char* widgetname, void *usrdata)
{
#if MEDIA_SUBTITLE_SUPPORT
	subtitle_resume();
#endif
	return 0;
}


SIGNAL_HANDLER int movie_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_LEFT:
			key_left_right(event->key.sym);
			break;
		case APPK_RIGHT:
			key_left_right(event->key.sym);
			break;				

		case APPK_OK:
			key_ok();
			break;

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

			GUI_SetProperty(COMBOBOX_SWITCH_SEQUENCE, "select", (void*)&value);
			break;
		}
			
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}



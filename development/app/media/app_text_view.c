#include "app.h"
#include "app_pop.h"
//widget
#define NOTEPAD					"text_view_notepad"

uint32_t stop_line = 0;
uint32_t text_roll_line_num = 1;
//int s_start_line = 0;
extern event_list* text_auto_roll_timer ;
extern int text_roll_start(void* usrdata);
extern bool music_play_state(void);
extern void music_status_init(void);

static int  text_get_open_size(const char* fname)
{
#define FILE_MODE_ "r"
#define MAX_TEXT_FILE_SIZE    (512*1024)
	int fd;
	int len = 0;
	GxFileInfo info;

	fd = GxCore_Open(fname,FILE_MODE_);
	if(fd < 0 )
	{
		GxCore_FileDelete(fname);
		fd = GxCore_Open(fname,FILE_MODE_);
		if(fd < 0)
		{
			printf("can not open file %s\n",fname);
			return 1;
		}
	}

	GxCore_GetFileStat(fd,&info);
	len =  info.size_by_bytes;
	GxCore_Close(fd);
	if(len > MAX_TEXT_FILE_SIZE)
	{
		{
			PopDlg pop = {0};
			memset(&pop, 0, sizeof(PopDlg));
			pop.type = POP_TYPE_NO_BTN;
			pop.format = POP_FORMAT_DLG;
			pop.str = "Text file is too large!";
			pop.mode = POP_MODE_UNBLOCK;
			pop.timeout_sec = 3;
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;	
//			media_popmsg(pop.pos.x, pop.pos.y,pop.str, MEDIA_POPMSG_TYPE_OK);				
			popdlg_create(&pop);  
		}
	}

	return len;
}

int text_file_get_open_size(void)
{
	uint32_t len = 0;	
	play_list* list = NULL;
	char* path = NULL;
	list = play_list_get(PLAY_LIST_TYPE_TEXT);
	APP_CHECK_P(list, GXCORE_ERROR);
	if(list->play_no >= list->nents) return GXCORE_ERROR;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	APP_CHECK_P(path, GXCORE_ERROR);

	len = text_get_open_size(path);

	return len;
}
static int text_service_status(GxMsgProperty_PlayerStatusReport* player_status)
{
	APP_CHECK_P(player_status, 1);
	
	switch(player_status->status)
	{
		case PLAYER_STATUS_PLAY_START:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_START\n");
			break;
			
		case PLAYER_STATUS_ERROR:
			APP_Printf("[SERVICE] PLAYER_STATUS_ERROR, %d\n", player_status->error);
			
		case PLAYER_STATUS_PLAY_END:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_END\n");

			//view pic, player music in background
			if((0 == strcmp(player_status->player, PMP_PLAYER_AV))
				&& (music_play_state() == true))
			{
				//sequence
				pmpset_music_play_sequence sequence = 0;
				sequence = pmpset_get_int(PMPSET_MUSIC_PLAY_SEQUENCE);
				if(PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE == sequence)
				{
					APP_Printf("play only once\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
					music_status_init();
					break;
				}
				else if(PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE == sequence)
				{
					APP_Printf("play repeat one\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_PLAY);
					break;
				}
				else if(PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE == sequence)
				{
					play_list* list = NULL;
					list = play_list_get(PLAY_LIST_TYPE_MUSIC);	
					 if(list->play_no >= list->nents -1)
					{
						play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
						music_status_init();
					}
					else
					{
						play_music_ctrol(PLAY_MUSIC_CTROL_NEXT);
					}

					break;
				}
				/*else if(PMPSET_MUSIC_PLAY_SEQUENCE_RANDOM == sequence)
				{
					APP_Printf("play random\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_RANDOM);
					break;
				}*/
				else
				{
					//APP_Printf("play next\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
					music_status_init();
					break;
				}						
			}
			

		case PLAYER_STATUS_PLAY_RUNNING:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_RUNNING\n");
			break;


		default:
			break;
	}

	return 0;
}

SIGNAL_HANDLER  int text_view_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;
	
	event = (GUI_Event *)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			text_service_status(player_status);
			break;

		default:
			break;
	}
	
	
	return EVENT_TRANSFER_STOP;
}

static int _text_start_roll(PopDlgRet ret)
{
	uint32_t value_sel = 0;	
	char rows[]={1,3,5};
	play_list* list = NULL;
	char* path = NULL;

	/*list get*/
	list = play_list_get(PLAY_LIST_TYPE_TEXT);
	APP_CHECK_P(list, GXCORE_ERROR);
	if(list->play_no >= list->nents) return GXCORE_ERROR;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	APP_CHECK_P(path, GXCORE_ERROR);

#if 0
    if(ret != POP_VAL_OK)
    {
        s_start_line = 0;
        /*save tag*/
		pmp_save_tag(path, s_start_line);
    }
#endif
	GUI_SetProperty(NOTEPAD, "file", path);
#if 0
	if(0 < s_start_line)
		GUI_SetProperty(NOTEPAD, "cur_pos", &s_start_line);
#endif
		
	/*roll line num*/
	value_sel = pmpset_get_int(PMPSET_TEXT_ROLL_LINES);
	text_roll_line_num=rows[value_sel];

	/*auto roll*/
	value_sel = pmpset_get_int(PMPSET_TEXT_AUTO_ROLL);
	if(value_sel==PMPSET_TONE_ON)
	{
		text_auto_roll_timer = create_timer(text_roll_start, 5000, 0, TIMER_REPEAT);
	}
    return 0;
}

SIGNAL_HANDLER int text_view_init(const char* widgetname, void *usrdata)
{
	/*list get*/
	play_list* list = NULL;
	char* path = NULL;
	list = play_list_get(PLAY_LIST_TYPE_TEXT);
	APP_CHECK_P(list, GXCORE_ERROR);
	if(list->play_no >= list->nents) return GXCORE_ERROR;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	APP_CHECK_P(path, GXCORE_ERROR);

#if 0 //GUI did not support jump to line function.
	/*load tag*/
	s_start_line = pmp_load_tag(path);
	if(0 < s_start_line)
	{
		PopDlg  pop;
		memset(&pop, 0, sizeof(PopDlg));
		pop.type = POP_TYPE_YES_NO;
        pop.format = POP_FORMAT_DLG;
		pop.mode = POP_MODE_UNBLOCK;
       	pop.str = STR_ID_CONTINUE_VIEW;
       	pop.title = NULL;
       	pop.exit_cb = _text_start_roll;
		popdlg_create(&pop);
	}
	else
#endif
	{
		_text_start_roll(POP_VAL_CANCEL);
	}

	app_set_win_create_flag(MEDIA_TEXT_WIN);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int text_view_destroy(const char* widgetname, void *usrdata)
{		
	/*list get*/
	play_list* list = NULL;
	char* path = NULL;
	list = play_list_get(PLAY_LIST_TYPE_TEXT);
	APP_CHECK_P(list, GXCORE_ERROR);
	if(list->play_no >= list->nents) return GXCORE_ERROR;
	path = explorer_static_path_strcat(list->path, list->ents[list->play_no]);
	APP_CHECK_P(path, GXCORE_ERROR);
	
	/*save tag*/
	if(0 < stop_line)
	{
		pmp_save_tag(path, stop_line);
	}

	/*auto roll*/
	if(text_auto_roll_timer)
	{
		remove_timer(text_auto_roll_timer);
		text_auto_roll_timer = NULL;
	}

	app_set_win_destroy_flag(MEDIA_TEXT_WIN);

      return 0;
}

SIGNAL_HANDLER int text_view_keypress(const char* widgetname, void *usrdata)
{
    GUI_Event *event = NULL;
    uint32_t value=1;

    if(NULL == usrdata) return EVENT_TRANSFER_STOP;

    event = (GUI_Event *)usrdata;
    switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
    {
        case APPK_BACK:
            GUI_GetProperty(NOTEPAD, "cur_pos", &stop_line);
            //printf("########[%s]%d, stop_line = %d #########\n", __func__, __LINE__, stop_line);
            GUI_EndDialog("win_text_view");
            return EVENT_TRANSFER_STOP;

        case APPK_MENU:
            GUI_GetProperty(NOTEPAD, "cur_pos", &stop_line);
            //printf("########[%s]%d, stop_line = %d #########\n", __func__, __LINE__, stop_line);
            GUI_EndDialog("win_text_view");
            return EVENT_TRANSFER_STOP;

        case APPK_UP:
            GUI_SetProperty(NOTEPAD, "line_up", &text_roll_line_num);
            return EVENT_TRANSFER_STOP;

        case APPK_DOWN:
            GUI_SetProperty(NOTEPAD, "line_down", &text_roll_line_num);
            return EVENT_TRANSFER_STOP;

        case APPK_LEFT:
        case APPK_PAGE_UP:
            GUI_SetProperty(NOTEPAD, "page_up", &value);
            return EVENT_TRANSFER_STOP;

        case APPK_RIGHT:
        case APPK_PAGE_DOWN:
            GUI_SetProperty(NOTEPAD, "page_down", &value);
            return EVENT_TRANSFER_STOP;

        case APPK_OK:
            break;

        case APPK_SET:
            GUI_CreateDialog("win_text_set");
            return EVENT_TRANSFER_STOP;

        default:
            break;
    }


    return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int text_view_got_focus(const char* widgetname, void *usrdata)
{
	reset_timer(text_auto_roll_timer);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER  int text_view_lost_focus(const char* widgetname, void *usrdata)
{
	timer_stop(text_auto_roll_timer);
	return EVENT_TRANSFER_STOP;
}




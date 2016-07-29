#include "app.h"
#include "app_pop.h"
#include "app_utility.h"
#include "pmp_subtitle.h"

static char *s_file_path = NULL;
static char *s_file_path_bak = NULL;
static int32_t s_subt_type = 0;
static PlayerProgInfo s_subt_info;

typedef enum
{
    MOVIE_SUBT_TYPE = 0,
    MOVIE_SUBT_FILE,
    MOVIE_SUBT_VIS,
    MOVIE_SUBT_SEL,
    MOVIE_SUBT_DELAY,

}SubtBox;

#define WIN_MOVIE_VIEW                  "win_movie_view"
#define BOX_MOVIE_SUBT					"movie_subt_box"
#define BUTTON_SUBT_LOAD				"movie_subt_button_load"
#define BOXITEM_SUBT_LOAD				"movie_subt_boxitem0"
#define COMBOBOX_SUBT_VISIBILITY		"movie_subt_combo_visibility"
#define COMBOBOX_SUBT_SELECT			"movie_subt_combo_select"
#define COMBOBOX_SUBT_TYPE			"movie_subt_combo_type"
#define BUTTON_SUBT_DELAY				"movie_subt_button_delay"

static int combobox_set_visibility(PlayerSubtitle* list, pmpset_tone tone)
{	
	if(NULL == list) return 1;

	if(PMPSET_TONE_ON == tone)
	{
		GxSubtPrintf("GXMSG_PLAYER_SUBTITLE_SHOW\n");
		GxPlayer_MediaSubHide(PMP_PLAYER_AV,list);
		pmpset_set_int(PMPSET_MOVIE_SUBT_VISIBILITY, PMPSET_TONE_ON);
	}
	else
	{
		GxSubtPrintf("GXMSG_PLAYER_SUBTITLE_HIDE\n");
		GxPlayer_MediaSubShow(PMP_PLAYER_AV,list);
		pmpset_set_int(PMPSET_MOVIE_SUBT_VISIBILITY, PMPSET_TONE_OFF);
	}
	return 0;
}

static int combobox_init_select(pmp_subt_para* para)
{	
	char* combobox_buf = NULL;
	char* combobox_node = NULL;
	char default_buf[20];
	uint32_t i = 0;

	memset(&s_subt_info, 0, sizeof(PlayerProgInfo));
	GxPlayer_MediaGetProgInfoByName(PMP_PLAYER_AV, &s_subt_info);
	
	if(0 >= s_subt_info.sub_num||PLAYER_MAX_TRACK_SUB <s_subt_info.sub_num)
	{
		GUI_SetProperty(COMBOBOX_SUBT_SELECT, "content", (void*)"[subt 0]");
		return 1;
	}

	combobox_buf = GxCore_Malloc(PLAYER_MAX_TRACK_SUB*PLAYER_TARCK_LANG_LONG + PLAYER_MAX_TRACK_SUB+20);
	if(NULL == combobox_buf) return 1;
	memset(combobox_buf, 0, sizeof(combobox_buf));

	/*combobox context*/
	strcpy(combobox_buf,  "[");
	for(i = 0; i < s_subt_info.sub_num; i++)
	{
		if(0 != i) strcat(combobox_buf, ",");
		
		combobox_node = s_subt_info.sub[i].lang;
		
		/*default str*/
		if((NULL == combobox_node) || (0 == strlen(combobox_node)))
		{
			memset(default_buf, 0, sizeof(default_buf));
			sprintf(default_buf, "subt %d", i);
			combobox_node = default_buf;
		}

		strcat(combobox_buf, combobox_node);
	}
	strcat(combobox_buf,  "]");

	GxSubtPrintf("SUBT: combobox_buf %s\n", combobox_buf);
	GUI_SetProperty(COMBOBOX_SUBT_SELECT, "content", (void*)combobox_buf);
	GUI_SetProperty(COMBOBOX_SUBT_SELECT, "select", (void*)&para->cur_subt);
	APP_FREE(combobox_buf);

	return 0;
}

static int combobox_set_select(PlayerSubtitle* list, int index)
{
	PlayerSubPID subt_pid;

	if(NULL == list) return 1;
	
	subt_pid.pid = s_subt_info.sub[index].id;
	subt_pid.major = 0;
	subt_pid.minor = 0;
	
	GxSubtPrintf("GXMSG_PLAYER_SUBTITLE_SWITCH index = %d pid = %d\n", index, subt_pid.pid);
	GxPlayer_MediaSubSwitch(PMP_PLAYER_AV,list,subt_pid);

	return 0;
}

static int combobox_set_sync(PlayerSubtitle* list, int timems)
{
	if(NULL == list) return 1;

	GxSubtPrintf("GXMSG_PLAYER_SUBTITLE_SYNC\n");	
	GxPlayer_MediaSubSync(PMP_PLAYER_AV, list, timems);
		
	return 0;
}

static int combobox_update(pmp_subt_para* subt_para)
{
	int32_t value = 0;
	char string[10];

	combobox_init_select(subt_para);
	
	value = PMPSET_TONE_ON;
	GUI_SetProperty(COMBOBOX_SUBT_VISIBILITY, "select", (void*)&value);
	combobox_set_visibility(subt_para->list, value);
	subt_para->state = 1;

	subt_para->delay_ms = 0;
	memset(string,0,sizeof(string));
	sprintf(string, "%dms", subt_para->delay_ms);
	GUI_SetProperty(BUTTON_SUBT_DELAY, "string", (void*)string);
	GxPlayer_MediaSubSync(PMP_PLAYER_AV, subt_para->list,  subt_para->delay_ms);

	return 0;
}

void movie_sub_from_usb(void)
{
	int str_len = 0;
	WndStatus get_path_ret = WND_CANCLE;
	
	if(check_usb_status() == false)
	{
		PopDlg  pop;
		memset(&pop, 0, sizeof(PopDlg));
    		pop.type = POP_TYPE_OK;
   		pop.str = STR_ID_INSERT_USB;
   		pop.mode = POP_MODE_UNBLOCK;
		pop.pos.x =POP_MEDIA_X_START;
		pop.pos.y = POP_MEDIA_Y_START;			
		popdlg_create(&pop);
//		media_popmsg(pop.pos.x, pop.pos.y, pop.str, POPMSG_TYPE_OK);
	}
	else
	{
		FileListParam file_para;
		memset(&file_para, 0, sizeof(file_para));

		file_para.cur_path = s_file_path_bak;
		file_para.dest_path = &s_file_path;
		file_para.suffix = SUFFIX_SUBT;
		file_para.dest_mode = DEST_MODE_FILE;
		
		get_path_ret = app_get_file_path_dlg(&file_para);
		
		if(get_path_ret == WND_OK)
		{
			if(s_file_path != NULL)
			{
				////backup the path...////
				if(s_file_path_bak != NULL)
				{
					GxCore_Free(s_file_path_bak);
					s_file_path_bak = NULL;
				}
			
				str_len = strlen(s_file_path);
				s_file_path_bak = (char *)GxCore_Malloc(str_len + 1);
				if(s_file_path_bak != NULL)
				{
					memcpy(s_file_path_bak, s_file_path, str_len);
					s_file_path_bak[str_len] = '\0';
				}
			}
		}
	}
}

static status_t key_ok(void)
{
	uint32_t item_sel = 0;

	pmp_subt_para* subt_para = NULL;
	subt_para = subtitle_get();
	APP_CHECK_P(subt_para, GXCORE_ERROR);


	GUI_GetProperty(BOX_MOVIE_SUBT, "select", (void*)&item_sel);
	switch(item_sel)
	{
		/*load*/
		/*case 1:{
			//TODO:
			//char* subt_file = NULL;
			//subt_file = app_open_file(explorer_view->path, SUFFIX_SUBT);
	
			movie_sub_from_usb();
			
			if(s_file_path)
			{
				subtitle_start(s_file_path, PMP_SUBT_LOAD_OUTSIDE);
				combobox_update(subt_para);
				GUI_SetProperty(BUTTON_SUBT_LOAD, "string", (void*)s_file_path);
			}
			else
			{
				GUI_SetProperty(BUTTON_SUBT_LOAD, "string", (void*)(subt_para->file));
			}
			break;
			}*/
		default:
			break;
	}		
	return GXCORE_SUCCESS;
}

static status_t key_leftright(unsigned short value)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;
	char string[15];
	
	pmp_subt_para* subt_para = NULL;
	subt_para = subtitle_get();
	APP_CHECK_P(subt_para, 1);
	
	GUI_GetProperty(BOX_MOVIE_SUBT, "select", (void*)&item_sel);
	switch(item_sel)
	{	
		//type
		case MOVIE_SUBT_TYPE:
			{
				GUI_GetProperty(COMBOBOX_SUBT_TYPE, "select", &s_subt_type);
				if(s_subt_type == 1)//outside->inside
				{	
					printf("###outside->inside\n");
					subtitle_start(NULL, PMP_SUBT_LOAD_INSIDE);
					combobox_update(subt_para);
					GUI_SetProperty(BOXITEM_SUBT_LOAD, "state", "disable");
					GUI_SetProperty(BUTTON_SUBT_LOAD, "string", " ");
				}
				else//inside->outside
				{
					printf("###inside->outside\n");
					subtitle_start(NULL, PMP_SUBT_LOAD_OUTSIDE);
					GUI_SetProperty(BOXITEM_SUBT_LOAD, "state", "enable");
					combobox_update(subt_para);
					if(subt_para->file != NULL)
					{
						GUI_SetProperty(BUTTON_SUBT_LOAD, "string", (void*)(subt_para->file));
					}
					else
					{
						GUI_SetProperty(BUTTON_SUBT_LOAD, "string", " ");
					}
				}
			}
			break;
		/*visibility*/
		case MOVIE_SUBT_VIS:{
			GUI_GetProperty(COMBOBOX_SUBT_VISIBILITY, "select", &value_sel);
			combobox_set_visibility(subt_para->list, value_sel);
			break;
			}
		/*select*/
		case MOVIE_SUBT_SEL:{
			GUI_GetProperty(COMBOBOX_SUBT_SELECT, "select", (void*)&value_sel);
			subt_para->cur_subt = value_sel;
			combobox_set_select(subt_para->list, value_sel);
			break;
			}
		case MOVIE_SUBT_DELAY:	
			if(NULL == subt_para) return GXCORE_ERROR;

			if(APPK_LEFT==value)
			{
				if(subt_para->delay_ms >= -4800 )
					subt_para->delay_ms -= 200;
			}
			else 
			{
				if(subt_para->delay_ms < 5000 )
					subt_para->delay_ms += 200;
			}	
			memset(string,0,sizeof(string));
			sprintf(string, "%dms", subt_para->delay_ms);
			GUI_SetProperty(BUTTON_SUBT_DELAY, "string", (void*)string);
			combobox_set_sync(subt_para->list, subt_para->delay_ms);
			break;
			
		default:
			break;
	}
	
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER  int movie_subt_service(const char* widgetname, void *usrdata)
{
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
				GUI_EndDialog("win_movie_subt");
			break;
		default:
			break;
	}
	GUI_SendEvent(WIN_MOVIE_VIEW, event);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int movie_subt_init(const char* widgetname, void *usrdata)
{	
	int32_t value = 0;
	char string[10];
	
	subtitle_pause();
	
	pmp_subt_para* subt_para = NULL;
	subt_para = subtitle_get();
	APP_CHECK_P(subt_para, 1);

	if(subt_para->para.type == PLAYER_SUB_TYPE_INSIDE)
	{
		s_subt_type = 1;
		GUI_SetProperty(COMBOBOX_SUBT_TYPE, "select", &s_subt_type);
		GUI_SetProperty(BOXITEM_SUBT_LOAD, "state", "disable");
		GUI_SetProperty(BUTTON_SUBT_LOAD, "string", " ");
	}
	else
	{
		s_subt_type = 0;
		GUI_SetProperty(COMBOBOX_SUBT_TYPE, "select", &s_subt_type);
		GUI_SetProperty(BOXITEM_SUBT_LOAD, "state", "enable");
		//load
		if(subt_para->file != NULL)
			GUI_SetProperty(BUTTON_SUBT_LOAD, "string", (void*)(subt_para->file));
		else
			GUI_SetProperty(BUTTON_SUBT_LOAD, "string", " ");
	}
	
	/*visibility*/
	value = pmpset_get_int(PMPSET_MOVIE_SUBT_VISIBILITY);
	GUI_SetProperty(COMBOBOX_SUBT_VISIBILITY, "select", (void*)&value);

	/*select*/
	combobox_init_select(subt_para);

	/*subt delay*/
	//TODO: get subt_delay_ms from player
	memset(string,0,sizeof(string));
	sprintf(string, "%dms", subt_para->delay_ms);
	GUI_SetProperty(BUTTON_SUBT_DELAY, "string", (void*)string);
	
	return 0;
}

SIGNAL_HANDLER int movie_subt_destroy(const char* widgetname, void *usrdata)
{
	subtitle_resume();
	return 0;
}


SIGNAL_HANDLER int movie_subt_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_LEFT:
			key_leftright(event->key.sym);
			break;
		case APPK_RIGHT:
			key_leftright(event->key.sym);
			break;				

		case APPK_OK:
			key_ok();
			break;
			
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}



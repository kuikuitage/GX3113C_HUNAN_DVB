#include "app.h"
//#include "app_module.h"
//#include "app_send_msg.h"
//#include "app_epg.h"
#include "app_pop.h"
#include "app_pvr.h"
//#include "full_screen.h"
extern bool music_play_state(void);
extern void music_status_init(void);
extern	void app_create_pvr_media_menu(void);
//static bool s_media_on_fullscreen = false;

//resourse

//widget
#define BUTTON_DOWNLOAD			"media_centre_button_download"
#define IMAGE_DOWNLOAD			"media_centre_image_downloadsecant"
#define BUTTON_SETTING			"media_centre_button_setting"
#define IMAGE_SETTING			"media_centre_image_settingsecant"

#define TEXT_TITLE				"win_media_centre_text_title"
#define BOX_WINDOWN				"win_media_centre_box_main"
enum
{
	TYPE_FILE = 0,
	TYPE_VIDEO,
	TYPE_MUSIC,
	TYPE_PICTURE,
	TYPE_TEXT,
	TYPE_TOTAL,
};

int8_t *TextBuffer[] = 
{
	(int8_t*)"File",
	(int8_t*)"Video",
	(int8_t*)"Music",
	(int8_t*)"Picture",
#if MINI_16BIT_WIN8_OSD_SUPPORT
	(int8_t*)"PVR",
#else
	(int8_t*)"Text",
#endif
};
void app_create_media_centre_menu(void)
{

	HotplugPartitionList* partition_list =NULL;

	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

	if((NULL != partition_list) && (0 < partition_list->partition_num))
	{
		app_hide_prompt();
		
		app_stop_all_monitor_filter();
		app_play_stop();

		GUI_SetInterface("flush", NULL);
		
		app_win_set_focus_video_window(MEDIA_CENTER_MENU_WIN);
		GUI_CreateDialog("win_media_centre");
	}
	else
	{
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,STR_ID_INSERT_USB,POPMSG_TYPE_OK);
		return;
	}
	return ;
}

#if 0
static int32_t FocusType = TYPE_FILE;
static void _show_title(int32_t Key)
{
	if(APPK_RIGHT == Key)
	{
		if(FocusType == (TYPE_TOTAL - 1))
			return;
		else
		{
			FocusType++;
		}
	}
	else if(APPK_LEFT == Key)
	{
		if(FocusType == TYPE_FILE)
			return;
		else
		{
			FocusType--;
		}
	}
	GUI_SetProperty(TEXT_TITLE,"string",TextBuffer[FocusType]);
	
}
#endif

void app_create_media_centre_fullsreeen(void)
{
	if(GUI_CheckDialog("win_prog_bar")== GXCORE_SUCCESS)
	{
		GUI_EndDialog("win_prog_bar");
		GUI_SetInterface("flush", NULL);
	}
#if 0
     PopDlg pop = {0};


	
    if (g_AppPvrOps.state != PVR_DUMMY)
    {
        memset(&pop, 0, sizeof(PopDlg));
        pop.type = POP_TYPE_NO_BTN;
        pop.format = POP_FORMAT_DLG;
        pop.str = STR_ID_STOP_PVR_FIRST;
        pop.mode = POP_MODE_UNBLOCK;
        pop.timeout_sec = 3;
        popdlg_create(&pop);    
    }
    else
    {
        GUI_SetProperty("txt_full_state", "state", "hide");
        GUI_SetProperty("img_full_state", "state", "hide");
        g_AppFullArb.state.pause = STATE_OFF;
        g_AppFullArb.draw[EVENT_PAUSE](&g_AppFullArb);
		app_play_stop();
//        g_AppPlayOps.program_stop();
        GUI_CreateDialog("win_media_centre");
        s_media_on_fullscreen = true;
    }

#endif

    // TODO: 
    GUI_CreateDialog("win_media_centre");

}

void app_media_centre_quit(void)
{
#if 0
	GUI_EndDialog("win_media_centre");
	GUI_SetProperty("win_media_centre","draw_now",NULL);

	if(s_media_on_fullscreen == true)
	{
//	    g_AppFullArb.draw[EVENT_MUTE](&g_AppFullArb);
//	    g_AppFullArb.draw[EVENT_TV_RADIO](&g_AppFullArb);
/*	    if (g_AppPlayOps.normal_play.play_total != 0)
	    {
	        g_AppPlayOps.program_play(PLAY_MODE_POINT, g_AppPlayOps.normal_play.play_count);
	        //GUI_CreateDialog("wnd_channel_info");
	    }*/
	}
	s_media_on_fullscreen = false;
#endif

	GUI_EndDialog("win_media_centre");
	GUI_SetProperty("win_media_centre","draw_now",NULL);


}

SIGNAL_HANDLER int media_centre_init(const char* widgetname, void *usrdata)
{
	int32_t init_value;
	init_value = 0;
	app_send_msg(GXMSG_PLAYER_FREEZE_FRAME_SWITCH, &init_value);
//app_epg_disable();
	pmpset_init();
//GxBus_ConfigGetInt("av>mute", &init_value, 0);
	init_value = pmpset_get_int(PMPSET_MUTE);
	if (init_value == 0)  
		{pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_OFF);}
	else 
		{pmpset_set_int(PMPSET_MUTE, PMPSET_MUTE_ON);}

#ifdef PMP_ATTACH_DVB
	GUI_SetProperty(BUTTON_DOWNLOAD, "state", "hide");	
	GUI_SetProperty(IMAGE_DOWNLOAD, "state", "hide");	
	GUI_SetProperty(BUTTON_SETTING, "state", "hide");	
	GUI_SetProperty(IMAGE_SETTING, "state", "hide");	
#endif
	//FocusType = TYPE_FILE;
	GUI_SetProperty(TEXT_TITLE,"string",TextBuffer[0]);

	#if 0/* BEGIN: Deleted by yingc, 2013/12/12 */
	LogControlRecord tmp={0};
	tmp.eControlType=LOG_CONTROL_SAVE_DISK;
	snprintf(tmp.sControlValue,sizeof(tmp.sControlValue),"%s","/media/sda1");
	//SendToUnixSocketStruct("/tmp/unixdomain/log",(char *)(&tmp),sizeof(LogControlRecord));

	COMM_LOG("common",LOG_DEBUG,"%s","");
	COMM_LOG("common",LOG_ERROR,"%s","");
	CommLogSetModule("other",LOG_DEBUG);
	COMM_LOG("gcyin",LOG_DEBUG,"%s","");
	CommLogSetModule("gcyin",LOG_DEBUG);
	COMM_LOG(NULL,LOG_ERROR,"%s","");
	COMM_LOG("gcyin",LOG_ERROR,"%s","");
	COMM_LOG("other",LOG_DEBUG,"%s","");
	#endif/* END:   Deleted by yingc, 2013/12/12   PN: */

	app_set_win_create_flag(MEDIA_CENTER_MENU_WIN);

	return 0;
}

SIGNAL_HANDLER int media_centre_destroy(const char* widgetname, void *usrdata)
{
	int32_t init_value;
	//for dvb
	pmpset_exit();
	/*stop all player av*/
	GxMessage* new_msg_av = NULL;
	new_msg_av = GxBus_MessageNew(GXMSG_PLAYER_STOP);
	APP_CHECK_P(new_msg_av, GXCORE_ERROR);
	
	GxMsgProperty_PlayerStop *stop_av;
	stop_av = GxBus_GetMsgPropertyPtr(new_msg_av, GxMsgProperty_PlayerStop);
	APP_CHECK_P(stop_av, GXCORE_ERROR);
	stop_av->player = PMP_PLAYER_AV;

	GxBus_MessageSendWait(new_msg_av);
	GxBus_MessageFree(new_msg_av);


	/*stop all player pic*/
	GxMessage* new_msg_pic = NULL;
	new_msg_pic = GxBus_MessageNew(GXMSG_PLAYER_STOP);
	APP_CHECK_P(new_msg_pic, GXCORE_ERROR);
	
	GxMsgProperty_PlayerStop *stop_pic;
	stop_pic = GxBus_GetMsgPropertyPtr(new_msg_pic, GxMsgProperty_PlayerStop);
	APP_CHECK_P(stop_pic, GXCORE_ERROR);
	stop_pic->player = PMP_PLAYER_PIC;

	GxBus_MessageSendWait(new_msg_pic);
	GxBus_MessageFree(new_msg_pic);

	if(music_play_state() == TRUE)
	{
		music_status_init();
	}

	GxBus_ConfigGetInt(VIDEO_QUIET_SWITCH, &init_value, VIDEO_QUIET_SWITCH_DV);
	app_send_msg(GXMSG_PLAYER_FREEZE_FRAME_SWITCH, &init_value);


	app_set_win_destroy_flag(MEDIA_CENTER_MENU_WIN);
	app_start_all_monitor_filter();

	//app_epg_enable(g_AppPlayOps.normal_play.ts_src,TS_2_DMX);
	return 0;
}
SIGNAL_HANDLER int media_centre_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_BACK:
		case APPK_MENU:
			{
				GUI_SetProperty("win_media_centre_box_main", "state", "hide");
				GUI_SetProperty("win_media_centre_text_title", "state", "hide");
				GUI_SetProperty("win_media_centre_text_title", "draw_now", NULL);
				GUI_SetProperty("win_media_centre_image_title", "state", "hide");
				GUI_SetProperty("win_media_centre_image_title", "draw_now", NULL);
				app_media_centre_quit();
				/*
				* 退出media，恢复节目播放
				*/
				if(TRUE == app_play_get_msg_pop_type_state(MSG_POP_PROG_LOCK))
				{
					GUI_CreateDialog("win_password_input");
				}
				app_play_set_zoom_para(MAIN_MENU_VIDEO_X, MAIN_MENU_VIDEO_Y, MAIN_MENU_VIDEO_W, MAIN_MENU_VIDEO_H);	
				app_play_reset_play_timer(0);
				app_win_set_focus_video_window(MAIN_MENU_WIN);
				app_play_set_zoom_video_top();
           	}
			break;
		case APPK_UP:
		case APPK_DOWN:
			break;
			//return EVENT_TRANSFER_KEEPON;

		case APPK_RIGHT:
		case APPK_LEFT:
			return EVENT_TRANSFER_KEEPON;
			//event->key.sym = APPK_OK;
			//GUI_SendEvent(GUI_GetFocusWidget(), event);
			//return EVENT_TRANSFER_STOP;
						
		default:
			break;
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_button_video_clicked(const char* widgetname, void *usrdata)
{
	file_view_set_record_group(FILE_VIEW_GROUP_MOVIE);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_button_music_clicked(const char* widgetname, void *usrdata)
{
	file_view_set_record_group(FILE_VIEW_GROUP_MUSIC);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_button_picture_clicked(const char* widgetname, void *usrdata)
{
	file_view_set_record_group(FILE_VIEW_GROUP_PIC);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_button_text_clicked(const char* widgetname, void *usrdata)
{
	file_view_set_record_group(FILE_VIEW_GROUP_TEXT);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_button_file_clicked(const char* widgetname, void *usrdata)
{
	file_view_set_record_group(FILE_VIEW_GROUP_ALL);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_item_change(const char* widgetname, void *usrdata)
{
	int32_t Sel = 0;
	GUI_GetProperty(BOX_WINDOWN,"select",&Sel);
	GUI_SetProperty(TEXT_TITLE,"string",TextBuffer[Sel]);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int media_centre_box_keypress(GuiWidget *widget, void *usrdata)
{
	int ret = EVENT_TRANSFER_STOP;
	GUI_Event *event = NULL;
	uint32_t box_sel;
	
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
				case STBK_UP:
				case STBK_LEFT:
					GUI_GetProperty(BOX_WINDOWN, "select", &box_sel);
					if(TYPE_FILE == box_sel)
					{
						box_sel = TYPE_TEXT;
						GUI_SetProperty(BOX_WINDOWN, "select", &box_sel);
						GUI_SetProperty(TEXT_TITLE,"string",TextBuffer[box_sel]);
						ret = EVENT_TRANSFER_STOP;
					}
					else
					{
						ret = EVENT_TRANSFER_KEEPON;
					}
					break;
					
				case STBK_DOWN:
				case STBK_RIGHT:
					GUI_GetProperty(BOX_WINDOWN, "select", &box_sel);
					if(TYPE_TEXT == box_sel)
					{
						box_sel = TYPE_FILE;
						GUI_SetProperty(BOX_WINDOWN, "select", &box_sel);
						GUI_SetProperty(TEXT_TITLE,"string",TextBuffer[box_sel]);
						ret = EVENT_TRANSFER_STOP;
					}
					else
					{
						ret = EVENT_TRANSFER_KEEPON;
					}
					break;
					
				case STBK_OK:
#if MINI_16BIT_WIN8_OSD_SUPPORT
					GUI_GetProperty(BOX_WINDOWN, "select", &box_sel);
					if(box_sel == TYPE_TEXT)
						app_create_pvr_media_menu();
					ret = EVENT_TRANSFER_KEEPON;
					break;
#endif
				case STBK_MENU:
				case STBK_EXIT:
				//case STBK_UP:
				//case STBK_DOWN:
					ret = EVENT_TRANSFER_KEEPON;
					break;
					
				default:
					break;
			}
		default:
			break;
	}

	return ret;
}



#include "app.h"
#include "app_pop.h"

//resourse
#define IMG_DEV_UNSELECT		"MP_ICON_ARROE_UNSELECT.bmp"
#define IMG_DEV_SELECT			"MP_ICON_ARROE_SELECT.bmp"
#define IMG_DIR					"MP_ICON_FILE.bmp"
#define IMG_MOVIC				"MP_ICON_MOVICE.bmp"
#define IMG_PIC					"MP_ICON_PIC.bmp"
#define IMG_MUSIC				"MP_ICON_MUSIC.bmp"
#define IMG_TEXT				"MP_ICON_TXT.bmp"
#define IMG_UNKNOW				"MP_ICON_UNKNOW.bmp"
#define IMG_USB					"MP_ICON_USB.bmp"
#define IMG_SD					"MP_ICON_SD.bmp"

//widget
#define WIN_MEDIA_CENTRE			"win_media_centre"
#define IMAGE_TITLE				"file_browser_image1"
#define TEXT_TITLE				"file_browser_text_title"
#define TEXT_PATH1				"file_browser_text_path1"
#define TEXT_PATH2				"file_browser_text_path2"
#define IMAGE_USB_SELECT		"file_browser_image_usb"
#define IMAGE_SD_SELECT			"file_browser_image_sd"
#define IMAGE_ROOT_SELECT		"file_browser_image_root"
#define BUTTON_USB				"file_browser_button_usb"
#define BUTTON_SD				"file_browser_button_sd"
#define BUTTON_ROOT				"file_browser_button_root"
#define LISTVIEW_FILE_BROWSER	"file_browser_listview"

//add in 20120219
#define WIN_FILE_BROESER		"win_file_browser"
#define WIN_FILE_IMG_PATH		"win_file_browser_image_path"
#define BACKGROUND_IMAG				"win_file_browser_image_background"
#define	USB_SETTING_IMAGE		"win_file_browser_image_set"
#define USB_SETTING_TEXT		"win_file_browser_text_setting"

#define USB_DEVICE_SUPPORT			(1)
#define SD_DEVICE_SUPPORT			(0)
#define NET_DEVICE_SUPPORT			(0)


#define CHECK_FOR_NOTICE	((USB_DEVICE_SUPPORT + SD_DEVICE_SUPPORT + NET_DEVICE_SUPPORT)>1 ? 1:0)

typedef enum
{
	LISTVIEW_MODE_PARTITION,
	LISTVIEW_MODE_FILE
}listview_mode;

typedef enum
{
	//DEVICE_ROOT,
	DEVICE_USB,
	DEVICE_SD,
	DEVICE_NET,
	DEVICE_COUNT,
}DeviceType_t;

static char s_mcenter_dev_name[32] = {0};
static DeviceType_t DeviceType = DEVICE_USB;
//static char* opened_button = BUTTON_USB;
explorer_para* explorer_view = NULL;
HotplugPartitionList* partition_list = NULL;

int gi_BkPartionListSel = 0;

extern bool music_play_state(void);
extern void music_status_init(void);
extern int text_file_get_open_size(void);
extern  status_t HotplugPartitionDestroy(struct gxfs_hot_device* device);
static int DEVICE_IS_CHOSE(DeviceType_t Type)
{
	if(DeviceType == Type)
		return 1;

	return 0;
}

static void _deal_with_key(uint32_t Key)
{
	//DeviceType_t TypeTemp = DEVICE_USB;
	if(APPK_LEFT == Key)
	{
		if(DeviceType == DEVICE_USB)
			DeviceType = DEVICE_COUNT - 1;
		else
			DeviceType--;
	}
	else if(APPK_RIGHT == Key)
	{
		if(DeviceType == (DEVICE_COUNT - 1))
			DeviceType = DEVICE_USB;
		else
			DeviceType++;
	}
	else
		return;
}
	

#ifndef PMP_ATTACH_DVB_T // for sunxf revert menu
uint8_t app_get_menu_adjust(void)
{
	return 0;
}
#else
extern uint8_t app_get_menu_adjust(void);
#endif

static int set_path1(char* path)
{
	if(NULL == path) 
		GUI_SetProperty(TEXT_PATH1, "string", " ");
	else
		GUI_SetProperty(TEXT_PATH1, "string", path);
	return 0;
}

static int set_path2(char* path)
{
	if(NULL == path) 
	{
		GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
		GUI_SetProperty(TEXT_PATH2, "string", " ");
	}

	/*path2: view local, does not cut head */
	else if(NULL == partition_list)
	{
		GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
		GUI_SetProperty(TEXT_PATH2, "string", path);
	}

	/*path2: view usb or mmc, cut head, eg /mnt/usb0_1 */
	else
	{
		char* path_cut = NULL;
		int path_depth_cut = 1;
		int i = 0;
		for(i = 1; i < strlen(path); i++)
		{
			if('/' == path[i])
				path_depth_cut++;

			if(3 == path_depth_cut)
			{
				if(i+1 < strlen(path))
					path_cut = path + i + 1;
				else
					path_cut = NULL;
				break;
			}
		}
		if(NULL == path_cut)
			path_cut = " ";
		GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
		GUI_SetProperty(TEXT_PATH2, "string", path_cut);
	}

	return 0;
}

listview_mode view_mode_value = LISTVIEW_MODE_PARTITION;
listview_mode view_mode_value_record = LISTVIEW_MODE_PARTITION;
static int check_mode_change(void)
{
	return (view_mode_value_record != view_mode_value)? 1:0;
}

static int fs_setting_control(void)
{
	if(check_mode_change())
	{
#ifdef FILE_EDIT_VALID
#ifdef MEDIA_FILE_EDIT_UNVALID
	return TRUE;
#endif
		switch(view_mode_value)
		{
			case LISTVIEW_MODE_PARTITION:
				GUI_SetProperty(USB_SETTING_IMAGE,"state","hide");//osd_trans_hide
				GUI_SetProperty(USB_SETTING_TEXT,"state","hide");
				break;
			case LISTVIEW_MODE_FILE:
				GUI_SetProperty(USB_SETTING_IMAGE,"state","show");
				GUI_SetProperty(USB_SETTING_TEXT,"state","show");
				break;
			default:
				break;
		}
#endif			
		return TRUE;
	}

	return FALSE;
}
static int listview_mode_set(listview_mode mode)
{
	view_mode_value_record = view_mode_value;
	view_mode_value = mode;
	fs_setting_control();
	return 0;
}

listview_mode listview_mode_get(void)
{
	return view_mode_value;
}


static int device_check(DeviceType_t Type)
{
	HotplugPartitionList* list = NULL;
	
	DeviceType_t TempType = DeviceType;
	DeviceType = Type;
	/*
	//if(WIDGET_IS_FOCUS(BUTTON_ROOT)) 
	if(DEVICE_IS_CHOSE(DEVICE_ROOT))
		return 0;
	*/
	//if(WIDGET_IS_FOCUS(BUTTON_USB))
	if(DEVICE_IS_CHOSE(DEVICE_USB))
		list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
	//else if(WIDGET_IS_FOCUS(BUTTON_SD))
	else if(DEVICE_IS_CHOSE(DEVICE_SD))
		list = GxHotplugPartitionGet(HOTPLUG_TYPE_MMC);
	else if(DEVICE_IS_CHOSE(DEVICE_NET))
	{
		DeviceType = TempType;
		return 1;//no device
	}
	else
	{
		DeviceType = TempType;
		return 1;
	}
	DeviceType = TempType;
	if(NULL == list) 
		return 1;
	
	if(0 < list->partition_num) 
		return 0;
	else
		return 1;
}

static int device_open(void)
{
	uint32_t index = 0;
	//if(WIDGET_IS_FOCUS(BUTTON_USB))
	if(DEVICE_IS_CHOSE(DEVICE_USB))
	{
		//GUI_SetProperty(IMAGE_USB_SELECT, "img", IMG_DEV_SELECT);
		//GUI_SetProperty(IMAGE_SD_SELECT,  "img", IMG_DEV_UNSELECT);
		//GUI_SetProperty(IMAGE_ROOT_SELECT, "img", IMG_DEV_UNSELECT);

		

		//opened_button = BUTTON_USB;
		DeviceType = DEVICE_USB;
		set_path1("USB");
		set_path2(NULL);		

		listview_mode_set(LISTVIEW_MODE_PARTITION);
		partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

	}
	//else if(WIDGET_IS_FOCUS(BUTTON_SD))
	else if(DEVICE_IS_CHOSE(DEVICE_SD))
	{
	/*
		GUI_SetProperty(IMAGE_USB_SELECT, "img", IMG_DEV_UNSELECT);
		GUI_SetProperty(IMAGE_SD_SELECT,  "img", IMG_DEV_SELECT);
		GUI_SetProperty(IMAGE_ROOT_SELECT, "img", IMG_DEV_UNSELECT);
	*/
		
		//opened_button = BUTTON_SD;
		DeviceType = DEVICE_SD;
		set_path1("SD");
		set_path2(NULL);		
		
		listview_mode_set(LISTVIEW_MODE_PARTITION);
		partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_MMC);
	}
/*
	//else if(WIDGET_IS_FOCUS(BUTTON_ROOT))
	else if(DEVICE_IS_CHOSE(DEVICE_ROOT))
	{
		GUI_SetProperty(IMAGE_USB_SELECT, "img", IMG_DEV_UNSELECT);
		GUI_SetProperty(IMAGE_SD_SELECT,  "img", IMG_DEV_UNSELECT);
		GUI_SetProperty(IMAGE_ROOT_SELECT, "img", IMG_DEV_SELECT);

		//opened_button = BUTTON_ROOT;
		DeviceType = DEVICE_ROOT;
		char* group_suffixs = NULL;
		explorer_closedir(explorer_view);
		group_suffixs = file_view_get_group_suffixs(file_view_get_record_group());
		explorer_view = explorer_opendir("/", group_suffixs);
		GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);

		set_path1("LOCAL");
		set_path2("/");		

		listview_mode_set(LISTVIEW_MODE_FILE);
		partition_list = NULL;
	}
*/
	else if(DEVICE_IS_CHOSE(DEVICE_NET))
	{
		DeviceType = DEVICE_NET;
		set_path1("NET");
		set_path2(NULL);		
		
		listview_mode_set(LISTVIEW_MODE_PARTITION);
		partition_list = NULL;
	}
	else
		;

	
	GUI_SetFocusWidget(LISTVIEW_FILE_BROWSER);
	GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
	GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", &index);

	return 0;
}

/*
static char* find_next_music(void)
{
	GxDirent* ent = NULL;
	int32_t i = 0;
	int32_t file_count = 0;
	int32_t play_no = 0;
	file_view_group group = FILE_VIEW_GROUP_ALL;

	if(NULL == explorer_view) return NULL;

	GUI_GetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&play_no);
	if(play_no >= explorer_view->nents)
	{
		play_no = 0;
	}

	file_count = explorer_view->nents;
	
	//cur->end
	for(i = play_no + 1; i < file_count; i++)
	{
		ent = explorer_view->ents + i;
		if(NULL == ent) return NULL;

		group = file_view_get_file_group(ent->fname);
		if((GX_FILE_REGULAR == ent->ftype)&&(FILE_VIEW_GROUP_MUSIC == group))
		{
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&i);
			return ent->fname;
		}
	}
	
	//start->=cur
	for(i = 0; i <= play_no; i++)
	{
		ent = explorer_view->ents + i;
		if(NULL == ent) return NULL;

		group = file_view_get_file_group(ent->fname);
		if((GX_FILE_REGULAR == ent->ftype)&&(FILE_VIEW_GROUP_MUSIC == group))
		{
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&i);
			return ent->fname;
		}
	}
	
	return NULL;
}*/

static int service_status_report(GxMsgProperty_PlayerStatusReport* player_status)
{
	//char* file_name = NULL;
	
	APP_CHECK_P(player_status, 1);
	APP_CHECK_P(explorer_view, 1);

	/*play in file_view list, only music support*/
	file_view_group group = FILE_VIEW_GROUP_ALL;
	group = file_view_get_record_group();
	if((FILE_VIEW_GROUP_ALL != group)
		&& (FILE_VIEW_GROUP_MUSIC != group)
		&& (0 == strcmp(player_status->player, PMP_PLAYER_AV)))
	{
		play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
		music_status_init();
		return EVENT_TRANSFER_STOP;
	}	
	
	switch(player_status->status)
	{
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
			
			
			/*pmpset_music_play_sequence sequence = 0;
			sequence = pmpset_get_int(PMPSET_MUSIC_PLAY_SEQUENCE);
			if(PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE == sequence)
			{
				APP_Printf("play only once\n");
				
				play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
				break;
			}
			else if(PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE == sequence)
			{
				APP_Printf("play repeat one\n");
				
				int play_no = 0;
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&play_no);
				if(play_no >= explorer_view->nents) play_no = 0;
				file_name = explorer_view->ents[play_no].fname;
				APP_CHECK_P(file_name, 1);
				
				play_av_by_path_file(explorer_view->path, file_name, 0);
				break;
			}
			else
			{
				APP_Printf("play next\n");
				file_name = find_next_music();
				if(NULL == file_name) return EVENT_TRANSFER_STOP;

				play_av_by_path_file(explorer_view->path, file_name, 0);
				break;
			}*/		
			break;

		case PLAYER_STATUS_PLAY_RUNNING:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_RUNNING\n");
			break;

		case PLAYER_STATUS_ERROR:
			if((0 == strcmp(player_status->player, PMP_PLAYER_AV))
				&& (music_play_state() == true))
			{	
				APP_Printf("[SERVICE] PLAYER_STATUS_ERROR, %d\n", player_status->error);
				play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
			}
			break;

		default:
			break;
	}

	return 0;
}

SIGNAL_HANDLER  int file_browser_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;

	APP_CHECK_P(usrdata, EVENT_TRANSFER_STOP);
	
	event = (GUI_Event*)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			service_status_report(player_status);
			break;
			
		/*case GXMSG_HOTPLUG_IN:
			if(0 == device_check(DEVICE_USB))
			{
				if(device_check(DeviceType))
				{
					DeviceType = DEVICE_USB;
					//device_open();
				}
				else if(DEVICE_USB != DeviceType)
					break;
					
			}
			else if(0 == device_check(DEVICE_SD))
			{
				if(device_check(DeviceType))
				{
					DeviceType = DEVICE_SD;
					//device_open();
				}
				else if(DEVICE_SD != DeviceType)
					break;
			}
			else if(0 == device_check(DEVICE_NET))
			{
				if(device_check(DeviceType))
				{
					DeviceType = DEVICE_NET;
					//device_open();
				}
				else if(DEVICE_NET != DeviceType)
					break;
			}
			//GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
			device_open();
			break;*/
			
		default:
			break;
	}
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int file_browser_init(const char* widgetname, void *usrdata)
{
	file_view_group group = 0;

	group = file_view_get_record_group();
	// app_lang_set_menu_font_type(TEXT_TITLE);
	if(FILE_VIEW_GROUP_ALL == group)
	{
		GUI_SetProperty(TEXT_TITLE, "string", STR_ID_FILE);
	}
	else if(FILE_VIEW_GROUP_MOVIE == group)
	{
		GUI_SetProperty(TEXT_TITLE, "string", STR_ID_VIDEO);
	}
	else if(FILE_VIEW_GROUP_MUSIC == group)
	{
		GUI_SetProperty(TEXT_TITLE, "string", STR_ID_MUSIC);
	}
	else if(FILE_VIEW_GROUP_PIC == group)
	{
		GUI_SetProperty(TEXT_TITLE, "string", STR_ID_PICTURE);
	}
#if (MINI_256_COLORS_OSD_SUPPORT || MINI_16_BITS_OSD_SUPPORT)
#else
	else if(FILE_VIEW_GROUP_TEXT == group)
	{
		GUI_SetProperty(TEXT_TITLE, "string", STR_ID_TEXT);
	}
#endif

	explorer_view = NULL;
	partition_list = NULL;
	device_open();
	GUI_SetProperty(BACKGROUND_IMAG, "state", "hide");
	
	return 0;
}

SIGNAL_HANDLER int file_browser_destroy(const char* widgetname, void *usrdata)
{
	explorer_closedir(explorer_view);
	explorer_view = NULL;
	partition_list = NULL;
#if 0
	/*bean: shortcut to file_view, when win_media_centre not create*/
	char* focus_win = NULL;
	focus_win = (char*)GUI_GetFocusWindow();
	if(NULL == focus_win) return GXCORE_ERROR;
	if(strcasecmp(focus_win, WIN_MEDIA_CENTRE))
	{
		GUI_CreateDialog(WIN_MEDIA_CENTRE);
	}
#endif	

#if (MINI_256_COLORS_OSD_SUPPORT ||MINI_16_BITS_OSD_SUPPORT)
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

	int init_value = 0;
	GxBus_ConfigGetInt(VIDEO_QUIET_SWITCH, &init_value, VIDEO_QUIET_SWITCH_DV);
	app_send_msg(GXMSG_PLAYER_FREEZE_FRAME_SWITCH, &init_value);
#endif
	return 0;
}

SIGNAL_HANDLER int file_browser_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;

	if(app_get_menu_adjust() == 0)
	{
		switch(event->key.sym)
		{
			case APPK_BACK:
			case APPK_MENU:
				event->key.sym = APPK_BACK;
				/*
				{
					int32_t init_value;
					GUI_SetInterface("logic_clut", "MP_ICON_NET2.bmp");
					GxBus_ConfigGetInt("osd>trans_level", &init_value, 255);
					GUI_SetInterface("osd_alpha_global", &init_value);
				}
				*/
				return EVENT_TRANSFER_KEEPON;
				//return EVENT_TRANSFER_STOP;
			case APPK_UP:
				/*
				if(WIDGET_IS_FOCUS(BUTTON_USB))
				{
					//GUI_SetFocusWidget(BUTTON_ROOT);	// TODO: later
					return EVENT_TRANSFER_STOP;
				}
				*/
				break;
				
			case APPK_DOWN:
				/*
				if(WIDGET_IS_FOCUS(BUTTON_USB))
				//if(WIDGET_IS_FOCUS(BUTTON_ROOT))
				{
					//GUI_SetFocusWidget(BUTTON_USB);    // TODO: later
					return EVENT_TRANSFER_STOP;
				}
				*/
				break;
			case APPK_LEFT:	
			case APPK_RIGHT:
				_deal_with_key(event->key.sym);
			case APPK_OK:
				if(0 == device_check(DeviceType))
				{
					device_open();
				}
				else
				{
					_deal_with_key(event->key.sym == APPK_LEFT?APPK_RIGHT : APPK_LEFT);
					if (CHECK_FOR_NOTICE)
					{
                        PopDlg pop;
                        memset(&pop, 0, sizeof(PopDlg));
                        pop.type = POP_TYPE_OK;
                        pop.format = POP_FORMAT_DLG;
                        pop.str = STR_ID_INSERT_USB;
                        pop.mode = POP_MODE_UNBLOCK;
						pop.pos.x = POP_MEDIA_X_START;
						pop.pos.y = POP_MEDIA_Y_START;	
                        popdlg_create(&pop);	
//						media_popmsg(pop.pos.x, pop.pos.y,STR_ID_INSERT_USB, MEDIA_POPMSG_TYPE_OK);				
					}
					
					
				}
				return EVENT_TRANSFER_STOP;
				
			case STBK_PAGE_UP:
			case STBK_PAGE_DOWN:
				{
					return  EVENT_TRANSFER_STOP;
				}
				break;
/*			case VK_BOOK_TRIGGER:
					GUI_EndDialog("win_file_browser");
					GUI_SendEvent("wnd_main_menu_item", event);
					return  EVENT_TRANSFER_STOP;
					break; */
			default:
				break;
		}	
	}
	else //for sunxf revet menu
	{
		switch(event->key.sym)
		{			
			case APPK_RIGHT:
				event->key.sym = APPK_BACK;
				return EVENT_TRANSFER_KEEPON;
				
			case APPK_UP:
				/*
				if(WIDGET_IS_FOCUS(BUTTON_USB))
				{
					//GUI_SetFocusWidget(BUTTON_ROOT);	// TODO: later
					return EVENT_TRANSFER_STOP;
				}
				*/
				break;
				
			case APPK_DOWN:
				/*
				if(WIDGET_IS_FOCUS(BUTTON_USB))
				//if(WIDGET_IS_FOCUS(BUTTON_ROOT))
				{
					//GUI_SetFocusWidget(BUTTON_USB);    // TODO: later
					return EVENT_TRANSFER_STOP;
				}
				*/
				break;
				

			case APPK_LEFT:
			case APPK_OK:
				if(0 == device_check(DeviceType))
				{
					device_open();
				}
				else
				{
                    PopDlg pop;
                    memset(&pop, 0, sizeof(PopDlg));
                    pop.type = POP_TYPE_OK;
                    pop.format = POP_FORMAT_DLG;
                    pop.str = STR_ID_INSERT_USB;
                    pop.mode = POP_MODE_UNBLOCK;
					pop.pos.x = POP_MEDIA_X_START;
					pop.pos.y = POP_MEDIA_Y_START;
                    popdlg_create(&pop);
//					media_popmsg(pop.pos.x, pop.pos.y , STR_ID_INSERT_USB, MEDIA_POPMSG_TYPE_OK);
				}
				return EVENT_TRANSFER_STOP;
				
				
			default:
				break;
		}
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int file_browser_listview_get_count(const char* widgetname, void *usrdata)
{
	listview_mode mode = LISTVIEW_MODE_PARTITION;

	mode = listview_mode_get();
	if(LISTVIEW_MODE_PARTITION == mode)
	{
		if(NULL == partition_list)
			return 0;

		if(0 == partition_list->partition_num)
			return 1;
		else
			return partition_list->partition_num;
	}
	else if(LISTVIEW_MODE_FILE == mode)
	{
		if(NULL == explorer_view)
			return 0;
		return explorer_view->nents+1;
	}

	return 0;
}

static int item_not_exist(ListItemPara* item)
{
	if(app_get_menu_adjust() == 0)
	{
		/*col-0: img*/
		item->x_offset = 0;
		item->string = NULL;
		item->image = NULL;


		/*col-1: partition name*/
		item = item->next;
		if(NULL == item) return GXCORE_ERROR;

		item->x_offset = 0;
		item->image = NULL;

		if(LISTVIEW_MODE_FILE == listview_mode_get())
			item->string = STR_ID_FILE_NOT_EXIST;
		else
			item->string = STR_ID_INSERT_USB;
	}
	else
	{
		item->x_offset = 0;
		item->image = NULL;
		item->string = STR_ID_FILE_NOT_EXIST;
		item = item->next;
		if(NULL == item) return GXCORE_ERROR;

		item->x_offset = 0;
		item->string = NULL;
		item->image = NULL;
	}

	return 0;
}

SIGNAL_HANDLER int file_browser_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	uint32_t index = 0;
	listview_mode mode = LISTVIEW_MODE_PARTITION;
	char* img_pic = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) return GXCORE_ERROR;
	index = (uint32_t)item->sel;

	mode = listview_mode_get();

	/*listview view mode: PARTITION*/
	if(LISTVIEW_MODE_PARTITION == mode)
	{
		if(NULL == partition_list)
			return GXCORE_ERROR;

		if(0 == partition_list->partition_num) 
		{
			item_not_exist(item);
			return GXCORE_ERROR;
		}
	
		if(index >= partition_list->partition_num) 
			return GXCORE_ERROR;

		HotplugPartition* partition = NULL;
		partition = &(partition_list->partition[index]);
		if(NULL == partition)
			return GXCORE_ERROR;
	
		if(app_get_menu_adjust() == 0)
		{
			/*col-0: img*/
			item->x_offset = 0;
			item->string = NULL;
			if(HOTPLUG_TYPE_USB == partition_list->type)
				item->image = IMG_USB;
			else if(HOTPLUG_TYPE_MMC == partition_list->type)
				item->image = IMG_SD;


			/*col-1: partition name*/
			item = item->next;
			if(NULL == item) return GXCORE_ERROR;

			item->x_offset = 0;
			item->image = NULL;	
			item->string = partition->partition_name;
		}
		else
		{
			/*col-0: img*/
			item->x_offset = 0;
			item->image = NULL;	
			item->string = partition->partition_name;


			/*col-1: partition name*/
			item = item->next;
			if(NULL == item) return GXCORE_ERROR;

			item->x_offset = 0;
			item->string = NULL;
			if(HOTPLUG_TYPE_USB == partition_list->type)
				item->image = IMG_USB;
			else if(HOTPLUG_TYPE_MMC == partition_list->type)
				item->image = IMG_SD;
		}
		
	}
	
	/*listview view mode: FILE*/
	else if(LISTVIEW_MODE_FILE == mode)
	{

		if(NULL == explorer_view)
			return GXCORE_ERROR;

		if(index == 0) //last dir
		{
			/*col-0: img*/
			item->x_offset = 0;
			item->string = NULL;
			item->image = NULL;
			
			/*col-1: file name*/
			item = item->next;
			if(NULL == item) return GXCORE_ERROR;
			item->x_offset = 0;
			item->image = NULL;	
			item->string = "/..";
			
			return GXCORE_SUCCESS;
		}
		
		
		if(index > explorer_view->nents)
			return GXCORE_ERROR;

		GxDirent* ent = NULL;
		ent = explorer_view->ents + (index - 1);
		if(NULL == ent)
			return GXCORE_ERROR;
		
		if(GX_FILE_DIRECTORY == ent->ftype)
		{
			img_pic = IMG_DIR;
		}
		else if(GX_FILE_REGULAR == ent->ftype)
		{	
			file_view_group group = FILE_VIEW_GROUP_ALL;
			group = file_view_get_file_group(ent->fname);
			
			if(FILE_VIEW_GROUP_PIC == group)
			{
				img_pic = IMG_PIC;
			}
			else if(FILE_VIEW_GROUP_MUSIC == group)
			{
				img_pic = IMG_MUSIC;
			}
			else if(FILE_VIEW_GROUP_MOVIE == group)
			{
				img_pic = IMG_MOVIC;
			}
		#if (MINI_256_COLORS_OSD_SUPPORT || MINI_16_BITS_OSD_SUPPORT)
		#else
			else if(FILE_VIEW_GROUP_TEXT== group)
			{
				img_pic = IMG_TEXT;
			}
		#endif
			else
			{
				img_pic = IMG_UNKNOW;
			}	
		}

		
		if(app_get_menu_adjust() == 0)
		{
			/*col-0: img*/
			item->x_offset = 0;
			item->string = NULL;
			item->image = img_pic;
			
			/*col-1: file name*/
			item = item->next;
			if(NULL == item) return GXCORE_ERROR;
			item->x_offset = 0;
			item->image = NULL;	
			item->string = ent->fname;
		}
		else
		{
			/*col-0: file name*/
			item->x_offset = 0;
			item->image = NULL;	
			item->string = ent->fname;

				
			/*col-1: img*/
			item = item->next;
			if(NULL == item) return GXCORE_ERROR;
			item->x_offset = 0;
			item->string = NULL;
			item->image = img_pic;
		}
	}
	
	return GXCORE_SUCCESS;
}



SIGNAL_HANDLER int file_browser_listview_change(const char* widgetname, void *usrdata)
{
	return GXCORE_SUCCESS;	
}

static int file_browser_listview_backkey(void)
{
	listview_mode mode = LISTVIEW_MODE_PARTITION;
	int listview_sel = 0;
	
	mode = listview_mode_get();

	/*from partition to button*/
	if(LISTVIEW_MODE_PARTITION == mode)
	{
		//GUI_SetFocusWidget(opened_button);
		return 2;
		GUI_EndDialog(WIN_FILE_BROESER);
		// TODO:
	}
	else if(LISTVIEW_MODE_FILE == mode)
	{
		if(explorer_view == NULL)
			return 1;
		if(0 == explorer_view->path_depth)
		{
			/*local not have partition, direct from file to button*/
			if(NULL == partition_list)
			{
				//GUI_SetFocusWidget(opened_button);
				return 2;
				GUI_EndDialog(WIN_FILE_BROESER);
				// TODO:
			}
			/*from file to partition*/
			else
			{
				listview_mode_set(LISTVIEW_MODE_PARTITION);
				
				GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&gi_BkPartionListSel);
				GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);

				if(HOTPLUG_TYPE_USB == partition_list->type)
					set_path1("USB");
				else if(HOTPLUG_TYPE_MMC == partition_list->type)
					set_path1("SD");
				
				set_path2(NULL);	
			}
		}
		else
		{
			GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
			GUI_SetProperty(TEXT_PATH2, "string", STR_ID_WAITING);
			GUI_SetProperty(TEXT_PATH2, "draw_now", NULL);
		
			explorer_view = explorer_backdir(explorer_view);

			listview_sel = explorer_view->chdir_sel[explorer_view->path_depth];
			//GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&listview_sel);
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&listview_sel);
			set_path2(explorer_view->path);
		}	
	}
	
	return 0;
	
}


static int file_browser_listview_okkey(void)
{
	uint32_t index = 0;
	//#if ((0 == MINI_256_COLORS_OSD_SUPPORT))
	//char* play_path = NULL;
	//#endif
	int listview_sel = 0;
	int file_num = 0;
	listview_mode mode = LISTVIEW_MODE_PARTITION;

	GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &index);

	mode = listview_mode_get();

	/*listview view mode: PARTITION*/
	if(LISTVIEW_MODE_PARTITION == mode)
	{
		//listview_mode_set(LISTVIEW_MODE_FILE);
		

		if(NULL == partition_list) 
			return GXCORE_ERROR;
		if(index >= partition_list->partition_num) 
			return GXCORE_ERROR;


		HotplugPartition* partition = NULL;
		partition = &(partition_list->partition[index]);
		if(NULL == partition) 
			return GXCORE_ERROR;

		{
			char *p = NULL;

			memset(s_mcenter_dev_name, 0, sizeof(s_mcenter_dev_name));
			strcpy(s_mcenter_dev_name, partition_list->partition[index].dev_name);
#if 0
			uint32_t dev_len;
			dev_len = strlen(s_mcenter_dev_name);
			p = s_mcenter_dev_name + dev_len - 1;
			if(*p >= '0' && *p <= '9') //remove partition num
				*p = '\0';
#endif
#ifdef ECOS_OS
			p = strrchr(s_mcenter_dev_name, '/');
			*p = '\0';
#else
			uint32_t dev_len;
			dev_len = strlen(s_mcenter_dev_name);
			p = s_mcenter_dev_name + dev_len - 1;
			if(*p >= '0' && *p <= '9') //remove partition num
				*p = '\0';
#endif
	      }

		listview_mode_set(LISTVIEW_MODE_FILE);
		
		GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
		GUI_SetProperty(TEXT_PATH2, "string", STR_ID_WAITING);
		GUI_SetProperty(TEXT_PATH2, "draw_now", NULL);
	
		char* group_suffixs = NULL;
		explorer_closedir(explorer_view);
		group_suffixs = file_view_get_group_suffixs(file_view_get_record_group());
		explorer_view = explorer_opendir(partition->partition_entry, group_suffixs);
		GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
		GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
		if(file_num > 1)
			listview_sel = 1;

		GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&gi_BkPartionListSel);
		
		GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&listview_sel);

		set_path1(partition->partition_name);
		set_path2(NULL);		
	}
	
	/*listview view mode: FILE*/
	else  if(LISTVIEW_MODE_FILE == mode)
	{
		if(NULL == explorer_view) 
			return GXCORE_ERROR;

		if(index == 0)
		{
			file_browser_listview_backkey();
			return GXCORE_SUCCESS;
		}
		
		if(index > explorer_view->nents) 
			return GXCORE_ERROR;
		
		GxDirent* ent = NULL;
		ent = explorer_view->ents + (index - 1);
		if(NULL == ent) 
			return GXCORE_ERROR;

		/*enter dir*/
		if(GX_FILE_DIRECTORY == ent->ftype)
		{
			GUI_SetProperty(TEXT_PATH2, "rolling_stop",NULL);
			GUI_SetProperty(TEXT_PATH2, "string", STR_ID_WAITING);
			GUI_SetProperty(TEXT_PATH2, "draw_now", NULL);
		
			explorer_view = explorer_chdir(explorer_view, index);
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
			if(file_num > 1)
				listview_sel = 1;

			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&listview_sel);
            if(explorer_view)
            {
                set_path2(explorer_view->path);
            }
		}
		
		/*start reference play*/
		else if(GX_FILE_REGULAR == ent->ftype)
		{	
			file_view_group group = FILE_VIEW_GROUP_ALL;
			group = file_view_get_file_group(ent->fname);
			
			if(FILE_VIEW_GROUP_PIC == group)
			{
				play_list_init(PLAY_LIST_TYPE_PIC, explorer_view, index-1);
				GUI_CreateDialog("win_pic_view");
			}
			else if(FILE_VIEW_GROUP_MUSIC == group)
			{
#if 1		
				play_list_init(PLAY_LIST_TYPE_MUSIC, explorer_view, index-1);
				GUI_CreateDialog("win_music_view");
#else		
				int32_t active_no = 0;

				//play in music view window
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&active_no);
				if(index == active_no)
				{	
					/*clear active no*/
					active_no = -1;
					GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&active_no);
					/*enter*/
					play_list_init(PLAY_LIST_TYPE_MUSIC, explorer_view, index);
					GUI_CreateDialog("win_music_view");
				}
				//play in file browser window
				else
				{
					active_no = index;
					GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&active_no);

					play_av_by_path_file(explorer_view->path, ent->fname, 0);
				}
#endif
			}
			else if(FILE_VIEW_GROUP_MOVIE == group)
			{
				play_list_init(PLAY_LIST_TYPE_MOVIE, explorer_view, index-1);
				GUI_CreateDialog("win_movie_view");
			}
		#if (MINI_256_COLORS_OSD_SUPPORT || MINI_16_BITS_OSD_SUPPORT)
		#else
			else if(FILE_VIEW_GROUP_TEXT== group)
			{
				
				play_list_init(PLAY_LIST_TYPE_TEXT, explorer_view, index-1);
				{
				#define MAX_TEXT_FILE_SIZE_BIT    (512*1024)
				int len = 0;
				len = text_file_get_open_size();
				if(len > MAX_TEXT_FILE_SIZE_BIT)
				{
					return GXCORE_SUCCESS;
				}
				}
				GUI_CreateDialog("win_text_view");
				//play_path = explorer_static_path_strcat(explorer_view->path, ent->fname);
				//play_text(play_path);
			}
		#endif
			else
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = STR_ID_FILE_NOT_SUPPORT;
                pop.mode = POP_MODE_UNBLOCK;
                pop.timeout_sec = 3;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, STR_ID_FILE_NOT_SUPPORT, MEDIA_POPMSG_TYPE_OK);
			}
		}
	}
	
	return GXCORE_SUCCESS;


#if 0


	status_t ret = GXCORE_ERROR;
	int32_t file_no = 0;
	char* file_name = NULL;
	char* file_path = NULL;
	file_view_fileinfo info;
	char* play_path = NULL;
	
	GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &file_no);
	ret = file_view_get_item(file_no, &info);
	if(GXCORE_ERROR == ret) return GXCORE_ERROR;

	file_name = info.name;
	file_path = file_view_get_path();

	if(GX_FILE_DIRECTORY == info.info.type)
	{
		GUI_SetProperty(TEXT_PATH2, "string", STR_ID_WAITING);
		GUI_SetProperty(TEXT_PATH2, "draw_now", NULL);

		ret = file_view_chdir(file_name);
		if(GXCORE_SUCCESS == ret)
		{
			file_path = file_view_get_path();
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
		}
		GUI_SetProperty(TEXT_PATH2, "string", file_path);
	}
	else
	{
		if(file_view_get_file_group(file_name, FILE_VIEW_GROUP_PIC))
		{
			play_list_init(FILE_VIEW_GROUP_PIC, file_no);
			GUI_CreateDialog("win_pic_view");
		}
		else if(file_view_get_file_group(file_name, FILE_VIEW_GROUP_MUSIC))
		{
			int32_t active_no = 0;

			//play in music view window
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&active_no);
			if(file_no == active_no)
			{
				play_list_init(FILE_VIEW_GROUP_MUSIC, file_no);
				GUI_CreateDialog("win_music_view");
			}
			//play in file browser window
			else
			{
				active_no = file_no;
				GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", (void*)&active_no);

				play_path = explorer_static_path_strcat(file_path, file_name);
				if(NULL == play_path) return GXCORE_ERROR;
				play_av_by_url(play_path);
			}
		}
		else if(file_view_get_file_group(file_name, FILE_VIEW_GROUP_MOVIE))
		{
			play_list_init(FILE_VIEW_GROUP_MOVIE, file_no);
			GUI_CreateDialog("win_movie_view");
		}
		else if(file_view_get_file_group(file_name, FILE_VIEW_GROUP_TEXT))
		{
			GUI_CreateDialog("win_text_view");
			play_path = explorer_static_path_strcat(file_path, file_name);
			play_text(play_path);
		}
	}
#endif
}

SIGNAL_HANDLER int file_browser_listview_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
	{
			
		case APPK_BACK:
		case APPK_MENU:
			
#ifdef MENCENT_FREEE_SPACE
			 GUI_SetProperty("win_main_menu", "back_ground", (void*)"BG.jpg");
#endif
			if(file_browser_listview_backkey() == 2)
				return EVENT_TRANSFER_KEEPON;
			return EVENT_TRANSFER_STOP;

		/*page down*/
		case APPK_PREVIOUS:
			event->key.sym = GUIK_PAGE_UP;
			return EVENT_TRANSFER_KEEPON;
		/*page up*/
		case APPK_NEXT:
			event->key.sym = GUIK_PAGE_DOWN;
			return EVENT_TRANSFER_KEEPON;

			
		case APPK_LEFT:
			/*
			if(app_get_menu_adjust() == 0)
			{
				GUI_SetFocusWidget(opened_button);
			}
			else
			{
				file_browser_listview_okkey();
			}
			*/
			//return EVENT_TRANSFER_STOP;
			return EVENT_TRANSFER_KEEPON;
			
		case APPK_RIGHT:
			/*
			if(app_get_menu_adjust() == 0)
			{
				file_browser_listview_okkey();
			}
			else
			{
				GUI_SetFocusWidget(opened_button);
			}
			*/
			//return EVENT_TRANSFER_STOP;
			return EVENT_TRANSFER_KEEPON;

		case APPK_OK:
			GxCore_ThreadDelay(60);//20121031 for Mantis Bug:864
			file_browser_listview_okkey();
			return EVENT_TRANSFER_STOP;
			
		case APPK_SET:
		{
#ifdef FILE_EDIT_VALID	
#ifdef MEDIA_FILE_EDIT_UNVALID
			return EVENT_TRANSFER_STOP;
#endif
			int32_t file_no = 0;
			int32_t x = 0;
			int32_t y = 0;
	// for protection
			if(LISTVIEW_MODE_PARTITION == listview_mode_get())
			{
				/*if((NULL == partition_list) 
					||(0 == partition_list->partition_num))*/ 
				{
					return EVENT_TRANSFER_STOP;
				}
			}
			else
			{
				uint32_t index = 0;
				
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &index);
				if(index == 0)
				{
					//return EVENT_TRANSFER_STOP;
				}
			}
			GUI_CreateDialog("win_file_edit");
			x = 480*APP_XRES/720;
			GUI_SetProperty("win_file_edit", "move_window_x", (void*)&x);
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &file_no);
			file_no = file_no % 10;
			if(file_no <= 5)
			{
				y = 130*APP_YRES/576+ file_no*30;
			}
			else 
			{
				y = 130*APP_YRES/576+ file_no * 30 - 132*APP_YRES/576;
			}
			GUI_SetProperty("win_file_edit", "move_window_y", (void*)&y);
#endif			
			return EVENT_TRANSFER_STOP;
		}	
			case STBK_PAGE_UP:
			{
				uint32_t sel;
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &sel);
				if (sel == 0)
				{
					sel = file_browser_listview_get_count(NULL,NULL)-1;
					GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", &sel);
					return  EVENT_TRANSFER_STOP; 
				}
				else
				{
					return  EVENT_TRANSFER_KEEPON; 
				}
			}
			break;
			case STBK_PAGE_DOWN:
			{
				uint32_t sel;
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &sel);
				if (file_browser_listview_get_count(NULL,NULL)
				== sel+1)
				{
					sel = 0;
					GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", &sel);
					return  EVENT_TRANSFER_STOP; 
				}
				else
				{
					return  EVENT_TRANSFER_KEEPON; 
				}
			}
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int file_browser_lost_focus(GuiWidget *widget, void *usrdata)
{
	int listview_sel = 0;
	
	GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", (void*)&listview_sel);
	GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", &listview_sel);
	GUI_SetProperty(TEXT_PATH2, "rolling_stop", NULL);
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int file_browser_got_focus(GuiWidget *widget, void *usrdata)
{
	int listview_sel = -1;
	GUI_SetProperty(LISTVIEW_FILE_BROWSER, "active", &listview_sel);
	GUI_SetProperty(TEXT_PATH2, "reset_rolling", NULL);

// add in 20121123, for audio background display
	GUI_SetInterface("flush",NULL);
	GUI_SetInterface("video_disable", NULL);
	return EVENT_TRANSFER_STOP;
}

char *app_mcenter_dev_name(void)
{
	return s_mcenter_dev_name;
}

status_t app_mcentre_hotplug_out(char *hotplug_dev)
{

	struct gxfs_hot_device* dev;
    if(listview_mode_get() == LISTVIEW_MODE_PARTITION)
    {
        file_browser_init(NULL, NULL);
		dev = GxCore_HotplugGetFirst();
		HotplugPartitionDestroy(dev);
        GUI_SetProperty("win_file_browser", "update", NULL);
    }

    if((hotplug_dev != NULL)
            && (strcmp(hotplug_dev, s_mcenter_dev_name) == 0))
    {
        GxMessage* new_msg_av = NULL;
        new_msg_av = GxBus_MessageNew(GXMSG_PLAYER_STOP);
        APP_CHECK_P(new_msg_av, GXCORE_ERROR);

        GxMsgProperty_PlayerStop *stop_av;
        stop_av = GxBus_GetMsgPropertyPtr(new_msg_av, GxMsgProperty_PlayerStop);
        APP_CHECK_P(stop_av, GXCORE_ERROR);
        stop_av->player = PMP_PLAYER_AV;

        GxBus_MessageSendWait(new_msg_av);
        GxBus_MessageFree(new_msg_av);

        if(listview_mode_get() == LISTVIEW_MODE_FILE)
        {
            GUI_EndDialog("after win_file_browser");
            file_browser_init(NULL, NULL);
			dev = GxCore_HotplugGetFirst();
			HotplugPartitionDestroy(dev);
            GUI_SetProperty("win_file_browser", "update", NULL);
            // clean music play tag.
            clean_music_file_no_bak();
            music_status_init();
        }
    }

    return GXCORE_SUCCESS;
}

status_t app_mcentre_hotplug_in(void)
{
	if(listview_mode_get() == LISTVIEW_MODE_PARTITION)
	{
		file_browser_init(NULL, NULL);
	}

	return GXCORE_SUCCESS;
}



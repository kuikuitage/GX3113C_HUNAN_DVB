#include "app.h"
#include "app_pvr.h"

#define WND_PVR_MEIDA "wnd_pvr_media_list"
#define LISTVIEW_PVR_MEIDA      "listview_pvr_list"
#define TXT_PVR_MEDIA_INFO_1 "text_pvr_midia_info_1"
#define TXT_PVR_MEDIA_INFO_2 "text_pvr_midia_info_2"
#define TXT_PVR_MEDIA_INFO_3 "text_pvr_midia_info_3"

#define PVR_DIR_STR "/GxPvr" 
#define PVR_INDEX_SUFFIX "dvr"
#define PVR_MEDIE_SUFFIX "ts"
#define PVR_FIX_SIFFIX_LEN  (7) //.ts.dvr

#define PVR_PATH_LEN    (64)


typedef struct
{
    uint32_t size;
    time_t time;
}PvrFileInfo;

typedef struct
{
	char			path[500];
	int			nents;
	GxDirent*	ents;
}DirPara;

static struct
{
    int total;
    int *record;
}s_pvr_media_map = {0};

static struct
{
    HotplugPartitionList *partition_list;
    int pvr_partition_num;
}s_pvr_partition = {0};

static DirPara s_pvr_dir;
static bool s_pvr_on_fullscreen = false;
int gi_PvrFileSel = -1;
//extern AppPvrOps g_AppPvrOps;
extern 	void app_free_dir_ent(GxDirent **ents, int *nents);

/////GxCore_Free result extern/////
static char* _get_dvr_dir_path(int index)
{
    char *dir_path = NULL;
    int dir_len;
    int file_len;

    if((s_pvr_media_map.total == 0) || (s_pvr_media_map.record == NULL))
    {
        return NULL;
    }
    
    file_len = strlen(s_pvr_dir.ents[s_pvr_media_map.record[index]].fname);
    dir_len = file_len - PVR_FIX_SIFFIX_LEN;//xxx.ts.dvr
    dir_len += 1; // '/'
    dir_len += strlen(s_pvr_dir.path);
    dir_len += 1; // '\0'

    dir_path = (char*)GxCore_Malloc(dir_len * sizeof(char));
    if(dir_path != NULL)
    {
        memset(dir_path, 0, dir_len);
        strcpy(dir_path, s_pvr_dir.path);
        strcat(dir_path, "/");
        strncat(dir_path, s_pvr_dir.ents[s_pvr_media_map.record[index]].fname, file_len - PVR_FIX_SIFFIX_LEN);
    }
    return dir_path;
}

static status_t _get_pvr_media_info(int index, PvrFileInfo *Info)
{
    char *dvr_dir_path = NULL;
    DirPara dvr_dir_para;
    int i;
    status_t ret = GXCORE_ERROR;

    if((s_pvr_media_map.total == 0) || (s_pvr_media_map.record == NULL))
    {
        return GXCORE_ERROR;
    }
     
    dvr_dir_path = _get_dvr_dir_path(index);
    if(dvr_dir_path != NULL)
    {
        memset(&dvr_dir_para, 0, sizeof(dvr_dir_para));
        dvr_dir_para.nents  = GxCore_GetDir(dvr_dir_path, &(dvr_dir_para.ents), PVR_MEDIE_SUFFIX);
        GxCore_Free(dvr_dir_path);
        dvr_dir_path = NULL;
        
        if(dvr_dir_para.nents > 0)
        {
            Info->size = 0;
            for(i = 0; i < dvr_dir_para.nents; i++)
            {
                Info->size += dvr_dir_para.ents[i].fsize;
            }
            Info->time = s_pvr_dir.ents[s_pvr_media_map.record[index]].fmtime;
            ret = GXCORE_SUCCESS;
        }
        app_free_dir_ent(&(dvr_dir_para.ents), &(dvr_dir_para.nents));
    }
    
    return ret;
}

static void _pvr_media_display_size_str(uint32_t size, char* str)
{
    float display_size = 0.0;
    
    if(size >= (1024*1024*1024))
    {
        display_size = (float)size / (1024*1024*1024);
        sprintf(str, "%3.2f GB", display_size);
    }
    else if(size >= (1024*1024))
    {
        display_size = (float)size / (1024*1024);
        sprintf(str, "%3.2f MB", display_size);
    }
    else if(size >= (1024))
    {
        display_size = (float)size / (1024);
        sprintf(str, "%3.2f KB", display_size);
    }
    else
    {
        display_size = (float)size;
        sprintf(str, "%3.2f B", display_size);
    }
}

uint8_t* app_time_to_format_str(time_t time_sec)
{
	int32_t config = 0;
	int32_t second;
	struct tm LocalTime;
	static uint8_t buf[50]={0};

	config = app_flash_get_config_timezone();
	second = config*3600+time_sec;			
	memset((void*)(&LocalTime),0,sizeof(struct tm));
	memcpy((void*)(&LocalTime), localtime((const time_t*) &second), sizeof(struct tm));
	LocalTime.tm_year += 1900;
	LocalTime.tm_mon ++;

	sprintf((void*)buf,
		"%04d-%02d-%02d %02d:%02d",
		LocalTime.tm_year,
		LocalTime.tm_mon,
		LocalTime.tm_mday,
		LocalTime.tm_hour,
		LocalTime.tm_min
		);	
	return buf;
}


static void display_pvr_media_info(PvrFileInfo *Info)
{
    char str[128] = {0};
    char *tm_str = NULL;
    time_t time_sec = 0;

    memset(str, 0, sizeof(str));
    strcpy(str, s_pvr_partition.partition_list->partition[s_pvr_partition.pvr_partition_num].partition_name);
    strcat(str, " ");
    strcat(str, PVR_DIR_STR);
    GUI_SetProperty(TXT_PVR_MEDIA_INFO_1, "string", str);

    memset(str, 0, sizeof(str));
    _pvr_media_display_size_str(Info->size, str);
    GUI_SetProperty(TXT_PVR_MEDIA_INFO_2, "string", str);

    memset(str, 0, sizeof(str));
    //time_sec = get_display_time_by_timezone(Info->time);
    time_sec = Info->time;
    tm_str = (char *)app_time_to_format_str(time_sec);
    if(tm_str != NULL)
    {
        strcpy(str, tm_str);
        GUI_SetProperty(TXT_PVR_MEDIA_INFO_3, "string", str);
    }
}

static status_t _delete_pvr_media(int index)
{
    status_t ret = GXCORE_SUCCESS;
    int i = 0;
    char *path = NULL;
    int path_len = 0;
    DirPara dvr_dir_para;

    if((s_pvr_media_map.total == 0) || (s_pvr_media_map.record == NULL))
    {
        return GXCORE_ERROR;
    }
    
//////delete xxx.ts.dvr///////
    path_len = strlen(s_pvr_dir.path);
    path_len += 1;// '/'
    path_len += strlen(s_pvr_dir.ents[s_pvr_media_map.record[index]].fname);
    path_len += 1; // '\0'

    path = (char*)GxCore_Malloc(path_len);
    if(path != NULL)
    {
        memset(path, 0, path_len);
        strcpy(path, s_pvr_dir.path);
        strcat(path, "/");
        strcat(path, s_pvr_dir.ents[s_pvr_media_map.record[index]].fname);
        GxCore_FileDelete(path);
        GxCore_Free(path);
        path = NULL;
    }
   
    path = _get_dvr_dir_path(index);
    if(path != NULL)
    {
        memset(&dvr_dir_para, 0, sizeof(dvr_dir_para));
        dvr_dir_para.nents  = GxCore_GetDir(path, &(dvr_dir_para.ents), NULL);
        
        path_len = strlen(path);
        if( dvr_dir_para.nents > 0)
        {
            char* sub_path = NULL;
            int sub_path_len = 0;
            for(i = 0; i < dvr_dir_para.nents; i++)
            {
                sub_path_len = path_len;
                sub_path_len += 1; // '/'
                sub_path_len += strlen(dvr_dir_para.ents[i].fname);
                sub_path_len += 1; // '\0'

                sub_path = (char*)GxCore_Malloc(sub_path_len);
                if(sub_path != NULL)
                {
                    memset(sub_path, 0, sub_path_len);
                    strcpy(sub_path, path);
                    strcat(sub_path, "/");
                    strcat(sub_path, dvr_dir_para.ents[i].fname);
                    GxCore_FileDelete(sub_path);
                    GxCore_Free(sub_path);
                    sub_path = NULL;
                }
            }
        }
        app_free_dir_ent(&(dvr_dir_para.ents), &(dvr_dir_para.nents));
        ret = GxCore_Rmdir(path);

        GxCore_Free(path);
        path = NULL;
    }

    return GXCORE_SUCCESS;
}

static void _play_pvr_media(int index)
{
    explorer_para explorer;

    if((s_pvr_media_map.total == 0) || (s_pvr_media_map.record == NULL))
        return;
        
    memset(&explorer, 0, sizeof(explorer_para));
    explorer.path = s_pvr_dir.path;
    explorer.suffix = PVR_INDEX_SUFFIX;
    explorer.nents = s_pvr_dir.nents;
    explorer.ents = s_pvr_dir.ents;
    
    play_list_init(PLAY_LIST_TYPE_MOVIE, &explorer, s_pvr_media_map.record[index]);
    GUI_CreateDialog("win_movie_view");
}

status_t app_pvr_media_update(void)
{
//    int value;
    int i = 0;
    int num = 0;
    char partition[PVR_PATH_LEN] = {0};

    status_t ret = GXCORE_ERROR;

    g_AppPvrOps.tms_delete(&g_AppPvrOps); 
    memset(&s_pvr_media_map, 0, sizeof(s_pvr_media_map));

    s_pvr_partition.partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
    if((NULL != s_pvr_partition.partition_list) && (0 < s_pvr_partition.partition_list->partition_num))
    { 
        GxBus_ConfigGet(PVR_PARTITION_KEY, partition, PVR_PATH_LEN, PVR_PARTITION);
	    for(i = 0; i < s_pvr_partition.partition_list->partition_num; i++)
	    {
	        if(strcmp(partition, s_pvr_partition.partition_list->partition[i].dev_name) == 0)
	        {
	            break;
	        }
	    }
	    if(i >= s_pvr_partition.partition_list->partition_num)
	    {
	        i = 0;
			s_pvr_partition.pvr_partition_num= 0;
	        GxBus_ConfigSet(PVR_PARTITION_KEY, s_pvr_partition.partition_list->partition[i].dev_name);
	    }
		else
			{
				s_pvr_partition.pvr_partition_num = i;
			}
/*        GxBus_ConfigGetInt(PVR_PARTITION_KEY, &value, PVR_PARTITION);
        if(value >= s_pvr_partition.partition_list->partition_num)
        {
            s_pvr_partition.pvr_partition_num= 0;
        }
        else
        {
            s_pvr_partition.pvr_partition_num = value;
        }*/

        app_free_dir_ent(&(s_pvr_dir.ents), &(s_pvr_dir.nents));
        memset(&s_pvr_dir, 0, sizeof(s_pvr_dir));
        strcpy(s_pvr_dir.path, s_pvr_partition.partition_list->partition[s_pvr_partition.pvr_partition_num].partition_entry);
//        strcpy(s_pvr_dir.path, s_pvr_partition.partition_list->partition[i].partition_entry);
		strcat(s_pvr_dir.path, PVR_DIR_STR);
        
        s_pvr_dir.nents = GxCore_GetDir(s_pvr_dir.path, &(s_pvr_dir.ents), PVR_INDEX_SUFFIX);
        if(s_pvr_dir.nents > 0)
        {
            GxCore_SortDir(s_pvr_dir.ents, s_pvr_dir.nents, NULL);

            for(i = 0; i < s_pvr_dir.nents; i++)
            {
                if(s_pvr_dir.ents[i].ftype == GX_FILE_REGULAR)
                {
                    s_pvr_media_map.total++;
                }
            }
            
            s_pvr_media_map.record = (int*)GxCore_Malloc(s_pvr_media_map.total * sizeof(int));
            num = 0;
            for(i = 0; i < s_pvr_dir.nents; i++)
            {
                if(s_pvr_dir.ents[i].ftype== GX_FILE_REGULAR)
                {
                    s_pvr_media_map.record[num++] = i;
                }
            }
        }
        else
        {
            app_free_dir_ent(&(s_pvr_dir.ents), &(s_pvr_dir.nents));
        }
        

        ret = GXCORE_SUCCESS;
    }

    return ret;
}

void app_create_pvr_media_menu(void)
{
    if(app_pvr_media_update() == GXCORE_SUCCESS)
    {
//		app_epg_close();
		//GxTtx_TtxStop();
		GUI_CreateDialog(WND_PVR_MEIDA);
		s_pvr_on_fullscreen = false;
    }
    else
    {
    	//notice
    	
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Insert the device please",POPMSG_TYPE_OK);
//	popmsg(200, 170, "Insert the device please", POPMSG_TYPE_OK);
       return;
    }
}

void app_create_pvr_media_fullsreeen(void)
{
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
        if(app_pvr_media_update() == GXCORE_SUCCESS)
        {
            g_AppFullArb.timer_stop();
            GUI_SetProperty("txt_full_state", "state", "hide");
            GUI_SetProperty("img_full_state", "state", "hide");
            g_AppFullArb.state.pause = STATE_OFF;
            g_AppFullArb.draw[EVENT_PAUSE](&g_AppFullArb);
            app_subt_pause();
            app_player_close(PLAYER_FOR_NORMAL);
            GUI_CreateDialog(WND_PVR_MEIDA);
            s_pvr_on_fullscreen = true;
        }
        else
        {
            memset(&pop, 0, sizeof(PopDlg));
            pop.type = POP_TYPE_OK;
            pop.str = STR_ID_NO_DEVICE;
            pop.mode = POP_MODE_UNBLOCK;
            popdlg_create(&pop);
        }
    }
#endif
}

void app_pvr_media_quit(void)
{
	/*if(s_pvr_on_fullscreen == true)
	{
	    g_AppFullArb.draw[EVENT_MUTE](&g_AppFullArb);
	    g_AppFullArb.draw[EVENT_TV_RADIO](&g_AppFullArb);
	}*/
//	app_epg_open();
	//GxTtx_TtxStop();
#if 0
	GUI_SetProperty("text_pvr_meida_title","state","hide");
	GUI_SetProperty("text_pvr_meida_title","draw_now",NULL);

	GUI_SetProperty("listview_pvr_list","state","hide");
	GUI_SetProperty("listview_pvr_list","draw_now",NULL);

	GUI_SetProperty("wnd_pvr_media_list_image_line","state","hide");
	GUI_SetProperty("wnd_pvr_media_list_image_line","draw_now",NULL);

	GUI_SetProperty("wnd_pvr_media_list_img_bottom_line","state","hide");
	GUI_SetProperty("wnd_pvr_media_list_img_bottom_line","draw_now",NULL);

	GUI_SetProperty("wnd_pvr_media_list_image_background","state","hide");
	GUI_SetProperty("wnd_pvr_media_list_image_background","draw_now",NULL);

	GUI_SetProperty("wnd_pvr_media_list_img_top_line","state","hide");
	GUI_SetProperty("wnd_pvr_media_list_img_top_line","draw_now",NULL);


	GUI_SetProperty("text_pvr_midia_info_1","state","hide");
	GUI_SetProperty("text_pvr_midia_info_1","draw_now",NULL);

	GUI_SetProperty("text_pvr_midia_info_2","state","hide");
	GUI_SetProperty("text_pvr_midia_info_2","draw_now",NULL);

	GUI_SetProperty("text_pvr_midia_info_3","state","hide");
	GUI_SetProperty("text_pvr_midia_info_3","draw_now",NULL);

//	GUI_SetProperty(WND_PVR_MEIDA,"draw_now",NULL);
#endif
	GUI_EndDialog(WND_PVR_MEIDA);
	GUI_SetProperty(WND_PVR_MEIDA,"draw_now",NULL);
	if(s_pvr_media_map.record != NULL)
	{
		GxCore_Free(s_pvr_media_map.record);
		s_pvr_media_map.record= NULL;
	}
		s_pvr_media_map.total = 0;

	app_free_dir_ent(&(s_pvr_dir.ents), &(s_pvr_dir.nents));
	memset(&s_pvr_dir, 0, sizeof(s_pvr_dir));
//	GUI_SetProperty(NULL,"draw_now",NULL);


	/*if(s_pvr_on_fullscreen == true)
	{
	    if (g_AppPlayOps.normal_play.play_total != 0)
	    {
	        g_AppPlayOps.program_play(PLAY_MODE_POINT, g_AppPlayOps.normal_play.play_count);
	        GUI_CreateDialog("wnd_channel_info");
	    }
	}*/

	s_pvr_on_fullscreen = false;
}


SIGNAL_HANDLER int app_pvr_media_create(GuiWidget *widget, void *usrdata)
{
    int sel = 0;
    PvrFileInfo file_info;

//    g_bUsbInput = USB_VIEW_GROUP_RECORD_LIST;
    if(s_pvr_media_map.total > 0)
    {
    	gi_PvrFileSel = sel;
        //GUI_SetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);
        memset(&file_info, 0, sizeof(PvrFileInfo));
        if( _get_pvr_media_info(sel, &file_info) == GXCORE_SUCCESS)
        {
            display_pvr_media_info(&file_info);
        }        
    }
    else
    {
        GUI_SetProperty(TXT_PVR_MEDIA_INFO_1, "string", "No Record File!");
    }

	app_set_win_create_flag(PVR_MEDIA_WIN);
    
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_media_destroy(GuiWidget *widget, void *usrdata)
{
#if 0
    if(s_pvr_media_map.record != NULL)
    {
        GxCore_Free(s_pvr_media_map.record);
        s_pvr_media_map.record= NULL;
    }
    s_pvr_media_map.total = 0;

    app_free_dir_ent(&(s_pvr_dir.ents), &(s_pvr_dir.nents));
    memset(&s_pvr_dir, 0, sizeof(s_pvr_dir));
//	g_bUsbInput = USB_VIEW_NON;

#endif
	/* for dvb */
	pmpset_exit();

	app_set_win_destroy_flag(PVR_MEDIA_WIN);

    return EVENT_TRANSFER_STOP;
}

static int _delete_pvr_media_poptip_cb(void)
{
    int sel = 0;
    GUI_GetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);
    _delete_pvr_media(sel);
    app_pvr_media_update();
    return 0;
}
static int _delete_pvr_media_all(void)
{
    while(s_pvr_media_map.total>0)
    {
        _delete_pvr_media(0);
        app_pvr_media_update();
    }
    return 0;
}
SIGNAL_HANDLER int app_pvr_media_keypress(GuiWidget *widget, void *usrdata)
{
    GUI_Event *event = NULL;
    int sel = 0;
    PvrFileInfo file_info;
	 popmsg_ret ret = POPMSG_RET_NO;
    
    event = (GUI_Event *)usrdata;
    if(GUI_KEYDOWN ==  event->type)
    {
            switch(find_virtualkey(event->key.sym))
            {
                case STBK_EXIT:
						 app_pvr_media_quit();
//						 GUI_EndDialog("win_auto_search");
//						 GUI_EndDialog("win_pvr_management");
//						 GUI_EndDialog("win_media_center");
//						 GUI_EndDialog("win_main_menu");
//						 app_win_exist_to_full_screen();
//						 app_exit_to_full_window();
						 break;
                case STBK_MENU:
                    app_pvr_media_quit();
                    break;
                case KEY_BLUE://全部删除
                    if(s_pvr_media_map.total == 0)
                        break;
                    ret = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Delete all file?",POPMSG_TYPE_YESNO);
                    if(ret != POPMSG_RET_YES)
                    {
                        break;
                    }
                     _delete_pvr_media_all();
                    
                    GUI_SetProperty(LISTVIEW_PVR_MEIDA, "update_all", NULL);
                    if(s_pvr_media_map.total == 0)
                    {
                        sel =-1;
                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_1, "string", "No Record File!");
                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_2, "string", " ");
                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_3, "string", " ");
                    }

					gi_PvrFileSel = sel;
					
                    //GUI_SetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);
                    break;
                case KEY_RED:
					{
	                    if (s_pvr_media_map.total == 0)
	                        break;

						int noSel = -1;
	                        
	                    GUI_GetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);
						/* modify fy:设置某行为激活行 */
						GUI_SetProperty(LISTVIEW_PVR_MEIDA,"active",&sel);	
	                    
	                    /*memset(&pop, 0, sizeof(PopDlg));
	                    pop.type = POP_TYPE_NO_BTN;
	                    pop.str = STR_ID_DELETING;
	                    pop.creat_cb = _delete_pvr_media_poptip_cb;
	                    pop.timeout_sec = 1;
	                    popdlg_create(&pop);*/
	        			 ret = app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Delete the file?",POPMSG_TYPE_YESNO);
						 
						 /* modify fy:取消某行为激活行*/
						 GUI_SetProperty(LISTVIEW_PVR_MEIDA,"active",&noSel);	
	//                    ret = popmsg(200, 170, "Delete the file?", POPMSG_TYPE_YESNO);
						 if(ret!=POPMSG_RET_YES)
						 {
						 		break;
						 }
	                    _delete_pvr_media_poptip_cb();
	                    
	                    GUI_SetProperty(LISTVIEW_PVR_MEIDA, "update_all", NULL);
	                    if(s_pvr_media_map.total == 0)
	                    {
	                        sel =-1;
	                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_1, "string", "No Record File!");
	                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_2, "string", " ");
	                        GUI_SetProperty(TXT_PVR_MEDIA_INFO_3, "string", " ");
	                    }
	                    else if(sel >= s_pvr_media_map.total)
	                    {
	                        sel = s_pvr_media_map.total - 1;
	                    }

						gi_PvrFileSel = sel;
	                    GUI_SetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);

	                    if( _get_pvr_media_info(sel, &file_info) == GXCORE_SUCCESS)
	                    {
	                        display_pvr_media_info(&file_info);
	                    }
	                    
	                    break;
	                }

                case STBK_OK:
                    GUI_GetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);
                    _play_pvr_media(sel);
                    break;
                    
                default:
                    break;
            }
    }
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_media_list_get_total(GuiWidget *widget, void *usrdata)
{
    return s_pvr_media_map.total;
}

SIGNAL_HANDLER int app_pvr_media_list_get_data(GuiWidget *widget, void *usrdata)
{
    ListItemPara* item = NULL;

    item = (ListItemPara*)usrdata;
    if(NULL == item)
        return GXCORE_ERROR;
    if(0 > item->sel)
        return GXCORE_ERROR;
    if(s_pvr_media_map.record == NULL)
        return GXCORE_ERROR;

    // col-0: num
    item->x_offset = 0;
    item->image = NULL;
    item->string = NULL;

    //col-1: channel name
    item = item->next;
    item->image = NULL;
    item->string = s_pvr_dir.ents[s_pvr_media_map.record[item->sel]].fname;

    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_pvr_media_list_draw(GuiWidget *widget, void *usrdata)
{
     GUI_SetProperty(LISTVIEW_PVR_MEIDA, "select", &gi_PvrFileSel);

	 return 0;
	 
}

SIGNAL_HANDLER int app_pvr_media_list_change(GuiWidget *widget, void *usrdata)
{
    int sel;
    PvrFileInfo file_info;
    
    GUI_GetProperty(LISTVIEW_PVR_MEIDA, "select", &sel);

	gi_PvrFileSel = sel;

    memset(&file_info, 0, sizeof(PvrFileInfo));
    if( _get_pvr_media_info(sel, &file_info) == GXCORE_SUCCESS)
    {
        display_pvr_media_info(&file_info);
    }
    else    
    {
        GUI_SetProperty(TXT_PVR_MEDIA_INFO_1, "string", " ");
        GUI_SetProperty(TXT_PVR_MEDIA_INFO_2, "string", " ");
        GUI_SetProperty(TXT_PVR_MEDIA_INFO_3, "string", " ");
    }

    return EVENT_TRANSFER_STOP;
}


#include "app.h"
//#include "app_file_list.h"

#define WND_FILE_LIST "wnd_file_list"
#define LIST_VIEW_FILE_LIST "listview_file_list"
#define SCROLL_FILE_LIST "scroll_file_list"
#define TXT_PATH_INFO "header_text_file_list_2"
#define IMG_LAST "img_file_list_last"
#define IMG_NEXT "img_file_list_next"
#define IMG_OK "img_file_list_ok"
//#define IMG_FILE "s_icon_file.bmp"
//#define IMG_DIR "s_icon_dir.bmp"
#define IMG_DIR  "MP_ICON_FILE.bmp"
#define IMG_FILE "MP_ICON_MOVICE.bmp"


#define POS_X 225
#define POS_Y 130

typedef enum
{
	FILE_MODE_PARTITION,
	FILE_MODE_FILE,
	FILE_MODE_INVALID
}FileListMode;

typedef struct
{
	char			path[500];
	int			nents;
	GxDirent*	ents;
}DirPara;

typedef struct
{
	bool	last_info;
	bool next_info;
	bool ok_info;
}FileListInfo;

//static DestPathMode s_dest_path_mode = DEST_MODE_FILE;
static WndStatus s_file_list_state = WND_EXEC;
static HotplugPartitionList* partition_list = NULL;
static FileListMode s_file_list_mode = FILE_MODE_PARTITION;
static DirPara s_cur_dir_para = {{0},0};
static FileListInfo s_file_list_info = {false};

static FileListParam s_file_list_param;
int gi_FileCurSelect = -1;

//extern void app_free_dir_ent(GxDirent **ents, int *nents);
//extern void top_usb_plug_info(int  state);
int app_file_list_change(GuiWidget *widget, void *usrdata);

void app_free_dir_ent(GxDirent **ents, int *nents)
{
#if 0
	int i = 0;

	if((ents != NULL) && (*ents != NULL)
	    && (nents != NULL))
	{
		for(i = 0; i < *nents; i++)
		{
			if((*ents + i)->fname != NULL)
			{
				GxCore_Free((*ents + i)->fname);
				(*ents + i)->fname = NULL;
			}
		}
		GxCore_Free(*ents);
		*ents = NULL;
		*nents = 0;
	}
#else
    GxCore_FreeDir(*ents, *nents);
    *ents = NULL;
	*nents = 0;
#endif
}

static status_t app_filelist_getdir(DirPara *dest_dir_para, char *suffix)
{
	if(dest_dir_para == NULL)
		return GXCORE_ERROR;

	app_free_dir_ent(&(dest_dir_para->ents), &(dest_dir_para->nents));
	dest_dir_para->nents = GxCore_GetDir(dest_dir_para->path, &(dest_dir_para->ents), suffix);
	if(dest_dir_para->nents > 0)
	{
		GxCore_SortDir(dest_dir_para->ents, dest_dir_para->nents, NULL);
	}	
	else
	{
		app_free_dir_ent(&(dest_dir_para->ents), &(dest_dir_para->nents));
	}
	return GXCORE_SUCCESS;
}

//GxCore_Free the reslut extern if result != NULL
static char* app_filelist_get_parrent_path(char *path)
{
	int len = 0;
	int temp_len = 0;
	char *temp_path = NULL;
	char *ptr = NULL;

	if(path == NULL)
		return NULL;

	len = strlen(path);
	if(len <= 0)
		return NULL;

	temp_path = (char *)GxCore_Malloc(len);
	if(temp_path == NULL)
		return NULL;

	memcpy(temp_path, path, len);

	ptr = temp_path + len;
	temp_len = len;
	while(ptr != temp_path)
	{
		if(*(--ptr) ==  '/')
		{
			temp_len--;
			temp_path[temp_len] = '\0';
			
			break;
		}
		temp_len--;
		temp_path[temp_len] = '\0';
	}

	if(temp_len == 0)
	{
		GxCore_Free(temp_path);
		temp_path = NULL;
	}
	
	return temp_path;
}

static FileListMode app_filelist_get_path_mode(char *path)
{
 	FileListMode mode;

	if(path == NULL)
	{
		mode = FILE_MODE_INVALID;
	}
	else
	{
		char *ptr = path;
		int count = 0;
		
		while(*ptr != '\0')
		{
			if(*ptr == '/')
			{
				count++;
			}
			ptr++;
		}

		if(count < 1)
			mode = FILE_MODE_INVALID;
		else if(count == 2)
			mode = FILE_MODE_PARTITION;// /mnt/usb_*/file
		else
			mode = FILE_MODE_FILE;
			
	}
	return mode;
}

static int app_filelist_get_sel(char *path, char *suffix)
{
	int sel = -1;
	FileListMode mode;
	int count = 0;

	if(path == NULL)
		return sel;
	
	mode = app_filelist_get_path_mode(path);
	if(mode == FILE_MODE_INVALID)
		return sel;

	if(mode == FILE_MODE_PARTITION)
	{
		for(count = 0; count < partition_list->partition_num; count++)
		{
			if(strcmp(path, partition_list->partition[count].partition_entry) == 0)
			{
				sel = count;
				break;
			}
		}
	}
	else
	{
		char *parrent_path = NULL;
		
		parrent_path = app_filelist_get_parrent_path(path);
		if(parrent_path != NULL)
		{	
			DirPara parrent_dir = {{0},0};
			int len = 0;
			
			len = strlen(parrent_path);
			memcpy(parrent_dir.path, parrent_path, len);

			GxCore_Free(parrent_path);
			parrent_path = NULL;

			app_filelist_getdir(&parrent_dir, suffix);
			strcat(parrent_dir.path, "/");
			for(count = 0; count < parrent_dir.nents; count++)
			{
				int name_len = 0;

				name_len = strlen(parrent_dir.ents[count].fname);
				memcpy(parrent_dir.path + len + 1, parrent_dir.ents[count].fname, name_len);
				parrent_dir.path[len + name_len + 1] = '\0';
				
				if(strcmp(path, parrent_dir.path) == 0)
				{
					sel = count;
					break;
				}
			}
			app_free_dir_ent(&(parrent_dir.ents), &(parrent_dir.nents));
		}
	}
	
	return sel;
}

static void app_file_list_set_info(FileListInfo *list_info)
{
	if(list_info->last_info == true)
		GUI_SetProperty(IMG_LAST, "state", "show");
	else
		GUI_SetProperty(IMG_LAST, "state", "hide");

	if(list_info->ok_info== true)
		GUI_SetProperty(IMG_OK, "state", "show");
	else
		GUI_SetProperty(IMG_OK, "state", "hide");

	if(list_info->next_info== true)
		GUI_SetProperty(IMG_NEXT, "state", "show");
	else
		GUI_SetProperty(IMG_NEXT,"state", "hide");	

}

static void app_file_list_last_dir(int sel)
{
	int set_sel = 0;
	char *info = NULL;
	
	if(FILE_MODE_FILE == s_file_list_mode)
	{
		set_sel = app_filelist_get_sel(s_cur_dir_para.path, s_file_list_param.suffix);
		if(set_sel < 0)
		{
			set_sel = 0;
			app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
			memset(&s_cur_dir_para, 0, sizeof(DirPara));
			strcpy(s_cur_dir_para.path, partition_list->partition[0].partition_entry);
			s_file_list_mode = FILE_MODE_PARTITION;
		}
		else
		{
			FileListMode mode;
			mode = app_filelist_get_path_mode(s_cur_dir_para.path);
			if(mode == FILE_MODE_INVALID)
			{
				set_sel = 0;
				app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
				memset(&s_cur_dir_para, 0, sizeof(DirPara));
				strcpy(s_cur_dir_para.path, partition_list->partition[0].partition_entry);
				s_file_list_mode = FILE_MODE_PARTITION;
			}
			else if(mode == FILE_MODE_PARTITION)
			{
				app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
				memset(&s_cur_dir_para, 0, sizeof(DirPara));
				strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
				s_file_list_mode = FILE_MODE_PARTITION;
			}
			else
			{
				char *parrent_path = NULL;
				parrent_path = app_filelist_get_parrent_path(s_cur_dir_para.path);
				if(parrent_path != NULL)
				{
					int len =0;
					len = strlen(parrent_path);
					app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
					memset(&s_cur_dir_para, 0, sizeof(DirPara));
					memcpy(s_cur_dir_para.path, parrent_path, len);
					app_filelist_getdir(&s_cur_dir_para, s_file_list_param.suffix);
					GxCore_Free(parrent_path);
					parrent_path = NULL;
					s_file_list_mode = FILE_MODE_FILE;
					info = s_cur_dir_para.path;
				}
				else
				{
					set_sel = 0;
					app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
					memset(&s_cur_dir_para, 0, sizeof(DirPara));
					strcpy(s_cur_dir_para.path, partition_list->partition[0].partition_entry);
					s_file_list_mode = FILE_MODE_PARTITION;
				}
			}
		}

		gi_FileCurSelect = set_sel;
	}

	GUI_SetProperty(LIST_VIEW_FILE_LIST, "update_all", NULL);
	GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &set_sel);
	GUI_SetProperty(TXT_PATH_INFO, "string", info);
	app_file_list_change(NULL, NULL);
}

static void app_file_list_next_dir(int sel)
{
	int set_sel = 0;
	char *info = NULL;

	if(FILE_MODE_PARTITION == s_file_list_mode)
	{
		app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
		memset(&s_cur_dir_para, 0, sizeof(DirPara));
		
		if((sel >=0) 
			&& ( partition_list->partition_num > sel))
		{		
			strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
			app_filelist_getdir(&s_cur_dir_para, s_file_list_param.suffix);
			s_file_list_mode = FILE_MODE_FILE;

			GUI_SetProperty(LIST_VIEW_FILE_LIST, "update_all", NULL);
			if(s_cur_dir_para.nents  > 0)
			{
				GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &set_sel);
			}
			else
			{
				set_sel = -1;
				GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &set_sel);
			}
			info = s_cur_dir_para.path;
		}
	}
	else  if(FILE_MODE_FILE == s_file_list_mode)
	{		
		if((sel >=0)
			&&(s_cur_dir_para.nents > sel)
			&&(GX_FILE_DIRECTORY == s_cur_dir_para.ents[sel].ftype))
		{
			if(0 == strcasecmp( s_cur_dir_para.ents[sel].fname, ".."))
			{
				app_file_list_last_dir(sel);
				return;
			}
			strcat(s_cur_dir_para.path, "/");
			strcat(s_cur_dir_para.path,  s_cur_dir_para.ents[sel].fname);
			app_filelist_getdir(&s_cur_dir_para, s_file_list_param.suffix);
			GUI_SetProperty(LIST_VIEW_FILE_LIST, "update_all", NULL);
			if(s_cur_dir_para.nents  > 0)
			{
				GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &set_sel);
			}
			else
			{
				set_sel = -1;
				GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &set_sel);
			}	
		}
		info = s_cur_dir_para.path;
	}
	else
		;

	gi_FileCurSelect = set_sel;
	
	GUI_SetProperty(TXT_PATH_INFO, "string", info);
	app_file_list_change(NULL, NULL);
}

WndStatus app_get_file_path_dlg(FileListParam *file_list)
{
	int sel = 0;
	char *info = NULL;

	if((file_list == NULL) || (file_list->dest_path == NULL))
		return WND_CANCLE;

	memcpy(&s_file_list_param, file_list, sizeof(FileListParam));

	app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
	memset(&s_cur_dir_para, 0, sizeof(DirPara));

	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
	if((NULL != partition_list) && (0 < partition_list->partition_num))
	{
		sel = app_filelist_get_sel(s_file_list_param.cur_path, s_file_list_param.suffix);
		if(sel < 0)
		{
			sel = 0;
			strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
			s_file_list_mode = FILE_MODE_PARTITION;
		}
		else
		{
			FileListMode mode;
			mode = app_filelist_get_path_mode(s_file_list_param.cur_path);
			if(mode == FILE_MODE_PARTITION)
			{
				strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
				s_file_list_mode = FILE_MODE_PARTITION;
			}
			else if(mode == FILE_MODE_FILE)
			{
				char *parrent_path = NULL;
				parrent_path = app_filelist_get_parrent_path(s_file_list_param.cur_path);
				if(parrent_path != NULL)
				{
					int len =0;
					len = strlen(parrent_path);
					memcpy(s_cur_dir_para.path, parrent_path, len);
					app_filelist_getdir(&s_cur_dir_para, s_file_list_param.suffix);
					GxCore_Free(parrent_path);
					parrent_path = NULL;
					s_file_list_mode = FILE_MODE_FILE;

					info = s_cur_dir_para.path;
				}
				else
				{
					sel = 0;
					memset(&s_cur_dir_para, 0, sizeof(DirPara));
					strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
					s_file_list_mode = FILE_MODE_PARTITION;
				}

			}
			else
				;
		}
	}
	else
	{
		sel = -1;
		s_file_list_mode = FILE_MODE_INVALID;
		//info = "warning";
	}

	GUI_CreateDialog(WND_FILE_LIST);

	gi_FileCurSelect = sel;

	if((s_file_list_param.pos_x == 0) && (s_file_list_param.pos_y== 0))
	{
		s_file_list_param.pos_x = POS_X;
		s_file_list_param.pos_y = POS_Y;
	}
//	GUI_SetProperty(WND_FILE_LIST, "move_window_x", &s_file_list_param.pos_x);
//    	GUI_SetProperty(WND_FILE_LIST, "move_window_y", &s_file_list_param.pos_y);
	//GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
	GUI_SetProperty(TXT_PATH_INFO, "string", info);
    printf("----%s\n",info);
	app_file_list_change(NULL, NULL);
	s_file_list_state = WND_EXEC;
	/*if (TRUE == app_get_pop_msg_flag_status())
	{
		s_file_list_state = WND_CANCLE;
	}
	else
	{
		app_set_pop_msg_flag_status(TRUE);
		while(s_file_list_state == WND_EXEC)
		{
	//		GUI_Exec();
			GUI_LoopEvent();
			GxCore_ThreadDelay(50);
		}
		app_set_pop_msg_flag_status(FALSE);
	}*/

	int loop_num = 0;
	int last_partition_num = partition_list->partition_num;
	while(WND_EXEC == s_file_list_state)
    {
        GUI_Loop();

		if(0 == (loop_num%10))
		{
			loop_num = 0;

			//------------------
			partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
			if((NULL != partition_list) && (0 < partition_list->partition_num))
			{
				if(partition_list->partition_num > last_partition_num)
				{
//					top_usb_plug_info(0);
				}
				else if(partition_list->partition_num < last_partition_num)
				{
//					top_usb_plug_info(1);
				}
				
				if(partition_list->partition_num != last_partition_num)
				{
					last_partition_num = partition_list->partition_num;

					
					sel = app_filelist_get_sel(s_cur_dir_para.path, s_file_list_param.suffix);
					if(sel < 0)
					{
						sel = 0;
						app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
						memset(&s_cur_dir_para, 0, sizeof(DirPara));
						strcpy(s_cur_dir_para.path, partition_list->partition[0].partition_entry);
						s_file_list_mode = FILE_MODE_PARTITION;

						GUI_SetProperty(TXT_PATH_INFO, "string", NULL);
					}
					else
					{
						if(FILE_MODE_FILE == s_file_list_mode)
						{
							continue;
						}
					}

					GUI_SetProperty(LIST_VIEW_FILE_LIST, "update_all", NULL);
					GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &sel);

					
				}
			}
			else
			{
				s_file_list_state = WND_CANCLE;
			}
		}

		loop_num++;
    }
	GUI_StartSchedule();


	if(s_file_list_state == WND_OK)
	{
		*(file_list->dest_path) = s_cur_dir_para.path;
	}

	app_free_dir_ent(&(s_cur_dir_para.ents), &(s_cur_dir_para.nents));
	GUI_EndDialog(WND_FILE_LIST);
	
	if (0 == partition_list->partition_num)
	{
		app_win_exist_to_win_widget("win_update_manual");
	}
	
	return s_file_list_state;
}

SIGNAL_HANDLER int app_file_list_create(GuiWidget *widget, void *usrdata)
{


	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_file_list_destroy(GuiWidget *widget, void *usrdata)
{
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int app_file_list_get_total(GuiWidget *widget, void *usrdata)
{
#define MAX_PROG_LIST_ITEM 7
	int num = 0;

	if(FILE_MODE_PARTITION == s_file_list_mode)
	{
		num = partition_list->partition_num;
	}
	else if(FILE_MODE_FILE== s_file_list_mode)
	{
		num = s_cur_dir_para.nents;
	}
	else
	{
		num = 0;
	}
	
	return num;
}

SIGNAL_HANDLER int app_file_list_keypress(GuiWidget *widget, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_STOP;
	int sel = 0;

	GUI_GetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{
			case KEY_MENU:
			case KEY_EXIT:
				s_file_list_state = WND_CANCLE;	
				gi_FileCurSelect = -1;
				break;
				
			case KEY_OK:
				if(sel < 0)
					break;
					
				if(s_file_list_param.dest_mode == DEST_MODE_FILE)
				{
					if(s_file_list_mode == FILE_MODE_FILE)
					{
						if(GX_FILE_REGULAR == s_cur_dir_para.ents[sel].ftype)
						{
							strcat(s_cur_dir_para.path, "/");
							strcat(s_cur_dir_para.path,  s_cur_dir_para.ents[sel].fname);
							s_file_list_state = WND_OK;
						}
						else if(GX_FILE_DIRECTORY == s_cur_dir_para.ents[sel].ftype)
						{
							app_file_list_next_dir(sel);
						}
					}
					else
					{
						app_file_list_next_dir(sel);
					}
					
				}
				else if(s_file_list_param.dest_mode == DEST_MODE_DIR)
				{
					if(s_file_list_mode == FILE_MODE_PARTITION)
					{
						strcpy(s_cur_dir_para.path, partition_list->partition[sel].partition_entry);
						s_file_list_state = WND_OK;
					}
					else if(s_file_list_mode == FILE_MODE_FILE)
					{
						if(GX_FILE_DIRECTORY == s_cur_dir_para.ents[sel].ftype)
						{
							strcat(s_cur_dir_para.path, "/");
							strcat(s_cur_dir_para.path,  s_cur_dir_para.ents[sel].fname);
							s_file_list_state = WND_OK;
						}
					}
				}
				else
					;
				break;	

			case KEY_LEFT:
				app_file_list_last_dir(sel);
				break;
				
			case KEY_RIGHT:
				app_file_list_next_dir(sel);
				break;
			case STBK_PAGE_UP:
				{
					uint32_t sel;
					GUI_GetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
					if (sel == 0)
					{
						sel = app_file_list_get_total(NULL,NULL)-1;
						gi_FileCurSelect = sel;
						
						GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
						ret =   EVENT_TRANSFER_STOP; 
					}
					else
					{
						ret =  EVENT_TRANSFER_KEEPON; 
					}
				}
					break;
			case STBK_PAGE_DOWN:
				{
					uint32_t sel;
					GUI_GetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
					if (app_file_list_get_total(NULL,NULL)== sel+1)
					{
						sel = 0;
						gi_FileCurSelect = sel;
						
						GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
						ret =  EVENT_TRANSFER_STOP; 
					}
					else
					{
						ret =  EVENT_TRANSFER_KEEPON; 
					}
				}
				break;
				
			default:
				ret = EVENT_TRANSFER_KEEPON;
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_file_list_get_data(GuiWidget *widget, void *usrdata)
{
#define CHANNEL_NAME_LEN	10
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return EVENT_TRANSFER_KEEPON;

	if(0 > item->sel)
		return EVENT_TRANSFER_KEEPON;
		
	if(FILE_MODE_PARTITION == s_file_list_mode)
	{
		if(item->sel < partition_list->partition_num)
		{
			//col-0: num
			item->x_offset = 0;
			item->image = IMG_DIR;
			item->string = NULL;

			//col-1: channel name
			item = item->next;
			item->x_offset = 0;
			item->image = NULL;
			item->string = partition_list->partition[item->sel].partition_name;
		}
	}
	else if(FILE_MODE_FILE== s_file_list_mode)
	{	
		if(item->sel < s_cur_dir_para.nents)
		{
			//col-0: num
			item->x_offset = 0;
			if(GX_FILE_DIRECTORY == s_cur_dir_para.ents[item->sel].ftype)
			{
			    item->image = IMG_DIR;
			}
			else if(GX_FILE_REGULAR == s_cur_dir_para.ents[item->sel].ftype)
			{
			    item->image = IMG_FILE;
			}
			else
			{
			    item->image = NULL;
			}
			item->string = NULL;

			//col-1: channel name
			item = item->next;
			item->x_offset = 0;
			item->image = NULL;
			item->string = s_cur_dir_para.ents[item->sel].fname;
		}
	}
	else
		;

	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_file_list_draw(GuiWidget * widget, void *usrdata)
{
	GUI_SetProperty(LIST_VIEW_FILE_LIST, "select", &gi_FileCurSelect);

	return 0;
	
}

SIGNAL_HANDLER int app_file_list_change(GuiWidget * widget, void *usrdata)
{
	int ret = EVENT_TRANSFER_STOP;
	int sel = 0;

	s_file_list_info.last_info = false;
	s_file_list_info.next_info = false;
	s_file_list_info.ok_info = false;
		
	if(FILE_MODE_PARTITION == s_file_list_mode)
	{
		if(partition_list->partition_num > 0)
		{
			GUI_GetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
			//if(s_file_list_param.dest_mode == DEST_MODE_DIR)
			{
				s_file_list_info.ok_info= true;
			}
			s_file_list_info.next_info= true;
		}
	}
	else if(FILE_MODE_FILE== s_file_list_mode)
	{
		if(s_cur_dir_para.nents > 0)
		{
			GUI_GetProperty(LIST_VIEW_FILE_LIST, "select", &sel);
			if(s_file_list_param.dest_mode== DEST_MODE_FILE)
			{
				//if(GX_FILE_REGULAR == s_cur_dir_para.ents[sel].ftype)
				{
					s_file_list_info.ok_info= true;
				}
			}
			else if(s_file_list_param.dest_mode == DEST_MODE_DIR)
			{
				if(GX_FILE_DIRECTORY == s_cur_dir_para.ents[sel].ftype)
				{
					s_file_list_info.ok_info= true;
				}
			}

			if(GX_FILE_DIRECTORY == s_cur_dir_para.ents[sel].ftype)
			{
				s_file_list_info.next_info= true;
			}
		}
		s_file_list_info.last_info = true;
	}
	else
		;

	app_file_list_set_info(&s_file_list_info);
	gi_FileCurSelect = sel;

	return ret;
}

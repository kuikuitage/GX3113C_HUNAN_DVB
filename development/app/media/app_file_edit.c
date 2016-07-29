#include "app.h"
#include "app_pop.h"

#define TEXT_WAITTING				"file_edit_txt_wait"
#define BOX_FILE_EDIT				"file_edit_box_edit"
#define LISTVIEW_FILE_BROWSER		"file_browser_listview"
#define WIN_FILE_EDIT				"win_file_edit"

static char* get_full_name_by_malloc(void)
{
	uint32_t index = 0;
	GxDirent* ent = NULL;
	char* full_name = NULL;

	if(NULL == explorer_view) return NULL;
	
	GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &index);
	if(index > explorer_view->nents) return NULL;

	ent = explorer_view->ents + (index - 1);
	if(NULL == ent) return NULL;
	
	full_name = explorer_malloc_path_strcat(explorer_view->path, ent->fname);
	if(NULL == full_name) return NULL;

	return full_name;
}

static status_t file_edit_ok_key(void)
{
	status_t ret = GXCORE_SUCCESS;
	uint32_t item_sel = 0;
	uint32_t file_no = 0;
	uint32_t file_num = 0;
	char* full_name = NULL;
	char* paste_path = NULL;
	bool check = FALSE;
    PopDlg  pop;
//	media_popmsg_ret msgret = MEDIA_POPMSG_RET_NO;

	if(NULL == explorer_view) return GXCORE_ERROR;

	GUI_GetProperty(BOX_FILE_EDIT, "select", (void*)&item_sel);
	switch(item_sel)
	{
		//copy
		case 0:
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
			if(file_num <= 1)
				break;
			full_name = get_full_name_by_malloc();
			file_edit_copy(full_name);
			FILE_EDIT_FREE(full_name);
			GUI_EndDialog(WIN_FILE_EDIT);
			break;

		//cut
		case 1:
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
			if(file_num <= 1)
				break;
			full_name = get_full_name_by_malloc();
			file_edit_cut(full_name);
			FILE_EDIT_FREE(full_name);
			GUI_EndDialog(WIN_FILE_EDIT);
			break;

		//paster
		case 2:
			paste_path = explorer_view->path;
			check = file_edit_check_paste_valid(paste_path);
			if(check == false)
			{
                PopDlg pop;
				GUI_EndDialog(WIN_FILE_EDIT);
				GUI_SetProperty(NULL, "draw_now", NULL);
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "Paste failed!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;		
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_OK);

				break; 
			}
			// TODO: exsit? yes_no check	
			check = file_edit_paste_check_exist(paste_path);
			if(true == check)
			{
                PopDlg pop;
				GUI_EndDialog(WIN_FILE_EDIT);
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "File already exists!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_OK);
				break;
			}
			
			GUI_SetProperty(BOX_FILE_EDIT, "state", "hide");
			GUI_SetProperty(TEXT_WAITTING, "state", "show");
			GUI_SetProperty(NULL, "draw_now", NULL);
			
			ret = file_edit_paste(paste_path);
			
			explorer_view = explorer_refreshdir(explorer_view);
			
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
			if(file_num > 1)
				file_no = 1;
			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", &file_no);
			GUI_EndDialog(WIN_FILE_EDIT);
			
			if(GXCORE_SUCCESS != ret)
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "Paste failed!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = 250;
				pop.pos.y = 100;
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_OK);
			}
			break;

		//delete
		case 3:
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
			if(file_num <= 1)
				break;
			GUI_GetProperty(LISTVIEW_FILE_BROWSER, "select", &file_no);
			
			// TODO: yes_no check
			full_name = get_full_name_by_malloc();

            memset(&pop, 0, sizeof(PopDlg));
            pop.type = POP_TYPE_YES_NO;
            pop.str = "Delete the file?";
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;
//			msgret = media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_YESNO);
            if(popdlg_create(&pop) == POP_VAL_OK)	
//			if(MEDIA_POPMSG_RET_YES == msgret)		
			{
				GUI_SetProperty(BOX_FILE_EDIT, "state", "hide");
				GUI_SetProperty(TEXT_WAITTING, "state", "show");
				GUI_SetProperty(NULL, "draw_now", NULL);
				
				ret = file_edit_delete(full_name);

				explorer_view = explorer_refreshdir(explorer_view);
				GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
				GUI_GetProperty(LISTVIEW_FILE_BROWSER, "total_num", &file_num);
				if(file_num <= file_no)
				{
					file_no -= 1;
				}
				if(file_no < 0) file_no = 0;
				GUI_SetProperty(LISTVIEW_FILE_BROWSER, "select", &file_no);
			}
			FILE_EDIT_FREE(full_name);
			GUI_EndDialog(WIN_FILE_EDIT);

			if(GXCORE_SUCCESS != ret)
			{
                PopDlg pop;
                memset(&pop, 0, sizeof(PopDlg));
                pop.type = POP_TYPE_OK;
                pop.format = POP_FORMAT_DLG;
                pop.str = "Delete failed!";
                pop.mode = POP_MODE_UNBLOCK;
				pop.pos.x = POP_MEDIA_X_START;
				pop.pos.y = POP_MEDIA_Y_START;
                popdlg_create(&pop);
//				media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_OK);
			}
			break;

		//rename
		case 4:
			full_name = get_full_name_by_malloc();
			file_edit_rename(full_name);
			FILE_EDIT_FREE(full_name);
			
			explorer_view = explorer_refreshdir(explorer_view);

			GUI_SetProperty(LISTVIEW_FILE_BROWSER, "update_all", NULL);
			GUI_EndDialog(WIN_FILE_EDIT);
			break;
		default:
			break;
	}
	
	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int file_edit_init(const char* widgetname, void *usrdata)
{
	GUI_SetProperty(TEXT_WAITTING, "state", "hide");
	return 0;
}

SIGNAL_HANDLER int file_edit_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case APPK_OK:
					file_edit_ok_key();
					break;
					
				default:
					break;
			}
			break;
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}


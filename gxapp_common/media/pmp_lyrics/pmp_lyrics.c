#include "pmp_lyrics.h"
#include "play_manage.h"
#include "stdlib.h"
#include "gui_core.h"
#include "lrc.h"
#include "play_music.h"
#include "app_common_media.h"
//#include "app.h"

#define LYRICS_BUFF_SIZE	(5*1024)

#define WIN_MUSIC_VIEW   "win_music_view"

//static lrctimelist_t* LrcTimeList = NULL;
//static uint32_t record_lrc_line = 0;
//event_list* lrc_timer = NULL;



pmp_lrc_para* lrc_para = NULL;

lrctimelist_t* lrc_list_record = NULL;

#define  NOTEPAD_PATCH
#ifdef  NOTEPAD_PATCH
int shownotepad=0;
void set_Notepad(void)
{
	shownotepad=1;
}

void clear_Notepad(void)
{
	shownotepad=0;
}
static  int lrc_string_set(void *userdata)
{
	//printf("consule thread %d\n",GxCore_ThreadGetId());
 	//printf("show lrcset %d %d\n",shownotepad,GxCore_ThreadGetId());
    if(shownotepad)
	{
		GUI_SetProperty(lrc_para->widget_lrc, "string", lrc_para->lrc_buff);
		clear_Notepad();
    }
	return 0;
}
static event_list * timer_show;
void lrc_timer_string_set(void)
{
	APP_TIMER_ADD(timer_show,lrc_string_set, 20, TIMER_REPEAT);
}
void lrc_timer_string_Remove(void)
{
	APP_TIMER_REMOVE(timer_show);
}
#else
void set_Notepad(void)
{
}

void clear_Notepad(void)
{
}
static  int lrc_string_set(void *userdata)
{
}
static int timer_show;
void lrc_timer_string_set(void)
{
}
void lrc_timer_string_Remove(void)
{
	
}

#endif

	
static bool lrc_check_name(const char* check_file, const char* play_file)
{
	int check_file_len = 0;
	int play_file_len = 0;
	int base_file_name_len = 0;
	int base_lrc_name_len = 0;
	int i = 0;

	if(NULL == check_file || NULL == play_file)
	{
		return FALSE;
	}

	check_file_len = strlen(check_file);
	play_file_len = strlen(play_file);

	// get play file's name length without suffix
	for(i = play_file_len - 1; i >= 0 ; i--)
	{
		if('.' == play_file[i])
		{	
			break;
		}
	}
	if(0 == i) return FALSE;
	base_file_name_len = i;

	// get lrc file's name length without suffix
	for(i = check_file_len- 1; i >= 0 ; i--)
	{
		if('.' == check_file[i])
		{	
			break;
		}
	}
	if(0 == i) return FALSE;
	base_lrc_name_len = i;
	
	if(base_file_name_len != base_lrc_name_len)
	{
		return FALSE;
	}

	if(check_file_len <= base_file_name_len)
	{
		return FALSE;
	}
	
	if(0 == strncasecmp((const char*)check_file,play_file, base_file_name_len))
	{
		return TRUE;
	}

	return FALSE;
}

static  int lrc_scroll(void *userdata)
{
	lrctimelist_t* lrc_list_node = lrc_para->list;
	uint32_t lrc_no=0,scroll_line;
//	uint64_t cur_time_ms = 0;
	//int total_time_ms = 0;
	status_t ret = GXCORE_ERROR;
	PlayTimeInfo t_PlayTimeinfo = {0};

	char* focus_win = NULL;
	focus_win = (char*)GUI_GetFocusWindow();
	if(NULL == focus_win) 
		return 1;

	if (!((0 == strcasecmp(focus_win, WIN_MUSIC_VIEW)) || (0 == strcasecmp(focus_win, "win_volume"))))
		return 1;
		
	if(NULL == lrc_list_node) 
		return 1;

	
	ret = GxPlayer_MediaGetTime(PMP_PLAYER_AV, &t_PlayTimeinfo);//&total_time_ms);
	if(GXCORE_SUCCESS != ret) return 1;
	//if((cur_time_ms > total_time_ms)||(0 == total_time_ms)) return 1;
//printf("[LRC] %d ms\n", cur_time_ms);	
	/*init*/
	lrc_no = 0xff;
	if(NULL == lrc_list_record)
	{
		lrc_list_record = lrc_para->list;
	}

	/*not start*/
	lrc_list_node = lrc_para->list;
	if(t_PlayTimeinfo.current < lrc_list_node->time)
	{
		lrc_no = 0;
	}
	/*find next*/
	else
	{
		for (lrc_list_node = lrc_list_record;lrc_list_node != NULL;)
		{
			/*find*/
			if(lrc_list_node->next)
			{
				/*seek backward, find from first*/
				if(t_PlayTimeinfo.current < lrc_list_node->time)
				{
					printf("[LRC] seek backward, find from first\n");
					lrc_list_node = lrc_para->list;
					continue;
				}
				/*find node*/
				else if((t_PlayTimeinfo.current >= (lrc_list_node->time)) && (t_PlayTimeinfo.current < (lrc_list_node->next->time)))
				{
					lrc_no = lrc_list_node->number;
					break;
				}
				/*find next*/
				else
				{
					printf("[LRC] find next\n");// TODO: have bug
					lrc_list_node = lrc_list_node->next;
				}
			}
			/*last node*/
			else
			{
				/*last node actived to end*/
				if(t_PlayTimeinfo.current >= (lrc_list_node->time))
				{
					lrc_no = lrc_list_node->number;
					break;
				}
				/*seek backward from last node*/
				else
				{
					printf("[LRC] seek backward from last node\n");
					lrc_list_node = lrc_para->list;
					continue;
				}
			}
		}
	}

	


	lrc_list_record = lrc_list_node;
	
	if(lrc_no != lrc_para->record_line)
	{
		int lrc_no2=0;
		GUI_SetProperty(lrc_para->widget_lrc, "current_line", &lrc_no2);

		GUI_SetProperty(lrc_para->widget_lrc, "current_line", &lrc_no);

		scroll_line = lrc_no+3;
		GUI_SetProperty(lrc_para->widget_lrc, "active", &scroll_line);
		lrc_para->record_line = lrc_no;
	}

/*	
	if(LrcTempList!=NULL)
	{
		while(LrcTempList->next!=NULL)
		{
			if((cur_time_ms<(LrcTempList->time))&&(lrc_para->record_line == 0))
			{
				lrc_no = 0;
				break;
			}
			if((cur_time_ms>(LrcTempList->time))&&(cur_time_ms<(LrcTempList->next->time)))
			{
				lrc_no = LrcTempList->number;
				break;
			}
			lrc_no = LrcTempList->number;
			LrcTempList = LrcTempList->next;
		}
		if(lrc_no!= lrc_para->record_line)
		{
printf("[LRC] -----%d line\n", lrc_no);	
			//scroll_line = lrc_no-lrc_para->record_line;
			//GUI_SetProperty(lrc_para->widget_lrc, "line_down", &scroll_line);
			GUI_SetProperty(lrc_para->widget_lrc, "current_line", &lrc_no);
			scroll_line = lrc_no+7;
			GUI_SetProperty(lrc_para->widget_lrc, "active", &scroll_line);
			lrc_para->record_line = lrc_no;
		}
		return 1;
	}
	*/
	return 0;
}


char* lrc_getfile(void)
{
	GxDirent* ent = NULL;
	int count = 0;
	int i = 0;
	play_music_info music_info;
	explorer_para* explorer_lrc = NULL;

	if(NULL == explorer_view) return NULL;
	explorer_lrc = explorer_opendir(explorer_view->path, SUFFIX_LRC);
	if(NULL == explorer_lrc) return NULL;
		
	play_music_get_info(&music_info);

	count = explorer_lrc->nents;
	for(i = 0; i < count; i++)
	{
		ent = explorer_lrc->ents + i;
		if(NULL == ent) 
		{
			explorer_closedir(explorer_lrc);
			return NULL;
		}

		if(GX_FILE_REGULAR == ent->ftype)
		{
			if(lrc_check_name(ent->fname, music_info.name))
			{
				char* path = NULL;
				path = explorer_static_path_strcat(music_info.path, ent->fname);
				explorer_closedir(explorer_lrc);
				return path;
			}
		}
	}

	explorer_closedir(explorer_lrc);
	return NULL;
}

void lrc_load(const char* file)
{
	Lrc* thiz = NULL;
	LrcIdTagIter id_iter = {0};
	LrcTimeTagIter time_iter = {0};
	//const char* key = NULL;
	
	//uint8_t lrc_buff[4048];
	//memset(lrc_buff,0,4048);
	
	lrctimelist_t* LrcTempList = NULL;
	lrctimelist_t* TempList = NULL;
	static uint8_t i = 0;

	memset(lrc_para->lrc_buff,0,lrc_para->lrc_buff_size);
	strcat(lrc_para->lrc_buff,"\n\n\n");

	thiz = lrc_new_from_file(file);
	if(thiz != NULL)
	{
		id_iter = lrc_get_id_tags(thiz);
		while(lrc_id_tag_iter_has_next(&id_iter))
		{
			id_iter = lrc_id_tag_iter_next(&id_iter);
		}
		while(lrc_id_tag_iter_has_prev(&id_iter))
		{
			id_iter = lrc_id_tag_iter_prev(&id_iter);
		}
		
		while(!lrc_id_tag_iter_is_null(&id_iter))
		{
			//LrcIdTagIter iter1 = {0};
			id_iter = lrc_id_tag_iter_next(&id_iter);
		}

		time_iter = lrc_get_time_tags(thiz);
		while(lrc_time_tag_iter_has_next(&time_iter))
		{
			time_iter = lrc_time_tag_iter_next(&time_iter);
		}
		while(lrc_time_tag_iter_has_prev(&time_iter))
		{
			time_iter = lrc_time_tag_iter_prev(&time_iter);
		}

		while(!lrc_time_tag_iter_is_null(&time_iter))
		{
			LrcTimeTagIter iter = {0};
			size_t start_time = lrc_time_tag_iter_get_start_time(&time_iter);

			LrcTempList = lrc_para->list;
			TempList = (lrctimelist_t*)GxCore_Malloc(sizeof(lrctimelist_t));
			if (NULL == TempList)
				return;

			TempList->time = start_time*10;
			TempList->next = NULL;
			TempList->number = i;
			i++;
			if(NULL!=LrcTempList)
			{
				while( LrcTempList->next!= NULL )
				{
					LrcTempList = LrcTempList->next;
				}
				LrcTempList->next = TempList;
			}
			else
			{
				lrc_para->list = TempList;
			}												
			//APP_Printf("[%d:%02d.%02d] %s\n", start_time/6000, (start_time%6000)/100, (start_time%6000)%100,
			//	lrc_time_tag_iter_get_lrc(&time_iter));

			int len = 0;
			char* str = NULL;

			str = (char*)lrc_time_tag_iter_get_lrc(&time_iter);
			len = strlen(lrc_para->lrc_buff) + strlen(str) + 20;
			if(len > lrc_para->lrc_buff_size)
			{
				printf("[LRC] lrc_buff_size overflow\n");
				break;
			}
			
			strcat(lrc_para->lrc_buff, str);
			strcat(lrc_para->lrc_buff,"\n");
			
			iter = lrc_get_time_tag_by_time(thiz, start_time + 1);
			time_iter = lrc_time_tag_iter_next(&time_iter);
		}
		
		lrc_destroy(thiz);
		GUI_SetProperty(lrc_para->widget_lrc, "string", " ");
		//GUI_SetProperty(lrc_para->widget_lrc, "draw_now",NULL );
		//GUI_Exec();
		GUI_SetInterface("flush",NULL);

		
		#ifdef  NOTEPAD_PATCH
		printf("msg thread %d\n",GxCore_ThreadGetId());
		set_Notepad();
		#else
		printf("before show lrc\n");
		GUI_SetProperty(lrc_para->widget_lrc, "string", lrc_para->lrc_buff);
		printf("atfer  show lrc\n");
		#endif
		i = 0;
	}
}

void lrc_unload(void)
{
	lrctimelist_t *p_list = lrc_para->list;
	lrctimelist_t *temp = NULL;

	while(p_list != NULL)
	{
		temp = p_list;
		p_list = p_list->next;
		GxCore_Free(temp);
		temp = NULL;
	}
	lrc_para->list = NULL;
	
	if(lrc_para->file != NULL)
	{
		APP_FREE(lrc_para->file);
	}

	lrc_para->record_line = 0;
}

void lrc_timer_start(void)
{
	APP_TIMER_ADD(lrc_para->timer,  lrc_scroll, 300, TIMER_REPEAT);
}

void lrc_timer_stop(void)
{
	if(lrc_para->timer)
	{	
		timer_stop(lrc_para->timer);
	}
}

int lyrics_create(char* widget_lyc)
{
	printf("[LRC] lyrics_create\n");

	if(lrc_para != NULL)
	{
		lrc_unload();
		/*widget*/
		APP_FREE(lrc_para->widget_lrc);
		/*buff*/
		APP_FREE(lrc_para->lrc_buff);
	}
	
	/*para*/
	APP_FREE(lrc_para);
	
	lrc_para = GxCore_Malloc(sizeof(pmp_lrc_para));
	APP_CHECK_P(lrc_para, 1);
	memset(lrc_para, 0, sizeof(pmp_lrc_para));

	/*buff*/
	lrc_para->lrc_buff = GxCore_Malloc(LYRICS_BUFF_SIZE);
	APP_CHECK_P(lrc_para->lrc_buff, 1);
	lrc_para->lrc_buff_size = LYRICS_BUFF_SIZE;
	
	/*widget*/
	lrc_para->widget_lrc = GxCore_Strdup(widget_lyc);
	APP_CHECK_P(lrc_para->widget_lrc , 1);
	GUI_SetProperty(lrc_para->widget_lrc , "state", "show");

	lrc_list_record = NULL;
	
	return 0;
}

int lyrics_destroy(void)
{
	printf("[LRC] lyrics_destroy\n");

	APP_CHECK_P(lrc_para, 1);
	GUI_SetProperty(lrc_para->widget_lrc , "state", "hide");

	/*stop*/
	lyrics_stop();
	
	/*timer*/
	APP_TIMER_REMOVE(lrc_para->timer);	

	/*widget*/
	APP_FREE(lrc_para->widget_lrc);	
	
	/*buff*/
	APP_FREE(lrc_para->lrc_buff);
	
	/*para*/
	APP_FREE(lrc_para);

	return 0;
}


int lyrics_start(char* file)
{
	char* file_load = NULL;
	int32_t file_size;
	GxFileInfo info;
	char* osd_language=NULL;


	APP_CHECK_P(lrc_para, 1);
	lrc_list_record=NULL;
	/*get file*/	
	if(NULL == file)
	{
		file_load = lrc_getfile();
	}
	else
	{
		file_load = file;
	}
	printf("[LRC] lyrics_start, %s\n", file_load);


	/*unload old*/
	if(lrc_para->file)
	{
		if(NULL == file_load)
		{
			lyrics_stop();
		}
		else if(0 == strcasecmp(file_load, lrc_para->file))
		{
			printf("[LRC] already load %s\n", file_load);
			return 0;
		}
		else
		{
			lyrics_stop();
		}
	}

	/*load new*/
	if(NULL == file_load)
	{
		//bean
		pmpset_lang lang = 0;
		lang = pmpset_get_int(PMPSET_LANG);

		GUI_SetProperty(lrc_para->widget_lrc, "string", STR_ID_NO_LRC);
		osd_language = app_flash_get_config_osd_language();
		if (0 == strcmp(osd_language,"Chinese"))
			GUI_SetProperty(lrc_para->widget_lrc, "string", "Ã»ÓÐ¸è´Ê!");

			
		int sel = 3;
		GUI_SetProperty(lrc_para->widget_lrc, "active", (void*)&sel);
		return 1;
	}

	lrc_unload();
	/*strdup file*/
	lrc_para->file = GxCore_Strdup(file_load);
	APP_CHECK_P(lrc_para->file, 1);

	/*realloc*/
	GxCore_GetFileInfo(lrc_para->file, &info);
	file_size = info.size_by_bytes * 2;
	if(file_size > lrc_para->lrc_buff_size)
	{
		printf("[LRC] lrc_buff %dB relloc to %dB\n", lrc_para->lrc_buff_size, file_size);
		lrc_para->lrc_buff = GxCore_Realloc(lrc_para->lrc_buff ,file_size);
		APP_CHECK_P(lrc_para->lrc_buff, 1);
		lrc_para->lrc_buff_size = file_size;
	}
	
	/*load*/
	lrc_load(lrc_para->file);
	if(NULL == lrc_para->list)
	{
		printf("load error\n");
		APP_FREE(lrc_para->file);
		return 1;
	}
	lrc_para->record_line = 0xff;


	/*timer*/
	lrc_timer_start();

	return 0;
}

int lyrics_stop(void)
{
	printf("[LRC] lyrics_stop\n");

	APP_CHECK_P(lrc_para, 1);
	
	/*unload*/
	lrc_unload();

	/*free file*/
	//APP_FREE(lrc_para->file);

	/*timer*/
	lrc_timer_stop();

	GUI_SetProperty(lrc_para->widget_lrc, "string", NULL);
	int i = -1;
	GUI_SetProperty(lrc_para->widget_lrc, "active", &i);

	return 0;
}

int  lyrics_pause(void)
{
	printf("[LRC] lyrics_pause\n");

	APP_CHECK_P(lrc_para, 1);
	
	lrc_timer_stop();	

	//lrc_para->record_line = 0;

	return 0;
}

int  lyrics_resume(void)
{
	printf("[LRC] lyrics_resume\n");

	APP_CHECK_P(lrc_para, 1);
	
	lrc_timer_start();	

	return 0;
}



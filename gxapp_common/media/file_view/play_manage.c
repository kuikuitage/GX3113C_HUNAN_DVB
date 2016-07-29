//#include "app.h"
#include "play_manage.h"
#include "app_common_media.h"


status_t play_av_by_url(const char* url, int start_time_ms)
{
	if(NULL == url) return GXCORE_ERROR;

	pmpset_set_str(PMPSET_LAST_PLAY_PATH, (char*)url);
	
	GxMessage *msg;
	GxMsgProperty_PlayerPlay *player_play;

	msg = GxBus_MessageNew(GXMSG_PLAYER_PLAY);
	APP_CHECK_P(msg, GXCORE_ERROR);

	player_play = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerPlay);
	APP_CHECK_P(player_play, GXCORE_ERROR);
	player_play->player = PMP_PLAYER_AV;
	strcpy(player_play->url, url);
	player_play->start = start_time_ms;
	
	GxBus_MessageSend(msg);

	return GXCORE_SUCCESS;
}

status_t play_av_by_path_file(const char* path, const char* file, int start_time_ms)
{
	char* full_name = NULL;
	
	full_name = explorer_static_path_strcat(path, file);
	if(NULL == path) return GXCORE_ERROR;

	play_av_by_url(full_name, start_time_ms);
	
	return GXCORE_SUCCESS;
}

#if 0
status_t play_list_init(file_view_group group, int start_play_file)
{
	int i = 0;
	bool check = FALSE;
	file_view_files* files_src = NULL;
	play_list* list = NULL;

	if(FILE_VIEW_GROUP_ALL == group)return GXCORE_ERROR;
	
	if(FILE_VIEW_GROUP_ALL != file_view.files.group)
	{
		if(group != file_view.files.group)
		{
			printf( "[PLAY] can not create list from different view group\n");
			return GXCORE_ERROR;
		}
	}
	
	play_list_clear(group);
	play_list_get(group, &list);
	if(NULL == list) return GXCORE_ERROR;

	files_src = &file_view.files;

	list->group = group;
	list->play_no = 0;
	list->path = GxCore_Strdup(file_view.path);
	list->nents = 0;
	list->ents = (char**)GxCore_Malloc(files_src->nents * sizeof(char*));
	if(NULL == list->ents)return GXCORE_ERROR;
	
	
	for(i  = files_src->reg_ent_start; i < files_src->nents; i++)
	{
		if(FILE_VIEW_GROUP_ALL == files_src->group)
		{
			check = file_view_check_group(files_src->ents[i], group);
		}
		else
		{
			check = TRUE;
		}
		
		if(check)
		{
			if(i == start_play_file)
			{
				list->play_no = list->nents;
			}

			list->ents[list->nents] = GxCore_Strdup(files_src->ents[i]);
			list->nents++;
		}
	}

	return GXCORE_SUCCESS;
}


status_t play_list_get(file_view_group group, play_list** list)
{
	if(FILE_VIEW_GROUP_ALL == group) return GXCORE_ERROR;
	if(NULL == list) return GXCORE_ERROR;

	*list = NULL;
	
	if((FILE_VIEW_GROUP_MUSIC == group)
		||(FILE_VIEW_GROUP_MOVIE == group) )
	{
		*list = &app_play_music_movie_list;
	}
	else if((FILE_VIEW_GROUP_PIC == group)
		||(FILE_VIEW_GROUP_TEXT == group) )
	{
		*list = &app_play_pic_text_list;
	}
	
	return GXCORE_SUCCESS;
}


status_t play_list_clear(file_view_group group)
{
	int i = 0;
	play_list* list = NULL;
	
	play_list_get(group, &list);
	if(NULL == list) return GXCORE_ERROR;
	
	PLAY_FREE(list->path);
	for(i = 0; i < list->nents; i++)
	{
		PLAY_FREE(list->ents[i]);
	}
	PLAY_FREE(list->ents);
	
	memset(list, 0, sizeof(play_list));
	
	return GXCORE_SUCCESS;
}



#define sss
#endif

static play_list play_list_movie = {0};
static play_list play_list_music = {0};
static play_list play_list_pic = {0};
static play_list play_list_text = {0};



status_t play_init(void)
{

	memset(&play_list_movie, 0, sizeof(play_list));
	memset(&play_list_music, 0, sizeof(play_list));
	memset(&play_list_pic,   0, sizeof(play_list));
	memset(&play_list_text,  0, sizeof(play_list));

	return GXCORE_SUCCESS;
}


status_t play_list_clear(play_list* list)
{
	int i = 0;
	
	if(NULL == list) return GXCORE_ERROR;
	if(NULL == list->path) return GXCORE_ERROR; 

	PLAY_FREE(list->path);
	for(i = 0; i < list->nents; i++)
	{
		PLAY_FREE(list->ents[i]);
	}
	PLAY_FREE(list->ents);
	
	memset(list, 0, sizeof(play_list));

	return GXCORE_SUCCESS;
}


status_t play_list_init(play_list_type type, explorer_para* explorer, int select_file_no)
{
	file_view_group group_view = FILE_VIEW_GROUP_ALL;
	file_view_group group_ent = FILE_VIEW_GROUP_ALL;
	
	play_list* list = NULL;
	GxDirent* ent = NULL;
	int i = 0;
	
	if(NULL == explorer) return GXCORE_ERROR;
	if(select_file_no >= explorer->nents)
		select_file_no = explorer->nents - 1;

	switch(type)
	{
		case PLAY_LIST_TYPE_MOVIE:
			list = &play_list_movie;
			group_view = FILE_VIEW_GROUP_MOVIE;
			break;
		case PLAY_LIST_TYPE_MUSIC:
			list = &play_list_music;
			group_view = FILE_VIEW_GROUP_MUSIC;
			break;
		case PLAY_LIST_TYPE_PIC:
			list = &play_list_pic;
			group_view = FILE_VIEW_GROUP_PIC;
			break;
		case PLAY_LIST_TYPE_TEXT:
			list = &play_list_text;
			group_view = FILE_VIEW_GROUP_TEXT;
			break;
		default:
			list = NULL;
			return GXCORE_ERROR;
	}


	play_list_clear(list);


	list->type = type;
	list->play_no = 0;
	list->path = GxCore_Strdup(explorer->path);
	list->nents = 0;
	list->ents = (char**)GxCore_Malloc(explorer->nents * sizeof(char*));
	if(NULL == list->ents)return GXCORE_ERROR;

	for(i = 0; i < explorer->nents; i++)
	{
		ent = explorer->ents + i;
		if(NULL == ent) break;

		if(GX_FILE_DIRECTORY == ent->ftype) continue;

		//suppose: explorer->suffix accord with play_type
		if(NULL == explorer->suffix)
		{
			group_ent = file_view_get_file_group(ent->fname);
			if(group_ent != group_view) continue;
		}
		
		if(i == select_file_no)
		{
			list->play_no = list->nents;
		}

		list->ents[list->nents] = GxCore_Strdup(ent->fname);
		list->nents++;		
	}

	return GXCORE_SUCCESS;
}

play_list* play_list_get(play_list_type type)
{
	play_list* list = NULL;
	
	switch(type)
	{
		case PLAY_LIST_TYPE_MOVIE:
			list = &play_list_movie;
			break;
		case PLAY_LIST_TYPE_MUSIC:
			list = &play_list_music;
			break;
		case PLAY_LIST_TYPE_PIC:
			list = &play_list_pic;
			break;
		case PLAY_LIST_TYPE_TEXT:
			list = &play_list_text;
			break;
		default:
			list = NULL;
			break;
	}

	return list;
}


//#include "app.h"
#include "file_view.h"
#include "play_movie.h"
#include "play_manage.h"
#include "stdlib.h"
#include "app_common_media.h"
#include "gxapp_sys_config.h"


status_t play_movie(uint32_t file_no, int start_time_ms)
{
	play_list* list = NULL;
	char* path = NULL;

	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	if(NULL == list) return GXCORE_ERROR;

	if(file_no >= list->nents)
	{
		return GXCORE_ERROR;
	}

	list->play_no = file_no;
	path = explorer_static_path_strcat(list->path, list->ents[file_no]);
	if(NULL == path) return GXCORE_ERROR;

	printf("[PLAY] movie: %s, start: %d\n", path, start_time_ms);
	
#ifdef MENCENT_FREEE_SPACE
//	GUI_SetInterface("free_space", "fragment|spp|osd|back_osd");
	GUI_SetInterface("free_space", "fragment|spp");

	GxCore_HwCleanCache();
 #endif
	play_av_by_url(path, start_time_ms);

	return GXCORE_SUCCESS;
}

status_t play_movie_ctrol(play_movie_ctrol_state ctrol)
{
	status_t ret = GXCORE_ERROR;
	play_list* list = NULL;
	uint32_t file_no = 0;
	uint32_t file_count = 0;
	GxMessage *msg;


	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	if(NULL == list) return GXCORE_ERROR;
	
	file_no = list->play_no;
	file_count = list->nents;
	if(0 == file_count)
	{
		return GXCORE_ERROR;
	}


	if(PLAY_MOVIE_CTROL_PREVIOUS == ctrol)
	{
		if(0 == file_count) return GXCORE_SUCCESS;

		if(1 == file_count)
		{
			file_no = 0;
		}
		else if(file_no <=0)
		{
			file_no = file_count  - 1;
		}
		else
		{
			file_no--;
		}
		ret = play_movie(file_no, 0);
	}
	else if(PLAY_MOVIE_CTROL_NEXT == ctrol)
	{
		if(0 == file_count) return GXCORE_SUCCESS;
		
		if(1 == file_count)
		{
			file_no = 0;
		}
		else if(file_no >= file_count -1)
		{
			file_no = 0;
		}
		else
		{
			file_no++;
		}
		ret = play_movie(file_no, 0);
	}
	else if(PLAY_MOVIE_CTROL_PLAY == ctrol)
	{
		play_movie(list->play_no, 0);
	}
	else if(PLAY_MOVIE_CTROL_PAUSE == ctrol)
	{
		msg = GxBus_MessageNew(GXMSG_PLAYER_PAUSE);
		APP_CHECK_P(msg, GXCORE_ERROR);
		
		GxMsgProperty_PlayerPause *pause;
		pause = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerPause);
		APP_CHECK_P(pause, GXCORE_ERROR);
		pause->player = PMP_PLAYER_AV;

		GxBus_MessageSendWait(msg);
		GxBus_MessageFree(msg);
	}
	else if(PLAY_MOVIE_CTROL_RESUME == ctrol)
	{
		//TODO: after stop, resume change to play
		//play_movie(list->play_no, 0);
		
		msg = GxBus_MessageNew(GXMSG_PLAYER_RESUME);
		APP_CHECK_P(msg, GXCORE_ERROR);
		
		GxMsgProperty_PlayerResume *resume;
		resume = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerResume);
		APP_CHECK_P(resume, GXCORE_ERROR);
		resume->player = PMP_PLAYER_AV;

		GxBus_MessageSendWait(msg);
		GxBus_MessageFree(msg);
	}	
	else if(PLAY_MOVIE_CTROL_STOP == ctrol)
	{
		msg = GxBus_MessageNew(GXMSG_PLAYER_STOP);
		APP_CHECK_P(msg, GXCORE_ERROR);
		
		GxMsgProperty_PlayerStop *stop;
		stop = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerStop);
		APP_CHECK_P(stop, GXCORE_ERROR);
		stop->player = PMP_PLAYER_AV;

		GxBus_MessageSendWait(msg);
		GxBus_MessageFree(msg);
	}
	else if(PLAY_MOVIE_CTROL_RANDOM== ctrol)
	{
		int random_no = rand();
		random_no = random_no%file_count;
		if(random_no == list->play_no)
		{
			random_no = random_no+1%file_count;
		}
		ret = play_movie(random_no, 0);
	}
	else
	{
		return GXCORE_ERROR;
	}
	
	return GXCORE_SUCCESS;	
}


status_t play_movie_speed(play_movie_ctrol_state ctrol, play_movie_speed_state speed)
{
	GxMsgProperty_PlayerSpeed player_speed;
	GxMessage *msg;
	
	if(PLAY_MOVIE_SPEED_X1 == speed)
	{
		player_speed.speed = (float)speed;
	}
	else
	{
#if (TRICK_PLAY_SUPPORT > 0)
		if(PLAY_MOVIE_CTROL_BACKWARD== ctrol)
		{
			player_speed.speed = (0 - (float)speed);
		}
		else if(PLAY_MOVIE_CTROL_FORWARD== ctrol)
		{
			player_speed.speed = (float)speed;
		}
		else
#endif		
		{
			player_speed.speed = (float)(PLAY_MOVIE_SPEED_X1);
		}
	}

	printf("start speed %f\n", player_speed.speed);

	msg = GxBus_MessageNew(GXMSG_PLAYER_SPEED);
	APP_CHECK_P(msg, GXCORE_ERROR);
		
	GxMsgProperty_PlayerSpeed *spd;
	spd = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_PlayerSpeed);
	APP_CHECK_P(spd, GXCORE_ERROR);
	spd->player = PMP_PLAYER_AV;
	spd->speed = player_speed.speed;
	GxBus_MessageSendWait(msg);
	GxBus_MessageFree(msg);
	
	return GXCORE_SUCCESS;
}

status_t play_movie_zoom(play_movie_zoom_state zoom)
{
	return GXCORE_SUCCESS;
}


status_t play_movie_get_info(play_movie_info *info)
{
	play_list* list = NULL;

	list = play_list_get(PLAY_LIST_TYPE_MOVIE);
	if(NULL == list) return GXCORE_ERROR;

	// TODO:
	memset(info, 0, sizeof(play_movie_info));
	info->no = list->play_no;
	info->path = list->path;
	info->name = list->ents[list->play_no];

	return GXCORE_SUCCESS;
}





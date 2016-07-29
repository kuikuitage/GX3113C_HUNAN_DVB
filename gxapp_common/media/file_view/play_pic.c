//#include "app.h"
#include "play_manage.h"
#include "play_pic.h"
#include "gui_timer.h"
#include "gdi_play.h"
#include "stdlib.h"
#include "app_common_media.h"

event_list *pic_switch_timer = NULL;
static play_pic_ctrol_state s_play_pic_state = PLAY_PIC_CTROL_PAUSE;
status_t play_pic_get_info(play_pic_info *info);

static int  _auto_switch_pic(void *userdata)
{
	play_list* list = NULL;
	uint32_t file_count = 0;
	uint32_t random_no = 0;
	int32_t sequence = 0;

	sequence = pmpset_get_int(PMPSET_PIC_PLAY_SEQUENCE);
	
	list = play_list_get(PLAY_LIST_TYPE_PIC);
	if(NULL == list) return GXCORE_ERROR;

	file_count = list->nents;

	if(1 >= file_count)
	{
		s_play_pic_state = PLAY_PIC_CTROL_STOP;
		return GXCORE_ERROR;
	}

	switch(sequence)
	{
		case PMPSET_PIC_PLAY_SEQUENCE_SEQUENCE:
			play_pic_ctrol(PLAY_PIC_CTROL_NEXT);
			break;
			
		case PMPSET_PIC_PLAY_SEQUENCE_RANDOM:
			//srandom(file_count);
			random_no = rand();
			random_no = random_no%file_count;
			if(random_no == list->play_no)
			{
				random_no = (random_no+1)%file_count;
			}
			play_pic(random_no);
			break;

		default:
			printf("[PIC] auto_switch_pic sequence:%d not support\n", sequence);
			break;
	}
	
#define TEXT_PIC_NAME1	"pic_view_text_name1"
	play_pic_info info;
	play_pic_get_info(&info);
	if(NULL == &info) 
		return GXCORE_ERROR;
	GUI_SetProperty(TEXT_PIC_NAME1, "string", info.name);	
	
	return 0;	
}


status_t _auto_switch_start(void)
{
	uint32_t duration = 0;
	int32_t pic_switch_duration = 0;
	
	/*if(pic_switch_timer)
	{
		printf("[PIC] play_pic_auto_switch_start pic_switch_timer is already created\n");
		return GXCORE_ERROR;
	}*/

	pic_switch_duration = pmpset_get_int(PMPSET_PIC_SWITCH_DURATION);

	// TODO:  时间暂时推迟下，解码时间太长
	if(PMPSET_PIC_SWITCH_DURATION_3s == pic_switch_duration)duration = 3000;
	else if(PMPSET_PIC_SWITCH_DURATION_5s == pic_switch_duration)duration = 5000;
	else if(PMPSET_PIC_SWITCH_DURATION_7s == pic_switch_duration)duration = 7000;
	else if(PMPSET_PIC_SWITCH_DURATION_10s== pic_switch_duration)duration = 10000;

	if(0 != reset_timer(pic_switch_timer)) 
	{
		pic_switch_timer = create_timer(_auto_switch_pic, (int)duration, NULL, TIMER_REPEAT);
	}

	return GXCORE_SUCCESS;
}

status_t _auto_switch_stop(void)
{	
	if(pic_switch_timer)
	{
		remove_timer(pic_switch_timer);
		pic_switch_timer = NULL;
	}

	return GXCORE_SUCCESS;
}



status_t play_pic(int file_no)
{
	play_list* list = NULL;
	char* path = NULL;
	int ret = 0;
	PicPlayStatus play_status = PIC_PLAY_START;

	list = play_list_get(PLAY_LIST_TYPE_PIC);
	if(NULL == list) 
	{
		ret = GXCORE_ERROR;
		goto PIC_ERR;
	}

	if(file_no >= list->nents)
	{
		ret = GXCORE_ERROR;
		goto PIC_ERR;
		//return -1;
	}

	list->play_no = file_no;
	path = explorer_static_path_strcat(list->path, list->ents[file_no]);
	if(NULL == path) 
	{
		ret = GXCORE_ERROR;
		goto PIC_ERR;
	}

	printf("[PLAY] picture %s\n", path);
	
	pic_status_report(play_status);
	ret = GDI_PlayImage(path,-1,-1);

PIC_ERR:
	if(ret == 0)
		play_status = PIC_PLAY_RUNNING;
	else
		play_status = PIC_PLAY_ERROR;
	pic_status_report(play_status);

	//play_pic_ctrol(PLAY_PIC_CTROL_PLAY);

	return ret;
}


status_t play_pic_ctrol(play_pic_ctrol_state ctrol)
{
	status_t ret = GXCORE_ERROR;
	play_list* list = NULL;
	uint32_t file_no = 0;
	uint32_t file_count = 0;


	list = play_list_get(PLAY_LIST_TYPE_PIC);
	if(NULL == list) return GXCORE_ERROR;
	
	file_no = list->play_no;
	file_count = list->nents;
	if(0 == file_count)
	{
		s_play_pic_state = PLAY_PIC_CTROL_STOP;
		return GXCORE_ERROR;
	}


	if(PLAY_PIC_CTROL_PREVIOUS == ctrol)
	{
		if(1 >= file_count) 
		{
			s_play_pic_state = PLAY_PIC_CTROL_STOP;
			return GXCORE_SUCCESS;
		}

		if(file_no <=0)
		{
			file_no = file_count  - 1;
		}
		else
		{
			file_no--;
		}
		ret = play_pic(file_no);
		s_play_pic_state = PLAY_PIC_CTROL_PREVIOUS;
	}
	else if(PLAY_PIC_CTROL_NEXT == ctrol)
	{
		if(1 >= file_count)
		{
			s_play_pic_state = PLAY_PIC_CTROL_STOP;
			return GXCORE_SUCCESS;
		}
		
		if(file_no >= file_count -1)
		{
			file_no = 0;
		}
		else
		{
			file_no++;
		}
		ret = play_pic(file_no);
		s_play_pic_state = PLAY_PIC_CTROL_NEXT;
	}
	else if(PLAY_PIC_CTROL_PLAY == ctrol)
	{
		s_play_pic_state = PLAY_PIC_CTROL_PLAY;
		ret = _auto_switch_start();
	}
	else if(PLAY_PIC_CTROL_PAUSE == ctrol)
	{
		s_play_pic_state = PLAY_PIC_CTROL_PAUSE;
		ret = _auto_switch_stop();
	}
	else if(PLAY_PIC_CTROL_STOP == ctrol)
	{
		s_play_pic_state = PLAY_PIC_CTROL_STOP;
		ret = _auto_switch_stop();
	}
	
	return ret;	
}


status_t play_pic_rotate(play_pic_rotate_state rotate)
{
	return GXCORE_SUCCESS;
}
status_t play_pic_zoom(play_pic_zoom_state zoom)
{
	return GXCORE_SUCCESS;
}

status_t play_pic_get_info(play_pic_info *info)
{
	play_list* list = NULL;
	char* path = NULL;


	list = play_list_get(PLAY_LIST_TYPE_PIC);
	if(NULL == list) return GXCORE_ERROR;
	
	memset(info, 0, sizeof(play_pic_info));
	
	info->path = list->path;
	info->name = list->ents[list->play_no];

	/*from gdi*/
	path = explorer_static_path_strcat(info->path, info->name);
	if(NULL == path) return GXCORE_ERROR;
	GDI_ImageInformation((char*)path, &info->exif);

	extern ImageInfo_t ImageInfo;
	extern void ProcessFile(const char * FileName);
	ProcessFile(path);
	memcpy(&info->exif.image_ext_info, &ImageInfo, sizeof(info->exif.image_ext_info));
	
	return GXCORE_SUCCESS;
}

play_pic_ctrol_state play_pic_get_state(void)
{
	return s_play_pic_state;
}
	





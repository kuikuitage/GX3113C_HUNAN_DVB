#ifndef __PALY_MOVIE_H__
#define __PALY_MOVIE_H__

#include "gxtype.h"
#include "gxcore.h"
#include "gxapp_sys_config.h"
//#include "app.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	PLAY_MOVIE_CTROL_PREVIOUS,
#if (TRICK_PLAY_SUPPORT > 0)	
	PLAY_MOVIE_CTROL_BACKWARD,
#endif	
	PLAY_MOVIE_CTROL_PLAY,
	PLAY_MOVIE_CTROL_PAUSE,
	PLAY_MOVIE_CTROL_RESUME,
#if (TRICK_PLAY_SUPPORT > 0)	
	PLAY_MOVIE_CTROL_FORWARD,
#endif	
	PLAY_MOVIE_CTROL_NEXT,
	PLAY_MOVIE_CTROL_STOP,
	PLAY_MOVIE_CTROL_ZOOM,
	PLAY_MOVIE_CTROL_RANDOM
}play_movie_ctrol_state;

typedef enum
{
	PLAY_MOVIE_SPEED_X1 = 1,
	PLAY_MOVIE_SPEED_X2 = 2,
	PLAY_MOVIE_SPEED_X4 = 4,
	PLAY_MOVIE_SPEED_X8 = 8,
	PLAY_MOVIE_SPEED_X16 = 16,
	PLAY_MOVIE_SPEED_X32 = 32,
}play_movie_speed_state;

typedef enum
{
	PLAY_MOVIE_ZOOM_X1,
	PLAY_MOVIE_ZOOM_X2,
	PLAY_MOVIE_ZOOM_X4,
	PLAY_MOVIE_ZOOM_X8,
	PLAY_MOVIE_ZOOM_X16
}play_movie_zoom_state;

typedef struct
{
	uint32_t no;
	char* path;
	char* name;
	
}play_movie_info;


status_t play_movie(uint32_t file_no, int start_time_ms);
status_t play_movie_ctrol(play_movie_ctrol_state ctrol);
status_t play_movie_speed(play_movie_ctrol_state ctrol, play_movie_speed_state speed);
status_t play_movie_zoom(play_movie_zoom_state zoom);
status_t play_movie_get_info(play_movie_info *info);



#ifdef __cplusplus
}
#endif

#endif /* __PALY_MOVIE_H__ */


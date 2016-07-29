#ifndef __PLAY_MUSIC_H__
#define __PLAY_MUSIC_H__

#include "gxtype.h"
#include "gxcore.h"
#include "file_view.h"
#include "gxbus.h"
#include  "gxplayer_module.h"
#include "media_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	PLAY_MUSIC_CTROL_PREVIOUS,
	PLAY_MUSIC_CTROL_BACKWARD,
	PLAY_MUSIC_CTROL_PLAY,
	PLAY_MUSIC_CTROL_PAUSE,
	PLAY_MUSIC_CTROL_RESUME,
	PLAY_MUSIC_CTROL_FORWARD,	
	PLAY_MUSIC_CTROL_NEXT,
	PLAY_MUSIC_CTROL_STOP,
	PLAY_MUSIC_CTROL_RANDOM
}play_music_ctrol_state;

typedef enum
{
	PLAY_MUSIC_SPEED_X0,
	PLAY_MUSIC_SPEED_X1,
	PLAY_MUSIC_SPEED_X2,
	PLAY_MUSIC_SPEED_X4,
	PLAY_MUSIC_SPEED_X8
}play_music_speed_state;

typedef struct
{
	uint32_t no;
	char* path;
	char* name;
	
}play_music_info;

#if 0//---
typedef struct tag  //mp3's ID3 v2 tag header
{
	char header[3];   //must be "ID3"
	char ver;
	char revision;
	char flag;
	char size[4];  //ID3v2 tag length(except tag header)
}ID3Tagheader;

typedef struct frame  //mp3's ID v2 tag's frame header
{
	char frameID[4];
	char size[4];   //frame's size(except frame header)
	char flags[2];
}ID3frameheader;
#endif//---add by zmq

typedef enum 
{
	ID3V2_TITLE,
	ID3V2_ARTIST,
	ID3V2_ALBUM,
	ID3V2_YEAR,
	ID3V2_TRACK,
	ID3V2_GENRE,
	ID3V2_TOTAL
}ID3V2Type_e;
#define ID3V2_INFO_LENGTH 50
#define ID3V1_GENRE_TYPE_NUM  116

status_t play_music(uint32_t file_no);
status_t play_music_ctrol(play_music_ctrol_state ctrol);
status_t play_music_speed(play_music_ctrol_state ctrol, play_music_speed_state speed);
status_t play_music_get_info(play_music_info *info);
PlayerID3Info* play_music_get_id3_info(void);
int play_music_file_no_bak(void);
void clean_music_file_no_bak(void);
#if 0
play_music_ctrol_state music_view_get_ctrol_state(void);
void music_view_set_ctrol_state(play_music_ctrol_state state);
status_t play_music_get_id3_image(const char *mp3_path, char* image_path, int index);
void play_music_delete_id3_image(char *path);
#endif
void clean_music_path_bak(void);
int  _parse_id3v2(ID3V2Type_e Type, PlayerID3V2* pRawData, char * pOutData, unsigned int MaxLenToOut);
void music_info_get(MediaInfo *info);
void music_info_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* __PLAY_MUSIC_H__ */


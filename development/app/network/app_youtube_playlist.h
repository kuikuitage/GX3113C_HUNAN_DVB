/*
 * =====================================================================================
 *
 *       Filename:  app_youtube_playlist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年08月22日 10时32分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __APP_YOUTUBE_PLAYLIST_H__
#define __APP_YOUTUBE_PLAYLIST_H__

//#include "app_config.h"
#include "parser.h"

#include "youtube_tools.h"

//-----------------------------------------------------------------------------------------
//-----video-------------------------------------------------------------------------------
struct youtube_play_item
{
	char* 	media_title;
	char* 	media_url;
	int		media_duration; //second
	char* 	author;
	int 	viewCount;
	char*	thumbnail_url;
	bool 	b_download_ok;
};

extern int app_iptv_playlist_load_from_file(const char *filename);
extern int app_youtube_playlist_load_from_file(const char *filename);
extern int app_youtube_playlist_cleanup(void);
extern int app_youtube_playlist_get_total(void);
extern struct youtube_play_item* app_youtube_playlist_get_by_index(int index);

extern int app_iptv_feeds_load_from_file(const char *filename);
extern int app_youtube_playlist_get_cur_page_total(void);
extern char* app_youtube_playlist_get_next_page_url(void);
extern char* app_youtube_playlist_get_prev_page_url(void);



//-----------------------------------------------------------------------------------------
//-----feeds-------------------------------------------------------------------------------
typedef enum youtube_feed_type
{
	FEED_TYPE_STANDARD = 0,
	FEED_TYPE_CATEGORIES,
	FEED_TYPE_USER,
}FEED_TYPE;

struct youtube_feeds_item
{
	char * p_title;
	char * p_url;
	FEED_TYPE feed_type;
};

extern int app_youtube_feeds_load_from_file(const char *filename);
extern int app_youtube_feeds_cleanup(void);
extern int app_youtube_feeds_get_total(void);
extern struct youtube_feeds_item* app_youtube_feeds_get_by_index(int index);
#endif

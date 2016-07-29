/*
 * =====================================================================================
 *
 *       Filename:  app_youtube_playlist.c
 *
 *    Description:  youtube playlist
 *
 *        Version:  1.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
//#include "app_config.h"
#include "app_youtube_playlist.h"
//#include <stdio.h>
#include <stdlib.h>

#define BUF_FREE(x) if(x){free(x);x=NULL;}


#define MAX_READ_BUF 1024
char read_buf[MAX_READ_BUF];


//-----------------------------------------------------------------------------------------
//-----video-------------------------------------------------------------------------------
#define YOUTUBE_VIDEO

static struct youtube_play_item* s_ppYoutube_Playlist = NULL;
static int	s_iPlaylist_Count;
static int  s_iTotal_Count;
static char* s_playlist_href_next = NULL;
static char* s_playlist_href_prev = NULL;

#define MAX_NUM_YOUTUBE_ITEM 6
#define MAX_NUM_IPTV_ITEM 500

int app_youtube_playlist_load_from_file(const char *filename)
{
	#define MAX_TITLE_LEN 32

	int i, len, total_count;
	FILE *fp = NULL;
	
	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		printf("[Youtube] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		printf("[Youtube] error, open %s failed!!!\n", filename);
		return -1;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_playlist_href_prev = malloc(len+1);
		memcpy(s_playlist_href_prev, read_buf, len+1);
	}
	else
	{
		s_playlist_href_prev = NULL;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_playlist_href_next = malloc(len+1);
		memcpy(s_playlist_href_next, read_buf, len+1);
	}
	else
	{
		s_playlist_href_next = NULL;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		//s_iTotal_Count = atoi(read_buf);
		total_count = atoi(read_buf);
	}
	else
	{
		//s_iTotal_Count = 0;
		total_count = 0;
	}
	

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_iPlaylist_Count = atoi(read_buf);
		if(s_iPlaylist_Count > MAX_NUM_YOUTUBE_ITEM)
			s_iPlaylist_Count = MAX_NUM_YOUTUBE_ITEM;
	}
	else
	{
		s_iPlaylist_Count = 0;
	}

	if(s_iPlaylist_Count > 0)
	{
		s_ppYoutube_Playlist = malloc(s_iPlaylist_Count*sizeof(struct youtube_play_item));
		memset(s_ppYoutube_Playlist, 0, s_iPlaylist_Count*sizeof(struct youtube_play_item));
	}
	else
	{
		fclose(fp);
		return -1;
	}

	if(feof(fp))
	{
		s_iPlaylist_Count = 0;
		fclose(fp);
		return -1;
	}

	s_iTotal_Count = total_count;

	for(i = 0; i<s_iPlaylist_Count; i++)
	{
		s_ppYoutube_Playlist[i].b_download_ok = false;

		//title
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].media_title = malloc(MAX_TITLE_LEN);
			memcpy(s_ppYoutube_Playlist[i].media_title, read_buf, MAX_TITLE_LEN);
			if(len>MAX_TITLE_LEN)
			{
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-1] = '\0';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-2] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-3] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-4] = '.';
			}
		}
		else
		{
			s_ppYoutube_Playlist[i].media_title = NULL;
		}

		//author
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			#if 0
			s_ppYoutube_Playlist[i].author = malloc(len+1);
			memcpy(s_ppYoutube_Playlist[i].author, read_buf, len+1);
			#else
			s_ppYoutube_Playlist[i].author = malloc(MAX_TITLE_LEN);
			memcpy(s_ppYoutube_Playlist[i].author, read_buf, MAX_TITLE_LEN);
			if(len>MAX_TITLE_LEN)
			{
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-1] = '\0';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-2] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-3] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-4] = '.';
			}
			#endif
		}
		else
		{
			s_ppYoutube_Playlist[i].author = NULL;
		}

		//videoid
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].media_url = malloc(len+1);
			memcpy(s_ppYoutube_Playlist[i].media_url, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Playlist[i].media_url = NULL;
		}

		//img path
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].thumbnail_url = malloc(len+1);
			memcpy(s_ppYoutube_Playlist[i].thumbnail_url, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Playlist[i].thumbnail_url = NULL;
		}
		
		//duration
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].media_duration = atoi(read_buf);
		}
		else
		{
			s_ppYoutube_Playlist[i].media_duration = 0;
		}

		//viewcount
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].viewCount = atoi(read_buf);
		}
		else
		{
			s_ppYoutube_Playlist[i].viewCount = 0;
		}
	}

	fclose(fp);
	
	return 0;
}


int app_iptv_playlist_load_from_file(const char *filename)
{
	#define MAX_TITLE_LEN 32

	int i, len;
	FILE *fp = NULL;
	
	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		printf("[Youtube] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		printf("[Youtube] error, open %s failed!!!\n", filename);
		return -1;
	}

	//previous
	s_playlist_href_prev = NULL;

	//next
	s_playlist_href_next = NULL;

	//total
	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_iTotal_Count = atoi(read_buf);
		if(s_iTotal_Count > MAX_NUM_IPTV_ITEM)
			s_iTotal_Count = MAX_NUM_IPTV_ITEM;
	}
	else
	{
		s_iTotal_Count = 0;
	}
	
	s_iPlaylist_Count = s_iTotal_Count;

	//malloc the buf to restore playlist.
	if(s_iPlaylist_Count > 0)
	{
		s_ppYoutube_Playlist = malloc(s_iPlaylist_Count*sizeof(struct youtube_play_item));
		memset(s_ppYoutube_Playlist, 0, s_iPlaylist_Count*sizeof(struct youtube_play_item));
	}
	else
	{
		fclose(fp);
		return -1;
	}

	for(i = 0; i<s_iPlaylist_Count; i++)
	{
		s_ppYoutube_Playlist[i].b_download_ok = false;

		//title
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].media_title = malloc(MAX_TITLE_LEN);
			memcpy(s_ppYoutube_Playlist[i].media_title, read_buf, MAX_TITLE_LEN);
			if(len>MAX_TITLE_LEN)
			{
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-1] = '\0';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-2] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-3] = '.';
				s_ppYoutube_Playlist[i].media_title[MAX_TITLE_LEN-4] = '.';
			}
		}
		else
		{
			s_ppYoutube_Playlist[i].media_title = NULL;
		}

		//media url
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].media_url = malloc(len+1);
			memcpy(s_ppYoutube_Playlist[i].media_url, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Playlist[i].media_url = NULL;
		}

		//img path
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Playlist[i].thumbnail_url = malloc(len+1);
			memcpy(s_ppYoutube_Playlist[i].thumbnail_url, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Playlist[i].thumbnail_url = NULL;
		}

		s_ppYoutube_Playlist[i].author = NULL;
		s_ppYoutube_Playlist[i].media_duration = 0;
		s_ppYoutube_Playlist[i].viewCount = 0;
	}

	fclose(fp);
	
	return 0;
}


int app_youtube_playlist_cleanup(void)
{
	int i = 0;
	
	if(s_ppYoutube_Playlist)
	{
		for(i=0; i<s_iPlaylist_Count; i++)
		{
			//printf("[Youtube] cleanup %d start...\n", i);
			//printf("	author:%s\n", s_ppYoutube_Playlist[i].author);
			BUF_FREE(s_ppYoutube_Playlist[i].author);
			BUF_FREE(s_ppYoutube_Playlist[i].media_title);
			BUF_FREE(s_ppYoutube_Playlist[i].media_url);
			BUF_FREE(s_ppYoutube_Playlist[i].thumbnail_url);
			//printf("[Youtube] ......cleanup %d end\n", i);
		}
		s_iPlaylist_Count = 0;
	
		BUF_FREE(s_ppYoutube_Playlist);
	}

	BUF_FREE(s_playlist_href_next);
	BUF_FREE(s_playlist_href_prev);
	
	return 0;
}

int app_youtube_playlist_get_total(void)
{
	return s_iTotal_Count;
}

int app_youtube_playlist_get_cur_page_total(void)
{
	return s_iPlaylist_Count;
}

char* app_youtube_playlist_get_next_page_url(void)
{
	return s_playlist_href_next;
}

char* app_youtube_playlist_get_prev_page_url(void)
{
	return s_playlist_href_prev;
}


struct youtube_play_item* app_youtube_playlist_get_by_index(int index)
{
	if(NULL != s_ppYoutube_Playlist && s_iPlaylist_Count > 0 && index <= s_iPlaylist_Count)
	{
		return &s_ppYoutube_Playlist[index-1];
	}
	else
	{
		return NULL;
	}
}


//-----------------------------------------------------------------------------------------
//-----feeds-------------------------------------------------------------------------------
#define YOUTUBE_FEEDS

static struct youtube_feeds_item* s_ppYoutube_Feeds = NULL;
static int	s_iFeeds_Count = 0;

#define MAX_NUM_IPTV_FEEDS 50
#define MAX_NUM_YOUTUBE_FEEDS 50

int app_youtube_feeds_load_from_file(const char *filename)
{
	int i, len;
	FILE *fp = NULL;

	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		printf("[Youtube] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		printf("[Youtube] error, open %s failed!!!\n", filename);
		return -1;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_iFeeds_Count = atoi(read_buf);
		if(s_iFeeds_Count > MAX_NUM_YOUTUBE_FEEDS)
			s_iFeeds_Count = MAX_NUM_YOUTUBE_FEEDS;
	}
	else
	{
		s_iFeeds_Count = 0;
	}

	if(s_iFeeds_Count > 0)
	{
		s_ppYoutube_Feeds = malloc(s_iFeeds_Count*sizeof(struct youtube_feeds_item));
	}
	else
	{
		s_ppYoutube_Feeds = NULL;
		fclose(fp);
		return -1;
	}

	for(i=0; i<s_iFeeds_Count; i++)
	{
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Feeds[i].p_title = malloc(len+1);
			memcpy(s_ppYoutube_Feeds[i].p_title, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Feeds[i].p_title = NULL;
		}


		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Feeds[i].p_url = malloc(len+1);
			memcpy(s_ppYoutube_Feeds[i].p_url, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Feeds[i].p_url = NULL;
		}

		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Feeds[i].feed_type = atoi(read_buf);
		}
		else
		{
			s_ppYoutube_Feeds[i].feed_type = 0;
		}
	}

	fclose(fp);
	
	return 0;
}


int app_iptv_feeds_load_from_file(const char *filename)
{
	int i, len;
	FILE *fp = NULL;

	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		printf("[IPTV] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		printf("[IPTV] error, open %s failed!!!\n", filename);
		return -1;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_iFeeds_Count = atoi(read_buf);
		if(s_iFeeds_Count > MAX_NUM_IPTV_FEEDS)
			s_iFeeds_Count = MAX_NUM_IPTV_FEEDS;
	}
	else
	{
		s_iFeeds_Count = 0;
	}

	if(s_iFeeds_Count > 0)
	{
		s_ppYoutube_Feeds = malloc(s_iFeeds_Count*sizeof(struct youtube_feeds_item));
	}
	else
	{
		s_ppYoutube_Feeds = NULL;
		fclose(fp);
		return -1;
	}

	for(i=0; i<s_iFeeds_Count; i++)
	{
		//title
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			s_ppYoutube_Feeds[i].p_title = malloc(len+1);
			memcpy(s_ppYoutube_Feeds[i].p_title, read_buf, len+1);
		}
		else
		{
			s_ppYoutube_Feeds[i].p_title = NULL;
		}

		//url
		s_ppYoutube_Feeds[i].p_url = NULL;

		//feedtype
		s_ppYoutube_Feeds[i].feed_type = 0;
	}

	fclose(fp);
	
	return 0;
}

int app_youtube_feeds_cleanup(void)
{
	#if 1
	int i = 0;
	
	if(s_ppYoutube_Feeds)
	{
		for(i=0; i<s_iFeeds_Count; i++)
		{
			BUF_FREE(s_ppYoutube_Feeds[i].p_title);
			BUF_FREE(s_ppYoutube_Feeds[i].p_url);
		}
		s_iFeeds_Count = 0;
	
		BUF_FREE(s_ppYoutube_Feeds);
	}
	#endif
	
	return 0;
}

int app_youtube_feeds_get_total(void)
{
	return s_iFeeds_Count;
}

struct youtube_feeds_item* app_youtube_feeds_get_by_index(int index)
{
	if(NULL != s_ppYoutube_Feeds && s_iFeeds_Count > 0 && index < s_iFeeds_Count)
	{
		return &s_ppYoutube_Feeds[index];
	}
	else
	{
		return NULL;
	}
}



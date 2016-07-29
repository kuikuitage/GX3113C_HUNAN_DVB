/*
 * =====================================================================================
 *
 *       Filename:  youtube_tools.c
 *
 *    Description:  youtube tools
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
#ifndef __YOUTUBE_TOOLS_H__
#define __YOUTUBE_TOOLS_H__
//#include "app_config.h"

typedef enum
{
	WND_TYPE_YOUTUBE = 0,
	WND_TYPE_IPTV,
	WND_TYPE_REDTUBE,
	WND_TYPE_YOUPORN
}WND_TYPE;

typedef void (*SYSTEM_SHELL_PROC)(void* userdata);

extern int curl_http_download(const char* s_url, const char* s_out, const char* s_time_out, SYSTEM_SHELL_PROC proc);
extern int get_youtube_flv_url(char* str_dst, char* media_id, WND_TYPE wnd_type);
extern int system_shell(const char* s_cmd, int time_out, SYSTEM_SHELL_PROC step_proc, SYSTEM_SHELL_PROC finsh_proc, void* userdata);
extern char * fgets_own(char * s,int* size,FILE * stream);

extern int system_shell_clean(void);


#define MAX_READ_BUF 1024
extern char read_buf[MAX_READ_BUF];
#endif

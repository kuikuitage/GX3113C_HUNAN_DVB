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

//#include "app_config.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include <stdio.h>
#include <gxcore.h>

#include <signal.h>
#include "gui_timer.h"

#include "youtube_tools.h"

extern status_t GUI_Exec(void);
static int system_result;
static size_t f_result;

#if 1
int curl_http_download(const char* s_url, const char* s_out, const char* s_time_out, SYSTEM_SHELL_PROC proc)
{
	pid_t pid;
    int status;

	//proc = NULL;

//	printf("###curl [%s]\n", s_url);
    printf("###wget [%s]\n", s_url);


    if(s_url == NULL || s_out == NULL){
         return (1);
    }

    if((pid = fork())<0){
		status = -1;
    }
    else if(pid == 0){
		#if 0
		int i = 0;
		if(NULL != proc)
		{
			for(i=0; i<3; i++)
				proc();
		}
		#endif
		//execl("/sbin/curl", "curl", "-m", s_time_out, "-o", s_out, s_url, (char *)0);
		execl("/bin/wget", "wget", "-T", s_time_out, "-O", s_out, s_url, (char *)0);
        exit(127); //子进程正常执行则不会执行此语句
    }
    else
	{
		if(NULL == proc)
		{
			while(waitpid(pid, &status, 0) < 0){
                if(errno != EINTR){
                    status = -1;
                    break;
                }
            }
		}
		else
		{
			while(waitpid(pid, &status, WNOHANG) == 0){
				proc(NULL);
				usleep(50000);
            }
		}
	}
	return status;
}
#else
int curl_http_download(const char* s_url, const char* s_out, SYSTEM_SHELL_PROC proc)
{
	static char str_cmd[256];
	sprintf(str_cmd, "youtube_get_pic_proxy \"%s\" %s", s_url, s_out);
	system_shell(str_cmd, 0, proc, NULL, NULL);

	return 0;
}
#endif


int get_youtube_flv_url(char* str_dst, char* media_id, WND_TYPE wnd_type)
{
#define YOUTUBE_PATH_FLV_PLAY "/tmp/youtube/youtube_flv_url"

	FILE *fp = NULL;
	int len;

	//sprintf(str_dst, "http://www.youtube.com/get_video_info?video_id=%s", media_id);
	//curl_http_download(str_dst, YOUTUBE_PATH_FLV_PLAY, NULL);

	if(WND_TYPE_YOUTUBE == wnd_type)
	{
		sprintf(str_dst, "youtube_get_play_url %s %s", media_id, YOUTUBE_PATH_FLV_PLAY);
	}
	else if(WND_TYPE_REDTUBE == wnd_type)
	{
		sprintf(str_dst, "redtube_get_play_url %s %s", media_id, YOUTUBE_PATH_FLV_PLAY);
	}
	else if(WND_TYPE_YOUPORN == wnd_type)
	{
		sprintf(str_dst, "youporn_get_play_url %s %s", media_id, YOUTUBE_PATH_FLV_PLAY);
	}
		
	
	system_result = system(str_dst);

	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(YOUTUBE_PATH_FLV_PLAY))
	{
		printf("[Youtube] file [%s] unexist!!!\n", YOUTUBE_PATH_FLV_PLAY);
		return -2;
	}

	fp = fopen(YOUTUBE_PATH_FLV_PLAY, "r");
	if(NULL == fp)
	{
		printf("[Youtube] get video info, open %s failed!!!\n", YOUTUBE_PATH_FLV_PLAY);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
   	len = ftell(fp);
	rewind(fp);
	f_result = fread(str_dst, 1, len, fp);
	str_dst[len] = '\0';

	fclose(fp);
	
	return 0;
}

#if 0
int system_shell(const char* s_cmd, DOWNLOAD_STEP_PROC proc)
{
	pid_t pid;
    int status;

	//proc = NULL;

	printf("###system_shell: %s\n", s_cmd);

    if(s_cmd == NULL){
         return (1);
    }

    if((pid = fork())<0){
		status = -1;
    }
    else if(pid == 0){
		//execl("/usr/bin/curl", "curl", "-m", "60", "-o", s_out, s_url, (char *)0);
		execl("/bin/sh", "sh", "-c", s_cmd, (char *)0);
        exit(127); //子进程正常执行则不会执行此语句
    }
    else
	{
		if(NULL == proc)
		{
			while(waitpid(pid, &status, 0) < 0){
                if(errno != EINTR){
                    status = -1;
                    break;
                }
            }
		}
		else
		{
			while(waitpid(pid, &status, WNOHANG) == 0){
				proc();
                //usleep(50);
                //GUI_Exec();
            }
		}
	}
	return status;
}
#else
#if 0
struct shell_call_param
{
	pid_t pid;
	event_list* p_Timer;
	SYSTEM_SHELL_PROC step_proc;
	SYSTEM_SHELL_PROC finsh_proc;
	void* userdata;
	int time_out;
};

static struct shell_call_param* s_plast_call_param = NULL;

int g_system_shell_ret_status = 0;
static int system_shell_timeout(void *userdata)
{
	struct shell_call_param* p_call_param = NULL;

	p_call_param = (struct shell_call_param*)userdata;

	p_call_param->time_out--;
	if(p_call_param->time_out > 0)
	{
		if(waitpid(p_call_param->pid, &g_system_shell_ret_status, WNOHANG) == 0)
		{
			if(p_call_param->step_proc)
				p_call_param->step_proc(p_call_param->userdata);
		}
		else
		{
			s_plast_call_param = NULL;
			if(p_call_param->finsh_proc)
				p_call_param->finsh_proc(p_call_param->userdata);
			
			remove_timer(p_call_param->p_Timer);
			free(p_call_param);
		}
	}
	else
	{
		s_plast_call_param = NULL;
		kill(p_call_param->pid, SIGKILL);

		if(p_call_param->finsh_proc)
				p_call_param->finsh_proc(p_call_param->userdata);
			
		remove_timer(p_call_param->p_Timer);
		free(p_call_param);
	}
	

	return 0;
}

int system_shell_clean(void)
{
	if(NULL != s_plast_call_param)
	{
		remove_timer(s_plast_call_param->p_Timer);
		kill(s_plast_call_param->pid, SIGKILL);
		free(s_plast_call_param);
		s_plast_call_param = NULL;
	}
	return 0;
}

int system_shell(const char* s_cmd, int time_out, SYSTEM_SHELL_PROC step_proc, SYSTEM_SHELL_PROC finsh_proc, void* userdata)
{
	pid_t pid;
    int status;
	struct shell_call_param* p_call_param = NULL;

	printf("###system_shell: %s\n", s_cmd);

    if(s_cmd == NULL){
         return (1);
    }

	if(NULL != s_plast_call_param)
	{
		remove_timer(s_plast_call_param->p_Timer);
		kill(s_plast_call_param->pid, SIGKILL);
		free(s_plast_call_param);
		s_plast_call_param = NULL;
	}

    if((pid = fork())<0){
		status = -1;
    }
    else if(pid == 0){
		//execl("/usr/bin/curl", "curl", "-m", "60", "-o", s_out, s_url, (char *)0);
		execl("/bin/sh", "sh", "-c", s_cmd, (char *)0);
        exit(127); //子进程正常执行则不会执行此语句
    }
    else
	{
		if(0 == time_out)
		{
			if(NULL == step_proc)
			{
				while(waitpid(pid, &status, 0) < 0){
	                if(errno != EINTR){
	                    status = -1;
	                    break;
	                }
	            }
			}
			else
			{
				while(waitpid(pid, &status, WNOHANG) == 0){
					step_proc(userdata);
					usleep(50000);
	            }

				if(NULL != finsh_proc)
					finsh_proc(userdata);
			}
		}
		else
		{
			p_call_param = malloc(sizeof(struct shell_call_param));
			if(NULL == p_call_param)
			{
				s_plast_call_param = NULL;
				
				printf("!!!!error, system_shell malloc p_call_param failed!");
				while(waitpid(pid, &status, 0) < 0){
	                if(errno != EINTR){
	                    status = -1;
	                    break;
	                }
	            }
				if(NULL != finsh_proc)
					finsh_proc(userdata);
				return status;
			}

			p_call_param->pid = pid;
			p_call_param->time_out = time_out/100;
			p_call_param->step_proc = step_proc;
			p_call_param->finsh_proc = finsh_proc;
			p_call_param->userdata = userdata;

			s_plast_call_param = p_call_param;
			
			p_call_param->p_Timer = create_timer(system_shell_timeout, 100, (void *)p_call_param, TIMER_REPEAT);
		}
	}
	return status;
}
#endif
#endif


char * fgets_own(char * s,int* size,FILE * stream)
{
	char* pret = NULL, *p=NULL;
	pret = fgets(s, *size, stream);
	if(NULL == pret)
	{
		s[0] = '\0';
		*size = 0;
		return NULL;
	}
	else
	{
		*size = strlen(s);
		p = s;
		p += *size;
		p --;
		while ( (*p=='\n' || *p=='\r') && p>=s)
		{
			*p = '\0';
			p--;
			*size -= 1;
		}
	}
	
	return pret;
}


#if 0
int system_own(const char * cmdstring)
{
    pid_t pid;
    int status;

	printf("system_own [%s]", cmdstring);

    if(cmdstring == NULL){
         return (1);
    }


	printf("111111111\n");
    if((pid = fork())<0)
	{
		status = -1;
    }
    else if(pid == 0){

		printf("++\n");

		#if 1
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        #else
        execl("/usr/bin/curl", "curl", "-o", 
        		"/tmp/rss_youtube_playlist.xml",
        		"http://gdata.youtube.com/feeds/api/standardfeeds/most_viewed?alt=rss",
        		(char *)0);
		#endif
		
        exit(127); //子进程正常执行则不会执行此语句

    }
    else{
			printf("222222222\n");

			int setp = 100;

			#if 1
            while(waitpid(pid, &status, 0) < 0){

                if(errno != EINTR){

                    status = -1;

                    break;

                }

				printf("-");

            }
			#else
			while(waitpid(pid, &status, WNOHANG) == 0){

				app_youtube_print_to_osd(setp);

				setp ++;

                usleep(50);

				//printf("-");

            }
			#endif
	}

	printf("3333333333\n");
	return status;
}
#endif

#ifdef LINUX_OS
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "gui_timer.h"
extern int g_system_shell_ret_status;
typedef void (*SYSTEM_SHELL_PROC)(void* userdata);
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

static int system_shell_timeout(void *userdata)
{
    struct shell_call_param* p_call_param = NULL;

    p_call_param = (struct shell_call_param*)userdata;

	if((p_call_param != s_plast_call_param) || (s_plast_call_param == NULL))
	{
		printf("time param error p_call=%p,plast_call=%p\n",p_call_param,s_plast_call_param);
		return 0;
	}

    p_call_param->time_out--;
    if(p_call_param->time_out > 0)
    {
	//WNOHANG pid子进程即使未结束也返回0
	if(waitpid(p_call_param->pid, &g_system_shell_ret_status, WNOHANG) == 0)
	{
	    if(p_call_param->step_proc)
		p_call_param->step_proc(p_call_param->userdata);
	}
	else
	{
	    s_plast_call_param = NULL;
		remove_timer(p_call_param->p_Timer);
	    if(p_call_param->finsh_proc)
		p_call_param->finsh_proc(p_call_param->userdata);
	    
	    
	    GxCore_Free(p_call_param);
	}
    }
    else
    {
		s_plast_call_param = NULL;
		kill(p_call_param->pid, SIGKILL);
		remove_timer(p_call_param->p_Timer);

	if(p_call_param->finsh_proc)
		p_call_param->finsh_proc(p_call_param->userdata);
	    
		
		GxCore_Free(p_call_param);
    }
    

    return 0;
}
int system_shell_clean(void)
{
    if(NULL != s_plast_call_param)
    {
	remove_timer(s_plast_call_param->p_Timer);
	kill(s_plast_call_param->pid, SIGKILL);
	GxCore_Free(s_plast_call_param);
	s_plast_call_param = NULL;
    }
    return 0;
}

int system_shell(const char* s_cmd, int time_out, SYSTEM_SHELL_PROC step_proc, SYSTEM_SHELL_PROC finsh_proc, void* userdata)
{
    pid_t pid;
    int status = 0;
    struct shell_call_param* p_call_param = NULL;

    printf("###system_shell: %s\n", s_cmd);

    if(s_cmd == NULL){
         return (1);
    }

    if(NULL != s_plast_call_param)
    {
    printf("remove timer\n");
	remove_timer(s_plast_call_param->p_Timer);
	kill(s_plast_call_param->pid, SIGKILL);
	GxCore_Free(s_plast_call_param);
	s_plast_call_param = NULL;
    }

    if((pid = fork())<0){
	status = -1;
    }
    else if(pid == 0){
	//execl("/usr/bin/curl", "curl", "-m", "60", "-o", s_out, s_url, (char *)0);
	execl("/bin/sh", "sh", "-c", s_cmd, (char *)0);
        exit(127);
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
	    p_call_param = GxCore_Malloc(sizeof(struct shell_call_param));
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

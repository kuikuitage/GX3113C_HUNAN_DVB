/*
*  此文件接口实现CA移植操作系统公共接口
*/
#include "app_common_porting_ca_os.h"
#include <gxcore.h>


uint8_t	app_porting_ca_os_sleep(uint32_t wMilliSeconds)
{
	GxTime stoptime={0};
	GxTime starttime={0};
	uint32_t startMs,endMs;
	if (wMilliSeconds >= 100)
		{
			GxCore_GetTickTime(&starttime);
			startMs = starttime.seconds*1000 + starttime.microsecs/1000;
			while(1)
				{
					GxCore_GetTickTime(&stoptime);
					endMs = stoptime.seconds*1000 + stoptime.microsecs/1000;
					if (endMs - startMs >= wMilliSeconds)
						{
							return 0;
						}
					GxCore_ThreadDelay(20);	
				}
		}
	else
		{
 			   GxCore_ThreadDelay(wMilliSeconds);		
		}
	return 0;

}

 uint8_t   app_porting_ca_os_sem_create(handle_t *semaphore,uint32_t bInitVal)
{	
	if (NULL == semaphore)
	{
	    return 0;
	}
	
    GxCore_SemCreate(semaphore, bInitVal);
	return 0;
}

uint8_t   app_porting_ca_os_sem_delete(handle_t semaphore)
{	
	
    GxCore_SemDelete(semaphore);
	return 0;
}


uint8_t app_porting_ca_os_sem_signal(handle_t semaphore)
{

	    GxCore_SemPost(semaphore);
		return 0;
}

uint8_t app_porting_ca_os_sem_wait(handle_t semaphore)
{

	GxCore_SemWait(semaphore);
	return 0;
}

uint8_t app_porting_ca_os_sem_timeout_wait(handle_t semaphore,uint32_t ms)
{
	GxCore_SemTimedWait(semaphore,ms);
	return 0;	
}

int32_t app_porting_ca_os_create_task(const char *thread_name, handle_t *thread_id,
                                      void(*entry_func)(void *), void *arg, 
                                      uint32_t stack_size,
                                      uint32_t priority)

{
	if ((NULL == thread_name)||(NULL == thread_id)||(NULL == entry_func))
		return 0;

	return GxCore_ThreadCreate(thread_name, thread_id, (void*)entry_func,
                              arg, stack_size, priority);	
}

void *app_porting_ca_os_malloc(size_t size)
{
	return GxCore_Malloc(size);
}

void app_porting_ca_os_free(void *ptr)
{
	if (NULL != ptr)
		{
			GxCore_Free(ptr);
			ptr = NULL;		
		}
}







/**
 *
 * @file        app_common_porting_ca_os.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:33:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_CA_OS_H__
#define __APP_COMMON_PORTING_CA_OS_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>

uint8_t app_porting_ca_os_sleep(uint32_t wMilliSeconds);
 uint8_t  app_porting_ca_os_sem_create(handle_t *semaphore,uint32_t bInitVal);
   uint8_t   app_porting_ca_os_sem_delete(handle_t semaphore);
 uint8_t app_porting_ca_os_sem_signal(handle_t semaphore);
 uint8_t app_porting_ca_os_sem_wait(handle_t semaphore);
 uint8_t app_porting_ca_os_sem_timeout_wait(handle_t semaphore,uint32_t ms);
 int32_t app_porting_ca_os_create_task(const char *thread_name, handle_t *thread_id,
                                      void(*entry_func)(void *), void *arg, 
                                      uint32_t stack_size,
                                      uint32_t priority);
 void *app_porting_ca_os_malloc(size_t size);
 void app_porting_ca_os_free(void *ptr);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_CA_OS_H__*/


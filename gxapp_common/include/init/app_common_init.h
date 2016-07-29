/**
 *
 * @file        app_common_init.c
 * @brief
 * @version     1.1.0
 * @date        10/25/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_INIT__H__
#define __APP_COMMON_INIT__H__
#ifdef __cplusplus
extern "C" {
#endif

#if defined(ECOS_OS)
#include <cyg/io/gx3110_irr.h>
#endif

#ifdef LINUX_OS
typedef struct lowpower_info_s
 {
     unsigned int WakeTime;
     unsigned int GpioMask;
     unsigned int GpioData;
     unsigned int key;
     char * cmdline;
}lowpower_info;
#define IRR_LOWPOWER 0x2000
#endif


/*
* 以下接口提供menu调用
*/

/*
* 初始化前端
*/
int32_t app_init_frontend(int32_t time);
int32_t app_init_demod_tuner(void);
int32_t app_init_panel(void); 
int32_t app_lower_power_entry(uint32_t key,uint32_t io_num,uint32_t io_status);

 

/*
* 系统参数初始化
*/
void app_sys_init(void);

/*重启模块*/
int restart(void);

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_INIT__H__*/


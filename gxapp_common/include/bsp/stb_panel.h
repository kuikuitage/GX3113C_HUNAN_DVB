/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	stb_panel.h
* Author    : 	DAD
* Project   :	GXSDK
* Type      :	Template
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   0.0  	2010.08.04	      DAD	         creation
            hello, please write revision information here.
*****************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __STB_PANEL_H__
#define  __STB_PANEL_H__

/* Includes --------------------------------------------------------------- */
#if defined(ECOS_OS)
#include <gxtype.h>
#include "app_key.h"
#include "gxcore_hw_bsp.h"
#include "gui_key.h"
#elif defined(LINUX_OS)
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
#endif
#include "panel_key.h"
#include "gxapp_sys_config.h"
#include "app_common_panel.h"


/************************************************************************/

#if defined(ECOS_OS)
#define PANEL_NAME "/dev/gxpanel0"
#ifndef _U8_
#define _U8_
typedef unsigned char u8;
#endif
#ifndef _S8_
#define _S8_
typedef signed char s8;
#endif
#ifndef _S16_
#define _S16_
typedef signed short s16;
#endif
#ifndef _U16_
#define _U16_
typedef unsigned short u16;
#endif
#ifndef _S32_
#define _S32_
typedef signed int s32;
#endif
#ifndef _U32_
#define _U32_
typedef unsigned int u32;
#endif
#ifndef _U64_
#define _U64_
typedef unsigned long long u64;
#endif
#endif

#if defined(LINUX_OS)
#define FALSE      0
#define TRUE       1
#endif


/**************客户面板控制宏*****************/

/***************************************************/
#define STB_PANEL_INTERVAL_MS	3

#define MAX_PANEL_NAME_LENGTH	8

#define STB_KEY_NUMBER			4

#define GPIO_TOTAL_NUM				31

#define GX3113_PINMUX_PANEL  			0xd050a130

#define EPORT_ADDR                            (0xd0505000)

typedef enum GpioOutput_e
{
	GPIO_INPUT = 0,
	GPIO_OUTPUT
}GpioOutput_t;

typedef uint8_t (*gx_stb_panel_public_cfg_multiplex)(uint64_t);
typedef uint8_t (*gx_stb_panel_public_set_gpio_level)(uint8_t,uint8_t);/*gpio, level*/
typedef uint32_t (*gx_stb_panel_public_get_gpio_level)(uint8_t, uint64_t*);
typedef uint32_t (*gx_stb_panel_public_set_gpio_output)(uint8_t);
typedef uint32_t (*gx_stb_panel_public_set_gpio_input)(uint8_t);
typedef uint32_t (*gx_stb_panel_public_init_gpio)(uint64_t);
typedef uint32_t (*gx_stb_panel_public_open_gpio)(uint8_t, GpioOutput_t);
typedef uint32_t (*gx_stb_panel_public_deal_key)(uint32_t);

typedef struct GXPANEL_PUBLIC_Config_s
{
	uint32_t m_nGpioHandle;
	gx_stb_panel_public_cfg_multiplex m_PanelCfgMultiplexFun;
	gx_stb_panel_public_set_gpio_level m_PanelSetGpioLevelFun;
	gx_stb_panel_public_get_gpio_level m_PanelGetGpioLevelFun;
	gx_stb_panel_public_set_gpio_output m_PanelSetGpioOutPutFun;
	gx_stb_panel_public_set_gpio_input m_PanelSetGpioInPutFun;
	gx_stb_panel_public_init_gpio m_PanelInitGpioFun;
	gx_stb_panel_public_open_gpio m_PanelOpenGpioFun;
	gx_stb_panel_public_deal_key m_PanelDealKey;
}GXPANEL_PUBLIC_Config_t;
extern GXPANEL_PUBLIC_Config_t g_PanelPublicConfig;

enum
{
	PROT_BASE_LOW_ADDRESS,
	PROT_BASE_HIGH_ADDRESS,
	PORT_BASE_ADDRESS_END,
};

#define _NOP_ 	\
    {\
	    volatile uint32_t  temp = 10; \
	    do{\
	        temp--;\
	    }while(temp);\
    }
#if 0
#define BIT_A (0)//            a
#define BIT_B (1)//         -------
#define BIT_C (2)//        |       |
#define BIT_D (3)//    //f |       | b
#define BIT_E (4)//         ---g---
#define BIT_F (5)//        |       |	c
#define BIT_G (6)//    //e |       |
#define BIT_P (7)//         ---d---   p
#endif
#define BIT_A (0)//            a
#define BIT_B (1)//         -------
#define BIT_C (2)//        |       |
#define BIT_D (3)//    //f |       | b
#define BIT_E (4)//         ---g---
#define BIT_F (5)//        |       |	c
#define BIT_G (6)//    //e |       |
#define BIT_P (7)//         ---d---   p
#define BIT_O_1635 (0<<0)//            0
#define BIT_A_1635 (1<<0)//            a
#define BIT_B_1635 (1<<1)//         -------
#define BIT_C_1635 (1<<2)//        |       |
#define BIT_D_1635 (1<<3)//    //f |       | b
#define BIT_E_1635 (1<<4)//         ---g---
#define BIT_F_1635 (1<<5)//        |       | c
#define BIT_G_1635 (1<<6)//    //e |       |
#define BIT_P_1635 (1<<7)//         ---d---   p

/*typedef enum GXLED_Str_e
{
	GXLED_BOOT,
	GXLED_INIT,
	GXLED_ON,
	GXLED_OFF,
	GXLED_E,
	GXLED_P,
	GXLED_USB,
	GXLED_SEAR,
	GXLED_HIDE,
}GXLED_Str_t;

enum
{
	PANEL_READ_KEY = 0,
	PANEL_STRING,
	PANEL_DATA,
	PANEL_LOCK,
	PANEL_UNLOCK,
	PANEL_POWER_OFF,
	PANEL_SET_BLOCKMODE,
	PANEL_STANDBY,
	PANEL_WAKEUP,
	PANEL_KEY,
#if defined(LINUX_OS)
	PANEL_IO_SET,
#endif

};*/

typedef enum GXPanel_ShowMode_e
{
	PANEL_SHOW_NORMAL = 0,
	PANEL_SHOW_OFF,
	PANEL_SHOW_TIME
}GXPanel_ShowMode_t;

enum
{
	LED_DATA_0,
	LED_DATA_1,
	LED_DATA_2,
	LED_DATA_3,
	LED_DATA_4,
	LED_DATA_5,
	LED_DATA_6,
	LED_DATA_7,
	LED_DATA_8,
	LED_DATA_9,
	LED_DATA_E,
	LED_DATA_F,
	LED_DATA_N,
	LED_DATA_P,
	LED_DATA_t,
	LED_DATA_L,
	LED_DATA_b,
	LED_DATA_o,
	LED_DATA_HIDE,

	LED_DATA_C,
	LED_DATA_U,
	LED_DATA_DARK,
};

typedef uint32_t (*gx_stb_panel_config)(void);
typedef void (*gx_stb_panel_init_pin)(void);
typedef void (*gx_stb_panel_isr)(void);
typedef void (*gx_stb_panel_set_led_value)(uint32_t);
typedef void (*gx_stb_panel_set_led_signal_value)(uint32_t);
//typedef void (*gx_stb_panel_set_led_string)(GXLED_Str_t);
typedef void (*gx_stb_panel_set_led_string)(unsigned char*);
typedef void (*gx_stb_panel_lock)(void);
typedef void (*gx_stb_panel_unlock)(void);
typedef void (*gx_stb_panel_stand_by)(void);
typedef void (*gx_stb_panel_wake_up)(void);
typedef void (*gx_stb_panel_show_mode)(GXPanel_ShowMode_t);
typedef void(*gx_stb_panel_scan_manager)(void);
typedef void(*gx_stb_panel_power_off)(void);
#if defined(LINUX_OS)
typedef void (*gx_stb_panel_set_io)(int num,int level);
typedef void (*timer_func)(void);
#endif

typedef struct GXPANEL_PANEL_ControlBlock_s
{
	gx_stb_panel_config m_PanelConfigFun;
	gx_stb_panel_init_pin m_PanelInitPinFun;
	gx_stb_panel_isr m_PanelIsrFun;
	gx_stb_panel_set_led_value m_PanelSetValueFun;
	gx_stb_panel_set_led_signal_value m_PanelSetSignalValueFun;
	gx_stb_panel_set_led_string m_PanelSetStringFun;
	gx_stb_panel_lock m_PanelLockFun;
	gx_stb_panel_unlock m_PanelUnlockFun;
	gx_stb_panel_lock m_PanelStandByFun;
	gx_stb_panel_unlock m_PanelWakeUpFun;
	gx_stb_panel_show_mode m_PanelShowModeFun;
	gx_stb_panel_scan_manager m_PanelScanMangerFun;
	gx_stb_panel_power_off m_PanelPowerOffFun;
#if defined(LINUX_OS)
        gx_stb_panel_set_io    m_PanelSetIo;
#endif

}GXPANEL_PANEL_ControlBlock_t;

#if defined(LINUX_OS)
void create_bsp_timer(timer_func func, unsigned long ms);
extern GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock;
extern uint32_t s_panel_key;
#endif




void gx_stb_panel_public_config(void);


//#define __BSP_DEBUG__
#ifdef __BSP_DEBUG__
#if defined(ECOS_OS)
#define bsp_printf	diag_printf
#elif defined(LINUX_OS)
#define bsp_printf  printk
#endif
#else
	#define bsp_printf(...) {;}
#endif

#endif /* __STB_PANEL_H__ */

/* End of file -------------------------------------------------------------*/


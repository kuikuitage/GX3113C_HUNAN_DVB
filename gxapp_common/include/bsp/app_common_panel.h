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
#ifndef __APP_COMMON_PANEL_H__
#define  __APP_COMMON_PANEL_H__

/* Includes --------------------------------------------------------------- */
//#include <gxtype.h>
//#include "stb_panel.h"
typedef enum GXLED_Str_e
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
	PANEL_GPIO_HIGH,
	PANEL_GPIO_LOW,
#if defined(LINUX_OS)
	PANEL_IO_SET,
#endif

};

void app_panel_show(unsigned int key, void *buf);
void app_panel_set_gpio_high(unsigned int eport);
void app_panel_set_gpio_low(unsigned int eport);


#endif /* __APP_COMMON_PANEL_H__ */

/* End of file -------------------------------------------------------------*/


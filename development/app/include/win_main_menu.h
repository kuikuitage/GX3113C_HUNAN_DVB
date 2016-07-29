/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_main_menu.h
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2012.10.30		  zhouhm 	 			creation
*****************************************************************************/
#ifndef __APP_MAIN_MENU1_H__
#define __APP_MAIN_MENU1_H__
#include "gxapp_sys_config.h"

typedef enum  
{
#if (DVB_THEME_TYPE == DVB_THEME_HD)
	BUTTON_TV = 0,
	BUTTON_RADIO,
	BUTTON_EPG,
	BUTTON_INFO,
	BUTTON_MEDIA,
	BUTTON_SYSTEM_SET,
#endif
#if (DVB_THEME_TYPE == DVB_THEME_SD)
	BUTTON_TV = 0,
	BUTTON_RADIO,
	BUTTON_EPG,
	BUTTON_NVOD,
	BUTTON_MEDIA,
	BUTTON_SYSTEM_SET,
#endif
#if (DVB_THEME_TYPE == DVB_THEME_DTMB_HD)
	BUTTON_TV = 0,
	BUTTON_RADIO,
	BUTTON_INFO,
	BUTTON_MEDIA,
	BUTTON_EPG,
	BUTTON_SYSTEM_SET,
#endif

	BUTTON_MAIN_END
}win_main_menu_button;
int win_main_menu_change_button(int oldbuttonindex , int newbuttonindex);
#if (DVB_THEME_TYPE == DVB_THEME_HD)
int win_main_menu_set_img_list(void);
#endif


#endif

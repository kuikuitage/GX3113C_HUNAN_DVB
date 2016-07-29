/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.12.02		  zhouhm 	 			creation
*****************************************************************************/
#include "app_common_porting_stb_api.h"
#include "app_win_interface.h"


void dvb_cas_finger_show(uint8_t number,uint8_t showFlag,char* message)
{
	if (DVB_CA_FINGER_FLAG_HIDE == showFlag)
	{
		GUI_SetProperty("win_full_screen_text_finger0","state","hide");

		//printf("dvb_cas_finger_hide\n");
	}
	else
		if (DVB_CA_FINGER_FLAG_SHOW == showFlag)
		{
			GUI_SetProperty("win_full_screen_text_finger0","string", message);
			GUI_SetProperty("win_full_screen_text_finger0","state","show");
			//printf("dvb_cas_finger_show %s\n",message);
		}
	return;
}










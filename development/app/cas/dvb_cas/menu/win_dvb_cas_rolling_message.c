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
#include "win_dvb_cas_rolling_message.h"
#include "app_dvb_cas_api_pop.h"

void dvb_cas_rolling_message_show(uint8_t byStyle,uint8_t showFlag,char* message)
{
	if (DVB_CA_ROLLING_FLAG_HIDE == showFlag)
	{
		GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
	}
	else
		if (DVB_CA_ROLLING_FLAG_SHOW == showFlag)
		{
			GUI_SetProperty("win_full_screen_text_roll_top","state","show");
			GUI_SetProperty("win_full_screen_text_roll_top","string", message);
		}
		else
			if (DVB_CA_ROLLING_FLAG_RESET == showFlag)
			{
				GUI_SetProperty("win_full_screen_text_roll_top","rolling_stop","");
				GUI_SetProperty("win_full_screen_text_roll_top","reset_rolling","");
				GUI_SetProperty("win_full_screen_text_roll_top","string", message);
				GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
			}
	return;
}

int32_t dvb_cas_rolling_message_get_times(void)
{
	int32_t wTimes;
	GUI_GetProperty("win_full_screen_text_roll_top","times",&wTimes);
	return wTimes;
}




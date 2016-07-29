/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_signal_setup.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2012.11.20		  zhouhm 	 			creation
*****************************************************************************/
#include "gxbook.h"
#include "gxbus.h"
#include "gxprogram_manage_berkeley.h"
#include "app_common_prog.h"

static int nvod_fullscreen_flag = 0;
static  int nvod_playing_pos ;

int app_nvod_get_shift_prog_pos_from_nvod_id(int nvod_id,int nvod_type)
{
	GxBusPmViewInfo sysinfo;
	GxBusPmViewInfo sysinfo_old;
	int i;
	GxBusPmDataProg prog;

	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));

	
	if ((GROUP_MODE_ALL != sysinfo.group_mode)||(nvod_type != sysinfo.stream_type))
		{
			sysinfo.group_mode = GROUP_MODE_ALL;
			sysinfo.stream_type = nvod_type;
			GxBus_PmViewInfoModify(&sysinfo);		
		}

	app_prog_update_num_in_group();
	for(i=0;i<app_prog_get_num_in_group();i++)
	{
		GxBus_PmProgGetByPos(i,1,&prog);
		//这个判断本来要用ref_serviceid
		if(prog.service_id== nvod_id)
		{
			if (0 != memcmp(&sysinfo,&sysinfo_old,sizeof(GxBusPmViewInfo)))
				GxBus_PmViewInfoModify(&sysinfo_old);
			app_prog_update_num_in_group();
			return i;
		}
	}

	if (0 != memcmp(&sysinfo,&sysinfo_old,sizeof(GxBusPmViewInfo)))
		GxBus_PmViewInfoModify(&sysinfo_old);
	app_prog_update_num_in_group();
	return -1;
	
}

int app_nvod_get_shift_prog_id_from_nvod_id(int nvod_id)
{
	GxBusPmViewInfo sysinfo;
	GxBusPmViewInfo sysinfo_old;
	int i;
	GxBusPmDataProg prog;

	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));

	if ((GROUP_MODE_ALL != sysinfo.group_mode)||(GXBUS_PM_PROG_NVOD != sysinfo.stream_type))
		{
			sysinfo.group_mode = GROUP_MODE_ALL;
			sysinfo.stream_type = GXBUS_PM_PROG_NVOD;
			GxBus_PmViewInfoModify(&sysinfo);		
		}

	app_prog_update_num_in_group();
	for(i=0;i<app_prog_get_num_in_group();i++)
	{
		GxBus_PmProgGetByPos(i,1,&prog);
		//这个判断本来要用ref_serviceid
		if(prog.service_id== nvod_id)
		{
			if (0 != memcmp(&sysinfo,&sysinfo_old,sizeof(GxBusPmViewInfo)))
				GxBus_PmViewInfoModify(&sysinfo_old);
			app_prog_update_num_in_group();
			return prog.id;
		}
	}
	
	if (0 != memcmp(&sysinfo,&sysinfo_old,sizeof(GxBusPmViewInfo)))
		GxBus_PmViewInfoModify(&sysinfo_old);
	app_prog_update_num_in_group();
	return -1;
	
}


int app_nvod_set_full_screen_play_flag(int flag)
{
	nvod_fullscreen_flag = flag;
	return 0;
}


int app_nvod_get_full_screen_play_flag(void)
{
	return nvod_fullscreen_flag;
}

int app_nvod_set_playing_pos(int nvod_pos)
{
	nvod_playing_pos = nvod_pos;
	return 0;
}

int app_nvod_get_playing_pos(void)
{
	return nvod_playing_pos;
}















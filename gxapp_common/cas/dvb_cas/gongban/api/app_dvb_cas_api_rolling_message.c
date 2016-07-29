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
1.0	2012.12.02		  zhouhm				creation
 *****************************************************************************/
#include "dvbca_interface.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_pop.h"
#include "app_dvb_cas_api_rolling_message.h"
#include "gxmsg.h"
#include "gui_core.h"

static dvb_ca_rolling_message_st ca_rolling_msg = {0};
static GxTime nowtime={0};
static GxTime starttime={0};

extern	void dvb_cas_rolling_message_show(uint8_t dvbStyle,uint8_t showFlag,char* message);
extern	int32_t dvb_cas_rolling_message_get_times(void);
extern void win_dvb_cas_showDialog(void *data,int size);

int32_t app_dvb_cas_gxmsg_ca_on_event_rolling(GxMsgProperty0_OnEvent* event)
{
	dvb_ca_rolling_message_st* rolling_msg = NULL;
	if (NULL == event)
		return 0;

	rolling_msg = (dvb_ca_rolling_message_st*)(event->buf);

	if (NULL == rolling_msg)
		return 0;

	memcpy(&ca_rolling_msg,rolling_msg,sizeof(dvb_ca_rolling_message_st));

	dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_RESET,(char*)ca_rolling_msg.content);
	ca_rolling_msg.wTimes= 0;
	ca_rolling_msg.roll_status=FALSE;
	ca_rolling_msg.wStatus = TRUE;
	ca_rolling_msg.interval_status = FALSE;

	return 1;
}

int32_t app_dvb_cas_rolling_message_exec(void)
{
	char* curFocusWin = (char *)GUI_GetFocusWindow();
	uint16_t curTimes = 0;
	static uint16_t oldTimes = 0;

	if ((NULL != curFocusWin) && (strcasecmp(curFocusWin, "win_full_screen") != 0))
	{
		return TRUE;
	}

	if (TRUE == ca_rolling_msg.wStatus )
	{
		if ((FALSE == ca_rolling_msg.roll_status)&&(FALSE == ca_rolling_msg.interval_status))
		{
			dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_SHOW,(char*)ca_rolling_msg.content);
			ca_rolling_msg.roll_status= TRUE;
		}

		if (0 != ca_rolling_msg.wDuration)
		{
			curTimes = dvb_cas_rolling_message_get_times();
			//CAS_Dbg("%s %d times = %d  curTimes = %d durating = %d\n",__FUNCTION__,__LINE__,ca_rolling_msg.wTimes,curTimes,ca_rolling_msg.wDuration);
			if(curTimes != oldTimes)
			{
				if(curTimes > oldTimes)
					oldTimes = curTimes;
				else
				{
					oldTimes = curTimes;
					return TRUE;
				}
				ca_rolling_msg.wTimes++;
				if (ca_rolling_msg.wTimes < ca_rolling_msg.allTimes)
				{
					GxCore_GetTickTime(&starttime);
					ca_rolling_msg.wStartTime = starttime.seconds;
					dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_HIDE,NULL);
					ca_rolling_msg.interval_status = TRUE;
				}
				else
				{
					//CAS_Dbg("shwo end:curTimes = %d \n",ca_rolling_msg.wTimes);
					dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_HIDE,NULL);
					ca_rolling_msg.wStatus = FALSE;
					ca_rolling_msg.roll_status= FALSE;
					ca_rolling_msg.wTimes= 0;
					ca_rolling_msg.allTimes= 0;
					ca_rolling_msg.wDuration=0;
					oldTimes = 0;
					DVBCASTB_ShowOsdMessageEnd();
				}
			}

			if(ca_rolling_msg.interval_status == TRUE)
			{
				GxCore_GetTickTime(&nowtime);
				//CAS_Dbg("%s %d nowtime = %d starttime=%d\n",__FUNCTION__,__LINE__,nowtime.seconds,starttime.seconds);
				if (nowtime.seconds - starttime.seconds >= ca_rolling_msg.wDuration)
				{
					ca_rolling_msg.roll_status= FALSE;
					ca_rolling_msg.interval_status = FALSE;
				}
			}
		}
		else
		{
			curTimes = dvb_cas_rolling_message_get_times();
			if(oldTimes != curTimes)
			{
				if(curTimes > oldTimes)
					oldTimes = curTimes;
				else
				{
					oldTimes = curTimes;
					return TRUE;
				}
				ca_rolling_msg.wTimes++;

				if (ca_rolling_msg.wTimes < ca_rolling_msg.allTimes)
				{
					dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_HIDE,NULL);
					ca_rolling_msg.roll_status= FALSE;
				}
				else
				{
					// 查询滚动结束，清除OSD滚动标志
					ca_rolling_msg.wStatus = FALSE;
					ca_rolling_msg.roll_status= FALSE;
					dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_HIDE,NULL);
					ca_rolling_msg.wTimes= 0;
					ca_rolling_msg.allTimes= 0;
					DVBCASTB_ShowOsdMessageEnd();
				}
			}
		}
	}
	else
	{
		if (TRUE == ca_rolling_msg.roll_status)
		{
			dvb_cas_rolling_message_show(0,DVB_CA_ROLLING_FLAG_HIDE,NULL);
			ca_rolling_msg.roll_status= FALSE;
			ca_rolling_msg.wTimes= 0;
		}
	}
	return TRUE;
}

void app_dvb_cas_rolling_message_hide(void)
{
	CAS_Dbg("[app_dvb_cas_rolling_message_hide]----------\n");
	if (TRUE == ca_rolling_msg.roll_status)
	{
		dvb_cas_rolling_message_show(0, DVB_CA_ROLLING_FLAG_HIDE, NULL);
		ca_rolling_msg.roll_status = FALSE;
	}
	return;
}





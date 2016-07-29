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
#include "dvbca_interface.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_pop.h"
#include "app_dvb_cas_api_finger.h"

extern 	void dvb_cas_finger_show(uint8_t number,uint8_t showFlag,char* message);

static dvb_ca_finger_data_st ca_finger_data = {0};
int32_t app_dvb_cas_gxmsg_ca_on_event_finger(GxMsgProperty0_OnEvent* event)
{
	dvb_ca_finger_data_st* finger_data = NULL;
	if (NULL == event)
		return 0;

	finger_data = (dvb_ca_finger_data_st*)(event->buf);

	if (NULL == finger_data)
		return 0;

	if ((0 != ca_finger_data.wStatus)&&(TRUE == ca_finger_data.wShowStatus))
	{
		dvb_cas_finger_show(0,DVB_CA_FINGER_FLAG_HIDE,NULL);
	}

	memcpy(&ca_finger_data,finger_data,sizeof(dvb_ca_finger_data_st));

//	ca_finger_data.wStatus = 0;
	ca_finger_data.wShowStatus = 0;
	CAS_Dbg("dvb_cas_finger wStatus = %d;wEcmPid=%d;dwCardID=%d;wDuration=%d\n",ca_finger_data.wStatus,ca_finger_data.wEcmPid,ca_finger_data.dwCardID,ca_finger_data.wDuration);

	return 1;
}

int32_t app_dvb_cas_finger_exec(void)
{
	uint8_t chFingerBuf[40]={0};

	if (0 == ca_finger_data.wStatus)
	{
		/*清除*/
		if (TRUE == ca_finger_data.wShowStatus)
		{
			ca_finger_data.wShowStatus = 0;
			dvb_cas_finger_show(0,DVB_CA_FINGER_FLAG_HIDE,NULL);
		}
	}
	else
		if (1 == ca_finger_data.wStatus)
		{
			/*显示卡号*/
			if (FALSE == ca_finger_data.wShowStatus)
			{
				sprintf((char*)chFingerBuf, "id:%X", ca_finger_data.dwCardID);
				dvb_cas_finger_show(0,DVB_CA_FINGER_FLAG_SHOW,(char*)chFingerBuf);
				ca_finger_data.wShowStatus = TRUE;
				CAS_Dbg("dvb_cas_finger_exec wStatus = %d;wEcmPid=%d;dwCardID=%d;wDuration=%d\n",ca_finger_data.wStatus,ca_finger_data.wEcmPid,ca_finger_data.dwCardID,ca_finger_data.wDuration);
			}
		}
		else
			if (2 == ca_finger_data.wStatus)
			{
				/*显示PID和卡号*/
				if (FALSE == ca_finger_data.wShowStatus)
				{
					sprintf((char*)chFingerBuf, "ecmpid:0x%x ,id:%X", ca_finger_data.wEcmPid,ca_finger_data.dwCardID);
					dvb_cas_finger_show(0,DVB_CA_FINGER_FLAG_SHOW,(char*)chFingerBuf);
					ca_finger_data.wShowStatus = TRUE;
				}
			}

	return 1;
}








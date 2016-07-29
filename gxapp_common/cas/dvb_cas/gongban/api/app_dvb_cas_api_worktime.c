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
#include "app_dvb_cas_api_worktime.h"

ST_DVBCAWORKTIME stStartTime;
ST_DVBCAWORKTIME stEndTime;

uint8_t app_dvb_cas_api_init_worktime_data(void)
{
	stStartTime.ucHour = 0;
	stStartTime.ucMinute = 0;
	stEndTime.ucHour = 0;
	stEndTime.ucMinute = 0;
	DVBCASTB_GetWorkTime(&stStartTime, &stEndTime);
	return 0;
}

char * app_dvb_cas_api_get_worktime_data(ca_get_date_t *data)
{
	uint32_t pos = 0 ;
	uint32_t ID = 0;
	static char            s_text_buffer[30];

	if (NULL == data)
		return NULL;

	pos = data->pos;
	ID = data->ID;

	memset(s_text_buffer,0,sizeof(s_text_buffer));
	switch(ID)
	{
		case DVB_WORKTIME_START_ID:
			CAS_Dbg("%02d:%02d", stStartTime.ucHour,stStartTime.ucMinute);
			sprintf(s_text_buffer, "%02d:%02d", stStartTime.ucHour,stStartTime.ucMinute);
			return s_text_buffer;
		case DVB_WORKTIME_END_ID:
			CAS_Dbg("%02d:%02d", stEndTime.ucHour,stEndTime.ucMinute);
			sprintf(s_text_buffer, "%02d:%02d", stEndTime.ucHour,stEndTime.ucMinute);
			return s_text_buffer;
		default:
			break;
	}
	return NULL;
}

char* app_dvb_cas_api_change_worktime(ca_work_time_t* data)
{
	char StartTime[20]={0,};
	char EndTime[20]={0,};
	char  StartTimeHour[3];
	char  StartTimeMin[3];
	char  StartTimeSec[3];
	char  EndTimeHour[3];
	char  EndTimeMin[3];
	char  EndTimeSec[3];
	unsigned char i;
	short  wRet;
	ST_DVBCAWORKTIME stStartTime;
	ST_DVBCAWORKTIME stEndTime;
	uint8_t PinValue[DVBCA_PIN_MAX_LENGTH+1]={0};


	if (NULL == data)
		return NULL;

	memcpy(PinValue,data->pin,DVBCA_PIN_MAX_LENGTH);
	if(strlen((char*)PinValue)!=DVBCA_PIN_MAX_LENGTH)
	{
		return (char*)"Invalid PIN";
	}

	for( i = 0; i < DVBCA_PIN_MAX_LENGTH; i++ )
	{
		PinValue[i] -= 0x30;
	}
	memcpy(StartTime,data->starttime,8);
	memcpy(EndTime,data->endtime,8);

	StartTimeHour[2] = '\0';
	memcpy(StartTimeHour, StartTime, 2);
	StartTimeMin[2] = '\0';
	memcpy(StartTimeMin, StartTime + 3, 2);
	StartTimeSec[2] = '\0';
	memcpy(StartTimeSec, StartTime + 6, 2);

	EndTimeHour[2] = '\0';
	memcpy(EndTimeHour, EndTime, 2);
	EndTimeMin[2] = '\0';
	memcpy(EndTimeMin, EndTime + 3, 2);
	EndTimeSec[2] = '\0';
	memcpy(EndTimeSec, EndTime + 6, 2);

	stStartTime.ucHour    = atoi(StartTimeHour);
	stStartTime.ucMinute  = atoi(StartTimeMin);
	stEndTime.ucHour      = atoi(EndTimeHour);
	stEndTime.ucMinute    = atoi(EndTimeMin);

	wRet = DVBCASTB_SetWorkTime(PinValue,&stStartTime,&stEndTime);
	switch( wRet )
	{
		case DVBCA_SUCESS:
			return (char*)"Working Hours changed successfully";
		case DVBCA_INSERTCARD:
			return (char*)"ERROR NO CARD";
		case DVBCA_CARD_INVALID:
			return (char*)"Invalid Smartcard";
		case DVBCA_PIN_ERROR:
			return (char*)"Invalid PIN";
		case DVBCA_FAILED:
			return (char*)"Unknow Error";
		case DVBCA_INIT_ERROR:
			return "(DVBCA)Initialization failed";
		case DVBCA_CARD_VER_ERROR:
			return "(DVBCA)CA Version Error";
		default:
			break;
	}

	return NULL;
}








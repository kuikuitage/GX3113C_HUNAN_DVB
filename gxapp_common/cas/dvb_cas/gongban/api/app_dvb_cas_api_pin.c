/*****************************************************************************
 * 						   CONFIDENTIAL
 *        Hangzhou GuoXin Science and Technology Co., Ltd.
 *                      (C)2012, All right reserved
 ******************************************************************************

 ******************************************************************************
 * File Name :app_dvb_cas_api_pin.c
 * Author    :fuxl
 * Project   :goxceed dvbc
 * Type      :
 ******************************************************************************
 * Purpose   :模块头文件
 ******************************************************************************
 * Release History:
 VERSION	Date			  AUTHOR         Description
 1.0  	2012.12.02		  zhouhm 	 			creation
 *****************************************************************************/
#include "dvbca_interface.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_pin.h"

char* app_dvb_cas_api_change_pin(ca_pin_t* data)
{
	uint8_t newPassword1[DVBCA_PIN_MAX_LENGTH+1]={0};
	uint8_t newPassword2[DVBCA_PIN_MAX_LENGTH+1]={0};
	uint8_t newPassword3[DVBCA_PIN_MAX_LENGTH+1]={0};
	short   wRet   = 0;
	unsigned char i = 0;

	if (NULL == data)
		return NULL;


	memcpy(newPassword1,data->oldpin, DVBCA_PIN_MAX_LENGTH);
	memcpy(newPassword2, data->newpin, DVBCA_PIN_MAX_LENGTH);
	memcpy(newPassword3, data->newconfirmpin, DVBCA_PIN_MAX_LENGTH);
	if(strlen((char*)newPassword1)!=DVBCA_PIN_MAX_LENGTH )
	{
		return (char*)"Invalid PIN";//"Input old PIN";
	}
	else if(strlen((char*)newPassword2)!=DVBCA_PIN_MAX_LENGTH)
	{
		return (char*)"Invalid PIN";//"Input new PIN";
	}
	else if(strlen((char*)newPassword3)!=DVBCA_PIN_MAX_LENGTH )
	{
		return (char*)"Invalid PIN";//"Input new PIN";
	}
	if(atoi( (char*)newPassword3 ) != atoi( (char*)newPassword2 ))
	{
		return (char*)"Invalid PIN";//"Verify new PIN failed";
	}
	for( i = 0; i < DVBCA_PIN_MAX_LENGTH; i++ )
	{
		newPassword1[i] -= 0x30;
		newPassword2[i] -= 0x30;
		newPassword3[i] -= 0x30;
	}

	wRet = DVBCASTB_ChangePin(newPassword1,newPassword2);
	switch( wRet )
	{
		case DVBCA_SUCESS:
			data->errorCode = 0;
			return (char*)"PIN has been changed";
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








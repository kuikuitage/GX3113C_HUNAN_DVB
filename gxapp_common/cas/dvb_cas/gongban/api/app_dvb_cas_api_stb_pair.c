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
#include "app_dvb_cas_api_stb_pair.h"

char* app_dvb_cas_api_stb_pair(ca_pin_t* data)
{
	uint8_t pin[DVBCA_PIN_MAX_LENGTH+1]={0};
	short   wRet   = 0;
	unsigned char i = 0;

	if (NULL == data)
		return NULL;

	memcpy(pin,data->oldpin, DVBCA_PIN_MAX_LENGTH);

	if(strlen((char*)pin)!=DVBCA_PIN_MAX_LENGTH )
	{
		return (char*)"Invalid PIN";
	}
	for( i = 0; i < DVBCA_PIN_MAX_LENGTH; i++ )
	{
		pin[i] -= 0x30;
	}

	wRet = DVBCASTB_SetStbPair(pin);
	switch( wRet )
	{
		case DVBCA_SUCESS:
			return (char*)"CARD PAIRING OK";
		case DVBCA_INSERTCARD:
			return (char*)"ERROR NO CARD";
		case DVBCA_CARD_INVALID:
			return (char*)"Invalid Smartcard";
		case DVBCA_PIN_ERROR:
			return (char*)"Invalid PIN";
		case DVBCA_FAILED:
		case DVBCA_INIT_ERROR:
		case DVBCA_CARD_VER_ERROR:
			return (char*)"Unknow Error";
		default:
			break;
	}

	return NULL;
}








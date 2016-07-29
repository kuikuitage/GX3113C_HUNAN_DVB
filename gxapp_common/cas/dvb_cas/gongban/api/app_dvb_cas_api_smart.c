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
#include "app_common_play.h"
#include "app_common_porting_stb_api.h"
#include "app_common_search.h"
#include "app_common_porting_ca_flash.h"
#include "app_common_porting_ca_smart.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_ca_descramble.h"
#include "app_dvb_cas_api_demux.h"
#include "app_dvb_cas_api_smart.h"
#include "dvbca_interface.h"


#if 0
#define RUNTIME_ETU                         (372)
#define RUNTIME_BAUD_RATE                   (9600*RUNTIME_ETU)
#define RUNTIME_EGT                         (RUNTIME_ETU * 20)

#define RUNTIME_WDT                         (48 * RUNTIME_ETU)
//#define RESET_WDT            	(9600 * RUNTIME_ETU)
#define RUNTIME_TGT                         (0)
#define RUNTIME_TWDT                        (3*9600 * RUNTIME_ETU)
#else
#define RUNTIME_ETU   			(372)
#define RUNTIME_BAUD_RATE 	 	(9600 * RUNTIME_ETU)
#define RUNTIME_EGT    			(RUNTIME_ETU * 100)

#define RUNTIME_WDT   			(100 * RUNTIME_ETU)
#define RUNTIME_TGT   			(0)
#define RUNTIME_TWDT   			(4 * 9600 * RUNTIME_ETU)
#endif

uint8_t app_dvb_cas_api_smart_check_valid(void)
{
	uint8_t valid_art[8]={0x3b,0x94,0x11,0x00,0x97,0x04,0x01,0x00};
	uint8_t len;
	uint8_t i;
	uint8_t atr[ATR_MAX_LEN] = {0,};
	uint8_t ret = 0;

	app_dvb_cas_api_smart_init();
	ret = app_dvb_cas_api_smart_reset(atr, &len);
	if (FALSE == ret)
	{
		CAS_Dbg(" smart card is not dvb cas smart \n");
		return FALSE;
	}

	for (i=0; i<8; i++)
	{
		if (valid_art[i]!=atr[i])
		{
			CAS_Dbg(" smart card is not dvb cas smart \n");
			return FALSE;
		}
	}

	CAS_Dbg(" smart card is dvb cas smart\n");
	return TRUE;
}

void app_dvb_cas_api_smart_init(void)
{
	GxSmcParams	  param;
	GxSmcTimeParams	  time;

	memset(&time,0,sizeof(GxSmcTimeParams));
	memset(&param,0,sizeof(GxSmcParams));

	time.baud_rate  = RUNTIME_BAUD_RATE;
	time.egt	   = RUNTIME_EGT;
	time.etu	   = RUNTIME_ETU;
	time.tgt	   = RUNTIME_TGT;
	time.twdt	   = RUNTIME_TWDT;
	time.wdt	   = RUNTIME_WDT;

	param.detect_pole = GXSMC_DETECT_HIGH_LEVEL;//GXSMC_DETECT_LOW_LEVEL;
	param.io_conv = GXSMC_DATA_CONV_DIRECT;
	param.parity = GXSMC_PARITY_ODD;// GXSMC_PARITY_EVEN;
	param.protocol = DISABLE_REPEAT_WHEN_ERR;
	param.sci_sel = __GXSMART_SCI2;
	param.stop_len = GXSMC_STOPLEN_0BIT;
	param.vcc_pole = GXSMC_VCC_HIGH_LEVEL;
	param.default_etu = 372;
	param.auto_etu = 1;
	param.auto_parity = 1;
	//	   param.debug_info = O_BDBG|O_CDBG;

	return app_porting_ca_smart_card_init(param,time);
}

uint8_t app_dvb_cas_api_smart_reset(uint8_t* pbyATR, uint8_t *pbyLen )
{
	GxSmcTimeParams    time;
	memset(&time,0,sizeof(GxSmcTimeParams));
       	time.baud_rate  = RUNTIME_BAUD_RATE;
	 time.egt        = RUNTIME_EGT;
	 time.etu        = RUNTIME_ETU;
	 time.tgt        = RUNTIME_TGT;
	 time.twdt       = RUNTIME_TWDT;
	 time.wdt        = RUNTIME_WDT;

	 return app_porting_ca_smart_reset(time,pbyATR,pbyLen);
}

uint8_t app_dvb_cas_api_card_in(char* atr,uint8_t len)
{
	DVBCASTB_SCInsert();

#ifdef DVB_CA_FREE_STOP
	app_play_reset_play_timer(0);
#endif
	app_dvb_cas_api_reset_ecm_filter();
	app_dvb_cas_api_reset_emm_filter();
	return TRUE;
}

uint8_t app_dvb_cas_api_card_out(void)
{
	DVBCASTB_SCRemove();
	CAS_Dbg("app_dvb_cas_api_card_out \n");

#ifdef DVB_CA_FREE_STOP
	app_play_stop();
#endif
	return 1;
}


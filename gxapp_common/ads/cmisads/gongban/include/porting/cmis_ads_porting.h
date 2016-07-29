#ifndef __CMIS_ADS_PORTING_H__
#define __CMIS_ADS_PORTING_H__
#include "cmis_ads_api.h"

#define MAX_CMIS_SECTION_BUFFER (64*1024)
#define CMIS_SECTION_OK 				     	 0
#define CMIS_TABLE_OK						 1
#define CMIS_AD_CURTAIN_FILE_PROTOCOL_VERSION	 1
#define CMIS_AD_EPG_FILE_PROTOCOL_VERSION	 2
#define CMIS_CURTAIN_DATA_MAGIC_NO			 0xAB0CD0EF
#define CMIS_EPG_DATA_MAGIC_NO			  	 0xAB1CD1EF
#define CMIS_DATA_RECEIVE_TIMEOUT_S 		 2 /*time out 5S*/

#define SDT_CMIS_CHANNEL_DES_TAG			0xA2
#define SDT_CMIS_EPG_DES_TAG				0xA1
#define CMIS_CURTAIN_DATA_TABLE_ID		0xAD
#define CMIS_EPG_DATA_TABLE_ID			0xAF
#define CMIS_SDT_PID				0x11
#define CMIS_SDT_TABLE_ID			0x42

#define CMIS_PROG_LIST_LEN			1000
#define CMIS_INVALID_PID			0x1fff

#define PRIVATE_SECTION_PARSE_ERROR  0xffff
#define PRIVATE_SECTION_MAX_SECNO  	256

//#define TODATA16(data1,data2)         		   	   (data1<<8 )| data2
//#define TODATA12(data1,data2) 		 		((data1 & 0x0f)<< 8 ) |data2
//#define TODATA32(data1,data2,data3,data4) 	 ((data1<<24)|(data2<<16)|(data3<<8)|(data4))

#define CMIS_FREE 		GxCore_Free
#define CMIS_MALLOC		GxCore_Malloc

#define CMIS_FREE_PTR_MEM(ptr)	do{	\
	if(ptr)	\
	{	\
		CMIS_FREE(ptr);	\
		ptr = NULL;	\
	}	\
}while(0)
	
Cmis_err_t cmis_ad_prog_list_init(void);
Cmis_err_t cmis_ad_prog_list_get_prog_by_type(Cmis_prog_type_t prog_type,Cmis_prog_t * prog_data);
Cmis_err_t cmis_ad_prog_list_get_prog_by_service_id(uint16_t service_id,Cmis_prog_t * prog_data);
Cmis_err_t cmis_ad_demux_private_filter_close(void);
Cmis_err_t cmis_ad_demux_sdt_filter_open(uint16_t pid);
Cmis_err_t cmis_ad_demux_sdt_filter_close(void);
void cmis_ad_demux_init(void);

#endif

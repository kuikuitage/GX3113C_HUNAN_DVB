#ifndef __CMIS_ADS_FLASH_H__
#define __CMIS_ADS_FLASH_H__

#include <gxtype.h>

#define CMIS_ADS_FLASH_LEN		0x10000

#define CMIS_ADS_FLASH_AD_INFO_LEN		0x6000
#define CMIS_ADS_FLASH_CHANNEL_LEN		0x2000

#define CMIS_ADS_FLASH_AD_INFO_ITEM_LEN		(sizeof(uint16_t)+sizeof(Cmis_prog_type_t)+sizeof(uint16_t)+sizeof(uint16_t))
//#define CMIS_ADS_FLASH_AD_CAMAIL_ITEM_LEN		sizeof()


#define CMIS_ADS_FLASH_AD_INFO_OFFSET	0
#define CMIS_ADS_FLASH_CHANNEL_OFFSET		CMIS_ADS_FLASH_AD_INFO_OFFSET+CMIS_ADS_FLASH_AD_INFO_LEN

void cmis_ad_flash_init(void);
void cmis_ad_flash_load_ad_info(void);
Cmis_err_t cmis_ad_flash_write_ad_info(Cmis_prog_t * prog_info,int32_t index_offset);
void cmis_ad_flash_load_cmis_channel_info(void);
Cmis_err_t cmis_ad_flash_write_cmis_channel_info
	(Cmis_prog_type_t prog_type,GxBusPmDataProg * prog_data);
void cmis_ad_flash_clean(void);
void cmis_ad_flash_update(void);


#endif

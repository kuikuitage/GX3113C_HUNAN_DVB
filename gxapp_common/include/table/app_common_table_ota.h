/**
 *
 * @file        app_common_table_ota.h
 * @brief
 * @version     1.1.0
 * @date        10/18/2013 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_OTA__H__
#define __APP_COMMON_OTA__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <module/si/si_parser.h>

typedef uint8_t (*ota_parse_descriptor)(uint8_t tag,uint8_t* pData);	

handle_t app_table_ota_get_filter_handle(void);
handle_t app_table_ota_get_channel_handle(void);
int app_table_ota_monitor_filter_close(void);
int app_table_ota_monitor_filter_open(uint16_t pid,uint8_t tabieid);
int app_table_ota_monitor_filter_restart(void);
void app_table_ota_monitor_section_parse(uint8_t* pSectionData,size_t Size);
void app_table_ota_register_parse_descriptor_callback(ota_parse_descriptor ota_parse_descriptor_call_back);

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_OTA__H__*/


/**
 *
 * @file        app_common_table_nit.h
 * @brief
 * @version     1.1.0
 * @date        10/18/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_NIT__H__
#define __APP_COMMON_NIT__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <module/si/si_parser.h>

#define NIT_TABLE_ID     0x0010

handle_t app_table_nit_get_filter_handle(void);
handle_t app_table_nit_get_channel_handle(void);

typedef uint8_t (*nit_parse_descriptor)(uint8_t tag,uint8_t* pData);	
void app_table_nit_register_parse_descriptor_callback(nit_parse_descriptor nit_parse_descriptor_call_back);
void app_table_nit_search_filter_close(void);
void app_table_nit_search_filter_open(void);
void app_table_nit_get_search_filter_info(int32_t* pNitSubtId,uint32_t* pNitRequestId);
private_parse_status app_table_nit_search_section_parse(uint8_t* p_section_data, size_t Size);

int app_table_nit_monitor_filter_close(void);
int app_table_nit_monitor_filter_open(void);
int app_table_nit_monitor_filter_restart(void);
void app_table_nit_monitor_section_parse(uint8_t* pSectionData,size_t Size);
uint16_t app_table_nit_get_network_id(void);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_NIT__H__*/


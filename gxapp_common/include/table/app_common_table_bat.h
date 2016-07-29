/**
 *
 * @file        app_common_table_bat.h
 * @brief
 * @version     1.1.0
 * @date        04/30/2014 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_BAT__H__
#define __APP_COMMON_BAT__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <module/si/si_parser.h>

void app_table_bat_filter_close(void);
void app_table_bat_filter_close_byid(int32_t BatSubtId);
void app_table_bat_filter_open(uint16_t bouquet_id);
void app_table_bat_get_filter_info(int32_t* pBatSubtId,uint32_t* pBatRequestId);
typedef uint8_t (*bat_parse_descriptor)(uint8_t tag,uint8_t* pData,uint16_t ts_id);	
void app_table_bat_register_parse_descriptor_callback(bat_parse_descriptor bat_parse_descriptor_call_back);
private_parse_status app_table_bat_search_section_parse(uint8_t* p_section_data, size_t len);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_BAT__H__*/


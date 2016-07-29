/**
 *
 * @file        app_serial_table_ts.h
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 15:17:49 PM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_SERIAL_TABLE_TS__H__
#define __APP_SERIAL_TABLE_TS__H__
#ifdef __cplusplus
extern "C" { 
#endif
#include "gxtype.h"


int app_table_ts_monitor_filter_open(uint16_t pid);

handle_t app_table_ts_get_channel_handle(void);
handle_t app_table_ts_get_filter_handle(void);
#ifdef __cplusplus
}
#endif
#endif /*__APP_SERIAL_LANDIAN__H__*/






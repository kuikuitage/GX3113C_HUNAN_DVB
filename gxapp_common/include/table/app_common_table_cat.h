/**
 *
 * @file        app_common_table_cat.h
 * @brief
 * @version     1.1.0
 * @date        10/29/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_CAT__H__
#define __APP_COMMON_CAT__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <module/si/si_parser.h>

#define CAT_TABLE_ID     0x0001

handle_t app_table_cat_get_filter_handle(void);
handle_t app_table_cat_get_channel_handle(void);

int app_table_cat_filter_restart(void);
int app_table_cat_filter_close(void);

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_CAT__H__*/


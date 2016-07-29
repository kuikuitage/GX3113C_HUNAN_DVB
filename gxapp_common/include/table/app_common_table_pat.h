/**
 *
 * @file        app_common_table_pat.h
 * @brief
 * @version     1.1.0
 * @date        02/21/2016 11:00:10 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PAT_H__
#define __APP_COMMON_PAT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <module/si/si_parser.h>
#include "gxapp_sys_config.h"


#define PAT_TABLE_ID                            0x00

private_parse_status app_table_pat_section_parse(uint8_t* pSectionData, size_t Size);


#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PAT_H__*/


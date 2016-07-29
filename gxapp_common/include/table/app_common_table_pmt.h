/**
 *
 * @file        app_common_table_pmt.h
 * @brief
 * @version     1.1.0
 * @date        10/29/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PMT_H__
#define __APP_COMMON_PMT_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <module/si/si_parser.h>
#include "gxapp_sys_config.h"


#define PSI_INVALID_PID (0x1FFF)
#define PMT_TABLE_ID                            0x02

//stream type 
#define  MPEG_1_VIDEO		0x01
#define  MPEG_2_VIDEO		0x02
#define  MPEG_4_VIDEO       0x10
#define  MPEG_1_AUDIO		0x03
#define  MPEG_2_AUDIO		0x04
#define  H264				0x1b
#define  H265               0x24
#define  AAC_ADTS			0xf
#define  AAC_LATM			0x11
#define  AVS				0x42
#define	 PRIVATE_PES_STREAM  0x06//出现在pmt中一般是EAC3
#define	 LPCM  0x80
#define	 AC3  0x81
#define	 DTS  0x82
#define	 DOLBY_TRUEHD  0x83
#define	 AC3_PLUS  0x84
#define	 DTS_HD  0x85
#define	 DTS_MA  0x86
#define	 AC3_PLUS_SEC  0xa1
#define	 DTS_HD_SEC  0xa2
#define  DRA_RESTRATION 0x05
#define  DRA_AUDIO   0xa0


#define CA_DESCRIPTOR	                        0x09
#define AC3_DESCRIPTOR                          0x6A

#define dbsm_ADTag                                 0xD2 
#define SUBT_DESCRIPTOR                         0x59
#define TTX_DESCRIPTOR                          0x56


//加扰类型
#define SCRAMBLE_TS                           1
#define SCRAMBLE_PES                          2

int app_table_pmt_filter_open(uint16_t pid,uint32_t service_id,uint8_t version,uint8_t versionEQ);
int app_table_pmt_filter_close();
handle_t app_table_pmt_get_filter_handle(void);

private_parse_status app_table_pmt_section_parse(uint8_t* pSectionData, size_t Size);
#if MEDIA_SUBTITLE_SUPPORT
void app_pmt_ttx_subt_clear(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PMT_H__*/


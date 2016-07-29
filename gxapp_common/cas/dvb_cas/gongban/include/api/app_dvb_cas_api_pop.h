/**
 *
 * @file        app_dvb_cas_api_baseinfo.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVBCAS_API_POP_H__
#define __APP_DVBCAS_API_POP_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint16_t wStatus; /* 0 --清除，1 -- 显示卡号 2 -- 显示ECMID和卡号*/
	uint16_t wShowStatus; /* 0 --隐藏状态，1 -- 显示状态*/
	uint16_t wEcmPid;
	uint32_t dwCardID;
	uint32_t wDuration;
} dvb_ca_finger_data_st;

#define ROLL_TITLE_MAX (100)
#define ROLL_CONTENT_MAX (1024)

typedef struct {
	uint8_t wStatus; /* 0 --不需要显示，1-- 需要显示*/
	uint8_t roll_status; /*0 -- 当前滚动状态，1-- 当前非滚动状态*/
	uint8_t interval_status; /*0 -- 非间隔，1-- 间隔记时*/
	uint8_t title[ROLL_TITLE_MAX];
	uint8_t content[ROLL_CONTENT_MAX];
	uint16_t wDuration;
	int32_t wTimes;       /*已滚动次数*/
	int32_t allTimes;       /*总滚动次数*/
	uint32_t wStartTime; /*记录开始滚动时间，用以计算滚动时长*/
} dvb_ca_rolling_message_st;

typedef struct {
	uint8_t wStatus; /* 0 --不需要显示，1-- 需要显示*/
	uint8_t *title;
	uint8_t *content;
} dvb_ca_urgent_message_st;
#ifdef __cplusplus
}
#endif
#endif /*__APP_DVBCAS_API_POP_H__*/


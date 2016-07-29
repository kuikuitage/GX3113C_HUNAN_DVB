/**
 *
 * @file        app_serial_landian.h
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 15:17:49 PM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_SERIAL_LANDIAN__H__
#define __APP_SERIAL_LANDIAN__H__
#ifdef __cplusplus
extern "C" {
#endif
#include "gxtype.h"
typedef enum 
{
	SEND_TS_ID= 1,
	GET_PID_ID =2,
	GET_VOLUME_ID = 3,
	GET_AUDIO_PID_ID = 4,
	SEND_SIGNAL_QUALITY=5,
	GET_TV_RADIO_MODE = 6,
	GET_ALL_SEARCH = 7,
	SEND_DATA_STATUS = 8,
}SERIAL_ID_E;
typedef struct
{
	uint32_t pid;
	uint16_t length;
	uint8_t buffer[1024];
}GXSerialSection;



bool ComData_Process_task_init(void);

bool Send_ComData(uint8_t *buf,uint8_t Length,uint32_t TimeOut);

#ifdef __cplusplus
}
#endif
#endif /*__APP_SERIAL_LANDIAN__H__*/






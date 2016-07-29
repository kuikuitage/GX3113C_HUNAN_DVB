/*
 * =====================================================================================
 *
 *       Filename:  app_update.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年09月20日 14时18分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __APP_UPDATE_H__
#define __APP_UPDATE_H__


#define APP_UPDATE_SERIAL_DEVICE_NAME "/dev/ser0"
#define APP_UPDATE_FLASH_DEVICE_NAME "/dev/flash/0/0"

int32_t  SerialUpdate(void);

#endif


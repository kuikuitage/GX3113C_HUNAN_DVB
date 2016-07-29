/*****************************************************************************
* 			  CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2009, All right reserved
******************************************************************************

******************************************************************************
* File Name :	xxx.c
* Author    : 	xxx
* Project   :	GoXceed -S
******************************************************************************
* Purpose   :	
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   0.0  	2009.12.04	            xxx         creation
*****************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __APP_SEND_MSG_H__
#define __APP_SEND_MSG_H__

/* Includes --------------------------------------------------------------- */

/* Cooperation with C and C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include "gxcore.h"
#include "gxmsg.h"
/* Exported Constants ----------------------------------------------------- */
#include "gxmsg.h"
#include "service/gxplayer.h"
#include "service/gxextra.h"
#include "service/gxfrontend.h"
#include "service/gxepg.h"
#include "service/gxsearch.h"
#include "service/gxsi.h"
#include "app_common_search.h"

//#include "app_common_media.h"



extern status_t app_send_msg_exec(uint32_t msg_id,void* params);
#define APP_PRINT(...)     printf(__VA_ARGS__)
#endif


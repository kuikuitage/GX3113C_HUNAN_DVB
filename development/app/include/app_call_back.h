/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_call_back.h
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2010.10.30		  zhouhm 	 			creation
*****************************************************************************/
#ifndef __APP_CALL_BACK_H__
#define __APP_CALL_BACK_H__
#include "gxbook.h"
#include "app_common_book.h"
#include "app_common_search.h"

int app_book_add_event_pop_msg_call_back(int ret, uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet ,GxBook* pbooktmp);
int app_book_confirm_play_pop_msg_call_back(book_play book,GxBusPmDataProg   prog);
int app_book_confirm_record_pop_msg_call_back(book_play book,GxBusPmDataProg   prog);
int app_search_add_extend_table_call_back(search_extend* searchExtendList );
void app_win_prompt_msg_exec_call_back(void);
uint8_t app_table_nit_parse_descriptor_call_back(uint8_t tag, uint8_t* pData);

void app_notice_nit_change_for_search_show(void);
uint32_t app_win_check_video_win(void);

void app_hide_prompt(void);
void app_show_prompt(char* context);

#endif

/*
 * =====================================================================================
 *
 *       Filename:  app_input.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年09月14日 15时15分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __APP_INPUT_H__
#define __APP_INPUT_H__

#include "gxcore.h"

#define INPUT_KEY_NUM       (10)

/* input */
#define INPUT_NAME_MAX   (51)
typedef struct
{
    char        buf[INPUT_NAME_MAX];
    uint32_t	max_len;
    uint32_t    first_click;
    uint32_t    prev_key;
    uint32_t    cur_sel;
    uint32_t    cur_max;
    uint32_t    key_click;      /* 单个键按了的次数，换键后清零 */
    uint32_t    caps;           /* 0."a-z"  1."A-Z" */
    uint32_t    number;           /* 0.word  1.number */
    char*       (*name_get)(void);
    void        (*keypress)(uint32_t);
    status_t    (*init)(char*, uint32_t);
}AppInput;


extern AppInput     g_AppInput;

#endif

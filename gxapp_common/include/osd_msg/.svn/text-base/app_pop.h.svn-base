/*
 * =====================================================================================
 *
 *       Filename:  app_pop.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年08月18日 15时18分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#include "gxcore.h"
#include "gui_event.h"
#include "gui_core.h"
#include "app_key.h"


/******************** POP DIALOG ********************/

typedef enum                    
{                               
    POP_VAL_OK,                  
    POP_VAL_CANCEL,              
    POP_VAL_NONE,              
}PopDlgRet;

typedef enum                    
{                               
    POP_TYPE_NO_BTN,
    POP_TYPE_OK,
    POP_TYPE_YES_NO,    /* OK Cancel */
    POP_TYPE_WAIT,
}PopDlgType;   

typedef enum                    
{                               
    POP_MODE_BLOCK = 0,
    POP_MODE_UNBLOCK
}PopDlgMode; 

typedef enum                    
{                               
    POP_FORMAT_POPUP = 0,
    POP_FORMAT_DLG
}PopDlgFormat; 


typedef struct 
{
    uint16_t x;
    uint16_t y;
}PopDlgPos;

typedef int (*CreatCb)(void);
typedef int (*ExitCb)(PopDlgRet);
typedef struct
{
    PopDlgType          type; 
    PopDlgMode         mode;
    PopDlgFormat    format;
    char*               title;
    char*               str;
    PopDlgPos           pos;        /* pos.x = 0, used hcentre & vcentre */
    CreatCb creat_cb;
    ExitCb      exit_cb;
    uint32_t timeout_sec;
    PopDlgRet default_ret;
    bool show_time;
}PopDlg;

typedef enum                    
{                               
    PASSWD_OK,                  
    PASSWD_ERROR, 
    PASSWD_CANCEL
}PopPwdResult;

typedef struct
{
    PopPwdResult result;
    GUI_Event *event;
}PopPwdRet;
/******************** POP INPUT ********************/
typedef struct pop_keyboard PopKeyboard;
struct pop_keyboard
{   
    PopDlgPos pos;
    char *in_name;
    char *out_name;
    uint32_t max_num;
    PopDlgRet in_ret;
    void (*change_cb)(PopKeyboard*);
    void (*release_cb)(PopKeyboard*);
    void *usr_data;
};

/******************** POP PASSWD********************/
typedef enum
{
	KEY_MSG_KEEPON = 0,
	KEY_MSG_STOP
}PasswdKeyMsgState;
typedef int (*PasswdExitCB)(PopPwdRet *ret, void *usr_data);
typedef struct
{   
    PopDlgPos pos;  
    PasswdExitCB passwd_exit_cb;
    PasswdKeyMsgState passwd_key_msg;
    void *usr_data;
}PasswdDlg;

/******************** POP PASSWD********************/
typedef struct
{   
    PopDlgPos pos;  
    char *title;
    int item_num;
    char **item_content;
    int sel;
    bool show_num;
}PopList;

/******************** EXTERN FUNCTION ********************/
extern PopDlgRet popdlg_create(PopDlg* dlg);
extern void popdlg_destroy(void);
extern status_t multi_language_keyboard_create(PopKeyboard *keyboard);
extern status_t passwd_dlg_create(PasswdDlg *passwd_dlg);
extern int poplist_create(PopList* pop_list);
extern void app_pop_sort_create(PopDlgPos pop_pos, uint32_t prog_pos, void (*exit_cb)(PopDlgRet,uint32_t));
extern void app_ch_pid_pop(PopDlgPos pop_pos, uint32_t prog_id, void (*exit_cb)(PopDlgRet,uint32_t,uint32_t,uint32_t));


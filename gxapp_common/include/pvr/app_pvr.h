/*
 * =====================================================================================
 *
 *       Filename:  app_pvr.h
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

#ifndef __APP_PVR_H__
#define __APP_PVR_H__

#include "gxcore.h"


#define PLAYER_FOR_NORMAL    PLAYER0
#define PLAYER_FOR_TIMESHIFT		PLAYER_FOR_NORMAL
#define PLAYER_FOR_REC          "player2"
#define PVR_PLAYER      "player2"


/* *****************FUNCTION********************* */
#define PVR_DEV_LEN    (16)
//#define PVR_PATH_LEN    (64)
#define PVR_NAME_LEN    (64)
#define PVR_URL_LEN     GX_PM_MAX_PROG_URL_SIZE
#define PVR_DIR         "/GxPvr"
#define PVR_PARTITION_KEY   "pvr>partition"

#ifdef ECOS_OS
#define PVR_PARTITION          "/dev/usb01"
#endif
#ifdef LINUX_OS
#define PVR_PARTITION          "/dev/sda1"
#endif


typedef enum
{
    PVR_DUMMY,
    PVR_RECORD,
    PVR_TIMESHIFT,
    PVR_TMS_AND_REC,
    PVR_SPEED
}pvr_state;
#if 0
typedef struct
{
    uint32_t    base_tick;
    uint32_t    cur_tick;
    uint32_t    total_tick;
    uint32_t    rec_start;
    uint32_t    rec_duration;
}pvr_time;
#endif
typedef struct
{
    uint32_t    cur_tick;
    uint32_t    total_tick;
    uint32_t    remaining_tick;
    uint32_t    rec_duration;
}pvr_time;


typedef struct
{
    uint32_t    prog_id;
    char        *path;  // 录制的路径,动态申请
    char        *name;  // 录制的件名,动态申请
    char        *url;   // 发给player时移或录制的url，动态申请
}pvr_env;

typedef enum
{
    PVR_SPD_1    =   (1),
    PVR_SPD_2    =   (2),
    PVR_SPD_4    =   (4),
    PVR_SPD_8    =   (8),
    PVR_SPD_16   =   (16),
    PVR_SPD_24   =   (24)
}pvr_speed;

typedef enum
{
    USB_OUT = 0,
    USB_IN,
}usb_state;

typedef enum
{
    USB_OK = 0,
    USB_NO_SPACE,
    USB_ERROR,
}usb_check_state;

enum
{
    EVENT_TV_RADIO  ,   /* KEY TRIGGER */    
    EVENT_RECALL,
    EVENT_MUTE      ,   /* KEY TRIGGER */   
    EVENT_PAUSE     ,   /* KEY TRIGGER */   
    EVENT_SUBT      ,   /* KEY TRIGGER */   
    EVENT_TTX      ,   /* KEY TRIGGER */   
    EVENT_REC       ,   /* KEY TRIGGER */   
    EVENT_STATE       ,   /* TIMER TRIGGER */  
    EVENT_TOTAL
};

typedef struct 
{
    uint32_t    mute;
    uint32_t    pause;//时移状态
    uint32_t    subt;
    uint32_t    ttx;
    uint32_t    rec;//录像状态
    uint32_t    tms;
    uint32_t    tv;     /* tv- on means tv tv-off means radio */
#define STATE_ON    (1)
#define STATE_OFF   (0)
}EventState;

typedef struct _pvr_ops AppPvrOps;
struct _pvr_ops
{
    pvr_state       state;
    pvr_time        time;
    pvr_env         env;
    int32_t         spd;
    int32_t         usbstate;
    int32_t         stoppvr_flag;
    usb_check_state    (*usb_check)(AppPvrOps*);
    status_t        (*env_sync)(AppPvrOps*);
    void            (*env_clear)(AppPvrOps*);
    void            (*rec_start)(AppPvrOps*);
    void            (*rec_stop)(AppPvrOps*);   // RECORD only
    void            (*tms_stop)(AppPvrOps*); // TMS onlt
    void            (*pause)(AppPvrOps*);
    void            (*resume)(AppPvrOps*);
    void            (*seek)(AppPvrOps*, int64_t);
    void            (*speed)(AppPvrOps*, float);
    int32_t        (*percent)(AppPvrOps*);
    void            (*tms_delete)(AppPvrOps*);  // delete tms file
    //status_t        (*free_space)(AppPvrOps*);
};

extern AppPvrOps g_AppPvrOps;


extern status_t app_pvr_media_update(void);
extern uint8_t app_pvr_state_check(uint8_t flag, uint32_t pos);
#endif


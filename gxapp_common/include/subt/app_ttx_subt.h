/*
 * =====================================================================================
 *
 *       Filename:  app_ttx_subt.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年10月13日 09时13分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __APP_TTX_SUBT_H__
#define __APP_TTX_SUBT_H__

#include "gxcore.h"
#include "module/si/si_pmt.h"

typedef struct 
{
    uint16_t magazine;
    uint16_t page;
 }ttx_param;

typedef struct 
{
    uint16_t composite_page_id;
    uint16_t ancillary_page_id;
}subt_param;

#define TTX_SUBT_TOTAL  (16)
typedef struct
{
    uint32_t elem_pid;
    uint8_t lang[3];
    uint8_t type;
    union
    {
        ttx_param   ttx;
        subt_param  subt;
    };
}ttx_subt;

typedef struct ttx_subt_ops TtxSubtOps;
struct ttx_subt_ops
{
    ttx_subt  magazine;
    PlayerSubtitle* player_subt;

    ttx_subt  content[TTX_SUBT_TOTAL];
    uint32_t  ttx_num;
    uint32_t  subt_num;
    uint32_t  sync_flag;

    void (*sync)(TtxSubtOps*,PmtInfo *pmt);

    status_t (*ttx_magz_open)(TtxSubtOps*);    // close will do automatic in ttx_magz_ops magz=magazine
    status_t (*ttx_magz_opt)(TtxSubtOps*,uint16_t); // param:key value
    
    status_t (*ttx_subt_open)(TtxSubtOps*,uint32_t);    // pos of ttx, like choose language
    void     (*ttx_subt_close)(TtxSubtOps*);
    
    status_t (*dvb_subt_open)(TtxSubtOps*,uint32_t);    // pos of subt, like choose language
    void     (*dvb_subt_close)(TtxSubtOps*);
	void     (*clean)(TtxSubtOps*);
	void     (*ttx_pre_process_init)(TtxSubtOps*);
	void     (*ttx_pre_process_close)(TtxSubtOps*);
	void     (*ttx_config)(void);
};

extern TtxSubtOps g_AppTtxSubt;
#endif


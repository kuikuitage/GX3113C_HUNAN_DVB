/*
 * =====================================================================================
 *
 *       Filename:  app_id_manage.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年09月14日 15时13分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __APP_ID_MANAGE_H__
#define __APP_ID_MANAGE_H__
#include "gxcore.h"

/**
 * manage the id 
 */
typedef struct id_ops AppIdOps;
struct id_ops
{
	status_t (*id_open)(AppIdOps*, int32_t, uint32_t);	// (mode, total number)prepare buffer for id operation, param:id total number return:success or faild
	status_t (*id_add)(AppIdOps*, uint32_t, uint32_t);		// (pos, id)add one id to the manage buffer
	status_t (*id_delete)(AppIdOps*, uint32_t);	// (pos)delete one id from the manage buffer,one delete the follow need move front
	status_t (*id_close)(AppIdOps*);	// GxCore_Free the buffer alloc when open, and finish the manager operation
	uint32_t (*id_check)(AppIdOps*, uint32_t);	// (same list: pos   diff list : id) check the postion is set effectively id, 1 exist  0 un exist
	status_t (*id_pos_get)(AppIdOps*, uint32_t*);
	status_t (*id_get)(AppIdOps*, uint32_t*);	// use pos to get id! param:position  return:id value
	uint32_t (*id_total_get)(AppIdOps*);	// the total id number which have been modified
// mode in open
#define MANAGE_SAME_LIST  0
#define MANAGE_DIFF_LIST 1
};

#endif


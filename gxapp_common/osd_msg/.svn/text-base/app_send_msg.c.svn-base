/*****************************************************************************
*             CONFIDENTIAL                              
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2009, All right reserved
******************************************************************************

******************************************************************************
* File Name :   xxx.c
* Author    :   xxx
* Project   :   GoXceed -S
******************************************************************************
* Purpose   :   
******************************************************************************
* Release History:
  VERSION   Date              AUTHOR         Description
   0.0      2009.12.04              xxx         creation
*****************************************************************************/

/* Includes --------------------------------------------------------------- */

/* Private types/constants ------------------------------------------------ */

/* Private Functions ----------------------------------------------------- */

/* Exported Functions ----------------------------------------------------- */
#include <gxmsg.h>
#include <gxbus.h>
#include <service/gxbook.h>
#include "gxupdate.h"
#include "app_send_msg.h"
//#include "app_nim.h"
#include "gxavdev.h"
#if (DVB_DEMOD_MODE == DVB_DEMOD_DVBS)

#define CHK_RESULT(num)\
((num)==-1?GXCORE_ERROR:GXCORE_SUCCESS)

typedef status_t (*msg_func)(uint32_t msg_id, void* params);


static  status_t  gxmsg_node_num_get(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeNumGet *numGet = NULL;
    int32_t number = -1;
    
    
    if(NULL == params ||GXMSG_PM_NODE_NUM_GET !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }

    numGet = (GxMsgProperty_NodeNumGet*)params;

    switch(numGet->node_type)
    {
        case NODE_SAT:
        {
            number = GxBus_PmSatNumGet();
            break;
        }
        case NODE_TP:
        {
            number = GxBus_PmTpNumGetBySat(numGet->sat_id);         
            break;
        }
        case NODE_TP_ALL:
        {
            number = GxBus_PmTpNumGet();    
            break;
        }
        case NODE_PROG:
        {
            number = GxBus_PmProgNumGet();
            break;
        }
        case NODE_CAS:
        {
            number = GxBus_PmCasNumGet();
            break;
        }
        case NODE_FAV_GROUP:
        {
            number = GxBus_PmFavGroupNumGet();
            break;
        }
        default:
        {
            number = -1;
            break;
        }
    }

    if(-1 == number)
    {
        printf("can not get the number of PM successfully  numGet->node_type = %d\n",numGet->node_type);
        numGet->node_num = 0;
        return GXCORE_ERROR;
    }

    else
    {
        numGet->node_num = number;
        return GXCORE_SUCCESS;
    }
    
}

static  status_t  gxmsg_node_by_pos_get(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeByPosGet *nodeGet = NULL;
    status_t ret  = GXCORE_ERROR;
    
    
    if(NULL == params ||GXMSG_PM_NODE_BY_POS_GET !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }

    nodeGet = (GxMsgProperty_NodeByPosGet*)params;

    switch(nodeGet->node_type)
    {
        case NODE_SAT:
        {
            int32_t number = -1; 
            number =  GxBus_PmSatsGetByPos(nodeGet->pos,1,&(nodeGet->sat_data));
            ret = CHK_RESULT(number);
            break;
        }
        case NODE_TP:
        {   
            int32_t number = -1; 
            number = GxBus_PmTpGetByPosInSat(nodeGet->pos,1,&(nodeGet->tp_data));
            ret = CHK_RESULT(number);
            break;  
        }
        case NODE_TP_ALL:
        {
            int32_t number = -1; 
            number = GxBus_PmTpGetByPos(nodeGet->pos,1,&(nodeGet->tp_data));
            ret = CHK_RESULT(number);
            break;          
        }
        case NODE_PROG:
        {
            int32_t number = -1;
            number = GxBus_PmProgGetByPos(nodeGet->pos,1,&(nodeGet->prog_data));
            ret = CHK_RESULT(number);
            break;
        }
        case NODE_CAS:
        {
            ret = GxBus_PmCasGetByPos(nodeGet->pos,&(nodeGet->cas_info));
            break;
        }
        case NODE_FAV_GROUP:
        {
            int32_t number = -1; 
            number = GxBus_PmFavGroupGetByPos(nodeGet->pos,1,&(nodeGet->fav_item));
            ret = CHK_RESULT(number);
            break;
        }
        default:
        {
            ret = GXCORE_ERROR;
            break;
        }
    }

    if(GXCORE_ERROR == ret)
    {
        printf("[%s]%d,can not get PM node by pos! nodeGet->node_type = %d\n", __FILE__, __LINE__, nodeGet->node_type);
    }

    return ret;
    
}




static  status_t  gxmsg_node_by_id_get(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeByIdGet *nodeGet = NULL;
    status_t ret  = GXCORE_ERROR;
    
    
    if(NULL == params ||GXMSG_PM_NODE_BY_ID_GET !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }

    nodeGet = (GxMsgProperty_NodeByIdGet*)params;

    switch(nodeGet->node_type)
    {
        case NODE_SAT:
        {
            ret =  GxBus_PmSatGetById(nodeGet->id,&(nodeGet->sat_data));
            break;
        }
        case NODE_TP:
        {   
            ret =  GxBus_PmTpGetById(nodeGet->id,&(nodeGet->tp_data));
            break;
        }
        case NODE_PROG:
        {
            ret =  GxBus_PmProgGetById(nodeGet->id,&(nodeGet->prog_data));
            break;
        }
        case NODE_CAS:
        {
            ret =  GxBus_PmCasGetById(nodeGet->id,&(nodeGet->cas_info));
            break;
        }
        case NODE_FAV_GROUP:
        {
            ret =  GxBus_PmFavGroupGetById(nodeGet->id,&(nodeGet->fav_item));
            break;
        }
        default:
        {
            ret = GXCORE_ERROR;
            break;
        }
    }

    if(GXCORE_ERROR == ret)
    {
        printf("can not get PM node by id! nodeGet->node_type = %d\n",nodeGet->node_type);
    }
    
    return ret;
}



static  status_t  gxmsg_pm_viewinfo_set(uint32_t msg_id,void* params)
{
    GxBusPmViewInfo *viewinfo = NULL ;
    status_t ret  = GXCORE_ERROR;
    
    if(NULL == params ||    GXMSG_PM_VIEWINFO_SET !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }
    
    viewinfo = (GxBusPmViewInfo*)params;
     
    ret = GxBus_PmViewInfoModify(viewinfo);

    if(GXCORE_ERROR == ret)
    {
        printf("modify view info error!\n");
    }
    return ret;
}




static  status_t  gxmsg_node_delete(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeDelete *nodeDel = NULL;
    GxBusPmSyncType syncType = 0;
    status_t ret  = GXCORE_ERROR;
    
    
    if(NULL == params ||GXMSG_PM_NODE_DELETE !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }
    nodeDel = (GxMsgProperty_NodeDelete*)params;

    switch(nodeDel->node_type)
    {
        case NODE_SAT:
        {
            syncType = GXBUS_PM_SYNC_SAT;
            ret = GxBus_PmSatDelete(nodeDel->id_array,nodeDel->num);
            break;
        }
        case NODE_TP:
        {
            syncType = GXBUS_PM_SYNC_TP;
            ret = GxBus_PmTpDelete(nodeDel->id_array,nodeDel->num);
            break;
        }
        case NODE_PROG:
        {
            syncType = GXBUS_PM_SYNC_PROG;
            ret = GxBus_PmProgDelete(nodeDel->id_array,nodeDel->num);
            break;
        }
        default:
        {
            ret = GXCORE_ERROR;
            break;
        }
    }

    if(GXCORE_ERROR == ret)
    {
        printf("can not delete the node of PM! nodeDel->node_type = %d\n",nodeDel->node_type);
    }
    else
    {
        ret = GxBus_PmSync(syncType);
    }

    return ret;
}

static  status_t  gxmsg_node_add(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeAdd *nodeAdd = NULL;
    GxBusPmSyncType syncType = 0;
    status_t ret  = GXCORE_ERROR;
    
    
    if(NULL == params ||GXMSG_PM_NODE_ADD !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }
    nodeAdd = (GxMsgProperty_NodeAdd*)params;
    
    switch(nodeAdd->node_type)
    {
        case NODE_SAT:
        {
            syncType = GXBUS_PM_SYNC_SAT;
            ret = GxBus_PmSatAdd(&(nodeAdd->sat_data));
            break;
        }
        case NODE_TP:
        {   
            syncType = GXBUS_PM_SYNC_TP;
            ret = GxBus_PmTpAdd(&(nodeAdd->tp_data));
            break;
        }
        case NODE_PROG:
        {
            syncType = GXBUS_PM_SYNC_PROG;
            ret = GxBus_PmProgAdd(&(nodeAdd->prog_data));
            break;
        }
        default:
        {
            ret = GXCORE_ERROR;
            break;
        }
    }

    if(GXCORE_ERROR == ret)
    {
        printf("can not add node into PM! nodeAdd->node_type = %d\n",nodeAdd->node_type);
    }
    else
    {
        ret = GxBus_PmSync(syncType);
    }
    
    return ret;
}



static  status_t  gxmsg_node_modify(uint32_t msg_id,void* params)
{
    GxMsgProperty_NodeModify *nodeModify = NULL;
    GxBusPmSyncType syncType = 0;
    status_t ret  = GXCORE_ERROR;
    
    
    if(NULL == params ||GXMSG_PM_NODE_MODIFY !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }
    nodeModify = (GxMsgProperty_NodeModify*)params;
    
    switch(nodeModify->node_type)
    {
        case NODE_SAT:
        {
            syncType = GXBUS_PM_SYNC_SAT;
            ret = GxBus_PmSatModify(&(nodeModify->sat_data));
            break;
        }
        case NODE_TP:
        {   
            syncType = GXBUS_PM_SYNC_TP;
            ret = GxBus_PmTpModify(&(nodeModify->tp_data));
            break;
        }
        case NODE_PROG:
        {
            syncType = GXBUS_PM_SYNC_PROG;
            ret = GxBus_PmProgInfoModify(&(nodeModify->prog_data));
            break;
        }
        case NODE_FAV_GROUP:
        {
            syncType = GXBUS_PM_SYNC_FAV;
            ret = GxBus_PmFavGroupModify(&(nodeModify->fav_item));
            break;
        }
        default:
        {
            ret = GXCORE_ERROR;
            break;
        }
    }

    if(GXCORE_ERROR == ret)
    {
        printf("can not modify the node of PM! nodeModify->node_type = %d\n",nodeModify->node_type);
    }
    else
    {
        ret = GxBus_PmSync(syncType);
    }

    return ret; 
}

static  status_t  gxmsg_pm_viewinfo_get(uint32_t msg_id,void* params)
{
    GxBusPmViewInfo *viewinfo = NULL ;
    status_t ret  = GXCORE_ERROR;

    if(NULL == params ||    GXMSG_PM_VIEWINFO_GET !=  msg_id)
    {
        printf("msg id or params is incorrect! msg_id = %d\n",msg_id);
        return GXCORE_ERROR;
    }

    viewinfo = (GxBusPmViewInfo*)params;
     
    ret = GxBus_PmViewInfoGet(viewinfo);

    if(GXCORE_ERROR == ret)
    {
        printf("get view info error!\n");
    }
    
    return ret;
}

#define DEF_APP_SEND_MSG_SYNC(fun_name,id,msg_type)                 \
static  status_t fun_name(uint32_t msg_id ,  void* params)                      \
{                                                                       \
    GxMessage *msg = NULL;                                              \
    status_t ret = GXCORE_ERROR;                                            \
    msg_type *data = NULL;                                              \
    msg_type *out_in_param = NULL;                                      \
                                                                        \
                                                                        \
    if(NULL == params ||id !=  msg_id)                                      \
    {                                                                   \
        printf("params or msg id is incorrect!msg_id = %d\n",msg_id);      \
        return GXCORE_ERROR;                                            \
    }                                                                   \
                                                                        \
    msg = GxBus_MessageNew(id);                                     \
    if(NULL == msg)                                                     \
    {                                                                   \
        printf("create the new message failed!msg_id = %d\n",id);                      \
        return GXCORE_ERROR;                                            \
    }                                                                   \
                                                                        \
    data = (msg_type*)GxBus_GetMsgPropertyPtr(msg, msg_type);           \
    if(NULL == data)                                                        \
    {                                                                   \
		GxBus_MessageFree(msg);											\
        printf("can not get the params's memory from msg !\n");          \
        return GXCORE_ERROR;                                            \
    }                                                                   \
    out_in_param = (msg_type*)params;                                   \
                                                                        \
    memcpy(data,out_in_param,sizeof(msg_type));                         \
                                                                        \
    ret = GxBus_MessageSendWait(msg);                                   \
    if(GXCORE_SUCCESS != ret)                                             \
    {                                                                   \
		GxBus_MessageFree(msg);											\
        printf("send the massage failed !");                             \
        return GXCORE_ERROR;                                            \
    }                                                                   \
    memcpy(out_in_param,data,sizeof(msg_type));                         \
                                                                        \
    ret = GxBus_MessageFree(msg);                                       \
    if(GXCORE_SUCCESS != ret)                                             \
    {                                                                   \
        printf("GxCore_Free the message failed !\n");                           \
        return GXCORE_ERROR;                                            \
    }                                                                   \
                                                                        \
    return GXCORE_SUCCESS;                                              \
}
DEF_APP_SEND_MSG_SYNC(gxmsg_frontend_set_tp,GXMSG_FRONTEND_SET_TP,GxMsgProperty_FrontendSetTp)
DEF_APP_SEND_MSG_SYNC(gxmsg_frontend_set_diseqc,GXMSG_FRONTEND_SET_DISEQC,GxMsgProperty_FrontendSetDiseqc)
DEF_APP_SEND_MSG_SYNC(gxmsg_frontend_monitor_start_asyn,GXMSG_FRONTEND_START_MONITOR, GxMsgProperty_FrontendMonitor)
DEF_APP_SEND_MSG_SYNC(gxmsg_frontend_monitor_stop_asyn,GXMSG_FRONTEND_STOP_MONITOR, GxMsgProperty_FrontendMonitor)

/**
 * @brief       发送消息
 * @param       msg_id:消息的id
                params消息的参数

 * @return      消息发送成功返回GXCORE_SUCCESS
                消息发送失败返回GXCORE_ERROR
 */
status_t app_send_msg_exec(uint32_t msg_id,void* params)
{
    if(GXMAX_MSG_NUM <= msg_id)
    {
        printf("msg id is incorrect\n");
        return GXCORE_ERROR;
    }

    if(NULL== msg_id)
    {
        printf("the msg handling funtion is NULL msg_id = %d \n",msg_id);
        return GXCORE_ERROR;
    }
if(msg_id==GXMSG_PM_NODE_BY_ID_GET)
{
return gxmsg_node_by_id_get( msg_id,params);
}
if(msg_id==GXMSG_PM_NODE_BY_POS_GET)
{
 return gxmsg_node_by_pos_get( msg_id,params);
}
if(msg_id==GXMSG_PM_NODE_MODIFY)
{
 return gxmsg_node_modify( msg_id,params);
}
if(msg_id==GXMSG_PM_NODE_NUM_GET)
{
 return gxmsg_node_num_get( msg_id,params);
}
if(msg_id==GXMSG_PM_NODE_ADD)
{
 return gxmsg_node_add( msg_id,params);
}
if(msg_id==GXMSG_PM_NODE_DELETE)
{
 return gxmsg_node_delete( msg_id,params);
}
if(msg_id==GXMSG_FRONTEND_SET_TP)
{
return gxmsg_frontend_set_tp( msg_id,params);
}
if(msg_id==GXMSG_FRONTEND_SET_DISEQC)
{
return gxmsg_frontend_set_diseqc( msg_id,params);
}
if(msg_id==GXMSG_FRONTEND_START_MONITOR)
{
return gxmsg_frontend_monitor_start_asyn( msg_id,params);
}
if(msg_id==GXMSG_FRONTEND_STOP_MONITOR)
{
return gxmsg_frontend_monitor_stop_asyn( msg_id,params);
}
}
/* End of file -------------------------------------------------------------*/
#endif

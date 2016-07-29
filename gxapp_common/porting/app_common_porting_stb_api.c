/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_ca_api.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.12.02		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "gxapp_sys_config.h"
#include "app_common_play.h"
#include "app_common_table_pmt.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_os.h"
#include "gxmsg.h"

typedef struct
{
	GxMessage *msg;
	void *param;

}ca_msg;

#define MAX_CA_MSG_NUM (256)
ca_msg gCa_msg_list[MAX_CA_MSG_NUM];


app_cas_update_dvb_ca_flag_callback m_cas_update_dvb_ca_flag_callback[DVB_CA_TYPE_MAX];

APP_CAS_API_ControlBlock_t gs_AppCasPortControlBlock={0};
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "app_cd_cas_3.0_api_smart.h"	
    extern APP_CAS_API_ControlBlock_t CdCas30PortControlBlock;
#endif
#ifdef DVB_CA_TYPE_BY_FLAG
#include "app_by_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t ByCasPortControlBlock;
#endif

#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#include "app_desai_cas_5.0_api_smart.h"
    extern APP_CAS_API_ControlBlock_t DesaiCas50PortControlBlock;
#endif
#ifdef DVB_CA_TYPE_KN_FLAG
#include "app_kn_cas_api_smart.h"
	APP_CAS_API_ControlBlock_t knCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
#include "app_xinshimao_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t XinShiMaoCaPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
#include "app_divitone_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t DivitoneCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_DVT_FLAG
#include "app_dvt_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t DvtCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_QILIAN_FLAG
#include "app_qilian_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t QilianCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_MG_FLAG
#include "app_mg_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t MgCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
#include "app_mg_cas312_api_smart.h"
    extern APP_CAS_API_ControlBlock_t MgCas312PortControlBlock;
#endif
#ifdef DVB_CA_TYPE_QZ_FLAG
#include "app_qz_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t QzCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_GOS_FLAG
#include "app_gos_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t GosCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
#include "app_gy_cas_api_smart.h"	
    extern APP_CAS_API_ControlBlock_t GyCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_WF_CA_FLAG
#include "app_wf_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t WfCasPortControlBlock;
#endif
#ifdef DVB_CA_TYPE_TR_FLAG
#include "app_tr_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t TRCasPortControlBlock;
#endif


#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
#include "app_abv_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t abvCas53PortControlBlock;
#endif

#ifdef DVB_CA_TYPE_KP_FLAG
#include "app_kp_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t KpCasPortControlBlock;
#endif
#endif

#ifdef DVB_CA_TYPE_DVB_FLAG
#include "app_dvb_cas_api_smart.h"
    extern APP_CAS_API_ControlBlock_t DvbCasPortControlBlock;
#endif


/*
* 此文件函数实现提供所有界面调用CA接口
* 具体CA API功能在对应模块app_xxxcasxxx_api_xxxx.c 如app_cdcas30_api_entitle.c中实现
*/

/*
* 以下接口提供COMMON 、非CA菜单调用
*/

/*
* 初始化
* 返回值: 0 --CA初始失败，1-- CA初始化成功
*/

uint8_t app_cas_update_dvb_ca_flag(void)
{
	uint32_t i = 0;
	uint8_t count = 0;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	for (i = DVB_CA_TYPE_NONE+1;i < DVB_CA_TYPE_MAX; i++ )
		{
			m_cas_update_dvb_ca_flag_callback[i] = NULL;
			switch(i)
				{
					case DVB_CA_TYPE_DVB:
#ifdef DVB_CA_TYPE_DVB_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_dvb_cas_api_smart_check_valid;
#endif
						break;
					case DVB_CA_TYPE_BY:
#ifdef DVB_CA_TYPE_BY_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_by_cas_api_smart_check_valid;				
#endif			
						break;
					case DVB_CA_TYPE_CDCAS30:
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_cd_cas30_api_smart_check_valid;				
#endif		
						break;

					case DVB_CA_TYPE_KN:
#ifdef DVB_CA_TYPE_KN_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_kn_cas_api_smart_check_valid;				
#endif	
						break;	
					case DVB_CA_TYPE_XINSHIMAO:
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_xinshimao_cas_api_smart_check_valid;				
#endif		
						break;
			case DVB_CA_TYPE_DIVITONE:
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
				m_cas_update_dvb_ca_flag_callback[i] = app_divitone_cas_api_smart_check_valid; 			
#endif		
				break;
			case 	DVB_CA_TYPE_DSCAS50:
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
				m_cas_update_dvb_ca_flag_callback[i] = app_desai_cas50_api_smart_check_valid; 			
#endif		
				break;
			case DVB_CA_TYPE_DVT:
#ifdef DVB_CA_TYPE_DVT_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_dvt_cas_api_smart_check_valid; 			
#endif		
						break;
					case DVB_CA_TYPE_QILIAN:
#ifdef DVB_CA_TYPE_QILIAN_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_qilian_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_ABV53:
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_abv_cas53_api_smart_check_valid; 			
#endif		
						break;
					case DVB_CA_TYPE_MG:
#ifdef DVB_CA_TYPE_MG_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_mg_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_QZ:
#ifdef DVB_CA_TYPE_QZ_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_qz_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_GOS:
#ifdef DVB_CA_TYPE_GOS_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_gos_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_GY:
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_gy_cas_api_smart_check_valid;				
#endif		
						break;

					case DVB_CA_TYPE_WF:
#ifdef DVB_CA_TYPE_WF_CA_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_wf_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_KP:
#ifdef DVB_CA_TYPE_KP_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_kp_cas_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_MG312:
#ifdef DVB_CA_TYPE_MG312_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_mg_cas312_api_smart_check_valid;				
#endif		
						break;
					case DVB_CA_TYPE_TR:
#ifdef DVB_CA_TYPE_TR_FLAG
						m_cas_update_dvb_ca_flag_callback[i] = app_tr_cas_api_smart_check_valid;				
#endif	
						break;
					default:
						break;
				}

			
		}

	/*
	* first check smartcard type is same equal flash save flag or not 
	*/
	if (NULL != m_cas_update_dvb_ca_flag_callback[dvb_ca_flag])
		{
			if (TRUE == m_cas_update_dvb_ca_flag_callback[dvb_ca_flag]())
				return TRUE;
		}
	/*else
		{
			return FALSE;
		}*/
	
	/*
	* smartcard invalid or error ,judge is other ca card or not (dual ca support)
	*/
	for (i = DVB_CA_TYPE_NONE+1;i < DVB_CA_TYPE_MAX; i++ )
		{
			if (i == dvb_ca_flag)
				continue;

			if (NULL != m_cas_update_dvb_ca_flag_callback[i])
				{
					count++;
					if (TRUE == m_cas_update_dvb_ca_flag_callback[i]())
						{
							app_flash_save_config_dvb_ca_flag(i);	
							return TRUE;
						}
				}			
		}

	/*
	* check the smartcard error or no smartcard , init the flash flag type
	*/
	if (count > 0)
		{
			if (NULL != m_cas_update_dvb_ca_flag_callback[dvb_ca_flag])
				{
					if (TRUE == m_cas_update_dvb_ca_flag_callback[dvb_ca_flag]())
						return TRUE;
					else
						return FALSE;
				}
			else
				{
					return FALSE;
				}		
		}
	return FALSE;
	
}
//ca模块初始化
uint8_t app_cas_init(void)
{
	play_scramble_para_t playpara = {0};
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	app_play_get_playing_para(&playpara);

	memset(&gs_AppCasPortControlBlock,0,sizeof(APP_CAS_API_ControlBlock_t));
	if (dvb_ca_flag >= DVB_CA_TYPE_MAX)
		return 0;
	switch(dvb_ca_flag)
		{
			case DVB_CA_TYPE_DVB:
#ifdef DVB_CA_TYPE_DVB_FLAG
				memcpy(&gs_AppCasPortControlBlock,&DvbCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));
#endif
				break;
			case DVB_CA_TYPE_BY:
#ifdef DVB_CA_TYPE_BY_FLAG
				memcpy(&gs_AppCasPortControlBlock,&ByCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));				
#endif			
				break;
			case DVB_CA_TYPE_CDCAS30:
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
				memcpy(&gs_AppCasPortControlBlock,&CdCas30PortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));								

#endif		
				break;
			case DVB_CA_TYPE_DSCAS50:
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
				memcpy(&gs_AppCasPortControlBlock,&DesaiCas50PortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));								
#endif	
			break;
			case DVB_CA_TYPE_KN:
#ifdef DVB_CA_TYPE_KN_FLAG  
				memcpy(&gs_AppCasPortControlBlock,&knCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));								

#endif	
				break;
			case DVB_CA_TYPE_XINSHIMAO:
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
				memcpy(&gs_AppCasPortControlBlock,&XinShiMaoCaPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t)); 								
#endif		
				break;
			case DVB_CA_TYPE_DIVITONE:
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
				memcpy(&gs_AppCasPortControlBlock,&DivitoneCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t)); 								
#endif		
				break;
			case DVB_CA_TYPE_DVT:
#ifdef DVB_CA_TYPE_DVT_FLAG
				memcpy(&gs_AppCasPortControlBlock,&DvtCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t)); 								
#endif		
				break;
			case DVB_CA_TYPE_QILIAN:
#ifdef DVB_CA_TYPE_QILIAN_FLAG
				//m_cas_update_dvb_ca_flag_callback[i] = app_rk_cas_api_smart_check_valid;			
				memcpy(&gs_AppCasPortControlBlock,&QilianCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t)); 		
#endif		
				break;
			case DVB_CA_TYPE_MG:
#ifdef DVB_CA_TYPE_MG_FLAG
				memcpy(&gs_AppCasPortControlBlock,&MgCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_QZ:
#ifdef DVB_CA_TYPE_QZ_FLAG
				memcpy(&gs_AppCasPortControlBlock,&QzCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_GOS:
#ifdef DVB_CA_TYPE_GOS_FLAG
				memcpy(&gs_AppCasPortControlBlock,&GosCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif	
				break;
			case DVB_CA_TYPE_ABV53:
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
				memcpy(&gs_AppCasPortControlBlock,&abvCas53PortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));
#endif		
				break;
			case DVB_CA_TYPE_GY:
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
				memcpy(&gs_AppCasPortControlBlock,&GyCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_WF:
#ifdef DVB_CA_TYPE_WF_CA_FLAG
				memcpy(&gs_AppCasPortControlBlock,&WfCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_KP:
#ifdef DVB_CA_TYPE_KP_FLAG
				memcpy(&gs_AppCasPortControlBlock,&KpCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_MG312:
#ifdef DVB_CA_TYPE_MG312_FLAG
				memcpy(&gs_AppCasPortControlBlock,&MgCas312PortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			case DVB_CA_TYPE_TR:
#ifdef DVB_CA_TYPE_TR_FLAG
				memcpy(&gs_AppCasPortControlBlock,&TRCasPortControlBlock,sizeof(APP_CAS_API_ControlBlock_t));		
#endif		
				break;
			default:
				break;
		}

	playpara.program_num = 0xFFFF;
	playpara.program_type = FALSE;
	playpara.scramble_type = 0;
	playpara.p_video_pid = PSI_INVALID_PID;
	playpara.p_audio_pid = PSI_INVALID_PID;
	playpara.p_ecm_pid = PSI_INVALID_PID;
	playpara.p_ecm_pid_video = PSI_INVALID_PID;
	playpara.p_ecm_pid_audio = PSI_INVALID_PID;
	playpara.p_emm_pid = PSI_INVALID_PID;
 
	app_play_set_playing_para(&playpara);
	memset(&gCa_msg_list[0],0,MAX_CA_MSG_NUM*sizeof(ca_msg));

    //模块初始化
	if (NULL != gs_AppCasPortControlBlock.m_cas_init_callback)
		return gs_AppCasPortControlBlock.m_cas_init_callback();
	
	return 0;
}

uint8_t app_cas_close(void)
{
	if (NULL != gs_AppCasPortControlBlock.m_cas_close_callback)
		return gs_AppCasPortControlBlock.m_cas_close_callback();
	
	return 0;
}

/*
* 释放ecm filter , 切台、停止播放、恢复出厂设置等情况下调用
* 返回值: 0 --释放ECM FILTER失败，1-- 释放ECM FILTER成功
*/
uint8_t	app_cas_api_release_ecm_filter(void)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_release_ecm_filter_callback)
		return gs_AppCasPortControlBlock.m_cas_api_release_ecm_filter_callback();
	
	return 0;

}

/*
* 设置ecm filter , 切台调用，有些CA可能需要绑定解扰通道
* 根据不同CA实际实现。
* 解析到PMT表时候调用
*/
uint8_t app_cas_api_start_ecm_filter(play_scramble_para_t *playPara)
{
	play_scramble_para_t oldplaypara = {0};
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (NULL == playPara)
		return 0;
	app_play_get_playing_para(&oldplaypara);

	if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
	{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
		if ((0 == memcmp(&oldplaypara, playPara, sizeof(play_scramble_para_t)))
				&& (oldplaypara.scramble_type) )

		{
			/*
			 * 对应节目ECM-FILTER已设置，不用重复设置过滤器
			 */
			return 0;
		}
#endif
	}
	else
	{	
		if ((0 == memcmp(&oldplaypara, playPara, sizeof(play_scramble_para_t))))
		{
			/*
			 * 对应节目ECM-FILTER已设置，不用重复设置过滤器
			 */
			return 0;
		}
	}
	app_play_set_playing_para(playPara);		

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_start_ecm_filter_callback)
		return gs_AppCasPortControlBlock.m_cas_api_start_ecm_filter_callback(playPara);

	return 0;

}

/*
 * 释放emm filter, 切换频点、恢复出厂设置等情况下调用
 */
uint8_t	app_cas_api_release_emm_filter(void)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_release_emm_filter_callback)
		return gs_AppCasPortControlBlock.m_cas_api_release_emm_filter_callback();

	return 0;

}

/*
 * 设置emm filter, 切台过滤到CAT表调用
 */
uint8_t app_cas_api_start_emm_filter(uint16_t emm_pid)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_start_emm_filter_callback)
		return gs_AppCasPortControlBlock.m_cas_api_start_emm_filter_callback(emm_pid);

	return 0;

}


/*
 * 判断是否对应CA系统描述子
 * 返回TURE -- 对应CA系统CA描述子
 * 返回FALSE -- 非对应CA系统CA描述子
 */
bool app_cas_api_check_cat_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id)
{
	if (NULL == sectionData)
		return FALSE;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_check_cat_ca_descriptor_valid_callback)
		return gs_AppCasPortControlBlock.m_cas_api_check_cat_ca_descriptor_valid_callback(sectionData,CA_system_id);

	return FALSE;

}
/*
 * 判断是否对应CA系统描述子
 * 返回TURE -- 对应CA系统CA描述子
 * 返回FALSE -- 非对应CA系统CA描述子
 */
bool app_cas_api_check_pmt_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id)
{
	if (NULL == sectionData)
		return FALSE;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_check_pmt_ca_descriptor_valid_callback)
		return gs_AppCasPortControlBlock.m_cas_api_check_pmt_ca_descriptor_valid_callback(sectionData,CA_system_id);

	return FALSE;

}

uint32_t app_cas_api_check_bus_pmt_ca_valid(uint16_t ca_system_id,uint16_t ele_pid,uint16_t ecm_pid)
{
	if (NULL != gs_AppCasPortControlBlock.m_cas_api_check_pmt_ca_descriptor_valid_callback)
		return gs_AppCasPortControlBlock.m_cas_api_check_pmt_ca_descriptor_valid_callback(NULL,ca_system_id);
	return 0;	
}

/*提示消息、弹出框、滚动消息等*/

/*
 * 判断是否CA提示消息
 */
uint8_t app_cas_api_is_ca_pop_msg_type(uint8_t type)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_is_ca_pop_msg_type_callback)
		return gs_AppCasPortControlBlock.m_cas_api_is_ca_pop_msg_type_callback(type);

	return 0;

}
/*
 * 显示、隐藏无授权等提示
 */
int32_t app_cas_api_pop_msg_exec(uint8_t type)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_pop_msg_exec_callback)
		return gs_AppCasPortControlBlock.m_cas_api_pop_msg_exec_callback(type);

	return 0;

}


/*
 * 更新显示CA相关信息，如OSD滚动消息，邮件提醒，指纹等
 */
//200ms 定时器
int32_t app_cas_api_osd_exec(void)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_osd_exec_callback)
		return gs_AppCasPortControlBlock.m_cas_api_osd_exec_callback();

	return 0;
}

/*
 * 实现CA弹出框功能，如IPPV/IPPT购买框，成人密码框，OSD滚动消息更新等
 */
int32_t app_cas_api_gxmsg_ca_on_event_exec(GxMessage * msg)
{
	if (NULL == msg)
		return 0;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_gxmsg_ca_on_event_exec_callback)
		return gs_AppCasPortControlBlock.m_cas_api_gxmsg_ca_on_event_exec_callback(msg);

	return 0;

}


/*
 * 以下接口CA菜单调用
 */


/*
 *  获取对应数据之前，初始化数据管理
 (一般创建对应窗体之前或create中调用)。
 * 如初始化邮件、授权等
 */
int32_t app_cas_api_init_data(dvb_ca_data_type_t date_type)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_init_data_callback)
		return gs_AppCasPortControlBlock.m_cas_api_init_data_callback(date_type);

	return 0;		
}

char * app_cas_api_get_data(ca_get_date_t *data)
{

	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_get_data_callback)
		return gs_AppCasPortControlBlock.m_cas_api_get_data_callback(data);

	return NULL;	
}

uint8_t app_cas_api_get_count(ca_get_count_t *data)
{

	if (NULL == data)
		return 0;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_get_count_callback)
		return gs_AppCasPortControlBlock.m_cas_api_get_count_callback(data);

	return 0;		
}

bool app_cas_api_delete(ca_delete_data_t *data)
{

	if (NULL == data)
		return FALSE;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_delete_callback)
		return gs_AppCasPortControlBlock.m_cas_api_delete_callback(data);

	return FALSE;	
}

bool app_cas_api_delete_all(ca_delete_data_t *data)
{

	if (NULL == data)
		return FALSE;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_delete_all_callback)
		return gs_AppCasPortControlBlock.m_cas_api_delete_all_callback(data);

	return FALSE;		
}

uint8_t app_cas_api_buy_ipp(ca_buy_ipp_t *data)
{
	if (NULL == data)
		return 0;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_buy_ipp_callback)
		return gs_AppCasPortControlBlock.m_cas_api_buy_ipp_callback(data);

	return 0;		
}

uint8_t app_cas_api_get_lock_status(void)
{

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_get_lock_status_callback)
		return gs_AppCasPortControlBlock.m_cas_api_get_lock_status_callback();

	return 0;		

}

/*
 * 子卡写入喂养数据
 */
char* app_cas_api_feed_mother_child_card(ca_mother_child_card_t *data)
{
	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_feed_mother_child_card_callback)
		return gs_AppCasPortControlBlock.m_cas_api_feed_mother_child_card_callback(data);

	return NULL;		
}


/*
 * 修改密码
 */
char* app_cas_api_change_pin(ca_pin_t* data)
{
	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_change_pin_callback)
		return gs_AppCasPortControlBlock.m_cas_api_change_pin_callback(data);

	return NULL;		
}

char* app_cas_api_verify_pin(ca_pin_t* data)
{
	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_verify_pin_callback)
		return gs_AppCasPortControlBlock.m_cas_api_verify_pin_callback(data);

	return NULL;		
}




/*
 * 修改成人级别
 */
char* app_cas_api_change_rating(ca_rating_t* data)
{
	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_change_rating_callback)
		return gs_AppCasPortControlBlock.m_cas_api_change_rating_callback(data);

	return NULL;		
}


/*
 * 修改工作时段
 */
char* app_cas_api_change_worktime(ca_work_time_t* data)
{
	if (NULL == data)
		return NULL;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_change_worktime_callback)
		return gs_AppCasPortControlBlock.m_cas_api_change_worktime_callback(data);

	return NULL;		
}

//智能卡插入消息回调处理
uint8_t app_cas_api_card_in(char* atr,uint8_t len)
{
	if (NULL == atr)
		return 0;

	if (NULL != gs_AppCasPortControlBlock.m_cas_api_card_in_callback)
		return gs_AppCasPortControlBlock.m_cas_api_card_in_callback(atr,len);

	return 0;		

}
//智能卡移除消息处理
uint8_t app_cas_api_card_out(void)
{
	if (NULL != gs_AppCasPortControlBlock.m_cas_api_card_out_callback)
		return gs_AppCasPortControlBlock.m_cas_api_card_out_callback();

	return 0;
}

/* add fy 2015-04-21 start 观看成人级节目 */
char* app_cas_api_watch_maturity(ca_rating_t* data)
{
	if(NULL == data)
	{
		return NULL;
	}
	return NULL;	
	
}
/* add fy 2015-04-21 end */
//传递消息
int app_cas_api_on_event(int key, const char* name, void* buf, size_t size)
{

	GxMsgProperty0_OnEvent*  p;
	uint32_t i =0;
	static	uint32_t j = 0;
	uint32_t msg_id =MAX_CA_MSG_NUM;
	static handle_t sem = 0;
	if(!sem)
	{
		//app_porting_ca_os_sem_create(&sem, 1);
        if(GXCORE_SUCCESS != GxCore_MutexCreate(&sem))
        {
            printf("GxCore_MutexCreate failed.\n");
            return -1;
        }
	}
	//app_porting_ca_os_sem_wait(sem);
    GxCore_MutexLock (sem);
	if (j+1 <MAX_CA_MSG_NUM)
	{
		for(i = j+1;i<MAX_CA_MSG_NUM;i++)
		{
			if(gCa_msg_list[i].msg == NULL)
			{
				msg_id = i;
				break;
			}
		}		
	}

	if (MAX_CA_MSG_NUM == msg_id)
	{
		for(i = 1;i<j;i++)
		{
			if(gCa_msg_list[i].msg == NULL)
			{
				msg_id = i;
				break;
			}					
		}
	}
	
	if(msg_id == MAX_CA_MSG_NUM)//消息队列满
	{
		//app_porting_ca_os_sem_signal(sem);
        GxCore_MutexUnlock (sem);
        printf("failed,it is full.\n");
		return -1;
	}

	j = msg_id;

	gCa_msg_list[msg_id].msg = GxBus_MessageNew(GXMSG_CA_ON_EVENT);
	if (NULL == gCa_msg_list[msg_id].msg)
	{
	    //app_porting_ca_os_sem_signal(sem);
        GxCore_MutexUnlock (sem);
		return 0;			
	}
    p = GxBus_GetMsgPropertyPtr(gCa_msg_list[msg_id].msg, GxMsgProperty0_OnEvent);
	   
    if (p == NULL) {
    	printf("GxCA0_OnEvent p = NULL\n");
        GxBus_MessageFree(gCa_msg_list[msg_id].msg );
    	gCa_msg_list[msg_id].msg = NULL;
    	gCa_msg_list[msg_id].param = NULL;		   
    	//app_porting_ca_os_sem_signal(sem);
        GxCore_MutexUnlock (sem);
       return 0;
    }
    //填装数据
    p->name = (char*)name;
    p->key = key;
    p->buf = buf;
    p->size = size;
    GxBus_MessageSend(gCa_msg_list[msg_id].msg );
	   
    gCa_msg_list[msg_id].msg = NULL;
    gCa_msg_list[msg_id].param = NULL;		

    //printf("app_cas_api_on_event i = %d key=0x%x name=%s\n",i,key,name);
    //app_porting_ca_os_sem_signal(sem);
    GxCore_MutexUnlock (sem);
    return 0;
}




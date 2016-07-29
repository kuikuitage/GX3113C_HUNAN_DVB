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
* Purpose   :	ģ??ͷ?ļ?
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.12.02		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "gxfrontend.h"
#include "gxapp_sys_config.h"
#include "app_common_play.h"
#include "app_common_table_pmt.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_flash.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_ca_descramble.h"
#include "app_common_search.h"
#include "app_common_porting_ca_smart.h"

#include "app_dvb_cas_api_demux.h"
#include "app_dvb_cas_api_smart.h"
#include "app_dvb_cas_api_baseinfo.h"
#include "dvbca_interface.h"

#include "app_dvb_cas_api_pin.h"
#include "app_dvb_cas_api_stb_pair.h"
#include "app_dvb_cas_api_worktime.h"
#include "app_dvb_cas_api_rating.h"
#include "app_dvb_cas_api_email.h"
#include "app_dvb_cas_api_entitle.h"
#include "app_dvb_cas_api_finger.h"
#include "app_dvb_cas_api_rolling_message.h"

extern	void app_dvb_cas_hide_ca_prompt(void);
extern void app_dvb_cas_show_ca_prompt(char* context);

extern uint8_t bat_recieve_flag ;

#define MAX_SEARIAL_LEN (4)
uint8_t app_dvb_cas_api_init(void)
{
	char caUser[MAX_SEARIAL_LEN+1]={0};
	unsigned char   aucstb_id[4] = {0};
	app_porting_ca_flash_init(64*1024);
	app_porting_ca_demux_init();
	app_porting_ca_descramble_init();
	app_porting_ca_create_filter_task();
	app_dvb_cas_api_smart_init();
	app_porting_ca_smart_create_task();
	//	app_flash_save_config_center_freq(858);
	app_flash_get_serial_number(0,(char *)caUser,MAX_SEARIAL_LEN);
	memcpy(aucstb_id, caUser, 4);
	DVBCASTB_Init(GXOS_DEFAULT_PRIORITY, aucstb_id);
	return TRUE;
}

/*
* ???½ӿ?CA?˵?????
*/


/*
*  ??ȡ??Ӧ????֮ǰ????ʼ?????ݹ???
(һ?㴴????Ӧ????֮ǰ??create?е???)??
* ????ʼ???ʼ?????Ȩ??
*/

int32_t app_dvb_cas_api_init_data(dvb_ca_data_type_t date_type)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag != DVB_CA_TYPE_DVB)
		return FALSE;

	switch(date_type)
	{
		case DVB_CA_BASE_INFO:	/*???\u0178??Ϣ*/
			return app_dvb_cas_api_init_baseinfo_data();
		case DVB_CA_EMAIL_INFO: /*??\u0152?*/
			return app_dvb_cas_api_init_email_data();
		case DVB_CA_ENTITLE_INFO: /*???\u0161*/
			return app_dvb_cas_api_init_entitle_data();
		case DVB_CA_OPERATOR_INFO:
			return app_dvb_cas_api_init_operator_data();
		case DVB_CA_RATING_INFO:
			return app_dvb_cas_api_init_rating_data();
		case DVB_CA_WORK_TIME_INFO:
			return app_dvb_cas_api_init_worktime_data();
		default:
			break;
	}

	return FALSE;
}

char* app_dvb_cas_api_get_data(ca_get_date_t *data)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag != DVB_CA_TYPE_DVB)
		return NULL;

	if (NULL == data)
		return NULL;

	switch(data->date_type)
	{
		case DVB_CA_BASE_INFO:	/*???\u0178??Ϣ*/
			return app_dvb_cas_api_get_baseinfo_data(data);
		case DVB_CA_EMAIL_INFO: /*??\u0152?*/
			return app_dvb_cas_api_get_email_data(data);
		case DVB_CA_ENTITLE_INFO: /*???\u0161*/
			return app_dvb_cas_api_get_entitle_data(data);
		case DVB_CA_OPERATOR_INFO:
			return app_dvb_cas_api_get_operator_data(data);
		case DVB_CA_RATING_INFO:
			return app_dvb_cas_api_get_rating_data(data);
		case DVB_CA_WORK_TIME_INFO:
			return app_dvb_cas_api_get_worktime_data(data);
		default:
			break;
	}

	return NULL;
}

bool app_dvb_cas_api_delete(ca_delete_data_t *data)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag != DVB_CA_TYPE_DVB)
		return FALSE;


	if (NULL == data)
		return FALSE;

	switch(data->date_type)
		{
			case DVB_CA_EMAIL_INFO: /*?ʼ?*/
				return app_dvb_cas_api_delete_email(data);
			default:
				break;
		}

	return 0;
}

bool app_dvb_cas_api_delete_all(ca_delete_data_t *data)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag != DVB_CA_TYPE_DVB)
		return FALSE;


	if (NULL == data)
		return FALSE;


	switch(data->date_type)
	{
		case DVB_CA_EMAIL_INFO: /*?ʼ?*/
			return app_dvb_cas_api_delete_all_email(data);
		default:
			break;
	}

	return 0;
}

uint8_t app_dvb_cas_api_get_count(ca_get_count_t *data)
{
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (dvb_ca_flag != DVB_CA_TYPE_DVB)
		return 0;

	if (NULL == data)
		return 0;


	switch(data->date_type)
	{
		case DVB_CA_EMAIL_INFO: /*??\u0152?*/
			return app_dvb_cas_api_get_email_count(data);
		case DVB_CA_OPERATOR_INFO: /*???\u0161*/
			return app_dvb_cas_api_get_operator_count(data);
		case DVB_CA_ENTITLE_INFO: /*???\u0161*/
			return app_dvb_cas_api_get_entitle_count(data);
		default:
			break;
	}


	return 0;
}


extern void app_win_dvb_upgrade_freq(ST_DVBCA_FREQ *pstFreq, unsigned char ucFreqSum);
static uint8_t ca_upgrade_flag = 0;
int32_t app_dvb_cas_gxmsg_ca_on_event_upgrade(GxMsgProperty0_OnEvent* event)
{
	ST_DVBCA_UPGRADEPROG* upgrade = NULL;
	ST_DVBCA_FREQ fre[TP_MAX_NUM] = {{0,},};
	char* focus_Window = NULL;
	if (NULL == event)
		return 0;

	upgrade = (ST_DVBCA_UPGRADEPROG*)(event->buf);

	if (NULL == upgrade)
		return 0;

	CAS_Dbg("upgrade->=%d\n",upgrade->ucFreqCount);
	focus_Window = (char*)GUI_GetFocusWindow();
	if ((ca_upgrade_flag  == 0 )&& (0 == strcasecmp("win_full_screen", focus_Window)))
	{
		ca_upgrade_flag = 1;
		CAS_Dbg("upgrade_flag=%d\n",ca_upgrade_flag);
		app_win_exist_to_full_screen();
		memcpy(&fre[0],upgrade->pstFreq,upgrade->ucFreqCount*sizeof(ST_DVBCA_FREQ));
		app_win_dvb_upgrade_freq(fre, upgrade->ucFreqCount);
		ca_upgrade_flag = 0;
		CAS_Dbg("upgrade_flag=%d\n",ca_upgrade_flag);
	}

	return 1;
}

int32_t app_dvb_cas_gxmsg_ca_on_event_exec(GxMessage * msg)
{
	switch(msg->msg_id)
	{
		case GXMSG_CA_ON_EVENT:
			{
				GxMsgProperty0_OnEvent* event;
				event = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty0_OnEvent);
				CAS_Dbg("app_dvb_cas_gxmsg_ca_on_event_exec GXMSG_CA_ON_EVENT\n");
				switch (event->key) {
					case DVB_CA_BASE_INFO:		       /*??????Ϣ*/
						break;
					case DVB_CA_OPERATOR_INFO:		/*??Ӫ????Ϣ*/
						break;
					case DVB_CA_EMAIL_INFO:		/*?ʼ?*/
						{
							app_dvb_cas_gxmsg_ca_on_event_email(event);
						}
						break;
					case DVB_CA_ENTITLE_INFO :			/*??Ȩ*/
						break;
					case DVB_CA_DETITLE_INFO:                /*????Ȩ*/
						break;
					case DVB_CA_EMERGENCY_INFO:           /*Ӧ???㲥*/
						break;
					case DVB_CA_ROLLING_INFO:               /*OSD??????Ϣ*/
						app_dvb_cas_gxmsg_ca_on_event_rolling(event);
						break;
					case DVB_CA_FINGER_INFO:
						app_dvb_cas_gxmsg_ca_on_event_finger(event);
						break;
					case DVB_CA_CARD_UPDATE_INFO:       /*??????*/
						break;
					case DVB_CA_FETURE_INFO:                  /*????ֵ*/
						break;
					case DVB_CA_IPPV_POP_INFO:                /*IPPV????????Ϣ*/
						break;
					case DVB_CA_IPPT_POP_INFO:                /*IPPT????????Ϣ*/
						break;
					case DVB_CA_IPPV_PROG_INFO:            /*IPPV?ѹ?????Ŀ??Ϣ*/
						break;
					case DVB_CA_IPPV_SLOT_INFO:             /*IPPVǮ????Ϣ*/
						break;
						//								case DVB_CA_CHILD_CARD_INFO:            /*?ӿ???Ϣ*/
						//									break;
					case DVB_CA_MOTHER_CARD_INFO:            /*ĸ????Ϣ*/
						break;
					case DVB_CA_CHANGE_PIN_INFO:             /*?޸???????Ϣ*/
						break;
					case DVB_CA_PROGRESS_INFO:             /*???ܿ???????????Ϣ*/
						break;
					case DVB_CA_RATING_INFO:                /*???˼?????Ϣ*/
						break;
					case DVB_CA_WORK_TIME_INFO:          /*????ʱ????Ϣ*/
						break;
					case DVB_CA_CURTAIN_INFO:          /*??q??Ϣ*/
						break;
					case DVB_CA_PAIRED_INFO:          /*??????Ϣ*/
						break;
					case DVB_CA_UPGRADE_FRE:
						app_dvb_cas_gxmsg_ca_on_event_upgrade(event);
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

uint8_t app_dvb_cas_api_is_ca_pop_msg_type(uint8_t type)
{
	if ((type >= MSG_POP_COMMON_END+ DVBCA_MESSAGE_INSERTCARD +1)&&(type <=MSG_POP_COMMON_END+1+DVBCA_MESSAGE_MAX_NUM))
		return TRUE;
	return FALSE;
}

//消息提示框
int32_t app_dvb_cas_api_pop_msg_exec(uint8_t type)
{
	switch(type-MSG_POP_COMMON_END-1)
	{
		case DVBCA_MESSAGE_INSERTCARD :		/*加扰节目，请插入智能卡*/
			app_dvb_cas_show_ca_prompt("The program  is scramble");
			break;
		case DVBCA_MESSAGE_BADCARD :		/*无法识别卡，不能使用*/
			app_dvb_cas_show_ca_prompt("Smart Card Is Error");
			break;
		case DVBCA_MESSAGE_NOENTITLE :		/*没有授权*/
			app_dvb_cas_show_ca_prompt("No Entitle");
			break;
		case DVBCA_MESSAGE_NOOPER_TYPE :	/*卡中不存在节目运营商*/
			app_dvb_cas_show_ca_prompt("No Operator");
			break;
		case DVBCA_MESSAGE_BLOCK_AREA :		/*区域禁播*/
			app_dvb_cas_show_ca_prompt("Err Limit Area");
			break;
		case DVBCA_MESSAGE_EXPIRED :		/*授权过期*/
			app_dvb_cas_show_ca_prompt("Err Auth Overdue");
			break;
		case DVBCA_MESSAGE_PASSWORD_ERROR :	/*运营商密码错误*/
			app_dvb_cas_show_ca_prompt("Operator password error");
			break;
		case DVBCA_MESSAGE_ERRREGION :		/*区域不正确*/
			app_dvb_cas_show_ca_prompt("Regional Lockout");
			break;
		case DVBCA_MESSAGE_PAIRING_ERROR :	/*机卡不匹配*/
			app_dvb_cas_show_ca_prompt("Err Smart Stb NoPair");
			break;
		case DVBCA_MESSAGE_WATCHLEVEL :		/*节目级别高于设定观看级别*/
			app_dvb_cas_show_ca_prompt("(DVBCA)Program level is higher than rating");
			break;
		case DVBCA_MESSAGE_EXPIRED_CARD :	/*智能卡过期*/
			app_dvb_cas_show_ca_prompt("Card Is Expire");
			break;
		case DVBCA_MESSAGE_DECRYPTFAIL :	/*节目解密失败*/
			app_dvb_cas_show_ca_prompt("Decrypt fail");
			break;
		case DVBCA_MESSAGE_WORKTIME :	/*不在工作时段内*/
			app_dvb_cas_show_ca_prompt("OUT OF WORKTIME!");
			break;
		case DVBCA_MESSAGE_FREQ_UPDATE:		/*发现新节目,即将进行自动扫描*/
			app_dvb_cas_show_ca_prompt("(DVBCA)Upgrade fre");
			break;
		default:
			break;
	}
	return 0;
}

int32_t app_dvb_cas_api_osd_exec(void)
{
	app_dvb_cas_rolling_message_exec();
	app_dvb_cas_email_flag_exec();
	app_dvb_cas_finger_exec();
	return 0;
}


APP_CAS_API_ControlBlock_t DvbCasPortControlBlock=
{
	.m_cas_init_callback =  app_dvb_cas_api_init,
	.m_cas_close_callback = NULL,
	.m_cas_api_card_in_callback = app_dvb_cas_api_card_in,
	.m_cas_api_card_out_callback = app_dvb_cas_api_card_out,
	.m_cas_api_release_ecm_filter_callback= app_dvb_cas_api_release_ecm_filter,
	.m_cas_api_start_ecm_filter_callback = app_dvb_cas_api_start_ecm_filter,
	.m_cas_api_release_emm_filter_callback = app_dvb_cas_api_release_emm_filter,
	.m_cas_api_start_emm_filter_callback = app_dvb_cas_api_start_emm_filter,
	.m_cas_api_check_cat_ca_descriptor_valid_callback= app_dvb_cas_api_check_cat_ca_descriptor_valid,
	.m_cas_api_check_pmt_ca_descriptor_valid_callback= app_dvb_cas_api_check_pmt_ca_descriptor_valid,
	.m_cas_api_is_ca_pop_msg_type_callback= app_dvb_cas_api_is_ca_pop_msg_type,
	.m_cas_api_pop_msg_exec_callback= app_dvb_cas_api_pop_msg_exec,
	.m_cas_api_osd_exec_callback= app_dvb_cas_api_osd_exec,
	.m_cas_api_gxmsg_ca_on_event_exec_callback= app_dvb_cas_gxmsg_ca_on_event_exec,
	.m_cas_api_init_data_callback = app_dvb_cas_api_init_data,
	.m_cas_api_get_data_callback = app_dvb_cas_api_get_data,
	.m_cas_api_get_count_callback = app_dvb_cas_api_get_count,
	.m_cas_api_delete_callback =  app_dvb_cas_api_delete,
	.m_cas_api_delete_all_callback = app_dvb_cas_api_delete_all,
	.m_cas_api_buy_ipp_callback = NULL,
	.m_cas_api_get_lock_status_callback =	NULL, //app_dvb_cas_api_get_lock_status,
	.m_cas_api_feed_mother_child_card_callback = NULL,
	.m_cas_api_change_pin_callback = app_dvb_cas_api_change_pin,
	.m_cas_api_verify_pin_callback = app_dvb_cas_api_stb_pair,
	.m_cas_api_change_rating_callback = app_dvb_cas_api_change_rating,
	.m_cas_api_change_worktime_callback= app_dvb_cas_api_change_worktime,
};




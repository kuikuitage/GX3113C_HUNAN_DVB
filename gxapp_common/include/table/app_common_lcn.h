#ifndef __APP_COMMON_LCN_H__
#define __APP_COMMON_LCN_H__

#include "gxtype.h"
#include "service/gxsi.h"
#include "gxprogram_manage_berkeley.h"


typedef struct LCNInfoOne_s
{
	uint16_t m_wStreamId;
	uint16_t m_wServiceId;
	uint32_t m_wLcn;
	uint8_t prog_name[MAX_PROG_NAME];///<节目的名称
	uint16_t m_wTpId;
	uint16_t m_ref_serviceid;
	uint16_t m_nvod_serviceid;
	void* next;
}LCNInfoOne_t;

void     app_lcn_list_init(void);
void     app_lcn_list_clear(void);
void     app_lcn_list_add_one(LCNInfoOne_t lcnInfo);
uint8_t  app_lcn_list_check_nvod_one_exist(LCNInfoOne_t* date);
bool     app_lcn_list_get_pos_in_group_by_lcn(uint16_t lcn, uint16_t* pos);
bool 	 app_lcn_list_get_num_prog_pos(uint16_t lcn, uint16_t* pos);
void     app_lcn_list_update_prog_pos(void);
void     app_lcn_list_update_nvod_prog_pos(void);
void     app_lcn_set_searching_tp_id(uint16_t tp_id);
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
int32_t lcn_descriptor_parse(uint8_t tag, uint8_t *p_section_data, uint16_t len);
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
#endif



#include "app_common_lcn.h"
#include "app_common_flash.h"
#include "app_common_search.h"
#include "gxapp_sys_config.h"
#include "app_common_prog.h"

static LCNInfoOne_t* g_pLCNInfo=NULL;
uint16_t cur_tp_id;
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
uint16_t cur_ts_id = 0;
#endif
static uint16_t s_max_LCN=0;

//uint8_t pSectionFlag[256] = {0};
//char service_name[MAX_PROG_NAME+1]={0}; 
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
static uint8_t app_lcn_list_check_one_exist(LCNInfoOne_t* date);
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/

#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
int32_t lcn_descriptor_parse(uint8_t tag, uint8_t *p_section_data, uint16_t len)
{
	LCNInfoOne_t lcnOne;
	uint16_t LcnLength = len/4;
	uint16_t i;
	LCNInfoOne_t *date = NULL;

	switch(tag)
	{
		case 0x83://LCN
		{

			for(i=0;i<LcnLength;i++)
			{
				memset(&lcnOne,0,sizeof(LCNInfoOne_t));
				date = &lcnOne;
				date->m_wServiceId = ((p_section_data[2+i*4]<<8)&0xff00) +p_section_data[3+i*4];
				date->m_wLcn = ((p_section_data[4+i*4]<<8)&0xff00) +p_section_data[5+i*4];
				date->m_wLcn &= 0x3ff;
				date->m_wStreamId = cur_ts_id;
				date->m_wTpId = cur_tp_id;
				date->next = NULL;
				if(0 == app_lcn_list_check_one_exist(date))
				{
					app_lcn_list_add_one(lcnOne);
				}
			}
		}
		break;
	}
	return 0;
}
#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/

static void app_lcn_list_insert_one(LCNInfoOne_t* date)
{
	LCNInfoOne_t **p = NULL;
	Lcn_State_t lcn_flag;
	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		return ;
	}

	if (0 == PROG_MAX_LCN)
	{
		return ;
	}


	if (NULL == date)
		return;



	p = &g_pLCNInfo;

	while(*p != NULL)
	{
		p = (LCNInfoOne_t **)(&(((LCNInfoOne_t *)(*p))->next));
	}
	*p = GxCore_Malloc(sizeof(LCNInfoOne_t));
	if (NULL == *p)
		return;
	((LCNInfoOne_t *)(*p))->m_wStreamId = date->m_wStreamId;
	((LCNInfoOne_t *)(*p))->m_wServiceId = date->m_wServiceId;
	((LCNInfoOne_t *)(*p))->m_wLcn = date->m_wLcn;
	if (strlen((char *)date->prog_name)>0)
		memcpy(((LCNInfoOne_t *)(*p))->prog_name,date->prog_name,MAX_PROG_NAME);
	((LCNInfoOne_t *)(*p))->m_wTpId = date->m_wTpId;
	((LCNInfoOne_t *)(*p))->m_ref_serviceid = date->m_ref_serviceid;
	((LCNInfoOne_t *)(*p))->m_nvod_serviceid = date->m_nvod_serviceid;
	((LCNInfoOne_t *)(*p))->next = NULL;
}


void app_lcn_list_init(void)
{
	int16_t prog_count = 0;
	int16_t prog_pos = 0;
	Lcn_State_t lcn_flag;
	GxBusPmViewInfo sys;
	GxBusPmViewInfo sys_old;
	GxBusPmDataProg prog_arry;
	LCNInfoOne_t *date = NULL;

	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return ;
	}


	if (0 == PROG_MAX_LCN)
	{
		return ;
	}


	GxBus_PmViewInfoGet(&sys);
	memcpy(&sys_old,&sys,sizeof(GxBusPmViewInfo));
	if (TAXIS_MODE_NON != sys.taxis_mode)
		{
			sys.taxis_mode = TAXIS_MODE_NON;
			GxBus_PmViewInfoModify(&sys);		
		}

	app_prog_change_group(GROUP_MODE_ALL, GXBUS_PM_PROG_ALL, 0);
	prog_count = GxBus_PmProgNumGet();
	s_max_LCN = 0;
//	memset(pSectionFlag,0,256);
	if(0 == prog_count)
	{
		GxBus_PmViewInfoModify(&sys_old);	
		return;
	}


	for(prog_pos = 0;prog_pos<prog_count; prog_pos++)
	{
		date = GxCore_Malloc(sizeof(LCNInfoOne_t));
		if(date == NULL)
		{
			printf("[LCN INFO]-----MALLOC-----Failed!!!!!");
			return;
		}
		GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
		date->m_wStreamId = prog_arry.ts_id;
		date->m_wServiceId = prog_arry.service_id;
		date->m_wLcn = prog_arry.pos;
		memcpy(date->prog_name,prog_arry.prog_name,MAX_PROG_NAME);
		date->m_wTpId = prog_arry.tp_id;
		date->next = NULL;
		printf(" m_wServiceId = %d m_wLcn=%d\n",date->m_wServiceId,date->m_wLcn);
		app_lcn_list_insert_one(date);
		if(s_max_LCN < date->m_wLcn)
		{
			s_max_LCN = date->m_wLcn;
		}
		GxCore_Free(date);
	}

	GxBus_PmViewInfoModify(&sys_old);	
	return ;
}

static uint8_t app_lcn_list_check_one_exist(LCNInfoOne_t* date)
{
	LCNInfoOne_t *p = NULL;
	Lcn_State_t lcn_flag;
	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return 0;
	}

	if (0 == PROG_MAX_LCN)
	{
		return 0;
	}


	if (NULL == date)
		return 0;

	p = g_pLCNInfo;
	while(p != NULL)
	{
		if (p->m_wServiceId == date->m_wServiceId)
		{	// 存在相同业务ID，更新逻辑频道号
			if (p->m_wLcn != date->m_wLcn)
			{
				printf("app_lcn_check_list_exist exist date->m_wServiceId=%d date->m_wLcn=%d\n",
					date->m_wServiceId,date->m_wLcn);
				p->m_wLcn = date->m_wLcn;
			}

			return 1;
		}
		p = (LCNInfoOne_t*)(p->next);
	}
	return 0;
}

uint8_t app_lcn_list_check_nvod_one_exist(LCNInfoOne_t* date)
{
	LCNInfoOne_t *p = NULL;
	Lcn_State_t lcn_flag;
	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return 0;
	}


	if (0 == PROG_MAX_LCN)
	{
		return 0;
	}


	if (NULL == date)
		return 0;


	p = g_pLCNInfo;
	while(p != NULL)
	{
		if ((p->m_ref_serviceid == date->m_ref_serviceid) && (p->m_nvod_serviceid == date->m_nvod_serviceid))
		{	// 存在相同业务ID，更新逻辑频道号
			
			printf("app_lcn_check_list_exist exist date nvod");
			return 1;
		}
		p = (LCNInfoOne_t*)(p->next);
	}
	return 0;
}

void app_lcn_list_add_one(LCNInfoOne_t lcnInfo)
{
	LCNInfoOne_t *date = NULL;
	Lcn_State_t lcn_flag;
	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return ;
	}	

	if (0 == PROG_MAX_LCN)
	{
		return ;
	}


	date = GxCore_Malloc(sizeof(LCNInfoOne_t));
	if(date == NULL)
	{
		printf("[LCN INFO]-----MALLOC-----Failed!!!!!");
		return ;
	}
	memset(date,0,sizeof(LCNInfoOne_t));
	date->m_wServiceId = lcnInfo.m_wServiceId;
	date->m_wLcn = lcnInfo.m_wLcn;
	date->m_wStreamId = lcnInfo.m_wStreamId;
	date->m_wTpId = cur_tp_id;
	/*
	* 更新节目名称，如无更新节目需求，lcnInfo.prog_name可为NULL
	* 
	*/
	if (strlen((char*)lcnInfo.prog_name)>0)
		memcpy((char*)date->prog_name,(char*)lcnInfo.prog_name,MAX_PROG_NAME);
	
	/*
	* nvod, 普通节目可不关心
	*/

	date->m_ref_serviceid = lcnInfo.m_ref_serviceid;
	date->m_nvod_serviceid = lcnInfo.m_nvod_serviceid;
	
	date->next = NULL;
	printf(" m_wServiceId = %d m_wLcn=%d\n",date->m_wServiceId,date->m_wLcn);
	if(0 == app_lcn_list_check_one_exist(date))
	{
		app_lcn_list_insert_one(date);
	}
	GxCore_Free(date);
	return ;
	
}

void app_lcn_list_clear(void)
{
	Lcn_State_t lcn_flag;
	LCNInfoOne_t *p = NULL;
	LCNInfoOne_t *d = NULL;

	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return ;
	}


	if (0 == PROG_MAX_LCN)
	{
		return ;
	}


	p = g_pLCNInfo;
	while(p != NULL)
	{
		d = (LCNInfoOne_t *)(p->next);
		GxCore_Free(p);
		p = d;
	}
	g_pLCNInfo = NULL;
}

void app_lcn_set_searching_tp_id(uint16_t tp_id)
{
	cur_tp_id = tp_id;
}

void app_lcn_list_update_prog_pos(void)
{
	Lcn_State_t lcn_flag;
	int16_t prog_count = 0;
	int16_t prog_pos = 0;
	GxBusPmDataProg prog_arry;
	LCNInfoOne_t *p = NULL;

	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return ;
	}

	if (0 == PROG_MAX_LCN)
	{
		return ;
	}

	prog_count = GxBus_PmProgNumGet();



	for(prog_pos = 0;prog_pos<prog_count; prog_pos++)
	{
		GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);

		p = g_pLCNInfo;
		while(p != NULL)
		{		
		
					if (p->m_wServiceId == prog_arry.service_id)
					{
						printf("app_lcn_list_update_prog_pos  p->m_wServiceId=%d prog_arry. prog_name=%s\n",
							p->m_wServiceId,prog_arry.prog_name);
						break;
					}				

			p = (LCNInfoOne_t*)(p->next);
		}
		if(p != NULL)
		{
			printf("app_lcn_update_prog_pos_by_lcn_number  p->m_wServiceId=%d p->m_wLcn=%d\n",
					p->m_wServiceId,p->m_wLcn);
			prog_arry.pos = p->m_wLcn;
			GxBus_PmProgInfoModify(&prog_arry);
		}
		else
		{
			if (PROG_MAX_LCN > 0)
			{
				if(s_max_LCN<=PROG_MAX_LCN)
				{
					s_max_LCN = PROG_MAX_LCN;
				}
				prog_arry.pos = s_max_LCN+1;
				s_max_LCN++;
				GxBus_PmProgInfoModify(&prog_arry);			
			}
		}
	}
	GxBus_PmSync(GXBUS_PM_SYNC_PROG);
}

static int get_nvod_num(char *name)
{

	if (NULL == name)
		return 0;
	while(*name)
	{
		if(((*name) >= 0x30) && ((*name) <= 0x39))
		{
			return (int)(*name - 0x30);
		}
		name++;
	}
	return 0;
}

void app_lcn_list_update_nvod_prog_pos(void)
{
	int16_t prog_count = 0;
	int16_t prog_pos = 0;
	GxBusPmDataProg prog_arry;
	LCNInfoOne_t *p = NULL;
	Lcn_State_t lcn_flag;
	/*如果设置为无逻辑频道号模式，直接退出*/
	lcn_flag = app_flash_get_config_lcn_flag();
	if((LCN_STATE_OFF == lcn_flag)&&(0 == app_flash_get_config_sort_by_pat_flag()))
	{
		/*
		* 没有开启逻辑频道号排序、并且不需要根据pat表排序
		*/
		return ;
	}


	if (0 == PROG_MAX_LCN)
	{
		return ;
	}



	prog_count = GxBus_PmProgNumGet();

	for(prog_pos = 0;prog_pos<prog_count; prog_pos++)
	{
		GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);

		p = g_pLCNInfo;
		while(p)
		{
			if(p->m_ref_serviceid == prog_arry.service_id)
			{
				prog_arry.pos = get_nvod_num((char *)p->prog_name);//p->m_nvod_serviceid;
				break;
			}
			p = (LCNInfoOne_t*)(p->next);
		}
		if(p != NULL)
		{
			GxBus_PmProgInfoModify(&prog_arry);
		}
	}
	//GxBus_PmSync(GXBUS_PM_SYNC_PROG);
}


bool app_lcn_list_get_pos_in_group_by_lcn(uint16_t lcn, uint16_t* pos)
{
//	Lcn_State_t lcn_flag;
	uint16_t prog_count = 0;
	uint16_t prog_pos = 0;
	GxBusPmDataProg prog_arry;

	/*如果设置为无逻辑频道号模式，直接退出*/
/*	lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_OFF == lcn_flag)
	{
		return FALSE;
	}*/

	if (NULL == pos)
		return FALSE;
		

	prog_count = GxBus_PmProgNumGet();
	for(prog_pos = 0;prog_pos<prog_count;prog_pos++)
	{
		GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
		if(prog_arry.pos == lcn)
		{
			*pos = prog_pos;
			return TRUE;
		}
	}

	if(prog_pos == prog_count)
	{
		*pos = prog_count;
	}
	return FALSE;
}

bool app_lcn_list_get_num_prog_pos(uint16_t lcn, uint16_t* pos)
{

	uint16_t prog_count = 0;
	uint16_t prog_pos = 0;
	GxBusPmDataProg prog_arry;
	GxBusPmViewInfo sysinfo;
	GxBusPmViewInfo old_sysinfo;
	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&old_sysinfo,&sysinfo,sizeof(GxBusPmViewInfo));

	if (NULL == pos)
		return FALSE;
		

	prog_count = GxBus_PmProgNumGet();
	for(prog_pos = 0;prog_pos<prog_count;prog_pos++)
	{
		GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
		if(prog_arry.pos == lcn)
		{
			*pos = prog_pos;
			return TRUE;
		}
	}
#if (1 == DVB_HD_LIST)	
	if(prog_pos == prog_count)
		{
//			if(old_sysinfo.stream_type != GXBUS_PM_PROG_HD_SERVICE)
			{
				app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_HD_SERVICE,0);
				prog_count = GxBus_PmProgNumGet();
				for(prog_pos = 0;prog_pos<prog_count;prog_pos++)
				{
					GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
					if(prog_arry.pos == lcn)
					{
						*pos = prog_pos;
						return TRUE;
					}
				}
				
				GxBus_PmViewInfoModify(&old_sysinfo);
			}
			
			
		}
#endif


	if(prog_pos == prog_count)
		{
//			if(old_sysinfo.stream_type != GXBUS_PM_PROG_TV)
			{
				app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0);
				prog_count = GxBus_PmProgNumGet();
				for(prog_pos = 0;prog_pos<prog_count;prog_pos++)
				{
					GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
					if(prog_arry.pos == lcn)
					{
						*pos = prog_pos;
						return TRUE;
					}
				}
				
				GxBus_PmViewInfoModify(&old_sysinfo);
			}
			
			
		}

	if(prog_pos == prog_count)
	{
//		if(old_sysinfo.stream_type != GXBUS_PM_PROG_RADIO)
		{
			app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);
			prog_count = GxBus_PmProgNumGet();
			for(prog_pos = 0;prog_pos<prog_count;prog_pos++)
			{
				GxBus_PmProgGetByPos(prog_pos, 1, &prog_arry);
				if(prog_arry.pos == lcn)
				{
					*pos = prog_pos;
					return TRUE;
				}
			}
			
			GxBus_PmViewInfoModify(&old_sysinfo);
		}
		
		
	}


	
	

//	if(prog_pos == prog_count)
//	{
//		GxBus_PmViewInfoModify(&old_sysinfo);
//		*pos = prog_count;
//	}

	return FALSE;
}




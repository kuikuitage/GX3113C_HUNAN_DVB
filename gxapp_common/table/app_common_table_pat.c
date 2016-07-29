#include <gxtype.h>
#include "gxapp_sys_config.h"
#include "app_common_flash.h"
#include "app_common_play.h"
#include "app_common_prog.h"
#include "app_common_lcn.h"
#include "app_common_table_pat.h"
#include "app_common_search.h"
#include "app_common_porting_ca_demux.h"
#include "gxfrontend.h"
#include "gx_demux.h"

private_parse_status app_table_pat_section_parse(uint8_t* pSectionData, size_t Size)
{
	uint32_t ret = PRIVATE_SECTION_OK;

	uint8_t *date;
	int16_t len1 = 0;
	uint16_t ts_id = 0;
//	uint16_t num = 0;
	uint16_t i =0 ;
	uint8_t version = 0;
	uint32_t m_wLcn = 0;
	uint32_t fre = 0;
	LCNInfoOne_t lcnInfo = {0};

	date = pSectionData;

	char *parse_data = NULL;

	if (NULL == pSectionData)
		return ret;
	
	if(date[0] == PAT_TID)
	{
		len1 = ((date[1]&0x0f) << 8) | date[2];
		len1 = len1+3-4;
		ts_id = (date[3] << 8) | date[4];
		version = (date[5]&0x3e)>>1;
		
		fre = app_search_get_cur_searching_tp_fre();
		fre = fre/1000;

		if (len1 > 1021)
		{
			return ret;
		}
		
		len1-=8;
		parse_data = (char *)&date[8];


		while(len1 > 0)
		{
			uint16_t m_wServiceId = 0;
//			int16_t pmt_pid = 0;
			uint16_t network_pid = 0;

			m_wServiceId = ((parse_data[0] ) << 8) + parse_data[1] ;
			if(m_wServiceId == 0)
			{
				network_pid = ((parse_data[2] & 0x1F) << 8) + parse_data[3] ;
			}
			else
			{
				m_wLcn = (fre << 16)|i;
				lcnInfo.m_wLcn = m_wLcn;
				lcnInfo.m_wServiceId = m_wServiceId;
				lcnInfo.m_wStreamId = ts_id;
				app_lcn_list_add_one(lcnInfo);
				
				printf("program number %d lcn 0x%08x\n",m_wServiceId,m_wLcn);
				i++;
			}
			len1 -= 4;
			parse_data +=4;
			
		}

//		app_search_update_ts_info(ts_id,Prog.tp_id);
		ret = PRIVATE_SUBTABLE_OK;
		
	}
	return ret;
}


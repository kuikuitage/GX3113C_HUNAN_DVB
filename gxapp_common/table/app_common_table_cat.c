#include "gxapp_sys_config.h"
#include <gxtype.h>
#include "app_common_flash.h"
#include "app_common_table_pmt.h"
#include "app_common_table_cat.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "gxfrontend.h"
#include "gx_demux.h"

#ifdef CA_FLAG
#define MAX_SECTION_SMALL_BUF_SIZE (1024)
static handle_t cat_filter = 0;
static handle_t cat_channel = 0;
#ifdef  DVB_CA_TYPE_TR_FLAG
#include "Tr_Cas.h"
static uint8_t CatBuf[MAX_SECTION_SMALL_BUF_SIZE] = {0,};
#endif
#endif



handle_t app_table_cat_get_filter_handle(void)
{
#ifdef CA_FLAG
	return cat_filter;
#endif

	return 0;
}

handle_t app_table_cat_get_channel_handle(void)
{
#ifdef CA_FLAG
	return cat_channel;
#endif

	return 0;
}
//解析CAT表数据
void app_table_cat_section_parse(uint8_t* section, size_t Size)
{
    uint8_t                 version;
    uint8_t                 descriptor_tag;
    uint8_t                 descriptor_length;
    uint8_t*                data = section;
    int                     len = Size;
	if (NULL == section)
		return;
     int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef CA_FLAG
     uint32_t CA_system_id;
     u_int16_t emm_pid1 = PSI_INVALID_PID;
     u_int16_t emm_pid2 = PSI_INVALID_PID;
	 play_scramble_para_t playpara = {0};
#endif

	 if (dvb_ca_flag == DVB_CA_TYPE_TR  )			
	 {
#ifdef  DVB_CA_TYPE_TR_FLAG
		 int nRet = 0;
		 memset(CatBuf, 0, MAX_SECTION_SMALL_BUF_SIZE);
		 memcpy(CatBuf, data, len);
		 nRet = MC_MNGR_PostPsiTable(CAS_CAT_UPDATE, CatBuf);
		 if (nRet == 0)
		 {
			 //CAS_Dbg("[app_table_cat_section_parse]MC_MNGR_PostPsiTable CAT update OK.\n");
		 }
		 else
		 {
			 CAS_Dbg("[app_table_cat_section_parse]MC_MNGR_PostPsiTable CAT fail!!!\n");
		 }
#endif
	 }
	 version = (data[5] & 0x3E) >> 1;
	 len     -= 12;
	 data    += 8;
	 //    printf("\n\nCAT Got  version:%d\n", version);
	 while (len > 0) 
	 {
		 descriptor_tag = data[0];
		 descriptor_length = data[1];
#ifdef CA_FLAG
		 if(descriptor_tag == 0x09) 
		 {
			 CA_system_id = data[2] << 8 | data[3];
			 emm_pid1 = (data[4] & 0x1F) << 8 | data[5];;
			 if (TRUE == app_cas_api_check_cat_ca_descriptor_valid(data,CA_system_id))
			 {
				 if (PSI_INVALID_PID != emm_pid1)
				 {
					 emm_pid2 = emm_pid1;
				 }
			 }
		 }

		 if (PSI_INVALID_PID != emm_pid2)
		 {

			 if (dvb_ca_flag == DVB_CA_TYPE_TR  )			
			 {
#ifdef DVB_CA_TYPE_TR_FLAG

#else
				 app_cas_api_release_emm_filter();
#endif
			 }
			 else
			 {
				 app_cas_api_release_emm_filter();
			 }	 
			 app_play_get_playing_para(&playpara);
			 playpara.p_emm_pid = emm_pid2;
			 app_play_set_playing_para(&playpara); 
			 app_cas_api_start_emm_filter(emm_pid2);//开启emm过滤器  		
		 }
#endif

		 data += 2;
		 len -= 2;
		 if(descriptor_length == 0) 
		 {
			 continue;
		 }

		 len -= descriptor_length;
		 data += descriptor_length;
	 }
}

void app_table_cat_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
#ifdef CA_FLAG	
	int16_t            pid;
	uint16_t            section_length;
	uint8_t*            data = (uint8_t*)Section;
	int                 len = Size;
	uint8_t version = 0;
	uint8_t match[6] = {0,};
	uint8_t mask[6] = {0,};
	int32_t ret;

	if (NULL == Section)
		return ;


	//    ASSERT(Section != NULL);
	ret = GxDemux_FilterGetPID(Filter,&pid);
	if (CAT_PID != pid)
	{
		CAS_Dbg("app_table_cat_filter_notify pid error  pid:0x%x\n", pid);
		return ;
	}

	CAS_Dbg("app_table_cat_filter_notify total size:%d\n", len);


	section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
	//    CAS_Dbg("length=%d\n", section_length);
	version = data[5] & 0x3E;
	app_table_cat_section_parse(data, section_length);
	/*
	 * 设置版本不等过滤未起作用?
	 */
	//    data += section_length;
	//    len -= section_length;
	app_porting_psi_demux_lock();
	ret = GxDemux_FilterDisable(Filter);
	CHECK_RET(DEMUX,ret);
	match[0] = CAT_TID;
	mask[0]  = 0xFF;
	match[5] = version;
	mask[5]  = 0x3E;
	ret = GxDemux_FilterSetup(Filter, match, mask, FALSE, TRUE,0, 6);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_ChannelEnable(cat_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(Filter);
	CHECK_RET(DEMUX,ret);
	app_porting_psi_demux_unlock();

	return;
#endif
	return;		
}

int app_table_cat_filter_restart(void)
{
#ifdef CA_FLAG

	handle_t filter;
	uint8_t  match = CAT_TID;
	uint8_t  mask  = 0xFF;
	int32_t  ret = 0;
	CAS_Dbg("app_table_cat_filter_restart\n");
	app_porting_psi_demux_lock();
	if (0 != cat_filter)
	{
		//		ret = GxDemux_FilterDisable(cat_filter);
		//		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(cat_filter);
		CHECK_RET(DEMUX,ret);
		cat_filter = 0;
	}
	if (0 != cat_channel)
	{
		ret = GxDemux_ChannelFree(cat_channel);
		CHECK_RET(DEMUX,ret);
		cat_channel = 0;
	}

	cat_channel = GxDemux_ChannelAllocate(0, CAT_PID);
	if ((0 == cat_channel)||(-1 == cat_channel))
	{
		CA_ERROR(" cat_channel=0x%x\n",cat_channel);
		cat_channel = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}
	filter = GxDemux_FilterAllocate(cat_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(cat_channel);
		CHECK_RET(DEMUX,ret);
		cat_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}


	ret = GxDemux_FilterSetup(filter, &match, &mask, TRUE, TRUE,0, 1);
	CHECK_RET(DEMUX,ret);
	//	GxCA_DemuxFilterRigsterNotify(filter, app_table_cat_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(cat_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	cat_filter = filter;
	app_porting_psi_demux_unlock();
#endif
	return 0;
}

int app_table_cat_filter_close(void)
{
#ifdef CA_FLAG
	int32_t ret = 0;
	app_porting_psi_demux_lock();
	if (0 != cat_filter)
	{
		//		ret = GxDemux_FilterDisable(cat_filter);
		//		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(cat_filter);
		CHECK_RET(DEMUX,ret);
		cat_filter = 0;
	}
	if (0 != cat_channel)
	{
		ret = GxDemux_ChannelFree(cat_channel);
		CHECK_RET(DEMUX,ret);
		cat_channel = 0;
	}

	app_porting_psi_demux_unlock();
#endif	
	return 0;
}







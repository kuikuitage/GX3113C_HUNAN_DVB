#include <gxtype.h>
#include "service/gxsi.h"
#include "gxapp_sys_config.h"
#include "app_common_table_nit.h"
#include "app_common_flash.h"
#include "app_common_search.h"
#include "gxfrontend.h"
#include "gx_demux.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_init.h"
#include "app_common_prog.h"
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
#include "app_by_cas_api_demux.h"
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
#include "xinshimao_ca.h"
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
#include "app_desai_cas_5.0_api_demux.h"
#include "app_common_lcn.h"

#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_KN_FLAG
#include "app_kn_cas_api_demux.h"
uint8_t zone_flag = 0;
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#include "app_cd_cas_3.0_api_demux.h"
extern uint16_t cdcasNeedNitDataFlag;
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
#include "gyca_interface.h"
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_WF_CA_FLAG
#include "app_wf_cas_api_demux.h"
#endif
#endif

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_TR_FLAG
#include "app_tr_cas_api_demux.h"
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DVB_FLAG
#include "dvbca_interface.h"
#endif
#endif
static uint8_t app_nit_start_flag = 0; /*NIT搜索FILTER开启标志*/
uint8_t* pNitSectionFlag = NULL;
static int32_t s_NitSubtId = -1;
static uint32_t s_NitRequestId = 0;
extern search_fre_list searchFreList ;

static nit_parse_descriptor app_table_nit_parse_descriptor_callback = NULL;
static handle_t nit_filter =0;
static handle_t nit_channel=0;
static uint16_t network_id = 0;
#define DVT_PDSD_DESC 0x5F

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DVT_FLAG
static int DvtPDSDValue_flag=0;
static int NIT_PDSDVaule=0;
#endif
#endif

uint8_t nit_status[256]={0};
#if (DVB_OTA_TYPE ==  DVB_3H_OTA)
static char* ota_qam[]={"QAM_16","QAM_32","QAM_64","QAM_128","QAM_256"};
extern unsigned int convert_version_str(const char *str);
#endif
uint8_t zonecode = 0; 
uint16_t app_table_nit_get_network_id(void)
{
	return network_id;
}
handle_t app_table_nit_get_filter_handle(void)
{
	return nit_filter;
}

handle_t app_table_nit_get_channel_handle(void)
{
	return nit_channel;
}

void app_table_nit_register_parse_descriptor_callback(nit_parse_descriptor nit_parse_descriptor_call_back)
{
	if (NULL != nit_parse_descriptor_call_back)
		{
			app_table_nit_parse_descriptor_callback = nit_parse_descriptor_call_back;
		}
	return ;
}

void app_table_nit_search_filter_close(void)
{
	GxMsgProperty_SiRelease params_release;
	if (NULL !=pNitSectionFlag )
	{
		GxCore_Free(pNitSectionFlag);
		pNitSectionFlag = NULL;
	}
	
	if (-1 != s_NitSubtId)
	{
		params_release = s_NitSubtId;
		app_send_msg(GXMSG_SI_SUBTABLE_RELEASE, (void*)&params_release);
	}
	s_NitSubtId = -1;
	app_nit_start_flag = 0;/*NIT搜索FILTER标志清零*/
	
}

void app_table_nit_search_filter_open(void)
{

	static GxSubTableDetail subt_detail = {0};
	GxMsgProperty_SiCreate	params_create;
	GxMsgProperty_SiStart params_start;


	if (NULL !=pNitSectionFlag )
	{
		GxCore_Free(pNitSectionFlag);
		pNitSectionFlag = NULL;
	}
	app_nit_start_flag = 1; /*NIT搜索FILTER标志置1*/
	app_table_nit_monitor_filter_close(); /*关闭后台监测NIT表FILTER*/

	if(s_NitSubtId == -1)
	{
		subt_detail.ts_src = DVB_TS_SRC;
		subt_detail.demux_id = 0;
		subt_detail.time_out = 10000;
		subt_detail.si_filter.pid = NIT_PID;
		subt_detail.si_filter.match_depth = 1/*5*/;
		subt_detail.si_filter.eq_or_neq = EQ_MATCH;
		subt_detail.si_filter.match[0] = NIT_ACTUAL_NETWORK_TID;
		subt_detail.si_filter.mask[0] = 0xff;
		params_create = &subt_detail;

		subt_detail.table_parse_cfg.mode = PARSE_PRIVATE_ONLY;
		subt_detail.table_parse_cfg.table_parse_fun = app_table_nit_search_section_parse;

		app_send_msg(GXMSG_SI_SUBTABLE_CREATE,(void*)&params_create);

		s_NitSubtId = subt_detail.si_subtable_id;
		s_NitRequestId = subt_detail.request_id;
				
		// start si
		params_start = s_NitSubtId;
		app_send_msg(GXMSG_SI_SUBTABLE_START, (void*)&params_start);

	}



}

void app_table_nit_get_search_filter_info(int32_t* pNitSubtId,uint32_t* pNitRequestId)
{
	if ((NULL == pNitSubtId)||(NULL == pNitRequestId))
		return;

	*pNitSubtId = s_NitSubtId ;
	*pNitRequestId = s_NitRequestId;
	return;
}

/*
* NIT搜索，分析NIT表
*/
private_parse_status app_table_nit_search_section_parse(uint8_t* p_section_data, size_t Size)
{
	uint8_t *date, *date1;
	int16_t len1 = 0;
	int16_t len2 = 0;
	int16_t len3 = 0;
	int16_t len4 = 0;
	int16_t i = 0;
	int16_t j = 0;
	uint32_t		freq = 0;
	uint32_t		symbol = 0;
	uint32_t		modulation = 0;
	int32_t nitVersion = 0;
	uint32_t temp = 0;
	uint16_t net_work_id = 0;

	uint32_t ret = PRIVATE_SECTION_OK;
	uint16_t ts_id;
	date = p_section_data;
	network_id=net_work_id = (date[3]<<8)|date[4];
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
	XinShiMao_SetNetWorkRegionID(net_work_id);
#endif
#endif
	zonecode = date[4];



	if (NULL == p_section_data)
		return ret;
	
	if(NULL == pNitSectionFlag)
	{
		pNitSectionFlag = GxCore_Malloc(date[7]+1);
		if (NULL == pNitSectionFlag)
			return ret;
		memset(pNitSectionFlag,0,date[7]+1);
	}
	printf("app_table_nit_search_section_parse Size=%d len=%d\n",Size, ((date[1]&0x0f)<<8) + date[2] + 3 );
	printf("app_table_nit_search_section_parse date[7]=%d date[6]=%d\n",date[7],date[6]);
	if(date[0] == NIT_ACTUAL_NETWORK_TID)
	{
		pNitSectionFlag[date[6]] = 1;
		for(i = 0;i<=date[7];i++)
		{
			if(pNitSectionFlag[i]!=1)
			{
				break;
			}
		}
		if(i == (date[7]+1))
		{
			GxCore_Free(pNitSectionFlag);
			pNitSectionFlag = NULL;
			ret = PRIVATE_SUBTABLE_OK;
		}
		{
			nitVersion = (date[5] & 0x3E) >> 1;
			app_flash_save_config_center_nit_fre_version(nitVersion);
			extern uint8_t app_set_nit_change_for_search_status(uint8_t flag);
			app_set_nit_change_for_search_status(0);
			freq = searchFreList.app_fre_array[0];
			printf("MAIN_FREQ_NIT=%d\n", freq);
			printf("MAIN_FREQ_NITVERSION=%d\n", nitVersion);
			freq = 0;
		}
		len1 = ((date[8]&0xf)<<8)+date[9];
		len2 = ((date[9+len1+1]&0xf)<<8)+date[9+len1+2];
		date +=(9+1+len1+2);
		while(len2>0)
		{
			ts_id = ((date[0]<<8)&0xff00)+date[1];
			len4 = len3 = ((date[4]&0xf)<<8)+date[5];
			date1 = &date[6];
			while(len3>0)
			{
				switch(date1[0])
				{
					case 0x83:
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
						if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
						{
							lcn_descriptor_parse(date1[0],&(date1[0]),date1[1]);
						}               
#endif
						break;
					case 0x44://CABLE_DELIVERY_SYSTEM_DESCRIPTOR:
						{ // -c
							freq = ((date1[2] & 0xf0)>>4) * 10000000 + (date1[2] & 0xf) * 1000000
								+ ((date1[3] & 0xf0)>>4) * 100000 + (date1[3] & 0xf) * 10000
								+ ((date1[4] & 0xf0)>>4) * 1000 + (date1[4] & 0xf) * 100
								+ ((date1[5] & 0xf0) >> 4) * 10 + (date1[5] & 0xf);
							symbol = ((date1[9] & 0xf0)>>4) * 1000000 + (date1[9] & 0xf) * 100000
								+ ((date1[10] & 0xf0)>>4) * 10000 + (date1[10] & 0xf) * 1000
								+ ((date1[11] & 0xf0)>>4) * 100 + (date1[11] & 0xf) * 10
								+ ((date1[12] & 0xf0) >> 4) ;
							symbol = symbol /10;  // k
							freq= freq/10; // khz
//							freq = freq/1000;
							modulation = date1[8];
							printf("app_table_nit_search_section_parse cable freq=%d\n",freq);
							printf("app_table_nit_search_section_parse cable modulation=%d\n",modulation);
							for (i=0; i< searchFreList.num;i++)
							{
								if (freq == searchFreList.app_fre_array[i])
								{
									searchFreList.app_fre_tsid[i]=ts_id;
									break;
								}
							}
							if (i == searchFreList.num)
							{
								if (freq > 0)
								{
									searchFreList.app_fre_array[searchFreList.num]=freq;
									searchFreList.app_symb_array[searchFreList.num]=symbol;
									searchFreList.app_qam_array[searchFreList.num]=modulation-1;
									searchFreList.app_fre_tsid[searchFreList.num]=ts_id;
									searchFreList.num++;
								}
							}
						}
						break;
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
					case 0x5A://TERRSTRIAL_DELIVERY_SYSTEM_DESCRIPTOR:
						{
							freq = (uint32_t)((date1[2]<<24 | date1[3]<<16 | date1[4]<<8 | date1[5])*10/1000); // khz
					
//							freq = freq/1000;
							printf("nit_descriptor_parse ter freq=%d\n",freq);

							for (i=0; i< searchFreList.num;i++)
							{
								if (freq == searchFreList.app_fre_array[i])
								{
									searchFreList.app_fre_tsid[i]=ts_id;
									break;
								}
							}

							if (i == searchFreList.num)
							{
								if (freq > 0)
								{
									searchFreList.app_fre_array[searchFreList.num]=freq;
									searchFreList.app_symb_array[searchFreList.num]=app_flash_get_config_center_freq_symbol_rate();
									searchFreList.app_qam_array[searchFreList.num]=app_flash_get_config_center_freq_qam();
									searchFreList.app_fre_tsid[searchFreList.num]=ts_id;
									searchFreList.num++;
								}
							}
						}
						break;
#endif
					default:
						break;
				}
				len3 -= (date1[1]+2);
				date1 += (date1[1]+2);
			}
			date +=(6+len4);
			len2 -=(6+len4);
		}
	}

	/*
	 * 频点排序，按频点大小顺序搜索
	 */
	if (PRIVATE_SUBTABLE_OK == ret)
	{
		for (i = 0; i< searchFreList.num-1;i++)
			for (j=i+1;j<searchFreList.num;j++)
			{
				if (searchFreList.app_fre_array[j]<searchFreList.app_fre_array[i])
				{
					temp = searchFreList.app_fre_array[j];
					searchFreList.app_fre_array[j] = searchFreList.app_fre_array[i];
					searchFreList.app_fre_array[i] = temp;

					temp = searchFreList.app_symb_array[j];
					searchFreList.app_symb_array[j] = searchFreList.app_symb_array[i];
					searchFreList.app_symb_array[i] = temp;

					temp = searchFreList.app_qam_array[j];
					searchFreList.app_qam_array[j] = searchFreList.app_qam_array[i];
					searchFreList.app_qam_array[i] = temp;

					temp = searchFreList.app_fre_tsid[j];
					searchFreList.app_fre_tsid[j] = searchFreList.app_fre_tsid[i];
					searchFreList.app_fre_tsid[i] = temp;								
				}

			}
		GxCore_ThreadDelay(50);
	}
	return ret;
}

int app_table_nit_monitor_filter_close(void)
{
	/*
	 * 关闭后台监测NIT表FILTER
	 */
	printf("!!!!!!!!!!app_table_nit_monitor_filter_close!!!!!!!!!!!!!!!!!\n");
	int32_t ret = 0;
	app_porting_psi_demux_lock();
	if (0 != nit_channel)
	{
		if(nit_filter != 0)
		{
			//				ret = GxDemux_FilterDisable(nit_filter);
			//				CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(nit_filter);
			CHECK_RET(DEMUX,ret);				
			nit_filter = 0;
		}

		ret = GxDemux_ChannelFree(nit_channel);
		CHECK_RET(DEMUX,ret);
		nit_channel = 0;
	}

	memset(nit_status,0,256);

	app_porting_psi_demux_unlock();	
	return 0;
}
/*
   解析NIT表
   */
void app_table_nit_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{

	int16_t            pid;
	uint16_t            section_length;
	uint8_t*            data = (uint8_t*)Section;
	uint8_t version = 0;
	uint8_t match[6] = {0,};
	uint8_t mask[6] = {0,};
	int32_t ret;
	int32_t i=0;
	int                 len = Size;


	if (NULL == Section)
		return;

	//    ASSERT(Section != NULL);
	ret = GxDemux_FilterGetPID(Filter,&pid);
	if (NIT_PID != pid)
	{
		CAS_Dbg("app_table_nit_filter_notify pid error  pid:0x%x\n", pid);
		return ;
	}

	CAS_Dbg("app_table_nit_filter_notify total size:%d\n", Size);

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
	extern void cd_cas_30_private_data_got( CDCA_U8        byReqID,
			CDCA_BOOL      bTimeout,
			CDCA_U16       wPid,
			const CDCA_U8* pbyReceiveData,
			CDCA_U16       wLen            );
	/*交给永新视博处理。如果CA库有打开NIT filter的动作的话*/
	if(cdcasNeedNitDataFlag&0xff)
	{

		cd_cas_30_private_data_got( cdcasNeedNitDataFlag>>8,
				0,
				pid,
				data,
				len);
		if(((cdcasNeedNitDataFlag>>8)&0x80) == 0x80)
		{
			cdcasNeedNitDataFlag = 0;
		}
	}
#endif
#endif


	/*应用分析NIT表，先调用应用分析NIT表
	  之后永新CA库中设置NIT过滤分析*/
	while(len > 0) {
		app_porting_psi_demux_lock();
		if (1 != nit_status[data[6]])
		{
			nit_status[data[6]]=1;	
		}

		if(nit_status [0] != 1) /*内蒙广电必须按照section0 section1顺序来分析，所以如果先收到section1直接丢弃*/
		{
			nit_status[data[6]] = 0;
			app_porting_psi_demux_unlock();
			return ;
		}

		for (i = 0;i<=data[7];i++)
		{
			if (0 == nit_status[i])
				break;
		}
		app_porting_psi_demux_unlock();


		section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
		version = data[5] & 0x3E;
		app_table_nit_monitor_section_parse(data, section_length);

		if (i > data[7])	
		{
			app_porting_psi_demux_lock();
			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			memset(nit_status,0,256);
			match[0] = NIT_ACTUAL_NETWORK_TID;
			mask[0]  = 0xFF;
			match[5] = version;
			mask[5]  = 0x3E;
			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterSetup(Filter, match, mask, FALSE, TRUE,0, 6);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_ChannelEnable(nit_channel);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterEnable(Filter);
			CHECK_RET(DEMUX,ret);
			app_porting_psi_demux_unlock();
			return;
		}
		data += section_length;
		len -= section_length;
	}

	return;

}

int app_table_nit_monitor_filter_open(void)
{
	handle_t filter;
	uint8_t  match = NIT_ACTUAL_NETWORK_TID;
	uint8_t  mask  = 0xFF;
	int32_t ret = 0;
	CAS_Dbg("app_table_nit_monitor_filter_open\n");
	app_porting_psi_demux_lock();
	if(nit_channel != 0)
	{
		printf("nit_channel = 0x%x\n",nit_channel);
		app_porting_psi_demux_unlock();
		return 0;
	}
	printf("!!!!!!!!!!app_table_nit_monitor_filter_open!!!!!!!!!!!!!!!!!\n");
	nit_channel = GxDemux_ChannelAllocate(0, NIT_PID);
	if ((0 == nit_channel)||(-1 == nit_channel))
	{
		CA_ERROR("  nit_channel=0x%x\n",nit_channel);
		nit_channel = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}

	filter = GxDemux_FilterAllocate(nit_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(nit_channel);
		CHECK_RET(DEMUX,ret);
		nit_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}
	memset(nit_status,0,256);

	nit_filter = filter;
	ret = GxDemux_FilterSetup(filter, &match, &mask, TRUE, TRUE,0, 1);
	CHECK_RET(DEMUX,ret);
	//	GxCA_DemuxFilterRigsterNotify(filter, app_table_nit_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(nit_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);

	app_porting_psi_demux_unlock();

	return 0;
}

int app_table_nit_monitor_filter_restart(void)
{
	/*
	 * 复位后台监测NIT表FILTER
	 */
	uint8_t match[6] = {0};
	uint8_t mask[6] = {0};
	int32_t ret = 0;

	match[0] = NIT_ACTUAL_NETWORK_TID;
	mask[0]  = 0xFF;
	app_porting_psi_demux_lock();
	if ((0 == nit_filter)||(0 == nit_channel ))
	{
		app_porting_psi_demux_unlock();
		app_table_nit_monitor_filter_open();
	}
	else
	{
		memset(nit_status,0,256);
		CAS_Dbg("GxCAS_NITFilterRestart\n");
		ret = GxDemux_FilterDisable(nit_filter);
		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterSetup(nit_filter, match, mask, TRUE, TRUE,0, 1);
		CHECK_RET(DEMUX,ret);
		ret = GxDemux_ChannelEnable(nit_channel);
		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterEnable(nit_filter);
		CHECK_RET(DEMUX,ret);
		app_porting_psi_demux_unlock();
	}

	return 0;
}

//cth
void app_table_nit_monitor_section_parse(uint8_t* pSectionData,size_t Size)
{
	uint8_t*  sectionData;
	int32_t  sectionLen = 0;
	int32_t  parseTotalLen=0;
	int32_t  descriptorLen = 0;
	uint8_t  nitVersion =0;
	int32_t  oldnitVersion = 0;
	uint32_t  freq = 0;
	uint16_t regionId = 0;
	uint8_t secNum = 0;
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_GY_CAS_FLAG	
	uint16_t gynetWorkId =0;
	uint16_t gyTsId =0;
	uint16_t gyOrignetWorkId =0;
#endif
#endif
#if (DVB_OTA_TYPE ==  DVB_3H_OTA)
	static bool               otaFlag = FALSE;
	static int neimeng_ota_record = 0;
	uint8_t*    otaSectionData = NULL;
	uint16_t netWorkId =0;
	uint32_t 	   symbol = 0;
	uint16_t 	   transId =0;
	int32_t	   secondDesLen=0;
	uint16_t 	   otatransId =0xFFFF;/*内蒙广电nit分为2个section，需要分2次解析，所以前面加static*/
	uint16_t 	   otanetwordid =0xFFFF;   
	uint16_t  otaServiceId= 0xFFFF;
	int32_t ouilen = 0;
	uint32_t manufactureId = 0;
	uint8_t modulation = 0;
	uint32_t platformId = 0;
	uint8_t majorversion = 0;   
	uint8_t minorversion = 0;
	uint32_t uSWVer=0;
	uint8_t			 oemValue[20]={0};
	//uint8_t secNum = 0;
#endif
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	GxFrontend frontendpara[FRONTEND_MAX];

	if (NULL == pSectionData)
		return;
	zonecode = pSectionData[4];

	secNum = pSectionData[6];
	regionId = (pSectionData[3] << 8)|(pSectionData[4]);

	nitVersion = (pSectionData[5] & 0x3E) >> 1;

	//    	printf("app_table_nit_monitor_section_parseSize=%d\n",Size);

	sectionLen = ((pSectionData[8] & 0xF)<<8)|(pSectionData[9]);
	sectionData = pSectionData + 10;

	printf ("section num = %d\n",pSectionData[6]);
	printf ("last section num = %d\n",pSectionData[7]);
	//	nitVersion = (pSectionData[5] & 0x3E) >> 1;
	//	sectionLen = ((pSectionData[8] & 0xF)<<8)|(pSectionData[9]);
	//	sectionData = pSectionData + 10;

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DVB_FLAG
	DVBCASTB_NitDataGot(pSectionData,sectionLen);
#endif
#endif
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_GY_CAS_FLAG
	gyTsId = 0;
	gyOrignetWorkId = 0;
	gynetWorkId = regionId;
	GYCA_RegisterNetworkInfo(gynetWorkId,gyTsId,gyOrignetWorkId);
#endif
#endif

	if (sectionLen>0)
	{
		descriptorLen = 0;
		parseTotalLen = 0;
		printf("parseTotalLen:%d,sectionLen:%d\n",parseTotalLen,sectionLen);

		while (parseTotalLen < sectionLen)
		{
			descriptorLen = sectionData[1]+2;
			/*
			 * 回调分析NIT描述子，差异化
			 */
			printf("target ID:%x\n",sectionData[0]);
			switch(sectionData[0])
			{
#if (DVB_OTA_TYPE ==  DVB_3H_OTA)
				case 0x4a:
					printf("sectionData[8]:%x\n",sectionData[8]);
					if (0x09 == sectionData[8] )
					{
						otaSectionData = sectionData;
						otatransId = ((sectionData[2] )<<8)|(sectionData[3]);
						otanetwordid = ((sectionData[4] )<<8)|(sectionData[5]);
						otaServiceId = ((sectionData[6] )<<8)|(sectionData[7]);
						ouilen = sectionData[9];
						otaSectionData+=10;
						while(ouilen >0)
						{
							manufactureId = otaSectionData[0]&0x1f;
							platformId = (otaSectionData[1]&0xF0)>>4;
							majorversion = (otaSectionData[1]&0x0F);
							minorversion = (otaSectionData[2]&0xF0)>>4;
							uSWVer = (majorversion<<8)|(minorversion);
							printf("\n NIT manufactureId = 0x%X, platformId = 0x%X, majorversion=0x%X, minorversion = 0x%X,uSWVer = 0x%X\n", 
									manufactureId,
									platformId,
									majorversion,
									minorversion,
									uSWVer);

							if((manufactureId == atoi(app_flash_get_oem_manufacture_id()))&&(platformId == app_flash_get_oem_platform_id()))
							{
								//								sleep(2);
								uSWVer = (majorversion<<8)|(minorversion);
								neimeng_ota_record = otatransId;
								printf("\n NIT uSWVer = %d,otatransId:%d,neimeng_ota_record:%d\n",
										uSWVer,otatransId,neimeng_ota_record);

								{
									if(uSWVer > convert_version_str(app_flash_get_oem_softversion_str()))
									{
										otaFlag = TRUE;	
									}									
								}

							}

							otaSectionData+=4;
							ouilen-=4;
						}				
					}

					break;
#endif
				case 0x9c:
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
					app_by_cas_api_nit_zone_des(sectionData);
#endif
#endif
					break;
				case  0x5F://DVT_PDSD_DESC:
#ifdef DVB_CA_TYPE_DVT_FLAG
					{
						int32_t PDSDVaule = 0;						
						PDSDVaule = (sectionData[5]<<24)|(sectionData[6]<<16)
							|(sectionData[7]<<8)|(sectionData[8]);
						if((DvtPDSDValue_flag==0) ||(PDSDVaule != NIT_PDSDVaule))
						{
							GxBus_ConfigSetInt("DvtPDSDValue_flag", 1);
							GxBus_ConfigSetInt("DvtPDSDValue", PDSDVaule);
							NIT_PDSDVaule = PDSDVaule;
							DvtPDSDValue_flag = 1;
						}
					}

#endif

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_QZ_FLAG
					{
						int32_t AreaCode = 0;						
						AreaCode = (sectionData[2]<<24)|(sectionData[3]<<16)
							|(sectionData[4]<<8)|(sectionData[5]);
						GxBus_ConfigSetInt("QZAreaCode", AreaCode);
						printf("QZAreaCode:0x%08x\n",(uint32_t)AreaCode);
					}
#endif
#ifdef DVB_CA_TYPE_TR_FLAG

					if (dvb_ca_flag == DVB_CA_TYPE_TR  )
					{
						app_tr_cas_api_nit_zone_des(sectionData);
					}
#endif
#endif
					break;
				case 0xa5:
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
					if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )
					{
						app_desai_cas50_api_nit_zone_des(sectionData);
					}
#endif
#endif
					break;
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_KN_FLAG
				case 0xfd:
					app_kn_cas_api_nit_zone_des(&sectionData[2]);
					zone_flag = 1;
					printf("\n NIT zone_flag=%d,data=0x%x\n",zone_flag,sectionData[2]);
					break;
#endif
#ifdef DVB_CA_TYPE_WF_CA_FLAG
				case 0xfe:
					app_wf_cas_api_nit_zone_des(sectionData);
					break;
#endif
#endif
				default://ota注册
					if (NULL != app_table_nit_parse_descriptor_callback)
					{
						app_table_nit_parse_descriptor_callback(sectionData[0],sectionData);
					}
					break;

			}	
			sectionData += descriptorLen;
			parseTotalLen += descriptorLen;
		}
	}

#if (DVB_OTA_TYPE ==  DVB_3H_OTA)
	sectionLen  =	((sectionData[0] & 0xF)<<8)|(sectionData[1]);
	sectionData += 2;

	while (sectionLen > 0)
	{
		transId = (sectionData[0]<<8)|(sectionData[1]);
		netWorkId = (sectionData[2]<<8)|(sectionData[3]);
		descriptorLen = ((sectionData[4] << 8)&0xF00) | sectionData[5];
		sectionData += 6;

		if (descriptorLen>0)
		{
			secondDesLen = 0;
			parseTotalLen = 0;
			while (parseTotalLen < descriptorLen)
			{
				secondDesLen = sectionData[1]+2;
				switch(sectionData[0]) 
				{

					case 0x44:
						{ // -c
							freq = ((sectionData[2] & 0xf0)>>4) * 10000000 + (sectionData[2] & 0xf) * 1000000
								+ ((sectionData[3] & 0xf0)>>4) * 100000 + (sectionData[3] & 0xf) * 10000
								+ ((sectionData[4] & 0xf0)>>4) * 1000 + (sectionData[4] & 0xf) * 100 
								+ ((sectionData[5] & 0xf0) >> 4) * 10 + (sectionData[5] & 0xf);

							symbol = ((sectionData[9] & 0xf0)>>4) * 1000000 + (sectionData[9] & 0xf) * 100000
								+ ((sectionData[10] & 0xf0)>>4) * 10000 + (sectionData[10] & 0xf) * 1000
								+ ((sectionData[11] & 0xf0)>>4) * 100 + (sectionData[11] & 0xf) * 10 
								+ ((sectionData[12] & 0xf0) >> 4) ;

							symbol = symbol /10;  // k	
							freq= freq/10; // khz
							modulation = sectionData[8];//老版本应用增加modulation

							if ((TRUE ==  otaFlag )&&(transId == neimeng_ota_record))
								//if(TRUE == otaFlag)
							{
								printf("\n NIT freq = %d, symbol = %d,transId:%d,neimeng_ota_record:%d\n", 
										freq,
										symbol,transId,neimeng_ota_record);

								memset(oemValue,0,20);
								sprintf((char *)oemValue,"%d",symbol*1000);
								printf("symbol:%s\n",oemValue);
								app_flash_set_oem_fe_symbolrate((char *)oemValue);


								memset(oemValue,0,20);
								sprintf((char *)oemValue, "%02x.%02x.%02x.%02x",
										0,0,(unsigned int)majorversion,(unsigned int)minorversion);
								app_flash_set_oem_soft_updateversion((char *)oemValue);
								printf("\n NIT application_update_version= %s\n", oemValue);

								memset(oemValue,0,20);
								sprintf((char *)oemValue, "%d",(unsigned int)otaServiceId);

								app_flash_set_oem_dmx_serviceId((char *)oemValue);

								printf("\n NIT dmx_serviceid= %s\n", oemValue);

								app_flash_set_oem_fe_modulation((char*)ota_qam[modulation-1]);

								memset(oemValue,0,20);
								sprintf((char *)oemValue, "%d",5);

								app_flash_set_oem_repeat_times((char *)oemValue);

								memset(oemValue,0,20);
								sprintf((char *)oemValue, "%d",freq*1000);

								app_flash_set_oem_dmx_frequency((char *)oemValue);
								printf("\n NIT freq= %s\n", oemValue);
								app_flash_set_oem_ota_update_type("ts");
								app_flash_set_oem_ota_flag("yes");
								app_flash_save_oem();

								if(0 != strcmp("yes",app_flash_get_oem_ota_flag()))
								{
									app_flash_set_oem_ota_update_type("ts");
									app_flash_set_oem_ota_flag("yes"); 
									app_flash_save_oem();              
									GxCore_ThreadDelay(1000);      
									restart();
								}
								else
								{
									printf("will restart\n");
									//									sleep(5);
									app_flash_save_oem();
									restart();	
								}
							}
						}
						break; 
					default:
						break;
				}
				sectionData += secondDesLen;
				parseTotalLen += secondDesLen;
			}
		}
		sectionLen =	sectionLen -	descriptorLen - 6;
	}  
#endif

	if (0 != strcasecmp("yes",app_flash_get_oem_ota_flag()))
	{
		/*
		 * 无升级标志。检测主频点NIT版本如发生变化，退出到全屏自动搜索节目
		 */

		oldnitVersion = app_flash_get_config_center_nit_fre_version();
		freq = app_flash_get_config_center_freq();
		GxFrontend_GetCurFre(0, &frontendpara[0]);

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
		if (frontendpara[0].fre == freq*1000)
		{
			XinShiMao_SetNetWorkRegionID(regionId);	
		}
#endif
#endif

		if (0 == secNum)
			{				
				if ((frontendpara[0].fre == freq)&&(nitVersion != oldnitVersion)&&(32 != oldnitVersion))
				{
					if (0 != app_prog_get_num_in_group())
						{
							printf("frontendpara[0].fre=%d\n",frontendpara[0].fre);
							printf("nitVersion=%d\n", nitVersion);
							printf("oldnitVersion=%d\n", oldnitVersion);
							printf("freq =%d\n", freq );
							{
#if (PANEL_TYPE != PANEL_TYPE_CT1642_JINYA)
						//cth,nit表发生变化，自动搜索
						app_cas_api_on_event(DVB_CA_NIT,CAS_NAME, NULL, 0); 
#endif
					}
				}
			}			
		}

	}
	return ;
}



#include <gxtype.h>
#include "service/gxsi.h"
#include "gxapp_sys_config.h"
#include "app_common_table_ota.h"
#include "app_common_flash.h"
#include "app_common_search.h"
#include "app_common_init.h"
#include "gxfrontend.h"
#include "gx_demux.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"

#define UPDATE_TYPE_MANUAL			(0x01)
#define UPDATE_TYPE_FORCE				(0x02)
#define UPDATE_TYPE_BATCH				(0x03)
#define UPDATE_TYPE_SERIAL       		(0x04)
#define SN_MAX_LEN 	  				(29)
#define OTA_CAS_DOWNLOAD_DESC			0xA1

static handle_t ota_filter =0;
static handle_t ota_channel=0;
//static ota_parse_descriptor app_table_ota_parse_descriptor_callback = NULL;
uint16_t back_pid = 0;
uint8_t back_tableid = 0;
uint8_t count_flag = 0;

uint8_t ota_flag = 0;
uint16_t ota_pid = 0x1fff;
uint8_t ota_table_id = 0x0;
static char* ota_qam[]={"QAM_16","QAM_32","QAM_64","QAM_128","QAM_256"};
extern unsigned int htoi(const char *str);
extern unsigned int convert_version_str(const char *str);
extern int app_ota_popmsg(uint32_t x, uint32_t y, const char* context, int32_t ri_Timeout);

/*void app_table_ota_register_parse_descriptor_callback(ota_parse_descriptor ota_parse_descriptor_call_back)
{
	if (NULL != ota_parse_descriptor_call_back)
		{
			app_table_ota_parse_descriptor_callback = ota_parse_descriptor_call_back;
		}
	return ;
}*/


handle_t app_table_ota_get_filter_handle(void)
{
	return ota_filter;
}

handle_t app_table_ota_get_channel_handle(void)
{
	return ota_channel;
}


int app_table_ota_monitor_filter_close(void)
{
	int32_t ret = 0;
	app_porting_psi_demux_lock();

	if (0 != ota_channel)
		{
			printf("!!!!!!!!!!app_table_ota_monitor_filter_close!!!!!!!!!!!!!!!!!\n");

			if(ota_filter != 0)
			{
//				ret = GxDemux_FilterDisable(ota_filter);
//				CHECK_RET(DEMUX,ret);
				ret = GxDemux_FilterFree(ota_filter);
				CHECK_RET(DEMUX,ret);				
				ota_filter = 0;
			}

			ret = GxDemux_ChannelFree(ota_channel);
			CHECK_RET(DEMUX,ret);
			ota_channel = 0;
		}
	app_porting_psi_demux_unlock();

	return 0;
}


 void app_table_ota_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
	
    int16_t            pid;
    uint16_t            section_length;
    uint8_t*            data = (uint8_t*)Section;
    uint8_t version = 0;
    uint8_t match[8] = {0,};
    uint8_t mask[8] = {0,};
    int32_t ret;

	if (NULL == Section)
		return;

//    ASSERT(Section != NULL);
	app_porting_psi_demux_lock();

    ret = GxDemux_FilterGetPID(Filter,&pid);
    if (ota_pid != pid)
    	{
             CAS_Dbg("app_table_ota_filter_notify pid error  pid:0x%x\n", pid);
			 app_porting_psi_demux_unlock();
    		return ;
    	}
	app_porting_psi_demux_unlock();

    CAS_Dbg("app_table_ota_filter_notify total size:%d\n", Size);
 


	/*应用分析NIT表，先调用应用分析NIT表
	   之后永新CA库中设置NIT过滤分析*/

	section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
	version = data[5] & 0x3E;
	app_table_ota_monitor_section_parse(data, section_length);
	{
			app_porting_psi_demux_lock();
			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			match[0] = ota_table_id;
			mask[0]  = 0xFF;
			match[5] = version;
			mask[5]  = 0x3E;
			match[6] = 0x00; // section number 0
			mask[6]  = 0xFF;

			ret = GxDemux_FilterDisable(Filter);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterSetup(Filter, match, mask, FALSE, TRUE,0, 8);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_ChannelEnable(ota_channel);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterEnable(Filter);
			CHECK_RET(DEMUX,ret);
			app_porting_psi_demux_unlock();

	}
			
        return;
		
}

int app_table_ota_monitor_filter_open(uint16_t pid,uint8_t tabieid)
{
	handle_t filter;
    uint8_t match[8] = {0,};
    uint8_t mask[8] = {0,};

	int32_t ret = 0;
	match[0] = tabieid;
	mask[0]  = 0xFF;
	match[6] = 0x00; // section number 0
	mask[6]  = 0xFF;
    if(0 == count_flag)
    {
        back_pid = pid;
        back_tableid = tabieid;
        count_flag = 1;
    }
	CAS_Dbg("app_table_ota_monitor_filter_open\n");
	app_porting_psi_demux_lock();

	if(ota_channel != 0)
	{
		printf("ota_channel = 0x%x\n",ota_channel);
		app_porting_psi_demux_unlock();

		return 0;
	}
	printf("!!!!!!!!!!app_table_ota_monitor_filter_open!!!!!!!!!!!!!!!!!\n");
	ota_channel = GxDemux_ChannelAllocate(0, pid);
	if ((0 == ota_channel)||(-1 == ota_channel))
		{
			CA_ERROR("  nit_channel=0x%x\n",ota_channel);
			ota_channel = 0;
			app_porting_psi_demux_unlock();

			return 0;
		}

	filter = GxDemux_FilterAllocate(ota_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(ota_channel);
		CHECK_RET(DEMUX,ret);
		ota_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();

		return 0;
	}

	ota_filter = filter;
	ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 8);
	CHECK_RET(DEMUX,ret);
//	GxCA_DemuxFilterRigsterNotify(filter, app_table_nit_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(ota_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	ota_pid = pid;
	ota_table_id = tabieid;
	app_porting_psi_demux_unlock();
	


	return 0;
}

int app_table_ota_monitor_filter_restart(void)
{	
			handle_t filter;
			uint8_t match[8] = {0,};
			uint8_t mask[8] = {0,};
			
			int32_t ret = 0;
			match[0] = ota_table_id;
			mask[0]  = 0xFF;
			match[6] = 0x00; // section number 0
			mask[6]  = 0xFF;
			app_porting_psi_demux_lock();

			if (0x1fff == ota_pid)
				{
					/*
					* ota pid invalid 
					*/
					app_porting_psi_demux_unlock();
					return 0;
				}


			CAS_Dbg("app_table_ota_monitor_filter_restart\n");
		
		
			if (0 != ota_filter)
			{
//				ret = GxDemux_FilterDisable(ota_filter);
//				CHECK_RET(DEMUX,ret);
				ret = GxDemux_FilterFree(ota_filter);
				CHECK_RET(DEMUX,ret);
				ota_filter = 0;
			}
			if (0 != ota_channel)
			{
				ret = GxDemux_ChannelFree(ota_channel);
				CHECK_RET(DEMUX,ret);
				ota_channel = 0;
			}
		
			ota_channel = GxDemux_ChannelAllocate(0, ota_pid);
			if ((0 == ota_channel)||(-1 == ota_channel))
				{
					CA_ERROR(" ota_channel=0x%x\n",ota_channel);
					ota_channel = 0;
					app_porting_psi_demux_unlock();
					return 0;
				}
			filter = GxDemux_FilterAllocate(ota_channel);
			if (( 0 == filter)||(-1 == filter))
			{
				ret = GxDemux_ChannelFree(ota_channel);
				CHECK_RET(DEMUX,ret);
				ota_channel = 0;
		
				CA_ERROR(" filter=0x%x\n",filter);
				filter = 0;
				app_porting_psi_demux_unlock();
				return 0;
			}
		
		
			ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 8);
			CHECK_RET(DEMUX,ret);
		//	GxCA_DemuxFilterRigsterNotify(filter, app_table_cat_filter_notify,NULL);
			ret = GxDemux_ChannelEnable(ota_channel);
			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterEnable(filter);
			CHECK_RET(DEMUX,ret);
			ota_filter = filter;
			app_porting_psi_demux_unlock();

	return 0;
}
//cth
#ifdef USE_OTA_DOWNLOAD_DESC
static void ota_parse_descriptor(uint8_t* pSectionData)
{
	uint8_t *pData = pSectionData;
	int32_t iLength = 0;
	char oemValue[20] = {0};
	GxFrontend frontendpara[FRONTEND_MAX];
	char* softwareVer = NULL;
	char* hardwareVer = NULL;
	uint32_t m_Frequency = 0;
	uint32_t m_PlatformID = 0;
	uint8_t m_Modulation = 0;
	uint32_t m_SymbolRate = 0;
	char* platformIDStr = NULL;
	unsigned int manufacture_id = 0;		
	unsigned int soft_ver = 0;
	unsigned int hard_ver = 0;
	unsigned int update_type = 0;
	uint16_t m_PID = 0;
	uint8_t m_TableID = 0;
	uint8_t m_UpdateTimes = 0;
	uint32_t cmperr = 0;
	char serialNumber[SN_MAX_LEN] = {0};
	char caUser[SN_MAX_LEN] = {0};
	uint64_t manufSerial = 0;
	uint64_t sequenceStart = 0;
	uint64_t sequenceEnd = 0;	
	uint8_t bSwVerHigh = TRUE;

	if (NULL == pSectionData)
	{
		return;
	}
	
	printf("+[update]ota_parse_descriptor----------------------------------.\n");
	manufacture_id = (pData[2] << 8) | pData[3];
	platformIDStr = app_flash_get_oem_manufacture_id();
	m_PlatformID = htoi(platformIDStr);
	if (manufacture_id != m_PlatformID)
	{
		CAS_Dbg("+[update]manufacture_id=0x%x m_PlatformID=0x%x!!!\n", manufacture_id, m_PlatformID);
		cmperr = 1;
	}

	pData += 4;
	iLength = pData[1];

	while (iLength > 0)
	{
		if ((0x44 == pData[0]) || (0x5A == pData[0]))
		{
			uint32_t uNoOfDataBytes = 0;

			uNoOfDataBytes = ((pData[2] >> 4) & 0xF) * 10 + (pData[2] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100 + ((pData[3] >> 4) & 0xF) * 10 + (pData[3] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100 + ((pData[4] >> 4) & 0xF) * 10 + (pData[4] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100 + ((pData[5] >> 4) & 0xF) * 10 + (pData[5] & 0xF);
			m_Frequency = uNoOfDataBytes ;
			m_Frequency = m_Frequency/10;

			m_Modulation = pData[8];
			uNoOfDataBytes = ((pData[9] >> 4) & 0xF) * 10 + (pData[9] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100 + ((pData[10] >> 4) & 0xF) * 10 + (pData[10] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 100 + ((pData[11] >> 4) & 0xF) * 10 + (pData[11] & 0xF);
			uNoOfDataBytes = uNoOfDataBytes * 10 + ((pData[12] >> 4) & 0xF);
			m_SymbolRate = uNoOfDataBytes;
			m_SymbolRate = m_SymbolRate/10;

			iLength -= (pData[1] + 2);
			pData += (pData[1] + 2);
			printf("+[update]>>Freq=%d, Symbol=%d, QAM=%d.\n", m_Frequency, m_SymbolRate, m_Modulation);
		}
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
		else if (0x5A == pData[0])
		{
			m_Frequency = (uint32_t)(((pData[2] << 24) | (pData[3] << 16) | (pData[4] << 8) | pData[5])*10/1000); // khz
			iLength -= (pData[1] + 2);
			pData += (pData[1] + 2);
			printf("+[update]>>Freq(%d).\n",m_Frequency);
		}
#endif
		else			
		{
			/*not valid, skips.*/
			iLength -= (pData[1] + 2);
			pData += (pData[1] + 2);

			/*skip to private data.*/
			iLength -= (pData[2] + 3);
			pData += (pData[2] + 3);
			continue;
		}
		
		m_PID = (pData[0] << 8) | pData[1];
		m_PID >>= 3;
		update_type = pData[1] & 0x07;
		
		m_TableID = pData[3];
		m_UpdateTimes = pData[4];
		printf("+[update]>>PID = 0x%x;\n", m_PID);
		printf("+[update]>>DownType = %d;\n", update_type);
		printf("+[update]>>TableID = 0x%x;\n", m_TableID);
		printf("+[update]>>Times = %d.\n", m_UpdateTimes);

		/*Compare Hardware.*/
		hard_ver = (pData[5] << 24) | (pData[6] << 16) | (pData[7] << 8) | (pData[8]);
		hardwareVer = app_flash_get_oem_hardware_version_str();
		if (hard_ver != convert_version_str(hardwareVer))
		{
			printf("+[update]uHWVer(0x%x) != hardwareVerOem(0x%x)!!!\n",\
				   	hard_ver, convert_version_str(hardwareVer));	
			cmperr = 2;	
		}

		/*Compare Software.*/
		bSwVerHigh = TRUE;
		soft_ver = (pData[9] << 24) | (pData[10] << 16) | (pData[11] << 8) | (pData[12]);
		printf("+[update]>>HW=V%x, SoftVer=V%x.\n", hard_ver, soft_ver);
		
		softwareVer = app_flash_get_oem_softversion_str();
		if (soft_ver <= convert_version_str(softwareVer))
		{
			printf("+[update]SwVer(0x%x) < softwareVerOem(0x%x)!!!\n",\
				   	soft_ver, convert_version_str(softwareVer));	
			cmperr = 3;
			bSwVerHigh = FALSE;
		}
		
		/*Compare SN.*/
		if ((0 == cmperr) && (UPDATE_TYPE_SERIAL == update_type))
		{
			app_flash_get_serial_number(0, (char *)caUser, SN_MAX_LEN);
			memset(serialNumber, 0, SN_MAX_LEN);
			memcpy(serialNumber, caUser, SN_MAX_LEN);
			manufSerial = atol(serialNumber);
			
			if ((manufSerial >= sequenceStart) && (manufSerial <= sequenceEnd))
			{
				/*按序列号升级*/
				printf("+++[update]<按序列号升级>stbId(%lld); m_seqS(%lld); m_seqE(%lld).+++\n",
						manufSerial, sequenceStart, sequenceEnd);
			}
			else
			{
				cmperr = 5;
				printf("---[update]invalid stbId(%lld); m_seqS(%lld); m_seqE(%lld)!!!---\n",
						manufSerial, sequenceStart, sequenceEnd);
			}	
		}
	
		if ((0 == cmperr))
		{
			printf("+[update]>>软件需要升级---+++++++++++++++++++++=.\n");
			/* promt start */
			
			memset(oemValue, 0, 20);
			sprintf((char *)oemValue, "%02x.%02x.%02x.%02x",\
				      pData[9], pData[10], pData[11], pData[12]);
			app_flash_set_oem_soft_updateversion((char *)oemValue);

			memset(oemValue,0,20);
			sprintf((char*)oemValue, "0x%x", m_PID);
			app_flash_set_oem_dmx_pid((char*)oemValue);
			
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "0x%x", m_TableID);
			app_flash_set_oem_dmx_tableid((char*)oemValue);
			
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "0x%x", m_UpdateTimes);
			app_flash_set_oem_repeat_times((char*)oemValue);
			
			/*
			  升级私有表和升级流在同一个频点中下发，检测到升级描述子记录
			  当前频点信息，OTA锁当前频点过滤升级流*/
			memset(oemValue,0,20);  
			GxFrontend_GetCurFre(0, &frontendpara[0]);
			m_Frequency = frontendpara[0].fre;
			if (0 == m_Frequency)
			{
				m_Frequency = app_flash_get_config_center_freq();
			}
			
			m_Frequency = m_Frequency*1000;
			sprintf((char*)oemValue, "%d", m_Frequency);
			app_flash_set_oem_dmx_frequency((char*)oemValue);
			CAS_Dbg("+[update]m_Frequency = %s#\n",oemValue);
			
			memset(oemValue,0,20);
			m_SymbolRate = frontendpara[0].symb;
			sprintf((char*)oemValue, "%d",m_SymbolRate);
			app_flash_set_oem_fe_symbolrate((char*)oemValue);

			m_Modulation = frontendpara[0].qam;
			app_flash_set_oem_fe_modulation((char*)ota_qam[m_Modulation-1]);

			memset(oemValue,0,20);
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			if (frontendpara[0].type_1501 == GXBUS_PM_SAT_1501_DTMB)  //DTMB
			{
				sprintf((char*)oemValue,"%s","DTMB");
			}
			else
			{
				sprintf((char*)oemValue,"%s","DVB_C");
			}
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			sprintf((char*)oemValue,"%s","DVB_C");
#endif
			app_flash_set_oem_fe_workmode((char*)oemValue);

			if (0 != strcasecmp("yes", app_flash_get_oem_ota_flag()))
			{
				/* 无升级标志。检测升级描述子存在，立即触发升级。*/
				app_flash_set_oem_ota_flag("yes");
				app_flash_save_oem();

				app_cas_api_on_event(DVB_CA_OTA,CAS_NAME, NULL, 0);
			}
			else
			{
				/*如果升级标志已保存(之前选择取消)，不触发*/
				app_flash_save_oem();
			}
			
			return;
		}				
		
		iLength -= (pData[2] + 3);
		pData += (pData[2] + 3);
	}
	
	return;
}
#endif

void app_table_ota_monitor_section_parse(uint8_t* pSectionData,size_t Size)
{
	uint8_t *pData = pSectionData;
	#ifdef USE_OTA_DOWNLOAD_DESC
	uint8_t *sectionData = NULL;
	int32_t  sectionLen = 0;
	int32_t  parseTotalLen = 0;
	int32_t  descriptorLen = 0;
	#else
	uint16_t wSectionLength = 0;
	uint16_t wSectionNumber = 0;
	uint8_t  chFileHeadLength = 0;
	uint8_t  chTableId = 0;
	uint16_t wLastSectionNumber = 0;
	char oemValue[20]={0};
	GxFrontend frontendpara[FRONTEND_MAX];
	char*          softwareVer = NULL;
	char*          hardwareVer = NULL;
	uint32_t       m_Frequency=0;
	uint32_t       m_PlatformID = 0;
	uint8_t        m_Modulation=0;
	uint32_t       m_SymbolRate=0;
	char*          PlatformIDStr = NULL;
	uint32_t	   cmperr=0;
	#endif

	if (NULL == pSectionData)
		return;

	printf("app_table_ota_monitor_section_parse=%d\n",Size);

	#ifdef USE_OTA_DOWNLOAD_DESC
	sectionLen = ((pData[8] & 0xF) << 8) | (pData[9]);
	sectionData = pData + 10;

	if (sectionLen > 0)
	{
		descriptorLen = 0;
		parseTotalLen = 0;

		while (parseTotalLen < sectionLen)
		{
			descriptorLen = sectionData[1]+2;
			switch (sectionData[0])
			{
				case OTA_CAS_DOWNLOAD_DESC:
				{
					ota_parse_descriptor(sectionData);
				}
				break;

				default:
				break;
			}

			sectionData += descriptorLen;
			parseTotalLen += descriptorLen;
		 }
	}
	#else
	chTableId	   = pData[0];
	wSectionLength = (((uint16_t)pData[1] << 8) | pData[2]) & 0x0fff;
	wSectionNumber = ((uint16_t)pData[6] << 8 )| pData[7];
	wLastSectionNumber = ((uint16_t)pData[8] << 8 )| pData[9];
	pData += 10;
	/*第一个分段解析，第一个分段中包含版本控制信息
	  需要分开解析。具体版本信息参考相关文档*/
	if(0 == wSectionNumber)
	{
		unsigned int manufacture_id = 0;		
		unsigned int soft_ver = 0;
		unsigned int hard_ver = 0;
		unsigned int update_type = 0;

		chFileHeadLength = *pData++;
		CAS_Dbg("section 0  head len = %d \n",chFileHeadLength);
		
		update_type = (*(pData+12));
		CAS_Dbg("update_type = %x\n",update_type);

		manufacture_id = ((*pData)<<24)
			+(*(pData+1)<<16)
			+(*(pData+2)<<8)
			+(*(pData+3));
		pData += 4;
		CAS_Dbg("manufacture_id = %x\n",manufacture_id);
		PlatformIDStr = app_flash_get_oem_manufacture_id();
		m_PlatformID = htoi(PlatformIDStr);

		if (manufacture_id != m_PlatformID)
		{
			CAS_Dbg("manufacture_id=0x%x m_PlatformID=0x%x\n",manufacture_id,m_PlatformID);
			cmperr =1;
		}

		hard_ver = ((*pData)<<24)
			+(*(pData+1)<<16)
			+(*(pData+2)<<8)
			+(*(pData+3));
		memset(oemValue,0,20);
		sprintf(oemValue,"%02x.%02x.%02x.%02x",(*pData),*(pData+1),*(pData+2),*(pData+3));
		CAS_Dbg("hardware_version = %s\n",oemValue);

		//匹配硬件版本号，ps: 版本号00.00.00.00,转成字符串00.00.00.00保存
		hardwareVer = app_flash_get_oem_hardware_version_str();
		if (hard_ver  != convert_version_str(hardwareVer))
		{
			cmperr =2;	
		}

		pData += 4;

		soft_ver = ((*pData)<<24)
			+(*(pData+1)<<16)
			+(*(pData+2)<<8)
			+(*(pData+3));
		//匹配软件版本号

		memset(oemValue,0,20);
		sprintf(oemValue,"%02x.%02x.%02x.%02x",(*pData),*(pData+1),*(pData+2),*(pData+3));
		pData += 4;
		CAS_Dbg("softver = %x\n",soft_ver);		

		softwareVer = app_flash_get_oem_softversion_str();
		if(soft_ver <= convert_version_str(softwareVer))
		{
			cmperr =3;
		}
		if((0 == cmperr)||((5 ==update_type)&&(cmperr !=3)))
		{
			printf("\n----> enter update now.\n");
			/*升级类型1=手动下载2=强制升级*/
			/*发送消息至APP，需要进行更新提示*/
			printf("[update]>>需要升级，界面提示\n");
			app_flash_set_oem_soft_updateversion((char*)oemValue);

			memset(oemValue,0,20);
			sprintf((char*)oemValue, "0x%x",ota_pid);
			app_flash_set_oem_dmx_pid((char*)oemValue);
			CAS_Dbg("ota_pid = %s\n",oemValue);	
			memset(oemValue,0,20);
			sprintf((char*)oemValue, "0x%x",ota_table_id);
			app_flash_set_oem_dmx_tableid((char*)oemValue);
			CAS_Dbg("ota_table_id = %s\n",oemValue);	

			/*				memset(oemValue,0,20);
							sprintf((char*)oemValue, "%d",m_UpdateTimes);
							app_flash_set_oem_repeat_times((char*)oemValue);*/
			memset(oemValue,0,20);
			/*
			   升级私有表和升级流在同一个频点中下发，检测到升级描述子记录
			   当前频点信息，OTA锁当前频点过滤升级流*/

			GxFrontend_GetCurFre(0, &frontendpara[0]);
			m_Frequency = frontendpara[0].fre;
			m_Frequency = m_Frequency*1000;
			sprintf((char*)oemValue, "%d",m_Frequency);
			app_flash_set_oem_dmx_frequency((char*)oemValue);
			CAS_Dbg("m_Frequency = %s\n",oemValue);


			memset(oemValue,0,20);
			m_SymbolRate = frontendpara[0].symb;
#ifdef CUST_TAIKANG
			m_SymbolRate = 6875000;//else ota sym invalid
#endif
			CAS_Dbg("m_SymbolRate = %d\n",m_SymbolRate);
			sprintf((char*)oemValue, "%d",m_SymbolRate);
			app_flash_set_oem_fe_symbolrate((char*)oemValue);
			CAS_Dbg("m_SymbolRate = %s\n",oemValue);


			m_Modulation = frontendpara[0].qam;
			app_flash_set_oem_fe_modulation((char*)ota_qam[m_Modulation-1]);
			CAS_Dbg("qam = %s\n",(char*)ota_qam[m_Modulation-1]);


			memset(oemValue,0,20);
			//				GxFrontend_GetCurFre(0, &frontendpara[0]);
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			if(frontendpara[0].type_1501 == GXBUS_PM_SAT_1501_DTMB)  //DTMB
			{
				sprintf((char*)oemValue,"%s","DTMB");
			}
			else
			{
				sprintf((char*)oemValue,"%s","DVB_C");
			}
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			sprintf((char*)oemValue,"%s","DVB_C");
#endif
			CAS_Dbg("DVB_MODE = %s\n",oemValue);

			app_flash_set_oem_fe_workmode((char*)oemValue);

				if (0 != strcasecmp("yes",app_flash_get_oem_ota_flag()))
				{
					/*
					* 无升级标志。检测升级描述子存在，立即触发升级。*/
					
					app_flash_set_oem_ota_update_type("ts");
					app_flash_set_oem_ota_flag("yes");
					app_flash_save_oem();

					app_cas_api_on_event(DVB_CA_OTA,CAS_NAME, NULL, 0);
				}
				else
					{
						/*
						* 如果升级标志已保存(之前选择取消)，不触发
						*/
						app_flash_save_oem();
					}
				return ;

			}				
		}
#endif 

	return ;
}



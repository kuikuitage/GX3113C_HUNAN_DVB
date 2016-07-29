#include <gxtype.h>
#include "gxapp_sys_config.h"
#include "app_common_flash.h"
#include "app_common_play.h"
#include "app_common_prog.h"
#include "app_common_table_cat.h"
#include "app_common_table_pmt.h"
#include "app_common_table_nit.h"
#include "app_common_table_ota.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "gxfrontend.h"
#include "gx_demux.h"
#include "app_common_porting_ca_descramble.h"
#include "app_common_ad.h"
#ifdef DVB_AD_TYPE_BY_FLAG
//#include "adver.h"
#include "app_by_ads_porting_stb_api.h"
#endif

#if MEDIA_SUBTITLE_SUPPORT
#include "bus/module/si/si_pmt.h"
#include "app_ttx_subt.h"
extern void app_subt_change(void);
#endif


static handle_t pmt_filter =0;
static handle_t pmt_channel=0;
#ifdef  DVB_CA_TYPE_TR_FLAG
#include "Tr_Cas.h"

static char pmtData[1024];
#endif

uint16_t pmt_pid = PSI_INVALID_PID;
uint16_t pmt_serviceid = 0xFFFF;

#if MEDIA_SUBTITLE_SUPPORT
SubtDescriptor* pSubtDescriptor = NULL;
TeletextDescriptor* pTtxDescriptor = NULL;
PmtInfo pmt_info;
uint8_t s_subt_sync_flag = 0;
extern TtxSubtOps g_AppTtxSubt;
#endif

handle_t app_table_pmt_get_filter_handle(void)
{
	return pmt_filter;
}

//分析处理pmt信息
void app_table_pmt_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
	
    uint16_t            section_length;
	uint16_t            serviceId;
    int16_t            pid;
    int32_t ret;
    uint8_t*            data = (uint8_t*)Section;

	if (NULL == Section)
		return;

//    ASSERT(Section != NULL);
    section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
	
	serviceId = (data[3] << 8)|(data[4]);
    ret = GxDemux_FilterGetPID(Filter,&pid);
	if (pid != pmt_pid)
		{
			CAS_Dbg("%s  pmt_filter=0x%x  Filter=0x%xerror	\n", __func__,pid,pmt_pid);
			return ;	
		}

    if (pmt_filter != Filter)
	{
         CAS_Dbg("%s pmt_filter=0x%x  Filter=0x%xerror  \n", __func__,pmt_filter,Filter);
		return ;
	}


	/*根据tableid判断是否为当前PMT表*/
	
	if ((PMT_TABLE_ID == data[0])&&(serviceId ==  pmt_serviceid))
	{	// pmt table
		app_table_pmt_section_parse(data,section_length);
		return;
	}
			
    return;	
}


int app_table_pmt_filter_open(uint16_t pid,uint32_t service_id,uint8_t version,uint8_t versionEQ)
{
	handle_t filter;
	uint8_t     match[18] = {0,};
	uint8_t     mask[18] = {0,};
	int32_t  ret = 0;
	
	CAS_Dbg("[app_table_pmt_filter_open]pid(0x%x); servId(0x%x).\n", pid, service_id);
	match[0] = 0x2;
	mask[0] = 0xff;
	match[3] = (service_id>>8);
	mask[3] = 0xff;
	match[4] = service_id;
	mask[4] = 0xff;

	app_porting_psi_demux_lock();

	pmt_pid = pid;
	if (FALSE== versionEQ)
	{
		match[5] = version;
		mask[5] = 0x3E;
	}
	if (0 != pmt_filter)
	{
//		ret = GxDemux_FilterDisable(pmt_filter);
//		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(pmt_filter);
		CHECK_RET(DEMUX,ret);
		pmt_filter = 0;
	}
	if (0 != pmt_channel)
	{
		ret = GxDemux_ChannelFree(pmt_channel);
		CHECK_RET(DEMUX,ret);
		pmt_channel = 0;
	}
	pmt_channel = GxDemux_ChannelAllocate(0, pid);
	if ((0 == pmt_channel)||(-1 == pmt_channel))
		{
			CA_ERROR("  pmt_channel=0x%x\n",pmt_channel);
			pmt_channel = 0;
			app_porting_psi_demux_unlock();
			return 0;
		}

	filter = GxDemux_FilterAllocate(pmt_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(pmt_channel);
		CHECK_RET(DEMUX,ret);
		pmt_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();

		return 0;
	}

	pmt_serviceid = service_id;
	ret = GxDemux_FilterSetup(filter, match, mask, versionEQ, TRUE,0, 6);
	CHECK_RET(DEMUX,ret);
//	GxCA_DemuxFilterRigsterNotify(filter, app_table_pmt_filter_notify,NULL);
	ret = GxDemux_ChannelEnable(pmt_channel);
	CHECK_RET(DEMUX,ret);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	pmt_filter = filter;
	app_porting_psi_demux_unlock();

	return 0;
}

int app_table_pmt_filter_close(void)
{
	int32_t  ret = 0;
	app_porting_psi_demux_lock();

	if (0 != pmt_filter)
	{
//		ret = GxDemux_FilterDisable(pmt_filter);
//		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterFree(pmt_filter);
		CHECK_RET(DEMUX,ret);
		pmt_filter = 0;
	}
	if (0 != pmt_channel)
	{
		ret = GxDemux_ChannelFree(pmt_channel);
		CHECK_RET(DEMUX,ret);
		pmt_channel = 0;
	}
	pmt_serviceid = 0xFFFF;
	app_porting_psi_demux_unlock();

	return 0;
}

#if MEDIA_SUBTITLE_SUPPORT
void app_pmt_ttx_subt_clear(void)
{
	uint8_t i = 0;
	SubtDescriptor * pSubt = NULL; 
	TeletextDescriptor *pTtx = NULL;

	for(i = 0; i<pmt_info.subt_count; i++)
	{
		pSubt = (SubtDescriptor*)pmt_info.subt_info[i];
		GxCore_Free(pSubt->subt);
	}
	if(i == pmt_info.subt_count && i!=0)
	{
		GxCore_Free(pSubtDescriptor);
		pSubtDescriptor = NULL;
		GxCore_Free(pmt_info.subt_info);
	}
	for(i = 0; i<pmt_info.ttx_count; i++)
	{
		pTtx = (TeletextDescriptor*)pmt_info.ttx_info[i];
		GxCore_Free(pTtx->ttx);
	}
	if(i == pmt_info.ttx_count && i!=0)
	{
		GxCore_Free(pTtxDescriptor);
		pTtxDescriptor = NULL;
		GxCore_Free(pmt_info.ttx_info);
	}

	memset(&pmt_info, 0, sizeof(PmtInfo));
	g_AppTtxSubt.sync(&g_AppTtxSubt,&pmt_info);
}
#endif

//解析PMT数据
private_parse_status app_table_pmt_section_parse(uint8_t* pSectionData, size_t Size)
{
	static uint32_t nOldTpId = 0xFFFFFFFF;
	uint8_t      *sectionData;
	uint8_t      *streamSceton;
	int32_t     secTotalLen = 0;
	int32_t  sectionLen;//section段总长度
	u_int16_t     serviceId;
	int32_t     parseTotalLen=0;//已经解析完成的section段长度
	int32_t     descriptorLen = 0;//当前解析的section段长度
	u_int16_t     pid;
	int32_t 	esInfoLen = 0;
    uint8_t version = 0;
	u_int16_t program_num = 0xFFFF;
	u_int16_t p_pcr_pid = PSI_INVALID_PID;
	u_int16_t p_video_pid = PSI_INVALID_PID;
	u_int16_t p_audio_pid = PSI_INVALID_PID;
	u_int16_t p_ecm_pid = PSI_INVALID_PID;
	u_int16_t p_ecm_pid_video = PSI_INVALID_PID;
	u_int16_t p_ecm_pid_audio = PSI_INVALID_PID;
	uint8_t stream_type = 0;
	uint8_t video_type = GXBUS_PM_PROG_MPEG;
	uint8_t audio_type = GXBUS_PM_AUDIO_MPEG1;
	GxBusPmDataProg Prog={0};
	uint32_t pos = 0;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#if MEDIA_SUBTITLE_SUPPORT
	int32_t 	  indescriptorLen = 0;
	uint8_t subt_num = 0;
	uint8_t ttx_num = 0;
	uint8_t subt_count = 0;
	uint8_t ttx_count = 0;
	uint8_t i = 0;
	Subtiling *pSubtitling = NULL;
	Teletext *pTeleText = NULL;;
#endif


	 if (NULL == pSectionData)
		return  PRIVATE_SECTION_OK;

#ifdef CA_FLAG
	bool program_type = FALSE;
	uint8_t scramble_type = 0;
	u_int16_t ecmPid =PSI_INVALID_PID;
	u_int16_t caSystemId = 0;
	play_scramble_para_t playPara={0};
	play_scramble_para_t oldplayPara={0};

#endif
#if MEDIA_SUBTITLE_SUPPORT
	app_pmt_ttx_subt_clear();
#endif

    version = pSectionData[5] & 0x3E;
	serviceId = (pSectionData[3] << 8)|(pSectionData[4]);
	p_pcr_pid = ((pSectionData[8] & 0x1F)<<8)|(pSectionData[9]); // pcr pid

	secTotalLen = ((pSectionData[1]&0x0f)<<8) + pSectionData[2] + 3;	
	printf("[app_table_pmt_section_parse]serviceId(0x%x); Size(%d); secTotalLen(%d).\n",\
		  	serviceId, Size, secTotalLen);
	

	if (dvb_ca_flag == DVB_CA_TYPE_TR  )			
	{
#ifdef  DVB_CA_TYPE_TR_FLAG
		int nRet = 0;

		memset(pmtData, 0x00, 1024);
		memcpy(pmtData, pSectionData, Size);
		nRet = MC_MNGR_PostPsiTable(CAS_PMT_UPDATE, pmtData);
		if (0 == nRet)
		{  
			//printf("[app_table_pmt_section_parse]MC_MNGR_PostPsiTable update PMT OK.\n");
		}
		else
		{
			printf("[app_table_pmt_section_parse]MC_MNGR_PostPsiTable update PMT fail!!!\n");
		}
#endif
	}
	if (Size != secTotalLen)
	{
		printf("[app_table_pmt_section_parse] Size=%d secTotalLen=%d#\n", Size, secTotalLen);
		return  PRIVATE_SECTION_OK;
	}
	secTotalLen = Size-3;
	program_num = serviceId;//节目号

	if (secTotalLen>1021)
	{
		printf("[app_table_pmt_section_parse]secTotalLen=%d.\n",secTotalLen);
		return  PRIVATE_SECTION_OK;
	}

	sectionLen = ((pSectionData[10] & 0xF)<<8)|(pSectionData[11]);

	if (sectionLen>=secTotalLen)
	{
		printf("[app_table_pmt_section_parse]sectionLen=%d secTotalLen=%d.\n",sectionLen,secTotalLen);
		return  PRIVATE_SECTION_OK;
	}

	//节目信息
	sectionData = pSectionData+12;
	if (sectionLen>0)
	{
		while(parseTotalLen < sectionLen)
		{
			descriptorLen = sectionData[1]+2;
			if (descriptorLen>sectionLen-parseTotalLen)
			{
				printf("[app_table_pmt_section_parse]descriptorLen=%d sectionLen-parseTotalLen=%d.\n",descriptorLen,sectionLen-parseTotalLen);
				return  PRIVATE_SECTION_OK;
			}
			switch(sectionData[0])
			{
				case CA_DESCRIPTOR:
#ifdef CA_FLAG
					program_type = TRUE;
					scramble_type = SCRAMBLE_TS;
					if (PSI_INVALID_PID == p_ecm_pid)
					{
						caSystemId = ( sectionData[2] << 8 ) + sectionData[3];
						ecmPid = ( (sectionData[4] & 0x1F) << 8 ) + sectionData[5];
						if (TRUE == app_cas_api_check_pmt_ca_descriptor_valid(sectionData,caSystemId))
						{
							if (PSI_INVALID_PID != ecmPid)
							{
								p_ecm_pid = ecmPid;
							}						
						}							
					}
#endif
					break;
#ifdef DVB_AD_TYPE_BY_FLAG
				case dbsm_ADTag:
					AdverPsiStop();
					app_by_pmt_Private_AD_Descriptor((char*)sectionData);
					break;
#endif
				default:
					break;
			}
			sectionData += descriptorLen;
			parseTotalLen += descriptorLen;
		}
	}

	sectionLen =secTotalLen - sectionLen - 13;
	while (sectionLen > 0)
	{
		esInfoLen = ((sectionData[3] &0xF)<<8)|(sectionData[4]);
		if (esInfoLen+5>sectionLen)
		{
			printf("[app_table_pmt_section_parse]esInfoLen+5=%d sectionLen=%d.\n",esInfoLen+5,sectionLen);
			return PRIVATE_SECTION_OK;
		}

		pid = ((sectionData[1]&0x1F)<<8)|(sectionData[2]);
		stream_type = sectionData[0];
		switch (sectionData[0])
		{
			case MPEG_1_VIDEO:
			case MPEG_2_VIDEO:
				video_type = GXBUS_PM_PROG_MPEG;
				p_video_pid = pid;
				break;
			case MPEG_4_VIDEO:
				video_type = GXBUS_PM_PROG_MPEG4;
				p_video_pid = pid;
				break;
			case H264:
				video_type = GXBUS_PM_PROG_H264;
				p_video_pid = pid;
				break;
			case H265:
				video_type = GXBUS_PM_PROG_H265;
				p_video_pid = pid;
				break;
			case AVS:
				video_type = GXBUS_PM_PROG_AVS;
				p_video_pid = pid;
				break;
			case MPEG_1_AUDIO:
				if ((PSI_INVALID_PID == p_audio_pid)||(GXBUS_PM_AUDIO_MPEG1 != audio_type))
					{
						audio_type = GXBUS_PM_AUDIO_MPEG1;
						p_audio_pid = pid;				
					}
				break;
			case MPEG_2_AUDIO:		
				if ((PSI_INVALID_PID == p_audio_pid)||(GXBUS_PM_AUDIO_MPEG2 != audio_type))
					{
						audio_type = GXBUS_PM_AUDIO_MPEG2;
						p_audio_pid = pid;				
					}
				break;
			case AAC_ADTS:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_AAC_ADTS;					
					p_audio_pid = pid;				
				}

				break;
			case AAC_LATM:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_AAC_LATM;
					p_audio_pid = pid;				
				}
				break;
			case LPCM:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_LPCM;
					p_audio_pid = pid;				
				}
				break;
			case AC3:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_AC3;
					p_audio_pid = pid;								
				}
				break;
			case DTS:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_DTS;
					p_audio_pid = pid;								
				}
				break;
			case DOLBY_TRUEHD:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_DOLBY_TRUEHD;
					p_audio_pid = pid;								
				}
				break;
			case AC3_PLUS:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_AC3_PLUS;
					p_audio_pid = pid;								
				}
				break;
			case DTS_HD:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_DTS_HD;
					p_audio_pid = pid;								
				}
				break;
			case DTS_MA:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_DTS_MA;
					p_audio_pid = pid;								
				}
				break;
			case AC3_PLUS_SEC:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_AC3_PLUS_SEC;
					p_audio_pid = pid;								
				}
				break;
			case DTS_HD_SEC:
				if (PSI_INVALID_PID == p_audio_pid)
				{
					audio_type = GXBUS_PM_AUDIO_DTS_HD_SEC;
					p_audio_pid = pid;								
				}
				break;
			case PRIVATE_PES_STREAM:
#if MEDIA_SUBTITLE_SUPPORT
				{
					descriptorLen = 0;
					parseTotalLen = 0;
					streamSceton = sectionData+5;

					while(parseTotalLen < esInfoLen)
					{
						descriptorLen = streamSceton[1]+2;
						switch(streamSceton[0])
						{
							case SUBT_DESCRIPTOR:
								{
									//dvb_subt
									indescriptorLen = streamSceton[1];
									subt_num = 0;
									streamSceton += 2;
									while(indescriptorLen>=8)
									{
										pSubtitling = (Subtiling*)GxCore_Realloc(pSubtitling, sizeof(Subtiling)*(subt_num+1));
										pSubtitling[subt_num].iso639[0] = streamSceton[0];
										pSubtitling[subt_num].iso639[1] = streamSceton[1];
										pSubtitling[subt_num].iso639[2] = streamSceton[2];
										pSubtitling[subt_num].type = streamSceton[3];
										pSubtitling[subt_num].composite_page_id = (streamSceton[4]<<8)|streamSceton[5];
										pSubtitling[subt_num].ancillary_page_id = (streamSceton[6]<<8)|streamSceton[7];
										subt_num ++;
										indescriptorLen -=8;
										streamSceton +=8;
										if(indescriptorLen<8)
										{
											break;
										}
									}
									if(subt_num == 0)
									{
										break;
									}
									pSubtDescriptor = (SubtDescriptor*)GxCore_Realloc(pSubtDescriptor, sizeof(SubtDescriptor)*(subt_count+1));
									pSubtDescriptor[subt_count].elem_pid = pid;
									pSubtDescriptor[subt_count].subt_num = subt_num;
									pSubtDescriptor[subt_count].subt = GxCore_Malloc(sizeof(Subtiling)*pSubtDescriptor[subt_count].subt_num);

									for(i = 0; i<pSubtDescriptor[subt_count].subt_num; i++)
									{
										memcpy(&(pSubtDescriptor[subt_count].subt[i]), &pSubtitling[i], sizeof(Subtiling));
									}
									GxCore_Free(pSubtitling);
									pSubtitling = NULL;
									subt_count++;
								}
								break;
							case TTX_DESCRIPTOR:
								{
									//ttx_subt
									indescriptorLen = streamSceton[1];
									ttx_num = 0;
									streamSceton += 2;
									while(indescriptorLen>=5)
									{
										if((streamSceton[3]>>3)!=2&&(streamSceton[3]>>3)!=5)
										{
											indescriptorLen -=5;
											streamSceton +=5;
											continue;
										}
										pTeleText = (Teletext*)GxCore_Realloc(pTeleText, sizeof(Teletext)*(ttx_num+1));
										pTeleText[ttx_num].iso639[0] = streamSceton[0];
										pTeleText[ttx_num].iso639[1] = streamSceton[1];
										pTeleText[ttx_num].iso639[2] = streamSceton[2];
										pTeleText[ttx_num].type = streamSceton[3]>>3;
										pTeleText[ttx_num].magazine = streamSceton[3]&0x7;
										pTeleText[ttx_num].page = streamSceton[4];
										ttx_num ++;
										indescriptorLen -=5;
										streamSceton +=5;
										if(indescriptorLen<5)
										{
											break;
										}
									}
									if(ttx_num == 0)
									{
										break;
									}
									pTtxDescriptor = (TeletextDescriptor*)GxCore_Realloc(pTtxDescriptor, sizeof(TeletextDescriptor)*(ttx_count+1));
									pTtxDescriptor[ttx_count].elem_pid = pid;
									pTtxDescriptor[ttx_count].ttx_num = ttx_num;
									pTtxDescriptor[ttx_count].ttx = GxCore_Malloc(sizeof(Teletext)*pTtxDescriptor[ttx_count].ttx_num);

									for(i = 0; i<pTtxDescriptor[ttx_count].ttx_num; i++)
									{
										memcpy(&(pTtxDescriptor[ttx_count].ttx[i]), &pTeleText[i], sizeof(Teletext));
									}
									GxCore_Free(pTeleText);
									pTeleText = NULL;
									ttx_count++;									
								}
								break;
						}	
						streamSceton += descriptorLen;
						parseTotalLen += descriptorLen;
					}
				}
#endif
				break;
#ifdef DVB_AD_TYPE_BY_FLAG
			case dbsm_ADTag:
				AdverPsiStop();
				app_by_pmt_Private_AD_Descriptor((char*)sectionData);
				break;
#endif

			default:
				break;
		}


		if (esInfoLen >0 )
		{
			descriptorLen = 0;
			parseTotalLen = 0;
			streamSceton = sectionData + 5;
			while (parseTotalLen < esInfoLen)
			{
				descriptorLen = streamSceton[1]+2;
				if (descriptorLen>esInfoLen-parseTotalLen)
				{
					printf("[app_table_pmt_section_parse]descriptorLen=%d esInfoLen-parseTotalLen=%d.\n",descriptorLen,esInfoLen-parseTotalLen);
					return PRIVATE_SECTION_OK;
				}
				switch(streamSceton[0])
				{
					case CA_DESCRIPTOR:
#ifdef CA_FLAG
						program_type = TRUE;
						scramble_type = SCRAMBLE_PES;
						if (pid == p_video_pid)
						{
							if (PSI_INVALID_PID == p_ecm_pid_video)
							{
								caSystemId = ( streamSceton[2] << 8 ) + streamSceton[3];
								ecmPid = ( (streamSceton[4] & 0x1F) << 8 ) + streamSceton[5];
								if (TRUE == app_cas_api_check_pmt_ca_descriptor_valid(sectionData,caSystemId))
								{
									if (PSI_INVALID_PID != ecmPid)
									{
										p_ecm_pid_video = ecmPid;									
									}									
								}
							}
						}
						else
							if (pid == p_audio_pid)
							{
								if (PSI_INVALID_PID == p_ecm_pid_audio)
								{
									caSystemId = ( streamSceton[2] << 8 ) + streamSceton[3];
									ecmPid = ( (streamSceton[4] & 0x1F) << 8 ) + streamSceton[5];
									if (TRUE == app_cas_api_check_pmt_ca_descriptor_valid(sectionData,caSystemId))
									{
										if (PSI_INVALID_PID != ecmPid)
										{
											p_ecm_pid_audio = ecmPid;									
										}									
									}
								}
							}					
#endif
						break;
					case AC3_DESCRIPTOR:
						{	// type PES_TYPE
							if (PSI_INVALID_PID == p_audio_pid)
							{
								audio_type = GXBUS_PM_AUDIO_AC3;
								p_audio_pid = pid;									
							}
					}
					break;
				case DRA_RESTRATION:
					if (PRIVATE_PES_STREAM == stream_type)
					{	// dra1
							{
								if (PSI_INVALID_PID == p_audio_pid)
									{
										if ((4 == streamSceton[1])&&('D' == streamSceton[2])&&('R' == streamSceton[3])
											&&('A' == streamSceton[4])&&('1' == streamSceton[5]))
											{
												audio_type = GXBUS_PM_AUDIO_DRA;
												p_audio_pid = pid;										
											}
										
									}							
							}
					}					
					break;
	     		case DRA_AUDIO:
					if (PRIVATE_PES_STREAM == stream_type)
					{	// dra1
							{
								if (PSI_INVALID_PID == p_audio_pid)
									{
										audio_type = GXBUS_PM_AUDIO_DRA;
										p_audio_pid = pid;									
									}						
							}

					}
					break;					
					default:
						break;
				}
				streamSceton += descriptorLen;
				parseTotalLen += descriptorLen;
			}
		}

		sectionData += (esInfoLen + 5);
		sectionLen -= (esInfoLen +	5);
	}
    
    /*解析后，更新数据并同步数据*/
	app_prog_mutex_lock();
	if (-1 != app_prog_get_playing_pos_in_group(&pos))
	{
		GxBus_PmProgGetByPos(pos,1,&Prog);
		if (serviceId == Prog.service_id)
		{
			if ((PSI_INVALID_PID == p_video_pid)&&( 0 == Prog.video_pid))
			{
				/*音频节目，应用保存视频pid 0x1fff,gxbus保存pid 0*/
				p_video_pid = Prog.video_pid;
				video_type = Prog.video_type;
			}

			if ((PSI_INVALID_PID == p_audio_pid)&&( 0 == Prog.cur_audio_pid))
			{
				/*无音频PID 节目，应用保存音频pid 0x1fff,gxbus保存pid 0*/
				p_audio_pid = Prog.cur_audio_pid;
				audio_type = Prog.cur_audio_type;
			}

			if ((PSI_INVALID_PID == p_pcr_pid)&&( 0 == Prog.pcr_pid))
			{
				/*无PCR-PID 节目，应用保存PCR pid 0x1fff,gxbus保存pid 0*/
				p_pcr_pid = Prog.pcr_pid;
			}
			
			if ((p_video_pid != Prog.video_pid)||(p_audio_pid !=Prog.cur_audio_pid )
					||(video_type != Prog.video_type)||(audio_type != Prog.cur_audio_type)
					||(p_pcr_pid != Prog.pcr_pid))
			{
				printf("[app_table_pmt_section_parse]p_video_pid=0x%x; p_audio_pid=0x%x@\n",
						p_video_pid, p_audio_pid);
				/*音视频PID发生改变*/
				Prog.video_type = video_type;
				Prog.video_pid = p_video_pid;
				Prog.cur_audio_type = audio_type;
				Prog.cur_audio_pid = p_audio_pid;
				Prog.pcr_pid =  p_pcr_pid;
				if ((0x1FFF != p_ecm_pid)||(0x1FFF != p_ecm_pid_video)||(0x1FFF != p_ecm_pid_audio))
				{
					Prog.scramble_flag = GXBUS_PM_PROG_BOOL_ENABLE;
				}
				GxBus_PmProgInfoModify(&Prog);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
				app_prog_mutex_unlock();
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
				app_table_pmt_filter_open(pmt_pid,serviceId,version,FALSE);
#endif
				//				app_table_pmt_filter_close();
				app_play_stop_audio(); 
				app_play_video_audio(pos);
				return  PRIVATE_SUBTABLE_OK ;

			}
		}
		else
			{
				/*
				* pmt表分析的service id与当前播放节目的service id不一致，
				* 过滤到的pmt表并非当前节目
				*/
				app_prog_mutex_unlock();
				if (serviceId == pmt_serviceid)
					{
						app_table_pmt_filter_open(pmt_pid,serviceId,version,FALSE);				
					}
				return  PRIVATE_SUBTABLE_OK ;
			}
	}
	app_prog_mutex_unlock();

#if MEDIA_SUBTITLE_SUPPORT
	if(subt_count!=0||ttx_count!=0)
	{
		//memset(&pmt_info, 0, sizeof(PmtInfo));
		pmt_info.subt_count = subt_count;
		pmt_info.subt_info = GxCore_Malloc(sizeof(uint32_t)*subt_count);
		for(i = 0; i<subt_count; i++)
		{
			pmt_info.subt_info[i] = (uint32_t)&pSubtDescriptor[i];
		}

		pmt_info.ttx_count = ttx_count;
		pmt_info.ttx_info = GxCore_Malloc(sizeof(uint32_t)*ttx_count);
		for(i = 0; i<ttx_count; i++)
		{
			pmt_info.ttx_info[i] = (uint32_t)&pTtxDescriptor[i];
		}

		g_AppTtxSubt.sync(&g_AppTtxSubt,&pmt_info);
		s_subt_sync_flag = 1;
	}
#endif

	if (0 == p_video_pid)
	{
		/*音频节目，应用视频pid恢复成0x1fff*/
		p_video_pid = PSI_INVALID_PID;
	}
#if MEDIA_SUBTITLE_SUPPORT
	else
	{
		app_subt_change();
	}
#endif

	if (0 == p_audio_pid)
	{
		/*无音频节目，应用音频pid恢复成0x1fff*/
		p_audio_pid = PSI_INVALID_PID;
	}

	if (0 == p_pcr_pid)
	{
		/*无pcr-pid 节目，应用pcr-pid恢复成0x1fff*/
		p_pcr_pid = PSI_INVALID_PID;
	}	
	//	app_table_pmt_filter_close();
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
	app_table_pmt_filter_open(pmt_pid,serviceId,version,FALSE);
#endif

#ifdef CA_FLAG
	{
		// 快速切台，只需要设置最后播放节目的ECMPID即可
		app_play_get_playing_para(&playPara);
		memcpy(&oldplayPara,&playPara,sizeof(play_scramble_para_t));
		playPara.program_type = program_type;
		playPara.program_num = program_num;
		playPara.scramble_type = scramble_type;
		playPara.p_video_pid = p_video_pid;
		playPara.p_audio_pid = p_audio_pid;
		playPara.p_ecm_pid = p_ecm_pid;
		playPara.p_ecm_pid_video = p_ecm_pid_video;
		playPara.p_ecm_pid_audio = p_ecm_pid_audio;

		if ((PSI_INVALID_PID != playPara.p_ecm_pid_video)&&(PSI_INVALID_PID != playPara.p_ecm_pid_audio)
				&&(playPara.p_ecm_pid_video == playPara.p_ecm_pid_audio))
		{
			playPara.p_ecm_pid = playPara.p_ecm_pid_video;
			playPara.p_ecm_pid_video = PSI_INVALID_PID;
			playPara.p_ecm_pid_audio = PSI_INVALID_PID;
			playPara.scramble_type = SCRAMBLE_TS;
		}

		if (0 == memcmp(&oldplayPara, &playPara, sizeof(play_scramble_para_t)))
		{
			printf("[app_table_pmt_section_parse]ecm filter start already..\n");
#ifdef DVB_CA_TYPE_DVB_FLAG
#ifdef DVB_CA_FREE_STOP
#include "dvbca_interface.h"
			if ((0x1FFF ==playPara.p_ecm_pid )&&(0x1FFF == playPara.p_ecm_pid_audio)&&(0x1FFF == playPara.p_ecm_pid_video))
			{	
				if ( 0 == app_dvb_cas_get_entitle_count(1) )
				{
					app_play_stop();
					DVBSTBCA_ShowMessage(0x1FFF,DVBCA_MESSAGE_NOENTITLE  );
				}
			}
#endif
#endif

		}
		else
		{
		//	app_play_clear_ca_msg();
			app_cas_api_release_ecm_filter();
			app_cas_api_start_ecm_filter(&playPara);				
		}

#ifdef SHOW_ECM_GOT_TIME
		extern GxTime starttime;
		GxTime nowtime={0};
		GxCore_GetTickTime(&nowtime);
		printf("\n Re start filter ecm >>>>>[app_table_pmt_section_parse]->%d MS<<<<<\n\n",((nowtime.seconds*1000+nowtime.microsecs/1000) - (starttime.seconds*1000+starttime.microsecs/1000)));
		starttime.seconds = nowtime.seconds;
		starttime.microsecs = nowtime.microsecs;
#endif
		app_cas_api_release_ecm_filter();
		app_cas_api_start_ecm_filter(&playPara);//开启ecm过滤器

	}
#endif

	if(nOldTpId != Prog.tp_id)
	{
		/*tp id不同，频点变化复位cat \ nit filter*/
		nOldTpId = Prog.tp_id;
		printf("nOldTpId=%d prog_data.tp_id=%d\n",
				nOldTpId,Prog.tp_id);
#ifdef DVB_AD_TYPE_MAIKE_FLAG
		start_monitor_mad();
#endif
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
		app_table_cat_filter_restart();
		app_table_ota_monitor_filter_restart();
#ifdef DVB_SERIAL_SUPPORT
		app_table_ts_monitor_filter_restart();
#endif
#endif
		app_table_nit_monitor_filter_restart();

	}
	else
	{
		/*tp id 相同，相同频点判断cat filter , nit filter是否已开启*/
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG

		if ((0 == app_table_cat_get_filter_handle())||(0 == app_table_cat_get_channel_handle()))
		{
			app_table_cat_filter_restart();				
		}
#endif

		if ((0 == app_table_nit_get_filter_handle())||(0 == app_table_nit_get_channel_handle()))
		{
			app_table_nit_monitor_filter_restart();				
		}
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG

		if ((0 == app_table_ota_get_filter_handle())||(0 == app_table_ota_get_channel_handle()))
		{
			app_table_ota_monitor_filter_restart();				
		}	
#endif						
#ifdef DVB_SERIAL_SUPPORT 
		if ((0 == app_table_ts_get_filter_handle())||(0 == app_table_ts_get_channel_handle()))
		{
			app_table_ts_monitor_filter_restart();				
		}			
#endif
	}

	return PRIVATE_SUBTABLE_OK;

}



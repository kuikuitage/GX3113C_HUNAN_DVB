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
#include "app_common_play.h"
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_demux.h"
#include "app_common_porting_ca_smart.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_ca_descramble.h"
#include "gx_demux.h"
#include "gx_descrambler.h"
#include "dvbca_interface.h"

void app_dvb_cas_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
	int16_t             pid;
	uint16_t            section_length;
	bool		    bFlag = FALSE;
	uint8_t*            data = (uint8_t*)Section;
	int                 len = Size;
	int32_t		    ret;

	//    ASSERT(Section != NULL);
	ret = GxDemux_FilterGetPID(Filter,&pid);

	while(len > 0)
	{
		bFlag = FALSE;
		section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
		if ((data[0]&0xF0)==0x80)
		{
			//CAS_Dbg("pid= 0x%x section_length = %d\n",pid,section_length);
			//CAS_DUMP("[data]",data,10);
			DVBCASTB_PrivateDataGot(pid,data,section_length);
		}
		data += section_length;
		len -= section_length;
	}
	return;
}


void app_dvb_cas_filter_timeout_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
	uint8_t filterId;
	uint16_t	pid;
	ca_filter_t filter = {0};

	for (filterId= 0; filterId< MAX_FILTER_COUNT; filterId++)
	{
		app_porting_ca_demux_get_by_filter_id(filterId,&filter);
		if (( 0 != filter.usedStatus)&&(0 != filter.handle)
				&&(0 != filter.channelhandle))
		{

			if (Filter == filter.handle)
			{
				pid = filter.pid;

				CAS_Dbg("pid= 0x%x timeout\n",pid);
				/* 私有数据接收回调 */
				//BYCASTB_StreamGuardDataGot( filter.byReqID,FALSE,pid,NULL,0);
			}
		}
	}


	return;	
}


uint8_t	app_dvb_cas_api_release_ecm_filter(void)
{
	int32_t filterId =0;
	ca_filter_t filter = {0};
	play_scramble_para_t playpara = {0};
	app_play_get_playing_para(&playpara);

	for (filterId = 0; filterId<MAX_FILTER_COUNT;filterId++)
	{
		app_porting_ca_demux_get_by_filter_id(filterId,&filter);
		if (( 0 != filter.usedStatus)&&(0 != filter.handle)&&
				(0!=filter.channelhandle))
		{
			if ((filter.pid == playpara.p_ecm_pid)||(filter.pid == playpara.p_ecm_pid_video)
					||(filter.pid == playpara.p_ecm_pid_audio))
				app_porting_ca_demux_release_filter( filterId, TRUE);
		}

	}

	/*
	 *如果当前频道为不加扰的频道，则可以停止ECM数据
	 */

	playpara.p_ecm_pid= PSI_INVALID_PID;
	playpara.p_ecm_pid_video= PSI_INVALID_PID;
	playpara.p_ecm_pid_audio= PSI_INVALID_PID;
	app_play_set_playing_para(&playpara);

	return 1;
}

static play_scramble_para_t dvb_play_para = {0};
uint8_t app_dvb_cas_api_start_ecm_filter(play_scramble_para_t *pplayPara)
{
	CAS_Dbg("app_dvb_cas_api_start_ecm_filter \n");
	if (NULL == pplayPara)
	{
		return 0;
	}

	memcpy(&dvb_play_para,pplayPara,sizeof(play_scramble_para_t));
	if ((0x1FFF ==dvb_play_para.p_ecm_pid )&&(0x1FFF == dvb_play_para.p_ecm_pid_audio)&&(0x1FFF == dvb_play_para.p_ecm_pid_video))
	{
		/*清流节目*/
#ifdef DVB_CA_FREE_STOP
		if ( 0 == app_dvb_cas_get_entitle_count(0) )
		{
			app_play_stop();
			DVBSTBCA_ShowMessage(0x1FFF,DVBCA_MESSAGE_NOENTITLE  );
		}
		return 0;
#endif
	}

	if (0x1FFF != dvb_play_para.p_ecm_pid)
	{
		CAS_Dbg("app_dvb_cas_api_start_ecm_filter old_p_audio_pid=0x%x old_p_ecm_pid =0x%x\n",pplayPara->p_audio_pid,pplayPara->p_ecm_pid);
		CAS_Dbg("app_dvb_cas_api_start_ecm_filter old_p_video_pid=0x%x old_p_ecm_pid =0x%x\n",pplayPara->p_video_pid,pplayPara->p_ecm_pid);

		if (0 == app_porting_ca_descramble_set_pid(1,pplayPara->p_audio_pid ))
		{
			CA_FAIL(" Failed\n");
			return 0;
		}

		if (0 == app_porting_ca_descramble_set_pid(0,pplayPara->p_video_pid ))
		{
			CA_FAIL(" Failed\n");
			return 0;
		}

		DVBCASTB_SetEcmPid((DVBCA_UINT16 *)&pplayPara->p_ecm_pid,1);
	}

	if (0x1FFF != dvb_play_para.p_ecm_pid_audio)
	{
		CAS_Dbg("app_dvb_cas_api_start_ecm_filter old_p_video_pid=0x%x old_p_ecm_pid_video =0x%x\n",pplayPara->p_video_pid,pplayPara->p_ecm_pid_video);

		if (0 == app_porting_ca_descramble_set_pid(1,pplayPara->p_audio_pid ))
		{
			CA_FAIL(" Failed\n");
			return 0;
		}
		DVBCASTB_SetEcmPid((DVBCA_UINT16 *)&pplayPara->p_ecm_pid,1);
	}


	if (0x1FFF != dvb_play_para.p_ecm_pid_video)
	{
		CAS_Dbg("app_dvb_cas_api_start_ecm_filter old_p_video_pid=0x%x old_p_ecm_pid_video =0x%x\n",pplayPara->p_video_pid,pplayPara->p_ecm_pid_video);

		if (0 == app_porting_ca_descramble_set_pid(0,pplayPara->p_video_pid ))
		{
			CA_FAIL(" Failed\n");
			return 0;
		}
		DVBCASTB_SetEcmPid((DVBCA_UINT16 *)&pplayPara->p_ecm_pid,1);
	}

	CAS_Dbg("app_dvb_cas_api_start_ecm_filter End\n");
	return 1;
}

uint8_t app_dvb_cas_api_reset_ecm_filter(void)
{
	CAS_Dbg("app_dvb_cas_api_reset_ecm_filter.\n");
	app_cas_api_release_ecm_filter();
	app_cas_api_start_ecm_filter(&dvb_play_para);//开启ecm过滤器
	return 0;
}


uint8_t app_dvb_cas_api_release_emm_filter(void)
{
	int32_t filterId =0;
	ca_filter_t filter = {0};
	play_scramble_para_t playpara = {0};
	app_play_get_playing_para(&playpara);

	for (filterId = 0; filterId<MAX_FILTER_COUNT;filterId++)
	{
		app_porting_ca_demux_get_by_filter_id(filterId,&filter);
		if (( 0 != filter.usedStatus)&&(0 != filter.handle)&&
				(0!=filter.channelhandle))
		{
			if (filter.pid == playpara.p_emm_pid)
				app_porting_ca_demux_release_filter( filterId, TRUE);
		}
	}

	DVBCASTB_SetEmmPid(0);
	playpara.p_emm_pid = 0x1fff;
	app_play_set_playing_para(&playpara);

	return TRUE;
}

uint8_t app_dvb_cas_api_start_emm_filter(uint16_t emm_pid)
{
	CAS_Dbg("%s emm_pid=%d\n",__FUNCTION__,emm_pid);
	DVBCASTB_SetEmmPid(emm_pid);
	return TRUE;
}

uint8_t app_dvb_cas_api_reset_emm_filter(void)
{
	play_scramble_para_t playpara = {0};
	app_play_get_playing_para(&playpara);

	CAS_Dbg("app_dvb_cas_api_reset_emm_filter \n");

	if (0x1fff == playpara.p_emm_pid)
	{
		app_dvb_cas_api_start_emm_filter(playpara.p_emm_pid);
		CAS_Dbg("app_dvb_cas_api_reset_emm_filter ok.\n");
	}
	return 0;
}


bool app_dvb_cas_api_check_cat_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id)
{
	CAS_Dbg("%s ca_system id=%d\n",__FUNCTION__,CA_system_id);
	if( DVBCA_SUCESS== DVBCASTB_IsDVBCA(CA_system_id) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	

}

bool app_dvb_cas_api_check_pmt_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id)
{
	CAS_Dbg("%s ca_system id=%d\n",__FUNCTION__,CA_system_id);
	if( DVBCA_SUCESS== DVBCASTB_IsDVBCA(CA_system_id) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void app_dvb_cas_api_nit_zone_des(uint8_t* databuf)
{
	return ;
}










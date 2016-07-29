/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	cmis_ads_api.c
* Author    : 	wufei
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2014.8.27		  wufei 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ads_flash.h"
#include "app_common_prog.h"
#include "cmis_ads_porting.h"
#include "cmis_ads_api.h" 
#include "cmis_ads_camail.h" 

#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#endif
handle_t g_cmis_sem = 0;
extern uint8_t gShowAudioFalg;
extern uint8_t mute_flag;
extern uint8_t cmis_task_running;
extern Cmis_mail_info_t g_cmis_ca_mail_info ;
extern int8_t url[GX_PM_MAX_PROG_URL_SIZE];
GxBusPmDataProg g_cmis_homepage_prog = {0};
GxBusPmDataProg g_cmis_recharge_prog = {0};

/*
* 设置音量
*/
static void app_cmis_av_set_volumn(int32_t Volume)
{
	/*
	* if Volume is 0, mute; else unmute
	*/
	 int32_t volume_pre= 0xff;
	int32_t Config;
	int32_t volumn_globle_flag = 0;

	Config =	app_flash_get_config_mute_flag();					

	if(Config)
	{
		app_play_set_mute(Config);
		return;
	}
	else
	{
		app_send_msg(GXMSG_PLAYER_AUDIO_VOLUME, &Volume);
		return;

	}
	volumn_globle_flag = app_flash_get_config_volumn_globle_flag();

	if (0 == Volume)
	{
		if (1 == volumn_globle_flag)
		{
			Config = app_flash_get_config_mute_flag();
			if(Config == 0)
			{
				Config = 1;
				app_flash_save_config_mute_flag(Config);
			}				
		}
		
//			if (0 == mute_flag )
		{
			Config = 1;
			app_play_set_mute(Config);
		}

		volume_pre = Volume;
	}
	else
	{
		if (1 == mute_flag )
		{
			Config = 0;
			if (1 == volumn_globle_flag)
			{
				app_flash_save_config_mute_flag(Config);					
			}
			app_play_set_mute(Config);
			app_send_msg(GXMSG_PLAYER_AUDIO_VOLUME, &Volume);
			volume_pre = Volume;
		}
		else if (volume_pre != Volume)
		{
				app_send_msg(GXMSG_PLAYER_AUDIO_VOLUME, &Volume);
				volume_pre = Volume;						
		}
	}

	return;
}

static app_cmis_av_play(GxBusPmDataProg prog_data)
{
	//GxBusPmViewInfo sys_info;
	int32_t volume = 0;
	GxBusPmDataTP tp = {0};
	uint8_t chSyncModeBuffer[20]={0};
	app_play_stop();
	GxBus_PmTpGetById(prog_data.tp_id, &tp);

#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)				
	app_search_lock_tp(tp.frequency/1000, tp.tp_c.symbol_rate/1000, INVERSION_OFF, tp.tp_c.modulation-1 ,0);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	app_search_lock_tp(tp.frequency/1000, tp.tp_dtmb.symbol_rate/1000, INVERSION_OFF, tp.tp_dtmb.modulation-1 ,0);	
#endif

	/*
	 *切台前，清除ECM PID
	 */
#ifdef CA_FLAG
	app_cas_api_release_ecm_filter();
#endif
	app_table_pmt_filter_close();

	memset(url, 0, GX_PM_MAX_PROG_URL_SIZE);
	GxBus_PmProgUrlGet(&prog_data, url, GX_PM_MAX_PROG_URL_SIZE);
	sprintf((void*)chSyncModeBuffer, "&sync:%d", SYSC_MODE);
	strcat((void*)url, (void*)chSyncModeBuffer);
	switch(DVB_TS_SRC)
	{
		case 0:
			strcat((void*)url, "&tsid:0&dmxid:0");
			break;
		case 1:
			strcat((void*)url, "&tsid:1&dmxid:0");
			break;
		case 2:
			strcat((void*)url, "&tsid:2&dmxid:0");
			break;
		default:
			strcat((void*)url, "&tsid:0&dmxid:0");
			break;
	}
	printf("url=%s\n",url);

	GxPlayer_MediaPlay(PLAYER0,(const char*)url,0,0,NULL);

	app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);
#ifndef CA_FLAG
	if(TRUE == prog_data.scramble_flag)
	{
		app_play_set_msg_pop_type(MSG_POP_PROG_SCRAMBLE);			
	}
	else
	{
		app_play_clear_msg_pop_type_flag(MSG_POP_PROG_SCRAMBLE);
	}
#endif
#ifdef CA_FLAG
#if (RC_VERSION>GOXCEED_v1_9_2)
{
	play_scramble_para_t playPara={0};
	app_play_get_playing_para(&playPara);	
	if(prog_data.scramble_flag == GXBUS_PM_PROG_BOOL_ENABLE)
		playPara.program_type = TRUE;
	else
		playPara.program_type = FALSE;
	
	playPara.program_num = prog_data.service_id;
	playPara.p_video_pid = prog_data.video_pid;
	playPara.p_audio_pid = prog_data.cur_audio_pid;
	if ((prog_data.cur_audio_ecm_pid >0)&&(prog_data.cur_audio_ecm_pid <0x1FFF))
	{
		if (prog_data.cur_audio_ecm_pid == prog_data.ecm_pid_v)
		{	// 音视频ECMPID有效完全一样
			playPara.p_ecm_pid = prog_data.ecm_pid_v;;	
			playPara.p_ecm_pid_video = 0x1FFF;
			playPara.p_ecm_pid_audio = 0x1FFF;	
			playPara.scramble_type = SCRAMBLE_TS;
		}
		else
		{	// 音视频ECMPID不一样
			if (0 ==prog_data.ecm_pid_v )
				playPara.p_ecm_pid_video = 0x1FFF;
			else
				playPara.p_ecm_pid_video = prog_data.ecm_pid_v; 
			
				playPara.p_ecm_pid_audio = prog_data.cur_audio_ecm_pid; 
				playPara.p_ecm_pid = 0x1FFF;
				playPara.scramble_type = SCRAMBLE_PES;
		}

	}
	else if ((prog_data.ecm_pid_v >0)&&(prog_data.ecm_pid_v <0x1FFF))
	{
		if (prog_data.ecm_pid_v == prog_data.cur_audio_ecm_pid)
		{ // 音视频ECMPID完全一样
			playPara.p_ecm_pid = prog_data.ecm_pid_v;;	
			playPara.p_ecm_pid_video = 0x1FFF;
			playPara.p_ecm_pid_audio = 0x1FFF;
			playPara.scramble_type = SCRAMBLE_TS;											
		}
		else
		{ // 音视频ECMPID不一样
			if (0 ==prog_data.cur_audio_ecm_pid )
				playPara.p_ecm_pid_audio = 0x1FFF;
			else
				playPara.p_ecm_pid_audio = prog_data.cur_audio_ecm_pid; 				

			playPara.p_ecm_pid_video = prog_data.ecm_pid_v; 
			playPara.p_ecm_pid = 0x1FFF;
			playPara.scramble_type = SCRAMBLE_PES;											
		}

	}
	app_play_clear_ca_msg();
	app_cas_api_start_ecm_filter(&playPara);
}
#endif
#endif
	app_table_pmt_filter_open(prog_data.pmt_pid,prog_data.service_id,0,TRUE);
	if(prog_data.service_type == GXBUS_PM_PROG_TV)
	{
		gShowAudioFalg = FALSE;
	}
	volume = app_flash_get_config_audio_volume();
	app_cmis_av_set_volumn(volume);
}
void APP_CMIS_AD_Init(void)
{
	GxCore_SemCreate(&g_cmis_sem,1);
	cmis_ad_flash_init();
	cmis_ad_prog_list_init();
	cmis_ad_flash_load_ad_info();
	cmis_ad_flash_load_cmis_channel_info();
	cmis_ad_camail_init();
	cmis_ad_demux_init();
	cmis_ad_demux_sdt_filter_open(CMIS_SDT_PID);
}
/*Must be called after program search.*/
void APP_CMIS_AD_Open(void)
{
	GxCore_SemWait(g_cmis_sem);
	cmis_task_running= 1;
	cmis_ad_demux_private_filter_close();
	cmis_ad_demux_sdt_filter_open(CMIS_SDT_PID);
	GxCore_SemPost(g_cmis_sem);
}
/*Must be called before program search.*/
void APP_CMIS_AD_Close(void)
{
	GxCore_SemWait(g_cmis_sem);
	cmis_task_running = 0;
	cmis_ad_prog_list_init();
	cmis_ad_demux_sdt_filter_close();
	cmis_ad_demux_private_filter_close();
	//cmis_ad_flash_clean();
	GxCore_SemPost(g_cmis_sem);
}

Cmis_err_t APP_CMIS_AD_GetTvNo(char * TVNOStr)
{
	ca_get_date_t get_data = {0};
	Cmis_err_t ret = CMIS_ERROR;
	char * temStr = NULL;
	char * temStrHigh = NULL;
	char * temStrLow = NULL;
	uint32_t High = 0;
	uint32_t Middle = 0;
	uint32_t Low = 0;
	int8_t i = 0;
	int32_t max_operator = 0;

	if(!TVNOStr)
	{
		return ret;
	}
	
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
	app_cd_cas30_api_init_features_data();

	max_operator = CDCA_MAXNUM_OPERATOR;
	
	for(i = 0;i < CDCA_MAXNUM_OPERATOR;i++)
	{
		get_data.pos = i;
		get_data.date_type = DVB_CA_FETURE_INFO;
		get_data.ID = 0;
		temStr = app_cas_api_get_data(&get_data);
		if(temStr == NULL)
		{
			continue;
		}
		if(atoi(temStr) == 2802)
		{
			get_data.ID = 3;/*CD_FEATURES_ID1_ID*/
			temStr = app_cas_api_get_data(&get_data);
			if(temStr)
			{
				//temStr = "0x00000118 0x0139ab44 0x00000000";
				ADS_Dbg("feature id(%s) \n",temStr); 
				sscanf(temStr,"0x%08x 0x%08x 0x%08x",&High,&Middle,&Low);
				sprintf(TVNOStr,"%010d",Middle);
				ADS_Dbg("get TVNO(%s) in card \n",TVNOStr); 
				ret = CMIS_SUCCESS;
				break;
			}
			
		}
	}
#endif
	if(ret == CMIS_ERROR)
	{
		sprintf(TVNOStr,"%s %s","00000","00000");
	}
	return ret;
}
void APP_CMIS_AD_ProgramSave()
{
	GxBusPmDataProg prog_data;
	int32_t i = 0;
	int16_t prog_count = GxBus_PmProgNumGet();
	Cmis_err_t ret = CMIS_ERROR;
	Cmis_prog_t  cmis_prog = {0};
	
//	if(prog_data == NULL)
	{
//		return;
	}
	for(i = 0;i < prog_count;i++)
	{
		ret = GxBus_PmProgGetByPos(i, 1, &prog_data);
		if(ret <= 0)
		{
			continue;
		}
		ret = cmis_ad_prog_list_get_prog_by_service_id(prog_data.service_id,&cmis_prog);
		if(ret == CMIS_ERROR)
		{
			continue;
		}

		if(cmis_prog.prog_type == CMIS_PROG_TYPE_HOME_PROG)
		{
			memcpy(&g_cmis_homepage_prog,&prog_data,sizeof(GxBusPmDataProg));
			cmis_ad_flash_write_cmis_channel_info(cmis_prog.prog_type ,&prog_data);
		}
		else if(cmis_prog.prog_type == CMIS_PROG_TYPE_RECHARGE_BACKGROUND)
		{
			memcpy(&g_cmis_recharge_prog,&prog_data,sizeof(GxBusPmDataProg));
			cmis_ad_flash_write_cmis_channel_info(cmis_prog.prog_type ,&prog_data);
		}
	}
}
Cmis_err_t APP_CMIS_AD_ShowADData
	(uint16_t service_id, /*Used for CURTAIN,EPG ad show*/
	Cmis_data_type_t data_type,
	uint8_t *mailStr, /*Used for CA mail show*/
	ad_show_callback_fun call_back)
{
	Cmis_err_t ret = CMIS_ERROR;
	Cmis_prog_t prog_data = {0};
	bool bRet = FALSE;
	void * pData = NULL;
	
	if(call_back == NULL)
	{
		return ret;
	}
	
	GxCore_SemWait(g_cmis_sem);
	do{
		if(data_type == CMIS_DATA_TYPE_MAIL)
		{
			if(mailStr == NULL)
			{
				break;
			}
			bRet = cmis_ad_camail_parse_content(mailStr);
			ADS_Dbg("%s %d %d\n",__FUNCTION__,__LINE__,bRet);
			if(bRet == FALSE)
			{
				break;
			}
			pData = &g_cmis_ca_mail_info;
			ret = CMIS_SUCCESS;
			break;
		}
		/**/
		cmis_ad_force_search_ad_data(service_id,data_type);
		
		bRet = cmis_ad_prog_list_check_ad_data(service_id, data_type);
			ADS_Dbg("%s %d %d\n",__FUNCTION__,__LINE__,bRet);
		if(bRet == FALSE)
		{
			ADS_Dbg("%s %d sid[%d]\n",__FUNCTION__,__LINE__,service_id);
			break;
		}
	 	ret = cmis_ad_prog_list_get_prog_by_service_id(service_id,&prog_data);
			ADS_Dbg("%s %d %d\n",__FUNCTION__,__LINE__,bRet);
		if(ret == CMIS_ERROR)
		{
			break;
		}
		if(data_type == CMIS_DATA_TYPE_CURTAIN)
		{
			pData = (void *)prog_data.curtain_data;
		}
		else if(data_type == CMIS_DATA_TYPE_EPG)
		{
			pData = (void *)prog_data.epg_data;
		}
		else
		{
			break;
		}
		ret = CMIS_SUCCESS;

	}while(0);
	if(ret == CMIS_SUCCESS)
	{
		ADS_Dbg("%s %d %d\n",__FUNCTION__,__LINE__,bRet);
		call_back(pData);
		ADS_Dbg("%s %d \n",__FUNCTION__,__LINE__);
	}
	GxCore_SemPost(g_cmis_sem);
	return ret;
}

Cmis_err_t APP_CMIS_AD_PlayHomeService(void)
{
	int32_t i = 0;
	Cmis_prog_t  prog_data = {0};
	Cmis_err_t ret = CMIS_ERROR;
	int32_t iRet = 0;
	GxBusPmDataProg prog_arry1;
	uint16_t prog_count = GxBus_PmProgNumGet();
	
	GxCore_SemWait(g_cmis_sem);

	 #if 0
	 ret = cmis_ad_prog_list_get_prog_by_type
	 	(CMIS_PROG_TYPE_HOME_PROG,&prog_data);
	 if(ret == CMIS_ERROR)
	 {
		GxCore_SemPost(g_cmis_sem);
	 	return ret;
	 }
	 for(i = 0;i < prog_count;i++)
	 {
		iRet = GxBus_PmProgGetByPos(i,1,&prog_arry1);
		if(iRet <= 0)
		{
			continue;
		}

		if(prog_arry1.service_id == prog_data.service_id)
		{
			app_prog_save_playing_pos_in_group(i);
			app_play_switch_prog_clear_msg();	
			app_play_reset_play_timer(0);	
			ret = CMIS_SUCCESS;
			break;
		}

	 }
	 #else
	 app_cmis_av_play(g_cmis_homepage_prog);
	 #endif
	GxCore_SemPost(g_cmis_sem);
	return ret;
}
Cmis_err_t APP_CMIS_AD_PlayRechargeService(void)
{
	int32_t i = 0;
	Cmis_prog_t  prog_data = {0};
	Cmis_err_t ret = CMIS_ERROR;
	int32_t iRet = 0;
	GxBusPmDataProg prog_arry1={0};
	uint16_t prog_count = GxBus_PmProgNumGet();
	GxCore_SemWait(g_cmis_sem);

	/* ret = cmis_ad_prog_list_get_prog_by_type
	 	(CMIS_PROG_TYPE_RECHARGE_BACKGROUND,&prog_data);
	 if(ret == CMIS_ERROR)
	 {
		GxCore_SemPost(g_cmis_sem);
	 	return ret;
	 }
	 */
	 for(i = 0;i < prog_count;i++)
	 {
		iRet = GxBus_PmProgGetByPos(i,1,&prog_arry1);
		if(iRet <= 0)
		{
			continue;
		}

		if(prog_arry1.service_id == g_cmis_recharge_prog.service_id)
		{
			app_prog_save_playing_pos_in_group(i);
			app_play_switch_prog_clear_msg();	
			app_play_reset_play_timer(0);			
			ret = CMIS_SUCCESS;
			break;
		}

	 }
	 if(ret != CMIS_SUCCESS)
	 {
	 	app_cmis_av_play(g_cmis_recharge_prog);
	 }
	 GxCore_SemPost(g_cmis_sem);

	return ret;
}

void APP_CMIS_AD_TVNoShow(char * widget_name)
{
	char temStr[20] = {0};

	APP_CMIS_AD_GetTvNo(temStr);

	GUI_SetProperty(widget_name,"string",temStr);
	GUI_SetProperty(widget_name,"state","show");

}
void APP_CMIS_AD_TVNoHide(char * widget_name)
{
	GUI_SetProperty(widget_name,"string"," ");
	GUI_SetProperty(widget_name,"state","hide");
}
Cmis_prog_type_t APP_CMIS_AD_CheckCurProgramType(void)
{
	Cmis_err_t ret = CMIS_ERROR;
	uint32_t pos;
	GxBusPmDataProg prog_data={0};
	Cmis_prog_t cmis_prog = {0};
	app_prog_get_playing_pos_in_group(&pos);
	GxBus_PmProgGetByPos(pos,1,&prog_data);
	
	 ret = cmis_ad_prog_list_get_prog_by_service_id(prog_data.service_id, &cmis_prog);
	 if(ret == CMIS_ERROR)
	 {
	 	return CMIS_PROG_TYPE_UNDEFINE;
	 }
	 return cmis_prog.prog_type;
}
uint8_t APP_CMIS_AD_CheckMailIsCmis(char * mail_str)
{
	if(mail_str == NULL)
	{
		return FALSE;
	}
	do
	{
		if(mail_str[0] != '#')
		{
			break;
		}
		if(mail_str[1] != '#')
		{
			break;
		}
		if(mail_str[2] != '#')
		{
			break;
		}
		if(mail_str[3] != '#')
		{
			break;
		}
		return TRUE;
	}while(0);
	return FALSE;
}
/**/

/*
*  此文件接口实现CA移植操作系统公共接口
*/
#include "gx_descrambler.h"
#include "app_common_porting_ca_descramble.h"
#include "app_common_porting_stb_api.h"
#include "app_common_play.h"
#include "gxapp_sys_config.h"




static ca_descramble_t g_descramlbeList[MAX_SCRAMBLE_CHANNEL];
uint8_t app_porting_ca_descramble_init(void)
{
	static uint8_t init_flag = FALSE;
	
	if (TRUE == init_flag)
		return TRUE;
	
		/*解扰器初始化打开一次，0设置为视频，1设置为音频*/
	memset(&g_descramlbeList[0],0,MAX_SCRAMBLE_CHANNEL*sizeof(ca_descramble_t));
	if ( 0 == g_descramlbeList[0].handle)
	{	// video
		g_descramlbeList[0].handle = GxDescrmb_Open(0);
		if (0 == g_descramlbeList[0].handle)
		{
			CA_ERROR("irdRequestDescramblerChannel Call  GXDEMUX_DescramblerAllocate 0 == g_descramlbeList[0].handle\n");
			return FALSE;
		}
	}

	if (0 == g_descramlbeList[1].handle)
	{	// audio
		g_descramlbeList[1].handle = GxDescrmb_Open(0);
		if (0 == g_descramlbeList[1].handle)
		{
			CA_ERROR("irdRequestDescramblerChannel Call  GXDEMUX_DescramblerAllocate 0 == g_descramlbeList[1].handle\n");
			return FALSE;
		}
	}

	init_flag = TRUE;
	return TRUE;

}


uint8_t	app_porting_ca_descramble_set_pid(uint8_t index,uint16_t pid )
{
	if (index >1)
		{
			CA_FAIL(" para error index=%d\n",index);
			return 0;
		}

	if (0 == g_descramlbeList[index].handle)
	{
		CA_FAIL("g_descramlbeList[%d].handle=0 \n",index);
		return 0;
	}
	
	GxDescrmb_SetStreamPID(g_descramlbeList[index].handle, pid);


	return 1;
	
}


uint8_t	app_porting_ca_descramble_set_cw(uint16_t       wEcmPID,
							  const uint8_t* pbyOddKey,
							  const uint8_t* pbyEvenKey,
							  uint8_t        byKeyLen,
							  bool      bTapingEnabled )
{
	uint8_t* pOddKey = (uint8_t*)pbyOddKey;
	uint8_t*pEvenKey = (uint8_t*)pbyEvenKey;
	int i, j, k;
	uint16_t ecmPid = wEcmPID;
	int32_t ret = 0;
	play_scramble_para_t playpara = {0};
	app_play_get_playing_para(&playpara);


#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_DVB_FLAG
	{
		/*
		 * xinshimao ca wEcmPID=0
		 */
		int32_t dvb_ca_flag;
		dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
		if (DVB_CA_TYPE_DVB == dvb_ca_flag)
		{
			ecmPid = playpara.p_ecm_pid;
		}
	}
#endif
#ifdef DVB_CA_TYPE_XINSHIMAO_FLAG
{
	/*
	* xinshimao ca wEcmPID=0
	*/
	int32_t dvb_ca_flag;
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (DVB_CA_TYPE_XINSHIMAO == dvb_ca_flag)
		{
			ecmPid = playpara.p_ecm_pid;
		}
}
#endif
#ifdef DVB_CA_TYPE_DIVITONE_FLAG
{
	int32_t dvb_ca_flag;
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (DVB_CA_TYPE_DIVITONE == dvb_ca_flag)
		{		
			ecmPid = playpara.p_ecm_pid;
		}
}
#endif
#ifdef DVB_CA_TYPE_QILIAN_FLAG
{
	int32_t dvb_ca_flag;
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (DVB_CA_TYPE_QILIAN == dvb_ca_flag)
		{		
			ecmPid = playpara.p_ecm_pid;
		}
}
#endif
#ifdef DVB_CA_TYPE_WF_CA_FLAG
{
	int32_t dvb_ca_flag;
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if (DVB_CA_TYPE_WF == dvb_ca_flag)
	{
		ecmPid = playpara.p_ecm_pid;
	}
}
#endif
#endif

	if (NULL == pOddKey || NULL == pbyEvenKey)
	{
		CA_FAIL("app_porting_ca_descramble_set_cw Failed pOddKey=0x%x pbyEvenKey=0x%x\n",(unsigned int)pOddKey,(unsigned int)pbyEvenKey);
		return FALSE;
	}

	if (8 != byKeyLen)
	{
		CA_FAIL("app_porting_ca_descramble_set_cw Failed byKeyLen=%d \n",byKeyLen);
		return FALSE;
	}

	if (ecmPid == 0x1FFF)
	{
		CA_FAIL("app_porting_ca_descramble_set_cw Failed wEcmPID=0x%x \n",ecmPid);
		return FALSE;
	}
#if 0
	printf("pbyOddKey :\n");
	for(i=0;i<byKeyLen;i++)
		{
			printf("0x%02x ",pbyOddKey[i]);
		}
	printf("\n");

	printf("pbyEvenKey :\n");
	for(i=0;i<byKeyLen;i++)
		{
			printf("0x%02x ",pbyEvenKey[i]);
		}
	printf("\n");
#endif

	/*针对CW字不标准情况进行处理*/
	 for(i=0;i<2;i++)
	 {
		k = 0;
		for(j=0;j<3;j++)
			k +=pEvenKey[4*i+j];
		pEvenKey[4*i+3] = k&0xff;
	 }
	for(i=0;i<2;i++)
	{
		k = 0;
		for(j=0;j<3;j++)
		k +=pOddKey[4*i+j];
		pOddKey[4*i+3] = k&0xff;
	}

	if (0x00 == pOddKey[0] && 0x00 == pOddKey[1] && 0x00 == pOddKey[2] && 0x00 == pOddKey[3] &&
		0x00 == pOddKey[4] && 0x00 == pOddKey[5] && 0x00 == pOddKey[6] && 0x00 == pOddKey[7])
	{
		memset(pOddKey, 0xff, 8);
	}

	if (0x00 == pEvenKey[0] && 0x00 == pEvenKey[1] && 0x00 == pEvenKey[2] && 0x00 == pEvenKey[3] &&
		0x00 == pEvenKey[4] && 0x00 == pEvenKey[5] && 0x00 == pEvenKey[6] && 0x00 == pEvenKey[7])
	{
		memset(pEvenKey, 0xff, 8);
	}

//	CAS_Dbg(" app_porting_ca_descramble_set_cw  old_p_ecm_pid=0x%x\n",playpara.p_ecm_pid);
//	CAS_Dbg(" app_porting_ca_descramble_set_cw old_p_ecm_pid_audio=0x%x p_ecm_pid_video=0x%x\n",playpara.p_ecm_pid_audio,playpara.p_ecm_pid_video);
//	CAS_Dbg(" app_porting_ca_descramble_set_cw wEcmPID=0x%x\n",wEcmPID);

	if(( playpara.p_ecm_pid==ecmPid )||(playpara.p_ecm_pid_audio==ecmPid))
	{
		//CAS_Dbg(" app_porting_ca_descramble_set_cw PSI_STREAM_TYPE_AUDIO\n");

		if (0 == g_descramlbeList[1].handle)
		{
			CA_FAIL("app_porting_ca_descramble_set_cw Failed\n");

			return FALSE;

		}

		ret = GxDescrmb_SetCW(g_descramlbeList[1].handle, pOddKey, pEvenKey, 8);
		CHECK_RET(DESCRAMBLER,ret);
	}


	if(( playpara.p_ecm_pid==ecmPid )||(playpara.p_ecm_pid_video==ecmPid))
	{
		//CAS_Dbg(" app_porting_ca_descramble_set_cw PSI_STREAM_TYPE_VIDEO\n");

		if (0 == g_descramlbeList[0].handle)
		{
			CA_FAIL("app_porting_ca_descramble_set_cw Failed\n");

			return FALSE;
		}

		ret = GxDescrmb_SetCW(g_descramlbeList[0].handle, pOddKey, pEvenKey, 8);
#ifdef SHOW_ECM_GOT_TIME		
		extern GxTime starttime;
		GxTime nowtime={0};
		GxCore_GetTickTime(&nowtime);
		printf("\n start filter CW SET >>>>>[app_porting_ca_descramble_set_cw]->%d MS<<<<<\n\n",((nowtime.seconds*1000+nowtime.microsecs/1000) - (starttime.seconds*1000+starttime.microsecs/1000)));
		starttime.seconds = nowtime.seconds;
		starttime.microsecs = nowtime.microsecs;
#endif
		CHECK_RET(DESCRAMBLER,ret);
	}

	return TRUE;
} 







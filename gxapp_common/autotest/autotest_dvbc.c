#include <stddef.h>
#include <stdio.h>
#include "./include/module_device.h"
#include "./include/common_autotest.h"

typedef enum  {
	TEST_QAM_NONE = 0,
	TEST_QAM_16 = 1,
	TEST_QAM_32 = 2,
	TEST_QAM_64 = 3,
	TEST_QAM_128 = 4,
	TEST_QAM_256 = 5,
	TEST_QAM_TOTAL,
} qam_t;


typedef struct DVBCTp_s
{
	int Fre;		// ex. center tp:1150000
	int Sym;		// ex. 27500000
	int Qam;		// ex. 
	FrontendInfo_t TpInfo;
}DVBCTp_t;
static DVBCTp_t sg_DvbcTpInfo;


extern int frontend_voltage(int *pFrontendFd, unsigned int voltage);

static int dvbc_tp_lock(void* pTpInfo)
{
	int ret = 0;
	DVBCTp_t *p = NULL;
	struct dvb_frontend_parameters params;

	if(pTpInfo == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	p = (DVBCTp_t*)pTpInfo;
	
	// set tp
	params.frequency = p->Fre;
	params.inversion = INVERSION_OFF;
 	params.u.qam.modulation = p->Qam;
 	params.u.qam.symbol_rate = p->Sym;	
	
	


	ret = ioctl(p->TpInfo.FrontendDev, FE_SET_FRONTEND, &params);
#ifdef  LINUX_OS
    if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if(ret == 0)// wait lock TP
	{
		struct dvb_frontend_event event;
		do
		{	
			GxFrontend_GetEvent(p->TpInfo.TunerSelect, &event);
			GxCore_ThreadDelay(10);
		}while(event.status == 0 || event.parameters.frequency != params.frequency);
	}
#else
    if(ret < 0)
        log_printf("\nTEST, unlock, %s, %d\n",__FUNCTION__, __LINE__);
    else
        log_printf("\nTEST, lock, %s, %d\n",__FUNCTION__, __LINE__);
    ret = 0;
#endif
	return ret;
}

//"dvbt2://fre:%d&bandwidth:%d&workmode:%d&vpid:%d&apid:%d&pcrpid:%d&vcodec:%d&acodec:%d&tuner:%d&scramble:%d&pmt:%d&data_plp_id:%d&common_plp_exist:%d&common_plp_id:%d",
//dvbt2://fre:850000&bandwidth:0&workmode:3&vpid:841&apid:842&pcrpid:841&vcodec:1&acodec:5&tuner:0&scramble:0&pmt:840&data_plp_id:0&common_plp_exist:0&common_plp_id:0&tsid:1&dmxid:0&progid:27

//dvbc://fre:850000&symbol:687500000&qam:2&vpid:515&apid:652&pcrpid:131&vcodec:0&acodec:0&tuner:0&scramble:0&pmt:5202&tsid:0&dmxid:0"};
static int _generate_url(char *pUrl, ProgInfo_t *pProgInfo)
{
	char Buffer[50] = {0};	
	if((pUrl == NULL) || (pProgInfo == NULL))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	// head
	strcat(pUrl,"dvbc://fre:0&symbol:0&qam:0");
	// vpid, apid, pcrpid
	sprintf(Buffer,"&vpid:%d&apid:%d&pcrpid:%d", pProgInfo->VideoPid,pProgInfo->AudioPid, pProgInfo->PCRPid);
	strcat(pUrl, Buffer);
	//vcodeformat, acodeformat
	memset(Buffer, 0, 50);
	sprintf(Buffer,"&vcodec:%d&acodec:%d", pProgInfo->VCodecFormat,pProgInfo->ACodecFormat);
	strcat(pUrl, Buffer);
	//demux parameter
	memset(Buffer, 0, 50);
	sprintf(Buffer,"&tuner:%d&tsid:%d&tsmode:%d&dmxid:%d",pProgInfo->TunerSelect,pProgInfo->TsSource,pProgInfo->TsMode,pProgInfo->DemuxId);
	strcat(pUrl, Buffer);
	log_printf("\nTEST, play url = %s, len = %d\n",pUrl,strlen(pUrl));
    return 0;	
}


static int dvbc_play(void *pInfo)
{
	int ret = 0;
	GxMsgProperty_PlayerPlay play = {0};
//    PlayerWindow            VideoRect 	= {0, 0, 1280, 720};
	ProgInfo_t *pProgInfo = NULL;
	
	if(pInfo == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}	

	pProgInfo = (ProgInfo_t*)pInfo;
	play.player = PLAYER_NAME;
    memset(play.url, 0, PLAYER_URL_LONG);
	ret = _generate_url(play.url, pProgInfo);
	if(ret < 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
//	memcpy(&play.window, &VideoRect, sizeof(PlayerWindow));
	
    GxPlayer_MediaPlay(play.player, play.url, 0, 0, NULL);
	return ret;
}

// now only deal with "SET_TP", "SET_PLAY", "SET_TS"

// KEYWORD: FRE, SYM, PRO, DMX, TSMODE, TSSOURCE, TESELECT
static int _parse_tp_para(char *Command, void **ppInOutFrontendInfo, void **ppInOutTpInfo)
{
	int ret = 0;
	//static DVBSTp_t Info; 
	char *p = NULL;
	char Buffer[50] = {0};
	if((ppInOutFrontendInfo == NULL) || (Command == NULL) || (ppInOutTpInfo == NULL))
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	memset(&sg_DvbcTpInfo, 0, sizeof(DVBCTp_t));
	// demod type
	sg_DvbcTpInfo.TpInfo.DemodType = FRONTEND_DVB_C;
	// FRE
	p = strstr(Command, KEYWORD_FRE);
	if(p == NULL)
	{// DEFAULT FRE
		sg_DvbcTpInfo.Fre = 666000;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", &sg_DvbcTpInfo.Fre);
	}
	
	// SYM
	p = strstr(Command, KEYWORD_SYM);
	if(p == NULL)
	{// DEFAULT SYM
		sg_DvbcTpInfo.Sym = 6875000;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", &sg_DvbcTpInfo.Sym);
	}
	
		
	// QAM
	p = strstr(Command, KEYWORD_QAM);
	if(p == NULL)
	{// DEFAULT QAM
		sg_DvbcTpInfo.Qam = TEST_QAM_64;// enum: TEST_QAM_16=0;TEST_QAM_32=1;TEST_QAM_64=2;TEST_QAM_128=3;TEST_QAM_256=4;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", &sg_DvbcTpInfo.Qam);		
	}



	// DMX
	p = strstr(Command, KEYWORD_DMX);
	if(p == NULL)
	{// DEFAULT DEMUX ID
		sg_DvbcTpInfo.TpInfo.DemuxPara.DemuxId = 0;// demux 0
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&sg_DvbcTpInfo.TpInfo.DemuxPara.DemuxId);
	}

	// TSMODE
	p = strstr(Command, KEYWORD_TSMODE);
	if(p == NULL)
	{// DEFAULT TSMODE
		sg_DvbcTpInfo.TpInfo.DemuxPara.TsMode = 0;// paralle
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&sg_DvbcTpInfo.TpInfo.DemuxPara.TsMode);
	}

	// TSSOURCE
	p = strstr(Command, KEYWORD_TSSOURCE);
	if(p == NULL)
	{// DEFAULT SOURCE
		sg_DvbcTpInfo.TpInfo.DemuxPara.TsSource = 0;// source 0
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&sg_DvbcTpInfo.TpInfo.DemuxPara.TsSource);
	}

	// TSSELECT
	p = strstr(Command, KEYWORD_TSSELECT);
	if(p == NULL)
	{// DEFAULT TS SELECT
		sg_DvbcTpInfo.TpInfo.TunerSelect = 0; // tuner 1
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&sg_DvbcTpInfo.TpInfo.TunerSelect);
	}

	*ppInOutFrontendInfo = (void*)&(sg_DvbcTpInfo.TpInfo);
	*ppInOutTpInfo = (void*)&sg_DvbcTpInfo;
	return ret; 
}

// KEYWORD: VPID, APID, PPID, VFORMAT, AFORMAT, DMX, TSMODE, TSSOURCE, TSSELECT
static int _parse_play_para(char *Command, void **pInOutPlayInfo, void **Out2)
{
	int ret = 0;
	char *p = NULL;
	char Buffer[50] = {0};
	static ProgInfo_t Info;
	
	if((pInOutPlayInfo == NULL) || (Command) == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	log_printf("\nTEST, play cmd= %s\n",Command);
	memset(&Info, 0, sizeof(ProgInfo_t));
	// VPID
	p = strstr(Command, KEYWORD_VPID);
	if(p == NULL)
	{// DEFAULT VPID
		Info.VideoPid = 36;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.VideoPid);
	}
	// APID
	p = strstr(Command, KEYWORD_APID);
	if(p == NULL)
	{// DEFAULT APID
		Info.AudioPid= 35;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.AudioPid);
	}

	// PPID
	p = strstr(Command, KEYWORD_PPID);
	if(p == NULL)
	{// DEFAULT PPID
		Info.PCRPid = 36;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.PCRPid);
	}

	// DMX
	p = strstr(Command, KEYWORD_DMX);
	if(p == NULL)
	{// DEFAULT DEMUX ID
		//Info.DemuxId = 0xff;// need from tp info
		Info.DemuxId = sg_DvbcTpInfo.TpInfo.DemuxPara.DemuxId;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.DemuxId);
	}

	// TSMODE
	p = strstr(Command, KEYWORD_TSMODE);
	if(p == NULL)
	{// DEFAULT TSMODE
		//Info.TsMode = 0xff;// need from tp info
		Info.TsMode = sg_DvbcTpInfo.TpInfo.DemuxPara.TsMode;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.TsMode);
	}

	// TSSOURCE
	p = strstr(Command, KEYWORD_TSSOURCE);
	if(p == NULL)
	{// DEFAULT SOURCE
		//Info.TsSource = 0xff;// need from tp info
		Info.TsSource = sg_DvbcTpInfo.TpInfo.DemuxPara.TsSource;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.TsSource);
	}

	// TSSELECT
	p = strstr(Command, KEYWORD_TSSELECT);
	if(p == NULL)
	{// DEFAULT TS SELECT
		//Info.TunerSelect = 0xff; // need from tp info
		Info.TunerSelect = sg_DvbcTpInfo.TpInfo.TunerSelect;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.TunerSelect);
	}

	// VFORMAT
	p = strstr(Command, KEYWORD_VFORMAT);
	if(p == NULL)
	{// DEFAULT V FORMAT
		Info.VCodecFormat= 0; // h.264
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.VCodecFormat);
	}

	// AFORMAT
	p = strstr(Command, KEYWORD_AFORMAT);
	if(p == NULL)
	{// DEFAULT A FORMAT
		Info.ACodecFormat= 0; // mpeg2
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.ACodecFormat);
	}

	*pInOutPlayInfo = (void*)&Info;
	return ret; 
}

static int _parse_ts_para(char *Command, void **pInOutTsInfo, void **Out2)
{
	int ret = 0;
	static TsFilterPara_t Info; 
	char *p = NULL;
	char Buffer[50] = {0};
	if((pInOutTsInfo == NULL) || (Command) == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}

	memset(&Info, 0, sizeof(TsFilterPara_t));

	// TS PID
	p = strstr(Command, KEYWORD_TSPID);
	if(p == NULL)
	{// DEFAULT TS ID
		Info.TsPID = 36;// video pid
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.TsPID);
	}

	// DMX
	p = strstr(Command, KEYWORD_DMX);
	if(p == NULL)
	{// DEFAULT DEMUX ID
		//Info.DemuxPara.DemuxId = 0xff;// from tp info
		Info.DemuxPara.DemuxId = sg_DvbcTpInfo.TpInfo.DemuxPara.DemuxId;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.DemuxPara.DemuxId);
	}

	// TSMODE
	p = strstr(Command, KEYWORD_TSMODE);
	if(p == NULL)
	{// DEFAULT TSMODE
		//Info.DemuxPara.TsMode = 0xff;// from tp info
		Info.DemuxPara.TsMode = sg_DvbcTpInfo.TpInfo.DemuxPara.TsMode;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.DemuxPara.TsMode);
	}

	// TSSOURCE
	p = strstr(Command, KEYWORD_TSSOURCE);
	if(p == NULL)
	{// DEFAULT SOURCE
		//Info.DemuxPara.TsSource = 0xff;// from tp info
		Info.DemuxPara.TsSource = sg_DvbcTpInfo.TpInfo.DemuxPara.TsSource;
	}
	else
	{// find the keyword
		memset(Buffer, 0, sizeof(Buffer));
		sscanf(p,"%*[^=]=%[^ ]", Buffer);
		sscanf(Buffer, "%d", (int*)&Info.DemuxPara.TsSource);
	}

	*pInOutTsInfo = (void*)&Info;
	return ret; 
}

static int dvbc_parse(char* Command, void **ppInOut1, void **ppInOut2)
{
extern dev_tbl_t dev_dvbc_reg;
	int ret = 0;
	char Buffer[256] = {0};
	int i = 0;
	if((ppInOut1 == NULL) || (Command) == NULL)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}
	//start parse
	if((Command[0] < 'a') || (Command[0] > 'z'))
		sscanf(Command, "%*[^a-z]%s",Buffer);
	else
		sscanf(Command, "%s", Buffer);
	//sscanf(Command, "%s",Buffer);
	if(strlen(Buffer) == 0)
	{
		log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
		return -1;
	}	
	for(i = 0; i < dev_dvbc_reg.UserDefineFuncs.CmdNum; i++)
	{
		if(0 == memcmp(Buffer, dev_dvbc_reg.UserDefineFuncs.Cmds[i].CommandName, strlen(dev_dvbc_reg.UserDefineFuncs.Cmds[i].CommandName)))
		{// need to parse
			if(dev_dvbc_reg.UserDefineFuncs.Cmds[i].ParseFunction)
			{
				ret = dev_dvbc_reg.UserDefineFuncs.Cmds[i].ParseFunction(Command, ppInOut1, ppInOut2);
				if(ret < 0)
				{
					log_printf("\nTEST, error, %s, %d\n",__FUNCTION__,__LINE__);
					return -1;
				}
				return 0;
			}
			break;
		}
	}
	return -1;
}

dev_tbl_t dev_dvbc_reg = {
	.name       = "dvbc",
	.lock_tp    = dvbc_tp_lock,
	.play       = dvbc_play,
	.parse      = dvbc_parse,
	.UserDefineFuncs = {
		.CmdNum = 3,
		.Cmds   = {
			{.CommandName = "set_tp", .ParseFunction = _parse_tp_para,},
			{.CommandName = "play", .ParseFunction = _parse_play_para,},
			{.CommandName = "set_ts_compare", .ParseFunction = _parse_ts_para,},
		},
	},
	.usage      = "function for dvbc",
};


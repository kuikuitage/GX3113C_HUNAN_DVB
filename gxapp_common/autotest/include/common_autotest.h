#ifndef __COMMON_AUTOTEST_H_
#define __COMMON_AUTOTEST_H_

#include <gxtype.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "gxcore.h"
#include "module/frontend/gxfrontend_module.h"
#include "service/gxplayer.h"
#include "gxavdev.h"


typedef enum TsSource_s{
	TS_SOURCE_1 = 0,
	TS_SOURCE_2,
	TS_SOURCE_3,
	TS_SOURCE_TOTAL,
}TsSource_e;

typedef enum DemuxId_s{
	DEMUX_ID_1 = 0,
	DEMUX_ID_2,
	DEMUX_ID_3,
	DEMUX_ID_4,
	DEMUX_ID_TOTAL,
}DemuxId_e;

typedef enum TsMode_s{
	TS_PARALLEL = 0,
	TS_SERIAL,
	TS_MODE_TOTAL,
}TsMode_e;

typedef enum TunerNum_s
{
	TUNER_1 = 0,
	TUNER_2,
	TUNER_NUM_TOTAL
}TunerNum_e;

typedef struct DemuxConfigPara_s
{
	TsSource_e TsSource;
	DemuxId_e  DemuxId;
	TsMode_e   TsMode;
	int        DemuxDev;
	int        DemuxModule;
}DemuxConfigPara_t;


typedef struct TsFilterPara_s
{
	DemuxConfigPara_t 	DemuxPara;
	unsigned short 		TsPID;//for slot,invilad id >= 0x1fff
	unsigned char  		TsPackageLen;// 188 or 224
}TsFilterPara_t;


typedef struct TpInfo_s
{
	DemuxConfigPara_t 	DemuxPara;
 	TunerNum_e 			TunerSelect;
	GxFrontendType 		DemodType;
	int        			FrontendDev;
	//DVBSTp_t   			DVBSTP;
  
}FrontendInfo_t;

typedef struct ProgInfo_s
{
	int 				VideoPid;
	int 				AudioPid;
 	int 				PCRPid;
	int 				VCodecFormat;
	int 				ACodecFormat;
	TunerNum_e 			TunerSelect;
	TsSource_e 			TsSource;
	DemuxId_e  			DemuxId;
	TsMode_e   			TsMode;
	GxFrontendType 		DemodType;  
}ProgInfo_t;

typedef enum FrontendMode_s
{   
	TEST_DVBS2_NORMAL,
	TEST_DVBS2_BLIND,	  
	TEST_DVBT_AUTO_MODE,
	TEST_DVBT_NORMAL,
	TEST_DVBT2_BASE,
	TEST_DVBT2_LITE,
	TEST_DVBT2_BASE_LITE,
	TEST_DVBC_J83A,
	TEST_DVBC_J83B,
}FrontendMode_e;



#define KEYWORD_type 		"type"
#define KEYWORD_FRE			"fre"
#define KEYWORD_SYM			"sym"
#define KEYWORD_PRO			"pol"
#define KEYWORD_22K			"22k"
#define KEYWORD_DMX			"dmx"
#define KEYWORD_TSMODE		"tsmode"
#define KEYWORD_TSSOURCE	"tssource"
#define KEYWORD_TSSELECT	"tsselect"
#define KEYWORD_VPID		"vpid"
#define	KEYWORD_APID		"apid"
#define KEYWORD_PPID		"ppid"
#define KEYWORD_VFORMAT		"vformat"
#define KEYWORD_AFORMAT		"aformat"
#define KEYWORD_TSPID		"tspid"

// FOR T OR T2 OR C
#define KEYWORD_BANDWIDTH	"bandwidth"
// FOR T2
#define KEYWORD_WORKMODE	"workmode"
#define KEYWORD_DATAPLPID	"dataplpid"
#define KEYWORD_COMMONPLPID	"commonplpid"
#define KEYWORD_COMMONPLPEXIST	"commonplpexite"
// FOR C
#define KEYWORD_QAM "qam"


// for play
#define PLAYER_NAME			"player1"
#define _SIZEOF(ptr) (sizeof(ptr)/sizeof(ptr[0]))

// uart
int uart_init(unsigned int Baudrate);
int uart_receive(unsigned char *Buffer, unsigned int byteToRead);
int uart_send(unsigned char *Buffer, unsigned int byteToWrite);
int uart_close(void);
void UartPrintf(const char *fmt, ...);
#ifdef ECOS_OS
//#define log_printf UartPrintf
#define log_printf(...) do{}while(0)
#else
#define log_printf  printf
//#define log_printf(...) do{}while(0)
#endif
// ts compare
int start_ts_compare(void *pTsFilterPara);
int stop_ts_compare(void);
int get_ts_compare_result(int *ReceivePackageCount, int *ErrorPackageCount);

// frontend
int init_frontend(void* pTpInfo);
int get_lock_status(void);
int init_play(void* ProgInfo);
int stop_play(void);
int stop_all(void);
int compare_ts_data(void *TsPara);
int destroy_frontend(void);

// common
int auto_test_reply(char *cmd, char *str, unsigned int error_code, char *result);
int auto_test_protocol_data_receive(unsigned char* MemBuffer, int MemSize,
											 int *pReadPos,int *pWritePos,
											 unsigned char *ReceiveData,int ReceiveSize);
int auto_test_protocol_data_dealwith(unsigned char* MemBuffer, int MemSize,
											 int *pReadPos,int *pWritePos,
											 int ByteToDo);


#endif


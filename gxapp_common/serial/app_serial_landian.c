#include "app_serial_landian.h"
//#include "connect_api.h"
#include "app_common_play.h"
#include "app_common_prog.h"
#include "app_common_search.h"
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ca_flash.h"
#include "app_common_lcn.h"

#include "gxfrontend.h"
#include "gxmsg.h"
#include "gxprogram_manage_berkeley.h"
#include "service/gxplayer.h"
#include "gxplayer_url.h"
#include "gui_core.h"

extern void app_win_exist_to_full_screen(void);


#include <gxcore.h>

#define SUCCESS 0

#define SERIAL_DEVICE_NAME                          "/dev/ser0"

static  FILE*                   ser_read_fd = NULL;
FILE*                   abv_w_stream = NULL;

#define COMDATA_DMX_MUTEX_LOCK() cyg_scheduler_lock();
#define COMDATA_DMX_MUTEX_UNLOCK() cyg_scheduler_unlock();
#define COMDATA_DMX_MUTEX_INIT()


void ABV_STBCA_Enter_Provider_Mode(void)
{
	extern int print_enable;
	print_enable  = 0;
		
}
void ABV_STBCA_Exit_Provider_Mode(void)
{
	extern int print_enable;
	print_enable  = 1;
	
} 
bool Send_ComData(uint8_t *buf,uint8_t Length,uint32_t TimeOut)
{
	uint32_t len;
	ssize_t                     real_size;
//	return TRUE;
	if(buf == NULL)
	{
		return TRUE;
	}
	
	if(Length >= 255)
	{
		return TRUE;
	}
	if(abv_w_stream == NULL)
	{
		return TRUE;
	}

	real_size = fwrite(buf,1,Length,abv_w_stream);
	fflush(abv_w_stream);
	if(real_size == Length)
		return FALSE;
	else
		return TRUE;
}
bool com_read_tm(uint8_t *buf,uint8_t Length,uint32_t TimeOut)
{
	ssize_t                     read_size;
	if(buf == NULL)
	{
		return TRUE;
	}
	
	if(Length >= 255)
	{
		return TRUE;
	}
	if(ser_read_fd == NULL)
	{
		return TRUE;
	}
	read_size = fread(buf, 1, Length, ser_read_fd);
        if(read_size <= Length)
		return FALSE;
	else
	  	return TRUE;
}


#define libc_printf_abv_com1 // libc_printf

#define HEAD_DATA_SIZE 1
#define END_DATA_SIZE 1
/*
同步头由"tttt"改为"[satus]"
结束字符由"nnnn"改为"[finis]"
*/


#if 1
static uint8_t pairhead[HEAD_DATA_SIZE] = {0xAA,};// pair head
static uint8_t pairend[END_DATA_SIZE+1] = {0xBB,0};// pair end
uint8_t getComDataType(uint8_t * datatype)
{
	uint8_t i;
	uint8_t	data[HEAD_DATA_SIZE];
	uint8_t	data1[HEAD_DATA_SIZE];
	uint8_t	ret = 0;

	uint16_t msecs;
	uint8_t *ptmpBuffer = data1;

	

	i = 0;
	msecs = 10;
	memset(data,0,HEAD_DATA_SIZE);
	if(SUCCESS != com_read_tm(data,1,msecs))
    {
		return 0;
    }
//	libc_printf_abv_com1("getPCPairData data[0] is :0x%02x\n",data[0]);
	
	//libc_printf_abv_com1("getPCPairData 1\n");
	if(data[0] == 0xAA)
	{
		if(SUCCESS != com_read_tm(data,1,msecs))
		 	return 0;

		if ((data[0]>0)&&(data[0]<9))
		{
			*datatype = data[0];
			return 1;
		}
		else
			return 0;
	}
	else
	{
		return 0;
	}	


	return ret;
}



//ABV_UInt16 aaaa;
static bool getPCPairData(uint8_t *pBuffer,uint8_t *Length)
{
	uint8_t	data[END_DATA_SIZE+1];
	//ABV_UInt8	enddata[END_DATA_SIZE];
	uint16_t 	dataLen = 0;
	uint16_t msecs = 1;
	uint8_t *ptmpBuffer = pBuffer;
	/*
	同步头由"tttt"改为"[satus]"
 	结束字符由"nnnn"改为"[finis]"
	*/
	
    if(SUCCESS != com_read_tm(data,1,msecs))
    {
	 	return TRUE;
    }
	
	*Length = 0;
	while(data[0] != 0xBB)
	{
		ptmpBuffer[0] = data[0];
		ptmpBuffer++;
		dataLen++;
		if(SUCCESS != com_read_tm(data,1,msecs))
	    {
		 	return TRUE;
	    }
				
		if(dataLen >254)
			break;
	}	

	if(data[0] != 0xBB)
	{
		return TRUE;
	}
	
	*Length = dataLen;

	return FALSE;
			
	
}



bool SendReplyToPC(uint8_t datatype,uint8_t param)
{
	uint8_t data[256] ;
	memset(data,0,sizeof(data));
	if( datatype == GET_PID_ID
		||datatype == GET_VOLUME_ID
		||datatype ==GET_AUDIO_PID_ID 
		||datatype ==GET_ALL_SEARCH
		||datatype == GET_TV_RADIO_MODE)
	{
		data[0] = 0xAA;
		data[1] = datatype;
		data[2] = 1;
	        data[3]	=param;
		data[4] = data[0]+data[2]+data[3];
		data[5] = 0xBB;
		while(Send_ComData(data,6,100) == FALSE)
			break;
	}
}

bool SendDataToPC(uint8_t datatype,uint8_t* m_buffer,uint8_t m_length)
{
	uint8_t data[1024] ;
	uint8_t	crc = 0;
	int i = 0;
	memset(data,0,sizeof(data));
	if( datatype == SEND_TS_ID
		||datatype == SEND_SIGNAL_QUALITY
		||datatype == SEND_DATA_STATUS)
	{
		data[0] = 0xAA;
		data[1] = datatype;
		data[2] = m_length;
	        memcpy(&data[3],m_buffer,m_length);
		for( i==0 ; i<m_length ; i++ )
		{
			crc+=m_buffer[i];
		}	
		data[m_length+3] = data[0]+data[2]+crc;
		data[m_length+3+1] = 0xBB;
		while(Send_ComData(data,m_length+5,100) == FALSE)
			break;
	}
}

#endif

uint32_t app_serial_cas_api_lock_service(uint8_t pos)
{
	int32_t serviceIndex = 0; // 逻辑频道号
	GxBusPmDataProg prog_data = {0};
	GxBusPmViewInfo sysinfo = {0};
	int32_t config = 0;
	uint32_t audio_num = 	0;
	GxBusPmViewInfo sysinfo_old;
	audio_num = app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);
	if (audio_num <= 0)
	{
		printf("[app_serial_cas_api_lock_service]invalid serviceIndex!!!\n");
		return 0;
	}
	if( pos >= audio_num )
	{
		return audio_num;
	}
	GxBus_PmViewInfoGet(&sysinfo_old);	
	if( sysinfo_old.stream_type == 	GXBUS_PM_PROG_RADIO)
	{
	}
	else
	{
		GxBus_PmViewInfoGet(&sysinfo);
		sysinfo.stream_type = GXBUS_PM_PROG_RADIO;
		GxBus_PmViewInfoModify(&sysinfo);
		app_prog_update_num_in_group();
	}	

	app_win_exist_to_full_screen();

	GxBus_PmProgGetByPos(pos, 1, &prog_data); 
	if (GXCORE_SUCCESS == GxBus_PmViewInfoGet(&sysinfo))
	{
		if (sysinfo.stream_type == GXBUS_PM_PROG_TV)
		{
			sysinfo.tv_prog_cur = prog_data.id;
		}
#if (1 == DVB_HD_LIST)
		else if(sysinfo.stream_type == GXBUS_PM_PROG_HD_SERVICE)
		{
			sysinfo.hd_prog_cur = prog_data.id;
		}
#endif
		else
		{
			sysinfo.radio_prog_cur = prog_data.id;
		}

		GxBus_PmViewInfoModify(&sysinfo);
		app_prog_save_playing_pos_in_group(pos);//设置节目号
	}

	app_play_switch_prog_clear_msg();

	GUI_CreateDialog("win_prog_bar");
	app_play_reset_play_timer(0);

	return;	
}
static uint8_t pBuffer[1000];
uint16_t SerialTsPID = 0x1FFF;
uint8_t SerialVolume = 0;
uint8_t SerialServiceId = 0xFF;
extern GXSerialSection serialSection ;
void serial_send_data_status(void)
{
	GxBusPmViewInfo sysinfo = {0};
	int32_t volume = 0;
	uint8_t pos = 0;
	uint8_t group_mode = 0 ;
	uint8_t stream_type = 0;
	uint8_t fav_id = 0;
	uint8_t audio_num = 	0;
	uint32_t value = 0;
	uint8_t data[1024] ;
	uint8_t	crc = 0;
	int i = 0;
	GxFrontendSignalQuality Singal;
	uint8_t signalvalue = 0;
	
	memset(data,0,sizeof(data));
	value = GxFrontend_GetQuality(0, &Singal);
	signalvalue = Singal.snr;//*100/35;
	if (signalvalue >=100)
		signalvalue = 100;

	volume = app_flash_get_config_audio_volume();
	audio_num = app_prog_check_group_num(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);
	if (GXCORE_SUCCESS == GxBus_PmViewInfoGet(&sysinfo))
	{
		app_prog_find_by_id(sysinfo.radio_prog_cur,&pos,&group_mode,&stream_type,&fav_id);
	}
	data[0] = 0xAA;
	data[1] = SEND_DATA_STATUS;
	data[2] = 196;
	data[3] = (uint8_t)(volume/3);
	data[4] = (uint8_t)pos;
	data[5] = (uint8_t)audio_num;
	if (sysinfo.stream_type ==  GXBUS_PM_PROG_TV )
	{
		data[6] = 1;
	}
	else 
	{
		data[6] = 0;
	}
	data[7] = signalvalue;
	data[8] = (uint8_t)((SerialTsPID&0xFF00)>>8);
	data[9] = (uint8_t)(SerialTsPID&0xFF);
	if( serialSection.pid == SerialTsPID )
	{
		data[10] =  1;
		memcpy(&data[11],serialSection.buffer,188);
	}
	else
	{
		data[10] = 0;
	}
	for( i==0 ; i<196 ; i++ )
	{
		crc+=data[i+3];
	}	
	data[196+3] = data[0]+data[2]+crc;
	data[196+3+1] = 0xBB;
	while(Send_ComData(data,196+5,100) == FALSE)
		break;

}

static event_list* spSerialTime = NULL;
static event_list* spSerialTime3S = NULL;
static  int timer(void *userdata)
{
	serial_send_data_status();
	return 0;
}
static  int timer3S(void *userdata)
{
	if (NULL == spSerialTime)
	{
		spSerialTime	= create_timer(timer, 100, NULL,  TIMER_REPEAT);
	}
	else
	{
		reset_timer(spSerialTime);
	}
	spSerialTime3S = NULL;
	return 0;
}
static void ComData_Process(void* args)
{

	uint32_t			msgSize;
	uint8_t data_length = 0;
	uint32_t timeout = 10;
	uint8_t  crc = 0;
	uint8_t Act;
	bool ERR;
	uint8_t dataType;
	uint8_t ret = 0;
	uint8_t Length = 1000;//0;
	GxBusPmViewInfo sysinfo = {0};
	ABV_STBCA_Enter_Provider_Mode();

	while(1)
	{

		COMDATA_DMX_MUTEX_LOCK();

		ret = 0;
		//osdDebug("[getComDataType]: 1.\n");

		ret = getComDataType(&dataType);
		if(ret == 0)
		{
			COMDATA_DMX_MUTEX_UNLOCK();
			GxCore_ThreadDelay(500);
			continue;
		}
		if(getPCPairData(pBuffer,&Length) == FALSE)
		{
			if (NULL != spSerialTime)	
			{
				timer_stop(spSerialTime);
			}
			spSerialTime3S	= create_timer(timer3S, 3000, NULL,  TIMER_ONCE);
			data_length = pBuffer[0];
			crc = pBuffer[Length-1];	
			if( dataType == SEND_TS_ID )
			{

			}
			else if( dataType == GET_PID_ID )
			{
				SerialTsPID = pBuffer[1]*256+pBuffer[2];
				app_table_ts_monitor_filter_open(SerialTsPID);
				SendReplyToPC(dataType,0x01);
			}
			else if( dataType == GET_VOLUME_ID )
			{
				SerialVolume = pBuffer[1]*3;	
				app_flash_save_config_audio_volume(SerialVolume);
				app_play_set_volumn(SerialVolume);
				SendReplyToPC(dataType,0x01);
			}
			else if( dataType == GET_AUDIO_PID_ID )
			{
				uint8_t audio_num;
				SerialServiceId = pBuffer[1];
				audio_num = app_serial_cas_api_lock_service(SerialServiceId);
				SendReplyToPC(dataType,audio_num);
			}
			else if( dataType ==  SEND_SIGNAL_QUALITY )
			{
				uint32_t value = 0;
				GxFrontendSignalQuality Singal;
				uint8_t signalvalue = 0;
				value = GxFrontend_GetQuality(0, &Singal);
				signalvalue = Singal.snr;//*100/35;
				if (signalvalue >=100)
					signalvalue = 100;
				SendDataToPC(SEND_SIGNAL_QUALITY,&signalvalue,1);

			}
			else if( dataType == GET_TV_RADIO_MODE )
			{
				uint8_t tv_radio_mode = 0;
				tv_radio_mode = pBuffer[1];
				if (GXBUS_PM_PROG_RADIO == app_prog_get_stream_type())
				{
					if(tv_radio_mode == 1 )
					{
						if (0 != app_play_switch_tv_radio())
						{
							break;
						}
						app_play_switch_prog_clear_msg();
						GUI_EndDialog("win_menu_volume");
						GUI_EndDialog("win_prog_bar");
						GUI_EndDialog("win_prog_num");
						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(300);
					}
				}
				else
				{
					if( tv_radio_mode == 0 )
					{
						if (0 != app_play_switch_tv_radio())
						{
							break;
						}
						app_play_switch_prog_clear_msg();
						GUI_EndDialog("win_menu_volume");
						GUI_EndDialog("win_prog_bar");
						GUI_EndDialog("win_prog_num");
						GUI_CreateDialog("win_prog_bar");
						app_play_reset_play_timer(300);
					}
				}
				SendReplyToPC(dataType,0x01);
			}
			else if( dataType == GET_ALL_SEARCH )
			{
				uint32_t symbol_rate = 0;
				uint32_t qam = 0;
				symbol_rate = app_flash_get_config_center_freq_symbol_rate();
				qam = app_flash_get_config_center_freq_qam();
				app_play_stop();
				app_play_clear_msg_pop_type(MSG_POP_NO_PROG);
				GUI_CreateDialog("win_search_result");	
				GUI_SetInterface("flush", NULL);
				app_search_scan_all_mode(115,858,symbol_rate,qam);
				SendReplyToPC(dataType,0x01);
			}

		}
		else
		{
		
		}	

		COMDATA_DMX_MUTEX_UNLOCK();


	}

}


static bool bComDataTask_init = FALSE;
bool ComData_Process_task_init(void)
{
	handle_t	com_handle;

	if(bComDataTask_init == TRUE)
		return FALSE;

	memset(&serialSection,0,sizeof(GXSerialSection));
	COMDATA_DMX_MUTEX_INIT();
	ABV_STBCA_Enter_Provider_Mode();
	//ser_read_fd = fopen(SERIAL_DEVICE_NAME, "r+");
	ser_read_fd = fopen(/*SERIAL_DEVICE_NAME*/"/dev/ser0", "r+");
	abv_w_stream = fopen(/*SERIAL_DEVICE_NAME*/"/dev/ser0", "r+");
	GxCore_ThreadCreate("com_provider",&com_handle, ComData_Process, NULL, 10 * 1024, GXOS_DEFAULT_PRIORITY);
#if 1
	if (NULL == spSerialTime)
	{
		spSerialTime	= create_timer(timer, 100, NULL,  TIMER_REPEAT);
	}
	else
	{
		reset_timer(spSerialTime);
	}
#endif
	bComDataTask_init = TRUE;
	return TRUE;
}





#include "app_serial_yuehai.h"
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

#define KEY_OK              GUIK_RETURN
#define KEY_MENU            GUIK_M
#define KEY_EXIT            GUIK_ESCAPE
#define KEY_RECALL          GUIK_BACKSPACE
#define KEY_TV_RADIO        GUIK_T

#define KEY_MUTE            GUIK_HOME


#define KEY_FAV             GUIK_F
#define KEY_EPG             GUIK_E

#define KEY_LEFT            GUIK_LEFT
#define KEY_RIGHT           GUIK_RIGHT
#define KEY_UP              GUIK_UP
#define KEY_DOWN            GUIK_DOWN

#define KEY_1               GUIK_1
#define KEY_2               GUIK_2
#define KEY_3               GUIK_3
#define KEY_4               GUIK_4
#define KEY_5               GUIK_5
#define KEY_6               GUIK_6
#define KEY_7               GUIK_7
#define KEY_8               GUIK_8
#define KEY_9               GUIK_9
#define KEY_0               GUIK_0

#define KEY_RED             GUIK_R
#define KEY_YELLOW          GUIK_Y
#define KEY_BLUE            GUIK_B
#define KEY_GREEN           GUIK_G
#define KEY_POWER           GUIK_P
#define KEY_PROG_INFO	          GUIK_I
#define KEY_MAIL						GUIK_F5

#define KEY_PAGE_UP         GUIK_PAGE_UP
#define KEY_PAGE_DOWN       GUIK_PAGE_DOWN

#define KEY_TRACK           GUIK_S


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
uint8_t getComDataType(void)
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
	if(data[0] == 'A')
	{
		return 1;
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
	while(data[0] != 0x0D)
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

	if(data[0] != 0x0D)
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


static uint8_t pBuffer[100];
uint16_t SerialTsPID = 0x1FFF;
uint8_t SerialVolume = 0;
uint8_t SerialServiceId = 0xFF;



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
	GUI_Event keyEvent = {0};
	int keymap=0;
	while(1)
	{
		COMDATA_DMX_MUTEX_LOCK();
		ret = getComDataType();
		if(ret == 0)
		{
			COMDATA_DMX_MUTEX_UNLOCK();
			GxCore_ThreadDelay(500);
			continue;
		}
		if(getPCPairData(pBuffer,&Length) == FALSE)
		{
	
			keyEvent.type = GUI_KEYDOWN;
			switch ( pBuffer[0] )
			{
				case 0x30 :
					keyEvent.key.scancode = 0xfd77;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '1' :
					keyEvent.key.scancode = 0xfddf;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '2':
					keyEvent.key.scancode = 0xfd5f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '3':
					keyEvent.key.scancode = 0xfd9f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '4':
					keyEvent.key.scancode = 0xfdef;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '5':
					keyEvent.key.scancode = 0xfd6f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '6':
					keyEvent.key.scancode = 0xfdaf;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '7':
					keyEvent.key.scancode = 0xfdcf;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '8':
					keyEvent.key.scancode = 0xfd4f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case '9':
					keyEvent.key.scancode = 0xfd8f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'A':
				//	keyEvent.key.sym = KEY_MENU;
					keyEvent.key.scancode = 0xfd65;
					keymap =  find_key(keyEvent.key.scancode);
				//	GUI_SendEvent(GUI_GetFocusWindow(),&keyEvent);	
					break;
				case 'B':
				//	keyEvent.key.sym = KEY_EXIT;
					keyEvent.key.scancode = 0xfdc5;
					keymap =  find_key(keyEvent.key.scancode);
					//GUI_SendEvent(GUI_GetFocusWindow(),&keyEvent);	
					break;
				case 'C':
					keyEvent.key.scancode = 0xfd25;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'D':
				//	keyEvent.key.sym = KEY_OK;
					keyEvent.key.scancode = 0xfdc7;
					keymap =  find_key(keyEvent.key.scancode);
					//GUI_SendEvent(GUI_GetFocusWindow(),&keyEvent);	
					break;
				case 'E':
					keyEvent.key.scancode = 0xfd2d;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'F':
					keyEvent.key.scancode = 0xfd8d;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'G':
					keyEvent.key.scancode = 0xfdcd;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'H':
					keyEvent.key.scancode = 0xfd4d;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'I':
					keyEvent.key.scancode = 0xfdff;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'J':
					keyEvent.key.scancode = 0xfdd7;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'K':
					keyEvent.key.scancode = 0xfd97;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'L':
					keyEvent.key.scancode = 0xfd17;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'M':
					keyEvent.key.scancode = 0xfd57;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'N':
					keyEvent.key.scancode = 0xfd3f;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'O':
					keyEvent.key.scancode = 0xfdf7;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'P':
					keyEvent.key.scancode = 0xfd87;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				case 'Q':
					keyEvent.key.scancode = 0xfde7;
					keymap =  find_key(keyEvent.key.scancode);
					break;

				case 'R':
					keyEvent.key.scancode = 0xfde5;
					keymap =  find_key(keyEvent.key.scancode);
					break;

				case 'S':
					keyEvent.key.scancode = 0xfda7;
					keymap =  find_key(keyEvent.key.scancode);
					break;

				case 'T':
					keyEvent.key.scancode = 0xfd6d;
					keymap =  find_key(keyEvent.key.scancode);
					break;

				case 'U':
					keyEvent.key.scancode = 0xfd3d;
					keymap =  find_key(keyEvent.key.scancode);
					break;

				case 'V':
					keyEvent.key.scancode = 0xfd9d;
					keymap =  find_key(keyEvent.key.scancode);
					break;
				default :
					break;
			}
			
			Gui_WriteKeyBuf(keymap);
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

	COMDATA_DMX_MUTEX_INIT();
	ABV_STBCA_Enter_Provider_Mode();
	//ser_read_fd = fopen(SERIAL_DEVICE_NAME, "r+");
	ser_read_fd = fopen(/*SERIAL_DEVICE_NAME*/"/dev/ser0", "r+");
	abv_w_stream = fopen(/*SERIAL_DEVICE_NAME*/"/dev/ser0", "r+");
	GxCore_ThreadCreate("com_provider",&com_handle, ComData_Process, NULL, 10 * 1024, GXOS_DEFAULT_PRIORITY);
	bComDataTask_init = TRUE;
	return TRUE;
}





#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_common_porting_ads_flash.h"
#include "app_common_porting_ca_demux.h"
#include "service/gxsearch.h"
#include "app_common_search.h"
#include "cmis_ads_porting.h"
#include "cmis_ads_api.h"
#include "cmis_ads_camail.h"
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#include "app_cd_cas_3.0_api_email.h"

#define READED_EMAIL_CHINISE_MARK	"Î´¶Á"
#define READED_EMAIL_ENGLISH_MARK	"Unreaded"
#endif

#define CMIS_CAMAIL_HEAD				"####"
#define CMIS_CAMAIL_SEPARATOR			'|'

Cmis_mail_info_t g_cmis_ca_mail_info = {0};
handle_t g_cmis_mail_sem = 0;

/**/
bool cmis_ad_camail_parse_content(char * content_str)
{
	char * p = content_str;
	char * pb = NULL;
	char temBuff[100];
	uint8_t separator_index = 0; 
	Cmis_mail_info_t mail_info = {0};
	
	if(p == NULL)
	{
		return FALSE;
	}
	p = strstr(p,CMIS_CAMAIL_HEAD);
	if(p == NULL)
	{
		return FALSE;
	}
	ADS_Dbg("MAIL [%s]\n",content_str);
	p += strlen(CMIS_CAMAIL_HEAD);
	
	pb = temBuff;
	memset(temBuff,0,100);
	while(*p != '\0')
	{
		if( *p == CMIS_CAMAIL_SEPARATOR)
		{
			separator_index++;
			switch (separator_index)
			{
				case 1:
					mail_info.mail_type  = atoi(temBuff);
					break;
				case 2:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						strcpy(mail_info.lock_play_service.service_name,temBuff);
					}
					else
					{
						strcpy(mail_info.text_line_1,temBuff);
					}
					break;
				case 3:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.duation_s = atoi(temBuff);
					}
					else
					{
						strcpy(mail_info.text_line_2,temBuff);
					}
					break;
				case 4:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.freq = atoi(temBuff);
					}
					else
					{
						strcpy(mail_info.text_line_3,temBuff);
					}
					break;
				case 5:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.sym_rate = atoi(temBuff);
					}
					else
					{
						break;
					}
					break;
				case 6:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.qam = atoi(temBuff);
					}
					else
					{
						break;
					}
					break;
				case 7:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.service_id = atoi(temBuff);
					}
					else
					{
						break;
					}

					break;
				case 8:
					if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
					{
						mail_info.lock_play_service.play_start_time.year = (((temBuff[0])-48) * 1000);
						mail_info.lock_play_service.play_start_time.year += (((temBuff[1])-48) * 100);
						mail_info.lock_play_service.play_start_time.year += (((temBuff[2])-48) * 10);
						mail_info.lock_play_service.play_start_time.year += (((temBuff[3])-48) * 1);
						mail_info.lock_play_service.play_start_time.month = (((temBuff[4])-48) * 10);
						mail_info.lock_play_service.play_start_time.month += (((temBuff[5])-48) * 1);
						mail_info.lock_play_service.play_start_time.day = (((temBuff[6])-48) * 10);
						mail_info.lock_play_service.play_start_time.day += (((temBuff[7])-48) * 1);
						mail_info.lock_play_service.play_start_time.hour = (((temBuff[8])-48) * 10);
						mail_info.lock_play_service.play_start_time.hour += (((temBuff[9])-48) * 1);
						mail_info.lock_play_service.play_start_time.minute = (((temBuff[10])-48) * 10);
						mail_info.lock_play_service.play_start_time.minute += (((temBuff[11])-48) * 1);
						mail_info.lock_play_service.play_start_time.second = (((temBuff[12])-48) * 10);
						mail_info.lock_play_service.play_start_time.second += (((temBuff[13])-48) * 1);

					}
					else
					{
						break;
					}

					break;
				default:
					break;
			}
			memset(temBuff,0,100);
			pb = temBuff;
			p++;
		}
		else
		{
			*pb = *p;
			pb++;
			p++;
		}
	}
	if(mail_info.mail_type == CMIS_MAIL_TYPE_LOCK_PLAY)
	{
		mail_info.lock_play_service.mail_id = atoi(temBuff);
	}
	else
	{/*Time out seconds*/
		mail_info.time_out_s = atoi(temBuff);
	}
	//GxCore_SemWait(g_cmis_mail_sem);

	//if(mail_info.mail_type > g_cmis_ca_mail_info.mail_type)
//	{
		memcpy(&g_cmis_ca_mail_info,&mail_info,sizeof(Cmis_mail_info_t));
//	}
//	else
//	{
		/*Drop the low level message*/
	//	GxCore_SemPost(g_cmis_mail_sem);
//		return FALSE;
//	}
	//GxCore_SemPost(g_cmis_mail_sem);
	return TRUE;
}
void cmis_ad_camail_init(void)
{
	GxCore_SemCreate(&g_cmis_mail_sem,1);
	memset(&g_cmis_ca_mail_info,0,sizeof(Cmis_mail_info_t));
}
/**/

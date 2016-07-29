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
#include "app_common_porting_stb_api.h"
#include "app_dvb_cas_api_email.h"
#include "app_common_epg.h"
#include "dvbca_interface.h"

static dvb_cas_email_st ca_email_flag = {0};
static dvb_cas_get_mail dvbca_mail;

extern	void app_dvb_cas_email_flag_img_show(uint8_t showFlag);

int32_t app_dvb_cas_gxmsg_ca_on_event_email(GxMsgProperty0_OnEvent* event)
{
	dvb_cas_email_st* email_flag = NULL;
	if (NULL == event)
		return 0;

	email_flag = (dvb_cas_email_st*)(event->buf);

	if (NULL == email_flag)
		return 0;

	CAS_Dbg("email_flag->wStatus=%d\n",email_flag->wStatus);

	if ((email_flag->wStatus != ca_email_flag.wStatus)&&(TRUE == ca_email_flag.wShowStatus))
	{
		/*关闭之前显示*/
		app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_HIDE);
		ca_email_flag.wStatus = 0;
		ca_email_flag.wShowStatus = 0;
		ca_email_flag.uCount = 0;
	}
	//ca_email_flag.wStatus = email_flag->wStatus;
	app_dvb_cas_api_init_email_data();

	return 1;
}

/*
* 反授权图标显示、隐藏
*/
int32_t app_dvb_cas_email_flag_exec(void)
{
	uint8_t detitle_status = ca_email_flag.wStatus;
	char* curFocusWin = (char *)GUI_GetFocusWindow();

	if ((NULL != curFocusWin) && (strcasecmp(curFocusWin, "win_full_screen") != 0))
	{
		return TRUE;
	}
	else
	{
		app_dvb_cas_email_flag_check();
	}

	if ( DVB_CA_EMAIL_NEW == detitle_status)
	{
		/* 收到新的反授权码，显示反授权码图标 */
		if (FALSE == ca_email_flag.wShowStatus)
		{
			app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_SHOW);
		}
		ca_email_flag.wShowStatus = TRUE;
	}
	else
		if ( DVB_CA_EMAIL_EXHAUST == detitle_status)
		{
			ca_email_flag.uCount++;
			ca_email_flag.uCount = ca_email_flag.uCount%3;

			/* 邮件空间不足，改变图标状态提示用户 */

			if (0 != ca_email_flag.uCount)
			{
				app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_SHOW);
				ca_email_flag.wShowStatus = TRUE;
			}
			else
			{
				app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_TRANS_HIDE);
				ca_email_flag.wShowStatus = FALSE;
			}
		}
		else
			if (DVB_CA_EMAIL_FLAG_HIDE == detitle_status)
			{
				/* 要求隐藏邮件图标 */
				if (TRUE == ca_email_flag.wShowStatus)
				{
					app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_HIDE);
				}
				ca_email_flag.wShowStatus = FALSE;
			}
	return TRUE;

}

void app_dvb_cas_email_flag_check(void)
{
	int i;
	for(i=0; i<dvbca_mail.get_num;i++ )
	{
		if(dvbca_mail.mail_list[i].ucNewEmail == TRUE)
			break;
	}
	if(i < dvbca_mail.get_num)
	{
		ca_email_flag.wStatus = DVB_CA_EMAIL_NEW;
		ca_email_flag.wShowStatus = FALSE;
	}
	else if(dvbca_mail.get_num < DVBCA_MAX_MAIL)
	{
		ca_email_flag.wStatus = DVB_CA_EMAIL_FLAG_HIDE;
	}
	else
	{
		ca_email_flag.wStatus = DVB_CA_EMAIL_EXHAUST;
	}
}

void app_dvb_cas_email_flag_hide(void)
{
	ca_email_flag.wStatus = DVB_CA_EMAIL_FLAG_HIDE;
	app_dvb_cas_email_flag_img_show(DVB_CA_EMAIL_PROMTP_TRANS_HIDE);
}

int32_t app_dvb_cas_api_init_email_data(void)
{
	memset(&dvbca_mail,0,sizeof(dvb_cas_get_mail));
	dvbca_mail.start_pos = 0;
	dvbca_mail.get_num = DVBCASTB_GetMailNum(CA_DB_MAIL_TOTAL_NUM);
	dvbca_mail.get_num =  DVBCASTB_GetMail(dvbca_mail.mail_list,dvbca_mail.start_pos,dvbca_mail.get_num);
	return 0;
}

char * app_dvb_cas_api_get_email_data(ca_get_date_t *data)
{
	uint32_t pos = 0 ;
	uint32_t ID = 0;
	char* osd_language=NULL;

	static char   cTextBuffer_flag[10] ={0};
	//static char   cTextBuffer_title[12] ={0};
	//static char   cTextBuffer_sender[24] ={0};

	if (NULL == data)
		return NULL;

	pos = data->pos;
	ID = data->ID;

	switch(ID)
	{
		case DVB_EMAIL_FLAG_ID:
			osd_language = app_flash_get_config_osd_language();
			memset(cTextBuffer_flag,0,sizeof(cTextBuffer_flag));
			if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
			{
				if (TRUE == dvbca_mail.mail_list[pos].ucNewEmail)
					sprintf(cTextBuffer_flag,"新邮件");
				else
					sprintf(cTextBuffer_flag,"已阅读");
			}
			else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
			{
				if (TRUE == dvbca_mail.mail_list[pos].ucNewEmail)
					sprintf(cTextBuffer_flag,"New");
				else
					sprintf(cTextBuffer_flag,"Readed");
			}
			return cTextBuffer_flag;
		case DVB_EMAIL_TITLE_ID:
			//memset(cTextBuffer_title,0,sizeof(cTextBuffer_title));
			//sprintf(cTextBuffer_title,"%s",dvbca_mail.mail_list[pos].aucTitle);
			//return cTextBuffer_title;
			return dvbca_mail.mail_list[pos].aucTitle;
		case DVB_EMAIL_SENDTIME_ID:
			//memset(cTextBuffer_sender,0,sizeof(cTextBuffer_sender));
			//sprintf(cTextBuffer_sender,"%s",dvbca_mail.mail_list[pos].aucTerm);
			//return cTextBuffer_sender;
			return dvbca_mail.mail_list[pos].aucTerm;
		case DVB_EMAIL_VALIDTIME_ID:
			break;
		case DVB_EMAIL_CONTENT_ID:
			DVBCASTB_SetMailRead(dvbca_mail.mail_list[pos].usEmailID);
			return (char*)dvbca_mail.mail_list[pos].aucContext;
		default:
			break;
	}
	return NULL;

}

uint8_t app_dvb_cas_api_get_email_count(ca_get_count_t *data)
{
	data->totalnum = DVBCASTB_GetMailNum(CA_DB_MAIL_TOTAL_NUM);
	data->newnum   = DVBCASTB_GetMailNum(CA_DB_MAIL_NOT_READ_NUM);
	CAS_Dbg("data->totalnum=%d, data->newnum=%d\n",data->totalnum,data->newnum);
	return data->totalnum;
}

bool app_dvb_cas_api_delete_email(ca_delete_data_t *data)
{
	uint32_t pos = 0 ;
	if (NULL == data)
		return FALSE;

	pos = data->pos;
	DVBCASTB_DelMail(dvbca_mail.mail_list[pos].usEmailID);
	return 0;
}

bool app_dvb_cas_api_delete_all_email(ca_delete_data_t *data)
{
	int i;
	if (NULL == data)
		return FALSE;

	for(i=0; i<dvbca_mail.get_num;i++)
	{
		DVBCASTB_DelMail(dvbca_mail.mail_list[i].usEmailID);
	}
	return 0;
}



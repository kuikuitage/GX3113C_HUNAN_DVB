/**
 *
 * @file        app_common_epg.c
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 15:17:49 PM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include "gxapp_sys_config.h"
#include "app_common_epg.h"
#include "app_common_flash.h"
#include "app_common_play.h"
#include "gxepg.h"
#include "gxmsg.h"
#include "gxprogram_manage_berkeley.h"


static epg_get_para epg_para={0};
static char* s_event_info =NULL;
/*
* 获取EPG参数信息
*/
epg_get_para* app_epg_get_para(void)
{
	return &epg_para;
}

/*
* 复位EPG参数信息
*/
void app_epg_reset_get_para(void)
{
	uint8_t epg_enable = 0;
	uint8_t detail_flag = 0;
	epg_enable = epg_para.gApp_epg_enable;
	detail_flag = epg_para.detail_flag;
	app_epg_free();
	memset(&epg_para,0,sizeof(epg_get_para));
	epg_para.gApp_epg_enable = epg_enable;
	epg_para.detail_flag = detail_flag;
	
	return ;
}

/*
* 设置系统时间
*/
void app_epg_set_local_time(struct tm *pTime,uint32_t timezoneFlag)
{
	GxTime sys_time;
	uint32_t seconds;
	int32_t config = 0;
	struct tm time;

	if (NULL == pTime)
		return;

	memcpy(&time ,pTime , sizeof(struct tm));

	if(time.tm_year <1900 ||time.tm_mon > 12)
	{
		return;
	}
	time.tm_mon--;
	time.tm_year -= 1900;

	seconds = (uint32_t)mktime(&time);

	if (TRUE == timezoneFlag)
		{
			config = app_flash_get_config_timezone();
			seconds -= config*3600;		
		}

	sys_time.seconds = seconds;
	sys_time.microsecs = 0;
	GxCore_SetLocalTime(&sys_time);
}

/*
* 获取当前天之后day天的年/月/日/星期等信息
*/
void app_epg_get_time_by_day_offset(struct tm *pLocalTime,int day,uint32_t timezoneFlag)
{
	GxTime time;
	int32_t config = 0;
	int32_t second;

	if (NULL == pLocalTime)
		return;

	GxCore_GetLocalTime(&time);
	if (TRUE == timezoneFlag)
		{
			config = app_flash_get_config_timezone();
			second = config*3600+time.seconds;		
		}
	second += day*24*3600;
	memset(pLocalTime,0,sizeof(struct tm));
	memcpy((void*)pLocalTime, localtime((const time_t*) &second), sizeof(struct tm));
	pLocalTime->tm_year += 1900;
	pLocalTime->tm_mon ++;
}

/*
* 获取当前时间信息
*/
void app_epg_get_local_time(struct tm *pLocalTime,uint32_t timezoneFlag)
{
	GxTime time;
	int32_t config = 0;
	int32_t second;

	if (NULL == pLocalTime)
		return;


	GxCore_GetLocalTime(&time);
	if (TRUE == timezoneFlag)
	{
		config = app_flash_get_config_timezone();
		second = config*3600+time.seconds;			
	}
	memset(pLocalTime,0,sizeof(struct tm));
	memcpy((void*)pLocalTime, localtime((const time_t*) &second), sizeof(struct tm));
	pLocalTime->tm_year += 1900;
	pLocalTime->tm_mon ++;
}

/*
* 开启epg过滤
*/
void app_epg_open(void)
{
	GxMsgProperty_EpgCreate  cfg_params = {0, };

	cfg_params.service_count = 200;
//	cfg_params.service_count = 400;
//	cfg_params.event_count = 32000;
	cfg_params.event_count = 38400;
	cfg_params.event_size = 80;
//	cfg_params.event_size = 300;
	epg_para.detail_flag = 0;
#ifdef DVB_32MB
	cfg_params.event_count = 10;
	cfg_params.event_size = 80;

#endif
	cfg_params.epg_day = 7;
	cfg_params.ts_src = DVB_TS_SRC;
	cfg_params.cur_tp_only = 1;
	strcpy((char*)cfg_params.language, "all");
#ifndef DVB_32MB
	if (NULL == s_event_info)
		{
			s_event_info = GxCore_Malloc(60*1024);
			if (NULL == s_event_info)
				{
					printf("%s %d failed\n",__FILE__,__LINE__);
				}
		}
#endif		
	//cfg_params.txt_format = EPG_TXT_UTF8;
	if(epg_para.gApp_epg_enable == 0)
	{
		app_send_msg(GXMSG_EPG_CREATE,(void*)&cfg_params);
		epg_para.gApp_epg_enable = 1;

		/*
		* 开启epg过滤同时,开启tdt同步
		*/
//		app_epg_sync_time();		
	}
}

/*
* 关闭epg过滤
*/
void app_epg_close(void)
{
	if(epg_para.gApp_epg_enable == 1)
	{
		app_send_msg(GXMSG_EPG_RELEASE,NULL);
		epg_para.gApp_epg_enable = 0;
	}
}

/*
* 退出显示EPG信息界面 (如信息条、节目列表、EPG)
* 等界面中调用
*/
void app_epg_free(void)
{
	if(epg_para.msg_get_epg_num_id>0)
	{
		app_free_msg(GXMSG_EPG_NUM_GET,epg_para.msg_get_epg_num_id);
		epg_para.msg_get_epg_num_id = 0;		
	}

	if(epg_para.msg_get_epg_id>0)
	{
		app_free_msg(GXMSG_EPG_GET,epg_para.msg_get_epg_id);
		epg_para.msg_get_epg_id = 0;
	}
}

/*
* 获取p/f epg信息
*/
GxEpgInfo * app_epg_get_pf_event(uint32_t pos)
{
	GxBusPmDataProg  prog;
	GxMsgProperty_EpgGet epg_params ;
	GxMsgProperty_EpgNumGet epg_num_get;
	app_msg* msg_temp;
	app_msg* msg1_temp;
	app_epg_free();
	GxBus_PmProgGetByPos(pos,1,&prog);
	epg_num_get.service_id = prog.service_id;
	epg_num_get.ts_id = prog.ts_id;
	epg_num_get.orig_network_id = prog.original_id;
	if(epg_para.gApp_epg_enable == 0)
	{
		return NULL;
	}
	epg_para.msg_get_epg_num_id = app_send_msg(GXMSG_EPG_NUM_GET,(void*)&epg_num_get);
	if(epg_para.msg_get_epg_num_id<=0)
	{
		return NULL;
	}
	
	msg1_temp = app_get_msg(epg_para.msg_get_epg_num_id);
	if(((GxMsgProperty_EpgNumGet*)msg1_temp->param)->event_per_day[0] == 0)
	{
		app_free_msg(GXMSG_EPG_NUM_GET,epg_para.msg_get_epg_num_id);
		epg_para.msg_get_epg_num_id = 0;
		return NULL;
	}
	
//	epg_params.orig_network_id = prog.original_id;
	epg_params.service_id = prog.service_id;
	epg_params.ts_id = prog.ts_id;
	epg_params.get_event_pos = 0;
	epg_params.want_event_count = 2;
	epg_params.epg_info_size = 15*1024;
	epg_params.epg_info = (GxEpgInfo*)s_event_info;

	
	
    //等待消息被执行结束
	epg_para.msg_get_epg_id = app_send_msg(GXMSG_EPG_GET,(void*)&epg_params);
	if(epg_para.msg_get_epg_id<=0)
	{
		app_free_msg(GXMSG_EPG_GET,epg_para.msg_get_epg_num_id);
		epg_para.msg_get_epg_num_id = 0;
		return NULL;
	}

	msg_temp = app_get_msg(epg_para.msg_get_epg_id);
	if(((GxMsgProperty_EpgGet*)msg_temp->param)->get_event_count == 0)
	{
		app_free_msg(GXMSG_EPG_GET,epg_para.msg_get_epg_id);
		epg_para.msg_get_epg_id = 0;
		return NULL;
	}

	return &(((GxMsgProperty_EpgGet*)msg_temp->param)->epg_info[0]);

}

/*
* 根据当前时间查找第一条有效EPG信息
* 可配置是否保留当前时间之前的EPG信息
* 如当前时间13:43,保留08:00EPG信息
*/
static void app_epg_rebuild_valid_event(uint32_t checkflag)
{
	uint32_t i = 0, event_num = 0;
	app_msg* msg0_temp;
	app_msg* msg1_temp;
	GxEpgInfo *epg_info_temp = NULL;
	GxTime time;

	if(epg_para.sEpg_day!=0)
	{
		epg_para.sOffSetSel = 0;
		return;
	}

	if (FALSE == checkflag)
		{
			/*
			* 显示读取到的所有EPG信息，不删除过期EPG
			*/
			epg_para.sOffSetSel = 0;
			return ;
		}
	
	msg0_temp = app_get_msg(epg_para.msg_get_epg_id);
	msg1_temp = app_get_msg(epg_para.msg_get_epg_num_id);
	if(((GxMsgProperty_EpgNumGet*)msg1_temp->param)->event_per_day[epg_para.sEpg_day+1] == 0)
	{
		return ;
	}	
	event_num = ((GxMsgProperty_EpgNumGet*)msg1_temp->param)->event_per_day[epg_para.sEpg_day+1];
	GxCore_GetLocalTime(&time);
	for(i = 0; i<event_num; i++)
	{
		epg_info_temp = &(((GxMsgProperty_EpgGet*)msg0_temp->param)->epg_info[i]);
		if((epg_info_temp->start_time<time.seconds)&&(epg_info_temp->start_time + epg_info_temp->duration>=time.seconds))
		{
			/*
			* 开始时间小于当前时间，结束时间大于当前时间
			* 找到的第一个有效EPG
			*/
			
			epg_para.sOffSetSel = i;
			break;
		}
		else if(epg_info_temp->start_time>time.seconds)
		{
			/*
			* 开始时间大于当前时间
			* 找到的第一个有效EPG
			*/
			epg_para.sOffSetSel = (i>0)?(i-1):0;
			printf("time.seconds i=%d\n",epg_para.sOffSetSel);
			break;
		}		
	}
	if(i == event_num)
	{
		/*
		* 所有EPG无效，EPG个数为0
		*/
		epg_para.sOffSetSel = event_num;
		printf("time.seconds i=%d\n",epg_para.sOffSetSel);
	}
	
}

/*
* 更新选中节目某天的EPG信息(返回当前选中行EPG)
*/
GxEpgInfo * app_epg_update_event_by_day(uint32_t value,int32_t nListSel,uint32_t checkflag)
{
	uint32_t offset ,i;
	GxBusPmDataProg prog;
	GxMsgProperty_EpgNumGet epg_num_get; 
	GxMsgProperty_EpgGet epg_params ;
	app_msg* msg_temp;
	app_msg* msg1_temp;
	
	GxEpgInfo *epg_info = NULL;
	app_epg_free();
	GxBus_PmProgGetByPos(value,1,&prog);

	epg_num_get.service_id = prog.service_id;
	epg_num_get.ts_id = prog.ts_id;
	epg_num_get.orig_network_id = prog.original_id;
	if(epg_para.gApp_epg_enable == 0)
	{
		return NULL;
	}
	epg_para.msg_get_epg_num_id = app_send_msg(GXMSG_EPG_NUM_GET,(void*)&epg_num_get);
	if(epg_para.msg_get_epg_num_id>0)
	{
		msg_temp = app_get_msg(epg_para.msg_get_epg_num_id);

		if(((GxMsgProperty_EpgNumGet*)msg_temp->param)->event_per_day[epg_para.sEpg_day+1])
		{
			offset = 0;
			for(i=0;i<epg_para.sEpg_day+1;i++)
			{
				offset += ((GxMsgProperty_EpgNumGet*)msg_temp->param)->event_per_day[i];
			}
			epg_params.want_event_count =((GxMsgProperty_EpgNumGet*)msg_temp->param)->event_per_day[epg_para.sEpg_day+1];
//			epg_params.orig_network_id = prog.original_id;
			epg_para.get_event_count = epg_params.want_event_count;
			epg_params.service_id = prog.service_id;
			epg_params.ts_id = prog.ts_id;
			epg_params.get_event_pos =offset;
			epg_params.epg_info_size = 60*1024;
			epg_params.epg_info = (GxEpgInfo*)s_event_info;
			epg_para.msg_get_epg_id = app_send_msg(GXMSG_EPG_GET,(void*)&epg_params);
			if(epg_para.msg_get_epg_id>0)
			{
				msg1_temp = app_get_msg(epg_para.msg_get_epg_id);
				#if 0
				for(i=0; i<((GxMsgProperty_EpgGet*)msg1_temp->param)->get_event_count; i++)
				{
				
					//No. hour:min:sec - hour:min:sec	  event_name
					printf("  %s\n", ((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[i].event_name);
					//printf("	%s\n\n", ((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[i].event_detail);
				}
				#endif

				if(epg_para.sEpg_day == 0)
				{
					app_epg_rebuild_valid_event(checkflag);
				}
				
				if(-1 != nListSel)
				{
					/*
					* EPG列表选中，获取当前选中EPG
					*/
					if(epg_para.sEpg_day == 0)
					{
						epg_info = &(((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[nListSel+epg_para.sOffSetSel]);
					}
					else
					{
						epg_info = &(((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[nListSel]);
					}
				}
				else
				{
					/*
					* EPG列表未选中，默认取第一条
					*/
					if(epg_para.sEpg_day == 0)
					{
						epg_info = &(((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[epg_para.sOffSetSel]);
					}
					else
					{
						epg_info = &(((GxMsgProperty_EpgGet*)msg1_temp->param)->epg_info[0]);
					}
				}
			}
		}

	}


	return epg_info;		

}

/*
* 获取某天对应索引的EPG信息
*/
GxEpgInfo *  app_epg_get_event_info_by_sel(uint32_t nListSel)
{
	app_msg* msg0_temp;
	app_msg* msg1_temp;
	GxEpgInfo *epg_info = NULL;
	 uint32_t event_count ;


	if(epg_para.msg_get_epg_id<=0)
	{
		return NULL;
	}

	msg0_temp = app_get_msg(epg_para.msg_get_epg_id);
	msg1_temp = app_get_msg(epg_para.msg_get_epg_num_id);
	event_count = ((GxMsgProperty_EpgNumGet*)msg1_temp->param)->event_per_day[epg_para.sEpg_day+1];
	if(event_count == 0)
	{
		return NULL;
	}

	if(epg_para.sEpg_day == 0)
	{
		event_count -=  epg_para.sOffSetSel;
		if (nListSel >= event_count)
			{
				return NULL;
			}
		
		epg_info = &(((GxMsgProperty_EpgGet*)msg0_temp->param)->epg_info[nListSel+epg_para.sOffSetSel]);
	}
	else
	{
		if (nListSel >= event_count)
			{
				return NULL;
			}
		epg_info = &(((GxMsgProperty_EpgGet*)msg0_temp->param)->epg_info[nListSel]);
	}
	return epg_info;
}

uint32_t app_epg_get_event_count(void)
{
	app_msg* msg_temp;
	if(epg_para.msg_get_epg_id<=0 || epg_para.msg_get_epg_num_id<=0)
	{
		epg_para.cur_event_count = 0;
		return epg_para.cur_event_count;
	}

	msg_temp = app_get_msg(epg_para.msg_get_epg_num_id);
	printf("epg_para.sEpg_day=%d\n",epg_para.sEpg_day);
	epg_para.cur_event_count = ((GxMsgProperty_EpgNumGet*)msg_temp->param)->event_per_day[epg_para.sEpg_day+1];

	if(epg_para.sEpg_day == 0)
	{
		epg_para.cur_event_count -=epg_para.sOffSetSel;
	}
	return epg_para.cur_event_count;
}

uint32_t app_epg_switch_day_offset(void)
{
	if(epg_para.sEpg_day < 6)
	{
		epg_para.sEpg_day++;
	}
	else
	{
		epg_para.sEpg_day = 0;
	}
	return epg_para.sEpg_day;
}

uint32_t app_jy_epg_switch_day_offset(int flag)
{
	if(flag==0)
	{
		if(epg_para.sEpg_day < 6)
		{
			epg_para.sEpg_day++;
		}
		else
		{
			epg_para.sEpg_day = 0;
		}
	}
	else if(flag==1)
	{
		if(epg_para.sEpg_day >0)
		{
			epg_para.sEpg_day--;
		}
		else
		{
			epg_para.sEpg_day = 6;
		}
	}		
	return epg_para.sEpg_day;
}

uint32_t app_jy_epg_get_day_offset(void)
{
	return epg_para.sEpg_day;
}

/*
* 同步更新当前时间
*/

void app_epg_sync_time(void)
{
	app_send_msg(GXMSG_EXTRA_SYNC_TIME, NULL);
	return ;
}

void app_epg_get_event_time(GxEpgInfo *epg_info_temp,struct tm *tm_start,struct tm *tm_end)
{
	int32_t TimeZone;
	time_t time_end;
	time_t time_start;

	TimeZone = app_flash_get_config_timezone();

	if (NULL == epg_info_temp)
		return;

	if (NULL == tm_start)
		return ;

	if (NULL == tm_end)
		return ;

	time_end = epg_info_temp->start_time + epg_info_temp->duration + TimeZone*3600;
	time_start = epg_info_temp->start_time+TimeZone*3600;
	memcpy(tm_start, localtime(&time_start), sizeof(struct tm));
	memcpy(tm_end, localtime(&time_end), sizeof(struct tm));
	
	return;
}


void  app_epg_convert_mjd_to_ymd( uint16_t   mjd, uint16_t   *year, uint8_t  *month, uint8_t  *day, uint8_t  *weekDay )
{
	uint32_t  uMjd;
	uint32_t  y, m, k;

	if ((NULL == year)||(NULL == month)||(NULL == day)||(NULL == weekDay))
		return;


	uMjd = (uint32_t)mjd;

	if( uMjd < 15079 ) {										/* MJD Lower than 1900/3/1 ? */
		uMjd += 0x10000;										/* Adjust MJD */
	}

	y = ( uMjd * 100 - 1507820 ) / 36525;					/* Calculate Y', M' */
	m = ( uMjd * 10000 - 149561000 - ( ( y * 36525 ) / 100 ) * 10000 ) / 306001;

	*day = (uint8_t)( uMjd - 14956 - ( ( y * 36525 ) / 100 ) - ( ( m * 306001 ) / 10000 ) );
	/* Calculate Day */

	k = ( ( m == 14 ) || ( m == 15 ) ) ? 1 : 0;					/* If M'=14 or M'=15 then K=1 else K=0 */

	*year = (uint16_t )( y + k ) + 1900;								/* Calculate Year */
	*month = (uint8_t)( m - 1 - k * 12 );							/* Calculate Month */

	*weekDay = (uint8_t)( ( ( uMjd + 2 ) % 7 ) + 1 );				/* Calculate Week Day */
	if (*weekDay >= 7)
	{
		*weekDay = *weekDay % 7;
	}
}

void  app_epg_convert_ymd_to_mjd( uint16_t   *mjd, uint16_t   year, uint8_t  month, uint8_t  day)
{
	uint32_t  uMjd;
	uint32_t  l;

	if (NULL == mjd)
		return;

	if( month == 1 || month ==2)
	{
		l = 1;
	}
	else
	{
		l = 0;
	}

	uMjd = 14956 + day + (uint32_t)((year - l)*36525/100) + (uint32_t)((month + 1 + l*12) * 306001/10000);
	*mjd = (uint16_t )uMjd;
}

void app_epg_convert_to_utc( unsigned short wDate, com_time_t* pTimeOut )
{
    unsigned short mjd =0;
    unsigned short wYear               = 2000-1900;
    unsigned char  chMonth             = 1;
    unsigned char  chDay               = 1;
    unsigned char  weekDay               = 1;

    if (NULL == pTimeOut)
		return;
    app_epg_convert_ymd_to_mjd(&mjd,wYear,chMonth,chDay);
    mjd = mjd + wDate;
    app_epg_convert_mjd_to_ymd(mjd,&wYear,&chMonth,&chDay,&weekDay);
    pTimeOut->year = wYear;
    pTimeOut->month = chMonth;
    pTimeOut->day = chDay;
}



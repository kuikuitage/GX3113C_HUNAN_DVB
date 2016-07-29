/**
 *
 * @file        app_common_prog.c
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 15:53:49 PM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
 #include <gxtype.h>
#include "app_common_prog.h"
#include "gxapp_sys_config.h"
#include "app_common_book.h"
#include "app_common_epg.h"
#include "app_common_init.h"
#include "app_common_play.h"
#include "app_common_flash.h"
#include "app_common_table_nit.h"
#include "app_common_table_pmt.h"
#include "app_common_table_bat.h"
#include <sys/ioctl.h>
#include "app_common_porting_stb_api.h"
#include "app_common_panel.h"
#include "gxmsg.h"
#include "app_common_search.h"
#include "gui_core.h"
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
#include "app_by_cas_api_demux.h"
#endif
#endif

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_MG_FLAG
extern char*  MGCA_Get_Config_Serial(void);
extern int32_t MGCA_Save_Config_Serial(const char* mgca_serial);
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
extern char*  MGCA312_Get_Config_Serial(void);
extern int32_t MGCA312_Save_Config_Serial(const char* mgca_serial);
#endif
#endif





static userlist_t userlist = {0}; //私有索引表
static play_prog gApp_old_prog;
static play_prog gApp_record_prog;
static uint32_t gApp_num_in_group;
static App_proedit_arr* pProedit_arr=NULL;
static handle_t   play_para_lock=0;
static handle_t   prog_mutex=0;


void app_prog_play_para_lock(void)
{
	GxCore_MutexLock(play_para_lock);
	return;	
}

void app_prog_play_para_unlock(void)
{
	GxCore_MutexUnlock(play_para_lock);
	return;	
}

void app_prog_mutex_lock(void)
{
	GxCore_MutexLock(prog_mutex);
	return;
}

void app_prog_mutex_unlock(void)
{
	GxCore_MutexUnlock(prog_mutex);
	return;
}


void app_prog_init_playing(void)
{
	GxCore_MutexCreate(&play_para_lock);
	GxCore_MutexCreate(&prog_mutex);
	memset(&gApp_record_prog,0,sizeof(play_prog));
	gApp_record_prog.id = 0xffffffff;
	memset(&gApp_old_prog,0,sizeof(play_prog));
	gApp_old_prog.id = 0xffffffff;
}

/*
* 设置排序方式
* SortType -- 排序方式
* SortOrder -- 顺序、倒序
*/
uint32_t app_prog_set_sort_mode(prog_sort_type_e SortType, prog_sort_order_e SortOrder)
{
	GxBusPmViewInfo sys;

	GxBus_PmViewInfoGet(&sys);
	switch(SortType)
	{
		case PROG_SORT_DEFAULT:
			sys.taxis_mode = TAXIS_MODE_NON;
			break;
		case PROG_SORT_SERVICE_ID:
			sys.taxis_mode = TAXIS_MODE_SERVICE_ID;
			break;
		case PROG_SORT_PROG_NAME:
			sys.taxis_mode = TAXIS_MODE_LETTER;
			memset(sys.letter,0,MAX_CMP_NAME);
			break;
		case PROG_SORT_FREE_CA:
			sys.taxis_mode = TAXIS_MODE_SCRAMBLE;
			break;
		default:
			break;
	}
	switch(SortOrder)
	{
		case PROG_SORT_POSITIVE:
			sys.order = VIEW_INFO_ORDER;
			break;
		case PROG_SORT_REVERSE:
			sys.order = VIEW_INFO_REVERSE;
			break;
		default:
			break;
	}

	GxBus_PmViewInfoModify(&sys);
	return 0;
}

void app_prog_userlist_init(void)
{
	uint32_t i;//,ret = 0;
	uint32_t program_num = 0;
	GxBusPmDataProg prog;

	if (0 == app_flash_get_user_list_flag())
		return;


	if(userlist.id_arry!=NULL)
	{
		GxCore_Free(userlist.id_arry);
		userlist.id_arry = NULL;
	}
	app_prog_update_num_in_group();
	program_num = app_prog_get_num_in_group();
	userlist.count = program_num;
	
	if (0 != program_num)
		{
			userlist.id_arry = (uint32_t *) GxCore_Malloc(4*program_num);
			if (NULL == userlist.id_arry)
				{
					return;
				}
			memset(userlist.id_arry,0,program_num*4);		
		}

	for(i=0;i<program_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,&prog);
		userlist.id_arry[i] = prog.id;
	}
}

void app_prog_userlist_del(void)
{
	if (0 == app_flash_get_user_list_flag())
		return;
	
	GxBus_PmProgUserListDel();
}

void app_prog_userlist_resume(void)
{
	uint32_t ret = 0;
	uint32_t num = 0;
	
	if (0 == app_flash_get_user_list_flag())
		return;

	
	num = userlist.count;
	ret = GxBus_PmProgUserListSave(userlist.id_arry,num);
	if(ret !=0)
	{
		printf("---------------user list error------------------");
	}
}

void app_prog_userlist_del_program(uint32_t* id_buf, uint32_t num)
{
	uint32_t *buffer;
	uint32_t count = 0, i = 0, j = 0, ret = 0;

	if (NULL == id_buf)
		return;
	
	if (0 == app_flash_get_user_list_flag())
		return;


	if(userlist.count == 0|| userlist.id_arry == NULL)
	{
		return;
	}
	if(userlist.count<=num)
	{
		return;
	}
	count = 0;
	buffer = (uint32_t *) GxCore_Malloc(4*(userlist.count-num));
	if (NULL == buffer)
		return;
	
	for(i = 0; i<userlist.count; i++)
	{
		for(j = 0; j<num; j++)
		{
			if(userlist.id_arry[i] == id_buf[j])
			{
				break;
			}
		}
		if(j == num)
		{
			if(count<userlist.count-num)
			{
				buffer[count] = userlist.id_arry[i];
				count++;
			}
			else
			{
				printf("---------------user list delete error------------------");
			}
		}
	}
	GxCore_Free(userlist.id_arry);
	userlist.id_arry = NULL;
	userlist.id_arry = (uint32_t *) GxCore_Malloc(4*(userlist.count-num));
	if (NULL == userlist.id_arry)
		return;
	
	userlist.count = userlist.count-num;
	memcpy(userlist.id_arry,buffer,4*userlist.count);
	ret = GxBus_PmProgUserListSave(userlist.id_arry,userlist.count);
	if(ret !=0)
	{
		printf("---------------user list error------------------");
	}
	GxCore_Free(buffer);
	buffer = NULL;
}

void app_prog_userlist_save(uint32_t* pos, uint32_t num)
{
	uint32_t i,ret = 0;
//	uint32_t program_num = 0;
	GxBusPmDataProg prog;

	if (0 == app_flash_get_user_list_flag())
		return;


	if(pos == NULL|| num == 0)
	{
		return;
	}
	if(userlist.id_arry == NULL)
	{
		userlist.count = num;
		userlist.id_arry = (uint32_t *) GxCore_Malloc(4*num);
		if (NULL == userlist.id_arry)
			return;
	}
	else if(userlist.count != num)
	{
		GxCore_Free(userlist.id_arry);
		userlist.id_arry = NULL;
		userlist.count = num;
		userlist.id_arry = (uint32_t *) GxCore_Malloc(4*num);
		if (NULL == userlist.id_arry)
			return;
	}
	for(i=0;i<num;i++)
	{
		GxBus_PmProgGetByPos(pos[i],1,&prog);
		userlist.id_arry[i] = prog.id;
	}
	
	ret = GxBus_PmProgUserListSave(userlist.id_arry,num);
	if(ret !=0)
	{
		printf("---------------user list error------------------");
	}
}

/*
* 保存当前播放节目到系统参数中
*/
int32_t app_prog_save_playing_pos_in_group(uint32_t pos)
{
	GxBusPmViewInfo sysinfo;
	GxBusPmDataProg prog_data = {0};
	int ret = 0;

	if (0 == app_prog_get_num_in_group())
		return -1;
	
	ret = GxBus_PmProgGetByPos(pos,1,&prog_data);
	if(ret != -1 )
	{
		ret = GxBus_PmViewInfoGet(&sysinfo);
		if(-1 != ret)
			{
				if(sysinfo.stream_type == GXBUS_PM_PROG_TV)
				{
					if (sysinfo.tv_prog_cur != prog_data.id)
						{
							sysinfo.tv_prog_cur = prog_data.id;
							GxBus_PmViewInfoModify(&sysinfo);						
						}
				}
#if (1 == DVB_HD_LIST)
				else if (sysinfo.stream_type == GXBUS_PM_PROG_HD_SERVICE)
					{
						if (sysinfo.hd_prog_cur != prog_data.id)
							{
								sysinfo.hd_prog_cur = prog_data.id;
								GxBus_PmViewInfoModify(&sysinfo);						
							}						
					}
#endif
				else
				{
					if (sysinfo.radio_prog_cur != prog_data.id)
						{
							sysinfo.radio_prog_cur = prog_data.id;
							GxBus_PmViewInfoModify(&sysinfo);						
						}

				}
				app_prog_record_playing_pos(pos);		
			}
		else
			{
				printf("%s %d GxBus_PmViewInfoGet error\n",__FILE__,__LINE__);
				return -1;
			}

	}
	else
	{
		printf("%s %d GxBus_PmProgGetByPos error\n",__FILE__,__LINE__);
		return -1;
	}

	return 0;
}

void app_prog_record_playing_pos(uint32_t pos_in_group)
{
	GxBusPmViewInfo pSysInfo;
	GxBus_PmViewInfoGet(&pSysInfo);
#if (1 == DVB_HD_LIST)
	if((pSysInfo.stream_type != GXBUS_PM_PROG_TV) && (pSysInfo.stream_type != GXBUS_PM_PROG_RADIO)
		 && (pSysInfo.stream_type != GXBUS_PM_PROG_HD_SERVICE))
	{
		return;
	}
#else
	if((pSysInfo.stream_type != GXBUS_PM_PROG_TV) && (pSysInfo.stream_type != GXBUS_PM_PROG_RADIO))
	{
		return;
	}
#endif

	if (0xffffffff == gApp_record_prog.id)
	{
		/*
		* record startup prog
		*/
		memcpy(&(gApp_record_prog.SysInfo), &pSysInfo,sizeof(GxBusPmViewInfo));
	}

	if (GXBUS_PM_PROG_TV == pSysInfo.stream_type)
		{
			gApp_record_prog.id = pSysInfo.tv_prog_cur;
		}
#if (1 == DVB_HD_LIST)
	else 
		if ( GXBUS_PM_PROG_HD_SERVICE == pSysInfo.stream_type  )
			{
				gApp_record_prog.id = pSysInfo.hd_prog_cur;
			}
#endif
	else
		{
			gApp_record_prog.id = pSysInfo.radio_prog_cur;
		}

	if(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_TV
			&&pSysInfo.stream_type == GXBUS_PM_PROG_TV
			&&gApp_record_prog.SysInfo.tv_prog_cur == pSysInfo.tv_prog_cur)
	{
		return;
	}
#if (1 == DVB_HD_LIST)
	if(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_HD_SERVICE
			&&pSysInfo.stream_type == GXBUS_PM_PROG_HD_SERVICE
			&&gApp_record_prog.SysInfo.hd_prog_cur == pSysInfo.hd_prog_cur)
	{
		return;
	}
#endif

	if(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_RADIO
			&&pSysInfo.stream_type == GXBUS_PM_PROG_RADIO
			&&gApp_record_prog.SysInfo.radio_prog_cur == pSysInfo.radio_prog_cur)
	{
		return;
	}

#if (1 == DVB_HD_LIST)
	if((gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_RADIO ||(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_HD_SERVICE))
			&&(pSysInfo.stream_type == GXBUS_PM_PROG_TV) )   //radio->tv
#else
	if(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_RADIO
			&&pSysInfo.stream_type == GXBUS_PM_PROG_TV)   //radio->tv
#endif
	{
		gApp_record_prog.id = pSysInfo.tv_prog_cur;
	}

#if (1 == DVB_HD_LIST)
	if((gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_RADIO ||(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_TV ))
			&&(pSysInfo.stream_type == GXBUS_PM_PROG_HD_SERVICE) )   //radio->tv
	{
		gApp_record_prog.id = pSysInfo.hd_prog_cur;
	}
#endif

#if (1 == DVB_HD_LIST)
	if(((gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_TV) ||(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_HD_SERVICE) )
			&&pSysInfo.stream_type == GXBUS_PM_PROG_RADIO)   //radio->tv
#else
	if(gApp_record_prog.SysInfo.stream_type == GXBUS_PM_PROG_TV
			&&pSysInfo.stream_type == GXBUS_PM_PROG_RADIO)   //radio->tv
#endif
	{
		gApp_record_prog.id = pSysInfo.radio_prog_cur;
	}

	memcpy(&gApp_old_prog,&gApp_record_prog,sizeof(play_prog));
	memcpy(&(gApp_record_prog.SysInfo), &pSysInfo,sizeof(GxBusPmViewInfo));

}

void app_prog_get_playing_record(play_prog** pprecord)
{
	if (NULL == pprecord)
		return;
	
	*pprecord = &gApp_record_prog;
}

void app_prog_get_old_play_prog(play_prog** ppold)
{
	if (NULL == ppold)
		return;

	*ppold = &gApp_old_prog;
}

void app_prog_update_num_in_group(void)
{
	return;
//	gApp_num_in_group = GxBus_PmProgNumGet();
}
#if 0
//cth
GxTime prog_time1,prog_time2;
int prog_flag = 0;
static int _get_take_millisecond(GxTime start, GxTime stop)
{
	int start_ms, stop_ms;

	start_ms = start.seconds * 1000 + start.microsecs / 1000;
	stop_ms = stop.seconds * 1000 + stop.microsecs / 1000;

	return (stop_ms - start_ms);
}
void prog_mark_time()
{
    printf("@@@@@@@@,mark time\n");
    memset(&prog_time1,0,sizeof(GxTime));
    GxCore_GetLocalTime(&prog_time1);
    prog_flag = 1;
}
void prog_print_time()
{
    if(prog_flag)
    {
        prog_flag = 0;
        memset(&prog_time2,0,sizeof(GxTime));
        GxCore_GetLocalTime(&prog_time2);
        printf("@@@@@@@@@@@@@@,take %d ms\n",_get_take_millisecond(prog_time1,prog_time2)); 
    }
}
#endif
//获取节目总数
uint32_t app_prog_get_num_in_group(void)
{
	gApp_num_in_group = GxBus_PmProgNumGet();
	return gApp_num_in_group;
}
void app_prog_get_pos_in_group_by_service_id(uint32_t service_id,uint32_t *ppos)
{
	GxBusPmDataProg prog_data;
	uint32_t prog_num,i;

	if (NULL == ppos)
		return;

	prog_num = GxBus_PmProgNumGet();

	for(i=0;i<prog_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
		if(service_id == prog_data.service_id)
		{
			*ppos = i;
			return ;
		}
	}
	if(i == prog_num)
	{
		*ppos = 0;
	}
	return;

}

int32_t app_prog_get_pos_in_group_by_id(uint32_t prog_id,uint32_t *ppos)
{
	GxBusPmDataProg prog_data;
	uint32_t prog_num,i;

	if (NULL == ppos)
		return -1;

	prog_num = GxBus_PmProgNumGet();
	
	for(i=0;i<prog_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
		if(prog_id == prog_data.id)
		{
			*ppos = i;
			return 0;
		}
	}
	if(i == prog_num)
	{
		*ppos = 0;
	}
	return -1;

}

int32_t app_prog_get_playing_pos_in_group(uint32_t *ppos)
{
	GxBusPmDataProg prog_data;
	GxBusPmViewInfo sysinfo;
	uint32_t prog_num,i;
	uint32_t prog_id;

	if (NULL == ppos)
		return -1;

	prog_num = GxBus_PmProgNumGet();
	if (0 == prog_num)
		{
		    //qm, 2015-01-28
			*ppos = 0;
			return -1;
		}
	GxBus_PmViewInfoGet(&sysinfo);

	if(sysinfo.stream_type == GXBUS_PM_PROG_TV)
	{
		prog_id = sysinfo.tv_prog_cur;
	}
	else
#if (1 == DVB_HD_LIST)
		if (GXBUS_PM_PROG_HD_SERVICE == sysinfo.stream_type)
			{
				prog_id = sysinfo.hd_prog_cur;	
			}
	else
#endif
	{
		prog_id = sysinfo.radio_prog_cur;
	}
	for(i=0;i<prog_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
		if(prog_id == prog_data.id)
		{
			*ppos = i;

			break;
		}
	}
	if(i == prog_num)
	{
		*ppos = 0;
		 app_prog_save_playing_pos_in_group(*ppos);
	}
	
	return 0;
}

/*
*输出查找对应节目的ppos,group_mode,stream_type,fav_id
* 
*/
int32_t app_prog_find_by_id(uint32_t prog_id,int32_t *ppos,uint8_t *group_mode,uint8_t *stream_type,uint8_t *fav_id)
{
	GxBusPmDataProg prog_data;
	GxBusPmViewInfo sysinfo_old;
	GxBusPmViewInfo sysinfo;
	uint32_t prog_num,i;

	if ((NULL == ppos)||(NULL == group_mode)||(NULL == stream_type)||(NULL == fav_id))
		return -1;


	/*
	* 首先查找节目是否被跳过
	*/
	GxBus_PmViewInfoGet(&sysinfo_old);
	GxBus_PmViewInfoGet(&sysinfo);
	sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
	sysinfo.group_mode = GROUP_MODE_ALL;
	sysinfo.stream_type = GXBUS_PM_PROG_ALL;
	GxBus_PmViewInfoModify(&sysinfo);
	prog_num = GxBus_PmProgNumGet();
	for(i=0;i<prog_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
		if(prog_id == prog_data.id)
		{
			if(1 == prog_data.skip_flag)
			{
				/*
				* 节目被跳过
				*/
				return 4;
			}
			else
				{
					break;
				}
		}
	}

	if (i == prog_num)
		{
			/*
			* 所有节目中找不到对应id的节目
			*/
			
			*ppos = -1;
			GxBus_PmViewInfoModify(&sysinfo_old);
			return -1;
		}


	/*
	* 正常查找节目
	*/
	GxBus_PmViewInfoModify(&sysinfo_old);
	GxBus_PmViewInfoGet(&sysinfo_old);
	prog_num = GxBus_PmProgNumGet();
	for(i=0;i<prog_num;i++)
	{
		GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
		if(prog_id == prog_data.id)
		{
			*ppos = i;
			 *group_mode = sysinfo_old.group_mode;
			 *stream_type = sysinfo_old.stream_type;
			 *fav_id = sysinfo_old.fav_id;
			 return 0;
		}
	}
	GxBus_PmViewInfoGet(&sysinfo);
	if(sysinfo.group_mode == GROUP_MODE_FAV)
	{
		sysinfo.group_mode = GROUP_MODE_ALL;
		sysinfo.fav_id = 0;
		GxBus_PmViewInfoModify(&sysinfo);	
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);
				
				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 1;
			}
		}

		sysinfo.stream_type = GXBUS_PM_PROG_TV;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				
				 return 2;
			}
		}
#if (1 == DVB_HD_LIST)
		sysinfo.stream_type = GXBUS_PM_PROG_HD_SERVICE;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				
				 return 2;
			}
		}
#endif

		sysinfo.stream_type = GXBUS_PM_PROG_RADIO;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				
				 return 2;
			}
		}
		
		sysinfo.stream_type = GXBUS_PM_PROG_NVOD;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);
				
				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 3;
			}
		}

	}
	else
	{
		if(sysinfo.stream_type == GXBUS_PM_PROG_NVOD)
		{
			sysinfo.stream_type = GXBUS_PM_PROG_TV;
			GxBus_PmViewInfoModify(&sysinfo);
			prog_num = GxBus_PmProgNumGet();
			for(i=0;i<prog_num;i++)
			{
				GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
				if(prog_id == prog_data.id)
				{
					*ppos = i;

					*group_mode = sysinfo.group_mode;
				 	*stream_type = sysinfo.stream_type;
				 	*fav_id = sysinfo.fav_id;
					 //GxBus_PmViewInfoModify(&sysinfo_old);
					 return 1;
				}
			}
#if (1 == DVB_HD_LIST)			
			sysinfo.stream_type = GXBUS_PM_PROG_HD_SERVICE;
			GxBus_PmViewInfoModify(&sysinfo);
			prog_num = GxBus_PmProgNumGet();
			for(i=0;i<prog_num;i++)
			{
				GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
				if(prog_id == prog_data.id)
				{
					*ppos = i;

					*group_mode = sysinfo.group_mode;
				 	*stream_type = sysinfo.stream_type;
				 	*fav_id = sysinfo.fav_id;
					 GxBus_PmViewInfoModify(&sysinfo_old);
					 return 1;
				}
			}
#endif

			
		}

		sysinfo.stream_type = GXBUS_PM_PROG_TV;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				 GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 2;
			}
		}
#if (1 == DVB_HD_LIST)	
		sysinfo.stream_type = GXBUS_PM_PROG_HD_SERVICE;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				 GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 2;
			}
		}
#endif

		sysinfo.stream_type = GXBUS_PM_PROG_RADIO;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				 GxBus_PmViewInfoModify(&sysinfo_old);

				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 2;
			}
		}


		
		sysinfo.stream_type = GXBUS_PM_PROG_NVOD;
		GxBus_PmViewInfoModify(&sysinfo);
		prog_num = GxBus_PmProgNumGet();
		for(i=0;i<prog_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,(void*)&prog_data);
			if(prog_id == prog_data.id)
			{
				*ppos = i;
				GxBus_PmViewInfoModify(&sysinfo_old);
				*group_mode = sysinfo.group_mode;
			 	*stream_type = sysinfo.stream_type;
			 	*fav_id = sysinfo.fav_id;
				 return 3;
			}
		}
	}
	*ppos = -1;
	GxBus_PmViewInfoModify(&sysinfo_old);
	return -1;

}


void app_prog_set_view_info_enable(void)
{
	GxBusPmViewInfo sys;
	GxBusPmDataProg prog;

	if(GxBus_PmProgNumGet()>0)
	{
		GxBus_PmViewInfoGet(&sys);
		if(sys.status == VIEW_INFO_DISABLE)
		{
			GxBus_PmProgGetByPos(0,1,&prog);

			if(sys.tv_prog_cur == 0 && 
				sys.stream_type == GXBUS_PM_PROG_TV)
			{
				sys.tv_prog_cur = prog.id;
			}
#if (1 == DVB_HD_LIST)
			else if(sys.hd_prog_cur == 0 && sys.stream_type == GXBUS_PM_PROG_HD_SERVICE)
			{
				sys.hd_prog_cur = prog.id;
			}	
#endif		
			else if(sys.radio_prog_cur == 0 && sys.stream_type == GXBUS_PM_PROG_RADIO)
			{
				sys.radio_prog_cur = prog.id;
			}
			sys.status = VIEW_INFO_ENABLE;
			GxBus_PmViewInfoModify(&sys);
		}
	}
}

/*
* 恢复出厂设置
* 无须恢复的值，在此先Get当前值，在app_set_default后再Set回去
*/

void app_prog_set_default(void)
{
	int32_t Config;
	VideoColor color;
	char *osd_language=NULL;
	GxBusPmDataSat sat;
	uint32_t   panel_prog_cur;
	int32_t nADVersion = 0;

#ifdef CA_FLAG
	int32_t dvb_ca_flag = 0;
#ifdef DVB_CA_TYPE_QZ_FLAG
    int32_t mode_value = 0;
#endif
	char* str;
    str = NULL;
	app_cas_api_release_ecm_filter();
	app_cas_api_release_emm_filter();
#ifdef DVB_CA_TYPE_BY_FLAG
	{
		dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
		if(DVB_CA_TYPE_BY == dvb_ca_flag)
			{
				app_by_cas_api_release_bat_filter();
			}
	}
#endif
#ifdef DVB_CA_TYPE_QILIAN_FLAG
	{
		dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
		 if(DVB_CA_TYPE_QILIAN == dvb_ca_flag)
		 {
			 app_qilian_cas_api_release_bat_filter();
		 }
	}
#endif

#endif

#ifdef DVB_AD_TYPE_3H_FLAG
	GxAD_ItemFilterClose();
#endif

	app_epg_close();


	/*
	* 后台监测表
	*/
	app_table_bat_filter_close();
	app_table_nit_search_filter_close();
	app_table_pmt_filter_close();
//	app_porting_disable_query_demux();
	app_play_clear_ca_msg();
	Config = app_flash_get_config_mute_flag();
//	if(Config == 1)
	{
		Config = 0;
		app_flash_save_config_mute_flag(Config);
		app_play_set_mute(Config);
	}

	app_play_stop();	
	app_flash_save_default_config_para();
	/*
	* restore ca flag
	*/
#ifdef CA_FLAG
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
#ifdef DVB_CA_TYPE_MG_FLAG
	{
		if(DVB_CA_TYPE_MG == dvb_ca_flag)
			{
				str = MGCA_Get_Config_Serial();
			}
	}
#endif

#ifdef DVB_CA_TYPE_MG312_FLAG
	{
		if (DVB_CA_TYPE_MG312 == dvb_ca_flag)
		{
			str = MGCA312_Get_Config_Serial();
		}
	}
#endif

#ifdef DVB_CA_TYPE_QZ_FLAG
	{
		if(DVB_CA_TYPE_QZ == dvb_ca_flag)
		{
            extern int32_t qzcas_get_config_hs_mode(void);
            mode_value = qzcas_get_config_hs_mode();
		}
	}
#endif

#endif
	GxBus_ConfigGetInt("ADVersion", &nADVersion, 32);
	GxBus_ConfigLoadDefault();
	GxBus_ConfigSetInt("ADVersion", nADVersion); 				
#ifdef CA_FLAG
	app_flash_save_config_dvb_ca_flag(dvb_ca_flag);
#ifdef DVB_CA_TYPE_MG_FLAG
	{
		if(DVB_CA_TYPE_MG == dvb_ca_flag)
		{
			MGCA_Save_Config_Serial(str);
		}
	}
#endif
#ifdef DVB_CA_TYPE_MG312_FLAG
	{
		if (DVB_CA_TYPE_MG312 == dvb_ca_flag)
		{
			MGCA312_Save_Config_Serial(str);
		}
	}
#endif

#ifdef DVB_CA_TYPE_QZ_FLAG
	{
		if(DVB_CA_TYPE_QZ == dvb_ca_flag)
		{
            extern int32_t qzcas_set_config_hs_mode(int32_t config);
            qzcas_set_config_hs_mode(mode_value);
		}
	}
#endif

#endif
	app_sys_init();

	Config = app_flash_get_config_osd_trans();	
	app_play_set_osd_trasn_level(Config);

	/*
	* set video color
	*/
	color.brightness = app_flash_get_config_videocolor_brightness();
	color.saturation = app_flash_get_config_videocolor_saturation();
	color.contrast = app_flash_get_config_videocolor_contrast();
	app_play_set_videoColor_level(color);


	osd_language = app_flash_get_config_osd_language();
	app_play_set_osd_language(osd_language);

	app_send_msg(GXMSG_EPG_CLEAN, NULL);

	GxBus_PmLoadDefault(SAT_MAX_NUM,TP_MAX_NUM,SERVICE_MAX_NUM,NULL);
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
	sat.type = GXBUS_PM_SAT_C;
	sat.tuner = 0;
	GxBus_PmSatAdd(&sat);	
	GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	sat.type = GXBUS_PM_SAT_DTMB;
	sat.tuner = 0;
	sat.sat_dtmb.work_mode = app_flash_get_config_dtmb_dvbc_switch();
	GxBus_PmSatAdd(&sat);
	GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#endif


	app_book_clear_all();
	app_prog_set_view_info_enable();
	app_prog_update_num_in_group();

	/*前面板显示0*/
	panel_prog_cur = 0;
	app_panel_show(PANEL_DATA,&(panel_prog_cur));	

#if (DVB_CHIP_TYPE == GX_CHIP_3115)||(DVB_CHIP_TYPE == GX_CHIP_3113C)
    *(unsigned int*)0xa4804030 = 0x6a963f3f;
    *(unsigned int*)0xa4804028 = 0xa0248cf;
#endif
}

/*
* center fre nit version change , delete all prog and research prog
*/
void app_prog_delete_all_prog(void)
{						
	GxBusPmDataSat sat;
	uint32_t   panel_prog_cur;

	
#ifdef CA_FLAG
	app_cas_api_release_ecm_filter();
	app_cas_api_release_emm_filter();
#ifdef DVB_CA_TYPE_BY_FLAG
	if(DVB_CA_TYPE_BY == app_flash_get_config_dvb_ca_flag())
		{
			app_by_cas_api_release_bat_filter();
		}
#endif
#ifdef DVB_CA_TYPE_QILIAN_FLAG
	if(DVB_CA_TYPE_QILIAN == app_flash_get_config_dvb_ca_flag())
		{
			app_qilian_cas_api_release_bat_filter();
		}
#endif
#endif
		app_epg_close();
		/*
		* 后台监测表
		*/
		app_table_nit_monitor_filter_close();
		app_table_nit_search_filter_close();
		app_table_bat_filter_close();
		app_table_pmt_filter_close();
		app_play_clear_ca_msg();


		app_play_stop();	
		app_send_msg(GXMSG_EPG_CLEAN, NULL);
		GxBus_PmLoadDefault(SAT_MAX_NUM,TP_MAX_NUM,SERVICE_MAX_NUM,NULL);
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
		sat.type = GXBUS_PM_SAT_C;
		sat.tuner = 0;
		GxBus_PmSatAdd(&sat);	
		GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
		sat.type = GXBUS_PM_SAT_DTMB;
		sat.tuner = 0;
		sat.sat_dtmb.work_mode = app_flash_get_config_dtmb_dvbc_switch();
		GxBus_PmSatAdd(&sat);
		GxBus_PmSync(GXBUS_PM_SYNC_SAT);
#endif


		app_book_clear_all();
		app_prog_set_view_info_enable();
		app_prog_update_num_in_group();

		/*前面板显示0*/
		panel_prog_cur = 0;
		app_panel_show(PANEL_DATA,&(panel_prog_cur));								
		app_search_set_auto_flag(TRUE);	
}


/*
* 获取当前节目类型TV/RADIO
*/
uint8_t app_prog_get_stream_type(void)
{
	uint8_t stream_type = 0;
	GxBusPmViewInfo sysinfo;
	GxBus_PmViewInfoGet(&sysinfo);	
	stream_type = sysinfo.stream_type;
	return stream_type;
}

/*
* 切换磄roup_mode、stream_type、fav_id类型读取节目个数,接口返回之前恢复组、类型参数
* 进入菜单前调用，如需提示"无电视节目等"
* group_mode为GROUP_MODE_FAV，fav_id参数有效
*/
uint32_t app_prog_check_group_num(uint8_t group_mode,uint8_t stream_type,uint8_t fav_id)
{
	uint32_t num = 0;
	GxBusPmViewInfo sysinfo;
	GxBusPmViewInfo sysinfo_old;
	GxBus_PmViewInfoGet(&sysinfo);
	switch(group_mode)
		{
		case GROUP_MODE_ALL:
        printf("###%d,%d,%s\n",sysinfo.group_mode,sysinfo.stream_type,
            sysinfo.gx_pm_prog_check?"func":"NULL");
			if ((sysinfo.stream_type != stream_type)||(sysinfo.group_mode != group_mode))
				{
					memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));
					sysinfo.stream_type = stream_type;
					sysinfo.group_mode = group_mode;
					GxBus_PmViewInfoModify(&sysinfo);
					app_prog_update_num_in_group();
					num =  app_prog_get_num_in_group();
					/*
					* 此接口仅判断节目是否存在
					* 返回之前的配置
					*/
					GxBus_PmViewInfoModify(&sysinfo_old);
					app_prog_update_num_in_group();		
					
				}
			else
				{
					app_prog_update_num_in_group();
					num =  app_prog_get_num_in_group();					
				}
			break;
			case GROUP_MODE_SAT:
				break;
			case GROUP_MODE_FAV:
				if ((sysinfo.stream_type != stream_type)||(sysinfo.group_mode != group_mode)||(sysinfo.fav_id != fav_id))
					{
						/*
						* 切换到喜爱模式读取节目个数
						*/
						memcpy(&sysinfo_old,&sysinfo,sizeof(GxBusPmViewInfo));

						/*
						* 喜爱模式可切换喜爱分组
						*/
						sysinfo.stream_type = stream_type;
						sysinfo.group_mode = group_mode;
						sysinfo.fav_id = fav_id;
						GxBus_PmViewInfoModify(&sysinfo);
						app_prog_update_num_in_group();
						num =  app_prog_get_num_in_group();
						/*
						* 此接口仅判断节目是否存在
						* 返回之前的配置
						*/
						GxBus_PmViewInfoModify(&sysinfo_old);
						app_prog_update_num_in_group();		
					}
				else
					{
						app_prog_update_num_in_group();
						num =  app_prog_get_num_in_group();											
					}
				break;
			default:
				break;
			}
	return num;
}

/*
* 仅切换节目类型 ，
* 不改变当前group_mode，喜爱分组
*/
uint32_t app_prog_change_stream_type(uint8_t stream_type)
{
	uint32_t num = 0;
	GxBusPmViewInfo sysinfo;
	GxBus_PmViewInfoGet(&sysinfo);
	switch(sysinfo.group_mode)
		{
			case GROUP_MODE_ALL:
				if (sysinfo.stream_type != stream_type)
					{
						sysinfo.stream_type = stream_type;
						GxBus_PmViewInfoModify(&sysinfo);						
					}				
				break;
			case GROUP_MODE_SAT:
				break;
			case GROUP_MODE_FAV:
				if (sysinfo.stream_type != stream_type)
					{
						/*
						* 喜爱模式可切换喜爱分组
						*/
						sysinfo.stream_type = stream_type;
						GxBus_PmViewInfoModify(&sysinfo);
					}
				break;
			default:
				break;
		}
	app_prog_update_num_in_group();
	num =  app_prog_get_num_in_group();
	return num;

}


/*
* 切换节目分组、节目类型、喜爱分组等
* group_mode为GROUP_MODE_FAV，fav_id参数有效
*/
uint32_t app_prog_change_group(uint8_t group_mode,uint8_t stream_type,uint8_t fav_id)
{
	uint32_t num = 0;
	GxBusPmViewInfo sysinfo;
	GxBus_PmViewInfoGet(&sysinfo);
	switch(group_mode)
		{
			case GROUP_MODE_ALL:
				if ((sysinfo.stream_type != stream_type)||(sysinfo.group_mode != group_mode))
					{
						sysinfo.stream_type = stream_type;
						sysinfo.group_mode = group_mode;
//						sysinfo.fav_id = fav_id; /*非喜爱模式，不修改原有喜爱分组*/
						GxBus_PmViewInfoModify(&sysinfo);						
					}				
				break;
			case GROUP_MODE_SAT:
				break;
			case GROUP_MODE_FAV:
				if ((sysinfo.stream_type != stream_type)||(sysinfo.group_mode != group_mode)||(sysinfo.fav_id != fav_id))
					{
						/*
						* 喜爱模式可切换喜爱分组
						*/
						sysinfo.stream_type = stream_type;
						sysinfo.group_mode = group_mode;
						sysinfo.fav_id = fav_id;
						GxBus_PmViewInfoModify(&sysinfo);
					}
				break;
			default:
				break;
		}
	app_prog_update_num_in_group();
	num =  app_prog_get_num_in_group();
	return num;

}

/*
* 获取每个节目单独声道接口
*/
uint32_t app_prog_get_cur_prog_audio_track()
{
       uint32_t pos;
	GxBusPmDataProg Prog={0};
	uint32_t audiotrack=AUDIO_TRACK_LEFT;

	if ( -1 != app_prog_get_playing_pos_in_group(&pos))
		{
			GxBus_PmProgGetByPos(pos,1,&Prog);
			audiotrack = Prog.audio_mode;			
		}


	return audiotrack;
}

uint8_t app_prog_get_group_mode(void)
{
	uint8_t group_mode = 0;
	GxBusPmViewInfo sysinfo;
	GxBus_PmViewInfoGet(&sysinfo);	
	group_mode = sysinfo.group_mode;
	return group_mode;
}

/*
* 每个节目单独保存声道接口
*/
uint32_t app_prog_save_cur_prog_audio_track(uint32_t audiotrack)
{
       uint32_t pos;
	GxBusPmDataProg Prog;
	if ( -1 != app_prog_get_playing_pos_in_group(&pos))
		{
			GxBus_PmProgGetByPos(pos,1,&Prog);
			if (audiotrack != Prog.audio_mode)
				{
					Prog.audio_mode = audiotrack;
					GxBus_PmProgInfoModify(&Prog);
					GxBus_PmSync(GXBUS_PM_SYNC_PROG);				
				}
		//	app_play_set_audio_track(audio_track);
		}

	return 0;
}


/*
* 获取每个节目单独音量接口
*/
uint32_t app_prog_get_cur_prog_audio_volumn()
{
    uint32_t pos;
	GxBusPmDataProg Prog={0};
	uint32_t audiovolumn=48; // AUDIO_VOLUME_DV

	if ( -1 != app_prog_get_playing_pos_in_group(&pos))
		{
			GxBus_PmProgGetByPos(pos,1,&Prog);
			audiovolumn = Prog.audio_volume;		
		}
	
	return audiovolumn;
}

/*
* 每个节目单独保存声道接口
*/
uint32_t app_prog_save_cur_prog_audio_volumn(uint32_t audiovolumn)
{
       uint32_t pos;
	GxBusPmDataProg Prog;
	if (-1 != app_prog_get_playing_pos_in_group(&pos))
		{
			GxBus_PmProgGetByPos(pos,1,&Prog);

			Prog.audio_volume= audiovolumn;
			GxBus_PmProgInfoModify(&Prog);
			GxBus_PmSync(GXBUS_PM_SYNC_PROG);				
		}

	return 0;
}


/*
* 
*/
uint32_t app_prog_edit_init_attr(void)
{
	uint32_t program_num,i;
	GxBusPmDataProg prog;

	
	if (NULL != pProedit_arr)
		{
			GxCore_Free(pProedit_arr);
			pProedit_arr = NULL;		
		}

	program_num = app_prog_get_num_in_group();

		pProedit_arr = (App_proedit_arr *) GxCore_Malloc(program_num*sizeof(App_proedit_arr));
		if (NULL == pProedit_arr)
			return 0;
		memset(pProedit_arr,0,program_num*sizeof(App_proedit_arr));
		for(i=0;i<program_num;i++)
		{
			GxBus_PmProgGetByPos(i,1,&prog);
			pProedit_arr[i].change_flag = 0;
			pProedit_arr[i].prog_pos = i;
			pProedit_arr[i].prog_id = prog.id;
			memcpy( pProedit_arr[i].prog_name,prog.prog_name,MAX_PROG_NAME);
			if(prog.favorite_flag!=0)
			{
				pProedit_arr[i].fav_flag = 1;
			}			
			if(prog.skip_flag == GXBUS_PM_PROG_BOOL_ENABLE )
			{
				pProedit_arr[i].skip_flag = 1;
			}			
			if(prog.lock_flag == GXBUS_PM_PROG_BOOL_ENABLE )
			{
				pProedit_arr[i].lock_flag = 1;
			}			
		}
	return 0;
	
}

App_proedit_arr* app_prog_edit_get_attr(uint32_t sel)
{
	if (sel >= app_prog_get_num_in_group())
		return NULL;
	if (NULL == pProedit_arr)
		return NULL;

	return &(pProedit_arr[sel]);
}

uint32_t app_prog_edit_rename_attr(char * widget_listview,char* prog_name)
{
	uint32_t sel = 0;
	if (NULL != widget_listview)
		GUI_GetProperty(widget_listview,"select",(void*)&sel);
	
	if (0 == app_prog_get_num_in_group())
		return 0;
	if (sel >= app_prog_get_num_in_group())
		return 0;
	if (NULL == pProedit_arr)
		return 0;

	if (NULL == prog_name)
		return 0;

	memcpy(pProedit_arr[sel].prog_name,prog_name,MAX_PROG_NAME);
	pProedit_arr[sel].prog_name[MAX_PROG_NAME -1] = 0;
	
	if (NULL != widget_listview)	
		GUI_SetProperty(widget_listview,"update_row",(void*)&sel);
	return 1;	
	
}

uint32_t app_prog_edit_set_attr(char * widget_listview,uint32_t type)
{
	uint32_t sel = 0;
	if (NULL != widget_listview)
		GUI_GetProperty(widget_listview,"select",(void*)&sel);
	
	if (0 == app_prog_get_num_in_group())
		return 0;
	if (sel >= app_prog_get_num_in_group())
		return 0;
	if (NULL == pProedit_arr)
		return 0;

	switch(type)
		{
			case PROG_EDIT_LOCK:
				pProedit_arr[sel].lock_flag ^=1;
				pProedit_arr[sel].change_flag ^=1; 
				break;
			case PROG_EDIT_DEL:
				pProedit_arr[sel].del_flag^=1;
				pProedit_arr[sel].change_flag ^=2; 
				break;
			case PROG_EDIT_FAV:
				pProedit_arr[sel].fav_flag ^=1;
				pProedit_arr[sel].change_flag ^=4; 
				break;
			case PROG_EDIT_SKIP:
				pProedit_arr[sel].skip_flag ^=1;
				pProedit_arr[sel].change_flag ^=8; 
				break;
			case PROG_EDIT_MOVE:
				pProedit_arr[sel].mov_flag ^=1;
				break;
			default:
				break;	
		}
	
	if (NULL != widget_listview)	
		GUI_SetProperty(widget_listview,"update_row",(void*)&sel);
	return 1;	
}
uint32_t app_jy_prog_edit_set_attr(uint32_t sel,uint32_t type)
{
//	uint32_t sel = 0;
	//if (NULL != widget_listview)
		//GUI_GetProperty(widget_listview,"select",(void*)&sel);
	
	if (0 == app_prog_get_num_in_group())
		return 0;
	if (sel >= app_prog_get_num_in_group())
		return 0;
	if (NULL == pProedit_arr)
		return 0;

	switch(type)
		{
			case PROG_EDIT_LOCK:
				pProedit_arr[sel].lock_flag ^=1;
				pProedit_arr[sel].change_flag ^=1; 
				break;
			case PROG_EDIT_DEL:
				pProedit_arr[sel].del_flag^=1;
				pProedit_arr[sel].change_flag ^=2; 
				break;
			case PROG_EDIT_FAV:
				pProedit_arr[sel].fav_flag ^=1;
				pProedit_arr[sel].change_flag ^=4; 
				break;
			case PROG_EDIT_SKIP:
				pProedit_arr[sel].skip_flag ^=1;
				pProedit_arr[sel].change_flag ^=8; 
				break;
			case PROG_EDIT_MOVE:
				pProedit_arr[sel].mov_flag ^=1;
				break;
			default:
				break;	
		}

	return 1;	
}

uint32_t app_prog_edit_move_attr(char * widget_listview,int32_t direction)
{
	uint32_t value = 0;
	uint32_t value2 = 0;

	App_proedit_arr* pedit_arr_1=NULL;
	App_proedit_arr* pedit_arr_2=NULL;
	App_proedit_arr temp;

	
	if (NULL != widget_listview)	
		GUI_GetProperty(widget_listview,"select",(void*)&value);

	if (1 >= app_prog_get_num_in_group())
		{
			/*
			* prog count is 1 or 0, don't support  move 
			*/
			return 0;
		}
	
	switch(direction)
		{
		 	case -1: /*向上移动*/
				if(value == 0)
					{
						/*
						* top
						*/
						value2 = app_prog_get_num_in_group()-1;
					}
				else
					{
						value2 = 	value-1;
						
					}
				
					pedit_arr_1 = app_prog_edit_get_attr(value);
					if ((NULL != pedit_arr_1)&&(pedit_arr_1->mov_flag == 1))
					{
						pedit_arr_2 = app_prog_edit_get_attr(value2);
						memcpy(&temp,pedit_arr_2,sizeof(App_proedit_arr));
						memcpy(pedit_arr_2,pedit_arr_1,sizeof(App_proedit_arr));
						memcpy(pedit_arr_1,&temp,sizeof(App_proedit_arr));
					}

				break;
			case 1: /*向下移动*/
					if(value == app_prog_get_num_in_group()-1)
					{
						/*
						* bottom
						*/
						value2 = 0;
					}
					else
						{
							value2 = 	value+1;
							
						}
					
					pedit_arr_1 = app_prog_edit_get_attr(value);
					if ((NULL != pedit_arr_1)&&(pedit_arr_1->mov_flag == 1))
					{
						pedit_arr_2 = app_prog_edit_get_attr(value2);
						memcpy(&temp,pedit_arr_2,sizeof(App_proedit_arr));
						memcpy(pedit_arr_2,pedit_arr_1,sizeof(App_proedit_arr));
						memcpy(pedit_arr_1,&temp,sizeof(App_proedit_arr));
					}
				break;
			default:
				break;
		}

	return 0;
}

uint32_t app_prog_edit_get_attr_change_flag()
{
	uint32_t i = 0;
	GxBusPmDataProg prog = {0};

	if (NULL == pProedit_arr)
		{
			return FALSE;
		}

	for(i=0;i<app_prog_get_num_in_group();i++)
	{
		GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);		
		if(pProedit_arr[i].change_flag != 0 || pProedit_arr[i].prog_pos != i
			||pProedit_arr[i].prog_id != prog.id
			||0!= memcmp( pProedit_arr[i].prog_name,prog.prog_name,MAX_PROG_NAME))
		{
			/*节目喜爱、删除、加锁、跳过等标志改变
			*  或节目名称改变、移动变化
			*/
			return TRUE;
		}
	}

	return FALSE;
	
}

void app_prog_eidt_save_attr(char * widget_listview)
{
	uint32_t value = 0;
	static GxBusPmDataProg prog;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t *data;
	uint32_t ProgNum = 0;
	uint32_t count = 0;

	if (NULL == pProedit_arr)
		{
			return;
		}
	if (NULL == widget_listview)
		return;
	
		{
			data = (uint32_t *) GxCore_Malloc(4*app_prog_get_num_in_group());
			if(data == NULL)
			{
				if (NULL != pProedit_arr)
				{
					GxCore_Free(pProedit_arr);
					pProedit_arr = NULL;		
				}
				return;
			}			
			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x01) != 0)//加锁
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_LOCK,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x08) != 0)//跳过
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_SKIP,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}


			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x04) != 0)//收藏
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}	
			if(count != 0)
			{
				GxBus_PmProgFavModify(1,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			/*
			* 重命名
			*/			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);

					if (0!= memcmp( pProedit_arr[i].prog_name,prog.prog_name,MAX_PROG_NAME))
						{
							memcpy(prog.prog_name,pProedit_arr[i].prog_name,MAX_PROG_NAME -1);
							prog.prog_name[MAX_PROG_NAME -1] = 0;
							GxBus_PmProgInfoModify(&prog);
							count++;
						}
			}
			if(count != 0)
			{
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			memset(data,0,4*app_prog_get_num_in_group()); 
			count = 0;
			for(j=0;j<app_prog_get_num_in_group();j++)
			{
				if(pProedit_arr[j].prog_pos != j)
					{
						// 移动
						for(i=0;i<app_prog_get_num_in_group();i++)
						{
								data[count] = pProedit_arr[i].prog_pos;
								count++;
						}
						
						GxBus_PmProgMoveSpecial(data, count);
						GxBus_PmSync(GXBUS_PM_SYNC_PROG);
							
						/*********************update userlist***********************/
						app_prog_userlist_save(data, count);					
						break;
					}

			}

			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;							
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x02) != 0)//删除
				{
//					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
//					data[count] = prog.id;		
//					app_book_delete_prog(prog.id);
					data[count] = pProedit_arr[i].prog_id;
					app_book_delete_prog(pProedit_arr[i].prog_id);
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgDelete(data, count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
                
				/*********************update userlist***********************/
				app_prog_userlist_del();
				app_prog_userlist_del_program(data, count);
				
				app_prog_update_num_in_group();
				if (NULL != widget_listview)
					GUI_SetProperty(widget_listview,"update_all",NULL);
			}		
			
			if (NULL != widget_listview)
				GUI_GetProperty(widget_listview,"select",(void*)&value);
			ProgNum = app_prog_get_num_in_group();
			if(value>=ProgNum)
			{
				value = ProgNum-1;
			}

//			app_prog_save_playing_pos_in_group(value);
			GxCore_Free(data);
			data = NULL;
		}

	if (NULL != pProedit_arr)
	{
		GxCore_Free(pProedit_arr);
		pProedit_arr = NULL;		
	}

	return ;


}
void app_jy_prog_eidt_save_lock_attr(char * widget_listview)
{
	uint32_t value = 0;
	static GxBusPmDataProg prog;
	uint32_t i = 0;
	//uint32_t j = 0;
	uint32_t *data;
	uint32_t ProgNum = 0;
	uint32_t count = 0;

	if (NULL == pProedit_arr)
		{
			return;
		}
	if (NULL == widget_listview)
		return;
	
		{
			data = (uint32_t *) GxCore_Malloc(4*app_prog_get_num_in_group());
			if(data == NULL)
			{
				if (NULL != pProedit_arr)
				{
					GxCore_Free(pProedit_arr);
					pProedit_arr = NULL;		
				}
				return;
			}			
			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x01) != 0)//加锁
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_LOCK,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}
			#if 0
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x08) != 0)//跳过
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_SKIP,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x04) != 0)//收藏
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}	
			if(count != 0)
			{
				GxBus_PmProgFavModify(1,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			/*
			* 重命名
			*/			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);

					if (0!= memcmp( pProedit_arr[i].prog_name,prog.prog_name,MAX_PROG_NAME))
						{
							memcpy(prog.prog_name,pProedit_arr[i].prog_name,MAX_PROG_NAME -1);
							prog.prog_name[MAX_PROG_NAME -1] = 0;
							GxBus_PmProgInfoModify(&prog);
							count++;
						}
			}
			if(count != 0)
			{
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}
			
			memset(data,0,4*app_prog_get_num_in_group()); 
			count = 0;
			for(j=0;j<app_prog_get_num_in_group();j++)
			{
				if(pProedit_arr[j].prog_pos != j)
					{
						// 移动
						for(i=0;i<app_prog_get_num_in_group();i++)
						{
								data[count] = pProedit_arr[i].prog_pos;
								count++;
						}
						
						GxBus_PmProgMoveSpecial(data, count);
						GxBus_PmSync(GXBUS_PM_SYNC_PROG);
							
						/*********************update userlist***********************/
						app_prog_userlist_save(data, count);					
						break;
					}

			}

			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;							
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x02) != 0)//删除
				{
//					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
//					data[count] = prog.id;		
//					app_book_delete_prog(prog.id);
					data[count] = pProedit_arr[i].prog_id;
					app_book_delete_prog(pProedit_arr[i].prog_id);
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgDelete(data, count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
				/*********************update userlist***********************/
				app_prog_userlist_del();
				app_prog_userlist_del_program(data, count);
				
				app_prog_update_num_in_group();
				if (NULL != widget_listview)
					GUI_SetProperty(widget_listview,"update_all",NULL);
			}		
			#endif
			if (NULL != widget_listview)
				GUI_GetProperty(widget_listview,"select",(void*)&value);
			ProgNum = app_prog_get_num_in_group();
			if(value>=ProgNum)
			{
				value = ProgNum-1;
			}
			
			//			app_prog_save_playing_pos_in_group(value);


			GxCore_Free(data);
			data = NULL;
		}		

	if (NULL != pProedit_arr)
	{
		GxCore_Free(pProedit_arr);
		pProedit_arr = NULL;		
	}

	return ;


}

void app_jy_prog_eidt_save_fav_skip_attr(char * widget_listview,int org_index,int mod_index)
{
	uint32_t value = 0;
	static GxBusPmDataProg prog;
	uint32_t i = 0;
	//uint32_t j = 0;
	uint32_t *data;
	uint32_t ProgNum = 0;
	uint32_t count = 0;

	if (NULL == pProedit_arr)
		{
			return;
		}
	if (NULL == widget_listview)
		return;
	
		{
			data = (uint32_t *) GxCore_Malloc(4*app_prog_get_num_in_group());
			if(data == NULL)
			{
				if (NULL != pProedit_arr)
				{
					GxCore_Free(pProedit_arr);
					pProedit_arr = NULL;		
				}
				return;
			}			
			#if 0
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x01) != 0)//加锁
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_LOCK,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}
			#endif
			//skip
			#if 1
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;					
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x08) != 0)//跳过
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}
			if(count != 0)
			{
				for(i=0;i<count;i++)
					printf("data[i]=%d\n",data[i]);
				GxBus_PmProgBoolModify(GXBUS_PM_PROG_MODIFY_SKIP,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}
			#endif
			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x04) != 0)//收藏
				{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
					data[count] = prog.id;
					count++;
				}
			}	
			if(count != 0)
			{
				GxBus_PmProgFavModify(1,data,count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}
			
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;	
			
			//int total_num=app_prog_get_num_in_group();
			if((mod_index)>app_prog_get_num_in_group())
			{
				printf("move no active\n");
			}
			else
			{
				//	static GxBusPmDataProg prog;
				//int i=0;
				printf("before move\n");
				for(i=0;i<app_prog_get_num_in_group();i++)
				{
					GxBus_PmProgGetByPos(i,1,&prog);
					//printf("i=%d pos=%d id=%d name=%s skip_flag=%d favorite_flag=%d\n",
					//	   i,prog.pos,prog.id,prog.prog_name,prog.skip_flag, prog.favorite_flag);
				}

				#if  0
				if(mod_index==org_index)
				{
					printf("move no need\n");
				}
				else if(mod_index==0)
				{
					printf("move no need\n");
				}
				else if(mod_index>org_index)
				{
					//int i=0;
					for(i=0;i<(org_index-1);i++)
					{
						GxBus_PmProgGetByPos(i,1,&prog);
						data[i]=i;
						data[i]=prog.id;
						//data[i]=prog.pos;
						count++;
					}
										
					for(i=(org_index-1);i<(mod_index-1);i++)
					{
						GxBus_PmProgGetByPos(i+1,1,&prog);
						//data[i]=prog.pos;
						data[i]=i+1;
						data[i]=prog.id;
						//data[i]=prog.pos;
						count++;
					}
					
					GxBus_PmProgGetByPos(org_index-1,1,&prog);
				//	data[mod_index-1] = data[i]=prog.pos;//org_index-1;//prog.id;
					data[mod_index-1] = org_index-1;//org_index-1;//prog.id;
					data[mod_index-1] = prog.id;
					//data[mod_index-1] = prog.pos;
					count++;	
					
					
					for(i=(mod_index);i<total_num;i++)
					{
						GxBus_PmProgGetByPos(i,1,&prog);
						data[i]=i;//prog.id;
						data[i]=prog.id;
						//data[i]=prog.pos;
						count++;
					}

					
					for(i=0;i<count;i++)
						printf("data[%d]=%d\n",i,data[i]);
					GxBus_PmProgMoveSpecial(data, count);

				}
				else if(mod_index<org_index)
				{
					//int i=0;
					for(i=0;i<(mod_index-1);i++)
					{
						GxBus_PmProgGetByPos(i,1,&prog);
						//data[i]=i;//prog.id;
						data[i]=prog.pos;
						count++;
					}
										
					GxBus_PmProgGetByPos(org_index-1,1,&prog);
					data[mod_index-1] =prog.pos;//org_index-1;// prog.id;
					count ++;
					
					for(i=(mod_index);i<(org_index-1);i++)
					{
						GxBus_PmProgGetByPos(i-1,1,&prog);
						data[i+1]=prog.pos;//i-1;//prog.id;
						count++;
					}

					for(i=(org_index);i<total_num;i++)
					{
						GxBus_PmProgGetByPos(i,1,&prog);
						data[i]= prog.pos;//i;//prog.id;
						count++;
					}
					for(i=0;i<count;i++)
						printf("data[%d]=%d\n",i,data[i]);
					GxBus_PmProgMoveSpecial(data, count);

				}
				#else
				uint32_t  id_arry[2]={0};
				GxBus_PmProgGetByPos(org_index-1,1,&prog);
				id_arry[0]=prog.id;
				printf("org_index=%d pos=%d id=%d name=%s skip_flag=%d favorite_flag=%d\n",
						   org_index,prog.pos,prog.id,prog.prog_name,prog.skip_flag, prog.favorite_flag);
				
				GxBus_PmProgGetByPos(mod_index-1,1,&prog);
				//GxBus_PmProgGetByPos(mod_index,1,&prog);
				id_arry[1]=prog.id;
				printf("org_index=%d pos=%d id=%d name=%s skip_flag=%d favorite_flag=%d\n",
						   mod_index,prog.pos,prog.id,prog.prog_name,prog.skip_flag, prog.favorite_flag);
				
				if(mod_index==1)
				{
					printf("move to 1 mod_index=%d\n",mod_index);
					 GxBus_PmProgMove(id_arry,1,id_arry[1],GXBUS_PM_PROG_MOVE_BEFORE);
					 GxBus_PmSync(GXBUS_PM_SYNC_PROG);
					 goto AF;
				}

				if(mod_index==0)
				{
					printf("move no need\n");
				}
				else if(mod_index==org_index)
				{
					printf("move no need\n");
				}
				else if(mod_index>org_index)
				{
					#if 0
					 App_proedit_arr  tmp_Proedit_arr={0};
					 memcpy(&tmp_Proedit_arr,pProedit_arr[org_index],sizeof(App_proedit_arr));
					for(j=org_index+1;j<mod_index;j++)
					{
						memcpy(pProedit_arr[j],pProedit_arr[j+1],sizeof(App_proedit_arr));
					}
					 memcpy(pProedit_arr[mod_index],&tmp_Proedit_arr,sizeof(App_proedit_arr));
					#endif
					
					 GxBus_PmProgMove(id_arry,1,id_arry[1],GXBUS_PM_PROG_MOVE_AFTER);
					// GxBus_PmProgMove(id_arry,1,id_arry[1],GXBUS_PM_PROG_MOVE_BEFORE);
					 GxBus_PmSync(GXBUS_PM_SYNC_PROG);
				}
				else if(mod_index<org_index)
				{
					#if 0
					 App_proedit_arr  tmp_Proedit_arr={0};
					 memcpy(&tmp_Proedit_arr,pProedit_arr[org_index],sizeof(App_proedit_arr));
					for(j=org_index+1;j<mod_index;j++)
					{
						memcpy(pProedit_arr[j],pProedit_arr[j+1],sizeof(App_proedit_arr));
					}
					 memcpy(pProedit_arr[mod_index],&tmp_Proedit_arr,sizeof(App_proedit_arr));
					#endif
					// GxBus_PmProgMove(&org_index,1,mod_index,GXBUS_PM_PROG_MOVE_BEFORE);
					GxBus_PmProgMove(id_arry,1,id_arry[1],GXBUS_PM_PROG_MOVE_AFTER);
					 //GxBus_PmProgMove(id_arry,1,id_arry[1],GXBUS_PM_PROG_MOVE_BEFORE);
					 GxBus_PmSync(GXBUS_PM_SYNC_PROG);
				}
				#endif
AF:				printf("after move\n");
				for(i=0;i<app_prog_get_num_in_group();i++)
				{
					GxBus_PmProgGetByPos(i,1,&prog);
					printf("i=%d pos=%d id=%d name=%s skip_flag=%d favorite_flag=%d\n",i,prog.pos,prog.id,prog.prog_name,prog.skip_flag, prog.favorite_flag);
				}
			}
			/*
			* 重命名
			*/		
			#if 0
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);

					if (0!= memcmp( pProedit_arr[i].prog_name,prog.prog_name,MAX_PROG_NAME))
						{
							memcpy(prog.prog_name,pProedit_arr[i].prog_name,MAX_PROG_NAME -1);
							prog.prog_name[MAX_PROG_NAME -1] = 0;
							GxBus_PmProgInfoModify(&prog);
							count++;
						}
			}
			if(count != 0)
			{
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
			}

			memset(data,0,4*app_prog_get_num_in_group()); 
			count = 0;
			for(j=0;j<app_prog_get_num_in_group();j++)
			{
				if(pProedit_arr[j].prog_pos != j)
					{
						// 移动
						for(i=0;i<app_prog_get_num_in_group();i++)
						{
								data[count] = pProedit_arr[i].prog_pos;
								count++;
						}
						
						GxBus_PmProgMoveSpecial(data, count);
						GxBus_PmSync(GXBUS_PM_SYNC_PROG);
							
						/*********************update userlist***********************/
						app_prog_userlist_save(data, count);					
						break;
					}

			}
		
			memset(data,0,4*app_prog_get_num_in_group());
			count = 0;							
			for(i=0;i<app_prog_get_num_in_group();i++)
			{
				if((pProedit_arr[i].change_flag & 0x02) != 0)//删除
				{
//					GxBus_PmProgGetByPos(pProedit_arr[i].prog_pos,1,&prog);
//					data[count] = prog.id;		
//					app_book_delete_prog(prog.id);
					data[count] = pProedit_arr[i].prog_id;
					app_book_delete_prog(pProedit_arr[i].prog_id);
					count++;
				}
			}
			if(count != 0)
			{
				GxBus_PmProgDelete(data, count);
				GxBus_PmSync(GXBUS_PM_SYNC_PROG);
				/*********************update userlist***********************/
				app_prog_userlist_del();
				app_prog_userlist_del_program(data, count);
				
				app_prog_update_num_in_group();
				if (NULL != widget_listview)
					GUI_SetProperty(widget_listview,"update_all",NULL);
			}		
			#endif
			if (NULL != widget_listview)
				GUI_GetProperty(widget_listview,"select",(void*)&value);
			ProgNum = app_prog_get_num_in_group();
			if(value>=ProgNum)
			{
				value = ProgNum-1;
			}
			
			//			app_prog_save_playing_pos_in_group(value);

			GxCore_Free(data);
			data = NULL;
		}		

	if (NULL != pProedit_arr)
	{
		GxCore_Free(pProedit_arr);
		pProedit_arr = NULL;		
	}

	return ;


}

int app_prog_edit_play_skip_all(int32_t ppos, uint8_t stream_type)
{
	int i = 0;
	uint32_t program_num = 0;
	GxBusPmViewInfo sysinfo = {0};
	GxBusPmViewInfo sysinfo_old;
	
	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysinfo_old, &sysinfo, sizeof(GxBusPmViewInfo));

	sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
	GxBus_PmViewInfoModify(&sysinfo);	

	program_num = app_prog_get_num_in_group();
	for(i = 0; i < program_num; i++)
	{
		if (1 != pProedit_arr[i].skip_flag)
		{
			return 0;
			break;
		}
	}

	if ((i + 1) > program_num)
	{
		if (sysinfo.stream_type != stream_type)
		{
			GxBusPmDataProg prog_data = {0};
			
			sysinfo_old.stream_type = stream_type;
			GxBus_PmViewInfoModify(&sysinfo_old);

			program_num = app_prog_get_num_in_group();
			for(i = 0; i < program_num; i++)
			{
				GxBus_PmProgGetByPos(i,1,&prog_data);
				if (GXBUS_PM_PROG_BOOL_DISABLE == prog_data.skip_flag)
				{
					GxBus_PmViewInfoModify(&sysinfo);	

					return 0;
					break;
				}
			}
		}
		GxBus_PmViewInfoModify(&sysinfo);	
		app_play_reset_play_timer(0);

		return 1; /* skip all */
	}

	return 0;
	
}


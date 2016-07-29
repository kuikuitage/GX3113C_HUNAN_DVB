#include "app_common_book.h"
#include "app_common_play.h"
#include "app_common_epg.h"
#include "app_common_prog.h"
#include "app_common_nvod.h"
#include "app_common_init.h"
#include "gxbook.h"
#include "service/gxepg.h"
#include "gxmsg.h"
#include "gui_key.h"
#include "app_key.h"
#include "gxapp_sys_config.h"
#include "gui_core.h"

static GxBook book_trigger_start;
static GxBook book_trigger_end;
static book_add_event_popmsg_callback app_book_add_event_popmsg_callback=NULL;
static book_play_popmsg_callback app_book_play_popmsg_callback = NULL;
static book_record_popmsg_callback app_book_record_popmsg_callback = NULL;

void app_book_register_play_popmsg_callback(book_play_popmsg_callback bookplaycallback)
{
	if (NULL != bookplaycallback)
		{
			app_book_play_popmsg_callback = bookplaycallback;
		}
}

void app_book_register_record_popmsg_callback(book_record_popmsg_callback bookrecordcallback)
{
	if (NULL != bookrecordcallback)
		{
			app_book_record_popmsg_callback = bookrecordcallback;
		}
}


void app_book_register_add_event_popmsg_callback(book_add_event_popmsg_callback bookaddeventcallback)
{
	if (NULL != bookaddeventcallback)
		app_book_add_event_popmsg_callback = bookaddeventcallback;
	return ;
}


int8_t app_book_init(void)
{
	uint32_t i;
	GxBookGet EpgBookGet;
	GxBusPmDataProg   prog = {0, };
	book_play book;
	int ret = 0;
			
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);

	for(i=0;i<EpgBookGet.book_number;i++)
	{
		if( EpgBookGet.book[i].trigger_mode == BOOK_TRIG_ING)
		{
			app_book_delete(&EpgBookGet.book[i]);
		}
		else
		{
			if((BOOK_POWER_OFF != EpgBookGet.book[i].book_type )&&(BOOK_POWER_ON!=EpgBookGet.book[i].book_type))
			{
				/*
				* if not power off type , check prog exist or not
				*/
				memcpy(&book, EpgBookGet.book[i].struct_buf, MAX_BOOK_STRUCT_LEN);
				ret = GxBus_PmProgGetById(book.prog_id, &prog);

				if(GXCORE_SUCCESS != ret)
				{
					app_book_delete(&EpgBookGet.book[i]);
				}				
			}
		}
	}	
	book_trigger_start.trigger_time_start = 0xffffffff;
	book_trigger_end.trigger_time_start = 0xffffffff;
	return 0;
}
//检测当前预约节目是否存在
int8_t app_book_check_exist(GxBookGet* pEpgBookGet,GxBookType type,int32_t book_id, int *ri_Sel)
{
	int32_t book_get_msg_id;
	app_msg* pmsg;
	int i;
	memset(pEpgBookGet,0,sizeof(GxBookGet));
	book_get_msg_id = app_send_msg(GXMSG_BOOK_GET,&type);//获取预约消息
	if(book_get_msg_id)
	{
		pmsg = app_get_msg(book_get_msg_id);//获取数据
		memcpy(pEpgBookGet , (GxBookGet*)(pmsg->param),sizeof(GxBookGet));
		app_free_msg(GXMSG_BOOK_GET,book_get_msg_id);//释放数据空间
	}
	for(i=0;i<pEpgBookGet->book_number;i++)
	{
		if(book_id == pEpgBookGet->book[i].id)
		{
			*ri_Sel = i;
			
			return 0;
		}
	}
	return -1;
}
//预约同步
int8_t app_book_sync(GxBookGet* pEpgBookGet,GxBookType type)
{
	uint32_t i,j;
	int8_t ret=0;
	int32_t book_get_msg_id;
	app_msg* pmsg;
	GxBook pBookTemp;
	book_play bookPlay1={0};
	book_play bookPlay2={0};

	if (NULL == pEpgBookGet)
		return -1;
		
	memset(pEpgBookGet,0,sizeof(GxBookGet));
	book_get_msg_id = app_send_msg(GXMSG_BOOK_GET,&type);//获取预约消息
	if(book_get_msg_id)
	{
		pmsg = app_get_msg(book_get_msg_id);//获取数据
		memcpy(pEpgBookGet , (GxBookGet*)(pmsg->param),sizeof(GxBookGet));
		app_free_msg(GXMSG_BOOK_GET,book_get_msg_id);//释放数据空间
		for(i = 0; i<pEpgBookGet->book_number; i++)
		{
			if(BOOK_TRIG_OFF == pEpgBookGet->book[i].trigger_mode)
			{
				app_book_delete(&(pEpgBookGet->book[i]));
				ret = -1;
			}
			if((BOOK_TRIG_ING == pEpgBookGet->book[i].trigger_mode )
				&&(pEpgBookGet->book[i].book_type == BOOK_POWER_ON||pEpgBookGet->book[i].book_type == BOOK_POWER_OFF
				||BOOK_PROGRAM_PLAY == pEpgBookGet->book[i].book_type))
			{
				app_book_delete(&(pEpgBookGet->book[i]));
				ret = -1;
			}
		}
        //重新获取数据
		if(ret == -1)
		{
			book_get_msg_id = app_send_msg(GXMSG_BOOK_GET,&type);
			if(book_get_msg_id)
			{
				pmsg = app_get_msg(book_get_msg_id);
				memcpy(pEpgBookGet , (GxBookGet*)(pmsg->param),sizeof(GxBookGet));
				app_free_msg(GXMSG_BOOK_GET,book_get_msg_id);
			}
			else
			{
				return -1;
			}
		}
		/*
		*   预约排序(选择排序) ,根据时间先后顺序
		*/
		if (pEpgBookGet->book_number > 1)
			{
				for (i =0; i< pEpgBookGet->book_number-1;i++)
					{
						for (j=i+1; j<pEpgBookGet->book_number;j++)
							{
								memcpy(&bookPlay1,pEpgBookGet->book[i].struct_buf,sizeof(book_play));
								memcpy(&bookPlay2,pEpgBookGet->book[j].struct_buf,sizeof(book_play));
								if (bookPlay1.start_time > bookPlay2.start_time)
									{
										memcpy(&pBookTemp,(GxBook  *)&pEpgBookGet->book[i],sizeof(GxBook));
										memcpy((GxBook  *)&pEpgBookGet->book[i],(GxBook  *)&pEpgBookGet->book[j],sizeof(GxBook));
										memcpy((GxBook  *)&pEpgBookGet->book[j],&pBookTemp,sizeof(GxBook));
									}
							}
					}				
			}
	
		return 0 ;
	}
	return -1;
}

void* app_book_starttime_check_exist(GxBookType book_type,uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet)
{
	uint32_t i;
	book_play bookPlay={0};

	if (NULL == pEpgBookGet)
		return NULL;
	

	for(i = 0; i<pEpgBookGet->book_number; i++)
	{
		memcpy(&bookPlay,pEpgBookGet->book[i].struct_buf,sizeof(book_play));
		if(pEpgBookGet->book[i].trigger_mode != BOOK_TRIG_OFF
			&&book_type == pEpgBookGet->book[i].book_type
			&&prog_id != bookPlay.prog_id
			&&start_time == bookPlay.start_time )
		{
			return (void*)&pEpgBookGet->book[i];
		}
	}
	return NULL;
}

void* app_book_channel_check_exist(GxBookType book_type,uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet)
{
	uint32_t i;
	book_play bookPlay={0};

	if (NULL == pEpgBookGet)
		return NULL;


	for(i = 0; i<pEpgBookGet->book_number; i++)
	{
		memcpy(&bookPlay,pEpgBookGet->book[i].struct_buf,sizeof(book_play));
		if(pEpgBookGet->book[i].trigger_mode != BOOK_TRIG_OFF
			&&pEpgBookGet->book[i].book_type ==book_type
			&&prog_id == bookPlay.prog_id
			&&start_time == bookPlay.start_time )
		{
			return (void*)&pEpgBookGet->book[i];
		}
	}
	return NULL;
}
//创建节目预约
int8_t app_book_create(GxBookType type,uint32_t prog_id,
	                                   uint32_t event_id,
	                                   time_t start_time,
	                                   time_t end_time,
	                                   uint8_t *event_name)
{
	GxBook book={0};
	book_play bookplay;
	GxBookGet EpgBookGet;

	if ((BOOK_POWER_OFF != type)&&(BOOK_POWER_ON != type))
		{
			if (NULL == event_name)
				return 0;		
		}

	
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
	
	book.book_type = type;
	book.trigger_mode = BOOK_TRIG_BY_SEGMENT;
	book.repeat_mode.mode = BOOK_REPEAT_ONCE;
	book.trigger_time_start = start_time;
	book.trigger_time_end = end_time;

	book.struct_size = sizeof(book_play);	
	memset(&bookplay,0,sizeof(book_play));
	bookplay.start_time = start_time;
	bookplay.end_time = end_time;
	if ((BOOK_POWER_OFF != type)&&(BOOK_POWER_ON != type))
	{
		bookplay.prog_id = prog_id;
		bookplay.event_id = event_id;
		memset(bookplay.event_name,0,36);
		if (strlen((char*)event_name)>=35)
			memcpy(bookplay.event_name, event_name, 35);
		else
			memcpy(bookplay.event_name, event_name, strlen((char*)event_name));				
	}
	memcpy((void*)(book.struct_buf),(void*)&bookplay,sizeof(book_play));
	
	app_send_msg(GXMSG_BOOK_CREATE,(void*)&book);
	
	return 0;
}

int8_t app_book_modify(GxBook* pbook)
{
	if (NULL == pbook)
		return 0;
	
	app_send_msg(GXMSG_BOOK_MODIFY,(void*)pbook);
	return 0;
}

int8_t app_book_delete(GxBook* pbook)
{
	if (NULL == pbook)
		return 0;
	
	app_send_msg(GXMSG_BOOK_DESTROY,(void*)pbook);
	return 0;
}

int8_t app_book_clear_all(void)
{
	app_send_msg(GXMSG_BOOK_RESET,NULL);
	return 0;
}

/*
* 删除对应TP下所有节目的预约
*/
void app_book_delete_tp(search_fre_list searchlist)
{
	uint32_t i,j;
	GxBusPmViewInfo sysinfo;
	GxBusPmViewInfo sysoldinfo;
	GxBookGet EpgBookGet;
	book_play bookplay;
	GxBusPmDataTP tp = {0};
	GxBusPmDataProg Prog;


	
	GxBus_PmViewInfoGet(&sysinfo);
	memcpy(&sysoldinfo,&sysinfo,sizeof(GxBusPmViewInfo));
	sysinfo.group_mode = GROUP_MODE_ALL;
	sysinfo.stream_type = GXBUS_PM_PROG_ALL;
	sysinfo.fav_id = 0;
	sysinfo.skip_view_switch = VIEW_INFO_SKIP_CONTAIN;
	GxBus_PmViewInfoModify(&sysinfo);
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
	//	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
	for(i=0;i<EpgBookGet.book_number;i++)
	{
		memcpy((void*)&bookplay,(void*)(EpgBookGet.book[i].struct_buf),sizeof(book_play));
		GxBus_PmProgGetById(bookplay.prog_id,&Prog);
		GxBus_PmTpGetById(Prog.tp_id, &tp);
		for(j = 0;j<searchlist.num;j++)
			{
				if (tp.frequency == searchlist.app_fre_array[j])
					{
						printf("delete book %d fre = %d\n",i,tp.frequency);
						app_book_delete(&EpgBookGet.book[i]);
					}
			}
	}

	GxBus_PmViewInfoModify(&sysoldinfo);
	
	return;
}


void app_book_delete_prog(uint32_t prog_id)
{
	GxBookGet EpgBookGet;
	uint32_t i;
	book_play bookplay;

	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
//	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
	for(i=0;i<EpgBookGet.book_number;i++)
	{
		memcpy((void*)&bookplay,(void*)(EpgBookGet.book[i].struct_buf),sizeof(book_play));
		if(prog_id == bookplay.prog_id)
		{
			app_book_delete(&EpgBookGet.book[i]);
//			GxCore_ThreadDelay(50);		
		}
	}
	return ;
}

void app_book_trigger_start(GxBook* pbook)
{
/*	int32_t times;
	book_play book1;*/

	if (NULL == pbook)
		return;

	if(pbook->book_type == BOOK_POWER_OFF)
	{
		app_book_delete(pbook);
		/*
		* 待机
		*/
/*		memcpy(&book1, pbook->struct_buf, sizeof(book_play));
		times = book1.end_time - book1.start_time;
		if (times <=0)
			times = 0;
		app_lower_power_entry(times);*/
	}
	else if(pbook->book_type == BOOK_PROGRAM_PLAY )//预约播放节目
	{
		memcpy((void*)&book_trigger_start,(void*)pbook,sizeof(GxBook));
	}
	else if (pbook->book_type == BOOK_TYPE_1)
	{
		memcpy((void*)&book_trigger_start,(void*)pbook,sizeof(GxBook));
	}
	
	printf("********start********\n");
}

void app_book_trigger_end(GxBook* pbook)
{
	if (NULL == pbook)
		return;

	if(pbook->book_type == BOOK_PROGRAM_PLAY )
	{
		memcpy((void*)&book_trigger_end,(void*)pbook,sizeof(GxBook));
	}
	else if (pbook->book_type == BOOK_TYPE_1)
	{
		memcpy((void*)&book_trigger_end,(void*)pbook,sizeof(GxBook));
	}
	
	
	printf("********end********\n");
}

/*预约播放*/
//200ms 全屏定时器中轮训
int8_t app_book_query_play(void)
{
	uint32_t prog_id;
	int32_t ret_pop = FALSE; 
	int32_t ret; 
	int32_t pos;
	uint32_t i;
	uint8_t group_mode = 0;
	uint8_t stream_type =0;
	uint8_t fav_id = 0;
	//HotplugPartitionList* partition_list=NULL;
	GxBusPmDataProg  prog;
	GxBusPmViewInfo sysinfo;
	GxBookGet EpgBookGet;
	static uint8_t busy = 0;
	book_play book;
	book_play book1;

	int RetValue = 0;

	static uint8_t ubooking = 0;
	GxTime temp;

	if (1 == ubooking)
		{
			/*如果提示预约框状态，不响应新的预约*/
			return FALSE;
		}

	if (1 == app_play_get_popmsg_status())
	{
		return FALSE;
	}
	/* 为了在系统设置里面退出设置预约框后,判断事件是否过期*/
	if (0xffffffff != book_trigger_start.trigger_time_start)
	{
		memset(&temp, 0, sizeof(GxTime));
		GxCore_GetLocalTime(&temp);

		if (temp.seconds >= book_trigger_start.trigger_time_end)
		{
			app_book_delete(&book_trigger_start);
			book_trigger_start.trigger_time_start = 0xffffffff;

			printf("\n------------------->:BOOK:TIME OUT --TIME OUT--TIME OUT.\n");

			busy = 0;

			return FALSE;
		}
		/* 添加判断是否手动删除 #fy  43511 */
		extern int app_book_GetDelFlag(void);
		if (app_book_GetDelFlag())
		{
			int sel = 0;
			if(-1 == app_book_check_exist(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1,book_trigger_start.id,&sel))
			{
				busy = 0;
				book_trigger_start.trigger_time_start = 0xffffffff;
				return FALSE;
			}
		}
	}
    //执行预约任务
	if(book_trigger_start.trigger_time_start != 0xffffffff && busy == 0)
	{
		busy = 1;
		book_trigger_start.trigger_time_start = 0xffffffff;
		app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);
//		app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
		memcpy(&book1, book_trigger_start.struct_buf, sizeof(book_play));
		prog_id = book1.prog_id;
		
		GxBus_PmProgGetById(prog_id, &prog);
		ret = app_prog_find_by_id(prog_id,&pos,&group_mode,&stream_type,&fav_id);
		if(prog.skip_flag == 1)
		{
			ret = -1;
		}
		if(ret == -1)
		{
			/*节目类表中找不到预约的节目*/
			for(i=0;i<EpgBookGet.book_number;i++)
			{
				if(book_trigger_start.id == EpgBookGet.book[i].id)
				{
					app_book_delete(&EpgBookGet.book[i]);
				}
			}
			busy = 0;
			return FALSE;
		}
		if(book_trigger_start.book_type == BOOK_PROGRAM_PLAY)//预约播放
		{
			GxBusPmDataProg   prog = {0, };
			memcpy(&book, book_trigger_start.struct_buf, MAX_BOOK_STRUCT_LEN);
			RetValue = GxBus_PmProgGetById(book.prog_id, &prog);//根据节目号查找节目信息

			if(GXCORE_SUCCESS == RetValue)
			{
				ubooking = 1;	
				if (NULL != app_book_play_popmsg_callback)//确认是否播放
					{
						ret_pop = app_book_play_popmsg_callback(book,prog);
					}
				ubooking = 0;		
			}
			else
			{
				return FALSE;
			}
		}
		else  if(book_trigger_start.book_type == BOOK_TYPE_1) 
		{
			GxBusPmDataProg   prog = {0, };
			memcpy(&book, book_trigger_start.struct_buf, MAX_BOOK_STRUCT_LEN);
			RetValue = GxBus_PmProgGetById(book.prog_id, &prog);

			if(GXCORE_SUCCESS == RetValue)
			{
				ubooking = 1;	
				if (NULL != app_book_record_popmsg_callback)
					{
						ret_pop = app_book_record_popmsg_callback(book,prog);
					}
				ubooking = 0;		
			}
			else
			{
				return FALSE;
			}
		}
		else 
		{
			busy = 0;
			return FALSE;
		}
		

		if(ret_pop == TRUE)//确认播放
		{
			/* clear signal message */
			app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);

			for(i=0;i<EpgBookGet.book_number;i++)
			{
				if(book_trigger_start.id == EpgBookGet.book[i].id)
				{
					GxBus_PmViewInfoGet(&sysinfo);
					if(sysinfo.stream_type == GXBUS_PM_PROG_TV)
					{
						memcpy(&book1, EpgBookGet.book[i].struct_buf, sizeof(book_play));
						book1.prog_id_recall = sysinfo.tv_prog_cur;
						memcpy(EpgBookGet.book[i].struct_buf,&book1, sizeof(book_play));					
					}
					else
#if (1 == DVB_HD_LIST)
						if (sysinfo.stream_type == GXBUS_PM_PROG_HD_SERVICE)
							{
								memcpy(&book1, EpgBookGet.book[i].struct_buf, sizeof(book_play));
								book1.prog_id_recall = sysinfo.hd_prog_cur;
								memcpy(EpgBookGet.book[i].struct_buf,&book1, sizeof(book_play));					
							}
					else
#endif
					{
						memcpy(&book1, EpgBookGet.book[i].struct_buf, sizeof(book_play));
						book1.prog_id_recall = sysinfo.radio_prog_cur;
						memcpy(EpgBookGet.book[i].struct_buf,&book1, sizeof(book_play));						
					}
					app_book_modify(&EpgBookGet.book[i]);
				}
				else if(EpgBookGet.book[i].trigger_mode == BOOK_TRIG_ING)
				{
					app_book_delete(&EpgBookGet.book[i]);
				}
			}
/*			if(book_trigger_start.book_type == BOOK_TYPE_1) 
			{
				app_send_msg(GXMSG_PLAYER_RECORD, &pos);
			}*/
			app_play_switch_prog_clear_msg();

			/*
			* 切换到查找到对应节目的group_mode,stream_type,fav_id组
			*/
			if(prog.service_type == GXBUS_PM_PROG_NVOD)
			{
				app_nvod_set_full_screen_play_flag(TRUE);
				app_prog_change_group(group_mode, stream_type,fav_id);
				app_play_video_audio(pos);
				app_nvod_set_playing_pos(pos);

				if(book_trigger_start.book_type == BOOK_TYPE_1) 
				{
					GUI_Event event = {0};

					event.type = GUI_KEYDOWN;
					event.key.sym = APP_KEY_RED;
					GUI_SendEvent("win_full_screen", &event);				
				}
				busy = 0;
				return TRUE;
			}
			if(ret == 0)
			{
				/*当前节目类表中找到预约的节目*/
				app_prog_change_group(group_mode, stream_type,fav_id);
				app_prog_save_playing_pos_in_group(pos);
				app_play_reset_play_timer(0);

				if(book_trigger_start.book_type == BOOK_TYPE_1) 
				{
					GUI_Event event = {0};

					event.type = GUI_KEYDOWN;
					event.key.sym = APP_KEY_RED;
					GUI_SendEvent("win_full_screen", &event);
				}
				busy = 0;
				return TRUE;
			}
			else if(ret == 1)
			{
				/*由喜爱观看状态，切换到预约观看状态*/
				app_prog_change_group(group_mode, stream_type,fav_id);
				app_prog_save_playing_pos_in_group(pos);
				app_prog_record_playing_pos(pos);
				app_play_reset_play_timer(0);

				if(book_trigger_start.book_type == BOOK_TYPE_1) 
				{
					GUI_Event event = {0};

					event.type = GUI_KEYDOWN;
					event.key.sym = APP_KEY_RED;
					GUI_SendEvent("win_full_screen", &event);
				}
				busy = 0;
				return TRUE;

			}
			else if(ret == 2)
			{
				/*电视广播切换，播放预约观看节目*/
				app_prog_change_group(group_mode, stream_type,fav_id);
				app_prog_save_playing_pos_in_group(pos);
				app_prog_record_playing_pos(pos);
				app_play_reset_play_timer(0);

				if(book_trigger_start.book_type == BOOK_TYPE_1) 
				{
					GUI_Event event = {0};

					event.type = GUI_KEYDOWN;
					event.key.sym = APP_KEY_RED;
					GUI_SendEvent("win_full_screen", &event);
				}
				busy = 0;
				return TRUE;

			}	
			busy = 0;
			return FALSE;
			
		}
		else
		{
			for(i=0;i<EpgBookGet.book_number;i++)
			{
				if(EpgBookGet.book[i].trigger_mode == BOOK_TRIG_ING)
				{
					app_book_delete(&EpgBookGet.book[i]);
				}
			}	
			busy = 0;
			return FALSE;
		}
		busy = 0;
	}	
	else if(book_trigger_end.trigger_time_start != 0xffffffff && busy == 0)
	{
		busy = 1;
		book_trigger_end.trigger_time_start = 0xffffffff;

		memcpy(&book1, book_trigger_end.struct_buf, sizeof(book_play));
		prog_id = book1.prog_id_recall;		
		GxBus_PmProgGetById(prog_id, &prog);
		ret = app_prog_find_by_id(prog_id,&pos,&group_mode,&stream_type,&fav_id);
		if(prog.skip_flag == 1)
		{
			ret = -1;
		}
		if(ret == -1)
		{
			busy = 0;
			return FALSE;	
		}
		return FALSE;//zhuangzh note
	}
	busy = 0;
	return FALSE;
}

int32_t app_book_check_trig_ing(void)
{
	uint32_t i;
	GxBookGet EpgBookGet;
	
	app_book_sync(&EpgBookGet,BOOK_PROGRAM_PLAY|BOOK_TYPE_1);

	for(i=0;i<EpgBookGet.book_number;i++)
	{
		if( EpgBookGet.book[i].trigger_mode == BOOK_TRIG_ING)
		{
			return 0;
		}
	}
	return -1;
}

int32_t app_book_add_event(event_book_para eventbookpara)
{
	GxBook* pbooktmp=NULL;
	GxTime time;
	int32_t ret = FALSE;

	if (NULL == eventbookpara.pEpgBookGet)
		return BOOK_STATUS_PARA_ERROR;	

	if ((BOOK_POWER_OFF != eventbookpara.book_type)&&(BOOK_POWER_ON != eventbookpara.book_type))
		{
			pbooktmp = app_book_channel_check_exist(eventbookpara.book_type,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet);
			if(pbooktmp)
			{
				/*
				* 如果预约已存在，删除预约。(预约/取消预约功能)
				*/
				app_book_delete(pbooktmp);
				app_book_sync(eventbookpara.pEpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
				if (app_book_add_event_popmsg_callback)
					{
						app_book_add_event_popmsg_callback(BOOK_STATUS_EXIST,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet,pbooktmp);
					}
				return BOOK_STATUS_EXIST;
			}		
		}


	GxCore_GetLocalTime(&time);
	if(eventbookpara.start_time < time.seconds)
		{
			if (app_book_add_event_popmsg_callback)
				{
					app_book_add_event_popmsg_callback(BOOK_STATUS_OVERDUE,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet,pbooktmp);
				}
			return BOOK_STATUS_OVERDUE;
		}
	
		/*
		* 判断如存在相同开始时间的预约
		* 可选择替换或取消
		*/
		pbooktmp = app_book_starttime_check_exist(eventbookpara.book_type,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet);
		if (pbooktmp)
			{
				if (app_book_add_event_popmsg_callback)
					{
						ret = app_book_add_event_popmsg_callback(BOOK_STATUS_CONFILCT,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet,pbooktmp);
					}
				if (TRUE != ret)
					{
						return BOOK_STATUS_CONFILCT;
					}
				else//替换原有预约
					{
						app_book_delete(pbooktmp);//先删除
						app_book_sync(eventbookpara.pEpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);
					}
			}
/*		else
			{
				app_book_sync(eventbookpara.pEpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);			
			}*/
		

		if ( eventbookpara.pEpgBookGet->book_number >= MAX_BOOK_NUM)
		{
			/*
			* 预约超过最大个数
			*/
			if (app_book_add_event_popmsg_callback)
				{
					ret = app_book_add_event_popmsg_callback(BOOK_STATUS_FULL,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet,pbooktmp);
				}
			return BOOK_STATUS_FULL;
		}
        //创建新的预约
		ret = app_book_create(	eventbookpara.book_type,
							eventbookpara.prog_id,
							eventbookpara.event_id,
							eventbookpara.start_time,
							eventbookpara.end_time,
							eventbookpara.event_name);
		app_book_sync(eventbookpara.pEpgBookGet,BOOK_PROGRAM_PLAY|BOOK_POWER_ON|BOOK_POWER_OFF|BOOK_TYPE_1);

		if (app_book_add_event_popmsg_callback)
			{
				ret = app_book_add_event_popmsg_callback(BOOK_STATUS_OK,eventbookpara.prog_id,eventbookpara.start_time,eventbookpara.pEpgBookGet,pbooktmp);
			}
	return BOOK_STATUS_OK;

	
}




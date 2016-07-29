/**
 *
 * @file        app_common_search.c
 * @brief
 * @version     1.1.0
 * @date        10/18/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include <gxtype.h>
#include "app_common_search.h"
#include "app_common_epg.h"
#include "app_common_flash.h"
#include "app_common_prog.h"
#include "gxbook.h"
#include "gxfrontend.h"
#include "module/si/si_public.h"
#include "app_common_panel.h"
#include "app_common_table_nit.h"
#include "app_common_table_pmt.h"
#include "app_common_table_cat.h"
#include "app_common_table_bat.h"
#include "app_common_table_ota.h"
#include "app_common_book.h"
#include "app_common_lcn.h"
#include <sys/ioctl.h>
#include "gui_core.h"
#include "app_common_porting_ca_demux.h"
#include "app_common_porting_stb_api.h"
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_BY_FLAG
#include "app_by_cas_api_demux.h"
#endif
#endif




/************************************************************************/
/* 函数声明，防止编译器警告                                               */
/************************************************************************/
 handle_t         sApp_frontend_device_handle;
 handle_t         sApp_frontend_demux_handle;

search_fre_list searchFreList ;
static search_extend searchExtendList ;
search_result searchresultpara;
static uint32_t cur_fre =0;
static search_add_extend_table app_search_add_extend_callback = NULL;

uint8_t gAutoSearchFlag = TRUE; // 无节目是否需要自动搜索标志
                                             // 该值应用可根据需求设置。如恢复出厂设置，NIT版本变更等


void app_search_register_add_extend_table_callback(search_add_extend_table search_extend_call_back)
{
	if (NULL != search_extend_call_back)
		{
			app_search_add_extend_callback = search_extend_call_back;
		}
	return ;
}

/*
* 开启信号监测
*/
uint8_t app_demodu_monitor_start(void)
{
	GxMsgProperty_FrontendMonitor tuner = 0;
	app_send_msg(GXMSG_FRONTEND_START_MONITOR, &tuner);
	return 0;
}

/*
* 关闭信号监测
*/
uint8_t app_demodu_monitor_stop(void)
{
	GxMsgProperty_FrontendMonitor tuner = 0;
	app_play_clear_msg_pop_type(MSG_POP_SIGNAL_BAD);
	app_send_msg(GXMSG_FRONTEND_STOP_MONITOR, &tuner);
	return 0;
}
uint8_t app_stop_all_monitor_filter(void)
{
	/*
	 * 搜索节目、进入多媒体
	 * 释放后台所有监测表FILTER、信号检测等
	 */

	 app_epg_close();
	 GxFrontend_StopMonitor(0);
	 app_table_nit_search_filter_close();
	 app_table_nit_monitor_filter_close();
	 app_table_bat_filter_close();
	 app_table_cat_filter_close();
	 app_table_ota_monitor_filter_close();
	 app_table_pmt_filter_close();
#ifdef DVB_AD_TYPE_CMIS_FLAG
	APP_CMIS_AD_Close();
#endif
#ifdef DVB_AD_TYPE_3H_FLAG
	#include"3h_ads_porting.h"
	GxAD_ItemFilterClose();
#endif
#ifdef CA_FLAG
	 app_cas_api_release_ecm_filter();
	 app_cas_api_release_emm_filter();
#ifdef DVB_CA_TYPE_BY_FLAG
	{
		int32_t dvb_ca_flag= 0;
		 dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
		 if(DVB_CA_TYPE_BY == dvb_ca_flag)
			 {
				 app_by_cas_api_release_bat_filter();
			 }
	}
#endif
#ifdef DVB_CA_TYPE_QILIAN_FLAG
	{
		int32_t dvb_ca_flag= 0;
		dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
		 if(DVB_CA_TYPE_QILIAN == dvb_ca_flag)
		 {
			 app_qilian_cas_api_release_bat_filter();
		 }
	}
#endif
#endif
	  app_porting_disable_query_demux();
	return 0;
}

uint8_t app_start_all_monitor_filter(void)
{
        app_epg_open();
		app_porting_enable_query_demux();
		GxFrontend_StartMonitor(0);
		if (0 == app_prog_get_num_in_group())
		{
			/*
			* 无节目开启NIT监控FILTER，节目存在切台开启NIT监控FILTER，
			* 避免多次开启
			*/
			app_table_nit_monitor_filter_open();
			app_table_ota_monitor_filter_restart();
		}
		return 0;
}



/*
* 搜索过程中接受到退出键，发送退出消息到GXBUS停止搜索
*/
uint8_t app_search_scan_stop(void)
{
	app_send_msg(GXMSG_SEARCH_SCAN_STOP,NULL);
	return 0;
}

/*
* 锁频
* fre - 频率(M)
* symb - 符号率(M)
* inversion -- 反转
* modulation -- 调试方式
*/
status_t app_search_lock_tp(uint32_t fre, uint32_t symb, fe_spectral_inversion_t inversion, fe_modulation_t modulation,uint32_t delayms)
{
	GxFrontend frontend={0};
	int ret = 0;
	int i =0;
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	GxBusPmDataSat Sat = {0,};
	GxBus_PmSatsGetByPos(0,1,&Sat);
#endif
	
	frontend.dev = sApp_frontend_device_handle;
	//frontend.frontend = sApp_frontend_module_handle;
	frontend.demux = sApp_frontend_demux_handle;
	frontend.tuner = 0;
	frontend.fre = fre;
	frontend.symb = symb*1000;
	frontend.qam = modulation+1;

#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	frontend.type = FRONTEND_DTMB;
	frontend.type_1501 = Sat.sat_dtmb.work_mode;
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
	frontend.type = FRONTEND_DVB_C;
#endif
	
	printf("\n\n\n\n app_search_lock_tp begin \n\n\n");

	GxFrontend_SetTp(&frontend);//锁频操作
	printf("[%s]:%d\n",__FUNCTION__,__LINE__);

	if (0 == delayms)
		{
			/*
			* 不需要等待锁频结果返回，如输入频点、加锁节目锁频等
			*/
		
			return 0;
		}
	
	
#if 1		
	while(1)
	{
		ret = GxFrontend_QueryFrontendStatus(0);//获取状态
		if(ret <=0)
		{
			GxCore_ThreadDelay(10);	
			i++;
		}
		else if(ret == 1)
		{
			printf("\n [app_common]app_search_lock_tp lock  \n\n");
			break;
		}
		if(i == delayms/10)
		{
			break;
		}
	}
#endif	
	if(i == delayms/10)
	{
		
		printf("\n [app_common]app_search_lock_tp unlock	\n\n");
		return 1;
	}
	return 0;
}


/*
* 搜索
* searchlist - 搜索参数，频率、符号率、调制方式列表，频点个数等
*/
void app_search_scan_cable_start(search_fre_list searchlist)
{
	GxBusPmDataSat sat_arry={0};
	GxBusPmDataTP tp={0};
	uint16_t i=0;

	uint16_t tpid_temp;
	uint32_t sat_id = 0;
	uint32_t tpid_temp1;
	int32_t ret;
	static uint32_t ts_id = DVB_TS_SRC;
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	GxMsgProperty_ScanDtmbStart dtmb_scan={0,};
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
	GxMsgProperty_ScanCableStart dvbc_scan={0,};
#endif
	static GxSearchExtend BatParse[SEARCH_EXTEND_MAX];
	memset(&searchresultpara,0,sizeof(search_result));
	app_stop_all_monitor_filter();

//	app_lcn_list_clear();

	app_send_msg(GXMSG_SEARCH_BACKUP, NULL);
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)	
	memset(&dtmb_scan,0,sizeof(GxMsgProperty_ScanDtmbStart));
#elif (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)	
	memset(&dvbc_scan,0,sizeof(GxMsgProperty_ScanCableStart));
#endif


	ret = GxBus_PmSatsGetByPos(sat_id,1,&sat_arry);
	if(ret == -1)
	{
		//sat获取不成功
		printf("can't get Sat\n");
		return;
	}

	/*
	* 首先删除需搜索频点的预约
	*/
	
	app_book_delete_tp(searchlist);
	
	

	for(i = 0;i<searchlist.num;i++)
	{
		ret = GxBus_PmTpExistChek(sat_arry.id, searchlist.app_fre_array[i], searchlist.app_symb_array[i]*1000, 0, searchlist.app_qam_array[i]+1,&tpid_temp);
		if(ret == 0)
		{
			tp.sat_id = sat_arry.id;
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			tp.frequency = searchlist.app_fre_array[i];
			{
				tp.tp_c.modulation  = searchlist.app_qam_array[i]+1;
				tp.tp_c.symbol_rate = searchlist.app_symb_array[i]*1000;
			}
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			tp.frequency = searchlist.app_fre_array[i];
			{
				tp.tp_dtmb.modulation  = searchlist.app_qam_array[i]+1;
				tp.tp_dtmb.symbol_rate = searchlist.app_symb_array[i]*1000;
			}
#endif
			ret = GxBus_PmTpAdd(&tp);
			if (GXCORE_SUCCESS != ret)
				{
					//tp加入不成功
					printf("can't add Tp\n");
					break;					
				}
			
			searchresultpara.app_tpid[i] = tp.id;
		}
		else if(ret > 0)
		{
			tp.sat_id = sat_arry.id;
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
			tp.frequency = searchlist.app_fre_array[i];
			{
				tp.tp_c.modulation	= searchlist.app_qam_array[i]+1;
				tp.tp_c.symbol_rate = searchlist.app_symb_array[i]*1000;
			}
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
			tp.frequency = searchlist.app_fre_array[i];
			{
				tp.tp_dtmb.modulation  = searchlist.app_qam_array[i]+1;
				tp.tp_dtmb.symbol_rate = searchlist.app_symb_array[i]*1000;
			}
#endif

			tpid_temp1 = tpid_temp;
//			app_book_delete_tp(tpid_temp1);
			ret = GxBus_PmTpDelete(&tpid_temp1, 1);
			ret = GxBus_PmTpAdd(&tp);
			
			if (GXCORE_SUCCESS != ret)
				{
					//tp加入不成功
					printf("can't add Tp\n");
					break;					
				}

			searchresultpara.app_tpid[i] = tp.id;
		}
		else
		{
			//tp加入不成功
			printf("can't add Tp\n");
			return;
		}
	}
	GxBus_PmSync(GXBUS_PM_SYNC_TP);

	if (0 == i)
		return;
	searchresultpara.tp_num = i;
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)	
	dvbc_scan.scan_type = GX_SEARCH_MANUAL;
	dvbc_scan.tv_radio = GX_SEARCH_TV_RADIO_ALL;
	dvbc_scan.fta_cas = GX_SEARCH_FTA_CAS_ALL;
	dvbc_scan.nit_switch = GX_SEARCH_NIT_DISABLE;
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	dtmb_scan.scan_type = GX_SEARCH_MANUAL;
	dtmb_scan.tv_radio = GX_SEARCH_TV_RADIO_ALL;
	dtmb_scan.fta_cas = GX_SEARCH_FTA_CAS_ALL;
	dtmb_scan.nit_switch = GX_SEARCH_NIT_DISABLE;
#endif

	/*
	* 添加扩展表过滤条件,需改成回调函数
	*/
	if (NULL !=  app_search_add_extend_callback)
		app_search_add_extend_callback(&searchExtendList);

	if (searchExtendList.extendnum > 0)
	{
		memset(BatParse,0,SEARCH_EXTEND_MAX*sizeof(GxSearchExtend));
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
		dvbc_scan.ext_num = searchExtendList.extendnum;
		memcpy(&BatParse[0],&searchExtendList.searchExtend[0],SEARCH_EXTEND_MAX*sizeof(GxSearchExtend));
		dvbc_scan.ext = &BatParse[0];
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
		dtmb_scan.ext_num = searchExtendList.extendnum;
		memcpy(&BatParse[0],&searchExtendList.searchExtend[0],SEARCH_EXTEND_MAX*sizeof(GxSearchExtend));
		dtmb_scan.ext = &BatParse[0];
#endif
	}

	/*
	 * 搜索节目前，初始化逻辑频道号列表
	 */
	app_lcn_list_init();
#ifdef CA_FLAG
#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
	dvbc_scan.check_ca_fun = app_cas_api_check_bus_pmt_ca_valid;
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	dtmb_scan.check_ca_fun = app_cas_api_check_bus_pmt_ca_valid;
#endif
#endif

#if (DVB_DEMOD_DVBC == DVB_DEMOD_MODE)
	dvbc_scan.params_c.sat_id = sat_arry.id;
	dvbc_scan.params_c.max_num = i;//searchlist.num;
	dvbc_scan.params_c.array = searchresultpara.app_tpid;
//	dvbc_scan.params_c.array = app_tpid;
	dvbc_scan.params_c.ts = &ts_id;
	dvbc_scan.time_out.pat = PAT_FILTER_TIMEOUT; 
	dvbc_scan.time_out.sdt = SDT_FILTER_TIMEOUT; 
	dvbc_scan.time_out.nit = NIT_FILTER_TIMEOUT; 
	dvbc_scan.time_out.pmt = PMT_FILTER_TIMEOUT; 

	app_send_msg(GXMSG_SEARCH_SCAN_CABLE_START,(void*)&dvbc_scan);//有线方案的搜索
#elif (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	dtmb_scan.params_dtmb.sat_id = sat_arry.id;
	dtmb_scan.params_dtmb.max_num = i;//searchlist.num;
	dtmb_scan.params_dtmb.array = searchresultpara.app_tpid;
//	dvbc_scan.params_c.array = app_tpid;
	dtmb_scan.params_dtmb.ts = &ts_id;
	dtmb_scan.time_out.pat = PAT_FILTER_TIMEOUT; 
	dtmb_scan.time_out.sdt = SDT_FILTER_TIMEOUT; 
	dtmb_scan.time_out.nit = NIT_FILTER_TIMEOUT; 
	dtmb_scan.time_out.pmt = PMT_FILTER_TIMEOUT; 

	app_send_msg(GXMSG_SEARCH_SCAN_DTMB_START,(void*)&dtmb_scan);
#endif
}

//设置搜索参数
void app_search_scan_manual_mode(uint32_t fre,uint32_t symbol_rate,uint32_t qam)
{
	//app_play_stop();
	memset(&searchFreList,0,sizeof(search_fre_list));	
	searchFreList.app_fre_array[0] = fre;		
	searchFreList.app_qam_array[0] = qam;
	searchFreList.app_symb_array[0] = symbol_rate;
	searchFreList.num = 1;
	searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
	app_lcn_list_clear();
	app_search_scan_cable_start(searchFreList);

	return ;
}

void app_search_scan_nit_mode(void)
{
	GxBusPmDataSat sat;
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint32_t fre = 0;

	symbol_rate = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	fre = app_flash_get_config_center_freq();

	app_epg_close();
//	app_pmt_close();
	app_table_bat_filter_close();
	app_table_nit_search_filter_close();
	app_play_stop();
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

	memset(&searchFreList,0,sizeof(search_fre_list));

	searchFreList.num = 1;
	searchFreList.app_fre_array[0] = fre;
	searchFreList.app_qam_array[0]=qam;
	searchFreList.app_symb_array[0]=symbol_rate;
	app_lcn_list_clear();
	app_table_nit_search_filter_open();

	return ;
	
}


/*
* 全频段搜索
* begin_fre -- 开始频率
*end_fre -- 结束频率
*/
uint32_t app_search_scan_all_mode(uint32_t begin_fre,uint32_t end_fre,uint32_t symbol_rate,uint32_t qam)
{
    uint16_t i;
    uint32_t fre;
    uint16_t freArrayCount =0;
    uint32_t end_fre_tmp = end_fre;
	search_fre_list* fre_list = NULL;
	fre_list = app_enum_search_get_fre_list();
	memset(&searchFreList,0,sizeof(search_fre_list));
	app_play_stop();

	if ((NULL != fre_list)&&(fre_list->num >0))
		{
			/*
			* 针对固定频点列表搜索方式
			*/
			memcpy(&searchFreList,fre_list,sizeof(search_fre_list));
			app_lcn_list_clear();
			app_search_scan_cable_start(searchFreList);
			return 0;
		}

#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	if(begin_fre<=52500)
	{
		begin_fre = 52500;
	}
	else if(begin_fre<=60500)
	{
		begin_fre = 60500;
	}
	else if(begin_fre<=68500)
	{
		begin_fre = 68500;
	}
	else if(begin_fre<=80000)
	{
		begin_fre = 80000;
	}
	else if(begin_fre<=88000)
	{
		begin_fre = 88000;
	}
	else if (begin_fre > 88000 && begin_fre < 171000)
	{
		begin_fre = 171000;
	}
	else if ((begin_fre >= 171000) && (begin_fre <= 219000))
	{
		begin_fre = (219000 - (abs(begin_fre - 219000)/8000)*8000);
	}
#elif (DVB_DEMOD_MODE == DVB_DEMOD_DVBC)
	if(begin_fre<=115000)
	{
		begin_fre = 115000;
	}
	else if(begin_fre<=467000)
	{
		begin_fre = (467000 - (abs(begin_fre - 467000)/8000)*8000);
	}	
#endif
	else if(begin_fre <474000)
	{
		begin_fre = 474000;
	}
	else
	{
		begin_fre = (858000 - (abs(begin_fre - 858000)/8000)*8000);
	}

#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	if(end_fre<= 52500)
	{
		end_fre = 52500;     
	}
	else if (end_fre > 52500 && end_fre < 60500)
	{
		end_fre = 52500;
	}
	else if (end_fre >= 60500 && end_fre < 68500)
	{
		end_fre = 60500;
	}
	else if (end_fre >= 68500 && end_fre < 80000)
	{
		end_fre = 68500;
	}
	else if (end_fre >= 80000 && end_fre < 88000)
	{
		end_fre = 80000;
	}
	else if (end_fre >= 88000 && end_fre < 171000)
	{
		end_fre = 88000;
	}
	else if ((end_fre >= 171000) && (end_fre <= 219000))
	{
		end_fre = (219000 - (abs(end_fre - 219000)/8000)*8000);
	}
	else if(end_fre < 474000)
	{
		end_fre = 219000;
	}
#elif (DVB_DEMOD_MODE == DVB_DEMOD_DVBC)
	if(end_fre<=467000)
	{
		end_fre = (467000 - (abs(end_fre - 467000)/8000)*8000);
        
        if(end_fre>end_fre_tmp)
        {
            end_fre -=8000;
        }
	}
	else if(end_fre < 474000)
	{
		end_fre = 467000;
	}
#endif	
	else if(end_fre <= 866000 )
	{
		end_fre = (866000 - (abs(end_fre - 866000)/8000)*8000);
        if(end_fre>end_fre_tmp)
        {
            end_fre -=8000;
        }
	}
	else
	{
		end_fre = 866000;
	}
		
	freArrayCount = 0;
	fre = begin_fre;
    
	for(i=0;i<TP_MAX_NUM;i++)
	{
#if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
		if(fre==76500)
		{
			fre = 80000;
		}
		else if (fre==96000)
		{
			fre = 171000;
		}
		else if (fre==227000)
		{
			fre = 474000;
		}
#elif (DVB_DEMOD_MODE == DVB_DEMOD_DVBC)
		if (fre==475000)
		{
			fre = 474000;
		}
#endif

		if (fre > end_fre)
			{
				break;
			}
        
		searchFreList.app_fre_array[i]=fre;
		searchFreList.app_qam_array[i]=qam;
		searchFreList.app_symb_array[i]=symbol_rate;
		freArrayCount++;

		{
			fre = fre +8000;
		}
	}

    printf("====All Search Freq Info,total:%d=====\n",freArrayCount);
	if (0 == freArrayCount)
	{
		return 1;
	}
	
/*    for(i=0;i<freArrayCount;i++)
    {
        printf("%d-----fre:%d\n",i+1,searchFreList.app_fre_array[i]);
    }*/
    
	searchFreList.num = freArrayCount;
	searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
	app_lcn_list_clear();
	app_search_scan_cable_start(searchFreList);
	return 0;
}
//特定频率搜索
uint32_t app_search_scan_mode(uint32_t *fre,uint16_t size,uint32_t symbol_rate,uint32_t qam)
{
    uint16_t i;
    
	memset(&searchFreList,0,sizeof(search_fre_list));
	for(i=0;i<size;i++)
	{
		searchFreList.app_fre_array[i]=fre[i];
		searchFreList.app_qam_array[i]=qam;
		searchFreList.app_symb_array[i]=symbol_rate;
		
	}
	searchFreList.num = size;
	searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
	app_lcn_list_clear();
	app_search_scan_cable_start(searchFreList);
	return 0;
}


/*指定频率表搜索*/
uint32_t app_search_enum_enum_mode(search_fre_list t_searchFreList)
{
	uint16_t i;

	memset(&searchFreList,0,sizeof(search_fre_list));
	for(i=0;i<t_searchFreList.num;i++)
	{
		searchFreList.app_fre_array[i]=t_searchFreList.app_fre_array[i];
		searchFreList.app_qam_array[i]=t_searchFreList.app_qam_array[i];
		searchFreList.app_symb_array[i]=t_searchFreList.app_symb_array[i];
	}
	printf("enum_all_mode %d,%d,%d\n",searchFreList.app_fre_array[1],searchFreList.app_qam_array[1],searchFreList.app_symb_array[1]);
	searchFreList.num = t_searchFreList.num;
	searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
	app_lcn_list_clear();
	app_search_scan_cable_start(searchFreList);
	return 0;
}
#ifdef DVB_CA_TYPE_MG312_FLAG
#include "mg312def.h"
#ifdef MG_CAS_VER_3_1_1
int app_win_fengyang_search(void)
{
	search_fre_list t_searchFreList={{474000,626000,706000,714000,730000},{2,2,2,2,2},{6875,6875,6875,6875,6875},{0,0,0,0,0},5,1};
	GUI_CreateDialog("win_search_result");
	GUI_SetInterface("flush", NULL);
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}

int app_win_wudian_search(void)
{
	search_fre_list t_searchFreList={{746000,762000,770000,794000,802000},{2,2,2,2,2},{6875,6875,6875,6875,6875},{0,0,0,0,0},5,1};
	GUI_CreateDialog("win_search_result");
	GUI_SetInterface("flush", NULL);
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}

int app_win_feng_wu_search(void)
{
	search_fre_list t_searchFreList={{474000,626000,706000,714000,730000,746000,762000,770000,794000,802000},{2,2,2,2,2,2,2,2,2,2},{6875,6875,6875,6875,6875,6875,6875,6875,6875,6875},\
				{0,0,0,0,0,0,0,0,0,0},10,1};
	GUI_CreateDialog("win_search_result");
	GUI_SetInterface("flush", NULL);
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}
#endif
#endif

#ifdef DVB_CA_TYPE_DVB_FLAG


int app_win_hunan_linli_search(void)
{
	search_fre_list t_searchFreList={{746000,754000,770000,778000,786000,794000},{2,2,2,2,2,2},{6875,6875,6875,6875,6875,6875},\
				{0,0,0,0,0,0},6,1};
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}
int app_win_hunan_taoyuan_search(void)
{
	search_fre_list t_searchFreList={{674000,682000,690000,706000,714000,722000,746000,754000,770000,778000,786000,794000},{2,2,2,2,2,2,2,2,2,2,2,2},{6875,6875,6875,6875,6875,6875,6875,6875,6875,6875,6875,6875},\
				{0,0,0,0,0,0,0,0,0,0,0,0},12,1};
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}

int app_win_hunan_taikang_search(void)
{
	search_fre_list t_searchFreList={{706000,722000,730000,746000,754000,770000},{2,2,2,2,2,2},{6875,6875,6875,6875,6875,6875},\
				{0,0,0,0,0,0},6,1};
	app_prog_delete_all_prog();
	app_search_set_auto_flag(FALSE);
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}

int app_win_shangshui_search(void)
{
	search_fre_list t_searchFreList={{706000,730000,746000,762000},{2,2,2,2},{6875,6875,6875,6875},\
				{0,0,0,0},4,1};
	app_search_enum_enum_mode(t_searchFreList);
	return EVENT_TRANSFER_KEEPON;	
}
#endif
void app_lock_main_fre(void)
{
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;

	memset(&searchFreList,0,sizeof(search_fre_list));
	symbol_rate = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	searchFreList.num = 1;
	searchFreList.app_fre_array[0] = app_flash_get_config_center_freq();
	searchFreList.app_qam_array[0]=qam;
	searchFreList.app_symb_array[0]=symbol_rate;
	app_search_lock_tp(searchFreList.app_fre_array[0], searchFreList.app_symb_array[0],
			INVERSION_OFF, searchFreList.app_qam_array[0],2000);

}


void app_search_startup_auto_scan(startup_search_para startuppara)
{
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint32_t   panel_prog_cur=0;
	int32_t dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	if(app_prog_get_num_in_group() >0)
	{
		/*存在节目*/
		return;
	}
	else
	{
		panel_prog_cur = 0;
		app_panel_show(PANEL_DATA,&(panel_prog_cur));


		GUI_SetProperty(NULL,"draw_now",NULL);
		{
			memset(&searchFreList,0,sizeof(search_fre_list));
			symbol_rate = app_flash_get_config_center_freq_symbol_rate();
			qam = app_flash_get_config_center_freq_qam();
			searchFreList.num = 1;
			searchFreList.app_fre_array[0] = app_flash_get_config_center_freq();
			searchFreList.app_qam_array[0]=qam;
			searchFreList.app_symb_array[0]=symbol_rate;

			switch(startuppara.nit_flag)
			{
				case 1:

					if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
					{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
						GxFrontend_StopMonitor(0);
#endif /* DVB_CA_TYPE_DS_CAS50_FLAG */
					}
					if (0 == app_search_lock_tp(searchFreList.app_fre_array[0], searchFreList.app_symb_array[0],
								INVERSION_OFF, searchFreList.app_qam_array[0],2000))
					{
						// 主频点锁频成功

						if (dvb_ca_flag == DVB_CA_TYPE_DSCAS50  )			
						{
#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
							GUI_CreateDialog("win_search_result");
							GUI_SetInterface("flush", NULL);
							app_search_scan_nit_mode();
#endif
						}
						else
						{	
							app_lcn_list_clear();
							app_table_nit_search_filter_open();
						}
					}
					else
					{	// 主频点锁频失败
						app_search_scan_all_mode(startuppara.fre_low,startuppara.fre_high,symbol_rate,qam);
					}
					break;
				case 0:
				default:
					/*
					 * 全频段搜索
					 */
#ifdef DVB_CA_TYPE_DVB_FLAG
#ifdef CUST_TAOYUAN
					app_win_hunan_taoyuan_search();		
#endif
#ifdef CUST_LINLI
					app_win_hunan_linli_search();		
#endif
#ifdef CUST_TAIKANG

					app_win_hunan_taikang_search();		
#endif
#ifdef CUST_JINGANGSHAN
					app_win_hunan_taikang_search();		
#endif
#ifdef CUST_SHANGSHUI
					app_win_shangshui_search();		
#endif

#else
						app_search_scan_all_mode(startuppara.fre_low,startuppara.fre_high,symbol_rate,qam);
#endif
						break;
			}
		}
	}
}
void app_search_startup_auto_scan2(startup_search_para startuppara,uint32_t *fre,uint16_t size)
{
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint32_t   panel_prog_cur=0;
	if(app_prog_get_num_in_group() >0)
	{
		/*存在节目*/
		return;
	}
	else
	{
		panel_prog_cur = 0;
		app_panel_show(PANEL_DATA,&(panel_prog_cur));


		GUI_SetProperty(NULL,"draw_now",NULL);
		{
			memset(&searchFreList,0,sizeof(search_fre_list));
			symbol_rate = app_flash_get_config_center_freq_symbol_rate();
			qam = app_flash_get_config_center_freq_qam();
			searchFreList.num = 1;
			searchFreList.app_fre_array[0] = app_flash_get_config_center_freq();
			searchFreList.app_qam_array[0]=qam;
			searchFreList.app_symb_array[0]=symbol_rate;

			switch(startuppara.nit_flag)
			{
				case 1:
					if (0 == app_search_lock_tp(searchFreList.app_fre_array[0], searchFreList.app_symb_array[0],
								INVERSION_OFF, searchFreList.app_qam_array[0],2000))
					{
#ifdef MG_CAS_VER_3_1_1
						app_win_feng_wu_search();//凤阳前端没有nit，要求直接指定频率表搜索
#else
						// 主频点锁频成功
						app_lcn_list_clear();
						app_table_nit_search_filter_open();
#endif
					}
					else
					{
#ifdef MG_CAS_VER_3_1_1
						app_win_feng_wu_search();
#else
						// 主频点锁频失败
						app_search_scan_all_mode(startuppara.fre_low,startuppara.fre_high,symbol_rate,qam);
#endif
					}
					break;
				case 2://特定频点搜索
					if(fre && size >0)
					{
						app_search_scan_mode(fre,size,symbol_rate,qam);
					}
					break;
				case 3:
					if (0 == app_search_lock_tp(searchFreList.app_fre_array[0], searchFreList.app_symb_array[0],
								INVERSION_OFF, searchFreList.app_qam_array[0],2000))
					{	// 主频点锁频成功
						app_lcn_list_clear();
						app_table_nit_search_filter_open();
					}
					else
					{	// 主频点锁频失败
						if(fre && size >0)
						{//特定频点搜索
							app_search_scan_mode(fre,size,symbol_rate,qam);
						}
					}
					break;
				case 0:
				default:
					/*
					 * 全频段搜索
					 */
					app_search_scan_all_mode(startuppara.fre_low,startuppara.fre_high,symbol_rate,qam);
					break;
			}
		}

	}	
}

/*
 * 处理NIT搜索方式，gxbus发送NIT过滤状态消息
 */
void app_search_si_subtable_msg(GxMessage * msg)
{
	GxMsgProperty_SiSubtableOk *parse_result = NULL;
	int32_t NitSubtId=0;
	uint32_t NitRequestId=0;
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	if (NULL == msg)
	{
		return ;
	}
	switch(msg->msg_id)
	{
		case GXMSG_SI_SUBTABLE_OK:
			parse_result = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_SiSubtableOk);
			if(parse_result->table_id == BAT_TID)
			{
				app_table_bat_filter_close_byid(parse_result->si_subtable_id);
				return;
			}
			/*
			 * 所有nit表section 过滤成功
			 */
			symbol_rate = app_flash_get_config_center_freq_symbol_rate();
			qam = app_flash_get_config_center_freq_qam();
			app_table_nit_get_search_filter_info(&NitSubtId,&NitRequestId);
			
			if((NitSubtId == parse_result->si_subtable_id) && (NitRequestId == parse_result->request_id))
			{
				app_table_nit_search_filter_close();
				if(parse_result->table_id == 0x40)
				{
					searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
					app_search_scan_cable_start(searchFreList);				
				}
				else
				{
					printf("\n####error####service_msg_to_app:%d#######\n",parse_result->table_id );
				}
			}

			return ;
		case GXMSG_SI_SUBTABLE_TIME_OUT:
			/*
			 * nit表过滤超时
			 */
			symbol_rate = app_flash_get_config_center_freq_symbol_rate();
			qam = app_flash_get_config_center_freq_qam();
			app_table_nit_get_search_filter_info(&NitSubtId,&NitRequestId);
			parse_result = GxBus_GetMsgPropertyPtr(msg, GxMsgProperty_SiSubtableOk);
			if((NitSubtId == parse_result->si_subtable_id) && (NitRequestId == parse_result->request_id))
			{
				printf("filter nit table timeout  fre = %d!!\n",searchFreList.app_fre_array[0]);
				app_table_nit_search_filter_close();
				{ // 备用主频点NIT过滤超时
					searchFreList.nit_flag = GX_SEARCH_NIT_DISABLE;
					app_search_scan_cable_start(searchFreList);				
				}

			}
			return ;	
		default:
			break;
	}

	return ;
}

/*
 * 搜索结束消息处理
 */
void app_search_stop_ok_msg( search_ok_msg ok_msg)
{
	uint32_t provalue = 0;
	uint32_t pos;
	Lcn_State_t lcn_flag = 0;
	GxBusPmViewInfo sys;
	GxBusPmViewInfo sysinfo;
	uint16_t pos1;
	uint16_t i;
	uint32_t   panel_prog_cur;
	uint8_t delete_flag = FALSE;
	uint8_t buf1[10]={0};
	int ret =0;

	app_table_nit_search_filter_close();
	//	if (1 == searchresultpara.tp_num)
	{
		provalue = 100;

		if (NULL != ok_msg.widget_search_bar)
		{
			GUI_SetProperty(ok_msg.widget_search_bar, "value", (void*)&provalue);		
			GUI_SetProperty(ok_msg.widget_search_bar, "draw_now", NULL);			
		}

		if (NULL != ok_msg.widget_search_bar_value)
		{
			sprintf((char*)buf1, "%d%%", provalue);
			GUI_SetProperty(ok_msg.widget_search_bar_value, "string", buf1);
			GUI_SetProperty(ok_msg.widget_search_bar_value, "draw_now", NULL);	
		}		
	}

	lcn_flag = app_flash_get_config_lcn_flag();


	if(ok_msg.save_flag== FALSE)
	{
		app_send_msg(GXMSG_SEARCH_NOT_SAVE,NULL);
	}
	else
	{
		/*
		 * 搜索成功或失败的提示信息,通过调用回调函数显示
		 */
		GxCore_ThreadDelay(500);
		app_send_msg(GXMSG_SEARCH_SAVE,NULL);
		GxCore_ThreadDelay(1000);

		if (NULL != ok_msg.app_search_ok_pomsg&&(searchresultpara.app_tv_num+searchresultpara.app_radio_num)!=0)
			ret=ok_msg.app_search_ok_pomsg();
		/*
		 * 更新tv / radio , nvod等节目lcn,默认lcn方式排序
		 */
		if (TAXIS_MODE_NON == ok_msg.taxis_mode  )
		{
			if (((LCN_STATE_ON == lcn_flag )||(0 != app_flash_get_config_sort_by_pat_flag()))
				&&((ret == 2)||(ok_msg.app_search_ok_pomsg==NULL)))
			{
				GxBus_PmViewInfoGet(&sys);
				memcpy(&sysinfo,&sys,sizeof(GxBusPmViewInfo));
				if ((GROUP_MODE_ALL !=sys.group_mode )||(GXBUS_PM_PROG_TV != sys.stream_type))
				{
					sys.group_mode = GROUP_MODE_ALL;
					sys.stream_type = GXBUS_PM_PROG_TV;
					GxBus_PmViewInfoModify(&sys);
				}

				app_lcn_list_update_prog_pos();
				if(sys.stream_type == GXBUS_PM_PROG_TV)
				{
					sys.stream_type = GXBUS_PM_PROG_USER2;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_nvod_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_NVOD;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_nvod_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_RADIO;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_prog_pos();
#if (1 == DVB_HD_LIST)	
					sys.stream_type = GXBUS_PM_PROG_HD_SERVICE;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_prog_pos();
#endif
					sys.stream_type = GXBUS_PM_PROG_TV;
					GxBus_PmViewInfoModify(&sys);

				}
				else if(sys.stream_type == GXBUS_PM_PROG_RADIO)
				{
					sys.stream_type = GXBUS_PM_PROG_USER2;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_nvod_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_NVOD;
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_nvod_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_TV;
#if (1 == DVB_HD_LIST)	
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_HD_SERVICE;
#endif
					GxBus_PmViewInfoModify(&sys);
					app_lcn_list_update_prog_pos();
					sys.stream_type = GXBUS_PM_PROG_RADIO;
					GxBus_PmViewInfoModify(&sys);
				}			

			}
		}

		/*
		 * 按service id排序
		 */
		if (TAXIS_MODE_SERVICE_ID == ok_msg.taxis_mode)
		{
			GxBus_PmViewInfoGet(&sys);
			if (TAXIS_MODE_SERVICE_ID != sys.taxis_mode)
			{
				sys.taxis_mode = TAXIS_MODE_SERVICE_ID;
				GxBus_PmViewInfoModify(&sys);					
			}
		}
		else if(TAXIS_MODE_USER == ok_msg.taxis_mode)
		{
			GxBus_PmViewInfoGet(&sys);

			if (NULL != ok_msg.app_prog_check 
					&& NULL != ok_msg.app_prog_order)
			{
				sys.taxis_mode = TAXIS_MODE_USER;
				sys.gx_pm_prog_check = ok_msg.app_prog_check;
				sys.gx_pm_prog_order = ok_msg.app_prog_order;
				GxBus_PmViewInfoModify(&sys);
			}
			else
			{
				printf("Error,ok_msg.app_prog_order == NULL\n");
				return ;
			}
		}
		else if( TAXIS_MODE_SCRAMBLE == ok_msg.taxis_mode )
		{
			GxBus_PmViewInfoGet(&sys);
			if (TAXIS_MODE_SCRAMBLE != sys.taxis_mode)
			{
				sys.taxis_mode = TAXIS_MODE_SCRAMBLE;
				sys.order = VIEW_INFO_REVERSE;
				GxBus_PmViewInfoModify(&sys);					
			}
		}
		else if (TAXIS_MODE_SERVICE_ID_SCRAMBLE == ok_msg.taxis_mode)
		{
			GxBus_PmViewInfoGet(&sys);
			if (TAXIS_MODE_SERVICE_ID_SCRAMBLE != sys.taxis_mode)
			{
				sys.taxis_mode = TAXIS_MODE_SERVICE_ID_SCRAMBLE;
				sys.order = VIEW_INFO_REVERSE;
				GxBus_PmViewInfoModify(&sys);					
			}
		}
		app_prog_userlist_del();
		app_prog_userlist_init();
	}


	/*
	 * 发送存储消息后，delay 500 毫秒，避免节目未保存切台
	 */
	GxCore_ThreadDelay(500);

	/*搜索结束，判断TP下是否存在节目，如无节目，删除TP
	  避免TP超过最大个数溢出*/
	for (i = 0; i< searchresultpara.tp_num; i++)
	{
		if ((0 == searchresultpara.app_radio_num_perTP[i])&&(0 == searchresultpara.app_tv_num_perTP[i]))
		{
			GxBus_PmTpDelete(&searchresultpara.app_tpid[i], 1);
			delete_flag = TRUE;
		}	  		
	}
	if (TRUE == delete_flag)
		GxBus_PmSync(GXBUS_PM_SYNC_TP);


	memset(&searchresultpara,0,sizeof(search_result));

	provalue = 100;

	if (NULL != ok_msg.widget_search_bar)
	{
		GUI_SetProperty(ok_msg.widget_search_bar, "value", (void*)&provalue);		
		GUI_SetProperty(ok_msg.widget_search_bar, "draw_now", NULL);			
	}


	if (NULL != ok_msg.widget_search_bar_value)
	{
		sprintf((char*)buf1, "%d%%", provalue);
		GUI_SetProperty(ok_msg.widget_search_bar_value, "string", buf1);
		GUI_SetProperty(ok_msg.widget_search_bar_value, "draw_now", NULL);	
	}

	/*
	 * 搜索结束是否播放
	 */
	if (FALSE == ok_msg.play_flag)
	{
		app_epg_open();
		app_porting_enable_query_demux();
		return ;			
	}

	pos = ok_msg.play_pos;
	if (LCN_STATE_ON == lcn_flag)
	{
		if (TRUE == app_lcn_list_get_pos_in_group_by_lcn(ok_msg.play_pos,&pos1))
		{
			pos = pos1;
		}
		else
		{
			pos = ok_msg.play_pos;
		}
	}

	app_prog_update_num_in_group();
	app_prog_save_playing_pos_in_group(pos);
	app_play_switch_prog_clear_msg();	
	app_epg_open();//播放节目,开启epg过滤
	app_porting_enable_query_demux();
#if (1 == DVB_HD_LIST)	
	GxBus_PmViewInfoGet(&sys);
	if (0 == app_prog_get_num_in_group())
	{
		if(sys.stream_type == GXBUS_PM_PROG_TV)
		{
			app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_HD_SERVICE,0);

		}
		else
			if(sys.stream_type == GXBUS_PM_PROG_HD_SERVICE)
			{
				app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0);
			}
	}
#else
	if (0 == app_prog_get_num_in_group())
	{
		GxBus_PmViewInfoGet(&sys);
		if(sys.stream_type == GXBUS_PM_PROG_TV)
			{
				app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_RADIO,0);

			}
		else
			if(sys.stream_type == GXBUS_PM_PROG_RADIO)
			{
				app_prog_change_group(GROUP_MODE_ALL,GXBUS_PM_PROG_TV,0);
			}
	}	
#endif

	if (0 == app_prog_get_num_in_group())
	{
		/*
		 * 无节目开启NIT监控FILTER，节目存在切台开启NIT监控FILTER，
		 * 避免多次开启
		 */
		app_table_nit_monitor_filter_open();
		app_table_ota_monitor_filter_restart();
		panel_prog_cur = 0;
		app_panel_show(PANEL_DATA,&(panel_prog_cur));
	}
}

#if defined(LINUX_OTT_SUPPORT)&&(1 == LINUX_OTT_SUPPORT)
#define LANGUAGE_CHINESE ("Chinese")
extern void transform_entry(char * utf8 ,int utf8_len, unsigned char * gb);
void app_search_new_prog_get_msg(search_new_prog_get_msg new_prog_get_msg)
{
	uint8_t buf[50]= {0};
	unsigned char tempbufer[200]={0};
	GxMsgProperty_NewProgGet* params = new_prog_get_msg.params;
	if (NULL == params)
		return ;

	memset(tempbufer,0,sizeof(tempbufer));
	if((params->type == GXBUS_PM_PROG_TV)||(params->type == GXBUS_PM_PROG_NVOD))
	{
		if(searchresultpara.app_tv_num_perTP[searchresultpara.tp_cur]%new_prog_get_msg.max_line_num== 0)
		{
			memset(searchresultpara.app_buf_tv,0,sizeof(searchresultpara.app_buf_tv));
		}
		searchresultpara.app_tv_num_perTP[searchresultpara.tp_cur]++;
		searchresultpara.app_tv_num++;

#if (1==LINUX_OTT_SUPPORT)
		char *osd_language = app_flash_get_config_osd_language();
		if(0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			char utf8_str_buffer[100]={0};
			unsigned char  gb_str[100]={0};
			int str_len = strlen((char *)params->name);
			if(params->name[0]==0x13)
			{
				memcpy(gb_str,&params->name[1],str_len-1);
			}
			else
			{
				memcpy(gb_str,&params->name[0],str_len);
			}
			transform_entry(utf8_str_buffer,sizeof(utf8_str_buffer),gb_str);
			memcpy(tempbufer,utf8_str_buffer,strlen(utf8_str_buffer));//66);
			//printf("\ntempbufer=%s\n",tempbufer);

		}
		else 
#endif
		{
			memcpy(tempbufer,params->name,strlen((char *)params->name));//66);

		}

		sprintf((void*)buf,"   %03d  %s \n", searchresultpara.app_tv_num, tempbufer);
		strcat((char*)searchresultpara.app_buf_tv,(char*)buf);

		if (NULL != new_prog_get_msg.widget_tv_list_name)
			GUI_SetProperty(new_prog_get_msg.widget_tv_list_name, "string", searchresultpara.app_buf_tv);

		memset(buf,0,50);
		sprintf((void*)buf,"%03d",searchresultpara.app_tv_num);

		if (NULL != new_prog_get_msg.widget_tv_num)
			GUI_SetProperty(new_prog_get_msg.widget_tv_num, "string", buf);	

	}
	else
	{
		if(searchresultpara.app_radio_num_perTP[searchresultpara.tp_cur]%new_prog_get_msg.max_line_num == 0)
		{
			memset(searchresultpara.app_buf_radio,0,sizeof(searchresultpara.app_buf_radio));
		}
		searchresultpara.app_radio_num_perTP[searchresultpara.tp_cur]++;
		searchresultpara.app_radio_num++;

#if (1==LINUX_OTT_SUPPORT)
		char *osd_language = app_flash_get_config_osd_language();
		if(0 == strcmp(osd_language,LANGUAGE_CHINESE))
		{
			char utf8_str_buffer[100]={0};
			unsigned char  gb_str[100]={0};
			int str_len=strlen((char *)params->name);
			if(params->name[0]==0x13)
			{
				memcpy(gb_str,&params->name[1],str_len-1);
			}
			else
			{
				memcpy(gb_str,&params->name[0],str_len);
			}
			//unsigned char  gb_str[]={0xb6, 0xab ,0xc4 ,0xcf ,0xce ,0xc0, 0xca ,0xd3 };
			transform_entry(utf8_str_buffer,sizeof(utf8_str_buffer),gb_str);
			memcpy(tempbufer,utf8_str_buffer,strlen(utf8_str_buffer));//66);

		}
		else 
#endif
		{
			memcpy(tempbufer,params->name,strlen((char *)params->name));//66);

		}

		sprintf((void*)buf,"%03d %s \n", searchresultpara.app_radio_num, tempbufer);
		strcat((char*)searchresultpara.app_buf_radio, (char*)buf);

		if (NULL != new_prog_get_msg.widget_radio_list_name)
			GUI_SetProperty(new_prog_get_msg.widget_radio_list_name, "string", searchresultpara.app_buf_radio);			

		memset(buf,0,50);
		sprintf((void*)buf,"%03d",searchresultpara.app_radio_num);
		if (NULL != new_prog_get_msg.widget_radio_num)
			GUI_SetProperty(new_prog_get_msg.widget_radio_num, "string", buf);

	}
}

#else
/*
 * 搜索到节目消息 
 */
void app_search_new_prog_get_msg(search_new_prog_get_msg new_prog_get_msg)
{
	uint8_t buf[50]= {0};
	GxMsgProperty_NewProgGet* params = new_prog_get_msg.params;
	if (NULL == params)
		return ;
	if(params->type == GXBUS_PM_PROG_RADIO)
	{
		if(searchresultpara.app_radio_num_perTP[searchresultpara.tp_cur]%new_prog_get_msg.max_line_num == 0)
		{
			memset(searchresultpara.app_buf_radio,0,sizeof(searchresultpara.app_buf_radio));
		}
		searchresultpara.app_radio_num_perTP[searchresultpara.tp_cur]++;
		searchresultpara.app_radio_num++;
#if (PANEL_TYPE == PANEL_TYPE_CT1642_JINYA)
		sprintf((void*)buf,"  %s \n", params->name);
#else
		sprintf((void*)buf,"%03d %s \n", searchresultpara.app_radio_num, params->name);
#endif
		//			printf("app_radio_num =%d name = %s\n",searchresultpara.app_radio_num,params->name);
		strcat((char*)searchresultpara.app_buf_radio, (char*)buf);

		if (NULL != new_prog_get_msg.widget_radio_list_name)
			GUI_SetProperty(new_prog_get_msg.widget_radio_list_name, "string", searchresultpara.app_buf_radio);			

		memset(buf,0,50);
		sprintf((void*)buf,"%03d",searchresultpara.app_radio_num);
		if (NULL != new_prog_get_msg.widget_radio_num)
			GUI_SetProperty(new_prog_get_msg.widget_radio_num, "string", buf);

	}
	else
	{
		if(searchresultpara.app_tv_num_perTP[searchresultpara.tp_cur]%new_prog_get_msg.max_line_num== 0)
		{
			memset(searchresultpara.app_buf_tv,0,sizeof(searchresultpara.app_buf_tv));
		}
		searchresultpara.app_tv_num_perTP[searchresultpara.tp_cur]++;
		searchresultpara.app_tv_num++;
#if (PANEL_TYPE == PANEL_TYPE_CT1642_JINYA)
		sprintf((void*)buf,"  %s \n", params->name);
#else
		sprintf((void*)buf,"%03d  %s \n", searchresultpara.app_tv_num, params->name);
#endif
		//			printf("app_tv_num = %d name = %s\n",searchresultpara.app_tv_num,params->name);
		strcat((char*)searchresultpara.app_buf_tv,(char*)buf);

		if (NULL != new_prog_get_msg.widget_tv_list_name)
			GUI_SetProperty(new_prog_get_msg.widget_tv_list_name, "string", searchresultpara.app_buf_tv);

		memset(buf,0,50);
		sprintf((void*)buf,"%03d",searchresultpara.app_tv_num);

		if (NULL != new_prog_get_msg.widget_tv_num)
			GUI_SetProperty(new_prog_get_msg.widget_tv_num, "string", buf);	

	}
}
#endif

uint32_t app_search_get_cur_searching_tp_fre(void)
{
	return cur_fre;
}


/*
 * 搜索新TP消息 
 */
void app_search_sat_tp_reply_msg(search_sat_tp_reply_msg  sat_tp_reply_msg)
{
	uint32_t provalue = 0;
	uint8_t buf[50]= {0};
	uint8_t buf1[10]= {0};
	uint16_t i =0;


	memset(searchresultpara.app_buf_tv,0,sizeof(searchresultpara.app_buf_tv));
	memset(searchresultpara.app_buf_radio,0,sizeof(searchresultpara.app_buf_radio));


	GxMsgProperty_SatTpReply* params = sat_tp_reply_msg.params;
	if (NULL == params)
		return ;

	for (i =0; i<searchresultpara.tp_num; i++)
	{
		if (params->tp_id == searchresultpara.app_tpid[i])
		{
			searchresultpara.app_radio_num_perTP[i] = 0;
			searchresultpara.app_tv_num_perTP[i] = 0;
			searchresultpara.tp_cur = i;
			break;
		}
	}

		cur_fre = params->frequency;
		sprintf((char*)buf,"%03d.%d MHz",params->frequency/1000,(params->frequency%1000)/100);
		if (0 != params->tp_max_count)
		{
			//if (1 == params->tp_num)
			{
				provalue = params->tp_num * 100/params->tp_max_count;
			}
			/*else
			{
				provalue = (params->tp_num-1)*100/params->tp_max_count;
			}*/
		}
		if (1 == params->tp_max_count)
		{
			/*
			* 搜索单频点
			*/
			 provalue = 50;
		}

	if (NULL != sat_tp_reply_msg.widget_search_progress_bar)
		GUI_SetProperty(sat_tp_reply_msg.widget_search_progress_bar, "value", (void*)&provalue);

	sprintf((char*)buf1, "%d%%", provalue);
	if (NULL != sat_tp_reply_msg.widget_search_progress_bar_value)
		GUI_SetProperty(sat_tp_reply_msg.widget_search_progress_bar_value, "string", buf1);

	/*
	 * 清除电视/广播显示列表
	 */

	if (NULL != sat_tp_reply_msg.widget_tv_list_name)
		GUI_SetProperty(sat_tp_reply_msg.widget_tv_list_name, "string", " ");

	if (NULL != sat_tp_reply_msg.widget_radio_list_name)
		GUI_SetProperty(sat_tp_reply_msg.widget_radio_list_name, "string",  " ");


	if (NULL != sat_tp_reply_msg.widget_fre_name)
		GUI_SetProperty(sat_tp_reply_msg.widget_fre_name, "string", buf);		 			

	app_lcn_set_searching_tp_id(params->tp_id);

	GUI_SetProperty(sat_tp_reply_msg.widget_fre_name, "draw_now", NULL);
	printf("FRE: == %d, %s \n", params->frequency, buf);
}
/*
 * 显示信号强度百分比/数值
 */
void app_search_set_strength_progbar(strength_xml strengthxml)
{
	uint32_t value = 0;
	int8_t   chSignalStrBuf[10]= {0};

	if (0 == strengthxml.lock_status_invaild)
	{
		if(strengthxml.lock_status == 1)
			value = GxFrontend_GetStrength(0);
		else
			value = 0;	
	}
	else
	{
		value = GxFrontend_GetStrength(0);
	}
	if (value >=100)
		value = 100;
	if (NULL != strengthxml.widget_name_strength_bar)
		GUI_SetProperty(strengthxml.widget_name_strength_bar, "value", (void*)&value);

	if (0 == strengthxml.unit)
	{
		sprintf((char*)chSignalStrBuf, "%ddBuV", value);
	}
	else
	{
		sprintf((char*)chSignalStrBuf, "%d%%", value);
	}

	if (NULL != strengthxml.widget_name_strength_bar_value)
		GUI_SetProperty(strengthxml.widget_name_strength_bar_value, "string", (void*)chSignalStrBuf);

	return;
}

/*
 * 显示信号质量百分比/数值
 */
void app_search_set_signal_progbar(signal_xml singalxml)
{
	uint32_t value = 0;
	uint16_t E_param = 0;
	int16_t  nErrotRate = 0;
	int8_t   chErrorRateBuf[20]= {0};
	int8_t   chSignalQualityBuf[10]= {0};
	GxFrontendSignalQuality Singal;
	uint32_t signalvalue = 0;

	if (0 == singalxml.lock_status_invaild)
	{
		if(singalxml.lock_status ==1)
		{	
			value = GxFrontend_GetQuality(0, &Singal);
			E_param = (Singal.error_rate&0xffff);
			nErrotRate = (Singal.error_rate>>16)*100;
		}
		else if(singalxml.lock_status ==0)
		{
			value=0;
			E_param=0;
			nErrotRate=0;
			Singal.snr =0;
		}
	}
	else
	{
		value = GxFrontend_GetQuality(0, &Singal);
		E_param = (Singal.error_rate&0xffff);
		nErrotRate = (Singal.error_rate>>16)*100;
	}


	sprintf((char*)chErrorRateBuf, "%d.%02dE-%02d", nErrotRate/100, nErrotRate%100, E_param);
	if (NULL != singalxml.widget_name_error_rate)
		GUI_SetProperty(singalxml.widget_name_error_rate, "string", (void*)chErrorRateBuf);

	if (NULL != singalxml.widget_name_error_rate_bar)
		GUI_SetProperty(singalxml.widget_name_error_rate_bar, "value", (void*)&Singal.error_rate);
	/* zhouhm : 信号质量转化为db显示*/

	signalvalue = Singal.snr;//*100/35;
	if (signalvalue >=100)
		signalvalue = 100;
	//	signalvalue = Singal.snr;

	if (NULL != singalxml.widget_name_signal_bar)	
		GUI_SetProperty(singalxml.widget_name_signal_bar, "value", (void*)&signalvalue);


	if (0 == singalxml.unit)
	{
		sprintf((char*)chSignalQualityBuf, "%ddB", signalvalue);
	}
	else
	{
		sprintf((char*)chSignalQualityBuf, "%d%%", signalvalue);
	}	

	if (NULL != singalxml.widget_name_signal_bar_value)		
		GUI_SetProperty(singalxml.widget_name_signal_bar_value, "string", (void*)chSignalQualityBuf);

	return;
}

search_result* app_search_get_result_para(void)
{
	return (search_result*)&searchresultpara;
}

uint8_t app_search_get_auto_flag(void)
{
	return gAutoSearchFlag;
}

void app_search_set_auto_flag(uint8_t flag)
{
	gAutoSearchFlag = flag;
}

float app_float_edit_str_to_value(const char *str)
{
	const char *cp;
	float data;
	char chHasDot=0;

	for (cp = str, data = 0; *cp != 0; ++cp) 
	{
		if (*cp >= '0' && *cp <= '9')
		{
			data = data * 10 + *cp - '0';
			if (chHasDot)
			{
				chHasDot ++; 
			}
		}
		else if(*cp == '.')
		{
			chHasDot = 1;
		}
		else
			break;
	}
	if(chHasDot>1)
	{
		data /=(10*(chHasDot-1));
	}
	return data;
}

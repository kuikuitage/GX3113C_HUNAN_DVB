/**
 *
 * @file        app_common_search.h
 * @brief
 * @version     1.1.0
 * @date        10/18/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_SEARCH__H__
#define __APP_COMMON_SEARCH__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <gxtype.h>

#include "service/gxsearch.h"
#include "app_common_play.h"
//#include "app_common_book.h"
//#include "gxbook.h"
#include "gxapp_sys_config.h"

/*
* 定义搜索节目同时过滤扩展表最大个数
*/
#define SEARCH_EXTEND_MAX (2)


typedef struct
{
	  uint8_t app_buf_tv[400];
	  uint8_t app_buf_radio[400];
	  uint16_t  app_tv_num ;
	  uint16_t  app_radio_num ;
	  uint16_t tp_num; /*实际搜索TP个数，超过最大TP个数情况下，
	                                tp_num小于设置的个数*/
	  uint16_t tp_cur; /*当前搜索TP索引*/
	  uint32_t app_tpid[TP_MAX_NUM]; /*实际搜索TP对应ID，超过最大TP个数情况下，
	                                个数小于设置的个数*/
	 uint32_t app_tv_num_perTP[TP_MAX_NUM]; /*每个TP对应搜索到的电视节目数*/
	 uint32_t app_radio_num_perTP[TP_MAX_NUM]; /*每个TP对应搜索到的广播节目数*/
	 
}search_result;
  
typedef struct
{
	uint32_t app_fre_array[TP_MAX_NUM];
	uint16_t app_qam_array[TP_MAX_NUM];
	uint16_t app_symb_array[TP_MAX_NUM];
	uint16_t app_fre_tsid[TP_MAX_NUM];
	uint16_t num;
	uint8_t nit_flag;
}search_fre_list;


typedef struct
{
	uint32_t fre_low;
	uint32_t fre_high;
	uint8_t nit_flag; /*是否开机自动NIT搜索标1: NIT搜索，0: 全频段搜索 其他扩展支持*/ 
}startup_search_para;

/*
* 是否切换选择框回调函数
*/
typedef int (*search_ok_popmsg)(void);
typedef int32_t (*search_prog_order)(const void *p1, const void *p2);
typedef	int32_t (*search_prog_check)(GxBusPmDataProg* prog, GxBusPmProgSort* list);//检测节目是否符合要求的回调函数，符合要求节目需要把用于排序的成员填入后面的list指针并且返回0，不符合要求返回-1，不使用该功能请务必填NULL

typedef struct
{
	uint8_t taxis_mode; /* 排序方式*/
	uint8_t save_flag; /*是否保存标志*/ 
	uint8_t play_flag; /*是否播放标志，需要返回的窗体*/  
	                                       // 可能存在搜索结束仍要求返回到搜索界面的要求
	uint32_t play_pos; /*播放指定节目*/ 
										   
	const char* widget_search_bar; /*搜索进度条控件，搜索结束需设置为100
	                                                    如无进度条，设置为NULL*/
	const char* widget_search_bar_value; /*搜索进度条比率，搜索结束需显示为100%
	                                                    如无进度条比率，设置为NULL*/
	search_ok_popmsg app_search_ok_pomsg; 
    search_prog_order app_prog_order;  
    search_prog_check app_prog_check;
}search_ok_msg;


typedef struct
{
	const char * widget_name_strength_bar;  /*信号强度百分条*/
	const char * widget_name_strength_bar_value;  /*信号强度值*/
	const char* widget_name_strength_bar_value_format; /* 信号强度值格式如db , % */
	char        lock_status; /*1锁定，0失锁*/
	char        lock_status_invaild; /*0 in vaild , 1 vaild*/	
	char        unit; /*0 dB , 1 %*/
	                                                                                          
}strength_xml;


typedef struct
{
	const char * widget_name_signal_bar;  /*信号质量百分条*/
	const char * widget_name_signal_bar_value; /*信号强度百分值显示db*/
	const char * widget_name_error_rate_bar; /*误码率百分条*/
	const char * widget_name_error_rate; /*误码率值显示
	                                                                 格式"%d.%02dE-%02d"*/
	char       lock_status;
	char       lock_status_invaild; /*0  vaild , 1 invaild*/	
	char	   unit; /*0 dBuV mode, 1 % mode*/

}signal_xml;


typedef struct
{
	GxMsgProperty_NewProgGet* params;  
	const char * widget_tv_list_name; /*显示搜索当前TP电视名称控件，如不存在，设置为NULL*/
	const char * widget_tv_num; /*显示搜索到电视名称个数控件，如不存在，设置为NULL*/
	const char * widget_radio_list_name;  /*显示搜索当前TP广播名称控件，如不存在，设置为NULL*/
	const char * widget_radio_num;  /*显示搜索当前TP广播个数控件，如不存在，设置为NULL*/
	uint32_t max_line_num; /*显示最大行数*/
}search_new_prog_get_msg;


typedef struct
{
	GxMsgProperty_SatTpReply* params;  
	const char * widget_search_progress_bar;  /*搜索百分比滚动条*/ 
	const char * widget_search_progress_bar_value; /*搜索百分比值*/ 
	const char * widget_fre_name; /*显示当前TP频率值控件名称*/ 
	const char * widget_tv_list_name; /*显示搜索当前TP电视名称控件，
	                                                          搜索新TP将控件显示清除。如不存在，设置为NULL*/
	const char * widget_radio_list_name; /*显示搜索当前TP广播名称控件，
	                                                          搜索新TP将控件显示清除。如不存在，设置为NULL*/
}search_sat_tp_reply_msg;

typedef struct
{
	GxSearchExtend searchExtend[SEARCH_EXTEND_MAX];
	uint32_t extendnum;
}search_extend;

typedef struct 
{
	uint32_t fre;
	uint32_t symbol_rate;
	uint32_t qam;
	uint8_t  flag;//错误的频点参数标志
	

}search_dvbc_param;

typedef int (*search_add_extend_table)(search_extend* searchExtendList);	
void app_search_register_add_extend_table_callback(search_add_extend_table search_extend_call_back);

/*
* 以下接口提供menu调用
*/

/*
* 开启信号监测
*/
uint8_t app_demodu_monitor_start(void);

/*
* 关闭信号监测
*/
uint8_t app_demodu_monitor_stop(void);

/*
* 关闭所有后台过滤filter资源 
*/
uint8_t app_stop_all_monitor_filter(void);
/*
* 开启后台过滤filter
*/
uint8_t app_start_all_monitor_filter(void);


/*
* 搜索过程中接受到退出键，发送退出消息到GXBUS停止搜索
*/
uint8_t app_search_scan_stop(void);


/*
* 手动搜索
*/
void app_search_scan_manual_mode(uint32_t fre,uint32_t symbol_rate,uint32_t qam);

/*
* NIT搜索
*/
void app_search_scan_nit_mode(void);

/*
* 全频段搜索
*/
uint32_t app_search_scan_all_mode(uint32_t begin_fre,uint32_t end_fre,uint32_t symbol_rate,uint32_t qam);

/*
* 特定频率搜索
*/
uint32_t app_search_scan_mode(uint32_t *fre,uint16_t size,uint32_t symbol_rate,uint32_t qam);

/*指定频率表搜索*/
uint32_t app_search_enum_enum_mode(search_fre_list t_searchFreList);

#ifdef DVB_CA_TYPE_MG312_FLAG
#include "mg312def.h"
#ifdef MG_CAS_VER_3_1_1
int app_win_fengyang_search(void);
int app_win_wudian_search(void);
int app_win_feng_wu_search(void);
#endif
#endif

/*
* 无节目自动搜索
*/
void app_search_startup_auto_scan(startup_search_para startuppara);

/*
* 锁频
*/
status_t app_search_lock_tp(uint32_t fre, uint32_t symb, fe_spectral_inversion_t inversion, fe_modulation_t modulation,uint32_t delayms);


/*
* 显示新搜索到节目信息(收到GXMSG_SEARCH_NEW_PROG_GET消息)
*/
void app_search_new_prog_get_msg(search_new_prog_get_msg new_prog_get_msg);

/*
* 显示新TP信息(收到GXMSG_SEARCH_SAT_TP_REPLY消息)
*/
 void app_search_sat_tp_reply_msg(search_sat_tp_reply_msg  sat_tp_reply_msg);

/*
* 搜索结束处理(收到GXMSG_SEARCH_STOP_OK消息)
*/
 void app_search_stop_ok_msg( search_ok_msg ok_msg);

/*
*NIT表搜索结束处理(收到GXMSG_SI_SUBTABLE_OK 、GXMSG_SI_SUBTABLE_TIME_OUT 消息)
*/
void app_search_si_subtable_msg(GxMessage * msg);

/*
* 搜索接口
*/
void app_search_scan_cable_start(search_fre_list searchlist);

/*
*信号强度显示
*/
void app_search_set_strength_progbar(strength_xml strengthxml);

/*
* 信号质量显示 
*/
void app_search_set_signal_progbar(signal_xml singalxml);

/*
* 获取搜索结果信息
*/
search_result* app_search_get_result_para(void);

uint16_t app_enum_search_fre_list_init(void);
search_fre_list* app_enum_search_get_fre_list(void);

uint8_t app_search_get_auto_flag(void);

void app_search_set_auto_flag(uint8_t flag);
float app_float_edit_str_to_value(const char *str);
uint32_t app_search_get_cur_searching_tp_fre(void);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_SEARCH__H__*/


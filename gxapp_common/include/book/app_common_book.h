#ifndef __APP_COMMON_BOOK_H__
#define __APP_COMMON_BOOK_H__
#ifdef __cplusplus
extern "C" {
#endif


#include "gxtype.h"
#include "gxbook.h"
#include "gxepg.h"
#include "service/gxepg.h"
#include "gxprogram_manage_berkeley.h"
#include "app_common_search.h"

typedef enum
{
	BOOK_STATUS_OK=0,		/*OK*/
	BOOK_STATUS_EXIST ,			/*相同预约已存在*/
	BOOK_STATUS_CONFILCT ,			/*相同时间已存在其他预约*/
	BOOK_STATUS_OVERDUE ,			/*预约时间无效*/
	BOOK_STATUS_FULL,			/*预约已满*/
	BOOK_STATUS_PARA_ERROR /*参数错误*/
}book_add_error_t;

typedef struct
{
	uint32_t prog_id;//预约的节目号
	uint32_t prog_id_recall;
	uint32_t event_id;
	time_t  start_time;
	time_t  end_time;
	uint8_t  event_name[36];//事件
}book_play;

/*
* 增加预约失败回调显示处理，如预约冲突，预约已满等
*/
typedef int (*book_add_event_popmsg_callback)(int ret, uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet,GxBook* pbooktmp);

typedef struct
{
	GxBookType book_type; /*预约类型*/
	uint32_t prog_id; /*待机预约类型不需设置*/
	uint32_t event_id; /*待机预约类型不需设置*/
	time_t start_time; /*开始时间*/
	time_t end_time; /*结束时间，如果类型为待机，标志自动启机时间*/
	uint8_t* event_name; /*待机预约类型不需设置*/
	GxBookGet* pEpgBookGet;//预约信息
}event_book_para;

/*
* 是否切换选择框回调函数
*/
typedef int (*book_play_popmsg_callback)(book_play book, GxBusPmDataProg   prog);
typedef int (*book_record_popmsg_callback)(book_play book, GxBusPmDataProg   prog);

void* app_book_starttime_check_exist(GxBookType book_type,uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet);
int8_t app_book_create(GxBookType type,uint32_t prog_id,
	                                   uint32_t event_id,
	                                   time_t start_time,
	                                   time_t end_time,
	                                   uint8_t *event_name);
int8_t app_book_modify(GxBook* pbook);
int32_t app_book_check_trig_ing(void);
/*
* 删除对应节目的所有预约(删除节目时调用)
*/
void app_book_delete_prog(uint32_t prog_id);

/*
* 删除对应TP的所有预约(删除tp 时调用)
*/
void app_book_delete_tp(search_fre_list searchlist);



/*
* 以下接口提供具体菜单方案中调用
*/

void app_book_register_add_event_popmsg_callback(book_add_event_popmsg_callback bookaddeventcallback);
void app_book_register_play_popmsg_callback(book_play_popmsg_callback bookplaycallback);
void app_book_register_record_popmsg_callback(book_record_popmsg_callback bookrecordcallback);


/*
* 初始化预约数据
*/
int8_t app_book_init(void);

/*
* 查询预约播放(处理播放)
*/
int8_t app_book_query_play(void);

/*
* 新增预约
*/
int32_t app_book_add_event(event_book_para eventbookpara);

/*
* 同步更新预约、排序等功能
*/
int8_t app_book_sync(GxBookGet* pEpgBookGet,GxBookType type);

/*
* 判断对应节目、时间是否预约存在
*/
void* app_book_channel_check_exist(GxBookType book_type,uint32_t prog_id,uint32_t start_time,GxBookGet* pEpgBookGet);

/*
* 删除预约
*/
int8_t app_book_delete(GxBook* pbook);

/*
*开始 触发预约(收到gxbus的GXMSG_BOOK_TRIGGER消息)
*/
void app_book_trigger_start(GxBook* pbook);

/*
* 结束触发预约(收到gxbus的GXMSG_BOOK_FINISH消息)
*/
void app_book_trigger_end(GxBook* pbook);

/*
* 清除所有预约数据
*/
int8_t app_book_clear_all(void);

/*
* 检测当前预约节目是否存在
*/
int8_t app_book_check_exist(GxBookGet* pEpgBookGet,GxBookType type,int32_t book_id, int *ri_Sel);
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_BOOK_H__*/


/**
 *
 * @file        app_common_porting_stb_api.c
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_PORTING_CA_API_H__
#define __APP_COMMON_PORTING_CA_API_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_table_pmt.h"
#include "app_common_play.h"
/*wufei add for CDCA*/
#ifndef LANGUAGE_CHINESE
#define LANGUAGE_CHINESE ("Chinese")
#endif
#ifndef LANGUAGE_ENGLISH
#define LANGUAGE_ENGLISH ("English")
#endif

/*
* 此头文件定义提供给界面调用的CA公共接口
* 公共接口统一定义，切勿随意修改接口定义。
* 对应函数功能在app\ca\api xxx.c实现，不同CA根据实际情况实现对应功能。
*/

#define CAS_NAME                  ("cas")

#define CAS_PRINT_OPEN				0
#define ADS_PRINT_OPEN				1





typedef enum
{
	DVB_CA_TYPE_NONE=0,		/*无CA*/
	DVB_CA_TYPE_BY,                   /*博远CA*/
	DVB_CA_TYPE_DVB,                   /*乐豆CA*/
	DVB_CA_TYPE_CDCAS30 ,			/*TF 3.0 版本CA*/
	DVB_CA_TYPE_KN ,
	DVB_CA_TYPE_XINSHIMAO ,			/*XINSHIMAO*/
	DVB_CA_TYPE_DIVITONE ,
	DVB_CA_TYPE_DSCAS50 ,
	DVB_CA_TYPE_DVT, /*sumavision ca*/
	DVB_CA_TYPE_QILIAN,
	DVB_CA_TYPE_MG, /*san zhou*/
	DVB_CA_TYPE_QZ,/*全智*/
	DVB_CA_TYPE_GOS,                /*驰通CA*/
	DVB_CA_TYPE_GY,
	DVB_CA_TYPE_WF,/*成都万发*/
	DVB_CA_TYPE_ABV53,

	DVB_CA_TYPE_TR,/*滕锐CA*/
	DVB_CA_TYPE_CTI,/*算通ca*/	
	DVB_CA_TYPE_MG312, /*san zhou V3.1.2*/
	
	DVB_CA_TYPE_ABV38,/* ABV 3.8-version */
	
    DVB_CA_TYPE_KP,/*天柏CA,无卡CA,定义放在所有其他CA之后*/
	/*扩展，其他CA类型定义*/
	DVB_CA_TYPE_MAX
}dvb_ca_type_t;

typedef enum
{
	DVB_CA_BASE_INFO=0,		       /*基本信息*/
	DVB_CA_OPERATOR_INFO,		/*运营商信息*/
	DVB_CA_EMAIL_INFO,			/*邮件*/
	DVB_CA_ENTITLE_INFO ,			/*授权*/
	DVB_CA_DETITLE_INFO,                 /*反授权*/
	DVB_CA_EMERGENCY_INFO,           /*应急广播*/
	DVB_CA_ROLLING_INFO,                /*OSD滚动消息*/
	DVB_CA_FINGER_INFO,                  /*指纹*/
	DVB_CA_CARD_UPDATE_INFO,        /*卡升级*/
	DVB_CA_FETURE_INFO,                   /*特征值*/
	DVB_CA_IPPV_POP_INFO,                /*IPPV购买框信息*/
	DVB_CA_IPPT_POP_INFO,                /*IPPT购买框信息*/
	DVB_CA_IPPV_PROG_INFO,             /*IPPV已购买节目列表信息*/
	DVB_CA_IPPV_SLOT_INFO,             /*IPPV钱包列表信息*/
//	DVB_CA_CHILD_CARD_INFO,             /*子卡信息*/
	DVB_CA_MOTHER_CARD_INFO,             /*母卡信息*/
	DVB_CA_CHANGE_PIN_INFO,             /*修改密码信息*/
	DVB_CA_PROGRESS_INFO,             /*智能卡升级进度信息*/
	DVB_CA_RATING_INFO,                 /*成人级别信息*/
	DVB_CA_WORK_TIME_INFO,           /*工作时段信息*/
	DVB_CA_CURTAIN_INFO,           /*窗帘信息*/
	DVB_CA_PAIRED_INFO,           /*配对信息*/
	
	DVB_CA_PRE_AUTH_INFO, 		  /*预授权信息*/
	/*扩展，CA其他数据类型*/
	DVB_CA_OTA, /*ota升级*/
	DVB_CA_NIT, /*NIT virsion change*/
	DVB_CA_CHANGE_INFO, /*双ca*/
	#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	DVB_CA_CONDITION_SEARCH,/*德赛CA 条件搜索*/
	#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	DVB_CA_CONSUME_HISTORY,			/*历史充值/消费记录*/
	#endif
#ifdef DVB_CA_TYPE_DVB_FLAG
	DVB_CA_UPGRADE_FRE,
#endif
	/*
	* ad
	*/
	DVB_AD_BMP,
	DVB_AD_GIF,
	DVB_AD_SCROLL_BMP,
	DVB_CA_MAX_INFO
}dvb_ca_data_type_t;

typedef enum
{
	DVB_CA_EMAIL_FLAG_HIDE=0,		       /*hide email flag */
	DVB_CA_EMAIL_NEW,		/*new email flag show*/
	DVB_CA_EMAIL_EXHAUST			/*email space exhaust*/
}dvb_ca_email_flag_type_t;

typedef enum
{
	DVB_CA_EMAIL_PROMTP_NONE=0,
	DVB_CA_EMAIL_PROMTP_HIDE,
	DVB_CA_EMAIL_PROMTP_SHOW,
	DVB_CA_EMAIL_PROMTP_TRANS_HIDE,
	DVB_CA_EMAIL_PROMTP_MAX
}dvb_ca_email_prompt_type_t;

typedef enum
{
	DVB_CA_ROLLING_FLAG_NONE=0,
	DVB_CA_ROLLING_FLAG_HIDE,
	DVB_CA_ROLLING_FLAG_SHOW,
	DVB_CA_ROLLING_FLAG_RESET,
	#ifdef DVB_CA_TYPE_DS_CAS50_FLAG
	DVB_CA_ROLLING_FLAG_TRANS_HIDE,
	#endif /*DVB_CA_TYPE_DS_CAS50_FLAG*/
	DVB_CA_ROLLING_FLAG_CLEAR,
	DVB_CA_ROLLING_FLAG_MAX
}dvb_ca_rolling_message_flag_type_t;

typedef enum
{
	DVB_CA_FINGER_FLAG_NONE=0,
	DVB_CA_FINGER_FLAG_HIDE,
	DVB_CA_FINGER_FLAG_SHOW,
	DVB_CA_FINGER_FLAG_TRANS_HIDE,
	DVB_CA_FINGER_FLAG_MAX
}dvb_ca_finger_flag_type_t;

typedef enum
{
	DVB_CA_FEED_FLAG_NONE=0,
	DVB_CA_FEED_FLAG_HIDE,
	DVB_CA_FEED_FLAG_SHOW,
	DVB_CA_FEED_FLAG_MAX
}dvb_ca_feed_flag_type_t;




/*
* 初始化
*/
typedef uint8_t (*app_cas_update_dvb_ca_flag_callback)(void);
uint8_t app_cas_update_dvb_ca_flag(void);

typedef uint8_t (*app_cas_init_callback)(void);
uint8_t app_cas_init(void);

/*
* 关闭
*/
typedef uint8_t (*app_cas_close_callback)(void);
uint8_t app_cas_close(void);

/*
* 智能卡插入，部分CA需传递应答数据、长度
*/
typedef uint8_t (*app_cas_api_card_in_callback)(char* ,uint8_t );
uint8_t app_cas_api_card_in(char* atr,uint8_t len);

/*
* 智能卡拔出
*/
typedef uint8_t (*app_cas_api_card_out_callback)(void);
uint8_t app_cas_api_card_out(void);

/*
* ecm,emm filter
*/
/*
* 释放ecm filter , 切台、停止播放、恢复出厂设置等情况下调用
*/
typedef uint8_t (*app_cas_api_release_ecm_filter_callback)(void);
uint8_t	app_cas_api_release_ecm_filter(void);
/*
* 设置ecm filter , 切台调用，有些CA可能需要绑定解扰通道
* 根据不同CA实际实现。
*/
typedef uint8_t (*app_cas_api_start_ecm_filter_callback)(play_scramble_para_t *);
uint8_t app_cas_api_start_ecm_filter(play_scramble_para_t *playPara);
/*
* 释放emm filter, 切换频点、恢复出厂设置等情况下调用
*/
typedef uint8_t (*app_cas_api_release_emm_filter_callback)(void);
uint8_t	app_cas_api_release_emm_filter(void);
/*
* 设置emm filter, 切台过滤到CAT表调用
*/
typedef uint8_t (*app_cas_api_start_emm_filter_callback)(uint16_t);
uint8_t app_cas_api_start_emm_filter(uint16_t emm_pid);


/*
* 判断是否对应CA系统描述子
* 返回TURE -- 对应CA系统CA描述子
* 返回FALSE -- 非对应CA系统CA描述子
*/
typedef bool (*app_cas_api_check_cat_ca_descriptor_valid_callback)(uint8_t *,uint32_t );
bool app_cas_api_check_cat_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id);
/*
* 判断是否对应CA系统描述子
* 返回TURE -- 对应CA系统CA描述子
* 返回FALSE -- 非对应CA系统CA描述子
*/
typedef bool (*app_cas_api_check_pmt_ca_descriptor_valid_callback)(uint8_t *,uint32_t );
bool app_cas_api_check_pmt_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id);


uint32_t app_cas_api_check_bus_pmt_ca_valid(uint16_t ca_system_id,uint16_t ele_pid,uint16_t ecm_pid);
/*提示消息、弹出框、滚动消息等*/
/*
* 判断是否CA提示消息
*/
typedef uint8_t (*app_cas_api_is_ca_pop_msg_type_callback)(uint8_t);
uint8_t app_cas_api_is_ca_pop_msg_type(uint8_t type);

/*
* 显示、隐藏无授权等提示
*/
typedef int32_t (*app_cas_api_pop_msg_exec_callback)(uint8_t);
int32_t app_cas_api_pop_msg_exec(uint8_t type);
/*
* 更新显示CA相关信息，如OSD滚动消息，邮件提醒，指纹等
*/
typedef int32_t (*app_cas_api_osd_exec_callback)(void);
int32_t app_cas_api_osd_exec(void);
/*
* 实现CA弹出框功能，如IPPV/IPPT购买框，成人密码框，OSD滚动消息更新等
*/
typedef int32_t (*app_cas_api_gxmsg_ca_on_event_exec_callback)(GxMessage *);
int32_t app_cas_api_gxmsg_ca_on_event_exec(GxMessage * msg);


/*
* 以下接口提供CA菜单调用
*/
typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	uint32_t pos ;                             /*序号*/
	uint32_t ID;                                /*对应基本信息枚举,不同CA枚举定义存在差异化.具体枚举定义
	                                                    在app/xxxcas/include*/
	/******扩展*/															/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint32_t number;					/*节目供应商的序号*/
	uint32_t type;						/*数据类型eg. FREE PPC PPV*/
	#endif
}ca_get_date_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	uint32_t pos ;                             /*序号*/
	uint32_t totalnum;                     /* 总数*/
	uint32_t newnum;                      /*未读个数*/
	uint32_t emptynum;                    /*empty num*/
	/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint32_t type;						/*数据类型eg. FREE PPC PPV*/
	#endif
}ca_get_count_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	uint32_t pos ;                             /*序号*/
	/******扩展*/
}ca_delete_data_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	char* pin;                           /*密码*/
	uint8_t  buystate;                      /*是否购买:0-- 不购买，1-- 购买*/
	void* buydata;                           /*购买数据指针，不同CA存在差异*/
	/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint32_t serial_number;					/*节目的序号当前还是后续*/
	uint32_t pay_type;						/*数据类型eg. per min or per view*/	
	uint8_t reset_num;				/*密码可重复次数*/
	uint32_t pos;                    /*购买的节目序号*/
	uint16_t period;				/*购买周期*/
	#endif
}ca_buy_ipp_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	uint32_t pos ;
	uint32_t  state; /*0: init 1: read feed data 2: write feed data */
	/******扩展*/
}ca_mother_child_card_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	char* oldpin;                           /*旧密码*/
	char* newpin;                           /*新密码*/
	char* newconfirmpin;                           /*确认新密码*/
	int32_t errorCode;  /*wufei add for error verify 0:success 1:fail*/
	/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint8_t reset_num;				/*密码可重复次数*/
	#endif
}ca_pin_t;


typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	char* pin;                           /*密码*/
	uint8_t rate;                           /*新成人级别*/

	/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint8_t reset_num;				/*密码可重复次数*/
	#endif
}ca_rating_t;

typedef struct
{
	dvb_ca_data_type_t date_type;  /*CA数据类型*/
	char* pin;                           /*密码*/
	char* starttime;
	char* endtime;
//	uint8_t starthour;                           /*开始小时*/
//	uint8_t startmin;                           /*开始分钟*/
//	uint8_t startsec;                           /*开始秒*/

//	uint8_t endhour;                           /*结束小时*/
//	uint8_t endmin;                           /*结束分钟*/
//	uint8_t endsec;                           /*结束秒*/

	/******扩展*/
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	uint8_t reset_num;				/*密码可重复次数*/
	#endif

}ca_work_time_t;

/*
*  获取对应数据之前，初始化数据管理
(一般创建对应窗体之前或create中调用)。
* 如初始化邮件、授权等
*/
typedef int32_t (*app_cas_api_init_data_callback)(dvb_ca_data_type_t);
int32_t app_cas_api_init_data(dvb_ca_data_type_t date_type);


/*
* 获取对应CA信息
* date_type -- CA信息类型
* pos -- 序号
* ID -- 对应基本信息枚举
* (公共接口统一返回字符串，数值由不同CA自行转换)
*/
typedef char* (*app_cas_api_get_data_callback)(ca_get_date_t*);
char * app_cas_api_get_data(ca_get_date_t *data);

/*
* 获取CA信息个数(动态)
*/
typedef uint8_t (*app_cas_api_get_count_callback)(ca_get_count_t*);
uint8_t app_cas_api_get_count(ca_get_count_t *data);

/*
* 删除单个选中数据
* date_type -- CA信息类型
* pos -- 序号
*/
typedef bool (*app_cas_api_delete_callback)(ca_delete_data_t *);
bool app_cas_api_delete(ca_delete_data_t *data);

/*
* 删除所有数据
* date_type -- CA信息类型
*/
typedef bool (*app_cas_api_delete_all_callback)(ca_delete_data_t *);
bool app_cas_api_delete_all(ca_delete_data_t *data);

/*
* 购买如IPPV/IPPT等
*/
typedef uint8_t (*app_cas_api_buy_ipp_callback)(ca_buy_ipp_t *);
uint8_t app_cas_api_buy_ipp(ca_buy_ipp_t *data);


/*
* 获取应急广播状态
*/
typedef uint8_t (*app_cas_api_get_lock_status_callback)();
uint8_t app_cas_api_get_lock_status();

/*
* 子卡写入喂养数据
*/
typedef char* (*app_cas_api_feed_mother_child_card_callback)(ca_mother_child_card_t *);
char* app_cas_api_feed_mother_child_card(ca_mother_child_card_t *data);

/*
* 修改密码
*/
typedef char* (*app_cas_api_change_pin_callback)(ca_pin_t*);
char* app_cas_api_change_pin(ca_pin_t* data);


typedef char* (*app_cas_api_verify_pin_callback)(ca_pin_t*);
char* app_cas_api_verify_pin(ca_pin_t* data);



/*
* 修改成人级别
*/
typedef char* (*app_cas_api_change_rating_callback)(ca_rating_t*);
char* app_cas_api_change_rating(ca_rating_t* data);

/*
* 修改工作时段
*/
typedef char* (*app_cas_api_change_worktime_callback)(ca_work_time_t*);
char* app_cas_api_change_worktime(ca_work_time_t* data);
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
/*
*观看成人级节目
*/
typedef char* (*app_cas_api_watch_maturity_callback)(ca_rating_t*);
char* app_cas_api_watch_maturity(ca_rating_t* data);

/*
* 获取邮件状态
*/
typedef uint8_t (*app_cas_api_get_email_status_callback)();
uint8_t app_cas_api_get_email_status();

/*
* 获取OSD滚动状态
*/
typedef uint8_t (*app_cas_api_get_osdrolling_status_callback)();
uint8_t app_cas_api_get_osdrolling_status();
#endif

typedef struct {
	char*   name;
	int     key;
	void*   buf;
	size_t  size;
} GxMsgProperty0_OnEvent;


int app_cas_api_on_event(int key, const char* name, void* buf, size_t size);

typedef struct APP_CAS_API_ControlBlock_s
{
	app_cas_init_callback m_cas_init_callback;//ca模块初始化
 	app_cas_close_callback m_cas_close_callback;
	app_cas_api_card_in_callback m_cas_api_card_in_callback;//智能卡插入回调
	app_cas_api_card_out_callback m_cas_api_card_out_callback;//智能卡移除回调
	app_cas_api_release_ecm_filter_callback m_cas_api_release_ecm_filter_callback;
	app_cas_api_start_ecm_filter_callback m_cas_api_start_ecm_filter_callback;//过滤到Pmt表，调用
	app_cas_api_release_emm_filter_callback m_cas_api_release_emm_filter_callback;
	app_cas_api_start_emm_filter_callback m_cas_api_start_emm_filter_callback;//过滤到CAT表，调用
	app_cas_api_check_cat_ca_descriptor_valid_callback m_cas_api_check_cat_ca_descriptor_valid_callback;
	app_cas_api_check_pmt_ca_descriptor_valid_callback m_cas_api_check_pmt_ca_descriptor_valid_callback;
	app_cas_api_is_ca_pop_msg_type_callback m_cas_api_is_ca_pop_msg_type_callback;//ca消息判断
	app_cas_api_pop_msg_exec_callback m_cas_api_pop_msg_exec_callback;//播放ca消息框
	app_cas_api_osd_exec_callback m_cas_api_osd_exec_callback;//ca 消息(定时器任务)
	app_cas_api_gxmsg_ca_on_event_exec_callback m_cas_api_gxmsg_ca_on_event_exec_callback;//ca事件
	app_cas_api_init_data_callback m_cas_api_init_data_callback;
	app_cas_api_get_data_callback m_cas_api_get_data_callback;//获取菜单数据
	app_cas_api_get_count_callback m_cas_api_get_count_callback;
	app_cas_api_delete_callback m_cas_api_delete_callback;
	app_cas_api_delete_all_callback m_cas_api_delete_all_callback;
	app_cas_api_buy_ipp_callback m_cas_api_buy_ipp_callback;
	app_cas_api_get_lock_status_callback m_cas_api_get_lock_status_callback;
	app_cas_api_feed_mother_child_card_callback m_cas_api_feed_mother_child_card_callback;
	app_cas_api_change_pin_callback m_cas_api_change_pin_callback ;
	app_cas_api_verify_pin_callback m_cas_api_verify_pin_callback ;
	app_cas_api_change_rating_callback m_cas_api_change_rating_callback;
	app_cas_api_change_worktime_callback m_cas_api_change_worktime_callback;
	#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	app_cas_api_watch_maturity_callback m_cas_api_watch_maturity_callback;
	app_cas_api_get_email_status_callback m_cas_api_get_email_status_callback;
	app_cas_api_get_osdrolling_status_callback m_cas_api_get_osdrolling_status_callback;
	#endif
}APP_CAS_API_ControlBlock_t;

#if CAS_PRINT_OPEN

#define CAS_Dbg(...)                          do {                                            \
                                                    printf("[cas]\t");                        \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#define CAS_DUMP(str, ptr, size)                                                                    \
                    do {                                                                        \
                        int i;                                                                  \
                            printf("\t\%s  len=%d\n\t", (str), (size));     \
                        if (size != 0) {                                                        \
                            for (i = 0; i < (size); i++) {                                      \
                                printf("0x%02x,", (ptr)[i]);                                      \
                            }                                                                   \
                            printf("\n\t\n");                               \
                        }                                                                       \
                    } while (0)
#define CA_FAIL(...) do {                                            \
                                                    printf("[cas] CA_FAIL %s %d\t",__FILE__,__LINE__);                        \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#define CA_ERROR(...) do {                                            \
                                                    printf("[cas] CA_ERROR %s %d\t",__FILE__,__LINE__);                        \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#else
#define CAS_Dbg(...)                          do {                                            \
                                                } while(0)
#define CAS_DUMP(str, ptr, size)                                                                    \
                    do {                                                                        \
                    } while (0)
#define CA_FAIL(...) do {                                            \
                                                } while(0)
#define CA_ERROR(...) do {                                            \
                                                } while(0)

#endif
#if ADS_PRINT_OPEN
#define ADS_Dbg(...)                          do {                                            \
													printf("[ads]\t");						  \
													printf(__VA_ARGS__);						\
                                                } while(0)
#define ADS_DUMP(str, ptr, size)                                                                    \
                    do {                                                                        \
                        int i;                                                                  \
                            printf("\t\%s  len=%d\n\t", (str), (size));     \
                        if (size != 0) {                                                        \
                            for (i = 0; i < (size); i++) {                                      \
                                printf("0x%02x,", (ptr)[i]);                                      \
                            }                                                                   \
                            printf("\n\t\n");                               \
                        }                                                                       \
                    } while (0)
#define ADS_FAIL(...)		 do {                                            \
                                                    printf("[ADS] AD_FAIL %s %d\t",__FILE__,__LINE__);                        \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#define ADS_ERROR(...) 		do {                                            \
                                                    printf("[ADS] AD_ERROR %s %d\t",__FILE__,__LINE__);                        \
                                                    printf(__VA_ARGS__);                        \
                                                } while(0)
#else
#define ADS_Dbg(...)                          do {                                            \
                                                } while(0)
#define ADS_DUMP(str, ptr, size)                                                                    \
                    do {                                                                        \
                    } while (0)
#define ADS_FAIL(...) 		do {                                            \
                                                } while(0)
#define ADS_ERROR(...)		 do {                                            \
                                                } while(0)
#endif
#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_PORTING_CA_API_H__*/


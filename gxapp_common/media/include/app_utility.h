#ifndef __GXAPP_UTILITY_H__
#define __GXAPP_UTILITY_H__
#include "gxtype.h"
#include "gxcore.h"
#include <time.h>



/* BEGIN: Added by yingc, 2013/7/1 */
//#define ARRAY_LEN(array) (((NULL==array) ||(((void **)NULL)==array[0]))   ? 0:sizeof(array) / sizeof(array[0]))
#define ARRAY_LEN(array) ((NULL==array)   ? 0:sizeof(array) / sizeof(array[0]))
//#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

#define CMD_LEN_MAX (200)
#define IS_EMPTY_OR_NULL(str)	((NULL==str || str[0]=='\0') ? 1:0 )
#define NC_FREE(str)	{\
	if(NULL!=str)\
	free(str);\
	str=NULL;\
}\

#define GXCORE_FREE(str)	{\
	if(NULL!=str)\
	GxCore_Free(str);\
	str=NULL;\
}\

/* END:   Added by yingc, 2013/7/1   PN: */

typedef enum
{
	MD5_SUM_OK = 0,
	MD5_SUM_ERROR = 1
}Md5Sum;	

typedef struct
{
	char *quality_bar;
	char *strength_bar;
}SignalBarWidget;

typedef struct
{
	int quality_val;
	int strength_val;
}SignalValue;

typedef enum
{
	WEEK_SUN = 0,
	WEEK_MON, 
	WEEK_TUES, 
	WEEK_WED,
	WEEK_THURS,
	WEEK_FRI,
	WEEK_SAT
}WeekDay;

/*please GxCore_Free the memory by yourself*/
//char* get_widget_detail_name(char *base_name ,uint32_t index);

//char *get_str_duplication(char *src);

__BEGIN_DECLS
char* get_local_date(char *date_value,uint32_t data_len);

char* get_string_strcat(char *src,char *dst,char *separator);

time_t get_local_time_by_timezone(time_t src_date);

time_t get_display_time_by_timezone(time_t src_date);

WeekDay app_weekday_local2gmt(WeekDay local_weekday, time_t local_day_sec);

WeekDay app_weekday_gmt2local(WeekDay gmt_weekday, time_t gmt_day_sec);

bool check_usb_status(void);
uint32_t get_usb_partition_space(char *partition_path);
char *get_usb_url_partition(char *file_url);
char *app_time_to_date_edit_str(time_t time);
char *app_time_to_hourmin_edit_str(time_t time);
char *app_time_to_hms_str(time_t time);
char *app_time_to_format_str(time_t time);
status_t  app_edit_str_to_date_sec(char *date_str, time_t *res_sec);
time_t app_edit_str_to_hourmin_sec(char *time_str);

//float app_float_edit_str_to_value(const char *str);

status_t app_signal_progbar_update(uint32_t tuner, SignalBarWidget *bar, SignalValue *val);

uint32_t app_hexstr2value(uint8_t* string);
void app_getvalue(const char* url, const char* item, int32_t *retVal);
uint32_t app_key_full_code(uint16_t key_code);


/* BEGIN: Added by yingc, 2013/8/20 */
int GetFileSize(char * filepath,long *filesize);
int ExecuteCmd(const char *pc_cmd, char **ppc_result);   
int ExecuteCmdSyncNoQuery(const char *pc_cmd);

int GetChrCount(const char *pc_str,char c_ch);

Md5Sum Md5SumCheck(char *pcDir,char *pcSrcFile,char *pcMd5File);
int VersionCmp(char* pVersionOld,char* pVersionNew);

status_t GetFileContent(const char *pcFilePath,char **ppcOutBuf);
char * StrReplace(const char *pcSrc, const char *pcPattern, const char *pcReplace);
/* END:   Added by yingc, 2013/8/20   PN: */
__END_DECLS
#endif

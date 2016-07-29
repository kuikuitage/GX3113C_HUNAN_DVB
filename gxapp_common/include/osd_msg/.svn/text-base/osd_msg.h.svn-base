#ifndef __OSD_MSG_H__
#define __OSD_MSG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>

#define MAX_MSG_COUNT                   (64)
#define MAX_LEN_WINDOW_NAME    (100)

typedef enum osd_msg_pp_mode_e
{
	OSD_MSG_PP_MODE_NONE,
	OSD_MSG_PP_MODE_FULL,
	OSD_MSG_PP_MODE_SMALL
}osd_msg_pp_mode_t;

typedef struct  osd_msg_register_s
{
	int msg_no;
	char* content_short;
	char* content_long;
}osd_msg_register_t;

typedef struct osd_msg_process_s
{
	int msg_no;
	char* content;
}osd_msg_process_t;

typedef struct  osd_msg_item_s
{
	int msg_no;
	char* content_short;
	char* content_long;
	bool flag;
	int disable_time_ms;
}osd_msg_item_t;

typedef struct  osd_msg_list_s
{
	osd_msg_item_t msg[MAX_MSG_COUNT];
	int osd_msg_num;	
}osd_msg_list_t;

typedef struct  osd_msg_display_s
{
	char window[MAX_LEN_WINDOW_NAME];
	osd_msg_pp_mode_t pp_mode;
	int msg_no;
	char* content;
}osd_msg_display_t;

typedef struct  osd_msg_s
{
	osd_msg_list_t list;
	osd_msg_display_t display;
}osd_msg_t;

typedef void (*osd_msg_hide_cb)(void);
typedef void (*osd_msg_show_cb)(osd_msg_process_t* msg);

void osd_msg_init(void);
bool osd_msg_register(osd_msg_register_t* msg);
bool osd_msg_enable(int msg_no);
bool osd_msg_enable_timeout(int msg_no, int timeout_ms);
bool osd_msg_disable(int msg_no);
bool osd_msg_process(char* window, osd_msg_pp_mode_t pp_mode, osd_msg_show_cb show_cb, osd_msg_hide_cb hide_cb);
bool osd_msg_check(int msg_no);
int osd_msg_get_display_msg(void);

#ifdef __cplusplus
}
#endif
#endif /*__OSD_MSG_H__*/


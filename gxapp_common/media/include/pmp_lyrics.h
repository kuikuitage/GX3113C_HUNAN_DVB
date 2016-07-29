#ifndef __PMP_LYRICS_H__
#define __PMP_LYRICS_H__

#include <stdio.h>
#include <stdlib.h>
#include "gui_core.h"

typedef struct lrctimelist
{
	int32_t time;
	uint8_t number;
	struct lrctimelist* next;
}lrctimelist_t;

typedef struct
{
	/*create*/
	char* widget_lrc;
	char* lrc_buff;
	int lrc_buff_size;

	/*start*/
	char* file;
	event_list* timer;
	lrctimelist_t* list;
	uint32_t record_line;
}pmp_lrc_para;

#define SUFFIX_LRC					"lrc;LRC"

int lyrics_create(char* widget_lyc);
int lyrics_destroy(void);
int lyrics_start(char* file);
int lyrics_stop(void);
int lyrics_pause(void);
int lyrics_resume(void);
void lrc_timer_string_set(void);
void lrc_timer_string_Remove(void);


#endif


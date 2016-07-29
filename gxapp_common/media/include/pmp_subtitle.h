#ifndef __PMP_SUBTITLE_H__
#define __PMP_SUBTITLE_H__

#include "gxcore.h"
#include "module/player/gxplayer_module.h"
#include "gui_timer.h"
#include "gui_core.h"

#if 1
#define GxSubtPrintf(...) printf("[pmt_subt]"__VA_ARGS__)
#else
#define GxSubtPrintf(...) 
#endif

#define SUFFIX_SUBT		"idx;IDX;srt;SRT;ssa;SSA;ass;ASS;smi;SMI;sub;SUB"

typedef struct 
{
	int state;
	int osd_state;
	int type;
	int cur_subt;
	int delay_ms;
	int texts_len;
	char* texts_buf;
	char* file;
	char* widget_text;
	PlayerSubtitle* list;
	PlayerSubPara para;
}pmp_subt_para;

typedef enum{
	PMP_SUBT_LOAD_INIT,
	PMP_SUBT_LOAD_INSIDE,
	PMP_SUBT_LOAD_OUTSIDE,
	PMP_SUBT_LOAD_ERROR	
}pmp_subt_load_type;

int subtitle_create(char* widget_text);
int subtitle_destroy(void);
int subtitle_start(char* file,pmp_subt_load_type type);
int subtitle_stop(void);
int subtitle_pause(void);
int subtitle_resume(void);
pmp_subt_para* subtitle_get(void);

int app_subt_init(PlayerWindow* rect,int type);
void app_subt_destroy(int handle, int type);
//void app_subt_destroy(int handle);
int app_subt_show(int handle);
int app_subt_hide(int handle);
int app_subt_draw(void* data, int num, int type);
void app_subt_clear(int handle);
#endif


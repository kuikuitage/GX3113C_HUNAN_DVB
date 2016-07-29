#ifndef __FILE_VIEW_H__
#define __FILE_VIEW_H__

#include "gxtype.h"
#include "gxcore.h"
#include "gui_core.h"
#include "pmp_setting.h"
#include "pmp_explorer.h"

#ifdef __cplusplus
extern "C" {
#endif


#define FILE_VIEW_Printf(...)	printf(__VA_ARGS__)
#define FILE_VIEW_FREE(x)	if(x){GxCore_Free(x);x=NULL;}

typedef enum
{
	FILE_VIEW_GROUP_MOVIE,
	FILE_VIEW_GROUP_MUSIC,
	FILE_VIEW_GROUP_PIC,
	FILE_VIEW_GROUP_TEXT,
	FILE_VIEW_GROUP_ALL
}file_view_group;

char* file_view_get_dir_only(char* file);
char* file_view_get_base_name(char* file);
char* file_view_get_file_suffix(char* file);
file_view_group file_view_get_file_group(char* file);
char* file_view_get_group_suffixs(file_view_group group);
file_view_group file_view_get_record_group(void);
void file_view_set_record_group(file_view_group group);

extern explorer_para* explorer_view;


#ifdef __cplusplus
}
#endif

#endif /* __FILE_VIEW_H__ */


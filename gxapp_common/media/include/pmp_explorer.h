#ifndef __EXPLORER_H__
#define __EXPLORER_H__

#include "gxtype.h"
#include "gxcore.h"
#include "gui_core.h"
#include "pmp_setting.h"

#ifdef __cplusplus
extern "C" {
#endif


#define EXPLORER_Printf(...)	printf(__VA_ARGS__)
#define EXPLORER_FREE(x)	if(x){GxCore_Free(x);x=NULL;}

#define EXPLORER_MAX_PATH_DEPTH	50

typedef struct
{
	char*			path;
	char*			suffix;

	int				nents;
	GxDirent*		ents;

	int				path_depth;
	int				chdir_sel[EXPLORER_MAX_PATH_DEPTH];
}explorer_para;


char* explorer_static_path_strcat(const char* base_path, const char* file);
char* explorer_malloc_path_strcat(const char* base_path, const char* file);

explorer_para* explorer_opendir(const char* path, const char* suffix);
explorer_para* explorer_refreshdir(explorer_para* explorer);
status_t explorer_closedir(explorer_para* explorer);
explorer_para* explorer_chdir(explorer_para* explorer, int dir_index);
explorer_para* explorer_backdir(explorer_para* explorer);




#ifdef __cplusplus
}
#endif

#endif /* __EXPLORER_H__ */


#ifndef __FILE_EDIT_H__
#define __FILE_EDIT_H__

#include "gxtype.h"
#include "gxcore.h"
#include "file_view.h"


#ifdef __cplusplus
extern "C" {
#endif

#define FILE_EDIT_Printf(...)	printf(__VA_ARGS__)
#define FILE_EDIT_FREE(x)	if(x){GxCore_Free(x);x=NULL;}


typedef enum
{
	FILE_EDIT_FLAG_COPYED,
	FILE_EDIT_FLAG_CUTED,
	FILE_EDIT_FLAG_UNDO
}file_edit_flag;


status_t file_edit_delete(const char* file);
status_t file_edit_copy(const char* file);
status_t file_edit_cut(const char* file);
status_t file_edit_paste(const char* path);
status_t file_edit_rename(const char* file);
bool file_edit_paste_check_exist(const char* path);
bool file_edit_check_paste_valid(const char *paste_path);


#ifdef __cplusplus
}
#endif

#endif /* __FILE_EDIT_H__ */


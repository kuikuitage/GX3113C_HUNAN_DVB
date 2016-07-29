#include "pmp_explorer.h"
//#include "app.h"
#include <string.h>
#include "app_common_media.h"

#if 0
static int _get_take_millisecond(GxTime start, GxTime stop)
{
	int start_ms, stop_ms;

	start_ms = start.seconds * 1000 + start.microsecs / 1000;
	stop_ms = stop.seconds * 1000 + stop.microsecs / 1000;

	return (stop_ms - start_ms);
}
#endif

char* explorer_static_path_strcat(const char* base_path, const char* file)
{
#define MAX_PATH_LEN	1000

	int base_len = 0;
	int file_len = 0;
	static char buffer[MAX_PATH_LEN];
	
	if(NULL == base_path) return (char*)file;
	if(NULL == file) return NULL;

	base_len = strlen(base_path);
	file_len = strlen(file);

	if(base_len + file_len >= MAX_PATH_LEN) return NULL;

	memset(buffer, 0, MAX_PATH_LEN);
	
	memcpy(buffer, base_path, base_len);
	if('/' == base_path[base_len - 1])
	{
		memcpy(buffer + base_len, file, file_len);
		buffer[base_len + file_len] = '\0';
	}
	else
	{
		buffer[base_len] = '/';
		memcpy(buffer + base_len + 1, file, file_len);
		buffer[base_len + file_len + 1] = '\0';
	}

	return buffer;	
}

char* explorer_malloc_path_strcat(const char* base_path, const char* file)
{
	int base_len = 0;
	int file_len = 0;
	char* buffer = NULL;
	
	if(NULL == base_path) return (char*)file;
	if(NULL == file) return NULL;

	base_len = strlen(base_path);
	file_len = strlen(file);

	buffer = (char*)GxCore_Malloc(base_len + file_len + 3);
	if(NULL == buffer) return NULL;
	memset(buffer, 0, base_len + file_len + 3);
	
	memcpy(buffer, base_path, base_len);
	if('/' == base_path[base_len - 1])
	{
		memcpy(buffer + base_len, file, file_len);
		buffer[base_len + file_len] = '\0';
	}
	else
	{
		buffer[base_len] = '/';
		memcpy(buffer + base_len + 1, file, file_len);
		buffer[base_len + file_len + 1] = '\0';
	}

	return buffer;	
}


explorer_para* explorer_opendir(const char* path, const char* suffix)
{
	explorer_para* explorer = NULL;

	if(NULL == path) return NULL;
	EXPLORER_Printf("[EXPLORER] opendir: %s, suffix: %s\n", path, suffix);

	explorer = GxCore_Malloc(sizeof(explorer_para));
	if(NULL == explorer) return NULL;
	memset(explorer, 0, sizeof(explorer_para));

	explorer->path = GxCore_Strdup(path);
	if(NULL == suffix) 
		explorer->suffix = NULL;
	else
		explorer->suffix = GxCore_Strdup(suffix);

	explorer->nents = GxCore_GetDir(path, &(explorer->ents), suffix);
	GxCore_SortDir(explorer->ents, explorer->nents, NULL);

	return explorer;
}

explorer_para* explorer_refreshdir(explorer_para* explorer)
{
	explorer_para* explorer_old = NULL;
	explorer_para* explorer_new = NULL;

	APP_CHECK_P(explorer, NULL);
	
	explorer_old = explorer;
	explorer_new = explorer_opendir(explorer_old->path, explorer_old->suffix);
	if(explorer_new)
	{
		memcpy(explorer_new->chdir_sel, explorer_old->chdir_sel, sizeof(int)*EXPLORER_MAX_PATH_DEPTH);
		explorer_new->path_depth = explorer_old->path_depth;
	}
	
	explorer_closedir(explorer_old);

	return explorer_new;
}

status_t explorer_closedir(explorer_para* explorer)
{
	if(NULL == explorer) return GXCORE_ERROR;
	EXPLORER_Printf("[EXPLORER] closedir %s\n", explorer->path);
	EXPLORER_FREE(explorer->path);
	EXPLORER_FREE(explorer->suffix);
	GxCore_FreeDir(explorer->ents, explorer->nents);
	explorer->nents = 0;
	EXPLORER_FREE(explorer);

	return GXCORE_SUCCESS;
}

explorer_para* explorer_chdir(explorer_para* explorer, int dir_index)
{
	explorer_para* explorer_new = NULL;
	explorer_para* explorer_old = NULL;
	char* path = NULL;

	if(NULL == explorer) return NULL;
	if(dir_index > explorer->nents) return NULL;

	char* dir_file = NULL;
	dir_file = explorer->ents[dir_index - 1].fname;
	EXPLORER_Printf("[EXPLORER] chdir %s in explorer %s\n", dir_file, explorer->path);

	if(0 == strcasecmp(dir_file, ".."))
	{
		return explorer_backdir(explorer);
	}

	if(EXPLORER_MAX_PATH_DEPTH <= explorer->path_depth)
		return NULL;

	path = explorer_static_path_strcat(explorer->path, dir_file);
	if(NULL == path) return NULL;

	explorer_old = explorer;
	explorer_new = explorer_opendir(path, explorer->suffix);

	if(explorer_new)
	{
		memcpy(explorer_new->chdir_sel, explorer_old->chdir_sel, sizeof(int)*EXPLORER_MAX_PATH_DEPTH);
		explorer_new->chdir_sel[explorer_old->path_depth] = dir_index;
		explorer_new->path_depth = explorer_old->path_depth + 1;
	}

	explorer_closedir(explorer_old);

	return explorer_new;
}

explorer_para* explorer_backdir(explorer_para* explorer)
{
	explorer_para* explorer_new = NULL;
	explorer_para* explorer_old = NULL;
	char* back_path = NULL;
	int path_len = 0;
	int i = 0;
	
	if(NULL == explorer) return NULL;
	EXPLORER_Printf("[EXPLORER] backdir in explorer %s\n", explorer->path);
	
	back_path = GxCore_Strdup(explorer->path);
	path_len = strlen(explorer->path);
	
	for(i = path_len - 2; i > 0; i--)
	{
		if('/' == back_path[i])
		{
			back_path[i] = '\0';
			break;
		}
	}

	if(0 == i)
	{
		explorer_old = explorer;
		explorer_new = explorer_opendir("/", explorer->suffix);
		if(explorer_new)
		{
			memcpy(explorer_new->chdir_sel, explorer_old->chdir_sel, sizeof(int)*EXPLORER_MAX_PATH_DEPTH);
			explorer_new->path_depth = 0;
		}
	}
	else
	{
		explorer_old = explorer;
		explorer_new = explorer_opendir(back_path, explorer->suffix);
		if(explorer_new)
		{
			memcpy(explorer_new->chdir_sel, explorer_old->chdir_sel, sizeof(int)*EXPLORER_MAX_PATH_DEPTH);

			explorer_new->path_depth = explorer_old->path_depth - 1;
			if(0 > explorer_new->path_depth)
				explorer_new->path_depth = 0;
		}
	}

	explorer_closedir(explorer_old);
	EXPLORER_FREE(back_path);
	
	return explorer_new;
}


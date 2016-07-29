#include "file_view.h"
#include "pmp_explorer.h"
#include <string.h>

static file_view_group s_group_record = FILE_VIEW_GROUP_ALL;

#if 0
file_view_dir file_view = {0};

static int _get_take_millisecond(GxTime start, GxTime stop)
{
	int start_ms, stop_ms;

	start_ms = start.seconds * 1000 + start.microsecs / 1000;
	stop_ms = stop.seconds * 1000 + stop.microsecs / 1000;

	return (stop_ms - start_ms);
}

	
static status_t add_file(file_view_files* files, char* name)
{
	//FILE_VIEW_Printf("add_file: %s\n", name);
	files->ents[files->nents] = name;
	files->nents++;
	return GXCORE_SUCCESS;
}


bool  check_extend_name(const char* name, const char* extend)
{
	int name_len = 0;
	int extend_len = 0;
	char* extend_name = NULL;

	if(NULL == name || NULL == extend)
	{
		return FALSE;
	}

	/* //clib not support strcasestr
	if(strcasestr(name, extend))
		return TRUE;
	else
		return FALSE;
	*/

	name_len = strlen(name);
	extend_len = strlen(extend);
	
	if(name_len <= extend_len)
	{
		//printf( "=_check_extend_name '%s', '%s'= name_len < extend_len\n", name, extend);
		return FALSE;
	}
	
	extend_name = (char*)name + name_len -extend_len;
	
	if(0 == strcasecmp((const char*)extend_name,extend))
	{
		return TRUE;
	}

	return FALSE;
}


char* explorer_static_path_strcat(const char* base_path, const char* file)
{
#define MAX_PATH_LEN	500

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

char* file_view_malloc_path_strcat(const char* base_path, const char* file)
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



void file_view_init(void)
{
	memset(&file_view, 0, sizeof(file_view_dir));
	return;
}




status_t file_view_opendir(const char* path)
{
	handle_t handle = GXCORE_INVALID_POINTER;
	GxDir* dir = NULL;
	GxFileInfo file_info;

	//FILE_VIEW_Printf("=opendir=%s\n", path);

	if(NULL == path) return GXCORE_ERROR;
	
	handle = GxCore_FileExists(path);
	if(GXCORE_FILE_UNEXIST == handle)
	{
		printf( "[FILE] view opendir '%s' unexist\n", path);
		return GXCORE_ERROR;
	}

	GxCore_GetFileInfo(path, &file_info);
	if(GX_FILE_DIRECTORY != file_info.type)
	{
		printf( "[FILE]  view opendir '%s' is not a directory\n", path);
		return GXCORE_ERROR;
	}

	dir = GxCore_Opendir(path);
	if(NULL == dir)
	{
		printf( "[FILE] view opendir '%s' Error\n", path);
		return GXCORE_ERROR;
	}

	file_view_closedir();

	file_view.path = GxCore_Strdup(path);
	if(NULL == file_view.path) return GXCORE_ERROR;
	file_view.dir_open = dir;
	
	file_view.files.ents = GxCore_Malloc(dir->nents * sizeof(char*));
	if(NULL == file_view.files.ents) return GXCORE_ERROR;
	
	//ret = file_view_filter(file_view.group);
	return GXCORE_SUCCESS;
}

status_t file_view_closedir(void)
{
	if(NULL != file_view.dir_open)
	{
		FILE_VIEW_FREE(file_view.path);
		GxCore_Closedir(file_view.dir_open);
		file_view.dir_open = NULL;
		//file_view.dir_depth = 0;
		
		file_view.files.reg_ent_start = 0;
		FILE_VIEW_FREE(file_view.files.ents);
		file_view.files.nents = 0;
	}
	return GXCORE_SUCCESS;
}

status_t file_view_chdir(const char* dir_file)
{
	status_t ret = GXCORE_ERROR;
	char* path = NULL;

	//FILE_VIEW_Printf("=chdir=%s\n", dir_file);
	
	if(NULL == dir_file) return GXCORE_ERROR;
	if(NULL == file_view.path) return GXCORE_ERROR;
	
	path = explorer_static_path_strcat(file_view.path, dir_file);
	if(NULL == path) return GXCORE_ERROR;
	ret = file_view_opendir(path);
	
	if(GXCORE_SUCCESS == ret)
	{
		//FILE_VIEW_Printf("depth:%d to %d\n", file_view.dir_depth, file_view.dir_depth + 1);
		file_view.dir_depth++;
		ret = file_view_filter(file_view.files.group);
	}
	
	return ret;
}

status_t file_view_backdir(void)
{
	char* path = NULL;
	int path_len = 0;
	int i = 0;
	status_t ret = GXCORE_ERROR;
	
	//FILE_VIEW_Printf("=backdir=\n");
	
	if(NULL == file_view.path) return GXCORE_ERROR;
	if(0 == file_view.dir_depth) return GXCORE_ERROR;

	path = GxCore_Strdup(file_view.path);
	path_len = strlen(path);
	
	for(i = path_len - 2; i > 0; i--)
	{
		if('/' == path[i])
		{
			//FILE_VIEW_Printf("depth:%d to %d\n", file_view.dir_depth, file_view.dir_depth - 1);
			path[i] = '\0';
			file_view.dir_depth--;
			ret = file_view_opendir(path);
			break;
		}
	}
	if(0 == i)
	{
		file_view.dir_depth = 0;
		ret = file_view_opendir("/");
	}
	FILE_VIEW_FREE(path);

	if(GXCORE_SUCCESS == ret)
	{
		ret = file_view_filter(file_view.files.group);
	}
	
	return ret;
}

status_t file_view_filter(file_view_group group)
{
	GxFileInfo file_info;
	char* path = NULL; 
	GxDir* dir = NULL; 
	int i = 0;
	char** tmp_regular_ents = NULL;
	int tmp_regular_nents = 0;
	GxTime start;
	GxTime stop;

	//FILE_VIEW_Printf("=file_view_filter=\n");
	
	if(NULL == file_view.path) return GXCORE_ERROR;
	if(NULL == file_view.dir_open) return GXCORE_ERROR;
	if(NULL == file_view.files.ents) return GXCORE_ERROR;

	//create tmp buff save regular files, for filter one time
	tmp_regular_ents = GxCore_Malloc(file_view.dir_open->nents * sizeof(char*));
	if(NULL == tmp_regular_ents) return GXCORE_ERROR;

	
	file_view.files.group = group;
	file_view.files.reg_ent_start = 0;
	memset(file_view.files.ents, 0, file_view.dir_open->nents * sizeof(char*));
	file_view.files.nents= 0;
	
	/*add dirctory files first*/
	GxCore_GetLocalTime(&start);
	dir = file_view.dir_open;
	for(i = 0; i < dir->nents; i++)
	{
		if(0 == strcasecmp(dir->ents[i], ".") 
			||(0 == strcasecmp(dir->ents[i], "..")) )
		{
			continue;
		}

		path = explorer_static_path_strcat(file_view.path, dir->ents[i]);
		if(NULL == path) return GXCORE_ERROR;
		// TODO:
		//GxCore_GetFileInfo((const char*)path, &file_info);
		if(strstr(dir->ents[i], "."))
		{
			file_info.type = GX_FILE_REGULAR;
		}
		else
		{
			file_info.type = GX_FILE_DIRECTORY;
		}

		
		if(GX_FILE_DIRECTORY == file_info.type)
		{
			add_file(&file_view.files, dir->ents[i]);
		}
		else if(GX_FILE_REGULAR == file_info.type)
		{
			if(file_view_get_file_group(dir->ents[i], group))
			{
				tmp_regular_ents[tmp_regular_nents] = dir->ents[i];
				tmp_regular_nents++;
			}
		}
	}
	GxCore_GetLocalTime(&stop);
 	FILE_VIEW_Printf("file_view_filter time take %d ms\n", _get_take_millisecond(start, stop));


	/*add group regular files later*/
	file_view.files.reg_ent_start = file_view.files.nents;
	memcpy(&(file_view.files.ents[file_view.files.nents]), tmp_regular_ents, sizeof(char*)*tmp_regular_nents);
	file_view.files.nents += tmp_regular_nents;
	FILE_VIEW_FREE(tmp_regular_ents);

	
	/*
	for(i = 0; i < dir->nents; i++)
	{
		path = explorer_static_path_strcat(file_view.path, dir->ents[i]);
		if(NULL == path) return GXCORE_ERROR;
		GxCore_GetFileInfo((const char*)path, &file_info);
		
		if(GX_FILE_REGULAR == file_info.type)
		{
			if(file_view_get_file_group(dir->ents[i], group))
			{
				add_file(&file_view.files, dir->ents[i]);
			}
		}
	}
	*/
	
	return GXCORE_SUCCESS;
}





bool  file_view_check_group(const char* name, file_view_group group)
{
	bool ret = FALSE;
	
	switch(group)
	{
		case FILE_VIEW_GROUP_ALL:
			ret = TRUE;
			break;

		case FILE_VIEW_GROUP_PIC:
			if(check_extend_name(name, ".jpg")
				||check_extend_name(name, ".jpeg")
				||check_extend_name(name, ".bmp")
				||check_extend_name(name, ".png")
				||check_extend_name(name, ".tiff") )
			{
				ret = TRUE;
			}
			break;

		case FILE_VIEW_GROUP_MUSIC:
			if(check_extend_name(name, ".mp3")
				||check_extend_name(name, ".asf")
				||check_extend_name(name, ".ogg")
				||check_extend_name(name, ".wma"))
			{
				ret = TRUE;
			}
			break;

		case FILE_VIEW_GROUP_MOVIE:
			if(check_extend_name(name, ".mkv")
				||check_extend_name(name, ".avi")
				||check_extend_name(name, ".ts")
				||check_extend_name(name, ".mp4")
				||check_extend_name(name, ".dvr")
				||check_extend_name(name, ".mpg") )
			{
				ret = TRUE;
			}
			break;

		case FILE_VIEW_GROUP_TEXT:
			if(check_extend_name(name, ".txt")
				||check_extend_name(name, ".log")
				||check_extend_name(name, ".ini"))
			{
				ret = TRUE;
			}
			break;

		default:
			ret = FALSE;
			break;
	}

	return ret;
}






int file_view_get_dir_depth(void)
{
	return file_view.dir_depth;
}

char* file_view_get_path(void)
{
	return file_view.path;
}

int file_view_get_count(void)
{
	if(NULL == file_view.path) return 0;
	if(NULL == file_view.dir_open) return 0;
	if(NULL == file_view.files.ents) return 0;

	return file_view.files.nents;
}

status_t file_view_get_item(int index, file_view_fileinfo* info)
{
	char* path = NULL;
	
	if(NULL == info) return GXCORE_ERROR;
	if(NULL == file_view.path) return GXCORE_ERROR;
	if(NULL == file_view.dir_open) return GXCORE_ERROR;
	if(NULL == file_view.files.ents) return GXCORE_ERROR;
	if(index >= file_view.files.nents) return GXCORE_ERROR;

	memset(info, 0, sizeof(file_view_fileinfo));

	path = explorer_static_path_strcat(file_view.path, file_view.files.ents[index]);
	if(NULL == path) return GXCORE_ERROR;
	// TODO: 
	//GxCore_GetFileInfo((const char*)path, &(info->info));
	if(strstr(file_view.files.ents[index], "."))
	{
		info->info.type = GX_FILE_REGULAR;
	}
	else
	{
		info->info.type = GX_FILE_DIRECTORY;
	}


	info->name = file_view.files.ents[index];
	
	if(FILE_VIEW_GROUP_ALL == file_view.files.group)
	{
		if(GX_FILE_REGULAR == info->info.type)
		{
			if(file_view_check_group(info->name, FILE_VIEW_GROUP_MOVIE))
				info->group = FILE_VIEW_GROUP_MOVIE;
			else if(file_view_check_group(info->name, FILE_VIEW_GROUP_MUSIC))
				info->group = FILE_VIEW_GROUP_MUSIC;
			else if(file_view_check_group(info->name, FILE_VIEW_GROUP_PIC))
				info->group = FILE_VIEW_GROUP_PIC;
			else if(file_view_check_group(info->name, FILE_VIEW_GROUP_TEXT))
				info->group = FILE_VIEW_GROUP_TEXT;
			else
				info->group = FILE_VIEW_GROUP_ALL;
		}
	}
	else
	{
		info->group = file_view.files.group;
	}


	return GXCORE_SUCCESS;
}


status_t file_view_opendev(const char* dev)
{
	status_t ret = GXCORE_ERROR;

	file_view_closedir();
	file_view.dir_depth = 0;
	
	if(0 == strcasecmp(dev, "USB"))
	{
		ret = file_view_opendir("/mnt/usb0_1");
	}
	else if(0 == strcasecmp(dev, "SD"))
	{
		ret = file_view_opendir("/home/gx");
	
	}
	else if(0 == strcasecmp(dev, "ROOT"))
	{
		ret = file_view_opendir("/");
	
	}
	
	return ret;
}
#endif




char* file_view_get_dir_only(char* file)
{
	int len = 0;
	int i = 0;
	char* dir_only = NULL;
	GxFileInfo info;
	
	if(NULL == file) return NULL;
	dir_only = file;
	
	GxCore_GetFileInfo(file, &info);
	if(GX_FILE_REGULAR == info.type)
	{
		printf("cut name, get dir only\n");
		//fine directory
		len = strlen(file);
		for(i = 0; i < len; i++)
		{
			if('/' == *(file + len - i -1) && (i  != 0) )
			{
				*(file + len - i -1) = '\0';
				break;
			}
		}
	}

	return dir_only;
}

char* file_view_get_base_name(char* file)
{
	int len = 0;
	int i = 0;
	char* base_name = NULL;
	
	if(NULL == file) return NULL;
	if('/' != *(char*)file) return NULL;
	if(1 == strlen(file)) return NULL;
	
	len = strlen(file);
	base_name = (char*)file;
	for(i = len - 1; i >= 0; i--)
	{
		if( ('/' == *(char*)(file + i)) && (i != len-1) )
		{
			base_name = (char*)(file + i + 1);
			break;
		}
	}

	return base_name;
}


char* file_view_get_file_suffix(char* file)
{
	char* file_name = NULL;
	char* file_suffix = NULL;
	int len = 0;
	int i = 0;
	
	if(NULL == file) return NULL;

	file_name = file;
	len = strlen(file_name);
	if(3 > len) return NULL;

	for(i = len -2; i >0; i--)
	{
		if('.' == file_name[i])
			break;
	}

	if(0 == i) return NULL;

	file_suffix = &file_name[i+1];

	return file_suffix;
}


#define FILE_VIEW_SUFFIX_ALL	NULL
#define FILE_VIEW_SUFFIX_PIC	"jpg;JPG;jpeg;JPEG;bmp;BMP;"//"jpg;JPG;jpeg;JPEG;bmp;BMP;png:PNG;tiff;TIFF;gif;GIF"
#define FILE_VIEW_SUFFIX_MUSIC	"mp3;MP3;aac;AAC;m4a;M4A;ac3;AC3"
#define FILE_VIEW_SUFFIX_MOVIE	"mkv;MKV;avi;AVI;ts;TS;mp4;MP4;mpg;MPG;flv;FLV;dat;DAT;vob;VOB;tp;TP;trp;TRP;mov;MOV;dvr;MPEG;mpeg;m2ts;M2TS"
#define FILE_VIEW_SUFFIX_TEXT	"txt;TXT;log;LOG;ini;INI;xml;XML;lrc;LRC"

file_view_group file_view_get_file_group(char* file)
{
	char* file_suffix = NULL;
	
	if(NULL == file) return FILE_VIEW_GROUP_ALL;
	
	if(FILE_VIEW_GROUP_ALL != s_group_record)
	{
		return s_group_record;
	}
	
	file_suffix = file_view_get_file_suffix(file);
	if(NULL == file_suffix) return FILE_VIEW_GROUP_ALL;

	if(0 == strcasecmp(file_suffix, "bmp")
		|| 0 == strcasecmp(file_suffix, "jpg")
		|| 0 == strcasecmp(file_suffix, "jpeg"))
	{
		return FILE_VIEW_GROUP_PIC;
	}
	
	else if(0 == strcasecmp(file_suffix, "mp3")
		|| 0 == strcasecmp(file_suffix, "aac")
//|| 0 == strcasecmp(file_suffix, "wav")
		|| 0 == strcasecmp(file_suffix, "m4a")
		|| 0 == strcasecmp(file_suffix, "ogg")
		|| 0 == strcasecmp(file_suffix, "ac3"))
	{
		return FILE_VIEW_GROUP_MUSIC;
	}	

	else if(0 == strcasecmp(file_suffix, "mkv")
		|| 0 == strcasecmp(file_suffix, "avi")
		|| 0 == strcasecmp(file_suffix, "ts")
		|| 0 == strcasecmp(file_suffix, "mp4")
		|| 0 == strcasecmp(file_suffix, "mpg")
		|| 0 == strcasecmp(file_suffix, "flv")
		|| 0 == strcasecmp(file_suffix, "dat")
		|| 0 == strcasecmp(file_suffix, "vob")
		|| 0 == strcasecmp(file_suffix, "tp")
		|| 0 == strcasecmp(file_suffix, "trp")
		|| 0 == strcasecmp(file_suffix, "dvr")
		|| 0 == strcasecmp(file_suffix, "mov")
		|| 0 == strcasecmp(file_suffix, "mpeg")
		|| 0 == strcasecmp(file_suffix, "m2ts"))
		
	{
		return FILE_VIEW_GROUP_MOVIE;
	}
	
	
	else if(0 == strcasecmp(file_suffix, "txt")
		|| 0 == strcasecmp(file_suffix, "log")
		|| 0 == strcasecmp(file_suffix, "ini")
		|| 0 == strcasecmp(file_suffix, "xml")
		|| 0 == strcasecmp(file_suffix, "lrc"))
	{
		return FILE_VIEW_GROUP_TEXT;
	}

	return FILE_VIEW_GROUP_ALL;
}


char* file_view_get_group_suffixs(file_view_group group)
{
	char* suffixs = NULL;
	
	switch(group)
	{
		case FILE_VIEW_GROUP_ALL:
			suffixs = FILE_VIEW_SUFFIX_ALL;
			break;

		case FILE_VIEW_GROUP_PIC:
			suffixs = FILE_VIEW_SUFFIX_PIC;
			break;

		case FILE_VIEW_GROUP_MUSIC:
			suffixs = FILE_VIEW_SUFFIX_MUSIC;
			break;

		case FILE_VIEW_GROUP_MOVIE:
			suffixs = FILE_VIEW_SUFFIX_MOVIE;
			break;
			
		case FILE_VIEW_GROUP_TEXT:
			suffixs = FILE_VIEW_SUFFIX_TEXT;
			break;

		default:
			suffixs = FILE_VIEW_SUFFIX_ALL;
			break;
	}

	return suffixs;
}

file_view_group file_view_get_record_group(void)
{
	return s_group_record;
}

void file_view_set_record_group(file_view_group group)
{
	s_group_record = group;
}




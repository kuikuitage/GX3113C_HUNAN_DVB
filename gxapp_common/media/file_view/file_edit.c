#include "file_edit.h"
#include "string.h"
#include <stdio.h>


static char* edit_path = NULL;
static file_edit_flag edit_flag = FILE_EDIT_FLAG_UNDO;

bool file_edit_check_paste_valid(const char *paste_path)
{
	if((edit_flag != FILE_EDIT_FLAG_UNDO)
		&& (paste_path != NULL)
		&& (edit_path != NULL)
		&&(strcmp(paste_path, edit_path) != 0))
			return true;

	return false;
}

static bool _in_same_partition(const char* src1, const char* src2)
{
	int partition_len = 0;
	int path_depth = 0;
	int src_len = 0;

	if(NULL == src1) return false;
	if(NULL == src2) return false;
	
	src_len = strlen(src1);
	
	for(partition_len=0; partition_len < src_len; partition_len++)
	{	
		if('/'==*(char*)(src1 + partition_len))
		{
			path_depth++;
			if(path_depth > 2)// "/mnt/usb0_1/..."
			{				
				break;
			}
				
		}
	}	
	if(strncmp(src1, src2, partition_len )==0)
	{
		return true;
	}

	return false;
}

#define RD_WR_SIZE (64*1024)
static status_t _paste(const char* src, const char* dst)
{
	handle_t handle_src = GXCORE_INVALID_POINTER;
	handle_t handle_dst = GXCORE_INVALID_POINTER;
	ssize_t ret = 0;
	GxFileInfo info;
	GxDir* dir = NULL;
	char* src_sub = NULL;
	char* dst_sub = NULL;
	int src_len = 0;
	int i = 0;	
	char* name = NULL;
	//static uint8_t data_1B[1];
	uint8_t data_20KB[RD_WR_SIZE];

	if(NULL == src || NULL == dst)
	{
		return GXCORE_ERROR;
	}
	printf("[FILE] paste\n(src:%s)\n--->>---\n(dst:%s)\n", src, dst);
	
	src_len = strlen(src);
	name = file_view_get_base_name((char*)src);
	if(NULL == name) return GXCORE_ERROR;
	

	/*check src path and dst path*/
	if(src_len - strlen(name) == strlen(dst))
	{
	 	if(0 == strncmp( src, dst, strlen(dst) ) )
		{
			printf("[FILE] paste src path==dst path\n");
			return GXCORE_ERROR;
	 	}
	}


	/*paste regular file*/
	GxCore_GetFileInfo(src, &info);
	if(GX_FILE_REGULAR == info.type)
	{
		printf("addreg:%s--size:%lldB, \n", name,  info.size_by_bytes);
		dst_sub = explorer_malloc_path_strcat(dst, name);
		if(NULL == dst_sub) return GXCORE_ERROR;
		
		handle_src = GxCore_Open(src, "r");
		if(0 >= handle_src) return GXCORE_ERROR;
		handle_dst = GxCore_Open(dst_sub, "w+");
		if(0 >= handle_dst) 
		{
			GxCore_Close(handle_src);
			return GXCORE_ERROR;
		}
		
		for(i = 0; (i+RD_WR_SIZE) < info.size_by_bytes; i+=RD_WR_SIZE)
		{
			ret = GxCore_Read(handle_src, (void*)data_20KB, RD_WR_SIZE, 1);
			if(0 >= ret) goto faild;
			ret = GxCore_Write(handle_dst, (void*)data_20KB,RD_WR_SIZE, 1);
			if(0 > ret) goto faild;
		}
		
		int left_size = 0;
		left_size = info.size_by_bytes - i;
		printf("left size:%dB, \n", left_size);
		if(0 < left_size)
		{
			ret = GxCore_Read(handle_src, (void*)data_20KB, left_size, 1);
			if(0 > ret) goto faild;
			ret = GxCore_Write(handle_dst, (void*)data_20KB, left_size, 1);
			if(0 > ret) goto faild;
		}

faild:		
		GxCore_Close(handle_src);
		GxCore_Close(handle_dst);
		FILE_EDIT_FREE(dst_sub);
		printf("addreg:ok\n");
		return GXCORE_SUCCESS;
		
	}
	/*paste directory file*/
	else if(GX_FILE_DIRECTORY == info.type)
	{
		dst_sub = explorer_malloc_path_strcat(dst, name);
		if(NULL == dst_sub) return GXCORE_ERROR;
		
		printf("Mkdir:%s\n", dst_sub);
		GxCore_Mkdir(dst_sub);

		dir = GxCore_Opendir(src);
		if(NULL == dir)
		{
			FILE_EDIT_FREE(dst_sub);
			return GXCORE_ERROR;
		}
		
		for(i = 0; i < dir->nents; i++)
		{
			if( (0 == (strcmp(dir->ents[i], "."))) || (0 == (strcmp(dir->ents[i], "..") ) )  )
			{
				continue;
			}

			src_sub = explorer_malloc_path_strcat(src, dir->ents[i]);
			if(NULL == src_sub)
			{
				GxCore_Closedir(dir);
				FILE_EDIT_FREE(dst_sub);
				return GXCORE_ERROR;
			}
			
			_paste(src_sub, dst_sub);
			FILE_EDIT_FREE(src_sub);
		}
		
		GxCore_Closedir(dir);
		FILE_EDIT_FREE(dst_sub);
	}
	

	return GXCORE_SUCCESS;
}




status_t file_edit_delete(const char* file)
{
	handle_t handle = GXCORE_FILE_UNEXIST;
	status_t ret = GXCORE_SUCCESS;
	char* sub_file = NULL;
	GxFileInfo info;
	GxDir* dir = NULL;
	int i = 0;
	
	if(NULL == file)
	{
		return GXCORE_ERROR;
	}
	handle = GxCore_FileExists(file);
	if(GXCORE_FILE_UNEXIST == handle)
	{
		return GXCORE_ERROR;
	}
	GxCore_GetFileInfo(file, &info);

	
	if(GX_FILE_REGULAR == info.type)
	{
		printf("[FILE] delete-reg:%s\n", file);
		ret = GxCore_FileDelete(file);

		return ret;
	}
	else if(GX_FILE_DIRECTORY == info.type)
	{
		dir = GxCore_Opendir(file);
		if(NULL == dir)
		{
			return GXCORE_ERROR;
		}
		
		for(i = 0; i < dir->nents; i++)
		{
			if( (0 == (strcmp(dir->ents[i], "."))) || (0 == (strcmp(dir->ents[i], "..") ) )  )
			{
				continue;
			}

			sub_file = explorer_malloc_path_strcat(file, dir->ents[i]);
			if(NULL == sub_file)
			{
				GxCore_Closedir(dir);
				return GXCORE_ERROR;
			}
			
			if(GXCORE_ERROR == file_edit_delete(sub_file))
			{
				return GXCORE_ERROR;
			}
			FILE_EDIT_FREE(sub_file);
		}

		printf("[FILE] delete-dir:%s\n", file);
		GxCore_Closedir(dir);
		ret = GxCore_Rmdir(file);

		return ret;
	}

	return GXCORE_SUCCESS;
}



status_t file_edit_copy(const char* file)
{
	if(NULL == file)
	{	
		edit_flag = FILE_EDIT_FLAG_UNDO;
		return GXCORE_ERROR;
	}
	FILE_EDIT_FREE(edit_path);
	
	printf("[FILE] copy: %s\n", file);
	
	edit_flag = FILE_EDIT_FLAG_COPYED;
	edit_path = GxCore_Strdup(file);
	if(NULL == edit_path) return GXCORE_ERROR;
	
	return GXCORE_SUCCESS;
}

status_t file_edit_cut(const char* file)
{
	if(NULL == file)
	{	
		edit_flag = FILE_EDIT_FLAG_UNDO;
		return GXCORE_ERROR;
	}
	FILE_EDIT_FREE(edit_path);
	
	printf("[FILE] cut: %s\n", file);
	
	edit_flag = FILE_EDIT_FLAG_CUTED;
	edit_path = GxCore_Strdup(file);
	if(NULL == edit_path) return GXCORE_ERROR;
	
	return GXCORE_SUCCESS;
}

status_t file_edit_paste(const char* path)
{
	status_t ret = GXCORE_SUCCESS;
	handle_t handle = GXCORE_FILE_UNEXIST;
	GxFileInfo info;
	uint32_t i = 0;
	uint32_t len = 0;
	char* paste_path = NULL;

	paste_path = (char*)path;

	if(paste_path == NULL)
		return GXCORE_ERROR;
		
	/*check src*/
	if(FILE_EDIT_FLAG_UNDO == edit_flag)
	{
		return GXCORE_ERROR;
	}
	handle = GxCore_FileExists(edit_path);
	if(GXCORE_FILE_UNEXIST == handle)
	{
		//mkdir?
		return GXCORE_ERROR;
	}	

	/*check dst*/
	if(NULL == paste_path)
	{
		return GXCORE_ERROR;
	}
	handle = GxCore_FileExists(paste_path);
	if(GXCORE_FILE_UNEXIST == handle)
	{
		return GXCORE_ERROR;
	}
	GxCore_GetFileInfo(paste_path, &info);
	if(GX_FILE_REGULAR == info.type)
	{
		printf("cut name, get dir only\n");
		//find directory
		printf("=path=1=%s\n", paste_path);
		len = strlen(paste_path);
		for(i = 0; i < len; i++)
		{
			if('/' == *(paste_path + len - i -1) && (i  != 0) )
			{
				*(paste_path + len - i -1) = '\0';
				break;
			}
		}
		printf("=path=2=%s\n", paste_path);
	}


	
	if(FILE_EDIT_FLAG_CUTED == edit_flag)
	{
		if(true == _in_same_partition(edit_path, paste_path))//(false) 
		{
			char* name = NULL;
			char* rename_path = NULL;
			int ret=0;
			name = file_view_get_base_name(edit_path);
			if(NULL == name) return GXCORE_ERROR;

			rename_path=explorer_malloc_path_strcat(paste_path, name);
			if(NULL == rename_path) return GXCORE_ERROR;

			printf("[file cut] '%s' rename '%s'\n", edit_path, rename_path);
			ret=rename(edit_path, rename_path) ;
			printf("rename ret--------------------------=%d\n",ret);
			
			FILE_EDIT_FREE(rename_path);
		}
		else
		{
			printf("[file cut] '%s' paste to '%s' , then delete it\n", edit_path, paste_path);
			
			_paste(edit_path, paste_path);
			ret = file_edit_delete(edit_path);
			
		}
	}
	else
	{
		printf("[file copy] '%s' paste to '%s'\n", edit_path, paste_path);
		_paste(edit_path, paste_path);
	}
	

	edit_flag = FILE_EDIT_FLAG_UNDO;
	FILE_EDIT_FREE(edit_path);
	
	return ret;
}

status_t file_edit_rename(const char* file)
{
	return GXCORE_SUCCESS;
}


bool file_edit_paste_check_exist(const char* path)
{
	uint32_t i = 0;
	char* paste_path = NULL;
	char* name = NULL;
	char* src = NULL;
	GxDir* dir = NULL;

	src = edit_path;
	paste_path = (char*)path;
	
	if(FILE_EDIT_FLAG_UNDO == edit_flag)
	{
		return false;
	}
	if(NULL == src || NULL == paste_path)
	{
		return false;
	}

	name = file_view_get_base_name(src);
	if(NULL == name) return false;
	printf("[FILE] paste check file_name:%s\n", name);

	paste_path = file_view_get_dir_only(paste_path);
	printf("[FILE] paste check paste_path:%s\n", paste_path);
	
	dir = GxCore_Opendir(paste_path);
	if(NULL == dir)
	{
		return false;
	}
	for(i = 0; i < dir->nents; i++)
	{
		if( (0 == (strcmp(dir->ents[i], "."))) || (0 == (strcmp(dir->ents[i], "..") ) )  )
		{
			continue;
		}
		
		if(0 == strcasecmp(name, dir->ents[i]))
		{
			printf("[FILE] paste file exist\n");
			GxCore_Closedir(dir);
			return true;
		}
	}
	printf("[FILE] paste check file unexist\n");
	GxCore_Closedir(dir);
	return false;
}




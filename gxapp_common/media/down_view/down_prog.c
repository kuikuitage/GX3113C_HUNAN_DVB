#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "down_prog.h"
#include "down_prog_rss.h"
#include "down_prog_pc.h"
#include "tree.h"
#include "parser.h"

down_list_program object_list_program = {0};


status_t program_parser(const char *class, const char *filename)
{
	status_t ret = GXCORE_ERROR;
		
	if(0 == strcasecmp((const char*)DOWN_PROG_CLASS_RSS, class))
	{
		ret = program_parser_rss(filename);
	}
	else if(0 == strcasecmp((const char*)DOWN_PROG_CLASS_PC, class))
	{
		ret = program_parser_pc(filename);
	}
	else
	{
		printf("[DOWN] program parser, class '%s' not support\n", class);
	}
	
	return ret;
}
	
status_t program_get_count(const char* property, void* data)
{
	if(NULL == data)
	{
		return GXCORE_ERROR;
	}
	
	*(int32_t*)data = object_list_program.item_num;

	return GXCORE_SUCCESS;
}

status_t program_get_item(const char* property, int32_t no, void* data)
{
	int32_t index = 0;
	down_item_program* item = NULL;
	
	if(NULL == data)
	{
		return GXCORE_ERROR;
	}

	index = no;

	if(index >= object_list_program.item_num)
	{
		return GXCORE_ERROR;
	}
	
	item = object_list_program.items[index];
	if(NULL == item)
	{
		return GXCORE_ERROR;
	}
	*(down_item_program**)data = item;		

	return GXCORE_SUCCESS;
}

status_t  program_get_property(const char* property, void* data)
{

	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}
	
	return GXCORE_SUCCESS;
}


status_t  program_set_property(const char* property, void* data)
{
	
	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}

	//if(0 == strcasecmp(property, "release"))
	//{
	//	return list_release(&object_list_program);
	//}
	
	
	return GXCORE_SUCCESS;
}



down_ops down_ops_program = 
{
	.owner = "program",
	.parser = program_parser,
	.get_count = program_get_count,
	.get_item = program_get_item,
	.get = program_get_property,
	.set = program_set_property,
};





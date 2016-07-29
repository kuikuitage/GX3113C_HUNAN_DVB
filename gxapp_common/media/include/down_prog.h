#ifndef __DOWN_PROGRAM_H__
#define __DOWN_PROGRAM_H__
#include <gxcore.h>
#include "gxtype.h"
#include "down_view.h"

#define DOWN_PROG_CLASS_RSS			((unsigned char *)"rss")
#define DOWN_PROG_CLASS_PC			((unsigned char *)"pc")


typedef struct
{
	unsigned char*		title;
	unsigned char*		link;
	unsigned char*		size;
	unsigned char*		pubdate;
	unsigned char*		descriptor;
}down_item_program;

typedef struct
{
	uint32_t			item_num;
	down_item_program**	items;
}down_list_program;

extern down_list_program object_list_program;

extern down_ops down_ops_program;

#endif


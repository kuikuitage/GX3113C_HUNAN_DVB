#ifndef __DOWN_SITECFG_H__
#define __DOWN_SITECFG_H__
#include <gxcore.h>
#include "gxtype.h"
#include "down_view.h"


typedef struct
{
	unsigned char*		title;
	unsigned char*		link;
}channel_view;

typedef struct
{
	unsigned char*		name;
	unsigned char*		logo;
	unsigned char*		class;
	
	uint32_t			channel_num;
	uint32_t			visit_channel_no;
	channel_view**		channels;
}site_view;

typedef struct
{
	uint32_t		site_num;
	uint32_t		visit_site_no;
	site_view**		sites;
}down_view;



extern down_ops down_ops_sitecfg;



#endif


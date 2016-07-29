#ifndef __DOWN_H__
#define __DOWN_H__
#include <gxcore.h>
#include "gxtype.h"


#define DOWN_FREE(s) if(s){GxCore_Free(s);s=NULL;}


#ifndef xmlChar
typedef unsigned char xmlChar;
#endif

#define STR_DOWNLOAD	((unsigned char *)"download" )
#define STR_SITE		((unsigned char *)"site")
#define STR_NAME		((unsigned char *)"name")
#define STR_LOGO		((unsigned char *)"logo")
#define STR_OPS			((unsigned char *)"ops")
#define STR_CHANNEL		((unsigned char *)"channel")
#define STR_RSS			((unsigned char *)"rss")
#define STR_ITEM		((unsigned char *)"item")
#define STR_VIDEOLIST	((unsigned char *)"VideoList")
#define STR_VIDEO		((unsigned char *)"video")
#define STR_ID			((unsigned char *)"id")
#define STR_TITLE		((unsigned char *)"title")
#define STR_LINK		((unsigned char *)"link")
#define STR_DESCRIPTION	((unsigned char *)"description")
#define STR_CLASS		((unsigned char *)"class")
#define STR_LANGUAGE	((unsigned char *)"language")
#define STR_PUBDATE		((unsigned char *)"pubDate")
#define STR_MEDIA		((unsigned char *)"media")
#define STR_SIZE		((unsigned char *)"size")



typedef struct _down_ops{
	const char *owner;
	status_t (*parser)(const char* class, const char* filename);
	status_t (*get_count)(const char* property, void* data);
	status_t (*get_item)(const char* property, int32_t no, void* data);
	status_t (*get)(const char* property, void* data);
	status_t (*set)(const char* property, void* data);

	struct _down_ops *next;
}down_ops;

down_ops* down_get_ops(const char* name);
void down_view_init(void);

status_t down_view_start(const char* name, const char* class, const char* xmlfile);
status_t down_get_count(const char* name, const char* property, void* data);
status_t down_get_item(const char* name, const char* property, int32_t no, void* data);

status_t down_get_property(const char* name, const char* property, void* data);
status_t down_set_property(const char* name, const char* property, void* data);


#endif


#include <string.h>
#include <gxcore.h>
#include "down_view.h"
#include "down_sitecfg.h"
#include "down_prog.h"

down_ops* down_ops_list = NULL;

static int down_ops_register(const char *name, down_ops *ops)
{
	if (ops)
	{
		ops->next = down_ops_list;
		down_ops_list = ops;
		return 0;
	}
	return -1;
}

down_ops* down_get_ops(const char* name)
{
	down_ops* ops = NULL;

	if(NULL == down_ops_list)
	{
		return NULL;
	}

	ops = down_ops_list;

	while(ops)
	{
		if(0 == strcasecmp(name, ops->owner))
		{
			return (ops);
		}
			
		ops = ops->next;
	}

	printf("[DOWN] operation with '%s', not support!\n", name);

	return (NULL);
}


void down_view_init(void)
{
	down_ops_register("sitecfg", &down_ops_sitecfg);
	down_ops_register("program", &down_ops_program);
}

status_t down_view_start(const char* name, const char* class, const char* xmlfile)
{
	down_ops* ops = NULL;

	ops = down_get_ops(name);
	if(NULL == ops)
	{
		return GXCORE_ERROR;
	}
	
	printf("[DOWN] name: %s, class: %s, file: '%s'\n", name, class, xmlfile);

	return ops->parser(class, xmlfile);
}

status_t down_get_count(const char* name, const char* property, void* data)
{
	down_ops* ops = NULL;

	ops = down_get_ops(name);
	if(NULL == ops)
	{
		return GXCORE_ERROR;
	}

	return ops->get_count(property, data);
}

status_t down_get_item(const char* name, const char* property, int32_t no, void* data)
{
	down_ops* ops = NULL;

	ops = down_get_ops(name);
	if(NULL == ops)
	{
		return GXCORE_ERROR;
	}

	return ops->get_item(property, no, data);
}


status_t down_get_property(const char* name, const char* property, void* data)
{
	down_ops* ops = NULL;

	ops = down_get_ops(name);
	if(NULL == ops)
	{
		return GXCORE_ERROR;
	}

	return ops->get(property, data);
}

status_t down_set_property(const char* name, const char* property, void* data)
{
	down_ops* ops = NULL;

	ops = down_get_ops(name);
	if(NULL == ops)
	{
		return GXCORE_ERROR;
	}

	return ops->set(property, data);
}



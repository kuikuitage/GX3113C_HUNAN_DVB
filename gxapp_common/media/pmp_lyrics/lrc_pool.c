/*
 * lrc_pool.c
 * Copyright (C) 2006 absurd <xianjimli@hotmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include "lrc_pool.h"

struct _LrcPool
{
	size_t unit_size;
	size_t n_prealloc_units;
};

LrcPool* lrc_pool_new(size_t unit_size, size_t n_prealloc_units)
{
	LrcPool* thiz = LRC_ALLOC(LrcPool, 1);
	
	LRC_ASSERT(unit_size > 0);
	
	if(thiz != NULL)
	{
		thiz->unit_size = unit_size;
		thiz->n_prealloc_units = n_prealloc_units;
	}

	return thiz;
}

void*    lrc_pool_alloc(LrcPool* thiz)
{
	void* ret = NULL;
	
	LRC_ASSERT(thiz != NULL && thiz->unit_size > 0);
	
	if(thiz != NULL)
	{
		ret = LRC_CALLOC(thiz->unit_size, 1);
	}

	return ret;
}

void     lrc_pool_free(LrcPool* thiz, void* p)
{
	LRC_ASSERT(thiz != NULL && p != NULL);
	
	if(thiz != NULL && p != NULL)
	{
		GxCore_Free(p);
	}

	return;
}

void     lrc_pool_destroy(LrcPool* thiz)
{
	LRC_FREE(thiz);

	return;
}


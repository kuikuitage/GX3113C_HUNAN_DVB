/*
 * lrc_type.h
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


#ifndef LRC_TYPE_H
#define LRC_TYPE_H

#include <stddef.h>
#include <assert.h>
#include "gxcore.h"

enum _LRC_RESULT
{
	LRC_RESULT_OK,
	LRC_RESULT_FAIL,
	LRC_RESULT_NR
};

typedef enum _LRC_RESULT LRC_RESULT;

struct _LrcBuilder;
typedef struct _LrcBuilder LrcBuilder;

typedef LRC_RESULT (*LrcBuilderBegin)(LrcBuilder* thiz, const char* buffer);
typedef LRC_RESULT (*LrcBuilderOnIDTag)(LrcBuilder* thiz, const char* key, size_t key_length, 
					const char* value, size_t value_length);
typedef LRC_RESULT (*LrcBuilderOnTimeTag)(LrcBuilder* thiz, size_t start_time);
typedef LRC_RESULT (*LrcBuilderOnLrc)(LrcBuilder* thiz, const char* lrc, size_t lrc_length);
typedef LRC_RESULT (*LrcBuilderEnd)(LrcBuilder* thiz);
typedef LRC_RESULT (*LrcBuilderDestroy)(LrcBuilder* thiz);

struct _LrcBuilder
{
	LrcBuilderBegin     on_begin;
	LrcBuilderOnIDTag   on_id_tag;
	LrcBuilderOnTimeTag on_time_tag;
	LrcBuilderOnLrc     on_lrc;
	LrcBuilderEnd       on_end;
	LrcBuilderDestroy   destroy;

	char priv[1];
};

typedef int  (*LrcItemCompareFunc)(void* data, void* user_data);
typedef void (*LrcItemDestroyFunc)(void* data);

#define LRC_ASSERT(p) assert(p)
#define LRC_ALLOC(type, n) (type*)GxCore_Calloc(sizeof(type), n)
#define LRC_CALLOC(size, n) GxCore_Calloc(size, n)
#define LRC_MALLOC(size) GxCore_Malloc(size)
#define LRC_FREE(p) if(p != NULL) { GxCore_Free(p); p = NULL;}

#endif/*LRC_TYPE_H*/


/*
 * lrc_pool.h
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

#ifndef LRC_POOL_H
#define LRC_POOL_H

#include "lrc_type.h"

struct _LrcPool;
typedef struct _LrcPool LrcPool;

LrcPool* lrc_pool_new(size_t unit_size, size_t n_prealloc_units);
void*    lrc_pool_alloc(LrcPool* thiz);
void     lrc_pool_free(LrcPool* thiz, void* p);
void     lrc_pool_destroy(LrcPool* thiz);

#endif/*LRC_POOL_H*/

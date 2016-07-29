/*
 * lrc_list.h
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

#ifndef LRC_LIST_H
#define LRC_LIST_H

#include "lrc_type.h"

struct _LrcList;
typedef struct _LrcList LrcList;

struct _LrcListItem;
typedef struct _LrcListItem LrcListItem;

struct _LrcListIter
{
	LrcList*     list;
	LrcListItem* item;
};
typedef struct _LrcListIter LrcListIter;

LrcList* lrc_list_new(LrcItemDestroyFunc destroy, size_t n_prealloc);

LrcListIter lrc_list_insert_sorted(LrcList* thiz, void* p, LrcItemCompareFunc compare);
LrcListIter lrc_list_insert(LrcListIter* iter, void* p, int after);
void        lrc_list_remove(LrcList* thiz, void* p, LrcItemCompareFunc compare);
int         lrc_list_count(LrcList* thiz);
void        lrc_list_reset(LrcList* thiz);

LrcListIter lrc_list_first(LrcList* thiz);
LrcListIter lrc_list_last(LrcList* thiz);

LrcListIter lrc_list_iter_prev(LrcListIter* iter);
LrcListIter lrc_list_iter_next(LrcListIter* iter);
int         lrc_list_iter_has_prev(LrcListIter* iter);
int         lrc_list_iter_has_next(LrcListIter* iter);
int         lrc_list_iter_is_null(LrcListIter* iter);
void*       lrc_list_iter_data(LrcListIter* iter);

void  lrc_list_destroy(LrcList* thiz);

#endif/*LRC_LIST_H*/

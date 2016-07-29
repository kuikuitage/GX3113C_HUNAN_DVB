/*
 * lrc.h
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

#ifndef LRC_H
#define LRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lrc_list.h"

struct _LrcTree;
typedef struct _LrcTree Lrc;

typedef LrcListIter LrcIdTagIter;
typedef LrcListIter LrcTimeTagIter;

Lrc* lrc_new_from_buffer(const char* buffer);
Lrc* lrc_new_from_file(const char* filename);

LrcIdTagIter   lrc_get_id_tags(Lrc* thiz);
LrcTimeTagIter lrc_get_time_tags(Lrc* thiz);

LrcIdTagIter   lrc_get_id_tag_get_by_key(Lrc* thiz, const char* key);
LrcTimeTagIter lrc_get_time_tag_by_time(Lrc* thiz, size_t start_time);

LrcIdTagIter lrc_id_tag_iter_prev(LrcIdTagIter* iter);
LrcIdTagIter lrc_id_tag_iter_next(LrcIdTagIter* iter);
int          lrc_id_tag_iter_has_prev(LrcIdTagIter* iter);
int          lrc_id_tag_iter_has_next(LrcIdTagIter* iter);
int          lrc_id_tag_iter_is_null(LrcIdTagIter* iter);
const char*  lrc_id_tag_iter_get_key(LrcIdTagIter* iter);
const char*  lrc_id_tag_iter_get_value(LrcIdTagIter* iter);

LrcTimeTagIter lrc_time_tag_iter_prev(LrcTimeTagIter* iter);
LrcTimeTagIter lrc_time_tag_iter_next(LrcTimeTagIter* iter);
int            lrc_time_tag_iter_has_prev(LrcTimeTagIter* iter);
int            lrc_time_tag_iter_has_next(LrcTimeTagIter* iter);
int            lrc_time_tag_iter_is_null(LrcTimeTagIter* iter);
const char*    lrc_time_tag_iter_get_lrc(LrcTimeTagIter* iter);
size_t         lrc_time_tag_iter_get_start_time(LrcTimeTagIter* iter);
size_t         lrc_time_tag_iter_get_pause_time(LrcTimeTagIter* iter);
size_t         lrc_time_tag_iter_get_repeat_times(LrcTimeTagIter* iter);

void lrc_save(Lrc* thiz, const char* filename);

void lrc_destroy(Lrc* thiz);

#ifdef __cplusplus
}
#endif
#endif/*LRC_H*/


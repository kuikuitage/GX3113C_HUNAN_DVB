/*
 * lrc_time_tag.h
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

#ifndef LRC_TIME_TAG_H
#define LRC_TIME_TAG_H

#include "lrc_pool.h"

struct _LrcTimeTag;
typedef struct _LrcTimeTag LrcTimeTag;

size_t lrc_time_tag_size(void);
LrcTimeTag* lrc_time_tag_new(LrcPool* pool, size_t start_time, const char* lrc);

void lrc_time_tag_set_start_time(LrcTimeTag* thiz, size_t start_time);
void lrc_time_tag_set_lrc(LrcTimeTag* thiz, const char* lrc);
void lrc_time_tag_set_pause_time(LrcTimeTag* thiz, size_t pause_time);
void lrc_time_tag_set_repeat_times(LrcTimeTag* thiz, size_t repeat_times);

size_t lrc_time_tag_get_start_time(LrcTimeTag* thiz);
size_t lrc_time_tag_get_pause(LrcTimeTag* thiz);
size_t lrc_time_tag_get_repeat(LrcTimeTag* thiz);
size_t lrc_time_tag_get_pause_time(LrcTimeTag* thiz);
size_t lrc_time_tag_get_repeat_times(LrcTimeTag* thiz);

const char* lrc_time_tag_get_lrc(LrcTimeTag* thiz);

void lrc_time_tag_destroy(LrcTimeTag* thiz);

int lrc_time_tag_compare(void* data, void* user_data);

#endif/*LRC_TIME_TAG_H*/

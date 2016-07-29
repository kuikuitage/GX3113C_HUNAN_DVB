/*
 * lrc_id_tag.h
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

#ifndef LRC_ID_TAG_H
#define LRC_ID_TAG_H

#include "lrc_pool.h"

struct _LrcIdTag;
typedef struct _LrcIdTag LrcIdTag;

size_t    lrc_id_tag_size(void);

LrcIdTag* lrc_id_tag_new(LrcPool* pool, const char* key, const char* value);
void lrc_id_tag_set_key(LrcIdTag* thiz, const char* key);
void lrc_id_tag_set_value(LrcIdTag* thiz, const char* value);

const char* lrc_id_tag_get_key(LrcIdTag* thiz);
const char* lrc_id_tag_get_value(LrcIdTag* thiz);

void lrc_id_tag_destroy(LrcIdTag* thiz);

#endif/*LRC_ID_TAG_H*/

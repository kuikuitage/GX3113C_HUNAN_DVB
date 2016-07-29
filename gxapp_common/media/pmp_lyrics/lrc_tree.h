/*
 * lrc_tree.h
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

#ifndef LRC_TREE_H
#define LRC_TREE_H
#include "lrc_list.h"
#include "lrc_id_tag.h"
#include "lrc_time_tag.h"

struct _LrcTree;
typedef struct _LrcTree LrcTree;

LrcTree* lrc_tree_new(void);

void     lrc_tree_add_id_tag(LrcTree* thiz, LrcIdTag* id_tag);
void     lrc_tree_add_time_tag(LrcTree* thiz, LrcTimeTag* time_tag);

LrcList* lrc_tree_get_id_tags(LrcTree* thiz);
LrcList* lrc_tree_get_time_tags(LrcTree* thiz);

void     lrc_tree_set_user_data(LrcTree* thiz, void* user_data, LrcItemDestroyFunc user_data_destroy);
void     lrc_tree_destroy(LrcTree* thiz);

LRC_RESULT lrc_tree_save(LrcTree* thiz, LrcBuilder* builder);

#endif/*LRC_TREE_H*/

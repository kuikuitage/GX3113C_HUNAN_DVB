/*
 * lrc_tree.c
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

#include <string.h>
#include <stdlib.h>
#include "lrc_tree.h"
#include "lrc_type.h"

struct _LrcTree
{
	LrcList* id_tags;
	LrcList* time_tags;
	void* user_data;
	LrcItemDestroyFunc user_data_destroy;
};

LrcTree* lrc_tree_new(void)
{
	LrcTree* thiz = LRC_ALLOC(LrcTree, 1);

	if(thiz != NULL)
	{
		thiz->id_tags = lrc_list_new((LrcItemDestroyFunc)lrc_id_tag_destroy, 10);
		thiz->time_tags = lrc_list_new((LrcItemDestroyFunc)lrc_time_tag_destroy, 120);
	}

	return thiz;
}

void     lrc_tree_add_id_tag(LrcTree* thiz, LrcIdTag* id_tag)
{
	LRC_ASSERT(thiz != NULL && id_tag != NULL);

	if(thiz != NULL && id_tag != NULL)
	{
		LrcListIter iter = lrc_list_first(thiz->id_tags);
		lrc_list_insert(&iter, id_tag, 0);
	}

	return;
}

void     lrc_tree_add_time_tag(LrcTree* thiz, LrcTimeTag* time_tag)
{
	LRC_ASSERT(thiz != NULL && time_tag != NULL);

	if(thiz != NULL && time_tag != NULL)
	{
		lrc_list_insert_sorted(thiz->time_tags, time_tag, lrc_time_tag_compare);
	}

	return;
}

LrcList* lrc_tree_get_id_tags(LrcTree* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->id_tags : NULL;
}

LrcList* lrc_tree_get_time_tags(LrcTree* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->time_tags : NULL;
}

void     lrc_tree_set_user_data(LrcTree* thiz, void* user_data, LrcItemDestroyFunc user_data_destroy)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->user_data = user_data;
		thiz->user_data_destroy = user_data_destroy;
	}

	return;
}

void     lrc_tree_destroy(LrcTree* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->id_tags != NULL)
		{
			lrc_list_destroy(thiz->id_tags);
			thiz->id_tags = NULL;
		}

		if(thiz->time_tags != NULL)
		{
			lrc_list_destroy(thiz->time_tags);
			thiz->time_tags = NULL;
		}
		
		if(thiz->user_data_destroy != NULL)
		{
			thiz->user_data_destroy(thiz->user_data);
			thiz->user_data = NULL;
			thiz->user_data_destroy = NULL;
		}
		LRC_FREE(thiz);
	}

	return;
}

LRC_RESULT lrc_tree_save(LrcTree* thiz, LrcBuilder* builder)
{
	LrcIdTag* id_tag = NULL;
	LrcTimeTag* time_tag = NULL;
	LrcListIter iter = {0};

	LRC_ASSERT(thiz != NULL && builder != NULL);

	if(thiz != NULL && builder != NULL)
	{
		const char* key = NULL;
		const char* value = NULL;
		const char* lrc = NULL;
		
		iter = lrc_list_first(thiz->id_tags);
		while(!lrc_list_iter_is_null(&iter))
		{
			id_tag = (LrcIdTag*)lrc_list_iter_data(&iter);
			key = lrc_id_tag_get_key(id_tag);
			value = lrc_id_tag_get_value(id_tag);
			
			builder->on_id_tag(builder, key, strlen(key), value, strlen(value));
			
			iter = lrc_list_iter_next(&iter);			
		}

		iter = lrc_list_first(thiz->time_tags);
		while(!lrc_list_iter_is_null(&iter))
		{
			time_tag = (LrcTimeTag*)lrc_list_iter_data(&iter);
		
			lrc = lrc_time_tag_get_lrc(time_tag);
			builder->on_time_tag(builder, lrc_time_tag_get_start_time(time_tag));
			builder->on_lrc(builder, lrc, strlen(lrc));
			
			iter = lrc_list_iter_next(&iter);
		}
	}

	return LRC_RESULT_OK;
}

#ifdef LRC_TREE_TEST
#include <stdio.h>
#include "lrc_dumpbuilder.h"
#define N 10
int main(int argc, char* argv[])
{
	int i = 0;
	char key[100] = {"key"};
	char value[100] = {"value"};
	char lrc[100] = {"lrc"};
	
	LrcPool* id_tag_pool = lrc_pool_new(lrc_id_tag_size(), 10);
	LrcPool* time_tag_pool = lrc_pool_new(lrc_time_tag_size(), 100);
	LrcIdTag* id_tag = NULL;
	LrcTimeTag* time_tag = NULL;
	LrcBuilder* dumper = lrc_dump_builder_new(stdout);
	LrcTree* thiz = lrc_tree_new();
	
	for(i = 0; i < N; i++)
	{
		id_tag = lrc_id_tag_new(id_tag_pool, key, value);
		lrc_tree_add_id_tag(thiz, id_tag);
		time_tag = lrc_time_tag_new(time_tag_pool, i, lrc);
		lrc_tree_add_time_tag(thiz, time_tag);
	}
	
	lrc_tree_save(thiz, dumper);

	dumper->destroy(dumper);
	lrc_tree_destroy(thiz);
	lrc_pool_destroy(id_tag_pool);
	lrc_pool_destroy(time_tag_pool);
	
	return 0;
}
#endif

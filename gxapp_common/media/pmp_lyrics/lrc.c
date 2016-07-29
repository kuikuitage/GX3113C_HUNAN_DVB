/*
 * lrc.c
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
#include "lrc.h"
#include "lrc_tree.h"
#include "lrc_list.h"
#include "lrc_parser.h"
#include "lrc_defaultbuilder.h"

static Lrc* lrc_parse(LrcParser* parser)
{
	Lrc* thiz = NULL;
	
	if(parser != NULL)
	{
		LrcBuilder* builder = (LrcBuilder*)lrc_default_builder_new();
		lrc_parser_run(parser, builder);
		thiz = (Lrc*)lrc_default_builder_get_tree(builder);
		builder->destroy(builder);
		lrc_parser_destroy(parser);
	}

	return thiz;
}

Lrc* lrc_new_from_buffer(const char* buffer)
{
	return lrc_parse((LrcParser*)lrc_parser_new(buffer));
}

Lrc* lrc_new_from_file(const char* filename)
{
	return lrc_parse((LrcParser*)lrc_parser_new_from_file(filename));
}

LrcIdTagIter   lrc_get_id_tags(Lrc* thiz)
{
	LrcIdTagIter iter = {0};
	LrcTree* tree = (LrcTree*) thiz;

	LRC_ASSERT(tree != NULL);

	if(tree != NULL)
	{
		iter = lrc_list_first(lrc_tree_get_id_tags(tree));
	}

	return iter;
}

LrcIdTagIter   lrc_get_id_tag_get_by_key(Lrc* thiz, const char* key)
{
	LrcIdTagIter iter = {0};

	LRC_ASSERT(thiz != NULL && key != NULL);

	if(thiz != NULL && key != NULL)
	{
		iter = lrc_get_id_tags(thiz);
		while(!lrc_id_tag_iter_is_null(&iter))
		{
			if(strcmp(key, lrc_id_tag_iter_get_key(&iter)) == 0)
			{
				return iter;
			}
			iter = lrc_id_tag_iter_next(&iter);
		}
	}

	return iter;
}

LrcTimeTagIter lrc_get_time_tag_by_time(Lrc* thiz, size_t start_time)
{
	LrcTimeTagIter iter = {0};

	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		iter = lrc_get_time_tags(thiz);
		while(!lrc_time_tag_iter_is_null(&iter))
		{
			if(start_time < lrc_time_tag_iter_get_start_time(&iter))
			{
				return lrc_time_tag_iter_prev(&iter);;
			}
			
			if(!lrc_time_tag_iter_has_next(&iter) && (start_time > lrc_time_tag_iter_get_start_time(&iter)))
			{
				//last one.
				return iter;
			}
			else
			{
				iter = lrc_time_tag_iter_next(&iter);
			}
		}
	}

	return iter;
}

LrcTimeTagIter lrc_get_time_tags(Lrc* thiz)
{
	LrcTimeTagIter iter = {0};
	LrcTree* tree = (LrcTree*) thiz;

	LRC_ASSERT(tree != NULL);

	if(tree != NULL)
	{
		iter = lrc_list_first(lrc_tree_get_time_tags(tree));
	}

	return iter;
}

LrcIdTagIter lrc_id_tag_iter_prev(LrcIdTagIter* iter)
{
	return lrc_list_iter_prev(iter);
}

LrcIdTagIter lrc_id_tag_iter_next(LrcIdTagIter* iter)
{
	return lrc_list_iter_next(iter);
}

int          lrc_id_tag_iter_has_prev(LrcIdTagIter* iter)
{
	return lrc_list_iter_has_prev(iter);
}

int          lrc_id_tag_iter_has_next(LrcIdTagIter* iter)
{
	return lrc_list_iter_has_next(iter);
}

int          lrc_id_tag_iter_is_null(LrcIdTagIter* iter)
{
	return lrc_list_iter_is_null(iter);
}

const char*  lrc_id_tag_iter_get_key(LrcIdTagIter* iter)
{
	LrcIdTag* id_tag = (LrcIdTag*)lrc_list_iter_data(iter);

	return lrc_id_tag_get_key(id_tag);
}

const char*  lrc_id_tag_iter_get_value(LrcIdTagIter* iter)
{
	LrcIdTag* id_tag = (LrcIdTag*)lrc_list_iter_data(iter);

	return lrc_id_tag_get_value(id_tag);
}

LrcTimeTagIter lrc_time_tag_iter_prev(LrcTimeTagIter* iter)
{
	return lrc_list_iter_prev(iter);
}

LrcTimeTagIter lrc_time_tag_iter_next(LrcTimeTagIter* iter)
{
	return lrc_list_iter_next(iter);
}

int          lrc_time_tag_iter_has_prev(LrcTimeTagIter* iter)
{
	return lrc_list_iter_has_prev(iter);
}

int          lrc_time_tag_iter_has_next(LrcTimeTagIter* iter)
{
	return lrc_list_iter_has_next(iter);
}

int          lrc_time_tag_iter_is_null(LrcTimeTagIter* iter)
{
	return lrc_list_iter_is_null(iter);
}

const char*    lrc_time_tag_iter_get_lrc(LrcTimeTagIter* iter)
{
	LrcTimeTag* time_tag = (LrcTimeTag*)lrc_list_iter_data(iter);

	return lrc_time_tag_get_lrc(time_tag);
}

size_t         lrc_time_tag_iter_get_start_time(LrcTimeTagIter* iter)
{
	LrcTimeTag* time_tag = (LrcTimeTag*)lrc_list_iter_data(iter);

	return lrc_time_tag_get_start_time(time_tag);
}

size_t         lrc_time_tag_iter_get_pause_time(LrcTimeTagIter* iter)
{
	LrcTimeTag* time_tag = (LrcTimeTag*)lrc_list_iter_data(iter);

	return lrc_time_tag_get_pause_time(time_tag);
}

size_t         lrc_time_tag_iter_get_repeat_times(LrcTimeTagIter* iter)
{
	LrcTimeTag* time_tag = (LrcTimeTag*)lrc_list_iter_data(iter);

	return lrc_time_tag_get_repeat_times(time_tag);
}

void lrc_save(Lrc* thiz, const char* filename)
{
	FILE* fp = NULL;
	LrcBuilder* dumper = NULL;
	
	if(filename != NULL)
	{
		fp = fopen(filename, "w+");
	}
	
	dumper = lrc_dump_builder_new(fp);
	if(dumper != NULL)
	{
		lrc_tree_save(thiz, dumper);

		dumper->destroy(dumper);
	}

	if(fp != NULL)
	{
		fclose(fp);
	}

	return;
}

void lrc_destroy(Lrc* thiz)
{
	lrc_tree_destroy(thiz);

	return;
}


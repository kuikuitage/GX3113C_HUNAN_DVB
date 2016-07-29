/*
 * lrc_time_tag.c
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


#include "lrc_time_tag.h"

struct _LrcTimeTag
{
	LrcPool* pool;
	size_t start_time;
	size_t pause_time;
	size_t repeat_times;
	const char* lrc;
};

size_t lrc_time_tag_size(void)
{
	return sizeof(LrcTimeTag);
}

LrcTimeTag* lrc_time_tag_new(LrcPool* pool, size_t start_time, const char* lrc)
{
	LrcTimeTag* thiz = NULL;

	LRC_ASSERT(pool != NULL);

	if(pool != NULL)
	{
		thiz = (LrcTimeTag*)lrc_pool_alloc(pool);
		
		if(thiz != NULL)
		{
			thiz->pool = pool;
			thiz->start_time = start_time;
			thiz->lrc = lrc;
		}
	}

	return thiz;
}

void lrc_time_tag_set_start_time(LrcTimeTag* thiz, size_t start_time)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->start_time = start_time;
	}

	return;
}

void lrc_time_tag_set_lrc(LrcTimeTag* thiz, const char* lrc)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->lrc = lrc;
	}

	return;
}

void lrc_time_tag_set_pause_time(LrcTimeTag* thiz, size_t pause_time)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->pause_time = pause_time;
	}

	return;
}

void lrc_time_tag_set_repeat_times(LrcTimeTag* thiz, size_t repeat_times)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->repeat_times = repeat_times;
	}

	return;
}

size_t lrc_time_tag_get_start_time(LrcTimeTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->start_time : 0;
}

size_t lrc_time_tag_get_pause_time(LrcTimeTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->pause_time: 0;
}

size_t lrc_time_tag_get_repeat_times(LrcTimeTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->repeat_times : 0;
}

const char* lrc_time_tag_get_lrc(LrcTimeTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->lrc : NULL;
}

void lrc_time_tag_destroy(LrcTimeTag* thiz)
{
	if(thiz != NULL && thiz->pool != NULL)
	{
		lrc_pool_free(thiz->pool, thiz);
	}

	return;
}

int lrc_time_tag_compare(void* data, void* user_data)
{
	LrcTimeTag* tag1 = (LrcTimeTag*)data;
	LrcTimeTag* tag2 = (LrcTimeTag*)user_data;

	LRC_ASSERT(tag1 != NULL && tag2 != NULL);

	if(tag1 != NULL && tag2 != NULL)
	{
		return tag1->start_time - tag2->start_time;
	}

	return -1;
}

#ifdef LRC_TIME_TAG_TEST
int main(int argc, char* argv[])
{
	LrcPool* pool = lrc_pool_new(lrc_time_tag_size(), 100);
	LrcTimeTag* thiz = lrc_time_tag_new(pool, 1000, "first");

	LRC_ASSERT(lrc_time_tag_get_start_time(thiz) == 1000);
	LRC_ASSERT(strcmp(lrc_time_tag_get_lrc(thiz), "first") == 0);
	
	lrc_time_tag_set_start_time(thiz, 2000);
	lrc_time_tag_set_lrc(thiz, "second");
	lrc_time_tag_set_pause_time(thiz, 5);
	lrc_time_tag_set_repeat_times(thiz, 2);

	LRC_ASSERT(lrc_time_tag_get_start_time(thiz) == 2000);
	LRC_ASSERT(strcmp(lrc_time_tag_get_lrc(thiz), "second") == 0);
	
	LRC_ASSERT(lrc_time_tag_get_pause_time(thiz) == 5);
	LRC_ASSERT(lrc_time_tag_get_repeat_times(thiz) == 2);
	
	lrc_time_tag_destroy(thiz);
	lrc_pool_destroy(pool);
	return 0;
}
#endif

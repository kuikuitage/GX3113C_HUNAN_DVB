/*
 * lrc_id_tag.c
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

#include "lrc_id_tag.h"

struct _LrcIdTag
{
	const char* key;
	const char* value;
	LrcPool* pool;
};

size_t    lrc_id_tag_size(void)
{
	return sizeof(LrcIdTag);
}

LrcIdTag* lrc_id_tag_new(LrcPool* pool, const char* key, const char* value)
{
	LrcIdTag* thiz = NULL;
	
	LRC_ASSERT(pool != NULL);

	if(pool != NULL)
	{
		thiz = lrc_pool_alloc(pool);
		if(thiz != NULL)
		{
			thiz->pool = pool;
			thiz->key = key;
			thiz->value = value;
		}
	}
	
	return thiz;
}

void lrc_id_tag_set_key(LrcIdTag* thiz, const char* key)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->key = key;
	}

	return;
}

void lrc_id_tag_set_value(LrcIdTag* thiz, const char* value)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		thiz->value = value;
	}

	return;
}

const char* lrc_id_tag_get_key(LrcIdTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->key : NULL;
}

const char* lrc_id_tag_get_value(LrcIdTag* thiz)
{
	LRC_ASSERT(thiz != NULL);

	return thiz != NULL ? thiz->value : NULL;
}

void lrc_id_tag_destroy(LrcIdTag* thiz)
{
	if(thiz != NULL && thiz->pool != NULL)
	{
		lrc_pool_free(thiz->pool, thiz);
	}

	return;
}

#ifdef LRC_ID_TAG_TEST
int main(int argc, char* argv[])
{
	LrcPool* pool = lrc_pool_new(lrc_id_tag_size(), 10);
	
	LrcIdTag* thiz = lrc_id_tag_new(pool, "name", "jim");

	LRC_ASSERT(strcmp(lrc_id_tag_get_key(thiz), "name") == 0);
	LRC_ASSERT(strcmp(lrc_id_tag_get_value(thiz), "jim") == 0);

	lrc_id_tag_set_key(thiz, "age");
	lrc_id_tag_set_value(thiz, "28");

	LRC_ASSERT(strcmp(lrc_id_tag_get_key(thiz), "age") == 0);
	LRC_ASSERT(strcmp(lrc_id_tag_get_value(thiz), "28") == 0);
	
	lrc_id_tag_destroy(thiz);
	lrc_pool_destroy(pool);
	return 0;
}
#endif

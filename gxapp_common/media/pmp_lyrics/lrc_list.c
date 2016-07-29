/*
 * lrc_list.c
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
#include <stdio.h>
#include "lrc_pool.h"
#include "lrc_list.h"
#include "lrc_type.h"

struct _LrcListItem
{
	void* data;
	struct _LrcListItem* prev;
	struct _LrcListItem* next;
};

struct _LrcList
{
	LrcItemDestroyFunc destroy;
	LrcListItem*       first;
	LrcPool*           pool;
};

LrcListIter lrc_list_insert_sorted(LrcList* thiz, void* p, LrcItemCompareFunc compare);
LrcListIter lrc_list_insert(LrcListIter* iter, void* p, int after);
void        lrc_list_remove(LrcList* thiz, void* p, LrcItemCompareFunc compare);

LrcListIter lrc_list_first(LrcList* thiz);
LrcListIter lrc_list_last(LrcList* thiz);

LrcListIter lrc_list_iter_prev(LrcListIter* iter);
LrcListIter lrc_list_iter_next(LrcListIter* iter);
int         lrc_list_iter_has_prev(LrcListIter* iter);
int         lrc_list_iter_has_next(LrcListIter* iter);
int         lrc_list_iter_is_null(LrcListIter* iter);
void*       lrc_list_iter_data(LrcListIter* iter);

LrcList* lrc_list_new(LrcItemDestroyFunc destroy, size_t n_prealloc)
{
	LrcList* thiz = LRC_ALLOC(LrcList, 1);
	
	if(thiz != NULL)
	{
		thiz->destroy = destroy;
		thiz->pool = lrc_pool_new(sizeof(LrcListItem), n_prealloc);
	}

	return thiz;
}

LrcListIter lrc_list_insert_sorted(LrcList* thiz, void* p, LrcItemCompareFunc compare)
{
	LrcListIter iter = {0};
	
	LRC_ASSERT(thiz != NULL && compare != NULL);

	iter.list = thiz;
	iter.item = thiz->first;

	if(thiz->first == NULL)
	{
		lrc_list_insert(&iter, p, 0);
	}
	else
	{
		while(compare(iter.item->data, p) <= 0)
		{
			if(lrc_list_iter_has_next(&iter))
			{
				iter = lrc_list_iter_next(&iter);
			}
			else
			{
				break;
			}
		}
		
		lrc_list_insert(&iter, p, compare(iter.item->data, p) <= 0);
	}

	return iter;
}

LrcListIter lrc_list_insert(LrcListIter* iter, void* p, int after)
{
	LRC_ASSERT(iter != NULL && iter->list != NULL);

	if(iter->item == NULL)
	{
		iter->item = (LrcListItem*)lrc_pool_alloc(iter->list->pool);
		iter->item->data = p;
		iter->list->first = iter->item;
	}
	else
	{
		LrcListItem* item = (LrcListItem*)lrc_pool_alloc(iter->list->pool);
		item->data = p;

		if(after)
		{
			item->next = iter->item->next;
			if(item->next != NULL)
			{
				item->next->prev = item;
			}
			iter->item->next = item;
			item->prev = iter->item;
		}
		else
		{
			item->prev = iter->item->prev;
			if(item->prev != NULL)
			{
				item->prev->next = item;
			}
			iter->item->prev = item;
			item->next = iter->item;
		}

		iter->item = item;
		if(item->prev == NULL)
		{
			iter->list->first = item;
		}
	}

	return *iter;
}

void lrc_list_remove(LrcList* thiz, void* p, LrcItemCompareFunc compare)
{
	LrcListIter iter = {0};
	iter.list = thiz;
	iter.item = thiz->first;
	
	while(compare(iter.item->data, p) != 0)
	{
		if(lrc_list_iter_is_null(&iter))
		{
			break;
		}
		else
		{
			iter = lrc_list_iter_next(&iter);
		}
	}

	if(!lrc_list_iter_is_null(&iter))
	{
		if(iter.list->destroy != NULL)
		{
			iter.list->destroy(iter.item->data);
			iter.item->data = NULL;
		}

		if(iter.item->next != NULL)
		{
			iter.item->next->prev = iter.item->prev;
		}

		if(iter.item->prev != NULL)
		{
			iter.item->prev->next =iter.item->next;
		}

		iter.item->next = NULL;
		iter.item->prev = NULL;

		lrc_pool_free(iter.list->pool, iter.item);
	}

	return;
}

int         lrc_list_count(LrcList* thiz)
{
	int n = 0;
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		LrcListItem* item = thiz->first;
		while(item != NULL)
		{
			n++;
			item = item->next;
		}
	}

	return n;
}

void        lrc_list_reset(LrcList* thiz)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		LrcListItem* item = thiz->first;
		
		while(item != NULL)
		{
			thiz->first = item->next;
			
			if(thiz->destroy != NULL)
			{
				thiz->destroy(item->data);
			}
			
			lrc_pool_free(thiz->pool, item);
			item = thiz->first;
		}
	}

	return;
}

LrcListIter lrc_list_first(LrcList* thiz)
{
	LrcListIter iter = {0};
	
	LRC_ASSERT(thiz != NULL);
	
	iter.list = thiz;
	iter.item = thiz->first;

	return iter;
}

LrcListIter lrc_list_last(LrcList* thiz)
{
	LrcListIter iter = {0};
	
	LRC_ASSERT(thiz != NULL);
	
	iter.list = thiz;
	iter.item = thiz->first;

	while(lrc_list_iter_has_next(&iter))
	{
		iter = lrc_list_iter_next(&iter);
	}
	
	return iter;
}

LrcListIter lrc_list_iter_prev(LrcListIter* iter)
{
	LRC_ASSERT(iter != NULL);

	if(iter != NULL && iter->item != NULL)
	{
		iter->item = iter->item->prev;
	}

	return *iter;
}

LrcListIter lrc_list_iter_next(LrcListIter* iter)
{
	LRC_ASSERT(iter != NULL);

	if(iter != NULL && iter->item != NULL)
	{
		iter->item = iter->item->next;
	}

	return *iter;
}

int    lrc_list_iter_is_null(LrcListIter* iter)
{
	if(iter == NULL || iter->item == NULL || iter->list == NULL)
	{
		return !0;
	}
	else
	{
		return 0;
	}
}

int    lrc_list_iter_has_prev(LrcListIter* iter)
{
	LRC_ASSERT(iter != NULL);

	if(iter == NULL || iter->item == NULL || iter->item->prev == NULL)
	{
		return 0;
	}
	else
	{
		return !0;
	}
}

int    lrc_list_iter_has_next(LrcListIter* iter)
{
	LRC_ASSERT(iter != NULL);

	if(iter == NULL || iter->item == NULL || iter->item->next == NULL)
	{
		return 0;
	}
	else
	{
		return !0;
	}
}

void*    lrc_list_iter_data(LrcListIter* iter)
{
	LRC_ASSERT(iter != NULL);

	if(iter != NULL && iter->item != NULL)
	{
		return iter->item->data;
	}
	else
	{
		return NULL;
	}
}

void     lrc_list_destroy(LrcList* thiz)
{
	LRC_ASSERT(thiz != NULL);

	if(thiz != NULL)
	{
		lrc_list_reset(thiz);
		lrc_pool_destroy(thiz->pool);
		LRC_FREE(thiz);
	}

	return;
}

#ifdef LRC_LIST_TEST

#define N 100

static void dump_list(LrcList* thiz)
{
	LrcListIter iter = lrc_list_first(thiz);
	
	printf("%s\n", __func__);
	while(!lrc_list_iter_is_null(&iter))
	{
		printf("%p\n", lrc_list_iter_data(&iter));
		iter = lrc_list_iter_next(&iter);
	}

	return;
}

static void dump_r_list(LrcList* thiz)
{
	LrcListIter iter = lrc_list_last(thiz);

	printf("%s\n", __func__);
	while(!lrc_list_iter_is_null(&iter))
	{
		printf("%p\n", lrc_list_iter_data(&iter));
		iter = lrc_list_iter_prev(&iter);
	}

	return;
}

static int int_compare(void* data, void* user_data)
{
	return (int)data - (int)user_data;
}

static void int_destroy(void* data)
{
	printf("%s: %p\n", data);

	return;
}

static void test(LrcItemDestroyFunc destroy)
{
	int i = 0;
	LrcListIter iter = {0};
	LrcList* thiz = lrc_list_new(destroy, 100);

	iter = lrc_list_first(thiz);
	
	for(i = 0; i < N; i++)
	{
		lrc_list_insert(&iter, (void*)i, 0);
	}

	dump_list(thiz);
	dump_r_list(thiz);

	LRC_ASSERT(lrc_list_count(thiz) == N);
	lrc_list_reset(thiz);
	LRC_ASSERT(lrc_list_count(thiz) == 0);

	iter = lrc_list_first(thiz);
	for(i = 0; i < N; i++)
	{
		lrc_list_insert(&iter, (void*)i, 1);
	}

	dump_list(thiz);
	dump_r_list(thiz);

	LRC_ASSERT(lrc_list_count(thiz) == N);
	lrc_list_reset(thiz);
	LRC_ASSERT(lrc_list_count(thiz) == 0);
	
	iter = lrc_list_first(thiz);
	for(i = 0; i < N; i++)
	{
		lrc_list_insert_sorted(thiz, (void*)rand(), int_compare);
	}

	dump_list(thiz);
	dump_r_list(thiz);

	LRC_ASSERT(lrc_list_count(thiz) == N);
	lrc_list_reset(thiz);
	LRC_ASSERT(lrc_list_count(thiz) == 0);
	lrc_list_destroy(thiz);

	return;
}

int main(int argc, char* argv[])
{
	test(NULL);

	return 0;
}
#endif

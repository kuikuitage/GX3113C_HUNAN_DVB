/*
 * mark_map.c
 *
 *  Created on: 2016-6-21
 *      Author: Brant
 */

//#include "usifext.h"
#include "mark_map.h"
#include "mad_interface.h"
void* GSOS_Malloc(int size)
{
	return mad_malloc(size);
}

void GSOS_Free(void* p)
{
	if(NULL != p)
		mad_free(p);
	p = NULL;
}

BOOL init_mark_map(MarkMap *mark_map, U32 item_count)
{
	int array_size;

	if (item_count <= 0)
	{
		return FALSE;
	}
	array_size = (item_count + 7) / 8;
	mark_map->mark_array = (U8 *)GSOS_Malloc(array_size);
	if (mark_map->mark_array == NULL)
	{
		return FALSE;
	}
	mark_map->count = item_count;
	memset(mark_map->mark_array, 0, array_size);

	return TRUE;
}

void destroy_mark_map(MarkMap *mark_map)
{
	if (mark_map->mark_array)
	{
		GSOS_Free(mark_map->mark_array);
		mark_map->mark_array = NULL;
	}
	mark_map->count = 0;
}

BOOL is_empty_map(MarkMap *mark_map)
{
	if (!mark_map || mark_map->count <= 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL set_item_mark(MarkMap *mark_map, U32 item_index)
{
	int byte = item_index / 8;
	int bit = item_index % 8;

	if (!mark_map || !mark_map->mark_array || item_index >= mark_map->count)
	{
		return FALSE;
	}

	mark_map->mark_array[byte] = ((mark_map->mark_array[byte]) | (0x01 << bit));

	return TRUE;
}

BOOL clear_item_mark(MarkMap *mark_map, U32 item_index)
{
	int byte = item_index / 8;
	int bit = item_index % 8;

	if (!mark_map || !mark_map->mark_array || item_index >= mark_map->count)
	{
		return FALSE;
	}

	mark_map->mark_array[byte] = ((mark_map->mark_array[byte]) & (~(0x01 << bit)));

	return TRUE;
}

BOOL is_item_marked(MarkMap *mark_map, U32 item_index)
{
	int byte = item_index / 8;
	int bit = item_index % 8;

	if (!mark_map || !mark_map->mark_array || item_index >= mark_map->count)
	{
		return FALSE;
	}
	return (mark_map->mark_array[byte]) & (0x01 << bit);
}

U32 get_marked_item_count(MarkMap *mark_map)
{
	int i, mark_count;

	mark_count = 0;
	for (i = 0; i < mark_map->count; i++)
	{
		if (is_item_marked(mark_map, i))
		{
			mark_count++;
		}
	}
	return mark_count;
}

BOOL is_all_item_marked(MarkMap *mark_map)
{
	int i;

	for (i = 0; i < mark_map->count; i++)
	{
		if (!is_item_marked(mark_map, i))
		{
			return FALSE;
		}
	}
	return TRUE;
}

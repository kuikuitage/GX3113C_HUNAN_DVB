/*
 * mark_map.h
 *
 *  Created on: 2016-6-21
 *      Author: Brant
 */

#ifndef _MARK_MAP_H_
#define _MARK_MAP_H_
#include "maike_ads_porting.h"

typedef struct _mark_map_
{
	U32 count;
	U8 *mark_array;
} MarkMap;


BOOL init_mark_map(MarkMap *mark_map, U32 item_count);
void destroy_mark_map(MarkMap *mark_map);
BOOL is_empty_map(MarkMap *mark_map);
BOOL set_item_mark(MarkMap *mark_map, U32 item_index);
BOOL clear_item_mark(MarkMap *mark_map, U32 item_index);
BOOL is_item_marked(MarkMap *mark_map, U32 item_index);
BOOL is_all_item_marked(MarkMap *mark_map);
U32 get_marked_item_count(MarkMap *mark_map);


#endif /* _MARK_MAP_H_ */

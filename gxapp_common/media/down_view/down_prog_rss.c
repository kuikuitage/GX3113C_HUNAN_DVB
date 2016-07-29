#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "down_prog.h"
#include "down_prog_rss.h"
#include "tree.h"
#include "parser.h"




static down_item_program* item_create(GXxmlNodePtr GXxml_node)
{
	down_item_program* item = NULL;
	GXxmlNodePtr node = NULL;

	/*item create*/
	item = (down_item_program*)GxCore_Malloc(sizeof(down_item_program));
	if(NULL == item) return NULL;
	memset(item, 0, sizeof(down_item_program));
			
	/*item fill*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_TITLE))
		{
			item->title = GXxmlNodeGetContent(node);
		}
		else if(0 == GXxmlStrcmp(node->name, STR_LINK))
		{
			item->link = GXxmlNodeGetContent(node);
		}
		else if(0 == GXxmlStrcmp(node->name, STR_DESCRIPTION))
		{
			item->descriptor = GXxmlNodeGetContent(node);
		}			
		node = node->next;
	}

	return item;
}

static status_t item_release(down_item_program* item)
{
	if(NULL == item)
	{
		return GXCORE_ERROR;
	}
	
	DOWN_FREE(item->title);
	DOWN_FREE(item->link);
	DOWN_FREE(item->size);
	DOWN_FREE(item->pubdate);
	DOWN_FREE(item->descriptor);
	DOWN_FREE(item);
		
	return GXCORE_SUCCESS;
}

static status_t list_add_item(down_list_program* list, down_item_program* item)
{
	if(NULL == list || NULL == item)
	{
		return GXCORE_ERROR;
	}

	list->items[list->item_num] = item;
	list->item_num++;

	return GXCORE_SUCCESS;
}

static status_t list_create(down_list_program* list, GXxmlNodePtr GXxml_node)
{
	GXxmlNodePtr node = NULL;
	down_item_program* item = NULL;
	uint32_t item_num = 0;

	if(NULL == list)
	{
		return GXCORE_ERROR;
	}
	
	/*list get num*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_ITEM))
		{
			item_num++;
		}
		node = node->next;
	}

	/*list create*/
	list->items = (down_item_program**)GxCore_Malloc(item_num*sizeof(down_item_program*));
	if(NULL == list->items)
	{
		return GXCORE_ERROR;
	}

	/*list fill*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_ITEM))
		{
			item = item_create(node);
			list_add_item(list, item);
		}
		node = node->next;
	}

	printf("[DOWN] create program list, from class rss, item num:%d\n", list->item_num);

	return GXCORE_SUCCESS;
}

static status_t list_release(down_list_program* list)
{
	int32_t i = 0;

	if(NULL == list)
	{
		return GXCORE_ERROR;
	}

	printf("[DOWN] list release, item num:%d\n", list->item_num);
	
	for(i = 0; i < list->item_num; i++)
	{
		item_release(list->items[i]);
	}
	DOWN_FREE(list->items);

	memset(list, 0, sizeof(down_list_program));

	return GXCORE_SUCCESS;
}

status_t program_parser_rss(const char *filename)
{
	GXxmlDocPtr doc;

	if(NULL != object_list_program.items)
	{
		list_release(&object_list_program);	
	}
	memset(&object_list_program, 0, sizeof(object_list_program));

	
	doc = GXxmlParseFile(filename);
	if (NULL == doc) 
	{
		printf( "[XML] Document '%s' parse error.\n", filename);
		return GXCORE_ERROR;
	}

	GXxmlNodePtr root = doc->root;
	if (root == NULL) 
	{
		printf( "[XML] Document '%s' empty\n", filename);
		GXxmlFreeDoc(doc);
		return GXCORE_ERROR;
	}
	if(GXxmlStrcmp(root->name, STR_RSS))
	{
		GXxmlFreeDoc(doc);
		return GXCORE_ERROR;
	}
	if(NULL == root->childs)
	{
		GXxmlFreeDoc(doc);
		return GXCORE_ERROR;
	}
	if(GXxmlStrcmp(root->childs->name, STR_CHANNEL))
	{
		GXxmlFreeDoc(doc);
		return GXCORE_ERROR;
	}
	
	list_create(&object_list_program, root->childs);
	
	GXxmlFreeDoc(doc);
	
	return GXCORE_SUCCESS;
}




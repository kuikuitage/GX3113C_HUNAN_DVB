#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "down_sitecfg.h"
#include "tree.h"
#include "parser.h"


down_view object_down_sitecfg = {0};

status_t down_add_site(down_view* down, site_view* site)
{
	if(NULL == down || NULL == down->sites ||NULL == site)
	{
		return GXCORE_ERROR;
	}

	down->sites[down->site_num] = site;
	down->site_num++;

	return GXCORE_SUCCESS;
}

status_t site_add_channel(site_view* site, channel_view* channel)
{
	if(NULL == site || NULL == site->channels ||NULL == channel)
	{
		return GXCORE_ERROR;
	}

	site->channels[site->channel_num] = channel;
	site->channel_num++;

	return GXCORE_SUCCESS;
}

channel_view* channel_create(GXxmlNodePtr GXxml_node)
{
	channel_view* channel = NULL; 
	GXxmlNodePtr node = NULL;

	/*channel create*/
	channel = (channel_view*)GxCore_Malloc(sizeof(channel_view));
	if(NULL == channel) return NULL;
	memset(channel, 0, sizeof(channel_view));
			
	/*channel fill*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_TITLE))
		{
			channel->title = GXxmlNodeGetContent(node);
		}
		else if(0 == GXxmlStrcmp(node->name, STR_LINK))
		{
			channel->link = GXxmlNodeGetContent(node);
		}
		node = node->next;
	}
	printf("[DOWN] channel title: %s, link: %s\n", channel->title, channel->link);

	return channel;
}

status_t channel_release(channel_view* channel)
{
	if(NULL == channel)
	{
		return GXCORE_ERROR;
	}

	DOWN_FREE(channel->title);
	DOWN_FREE(channel->link);
	DOWN_FREE(channel);
	
	return GXCORE_SUCCESS;
}

site_view* site_create(GXxmlNodePtr GXxml_node)
{
	site_view* site = NULL;
	channel_view* channel = NULL;
	int32_t channel_num = 0;
	GXxmlNodePtr node = NULL;

	/*site create*/
	node = GXxml_node;
	site = (site_view*)GxCore_Malloc(sizeof(site_view));
	if(NULL == site) return NULL;
	memset(site, 0, sizeof(site_view));
	
	site->name = GXxmlGetProp(node, STR_NAME);
	site->logo = GXxmlGetProp(node, STR_LOGO);
	site->class = GXxmlGetProp(node, STR_CLASS);
	printf("[DOWN] site name: %s, logo: %s, class: %s\n", site->name, site->logo, site->class);


	/*channel get num*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_CHANNEL))
		{
			channel_num++;
		}
		node = node->next;
	}

	/*channel list create*/	
	site->channels = (channel_view**)GxCore_Malloc(channel_num*sizeof(channel_view*));
	if(NULL == site->channels)
	{
		return NULL;
	}

	/*channel list fill*/	
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_CHANNEL))
		{
			channel = channel_create(node);
			site_add_channel(site, channel);
		}
		node = node->next;
	}

	return site;
}

status_t site_release(site_view* site_view)
{
	int32_t i = 0;

	if(NULL == site_view)
	{
		return GXCORE_ERROR;
	}

	printf("[DOWN] site release, channel num:%d\n", site_view->channel_num);
	DOWN_FREE(site_view->name);
	DOWN_FREE(site_view->logo);
	DOWN_FREE(site_view->class);
	
	for(i = 0; i < site_view->channel_num; i++)
	{
		channel_release(site_view->channels[i]);
	}
	DOWN_FREE(site_view->channels);
	
	DOWN_FREE(site_view);

	return GXCORE_SUCCESS;
}

status_t down_create(down_view* down, GXxmlNodePtr GXxml_node)
{
	GXxmlNodePtr node = NULL;
	site_view* site = NULL;
	uint32_t site_num = 0;

	/*site get num*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_SITE))
		{
			site_num++;
		}
		node = node->next;
	}
	printf("[DOWN] down create, site num:%d\n", site_num);
	
	/*site list create*/
	down->sites = (site_view**)GxCore_Malloc(site_num*sizeof(site_view*));
	if(NULL == down->sites)
	{
		return GXCORE_ERROR;
	}

	/*site list fill*/
	node = GXxml_node->childs;
	while(node)
	{
		if(0 == GXxmlStrcmp(node->name, STR_SITE))
		{
			site = site_create(node);
			down_add_site(down, site);
		}
		node = node->next;
	}

	return GXCORE_SUCCESS;
}

status_t down_release(down_view* down_view)
{
	int32_t i = 0;

	if(NULL == down_view)
	{
		return GXCORE_ERROR;
	}

	printf("[DOWN] down release, site num:%d\n", down_view->site_num);
	
	for(i = 0; i < down_view->site_num; i++)
	{
		site_release(down_view->sites[i]);
	}
	DOWN_FREE(down_view->sites);

	memset(down_view, 0, sizeof(down_view));

	return GXCORE_SUCCESS;
}

status_t sitecfg_parser(const char* class, const char *filename)
{
	GXxmlDocPtr doc;

	if(NULL == filename) return GXCORE_ERROR;
	
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

	if(GXxmlStrcmp(root->name, STR_DOWNLOAD))
	{
		GXxmlFreeDoc(doc);
		return GXCORE_ERROR;
	}

	memset(&object_down_sitecfg, 0, sizeof(object_down_sitecfg));
	down_create(&object_down_sitecfg, root);
	
	GXxmlFreeDoc(doc);
	
	return GXCORE_SUCCESS;
}

status_t sitecfg_get_count(const char* property, void* data)
{
	site_view* browser_site = NULL;
	
	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}
	
	*(int32_t*)data = 0;
	
	if(0 == strcasecmp(property, "site"))
	{
		*(int32_t*)data = object_down_sitecfg.site_num;
	}
	else if(0 == strcasecmp(property, "channel"))
	{
		/*get visited site*/
		if(object_down_sitecfg.visit_site_no >= object_down_sitecfg.site_num)
		{
			return GXCORE_ERROR;
		}
		
		browser_site = object_down_sitecfg.sites[object_down_sitecfg.visit_site_no];
		if(NULL == browser_site)
		{
			return GXCORE_ERROR;
		}
		*(int32_t*)data = browser_site->channel_num;
	}

	return GXCORE_SUCCESS;
}

status_t sitecfg_get_item(const char* property, int32_t no, void* data)
{
	int32_t index = 0;
	site_view* browser_site = NULL;
	channel_view* browser_channel = NULL;
	
	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}

	index = no;
	
	if(0 == strcasecmp(property, "site"))
	{
		if(index >= object_down_sitecfg.site_num)
		{
			return GXCORE_ERROR;
		}
		
		browser_site = object_down_sitecfg.sites[index];
		if(NULL == browser_site)
		{
			return GXCORE_ERROR;
		}
		
		*(site_view**)data = browser_site;		
	}
	else if(0 == strcasecmp(property, "channel"))
	{
		/*get visited site*/
		if(object_down_sitecfg.visit_site_no >= object_down_sitecfg.site_num)
		{
			return GXCORE_ERROR;
		}
		browser_site = object_down_sitecfg.sites[object_down_sitecfg.visit_site_no];
		if(NULL == browser_site)
		{
			return GXCORE_ERROR;
		}
		
		/*get channel*/
		if(index >= browser_site->channel_num)
		{
			return GXCORE_ERROR;
		}
		browser_channel = browser_site->channels[index];
		if(NULL == browser_channel)
		{
			return GXCORE_ERROR;
		}
		*(channel_view**)data = browser_channel;
	}

	return GXCORE_SUCCESS;
}

status_t  sitecfg_get_property(const char* property, void* data)
{
	site_view* browser_site = NULL;

	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}
	
	*(int32_t*)data = 0;;
	if(0 == strcasecmp(property, "visit_site_no"))
	{
		*(int32_t*)data = object_down_sitecfg.visit_site_no;
	}
	else if(0 == strcasecmp(property, "visit_channel_no"))
	{
		browser_site = object_down_sitecfg.sites[object_down_sitecfg.visit_site_no];
		*(int32_t*)data = browser_site->visit_channel_no;
	}
	return GXCORE_SUCCESS;
}


status_t  sitecfg_set_property(const char* property, void* data)
{
	int32_t no = 0;
	site_view* browser_site = NULL;
	
	if(NULL == property || NULL == data)
	{
		return GXCORE_ERROR;
	}

	no = *(int32_t*)data;
	if(0 == strcasecmp(property, "visit_site_no"))
	{
		object_down_sitecfg.visit_site_no = no;
	}
	else if(0 == strcasecmp(property, "visit_channel_no"))
	{
		browser_site = object_down_sitecfg.sites[object_down_sitecfg.visit_site_no];
		browser_site->visit_channel_no = no;
	}
	
	return GXCORE_SUCCESS;
}


down_ops down_ops_sitecfg = 
{
	.owner =		"sitecfg",
	.parser =		sitecfg_parser,
	.get_count =	sitecfg_get_count,
	.get_item =		sitecfg_get_item,
	.get =			sitecfg_get_property,
	.set =			sitecfg_set_property,
};



/*
 * mad_util.c
 *
 *  Created on: 2016-6-23
 *      Author: Brant
 */

#include "mad_cfg.h"
#include "mad_interface.h"
#include "cJSON.h"
#include "mad_util.h"


/***********************************************************************************
 *                                     crc                                         *
 ***********************************************************************************/
#define MAD_GET_CRC32_POLY 0x04c11db7     /* AUTODIN II, Ethernet, & FDDI */

U32 mad_get_crc32(U8 *data, int length)
{
	U32 crc32_table[256];
	int i, j;
	unsigned int c;
	unsigned int crc;
	U8 *p;

	for (i = 0; i < 256; ++i)
	{
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ MAD_GET_CRC32_POLY : (c << 1);
		crc32_table[i] = c;
	}

	crc = 0xffffffff;
	for (p = data; length > 0; ++p, --length)
		crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];

	return crc;
}

BOOL mad_check_crc32(U8 *data, int length)
{
	U32 crc;

	if (data == NULL || length < 4)
	{
		return FALSE;
	}
	crc = data[length - 4] << 24 | data[length - 3] << 16 | data[length - 2] << 8 | data[length - 1];

	return mad_get_crc32(data, length - 4) == crc;
}

/***********************************************************************************
 *                                    json                                         *
 ***********************************************************************************/
cJSON* parse_mad_json_data(char *data, int len)
{
	char *json_string = NULL;
	cJSON *root = NULL;

	if ((json_string = mad_malloc(len + 1)) == NULL)
	{
		MAD_ERR("%s no enough memory!!\n", __FUNCTION__);
		return NULL;
	}
	memset(json_string, 0, len + 1);
	memcpy(json_string, data, len);
	root = cJSON_Parse(json_string);
	if (NULL == root)
	{
		MAD_ERR("parse fail, bad json data!!\n%s\n", json_string);
	}
	mad_free(json_string);

	return root;
}


static void print_json_node(cJSON *node)
{
	char *json_string;

	if (!node)
	{
		printf("json node is NULL!\n");
		return;
	}
	json_string = cJSON_Print(node);
	if (json_string)
	{
		printf("%s\n", json_string);
		mad_free(json_string);
	}
}

BOOL get_json_item_num_value(double *value, cJSON *obj, const char *string)
{
	cJSON *item;

	if (obj == NULL)
	{
		return FALSE;
	}
	item = cJSON_GetObjectItem(obj, string);
	if (!item)
	{
		MAD_WRN("no such item named %s!\n", string);
		print_json_node(obj);
		return FALSE;
	}
	if (item->type != cJSON_Number)
	{
		MAD_WRN("invalid data type of %s, its type is:%d\n", string, item->type);
		return FALSE;
	}
	*value = item->valuedouble;
	return TRUE;
}

BOOL get_json_item_string_value(char **value, cJSON *obj, const char *string)
{
	cJSON *item;

	if (obj == NULL)
	{
		return FALSE;
	}
	item = cJSON_GetObjectItem(obj, string);
	if (!item)
	{
		MAD_WRN("no such item named %s!\n", string);
		print_json_node(obj);
		return FALSE;
	}
	if (item->type != cJSON_String)
	{
		MAD_WRN("invalid data type of %s, its type is:%d\n", string, item->type);
		return FALSE;
	}
	*value = item->valuestring;
	return TRUE;
}

/*
 * lrc_parser.c
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
#include "lrc_parser.h"
#include "lrc_type.h"

#define skip_to_next_line(str) \
    do \
    { \
        while(*(str) != '\0' && *(str) != '\n') (str)++; \
        while(*(str) == '\n' || *(str) == '\r' ) str++; \
    }while(0);

#define skip_space(str) while((*(str) == ' ' || *(str) == '\t') && *(str) != '\0') str++;
#define skip_back_space(begin, str) while((*(str) == ' ' || *(str) == '\t') && ((str) > begin)) str--;

struct _LrcParser
{
	int   owner_buffer;
	const char* p;
	const char* buffer;
	LrcBuilder* builder;
};

LrcParser* lrc_parser_new(const char* buffer)
{
	LrcParser* thiz = NULL;
	
	if(buffer != NULL)
	{
		thiz = LRC_ALLOC(LrcParser, 1);
		thiz->buffer = (char*)buffer;
	}
	
	return thiz;
}

static char* read_file(const char* filename)
{
	FILE* fp = NULL;

    if(filename != NULL && (fp = fopen(filename, "rb")) != NULL)
    {
        char* buffer = NULL;
        size_t length = 0;

        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        buffer = (char*)LRC_MALLOC(length + 1);

        if(buffer != NULL)
        { 
            fread(buffer, length, 1, fp);
            buffer[length] = '\0';
        }
        fclose(fp);

		return buffer;
    }	
	else
	{
		return NULL;
	}
}

LrcParser* lrc_parser_new_from_file(const char* filename)
{
	LrcParser* thiz = NULL;
	char* buffer = read_file(filename);
	
	if(buffer != NULL)
	{
		thiz = lrc_parser_new(buffer);
		if(thiz != NULL)
		{
			thiz->owner_buffer = 1;
		}
	}

	return thiz;
}

static LRC_RESULT lrc_parser_parse_pair(LrcParser* thiz, const char** return_key, size_t* key_length, 
	const char** return_value, size_t* value_length)
{
	const char* p = thiz->p;
	const char* key = thiz->p;
	const char* value = NULL;
	
	while(*p != '\0' && *p != ':' && *p != ']') ++p;
	thiz->p = p;
	
	if(*p == ':')
	{
		--p;
	}
	else
	{
		return LRC_RESULT_FAIL;
	}
	
	skip_back_space(key, p);

	*return_key = key;
	*key_length =  p - key + 1;
	
	p = ++(thiz->p);
	skip_space(p);
	value = p;
	
	while(*p != '\0' && *p != ']') ++p;
	thiz->p = p;

	if(*p == ']')
	{
		++thiz->p;
		--p;
	}
	else
	{
		return LRC_RESULT_FAIL;
	}
	
	skip_back_space(value, p);

	if(p < value)
	{
		return LRC_RESULT_FAIL;
	}
	else
	{
		*return_value = value;
		*value_length = p - value + 1;
	}
	
	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_parser_parse_id_tag(LrcParser* thiz)
{
	const char* key = NULL;
	const char* value = NULL;
	size_t key_length = 0;
	size_t value_length = 0;

	if(lrc_parser_parse_pair(thiz, &key, &key_length, &value, &value_length) == LRC_RESULT_OK)
	{
		if(thiz->builder->on_id_tag != NULL)
		{
			thiz->builder->on_id_tag(thiz->builder, key, key_length, value, value_length);
		}
	}

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_parser_parse_time_tag(LrcParser* thiz)
{
	float seconds = 0;
	char number[32] = {0};
	size_t start_time = 0;
	const char* key = NULL;
	const char* value = NULL;
	size_t key_length = 0;
	size_t value_length = 0;

	if(lrc_parser_parse_pair(thiz, &key, &key_length, &value, &value_length) == LRC_RESULT_OK)
	{
		key_length = key_length < 20 ? key_length : 20;
		strncpy(number, key, key_length);
		start_time = atoi(number) * 60 * 100;

		value_length = value_length < 20 ? value_length : 20;
		strncpy(number, value, value_length);
		sscanf(number, "%f", &seconds);
		
		start_time += seconds * 100;

		if(thiz->builder->on_time_tag != NULL)
		{
			thiz->builder->on_time_tag(thiz->builder, start_time);
		}
	}

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_parser_parse_tag(LrcParser* thiz)
{
	const char* p = thiz->p;
	skip_space(p);

	if(*p >= '0' && *p <= '9')
	{
		lrc_parser_parse_time_tag(thiz);
	}
	else
	{
		lrc_parser_parse_id_tag(thiz);
	}

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_parser_parse_lrc(LrcParser* thiz)
{
	const char* lrc = thiz->p;
	const char* p = thiz->p;
	
	while(*p != '\0' && *p != '\n' && *p != '[') ++p;
	thiz->p = p;

	if(*p != '\0')
	{
		--p;
	}

	skip_back_space(lrc, p);

	if(lrc <= p && thiz->builder->on_lrc != NULL)
	{
		thiz->builder->on_lrc(thiz->builder, lrc, p - lrc + 1);
	}

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_parser_parse(LrcParser* thiz)
{
	LRC_RESULT ret = LRC_RESULT_OK;
	const char* p = thiz->p;

	if(thiz->builder->on_begin != NULL)
	{
		thiz->builder->on_begin(thiz->builder, p);
	}
	
	while(*p != '\0')
	{

		if(*p == '[')
		{
	            if(*(p+4) == '-')//Skip the line like this: [00:-3.00]XXX...
	            {
	                skip_to_next_line(thiz->p);
	            }
	            else
	            {
	                thiz->p = ++p;
	                skip_space(thiz->p);
	                lrc_parser_parse_tag(thiz);
	            }
		}
		else if(*p == '\n' || *p == '\r')
		{
			int i = 1;
			while((*(p-i) == ' ' || *(p-i) == '\t') && *(p-i) != '\0')++i;
			
			if(*(p-i) == ']') //for null lrc but only have time tag line #4218
			{
				thiz->p = p;
				lrc_parser_parse_lrc(thiz);
			}
			
			skip_to_next_line(thiz->p);
		}
		else if(*p == ' ' || *p == '\t')
		{
			skip_space(thiz->p);
		}
		else if(*p != ']')
		{
			thiz->p = p;
			lrc_parser_parse_lrc(thiz);
		}
		else
		{
			++thiz->p;
		}

		p = thiz->p;
	}
	
	if(thiz->builder->on_end != NULL)
	{
		thiz->builder->on_end(thiz->builder);
	}
	
	return ret;
}

LRC_RESULT lrc_parser_run(LrcParser* thiz, LrcBuilder* builder)
{
	LRC_RESULT ret = LRC_RESULT_FAIL;
	
	LRC_ASSERT(thiz != NULL && builder != NULL);

	if(thiz != NULL && builder != NULL)
	{
		thiz->builder = builder;
		thiz->p = thiz->buffer;

		lrc_parser_parse(thiz);
	}

	return ret;
}

void       lrc_parser_destroy(LrcParser* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->owner_buffer)
		{
			void* buffer = (void*)thiz->buffer;
			LRC_FREE(buffer);
		}
		
		LRC_FREE(thiz);
	}

	return;
}

#ifdef LRC_PARSER_TEST
const char* testcase[] =
{
	"",
	"[ar:]",
	"[]",
	"[:]",
	"[ar:lixianjing]",
	"[  ar:lixianjing]",
	"[ar  :lixianjing]",
	"[ar:   lixianjing]",
	"[ar:lixianjing    ]",
	"[ar:lixianjing]\n[0:10] first ",
	"[ar:lixianjing]\n[0:10] first [10:10] second",
	"[ar:lixianjing]\n[0:10] first \n[10:10] second",
	"[ar:lixianjing]\n[ 0 :  10  ][ 1:  10  ] first \n[10:10   ][20:00] second",
	NULL
};

#include "lrc_tree.h"

#include "lrc_defaultbuilder.h"

int main(int argc, char* argv[])
{
	int i = 0;
	LrcParser* thiz = NULL;
	LrcTree* tree = NULL;
	LrcBuilder* dumper = lrc_dump_builder_new(stdout);
	LrcBuilder* builder = lrc_default_builder_new();

	char* buffer = read_file(argv[1]);
	
	if(buffer != NULL)
	{
		thiz = lrc_parser_new(buffer);
		
		lrc_parser_run(thiz, builder);
		tree = lrc_default_builder_get_tree(builder);
		lrc_tree_save(tree, dumper);
		
		lrc_parser_destroy(thiz);
		LRC_FREE(buffer);
	}
	else
	{
		while(testcase[i] != NULL)
		{
			thiz = lrc_parser_new(testcase[i]);
			
			lrc_parser_run(thiz, builder);
			tree = lrc_default_builder_get_tree(builder);
			lrc_tree_save(tree, dumper);
			
			lrc_parser_destroy(thiz);

			printf("===============================\n");
			i++;
		}
	}


	builder->destroy(builder);
	dumper->destroy(dumper);

	return 0;
}
#endif

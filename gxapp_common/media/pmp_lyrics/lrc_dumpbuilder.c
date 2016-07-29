/*
 * lrc_dumpbuilder.c
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
#include <stdio.h>
#include "lrc_dumpbuilder.h"
#include "lrc_type.h"

struct _LrcDumpBuilder
{
	FILE* fp;
};
typedef struct _LrcDumpBuilder LrcDumpBuilder;

static LRC_RESULT lrc_dump_builder_on_begin(LrcBuilder* thiz, const char* buffer)
{
	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_dump_builder_on_id_tag(LrcBuilder* thiz, const char* key, size_t key_length, 
					const char* value, size_t value_length)
{
	LrcDumpBuilder* data = (LrcDumpBuilder*)thiz->priv;

	fprintf(data->fp, "[");
	fwrite(key, key_length, 1, data->fp);
	fprintf(data->fp, ":");
	fwrite(value, value_length, 1, data->fp);
	fprintf(data->fp, "]\n");

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_dump_builder_on_time_tag(LrcBuilder* thiz, size_t start_time)
{
	int min = start_time / 6000;
	int sec = start_time % 6000;
	int per_sec = sec % 100;
	sec = sec / 100;
	
	LrcDumpBuilder* data = (LrcDumpBuilder*)thiz->priv;
	
	fprintf(data->fp, "[%d:%02d.%02d]", min, sec, per_sec);
	
	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_dump_builder_on_lrc(LrcBuilder* thiz, const char* lrc, size_t lrc_length)
{
	LrcDumpBuilder* data = (LrcDumpBuilder*)thiz->priv;
	fwrite(lrc, lrc_length, 1, data->fp);
	fprintf(data->fp, "\n");

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_dump_builder_on_end(LrcBuilder* thiz)
{
	LrcDumpBuilder* data = (LrcDumpBuilder*)thiz->priv;
	
	fflush(data->fp);

	return LRC_RESULT_OK;
}

static LRC_RESULT lrc_dump_builder_destroy(LrcBuilder* thiz)
{
	LRC_FREE(thiz);

	return LRC_RESULT_OK;
}

LrcBuilder* lrc_dump_builder_new(FILE* fp)
{
	LrcDumpBuilder* data =  NULL;
	LrcBuilder* thiz = (LrcBuilder*)GxCore_Calloc(sizeof(LrcBuilder) + sizeof(LrcDumpBuilder), 1);

	if(thiz != NULL)
	{
		thiz->on_begin     = lrc_dump_builder_on_begin;
		thiz->on_id_tag    = lrc_dump_builder_on_id_tag;
		thiz->on_time_tag  = lrc_dump_builder_on_time_tag;
		thiz->on_lrc       = lrc_dump_builder_on_lrc;
		thiz->on_end       = lrc_dump_builder_on_end;
		thiz->destroy      = lrc_dump_builder_destroy;
		data = (LrcDumpBuilder*)thiz->priv;
		data->fp = fp != NULL ? fp : stdout;
	}

	return thiz;
}


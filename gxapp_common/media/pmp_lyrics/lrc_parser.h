/*
 * lrc_parser.h
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

#ifndef LRC_PARSER_H
#define LRC_PARSER_H

#include "lrc_dumpbuilder.h"

struct _LrcParser;
typedef struct _LrcParser LrcParser;

LrcParser* lrc_parser_new(const char* buffer);
LrcParser* lrc_parser_new_from_file(const char* filename);
LRC_RESULT lrc_parser_run(LrcParser* thiz, LrcBuilder* builder);
void       lrc_parser_destroy(LrcParser* thiz);

#endif/*LRC_PARSER_H*/

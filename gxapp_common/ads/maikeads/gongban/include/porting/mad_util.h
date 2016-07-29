/*
 * mad_util.h
 *
 *  Created on: 2016-6-23
 *      Author: Brant
 */

#ifndef _M_AD_UTIL_H_
#define _M_AD_UTIL_H_
#include "maike_ads_type.h"
#include "cJSON.h"

#define M_AD_MAKE_USHORT(p)		((p[0] << 8) | p[1])
#define M_AD_MAKE_ULONG(p)		((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]))
#define M_AD_MAKE_TAG(a,b,c,d)	((a << 24) | (b << 16) | (c << 8) | (d))


U32 mad_get_crc32(U8 *data, int length);
BOOL mad_check_crc32(U8 *data, int length);

cJSON* parse_mad_json_data(char *data, int len);
BOOL get_json_item_num_value(double *value, cJSON *obj, const char *string);
BOOL get_json_item_string_value(char **value, cJSON *obj, const char *string);


#endif /* _M_AD_UTIL_H_ */

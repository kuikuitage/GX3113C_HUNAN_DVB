/*
 * mad.h
 *
 *  Created on: 2015-4-2
 *      Author: Brant
 */

#ifndef _M_AD_H_
#define _M_AD_H_

#include "mad_cfg.h"
#include "mad_util.h"
#include "maike_ads_type.h"

/***********************************************************************************
 *                                 m-ad place id                                   *
 ***********************************************************************************/
#define M_AD_PLACE_ID_BOOT_AV		((1<<24)+1)
#define M_AD_PLACE_ID_BOOT_PIC		((1<<24)+2)
#define M_AD_PLACE_ID_SUBTITLE		(3)
#define M_AD_PLACE_ID_CORNER		((1<<24)+4)
#define M_AD_PLACE_ID_PROG_BAR		((1<<24)+5)
#define M_AD_PLACE_ID_VOL_BAR		((1<<24)+6)
#define M_AD_PLACE_ID_MAIN_MUNU		((1<<24)+7)
#define M_AD_PLACE_ID_PROG_LIST		((1<<24)+8)
#define M_AD_PLACE_ID_EPG			((1<<24)+11)


/***********************************************************************************
 *                                    typedef                                      *
 ***********************************************************************************/
typedef enum
{
	MAD_TYPE_BOOT = 1,	/* show ad when stb bootup */
	MAD_TYPE_APP,		/* show ad when user waiting stb do something */
	MAD_TYPE_SUBTITLE,	/* show ad as a subtitle */
	MAD_TYPE_CORNER,	/* show ad at the corner of screen */
	MAD_TYPE_NUM
} MadType;

typedef enum
{
	MAD_DATA_TYPE_INVALID	= 0,

	MAD_DATA_TYPE_TEXT		= M_AD_MAKE_TAG('T', 'E', 'X', 'T'),
	MAD_DATA_TYPE_BMP		= M_AD_MAKE_TAG('B', 'M', 'P', ' '),
	MAD_DATA_TYPE_JPG		= M_AD_MAKE_TAG('J', 'P', 'E', 'G'),
	MAD_DATA_TYPE_IFRAME	= M_AD_MAKE_TAG('I', 'F', 'R', 'M'),
	MAD_DATA_TYPE_GIF		= M_AD_MAKE_TAG('G', 'I', 'F', ' '),

	MAD_DATA_TYPE_DESC		= M_AD_MAKE_TAG('D', 'E', 'S', 'C'),
	MAD_DATA_TYPE_CONTROL	= M_AD_MAKE_TAG('C', 'T', 'R', 'L'),
} MadDataType;


typedef struct _mad_place_info_
{
	U32 ad_place_id;
	MadType ad_place_type;
	int limit_size;
	int width;
	int height;
	BOOL save_to_flash;
} MadPlaceInfo;


typedef struct _mad_data_
{
	U32		data_type;
	U32		data_length;
	U8*		data_addr;
} MadData;

typedef struct _mad_place_
{
	BOOL in_memory;			/* memory variable,not save to flash */
//	U32 place_id;
	U32 version;
//	U32 type;
	MadData control_data;
	U32 data_item_num;
	MadData *data_items;
} MadPlace;


/***********************************************************************************
 *                                  internal use                                   *
 ***********************************************************************************/
typedef struct _mad_data_info_
{
	U32 tag;
	U32 offset;
	U32 length;
	U32 crc32;
} MadDataInfo;

typedef struct _mad_data_string_type_map_
{
	char *string;
	int type;
} MadDataStringTyptMap;

typedef struct _mad_place_basic_info_
{
	int x;
	int y;
	int version;
	int data_item_num;
	int interval;
} MadPlaceBasicInfo;


enum
{
	SHOW_TOP,
	SHOW_BOTTOM,
	ON_TOP_AND_BOTTOM,
} MadOsdType;
/***********************************************************************************
 *                                   interface                                     *
 ***********************************************************************************/
int mad_init(void);

int get_mad_place_id(int place_index);
int get_mad_place_index_by_id(int place_id);

U32 get_mad_place_limit_size(int place_id);
U32 get_mad_place_content_version(int place_id);
int get_mad_place_content_num(int place_id);
int get_mad_place_interval(int place_id);

BOOL handle_mad_place_data(U32 place_id, U32 version, U8 *data, U32 length);
BOOL save_mad_place_data(U32 place_id, BOOL delete_from_mem);

BOOL show_mad(int x, int y, U32 place_id, U32 content_index);
BOOL clear_mad(U32 place_id);


/* for test */
#if ENABLE_TEST_M_AD
void mad_test_change_boot_ad(void);
void mad_test_destroy_boot_ad(void);
#endif

#endif /* _M_AD_H_ */

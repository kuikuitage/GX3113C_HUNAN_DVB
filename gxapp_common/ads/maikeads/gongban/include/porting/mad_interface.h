/*
 * mad_interface.h
 *
 *  Created on: 2015-4-8
 *      Author: Brant
 */

#ifndef _M_AD_INTERFACE_H_
#define _M_AD_INTERFACE_H_

#include "maike_ads_type.h"

/***********************************************************************************
 *                                      os                                         *
 ***********************************************************************************/
#define MAD_INVALID_MUTEX		(-1)
#define MAD_INVALID_TASK		(-1)
#define MAD_MUTEX_WAIT_FOREVER  ((unsigned int)-1)

typedef void (*MadTaskEntry)(unsigned short, unsigned short);
typedef signed int MadMutex;


void mad_memcpy(void *dst, void *src, U32 length);
void mad_memset(void *addr, U8 value, U32 length);
int mad_memcmp(void *addr1, void *addr2, U32 length);
void *mad_malloc(unsigned int size);
void mad_free(void *addr);

int mad_create_task(MadTaskEntry entry, int argc, void *argv, U32 stack_size, void *stack_base, U8 priority, const char *name);
void mad_task_sleep(U32 ms);

int mad_create_mutex(int *handle, const char *mutex_name);
int mad_delete_mutex(int handle);
int mad_mutex_lock(int handle, unsigned int wait_ms);
int mad_mutex_unlock(int handle);


/***********************************************************************************
 *                                    device                                       *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
char* mad_device_language(void);
#endif
int mad_device_pid(void);
int mad_device_cid(void);
int mad_device_mid(void);
BOOL mad_device_sn(char *sn);
#if ENABLE_M_AD_NET
BOOL mad_device_time(char *time);
U32 mad_device_memory_size(void);
#endif


/***********************************************************************************
 *                                     flash                                       *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
U32 mad_flash_total_mad_place_size(void)
#endif
BOOL mad_flash_save_mad_place_data(int place_index, U8 *data, U32 length);
BOOL mad_flash_read_mad_place_data(int place_index, U8 **data, U32 *length);


/***********************************************************************************
 *                                    network                                      *
 ***********************************************************************************/
#if ENABLE_M_AD_NET
BOOL mad_interface_http_download(char *url, U8 **buffer, U32 *data_length);
char* mad_interface_base64_encode_string(char *src, U32 src_length);
char* mad_interface_base64_decode_string(char *src, U32 src_length);
#endif


/***********************************************************************************
 *                                show/clear m-ad                                  *
 ***********************************************************************************/
BOOL mad_show_text_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length);
BOOL mad_show_bitmap_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length);
BOOL mad_show_jpeg_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length);
BOOL mad_show_iframe_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length);
BOOL mad_show_gif_data(int place_id, int x, int y, int w, int h, U8 *data, U32 length);

BOOL mad_clear_region(int place_id, int x, int y, int w, int h);

#endif /* _M_AD_INTERFACE_H_ */

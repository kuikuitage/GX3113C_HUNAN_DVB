#ifndef __PMP_TAGS_H__
#define __PMP_TAGS_H__

#include "gxcore.h"
#include "module/config/gxconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TAGS_NUM 20
#define MAX_URL_LENGTH 256

#define CUR_TAG_NO					"CUR_TAG_NUM"



status_t pmp_init_tags(void);
int32_t pmp_load_tag(const char* url);
status_t pmp_save_tag(const char* url,int32_t time);







#ifdef __cplusplus
}
#endif

#endif /* __PMP_TAGS_H__ */


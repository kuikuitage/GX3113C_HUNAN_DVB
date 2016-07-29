#ifndef __APP_MEDIA_POPMSG_H__
#define __APP_MEDIA_POPMSG_H__

//#include "app.h"
#include <gxtype.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	MEDIA_POPMSG_RET_NONE,
	MEDIA_POPMSG_RET_OK,
	MEDIA_POPMSG_RET_YES,
	MEDIA_POPMSG_RET_NO
}media_popmsg_ret;

typedef enum
{
	MEDIA_POPMSG_TYPE_YESNO,
	MEDIA_POPMSG_TYPE_OK
}media_popmsg_type;

typedef int (*MediaPopMsgExitCb)(media_popmsg_ret);

media_popmsg_ret media_popmsg(uint32_t x, uint32_t y, const char* context, media_popmsg_type type);
media_popmsg_ret media_popmsg_timeout(const char *info_str, time_t sec, media_popmsg_ret default_ret, bool show_time, 
MediaPopMsgExitCb exit_cb);


#ifdef __cplusplus
}
#endif

#endif /* __APP_MEDIA_POPMSG_H__ */


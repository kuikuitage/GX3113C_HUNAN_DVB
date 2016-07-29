#ifndef __MEDIA_INFO_H__
#define __MEDIA_INFO_H__

//#include "gxtype.h"
#include "gxcore.h"
//#include "gui_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_INFO_LINE (6)

typedef struct
{
	char *subt;
	char *info;
}LineInfo;

typedef struct media_info_struct MediaInfo;
struct media_info_struct
{
	LineInfo line[6];
    void (*change_cb)(MediaInfo*);
    void (*destroy_cb)(void);
};

status_t media_info_create(MediaInfo *info);


#ifdef __cplusplus
}
#endif

#endif /* __PLAY_PIC_H__ */


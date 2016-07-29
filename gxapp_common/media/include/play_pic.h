#ifndef __PLAY_PIC_H__
#define __PLAY_PIC_H__

#include "gxtype.h"
#include "gxcore.h"
#include "gui_core.h"
#include "file_view.h"
#include "gdi_play.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	PLAY_PIC_CTROL_PREVIOUS,
	PLAY_PIC_CTROL_PLAY,
	PLAY_PIC_CTROL_PAUSE,
	PLAY_PIC_CTROL_NEXT,
	PLAY_PIC_CTROL_STOP,
	PLAY_PIC_CTROL_ROTATE,
	PLAY_PIC_CTROL_ZOOM
}play_pic_ctrol_state;

typedef enum
{
	PLAY_PIC_ROTATE_0,
	PLAY_PIC_ROTATE_90,
	PLAY_PIC_ROTATE_180,
	PLAY_PIC_ROTATE_270
}play_pic_rotate_state;

typedef enum
{
	PLAY_PIC_ZOOM_X1,
	PLAY_PIC_ZOOM_X2,
	PLAY_PIC_ZOOM_X4,
	PLAY_PIC_ZOOM_X8,
	PLAY_PIC_ZOOM_X16
}play_pic_zoom_state;


typedef struct
{
	char* path;
	char* name;

	Image_Exif  exif;
}play_pic_info;

typedef enum
{
	PIC_PLAY_START,
	PIC_PLAY_RUNNING,
	PIC_PLAY_ERROR,
	PIC_PLAY_END,
}PicPlayStatus;

typedef enum
{
	PIC_BOX_BUTTON_PREVIOUS,
	PIC_BOX_BUTTON_PLAY_PAUSE,
	PIC_BOX_BUTTON_NEXT,
	PIC_BOX_BUTTON_STOP,
	PIC_BOX_BUTTON_SET,
	PIC_BOX_BUTTON_INFO,
	PIC_BOX_BUTTON_ROTATE, 
	PIC_BOX_BUTTON_ZOOM,		// TODO: later 
	PIC_BOX_BUTTON_BGM,
	PIC_BOX_BUTTON_PAUSE,
	PIC_BOX_BUTTON_PLAY,
}pic_box_button;

status_t play_pic(int file_no);
status_t play_pic_ctrol(play_pic_ctrol_state ctrol);
status_t play_pic_rotate(play_pic_rotate_state rotate);
status_t play_pic_zoom(play_pic_zoom_state zoom);
status_t play_pic_get_info(play_pic_info *info);
play_pic_ctrol_state play_pic_get_state(void);

int pic_status_report(PicPlayStatus player_status);
status_t pic_box_ctrol(pic_box_button button);


#ifdef __cplusplus
}
#endif

#endif /* __PLAY_PIC_H__ */


#ifndef __APP_OSD_MSG_H__
#define __APP_OSD_MSG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <gxtype.h>


typedef enum osd_msg_no_e
{
	OSD_MSG_NO_SIGNAL = 0,
	OSD_MSG_NETWORK_ERR,
	OSD_MSG_MRS_UPDATED,
	OSD_MSG_CHANNEL_UPDATED,
	OSD_MSG_NO_CHANNEL,
	OSD_MSG_NO_CHANNEL_100,	
	OSD_MSG_CHANNEL_LOCKED,
	OSD_MSG_INVALID_EMM,
	OSD_MSG_NO_CARD,
	OSD_MSG_OTHER_CARD,
	OSD_MSG_NOT_PAIRED_CARD,
	OSD_MSG_SCRAMBLED_CHANNEL,
	OSD_MSG_MAX
}osd_msg_no_t;

void app_osd_msg(void);
void app_osd_msg_clear(void);

#ifdef __cplusplus
}
#endif
#endif /*__APP_OSD_MSG_H__*/


#include "osd_msg.h"
#include "app_osd_msg.h"
#include "gxapp_sys_config.h"

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
#include "app_abv_cas_api_osd_msg.h"
#include "app_abv_cas_api_finger.h"
#endif
#endif
static osd_msg_register_t osd_msg_no_signal = 
{
	OSD_MSG_NO_SIGNAL, 
	"No Signal",
	"001 No Signal, Please Contact Customer Care..."
};

static osd_msg_register_t osd_msg_network_error = 
{
	OSD_MSG_NETWORK_ERR, 
	"Network Error",
	"002 Network Error, Please Contact Customer Care..."
};

static osd_msg_register_t osd_msg_mrs_updated = 
{
	OSD_MSG_MRS_UPDATED, 
	NULL,
	"003 New software are available -Press YES to Search or NO for next boot"
};

static osd_msg_register_t osd_msg_channel_updated = 
{
	OSD_MSG_CHANNEL_UPDATED, 
	"Please Wait For Update",
	"004 Dear Customer,Please Wait For Channel Update"
};

static osd_msg_register_t osd_msg_no_channel = 
{
	OSD_MSG_NO_CHANNEL, 
	"No Channel",
	"005 No Channel, Please Contact Customer Care..."
};

static osd_msg_register_t osd_msg_no_channel_100 = 
{
	OSD_MSG_NO_CHANNEL_100, 
	"NO Home Channel",
	"006 No Home Channel, Please Contact Customer Care..."
};

static osd_msg_register_t osd_msg_channel_locked = 
{
	OSD_MSG_CHANNEL_LOCKED, 
	"Channel Locked",
	"007 Channel Locked,Please Input Password"
};

static osd_msg_register_t osd_msg_invalid_emm = 
{
	OSD_MSG_INVALID_EMM, 
	"Invalid EMM",
	"008 Invalid EMM, Please Contact Customer Care..."
};

static osd_msg_register_t osd_msg_no_card = 
{
	OSD_MSG_NO_CARD, 
	"No Card",
	"009 No Card, Please Insert Smart Card"
};

static osd_msg_register_t osd_msg_other_card = 
{
	OSD_MSG_OTHER_CARD, 
	"Card Error",
	"010 Card Error, Please Insert Smart Card"
};

static osd_msg_register_t osd_msg_not_paired_card = 
{
	OSD_MSG_NOT_PAIRED_CARD, 
	"Not Paired Card",
	"011 Not Paired Card, Please Insert Paired Card"
};

static osd_msg_register_t osd_msg_scrambled_channel = 
{
	OSD_MSG_SCRAMBLED_CHANNEL, 
	"Scrambled Channel",
	"012 Scrambled Channel"
};

void app_osd_msg(void)
{
	osd_msg_init();
	
	osd_msg_register(&osd_msg_no_signal);
	osd_msg_register(&osd_msg_network_error);
	osd_msg_register(&osd_msg_mrs_updated);
	osd_msg_register(&osd_msg_no_channel);
	osd_msg_register(&osd_msg_no_channel_100);
	osd_msg_register(&osd_msg_channel_updated);
	osd_msg_register(&osd_msg_channel_locked);
	osd_msg_register(&osd_msg_invalid_emm);
	osd_msg_register(&osd_msg_no_card);
	osd_msg_register(&osd_msg_other_card);
	osd_msg_register(&osd_msg_not_paired_card);
	osd_msg_register(&osd_msg_scrambled_channel);
	return;
}

void app_osd_msg_clear(void)
{
#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_ABV_CAS53_FLAG
	app_abv_cas_osd_msg_clear();
	app_abv_cas53_clear_finger_other_way(0);
#endif
#endif
}



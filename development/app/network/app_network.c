#include "app.h"
#include "gui_core.h"

#define ETHERNET_DEV 		 			"/tmp/app/ethernet_dev"
#define ETHERNET_IP 		 			"/tmp/app/ethernet_ip"
#define ETHERNET_STATUS 		 		"/tmp/app/ethernet_status"

#define IMG_OPT_HALF_FOCUS_L			"s_bar_half_focus_l.bmp"
#define IMG_OPT_HALF_FOCUS_R			"s_bar_half_focus_r.bmp"
#define IMG_OPT_HALF_UNFOCUS 		"s_bar_half_unfocus.bmp"

typedef char ubyte32[32];

static int system_result;
static char * pf_result;
bool static s_ethernet_monitor_on = true;
static char s_cur_mac[32];
static uint32_t s_dev = 0;
static uint8_t s_sel = 0;
static uint32_t s_dev_total = 0;
static ubyte32 *s_network_dev = NULL;
static char *s_dev_content = NULL;
static uint32_t s_mode = 0;
static char *s_mode_content = "[Static IP,DHCP]";
static char *s_network_opt[6] = {
	"cmb_network_opt1",
	"cmb_network_opt2",
	"edit_network_opt1",
	"edit_network_opt2",
	"edit_network_opt3",
	"edit_network_opt4"
};	
static char *s_network_text[6] = {
	"text_network_item1",
	"text_network_item2",
	"text_network_item3",
	"text_network_item4",
	"text_network_item5",
	"text_network_item6",
};	
static char *s_network_img[6] = 
{
	"img_network_item_choice1",
	"img_network_item_choice2",
	"img_network_item_choice3",
	"img_network_item_choice4",
	"img_network_item_choice5",
	"img_network_item_choice6",
};
static char *s_network_title_str[6] = {
    "Device",
    "Mode",
    "IP Address",
    "Subnet Mask",
    "Gate Way",
    "DNS",
};

static event_list* s_network_status_timer = NULL;
static bool s_network_setting = false;
static uint8_t s_network_check_time = 0;

void app_network_mode_change()
{
	uint8_t i = 0;
	
	GUI_SetProperty(s_network_opt[1], "select", &s_mode);

	if(s_mode == 1)
	{
		for(i = 0; i < 4; i++)
		{
			GUI_SetProperty(s_network_opt[i+2], "state", "disable");
			GUI_SetProperty(s_network_text[i+2], "state", "disable");
		}
	}
	else
	{
		for(i = 0; i < 4; i++)
		{
			GUI_SetProperty(s_network_opt[i+2], "state", "enable");
			GUI_SetProperty(s_network_text[i+2], "state", "enable");
		}
	}
}

void app_network_ip_simplify(char *buf, char *ip)
{
	char *p;
	char tmp[16];
	int num[4] = {0, 0, 0, 0};

	strcpy(tmp, ip);
	num[0] = atoi(tmp);	
	
	p = strchr(tmp, '.');	
	p++;
	num[1] = atoi(p);
	
	p = strchr(p, '.');
	p++;
	num[2] = atoi(p);

	p = strchr(p, '.');
	p++;
	num[3] = atoi(p);
	
	sprintf(buf, "%d.%d.%d.%d", num[0], num[1], num[2], num[3]);
}

void app_network_ip_completion(char *ip)
{
	char *p;
	char buf[15];
	int num[4] = {0, 0, 0, 0};

	strcpy(buf, ip);

	num[0] = atoi(buf);	
	
	p = strchr(buf, '.');	
	if(p== NULL)
		goto END;
	p++;
	num[1] = atoi(p);
	
	p = strchr(p, '.');
	if(p== NULL)
		goto END;
	p++;
	num[2] = atoi(p);

	p = strchr(p, '.');
	if(p== NULL)
		goto END;
	p++;
	num[3] = atoi(p);
	
END:
	memset(ip, 0, 16);
	sprintf(ip, "%03d.%03d.%03d.%03d", num[0], num[1], num[2], num[3]);
	
}

void app_network_ip_update(void)
{
	uint8_t i;
	int len = 0;
	char buf[128];
	FILE * fp;
	
	memset(buf, 0, 128);
	sprintf(buf, "ethernet_get_ip %s %s", ETHERNET_IP, s_network_dev[s_dev]);
	system_result = system(buf);

	fp = fopen(ETHERNET_IP,"r");
	if(fp ==NULL) 
	{
		return;
	}

	//mac
	memset(s_cur_mac, 0, 32);
	pf_result = fgets(s_cur_mac, 32, fp);
	len = strlen(s_cur_mac);
	s_cur_mac[len -1] = 0;
	
	//mode,ip,subnet,gateway,dns
	memset(buf, 0, 128);
	pf_result = fgets(buf, 128, fp);
	s_mode = atoi(buf);
	app_network_mode_change();
	
	for(i = 0; i < 4; i++)
	{
		memset(buf, 0, 128);
		pf_result = fgets(buf, 18, fp);	
		if(buf[strlen(buf) -1] == '\n')
			buf[strlen(buf) -1] = 0;
		if(strlen(buf) < 15)		
			app_network_ip_completion(buf);
		GUI_SetProperty(s_network_opt[i+2], "string", buf);
	}
	
	fclose(fp);
}

void app_network_dev_update(void)
{
	GUI_SetProperty(s_network_opt[0], "select", &s_dev);
	app_network_ip_update();
}

void app_network_update(void)
{
	char buf[32];
	FILE * fp;
	uint8_t i =0;
	uint8_t len =0;
	
		system_result = system("ethernet_get_dev_list /tmp/app/ethernet_dev A");
	
	fp = fopen(ETHERNET_DEV,"r");
	if(fp ==NULL) 
	{
		return;
	}

	memset(buf, 0, 32);
	pf_result = fgets(buf, 32, fp);
	s_dev_total = atoi(buf);

	if(s_network_dev )
	{
		free(s_network_dev);
		s_network_dev = NULL;
	}
	s_network_dev = malloc(s_dev_total * sizeof(ubyte32));

	if(s_dev_content)
	{
		free(s_dev_content);
		s_dev_content = NULL;
	}
	s_dev_content = malloc(s_dev_total * 32);
	memset(s_dev_content, 0, s_dev_total * 32);
	
	for(i = 0; i < s_dev_total; i++)
	{
		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		
		memcpy(s_network_dev[i], buf, len);
		if(i ==0)
		{
			strcat(s_dev_content, "[");
			strcat(s_dev_content, s_network_dev[i]);
		}
		else
		{
			strcat(s_dev_content, ",");
			strcat(s_dev_content, s_network_dev[i]);
		}
	}
	strcat(s_dev_content, "]");
	fclose(fp);
	
	GUI_SetProperty(s_network_opt[0], "content", s_dev_content);
	s_dev = 0;
	app_network_dev_update();
}

void app_network_tip_show(void)
{
	GUI_SetProperty("img_network_tip_bk", "state", "show");
	GUI_SetProperty("text_network_tip_title", "state", "show");
	GUI_SetProperty("text_network_tip_status","state",  "show");
	GUI_SetProperty("text_network_tip_status","string",  "Please Wait");
}

void app_network_tip_hide(void)
{
	GUI_SetProperty("img_network_tip_bk", "state", "hide");
	GUI_SetProperty("text_network_tip_title", "state", "hide");
	GUI_SetProperty("text_network_tip_status", "state", "hide");
}

int app_network_status_check_timer(void* usrdata)
{
	FILE * fp;
	char buf[32];
	uint8_t len = 0;
	//PopDlg  pop;

	fp=fopen(ETHERNET_STATUS,"r");
	if(fp ==NULL) 
	{
		s_network_check_time++;
		return 0;
	}

	memset(buf, 0, 32);
	pf_result= fgets(buf, 32, fp);
	fclose(fp);
	
	len = strlen(buf);
	buf[len -1] = 0;
	if(strcmp(buf, "ok") == 0)
	{
		s_network_check_time = 0;
		timer_stop(s_network_status_timer);
		
		app_network_tip_hide();
		GUI_SetProperty(s_network_opt[s_sel], "unfocus_img", IMG_OPT_HALF_FOCUS_R);
		#if 0
		memset(&pop, 0, sizeof(PopDlg));
		pop.type = POP_TYPE_OK;
		pop.str = STR_ID_SET_SUC;
		pop.mode = POP_MODE_BLOCK;
		popdlg_create(&pop);
		#endif
		app_popmsg(340,200,"Set Success!",POPMSG_TYPE_OK);
		GUI_SetProperty(s_network_opt[s_sel], "unfocus_img", IMG_OPT_HALF_UNFOCUS);
		s_network_setting = false;
	}
	else if(strcmp(buf, "fail") == 0 || s_network_check_time > 25)
	{
		s_network_check_time = 0;
		timer_stop(s_network_status_timer);
		
		app_network_tip_hide();
		GUI_SetProperty(s_network_opt[s_sel], "unfocus_img", IMG_OPT_HALF_FOCUS_R);
		#if 0
		memset(&pop, 0, sizeof(PopDlg));
		pop.type = POP_TYPE_OK;
		pop.str = STR_ID_SET_FAIL;
		pop.mode = POP_MODE_BLOCK;
		popdlg_create(&pop);
		#endif
		app_popmsg(340,200,"Set Fail!",POPMSG_TYPE_OK);
		GUI_SetProperty(s_network_opt[s_sel], "unfocus_img", IMG_OPT_HALF_UNFOCUS);
		s_network_setting = false;
	}
	else
	{
		s_network_check_time++;
	}
	return 0;
}

void app_network_set_config(void)
{
	char cmd[256];

	s_network_setting = true;
	
	if(s_ethernet_monitor_on == true)
	{
		s_ethernet_monitor_on = false;
		system_result = system("ethernet_monitor 0");
	}
	
	memset(cmd, 0, 256);
	if(s_mode == 0)
	{
		uint8_t i = 0;
		char *p[4] = {NULL};
		char buf[4][16];
		
		for(i = 0; i < 4; i++)
		{
			GUI_GetProperty(s_network_opt[i+2], "string", &p[i]);
			app_network_ip_simplify(buf[i], p[i]);
		}
		sprintf(cmd, "ethernet_cfg_ip %s %s %s %d %s %s %s  %s &", 
			ETHERNET_STATUS, s_network_dev[s_dev], s_cur_mac, s_mode,
			buf[0], buf[1], buf[2], buf[3]);
	}
	else
	{
		sprintf(cmd, "ethernet_cfg_ip %s %s %s %d &", ETHERNET_STATUS, s_network_dev[s_dev], s_cur_mac, s_mode);
	}
	printf("[cmd]%s\n", cmd);
	system_result = system(cmd);

	if(0 != reset_timer(s_network_status_timer)) 
	{	
		s_network_status_timer = create_timer(app_network_status_check_timer, 200, NULL, TIMER_REPEAT);
	}
}

SIGNAL_HANDLER int app_network_cmb1_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		
		switch(event->key.sym)
		{
			case STBK_RIGHT:
				if(s_dev_total == 1)
					return EVENT_TRANSFER_STOP;
				if(s_dev < s_dev_total-1)
					s_dev++;
				else
					s_dev = 0;
				app_network_dev_update();
				break;
			case STBK_LEFT:
				if(s_dev_total == 1)
					return EVENT_TRANSFER_STOP;
				if(s_dev >0)
					s_dev--;
				else
					s_dev = s_dev_total-1;
				app_network_dev_update();
				break;
			case STBK_UP:
				GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_UNFOCUS);
				GUI_SetProperty(s_network_text[0], "string", s_network_title_str[0]);
                if(s_mode == 0)
				{
					GUI_SetProperty(s_network_img[5], "img", IMG_OPT_HALF_FOCUS_L);
                    GUI_SetProperty(s_network_text[5], "string", s_network_title_str[5]);
                    s_sel = 5;
					GUI_SetFocusWidget(s_network_opt[5]);
				}
				else
				{
					GUI_SetProperty(s_network_img[1], "img", IMG_OPT_HALF_FOCUS_L);
                    GUI_SetProperty(s_network_text[1], "string", s_network_title_str[1]);
                    s_sel = 1;
					GUI_SetFocusWidget(s_network_opt[1]);
				}
				break;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[0], "string", s_network_title_str[0]);
                GUI_SetProperty(s_network_img[1], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[1], "string", s_network_title_str[1]);
				s_sel = 1;
				GUI_SetFocusWidget(s_network_opt[1]);
				break;
			case STBK_OK:
			case STBK_EXIT:
				return EVENT_TRANSFER_KEEPON;
			default:
				break;
		}
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_network_cmb2_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;

		switch(event->key.sym)
		{
			case STBK_RIGHT:
			case STBK_LEFT:
				s_mode = (s_mode+1)%2;
				app_network_mode_change();
				break;
			case STBK_UP:
                GUI_SetProperty(s_network_img[1], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[1], "string", s_network_title_str[1]);
                GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[0], "string", s_network_title_str[0]);
				s_sel = 0;
				GUI_SetFocusWidget(s_network_opt[0]);
				break;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[1], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[1], "string", s_network_title_str[1]);
				if(s_mode == 0)
				{
                GUI_SetProperty(s_network_img[2], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[2], "string", s_network_title_str[2]);
				s_sel = 2;
				GUI_SetFocusWidget(s_network_opt[2]);
				}
				else
				{
					GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_FOCUS_L);
                    GUI_SetProperty(s_network_text[0], "string", s_network_title_str[0]);
                    s_sel = 0;
					GUI_SetFocusWidget(s_network_opt[0]);
				}
				break;
			case STBK_OK:
			case STBK_EXIT:
				return EVENT_TRANSFER_KEEPON;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_network_edit1_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		
		switch(event->key.sym)
		{
			case STBK_UP:
                GUI_SetProperty(s_network_img[2], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[2], "string", s_network_title_str[2]);
                GUI_SetProperty(s_network_img[1], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[1], "string", s_network_title_str[1]);
				s_sel = 1;
				GUI_SetFocusWidget(s_network_opt[1]);
				return EVENT_TRANSFER_STOP;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[2], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[2], "string", s_network_title_str[2]);
					GUI_SetProperty(s_network_img[3], "img", IMG_OPT_HALF_FOCUS_L);
                    GUI_SetProperty(s_network_text[3], "string", s_network_title_str[3]);
                    s_sel = 3;
					GUI_SetFocusWidget(s_network_opt[3]);
				return EVENT_TRANSFER_STOP;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int app_network_edit2_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		
		switch(event->key.sym)
		{
			case STBK_UP:
                GUI_SetProperty(s_network_img[3], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[3], "string", s_network_title_str[3]);
                GUI_SetProperty(s_network_img[2], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[2], "string", s_network_title_str[2]);
				s_sel = 2;
				GUI_SetFocusWidget(s_network_opt[2]);
				return EVENT_TRANSFER_STOP;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[3], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[3], "string", s_network_title_str[3]);
                GUI_SetProperty(s_network_img[4], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[4], "string", s_network_title_str[4]);
				s_sel = 4;
				GUI_SetFocusWidget(s_network_opt[4]);
				return EVENT_TRANSFER_STOP;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int app_network_edit3_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		
		switch(event->key.sym)
		{
			case STBK_UP:
                GUI_SetProperty(s_network_img[4], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[4], "string", s_network_title_str[4]);
                GUI_SetProperty(s_network_img[3], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[3], "string", s_network_title_str[3]);
				s_sel = 3;
				GUI_SetFocusWidget(s_network_opt[3]);
				return EVENT_TRANSFER_STOP;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[4], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[4], "string", s_network_title_str[4]);
                GUI_SetProperty(s_network_img[5], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[5], "string", s_network_title_str[5]);
				s_sel = 5;
				GUI_SetFocusWidget(s_network_opt[5]);
				return EVENT_TRANSFER_STOP;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int app_network_edit4_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		
		switch(event->key.sym)
		{
			case STBK_UP:
                GUI_SetProperty(s_network_img[5], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[5], "string", s_network_title_str[5]);
                GUI_SetProperty(s_network_img[4], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[4], "string", s_network_title_str[4]);
				s_sel = 4;
				GUI_SetFocusWidget(s_network_opt[4]);
				return EVENT_TRANSFER_STOP;
			case STBK_DOWN:
                GUI_SetProperty(s_network_img[5], "img", IMG_OPT_HALF_UNFOCUS);
                GUI_SetProperty(s_network_text[5], "string", s_network_title_str[5]);
                GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_FOCUS_L);
                GUI_SetProperty(s_network_text[0], "string", s_network_title_str[0]);
				s_sel = 0;
				GUI_SetFocusWidget(s_network_opt[0]);
				return EVENT_TRANSFER_STOP;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int app_network_create(const char* widgetname, void *usrdata)
{	
	s_sel = 0;
	
	GUI_SetProperty(s_network_opt[1], "content", s_mode_content);
    GUI_SetProperty(s_network_img[0], "img", IMG_OPT_HALF_FOCUS_L);	
    GUI_SetFocusWidget(s_network_opt[0]);
	app_network_update();
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_network_destroy(const char* widgetname, void *usrdata)
{
	s_network_setting = false;
	s_ethernet_monitor_on = true;
	s_network_check_time = 0;
	
	if(s_network_dev )
	{
		free(s_network_dev);
		s_network_dev = NULL;
	}

	if(s_dev_content)
	{
		free(s_dev_content);
		s_dev_content = NULL;
	}

	if(s_network_status_timer)
	{
		remove_timer(s_network_status_timer);
		s_network_status_timer = NULL;
	}

	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_network_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_network_setting && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
	
		switch(event->key.sym)
		{
			case STBK_OK:
				{
					app_network_tip_show();
					app_network_set_config();
				}
				break;
			case STBK_EXIT:
				if(s_network_setting == true)
				{
					char cmd[64];

					app_network_tip_hide();
					timer_stop(s_network_status_timer);
					s_network_setting = false;
					s_network_check_time = 0;
					
					memset(cmd, 0, 64);
					sprintf(cmd, "ethernet_cancel %s", s_network_dev[s_dev]);
					printf("[cmd] %s\n", cmd);
					system_result = system(cmd);
				}
				else
				{
					system_result = system("ethernet_exit");
					if(s_ethernet_monitor_on == false)
					{	
						s_ethernet_monitor_on = true;
						system_result = system("ethernet_monitor 1 &");
					}
					GUI_EndDialog("wnd_network");
				}
				break;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_STOP;
}


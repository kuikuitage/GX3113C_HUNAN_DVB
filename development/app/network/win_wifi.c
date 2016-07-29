#include "app.h"
#include "stdio.h"
#include "app_full_keyboard.h"


#define BUF_SMALL_LEN		33
#define BUF_BIG_LEN			1024


#define WND_WIFI						"win_wifi"
#define LIST_WIFI						"win_wifi_listview_ap"
#define TXT_WIFI_LINK_STATUS			"win_wifi_text_statu"
#define TXT_WIFI_LINK_TITLE				"win_wifi_text_st_title"
#define IMG_WIFI_LINK  					"img_wifi_link"
#define TXT_WIFI_MOVE 					"win_wifi_text_move"
#define IMG_WIFI_MOVE 					"win_wifi_tips_move"
#define TXT_WIFI_OK 					"win_wifi_text_confirm"
#define IMG_WIFI_OK 					"win_wifi_image_confirm"
#define TXT_WIFI_EXIT 					"win_wifi_text_exit"
#define IMG_WIFI_EXIT 					"win_wifi_image_exit"
#define TXT_WIFI_REFRESH 				"win_wifi_text_lefright"
#define IMG_WIFI_REFRESH 				"win_wifi_image_green"

#define WIFI_PSK						"/tmp/app/wifi_psk"
#define WIFI_DEV 		 				"/tmp/app/wifi_dev"
#define WIFI_AP		 					"/tmp/app/wifi_ap"
#define WIFI_CONNECT					"/tmp/app/wifi_connect"
#define WIFI_DISCONNECT					"/tmp/app/wifi_disconnect"
#define WIFI_STATUS						"/tmp/app/wifi_status"

#define WIFI_KEYBOARD_HINT		"Hint:Press Num Keys or Choose in KeyBoard"

#define GIF_PATH WORK_PATH"theme/image/wifi/loading.gif"


typedef enum _wifilink_status
{
	WIFI_LINKING,
	WIFI_LINK_OK,
	WIFI_LINK_FAIL,	
}WIFILINK_STATUS;

typedef enum _wifilink_active
{
	WIFI_LINK_CONNECT = 0,
	WIFI_LINK_DISCONNECT,
}WIFILINK_ACTIVE;


typedef struct _wifiap_para_t
{
	char essid[BUF_SMALL_LEN];
	char ip[BUF_SMALL_LEN];
	char psk[BUF_SMALL_LEN*2];
	char mac[BUF_SMALL_LEN];
	char ap_mac[BUF_SMALL_LEN];
	char encryption[8];
	char quality[8];
	uint8_t quality_level;
	uint8_t is_link;
	struct _wifiap_para_t* next;
}WifiApPara;

typedef char ubyte32[32];


int s_wifi_sel= 0;
static int system_result;
static char * pf_result;
//static bool s_wifi_psk_reset = false;
static uint8_t s_cur_dev = 0;
static uint8_t s_wifi_dev_total = 0;
static int s_ap_total = 0;
static ubyte32 *s_wifi_dev = NULL;
static char s_cur_wifi_mac[32];
static uint8_t s_wifi_connect = 0;
static uint8_t s_wifi_encryption = 0;
static WifiApPara *sp_ap_head = NULL;
static WifiApPara *sp_ap_cur = NULL;
static bool s_wifi_linking = false;

static char *s_img_strength[] = {
	"s_signal_0.bmp",
	"s_signal_1.bmp",
	"s_signal_2.bmp",
	"s_signal_3.bmp",
	"s_signal_4.bmp",
	"s_signal_5.bmp",
};

void app_wifi_set_hide(void)
{
	
	GUI_SetProperty("img_wifi_set_bk", "state", "hide");
	
	GUI_SetProperty("text_wifi_set0", "state", "hide");
	GUI_SetProperty("text_wifi_set1", "state", "hide");
	GUI_SetProperty("text_wifi_set2", "state", "hide");
	GUI_SetProperty("text_wifi_set3", "state", "hide");
	GUI_SetProperty("text_wifi_set4", "state", "hide");
	GUI_SetProperty("button_wifi_set5", "state", "hide");

	GUI_SetProperty("text_wifi_essid", "state", "hide");
	GUI_SetProperty("text_wifi_ip", "state", "hide");
	GUI_SetProperty("text_wifi_mac", "state", "hide");
	GUI_SetProperty("text_wifi_quality", "state", "hide");
	GUI_SetProperty("text_wifi_encryption", "state", "hide");

	GUI_SetProperty("button_wifi_psk", "state", "hide");
	GUI_SetProperty("img_wifi_tip_opt", "state", "hide");
	GUI_SetProperty("button_wifi_connect", "state", "hide");
	GUI_SetProperty("button_wifi_cancel", "state", "hide");
	GUI_SetProperty("img_wifi_tip_psk","state","hide");
	GUI_SetFocusWidget(LIST_WIFI);
}



void app_wifi_hint_show(void)
{	
	GUI_SetProperty(IMG_WIFI_OK, "state", "show");
	GUI_SetProperty(TXT_WIFI_OK, "state", "show");
	GUI_SetProperty(IMG_WIFI_MOVE, "state", "show");
	GUI_SetProperty(TXT_WIFI_MOVE, "state", "show");
	GUI_SetProperty(IMG_WIFI_EXIT, "state", "show");
	GUI_SetProperty(TXT_WIFI_EXIT, "state", "show");
	GUI_SetProperty(IMG_WIFI_REFRESH, "state", "show");
	GUI_SetProperty(TXT_WIFI_REFRESH, "state", "show");

}

void app_wifi_hint_hide(void)
{
	GUI_SetProperty(IMG_WIFI_OK, "state", "hide");
	GUI_SetProperty(TXT_WIFI_OK, "state", "hide");
	GUI_SetProperty(IMG_WIFI_MOVE, "state", "hide");
	GUI_SetProperty(TXT_WIFI_MOVE, "state", "hide");
	GUI_SetProperty(IMG_WIFI_EXIT, "state", "hide");
	GUI_SetProperty(TXT_WIFI_EXIT, "state", "hide");
	GUI_SetProperty(IMG_WIFI_REFRESH, "state", "hide");
	GUI_SetProperty(TXT_WIFI_REFRESH, "state", "hide");
}



void app_wifi_tip_show(void)
{		
	GUI_SetProperty(IMG_WIFI_LINK, "state", "show");
	GUI_SetProperty(TXT_WIFI_LINK_TITLE, "state", "show");
	GUI_SetProperty(TXT_WIFI_LINK_STATUS, "state", "show");
	GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "Please Wait...");
}

void app_wifi_tip_hide(void)
{
	
	GUI_SetProperty(IMG_WIFI_LINK, "state", "hide");
	GUI_SetProperty(TXT_WIFI_LINK_STATUS, "state", "hide");
	GUI_SetProperty(TXT_WIFI_LINK_TITLE, "state", "hide");
}



void wifi_get_psk( WifiApPara *p)
{
	FILE * fp;
	char cmd[256];
	char buf[32];

	memset(cmd, 0, 256);
	sprintf(cmd, "ethernet_get_ap_info %s %s \"%s\" %s", WIFI_PSK, s_cur_wifi_mac, p->essid, p->mac);
	system_result = system(cmd);
	
	fp = fopen(WIFI_PSK,"r");
	if(fp ==NULL) 
		return;

	//第一行无用
	memset(buf, 0, 32);
	pf_result = fgets(buf, 32, fp);
	if(atoi(buf) == 1)
	{
		memset(p->psk, 0, BUF_SMALL_LEN*2);
		pf_result = fgets(p->psk, BUF_SMALL_LEN*2, fp);
		p->psk[strlen(p->psk)-1] = 0;
		printf("[psk]%s=%s\n",p->essid, p->psk);
	}
	
	fclose(fp);
}

void ap_list_add(WifiApPara *p)
{
	if(sp_ap_head == NULL)
	{
		sp_ap_head = sp_ap_cur = p;
	}
	else
	{
		sp_ap_cur->next = p;
		sp_ap_cur = p;
	}
}

WifiApPara *ap_list_get(uint8_t index)
{
	WifiApPara *p = sp_ap_head;
	uint8_t i = 0;

	if(p == NULL)
		return p;
	for(i = 0; i < index; i++)
		p = p->next;
	return p;
}


void ap_list_destroy(void)
{	
	char cmd[256];
	WifiApPara *p = sp_ap_head;
	
	memset(cmd, 0, 256);
	
	if(sp_ap_head)
	{
		while(p)
		{
			sp_ap_head = p->next;
			free(p);
			p = sp_ap_head;
		}
	}
	sp_ap_head = sp_ap_cur = NULL;

	sprintf(cmd, "rm %s -rf", WIFI_AP);
	system(cmd);
}

void app_wifi_draw_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("win_wifi_image_gif", "draw_gif", &alu);
}

void app_wifi_hide_gif(void)
{
	GUI_SetProperty("win_wifi_image_gif", "state", "hide");
}

void app_wifi_show_gif(void)
{
	GUI_SetProperty("win_wifi_image_gif", "state", "show");
}

void app_wifi_load_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("win_wifi_image_gif", "load_img", GIF_PATH);
	GUI_SetProperty("win_wifi_image_gif", "init_gif_alu_mode", &alu);
}

void app_wifi_free_gif(void)
{
	GUI_SetProperty("win_wifi_image_gif", "load_img", NULL);
}



void app_wifi_gif_update_proc(void* userdata)	
{
	app_wifi_draw_gif();
}

void app_wifi_set_show(void)
{
	WifiApPara *p_ap = NULL;
	
	p_ap = ap_list_get(s_wifi_sel);
	if(p_ap == NULL)
		return;
	
	GUI_SetProperty("img_wifi_set_bk", "state", "show");
	
	GUI_SetProperty("text_wifi_set0", "state", "show");
	GUI_SetProperty("text_wifi_set1", "state", "show");
	GUI_SetProperty("text_wifi_set2", "state", "show");
	GUI_SetProperty("text_wifi_set3", "state", "show");
	GUI_SetProperty("text_wifi_set4", "state", "show");
	GUI_SetProperty("button_wifi_set5", "state", "show");

	GUI_SetProperty("text_wifi_essid", "string", p_ap->essid);
	GUI_SetProperty("text_wifi_essid", "state", "show");
	if(strlen(p_ap->ip) == 0)
		GUI_SetProperty("text_wifi_ip", "string", "N/A");
	else
		GUI_SetProperty("text_wifi_ip", "string", p_ap->ip);
	GUI_SetProperty("text_wifi_ip", "state", "show");
	GUI_SetProperty("text_wifi_mac", "string", p_ap->mac);
	GUI_SetProperty("text_wifi_mac", "state", "show");
	GUI_SetProperty("text_wifi_quality", "string", p_ap->quality);
	GUI_SetProperty("text_wifi_quality", "state", "show");
	GUI_SetProperty("text_wifi_encryption", "string", p_ap->encryption);
	if(strcmp(p_ap->encryption , "on") == 0)
		s_wifi_encryption = 1;
	else		
		s_wifi_encryption =0;
	GUI_SetProperty("text_wifi_encryption", "state", "show");

	if(strlen(p_ap->psk) == 0)
	{
		GUI_SetProperty("button_wifi_psk", "string", "N/A");
	}
	else
	{
		GUI_SetProperty("button_wifi_psk", "string", "******");
	}	
	GUI_SetProperty("button_wifi_psk", "state", "show");

	if(p_ap->is_link == 0)
	{
		s_wifi_connect = 0;
		GUI_SetProperty("button_wifi_connect", "string", "Connect");	
	}
	else
	{
		s_wifi_connect = 1;
		GUI_SetProperty("button_wifi_connect", "string", "Disconnect");
	}

	if(s_wifi_encryption == 1 && s_wifi_connect == 0)
	{
		GUI_SetProperty("button_wifi_set5","forecolor", "[#96C1F6,#96C1F6,#96C1F6]");
		GUI_SetProperty("button_wifi_psk","forecolor", "[#f4f4f4,#f4f4f4,#f4f4f4]");
	}
	else
	{
		GUI_SetProperty("button_wifi_set5","forecolor", "[#656570,#656570,#656570]");
		GUI_SetProperty("button_wifi_psk","forecolor", "[#656570,#656570,#656570]");
	}
	
	GUI_SetProperty("img_wifi_tip_opt", "state", "show");
	GUI_SetProperty("button_wifi_connect", "state", "show");
	GUI_SetProperty("button_wifi_cancel", "state", "show");
	GUI_SetProperty("img_wifi_tip_psk","state","hide");

	GUI_SetFocusWidget("button_wifi_connect");
	
}



void ap_list_create(void)
{
	char buf[32];
	char cmd[128];
	FILE* fp;
	uint8_t i =0;
	uint8_t len =0;
	WifiApPara *p_ap;
	
	s_ap_total = 0;
	
	memset(cmd, 0, 128);
	sprintf(cmd, "ethernet_get_wifi_ap_list %s %s", WIFI_AP, s_wifi_dev[s_cur_dev]);
	system_result = system_shell(cmd,0,app_wifi_gif_update_proc,NULL,NULL);
	
	fp = fopen(WIFI_AP,"r");
	if(fp ==NULL) 
	{
		return;
	}

	memset(s_cur_wifi_mac, 0, 32);
	pf_result = fgets(s_cur_wifi_mac, 32, fp);
	len = strlen(s_cur_wifi_mac);
	s_cur_wifi_mac[len -1] = 0;

	memset(buf, 0, 32);
	pf_result = fgets(buf, 32, fp);
	s_ap_total = atoi(buf);

	if(s_ap_total == 0)
		return;

	for(i = 0; i < s_ap_total; i++)
	{
		p_ap = (WifiApPara *)malloc(sizeof(WifiApPara));
		memset(p_ap, 0, sizeof(WifiApPara));
		ap_list_add(p_ap);

		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		memcpy(sp_ap_cur->mac, buf, len);

		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		memcpy(sp_ap_cur->essid, buf, len);

		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		sprintf(sp_ap_cur->quality, "%s/100",buf);
		len = atoi(buf);
		if(len == 0)
			sp_ap_cur->quality_level = 0;
		else if(len >0 && len<=20)
			sp_ap_cur->quality_level = 1;
		else if(len >20 && len<=40)
			sp_ap_cur->quality_level = 2;
		else if(len >40 && len<=60)
			sp_ap_cur->quality_level = 3;
		else if(len >60 && len<=80)
			sp_ap_cur->quality_level = 4;
		else 
			sp_ap_cur->quality_level = 5;

		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		memcpy(sp_ap_cur->encryption, buf, len);
		if(len == 3)//sp_ap_cur->encryption = "on"
			wifi_get_psk(sp_ap_cur);
	}
	
}


void ap_list_changeToHead(WifiApPara *p_ap)
{
	if(sp_ap_head == p_ap)
	{
		return;
	}
	else
	{
		WifiApPara tmp;
		WifiApPara *p_tmp;
		
		memset(&tmp, 0, sizeof(WifiApPara) );

		memcpy(&tmp, sp_ap_head, sizeof(WifiApPara));
		memcpy(sp_ap_head, p_ap, sizeof(WifiApPara));
		memcpy(p_ap, &tmp, sizeof(WifiApPara));

		p_tmp = sp_ap_head->next;
		sp_ap_head->next = p_ap->next;
		p_ap->next = p_tmp;
	}
	
}

WifiApPara * ap_list_find_curAp(void)
{
	char cur_ap_mac[BUF_SMALL_LEN]  = {0};
	char cmd[128];
	FILE * fp;
	
	memset(cmd, 0, 128);
	sprintf(cmd, "ethernet_get_connect_status %s %s", WIFI_STATUS, s_wifi_dev[s_cur_dev]);
	system_result = system(cmd);
	
	fp = fopen(WIFI_STATUS,"r");
	if(fp ==NULL) 
	{
		return NULL;;
	}
	
	memset(cur_ap_mac, 0, 32);
	pf_result = fgets(cur_ap_mac, 32, fp);
	
	if(strlen(cur_ap_mac) == 0)
	{
		fclose(fp);
		return NULL;
	}
	else
	{
		WifiApPara *p = sp_ap_head;
		uint8_t len = 0;
		
		while(p)
		{
			if(strncmp(p->mac, cur_ap_mac,strlen(p->mac)) == 0)
			{
				memset(p->ip, 0, BUF_SMALL_LEN);
				pf_result = fgets(p->ip, BUF_SMALL_LEN, fp);
				len = strlen(p->ip);
				p->ip[len-1] = 0;
				
				p->is_link = 1;
				break;
			}
			p = p->next;
		}
		fclose(fp);
		return p;
	}
}



void app_wifi_get_dev(void)
{
	char buf[32];
	FILE * fp;
	uint8_t i =0;
	uint8_t len =0;

	s_wifi_dev_total = 0;
	system_result = system_shell("ethernet_get_dev_list /tmp/app/wifi_dev W", 0,app_wifi_gif_update_proc,NULL,NULL);
	
	fp = fopen(WIFI_DEV,"r");
	if(fp ==NULL) 
	{
		return;
	}

	memset(buf, 0, 32);
	pf_result = fgets(buf, 32, fp);
	s_wifi_dev_total = atoi(buf);

	if(s_wifi_dev_total == 0)
		return;

	if(s_wifi_dev)
	{
		free(s_wifi_dev);
		s_wifi_dev = NULL;
	}
	s_wifi_dev = malloc(s_wifi_dev_total * sizeof(ubyte32));
	
	for(i = 0; i < s_wifi_dev_total; i++)
	{
		memset(buf, 0, 32);
		pf_result = fgets(buf, 32, fp);
		len = strlen(buf);
		buf[len -1] = 0;
		
		memcpy(s_wifi_dev[i], buf, len);
	}
	
	fclose(fp);
	s_cur_dev = 0;	
}

WIFILINK_STATUS app_wifi_connect_check(void)
{
	FILE * fp;
	char buf[32];
	WifiApPara *p_ap = NULL;
	uint8_t len = 0;
	
	fp=fopen(WIFI_CONNECT,"r");
	if(fp ==NULL) 
	{
		printf("### No file!\n");
		return WIFI_LINKING;
	}
	
	memset(buf,0,32);
	pf_result = fgets(buf ,32,fp);
	if(strncmp(buf, "ok", 2) == 0)
	{
		p_ap = ap_list_get(s_wifi_sel);
		if(p_ap == NULL)
		{	
			fclose(fp);
			return WIFI_LINK_OK;
		}
		
		p_ap->is_link = 1;

		memset(p_ap->ap_mac,0,BUF_SMALL_LEN);
		pf_result = fgets(p_ap->ap_mac ,BUF_SMALL_LEN,fp);
		len  = strlen(p_ap->ap_mac);
		p_ap->ap_mac[len-1] = 0;

		memset(p_ap->ip,0,BUF_SMALL_LEN);
		pf_result = fgets(p_ap->ip ,BUF_SMALL_LEN,fp);
		len  = strlen(p_ap->ip);
		p_ap->ip[len-1] = 0;

		fclose(fp);
		return WIFI_LINK_OK;
	}
	else if(strncmp(buf, "fail",4) == 0)
	{
		fclose(fp);
		return WIFI_LINK_FAIL;
	}
	else
	{
		fclose(fp);
		return WIFI_LINKING;
	}
}

void app_wifi_connect_ok_proc(void* userdata)
{
	WifiApPara *p_ap = NULL;

	if(WIFI_LINK_OK == app_wifi_connect_check())
	{
		app_wifi_hide_gif();
		
		p_ap = ap_list_get(s_wifi_sel);
		s_wifi_sel = 0;
		ap_list_changeToHead(p_ap);

		GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "success");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(1000);
		app_wifi_tip_hide();

        GUI_SetProperty(LIST_WIFI, "update_all", NULL);
		GUI_SetProperty(LIST_WIFI, "select", &s_wifi_sel);

		s_wifi_linking = false;
	}
	else
	{
		char cmd[32];

		sprintf(cmd, "ethernet_cancel %s", s_wifi_dev[s_cur_dev]);
		system_result = system(cmd);

		app_wifi_hide_gif();
		GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "failed");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(1000);
		app_wifi_tip_hide();
		
		s_wifi_linking = false;
	}
}

void app_wifi_clear(void)
{
	WifiApPara *p = sp_ap_head;

	while(p!=NULL)
	{
		if(p->is_link == 1)
		{
			p->is_link = 0;
			memset(p->ip, 0,BUF_SMALL_LEN); 
			memset(p->ap_mac, 0,BUF_SMALL_LEN); 
		}
		p = p->next;
	}
}

void app_wifi_connect(void)
{
	char cmd[256];
	WifiApPara *p_ap;
	
	s_wifi_linking = true;
	//app_wifi_set_hide();
	app_wifi_tip_show();
	GUI_SetInterface("flush",NULL);
	app_wifi_clear();

	p_ap = ap_list_get(s_wifi_sel);
	if(p_ap == NULL)
	{
		s_wifi_linking = false;
		GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "failed");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(1000);
		app_wifi_tip_hide();

		return;
	}

	/*if(s_wifi_psk_reset == true)
	{
		s_wifi_psk_reset = false;
		memset(p_ap->psk, 0, BUF_SMALL_LEN*2);
		memcpy(p_ap->psk, s_full_keyboard_input, strlen(s_full_keyboard_input));
	}*/
	
	memset(cmd, 0, 256);
	sprintf(cmd, "ethernet_wifi_connect %s %s \"%s\" %s \"%s\" ", WIFI_CONNECT, s_wifi_dev[s_cur_dev], p_ap->essid, p_ap->mac, p_ap->psk);
	printf("[cmd]%s",cmd);

	app_wifi_show_gif();
	system_result = system_shell(cmd,15000,app_wifi_gif_update_proc,app_wifi_connect_ok_proc,NULL);

}

void app_wifi_disconnect(void)
{
	char cmd[256];
	WifiApPara *p_ap;
	
	s_wifi_linking = true;
	
	//app_wifi_set_hide();
	app_wifi_tip_show();
	GUI_SetInterface("flush",NULL);
	
	app_wifi_clear();

	p_ap = ap_list_get(s_wifi_sel);
	memset(cmd, 0, 256);
	sprintf(cmd, "ethernet_disconnect %s %s \"%s\" %s \"%s\" ", WIFI_DISCONNECT, s_wifi_dev[s_cur_dev], p_ap->essid, p_ap->mac, p_ap->psk);
	printf("[cmd]%s\n",cmd);
	
	app_wifi_show_gif();
	system_result = system_shell(cmd, 0,app_wifi_gif_update_proc,NULL,NULL);
	app_wifi_hide_gif();
	GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "success");
	GUI_SetInterface("flush",NULL);
	GxCore_ThreadDelay(1000);
	app_wifi_tip_hide();

	s_wifi_linking = false;
}

#if 1
void app_wifi_ap_update(char *buf)
{
	app_wifi_tip_show();
	app_wifi_show_gif();
	GUI_SetInterface("flush",NULL);
	
	s_wifi_sel = 0;
	s_ap_total = 0;
	
	ap_list_destroy();
	ap_list_create();
	app_wifi_hide_gif();
	
	if(s_ap_total == 0)
	{
		GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "no ap");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(1000);
		app_wifi_tip_hide();
		GUI_SetProperty(LIST_WIFI, "update_all", NULL);
	}
	else
	{
		WifiApPara * p = NULL;
		if(( p =ap_list_find_curAp()))
		{
			ap_list_changeToHead(p);
		}

		GUI_SetProperty("text_wifi_dev", "string", s_wifi_dev[s_cur_dev]);

		if(buf != NULL)
		{
			GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", buf);
			GUI_SetInterface("flush",NULL);
			GxCore_ThreadDelay(1000);
		}
		app_wifi_tip_hide();

        GUI_SetProperty(LIST_WIFI, "update_all", NULL);
		GUI_SetProperty(LIST_WIFI, "select", &s_wifi_sel);
	}
}

void app_wifi_update(char *buf)
{
	app_wifi_tip_show();
	app_wifi_show_gif();
	GUI_SetInterface("flush",NULL);
	
	app_wifi_get_dev();

	if(s_wifi_dev_total ==0 )
	{
		s_wifi_sel = 0;
		s_ap_total = 0;

		GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "no wifi");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(1000);
		app_wifi_tip_hide();
	}
	else
	{
		if(s_wifi_dev_total >1)
		{
			GUI_SetProperty("img_wifi_l","state","show");
			GUI_SetProperty("img_wifi_r","state","show");
		}
		else
		{
			GUI_SetProperty("img_wifi_l","state","hide");
			GUI_SetProperty("img_wifi_r","state","hide");
		}
		GUI_SetProperty("win_wifi_text_dev", "string", s_wifi_dev[s_cur_dev]);

		s_wifi_sel = 0;
		s_ap_total = 0;
		ap_list_destroy();
		ap_list_create();

		app_wifi_hide_gif();

		if(s_ap_total == 0)
		{
			GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", "no ap");
			GUI_SetInterface("flush",NULL);
			GxCore_ThreadDelay(1000);
			app_wifi_tip_hide();
			GUI_SetProperty(LIST_WIFI, "update_all", NULL);
		}
		else
		{
			WifiApPara * p = NULL;
			if(( p =ap_list_find_curAp()))
			{
				p->is_link = 1;
				ap_list_changeToHead(p);
			}

			if(buf != NULL)
			{
				GUI_SetProperty(TXT_WIFI_LINK_STATUS, "string", buf);
				GUI_SetInterface("flush",NULL);
				GxCore_ThreadDelay(1000);
			}
			app_wifi_tip_hide();

            GUI_SetProperty(LIST_WIFI, "update_all", NULL);
			GUI_SetProperty(LIST_WIFI, "select", &s_wifi_sel);
		}
	}
}
#endif
extern full_keyboard_proc s_full_keyboard_proc;
extern char s_full_keyboard_input[BUF_LEN];
void app_wifi_full_keyboard_proc(void)
{
	WifiApPara *p_ap = NULL;
	p_ap = ap_list_get(s_wifi_sel);
		if(p_ap == NULL)
			return;
	memset(p_ap->psk, 0, BUF_SMALL_LEN*2);
	memcpy(p_ap->psk, s_full_keyboard_input, strlen(s_full_keyboard_input));
	GUI_SetProperty("button_wifi_psk", "state", "hide");
	if(strlen(p_ap->psk) == 0)
	{
		GUI_SetProperty("button_wifi_psk", "string", "N/A");
	}
	else
	{
		GUI_SetProperty("button_wifi_psk", "string", "******");
	}
	GUI_SetProperty("button_wifi_psk", "state", "show");
	GUI_SetFocusWidget("button_wifi_psk");
}

#if 0
void app_wifi_full_keyboard_proc(PopKeyboard *data)
{
	WifiApPara *p_ap = NULL;
	//app_wifi_hint_show();
	if(data->in_ret == POP_VAL_CANCEL)
		return;
	if (data->out_name == NULL)
		return;

	//s_wifi_psk_reset = true;
		p_ap = ap_list_get(s_wifi_sel);
		if(p_ap == NULL)
			return;

	memset(p_ap->psk, 0, BUF_SMALL_LEN*2);
	memcpy(p_ap->psk, data->out_name, strlen(data->out_name));

	GUI_SetProperty("button_wifi_psk", "state", "hide");
	if(strlen(p_ap->psk) == 0)
	{
		GUI_SetProperty("button_wifi_psk", "string", "N/A");
	}
	else
	{
		GUI_SetProperty("button_wifi_psk", "string", "******");
	}
	GUI_SetProperty("button_wifi_psk", "state", "show");
	GUI_SetFocusWidget("button_wifi_psk");
}
#endif
SIGNAL_HANDLER int app_wifi_button_psk_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{		
		switch(event->key.sym)
		{
			case STBK_EXIT:
				//s_wifi_psk_reset = false;
				app_wifi_set_hide();
				break;
			case STBK_UP:
			case STBK_DOWN:
				GUI_SetProperty("button_wifi_cancel","string","Cancel");
				GUI_SetFocusWidget("button_wifi_connect");
				break;
			case STBK_OK:
				#if 1
				app_wifi_hint_hide();
				s_full_keyboard_proc = app_wifi_full_keyboard_proc;
				GUI_CreateDialog(WND_FULL_KEYBOARD);
				GUI_SetProperty(TXT_FULL_KEYBOARD_HINT, "string",WIFI_KEYBOARD_HINT);
				#else
				{
					static PopKeyboard keyboard;
					WifiApPara *p_ap = NULL;

					p_ap = ap_list_get(s_wifi_sel);
					memset(&keyboard, 0, sizeof(PopKeyboard));
					keyboard.in_name    = p_ap->psk;
					keyboard.max_num = BUF_SMALL_LEN*2-1;
					keyboard.out_name   = NULL;
					keyboard.change_cb  = NULL;
					keyboard.release_cb = app_wifi_full_keyboard_proc;
					keyboard.usr_data   = NULL;
					keyboard.pos.x = 500;
					multi_language_keyboard_create(&keyboard);
                    //keyboard_create(&keyboard);
				}
				#endif
				break;
			default:
				break;
		}
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_wifi_button_connect_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{		
		if(s_wifi_linking == TRUE )
			return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_EXIT:
				//s_wifi_psk_reset = false;
				app_wifi_set_hide();
				break;
			case STBK_UP:
			case STBK_DOWN:
				printf("s_wifi_en=%d,s_wifi_con=%d\n",s_wifi_encryption,s_wifi_connect);
				if(s_wifi_encryption == 1 && s_wifi_connect == 0)
				{	
					GUI_SetProperty("img_wifi_tip_opt","state","hide");
					GUI_SetProperty("img_wifi_tip_psk","state","show");
					GUI_SetFocusWidget("button_wifi_psk");
				}
				break;
			case STBK_LEFT:
			case STBK_RIGHT:
				GUI_SetProperty("img_wifi_tip_opt","img","s_bar_choice_blue4.bmp");
				GUI_SetFocusWidget("button_wifi_cancel");
				break;
			case STBK_OK:
				if(s_wifi_connect == 1)
				{
					app_wifi_disconnect();
				}
				else
				{
					app_wifi_connect();
				}
				break;
			default:
				break;
		}
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_wifi_button_cancel_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{		
		if(s_wifi_linking == TRUE )
			return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_OK:
			case STBK_EXIT:
				//s_wifi_psk_reset = false;
				app_wifi_set_hide();
				break;
			case STBK_UP:
			case STBK_DOWN:
				if(s_wifi_encryption == 1 && s_wifi_connect == 0)
				{	
					GUI_SetProperty("img_wifi_tip_opt","state","hide");
					GUI_SetProperty("img_wifi_tip_psk","state","show");
					GUI_SetFocusWidget("button_wifi_psk");
				}
				break;
			case STBK_LEFT:
			case STBK_RIGHT:
				GUI_SetProperty("img_wifi_tip_opt","img","s_bar_choice_blue4_l.bmp");
				GUI_SetFocusWidget("button_wifi_connect");
				break;
			default:
				break;
		}
	}
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int app_wifi_create(const char* widgetname, void *usrdata)
{
	system_result = system("ethernet_monitor 0");
	app_wifi_load_gif();
	app_wifi_update(NULL);
	
	return EVENT_TRANSFER_STOP;
}



SIGNAL_HANDLER int app_wifi_destroy(const char* widgetname, void *usrdata)
{
	s_wifi_linking= FALSE;
	s_ap_total = 0;
	s_wifi_sel = 0;
	//s_wifi_psk_reset = false;
	
	app_wifi_free_gif();
	
	if(s_wifi_dev)
	{
		free(s_wifi_dev);
		s_wifi_dev = NULL;
	}

	ap_list_destroy();
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int app_wifi_keypress(const char* widgetname, void *usrdata)
{
	
}


SIGNAL_HANDLER int app_wifi_listview_get_total(const char* widgetname, void *usrdata)
{
	return s_ap_total;
}


SIGNAL_HANDLER int app_wifi_listview_get_data(const char* widgetname, void *usrdata)
{
	ListItemPara* item = NULL;
	static char buf[8];
	WifiApPara *p_ap;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	p_ap = ap_list_get(item->sel);
	if(p_ap == NULL)
		return GXCORE_ERROR;

	//col-0
	item->x_offset = 5;
	item->image = NULL;
	memset(buf,  0, 8);
	sprintf(buf, "%d. ", item->sel+1);
	item->string = buf;
	
	//col-1
	item = item->next;
	item->x_offset = 0;
	item->image = NULL;
	item->string = p_ap->essid;

	//col-2
	item = item->next;
	item->x_offset = 0;
	item->string = NULL;
	if(p_ap->is_link == 1)
		item->image = "s_choice.bmp";
	else
		item->image = NULL;
	
	//col-3
	item = item->next;
	item->x_offset = 0;
	item->string = NULL;
	if(strcmp(p_ap->encryption,"on")== 0)
		item->image = "s_lock.bmp";
	else
		item->image = NULL;

	//col-4
	item = item->next;
	item->x_offset = 0;
	item->string = NULL;
	item->image = s_img_strength[p_ap->quality_level];
		
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_wifi_listview_change(const char* widgetname, void *usrdata)
{
	GUI_GetProperty("win_wifi_listview_ap","select",(void*)&s_wifi_sel);
	printf("s_wifi_sel = %d\n",s_wifi_sel);
	return EVENT_TRANSFER_KEEPON;
}



SIGNAL_HANDLER int app_wifi_listview_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_wifi_linking == TRUE && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		switch(find_virtualkey(event->key.sym))
		{
			case STBK_EXIT:
				if(s_wifi_linking)
				{
					char cmd[32];
		
					sprintf(cmd, "ethernet_cancel %s", s_wifi_dev[s_cur_dev]);
					system_result = system(cmd);
					app_wifi_hide_gif();
					app_wifi_tip_hide();
					s_wifi_linking = false;
				}
				else
				{
					system_result = system("ethernet_exit");
					system_result = system("ethernet_monitor 1 &");
					GUI_EndDialog(WND_WIFI);
				}
				ret = EVENT_TRANSFER_STOP;
				break;
			case KEY_GREEN:
				if(s_wifi_dev_total == 0)
					app_wifi_update("Updete Success");
				else
					app_wifi_ap_update("Updete Success");
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_OK:
				app_wifi_set_show();
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}

	return ret;
}


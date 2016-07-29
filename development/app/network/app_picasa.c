//#include "app_root.h"
#include "app.h"

//#include "app_module.h"
//#include "app_msg.h"
//#include "app_send_msg.h"
//#include "full_screen.h"
//#include "app_pop.h"
//#include "app_book.h"
//#include "app_default_params.h"

#include "youtube_tools.h"

//define added by huangbc
#if defined(LINUX_OS)

#define STR_ID_SERVER_FAIL		"Server connecting failed, please retry!"


//---------------------------------------------------------------------
#define PIC_FEEDS_RESULT_PATH "/tmp/photo/picasa_feeds_result"
#define GIF_PATH WORK_PATH"theme/image/youtube/loading.gif"
#define PIC_LIST_RESULT_PATH "/tmp/photo/picasa_list_result"

static uint32_t s_total = 0;
static uint32_t s_total_page = 0;
static uint32_t s_cur_page = 0;
static uint32_t s_cur_sel = 0;
static uint32_t s_cur_feeds = 0;
static bool s_is_list_focus = true;
static int list_sel = 0;




//######################################################################
#define MAX_PICASA_FEED 10
#define PICASA_FEEDS_TITLE_LEN 32
#define PICASA_FEEDS_LOGO_LEN 128

typedef struct _picasa_feeds_item{
	char title[PICASA_FEEDS_TITLE_LEN];
	char logo_url[PICASA_FEEDS_LOGO_LEN];
}PICASA_FEEDS_ITEM;


static PICASA_FEEDS_ITEM s_Picasa_Feeds[MAX_PICASA_FEED];
static int s_Picasa_Feed_Count = 0;


int app_picasa_feeds_load_from_file(const char *filename)
{
	int i, len;
	FILE *fp = NULL;

	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		printf("[Picasa] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		printf("[Picasa] error, open %s failed!!!\n", filename);
		return -1;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_Picasa_Feed_Count = atoi(read_buf);
		printf("[Picasa] cur feeds count:%d\n", s_Picasa_Feed_Count);
		if(s_Picasa_Feed_Count > MAX_PICASA_FEED)
			s_Picasa_Feed_Count = MAX_PICASA_FEED;
	}
	else
	{
		s_Picasa_Feed_Count = 0;
	}

	if(s_Picasa_Feed_Count <= 0)
	{
		fclose(fp);
		return -1;
	}

	for(i=0; i<s_Picasa_Feed_Count; i++)
	{
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_Feeds[i].title, read_buf, PICASA_FEEDS_TITLE_LEN-1);
			s_Picasa_Feeds[i].title[PICASA_FEEDS_TITLE_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_Feeds[i].title[0] = '\0';
		}


		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_Feeds[i].logo_url, read_buf, PICASA_FEEDS_LOGO_LEN-1);
			s_Picasa_Feeds[i].logo_url[PICASA_FEEDS_LOGO_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_Feeds[i].logo_url[0] = '\0';
		}
	}

	fclose(fp);
	
	return 0;
}

//######################################################################

#define MAX_PICASA_LIST_ITEM 21
#define MAX_TITLE_LEN 32
#define MAX_SMALL_IMG_LEN 64
#define MAX_IMG_PATH 120

typedef struct _picasa_list_item{
	char author[MAX_TITLE_LEN]; //pet name
	char title[MAX_TITLE_LEN];
	char date[MAX_TITLE_LEN];
	char img_small[MAX_SMALL_IMG_LEN];
	char img_big[MAX_IMG_PATH];
	bool b_download_ok;
}PICASA_LIST_ITEM;

static PICASA_LIST_ITEM s_Picasa_List[MAX_PICASA_LIST_ITEM];
static int s_Picasa_List_Count = 0;

int app_picasa_piclist_load_from_file(const char *filename)
{
	int i, len, total_pag;
	FILE *fp = NULL;
	
	if(GXCORE_FILE_UNEXIST == GxCore_FileExists(filename))
	{
		s_Picasa_List_Count = 0;
		printf("[Picasa] file [%s] unexist!!!\n", filename);
		return -2;
	}

	fp = fopen(filename, "r");
	if(NULL == fp)
	{
		s_Picasa_List_Count = 0;
		printf("[Picasa] error, open %s failed!!!\n", filename);
		return -1;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		//s_total_page = atoi(read_buf);
		total_pag = atoi(read_buf);
	}
	else
	{
		//s_total_page = 0;
		total_pag = 0;
	}

	len = MAX_READ_BUF;
	fgets_own(read_buf, &len, fp);
	if(len > 0)
	{
		s_Picasa_List_Count = atoi(read_buf);
		if(s_Picasa_List_Count > MAX_PICASA_LIST_ITEM)
			s_Picasa_List_Count = MAX_PICASA_LIST_ITEM;
	}
	else
	{
		s_Picasa_List_Count = 0;
	}

	if(s_Picasa_List_Count <= 0)
	{
		fclose(fp);
		return -1;
	}

	if(feof(fp))
	{
		s_Picasa_List_Count = 0;
		fclose(fp);
		return -1;
	}

	s_total_page = total_pag;

	for(i = 0; i<s_Picasa_List_Count; i++)
	{
		s_Picasa_List[i].b_download_ok = false;

		//author pet name
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_List[i].author, read_buf, MAX_TITLE_LEN-1);
			s_Picasa_List[i].author[MAX_TITLE_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_List[i].author[0] = '\0';
		}
	}
	
	for(i = 0; i<s_Picasa_List_Count; i++)
	{
		//title
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_List[i].title, read_buf, MAX_TITLE_LEN-1);
			s_Picasa_List[i].title[MAX_TITLE_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_List[i].title[0] = '\0';
		}
	}

	for(i = 0; i<s_Picasa_List_Count; i++)
	{
		//date
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_List[i].date, read_buf, MAX_TITLE_LEN-1);
			s_Picasa_List[i].date[MAX_TITLE_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_List[i].date[0] = '\0';
		}
	}

	for(i = 0; i<s_Picasa_List_Count; i++)
	{
		//small img
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_List[i].img_small, read_buf, MAX_SMALL_IMG_LEN-1);
			s_Picasa_List[i].img_small[MAX_SMALL_IMG_LEN-1] = '\0';
		}
		else
		{
			s_Picasa_List[i].img_small[0] = '\0';
		}
	}

	for(i = 0; i<s_Picasa_List_Count; i++)
	{
		//big img
		len = MAX_READ_BUF;
		fgets_own(read_buf, &len, fp);
		if(len > 0)
		{
			memcpy(s_Picasa_List[i].img_big, read_buf, MAX_IMG_PATH-1);
			s_Picasa_List[i].img_big[MAX_IMG_PATH-1] = '\0';
		}
		else
		{
			s_Picasa_List[i].img_big[0] = '\0';
		}
	}

	fclose(fp);
	
	return 0;
}



//######################################################################

//######################################################################


//######################################################################
extern int picasa_update_cur_feeds_pic_page(uint32_t page_index);


//-------------------------------------------------------------------------
#define PICASA_GIF
void app_picasa_draw_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_gif", "draw_gif", &alu);
}

void app_picasa_hide_gif(void)
{
	GUI_SetProperty("img_gif", "state", "hide");
	//GUI_SetInterface("flush", NULL);
}

void app_picasa_show_gif(void)
{
	GUI_SetProperty("img_gif", "state", "show");
	//GUI_SetInterface("flush", NULL);
}

void app_picasa_load_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_gif", "load_img", GIF_PATH);
	GUI_SetProperty("img_gif", "init_gif_alu_mode", &alu);
}

void app_picasa_free_gif(void)
{
	GUI_SetProperty("img_gif", "load_img", NULL);
}


void picasa_draw_process_gif_proc(void* userdata)
{
	app_picasa_draw_gif();
}


//-------------------------------------------------------------------------
static event_list* sp_PicasaPopupTimer = NULL;

void hide_popup_msg_picasa(void)
{
	if(sp_PicasaPopupTimer)
	{
		remove_timer(sp_PicasaPopupTimer);
		sp_PicasaPopupTimer = NULL;
	}
	
	GUI_SetProperty("img_picasa_popup", "state", "hide");
	GUI_SetProperty("txt_picasa_popup", "state", "hide");
}

static int picasa_popup_timer_timeout(void *userdata)
{
	hide_popup_msg_picasa();
	return 0;
}

void show_popup_msg_picasa(char* pMsg, int time_out)
{
	GUI_SetProperty("txt_picasa_popup", "string", pMsg);
	
	GUI_SetProperty("img_picasa_popup", "state", "show");
	GUI_SetProperty("txt_picasa_popup", "state", "show");

	if(time_out > 0)
	{
		if (reset_timer(sp_PicasaPopupTimer) != 0)
		{
			sp_PicasaPopupTimer = create_timer(picasa_popup_timer_timeout, 
				time_out, NULL, TIMER_ONCE);
		}
	}
}



//------------------------------------------
#define PICASA_PLAY_ITEM
	
void app_picasa_unfocus_item(uint32_t sel)
{
	char buffer1[32];
	
	sprintf(buffer1, "img_focus_pic_%02d", sel);
	GUI_SetProperty(buffer1, "state", "hide");
}

void app_picasa_focus_item(uint32_t sel)
{
	char buffer1[32];

	sprintf(buffer1, "img_focus_pic_%02d", sel);
	GUI_SetProperty(buffer1, "state", "show");
}

void app_picasa_disable_item(uint32_t sel)
{
	char buffer1[32];
	
	sprintf(buffer1, "img_test_pic_%02d", sel);
	GUI_SetProperty(buffer1, "state", "hide");

	sprintf(buffer1, "img_focus_pic_%02d", sel);
	GUI_SetProperty(buffer1, "state", "hide");
}

void app_picasa_enable_item(uint32_t sel)
{
	char buffer1[32];

	if(GXCORE_FILE_UNEXIST != GxCore_FileExists(s_Picasa_List[sel].img_small))
	{
		sprintf(buffer1, "img_test_pic_%02d", sel);
		GUI_SetProperty(buffer1, "load_scal_img", s_Picasa_List[sel].img_small);
		GUI_SetProperty(buffer1, "state", "show");

		s_Picasa_List[sel].b_download_ok = true;
	}
}

void app_picasa_update_page_gui(void)
{
	char buffer1[32];

	if(s_total_page > 0)
	{
		sprintf(buffer1, "[ %d / %d ]", (s_cur_page>0?s_cur_page:1), s_total_page);
		GUI_SetProperty("text_page", "string", buffer1);
	}

	#if 0
	if(s_total_page <= 1)
	{
		GUI_SetProperty("img_page_left", "state", "hide");
		GUI_SetProperty("img_page_right", "state", "hide");
	}
	else
	{
		if(s_cur_page >= s_total_page)
		{
			GUI_SetProperty("img_page_left", "state", "show");
			GUI_SetProperty("img_page_right", "state", "hide");
		}
		else if(s_cur_page <= 1)
		{
			GUI_SetProperty("img_page_left", "state", "hide");
			GUI_SetProperty("img_page_right", "state", "show");
		}
		else
		{
			GUI_SetProperty("img_page_left", "state", "show");
			GUI_SetProperty("img_page_right", "state", "show");
		}
	}
	#endif
}

void app_picasa_update_page(void)
{
	int i;

	hide_popup_msg_picasa();

	app_picasa_update_page_gui();
	
	if(s_cur_page > 1)
		picasa_update_cur_feeds_pic_page(s_cur_page);
	else
		picasa_update_cur_feeds_pic_page(1);

	for(i=1; i<=21; i++)
	{
		app_picasa_disable_item(i);
	}
}

//-------------------------------------------------------------------------
#define PICASA_PIC_LIST
static event_list* sp_PicUpdateTimer = NULL;

static int picasa_pic_update_timeout(void *userdata)
{
	int i;
	char buffer1[32];

	bool b_all_ok = true;

	for(i = 1; i<=s_Picasa_List_Count; i++)
	{	
		if(s_Picasa_List[i-1].b_download_ok)
			continue;

		b_all_ok = false;

		//picasa_draw_process_gif_proc(NULL);

		if(GXCORE_FILE_UNEXIST != GxCore_FileExists(s_Picasa_List[i-1].img_small))
		{
			//printf("##########s_Picasa_List_Count=%d,  i=%d\n", s_Picasa_List_Count, i);
			
			if((!s_is_list_focus) && (i == s_cur_sel))
				app_picasa_focus_item(s_cur_sel);
			
			sprintf(buffer1, "img_test_pic_%02d", i);
			GUI_SetProperty(buffer1, "load_scal_img", s_Picasa_List[i-1].img_small);
			GUI_SetProperty(buffer1, "state", "show");

			//printf("#####small_img[%s] load ok...\n", s_Picasa_List[i-1].img_small);

			s_Picasa_List[i-1].b_download_ok = true;
		}
	}
	
	if(b_all_ok)
	{
		app_picasa_hide_gif();
		
		timer_stop(sp_PicUpdateTimer);
	}
	else
	{
		picasa_draw_process_gif_proc(NULL);
	}
	
	//GUI_SetProperty("wnd_picasa", "update", NULL);
	return 0;
}


void app_pic_update_timer_stop(void)
{
	//timer_stop(sp_PicUpdateTimer);
	remove_timer(sp_PicUpdateTimer);
	sp_PicUpdateTimer = NULL;
}

void app_pic_update_timer_reset(void)
{
	if (reset_timer(sp_PicUpdateTimer) != 0)
	{
		sp_PicUpdateTimer = create_timer(picasa_pic_update_timeout, 500, NULL, TIMER_REPEAT);
	}
}


static bool s_is_update = false;
bool is_picasa_page_updating(void)
{
	return s_is_update;
}

void set_picasa_page_update_state(bool bstate)
{
	s_is_update = bstate;
}

void piclist_parse_ok(void* userdata)
{
	int i;
	char buffer1[32];
	
	app_picasa_piclist_load_from_file(PIC_LIST_RESULT_PATH);
	
	printf("[Picasa] cur s_Picasa_List_Count:%d\n", s_Picasa_List_Count);

	app_picasa_update_page_gui();

	if(s_Picasa_List_Count > 0)
	{
		s_total = (s_total_page-1)*21 + s_Picasa_List_Count;

		#if 0
		if(0 != s_cur_page)
		{
			s_is_list_focus = false;
			
			list_sel = -1;
			GUI_SetProperty("list_feeds", "active", &list_sel);
			GUI_SetProperty("list_feeds", "select", &s_cur_feeds);
		}
		#endif

		app_pic_update_timer_reset();
	}
	else
	{
		#if 0
		for(i = 1; i<=21; i++)
		{
			picasa_draw_process_gif_proc(NULL);

			if((!s_is_list_focus) && (s_cur_page > 1))
				app_picasa_focus_item(s_cur_sel);
				
			sprintf(buffer1, "img_test_pic_%02d", i);
			GUI_SetProperty(buffer1, "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
			GUI_SetProperty(buffer1, "state", "show");
		}
		#endif

		app_picasa_hide_gif();
	
		show_popup_msg_picasa(STR_ID_SERVER_FAIL, 3000);
	}

	if(s_cur_page < s_total_page)
	{
		for(i = 21; i>s_Picasa_List_Count; i--)
		{
			//picasa_draw_process_gif_proc(NULL);

			if((!s_is_list_focus) && (s_cur_page > 1))
				app_picasa_focus_item(s_cur_sel);
				
			sprintf(buffer1, "img_test_pic_%02d", i);
			GUI_SetProperty(buffer1, "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
			GUI_SetProperty(buffer1, "state", "show");
		}
	}

	set_picasa_page_update_state(false);

	GUI_SetProperty("wnd_picasa", "update_all", NULL);
}


int picasa_update_cur_feeds_pic_page(uint32_t page_index)
{
	set_picasa_page_update_state(true);

	app_pic_update_timer_stop();
		
	app_picasa_show_gif();
		
	s_Picasa_List_Count = 0;

	system_shell("photo_share_exit", 0, picasa_draw_process_gif_proc, NULL, NULL);

	sprintf(read_buf, "photo_share_get_list %s en %s %d_21", PIC_LIST_RESULT_PATH,
		s_Picasa_Feeds[s_cur_feeds].title, page_index);

	#if 0
	system_shell(read_buf, 0, picasa_draw_process_gif_proc, piclist_parse_ok, NULL);
	#else
	system_shell(read_buf, 65000, picasa_draw_process_gif_proc, piclist_parse_ok, NULL);
	#endif

	return 0;
}


#define PICASA_FEEDS_AUTO_UPDATE
void picasa_show_logo_img(void);

static event_list* sp_PicasaFeedsUpdateTimer = NULL;

static int picasa_feeds_update_timer_timeout(void *userdata)
{
	remove_timer(sp_PicasaFeedsUpdateTimer);
	sp_PicasaFeedsUpdateTimer = NULL;

	if(s_Picasa_Feed_Count > 0)
	{
		GUI_GetProperty("list_feeds", "select", &s_cur_feeds);

		picasa_show_logo_img();
	
		//picasa_update_cur_feeds_playlist(PAGE_FIRST);
		//picasa_update_cur_feeds_pic_page(1);

		s_cur_page = 1;
		app_picasa_update_page();
		
		s_cur_sel = 1;
		//app_picasa_focus_item(s_cur_sel);
	}
	
	return 0;
}


void app_picasa_feeds_update_timer_stop(void)
{
	if(sp_PicasaFeedsUpdateTimer != NULL)
	{
		//timer_stop(sp_PicasaFeedsUpdateTimer);
		remove_timer(sp_PicasaFeedsUpdateTimer);
		sp_PicasaFeedsUpdateTimer = NULL;
	}
}

void app_picasa_feeds_update_timer_reset(void)
{
	if (reset_timer(sp_PicasaFeedsUpdateTimer) != 0)
	{
		sp_PicasaFeedsUpdateTimer = create_timer(picasa_feeds_update_timer_timeout, 1000, NULL, TIMER_ONCE);
	}
}


//-------------------------------------------------------------------------
#define PICASA_PLAYLIST_LOAD

void picasa_show_logo_img(void)
{
	if(NULL != s_Picasa_Feeds[s_cur_feeds].logo_url || 
		GXCORE_FILE_UNEXIST != GxCore_FileExists(s_Picasa_Feeds[s_cur_feeds].logo_url))
	{
		GUI_SetProperty("img_picasa_logo", "load_zoom_img", 
			s_Picasa_Feeds[s_cur_feeds].logo_url);
	}
}
	
void picasa_feeds_parse_ok(void* userdata)
{
	app_picasa_feeds_load_from_file(PIC_FEEDS_RESULT_PATH);

	GUI_SetProperty("list_feeds", "active", &s_cur_feeds);
	GUI_SetProperty("list_feeds", "select", &s_cur_feeds);

	GUI_SetProperty("list_feeds", "update_all", NULL);

	if(s_Picasa_Feed_Count>0)
	{
		picasa_show_logo_img();
		
		//picasa_update_cur_feeds_pic_page(1);
		
		s_cur_page = 1;
		
		s_cur_sel = 1;
		//app_picasa_focus_item(s_cur_sel);

		//app_picasa_update_page();
	}
	else
	{
		app_picasa_hide_gif();
		
		show_popup_msg_picasa(STR_ID_SERVER_FAIL, 0);
	}

	set_picasa_page_update_state(false);
}

void app_picasa_parse_feeds_xml(void)
{
	char cmd_buf[128];

	app_picasa_show_gif();
	
	sprintf(cmd_buf, "photo_share_get_list %s en", PIC_FEEDS_RESULT_PATH);
	system_shell(cmd_buf, 20000, picasa_draw_process_gif_proc, picasa_feeds_parse_ok, NULL);
}

SIGNAL_HANDLER int app_picasa_create(GuiWidget *widget, void *usrdata)
{
	s_is_list_focus = true;
	s_cur_sel = 0;
	s_cur_page = 0;
	s_total_page = 0;
	s_cur_feeds = 0;

	s_Picasa_Feed_Count = 0;
	s_Picasa_List_Count = 0;

	set_picasa_page_update_state(true);

	app_picasa_load_gif();

	//GUI_SetProperty("img_test_gif", "load_img", "/home/liuhy/theme/test/mm.jpg");
	//GUI_SetProperty("img_test_gif", "draw_gif", NULL);

	app_picasa_parse_feeds_xml();
	
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_destroy(GuiWidget *widget, void *usrdata)
{
	app_picasa_feeds_update_timer_stop();
	
	system_shell_clean();

	hide_popup_msg_picasa();
	
	app_pic_update_timer_stop();
	
	app_picasa_free_gif();

	system_shell("photo_share_exit", 0, NULL, NULL, NULL);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_keypress(GuiWidget *widget, void *usrdata)
{
    int ret = EVENT_TRANSFER_STOP;
    GUI_Event *event = NULL;

    event = (GUI_Event *)usrdata;
    switch(event->type)
    {
        case GUI_SERVICE_MSG:
            break;

        case GUI_MOUSEBUTTONDOWN:
            break;

        case GUI_KEYDOWN:
        {
            switch(event->key.sym)
            {
            	case STBK_EXIT:
				{
					GUI_EndDialog("wnd_picasa");
					break;
				}
                case STBK_OK:
                    break;
				case STBK_LEFT:
				{
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_picasa_unfocus_item(s_cur_sel);
					if(1 == s_cur_sel%7)
					{
						if(1 == s_cur_page)
						{
							s_is_list_focus = true;
								
							s_cur_sel = 0;
							s_cur_page = 0;
							//app_youtube_update_page();

							GUI_GetProperty("list_feeds", "select", &list_sel);
							if(list_sel < 0)
								list_sel = 0;
							GUI_SetProperty("list_feeds", "active", &list_sel);
						}
						else
						{
							s_cur_page--;
							/*if(0 == s_cur_page%5)
							{
								picasa_update_cur_feeds_playlist(PAGE_PREV);
							}*/
							//picasa_update_cur_feeds_pic_page(s_cur_page);

							app_picasa_update_page();
							
							s_cur_sel += 6;
							//app_picasa_focus_item(s_cur_sel);
						}
					}
					else
					{
						s_cur_sel--;
						app_picasa_focus_item(s_cur_sel);
					}
					break;
				}
				case STBK_RIGHT:
				{
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_picasa_unfocus_item(s_cur_sel);
					
					if(s_cur_page >= s_total_page)
					{
						if(0 == s_cur_sel%7 || s_cur_sel >= s_total - 21*(s_cur_page-1))
						{
							#if 0
							s_cur_sel = 1;
							s_cur_page = 1;

							app_youtube_update_page();
							#endif
						}
						else
						{
							s_cur_sel++;
						}

						app_picasa_focus_item(s_cur_sel);
					}
					else
					{
						if(0 == s_cur_sel%7)
						{
							/*if(0 == s_cur_page%5)
							{
								picasa_update_cur_feeds_playlist(PAGE_NEXT);
							}*/
						
							s_cur_page++;
							
							//picasa_update_cur_feeds_pic_page(s_cur_page);
							
							app_picasa_update_page();

							s_cur_sel -= 6;
							if(s_cur_page == s_total_page)
							{
								int cur_tmp = s_total%21;
								if(s_cur_sel > cur_tmp)
									s_cur_sel = (0 == cur_tmp) ? 1 : cur_tmp;
							}

							//app_picasa_focus_item(s_cur_sel);
						}
						else
						{
							s_cur_sel++;

							app_picasa_focus_item(s_cur_sel);
						}
					}
					
					break;
				}
				case STBK_UP:
				{
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_picasa_unfocus_item(s_cur_sel);
					if(s_cur_sel>7)
					{
						s_cur_sel -= 7;
					}
					app_picasa_focus_item(s_cur_sel);
					break;
				}
				case STBK_DOWN:
				{
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_picasa_unfocus_item(s_cur_sel);
					if(s_cur_sel<=14)
					{
						if(s_cur_page < s_total_page)
							s_cur_sel += 7;
						else
						{
							if(s_cur_sel+7 <= (s_total%21 ? s_total%21 : 21))
							{
								s_cur_sel += 7;
							}
						}
					}
					app_picasa_focus_item(s_cur_sel);
					break;
				}

                case STBK_MENU:
					break;
                default:
                    break;
            }
			break;
        }
		default:
			break;
    }
    return ret;
}

#define PICASA_FEEDS_LIST
SIGNAL_HANDLER int app_picasa_list_create(GuiWidget *widget, void *usrdata)
{	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_list_change(GuiWidget *widget, void *usrdata)
{
	GUI_GetProperty("list_feeds", "select", &list_sel);
	GUI_SetProperty("list_feeds", "active", &list_sel);

	app_picasa_feeds_update_timer_reset();
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_list_get_total(GuiWidget *widget, void *usrdata)
{
	return s_Picasa_Feed_Count;
}

SIGNAL_HANDLER int app_picasa_list_get_data(GuiWidget *widget, void *usrdata)
{
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item)
		return GXCORE_ERROR;
	if(0 > item->sel || item->sel >= MAX_PICASA_FEED)
		return GXCORE_ERROR;

	if(s_Picasa_Feed_Count<=0)
		return GXCORE_ERROR;

	//col-0: choice
	item->x_offset = 0;
	item->image = NULL;
	item->string = s_Picasa_Feeds[item->sel].title;

	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int app_picasa_list_keypress(GuiWidget *widget, void *usrdata)
{
	int ret = EVENT_TRANSFER_KEEPON;
	GUI_Event *event = NULL;
	
	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_SERVICE_MSG:
			break;

		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case STBK_DOWN:
				case STBK_UP:
					if(!s_is_list_focus)
					{
						app_picasa_keypress(NULL, event);
						return EVENT_TRANSFER_STOP;
					}
					break;
				case STBK_LEFT:
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_is_list_focus)
					{
						return EVENT_TRANSFER_STOP;
					}
					break;

				case STBK_OK:
					if(s_is_list_focus)
					{
						if(s_Picasa_Feed_Count > 0)
						{
							app_picasa_feeds_update_timer_stop();
								
							GUI_GetProperty("list_feeds", "select", &s_cur_feeds);

							picasa_show_logo_img();
						
							//picasa_update_cur_feeds_playlist(PAGE_FIRST);
							//picasa_update_cur_feeds_pic_page(1);
			
							s_cur_page = 1;
							app_picasa_update_page();
							
							s_cur_sel = 1;
							//app_picasa_focus_item(s_cur_sel);
						}
					}
					else
					{
						if(is_picasa_page_updating())
							return EVENT_TRANSFER_STOP;

						if(s_cur_sel > s_Picasa_List_Count || 
							s_cur_sel < 1 ||
							NULL == s_Picasa_List[s_cur_sel-1].img_big ||
							strlen(s_Picasa_List[s_cur_sel-1].img_big) < 10)
						{
							show_popup_msg_picasa(STR_ID_SERVER_FAIL, 2000);
							return EVENT_TRANSFER_STOP;
						}

						app_pic_update_timer_stop();

						app_picasa_hide_gif();
						
						//press ok on pic item...
						//TODO...
						GUI_CreateDialog("wnd_picasa_play");
					}
					return EVENT_TRANSFER_STOP;
					break;
				case STBK_RIGHT:
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_is_list_focus)
					{
						if(s_total > 0)
						{
							//set focus on picture...
							s_is_list_focus = false;
						
							list_sel = -1;
							GUI_SetProperty("list_feeds", "active", &list_sel);

							s_cur_page = 1;
							//app_youtube_update_page();
							//app_youtube_update_timer_reset();

							s_cur_sel = 1;
							app_picasa_focus_item(s_cur_sel);
						}
						
						return EVENT_TRANSFER_STOP;
					}
					break;
				//case STBK_EXIT:
				case KEY_RECALL:
				case KEY_MENU:
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(!s_is_list_focus)
					{
						/*if(s_cur_page > 5)
						{
							picasa_update_cur_feeds_playlist(PAGE_PREV);
						}*/
						//picasa_update_cur_feeds_pic_page(1);

						s_is_list_focus = true;

						#if 0
						GUI_SetProperty("list_feeds", "active", &s_cur_feeds);
						#else
						GUI_GetProperty("list_feeds", "select", &list_sel);
						GUI_SetProperty("list_feeds", "active", &list_sel);
						#endif
						
						app_picasa_unfocus_item(s_cur_sel);
						
						s_cur_sel = 0;
						
						if(s_cur_page > 1)
						{
							s_cur_page = 0;
							app_picasa_update_page();
						}
						else
							s_cur_page = 0;

						return EVENT_TRANSFER_STOP;
					}
					break;
				case KEY_PAGE_UP:
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;

					if(!s_is_list_focus)
					{
						if(s_cur_page > 1)
						{
							s_cur_page--;
							/*if(0 == s_cur_page%5)
							{
								picasa_update_cur_feeds_playlist(PAGE_PREV);
							}*/
							//picasa_update_cur_feeds_pic_page(s_cur_page);

							app_picasa_update_page();

							//app_picasa_focus_item(s_cur_sel);
						}
					}
					return EVENT_TRANSFER_STOP;
					break;
				case KEY_PAGE_DOWN:
					if(is_picasa_page_updating())
						return EVENT_TRANSFER_STOP;
					if(!s_is_list_focus)
					{
						if(s_cur_page > 0 && s_cur_page < s_total_page)
						{
							/*if(0 == s_cur_page%5)
							{
								youtube_update_cur_feeds_playlist(PAGE_NEXT);
							}*/

							s_cur_page++;
							
							//picasa_update_cur_feeds_pic_page(s_cur_page);
							
							app_picasa_update_page();

							if(s_cur_page == s_total_page)
							{
								int cur_tmp = s_total%21;
								if(s_cur_sel > cur_tmp)
									s_cur_sel = (0 == cur_tmp) ? 1 : cur_tmp;
							}

							//app_picasa_focus_item(s_cur_sel);
						}
					}
					return EVENT_TRANSFER_STOP;
					break;
			
				default:
					break;	
			}

		default:
			break;
	}

	return ret;
}



//###############################################################################
#define WND_PLAY

#define PIC_BIG_PATH "/tmp/photo/picasa_big_img"

static event_list* sp_PicPlay_FirstIn_Timer = NULL;
static event_list* sp_PicPlayTimer = NULL;
static int si_curPlayIndex = 0;

static int picasa_pic_play_timeout(void *userdata);


//------------------------------------------------------------------------------
static bool is_big_pic_download = false;

bool is_picasa_big_pic_downloading(void)
{
	return is_big_pic_download;
}

void set_picasa_big_pic_download_state(bool bstate)
{
	is_big_pic_download = bstate;
}


//-------------------------------------------------------------------------
static event_list* sp_PicPlayPopupTimer = NULL;

void hide_popup_msg_pic_play(void)
{
	if(sp_PicPlayPopupTimer)
	{
		remove_timer(sp_PicPlayPopupTimer);
		sp_PicPlayPopupTimer = NULL;
	}
	
	GUI_SetProperty("img_pic_play_popup", "state", "hide");
	GUI_SetProperty("txt_pic_play_popup", "state", "hide");
}

static int pic_play_popup_timer_timeout(void *userdata)
{
	hide_popup_msg_pic_play();
	return 0;
}

void show_popup_msg_pic_play(char* pMsg, int time_out)
{
	GUI_SetProperty("txt_pic_play_popup", "string", pMsg);
	
	GUI_SetProperty("img_pic_play_popup", "state", "show");
	GUI_SetProperty("txt_pic_play_popup", "state", "show");

	if(time_out > 0)
	{
		if (reset_timer(sp_PicPlayPopupTimer) != 0)
		{
			sp_PicPlayPopupTimer = create_timer(pic_play_popup_timer_timeout, 
				time_out, NULL, TIMER_ONCE);
		}
	}
}


//------------------------------------------------------------------------------
static event_list* sp_GifUpdateTimer = NULL;
static int picasa_pic_play_gif_timeout(void *userdata)
{
	GUI_SetProperty("img_picasa_play", "draw_gif", NULL);
	//GUI_SetProperty("wnd_picasa_play", "update", NULL);
	//GUI_SetProperty("wnd_picasa_play", "draw_now", NULL);
	return 0;
}


void app_pic_play_gif_timer_stop(void)
{
	//timer_stop(sp_GifUpdateTimer);
	remove_timer(sp_GifUpdateTimer);
	sp_GifUpdateTimer = NULL;
}

void app_pic_play_gif_timer_reset(void)
{
	if (reset_timer(sp_GifUpdateTimer) != 0)
	{
		sp_GifUpdateTimer = create_timer(picasa_pic_play_gif_timeout, 
			100, NULL, TIMER_REPEAT);
	}
}

void picasa_big_img_ok(void* userdata)
{
	int image_type = 0;
	
	GUI_SetProperty("img_loading_gif", "state", "hide");

	if(GXCORE_FILE_UNEXIST != GxCore_FileExists(PIC_BIG_PATH))
	{
		GUI_SetProperty("img_picasa_play", "load_zoom_img", PIC_BIG_PATH);

		#if 1
		GUI_GetProperty("img_picasa_play", "img_type", &image_type);
		if(/*GIF_TYPE*/2 == image_type)
		{
			app_pic_play_gif_timer_reset();
		}
		#endif
	}
	else
	{
		GUI_SetProperty("img_picasa_play", "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
		show_popup_msg_pic_play("Get picature failed!", 0);
	}

	set_picasa_big_pic_download_state(false);
}

void picasa_big_img_auto_ok(void* userdata)
{
	picasa_big_img_ok(NULL);

	sp_PicPlayTimer = create_timer(picasa_pic_play_timeout, 5000, NULL, TIMER_ONCE);
}

void picasa_big_img_gif(void* userdata)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_loading_gif", "draw_gif", &alu);
}


void app_picasa_load_image(int img_index, bool bauto)
{
	app_pic_play_gif_timer_stop();
	hide_popup_msg_pic_play();

	if(img_index > 0 && img_index <= s_Picasa_List_Count)
	{
		set_picasa_big_pic_download_state(true);
	
		system_shell_clean();
		//system_shell("killall curl", 0, NULL, NULL, NULL);
		system_shell("killall wget", 0, NULL, NULL, NULL);

		//GUI_SetProperty("img_picasa_play", "load_zoom_img", WORK_PATH"theme/image/picbg.jpg");
		//GUI_SetProperty("wnd_picasa_play", "draw_now", NULL);
		
		GUI_SetProperty("img_loading_gif", "state", "show");

		
		printf("####Large pic index=%d\n", img_index);
	
		sprintf(read_buf, "getPicFromNetwork %s \"%s\"", PIC_BIG_PATH,
			s_Picasa_List[img_index-1].img_big);

		if(bauto)
			system_shell(read_buf, 65000, picasa_big_img_gif, picasa_big_img_auto_ok, NULL);
		else
			system_shell(read_buf, 65000, picasa_big_img_gif, picasa_big_img_ok, NULL);
	}
}


//----------------------------------------------------------------------------------
static int picasa_pic_play_timeout(void *userdata)
{
	si_curPlayIndex++;
	if(si_curPlayIndex > s_Picasa_List_Count)
		si_curPlayIndex = 1;

	remove_timer(sp_PicPlayTimer);
	
	app_picasa_load_image(si_curPlayIndex, true);
	
	return 0;
}


void app_pic_play_timer_stop(void)
{
	//timer_stop(sp_PicPlayTimer);
	remove_timer(sp_PicPlayTimer);
	sp_PicPlayTimer = NULL;
}

void app_pic_play_timer_reset(void)
{
	if (reset_timer(sp_PicPlayTimer) != 0)
	{
		sp_PicPlayTimer = create_timer(picasa_pic_play_timeout, 5000, NULL, TIMER_ONCE);
	}
}

#if 1
//##############################################

static event_list* sp_PicPlayHelpTimer = NULL;

void app_pic_play_hide_help(void)
{
	#if 0
	GUI_SetProperty("text_picasa_play_green", "state", "hide");
	GUI_SetProperty("img_picasa_play_green", "state", "hide");
	GUI_SetProperty("img_picasa_play_blue", "state", "hide");
	GUI_SetProperty("text_picasa_play_blue", "state", "hide");
	GUI_SetProperty("img_picasa_play_move", "state", "hide");
	GUI_SetProperty("text_picasa_play_move", "state", "hide");
	GUI_SetProperty("img_picasa_play_ok", "state", "hide");
	GUI_SetProperty("text_picasa_play_ok", "state", "hide");
	GUI_SetProperty("img_picasa_play_exit", "state", "hide");
	GUI_SetProperty("text_picasa_play_exit", "state", "hide");
	#else
	GUI_SetProperty("img_picasa_play_help", "state", "hide");
	#endif

	//GUI_SetProperty("wnd_picasa_play", "draw_now", NULL);
	
	//timer_stop(sp_PicPlayHelpTimer);
	remove_timer(sp_PicPlayHelpTimer);
	sp_PicPlayHelpTimer = NULL;
}

static int picasa_pic_play_help_timeout(void *userdata)
{
	app_pic_play_hide_help();
	return 0;
}

void app_pic_play_show_help(int timeout)
{
	#if 0
	GUI_SetProperty("text_picasa_play_green", "state", "show");
	GUI_SetProperty("img_picasa_play_green", "state", "show");
	GUI_SetProperty("img_picasa_play_blue", "state", "show");
	GUI_SetProperty("text_picasa_play_blue", "state", "show");
	GUI_SetProperty("img_picasa_play_move", "state", "show");
	GUI_SetProperty("text_picasa_play_move", "state", "show");
	GUI_SetProperty("img_picasa_play_ok", "state", "show");
	GUI_SetProperty("text_picasa_play_ok", "state", "show");
	GUI_SetProperty("img_picasa_play_exit", "state", "show");
	GUI_SetProperty("text_picasa_play_exit", "state", "show");
	#else
	GUI_SetProperty("img_picasa_play_help", "state", "show");
	#endif
	
	if (reset_timer(sp_PicPlayHelpTimer) != 0)
	{
		sp_PicPlayHelpTimer = create_timer(picasa_pic_play_help_timeout, timeout, NULL, TIMER_ONCE);
	}
}

//##############################################

#endif

static int picasa_pic_play_first_in_timeout(void *userdata)
{
	int alu = GX_ALU_ROP_COPY_INVERT;

	remove_timer(sp_PicPlay_FirstIn_Timer);
	sp_PicPlay_FirstIn_Timer = NULL;
	
	GUI_SetProperty("img_loading_gif", "load_img", GIF_PATH);
	GUI_SetProperty("img_loading_gif", "init_gif_alu_mode", &alu);

	si_curPlayIndex = s_cur_sel;
	app_picasa_load_image(s_cur_sel, false);
	
	return 0;
}

SIGNAL_HANDLER int app_picasa_play_create(GuiWidget *widget, void *usrdata)
{
	set_picasa_big_pic_download_state(false);

	//GUI_SetProperty("img_picasa_play", "load_zoom_img", WORK_PATH"theme/image/picbg.jpg");
		
	sp_PicPlay_FirstIn_Timer = create_timer(picasa_pic_play_first_in_timeout, 500, NULL, TIMER_ONCE);

	app_pic_play_show_help(6000);
	
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_play_destroy(GuiWidget *widget, void *usrdata)
{
	if(sp_PicPlay_FirstIn_Timer)
	{
		remove_timer(sp_PicPlay_FirstIn_Timer);
		sp_PicPlay_FirstIn_Timer = NULL;
	}

	//system_shell("photo_share_exit", 0, NULL, NULL, NULL);

	system_shell_clean();

	hide_popup_msg_picasa();
	
	app_pic_play_timer_stop();

	GUI_SetProperty("img_loading_gif", "load_img", NULL);
	GUI_SetProperty("img_picasa_play", "load_img", NULL);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_picasa_play_keypress(GuiWidget *widget, void *usrdata)
{
    int ret = EVENT_TRANSFER_STOP;
    GUI_Event *event = NULL;

    event = (GUI_Event *)usrdata;
    switch(event->type)
    {
        case GUI_SERVICE_MSG:
            break;

        case GUI_MOUSEBUTTONDOWN:
            break;

        case GUI_KEYDOWN:
        {
            switch(find_virtualkey(event->key.sym))
            {
            	case STBK_EXIT:
					app_pic_play_gif_timer_stop();
					GUI_EndDialog("wnd_picasa_play");
					break;
				case STBK_LEFT:
					if(is_picasa_big_pic_downloading())
						return EVENT_TRANSFER_STOP;

					app_pic_play_gif_timer_stop();
					timer_stop(sp_PicPlayTimer);
					GUI_SetProperty("img_picasa_auto_icon", "state", "hide");
					
					si_curPlayIndex--;
					if(si_curPlayIndex <= 0)
						si_curPlayIndex = s_Picasa_List_Count;
					
					app_picasa_load_image(si_curPlayIndex, false);
					break;
				//case STBK_OK:
				case STBK_RIGHT:
					if(is_picasa_big_pic_downloading())
						return EVENT_TRANSFER_STOP;

					app_pic_play_gif_timer_stop();
					timer_stop(sp_PicPlayTimer);
					GUI_SetProperty("img_picasa_auto_icon", "state", "hide");
					
					si_curPlayIndex++;
					if(si_curPlayIndex > s_Picasa_List_Count)
						si_curPlayIndex = 1;
					
					app_picasa_load_image(si_curPlayIndex, false);
					break;
                case KEY_GREEN:
                //case STBK_ZOOM:
					if(is_picasa_big_pic_downloading())
						return EVENT_TRANSFER_STOP;
					
					GUI_SetProperty("img_picasa_auto_icon", "state", "show");
					app_pic_play_timer_reset();
                    break;
				case KEY_BLUE:
				//case STBK_PAUSE_STB:
					#if 0
					if(is_picasa_big_pic_downloading())
						return EVENT_TRANSFER_STOP;
					#endif
					
					GUI_SetProperty("img_picasa_auto_icon", "state", "hide");
					timer_stop(sp_PicPlayTimer);
					break;
                default:
					#if 1
					if(is_picasa_big_pic_downloading())
						return EVENT_TRANSFER_STOP;
					
					app_pic_play_show_help(3000);
					#endif
					
                    break;
            }
			break;
        }
		default:
			break;
    }
    return ret;
}
#endif


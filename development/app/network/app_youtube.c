//#include "app_root.h"
#include "app.h"
//#include "app_module.h"
//#include "app_msg.h"
//#include "app_send_msg.h"
//#include "full_screen.h"
//#include "app_pop.h"
//#include "app_book.h"
//#include "app_default_params.h"
#include "app_full_keyboard.h"
#include "app_youtube_playlist.h"
#include "youtube_tools.h"


//huangbc added string define 

#define STR_ID_SERVER_FAIL		"Server connecting failed, please retry!"

#define PLAYER_FOR_NORMAL       "player1"
#define PLAYER_FOR_PIP          "player2"
#define PLAYER_FOR_REC          "player3"
#define PLAYER_FOR_JPEG         "player4"
#define PLAYER_FOR_IPTV         "player5"




typedef enum _pag_update_type{
	PAGE_FIRST = 0,
	PAGE_NEXT,
	PAGE_PREV,
}PAGE_UPDATE_TYPE;


#define GIF_PATH WORK_PATH"theme/image/youtube/loading.gif"
#define DEFAULT_VPIC "/etc/config/iptv/logo/default.png"


static uint32_t s_total = 0;
static uint32_t s_total_page = 0;
static uint32_t s_cur_page = 0;
static uint32_t s_cur_sel = 0;
static uint32_t s_cur_feeds = 0;
static bool s_is_list_focus = true;

static int list_sel = 0;

//-----------------------------------------------------
//declare
void app_youtube_focus_item(uint32_t sel);
void app_youtube_enable_item(uint32_t sel);
extern void youtube_play_set_cur_play_item(struct youtube_play_item* pitem);




//-------------------------------------------------------------------------
#define SET_WND_TYPE
//-------------------------------------------------------------------------
//set wnd type
static WND_TYPE s_wnd_type = WND_TYPE_YOUTUBE;
void app_set_wnd_type(WND_TYPE type) /* 0:youtube  1:iptv */
{
	s_wnd_type = type;
	#if 0
	switch(s_wnd_type)
	{
		case 0:
			setenv("YOUTUBE_IPTV_TYPE", "youtube", 1);
			break;
		case 1:
			setenv("YOUTUBE_IPTV_TYPE", "iptv", 1);
			break;
		default:
			break;
	}
	#endif
}

WND_TYPE app_get_wnd_type(void)
{
	return s_wnd_type;
}


//-------------------------------------------------------------------------
static event_list* sp_YoutubePopupTimer = NULL;

void hide_popup_msg_youtube(void)
{
	if(sp_YoutubePopupTimer)
	{
		remove_timer(sp_YoutubePopupTimer);
		sp_YoutubePopupTimer = NULL;
	}
	
	GUI_SetProperty("img_youtube_popup", "state", "hide");
	GUI_SetProperty("txt_youtube_popup", "state", "hide");
}

static int youtube_popup_timer_timeout(void *userdata)
{
	hide_popup_msg_youtube();
	return 0;
}

void show_popup_msg_youtube(char* pMsg, int time_out)
{
	GUI_SetProperty("txt_youtube_popup", "string", pMsg);
	
	GUI_SetProperty("img_youtube_popup", "state", "show");
	GUI_SetProperty("txt_youtube_popup", "state", "show");

	if(time_out > 0)
	{
		if (reset_timer(sp_YoutubePopupTimer) != 0)
		{
			sp_YoutubePopupTimer = create_timer(youtube_popup_timer_timeout, time_out, NULL, TIMER_ONCE);
		}
	}
}



//-------------------------------------------------------------------------
static bool is_page_updating = false;

bool is_youtube_page_updating(void)
{
	return is_page_updating;
}

void set_youtube_page_update_state(bool bstate)
{
	is_page_updating = bstate;
}


//-------------------------------------------------------------------------
#define YOUTUBE_GIF
void app_youtube_draw_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_youtube_gif", "draw_gif", &alu);
}

void app_youtube_hide_gif(void)
{
	GUI_SetProperty("img_youtube_gif", "state", "hide");
}

void app_youtube_show_gif(void)
{
	GUI_SetProperty("img_youtube_gif", "state", "show");
}

void app_youtube_load_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_youtube_gif", "load_img", GIF_PATH);
	GUI_SetProperty("img_youtube_gif", "init_gif_alu_mode", &alu);
}

void app_youtube_free_gif(void)
{
	GUI_SetProperty("img_youtube_gif", "load_img", NULL);
}
	

void feeds_rss_download_proc(void* userdata)
{
	app_youtube_draw_gif();
}


//--------------------------------------------------------------------------
#define YOUTUBE_THUMBNAIL

static event_list* sp_YoutubeUpdateTimer = NULL;

static int youtube_update_timer_timeout(void *userdata)
{
	int i, cur_page_num;
	char buffer1[32];
	struct youtube_play_item* p_item = NULL;

	bool b_all_ok = true;

	cur_page_num = app_youtube_playlist_get_cur_page_total();

	for(i = 1; i<=cur_page_num; i++)
	{
		p_item = app_youtube_playlist_get_by_index(i);
		if(NULL == p_item)
			continue;
	
		if(p_item->b_download_ok)
			continue;

		b_all_ok = false;

		app_youtube_draw_gif();

		if(GXCORE_FILE_UNEXIST != GxCore_FileExists(p_item->thumbnail_url))
		{
			#if 1
			if( (!s_is_list_focus) && (i == s_cur_sel))
				app_youtube_focus_item(s_cur_sel);
			#endif

			printf("####cur:%d  cur_page_num:%d   img:%s\n", i, cur_page_num, p_item->thumbnail_url);
			
			sprintf(buffer1, "img_video_pic%d", i);
			GUI_SetProperty(buffer1, "load_scal_img", p_item->thumbnail_url);
			GUI_SetProperty(buffer1, "state", "show");

			p_item->b_download_ok = true;
		}
	}
	
	if(b_all_ok)
	{
		app_youtube_hide_gif();
		
		timer_stop(sp_YoutubeUpdateTimer);
	}
	
	return 0;
}


void app_youtube_update_timer_stop(void)
{
	//timer_stop(sp_YoutubeUpdateTimer);
	remove_timer(sp_YoutubeUpdateTimer);
	sp_YoutubeUpdateTimer = NULL;
}

void app_youtube_update_timer_reset(void)
{
	if (reset_timer(sp_YoutubeUpdateTimer) != 0)
	{
		sp_YoutubeUpdateTimer = create_timer(youtube_update_timer_timeout, 500, NULL, TIMER_REPEAT);
	}
}

//-------------------------------------------------------------------------
#define IPTV_DOWNLOAD_PIC
static int s_cur_page_num = 0;
static int s_iptv_pic_sel = 0;
static int s_page_index_array[6];

#define IPTV_PIC_PATH "/tmp/iptv/iptv_pic_tmp"

void app_iptv_pic_download_show(int cur_page_num);

void iptv_pic_download_ok(void* userdata)
{
	char buffer1[64];

	if(GXCORE_FILE_UNEXIST != GxCore_FileExists(IPTV_PIC_PATH))
	{	
		sprintf(buffer1, "img_video_pic%d", s_iptv_pic_sel);
		GUI_SetProperty(buffer1, "load_zoom_img", IPTV_PIC_PATH);
		GUI_SetProperty(buffer1, "state", "show");
	}
	else
	{
		sprintf(buffer1, "img_video_pic%d", s_iptv_pic_sel);
		//GUI_SetProperty(buffer1, "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
		GUI_SetProperty(buffer1, "load_zoom_img", DEFAULT_VPIC);
		GUI_SetProperty(buffer1, "state", "show");
	}

	app_iptv_pic_download_show(s_cur_page_num);
}

void app_iptv_pic_download_show(int cur_page_num)
{
	int i;
	bool bover = true;
	struct youtube_play_item* p_item = NULL;

	for(i=0; i<cur_page_num; i++)
	{
		if(s_page_index_array[i] > 0)
		{
			bover = false;
			
			p_item = app_youtube_playlist_get_by_index(s_page_index_array[i]);
			if(NULL == p_item)
				continue;

			s_iptv_pic_sel = i+1;
			s_cur_page_num = cur_page_num;
	
			sprintf(read_buf, "getPicFromNetwork %s \"%s\"", IPTV_PIC_PATH, p_item->thumbnail_url);

			system_shell(read_buf, 60000, feeds_rss_download_proc, iptv_pic_download_ok, NULL);

			s_page_index_array[i] = -1;

			return;
		}

		#if 1
		if( (!s_is_list_focus) && (i+1 == s_cur_sel))
			app_youtube_focus_item(s_cur_sel);
		#endif
	}

	if(bover)
	{
		app_youtube_hide_gif();
	}
}

//-------------------------------------------------------------------------
#define YOUTUBE_PLAY_ITEM
	
void app_youtube_unfocus_item(uint32_t sel)
{
	char buffer1[32];
	
	sprintf(buffer1, "img_video_focus_pic%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");
}

void app_youtube_focus_item(uint32_t sel)
{
	char buffer1[32];

	sprintf(buffer1, "img_video_focus_pic%d", sel);
	GUI_SetProperty(buffer1, "state", "show");
}

void app_youtube_disable_item(uint32_t sel)
{
	char buffer1[32];
	
	sprintf(buffer1, "img_video_pic%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");

	sprintf(buffer1, "img_video_focus_pic%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");

	sprintf(buffer1, "text_video_title%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");

	sprintf(buffer1, "text_video_author%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");

	sprintf(buffer1, "text_video_viewcount%d", sel);
	GUI_SetProperty(buffer1, "state", "hide");
}

void app_youtube_enable_item(uint32_t sel)
{
	struct youtube_play_item* p_item = NULL;
	char buffer1[64];
	char buffer3[64];
	char buffer4[64];
	int index = 1;

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		index = sel;
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		if(s_cur_page > 1)
			index = 6*(s_cur_page-1) + sel;
		else
			index = sel;
	}

	p_item = app_youtube_playlist_get_by_index(index);
	if(NULL == p_item)
		return;

	//Title
	sprintf(buffer1, "text_video_title%d", sel);
	GUI_SetProperty(buffer1, "state", "show");
	GUI_SetProperty(buffer1, "string", p_item->media_title);

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		//Author
		sprintf(buffer1, "text_video_author%d", sel);
		GUI_SetProperty(buffer1, "state", "show");
		sprintf(buffer3, "Author: %s", p_item->author);
		GUI_SetProperty(buffer1, "string", buffer3);

		//ViewCount
		sprintf(buffer1, "text_video_viewcount%d", sel);
		GUI_SetProperty(buffer1, "state", "show");
		sprintf(buffer4, "Viewcount: %d", p_item->viewCount);
		GUI_SetProperty(buffer1, "string", buffer4);
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		if(NULL != p_item->thumbnail_url)
		{
			if(NULL != strstr(p_item->thumbnail_url, "http://"))
			{
				s_page_index_array[sel-1] = index;

				sprintf(buffer1, "img_video_pic%d", sel);
				GUI_SetProperty(buffer1, "load_zoom_img", NULL);
			}
			else
			{
				s_page_index_array[sel-1] = -1;

				if(GXCORE_FILE_UNEXIST != GxCore_FileExists(p_item->thumbnail_url))
				{	
					sprintf(buffer1, "img_video_pic%d", sel);
					GUI_SetProperty(buffer1, "load_zoom_img", p_item->thumbnail_url);
					GUI_SetProperty(buffer1, "state", "show");
				}
				else
				{
					sprintf(buffer1, "img_video_pic%d", sel);
					GUI_SetProperty(buffer1, "load_zoom_img", DEFAULT_VPIC);
					GUI_SetProperty(buffer1, "state", "show");
				}

				if((!s_is_list_focus) && (sel == s_cur_sel))
					app_youtube_focus_item(s_cur_sel);
			}
		}
		else
		{
			s_page_index_array[sel-1] = -1;
			sprintf(buffer1, "img_video_pic%d", sel);
			GUI_SetProperty(buffer1, "load_zoom_img", DEFAULT_VPIC);
			GUI_SetProperty(buffer1, "state", "show");
		}
	}
}


void app_youtube_update_page(void)
{
	int i, cur_page_num=0;
	char buffer1[64];

	if(s_total_page > 0)
	{
		sprintf(buffer1, "[ %d / %d ]", (s_cur_page>0?s_cur_page:1), s_total_page);
		GUI_SetProperty("text_youtube_page", "string", buffer1);
	}
	else
	{
		sprintf(buffer1, "[ 0 / 0 ]");
		GUI_SetProperty("text_youtube_page", "string", buffer1);
	}

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		cur_page_num = app_youtube_playlist_get_cur_page_total();
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		if(s_cur_page >= s_total_page)
		{
			cur_page_num = s_total%6;
			if(0 == cur_page_num)
				cur_page_num = 6;
		}
		else
		{
			cur_page_num = 6;
		}	
	}		
	
	printf("[Youtube]  cur_page_num:%d\n", cur_page_num);
	
	for(i=1; i<=cur_page_num; i++)
	{
		s_page_index_array[i-1] = -1;
		app_youtube_enable_item(i);
	}

	if(s_cur_page < s_total_page)
	{
		for(i=6; i>cur_page_num; i--)
		{
			sprintf(buffer1, "img_video_pic%d", i);
			GUI_SetProperty(buffer1, "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
			GUI_SetProperty(buffer1, "state", "show");

			//Title
			sprintf(buffer1, "text_video_title%d", i);
			GUI_SetProperty(buffer1, "state", "show");
			GUI_SetProperty(buffer1, "string", "Invalid Video");
		}
	}

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

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		app_youtube_update_timer_reset();
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		app_iptv_pic_download_show(cur_page_num);
	}
}

void app_youtube_update_page_begin(void)
{
	int i;
	char buffer1[32];

	if(s_total_page > 0)
	{
		sprintf(buffer1, "[ %d / %d ]", (s_cur_page>0?s_cur_page:1), s_total_page);
		GUI_SetProperty("text_youtube_page", "string", buffer1);
	}

	for(i=1; i<=6; i++)
	{
		app_youtube_disable_item(i);
	}

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
}


#define YOUTUBE_WND
//static event_list* sp_YoutubeFirstInTimer = NULL;
#define YOUTUBE_PLAYLIST_RESULT "/tmp/youtube/playlist_result"
#define IPTV_PLAYLIST_RESULT "/tmp/iptv/playlist_result"

void playlist_parse_ok(void* userdata)
{
	int i;
	char buffer1[64];

	//app_youtube_hide_gif();

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		app_youtube_playlist_load_from_file(YOUTUBE_PLAYLIST_RESULT);
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		app_iptv_playlist_load_from_file(IPTV_PLAYLIST_RESULT);
	}
	
	s_total = app_youtube_playlist_get_total();
	printf("[Youtube]  s_total:%d\n", s_total);

	if(app_youtube_playlist_get_cur_page_total() > 0)
	{
		s_total_page = (0 == s_total%6)? (s_total/6) : (s_total/6+1);

		app_youtube_update_page();

		//app_youtube_focus_item(s_cur_sel);

		//app_youtube_update_timer_reset();
	}
	else
	{
		app_youtube_hide_gif();

		for(i = 1; i<=6; i++)
		{
			if((!s_is_list_focus) && (i==s_cur_sel))
				app_youtube_focus_item(s_cur_sel);
				
			sprintf(buffer1, "img_video_pic%d", i);
			GUI_SetProperty(buffer1, "load_zoom_img", WORK_PATH"theme/image/youtube/fail.jpg");
			GUI_SetProperty(buffer1, "state", "show");

			//Title
			sprintf(buffer1, "text_video_title%d", i);
			GUI_SetProperty(buffer1, "state", "show");
			GUI_SetProperty(buffer1, "string", "Update failed!");
		}

		show_popup_msg_youtube(STR_ID_SERVER_FAIL, 3000);
	}

	set_youtube_page_update_state(false);
}


#define YOUTUBE_SEARCH_URL "http://gdata.youtube.com/feeds/api/videos?start-index=1&max-results=6&v=2&q="


extern char s_full_keyboard_input[BUF_LEN];
void search_full_keyboard_proc(void)
{
	
	sprintf(read_buf, "youtube_get_list %s \"%s%s\"", YOUTUBE_PLAYLIST_RESULT, YOUTUBE_SEARCH_URL, s_full_keyboard_input);

	set_youtube_page_update_state(true);

	app_youtube_show_gif();

	system_shell(read_buf, 65000, feeds_rss_download_proc, playlist_parse_ok, NULL);
}

int youtube_update_cur_feeds_playlist(PAGE_UPDATE_TYPE page_type)
{
	struct youtube_feeds_item* p_feeds_item = NULL;
	char* s_url;
	int i;

	hide_popup_msg_youtube();
	
	app_youtube_show_gif();

	switch(page_type)
	{
		case PAGE_FIRST:
			{
				if(WND_TYPE_YOUTUBE == s_wnd_type || 
					WND_TYPE_REDTUBE == s_wnd_type || 
					WND_TYPE_YOUPORN == s_wnd_type)
				{
					app_youtube_update_timer_stop();

					system_shell("youtube_exit", 0, feeds_rss_download_proc, NULL, NULL);
				}
				else if(WND_TYPE_IPTV == s_wnd_type)
				{
					system_shell("iptv_exit", 0, feeds_rss_download_proc, NULL, NULL);
				}
					
				app_youtube_playlist_cleanup();

				app_youtube_update_page_begin();
				
				if(app_youtube_feeds_get_total()>0)
				{
					p_feeds_item = app_youtube_feeds_get_by_index(s_cur_feeds);
					if(NULL == p_feeds_item)
						break;
						
					if(WND_TYPE_YOUTUBE == s_wnd_type)
					{
						if(0 == s_cur_feeds)
						{
							s_total_page = 0;
							s_total = 0;
							app_youtube_hide_gif();
							app_youtube_update_page();

							extern full_keyboard_proc s_full_keyboard_proc;
							s_full_keyboard_proc = search_full_keyboard_proc;
							GUI_CreateDialog(WND_FULL_KEYBOARD);
							
#define WIFI_KEYBOARD_HINT		"Hint:Press Num Keys or Choose in KeyBoard"
							GUI_SetProperty(TXT_FULL_KEYBOARD_HINT, "string",WIFI_KEYBOARD_HINT);
						#if 0
							static PopKeyboard keyboard;
							memset(&keyboard, 0, sizeof(PopKeyboard));
							keyboard.in_name    = NULL;
							keyboard.max_num = 127;
							keyboard.out_name   = NULL;
							keyboard.change_cb  = NULL;
							keyboard.release_cb = search_full_keyboard_proc;
							keyboard.usr_data   = NULL;
							keyboard.pos.x = 500;
							multi_language_keyboard_create(&keyboard);
						#endif

							return 0;
						}
						else
						{
							sprintf(read_buf, "youtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, p_feeds_item->p_url);
						}
					}
					else if(WND_TYPE_IPTV == s_wnd_type)
					{
						sprintf(read_buf, "iptv_get_playlist %s en \"%s\"", IPTV_PLAYLIST_RESULT, p_feeds_item->p_title);
					}
					else if(WND_TYPE_REDTUBE == s_wnd_type)
					{
						sprintf(read_buf, "redtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, p_feeds_item->p_url);
					}
					else if(WND_TYPE_YOUPORN == s_wnd_type)
					{
						sprintf(read_buf, "youporn_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, p_feeds_item->p_url);
					}

					set_youtube_page_update_state(true);

					system_shell(read_buf, 65000, feeds_rss_download_proc, playlist_parse_ok, NULL);
				}
			}
			break;
		case PAGE_NEXT:
			{
				if(WND_TYPE_YOUTUBE == s_wnd_type || 
					WND_TYPE_REDTUBE == s_wnd_type || 
					WND_TYPE_YOUPORN == s_wnd_type)
				{
					s_url = app_youtube_playlist_get_next_page_url();
					if(NULL != s_url)
					{
						app_youtube_update_timer_stop();
						
						app_youtube_update_page_begin();

						if(WND_TYPE_YOUTUBE == s_wnd_type)
						{
							sprintf(read_buf, "youtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						else if(WND_TYPE_REDTUBE == s_wnd_type)
						{
							sprintf(read_buf, "redtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						else if(WND_TYPE_YOUPORN == s_wnd_type)
						{
							sprintf(read_buf, "youporn_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						

						system_shell("youtube_exit", 0, feeds_rss_download_proc, NULL, NULL);
					
						app_youtube_playlist_cleanup();

						set_youtube_page_update_state(true);

						system_shell(read_buf, 0, feeds_rss_download_proc, playlist_parse_ok, NULL);
					}
				}
				else if(WND_TYPE_IPTV == s_wnd_type)
				{
					for(i=1; i<=6; i++)
					{
						app_youtube_disable_item(i);
					}
					
					system_shell("iptv_exit", 0, NULL, NULL, NULL);
					
					//app_youtube_hide_gif();
					
					app_youtube_update_page();

					app_youtube_focus_item(s_cur_sel);

					//app_youtube_update_timer_reset();
				}
			}
			break;
		case PAGE_PREV:
			{
				if(WND_TYPE_YOUTUBE == s_wnd_type || 
					WND_TYPE_REDTUBE == s_wnd_type || 
					WND_TYPE_YOUPORN == s_wnd_type)
				{
					s_url = app_youtube_playlist_get_prev_page_url();
					if(NULL != s_url)
					{
						app_youtube_update_timer_stop();
						
						app_youtube_update_page_begin();

						if(WND_TYPE_YOUTUBE == s_wnd_type)
						{
							sprintf(read_buf, "youtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						else if(WND_TYPE_REDTUBE == s_wnd_type)
						{
							sprintf(read_buf, "redtube_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						else if(WND_TYPE_YOUPORN == s_wnd_type)
						{
							sprintf(read_buf, "youporn_get_list %s \"%s\"", YOUTUBE_PLAYLIST_RESULT, s_url);
						}
						
						system_shell("youtube_exit", 0, feeds_rss_download_proc, NULL, NULL);
					
						app_youtube_playlist_cleanup();

						set_youtube_page_update_state(true);

						system_shell(read_buf, 0, feeds_rss_download_proc, playlist_parse_ok, NULL);
					}
				}
				else if(WND_TYPE_IPTV == s_wnd_type)
				{
					for(i=1; i<=6; i++)
					{
						app_youtube_disable_item(i);
					}
					
					system_shell("iptv_exit", 0, NULL, NULL, NULL);
					
					//app_youtube_hide_gif();
					
					app_youtube_update_page();

					app_youtube_focus_item(s_cur_sel);

					//app_youtube_update_timer_reset();
				}
			}
			break;
	}

	return 0;
}


#define YOUTUBE_FEEDS_AUTO_UPDATE
static event_list* sp_YoutubeFeedsUpdateTimer = NULL;

static int youtube_feeds_update_timer_timeout(void *userdata)
{
	remove_timer(sp_YoutubeFeedsUpdateTimer);
	sp_YoutubeFeedsUpdateTimer = NULL;
	
	if(app_youtube_feeds_get_total()>0 /*&& s_is_list_focus*/)
	{
		GUI_GetProperty("youtube_list_feeds", "select", &s_cur_feeds);
		
		//list_sel = -1;
		//GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
		//GUI_SetProperty("youtube_list_feeds", "select", &s_cur_feeds);

		s_cur_page = 1;
		s_cur_sel = 1;

		youtube_update_cur_feeds_playlist(PAGE_FIRST);
	}
	
	return 0;
}


void app_youtube_feeds_update_timer_stop(void)
{
	if(sp_YoutubeFeedsUpdateTimer != NULL)
	{
		//timer_stop(sp_YoutubeFeedsUpdateTimer);
		remove_timer(sp_YoutubeFeedsUpdateTimer);
		sp_YoutubeFeedsUpdateTimer = NULL;
	}
}

void app_youtube_feeds_update_timer_reset(void)
{
	if (reset_timer(sp_YoutubeFeedsUpdateTimer) != 0)
	{
		sp_YoutubeFeedsUpdateTimer = create_timer(youtube_feeds_update_timer_timeout, 500, NULL, TIMER_ONCE);
	}
}


//-------------------------------------------------------------------------
#define YOUTUBE_PLAYLIST_LOAD
#define FEEDS_RESULT_PATH "/tmp/youtube/youtube_feeds_result"
#define IPTV_FEEDS_RESULT_PATH "/tmp/iptv/iptv_feeds_result"
	
void feeds_rss_parse_ok(void* userdata)
{
	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		app_youtube_feeds_load_from_file(FEEDS_RESULT_PATH);
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		app_iptv_feeds_load_from_file(IPTV_FEEDS_RESULT_PATH);
	}
	
	//GUI_SetProperty("youtube_list_feeds", "active", &s_cur_feeds);
	//GUI_SetProperty("youtube_list_feeds", "select", &s_cur_feeds);

	GUI_SetProperty("youtube_list_feeds", "update_all", NULL);

	if(app_youtube_feeds_get_total()>0)
	{
		s_cur_page = 1;
		s_cur_sel = 1;

		#if 1
		GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
		GUI_SetProperty("youtube_list_feeds", "select", &s_cur_feeds);
		#endif
		
		//youtube_update_cur_feeds_playlist(PAGE_FIRST);
	}
	else
	{
		app_youtube_hide_gif();
		
		show_popup_msg_youtube(STR_ID_SERVER_FAIL, 0);
	}

	set_youtube_page_update_state(false);
}

void app_youtube_parse_feeds_xml(void)
{
	char cmd_buf[128];
	if(WND_TYPE_YOUTUBE == s_wnd_type)
	{
		sprintf(cmd_buf, "youtube_get_feeds %s en", FEEDS_RESULT_PATH);
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		sprintf(cmd_buf, "iptv_get_playlist %s en", IPTV_FEEDS_RESULT_PATH);
	}
	else if(WND_TYPE_REDTUBE == s_wnd_type)
	{
		sprintf(cmd_buf, "redtube_get_feeds %s en", FEEDS_RESULT_PATH);
	}
	else if(WND_TYPE_YOUPORN == s_wnd_type)
	{
		sprintf(cmd_buf, "youporn_get_feeds %s en", FEEDS_RESULT_PATH);
	}

	app_youtube_show_gif();
	
	system_shell(cmd_buf, 20000, feeds_rss_download_proc, feeds_rss_parse_ok, NULL);
}

SIGNAL_HANDLER int app_youtube_create(GuiWidget *widget, void *usrdata)
{
	s_is_list_focus = true;
	s_cur_sel = 0;
	s_cur_page = 0;
	s_total_page = 0;
	if(WND_TYPE_YOUTUBE == s_wnd_type)
	{
		s_cur_feeds = 1;
		list_sel = 1;
	}
	else
		s_cur_feeds = 0;

	set_youtube_page_update_state(true);

	#if 0
	int32_t s_trans = OSD_TRANS_100PERCENT;
	GUI_SetInterface("osd_alpha_global", &s_trans);
	#endif

	if(WND_TYPE_YOUTUBE == s_wnd_type)
	{
		GUI_SetProperty("text_iptv_blue", "string", "Search");
		
		GUI_SetProperty("img_iptv_blue", "state", "show");
		GUI_SetProperty("text_iptv_blue", "state", "show");
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		GUI_SetProperty("img_youtube_logo","img","IPTV.bmp");
		GUI_SetProperty("text_youtube_title", "string", "IPTV");

		//GUI_SetProperty("text_iptv_blue", "string", "Add");
		GUI_SetProperty("img_iptv_blue", "state", "show");
		GUI_SetProperty("text_iptv_blue", "state", "show");

		//GUI_SetProperty("text_iptv_yellow", "string", "Recover");
		GUI_SetProperty("img_iptv_yellow", "state", "show");
		GUI_SetProperty("text_iptv_yellow", "state", "show");
	}
	else if(WND_TYPE_REDTUBE == s_wnd_type)
	{
		//GUI_SetProperty("img_youtube_logo","img","redtubelogo.bmp");
		GUI_SetProperty("text_youtube_title", "string", "Redtube");
	}
	else if(WND_TYPE_YOUPORN == s_wnd_type)
	{
		//GUI_SetProperty("img_youtube_logo","img","redtubelogo.bmp");
		GUI_SetProperty("text_youtube_title", "string", "Youporn");
	}

	app_youtube_load_gif();

	app_youtube_parse_feeds_xml();
	
    return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_youtube_destroy(GuiWidget *widget, void *usrdata)
{
	#if 0
	int32_t s_trans = OSD_TRANS_100PERCENT;
	GxBus_ConfigGetInt(TRANS_LEVEL_KEY, &s_trans, TRANS_LEVEL);
	GUI_SetInterface("osd_alpha_global", &s_trans);
	#endif

	app_youtube_feeds_update_timer_stop();
	
	system_shell_clean();

	hide_popup_msg_youtube();
	
	app_youtube_update_timer_stop();
	
	app_youtube_playlist_cleanup();

	app_youtube_feeds_cleanup();

	app_youtube_free_gif();

	if(WND_TYPE_YOUTUBE == s_wnd_type || 
		WND_TYPE_REDTUBE == s_wnd_type || 
		WND_TYPE_YOUPORN == s_wnd_type)
	{
		system_shell("youtube_exit", 0, NULL, NULL, NULL);
	}
	else if(WND_TYPE_IPTV == s_wnd_type)
	{
		system_shell("iptv_exit", 0, NULL, NULL, NULL);
	}
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_youtube_keypress(GuiWidget *widget, void *usrdata)
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
					GUI_EndDialog("wnd_youtube");
					printf("end youtube...\n");
					break;
				}
                case STBK_OK:
                    break;
				case STBK_LEFT:
				{
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_youtube_unfocus_item(s_cur_sel);
					if(1 == s_cur_sel%3)
					{
						if(1 == s_cur_page)
						{
							s_is_list_focus = true;
							
							s_cur_sel = 0;
							s_cur_page = 0;
							//app_youtube_update_page();

							GUI_GetProperty("youtube_list_feeds", "select", &list_sel);
							if(list_sel < 0)
								list_sel = 0;
							GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
						}
						else
						{
							s_cur_page--;

							//app_youtube_update_page();
							
							s_cur_sel += 2;
							//app_youtube_focus_item(s_cur_sel);

							youtube_update_cur_feeds_playlist(PAGE_PREV);
						}
					}
					else
					{
						s_cur_sel--;
						app_youtube_focus_item(s_cur_sel);
					}
					break;
				}
				case STBK_RIGHT:
				{
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_youtube_unfocus_item(s_cur_sel);
					
					if(s_cur_page >= s_total_page)
					{
						if(0 == s_cur_sel%3 || s_cur_sel >= s_total - 6*(s_cur_page-1))
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
						app_youtube_focus_item(s_cur_sel);
					}
					else
					{
						if(0 == s_cur_sel%3)
						{
							s_cur_page++;
							
							//app_youtube_update_page();

							s_cur_sel -= 2;
							if(s_cur_page == s_total_page)
							{
								int cur_tmp = s_total%6;
								if(s_cur_sel > cur_tmp)
									s_cur_sel = (0 == cur_tmp) ? 1 : cur_tmp;
							}

							//app_youtube_focus_item(s_cur_sel);

							youtube_update_cur_feeds_playlist(PAGE_NEXT);
						}
						else
						{
							s_cur_sel++;
							app_youtube_focus_item(s_cur_sel);
						}
					}
					
					break;
				}
				case STBK_UP:
				{
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_youtube_unfocus_item(s_cur_sel);
					if(s_cur_sel>3)
					{
						s_cur_sel -= 3;
					}
					app_youtube_focus_item(s_cur_sel);
					break;
				}
				case STBK_DOWN:
				{
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_total<=0)
						break;
					
					app_youtube_unfocus_item(s_cur_sel);
					if(s_cur_sel<=3)
					{
						if(s_cur_page < s_total_page)
							s_cur_sel += 3;
						else
						{
							if(s_cur_sel+3 <= (s_total%6 ? s_total%6 : 6))
							{
								s_cur_sel += 3;
							}
						}
					}
					app_youtube_focus_item(s_cur_sel);
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

#define YOUTUBE_FEEDS_LIST
SIGNAL_HANDLER int app_feeds_list_create(GuiWidget *widget, void *usrdata)
{
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_feeds_list_change(GuiWidget *widget, void *usrdata)
{
	GUI_GetProperty("youtube_list_feeds", "select", &list_sel);
	GUI_SetProperty("youtube_list_feeds", "active", &list_sel);

	app_youtube_feeds_update_timer_reset();
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_feeds_list_get_total(GuiWidget *widget, void *usrdata)
{
	return app_youtube_feeds_get_total();
}

SIGNAL_HANDLER int app_feeds_list_get_data(GuiWidget *widget, void *usrdata)
{
	ListItemPara* item = NULL;
	static struct youtube_feeds_item* sp_feeds_item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item)
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	sp_feeds_item = app_youtube_feeds_get_by_index(item->sel);
	if(NULL == sp_feeds_item)
		return GXCORE_ERROR;

	//col-0: choice
	item->x_offset = 0;
	item->image = NULL;
	item->string = sp_feeds_item->p_title;

	return EVENT_TRANSFER_STOP;
}


//#include "app_wnd_file_list.h"

typedef enum
{
	IPTV_UPDATE_TYPE_ADD = 0,
	IPTV_UPDATE_TYPE_RECOVER
}IPTV_UPDATE_TYPE;

#define IPTV_PLAYLIST_CSV "/etc/config/iptv/iptv_playlist_en.csv"
static char *s_file_path = NULL;
static char *s_file_path_bak = NULL;
void app_iptv_update_playlist_from_usb(IPTV_UPDATE_TYPE up_type)
{
	int str_len = 0;
	WndStatus get_path_ret = WND_CANCLE;
	
	if(check_usb_status() == false)
	{
	#if 0
		PopDlg  pop;
		memset(&pop, 0, sizeof(PopDlg));
    		pop.type = POP_TYPE_OK;
   		pop.str = STR_ID_NO_DEVICE;
   		pop.mode = POP_MODE_UNBLOCK;
		popdlg_create(&pop);
	#endif
		app_popmsg(340,200,"No device or not support!!",POPMSG_TYPE_OK);
	}
	else
	{
		FileListParam file_para;
		memset(&file_para, 0, sizeof(file_para));

		file_para.cur_path = s_file_path_bak;
		file_para.dest_path = &s_file_path;
		file_para.suffix = "csv";
		file_para.dest_mode = DEST_MODE_FILE;
		
		get_path_ret = app_get_file_path_dlg(&file_para);
		
		if(get_path_ret == WND_OK)
		{
			if(s_file_path != NULL)
			{
				////backup the path...////
				if(s_file_path_bak != NULL)
				{
					free(s_file_path_bak);
					s_file_path_bak = NULL;
				}
			
				str_len = strlen(s_file_path);
				s_file_path_bak = (char *)malloc(str_len + 1);
				if(s_file_path_bak != NULL)
				{
					memcpy(s_file_path_bak, s_file_path, str_len);
					s_file_path_bak[str_len] = '\0';
				}


				//TODO...
				if(IPTV_UPDATE_TYPE_ADD == up_type)
				{
					if(GXCORE_FILE_UNEXIST == GxCore_FileExists(s_file_path))
					{
						sprintf(read_buf, "cp -f %s %s", s_file_path, IPTV_PLAYLIST_CSV);
					}
					else
					{
						sprintf(read_buf, "cat %s | sed '1d' >> %s", s_file_path, IPTV_PLAYLIST_CSV);
					}
				}
				else if(IPTV_UPDATE_TYPE_RECOVER == up_type)
				{
					sprintf(read_buf, "cp -f %s %s", s_file_path, IPTV_PLAYLIST_CSV);
				}

				system_shell(read_buf, 0, NULL, NULL, NULL);

				#if 0
				GUI_EndDialog("wnd_youtube");
				GUI_SetInterface("flush",NULL);
				GUI_CreateDialog("wnd_youtube");
				GUI_SetInterface("flush",NULL);
				#else
				hide_popup_msg_youtube();
				app_youtube_update_timer_stop();
				app_youtube_playlist_cleanup();
				app_youtube_feeds_cleanup();
				system_shell("iptv_exit", 0, NULL, NULL, NULL);

				s_is_list_focus = true;
				s_cur_sel = 0;
				s_cur_page = 0;
				s_total_page = 0;
				if(WND_TYPE_YOUTUBE == s_wnd_type)
					s_cur_feeds = 1;
				else
					s_cur_feeds = 0;
				app_youtube_parse_feeds_xml();

				app_youtube_feeds_update_timer_reset();
				
				#endif
			}
		}
	}
}

SIGNAL_HANDLER int app_feeds_list_keypress(GuiWidget *widget, void *usrdata)
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
			switch(find_virtualkey(event->key.sym))
			{
				case STBK_DOWN:
				case STBK_UP:
					if(!s_is_list_focus)
					{
						app_youtube_keypress(NULL, event);
						return EVENT_TRANSFER_STOP;
					}
					break;
				case STBK_LEFT:
					if(s_is_list_focus)
					{
						return EVENT_TRANSFER_STOP;
					}
					break;

				case STBK_OK:
					if(s_is_list_focus)
					{
						#if 1
						if(is_youtube_page_updating())
							return EVENT_TRANSFER_STOP;
						
						if(app_youtube_feeds_get_total()>0)
						{
							app_youtube_feeds_update_timer_stop();
							
							GUI_GetProperty("youtube_list_feeds", "select", &s_cur_feeds);
							
							//list_sel = -1;
							//GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
							//GUI_SetProperty("youtube_list_feeds", "select", &s_cur_feeds);

							s_cur_page = 1;
							s_cur_sel = 1;

							youtube_update_cur_feeds_playlist(PAGE_FIRST);
						}
						#endif
					}
					else
					{
						extern GxMsgProperty_PlayerPlay g_youtube_playParm;
						
						//GxMsgProperty_PlayerPlay play = {0};
						struct youtube_play_item* p_play_item = NULL;
						int index = 0;

						if(is_youtube_page_updating())
							return EVENT_TRANSFER_STOP;

						if(WND_TYPE_YOUTUBE == s_wnd_type || 
							WND_TYPE_REDTUBE == s_wnd_type || 
							WND_TYPE_YOUPORN == s_wnd_type)
						{
							index = s_cur_sel;

							p_play_item = app_youtube_playlist_get_by_index(index);
							if(NULL != p_play_item)
							{
								if(0 == get_youtube_flv_url(g_youtube_playParm.url, p_play_item->media_url, s_wnd_type))
								{
									system_shell_clean();
									app_youtube_hide_gif();
									
									printf("[Youtube] flv actual url:------------------>\n%s\n\n",g_youtube_playParm.url);
									
									g_youtube_playParm.player = PLAYER_FOR_IPTV;
									//app_send_msg_exec(GXMSG_PLAYER_PLAY, (void *)(&g_youtube_playParm));

									youtube_play_set_cur_play_item(p_play_item);
									GUI_CreateDialog("wnd_youtube_play");
								}
							}
						}
						else if(WND_TYPE_IPTV == s_wnd_type)
						{
							index = 6*(s_cur_page-1) + s_cur_sel;

							p_play_item = app_youtube_playlist_get_by_index(index);
							if(NULL != p_play_item)
							{
								system_shell_clean();
								app_youtube_hide_gif();
								
								sprintf(g_youtube_playParm.url, "%s", p_play_item->media_url);
								printf("[IPTV] flv actual url:------------------>\n%s\n\n", g_youtube_playParm.url);
								
								g_youtube_playParm.player = PLAYER_FOR_IPTV;
								//app_send_msg_exec(GXMSG_PLAYER_PLAY, (void *)(&g_youtube_playParm));

								youtube_play_set_cur_play_item(p_play_item);
								GUI_CreateDialog("wnd_youtube_play");
							}
						}
						
						
					}
					return EVENT_TRANSFER_STOP;
					break;
				case STBK_RIGHT:
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(s_is_list_focus)
					{
						if(s_total > 0)
						{
							//set focus on picture...
							s_is_list_focus = false;
							
							list_sel = -1;
							GUI_SetProperty("youtube_list_feeds", "active", &list_sel);

							s_cur_page = 1;

							s_cur_sel = 1;
							app_youtube_focus_item(s_cur_sel);
						}
						
						return EVENT_TRANSFER_STOP;
					}
					break;
				//case STBK_EXIT:
				case KEY_RECALL:
				case KEY_MENU:
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;
					
					if(!s_is_list_focus)
					{
						app_youtube_unfocus_item(s_cur_sel);

						s_is_list_focus = true;

						GUI_GetProperty("youtube_list_feeds", "select", &list_sel);
						GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
						
						s_cur_sel = 0;
						
						if(s_cur_page > 1)
						{
							s_cur_page = 0;

							if(WND_TYPE_YOUTUBE == s_wnd_type)
							{
								if(0 != s_cur_feeds)
								{
									youtube_update_cur_feeds_playlist(PAGE_FIRST);
								}
							}
							else if(WND_TYPE_REDTUBE == s_wnd_type)
							{
								youtube_update_cur_feeds_playlist(PAGE_FIRST);
							}
							else if(WND_TYPE_YOUPORN == s_wnd_type)
							{
								youtube_update_cur_feeds_playlist(PAGE_FIRST);
							}
							else if(WND_TYPE_IPTV == s_wnd_type)
							{
								app_youtube_update_page();

								//app_youtube_focus_item(s_cur_sel);

								//app_youtube_update_timer_reset();
							}
						}
						else
						{
							s_cur_page = 0;
						}

						return EVENT_TRANSFER_STOP;
					}
					break;
				case KEY_PAGE_UP:
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;

					if(!s_is_list_focus)
					{
						if(s_cur_page > 1)
						{
							s_cur_page--;
							youtube_update_cur_feeds_playlist(PAGE_PREV);

							//app_youtube_update_page();

							//app_youtube_focus_item(s_cur_sel);
						}
					}
					return EVENT_TRANSFER_STOP;
					break;
				case KEY_PAGE_DOWN:
					if(is_youtube_page_updating())
						return EVENT_TRANSFER_STOP;

					if(!s_is_list_focus)
					{
						if(s_cur_page > 0 && s_cur_page < s_total_page)
						{
							s_cur_page++;

							youtube_update_cur_feeds_playlist(PAGE_NEXT);
							
							//app_youtube_update_page();

							if(s_cur_page == s_total_page)
							{
								int cur_tmp = s_total%6;
								if(s_cur_sel > cur_tmp)
									s_cur_sel = (0 == cur_tmp) ? 1 : cur_tmp;
							}

							//app_youtube_focus_item(s_cur_sel);
						}
					}
					
					return EVENT_TRANSFER_STOP;
					break;

				
				//case STBK_PAUSE_STB:
				case KEY_BLUE:
					{
						if(is_youtube_page_updating())
							return EVENT_TRANSFER_STOP;

						if(WND_TYPE_IPTV == s_wnd_type)
						{
							app_iptv_update_playlist_from_usb(IPTV_UPDATE_TYPE_ADD);
						}
						else if(WND_TYPE_YOUTUBE == s_wnd_type)
						{
							s_cur_feeds = 0;
							list_sel = 0;
							s_cur_page = 0;
							s_cur_sel = 0;

							GUI_SetProperty("youtube_list_feeds", "active", &list_sel);
							GUI_SetProperty("youtube_list_feeds", "select", &s_cur_feeds);

							GUI_SetProperty("youtube_list_feeds", "update_all", NULL);

							app_youtube_feeds_update_timer_reset();
						}
						
						return EVENT_TRANSFER_STOP;
					}
					break;

				case KEY_YELLOW:
					{
						if(is_youtube_page_updating())
							return EVENT_TRANSFER_STOP;

						if(WND_TYPE_IPTV == s_wnd_type)
						{
							app_iptv_update_playlist_from_usb(IPTV_UPDATE_TYPE_RECOVER);
						}
					}
					break;
					
				default:
					break;	
			}

		default:
			break;
	}

	return ret;
}


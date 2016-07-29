#include "app.h"
#include "gdi_core.h"
#include "media_info.h"

//#if ((MINI_256_COLORS_OSD_SUPPORT == 0) || ( MINI_16_BITS_OSD_SUPPORT == 0))
//#define	GDI_SW_JPEG_SUPPORE 0
//#else
//#define	GDI_SW_JPEG_SUPPORE 0
//#endif

//resourse
#if( MINI_16_BITS_OSD_SUPPORT == 1)
#define IMG_BUTTON1_PLAY_FOCUS		"s_mpicon_play_focus.bmp"
#define IMG_BUTTON1_PLAY_UNFOCUS		"s_mpicon_play_unfocus.bmp"
#define IMG_BUTTON1_PAUSE_FOCUS		"s_mpicon_pause_focus.bmp"
#define IMG_BUTTON1_PAUSE_UNFOCUS	"s_mpicon_pause_unfocus.bmp"
#else
#define IMG_BUTTON1_PLAY_FOCUS		"MP_ICON_PLAY_YELLOW.bmp"
#define IMG_BUTTON1_PLAY_UNFOCUS		"MP_ICON_PLAY.bmp"
#define IMG_BUTTON1_PAUSE_FOCUS		"MP_ICON_PAUSE_YELLOW.bmp"
#define IMG_BUTTON1_PAUSE_UNFOCUS	"MP_ICON_PAUSE.bmp"
#endif

#define IMG_POP_PREVIOUS				"MP_BUTTON_FRONT.bmp"
#define IMG_POP_PLAY					"MP_BUTTON_PLAY.bmp"
#define IMG_POP_NEXT					"MP_BUTTON_NEXT.bmp"
#define IMG_POP_PAUSE					"MP_BUTTON_PAUSE.bmp"
#define IMG_POP_ROTATE_0				""
#define IMG_POP_ROTATE_90				""
#define IMG_POP_ROTATE_180				""
#define IMG_POP_ROTATE_270				""
#define IMG_POP_ZOOM_1				    ""
#define IMG_POP_ZOOM_2				    ""
#define IMG_POP_ZOOM_4				    ""
#define IMG_POP_ZOOM_8				    ""
#define IMG_POP_ZOOM_16				    ""
#define IMG_WARNING_WAIT			   "MP_WAIT.bmp"
#define IMG_WARNING_ERROR              "MP_ERROR.bmp"

//widget
#define WIN_PIC_VIEW             "win_pic_view"
#define WIN_MUSIC_VIEW              			"win_music_view"
#define WIN_VOLUME						"win_volume"
#define TEXT_PIC_NAME1					"pic_view_text_name1"
#define IMAGE_POPMSG					"pic_view_image_popmsg"
#define IMAGE_BOX_BACK					"pic_view_image_boxback"
#if( MINI_16_BITS_OSD_SUPPORT == 1)
#define IMAGE_PIC_TITLE_BACK				"img_pic_view_back"
#endif
#define BOX_PIC_CTROL					"pic_view_box"
#define BUTTON_PLAY_STOP				"pic_view_boxitem1_button"
#define WARMING_PIC                     "pic_view_image_status"

#ifdef MINI_16BIT_WIN8_OSD_SUPPORT
#define PIC_LINE0                       "img_pic_view_lineup"
#define PIC_LINE1                       "img_pic_view_line"
#endif
static event_list *pic_box_hide_timer = NULL;
static event_list* pic_popmsg_topright_timer = NULL;
static play_pic_ctrol_state pic_box_button1_ctrol = PLAY_PIC_CTROL_PLAY;
static play_pic_rotate_state pic_box_button4_rotate = PLAY_PIC_ROTATE_0;
static play_pic_zoom_state pic_box_button5_zoom = PLAY_PIC_ZOOM_X1;

extern bool music_play_state(void);
extern void music_status_init(void);
static status_t popmsg_topright_ctrl(play_pic_ctrol_state button, uint32_t value);
static status_t pic_box_setinfo(play_pic_info* info);

static int popmsg_topright_hide(void* usrdata)
{
	char* focus_win = NULL;
	focus_win = (char*)GUI_GetFocusWindow();
	if(NULL == focus_win) 
		return 0;
	if(strcasecmp(focus_win, WIN_PIC_VIEW) == 0)
	{
		GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
	}
	else
	{
		GUI_SetProperty(IMAGE_POPMSG, "state", "osd_trans_hide");
	}
	remove_timer(pic_popmsg_topright_timer);
	pic_popmsg_topright_timer = NULL;
	return 0;
}

/***********************************************************

     in order to modify waiting pic hide result in setting interface flicker 
     
***********************************************************/

int waitshow_Active=0;

void set_waitShow_flag(void)
{
	waitshow_Active=1;
}

void clear_waitShow_flag(void)
{
	waitshow_Active=0;
}

void recover_waitshow(void)
{
	if(waitshow_Active==1)
	{
		GUI_SetProperty(WARMING_PIC, "state", "show");
	}
}
void hide_waitshow(void)
{
	GUI_SetProperty(WARMING_PIC, "state", "hide");
}

/***********************************************************

     in order to modify waiting pic hide result in setting interface flicker 
     
***********************************************************/

static int service_status_report(GxMsgProperty_PlayerStatusReport* player_status)
{
	APP_CHECK_P(player_status, 1);	
	switch(player_status->status)
	{
		case PLAYER_STATUS_PLAY_END:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_END\n");

			//view pic, player music in background
			if((0 == strcmp(player_status->player, PMP_PLAYER_AV))
					&& (music_play_state() == true))
			{
				//sequence
				pmpset_music_play_sequence sequence = 0;
				sequence = pmpset_get_int(PMPSET_MUSIC_PLAY_SEQUENCE);
				if(PMPSET_MUSIC_PLAY_SEQUENCE_ONLY_ONCE == sequence)
				{
					APP_Printf("play only once\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
					music_status_init();
					break;
				}
				else if(PMPSET_MUSIC_PLAY_SEQUENCE_REPEAT_ONE == sequence)
				{
					APP_Printf("play repeat one\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_PLAY);
					break;
				}
				else if(PMPSET_MUSIC_PLAY_SEQUENCE_SEQUENCE == sequence)
				{
					play_list* list = NULL;
					list = play_list_get(PLAY_LIST_TYPE_MUSIC);	
					if(list->play_no >= list->nents -1)
					{
						play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
						music_status_init();
					}
					else
					{
						play_music_ctrol(PLAY_MUSIC_CTROL_NEXT);
					}

					break;
				}
				/*else if(PMPSET_MUSIC_PLAY_SEQUENCE_RANDOM == sequence)
				  {
				  APP_Printf("play random\n");
				  play_music_ctrol(PLAY_MUSIC_CTROL_RANDOM);
				  break;
				  }*/
				else
				{
					//APP_Printf("play next\n");
					play_music_ctrol(PLAY_MUSIC_CTROL_STOP);
					music_status_init();
					break;
				}						
			}
			break;

		default:
			break;
	}

	return 0;
}

int pic_status_report(PicPlayStatus player_status)
{
	static play_pic_ctrol_state pic_state = PLAY_PIC_CTROL_PAUSE;
	play_pic_info info;

	switch(player_status)
	{
		case PIC_PLAY_START:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_START\n");

			play_pic_get_info(&info);
			pic_box_setinfo(&info);
			pic_state = play_pic_get_state();
			play_pic_ctrol(PLAY_PIC_CTROL_PAUSE);
			GUI_SetInterface("clear_image", NULL);
			GUI_SetProperty(WARMING_PIC, "img", IMG_WARNING_WAIT);
			set_waitShow_flag();
			GUI_SetProperty(WARMING_PIC, "state", "show");
			GUI_SetInterface("flush", NULL);
			break;

		case PIC_PLAY_ERROR:
			APP_Printf("[SERVICE] PLAYER_STATUS_ERROR\n");
			GUI_SetInterface("clear_image", NULL);

			GUI_SetProperty(WARMING_PIC, "img", IMG_WARNING_ERROR);
			GUI_SetProperty(WARMING_PIC, "state", "show");
			GUI_SetInterface("flush", NULL);
			set_waitShow_flag();

			play_list* list = NULL;
			list = play_list_get(PLAY_LIST_TYPE_PIC);
			if(list->play_no >= list->nents -1)
			{
				APP_Printf("play pause\n");
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PLAY;
				pic_box_ctrol(PIC_BOX_BUTTON_PAUSE);
			}
			else
			{
				play_pic_ctrol(PLAY_PIC_CTROL_PLAY);
				popmsg_topright_ctrl(PLAY_PIC_CTROL_NEXT, 0);
#if 0
				if(PLAY_PIC_CTROL_PREVIOUS == pic_state)
				{
					pic_box_ctrol(PIC_BOX_BUTTON_PREVIOUS);
					//play_pic_ctrol(PLAY_PIC_CTROL_PREVIOUS);
				}
				else	
				{
					APP_Printf("play next\n");
					pic_box_ctrol(PIC_BOX_BUTTON_NEXT);
					//play_pic_ctrol(PLAY_PIC_CTROL_NEXT);
				}
#endif
			}
			break;

			//GxCore_ThreadDelay(2000);
			//mp3 error, auto play next

		case PIC_PLAY_RUNNING:
			APP_Printf("[SERVICE] PLAYER_STATUS_PLAY_RUNNING\n");
			if(PLAY_PIC_CTROL_PLAY == pic_box_button1_ctrol)
			{
				play_pic_ctrol(PLAY_PIC_CTROL_PLAY);
			}
			clear_waitShow_flag();
			GUI_SetProperty(WARMING_PIC, "state", "hide");
			GUI_SetInterface("flush", NULL);
			break;

		default:
			break;
	}

	return 0;
}

static status_t popmsg_topright_ctrl(play_pic_ctrol_state button, uint32_t value)
{
	int timeout = 5000;

	if(pic_popmsg_topright_timer)
	{	
		printf("[PIC] pic_popmsg_topright_timer remove it\n");
		remove_timer(pic_popmsg_topright_timer);
		pic_popmsg_topright_timer = NULL;
	}	

	GUI_SetProperty(IMAGE_POPMSG, "state", "show");

	switch(button)
	{
		case PLAY_PIC_CTROL_PREVIOUS:
			timeout = 1500;
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PREVIOUS);
			break;

		case PLAY_PIC_CTROL_PLAY:
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PLAY);
			break;

		case PLAY_PIC_CTROL_PAUSE:
			timeout = 0;
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_PAUSE);
			break;

		case PLAY_PIC_CTROL_NEXT:
			timeout = 1500;
			GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_NEXT);
			break;


		case PLAY_PIC_CTROL_STOP:
			//timeout = 0;
			//GUI_SetProperty(IMAGE_POPMSG, "img", "mp_popmsg_stop.bmp");
			break;

		case  PLAY_PIC_CTROL_ROTATE:
			if(PLAY_PIC_ROTATE_0 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ROTATE_0);
				break;
			}

			timeout = 0;
			if(PLAY_PIC_ROTATE_90== value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ROTATE_90);
			}
			else if(PLAY_PIC_ROTATE_180 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ROTATE_180);
			}
			else if(PLAY_PIC_ROTATE_270 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ROTATE_270);
			}
			break;		

		case  PLAY_PIC_CTROL_ZOOM:
			if(PLAY_PIC_ZOOM_X1 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_1);
				break;
			}

			timeout = 0;
			if(PLAY_PIC_ZOOM_X2== value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_2);
			}
			else if(PLAY_PIC_ZOOM_X4 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_4);
			}
			else if(PLAY_PIC_ZOOM_X8 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_8);
			}
			else if(PLAY_PIC_ZOOM_X16 == value)
			{
				GUI_SetProperty(IMAGE_POPMSG, "img", IMG_POP_ZOOM_16);
			}
			break;	

		default:
			break;
	}

	if(timeout)
	{
		if(pic_popmsg_topright_timer)
		{	
			printf("[PIC] pic_popmsg_topright_timer is already created\n");
			return GXCORE_ERROR;
		}

		pic_popmsg_topright_timer = create_timer(popmsg_topright_hide, timeout, 0, TIMER_ONCE);
	}
	return GXCORE_SUCCESS;
}

//显示文件名信息
static status_t pic_box_setinfo(play_pic_info* info)
{
	if(NULL == info)return GXCORE_ERROR;

	GUI_SetProperty(TEXT_PIC_NAME1, "string", info->name);	
	
	return GXCORE_SUCCESS;
}


static status_t pic_box_button1_set_img(play_pic_ctrol_state ctrol)
{
	if(PLAY_PIC_CTROL_PAUSE == ctrol)
	{
		GUI_SetProperty(BUTTON_PLAY_STOP, "unfocus_img", IMG_BUTTON1_PLAY_UNFOCUS);
		GUI_SetProperty(BUTTON_PLAY_STOP, "focus_img", IMG_BUTTON1_PLAY_FOCUS);
	}
	else if(PLAY_PIC_CTROL_PLAY == ctrol)
	{
		GUI_SetProperty(BUTTON_PLAY_STOP, "unfocus_img", IMG_BUTTON1_PAUSE_UNFOCUS);
		GUI_SetProperty(BUTTON_PLAY_STOP, "focus_img", IMG_BUTTON1_PAUSE_FOCUS);
	}

	GUI_SetProperty(BOX_PIC_CTROL, "update", NULL);

	return GXCORE_SUCCESS;
}


static int pic_box_hide(void* usrdata)
{
	// add in pic_view_set_destroy
	GUI_SetProperty(IMAGE_BOX_BACK, "state", "hide");
#if( MINI_16_BITS_OSD_SUPPORT == 1)
	GUI_SetProperty(IMAGE_PIC_TITLE_BACK, "state", "hide");
#endif
#ifdef MINI_16BIT_WIN8_OSD_SUPPORT
	GUI_SetProperty(PIC_LINE0, "state", "hide");
	GUI_SetProperty(PIC_LINE1, "state", "hide");
#endif
	GUI_SetProperty(TEXT_PIC_NAME1, "state", "hide");
	GUI_SetProperty(BOX_PIC_CTROL, "state", "hide");
	pic_box_hide_timer = NULL;
	return 0;
}


static status_t pic_box_timer_start(void)
{
	if(pic_box_hide_timer)
	{	
		reset_timer(pic_box_hide_timer);	
	}
	else
	{
		pic_box_hide_timer = create_timer(pic_box_hide, 5000, 0, TIMER_ONCE);
	}
	return GXCORE_SUCCESS;
}

static status_t pic_box_timer_restart(void)
{
	reset_timer(pic_box_hide_timer);	
	return GXCORE_SUCCESS;
}

static status_t pic_box_timer_stop(void)
{
	if(pic_box_hide_timer)
	{
		remove_timer(pic_box_hide_timer);
		pic_box_hide_timer = NULL;
	}
	return GXCORE_SUCCESS;
}

static int pic_box_show(void* usrdata)
{
	play_pic_info info;

	/*set info*/
	play_pic_get_info(&info);
	pic_box_setinfo(&info);

	/*pause, when box show*/
	pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
	play_pic_ctrol(pic_box_button1_ctrol);
	popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
	pic_box_button1_set_img(pic_box_button1_ctrol);

	/*timer start*/
	pic_box_timer_start();

	/*show*/
	GUI_SetProperty(IMAGE_BOX_BACK, "state", "show");
	#if( MINI_16_BITS_OSD_SUPPORT == 1)
	GUI_SetProperty(IMAGE_PIC_TITLE_BACK, "state", "show");
	#endif
#ifdef MINI_16BIT_WIN8_OSD_SUPPORT
	GUI_SetProperty(PIC_LINE0, "state", "show");
	GUI_SetProperty(PIC_LINE1, "state", "show");
#endif
	GUI_SetProperty(TEXT_PIC_NAME1, "state", "show");
	GUI_SetProperty(BOX_PIC_CTROL, "state", "show");

	int item_sel = PIC_BOX_BUTTON_PLAY_PAUSE;
	GUI_SetProperty(BOX_PIC_CTROL, "select", (void*)&item_sel);
	GUI_SetFocusWidget(BOX_PIC_CTROL);
	
	return 0;
}

static void pic_view_info_get(MediaInfo *info)
{
	static play_pic_info pic_info;	
	static char str_res[20] = {0};
	static char str_len[20] = {0};
	static char str_time[20] = {0};
	memset(str_res , 0, sizeof(str_res));
	memset(str_len , 0, sizeof(str_len));
	memset(str_time, 0, sizeof(str_time));
	memset(&pic_info, 0, sizeof(play_pic_info));

	play_pic_get_info(&pic_info);

	info->line[0].subt = "Resolution:";
	if(pic_info.exif.width&&pic_info.exif.height)	//alpha8 fixed
	{
		sprintf(str_res, "%d x %d", pic_info.exif.width,pic_info.exif.height);	//alpha8 fixed
		info->line[0].info = str_res;
	}
	info->line[1].subt = "Date time:";
	if(pic_info.exif.image_ext_info.DateTime)
		info->line[1].info = pic_info.exif.image_ext_info.DateTime;

	info->line[2].subt = "Camera make:";
	if(pic_info.exif.image_ext_info.CameraMake)	
		info->line[2].info = pic_info.exif.image_ext_info.CameraMake;

	info->line[3].subt = "Camera model:";
	if(pic_info.exif.image_ext_info.CameraModel)	
		info->line[3].info = pic_info.exif.image_ext_info.CameraModel;

	info->line[4].subt = "Focal length:";
	if(pic_info.exif.image_ext_info.FocalLength)
	{
		sprintf(str_len, "%f",pic_info.exif.image_ext_info.FocalLength);	
		info->line[4].info = str_len;
	}

	info->line[5].subt = "Exposure time:";
	if(pic_info.exif.image_ext_info.ExposureTime)	
	{
		sprintf(str_time, "%f", pic_info.exif.image_ext_info.ExposureTime);	
		info->line[5].info = str_time;
	}
}

status_t pic_box_ctrol(pic_box_button button)
{
	play_pic_info info;
	
	switch(button)
	{
		case PIC_BOX_BUTTON_PREVIOUS:
			popmsg_topright_ctrl(PLAY_PIC_CTROL_PREVIOUS, 0);
			play_pic_ctrol(PLAY_PIC_CTROL_PREVIOUS);
			play_pic_get_info(&info);
			pic_box_setinfo(&info);
			break;
			
		case PIC_BOX_BUTTON_PLAY_PAUSE:
			if(PLAY_PIC_ROTATE_0 != pic_box_button4_rotate)
			{
				pic_box_button4_rotate = PLAY_PIC_ROTATE_0;
				play_pic_rotate(pic_box_button4_rotate);
			}
			if(PLAY_PIC_ZOOM_X1 != pic_box_button5_zoom)
			{
				pic_box_button5_zoom = PLAY_PIC_ZOOM_X1;
				play_pic_zoom(pic_box_button5_zoom);
			}
			
			if(PLAY_PIC_CTROL_PLAY == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
			}
			else if(PLAY_PIC_CTROL_PAUSE == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PLAY;
			}
			pic_box_button1_set_img(pic_box_button1_ctrol);
			play_pic_ctrol(pic_box_button1_ctrol);
			popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
			break;

		case PIC_BOX_BUTTON_PAUSE:
			if(PLAY_PIC_CTROL_PLAY == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
			}
			else if(PLAY_PIC_CTROL_PAUSE == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PLAY;
			}
			else
			{
				break;
			}
			
			if(PLAY_PIC_ROTATE_0 != pic_box_button4_rotate)
			{
				pic_box_button4_rotate = PLAY_PIC_ROTATE_0;
				play_pic_rotate(pic_box_button4_rotate);
			}
			if(PLAY_PIC_ZOOM_X1 != pic_box_button5_zoom)
			{
				pic_box_button5_zoom = PLAY_PIC_ZOOM_X1;
				play_pic_zoom(pic_box_button5_zoom);
			}
			
			pic_box_button1_set_img(pic_box_button1_ctrol);
			play_pic_ctrol(pic_box_button1_ctrol);
			popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
			break;	

		case PIC_BOX_BUTTON_PLAY:
			if(PLAY_PIC_CTROL_PAUSE == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PLAY;
			}
			else
			{
				break;
			}
			
			if(PLAY_PIC_ROTATE_0 != pic_box_button4_rotate)
			{
				pic_box_button4_rotate = PLAY_PIC_ROTATE_0;
				play_pic_rotate(pic_box_button4_rotate);
			}
			if(PLAY_PIC_ZOOM_X1 != pic_box_button5_zoom)
			{
				pic_box_button5_zoom = PLAY_PIC_ZOOM_X1;
				play_pic_zoom(pic_box_button5_zoom);
			}
			
			pic_box_button1_set_img(pic_box_button1_ctrol);
			play_pic_ctrol(pic_box_button1_ctrol);
			popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
			break;		
			
		case PIC_BOX_BUTTON_NEXT:
			popmsg_topright_ctrl(PLAY_PIC_CTROL_NEXT, 0);
			play_pic_ctrol(PLAY_PIC_CTROL_NEXT);
			play_pic_get_info(&info);
			pic_box_setinfo(&info);
			break;
			
		case PIC_BOX_BUTTON_STOP:
			break;

		case PIC_BOX_BUTTON_ROTATE:
			if(PLAY_PIC_CTROL_PLAY == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
				pic_box_button1_set_img(pic_box_button1_ctrol);
				play_pic_ctrol(pic_box_button1_ctrol);
			}
			
			if(PLAY_PIC_ROTATE_270 <= pic_box_button4_rotate)
			{
				pic_box_button4_rotate = PLAY_PIC_ROTATE_0;
			}
			else 
			{
				pic_box_button4_rotate++;
			}
			play_pic_rotate(pic_box_button4_rotate);
			popmsg_topright_ctrl(PLAY_PIC_CTROL_ROTATE, pic_box_button4_rotate);
			break;
			
		case PIC_BOX_BUTTON_ZOOM:
			if(PLAY_PIC_CTROL_PLAY == pic_box_button1_ctrol)
			{
				pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
				pic_box_button1_set_img(pic_box_button1_ctrol);
				play_pic_ctrol(pic_box_button1_ctrol);
			}
			
			if(PLAY_PIC_ZOOM_X16 <= pic_box_button5_zoom)
			{
				pic_box_button5_zoom = PLAY_PIC_ZOOM_X1;
			}
			else
			{
				pic_box_button5_zoom++;
			}
			play_pic_zoom(pic_box_button5_zoom);
			popmsg_topright_ctrl(PLAY_PIC_CTROL_ZOOM, pic_box_button5_zoom);
			break;
			
		case PIC_BOX_BUTTON_SET:
			//pic_box_timer_stop();
			pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
			pic_box_button1_set_img(pic_box_button1_ctrol);
			play_pic_ctrol(pic_box_button1_ctrol);
			popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
			
			GUI_CreateDialog("win_pic_set");
			break;
			
		case PIC_BOX_BUTTON_INFO:
			//pic_box_timer_stop();
			pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
			pic_box_button1_set_img(pic_box_button1_ctrol);
			play_pic_ctrol(pic_box_button1_ctrol);
			popmsg_topright_ctrl(pic_box_button1_ctrol, 0);
			
			MediaInfo info;
			memset(&info, 0, sizeof(MediaInfo));
			pic_view_info_get(&info);
			info.change_cb = pic_view_info_get;
			info.destroy_cb = NULL;
			media_info_create(&info);
			break;

		default:
			break;
	}

	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int pic_view_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t item_sel = 0;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			pic_box_timer_restart();

			switch(event->key.sym)
			{
				case APPK_BACK:
				case APPK_MENU:
					pic_box_timer_stop();
					pic_box_hide(NULL);
					return EVENT_TRANSFER_STOP;
					
				case APPK_OK:
					GUI_GetProperty(BOX_PIC_CTROL, "select", (void*)&item_sel);
					if(PIC_BOX_BUTTON_STOP == item_sel)
					{
						//event->key.sym = APPK_BACK;
						GUI_EndDialog(WIN_PIC_VIEW);
						return EVENT_TRANSFER_STOP;
					}
					else
					{
						pic_box_ctrol(item_sel);
						return EVENT_TRANSFER_STOP;
					}
					break;
					
				default:
					break;
			}
			break;
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}
/*
static int pic_view_info_init(const char* widgetname, void *usrdata)
{
	play_pic_info info;
	
	play_pic_get_info(&info);
	GUI_SetProperty(TEXT_PIC_NAME, "string", info.name);
	return 0;
}


static int pic_view_info_destroy(const char* widgetname, void *usrdata)
{
	pic_box_hide(NULL);
	return 0;
}
*/

SIGNAL_HANDLER  int pic_view_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	GxMessage * msg;
	GxMsgProperty_PlayerStatusReport* player_status = NULL;

	if(NULL == usrdata) return EVENT_TRANSFER_STOP;
	
	event = (GUI_Event *)usrdata;
	msg = (GxMessage*)event->msg.service_msg;
	switch(msg->msg_id)
	{
		case GXMSG_PLAYER_STATUS_REPORT:
			player_status = (GxMsgProperty_PlayerStatusReport*)GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_PlayerStatusReport);
			service_status_report(player_status);
			break;

		default:
			break;
	}
	
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int pic_view_init(const char* widgetname, void *usrdata)
{
	play_list* list = NULL;

	//#if (GDI_SW_JPEG_SUPPORE == 0) 
	GDI_RegisterJPEG();
    //#endif
	/*play*/

	list = play_list_get(PLAY_LIST_TYPE_PIC);
	if(NULL == list) return GXCORE_ERROR;	
	pic_box_show(NULL);

	GUI_SetInterface("clear_image", NULL);
	GUI_SetInterface("image_enable", NULL);
	hide_waitshow();
	play_pic(list->play_no);

	return 0;
}

SIGNAL_HANDLER int pic_view_destroy(const char* widgetname, void *usrdata)
{
		/*play stop*/

	GDI_StopImage((char*)"*.bmp");
	GDI_StopImage((char*)"*.jpeg");
	//GUI_SetProperty(widgetname, "draw_now", NULL);
	play_pic_ctrol(PLAY_PIC_CTROL_STOP);

	/*popmsg topright*/
	if(pic_popmsg_topright_timer)
	{	
		remove_timer(pic_popmsg_topright_timer);
		pic_popmsg_topright_timer = NULL;
	}	

	/*box*/
	pic_box_timer_stop();//first
	pic_box_hide(NULL);

	#if 0
	hd_img_clear();
	hd_commit_blit();
	gdi_image_enable(0);
	#else
	GUI_SetInterface("clear_image", NULL);
	GUI_SetInterface("image_disable", NULL);
	#endif

	//#if(GDI_SW_JPEG_SUPPORE == 0) 
    GDI_UnRegisterJPEG();
    //#endif
#ifdef	MENCENT_FREEE_SPACE
 	GUI_SetInterface("free_space", "fragment|back_osd");//
#endif


	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int pic_view_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int box_sel;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(find_virtualkey_ex(event->key.scancode,event->key.sym))
	{
/*		case VK_BOOK_TRIGGER:
			GUI_EndDialog("after win_full_screen");
			return EVENT_TRANSFER_STOP; */
		case APPK_BACK:
		case APPK_MENU:
			GUI_EndDialog(WIN_PIC_VIEW);
			return EVENT_TRANSFER_STOP;
			
		case APPK_VOLUP:
		case APPK_VOLDOWN:
                #if MINI_16_BITS_OSD_SUPPORT
  	         GUI_CreateDialog("win_volume");
               #else
			GUI_CreateDialog(WIN_VOLUME);
			GUI_SendEvent(WIN_VOLUME, event);
		#endif	
			return EVENT_TRANSFER_STOP;
			
		case APPK_OK:
			pic_box_show(NULL);
			return EVENT_TRANSFER_STOP;

		/*shortcut keys
*/
		case APPK_UP:
		case APPK_LEFT:
		case APPK_PREVIOUS:
			box_sel = PIC_BOX_BUTTON_PREVIOUS;
			GUI_SetProperty(BOX_PIC_CTROL, "select", &box_sel);
			pic_box_ctrol(PIC_BOX_BUTTON_PREVIOUS);
			return EVENT_TRANSFER_STOP;
		case APPK_PAUSE_PLAY:
			box_sel = PIC_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_PIC_CTROL, "select", &box_sel);
			pic_box_ctrol(PIC_BOX_BUTTON_PLAY_PAUSE);
			return EVENT_TRANSFER_STOP;	
		case APPK_PAUSE:
			box_sel = PIC_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_PIC_CTROL, "select", &box_sel);
			pic_box_ctrol(PIC_BOX_BUTTON_PAUSE);
			return EVENT_TRANSFER_STOP;	
		case APPK_PLAY:
			box_sel = PIC_BOX_BUTTON_PLAY_PAUSE;
			GUI_SetProperty(BOX_PIC_CTROL, "select", &box_sel);
			pic_box_ctrol(PIC_BOX_BUTTON_PLAY);
			return EVENT_TRANSFER_STOP;		
		case APPK_DOWN:
		case APPK_RIGHT:
		case APPK_NEXT:
			box_sel = PIC_BOX_BUTTON_NEXT;
			GUI_SetProperty(BOX_PIC_CTROL, "select", &box_sel);
			pic_box_ctrol(PIC_BOX_BUTTON_NEXT);
			return EVENT_TRANSFER_STOP;
		case APPK_STOP:
			event->key.sym = APPK_BACK;
			return EVENT_TRANSFER_KEEPON;
		/*case APPK_ROTATE:
			pic_box_ctrol(PIC_BOX_BUTTON_ROTATE);
			return EVENT_TRANSFER_STOP;	
		case APPK_ZOOM:
			pic_box_ctrol(PIC_BOX_BUTTON_ZOOM);
			return EVENT_TRANSFER_STOP;	*/
		case APPK_SET:
			pic_box_ctrol(PIC_BOX_BUTTON_SET);
			return EVENT_TRANSFER_STOP;	
					
		default:
			break;
	}
	
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int pic_view_got_focus(const char* widgetname, void *usrdata)
{
	pic_box_timer_start();
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER  int pic_view_lost_focus(const char* widgetname, void *usrdata)
{
	pic_box_timer_stop();
	pic_box_button1_ctrol = PLAY_PIC_CTROL_PAUSE;
	pic_box_button1_set_img(pic_box_button1_ctrol);
	play_pic_ctrol(pic_box_button1_ctrol);
	popmsg_topright_ctrl(pic_box_button1_ctrol, 0);	
	return EVENT_TRANSFER_STOP;
}



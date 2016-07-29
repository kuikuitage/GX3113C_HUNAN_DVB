#include "gui_timer.h"
#include "app_common_porting_stb_api.h"
#include "maike_ads_porting.h"
#include "mad.h"


#ifndef uint32_t
#define uint32_t unsigned int
#endif

#define MAD_WIDGET_NAME_LEN 64

int mad_corner_is_showing = 0;
int mad_corner_need_show = 1;
static int mad_osd_is_showing = 0;

mad_file_record mad_file_record_all;
extern MadPlace mad_place[M_AD_PLACE_NUM];

event_list* mad_timer_hide_corner = NULL;
event_list* mad_timer_show_corner = NULL;

int mad_timer_hide_corner_timer(void* data);
int mad_timer_show_corner_timer(void* data);


int mad_timer_hide_corner_timer(void* data)
{
	app_mad_ad_hide_pic("win_full_screen_ad");
	printf("timer hide corner\n");
	mad_corner_is_showing = 0;
	mad_corner_need_show = 0;
	
	remove_timer(mad_timer_show_corner);
	mad_timer_show_corner = NULL;
	
	if(NULL == mad_timer_show_corner)
	{
		printf("full screen mad_timer_show_corner created\n");
		mad_timer_show_corner = create_timer(mad_timer_show_corner_timer,5000,0,TIMER_ONCE);
	}
}

int mad_timer_show_corner_timer(void* data)
{
	if((1 == GxCore_FileExists("mad_corner_0.jpg"))/* &&(0 ==strcasecmp("win_full_screen",(char*)(GUI_GetFocusWindow())))*/)
		app_mad_ad_show_pic("win_full_screen_ad",M_AD_PLACE_ID_CORNER);
	printf("timer show corner\n");
	mad_corner_is_showing = TRUE;

	remove_timer(mad_timer_hide_corner);
	mad_timer_hide_corner = NULL;

	if(NULL == mad_timer_hide_corner)
	{
		printf("full screen mad_timer_hide_corner created\n");
		mad_timer_hide_corner = create_timer(mad_timer_hide_corner_timer,5000,0,TIMER_ONCE);
	}
}


int mad_get_osd_is_showing_flag(void)
{
	return mad_osd_is_showing;
}

void mad_set_osd_is_showing_flag(int flag)
{
	mad_osd_is_showing = flag;
}


//-----------------------------logo
void app_maike_ads_logo_sleep(uint32_t ms)
{
	GxCore_ThreadDelay(ms);
	return;
}

void app_ads_show_logo(uint32_t solution)
{
	ADS_Dbg("maike ads start to show logo!!!\n");
	if(1 == GxCore_FileExists("/home/gx/boot_0.bin"))
	{
		app_play_i_frame_logo(solution,"/home/gx/boot_0.bin");
	}
	else
		ADS_Dbg("maike ads not exit!!!\n");
}
//-----------------------------logo
#if 0
static void do_osd_show(ST_DSAD_OSD_PROGRAM_SHOW_INFO * osd_info,uint8_t curIndex)
{
	int32_t i = 0;
	//char * buf = NULL;
	uint8_t index = 0;
	ST_DSAD_OSD_SHOW_INFO * ad_osd = NULL;
	printf("TRACK %s %d IN\n",__FUNCTION__,__LINE__);
	if(osd_info == NULL)
	{
		return;
	}
	if(curIndex > osd_info->ucOsdNum)
	{
		return;
	}
	for(i = 0;i < 15 ;i++)
	{
		if(osd_info->aucFlag[i])
		{
			index++;
		}
		if(index == curIndex)
		{
			ad_osd = &osd_info->stTextShow[i];
			break;
		}
	}
	if(ad_osd == NULL)
	{
		return;
	}
}


#define DS_AD_WIDGET_NAME_LEN				64
static uint8_t osd_bottom_widget_name[DS_AD_WIDGET_NAME_LEN];
static uint8_t osd_top_widget_name[DS_AD_WIDGET_NAME_LEN];

void app_mad_ad_osd_display(char * TopIconeName,char * BottomIconeName)
{
	int index = 0;
	//static ST_DSAD_OSD_PROGRAM_SHOW_INFO osd_info;
	//EN_DSAD_FUNCTION_TYPE  ret;
	ST_DSAD_PROGRAM_PARA stProgramPara;
	GxBusPmDataProg prog={0};
	uint32_t ppos = 0;
	static uint16_t lastServiceId = 0xffff;
	static uint8_t lastIndex = 0;
	static uint8_t maxIndex = 0;
	static uint8_t name1_cpyed = 0;
	static uint8_t name2_cpyed = 0;

	if((strlen(TopIconeName) < MAD_WIDGET_NAME_LEN) && !name1_cpyed)
	{
		strcpy((char *)osd_top_widget_name,TopIconeName);
		name1_cpyed = 1;
	}
	if((strlen(BottomIconeName) < MAD_WIDGET_NAME_LEN)&& !name2_cpyed)
	{
		strcpy((char *)osd_bottom_widget_name,BottomIconeName);
		name2_cpyed = 1;
	}

	app_prog_get_playing_pos_in_group(&ppos);
	GxBus_PmProgGetByPos(ppos,1,&prog);
	stProgramPara.usNetworkId = prog.original_id;
	stProgramPara.usTsId	  = prog.ts_id;
	stProgramPara.usServiceId = prog.service_id;
	do
	{
		index = get_mad_place_index_by_id(M_AD_PLACE_ID_SUBTITLE);
		maxIndex = mad_place[index].data_item_num;
		printf("maxIndex = %d\n",maxIndex);
		
		if(0 < maxIndex()&&(lastServiceId != prog.service_id))
 		{
			lastIndex = 1;
			ds_ad_rolling_set_osd_flag(FALSE);
			ADS_Dbg("maxIndex = %d  lastServiceId=0x%x \n",maxIndex,lastServiceId);
			lastServiceId = prog.service_id;
			do_osd_show(&osd_info,lastIndex);
		}
		else
		{
			maxIndex = 0;
			lastIndex = 0;
			lastServiceId = prog.service_id;
			app_mad_ad_hide_osd();
		}
		else
		{
			lastIndex++;
			if(lastIndex > maxIndex)
			{
				lastIndex = 1;
			}
			do_osd_show(&osd_info,lastIndex);
		}
	}
	while(0);
	//printf("TRACK %s %d OUT\n",__FUNCTION__,__LINE__);
}


#endif
static void app_mad_ad_rolling_osd_display(uint8_t showFlag,char* message)
{
	if (DVB_CA_ROLLING_FLAG_HIDE == showFlag)
	{
		GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
	}
	else if (DVB_CA_ROLLING_FLAG_SHOW == showFlag)
	{
		GUI_SetProperty("win_full_screen_text_roll_top","state","show");
		GUI_SetProperty("win_full_screen_text_roll_top","string", message);
		GUI_SetInterface("flush", NULL);
	}
	else if(DVB_CA_ROLLING_FLAG_RESET == showFlag)
	{
		GUI_SetProperty("win_full_screen_text_roll_top","rolling_stop","");
		GUI_SetProperty("win_full_screen_text_roll_top","reset_rolling","");
		GUI_SetProperty("win_full_screen_text_roll_top","string", message);
		GUI_SetProperty("win_full_screen_text_roll_top","state","hide");
	}
}

//-----------------------------pic
void app_mad_ad_menu_pic_display(char * IconeName,char* path)
{
	status_t sret = 0;
	sret = GUI_SetProperty((char *)IconeName,(char *)"load_zoom_img"/*"load_img"*/,path);
	sret = GUI_SetProperty(IconeName, "state", "show");
}

void app_mad_ad_show_pic(char * IconeName,int place_id)
{
	int index = get_mad_place_index_by_id(place_id);
	//printf("ad_file_record_all.mad_file_name_record[%d][0] = %s\n",index,mad_file_record_all.mad_file_name_record[index][0]);
	//printf("IconeName = %s\n",IconeName);
	switch(place_id)
	{
		case M_AD_PLACE_ID_CORNER:
			if (GxCore_FileExists("/home/gx/mad_corner_0.jpg") == 1)
			{
				status_t sret = 0;
				int x,y,w,h;
				x = 580;
				y = 248;
				w = 80;
				h = 80;
				sret = GUI_SetProperty(IconeName,(char *)"x",(void *)&x);
				sret = GUI_SetProperty(IconeName,(char *)"y",(void *)&y);
				sret = GUI_SetProperty(IconeName,(char *)"width",(void *)&w);
				sret = GUI_SetProperty(IconeName,(char *)"height",(void *)&h);
				
				//MAD_INF("ads M_AD_PLACE_ID_CORNER file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_corner_0.jpg");
				mad_corner_is_showing = 1;
			}
			else
			{
				MAD_INF("ads M_AD_PLACE_ID_CORNER file NOT exit!!!\n");
				mad_corner_is_showing = FALSE;
			}
			break;
		case M_AD_PLACE_ID_VOL_BAR:
			if (GxCore_FileExists("/home/gx/mad_vol_bar_0.jpg") == 1)
			{
				status_t sret = 0;
				int x,y,w,h;
				x = 443;
				y = 403;
				w = 245;
				h = 110;
				sret = GUI_SetProperty(IconeName,(char *)"x",(void *)&x);
				sret = GUI_SetProperty(IconeName,(char *)"y",(void *)&y);
				sret = GUI_SetProperty(IconeName,(char *)"width",(void *)&w);
				sret = GUI_SetProperty(IconeName,(char *)"height",(void *)&h);
				
				MAD_INF("ads M_AD_PLACE_ID_VOL_BAR file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_vol_bar_0.jpg");

			}
			else
				MAD_INF("ads M_AD_PLACE_ID_VOL_BAR file NOT exit!!!\n");
			break;
		case M_AD_PLACE_ID_PROG_BAR:
			if (GxCore_FileExists("home/gx/mad_prog_bar_0.jpg"))
			{
				MAD_INF("ads M_AD_PLACE_ID_PROG_BAR file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_prog_bar_0.jpg");
			}
			else
				MAD_INF("ads M_AD_PLACE_ID_PROG_BAR file NOT exit!!!\n");
			break;
		case M_AD_PLACE_ID_MAIN_MUNU:
			if (GxCore_FileExists("home/gx/mad_main_menu_0.jpg"))
			{
				MAD_INF("ads M_AD_PLACE_ID_MAIN_MUNU file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_main_menu_0.jpg");
			}
			else
				MAD_INF("ads M_AD_PLACE_ID_MAIN_MUNU file NOT exit!!!\n");
			break;
		case M_AD_PLACE_ID_PROG_LIST:
			if (GxCore_FileExists("home/gx/mad_prog_list_0.jpg"))
			{
				MAD_INF("ads M_AD_PLACE_ID_PROG_LIST file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_prog_list_0.jpg");
			}
			else
				MAD_INF("ads M_AD_PLACE_ID_PROG_LIST file NOT exit!!!\n");
			break;
		case M_AD_PLACE_ID_EPG:
			if (GxCore_FileExists("home/gx/mad_epg_0.jpg"))
			{
				
				status_t sret = 0;
				int x,y,w,h;
				x = 365;
				y = 131;
				w = 245;
				h = 110;
				sret = GUI_SetProperty(IconeName,(char *)"x",(void *)&x);
				sret = GUI_SetProperty(IconeName,(char *)"y",(void *)&y);
				sret = GUI_SetProperty(IconeName,(char *)"width",(void *)&w);
				sret = GUI_SetProperty(IconeName,(char *)"height",(void *)&h);
				MAD_INF("ads M_AD_PLACE_ID_EPG file exit!!!\n");
				app_mad_ad_menu_pic_display(IconeName,"/home/gx/mad_epg_0.jpg");
			}
			else
				MAD_INF("ads M_AD_PLACE_ID_EPG file NOT exit!!!\n");
			break;
		default:
			break;
	}
}
void app_mad_ad_hide_pic(char * IconeName)
{
	status_t sret = GUI_SetProperty(IconeName, "state", "hide");
	ADS_Dbg("l=%d,sret = %d\n",__LINE__,sret);	

}
//----------------------------------pic

//----------------------------------OSD
void app_mad_ad_show_osd()
{
	int index = 0;
	index = get_mad_place_index_by_id(M_AD_PLACE_ID_SUBTITLE);
	if (mad_osd_is_showing == 0)
	{
		printf("show osd (char*)mad_place[%d].data_items[0].data_addr = %s\n",index,(char*)mad_place[index].data_items[0].data_addr);
		if(NULL != (char*)mad_place[index].data_items[0].data_addr)
		{
			app_mad_ad_rolling_osd_display(DVB_CA_ROLLING_FLAG_SHOW,(char*)mad_place[index].data_items[0].data_addr);
			mad_osd_is_showing = 1;
		}
		else
		{
			//app_mad_ad_rolling_osd_display(DVB_CA_ROLLING_FLAG_SHOW," ");
		}
	}
}
void app_mad_ad_hide_osd()
{
	app_mad_ad_rolling_osd_display(DVB_CA_ROLLING_FLAG_HIDE," ");
	mad_osd_is_showing = 0;
}
void app_mad_ad_reset_osd(int place_id)
{
	int index =0;
	if(place_id == M_AD_PLACE_ID_SUBTITLE)
	{
		index = get_mad_place_index_by_id(place_id);
		printf("show osd (char*)mad_place[%d].data_items[0].data_addr = %s\n",index,(char*)mad_place[index].data_items[0].data_addr);
		app_mad_ad_rolling_osd_display(DVB_CA_ROLLING_FLAG_RESET,(char*)mad_place[index].data_items[0].data_addr);
	}
	return;
}

//---------------------------------osd

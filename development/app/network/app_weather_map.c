#include "app.h"
//#include "app_module.h"
//#include "app_msg.h"
#include "gui_core.h"
//#include "app_default_params.h"
//#include "app_send_msg.h"
//#include "app_default_params.h"
//#include "app_full_keyboard.h"
//#include "app_pop.h"
#include "youtube_tools.h"
#include "app_full_keyboard.h"

#if defined(LINUX_OS)

//added by huangbc
typedef char ubyte64[64];
#define STR_ID_SERVER_FAIL		"Server connecting failed, please retry!"
#define WIFI_KEYBOARD_HINT		"Hint:Press Num Keys or Choose in KeyBoard"


extern void weather_map_set_area(char *file);
extern uint8_t weather_search_info_parse(void);
extern int app_weather_gif_timer(void* usrdata);
extern int map_update(float latitude, float longitude);
extern void map_img_init(void);

extern void * back_screen;
static int system_result;
static char * pf_result;
static uint8_t s_weather_map = 0;

/********************* weather params***************************/
//天气图片来自网络或者本地
#define PIC_LOCAL	
//#define PIC_NET

#define DEFAULT_STATE 			"Asia"
#define DEFAULT_COUNTRY			"China"
#define DEFAULT_PROVINCE			"Beijing"
#define DEFAULT_CITY				"Beijing"
#define DEFAULT_CODE				"CHXX0008"

#define WND_WEATHER_MAP 		"wnd_weather_map"
#define TXT_WEATHER_RED 			"text_weather_red"
#define IMG_WEATHER_RED  			"img_weather_red"
#define TXT_WEATHER_BLUE 			"text_weather_blue"
#define IMG_WEATHER_BLUE			"img_weather_blue"
#define TXT_WEATHER_GREEN 		"text_weather_green"
#define IMG_WEATHER_GREEN		"img_weather_green"
#define TXT_WEATHER_MOVE 		"text_weather_move"
#define IMG_WEATHER_MOVE 		"img_weather_move"
#define TXT_WEATHER_OK 			"text_weather_ok"
#define IMG_WEATHER_OK  			"img_weather_ok"
#define TXT_WEATHER_EXIT			"text_weather_exit"
#define IMG_WEATHER_EXIT  		"img_weather_exit"

#define WEATHER_CONF				"/home/gx/local/weather/weather_config"
#define WEATHER_AREA				"/home/gx/local/weather/weather_area"
#define WEATHER_RESULT			"/tmp/app/weather_result"
#define AREA_RESULT				"/tmp/app/area_result"

#define WEATHER_DEFAULT_PIC		"/home/tongcheng/theme/image/weather/weather_13.png"

#define AREA_CMD 					"weather_get_name_list \"/tmp/app/area_result\" \"en\" "
#define FORECAST_CMD				"weather_get_forecast"
#define SEARCH_CMD				"weather_get_name_list \"/tmp/app/area_result\" \"en\" \"search:\" "

typedef struct _temp_info{
	char day[8];
	char date[8];
	char	high[8];
	char low[8];
#ifdef PIC_LOCAL
	uint8_t  code;
#endif
#ifdef PIC_NET
	char path[128];
#endif
	struct _temp_info* next;
}TempInfo;

typedef struct _weather_info{
	char publish[64];
	char city[64];
	char latitude[8];
	char	longitude[8];
	char temp[8];
	char text[32];
	char sunrise[16];
	char sunset[16];
	char humidity[8];
	char visibility[16];
	char pressure[16];
	char wind[32];
#ifdef PIC_LOCAL
	uint8_t  cur_code;
#endif
#ifdef PIC_NET
	char path[128];
#endif
	TempInfo tempday[5];
	uint8_t  day_num;
	struct _weather_info* next;
}WeatherInfo;

typedef struct _area_info{	
	int total;	
	ubyte64 *name_list;
}AreaInfo;

typedef struct _search_info{
	uint8_t total;
	char info[20][256];
}SearchInfo;

static int s_cur_list = 3;
static event_list* s_weather_check_timer = NULL;
static uint8_t s_weather_check_time = 0;
static uint8_t s_cf = 0;
static bool s_cf_flag = false;
static bool s_weather_updating = false;
static bool s_area_update = false;
static WeatherInfo s_weather_info;
static AreaInfo s_area_info[4];
static SearchInfo s_search_info;
static int s_cur_sel[4] = {0,0,0,0};

char s_cur_area[5][64];
char s_old_area[5][64] ;
char *s_weather_area[4][2] = {
	{"button_weather_state", "list_weather0"},
	{"button_weather_country", "list_weather1"},
	{"button_weather_province", "list_weather2"},
	{"button_weather_city", "list_weather3"}
};
char *s_weather_day[5][5] = {
	{"text_weather_date0_week", "text_weather_date0_date", "text_weather_date0_temp", "img_weather_date0", "img_weather_small_ssd0"},
	{"text_weather_date1_week", "text_weather_date1_date", "text_weather_date1_temp", "img_weather_date1", "img_weather_small_ssd1"},
	{"text_weather_date2_week", "text_weather_date2_date", "text_weather_date2_temp", "img_weather_date2", "img_weather_small_ssd2"},
	{"text_weather_date3_week", "text_weather_date3_date", "text_weather_date3_temp", "img_weather_date3", "img_weather_small_ssd3"},
	{"text_weather_date4_week", "text_weather_date4_date", "text_weather_date4_temp", "img_weather_date4", "img_weather_small_ssd4"}
};
char *s_weather_cf[2][2] = {
	{"weather_deg_30.bmp","weather_deg_60.bmp"},
	{"weather_fah_30.bmp","weather_fah_60.bmp"}
};
char *s_weather_mon[12] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};
#ifdef PIC_LOCAL
char *s_weather_pic[49] = {
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_1.png",
	WORK_PATH"theme/image/weather/weather_2.png",
	WORK_PATH"theme/image/weather/weather_3.png",
	WORK_PATH"theme/image/weather/weather_4.png",
	WORK_PATH"theme/image/weather/weather_5.png",
	WORK_PATH"theme/image/weather/weather_2.png",
	WORK_PATH"theme/image/weather/weather_6.png",
	WORK_PATH"theme/image/weather/weather_6.png",
	WORK_PATH"theme/image/weather/weather_7.png",
	WORK_PATH"theme/image/weather/weather_8.png",
	WORK_PATH"theme/image/weather/weather_9.png",
	WORK_PATH"theme/image/weather/weather_8.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_2.png",
	WORK_PATH"theme/image/weather/weather_10.png",
	WORK_PATH"theme/image/weather/weather_11.png",
	WORK_PATH"theme/image/weather/weather_11.png",
	WORK_PATH"theme/image/weather/weather_11.png",
	WORK_PATH"theme/image/weather/weather_12.png",
	WORK_PATH"theme/image/weather/weather_12.png",
	WORK_PATH"theme/image/weather/weather_13.png",
	WORK_PATH"theme/image/weather/weather_14.png",
	WORK_PATH"theme/image/weather/weather_15.png",
	WORK_PATH"theme/image/weather/weather_16.png",
	WORK_PATH"theme/image/weather/weather_15.png",
	WORK_PATH"theme/image/weather/weather_16.png",
	WORK_PATH"theme/image/weather/weather_17.png",
	WORK_PATH"theme/image/weather/weather_18.png",
	WORK_PATH"theme/image/weather/weather_19.png",
	WORK_PATH"theme/image/weather/weather_20.png",
	WORK_PATH"theme/image/weather/weather_0.png",
	WORK_PATH"theme/image/weather/weather_18.png",
	WORK_PATH"theme/image/weather/weather_21.png",
	WORK_PATH"theme/image/weather/weather_21.png",
	WORK_PATH"theme/image/weather/weather_22.png",
	WORK_PATH"theme/image/weather/weather_6.png",
	WORK_PATH"theme/image/weather/weather_23.png",
	WORK_PATH"theme/image/weather/weather_24.png",
	WORK_PATH"theme/image/weather/weather_24.png",
	WORK_PATH"theme/image/weather/weather_13.png",
	WORK_PATH"theme/image/weather/weather_25.png",
	WORK_PATH"theme/image/weather/weather_26.png",
	WORK_PATH"theme/image/weather/weather_27.png",
	WORK_PATH"theme/image/weather/weather_13.png",
};
#endif
/********************* end ***************************/

/********************* map params***************************/
#define DEFAULT_LONGITUDE			116.28
#define DEFAULT_LATITUDE			39.93

#define MAP_SIZE					"580 303"
#define MAP_PIC 					"/tmp/app/map.jpg"
#define MAP_AREA					"/home/gx/local/map/map_area"
#define MAP_CONF					"/home/gx/local/map/map_config"

char * s_map_button[3] = {
	"button_map_lan",
	"button_map_type",
	"button_map_start",
};
char *s_map_type_data[4] = {
	"Roadmap",
	"Satellite",
	"Terrain",
	"Hybrid"
};
char *s_map_lan_data[6][2] = {
	{"ENGLISH", "en"},
	{"CHINESE", "zh"},
	{"ARARIC", "ar"},
	{"RUSSIAN", "ru"},
	{"JAPANESE", "ja"},
	{"KOREAN", "ko"},
};
int s_map_zoom = 9;
int s_map_type = 0;
int s_map_lan = 0;
float s_cur_latitude, s_center_latitude, s_temp_latitude; 
float s_cur_longitude, s_center_longitude, s_temp_longitude;
char *s_map_arrow[4] = 
{
	"img_map_up", 
	"img_map_down", 
	"img_map_left", 
	"img_map_right", 
};
const float s_map_step_lr[21] = {
	0,
	180,
	90,
	45,
	20,
	10,
	5,
	2.5,
	1.25,
	0.6,
	0.3,
	0.15,
	0.08,
	0.04,
	0.02,
	0.01,
	0.005,
	0.0025,
	0.0012,
	0.0006,
	0.0003,
};
const float s_map_step_ud[21] = {
	0,
	70,
	45,
	20,
	10,
	5,
	2.5,
	1.25,
	0.6,
	0.3,
	0.15,
	0.08,
	0.04,
	0.02,
	0.01,
	0.005,
	0.0025,
	0.0012,
	0.006,
	0.0003,
	0.00015,
};
/********************* end ***************************/

/********************* weather func***************************/
#define GIF_PATH WORK_PATH"theme/image/youtube/loading.gif"
void app_weather_draw_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_weather_gif", "draw_gif", &alu);
}

void app_weather_hide_gif(void)
{
	GUI_SetProperty("img_weather_gif", "state", "hide");
}

void app_weather_show_gif(void)
{
	GUI_SetProperty("img_weather_gif", "state", "show");
}

void app_weather_load_gif(void)
{
	int alu = GX_ALU_ROP_COPY_INVERT;
	GUI_SetProperty("img_weather_gif", "load_img", GIF_PATH);
	GUI_SetProperty("img_weather_gif", "init_gif_alu_mode", &alu);
}

void app_weather_free_gif(void)
{
	GUI_SetProperty("img_weather_gif", "load_img", NULL);
}

void app_weather_gif_update_proc(void* userdata)	
{
	app_weather_draw_gif();
}

void weather_get_config(void)
{
	FILE * fp = NULL;
	char buf[8];

	fp = fopen(WEATHER_CONF,"r");
	if(fp ==NULL) 
	{
		s_cf = 0;
		return;
	}
	
	memset(buf, 0, 8);
	pf_result = fgets(buf, 8, fp);

	s_cf = atoi(buf);

	fclose(fp);
}

void weather_set_config(void)
{
	FILE * fp = NULL;
	char buf[8];
	
	fp = fopen(WEATHER_CONF,"w");
	if(fp ==NULL) 
		return ;

	memset(buf, 0, 8);
	sprintf(buf,"%d", s_cf);
	fputs(buf, fp);
	
	fclose(fp);
	sync();
}

void weather_area_info_destroy(uint8_t index)
{
	if(s_area_info[index].name_list)
	{
		free(s_area_info[index].name_list);
		s_area_info[index].name_list = NULL;
		s_area_info[index].total = 0;
	}
}

void weather_area_parse(uint8_t index, bool get_default)
{
	FILE * fp = NULL;
	char buf[64];
	int i = 0;
	uint8_t len = 0;
	bool s_area_get = false;

	fp = fopen(AREA_RESULT,"r");
	if(fp ==NULL) 
		return;

	if(get_default)
	{
		pf_result = fgets(buf, 64, fp);
		
		for(i = 0; i <5; i++ )
		{
			memset(buf, 0, 64);
			pf_result = fgets(buf, 64, fp);
			len = strlen(buf);
			buf[len -1] = 0;
			
			memset(s_cur_area[i], 0, 64);
			memcpy(s_cur_area[i], buf, len);
		}
		if(s_weather_map  != 0)
		{
			memset(buf, 0, 64);
			pf_result = fgets(buf, 64, fp);
			s_temp_latitude = atof(buf);
			
			memset(buf, 0, 64);
			pf_result = fgets(buf, 64, fp);
			s_temp_longitude = atof(buf);
		}
	}
	else
	{
		if(index >= 4)
		{	
			fclose(fp);
			return;
		}
	
		weather_area_info_destroy(index);
	
		memset(buf, 0, 64);
		pf_result = fgets(buf, 64, fp);
		s_area_info[index].total = atoi(buf);
		
		for(i = 0; i <7; i++ )
		{
			pf_result = fgets(buf, 64, fp);
		}

		if(s_area_info[index].total > 0)	
		{
			s_area_info[index].name_list = malloc(s_area_info[index].total * sizeof(ubyte64));
			
			for(i = 0; i <s_area_info[index].total; i++ )
			{
				memset(buf, 0, 64);
				pf_result = fgets(buf, 64, fp);
				len = strlen(buf);
				buf[len -1] = 0;
				
				memcpy(s_area_info[index].name_list[i], buf, len);
				if(s_area_get != true && strcmp(s_cur_area[index],buf) == 0)
				{
					s_area_get = true;
					s_cur_sel[index] = i;
				}
			}
		}
	}

	fclose(fp);
}

void weather_area_info_create(uint8_t index, bool get_default)
{
	char cmd[256];
	
	memset(cmd, 0, 256);
	
	switch(index)
	{
		case 0:
			system_result = system(AREA_CMD);
			break;
		case 1:
			sprintf(cmd,"%s \"%s\"", AREA_CMD, s_cur_area[0]);
			system_result = system(cmd);
			break;
		case 2:
			sprintf(cmd,"%s \"%s\" \"%s\"", AREA_CMD, s_cur_area[0], s_cur_area[1]);
			system_result = system(cmd);
			break;
		case 3:
			sprintf(cmd,"%s \"%s\" \"%s\" \"%s\"", AREA_CMD, s_cur_area[0], s_cur_area[1], s_cur_area[2]);
			system_result = system(cmd);
			break;
		case 4:
			sprintf(cmd,"%s \"%s\" \"%s\" \"%s\" \"%s\"", AREA_CMD, s_cur_area[0], s_cur_area[1], s_cur_area[2], s_cur_area[3]);
			system_result = system(cmd);
			break;
		default:
			break;
	}
	
	printf("[cmd]%s\n", cmd);
	weather_area_parse(index, get_default);
	
}

void weather_area_update(void)
{	
	GUI_SetProperty(s_weather_area[0][0], "string", s_cur_area[0]);
	GUI_SetProperty(s_weather_area[1][0], "string", s_cur_area[1]);
	GUI_SetProperty(s_weather_area[2][0], "string", s_cur_area[2]);
	GUI_SetProperty(s_weather_area[3][0], "string", s_cur_area[3]);
}

void weather_area_partly_update(uint8_t index)
{
	switch(index)
	{
		case 0:
			GUI_SetProperty(s_weather_area[1][0], "string", s_cur_area[1]);
		case 1:
			GUI_SetProperty(s_weather_area[2][0], "string", s_cur_area[2]);
		case 2:
			GUI_SetProperty(s_weather_area[3][0], "string", s_cur_area[3]);
			break;
		default:
			break;
	}
}

void weather_info_default(void)
{
	uint8_t i = 0;
	
	s_weather_info.city[0] = '\n';
	s_weather_info.text[0] = '\n';
	s_weather_info.temp[0] = '\n';
	s_weather_info.publish[0] = '\n';
	s_weather_info.sunrise[0] = '\n';
	s_weather_info.sunset[0] = '\n';
	s_weather_info.humidity[0] = '\n';
	s_weather_info.visibility[0] = '\n';
	s_weather_info.wind[0] = '\n';
	s_weather_info.pressure[0] = '\n';
#ifdef PIC_LOCAL
	s_weather_info.cur_code = 48;
#endif
#ifdef PIC_NET
	s_weather_info.path[0] = '\n';
#endif
	
	for(i = 0; i < 5; i++)
	{
		s_weather_info.tempday[i].day[0] = '\n';
		s_weather_info.tempday[i].date[0] = '\n';
		s_weather_info.tempday[i].high[0] = '\n';
		s_weather_info.tempday[i].low[0] = '\n';
	#ifdef PIC_LOCAL
		s_weather_info.tempday[i].code = 48;
	#endif
	#ifdef PIC_NET
		s_weather_info.tempday[i].path[0] = '\n';
	#endif
	}
}

void weather_info_date_parse(char *dst, char *src)
{
	char *p, *q;
	char temp[8];
	uint8_t i = 0;
	uint8_t day = 0;

	if(dst == NULL || src == NULL)
		return ;

	day = atoi(src);
	if(day == 0)
	{
		dst[0] = '\n';
		return;
	}

	p = strchr(src, ' ');
	if(p == NULL)
	{
		dst[0] = '\n';
		return;
	}
	
	p++;
	q =  strchr(p, ' ');
	if(q == NULL)
	{
		dst[0] = '\n';
		return;
	}
	
	memset(temp, 0, 8);
	memcpy(temp, p, q-p);

	for(i = 0; i < 12; i++)
	{
		if(strcmp(temp, s_weather_mon[i]) == 0)
			break;
	}
	
	i++;
	if(i >12)
	{
		dst[0] = '\n';
		return;
	}
	
	sprintf(dst, "%02d-%02d", i,day);

}
	
bool weather_info_parse(void)
{
	bool ret = true;
	FILE * fp = NULL;
	char buf[128];
	uint8_t i = 0;
	uint8_t len = 0;

	memset(&s_weather_info, 0, sizeof(WeatherInfo));
	
	fp = fopen(WEATHER_RESULT,"r");
	if(fp ==NULL) 
	{
		//恢复默认设置
		weather_info_default();
		return false;
	}

	while(!feof(fp))
	{
		memset(buf, 0, 128);
		pf_result = fgets(buf, 128, fp);
		i++;
		switch(i)
		{
			case 1:
				{
					char *p = NULL;
					if((p = strchr(buf,',')))
					{
						p++;
						sprintf(s_weather_info.publish, "Publish:%s", p);
					}
					else
					{
						ret = false;
						s_weather_info.publish[0] = '\n';
					}
				}
				break;
			case 2:
				strcpy(s_weather_info.city,buf);
				break;
			case 3:
				{
					char * p ;
					strcpy(s_weather_info.temp,buf);
					if((p = strchr(s_weather_info.temp, 'f')))
					{
						s_cf = 1;
						*p = 0;
					}
					else if((p = strchr(s_weather_info.temp, 'c')))
					{
						s_cf = 0;
						*p = 0;
					}
				}
				break;
			case 4:
				strcpy(s_weather_info.text,buf);
				break;
		#ifdef PIC_LOCAL
			case 5:
				s_weather_info.cur_code = atoi(buf);
				if(((s_weather_info.cur_code== 0) && (buf[0] != '0')) || s_weather_info.cur_code>47)
				{
					s_weather_info.cur_code = 48;
				}
				break;
		#endif
		#ifdef PIC_NET
			case 6:
				strcpy(s_weather_info.path,buf);
				s_weather_info.path[strlen(buf) -1] = 0;
				break;
		#endif
			case 7:
				strcpy(s_weather_info.sunrise,buf);
				break;
			case 8:
				strcpy(s_weather_info.sunset,buf);
				break;
			case 9:
				strcpy(s_weather_info.humidity,buf);
				break;
			case 10:
				strcpy(s_weather_info.visibility,buf);
				break;
			case 11:
				strcpy(s_weather_info.pressure,buf);
				break;
			case 12:
				strcpy(s_weather_info.wind,buf);
				break;
			case 13:
				s_weather_info.day_num = atoi(buf);
				break;
			case 14:
			case 21:
			case 28:
			case 35:
			case 42:
				strcpy(s_weather_info.tempday[(i-14)/7].day,buf);
				break;
			case 15:
			case 22:
			case 29:
			case 36:
			case 43:
				weather_info_date_parse(s_weather_info.tempday[(i-14)/7].date,buf);
				break;
			case 16:
			case 23:
			case 30:
			case 37:
			case 44:
				{
					char *p;
					
					strcpy(s_weather_info.tempday[(i-14)/7].high,buf);
					if((p = strchr(s_weather_info.tempday[(i-14)/7].high, 'f')))
					{
						s_cf = 1;
						*p = 0;
					}
					else if((p = strchr(s_weather_info.tempday[(i-14)/7].high, 'c')))
					{
						s_cf = 0;
						*p = 0;
					}
					else
					{
						len = strlen(s_weather_info.tempday[(i-14)/7].high);
						s_weather_info.tempday[(i-14)/7].high[len -1] = 0;
					}
				}
				break;
			case 17:
			case 24:
			case 31:
			case 38:
			case 45:
				{
					char *p;
					
					strcpy(s_weather_info.tempday[(i-14)/7].low,buf);
					if((p = strchr(s_weather_info.tempday[(i-14)/7].low, 'f')))
					{
						s_cf = 1;
						*p = 0;
					}
					else if((p = strchr(s_weather_info.tempday[(i-14)/7].low, 'c')))
					{
						s_cf = 0;
						*p = 0;
					}
					else
					{
						len = strlen(s_weather_info.tempday[(i-14)/7].low);
						s_weather_info.tempday[(i-14)/7].low[len -1] = 0;
					}
				}
				break;
	#ifdef PIC_LOCAL
			case 19:
			case 26:
			case 33:
			case 40:
			case 47:
				s_weather_info.tempday[(i-14)/7].code = atoi(buf);
				if(((s_weather_info.tempday[(i-14)/7].code == 0) && (buf[0] != '0'))|| s_weather_info.cur_code>47)
				{
					s_weather_info.tempday[(i-14)/7].code = 48;
				}
				break;
	#endif
	#ifdef PIC_NET
			case 20:
			case 27:
			case 34:
			case 41:
			case 48:
				strcpy(s_weather_info.tempday[(i-14)/7].path,buf);
				s_weather_info.tempday[(i-14)/7].path[strlen(buf) -1] = 0;
				break;
	#endif
			default:
				break;	
		}
	}
	
	if(s_weather_info.day_num == 0)
	{
		s_weather_info.cur_code = 48;
		for(i = 0; i < 5; i++)
		{
			s_weather_info.tempday[i].day[0] = '\n';
			s_weather_info.tempday[i].date[0] = '\n';
			s_weather_info.tempday[i].high[0] = '\n';
			s_weather_info.tempday[i].low[0] = '\n';
		#ifdef PIC_LOCAL
			s_weather_info.tempday[i].code = 48;
		#endif
		#ifdef PIC_NET
			s_weather_info.tempday[i].path[0] = '\n';
		#endif
		
		}
	}

	fclose(fp);
	
	return ret;
}

void weather_info_update(void)
{
	uint8_t i = 0;
	static char buf[32];

	//GUI_SetProperty("text_weather_info_city", "string", s_weather_info.city);
	GUI_SetProperty("text_weather_info_city", "string", s_cur_area[3]);
	GUI_SetProperty("text_weather_info_day", "string", s_weather_info.text);
	GUI_SetProperty("text_weather_info_temp", "string", s_weather_info.temp);
	GUI_SetProperty("text_weather_info_publish", "string", s_weather_info.publish);
	GUI_SetProperty("img_weather_big_ssd", "img", s_weather_cf[s_cf][1]);

#ifdef PIC_LOCAL
	{	
		GUI_SetProperty("img_weather_info_pic", "load_img", s_weather_pic[s_weather_info.cur_code]);
		GUI_SetProperty("img_weather_pic_top", "load_img", s_weather_pic[s_weather_info.cur_code]);
	}
#endif
#ifdef PIC_NET
	if(s_weather_info.path[0] != '\n' && s_weather_info.path[0] != 0)
	{	
		GUI_SetProperty("img_weather_info_pic", "load_img", s_weather_info.path);
		GUI_SetProperty("img_weather_pic_top", "load_img", s_weather_info.path );
	}
	else
	{
		GUI_SetProperty("img_weather_info_pic", "load_img", WEATHER_DEFAULT_PIC);
		GUI_SetProperty("img_weather_pic_top", "load_img", WEATHER_DEFAULT_PIC);
	}
#endif
	
	GUI_SetProperty("text_weather_info_sunrise", "string", s_weather_info.sunrise);
	GUI_SetProperty("text_weather_info_sunset", "string", s_weather_info.sunset);
	GUI_SetProperty("text_weather_info_humidity", "string", s_weather_info.humidity);
	GUI_SetProperty("text_weather_info_visibility", "string", s_weather_info.visibility);
	GUI_SetProperty("text_weather_info_wind", "string", s_weather_info.wind);
	GUI_SetProperty("text_weather_info_pressure", "string", s_weather_info.pressure);

	for(i = 0; i < 5; i++)
	{
		GUI_SetProperty(s_weather_day[i][0], "string", s_weather_info.tempday[i].day);
		GUI_SetProperty(s_weather_day[i][1], "string", s_weather_info.tempday[i].date);
		memset(buf, 0, 32);
		sprintf(buf, "%s~%s", s_weather_info.tempday[i].high,s_weather_info.tempday[i].low);
		GUI_SetProperty(s_weather_day[i][2], "string", buf);
	#ifdef PIC_LOCAL
		{
			GUI_SetProperty(s_weather_day[i][3], "load_img",s_weather_pic[s_weather_info.tempday[i].code]);
		}
	#endif
	#ifdef PIC_NET
		if(s_weather_info.tempday[i].path[0] != '\n' && s_weather_info.tempday[i].path[0] != 0)
		{
			GUI_SetProperty(s_weather_day[i][3], "load_img", s_weather_info.tempday[i].path);
		}
		else
		{
			GUI_SetProperty(s_weather_day[i][3], "load_img", WEATHER_DEFAULT_PIC);
		}
	#endif
		GUI_SetProperty(s_weather_day[i][4], "img", s_weather_cf[s_cf][0]);
	}
}

void app_weather_hint_show(void)
{
	GUI_SetProperty(TXT_WEATHER_RED, "state", "show");
	GUI_SetProperty(IMG_WEATHER_RED, "state", "show");
	GUI_SetProperty(IMG_WEATHER_BLUE, "state", "show");
	GUI_SetProperty(TXT_WEATHER_BLUE, "state", "show");
	GUI_SetProperty(IMG_WEATHER_GREEN, "state", "show");
	GUI_SetProperty(TXT_WEATHER_GREEN, "state", "show");
	GUI_SetProperty(IMG_WEATHER_MOVE, "state", "show");
	GUI_SetProperty(TXT_WEATHER_MOVE, "state", "show");
	GUI_SetProperty(IMG_WEATHER_OK, "state", "show");
	GUI_SetProperty(TXT_WEATHER_OK, "state", "show");
	GUI_SetProperty(IMG_WEATHER_EXIT, "state", "show");
	GUI_SetProperty(TXT_WEATHER_EXIT, "state", "show");
}

void app_weather_hint_hide(void)
{
	GUI_SetProperty(IMG_WEATHER_RED, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_RED, "state", "hide");
	GUI_SetProperty(IMG_WEATHER_BLUE, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_BLUE, "state", "hide");
	GUI_SetProperty(IMG_WEATHER_GREEN, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_GREEN, "state", "hide");
	GUI_SetProperty(IMG_WEATHER_MOVE, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_MOVE, "state", "hide");
	GUI_SetProperty(IMG_WEATHER_OK, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_OK, "state", "hide");
	GUI_SetProperty(IMG_WEATHER_EXIT, "state", "hide");
	GUI_SetProperty(TXT_WEATHER_EXIT, "state", "hide");
}

void app_weather_tip_show(char *buf)
{
	GUI_SetProperty("img_weather_tip_bk", "state", "show");
	GUI_SetProperty("text_weather_tip_status", "string", buf);
	GUI_SetProperty("text_weather_tip_status", "state", "show");
}

void app_weather_tip_hide(void)
{
	GUI_SetProperty("img_weather_tip_bk", "state", "hide");
	GUI_SetProperty("text_weather_tip_status", "state", "hide");
}

void app_weather_update_ok_proc(void* userdata)
{
	bool ret = false;
	
	app_weather_tip_hide();
	ret = weather_info_parse();
	weather_info_update();
	if(s_area_update /* && ret*/)
	{
		weather_map_set_area(WEATHER_AREA);
		s_area_update = false;
	}
	s_cf_flag = false;
	s_weather_updating = false;
}

/*int app_weather_check_timer(void* usrdata)
{
	bool ret = false;
	
	ret = app_weather_status_check();
	s_weather_check_time++;

	if((ret)) //|| s_weather_check_time>7)
	{
		s_weather_check_time = 0;
		timer_stop(s_weather_check_timer);

		app_weather_tip_hide();
		ret = weather_info_parse();
		weather_info_update();
		if(s_area_update  && ret)
		{
			weather_map_set_area(WEATHER_AREA);
			s_area_update = false;
		}
		s_cf_flag = false;
		s_weather_updating = false;
	}
	return 0;
}*/

void weather_update(void)
{
	char cmd[256];

	s_weather_updating = true;
	memset(&s_weather_info, 0, sizeof(WeatherInfo));

	memset(cmd, 0, 256);
	if(s_cf == 1)
	{
		sprintf(cmd,"%s %s %s F ", FORECAST_CMD , WEATHER_RESULT, s_cur_area[4]);
	}
	else
	{
		sprintf(cmd,"%s %s %s C ", FORECAST_CMD , WEATHER_RESULT, s_cur_area[4]);
	}

	printf("[cmd]%s\n", cmd);
	//system_result = system(cmd);
	app_weather_tip_show("Updating...");
	system_result = system_shell(cmd,10000,app_weather_gif_update_proc,app_weather_update_ok_proc,NULL);
	/*if(0 != reset_timer(s_weather_check_timer)) 
	{	
		s_weather_check_timer = create_timer(app_weather_check_timer, 2000, NULL, TIMER_REPEAT);
	}*/
}

int weather_area_error_correction(char* src, char *dst)
{	
	int i = 0;
	int j = 0;
	int len = 0;
	int type = 0;
	bool caps_flag = true;
	bool blank_flag = false;

	len = strlen(src);
	if(len == 0)
		return 0;

	for(i = 0; i < len; i++)
	{
		if(src[i] >= 'a' && src[i] <='z')
			type = 0;
		else if(src[i] >= 'A' && src[i] <='Z')
			type = 1;
		else if(src[i] == ' ')
			type = 2;
		else 
			type = 3;

		switch(type)
		{
			case 0:
				if(caps_flag == true)
					dst[j++] = src[i]-32;
				else
					dst[j++] = src[i];
				caps_flag = false;
				blank_flag = true;
				break;
			case 1:
				if(caps_flag == true)
					dst[j++] = src[i];
				else
					dst[j++] = src[i]+32;
				caps_flag = false;
				blank_flag = true;
				break;
			case 2:
				if(blank_flag == true)
					dst[j++] = src[i];
				blank_flag = false;
				caps_flag = true;
				break;
			default:
				break;
		}
	}

	if(j > 0 && dst[j-1] == ' ')
		dst[j-1] = 0;
	
	if(strlen(dst) == 0)
		return 0;
	else
		return 1;
	
}


extern char s_full_keyboard_input[BUF_LEN];
void weather_full_keyboard_proc(void)
{
	char cmd[256] = {0};
	uint8_t total = 0;

	//app_weather_hint_show();

	app_weather_tip_show("searching...");
	GUI_SetInterface("flush",NULL);
	
	//sprintf(cmd, "%s \"%s\"", SEARCH_CMD, s_full_keyboard_input);
	sprintf(cmd, "%s \"%s\"", SEARCH_CMD, (const char*)s_full_keyboard_input);
	printf("[cmd]%s\n", cmd);
	system_result = system_shell(cmd,0,app_weather_gif_update_proc,NULL,NULL);
	
	total = weather_search_info_parse();
	//app_weather_tip_hide();
	if(total == 0)
	{
		//popmsg(440, 240, "Search Fail!", POPMSG_TYPE_OK);
		app_weather_tip_show("Search Fail!");
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(500);
		app_weather_tip_hide();
	}
	else
	{
		app_weather_tip_hide();
		GUI_SetProperty("img_weather_search_bk", "state", "show");
		GUI_SetProperty("text_weather_search_title", "state", "show");
		GUI_SetProperty("list_weather_search", "state", "show");
		GUI_SetFocusWidget("list_weather_search");
		GUI_SetProperty("list_weather_search", "update_all", NULL);
	}
}

void weather_search_get_area(int sel)
{
	char *p,*q;

	if((p = strchr(s_search_info.info[sel], ',')))
	{
		memset(s_cur_area[0], 0, 64);
		memcpy(s_cur_area[0], s_search_info.info[sel], p-s_search_info.info[sel]);
		p++;
		
		if((q = strchr(p, ',')))
		{
			memset(s_cur_area[1], 0, 64);
			memcpy(s_cur_area[1], p, q-p);
			p = q+1;

			if((q = strchr(p, ',')))
			{
				memset(s_cur_area[2], 0, 64);
				memcpy(s_cur_area[2], p, q-p);
				p = q+1;

				if((q = strchr(p, ',')))
				{
					memset(s_cur_area[3], 0, 64);
					memcpy(s_cur_area[3], p, q-p);
					p = q+1;
					
					if((q = strchr(p, ',')))
					{
						memset(s_cur_area[4], 0, 64);
						memcpy(s_cur_area[4], p, q-p);
					
						if(s_weather_map != 0)
						{
							p = q+1;
							s_temp_latitude = atof(p);
							s_center_latitude = s_cur_latitude = s_temp_latitude;
						
							p = strchr(p, ',');
							p++;
							s_temp_longitude = atof(p);
							s_center_longitude = s_cur_longitude = s_temp_longitude;
						}
					}
				}
			}
		}	
	}
}

uint8_t weather_search_info_parse(void)
{
	FILE * fp = NULL;
	char buf[256];
	int i = 0;
	uint8_t len = 0;

	fp = fopen(AREA_RESULT,"r");
	if(fp ==NULL) 
		return 0;
	
	memset(&s_search_info, 0, sizeof(SearchInfo));
	
	memset(buf, 0, 256);
	pf_result = fgets(buf, 256, fp);
	s_search_info.total= atoi(buf);
	
	for(i = 0; i < s_search_info.total; i++)
	{
		memset(buf, 0, 256);
		pf_result = fgets(buf, 256, fp);
		len = strlen(buf);
		buf[len -1] = 0;		
		memcpy(s_search_info.info[i] , buf, len);
	}
	
	fclose(fp);
	return s_search_info.total;

}

SIGNAL_HANDLER int app_weather_search_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_STOP;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{	
			case STBK_UP:
			case STBK_DOWN:
			case KEY_PAGE_DOWN:
			case KEY_PAGE_UP:
				ret = EVENT_TRANSFER_KEEPON;
				break;
			case STBK_EXIT:
				GUI_SetProperty("img_weather_search_bk", "state", "hide");
				GUI_SetProperty("text_weather_search_title", "state", "hide");
				GUI_SetProperty("list_weather_search", "state", "hide");
				GUI_SetFocusWidget(s_weather_area[s_cur_list][0]);
				break;
			case STBK_OK:
				{
					int sel = 0;
					GUI_GetProperty("list_weather_search", "select", &sel);
					weather_search_get_area(sel);
					GUI_SetProperty("img_weather_search_bk", "state", "hide");
					GUI_SetProperty("text_weather_search_title", "state", "hide");
					GUI_SetProperty("list_weather_search", "state", "hide");
					weather_area_update();
					if(s_weather_map == 0)
					{
						GUI_SetFocusWidget(s_weather_area[s_cur_list][0]);
						s_area_update = true;
						weather_update();
					}
					else
					{
						map_img_init();
						
						GUI_SetProperty("map_sliderbar", "state", "show");
						GUI_SetFocusWidget("map_sliderbar");
						map_update(s_cur_latitude, s_cur_longitude);
					}
				}
				break;
		}
	}
	return ret;
}

SIGNAL_HANDLER int app_weather_search_list_get_total(const char* widgetname, void *usrdata)
{	
	return s_search_info.total;
}

SIGNAL_HANDLER int app_weather_search_list_get_data(const char* widgetname, void *usrdata)
{	
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	//col-0
	item->string = s_search_info.info[item->sel];
	
	return EVENT_TRANSFER_STOP;
}
	
SIGNAL_HANDLER int app_weather_list_get_total(const char* widgetname, void *usrdata)
{	
	return s_area_info[s_cur_list].total;
}

SIGNAL_HANDLER int app_weather_list_get_data(const char* widgetname, void *usrdata)
{	
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	//col-0
	item->string = s_area_info[s_cur_list].name_list[item->sel];
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_weather_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int sel = 0;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{
			case STBK_OK:
				GUI_GetProperty(s_weather_area[s_cur_list][1], "select", &sel);
				if(strcmp(s_cur_area[s_cur_list], s_area_info[s_cur_list].name_list[sel]))
				{
					memset(s_cur_area[s_cur_list], 0, 64);
					memcpy(s_cur_area[s_cur_list], s_area_info[s_cur_list].name_list[sel], 64);
					GUI_SetProperty(s_weather_area[s_cur_list][0], "string", s_area_info[s_cur_list].name_list[sel]);
					weather_area_info_create(s_cur_list+1, true);
					weather_area_partly_update(s_cur_list);
				}
			case STBK_LEFT:
			case	 STBK_EXIT:
				GUI_SetProperty(s_weather_area[s_cur_list][1], "state", "hide");
				GUI_SetFocusWidget(s_weather_area[s_cur_list][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_weather_button_state_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		//更新时，只响应退出键
		if(s_weather_updating == true && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_UP:
				if(s_weather_map == 0)
				{
					GUI_SetFocusWidget(s_map_button[2]);
				}
				else
				{
					GUI_SetFocusWidget(s_map_button[1]);
				}
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				s_cur_list = 1;
				GUI_SetFocusWidget(s_weather_area[1][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				s_cur_list = 0;
				weather_area_info_create(s_cur_list,false);
				GUI_SetProperty(s_weather_area[0][1], "state", "show");
				GUI_SetFocusWidget(s_weather_area[0][1]);
				GUI_SetProperty(s_weather_area[0][1], "update_all", NULL);
				GUI_SetProperty(s_weather_area[0][1], "select", &s_cur_sel[0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_weather_button_country_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		//更新时，只响应退出键
		if(s_weather_updating == true && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_UP:
				s_cur_list = 0;
				GUI_SetFocusWidget(s_weather_area[0][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				s_cur_list = 2;
				GUI_SetFocusWidget(s_weather_area[2][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				s_cur_list = 1;
				weather_area_info_create(s_cur_list,false);
				GUI_SetProperty(s_weather_area[1][1], "state", "show");
				GUI_SetFocusWidget(s_weather_area[1][1]);
				GUI_SetProperty(s_weather_area[1][1], "update_all", NULL);
				GUI_SetProperty(s_weather_area[1][1], "select", &s_cur_sel[1]);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_weather_button_province_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{
			//更新时，只响应退出键
			if(s_weather_updating == true && event->key.sym != STBK_EXIT)
				return EVENT_TRANSFER_STOP;
			case STBK_UP:
				s_cur_list = 1;
				GUI_SetFocusWidget(s_weather_area[1][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				s_cur_list = 3;
				GUI_SetFocusWidget(s_weather_area[3][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				s_cur_list = 2;
				weather_area_info_create(s_cur_list,false);
				GUI_SetProperty(s_weather_area[2][1], "state", "show");
				GUI_SetFocusWidget(s_weather_area[2][1]);
				GUI_SetProperty(s_weather_area[2][1], "update_all", NULL);
				GUI_SetProperty(s_weather_area[2][1], "select", &s_cur_sel[2]);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_weather_button_city_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		//更新时，只响应退出键
		if(s_weather_updating == true && event->key.sym != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_UP:
				s_cur_list = 2;
				GUI_SetFocusWidget(s_weather_area[2][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				GUI_SetFocusWidget(s_map_button[2]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				s_cur_list = 3;
				weather_area_info_create(s_cur_list,false);
				GUI_SetProperty(s_weather_area[3][1], "state", "show");
				GUI_SetFocusWidget(s_weather_area[3][1]);
				GUI_SetProperty(s_weather_area[3][1], "update_all", NULL);
				GUI_SetProperty(s_weather_area[3][1], "select", &s_cur_sel[3]);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}
/********************* end***************************/

/********************* map func***************************/
void map_get_config(void)
{
	FILE * fp = NULL;
	char buf[8];
	char *p = NULL;

	fp = fopen(MAP_CONF,"r");
	if(fp ==NULL) 
	{
		s_map_type= 0;
		return;
	}
	
	memset(buf, 0, 8);
	pf_result = fgets(buf, 8, fp);
	s_map_lan = atoi(buf);

	if((p = strchr(buf,',')))
	{
		p++;
		s_map_type = atoi(p);
	}
	else
		s_map_type = 0;
	
	fclose(fp);
}

void map_set_config(void)
{
	FILE * fp = NULL;
	char buf[8];
	
	fp = fopen(MAP_CONF,"w");
	if(fp ==NULL) 
		return ;

	memset(buf, 0, 8);
	sprintf(buf,"%d,%d", s_map_lan, s_map_type);
	fputs(buf, fp);
	
	fclose(fp);
	sync();
}

void map_hide_weather_widget(void)
{
	uint8_t i,j= 0;

	//GUI_SetProperty("img_weather_top", "state", "hide");
	//GUI_SetProperty("img_weather_line_top", "state", "hide");
	GUI_SetProperty("img_logo", "state", "hide");
	GUI_SetProperty("img_weather_info_pic", "state", "hide");
	GUI_SetProperty("img_weather_pic_top", "state", "hide");
	//GUI_SetProperty("img_weather_mid", "state", "hide");
	
	GUI_SetProperty("text_weather_info_city", "state", "hide");
	GUI_SetProperty("text_weather_info_day", "state", "hide");
	GUI_SetProperty("text_weather_info_temp", "state", "hide");
	GUI_SetProperty("img_weather_big_ssd", "state", "hide");
	GUI_SetProperty("text_weather_info_publish", "state", "hide");

	GUI_SetProperty("text_weather_info_sunrise", "state", "hide");
	GUI_SetProperty("text_weather_info_sunset", "state", "hide");
	GUI_SetProperty("text_weather_info_visibility", "state", "hide");
	GUI_SetProperty("text_weather_info_humidity", "state", "hide");
	GUI_SetProperty("text_weather_info_wind", "state", "hide");
	GUI_SetProperty("text_weather_info_pressure", "state", "hide");
	GUI_SetProperty("text_weather_info0", "state", "hide");
	GUI_SetProperty("text_weather_info1", "state", "hide");
	GUI_SetProperty("text_weather_info2", "state", "hide");
	GUI_SetProperty("text_weather_info3", "state", "hide");
	GUI_SetProperty("text_weather_info4", "state", "hide");
	GUI_SetProperty("text_weather_info5", "state", "hide");

	GUI_SetProperty("img_weather_date0_up", "state", "hide");
	GUI_SetProperty("img_weather_date0_bottom", "state", "hide");
	GUI_SetProperty("img_weather_date1_up", "state", "hide");
	GUI_SetProperty("img_weather_date1_bottom", "state", "hide");
	GUI_SetProperty("img_weather_date2_up", "state", "hide");
	GUI_SetProperty("img_weather_date2_bottom", "state", "hide");
	GUI_SetProperty("img_weather_date3_up", "state", "hide");
	GUI_SetProperty("img_weather_date3_bottom", "state", "hide");
	GUI_SetProperty("img_weather_date4_up", "state", "hide");
	GUI_SetProperty("img_weather_date4_bottom", "state", "hide");
	GUI_SetProperty("img_weather_date5_up", "state", "hide");
	GUI_SetProperty("img_weather_date5_bottom", "state", "hide");
	GUI_SetProperty("img_weather_split0", "state", "hide");
	GUI_SetProperty("img_weather_split1", "state", "hide");
	GUI_SetProperty("img_weather_split2", "state", "hide");
	GUI_SetProperty("img_weather_split3", "state", "hide");
	
	for(i = 0; i<5; i++)
	{
		for(j = 0; j<5;j++)
		GUI_SetProperty(s_weather_day[i][j], "state", "hide");
	}
}

void map_img_init(void)
{
	char buf[64];

	GUI_SetProperty(TXT_WEATHER_BLUE, "string", "Zoom-");
	GUI_SetProperty(TXT_WEATHER_GREEN, "string", "Zoom+");
	GUI_SetProperty(TXT_WEATHER_OK, "string", "Update");
						
	memset(buf, 0, 64);
	sprintf(buf, "%f,%f", s_cur_latitude, s_cur_longitude);
	GUI_SetProperty("text_map_coordinate", "state", "show");
	GUI_SetProperty("text_map_coordinate", "string", buf);
	
	GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
	GUI_SetProperty("map_sliderbar", "state", "show");
	GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
	GUI_SetFocusWidget("map_sliderbar");
}

void map_img_end(void)
{
	GUI_SetProperty("map_sliderbar", "state", "hide");
	GUI_SetProperty("text_map_coordinate", "state", "hide");
	GUI_SetProperty(s_map_arrow[0], "state", "hide");
	GUI_SetProperty(s_map_arrow[1], "state", "hide");
	GUI_SetProperty(s_map_arrow[2], "state", "hide");
	GUI_SetProperty(s_map_arrow[3], "state", "hide");

	GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");

	GUI_SetProperty(TXT_WEATHER_BLUE, "string", "Save");
	GUI_SetProperty(TXT_WEATHER_GREEN, "string", "Update");
	GUI_SetProperty(TXT_WEATHER_OK, "string", "Enter");
}

int map_update(float latitude, float longitude)
{
	char cmd[256];
	
	memset(cmd, 0, 256);
	sprintf(cmd, "google_map_get_picture %s %f %f %s %d %s %s", MAP_PIC, latitude, longitude, MAP_SIZE, s_map_zoom, s_map_type_data[s_map_type], s_map_lan_data[s_map_lan][1]);
	printf("[cmd]%s\n", cmd);

	app_weather_show_gif();
	system_result = system_shell(cmd,0,app_weather_gif_update_proc,NULL,NULL);
	app_weather_hide_gif();
	
	if( 0 == access(MAP_PIC, F_OK) )
	{
		GUI_SetProperty("img_map", "load_scal_img", MAP_PIC);
		return 1;
	}
	else
	{
		//popmsg(615, 240, "Update Fail!", POPMSG_TYPE_OK);
		app_weather_tip_show(STR_ID_SERVER_FAIL);
		GUI_SetInterface("flush",NULL);
		GxCore_ThreadDelay(800);
		app_weather_tip_hide();
		return 0;
	}
}

SIGNAL_HANDLER int app_map_lan_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_STOP;
	int sel;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{	
			case STBK_UP:
			case STBK_DOWN:
			case KEY_PAGE_DOWN:
			case KEY_PAGE_UP:
				ret = EVENT_TRANSFER_KEEPON;
				break;
			case STBK_LEFT:
			case STBK_EXIT:
				GUI_SetProperty("list_map_lan", "state", "hide");
				GUI_SetFocusWidget(s_map_button[0]);
				break;
			case STBK_OK:
				{
					GUI_GetProperty("list_map_lan", "select", &sel);
					GUI_SetProperty("list_map_lan", "state", "hide");
					GUI_SetFocusWidget(s_map_button[0]);
					if(sel != s_map_lan)
					{
						s_map_lan = sel;
						printf("%s##", s_map_lan_data[sel][0]);
						GUI_SetProperty(s_map_button[0], "string", s_map_lan_data[sel][0]);
						GUI_SetInterface("flush",NULL);
						map_update(s_cur_latitude, s_cur_longitude);
					}
				}
				break;
		}
	}
	return ret;
}

SIGNAL_HANDLER int app_map_lan_list_get_total(const char* widgetname, void *usrdata)
{	
	return 6;
}

SIGNAL_HANDLER int app_map_lan_list_get_data(const char* widgetname, void *usrdata)
{	
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	//col-0
	item->string = s_map_lan_data[item->sel][0];
	
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int app_map_type_list_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_STOP;
	int sel;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{	
			case STBK_UP:
			case STBK_DOWN:
			case KEY_PAGE_DOWN:
			case KEY_PAGE_UP:
				ret = EVENT_TRANSFER_KEEPON;
				break;
			case STBK_LEFT:
			case STBK_EXIT:
				GUI_SetProperty("list_map_type", "state", "hide");
				GUI_SetFocusWidget(s_map_button[1]);
				break;
			case STBK_OK:
				{
					GUI_GetProperty("list_map_type", "select", &sel);
					GUI_SetProperty("list_map_type", "state", "hide");
					GUI_SetFocusWidget(s_map_button[1]);
					if(sel != s_map_type)
					{
						s_map_type = sel;
						GUI_SetProperty(s_map_button[1], "string", s_map_type_data[sel]);
						GUI_SetInterface("flush",NULL);
						map_update(s_cur_latitude, s_cur_longitude);
					}
				}
				break;
		}
	}
	return ret;
}

SIGNAL_HANDLER int app_map_type_list_get_total(const char* widgetname, void *usrdata)
{	
	return 4;
}

SIGNAL_HANDLER int app_map_type_list_get_data(const char* widgetname, void *usrdata)
{	
	ListItemPara* item = NULL;

	item = (ListItemPara*)usrdata;
	if(NULL == item) 	
		return GXCORE_ERROR;
	if(0 > item->sel)
		return GXCORE_ERROR;

	//col-0
	item->string = s_map_type_data[item->sel];
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_map_button_lan_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{
			case STBK_UP:
				GUI_SetFocusWidget(s_map_button[2]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				GUI_SetFocusWidget(s_map_button[1]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				GUI_SetProperty("list_map_lan", "state", "show");
				GUI_SetFocusWidget("list_map_lan");
				GUI_SetProperty("list_map_lan", "update_all", NULL);
				GUI_SetProperty("list_map_lan", "select", &s_map_lan);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_map_button_type_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(event->key.sym)
		{
			case STBK_UP:
				GUI_SetFocusWidget(s_map_button[0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_DOWN:
				s_cur_list = 0;
				GUI_SetFocusWidget(s_weather_area[0][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_RIGHT:
			case STBK_OK:
				GUI_SetProperty("list_map_type", "state", "show");
				GUI_SetFocusWidget("list_map_type");
				GUI_SetProperty("list_map_type", "update_all", NULL);
				GUI_SetProperty("list_map_type", "select", &s_map_type);
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_map_button_start_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_KEEPON;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		if(s_weather_updating == true && event->key.sym != STBK_EXIT)
				return EVENT_TRANSFER_STOP;
		switch(event->key.sym)
		{
			case STBK_DOWN:
				if(s_weather_map == 0)
				{
					s_cur_list = 0;
					GUI_SetFocusWidget(s_weather_area[0][0]);
					
				}
				else
				{
					GUI_SetFocusWidget(s_map_button[0]);
				}
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_UP:
				s_cur_list = 3;
				GUI_SetFocusWidget(s_weather_area[3][0]);
				ret = EVENT_TRANSFER_STOP;
				break;
			case STBK_OK:
				if(s_weather_map == 0)
				{
					if(strcmp(s_cur_area[4], s_old_area[4]))
					{
						s_area_update = true;
					}
					weather_update();
				}
				else
				{
					if(s_temp_latitude != s_center_latitude ||s_temp_longitude != s_center_longitude )
					{	
						s_center_latitude = s_cur_latitude = s_temp_latitude;
						s_center_longitude= s_cur_longitude= s_temp_longitude;
						map_img_init();
						map_update(s_cur_latitude, s_cur_longitude);
					}
					else
					{
						map_img_init();
					}
				}
				ret = EVENT_TRANSFER_STOP;
				break;
			default:
				break;
		}		
	}
	return ret;
}

SIGNAL_HANDLER int app_map_sliderbar_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	char buf[64];
	float latitude, longitude;
	static bool key_press = false;

	if(key_press == true)
		return EVENT_TRANSFER_STOP;
	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		key_press = true;
		switch(find_virtualkey(event->key.sym))
		{
			case KEY_EXIT:
				map_img_end();
				GUI_SetFocusWidget(s_map_button[2]);
				break;
			//查找
			//case STBK_AUDIO:
			case KEY_RED:
				//app_weather_hint_hide();
				s_full_keyboard_proc = weather_full_keyboard_proc;
				GUI_CreateDialog(WND_FULL_KEYBOARD);
				GUI_SetProperty(TXT_FULL_KEYBOARD_HINT, "string",WIFI_KEYBOARD_HINT);
				#if 0
				{
				static PopKeyboard keyboard;
				memset(&keyboard, 0, sizeof(PopKeyboard));
					keyboard.in_name    = NULL;
					keyboard.max_num = 64 - 1;
				keyboard.out_name   = NULL;
				keyboard.change_cb  = NULL;
				keyboard.release_cb = weather_full_keyboard_proc;
				keyboard.usr_data   = NULL;
				keyboard.pos.x = 500;
				multi_language_keyboard_create(&keyboard);
				}
				#endif
				break;
			//zoom-
			//case STBK_PAUSE_STB:
			case KEY_BLUE:
				if(s_map_zoom >1)
				{
					s_map_zoom--;			
					if(map_update(s_cur_latitude, s_cur_longitude) == 0)
					{
						s_map_zoom++;
					}
					
					GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
					GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
					GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				}
				break;
			//zoom+
			//case STBK_ZOOM:
			case KEY_GREEN:
				if(s_map_zoom < 20)
				{
					s_map_zoom++;
					if(map_update(s_cur_latitude, s_cur_longitude) == 0)
					{
						s_map_zoom--;
					}

					GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
					GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
					GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				}
				break;
			case STBK_OK:
				map_update(s_cur_latitude, s_cur_longitude);
				GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
				GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
				GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				break;
			case STBK_RIGHT:
				GUI_SetProperty(s_map_arrow[0], "state", "hide");
				GUI_SetProperty(s_map_arrow[1], "state", "hide");
				GUI_SetProperty(s_map_arrow[2], "state", "hide");
				GUI_SetProperty(s_map_arrow[3], "state", "show");
				GUI_SetInterface("flush",NULL);
				
				if((s_cur_longitude + s_map_step_lr[s_map_zoom]) < 180)
					longitude = s_cur_longitude + s_map_step_lr[s_map_zoom];
				else
					longitude = 180;
				if(map_update(s_cur_latitude, longitude))
				{
					s_cur_longitude = longitude;
				}
				
				memset(buf, 0, 64);
				sprintf(buf, "%f,%f", s_cur_latitude, s_cur_longitude);
				GUI_SetProperty("text_map_coordinate", "string", buf);

				GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
				GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
				GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				break;
			case STBK_LEFT:
				GUI_SetProperty(s_map_arrow[0], "state", "hide");
				GUI_SetProperty(s_map_arrow[1], "state", "hide");
				GUI_SetProperty(s_map_arrow[2], "state", "show");
				GUI_SetProperty(s_map_arrow[3], "state", "hide");
				GUI_SetInterface("flush",NULL);
				
				if((s_cur_longitude - s_map_step_lr[s_map_zoom] )> -180)
					longitude = s_cur_longitude - s_map_step_lr[s_map_zoom];
				else
					longitude = -180;
				if(map_update(s_cur_latitude, longitude))
				{
					s_cur_longitude = longitude;
				}
				
				memset(buf, 0, 64);
				sprintf(buf, "%f,%f", s_cur_latitude, s_cur_longitude);
				GUI_SetProperty("text_map_coordinate", "string", buf);

				GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
				GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
				GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				break;
			case STBK_UP:
				GUI_SetProperty(s_map_arrow[0], "state", "show");
				GUI_SetProperty(s_map_arrow[1], "state", "hide");
				GUI_SetProperty(s_map_arrow[2], "state", "hide");
				GUI_SetProperty(s_map_arrow[3], "state", "hide");
				GUI_SetInterface("flush",NULL);
				
				if((s_cur_latitude + s_map_step_ud[s_map_zoom]) < 90)
					latitude = s_cur_latitude + s_map_step_ud[s_map_zoom];
				else
					latitude = 90;
				if(map_update(latitude, s_cur_longitude))
				{
					s_cur_latitude = latitude;
				}

				memset(buf, 0, 64);
				sprintf(buf, "%f,%f", s_cur_latitude, s_cur_longitude);
				GUI_SetProperty("text_map_coordinate", "string", buf);

				GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
				GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
				GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				break;
			case STBK_DOWN:
				GUI_SetProperty(s_map_arrow[0], "state", "hide");
				GUI_SetProperty(s_map_arrow[1], "state", "show");
				GUI_SetProperty(s_map_arrow[2], "state", "hide");
				GUI_SetProperty(s_map_arrow[3], "state", "hide");
				GUI_SetInterface("flush",NULL);
				
				if((s_cur_latitude - s_map_step_ud[s_map_zoom]) > -90)
					latitude = s_cur_latitude - s_map_step_ud[s_map_zoom];
				else
					latitude = -90;
				if(map_update(latitude, s_cur_longitude))
				{
					s_cur_latitude = latitude;
				}

				memset(buf, 0, 64);
				sprintf(buf, "%f,%f", s_cur_latitude, s_cur_longitude);
				GUI_SetProperty("text_map_coordinate", "string", buf);

				GUI_SetProperty("img_map_sliderbar_bk", "state", "hide");
				GUI_SetProperty("img_map_sliderbar_bk", "state", "show");
				GUI_SetProperty("map_sliderbar", "value", &s_map_zoom);
				break;
			default:
				break;
		}		
	}
	key_press = false;
	return EVENT_TRANSFER_STOP;
}
/********************* end ***************************/

/********************* public func***************************/
void app_weather_map_set_wnd(uint8_t wnd)
{
	s_weather_map = wnd;
}

void weather_map_get_area(char *file)
{
	FILE * fp = NULL;
	char buf[256];
	char *p = NULL;
	char *q = NULL;

	fp = fopen(file,"r");
	if(fp ==NULL) 
	{

		memset(s_cur_area, 0, 5*64);
		memset(s_old_area, 0, 5*64);
		
		memcpy(s_cur_area[0], DEFAULT_STATE, 64);
		memcpy(s_old_area[0], DEFAULT_STATE, 64);
		memcpy(s_cur_area[1], DEFAULT_COUNTRY, 64);
		memcpy(s_old_area[1], DEFAULT_COUNTRY, 64);
		memcpy(s_cur_area[2], DEFAULT_PROVINCE, 64);
		memcpy(s_old_area[2], DEFAULT_PROVINCE, 64);
		memcpy(s_cur_area[3], DEFAULT_CITY, 64);
		memcpy(s_old_area[3], DEFAULT_CITY, 64);
		memcpy(s_cur_area[4], DEFAULT_CODE, 64);
		memcpy(s_old_area[4], DEFAULT_CODE, 64);
		if(s_weather_map != 0)
		{	
			s_temp_latitude = s_center_latitude = s_cur_latitude = DEFAULT_LATITUDE;
			s_temp_longitude = s_center_longitude = s_cur_longitude = DEFAULT_LONGITUDE;
		}
		return;
	}

	memset(buf, 0, 256);
	pf_result = fgets(buf, 256, fp);

	if((p = strchr(buf,',')))
	{
		memset(s_cur_area[0], 0, 64);
		memcpy(s_cur_area[0], buf, p-buf);
		memcpy(s_old_area[0], s_cur_area[0], 64);
		p++;
		
		if((q = strchr(p,',')))
		{
			memset(s_cur_area[1], 0, 64);
			memcpy(s_cur_area[1], p, q-p);
			memcpy(s_old_area[1], s_cur_area[1], 64);
			p = q+1;

			if((q = strchr(p,',')))
			{
				memset(s_cur_area[2], 0, 64);
				memcpy(s_cur_area[2], p, q-p);
				memcpy(s_old_area[2], s_cur_area[2], 64);
				p = q+1;

				if((q = strchr(p,',')))
				{
					memset(s_cur_area[3], 0, 64);
					memcpy(s_cur_area[3], p, q-p);
					memcpy(s_old_area[3], s_cur_area[3], 64);
					p = q+1;

					if((q = strchr(p,',')))
					{
						memset(s_cur_area[4], 0, 64);
						memcpy(s_cur_area[4], p, q-p);
						memcpy(s_old_area[4], s_cur_area[4], 64);

						if(s_weather_map != 0)
						{
							p = q+1;
							s_center_latitude = atof(p);
							s_temp_latitude = s_cur_latitude = s_center_latitude;
							
							p =  strchr(p,',');
							p++;
							s_center_longitude = atof(p);
							s_temp_longitude = s_cur_longitude = s_center_longitude;
						}
					}
				}
			}
		}
	}	
		
	fclose(fp);
}

void weather_map_set_area(char *file)
{
	FILE * fp = NULL;
	char buf[384];

	fp = fopen(file,"w");
	if(fp ==NULL) 
		return ;

	memcpy(s_old_area[0], s_cur_area[0], 64);
	memcpy(s_old_area[1], s_cur_area[1], 64);
	memcpy(s_old_area[2], s_cur_area[2], 64);
	memcpy(s_old_area[3], s_cur_area[3], 64);
	memcpy(s_old_area[4], s_cur_area[4], 64);
	
	memset(buf, 0, 384);
	if(s_weather_map == 0)
		sprintf(buf,"%s,%s,%s,%s,%s,", s_cur_area[0], s_cur_area[1], s_cur_area[2], s_cur_area[3], s_cur_area[4]);
	else
	{
		sprintf(buf,"%s,%s,%s,%s,%s,%f, %f", s_cur_area[0], s_cur_area[1], s_cur_area[2], s_cur_area[3], s_cur_area[4], s_center_latitude, s_center_longitude);
	}
	fputs(buf, fp);
	
	fclose(fp);
	sync();
}

SIGNAL_HANDLER int app_weather_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
	    uint16_t key = find_virtualkey(event->key.sym);
		//更新时，只响应退出键
		if(s_weather_updating == true && key != STBK_EXIT)
			return EVENT_TRANSFER_STOP;
		switch(key)
		{
			case STBK_EXIT:
				if(s_weather_updating)
				{
					timer_stop(s_weather_check_timer);
					system_result = system("weather_exit");
					app_weather_tip_hide();
					if(s_area_update)
					{
						memcpy(s_cur_area[0],  s_old_area[0], 64);
						memcpy(s_cur_area[1],  s_old_area[1], 64);
						memcpy(s_cur_area[2],  s_old_area[2], 64);
						memcpy(s_cur_area[3],  s_old_area[3], 64);
						memcpy(s_cur_area[4],  s_old_area[4], 64);
						weather_area_update();
						s_area_update = false;
					}
					if(s_cf_flag == true)
					{
						s_cf_flag = false;
						s_cf = (s_cf+1) %2;
					}
					s_weather_check_time = 0;
					s_weather_updating = false;
				}
				else
				{
					GUI_EndDialog(WND_WEATHER_MAP);
				}
				break;
			//查找
			//case STBK_AUDIO:
			case KEY_RED:
				//app_weather_hint_hide();
				s_full_keyboard_proc = weather_full_keyboard_proc;
				GUI_CreateDialog(WND_FULL_KEYBOARD);
				GUI_SetProperty(TXT_FULL_KEYBOARD_HINT, "string",WIFI_KEYBOARD_HINT);
				#if 0
				{
					static PopKeyboard keyboard;
					memset(&keyboard, 0, sizeof(PopKeyboard));
					keyboard.in_name    = NULL;
					keyboard.max_num = 64 - 1;
					keyboard.out_name   = NULL;
					keyboard.change_cb  = NULL;
					keyboard.release_cb = weather_full_keyboard_proc;
					keyboard.usr_data   = NULL;
					keyboard.pos.x = 500;
					multi_language_keyboard_create(&keyboard);
				}
				#endif
				break;
			//切换温度单位
			//case STBK_PAUSE_STB:
			case KEY_BLUE:
				if(s_weather_map == 0)
				{
					s_cf_flag = true;
					s_cf = (s_cf+1) %2;
					if(strcmp(s_cur_area[4], s_old_area[4]))
					{
						s_area_update = true;
					}
					weather_update();
				}
				else
				{
					if(s_temp_latitude != s_center_latitude ||s_temp_longitude != s_center_longitude )
					{	
						s_center_latitude = s_cur_latitude = s_temp_latitude;
						s_center_longitude= s_cur_longitude= s_temp_longitude;
					}
					weather_map_set_area(MAP_AREA);
					//popmsg(440, 240, "Save Success!", POPMSG_TYPE_OK);
					app_weather_tip_show("Save Success!");
					GUI_SetInterface("flush",NULL);
					GxCore_ThreadDelay(500);
					app_weather_tip_hide();
				}
				break;
			//更新
			//case STBK_ZOOM:
			case KEY_GREEN:
				if(s_weather_map == 0)
				{
					if(strcmp(s_cur_area[4], s_old_area[4]))
					{
						s_area_update = true;
					}
					weather_update();
				}
				else
				{
					map_update(s_cur_latitude, s_cur_longitude);
				}
				break;
			default:
				break;
		}		
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_weather_create(const char* widgetname, void *usrdata)
{	
	app_weather_load_gif();
	
	if(s_weather_map == 0)
	{
		uint8_t i = 0;
		
		weather_get_config();
		
		GUI_SetProperty("text_weather_title", "string", "Weather");
		GUI_SetProperty(TXT_WEATHER_BLUE, "string", "C/F");
		GUI_SetProperty(TXT_WEATHER_GREEN, "string", "Update");
		GUI_SetProperty("img_logo", "load_scal_img", WORK_PATH"theme/image/weather/yahoo_logo.jpg");
		
		weather_map_get_area(WEATHER_AREA);
		weather_area_update();

		s_cur_list = 3;
		GUI_SetFocusWidget(s_map_button[2]);
		GUI_SetInterface("flush",NULL);

		GUI_SetProperty("img_weather_info_pic", "load_img", WEATHER_DEFAULT_PIC);
		GUI_SetProperty("img_weather_pic_top", "load_img", WEATHER_DEFAULT_PIC);
		GUI_SetProperty("img_weather_big_ssd", "img", s_weather_cf[s_cf][1]);
		
		for(i = 0; i < 5; i++)
		{
			GUI_SetProperty(s_weather_day[i][3], "load_img",WEATHER_DEFAULT_PIC);
			GUI_SetProperty(s_weather_day[i][4], "img", s_weather_cf[s_cf][0]);
		}
		weather_update();
	}
	else
	{
		s_map_zoom = 9;
		map_get_config();
		
		GUI_SetProperty("text_weather_title", "string", "Google Map");
		GUI_SetProperty(TXT_WEATHER_BLUE, "string", "Save");
		GUI_SetProperty(TXT_WEATHER_GREEN, "string", "Update");

		GUI_SetProperty("img_map_logo", "state", "show");

		GUI_SetProperty(s_map_button[0], "string", s_map_lan_data[s_map_lan][0]);
		GUI_SetProperty(s_map_button[0], "state", "show");
		GUI_SetProperty(s_map_button[1], "string", s_map_type_data[s_map_type]);
		GUI_SetProperty(s_map_button[1], "state", "show");
		GUI_SetProperty(s_map_button[2], "state", "show");
		GUI_SetProperty("img_map", "state", "show");
		map_hide_weather_widget();
		
		weather_map_get_area(MAP_AREA);
		weather_area_update();
		
		s_cur_list = 3;
		GUI_SetFocusWidget(s_map_button[2]);
		GUI_SetInterface("flush",NULL);
		map_update(s_cur_latitude, s_cur_longitude);
	}
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_weather_destroy(const char* widgetname, void *usrdata)
{
	uint8_t i = 0;

	for(i = 0; i < 4; i++)
	{
		weather_area_info_destroy(i);
	}

	if(s_weather_check_timer)
	{
		remove_timer(s_weather_check_timer);
		s_weather_check_timer = NULL;
	}
	
	if(s_weather_map == 0)
	{
		weather_set_config();
		s_area_update = false;
		s_weather_check_time = 0;
	 	s_weather_updating = false;
		system_result = system("weather_exit");
	}
	else
	{	
		map_set_config();
		system_result = system("google_map_exit");
	}
	app_weather_free_gif();
	return EVENT_TRANSFER_STOP;
}
/********************* end ***************************/
#endif


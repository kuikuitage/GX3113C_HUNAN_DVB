/*
 * =====================================================================================
 *
 *       Filename:  app_pvr.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年08月05日 10时29分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#if 1
//#include "app.h" 
#include "gxhotplug.h"
#include "gxconfig.h"
#include "app_common_prog.h"
#include "app_common_flash.h"
#include "app_common_epg.h"
#include "app_common_play.h"
#include "file_edit.h"
#include "gxepg.h"
#include "gxmsg.h"
#include "gxprogram_manage_berkeley.h"
#include "app_pvr.h"
#include "gxapp_sys_config.h"
#define PVR_PATH_LEN    (64)
#if MEDIA_SUBTITLE_SUPPORT
extern void app_subt_pause(void);
#endif


#define TMS_FILE_KEY    "pvr>tms_file"
#define CHECK_ALLOC(p, size)   do {\
    if (p == NULL) {\
    p = (char*)GxCore_Malloc(size);\
    if (p == NULL)  return GXCORE_ERROR;\
    };\
    memset(p, 0, size);\
}while(0)

#define FREE_NULL(p) do{if (p!= NULL){\
    GxCore_Free(p);\
    p=NULL;}}while(0)
/*单时移时，切台处理
 * FB到头，自动开始播放？
 * 时移满了会咋样
 * */
#define PVR_PATITION_SIZE_GATE    95    //percent 


// return: 0-failed 1-path len
static uint32_t pvr_path_get(char *path, uint32_t max_len)
{
    uint32_t path_len;
    int i;
    char partition[PVR_PATH_LEN] = {0};
    HotplugPartitionList *phpl = NULL;

    phpl = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
    if (phpl->partition_num < 1)
    {
        return 0;
    }

    GxBus_ConfigGet(PVR_PARTITION_KEY, partition, PVR_PATH_LEN, PVR_PARTITION);
    for(i = 0; i < phpl->partition_num; i++)
    {
        if(strcmp(partition, phpl->partition[i].dev_name) == 0)
        {
            break;
        }
    }
    if(i >= phpl->partition_num)
    {
        i = 0;
        GxBus_ConfigSet(PVR_PARTITION_KEY, phpl->partition[i].dev_name);
    }

    // pvr path space
    path_len = ((strlen(phpl->partition[i].partition_entry))+(strlen(PVR_DIR)));

    if (path_len > max_len)
    {
        return 0;
    }

    // set pvr path
    strcpy(path, phpl->partition[i].partition_entry);
    strcat(path, PVR_DIR);

    return path_len;
}

static uint32_t pvr_url_get(char *url, uint32_t max_len)
{
    uint32_t pos = 0;
    char ts_path[20] = {0};
	 GxBusPmDataProg prog_data;

	app_prog_get_playing_pos_in_group(&pos);
	if(GxBus_PmProgGetByPos(pos, 1, &prog_data)==-1)
	{
		return 0;
	}
   GxBus_PmProgUrlGet(&prog_data, (int8_t*)url, GX_PM_MAX_PROG_URL_SIZE);
   switch(DVB_TS_SRC)
   {
	   case 0:
		   sprintf(ts_path, "&sync:%d&tsid:%d&dmxid:%d", 0, DVB_TS_SRC,0);
		   break;
	   case 1:
		   sprintf(ts_path, "&sync:%d&tsid:%d&dmxid:%d", 0, DVB_TS_SRC,0);
		   break;
	   case 2:
		   sprintf(ts_path, "&sync:%d&tsid:%d&dmxid:%d", 0, DVB_TS_SRC,0);
		   break;
	   default:
		   sprintf(ts_path, "&sync:%d&tsid:%d&dmxid:%d", 0, 0,0);
		   break;
   }
   
    strcat(url, ts_path);

    if (strlen(url) > max_len)
    {
        return 0;
    }

    return strlen(url);
}
uint16_t bad_char(uint16_t w)
{
	return (w < 0x0020)
	    || (w == '*') || (w == '?') || (w == '<') || (w == '>')
	    || (w == '|') || (w == '"') || (w == ':') || (w == '/')
	    || (w == '\\');
}

static void pvr_name_format(char *format_name, char *orig_name)
{
    uint32_t i = 0;
    uint32_t name_len = strlen(orig_name);

    if (format_name == NULL || orig_name == NULL)
    {
        return;
    }
	printf("[%#x][%s]name_len =%d\n",(unsigned char)orig_name[0],orig_name,name_len);
    

    for (i=0; i<name_len; i++)
    {
    #if 1
        if (bad_char(*(orig_name+i)))
        {
            *(format_name+i) = ' ';
        }
        else
	#endif
        {
            *(format_name+i) = *(orig_name+i);
        }
    }
    
	printf("[%s]name_len =%d\n",format_name,strlen(format_name));
}

static void pvr_time_format(char * file_name)
{
#define ONE_HOUR   (60*60)
   GxTime time;
	int32_t config = 0;
	int32_t second;
    struct tm *ptm;
    char buf[16];

   GxCore_GetLocalTime(&time);
//	GxBus_ConfigGetInt(TIMEZONE, &config, TIMEZONE_DV);
	config = app_flash_get_config_timezone();

	second = config*3600+time.seconds;

    ptm = localtime((time_t *)&second);

    memset(buf, 0, 16);
    sprintf(buf, "_%d%02d%02d-%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon+1,
            ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    strcat(file_name, buf);
}
 
#define PVR_TYPE_TS     0
#define PVR_TYPE_PS     1
#define PVR_REC_TYPE()  PVR_TYPE_TS
// ret: prog id
//cth
static uint32_t pvr_name_get(char *name, uint32_t max_len)
{
    uint32_t pos = 0;
	GxBusPmDataProg prog_data;
    char tempbufer[256]={0};
	app_prog_get_playing_pos_in_group(&pos);
	if(GxBus_PmProgGetByPos(pos, 1, &prog_data)==-1)
	{
		return 0;
	}
    memset(tempbufer,0,sizeof(tempbufer));
    memcpy(tempbufer,(char*)prog_data.prog_name,strlen((char*)prog_data.prog_name));
#if (1==LINUX_OTT_SUPPORT)
    extern void transform_entry(char * utf8 ,int utf8_len, unsigned char * gb);
    #define LANGUAGE_CHINESE ("Chinese")
    char *osd_language = app_flash_get_config_osd_language();
    if(0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
        char utf8_str_buffer[100]={0};
        unsigned char  gb_str[100]={0};
        int str_len = strlen((char*)prog_data.prog_name);
        memset(gb_str,0,sizeof(gb_str));
        memset(utf8_str_buffer,0,sizeof(utf8_str_buffer));
        if(prog_data.prog_name[0]==0x13)
        {
        	memcpy(gb_str,&prog_data.prog_name[1],str_len-1);
            //printf("11#####%#x\n",gb_str[0]);
            if(isascii(gb_str[0]))
            {
                memcpy(tempbufer,gb_str,strlen((char *)gb_str));
            }
            else
            {
                transform_entry(utf8_str_buffer,sizeof(utf8_str_buffer),gb_str);
                memcpy(tempbufer,utf8_str_buffer,strlen(utf8_str_buffer));
            }
        }
        else
        {
        	memcpy(gb_str,&prog_data.prog_name[0],str_len);
        	//printf("22#####%#x\n",gb_str[0]);
            if(isascii(gb_str[0]))
            {
                memcpy(tempbufer,gb_str,strlen((char *)gb_str));
            }
            else
            {
                transform_entry(utf8_str_buffer,sizeof(utf8_str_buffer),gb_str);
                memcpy(tempbufer,utf8_str_buffer,strlen(utf8_str_buffer));
            }
        }
        
        //transform_entry(utf8_str_buffer,sizeof(utf8_str_buffer),gb_str);
        //memcpy(tempbufer,utf8_str_buffer,strlen(utf8_str_buffer));
        printf("tempbufer=%s\n",tempbufer);
        printf("gb_str=%s\n",gb_str);
        
    }
#endif
    
    pvr_name_format(name, tempbufer);
    pvr_time_format(name);

    if (PVR_REC_TYPE() == PVR_TYPE_TS)
    {
        strcat(name, ".ts.dvr");
    }

    printf("\nPVR name:%s\n",name);
    return prog_data.id;
}

static usb_check_state pvr_usb_check(AppPvrOps* pvr)
{
#define MIN_VALID_SPACE (100) //M

    char env_path[PVR_PATH_LEN] = {0};
    char partition_path[PVR_PATH_LEN] = {0};
    GxDiskInfo disk_info;
    uint32_t free_space;
    usb_check_state state = USB_OK;

    // pvr file path
    if (/*(pvr&&(pvr->usbstate==USB_OUT))||*/0 == pvr_path_get(env_path, PVR_PATH_LEN))
    {
        state = USB_ERROR;
    }
    else
    {
        strncpy(partition_path, env_path, (strlen(env_path)-strlen(PVR_DIR)));
        GxCore_DiskInfo(partition_path, &disk_info);
		if (0 == disk_info.total_size)
    	{
    		return USB_ERROR;
    	}
		
		free_space = disk_info.freed_size/1048576;
		
        if((disk_info.used_size * 100) / disk_info.total_size >= PVR_PATITION_SIZE_GATE)
        {
            state =  USB_NO_SPACE;
        }
        else if (GxCore_FileExists(env_path) == GXCORE_FILE_UNEXIST) 
        {
            if(GxCore_Mkdir(env_path) < 0)
            {
                state =  USB_ERROR;
            }
        }
    }

    return state;
}

static status_t pvr_env_sync(AppPvrOps* pvr)
{
    if (pvr == NULL)  
        return GXCORE_ERROR;

    pvr->env_clear(pvr);

    printf("[sync] 1\n");
    // pvr file path
    CHECK_ALLOC(pvr->env.path, PVR_PATH_LEN);
    if (0 == pvr_path_get(pvr->env.path, PVR_PATH_LEN))
    {
        FREE_NULL(pvr->env.path);
        return GXCORE_ERROR;
    }

    printf("[sync] 2\n");
    if (GxCore_FileExists(pvr->env.path) == GXCORE_FILE_UNEXIST) 
    {
        if(GxCore_Mkdir(pvr->env.path) < 0)
        {
            FREE_NULL(pvr->env.path);
            return GXCORE_ERROR;
        }
    }

    printf("[sync] 3\n");
    // pvr url
    CHECK_ALLOC(pvr->env.url, PVR_URL_LEN);
    if (0 == pvr_url_get(pvr->env.url, PVR_URL_LEN))
    {
        FREE_NULL(pvr->env.path);
        FREE_NULL(pvr->env.url);
        return GXCORE_ERROR;
    }

    printf("[sync] 4\n");
    // pvr rec prog id
    CHECK_ALLOC(pvr->env.name, PVR_NAME_LEN);
    pvr->env.prog_id = pvr_name_get(pvr->env.name, PVR_NAME_LEN);

    printf("[sync] 5\n");
    printf("-------pvr_env_sync\n");
    return GXCORE_SUCCESS;
}

static void pvr_env_clear(AppPvrOps* pvr)
{
    if (pvr == NULL)  return;
    // sync env
    pvr->env.prog_id = 0;
    FREE_NULL(pvr->env.name);
    FREE_NULL(pvr->env.path);
    FREE_NULL(pvr->env.url);

    printf("------pvr_env_clear\n");
}

static void pvr_rec_start(AppPvrOps *pvr)
{
    GxMsgProperty_PlayerRecord rec_start = {0};
    GxTime tick;

    if (pvr == NULL)    
        return;

    // player open
    if (pvr->state == PVR_DUMMY)
    {
        if (GXCORE_SUCCESS != pvr->env_sync(pvr))
        {
            return;
        }

        
        app_epg_close();
        
        rec_start.player = PVR_PLAYER;
        strcpy((char*)rec_start.url, pvr->env.url);
        sprintf((char*)rec_start.file, "%s/%s", pvr->env.path, pvr->env.name);
		printf("pvr record url=%s file=%s\n",rec_start.url,rec_start.file);
        if(0 != app_send_msg(GXMSG_PLAYER_RECORD, (void *)(&rec_start)))    
        {
            app_send_msg(GXMSG_PLAYER_STOP,PVR_PLAYER);
        }

        GxCore_GetTickTime(&tick);
        pvr->time.cur_tick = 0;
        pvr->time.total_tick = 0;
        pvr->time.remaining_tick = 0;

        pvr->state = PVR_RECORD;

    }
    else if (pvr->state == PVR_TIMESHIFT)
    {
       
        rec_start.player = PVR_PLAYER;
        strcpy((char*)rec_start.url, pvr->env.url);
        sprintf((char*)rec_start.file, "%s/%s", pvr->env.path, pvr->env.name);
        printf("pvr timeshift_record url=%s file=%s\n",rec_start.url,rec_start.file);
        if(0 != app_send_msg(GXMSG_PLAYER_RECORD, (void *)(&rec_start)))    
        {
            app_send_msg(GXMSG_PLAYER_STOP,PVR_PLAYER);
        }
        else
        {
            pvr->state = PVR_TMS_AND_REC;
            GxBus_ConfigSet(TMS_FILE_KEY, "abc");
        }
	}
    else
    {}

    // play list lock current tp
}

static void pvr_rec_stop(AppPvrOps *pvr)
{
    if((pvr->state == PVR_RECORD)
        ||(pvr->state == PVR_TMS_AND_REC))
    {
        app_send_msg(GXMSG_PLAYER_STOP,PVR_PLAYER);
    }

    if(pvr->state == PVR_RECORD)
    {
        pvr->env_clear(pvr);
        pvr->state = PVR_DUMMY;
        app_epg_open();
    }
    else if(pvr->state == PVR_TMS_AND_REC)
    {
        pvr->state = PVR_TIMESHIFT;
    }
}


static void pvr_tms_stop(AppPvrOps *pvr)
{
    GxMsgProperty_PlayerTimeShift time_shift;
    memset(&time_shift,0,sizeof(GxMsgProperty_PlayerTimeShift));
    
    if (pvr == NULL)    return;
    printf("---pvr_tms_stop\n");

    time_shift.enable = 0;
    app_send_msg(GXMSG_PLAYER_TIME_SHIFT, (void*)(&time_shift));
    
    if((pvr->state == PVR_TIMESHIFT)
        ||(pvr->state == PVR_TMS_AND_REC))
    {
    	/*
    		* 小内存方案，先停止时移，释放出内存空间。
    		*/
#ifdef MENCENT_FREEE_SPACE
        app_send_msg(GXMSG_PLAYER_STOP,PLAYER_FOR_TIMESHIFT);
#endif
      //  app_send_msg(GXMSG_PLAYER_STOP,PLAYER_FOR_NORMAL);
    }

    if(pvr->state == PVR_TIMESHIFT)
    {
        pvr->env_clear(pvr);
        pvr->state = PVR_DUMMY;
        app_epg_open();
    }
    else if(pvr->state == PVR_TMS_AND_REC)
    {
        pvr->state = PVR_RECORD;
    }
}
static void pvr_tms_delete(AppPvrOps* pvr);
static void pvr_pause(AppPvrOps *pvr)
{
    GxMsgProperty_PlayerPause player_pause;
    GxMsgProperty_PlayerTimeShift time_shift;
    char file_name[PVR_NAME_LEN] = {0};
    GxTime tick;
    memset(&time_shift,0,sizeof(GxMsgProperty_PlayerTimeShift));

    if (pvr == NULL)   return;
    printf("---pvr_pause\n");
    //pvr->spd = PVR_SPD_1;
#if MEDIA_SUBTITLE_SUPPORT
    app_subt_pause();
#endif
    
    if (pvr->state == PVR_DUMMY)
    {
        GxBus_ConfigGet(TMS_FILE_KEY, file_name, PVR_NAME_LEN, "abc");
        if(strcmp(file_name,"abc"))
        {
            pvr_tms_delete(pvr);
        }
        
        // first time init pvr env
        if (GXCORE_SUCCESS != pvr->env_sync(pvr))
        {
            return;
        }

        app_epg_close();

        time_shift.enable = 1;
		time_shift.shift_dmxid = 0;
        sprintf((char*)time_shift.url, "%s/%s", pvr->env.path, pvr->env.name);
		printf("pvr_shift url=%s\n",time_shift.url);
        app_send_msg(GXMSG_PLAYER_TIME_SHIFT, (void*)(&time_shift));
       
        GxBus_ConfigSet(TMS_FILE_KEY, pvr->env.name);

        GxCore_GetTickTime(&tick);
        pvr->time.cur_tick = 0;
        pvr->time.total_tick = 0;
        pvr->time.remaining_tick = 0;

        pvr->state = PVR_TIMESHIFT;
    }
    else if (pvr->state == PVR_RECORD)
    {
        time_shift.enable = 1;
		time_shift.shift_dmxid = 0;
        sprintf((char*)time_shift.url, "%s/%s", pvr->env.path, pvr->env.name);
		printf("pvr_rec_shift url=%s\n",time_shift.url);
        app_send_msg(GXMSG_PLAYER_TIME_SHIFT, (void*)(&time_shift));

        pvr->state = PVR_TMS_AND_REC;        
    }
    else
    {}

    player_pause.player = PLAYER_FOR_NORMAL;
    app_send_msg(GXMSG_PLAYER_PAUSE, (void *)(&player_pause));

}

static void pvr_resume(AppPvrOps *pvr)
{
    if (pvr == NULL)   
        return;

    GxMsgProperty_PlayerResume player_resume;
    player_resume.player = PLAYER_FOR_NORMAL;
    app_send_msg(GXMSG_PLAYER_RESUME, (void *)(&player_resume));
}

static void pvr_seek(AppPvrOps *pvr, int64_t offset)
{
#define SEC_TO_MSEL     (1000)
    GxMsgProperty_PlayerSeek seek;

    seek.player = PLAYER_FOR_NORMAL;
    seek.time = offset*SEC_TO_MSEL;
    seek.flag = SEEK_ORIGIN_SET; 

    app_send_msg(GXMSG_PLAYER_SEEK, (void *)(&seek));
}

static void pvr_play_speed(AppPvrOps *pvr, float speed)
{
    GxMsgProperty_PlayerSpeed player_speed;

    if (pvr == NULL)   return;

    player_speed.player = PLAYER_FOR_NORMAL;
    player_speed.speed = (float)speed;

    //pvr->spd = (int32_t)speed;

    printf("start speed %f", speed);
    app_send_msg(GXMSG_PLAYER_SPEED, (void *)(&player_speed));
}

static void pvr_tms_delete(AppPvrOps* pvr)
{
    char file_path[PVR_PATH_LEN] = {0};
    char file_name[PVR_NAME_LEN] = {0};
    char tms_file[PVR_PATH_LEN+PVR_NAME_LEN] = {0};
	char tms_folder[PVR_PATH_LEN+PVR_NAME_LEN] = {0};
	char ret,length;

    if (pvr->state != PVR_DUMMY)   return;

    printf("---pvr_tms_delete\n");

    if (0 == pvr_path_get(file_path, PVR_PATH_LEN))
    {
        return;
    }

    if (GxCore_FileExists(file_path) == GXCORE_FILE_UNEXIST) 
    {
        // no dir , needn't delete
        return;
    }

    GxBus_ConfigGet(TMS_FILE_KEY, file_name, PVR_NAME_LEN, "abc");

    sprintf(tms_file, "%s/%s", file_path, file_name);
    
    printf("--file_path:%s\n",file_path);
    printf("--file_name:%s\n",file_name);
    printf("--tms_file:%s\n",tms_file);
    
    //if (GxCore_FileExists(pvr->env.path) == GXCORE_FILE_UNEXIST) 
    //{
    //    return;
    //}
    if (GxCore_FileExists(tms_file) == GXCORE_FILE_UNEXIST) 
    {
        // no dir , needn't delete
        return;
    }

	ret = GxCore_FileDelete(tms_file);

	length = strlen(tms_file);
	strncpy(tms_folder,tms_file,(length-7));
    if (GxCore_FileExists(tms_folder) == GXCORE_FILE_UNEXIST) 
    {
        // no dir , needn't delete
        return;
    }
#if DVB_MEDIA_FLAG
    ret = file_edit_delete(tms_folder);
#endif
    GxBus_ConfigSet(TMS_FILE_KEY, "abc");
}

static int32_t pvr_percent_get(AppPvrOps *pvr)
{
    char partition_path[PVR_PATH_LEN] = {0};
    GxDiskInfo disk_info;
    printf("---pvr_percent_get\n");
    if(pvr->env.path != NULL)
    {
        strncpy(partition_path, pvr->env.path, 
                (strlen(pvr->env.path)-strlen(PVR_DIR)));

        GxCore_DiskInfo(partition_path, &disk_info);
        if(disk_info.total_size > 0)
        {
        	return (disk_info.used_size*100)/disk_info.total_size;
        }
    }
    return -1;
}

/*static status_t pvr_check_free_space(AppPvrOps *pvr)
{
	char partition_path[PVR_PATH_LEN] = {0};
    GxDiskInfo disk_info;
	uint32_t free_space;

    strncpy(partition_path, pvr->env.path, 
            (strlen(pvr->env.path)-strlen(PVR_DIR)));

    GxCore_DiskInfo(partition_path, &disk_info);
	free_space = disk_info.freed_size/1048576;
	if(free_space<100)
	{
		return GXCORE_ERROR;
	}
	return GXCORE_SUCCESS;
}*/

AppPvrOps g_AppPvrOps = 
{
    .usbstate       =   USB_OUT,
    .stoppvr_flag   =   0,
    .usb_check      =   pvr_usb_check,
    .env_sync       =   pvr_env_sync,
    .env_clear      =   pvr_env_clear,
    .rec_start      =   pvr_rec_start,
    .rec_stop       =   pvr_rec_stop,
    .tms_stop       =   pvr_tms_stop,
    .pause          =   pvr_pause,
    .resume         =   pvr_resume,
    .seek           =   pvr_seek,
    .speed          =   pvr_play_speed,
    .percent        =   pvr_percent_get,
    .tms_delete     =   pvr_tms_delete,
    //.free_space     =   pvr_check_free_space
};
#endif


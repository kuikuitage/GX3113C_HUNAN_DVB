#include "app.h"
#if 0
#include "app.h"
#include "app_utility.h"
#include "gxhotplug.h"
//#include "app_default_params.h"
#include "module/config/gxconfig.h"
//#include "app_module.h"
#include <math.h>

#define INDEX_LEN 11
#define HOUR_TO_SEC 3600
#define LOCAL_DATE_LEN 40




status_t GetFileContent(const char *pcFilePath,char **ppcOutBuf)
{
	status_t ret=GXCORE_ERROR;//GXCORE_SUCCESS
	uint32_t unFileSize=0;
	handle_t fileHandle=-1;
	GxFileInfo fileInfo={0};
	if(NULL==pcFilePath)return ret;
	
	if(GXCORE_FILE_EXIST!=GxCore_FileExists(pcFilePath))
	{
		printf("file->%s is not exist!!",pcFilePath);
		return ret;
	}
	GxCore_GetFileInfo(pcFilePath,&fileInfo);
	//printf("sFilePath=%s,fileInfo.type=%d\n",sFilePath,fileInfo.type);
	if(fileInfo.type==GX_FILE_REGULAR)
	{
		unFileSize=fileInfo.size_by_bytes;
		
	}else
	{
		printf("file->%s is not a regular!!",pcFilePath);
		return ret;
	}	
	if(NULL==((*ppcOutBuf)=((char *)GxCore_Malloc(unFileSize+1))))
	{
		printf("--GxCore_Malloc  is error--\n");
		return ret;
	}else
	{
		memset(*ppcOutBuf,0,unFileSize+1);
		if(0>=(fileHandle=GxCore_Open(pcFilePath,"a+")))
		{
			printf("--fopen(%s,\"a+\")  is error--\n",pcFilePath);
			goto GETFILECONTENT;
		}else
		{
			if(GxCore_Read(fileHandle,*ppcOutBuf,unFileSize,1)<0)
			{
				printf("--GxCore_Read  is error--\n");
				goto GETFILECONTENT;
			}
			GxCore_Close(fileHandle);
			fileHandle=-1;
		}
		
	}
	
	ret=GXCORE_SUCCESS;
	
GETFILECONTENT:
	if(ret!=GXCORE_SUCCESS)
	{
		if(NULL!=*ppcOutBuf)GxCore_Free(*ppcOutBuf);
		*ppcOutBuf=NULL;
	}
	if(-1!=fileHandle)GxCore_Close(fileHandle);

	return ret;
}


char * StrReplace(const char *pcSrc, const char *pcPattern, const char *pcReplace)
{
   int nDestLen = 1;
   int nPatternLen=0, nReplaceLen=0;
   char * ps;
   char * pt;
   char * pn;
   char * pp;

   if(pcSrc==0 || pcPattern==0 || pcPattern==0) return 0;
   if(pcSrc[0]==0 || pcPattern[0]==0) return 0;

   nPatternLen = strlen(pcPattern);
   nReplaceLen = strlen(pcPattern);

   for(ps=(char *)pcSrc;;)
   {
      pt = strstr(ps, pcPattern);
      if(pt == 0)
      {
         nDestLen += strlen(ps);
         break;
      }
      nDestLen += (pt - ps) + nReplaceLen;
      ps = pt + nPatternLen;
   }

   pn = pp = GxCore_Malloc(nDestLen);
   if(pn == 0) return 0;
   pn[nDestLen-1] = 0;

   for(ps=(char *)pcSrc;;)
   {
      pt = strstr(ps, pcPattern);
      if(pt == 0)
      {
         memcpy(pp, ps, strlen(ps));
         break;
      }

      memcpy(pp, ps, pt - ps);
      pp += pt - ps;
      memcpy(pp, pcPattern, nReplaceLen);
      pp += nReplaceLen;
      ps = pt + nPatternLen;
   }

   return pn;
}

#ifdef LINUX_OS
/* BEGIN: Added by yingc, 2013/10/21 */
int VersionCmp(char* pVersionOld,char* pVersionNew)
{
	int nRet=0;//0:版本相同   1:版本不相同
	FILE* fp=NULL;
	char sVersionOld[32]={0};
	char sVersionNew[32]={0};
	//char buffer[128];

	if(NULL==pVersionOld || NULL==pVersionNew)
	{
		return nRet;
	}	
	if(GXCORE_FILE_EXIST != GxCore_FileExists(pVersionOld))//当old不存在时直接返回正确
	{
		nRet=1;
		return nRet;
	}
	if(GXCORE_FILE_EXIST != GxCore_FileExists(pVersionNew))
	{
		return nRet;
	}
	
	if(((fp=fopen(pVersionOld,"r"))==NULL))
	{
		printf("filename1 error\n");
		return nRet;
	}
	fread(sVersionOld,1,sizeof(sVersionOld),fp);
	fclose(fp);
	fp=NULL;
	if(((fp=fopen(pVersionNew,"r"))==NULL))
	{
		printf("filename2 error\n");
		return nRet;
	}

	fread(sVersionNew,1,sizeof(sVersionNew),fp);
	fclose(fp);
	fp=NULL;

	return strcmp(sVersionOld,sVersionNew);
}


#ifdef LINUX_OS
Md5Sum Md5SumCheck(char *pcDir,char *pcSrcFile,char *pcMd5File)
{
	Md5Sum eRet=MD5_SUM_ERROR;
	char sCmdBuf[CMD_LEN_MAX]={0};
	char sSrcFilePath[128]={0};
	char sMd5FilePath[128]={0};
	char *pcResult=NULL;
	int nResultLen=0,nSrcFileLen=0;

	if(NULL==pcDir||NULL==pcSrcFile || NULL==pcMd5File)
	{
		return eRet;
	}

	snprintf(sSrcFilePath,sizeof(sSrcFilePath),"%s%s",pcDir,pcSrcFile);
	snprintf(sMd5FilePath,sizeof(sMd5FilePath),"%s%s",pcDir,pcMd5File);
	if(GXCORE_FILE_EXIST != GxCore_FileExists(sSrcFilePath) ||GXCORE_FILE_EXIST != GxCore_FileExists(sMd5FilePath))
	{
		return eRet;
	}
	chdir(pcDir);
	snprintf(sCmdBuf,CMD_LEN_MAX,"md5sum -c %s",pcMd5File);
	if( (ExecuteCmd(sCmdBuf,&pcResult)==0))
	{
		if(NULL!=pcResult)
		{	
			nResultLen=strlen(pcResult);
			nSrcFileLen=strlen(pcSrcFile);
			if(nSrcFileLen<nResultLen)
			{
				if(NULL!=strstr(pcResult+nSrcFileLen,"OK"))
				{
					eRet=MD5_SUM_OK;
				}
			}
			else
			{
				goto MD5SUMCHECK;	
			}
			
		}	
		else
		{
			goto MD5SUMCHECK;	
		}
	}
	else
	{
		goto MD5SUMCHECK;	
	}
MD5SUMCHECK:
	GXCORE_FREE(pcResult);
	
	chdir("/");
	return eRet;
}
#endif
/* END:   Added by yingc, 2013/10/21   PN: */
#endif

/*please free the memory by yourself*/
#if 0
char* get_widget_detail_name(char *base_name ,uint32_t index)
{
    char *widget_name = NULL;
    uint32_t len = 0;
    char id[INDEX_LEN] = {0};

    if(NULL == base_name)
        return NULL;
    len = strlen(base_name);

    widget_name = GxCore_Malloc((len+INDEX_LEN)*sizeof(char));
    if(NULL != widget_name)
    {
        memset(widget_name,0,(len+INDEX_LEN)*sizeof(char));
        sprintf(id,"%d",index+1);
        id[INDEX_LEN-1] = '\0';
        strcpy(widget_name,base_name);
        strcat(widget_name,id);
    }
    return widget_name;
}

char *get_str_duplication(char *src)
{
    char *desStr = NULL;
    uint32_t strLen = 0;
    if(NULL == src)
        return NULL;

    strLen = strlen(src);

    desStr = GxCore_Malloc((strLen+1)*sizeof(char));
    if(NULL != desStr)
    {
        memset(desStr,0,(strLen+1)*sizeof(char));
        strcpy(desStr,src);
        desStr[strLen] = '\0';
    }
    
    return desStr;
    
}

#endif

/*please GxCore_Free the memory by yourself*/
char* get_string_strcat(char *src,char *dst,char *separator)
{
	int src_len = 0;
	int des_len = 0;
	int total_len = 0;
	int sep_len = 0;
	
	char *str = NULL;
	
	if(NULL != src)
		src_len = strlen(src);

	if(NULL != dst)
		des_len = strlen(dst);
	
	if(NULL != separator)
		sep_len = strlen(separator);
	

	total_len = src_len + sep_len+des_len+1;

	str = GxCore_Malloc(total_len*sizeof(char));
	if(NULL != str)
	{
		memset(str,0,total_len*sizeof(char));

		if(NULL != src)
			strcat(str,src);
		if(NULL != separator)
			strcat(str,separator);
		if(NULL != dst)
			strcat(str,dst);

		str[total_len-1] = '\0';
	}

	return str;
}

time_t get_local_time_by_timezone(time_t src_sec)
{
    int init_value = TIME_ZONE_VALUE;
    GxBus_ConfigGetInt(TIME_ZONE, &init_value, TIME_ZONE_VALUE);
    if(init_value < -12)
        init_value = -12;
    if(init_value > 12)
        init_value = 12;

    src_sec -= HOUR_TO_SEC * init_value;

    GxBus_ConfigGetInt(TIME_SUMMER, &init_value, TIME_SUMMER_VALUE);    

    return src_sec-HOUR_TO_SEC*init_value;
}

time_t get_display_time_by_timezone(time_t src_sec)
{
    int init_value = TIME_ZONE_VALUE;
    GxBus_ConfigGetInt(TIME_ZONE, &init_value, TIME_ZONE_VALUE);
    if(init_value < -12)
        init_value = -12;
    if(init_value > 12)
        init_value = 12;

   src_sec += HOUR_TO_SEC * init_value;

   GxBus_ConfigGetInt(TIME_SUMMER, &init_value, TIME_SUMMER_VALUE);

    return src_sec+HOUR_TO_SEC*init_value;
}

WeekDay app_weekday_local2gmt(WeekDay local_weekday, time_t local_day_sec)
{
    int init_value = TIME_ZONE_VALUE;
    WeekDay weekday_ret = local_weekday;
    int temp_sec;
    
    GxBus_ConfigGetInt(TIME_ZONE, &init_value, TIME_ZONE_VALUE);
    temp_sec = local_day_sec - init_value * HOUR_TO_SEC;
    GxBus_ConfigGetInt(TIME_SUMMER, &init_value, TIME_SUMMER_VALUE);
    temp_sec -= init_value * HOUR_TO_SEC;
    if(temp_sec < 0) //gmt+n
    {
        if(weekday_ret == WEEK_SUN)
        {
            weekday_ret = WEEK_SAT;
        }
        else
        {
            weekday_ret--; 
        }
    }
    else if(temp_sec >= 24 * HOUR_TO_SEC) //gmt-n
    {
         if(weekday_ret == WEEK_SAT)
        {
            weekday_ret = WEEK_SUN;
        }
        else
        {
            weekday_ret++; 
        }
    }

    return weekday_ret;
}

WeekDay app_weekday_gmt2local(WeekDay gmt_weekday, time_t gmt_day_sec)
{
    int init_value = TIME_ZONE_VALUE;
    WeekDay weekday_ret = gmt_weekday;
    int temp_sec;
    
    GxBus_ConfigGetInt(TIME_ZONE, &init_value, TIME_ZONE_VALUE);
    temp_sec = gmt_day_sec + init_value * HOUR_TO_SEC;
    GxBus_ConfigGetInt(TIME_SUMMER, &init_value, TIME_SUMMER_VALUE);
    temp_sec += init_value * HOUR_TO_SEC;
    
    if(temp_sec >= 24 * HOUR_TO_SEC)//gmt+n
    {
         if(weekday_ret == WEEK_SAT)
        {
            weekday_ret = WEEK_SUN;
        }
        else
        {
            weekday_ret++; 
        }
    }
    else if(temp_sec < 0)//gmt-n
    {
        if(weekday_ret == WEEK_SUN)
        {
            weekday_ret = WEEK_SAT;
        }
        else
        {
            weekday_ret--; 
        }
    }

    return weekday_ret;
}

char* get_local_date(char *date_value,uint32_t data_len)
{// data_len must over 17
    GxTime local_time;
    struct tm *ptm = NULL;
    time_t time_sec = 0;

    if(NULL == date_value || LOCAL_DATE_LEN > data_len)
        return NULL;
    
    GxCore_GetLocalTime(&local_time);
    time_sec = get_display_time_by_timezone((time_t)(local_time.seconds));
    ptm = localtime(&time_sec);

    if(NULL != ptm)
    {
        memset(date_value,0,data_len*sizeof(char));
        sprintf(date_value,"%d/%02d/%02d %02d:%02d", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min);
        date_value[data_len-1] = 0;
        return date_value;
    }
    return NULL;
}

bool check_usb_status(void)
{
    HotplugPartitionList* list = NULL;
    list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
    
    if((NULL != list)
        && (HOTPLUG_TYPE_USB == list->type)
        && (0 < list->partition_num))
    {
        return TRUE;
    }
    return FALSE;
}

uint32_t get_usb_partition_space(char *partition_path)
{
	if(partition_path == NULL)
		return 0;

	GxDiskInfo disk_info;
	if(GxCore_DiskInfo(partition_path, &disk_info) == GXCORE_ERROR)
		return 0;
		
	return disk_info.freed_size;
}

//free result extern, url:"/mnt/usbx_x/xxx"
char *get_usb_url_partition(char *file_url)
{
	char *p = file_url;
	char *res = NULL;
	int cnt = 0;
	int len = 0;
	
	if(p == NULL)
		return NULL;

	while(*p != '\0')
	{
		if(*p == '/')
		{
			cnt++;
			if(cnt >= 3)
				break;
		}
		p++;
	}

	if(cnt < 2)
		return NULL;

	len = p - file_url;
	res = (char*)GxCore_Calloc(len + 1, 1);
	if(res != NULL)
	{
		strncpy(res, file_url, len);
	}

	return res;
}

/*please GxCore_Free the memory by yourself*/
char *app_time_to_date_edit_str(time_t time)
{
#define DATE_STR_LEN 16
	char *string = NULL;
	struct tm *temp_tm = NULL;

	string = (char *)GxCore_Malloc(DATE_STR_LEN);
	if(string != NULL)
	{
		memset(string, 0, DATE_STR_LEN);
		temp_tm = localtime(&time);
		sprintf(string, "%d/%02d/%02d", temp_tm->tm_year + 1900, temp_tm->tm_mon + 1, temp_tm->tm_mday);
	}
	return string;
}

/*please GxCore_Free the memory by yourself*/
char *app_time_to_hourmin_edit_str(time_t time)
{
#define HM_STR_LEN 16
	char *string = NULL;
	struct tm *temp_tm = NULL;

	string = (char *)GxCore_Malloc(HM_STR_LEN);
	if(string != NULL)
	{
		memset(string, 0, HM_STR_LEN);
		temp_tm = localtime(&time);
		sprintf(string, "%02d:%02d", temp_tm->tm_hour , temp_tm->tm_min);
	}
	return string;
}

/*please GxCore_Free the memory by yourself*/
char *app_time_to_hms_str(time_t time)
{
#define HMS_STR_LEN 16
	char *string = NULL;
	struct tm *temp_tm = NULL;

	string = (char *)GxCore_Malloc(HMS_STR_LEN);
	if(string != NULL)
	{
		memset(string, 0, HMS_STR_LEN);
		temp_tm = localtime(&time);
		sprintf(string, "%02d:%02d:%02d", temp_tm->tm_hour , temp_tm->tm_min, temp_tm->tm_sec);
	}
	return string;
}

/*please GxCore_Free the memory by yourself*/
char *app_time_to_format_str(time_t time)
{
#define TM_STR_LEN 64

    char *string = NULL;
    struct tm *temp_tm = NULL;
    
    string = (char *)GxCore_Malloc(TM_STR_LEN);
    if(string != NULL)
    {
        memset(string, 0, TM_STR_LEN);
        temp_tm = localtime(&time);
        sprintf(string, "%d/%02d/%02d %02d:%02d:%02d", 
                    temp_tm->tm_year + 1900, temp_tm->tm_mon + 1, temp_tm->tm_mday, temp_tm->tm_hour , temp_tm->tm_min, temp_tm->tm_sec);
    }

    return string;
}

status_t  app_edit_str_to_date_sec(char *date_str, time_t *res_sec)
{
	char year_str[5];
	char month_str[5];
	char day_str[5];
	char *p = NULL;
	int i;
	//struct tm temp_tm;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	status_t ret = GXCORE_ERROR;
	
	if(date_str == NULL) 
	{
		return GXCORE_ERROR;
	}

	p = date_str;
	i = 0;
	memset(year_str, 0, sizeof(year_str));
	while((p != NULL) && (*p != '\0')  && (*p != '/'))
	{
		year_str[i++] = *p++;
	}
	p++; // '/'
	i = 0;
	memset(month_str, 0, sizeof(month_str));
	while((p != NULL) && (*p != '\0') && (*p != '/'))
	{
		month_str[i++] = *p++;
	}

	p++; //'/'
	i = 0;
	memset(day_str, 0, sizeof(day_str));
	while((p != NULL) && (*p != '\0'))
	{
		day_str[i++] = *p++;
	}

	year = atoi(year_str);
	month = atoi(month_str);
	day = atoi(day_str);
	if(app_month_date_valid(year, month, day) == true)
	{
		//temp_tm.tm_year = year - 1900;
		//temp_tm.tm_mon = month - 1;
		//temp_tm.tm_mday = day;
		//temp_tm.tm_hour = 0;
		//temp_tm.tm_min = 0;
		//temp_tm.tm_sec = 0;

		*res_sec = app_mktime(year, month, day, 0, 0, 0);
		ret = GXCORE_SUCCESS;
	}
	return ret;
}

time_t app_edit_str_to_hourmin_sec(char *time_str)
{
	char hour_str[5];
	char min_str[5];
	time_t hour;
	time_t min;
	char *p = NULL;
	int i;
	
	if(time_str == NULL) 
	{
		return 0;
	}
	p = time_str;
	i = 0;
	memset(hour_str, 0, sizeof(hour_str));
	while((p != NULL) && (*p != '\0')  && (*p != ':'))
	{
		hour_str[i++] = *p++;
	}
	p++; // ':'
	i = 0;
	memset(min_str, 0, sizeof(min_str));
	while((p != NULL) && (*p != '\0') )
	{
		min_str[i++] = *p++;
	}

	hour = atoi(hour_str);
	min = atoi(min_str);

	return hour * 3600 + min *60;
}

float app_float_edit_str_to_value(const char *str)
{
	const char *cp;
	float data;
	char chHasDot=0;

	for (cp = str, data = 0; *cp != 0; ++cp) 
	{
		if (*cp >= '0' && *cp <= '9')
		{
			data = data * 10 + *cp - '0';
			if (chHasDot)
			{
				chHasDot ++; 
			}
		}
		else if(*cp == '.')
		{
			chHasDot = 1;
		}
		else
			break;
	}
	if(chHasDot>1)
	{
		data /=(10*(chHasDot-1));
	}
	return data;
}

status_t app_signal_progbar_update(uint32_t tuner, SignalBarWidget *bar, SignalValue *val)
{
#define PROGBAR_STRENGTH_L "s_progress3_l.bmp"
#define PROGBAR_STRENGTH_M "s_progress3_m.bmp"
#define PROGBAR_STRENGTH_R "s_progress3_r.bmp"

#define PROGBAR_QUALITY_L "s_progress3_l_green.bmp"
#define PROGBAR_QUALITY_M "s_progress3_m_green.bmp"
#define PROGBAR_QUALITY_R "s_progress3_r_green.bmp"

#define PROGBAR_UNLOCKED_L "s_progress3_l_red.bmp"
#define PROGBAR_UNLOCKED_M "s_progress3_m_red.bmp"
#define PROGBAR_UNLOCKED_R "s_progress3_r_red.bmp"
    AppFrontend_LockState lock;

	if((bar == NULL) || (bar->strength_bar == NULL) || (bar->quality_bar== NULL))
		return GXCORE_ERROR;

    app_ioctl(tuner, FRONTEND_LOCK_STATE_GET, &lock);
	if(lock == FRONTEND_LOCKED) //锁定检查
	{
		GUI_SetProperty(bar->strength_bar, "fore_image_l", PROGBAR_STRENGTH_L);
		GUI_SetProperty(bar->strength_bar, "fore_image_m", PROGBAR_STRENGTH_M);
		GUI_SetProperty(bar->strength_bar, "fore_image_r", PROGBAR_STRENGTH_R);

		GUI_SetProperty(bar->quality_bar, "fore_image_l", PROGBAR_QUALITY_L);
		GUI_SetProperty(bar->quality_bar, "fore_image_m", PROGBAR_QUALITY_M);
		GUI_SetProperty(bar->quality_bar, "fore_image_r", PROGBAR_QUALITY_R);
	}
	else
	{
		GUI_SetProperty(bar->strength_bar, "fore_image_l", PROGBAR_UNLOCKED_L);
		GUI_SetProperty(bar->strength_bar, "fore_image_m", PROGBAR_UNLOCKED_M);
		GUI_SetProperty(bar->strength_bar, "fore_image_r", PROGBAR_UNLOCKED_R);

		GUI_SetProperty(bar->quality_bar, "fore_image_l", PROGBAR_UNLOCKED_L);
		GUI_SetProperty(bar->quality_bar, "fore_image_m", PROGBAR_UNLOCKED_M);
		GUI_SetProperty(bar->quality_bar, "fore_image_r", PROGBAR_UNLOCKED_R);
	}

	// strength value
	GUI_SetProperty(bar->strength_bar, "value", &(val->strength_val));

	// progbar quality
	GUI_SetProperty(bar->quality_bar, "value", &(val->quality_val));

	return GXCORE_SUCCESS;
}



uint32_t app_hexstr2value(uint8_t* string)
{
    int ret = 0;
    char *pstr = NULL;

    if(NULL == string)
    {
        return 0;
    }

    //hex
    if(strlen((char*)string) > 2)
    {
        if('0' == string[0] && 'x' == string[1])
        {
            pstr = (char*)&string[2];
            while(*pstr)
            {
                if (*pstr>='0' && *pstr<='9')
                {
                    ret=ret * 16 + ((int)*pstr-'0');
                }
                else
                {
                    if (*pstr>='A' && *pstr<='F')
                    {
                        ret=ret * 16 +((int)*pstr-'A'+10);
                    }
                    else if (*pstr>='a' && *pstr<='f')
                    {
                        ret=ret *16 + ((int)*pstr-'a'+10);
                    }
                }
                pstr++;
            }
            return ret;
        }
    }

    //dec
    pstr = (char*)string;
    while(*pstr)
    {
        if (*pstr>='0' && *pstr<='9')
        {
            ret=ret * 10 + ((int)*pstr-'0');
        }
        else
        {
            printf("[panel keymap]_hexstr2value error '%s'\n", string);
            return 0;
        }
        pstr++;
    }

    return (ret);
}

uint32_t app_bcd2value(uint32_t bcd)
{
	uint32_t ret = 0;
	uint8_t x = 0;
	uint8_t i = 0;

	while(bcd > 0)
	{
		x = bcd & 0xf;
		ret = x * pow(10, i) + ret;
		bcd >>= 4;
		i++;
	}

	return ret;
}

void app_getvalue(const char* url, const char* item, int32_t *retVal)
{
#define VAL_MAX 32

	char* ptr1 = NULL;
	char* ptr2 = NULL;
	char value[VAL_MAX];

	if(url && item){
		ptr1 = strstr(url,item);
		if(ptr1){
			ptr1 += (strlen(item)+1);
			ptr2 = strstr(ptr1,"&");
			if(ptr2 == NULL)
				ptr2 =(char*)(url+ strlen(url));
			if(ptr2-ptr1 > VAL_MAX)
			{
				printf("[ERROR]:(%s)-->(%s) too long !...\n",__func__, item);
				return ;
			}
			memcpy(value,ptr1,ptr2-ptr1);
			value[ptr2-ptr1] = '\0';
			*retVal = atoi((char*)value);
			return ;
		}
	}
	*retVal = -1;
}

uint32_t app_key_full_code(uint16_t key_code)
{
	uint8_t syscode;
	uint8_t syscode_r;
	uint8_t usrcode;
	uint8_t usrcode_r;
	uint32_t full_code;

	if(key_code == 0)
		return 0;

	syscode = (key_code >> 8) & 0xff;
	syscode_r = ~syscode;
	usrcode = key_code & 0xff;
	usrcode_r = ~usrcode;

	full_code = syscode << 24;
	full_code |= syscode_r << 16;
	full_code |= usrcode << 8;
	full_code |= usrcode_r;

	return full_code;
}




#ifdef LINUX_OS

/* BEGIN: Added by yingc, 2013/8/17 */


int GetFileSize(char * filepath,long *filesize)
{
	int rv = 0;
	FILE *fp=NULL;

	if((fp=fopen(filepath,"a+"))==NULL)
	{
		return -1; 
	}
	else
	{
		fseek(fp,0L,SEEK_END);
		*filesize=ftell(fp);
	}   
	fclose(fp);

	return rv; 
}
#if 0/* BEGIN: Deleted by yingc, 2013/8/17 */
int ExecuteCmd(const char *pc_cmd, char **ppc_result)   
{  
	int ret=0;

	long ln_filesize=0;
	int n_len=0;
	FILE *fp=NULL;   
	char s_buf[1024]={0};   

	if(NULL==pc_cmd || strlen(pc_cmd)==0)
	{
		//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
		ret=-1;
		goto EXECUTECMD;
	}


	if((fp=popen(pc_cmd, "r"))!=NULL)   
	{
		while(fgets(s_buf, sizeof(s_buf)-1, fp)!=NULL)   
		{   
			printf("\ns_buf=%s--\n",s_buf);
			memset(s_buf,0,sizeof(s_buf)); 
		}   
		pclose(fp);   
		fp = NULL;   
	}   
	else  
	{   
		//COMM_LOG(NULL,LOG_ERROR,"%s","fp  is null ");
		ret=-1;
		goto EXECUTECMD;
	}   


EXECUTECMD:
	if(ret<0)//说明出错了
	{
		if(NULL!=(*ppc_result))free((*ppc_result));
		*ppc_result=NULL;
	}
	return ret;
}  
#endif/* END:   Deleted by yingc, 2013/8/17   PN: */
#if 0/* BEGIN: Deleted by yingc, 2013/8/17 */
int ExecuteCmd(const char *pc_cmd, char **ppc_result)   
{  
	int ret=0;

	long ln_filesize=0;
	int n_len=0;
	FILE *fp=NULL;   
	char s_buf[1024]={0};   

	if(NULL==pc_cmd || strlen(pc_cmd)==0)
	{
		//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
		ret=-1;
		goto EXECUTECMD;
	}


	if((fp=popen(pc_cmd, "r"))!=NULL)   
	{
		fseek(fp,0L,SEEK_END);
		ln_filesize=ftell(fp);	
		//COMM_LOG(NULL,LOG_DEBUG,"pc_cmd=%s;ln_filesize=%d",pc_cmd,ln_filesize);	
		if(ln_filesize<=0)
		{
			ret=-1;
			goto EXECUTECMD;
		}
		else
		{
			if(NULL==((*ppc_result)=malloc(ln_filesize)))
			{
				//COMM_LOG(NULL,LOG_ERROR,"%s","malloc  is error ");	
				ret=-1;
				goto EXECUTECMD;
			}
			memset((*ppc_result),0,ln_filesize);
			fseek(fp,0L,SEEK_SET);
			while(fgets(s_buf, sizeof(s_buf)-1, fp)!=NULL)   
			{   
				n_len+=snprintf((*ppc_result)+n_len,ln_filesize-n_len,"%s",s_buf);
				memset(s_buf,0,sizeof(s_buf)); 
			}   
			pclose(fp);   
			fp = NULL;   
		}
	}   
	else  
	{   
		//COMM_LOG(NULL,LOG_ERROR,"%s","fp  is null ");
		ret=-1;
		goto EXECUTECMD;
	}   


EXECUTECMD:
	if(ret<0)//说明出错了
	{
		if(NULL!=(*ppc_result))free((*ppc_result));
		*ppc_result=NULL;
	}
	return ret;
}  
#endif/* END:   Deleted by yingc, 2013/8/17   PN: */



#if 0/* BEGIN: Deleted by yingc, 2013/8/21 */
int ExecuteCmd(const char *pc_cmd, char **ppc_result)   
{  
	int ret=0;

	long ln_filesize=0;
	int n_len=0,n_retry_count=0,n_random_no=0,n_random_count=5;
	FILE *fp=NULL,*tempfp=NULL;   
	char s_buf[1024]={0};   
	char s_cmd_tmp[300]={0};
	char s_file_tmp[50]={0};

	if(NULL==pc_cmd || strlen(pc_cmd)==0)
	{
		//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
		ret=-1;
		goto EXECUTECMD;
	}

	snprintf(s_cmd_tmp,sizeof(s_cmd_tmp),"%s",pc_cmd);


	if((fp=popen(s_cmd_tmp, "r"))!=NULL)   
	{

		if(NULL==(tempfp = tmpfile()))
		{
			//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
			ret=-1;
			goto EXECUTECMD;
		}
		else
		{
			while(fgets(s_buf, sizeof(s_buf)-1, fp)!=NULL)   
			{   
				fputs(s_buf,tempfp);
				memset(s_buf,0,sizeof(s_buf)); 
			}   
		}
		pclose(fp);   
		fp = NULL;  

		fseek(tempfp,0L,SEEK_SET);
		fseek(tempfp,0L,SEEK_END);
		ln_filesize=ftell(tempfp);	//只要有命令，就应该会有正确或错误输出，若没有输出的话，可能串口打印有问题，
		//此时可分析该cmd，将其输出重定向到一个临时的随机文件当中
		//COMM_LOG(NULL,LOG_DEBUG,"s_cmd_tmp=%s;ln_filesize=%d",s_cmd_tmp,ln_filesize);	

		if(ln_filesize<0)
		{
			ret=-1;
			goto EXECUTECMD;
		}
		else if(ln_filesize==0)
		{
#if 0/* BEGIN: Deleted by yingc, 2013/8/17 */
			if(n_retry_count>0)
			{
				//COMM_LOG(NULL,LOG_DEBUG,"%s","");
				ret=-1;
				goto EXECUTECMD;
			}
#endif/* END:   Deleted by yingc, 2013/8/17   PN: */
			memset(s_buf,0,sizeof(s_buf)); 
			ln_filesize=0;
			if(NULL!=tempfp)fclose(tempfp);
			if(NULL!=fp)pclose(fp);
			tempfp=NULL;
			fp=NULL;	

			memset(s_cmd_tmp,0,sizeof(s_cmd_tmp)); 

			srand( (unsigned)time( NULL ) ); 

			while(n_random_count--)
			{
				n_random_no = rand();
				n_random_no = n_random_no%200;	
				snprintf(s_file_tmp,sizeof(s_file_tmp),"/tmp/cmd_tmpfile_%d",n_random_no);

				if(GXCORE_FILE_EXIST != GxCore_FileExists(s_file_tmp))/*  如何没有使用  */
				{
					tempfp = fopen(s_file_tmp, "w+"); //新建一个可写的文件
					//unlink(s_file_tmp);//减去引用计数,临时文件可自动删除
					break;
				}
				continue;
			}
			if(n_random_count<=0)
			{
				//COMM_LOG(NULL,LOG_DEBUG,"%s","");
				ret=-1;
				goto EXECUTECMD;
			}
			snprintf(s_cmd_tmp,sizeof(s_cmd_tmp),"%s 2>%s 1>%s",pc_cmd,s_file_tmp,s_file_tmp);

			//n_retry_count++;

			if((fp=popen(s_cmd_tmp, "r"))!=NULL)   
			{
				while(fgets(s_buf, sizeof(s_buf)-1, fp)!=NULL)   
				{   
					fputs(s_buf,tempfp);
					memset(s_buf,0,sizeof(s_buf)); 
				}   	

				pclose(fp);   
				fp = NULL;  

				fclose(tempfp);   
				tempfp = NULL;   

				tempfp = fopen(s_file_tmp, "r");

				fseek(tempfp,0L,SEEK_SET);
				fseek(tempfp,0L,SEEK_END);
				ln_filesize=ftell(tempfp);	//只要有命令，就应该会有正确或错误输出，若没有输出的话，可能串口打印有问题，
				//此时可分析该cmd，将其输出重定向到一个临时的随机文件当中
				//COMM_LOG(NULL,LOG_DEBUG,"s_cmd_tmp=%s;ln_filesize=%d",s_cmd_tmp,ln_filesize);	

				if(ln_filesize<0)
				{
					ret=-1;
					//COMM_LOG(NULL,LOG_DEBUG,"%s","");
					goto EXECUTECMD;
				}
				if(ln_filesize=0)
				{
					ret=0;//如果其长度还为0的话，则该命令本身就是没有任何输出的
					//COMM_LOG(NULL,LOG_DEBUG,"%s","");
					goto EXECUTECMD;
				}				

			}
			else
			{
				//COMM_LOG(NULL,LOG_ERROR,"%s","fp  is null ");
				ret=-1;
				goto EXECUTECMD;
			}

		}

		//COMM_LOG(NULL,LOG_DEBUG,"%s","");
		ln_filesize=ln_filesize+1;
		if(NULL==((*ppc_result)=malloc(ln_filesize)))
		{
			//COMM_LOG(NULL,LOG_ERROR,"%s","malloc  is error ");	
			ret=-1;
			goto EXECUTECMD;
		}
		//COMM_LOG(NULL,LOG_DEBUG,"%s","");
		memset((*ppc_result),0,ln_filesize);
		fseek(tempfp,0L,SEEK_SET);
		memset(s_buf,0,sizeof(s_buf)); 
		n_len=0;
		//COMM_LOG(NULL,LOG_DEBUG,"%s","");
		while(fgets(s_buf, sizeof(s_buf)-1, tempfp)!=NULL)   
		{   
			n_len+=snprintf((*ppc_result)+n_len,ln_filesize-n_len,"%s",s_buf);
			//COMM_LOG(NULL,LOG_DEBUG,"*ppc_result=%s;ln_filesize=%d;n_len=%d;",*ppc_result,ln_filesize,n_len);
			//COMM_LOG(NULL,LOG_DEBUG,"s_buf=%s",s_buf);
			memset(s_buf,0,sizeof(s_buf)); 

		}   
		//COMM_LOG(NULL,LOG_DEBUG,"%s","");
		fclose(tempfp);   
		tempfp = NULL;   
	}   
	else  
	{   
		//COMM_LOG(NULL,LOG_ERROR,"%s","fp  is null ");
		ret=-1;
		goto EXECUTECMD;
	}   


EXECUTECMD:
	if(ret<0)//说明出错了
	{
		if(NULL!=(*ppc_result))free((*ppc_result));
		*ppc_result=NULL;
	}
	if(NULL!=tempfp)fclose(tempfp);
	if(NULL!=fp)pclose(fp);
	tempfp=NULL;
	fp=NULL;
#if 0/* BEGIN: Deleted by yingc, 2013/8/20 */
	if(GXCORE_FILE_EXIST == GxCore_FileExists(s_file_tmp))/*  删除产生的临时文件  */
	{
		GxCore_FileDelete(s_file_tmp);
	}
#endif/* END:   Deleted by yingc, 2013/8/20   PN: */

	//COMM_LOG(NULL,LOG_DEBUG,"%s","");
	return ret;
}  
#endif/* END:   Deleted by yingc, 2013/8/21   PN: */

#ifdef LINUX_OS
int ExecuteCmd(const char *pc_cmd, char **ppc_result)   
{  //此函数中不能使用COMM_LOG，否则可能出现递归调用(若comm_log守护进程起来的话，才可以使用)
	int ret=0;

	long ln_filesize=-1;
	int n_len=0,n_random_no=0,n_random_count=5;
	FILE *fp=NULL,*tempfp=NULL;   
	char s_buf[1024]={0};   
	char s_cmd_tmp[300]={0};
	char s_file_tmp[50]={0};

	if(NULL==pc_cmd || strlen(pc_cmd)==0)
	{
		//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
		ret=-1;
		goto EXECUTECMD;
	}

	memset(s_cmd_tmp,0,sizeof(s_cmd_tmp)); 

	srand( (unsigned)time( NULL ) ); 

	while(n_random_count--)
	{
		n_random_no = rand();
		n_random_no = n_random_no%200;	
		snprintf(s_file_tmp,sizeof(s_file_tmp),"/tmp/cmd_tmpfile_%d",n_random_no);

		if(GXCORE_FILE_EXIST != GxCore_FileExists(s_file_tmp))/*  如何没有使用  */
		{
			tempfp = fopen(s_file_tmp, "w+"); //新建一个可写的文件
			//unlink(s_file_tmp);//减去引用计数,临时文件可自动删除
			break;
		}
		continue;
	}
	if(n_random_count<=0)
	{
		//COMM_LOG(NULL,LOG_DEBUG,"%s","");
		ret=-1;
		goto EXECUTECMD;
	}
	snprintf(s_cmd_tmp,sizeof(s_cmd_tmp),"%s 2>%s 1>%s",pc_cmd,s_file_tmp,s_file_tmp);		

	if((fp=popen(s_cmd_tmp, "r"))!=NULL)   
	{
		while(fgets(s_buf, sizeof(s_buf)-1, fp)!=NULL)   
		{   
			fputs(s_buf,tempfp);
			memset(s_buf,0,sizeof(s_buf)); 
		}   	

		pclose(fp);   
		fp = NULL;  
		fclose(tempfp);   
		tempfp = NULL;   	

		if(0>GetFileSize(s_file_tmp,&ln_filesize))
		{
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			ret=-1;
			goto EXECUTECMD;
		}

		//COMM_LOG(NULL,LOG_DEBUG,"s_cmd_tmp=%s;ln_filesize=%d",s_cmd_tmp,ln_filesize);	

		if(ln_filesize<0)
		{
			ret=-1;
			goto EXECUTECMD;
		}
		else if(ln_filesize==0)
		{
			ret=0;//如果其长度还为0的话，则该命令本身就是没有任何输出的
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			goto EXECUTECMD;
		}
		else
		{
			tempfp = fopen(s_file_tmp, "r"); 
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			ln_filesize=ln_filesize+1;
			if(NULL==((*ppc_result)=GxCore_Malloc(ln_filesize)))
			{
				//COMM_LOG(NULL,LOG_ERROR,"%s","malloc  is error ");	
				ret=-1;
				goto EXECUTECMD;
			}
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			memset((*ppc_result),0,ln_filesize);
			memset(s_buf,0,sizeof(s_buf)); 
			n_len=0;
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			while(fgets(s_buf, sizeof(s_buf)-1, tempfp)!=NULL)   
			{   
				n_len+=snprintf((*ppc_result)+n_len,ln_filesize-n_len,"%s",s_buf);
				//COMM_LOG(NULL,LOG_DEBUG,"*ppc_result=%s;ln_filesize=%d;n_len=%d;",*ppc_result,ln_filesize,n_len);
				//COMM_LOG(NULL,LOG_DEBUG,"s_buf=%s",s_buf);
				memset(s_buf,0,sizeof(s_buf)); 

			}   
			//COMM_LOG(NULL,LOG_DEBUG,"%s","");
			fclose(tempfp);   
			tempfp = NULL;   			
		}


	}   
	else  
	{   
		//COMM_LOG(NULL,LOG_ERROR,"%s","fp  is null ");
		ret=-1;
		goto EXECUTECMD;
	}   


EXECUTECMD:
	if(ret<0)//说明出错了
	{
		if(NULL!=(*ppc_result))GxCore_Free((*ppc_result));
		*ppc_result=NULL;
	}
	if(NULL!=tempfp)fclose(tempfp);
	if(NULL!=fp)pclose(fp);
	tempfp=NULL;
	fp=NULL;

	if(GXCORE_FILE_EXIST == GxCore_FileExists(s_file_tmp))/*  删除产生的临时文件  */
	{
		GxCore_FileDelete(s_file_tmp);
	}

	//COMM_LOG(NULL,LOG_DEBUG,"%s","");
	return ret;
}  

int ExecuteCmdSyncNoQuery(const char *pc_cmd)   
{  
	int ret=-1;

	if(NULL==pc_cmd || strlen(pc_cmd)==0)
	{
		//COMM_LOG(NULL,LOG_ERROR,"%s is null ",pc_cmd);	
		ret=-1;
		goto EXECUTECMDSYNCNOQUERY;
	}

	ret=system(pc_cmd);
	if(-1==ret || ret==127)   
	{
		//COMM_LOG(NULL,LOG_ERROR,"system  is error  pc_cmd pc_cmd=%s;ret=%d",pc_cmd,ret);
		ret=-1;
		goto EXECUTECMDSYNCNOQUERY;
	}   

EXECUTECMDSYNCNOQUERY:

	//COMM_LOG(NULL,LOG_DEBUG,"%s","");
	return ret;
}  
#endif

/* END:   Added by yingc, 2013/8/17   PN: */

/* BEGIN: Added by yingc, 2013/8/29 */
int GetChrCount(const char *pc_str,char c_ch)   
{  
	int n_count=0,i=0,n_len=0;

	if(NULL==pc_str || (n_len=strlen(pc_str))==0)
	{
		return n_count;
	}

	for(i=0;i<n_len;i++)
	{
		if(c_ch==pc_str[i])n_count++;
	}

	return n_count;
}  


/* END:   Added by yingc, 2013/8/29   PN: */

#endif
#endif
/*BEGIN: add for virtualkey find*/
 int find_virtualkey_ex(unsigned int scan_code, unsigned int sym)
{
	if (scan_code == 0)
	{
		return sym;
	}

	return find_virtualkey(scan_code);
}
/*BEGIN: add for virtualkey find, 20131105, z.z.r*/


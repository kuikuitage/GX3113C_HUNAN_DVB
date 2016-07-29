#include "pmp_tags.h"
#include "gxbus.h"
//#include "app.h"
#include "pmp_setting.h"

int32_t pmp_get_cur_tag_num(void)
{
	int32_t value = 0;
	GxBus_ConfigGetInt(CUR_TAG_NO, &value, 0);
	return value;
}

status_t pmp_set_cur_tag_num(void)
{
	int32_t  cur_tag_no;
	cur_tag_no = pmp_get_cur_tag_num();
	if(cur_tag_no<(MAX_TAGS_NUM-1))
	{
		cur_tag_no+=1;
	}
	else
	{
		cur_tag_no = 0;
	}
	GxBus_ConfigSetInt(CUR_TAG_NO, cur_tag_no);
	return GXCORE_SUCCESS;
}

status_t pmp_init_tags(void)
{
	int32_t ret = 0;
	int32_t i;
	char tag_url[32];
	char tag_time[32];
	handle_t group_tag = 0;

	group_tag = GxBus_ConfigGroupOpen();
	ret = GxBus_ConfigGroupSetInt(group_tag, CUR_TAG_NO, 0);
	if(GXCONFIG_FAILURE == ret) return GXCORE_ERROR;
	
	for(i=0;i<MAX_TAGS_NUM;i++)
	{
		memset(tag_url,0,sizeof(tag_url));
		memset(tag_time,0,sizeof(tag_time));
		sprintf(tag_url,"TAG%d>URL", i);	
		sprintf(tag_time,"TAG%d>TIME", i);	

		GxBus_ConfigGroupSet(group_tag, tag_url, "null");
		GxBus_ConfigGroupSetInt(group_tag, tag_time,0);
	}
	
	GxBus_ConfigGroupSave(group_tag);
	GxBus_ConfigGroupClose(group_tag);
	
	return GXCORE_SUCCESS;
}

status_t pmp_get_tags(int32_t tag_no,char* purl,int32_t* ptime)
{
	char tag_url[32];
	char tag_time[32];

	if(tag_no>MAX_TAGS_NUM||tag_no<0||purl == NULL||ptime == NULL)
		return GXCORE_ERROR;

	sprintf(tag_url,"TAG%d>URL", tag_no);	
	sprintf(tag_time,"TAG%d>TIME", tag_no);
	
	GxBus_ConfigGet(tag_url, purl, MAX_URL_LENGTH, "");
	GxBus_ConfigGetInt(tag_time, ptime, 0);

	return GXCORE_SUCCESS;
}

status_t pmp_set_tags(int32_t tag_no,const char* url,int32_t time)
{
	char tag_url[32];
	char tag_time[32];

	if(tag_no>MAX_TAGS_NUM||tag_no<0||url == NULL)
		return GXCORE_ERROR;

	sprintf(tag_url,"TAG%d>URL", tag_no);	
	sprintf(tag_time,"TAG%d>TIME", tag_no);

	GxBus_ConfigSet(tag_url, (char*)url);
	GxBus_ConfigSetInt(tag_time,time);

	return GXCORE_SUCCESS;
}

int32_t pmp_load_tag(const char* url)
{
	uint8_t i;
	char tag_url[MAX_URL_LENGTH];
	int32_t time;
	int32_t value;
	
	value = pmpset_get_int(PMPSET_SAVE_TAGS);
	if(PMPSET_TONE_OFF == value)
		return 0;

	if(url == NULL)
		return 0;

	for(i=0;i<MAX_TAGS_NUM;i++)
	{
		pmp_get_tags(i,tag_url,&time);
        printf("tag_url %s,time %d\n", tag_url,time);
		if(NULL == tag_url) continue;
		if(strcmp(url, tag_url) == 0)
		{
			return time;
		}
	}
	return 0;
}

status_t pmp_save_tag(const char* url,int32_t time)
{
	uint8_t i=0,tag_no=0xff;
	char tag_url[MAX_URL_LENGTH];
	int32_t tag_time;
	int32_t value;
	
	value = pmpset_get_int(PMPSET_SAVE_TAGS);
	if(PMPSET_TONE_OFF == value)
		return GXCORE_ERROR;

	if(url == NULL)
		return GXCORE_ERROR;

	for(i=0;i<MAX_TAGS_NUM;i++)
	{
		pmp_get_tags(i,tag_url,&tag_time);
		if(NULL == tag_url) continue;
		if(strcmp(url, tag_url) == 0)
		{
			tag_no = i;
			break;
		}
	}

	if(tag_no == 0xff)
	{
		tag_no = pmp_get_cur_tag_num();
		pmp_set_cur_tag_num();
	}
	pmp_set_tags(tag_no,url,time);
	return GXCORE_SUCCESS;
}
					




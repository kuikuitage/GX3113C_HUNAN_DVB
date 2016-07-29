#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "app_common_porting_ads_flash.h"
#include "app_common_prog.h"
#include "cmis_ads_porting.h"
#include "cmis_ads_api.h" 
#include "cmis_ads_camail.h" 
#include "cmis_ads_flash.h" 
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#endif

#define CMIS_FLASH_FILE_NAME    "/home/gx/cmis.dat"
static uint8_t * g_flash_mem_map = NULL;
static uint32_t g_flash_dirty_flag = 0;

extern GxBusPmDataProg g_cmis_homepage_prog;
extern GxBusPmDataProg g_cmis_recharge_prog;
static void cmis_flash_printf(void)
{
	ADS_Dbg("homepage:name[%s]\n",g_cmis_homepage_prog.prog_name);
	ADS_Dbg("homepage:name[%s]\n",g_cmis_recharge_prog.prog_name);
}

void cmis_ad_flash_init(void)
#if 0
{
	uint8_t ret = 0;
	g_flash_mem_map = CMIS_MALLOC(CMIS_ADS_FLASH_LEN);
	if(g_flash_mem_map == NULL)
	{
		ADS_ERROR("flash malloc ERROR\n");
		while(1);
	}
	memset(g_flash_mem_map,0,CMIS_ADS_FLASH_LEN*sizeof(uint8_t));
	app_porting_ads_flash_init(CMIS_ADS_FLASH_LEN/*+0x1000*/);
	g_flash_dirty_flag = 0;

	ret = app_porting_ads_flash_read_data(0,g_flash_mem_map,CMIS_ADS_FLASH_LEN);
	if(ret != 1)
	{
		ADS_ERROR("app_porting_ads_flash_read_data failed\n");
		return;
	}

}
#else
{
	size_t   size;
	handle_t    handle;
	uint8_t     *value;
	uint8_t ret = 0;
	
	g_flash_mem_map = CMIS_MALLOC(CMIS_ADS_FLASH_LEN);
	if(g_flash_mem_map == NULL)
	{
		ADS_ERROR("flash malloc ERROR\n");
		while(1);
	}
	memset(g_flash_mem_map,0,CMIS_ADS_FLASH_LEN*sizeof(uint8_t));

	g_flash_dirty_flag = 0;

	if (GxCore_FileExists((char*)CMIS_FLASH_FILE_NAME) == GXCORE_MOD_EXIST)
	{
		ADS_Dbg("File is exist!\n");
		handle = GxCore_Open((char*)CMIS_FLASH_FILE_NAME, "r+");
		GxCore_Seek(handle, (long)0, GX_SEEK_SET);
		size = GxCore_Read(handle, g_flash_mem_map, 1,CMIS_ADS_FLASH_LEN);
		if(size != CMIS_ADS_FLASH_LEN)
		{
			ADS_ERROR("Read file err! ret size = %d, read size = %d\n",
				size,CMIS_ADS_FLASH_LEN);	
		}
		GxCore_Close(handle);
	}
	else
	{
		memset(g_flash_mem_map, 0xFF, CMIS_ADS_FLASH_LEN);

		handle = GxCore_Open((char*)CMIS_FLASH_FILE_NAME, "a+");
		GxCore_Seek(handle, 0, GX_SEEK_SET);
		size = GxCore_Write(handle, g_flash_mem_map,1, CMIS_ADS_FLASH_LEN);
		if(size != CMIS_ADS_FLASH_LEN)
		{
			ADS_ERROR("Read file err! ret size = %d, read size = %d\n",
				size,CMIS_ADS_FLASH_LEN);	
		}
		GxCore_Close(handle);
	}
}
#endif

void cmis_ad_flash_load_ad_info(void)
{
	uint8_t ret = 0;
	int32_t i = 0;
	uint8_t *p = g_flash_mem_map;
	
	Cmis_prog_t prog_info = {0};
	
	//ret = app_porting_ads_flash_read_data(CMIS_ADS_FLASH_AD_INFO_OFFSET,
	//	g_flash_mem_map,CMIS_ADS_FLASH_AD_INFO_LEN);
	//if(ret != 1)
	//{
//		ADS_ERROR("app_porting_ads_flash_read_data failed\n");
//		return;
//	}
	
	if(p  == NULL)
	{
		ADS_ERROR("g_flash_mem_map is NULL \n");
		return;
	}
	for(i = 0;i < CMIS_PROG_LIST_LEN;i++)
	{
		if(CMIS_ADS_FLASH_AD_INFO_ITEM_LEN*i > CMIS_ADS_FLASH_AD_INFO_LEN)
		{
			break;
		}
		memset(&prog_info,0,sizeof(Cmis_prog_t));
		prog_info.service_id = TODATA16(p[0],p[1]);
		prog_info.prog_type = TODATA32(p[2],p[3],p[4],p[5]);
		if(prog_info.prog_type == CMIS_PROG_TYPE_CURTAIN)
		{
			prog_info.curtain_channel_pid = TODATA16(p[6],p[7]);
		}
		else
		{
			prog_info.curtain_channel_pid = CMIS_INVALID_PID;
		}
		prog_info.epg_channel_pid = TODATA16(p[8],p[9]);
		/*For NULL data ,Do not add the invalid program to list*/
		if(prog_info.service_id == 0xFFFF 
			&& prog_info.prog_type == 0xFFFFFFFF
			&& prog_info.epg_channel_pid == 0xFFFF)
		{
			continue;
		}
		cmis_ad_prog_list_add_new_prog(&prog_info);
		p += CMIS_ADS_FLASH_AD_INFO_ITEM_LEN;
	}
	

}
Cmis_err_t cmis_ad_flash_write_ad_info(Cmis_prog_t * prog_info,int32_t index_offset)
{
	Cmis_prog_t prog_info_temp;
	uint8_t *p = g_flash_mem_map;
	uint32_t mem_offset = 0;
	uint8_t temBuff[CMIS_ADS_FLASH_AD_INFO_ITEM_LEN];
	
	if(prog_info == NULL)
	{
		return CMIS_ERROR;
	}
	if(index_offset*CMIS_ADS_FLASH_AD_INFO_ITEM_LEN > CMIS_ADS_FLASH_AD_INFO_LEN)
	{
		return CMIS_ERROR;
	}
	if(p  == NULL)
	{
		ADS_ERROR("g_flash_mem_map is NULL \n");
		return CMIS_ERROR;
	}
	mem_offset= index_offset*CMIS_ADS_FLASH_AD_INFO_ITEM_LEN ;
	p += mem_offset;
	
	memset(temBuff,0,CMIS_ADS_FLASH_AD_INFO_ITEM_LEN*(sizeof(uint8_t)));
	
	temBuff[0] = ((prog_info->service_id>>8)&0xff);
	temBuff[1] = ((prog_info->service_id)&0xff);
	temBuff[2] = ((prog_info->prog_type>>24)&0xff);
	temBuff[3] = ((prog_info->prog_type>>16)&0xff);
	temBuff[4] = ((prog_info->prog_type>>8)&0xff);
	temBuff[5] = ((prog_info->prog_type)&0xff);
	temBuff[6] = ((prog_info->curtain_channel_pid>>8)&0xff);
	temBuff[7] = ((prog_info->curtain_channel_pid)&0xff);
	temBuff[8] = ((prog_info->epg_channel_pid>>8)&0xff);
	temBuff[9] = ((prog_info->epg_channel_pid)&0xff);
	if(memcmp(p,temBuff,CMIS_ADS_FLASH_AD_INFO_ITEM_LEN))
	{
		memcpy(p,temBuff,CMIS_ADS_FLASH_AD_INFO_ITEM_LEN);
		g_flash_dirty_flag++;
	}
	return CMIS_SUCCESS;
}
void cmis_ad_flash_load_cmis_channel_info(void)
{
	uint8_t ret = 0;
	int32_t i = 0;
	uint8_t *p = g_flash_mem_map;
	Cmis_prog_t prog_info = {0};
	
//	ret = app_porting_ads_flash_read_data(CMIS_ADS_FLASH_CHANNEL_OFFSET,
//		g_flash_mem_map+CMIS_ADS_FLASH_CHANNEL_OFFSET,CMIS_ADS_FLASH_CHANNEL_LEN);
//	if(ret != 1)
//	{
//		ADS_ERROR("app_porting_ads_flash_read_data failed\n");
//		return;
//	}
	
	if(p  == NULL)
	{
		ADS_ERROR("g_flash_mem_map is NULL \n");
		return;
	}
	ADS_Dbg("%s %d\n",__FUNCTION__,__LINE__);
	p += CMIS_ADS_FLASH_CHANNEL_OFFSET;
	memcpy(&g_cmis_homepage_prog,p,sizeof(GxBusPmDataProg));

	p += sizeof(GxBusPmDataProg);
	memcpy(&g_cmis_recharge_prog,p,sizeof(GxBusPmDataProg));
	cmis_flash_printf();
	
}
Cmis_err_t cmis_ad_flash_write_cmis_channel_info
	(Cmis_prog_type_t prog_type,GxBusPmDataProg * prog_data)
{
	Cmis_prog_t prog_info_temp;
	uint8_t *p = g_flash_mem_map + CMIS_ADS_FLASH_CHANNEL_OFFSET;
	uint32_t mem_offset = 0;
	//ADS_Dbg("%s %d\n",__FUNCTION__,__LINE__);

	if(prog_data == NULL)
	{
		return CMIS_ERROR;
	}
	if(prog_type != CMIS_PROG_TYPE_HOME_PROG 
		&& prog_type != CMIS_PROG_TYPE_RECHARGE_BACKGROUND)
	{
		return CMIS_ERROR;
	}

	if(prog_type == CMIS_PROG_TYPE_HOME_PROG)
	{
		memcpy(&g_cmis_homepage_prog,prog_data,sizeof(GxBusPmDataProg));
		p = p;/*Do noting*/
	}
	else if(prog_type == CMIS_PROG_TYPE_RECHARGE_BACKGROUND)
	{
		memcpy(&g_cmis_recharge_prog,prog_data,sizeof(GxBusPmDataProg));
		p += sizeof(GxBusPmDataProg);
	}
	if(memcmp(p,prog_data,sizeof(GxBusPmDataProg)) != 0)
	{
		memcpy(p,prog_data,sizeof(GxBusPmDataProg));
		g_flash_dirty_flag++;
	}
	cmis_flash_printf();
}
void cmis_ad_flash_clean(void)
#if 0
{
	memset(g_flash_mem_map,0,CMIS_ADS_FLASH_LEN*sizeof(uint8_t));
	app_porting_ads_flash_write_data(0,g_flash_mem_map,CMIS_ADS_FLASH_LEN-1);
}
#else
{
	size_t   size;
	handle_t    handle;

	memset(g_flash_mem_map, 0xFF, CMIS_ADS_FLASH_LEN);
	handle = GxCore_Open((char*)CMIS_FLASH_FILE_NAME, "w+");
	GxCore_Seek(handle, (long)0, GX_SEEK_SET);
	size = GxCore_Write(handle, g_flash_mem_map, 1,CMIS_ADS_FLASH_LEN);
	if(size != CMIS_ADS_FLASH_LEN)
	{
		ADS_ERROR("Read file err! ret size = %d, read size = %d\n",
			size,CMIS_ADS_FLASH_LEN);	
	}
	GxCore_Close(handle);
}

#endif

void cmis_ad_flash_update(void)
#if 0
{
	if(g_flash_dirty_flag)
	{
		ADS_Dbg("cmis_ad_flash_update\n");
		app_porting_ads_flash_write_data(0,g_flash_mem_map,CMIS_ADS_FLASH_LEN-1);
		g_flash_dirty_flag = 0;
	}
}
#else
{
	size_t   size;
	handle_t    handle;

	if(g_flash_dirty_flag)
	{
		ADS_Dbg("cmis_ad_flash_update\n");

		handle = GxCore_Open((char*)CMIS_FLASH_FILE_NAME, "w+");
		GxCore_Seek(handle, 0, GX_SEEK_SET);
		size = GxCore_Write(handle, g_flash_mem_map,1, CMIS_ADS_FLASH_LEN);
		if(size != CMIS_ADS_FLASH_LEN)
		{
			ADS_ERROR("Read file err! ret size = %d, read size = %d\n",
				size,CMIS_ADS_FLASH_LEN);	
		}
		GxCore_Close(handle);

#if 0
{
	uint8_t * tembuf = NULL;
	tembuf = CMIS_MALLOC(CMIS_ADS_FLASH_LEN);
	if(tembuf == NULL)
	{
		ADS_ERROR("flash malloc ERROR\n");
		while(1);
	}
	memset(tembuf,0xab,CMIS_ADS_FLASH_LEN*sizeof(uint8_t));
	handle = GxCore_Open((char*)CMIS_FLASH_FILE_NAME, "r+");
	GxCore_Seek(handle, 0, GX_SEEK_SET);
		size = GxCore_Read(handle, tembuf, 1,CMIS_ADS_FLASH_LEN);
	if(size != CMIS_ADS_FLASH_LEN)
	{
		ADS_ERROR("Read file err! ret size = %d, read size = %d\n",
			size,CMIS_ADS_FLASH_LEN);	
	}
	else
	{
		ADS_DUMP("FLASH", tembuf, 1024);
	}
	GxCore_Close(handle);
}
#endif
		g_flash_dirty_flag = 0;
	}
}
#endif
/**/

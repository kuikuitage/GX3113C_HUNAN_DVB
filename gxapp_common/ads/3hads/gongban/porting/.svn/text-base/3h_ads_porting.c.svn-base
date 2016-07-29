/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2012, All right reserved
******************************************************************************

******************************************************************************
* File Name :	3h_ads_porting.c
* Author    : 	zhouhm
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2013.11.27		  zhouhm 	 			creation
*****************************************************************************/
#include <gxtype.h>
#include "app_common_porting_stb_api.h"
#include "gx_demux.h"
#include "gxapp_sys_config.h"
#include "gui_timer.h"
#include "gxmsg.h"
#include "app_common_porting_ads_flash.h"
#include "3h_ads_porting.h"
#include "app_3h_ads_porting_stb_api.h"
#include "app_common_porting_ca_demux.h"
#include "service/gxsearch.h"
#include "app_common_search.h"

static uint8_t startPlayFlag = FALSE;
static handle_t ad_channel=0; 
static handle_t ad_filter=0; 
static uint8_t ad_fail = 0; // 1: get ad timeout, 0: get ad success, 2: close hint info. add by zs
#define MAX_ITEMS_COUNT (3)

typedef struct ad_section_s
{
	uint8_t      recievedFlag;
	uint8_t*     addata;
	int32_t         len;
}ad_section_t;

#define SECTION_MAX_NUM (60)
typedef struct ad_items_s
{
	uint8_t      itemType;
	uint8_t      itemTableExId;
	uint8_t      itemDescLen;
	uint8_t	  itemName[20];
	uint8_t	  lastSectionNum;
	uint8_t	  recieveNum;
	ad_section_t sectionData[SECTION_MAX_NUM];
}ad_items_t;

#ifdef ECOS_OS
extern void cyg_scheduler_lock(void);
extern void cyg_scheduler_unlock(void);
#endif



ad_play_para ads3h_para[AD_3H_TYPE_MAX];
uint8_t      g_itemsCount = 0;
uint8_t      g_itemsFilter = 0;
uint8_t      g_itemsversion = 32;
uint32_t      g_Filternms = 0;
uint8_t      g_Filtertimeout = 0;
#define AD_FILTER_TIMEOUT (45)
ad_items_t g_adItems[MAX_ITEMS_COUNT];
int GxAD_ItemFilterOpen(uint8_t version);

int GxAD_DataFilterOpen(uint8_t item,uint8_t secNum);
private_parse_status ad_item_got(uint8_t* pSectionData, size_t Size);
private_parse_status ad_data_got(uint8_t* pSectionData, size_t Size);
uint8_t ad_version_flag = 0;  //addbyzs for if ad version is same, don't hint user "download ad data"
//ad_version_flag is 0: init value.1:version don't change, 2:version changed

#define AD_VERSION                	"ADVersion"
#define AD_VERSION_DV                			32

//#define AD_3H_PID (0x1dec)
#define AD_3H_PID (0x1df6)
//addbyzs for if ad version is same, don't hint user "download ad data" begin
uint8_t advertisement_get_ad_version_flag()
{
    return ad_version_flag;
}

void advertisement_set_ad_version_flag(uint8_t version_flag)
{
    ad_version_flag = version_flag;
}
//addbyzs for if ad version is same, don't hint user "download ad data" end

//add by zs 20110810 begin
uint8_t advertisement_set_timeout_flag(uint8_t flag)
{
    ad_fail = flag;
    return 0;
}

uint8_t advertisement_get_timeout_flag()
{
    return ad_fail;
}

uint8_t advertisement_get_startplayflag(void)
{
	return startPlayFlag;
}

uint8_t advertisement_set_startplayflag(uint8_t flag)
{	
	startPlayFlag = flag;
	return startPlayFlag;
}


handle_t app_3h_ads_get_filter_handle(void)
{
	return ad_filter;
}

void app_3h_ads_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size)
{
    int16_t            pid;
    uint16_t            section_length;
    uint8_t*            data = (uint8_t*)Section;
    int                 len = Size;
	int32_t ret;

	
	ret = GxDemux_FilterGetPID(Filter,&pid);

//    printf("adfilter_notify Size=%d pid=0x%x\n",Size,pid);
    switch(pid) {
    case AD_3H_PID: {
	        while(len > 0) {
				section_length = ((data[1] & 0x0F) << 8) + data[2] + 3;
				if (0xb1 == data[0])
					{	// 资源数据
						ad_data_got(data, section_length);
//						break;
						//return;
					}
				else
					if (0xb0 == data[0])
						{	// 资源名称
							ad_item_got(data, section_length);
							return;
						}
					else
						{
							printf("filter_notify tableid = 0x%x error \n",data[0]);
//							GxDemux_FilterReset(Filter);
							return;							
						}
				data += section_length;
	           		 len -= section_length;
	        	}

	 }
        return;
    default:
        return;
    }
}


void ADfiltertimeouttask(void* arg)
{
   		 uint8_t            i;  

		while(TRUE)
		{
			GxCore_ThreadDelay(500); 
			if (TRUE == advertisement_get_startplayflag())
				{
					continue;
				}

					if (( 0 != ad_filter)&&(0 != ad_channel))
					{
						g_Filtertimeout = TRUE;
						g_Filternms+=500;	
						if (g_Filternms>=1000*AD_FILTER_TIMEOUT)
						{
        					ad_fail = 1;
							/*time out*/

							GxAD_ItemFilterClose();
							if (0 == g_itemsCount)
								{
									GxAD_LoadData();
									advertisement_set_startplayflag(TRUE);	
				                    //addbyzs for if ad version is same, don't hint user "download ad data" begin
				                    if (0 == advertisement_get_ad_version_flag())
				                    {
				                        advertisement_set_ad_version_flag(2);
				                    }
				                    //addbyzs for if ad version is same, don't hint user "download ad data" end
									GxAD_ItemFilterOpen(32);
								}
							else
								{
									for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
									{
											if (NULL !=g_adItems[g_itemsFilter].sectionData[i].addata )
												{
													GxCore_Free(g_adItems[g_itemsFilter].sectionData[i].addata);
													g_adItems[g_itemsFilter].sectionData[i].addata = NULL;
												}
									}
									g_adItems[g_itemsFilter].lastSectionNum =0;
									g_adItems[g_itemsFilter].recieveNum =0;
									memset(&g_adItems[g_itemsFilter].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_section_t));

									g_itemsFilter++;

									printf("g_itemsFilter=%d g_itemsCount=%d\n",g_itemsFilter,g_itemsCount);
									if (g_itemsFilter < g_itemsCount  )
									{
										/*
										* 接收下一类型的广告数据
										*/
										GxAD_LoadData();
										advertisement_set_startplayflag(TRUE);													
				                        //addbyzs for if ad version is same, don't hint user "download ad data" begin
				                        if (0 == advertisement_get_ad_version_flag())
				                        {
				                            advertisement_set_ad_version_flag(2);
				                        }
				                        //addbyzs for if ad version is same, don't hint user "download ad data" end
										GxAD_DataFilterOpen(g_itemsFilter,0);	
									}
									else
									{
										/*所有广告接收完，设置开机播放标志*/
										GxAD_LoadData();
										advertisement_set_startplayflag(TRUE);
				                        //addbyzs for if ad version is same, don't hint user "download ad data" begin
				                        if (0 == advertisement_get_ad_version_flag())
				                        {
				                            advertisement_set_ad_version_flag(2);
				                        }
				                        //addbyzs for if ad version is same, don't hint user "download ad data" end
										GxAD_ItemFilterOpen(g_itemsversion);
									}										
								}
							
						}
					}

		}
}


int GxAD_LoadData(void) 
{
	uint8_t Buf[1024]={0};
    int fd;
	size_t   size;					
	handle_t handlejpg;
	handle_t handle;


	 if (GxCore_FileExists("/home/gx/banner.jpg") == 1)
	{
	 		printf("GxAD_LoadData banner.jpg  exist\n");
			if (GxCore_FileExists("/mnt/banner.jpg") == 1)
				GxCore_FileDelete("/mnt/banner.jpg");
	        fd = GxCore_Open("/home/gx/banner.jpg", "r");
			GxCore_Seek(fd, 0, GX_SEEK_SET);
			handlejpg = GxCore_Open("/mnt/banner.jpg", "a+");
			GxCore_Seek(handlejpg, 0, GX_SEEK_SET);
			size = GxCore_Read(fd, Buf, 1, 1024);
			while(size>0)
				{

		       		 	size = GxCore_Write(handlejpg, Buf, 1, size);	
						size = GxCore_Read(fd, Buf, 1, 1024);
				}
				GxCore_Close(fd);	
		        GxCore_Close(handlejpg);						
	
	}
	 else 
		 if (GxCore_FileExists("/theme/banner.jpg") == 1)
	 	{
			 		printf("GxAD_LoadData theme banner.jpg\n");
					if (GxCore_FileExists("/mnt/banner.jpg") == 1)
						GxCore_FileDelete("/mnt/banner.jpg");
  					handle = GxCore_Open("/theme/banner.jpg", "r");
					GxCore_Seek(handle, 0, GX_SEEK_SET);
					handlejpg = GxCore_Open("/mnt/banner.jpg", "a+");
					GxCore_Seek(handlejpg, 0, GX_SEEK_SET);
					size = GxCore_Read(handle, Buf, 1, 1024);
					while((size>0)&&(GXCORE_ERROR!=size))
						{

				       		 size = GxCore_Write(handlejpg, Buf, 1, size);	
							 size = GxCore_Read(handle, Buf, 1, 1024);
						}

				        GxCore_Close(handle);	
				        GxCore_Close(handlejpg);	
		
	 		printf("GxAD_LoadData banner.jpg not exist\n");
			
	 	}

#if  0
	 if (GxCore_FileExists("/home/gx/menu.jpg") == 1)
	{
	 		printf("GxAD_LoadData menu.jpg  exist\n");
				GxCore_FileDelete("/mnt/menu.jpg");
				fd = GxCore_Open("/home/gx/menu.jpg", "r");
				GxCore_Seek(fd, 0, GX_SEEK_SET);			
				handlejpg = GxCore_Open("/mnt/menu.jpg", "a+");
				GxCore_Seek(handlejpg, 0, GX_SEEK_SET);
				size = GxCore_Read(fd, Buf, 1, 1024);
				while(size>0)
					{

			       		 	size = GxCore_Write(handlejpg, Buf, 1, size);	
							size = GxCore_Read(fd, Buf, 1, 1024);
					}

			        GxCore_Close(fd);
			        GxCore_Close(handlejpg);						
	
	}
	 else 
		 if (GxCore_FileExists("/theme/menu.jpg") == 1)
	 	{
	 		printf("GxAD_LoadData menu.jpg not exist\n");
					GxCore_FileDelete("/mnt/menu.jpg");
  					handle = GxCore_Open("/theme/menu.jpg", "r");
					GxCore_Seek(handle, 0, GX_SEEK_SET);
					handlejpg = GxCore_Open("/mnt/menu.jpg", "a+");
					GxCore_Seek(handlejpg, 0, GX_SEEK_SET);
					size = GxCore_Read(handle, Buf, 1, 1024);
					while((size>0)&&(GXCORE_ERROR!=size))
						{

				       		 size = GxCore_Write(handlejpg, Buf, 1, size);	
							 size = GxCore_Read(handle, Buf, 1, 1024);
						}

				        GxCore_Close(handle);	
				        GxCore_Close(handlejpg);	
	 	}
#endif



	g_Filtertimeout =0;
	g_itemsCount = 0;
	g_itemsFilter = 0;
	g_Filternms = 0;
	return 0;

}


int GxAD_ItemFilterClose(void)
{
	int32_t  ret = 0;

	printf("GxAD_ItemFilterClose\n");
	app_porting_psi_demux_lock();

	if (0 != ad_filter)
		{
//			ret = GxDemux_FilterDisable(ad_filter);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter);
			CHECK_RET(DEMUX,ret);
			ad_filter = 0;
		}
	if (0 != ad_channel)
		{
			ret = GxDemux_ChannelFree(ad_channel);
			CHECK_RET(DEMUX,ret);
			ad_channel = 0;
		}
	g_Filternms=0;
	app_porting_psi_demux_unlock();

	return 0;
}


int GxAD_DataFilterOpen(uint8_t item,uint8_t secNum)
{
	uint16_t pid = AD_3H_PID;
	handle_t filter;
	uint8_t     match[18] = {0};
	uint8_t     mask[18] = {0};
	int32_t  ret = 0;

	printf("GxAD_DataFilterOpen\n");

	match[0] = 0xb1;
	mask[0] = 0xff;


	match[3] = g_adItems[item].itemType;
	mask[3] = 0xff;
	match[4] = g_adItems[item].itemTableExId;
	mask[4] = 0xff;
	printf("GxAD_DataFilterOpen g_adItems[%d].itemType=%d g_adItems[%d].itemTableExId=%d\n",
		item, g_adItems[item].itemType,item,g_adItems[item].itemTableExId);
	app_porting_psi_demux_lock();

	if (0 != ad_filter)
		{
//			ret = GxDemux_FilterDisable(ad_filter);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter);
			CHECK_RET(DEMUX,ret);

			ad_filter = 0;
		}
	if (0 != ad_channel)
		{
			ret = GxDemux_ChannelFree(ad_channel);
			CHECK_RET(DEMUX,ret);
			ad_channel= 0;
		}

	ad_channel = GxDemux_ChannelAllocate(0, pid);
	if ((0 == ad_channel)||(-1 == ad_channel))
		{
			CA_ERROR(" ad_channel=0x%x\n",ad_channel);
			ad_channel = 0;
			app_porting_psi_demux_unlock();
			return 0;
		}


	filter = GxDemux_FilterAllocate(ad_channel);
	if (( 0 == filter)||(-1 == filter))
	{
		ret = GxDemux_ChannelFree(ad_channel);
		CHECK_RET(DEMUX,ret);
		ad_channel = 0;

		CA_ERROR(" filter=0x%x\n",filter);
		filter = 0;
		app_porting_psi_demux_unlock();
		return 0;
	}

	ad_filter = filter;

	ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 5);	
	CHECK_RET(DEMUX,ret);

	ret = GxDemux_ChannelEnable(ad_channel);
	CHECK_RET(DEMUX,ret);


//	GxCA_DemuxFilterRigsterNotify(filter, adfilter_notify,NULL);
	ret = GxDemux_FilterEnable(filter);
	CHECK_RET(DEMUX,ret);
	app_porting_psi_demux_unlock();
	return 0;
}


int GxAD_ItemFilterOpen(uint8_t version)
{
	uint16_t pid = AD_3H_PID;
	handle_t filter;
	uint8_t     match[18] = {0};
	uint8_t     mask[18] = {0};
	int32_t  ret = 0;

	printf("GxAD_ItemFilterOpen\n");

	match[0] = 0xb0;
	mask[0] = 0xff;

    match[5] = (version<<1)&0x3E;
    mask[5]  = 0x3E;
	app_porting_psi_demux_lock();
	if (0 != ad_filter)
		{
//			ret = GxDemux_FilterDisable(ad_filter);
//			CHECK_RET(DEMUX,ret);
			ret = GxDemux_FilterFree(ad_filter);
			CHECK_RET(DEMUX,ret);
			ad_filter = 0;
		}
	if (0 != ad_channel)
		{
			ret = GxDemux_ChannelFree(ad_channel);
			CHECK_RET(DEMUX,ret);
			ad_channel = 0;
		}
	
	g_itemsCount = 0;
	g_itemsFilter = 0;
	g_Filternms=0;


		ad_channel = GxDemux_ChannelAllocate(0, pid);
		if ((0 == ad_channel)||(-1 == ad_channel))
			{
				CA_ERROR("	ad_channel=0x%x\n",ad_channel);
				ad_channel = 0;
				app_porting_psi_demux_unlock();
				return 0;
			}
	
		filter = GxDemux_FilterAllocate(ad_channel);
		if (( 0 == filter)||(-1 == filter))
		{
			ret = GxDemux_ChannelFree(ad_channel);
			CHECK_RET(DEMUX,ret);
			ad_channel = 0;
	
			CA_ERROR(" filter=0x%x\n",filter);
			filter = 0;
			app_porting_psi_demux_unlock();
			return 0;
		}
	
		ad_filter = filter;

	if (32 == version)
		{
			ret = GxDemux_FilterSetup(filter, match, mask, TRUE, TRUE,0, 1);	
			CHECK_RET(DEMUX,ret);
		}
	else
		{
			ret = GxDemux_FilterSetup(filter, match, mask, FALSE, TRUE,0, 6);
			CHECK_RET(DEMUX,ret);
		}
	
	//	GxCA_DemuxFilterRigsterNotify(filter, app_table_nit_filter_notify,NULL);
		ret = GxDemux_ChannelEnable(ad_channel);
		CHECK_RET(DEMUX,ret);
		ret = GxDemux_FilterEnable(filter);
		CHECK_RET(DEMUX,ret);
		app_porting_psi_demux_unlock();

	return 0;
}


void app_3h_ads_init(void)
{
    handle_t    ADfiltertimeout;
	uint32_t symbol_rate = 0;
	uint32_t qam = 0;
	uint16_t center_fre = 0;
	memset(&ads3h_para[0],0,AD_3H_TYPE_MAX*sizeof(ad_play_para));
	advertisement_play_init();
	memset(&g_adItems[0],0,MAX_ITEMS_COUNT*sizeof(ad_items_t));


	symbol_rate = app_flash_get_config_center_freq_symbol_rate();
	qam = app_flash_get_config_center_freq_qam();
	center_fre = app_flash_get_config_center_freq();
	

	if (0 == app_search_lock_tp(center_fre, symbol_rate, INVERSION_OFF, qam,2000))
	{	
		GxAD_ItemFilterOpen(32);
		GxCore_ThreadCreate("ADfilter_timeout",&ADfiltertimeout, ADfiltertimeouttask, NULL, 10 * 1024, /*30*/10);
		return ;
	}
	else
	{	
        //delete by zs for only use second freq to search ad. begin
		{        
            // second freq锁频失败
			GxAD_LoadData();
			advertisement_set_startplayflag(TRUE);
            ad_fail = 1;//add by zs 20110810 lock tuner fail

            //addbyzs for if ad version is same, don't hint user "download ad data" begin
            if (0 == advertisement_get_ad_version_flag())
            {
                advertisement_set_ad_version_flag(1);
            }
            //addbyzs for if ad version is same, don't hint user "download ad data" end
            
			GxAD_ItemFilterOpen(32);
			GxCore_ThreadCreate("ADfilter_timeout",&ADfiltertimeout, ADfiltertimeouttask, NULL, 10 * 1024, /*30*/10);
			return ;
		}
	}
  

}

private_parse_status ad_data_got(uint8_t* pSectionData, size_t Size)
{
	uint8_t      *sectionData;
	uint8_t      i;
	int32_t     secTotalLen = 0;
	uint8_t      ItemType;
	uint8_t      ItemTableExId;
	uint8_t      sectionNum;
	uint8_t      lastSectionNum;
	int32_t      dataLength = 0;
	int32_t      dataTotalLength = 0;
	size_t   size;					
    int fd;


	 ItemType = pSectionData[3];
	 ItemTableExId = pSectionData[4];
	sectionNum = pSectionData[6];
	lastSectionNum = pSectionData[7];

	
//	printf("ItemType=%d ItemTableExId=%d\n",ItemType,ItemTableExId);
	printf("seNum=%d lastSecNum=%d\n",sectionNum,lastSectionNum);
	
	
	secTotalLen = ((pSectionData[1] & 0x0F) << 8) + pSectionData[2] + 3;
	if (Size != secTotalLen)
	{
		printf("ad_data_got Size=%d secTotalLen=%d error\n",Size,secTotalLen);
//		Gx_DemuxFilterEnable(ad_filter);
		return PRIVATE_SECTION_OK;
	}
	secTotalLen = Size-3;
	
	if (secTotalLen>4093)
	{
		printf("ad_data_got  secTotalLen=%d err\n",secTotalLen);
//		Gx_DemuxFilterEnable(ad_filter);
		return PRIVATE_SECTION_OK;
	}

	if (lastSectionNum > SECTION_MAX_NUM)
		{
			printf("ad_data_got  lastSectionNum=%d \n",lastSectionNum);
//			Gx_DemuxFilterEnable(ad_filter);
			return PRIVATE_SECTION_OK;
		}
	g_adItems[g_itemsFilter].lastSectionNum = lastSectionNum;

	if (TRUE == g_adItems[g_itemsFilter].sectionData[sectionNum].recievedFlag)
		{
			return PRIVATE_SECTION_OK;		
		}

	 g_adItems[g_itemsFilter].sectionData[sectionNum].recievedFlag = TRUE;
	 
	if (0 == sectionNum)
		{
		dataLength =  ((pSectionData[13] << 24) |(pSectionData[14] << 16)
			|(pSectionData[15] << 8)|(pSectionData[16] ));
		printf("ad_data_got total len = %d\n ",dataLength);
		dataLength=	secTotalLen-18;
		}
	else
		dataLength=	secTotalLen-9;
//	printf("ad_data_got dataLength=%d\n",dataLength);

	if (NULL != g_adItems[g_itemsFilter].sectionData[sectionNum].addata)
		{
			GxCore_Free(g_adItems[g_itemsFilter].sectionData[sectionNum].addata);
			g_adItems[g_itemsFilter].sectionData[sectionNum].addata = NULL;
		}
	g_adItems[g_itemsFilter].sectionData[sectionNum].addata= GxCore_Malloc(dataLength);
	if (NULL == g_adItems[g_itemsFilter].sectionData[sectionNum].addata)
		{
			printf("g_adItems[%d].sectionData[%d].data =NULL\n",g_itemsFilter,sectionNum);
			return PRIVATE_SECTION_OK;			
		}
	if (0 == sectionNum)
		sectionData=pSectionData+17;
	else
		sectionData=pSectionData+8;

	memcpy(g_adItems[g_itemsFilter].sectionData[sectionNum].addata,sectionData,dataLength);
	
	g_adItems[g_itemsFilter].sectionData[sectionNum].len = dataLength; 
	
	
	g_adItems[g_itemsFilter].recieveNum++;
//	printf("g_adItems[%d].recieveNum=%d\n",g_itemsFilter,g_adItems[g_itemsFilter].recieveNum);
	cyg_scheduler_lock();
	if (g_adItems[g_itemsFilter].recieveNum >= (g_adItems[g_itemsFilter].lastSectionNum+1))
	{	
		/*
		*  当前类型广告所有SECTION接收完整
		*/

		/*
		* 判断数据大小是否有效
		*/
			for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
			{
				dataTotalLength+=g_adItems[g_itemsFilter].sectionData[i].len;
			}

			printf(" %d name = %s dataTotalLength=%d\n",g_itemsFilter,(char *)g_adItems[g_itemsFilter].itemName,dataTotalLength);
#if 0
			if(0 == strcmp("menu.jpg",(char *)g_adItems[g_itemsFilter].itemName))
			{
				if (dataTotalLength <= 40*1024)
					{	// 大小有效，保存至FLASH

						/*首先删除以原有文件*/
							if ( 0 != dataTotalLength)
								{
								
									GxCore_FileDelete("/home/gx/menu.jpg");
									printf(" save menu.jpg\n");
									fd = GxCore_Open("/home/gx/menu.jpg", "a+");							
									GxCore_Seek(fd, 0, GX_SEEK_SET);

								for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
									{	
												size = GxCore_Write(fd, g_adItems[g_itemsFilter].sectionData[i].addata, 1, g_adItems[g_itemsFilter].sectionData[i].len);	
									}

									GxCore_Close(fd);								
							}
							

						
						for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
							{
									if (NULL !=g_adItems[g_itemsFilter].sectionData[i].addata )
										{
											GxCore_Free(g_adItems[g_itemsFilter].sectionData[i].addata);
											g_adItems[g_itemsFilter].sectionData[i].addata = NULL;
										}
							}
							g_adItems[g_itemsFilter].lastSectionNum =0;
							g_adItems[g_itemsFilter].recieveNum =0;	
							memset(&g_adItems[g_itemsFilter].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_section_t));	
						
					}
				
			}
#endif

			if(0 == strcmp("banner.jpg",(char *)g_adItems[g_itemsFilter].itemName))
			{
//				if (dataTotalLength <= 15*1024)
				if (dataTotalLength <= 120*1024)

					{	// 大小有效，保存至FLASH
						/*首先删除以原有文件*/
							{
								if (GxCore_FileExists("/home/gx/banner.jpg") == 1)
									GxCore_FileDelete("/home/gx/banner.jpg");
								
							}

							fd = GxCore_Open("/home/gx/banner.jpg", "a+");							
							GxCore_Seek(fd, 0, GX_SEEK_SET);

						for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
							{	
										size = GxCore_Write(fd, g_adItems[g_itemsFilter].sectionData[i].addata, 1, g_adItems[g_itemsFilter].sectionData[i].len);	
							}

							GxCore_Close(fd);

						
						for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
							{
									if (NULL !=g_adItems[g_itemsFilter].sectionData[i].addata )
										{
											GxCore_Free(g_adItems[g_itemsFilter].sectionData[i].addata);
											g_adItems[g_itemsFilter].sectionData[i].addata = NULL;
										}
							}
							g_adItems[g_itemsFilter].lastSectionNum =0;
							g_adItems[g_itemsFilter].recieveNum =0;
							memset(&g_adItems[g_itemsFilter].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_section_t));	
						
					}			
			}

			if(0 == strcmp("start.mpg",(char *)g_adItems[g_itemsFilter].itemName))
			{
//				if (dataTotalLength <= 50*1024)
				if (dataTotalLength <= 200*1024)
					{	// 大小有效，保存至FLASH


							{
							
								if (GxCore_FileExists("/home/gx/openpicture.bin") == 1)
									GxCore_FileDelete("/home/gx/openpicture.bin");
								
							}
						
							fd = GxCore_Open("/home/gx/openpicture.bin", "a+");							
							GxCore_Seek(fd, 0, GX_SEEK_SET);

						for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
							{	
										size = GxCore_Write(fd, g_adItems[g_itemsFilter].sectionData[i].addata, 1, g_adItems[g_itemsFilter].sectionData[i].len);	
							}

							GxCore_Close(fd);
						

						for (i = 0; i<g_adItems[g_itemsFilter].lastSectionNum+1;i++ )
						{
								if (NULL !=g_adItems[g_itemsFilter].sectionData[i].addata )
									{
										GxCore_Free(g_adItems[g_itemsFilter].sectionData[i].addata);
										g_adItems[g_itemsFilter].sectionData[i].addata = NULL;
									}
						}

						g_adItems[g_itemsFilter].lastSectionNum =0;
						g_adItems[g_itemsFilter].recieveNum =0;
						memset(&g_adItems[g_itemsFilter].sectionData[0],0,SECTION_MAX_NUM*sizeof(ad_section_t));							
							
					}				
			}
		
			GxAD_ItemFilterClose();
			g_itemsFilter++;

			printf("g_itemsFilter=%d g_itemsCount=%d\n",g_itemsFilter,g_itemsCount);
			if (g_itemsFilter < g_itemsCount  )
			{
				/*
				* 接收下一类型的广告数据
				*/
				GxAD_DataFilterOpen(g_itemsFilter,0);	
			}
			else
			{
				/*所有广告接收完，设置开机播放标志*/
				/*所有广告接收完，设置广告版本*/

				GxAD_LoadData();
//				g_bannerFlag = FALSE;
//				g_menuFlag = FALSE;
				advertisement_set_startplayflag(TRUE);
				if (FALSE == g_Filtertimeout)
					{
						GxBus_ConfigSetInt(AD_VERSION, g_itemsversion);					
					}
				GxAD_ItemFilterOpen(g_itemsversion);
			}
	}
	else
		{
//			sectionNum = sectionNum+1;
//			GxAD_DataFilterOpen(g_itemsFilter,sectionNum);	
//			GxCA_DemuxFilterEnable(ad_filter);
		}
	cyg_scheduler_unlock();
	return PRIVATE_SUBTABLE_OK;

}

	private_parse_status ad_item_got(uint8_t* pSectionData, size_t Size)
{
	uint8_t      *sectionData;
	uint8_t      version;
	uint8_t      i;
	uint8_t      versionFormat;
	int32_t     secTotalLen = 0;
	int32_t nADVersion = 0;

					 
    	version = (pSectionData[5] & 0x3E) >> 1;
	/*
	* 判断广告流当前版本与断电前是否一致，如果一致则播放上次断电前节目
	* 如果版本不一致，则需要重新过滤广告并保存到FLASH中
	*/


	GxBus_ConfigGetInt(AD_VERSION, &nADVersion, AD_VERSION_DV);
	printf("ad_item_got version=%d nADVersion=%d\n",version,nADVersion);
	g_itemsversion = version;
	if (version == nADVersion)
	{
		GxAD_LoadData();
			/*所有广告接收完，设置开机播放标志*/
		advertisement_set_startplayflag(TRUE);

        //addbyzs for if ad version is same, don't hint user "download ad data" begin
        if (0 == advertisement_get_ad_version_flag())
        {
            advertisement_set_ad_version_flag(1);
        }
        //addbyzs for if ad version is same, don't hint user "download ad data" end

		GxAD_ItemFilterClose();
		/*过滤下一版本的广告流数据*/
		GxAD_ItemFilterOpen(g_itemsversion);		
		return PRIVATE_SUBTABLE_OK;
	}
	//g_bannerFlag = FALSE;
	//g_menuFlag = FALSE;
	
    //addbyzs for if ad version is same, don't hint user "download ad data" begin
    if (0 == advertisement_get_ad_version_flag())
    {
        advertisement_set_ad_version_flag(2);
    }
    //addbyzs for if ad version is same, don't hint user "download ad data" end

	secTotalLen = (((pSectionData[1]&0x0f) << 8) | (pSectionData[2]))+3;
		printf("ad_item_got Size=%d secTotalLen=%d \n",Size,secTotalLen);
		
	if (Size != secTotalLen)
	{
		printf("ad_item_got Size=%d secTotalLen=%d \n",Size,secTotalLen);
//		GxCA_DemuxFilterEnable(ad_filter);
		return PRIVATE_SECTION_OK;
	}
	secTotalLen = Size-3;

	if (secTotalLen>4093)
	{
		printf("ad_item_got  secTotalLen=%d \n",secTotalLen);
//		GxCA_DemuxFilterEnable(ad_filter);
		return PRIVATE_SECTION_OK;
	}

	versionFormat = pSectionData[8];
	
	g_itemsCount = pSectionData[9];
	printf("ad_item_got g_itemsCount=%d  versionFormat=%d\n",g_itemsCount,versionFormat);
	sectionData = pSectionData+10;
	if (g_itemsCount > MAX_ITEMS_COUNT)
		{
			printf("ad_item_got  g_itemsCount=%d \n",g_itemsCount);
//			GxCA_DemuxFilterEnable(ad_filter);
			return PRIVATE_SECTION_OK;		
		}

	secTotalLen -=10;
	for (i=0; i<g_itemsCount; i++ )
		{
			if (secTotalLen < 3)
				{
					printf("ad_item_got Size=%d secTotalLen=%d \n",Size,secTotalLen);
//					GxCA_DemuxFilterEnable(ad_filter);
					return PRIVATE_SECTION_OK;				
				}
			g_adItems[i].itemType = sectionData[0];
			g_adItems[i].itemTableExId= sectionData[1];			
			g_adItems[i].itemDescLen= sectionData[2];	
			memcpy(g_adItems[i].itemName,sectionData+3,sectionData[2]);
			printf("ad_item_got  item=%d itemname=%s\n",i,g_adItems[i].itemName);
			printf("ad_item_got  itemType=%d itemTableExId=%d\n",g_adItems[i].itemType,g_adItems[i].itemTableExId);
			secTotalLen -= sectionData[2];
			secTotalLen -= 3;
			sectionData += sectionData[2];
			sectionData +=3;
		}
	GxAD_ItemFilterClose();
	if (g_itemsCount >0 )
		GxAD_DataFilterOpen(/*0*/g_itemsFilter,0);
	return PRIVATE_SUBTABLE_OK;

}








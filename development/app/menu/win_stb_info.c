/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_stb_info.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.15		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"
#include "gxapp_sys_config.h"
#include "gxapp_development_svn.h"


SIGNAL_HANDLER int win_stb_info_create(const char* widgetname, void *usrdata)
{
	char* osd_language=NULL;
	char caUser[MAX_SEARIAL_LEN+1]={0};
	char *temp;
	int8_t chBuffer[60]={0};
	int16_t year,month,day;

    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))    
    {
 #ifdef SD
 //   	    GUI_SetProperty("win_stb_info_title", "img", "DS_TITLE_SYS_INFO.bmp");
#endif
#ifdef HD

    	    GUI_SetProperty("win_stb_info_title", "img", "title_stbinfo.bmp");
    	GUI_SetProperty("win_stb_info_tip_image_exit", "img", "tips_exit.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
 #ifdef SD
    //	    GUI_SetProperty("win_stb_info_title", "img", "DS_TITLE_SYS_INFO_ENG.bmp");
#endif

#ifdef HD
    	    GUI_SetProperty("win_stb_info_title", "img", "title_stbinfo_e.bmp");
    	GUI_SetProperty("win_stb_info_tip_image_exit", "img", "tips_exit_e.bmp");
#endif   
    } 
    
/*begin:modify by cth*/  
//全智CA特殊处理
#ifdef DVB_CA_TYPE_QZ_FLAG
    int len = MAX_SEARIAL_LEN;
    extern int DVTSTBCA_GetValidSNID(uint8_t *pSN,int *len);
    DVTSTBCA_GetValidSNID((uint8_t *)caUser,&len);
#else
	app_flash_get_serial_number(0,(char *)caUser,MAX_SEARIAL_LEN);
#endif
/*end:modify by cth*/

	temp = app_flash_get_oem_hardware_version_str();
	GUI_SetProperty("win_stb_info_hardware_version_value", "string", temp);
	
	temp = app_flash_get_oem_softversion_str();;
	GUI_SetProperty("win_stb_info_software_version_value", "string", temp);
	
	GUI_SetProperty("win_stb_info_serial_value", "string", caUser);	  // 序列号

#ifdef DEVELOPMENT_SVN
//    sprintf((char *)chBuffer, "%d", DEVELOPMENT_SVN);
  //  GUI_SetProperty("win_stb_info_svn_value", "string", chBuffer);	
#endif
    
	sprintf((char *)chBuffer, "%s %s", __DATE__, __TIME__);
	year = (chBuffer[7]-'0')*1000+(chBuffer[8]-'0')*100+(chBuffer[9]-'0')*10+(chBuffer[10]-'0');
	if (('0' == chBuffer[4])||('1' == chBuffer[4])||('2' == chBuffer[4])||('3' == chBuffer[4]))
	{
		day = (chBuffer[4]-'0')*10+(chBuffer[5]-'0');
	}
	else
	{
		day = (chBuffer[5]-'0');		
	}

	switch(chBuffer[2])
	{
		case 'n':
			if ('a' == chBuffer[1])
				month = 1;
			else
				month = 6;					
			break;
		case 'b':
			month = 2;
			break;
		case 'r':
			if ('M' == chBuffer[0])
				month = 3;
			else
				month = 4;				
			break;
		case 'y':
			month = 5;		
			break;
		case 'l':
			month = 7;	
			break;
		case 'g':
			month = 8;	
			break;
		case 'p':
			month = 9;				
			break;
		case 't':
			month = 10;	
			break;
		case 'v':
			month = 11;	
			break;
		case 'c':
			month = 12;	
			break;
		default:
			month = 1;			
			break;
		
	}
	memset(chBuffer,0,60);
	sprintf((char *)chBuffer, "%d.%02d.%02d %s", year,month,day, __TIME__);
	GUI_SetProperty("win_stb_info_Issue_date_value", "string", chBuffer);//软件编译时间
	GUI_SetProperty("win_stb_info_time_text", "string", app_win_get_local_date_string());//系统时间
#ifdef DVB_AD_TYPE_DS_FLAG
	memset(chBuffer,0,60);
	DSAD_GetADVersion(chBuffer);
	GUI_SetProperty("win_stb_info_ad_version_value", "string", chBuffer);
#endif
	return 0;
}

SIGNAL_HANDLER  int win_stb_info_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER int win_stb_info_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_RECALL:
		case KEY_EXIT:
		case KEY_OK:
		case KEY_MENU:
			GUI_EndDialog("win_stb_info");
			return EVENT_TRANSFER_STOP;								
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


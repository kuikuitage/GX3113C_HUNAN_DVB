/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_program_detail.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
	VERSION	Date			  AUTHOR         Description
	 1.0  	2010.04.13		  lijq 	 			creation
*****************************************************************************/
#include "app.h"
/*
extern uint8_t caUser[48];
extern uint32_t gApp_cur_prog_pos;
extern const char* TEXT_SCRAM_FLAG;
extern const char* TEXT_AUDIOTRACK[4] ;
extern int panel_fd;

*/
	static event_list* spApp_fatory_menu_time_port = NULL;

uint8_t usb_flag =0;

int app_factory_detect_usb(void)
{

	HotplugPartitionList* partition_list =NULL;

	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

	if((NULL != partition_list) && (0 < partition_list->partition_num))
	{
		return 0;
	}
	else
	{
		return 1;
	}
	//return ;
}

static  int fatory_menu_ca_time(void *userdata)
{

	if( app_factory_detect_usb())
	{
		GUI_SetProperty("win_factory_menu_text_usb", "string", "<未插入 USB>");
	}
	else 
	{
		GUI_SetProperty("win_factory_menu_text_usb", "string", "<USB 正常 >");
	}
	#if 0
	if(TRUE == SC_CheckCardIn())
	{
	    //wRet = CDCASTB_GetCardSN( chCardSN );
	    if( wRet == CDCA_RC_OK )
	    {
	    	sprintf( sTextBuffer,"CardNo:%s",chCardSN);
	        GUI_SetProperty("win_factory_menu_text_ca_card", "string",sTextBuffer);
	    }
	    else
	    {
	    	//sprintf( sTextBuffer,"%s:%s","CardNo:%s",chCardSN);
	        GUI_SetProperty("win_factory_menu_text_ca_card", "string","无法识别卡");
	    }
	}
	else
	{
		GUI_SetProperty("win_factory_menu_text_ca_card", "string","未插入智能卡");
	}
	#endif
	return 0;
}



SIGNAL_HANDLER  int app_factory_menu_create(const char* widgetname, void *usrdata)
{

	char caUser[MAX_SEARIAL_LEN+1]={0};
	char *temp;
	int8_t chBuffer[60]={0};

	temp = app_flash_get_oem_hardware_version_str();

	GUI_SetProperty("win_factory_menu_text_hwver", "string", temp);

	temp = app_flash_get_oem_softversion_str();
	GUI_SetProperty("win_factory_menu_text_softver", "string", temp);


//	temp = GxOem_GetValue("software", "logo_version");
//	GUI_SetProperty("win_setbox_info_logo", "string", temp);
	
	app_flash_get_serial_number(0,(char *)caUser,MAX_SEARIAL_LEN);

	GUI_SetProperty("win_factory_menu_text_stbid", "string", caUser);	  // 序列号

	sprintf((char *)chBuffer, "%s:%s","Ptime", __DATE__);
	GUI_SetProperty("win_factory_menu_text_time", "string", chBuffer);

	//GUI_SetProperty("win_factory_menu_text_loader", "string", sTextBuffer);
	spApp_fatory_menu_time_port = create_timer(fatory_menu_ca_time, 500, NULL,  TIMER_REPEAT);
	

	//wRet = CDCASTB_GetVer();
    //sprintf( sTextBuffer, "CA库版本:0x%08X", (unsigned int)wRet );//未知
    //GUI_SetProperty("win_factory_menu_text_card_lib", "string",sTextBuffer);
    
	return 0;
}

SIGNAL_HANDLER  int app_factory_menu_destroy(const char* widgetname, void *usrdata)
{

	remove_timer(spApp_fatory_menu_time_port);
	spApp_fatory_menu_time_port = NULL;

	return 0;
}

SIGNAL_HANDLER  int app_factory_menu_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_GREEN:
				GUI_EndDialog("win_factory_menu");
				GUI_CreateDialog("win_factory_menu_scan");
				return EVENT_TRANSFER_STOP;	
			case KEY_YELLOW:
			case KEY_MENU:
				GUI_EndDialog("win_factory_menu");
				GUI_CreateDialog("win_factory_menu_dplay");
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL: 
			case KEY_EXIT:
				//GxBus_ConfigSetInt(FACTORY_MENU_FLAG, 0);
				//GUI_EndDialog("win_factory_menu");
				//GUI_CreateDialog("win_full_screen");
				return EVENT_TRANSFER_STOP;	
			case KEY_RED:
				GxBus_ConfigSetInt(FRONTEND_CONFIG_NUM,1);
				GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_XRES,VIDEO_WINDOW_W);
				GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_SCREEN_YRES,VIDEO_WINDOW_H);
				GUI_EndDialog("win_factory_menu");
				GxCore_ThreadDelay(400);
				restart();
				//GUI_CreateDialog("win_full_screen");
				
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;	
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}


int app_factory_menu_service(void* usrdata)
{	
	GxMessage * msg;
	msg = (GxMessage*)usrdata;
	switch(msg->msg_id)
	{
        case GXMSG_HOTPLUG_IN:
			printf("[factory SERVICE] GXMSG_HOTPLUG_IN\n");
			GUI_SetProperty("win_factory_menu_text_usb", "string", "<USB 正常 >");
			//usb_flag = 1;
            //para_usb = GXLED_USB;
            //ioctl(panel_fd, PANEL_STRING, &para_usb);
			return EVENT_TRANSFER_STOP;
		case GXMSG_HOTPLUG_OUT:
			printf("[factory SERVICE] GXMSG_HOTPLUG_OUT\n");
			GUI_SetProperty("win_factory_menu_text_usb", "string", "<未插入 USB>");
			//usb_flag = 0;
           // para_usb = GXLED_SEAR;
           // ioctl(panel_fd, PANEL_STRING, &para_usb);
			return EVENT_TRANSFER_STOP;
		default:
			break;
	}
	return 0;
}
SIGNAL_HANDLER  int app_factory_menu_port_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

	switch(event->type)
	{
	case GUI_SERVICE_MSG:
		app_factory_menu_service(event->msg.service_msg);
		break;
	default:
		break;
	}

	return EVENT_TRANSFER_STOP;
}






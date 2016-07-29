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
#include "gxprogram_manage_berkeley.h"
//extern int panel_fd;
//static event_list* spApp_fatory_time_port = NULL;



static  int fatory_menu_port_time(void *userdata)
{

	uint32_t key = 0;
	//gx3113_panel_detet_key(&key);
	//printf("gx3113_panel_detet_key %x \n",key);
	if(0 !=key)
	{	
		if(0x4450 == key)
		{
			GUI_SetProperty("win_factory_menu_port_text_key", "string", "<频道+>");	
		}
		else if(0x5344 == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string", "<频道->");
		}
		else if(0x5a4a == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string","<音量+>");
		}
		else if(0x4548 == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string", "<菜单>");
		}
		else if(0x454e == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string","<确认>");
		}
		else if(0x4e4f == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string","<音量->");
		}
		else if(0x5057 == key)
		{
		GUI_SetProperty("win_factory_menu_port_text_key", "string","<待机>");
		}
		
	}

	return 0;
}
SIGNAL_HANDLER  int app_factory_menu_port_create(const char* widgetname, void *usrdata)
{

	int value2;
	int config;

	GUI_GetProperty("win_factory_menu_port_box_set", "select", &value2);
	if (0 == value2)
	{
		GUI_GetProperty("win_factory_menu_port_combobox17", "select", &config);
		if(1==config)
		{

			printf("gx_stb_panel_pt6964_standby\n");

		}
		else if(0 == config)
		{

			printf("gx_stb_panel_pt6964_on\n");
		}
		
	}
	else if(1 == value2)
	{
		GUI_GetProperty("win_factory_menu_port_combobox18", "select", &config);
		if(0== config)
		{
			//app_dvbc_dtmb_control_low();

			printf("app_dvbc_dtmb_control_low\n");
		}
		else if(1 == config)
		{
			//app_dvbc_dtmb_control_high();

			printf("app_dvbc_dtmb_control_high\n");
		}
	}

	return 0;
}

SIGNAL_HANDLER  int app_factory_menu_port_destroy(const char* widgetname, void *usrdata)
{

	return 0;

}

SIGNAL_HANDLER  int app_factory_menu_port_box_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_factory_menu_port_keypress(const char* widgetname, void *usrdata)
{

	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int value2;
	int config;
	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{
			case KEY_GREEN:
				GUI_EndDialog("win_factory_menu_port");
				GUI_CreateDialog("win_factory_menu_dplay");
				return EVENT_TRANSFER_STOP;	
			case KEY_RECALL: 
			case KEY_EXIT:
				GUI_EndDialog("win_factory_menu_port");
				GUI_CreateDialog("win_factory_menu");
				return EVENT_TRANSFER_STOP;	
			case KEY_YELLOW:
			case KEY_MENU:
				GUI_EndDialog("win_factory_menu_port");
				GUI_CreateDialog("win_factory_menu_scan");
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			case KEY_LEFT:
			case KEY_RIGHT:
				GUI_GetProperty("win_factory_menu_port_box_set", "select", &value2);
				if (0 == value2)
				{
					GUI_GetProperty("win_factory_menu_port_combobox17", "select", &config);
					if(1==config)
					{
						//gx_stb_panel_pt6964_standby();
						//gx_stb_panel_pt6964_unlock();
						printf("gx_stb_panel_pt6964_standby\n");

					}
					else if(0 == config)
					{
						//gx_stb_panel_pt6964_lock();
						//gx_stb_panel_pt6964_on();
						printf("gx_stb_panel_pt6964_on\n");
					}
					
				}
				else if(1 == value2)
				{
					GUI_GetProperty("win_factory_menu_port_combobox18", "select", &config);
					if(0== config)
					{
						//app_dvbc_dtmb_control_low();
						//gx_stb_panel_set_gpio_level(21,0);
						printf("app_dvbc_dtmb_control_low\n");
					}
					else if(1 == config)
					{
						//app_dvbc_dtmb_control_high();
						//gx_stb_panel_set_gpio_level(21,1);
						printf("app_dvbc_dtmb_control_high\n");
					}
				}
				else if(2 == value2)
				{
					GUI_GetProperty("win_factory_menu_port_combobox_led", "select", &config);
					if(0== config)
					{
						//app_dvbc_dtmb_control_low();
						//gx_stb_panel_pt6964_set_led_value(8888);
						printf("gx_stb_panel_pt6964_set_led_value 8888 \n");
					}
					else if(1 == config)
					{
						//app_dvbc_dtmb_control_high();
						//gx_stb_panel_pt6964_set_led_value(0);
						printf("gx_stb_panel_pt6964_set_led_value 0000\n");
					}
					
				}
				return EVENT_TRANSFER_STOP;	
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}

	return EVENT_TRANSFER_KEEPON;
}








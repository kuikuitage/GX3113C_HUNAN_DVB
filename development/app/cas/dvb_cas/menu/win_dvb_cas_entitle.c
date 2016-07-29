/*****************************************************************************
 * 						   CONFIDENTIAL
 *        Hangzhou GuoXin Science and Technology Co., Ltd.
 *                      (C)2012, All right reserved
 ******************************************************************************

 ******************************************************************************
 * File Name :	app_ca_api.c
 * Author    : 	zhouhm
 * Project   :	goxceed dvbc
 * Type      :
 ******************************************************************************
 * Purpose   :	模块头文件
 ******************************************************************************
 * Release History:
 VERSION	Date			  AUTHOR         Description
 1.0  	2012.12.02		  zhouhm 	 			creation
 *****************************************************************************/
#include "app_common_porting_stb_api.h"
#include "app_win_interface.h"
#include "app_dvb_cas_api_entitle.h"

static ca_get_count_t get_count = {0};
static ca_get_count_t get_entitle_count = {0};

SIGNAL_HANDLER  int app_operatorinfo_create(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int  app_operatorinfo_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER  int app_operatorinfo_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	uint32_t g_entitleSel = 0;

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
			case KEY_MENU:
			case KEY_RECALL:
				GUI_EndDialog("win_dvb_cas_operator");
				return EVENT_TRANSFER_STOP;
			case KEY_OK:
				if ( 0 == get_count.totalnum)
				{
					app_popmsg(210, 200,"No Operator",POPMSG_TYPE_OK);
				}
				else
				{
					GUI_GetProperty("entitle_list","select",(void*)&g_entitleSel);
					printf("[debug] sel = %d \n",g_entitleSel);
					memset(&get_entitle_count,0,sizeof(ca_get_count_t));
					get_entitle_count.date_type = DVB_CA_ENTITLE_INFO;
					get_entitle_count.pos = g_entitleSel; // 行
					app_cas_api_get_count(&get_entitle_count);
					if (get_entitle_count.totalnum != 0)
					{
						app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						GUI_CreateDialog("win_dvb_cas_entitle");
					}
					else
					{
						app_popmsg(210, 200,"No Entitle",POPMSG_TYPE_OK);
					}
				}
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_STOP;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_operatorinfo_list_get_total(const char* widgetname, void *usrdata)
{
	memset(&get_count,0,sizeof(ca_get_count_t));
	get_count.date_type = DVB_CA_OPERATOR_INFO;
	app_cas_api_get_count(&get_count);

	return get_count.totalnum;
}

SIGNAL_HANDLER  int app_operatorinfo_list_get_data(const char* widgetname, void *usrdata)
{
	ca_get_date_t get_data = {0};
	unsigned int      Row    = 0;
	unsigned int      ID    = 0;
	ListItemPara *item_para =NULL;
	if(NULL == widgetname || NULL == usrdata )
	{
		return (1);
	}
	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para)
		return GXCORE_ERROR;

	Row = item_para->sel;

	get_data.date_type = DVB_CA_OPERATOR_INFO;
	get_data.pos = Row; // 行

	for (ID = DVB_OPERATOR_PRODUCT_OPERATOR_ID;ID<=DVB_OPERATOR_PRODUCT_ENDTIME_ID;ID++)
	{
		get_data.ID = ID;
		item_para->string = app_cas_api_get_data(&get_data);

		if (ID < DVB_OPERATOR_PRODUCT_ENDTIME_ID)
		{
			/*
			 * 不是最后一列
			 */
			item_para= item_para->next;
			if(NULL == item_para)
				return GXCORE_ERROR;
		}
	}

	return 0;
}

SIGNAL_HANDLER  int app_operatorinfo_list_change(const char* widgetname, void *usrdata)
{
	return 0;
}



SIGNAL_HANDLER  int app_entitle_create(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER  int  app_entitle_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}


SIGNAL_HANDLER  int app_entitle_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_EXIT:
			case KEY_MENU:
			case KEY_RECALL:
				GUI_EndDialog("win_dvb_cas_entitle");
				return EVENT_TRANSFER_STOP;
			case KEY_OK:
				return EVENT_TRANSFER_STOP;
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;
			default:
				return EVENT_TRANSFER_STOP;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER  int app_entitle_list_get_total(const char* widgetname, void *usrdata)
{
	return get_entitle_count.totalnum;
}

SIGNAL_HANDLER  int app_entitle_list_get_data(const char* widgetname, void *usrdata)
{
	ca_get_date_t get_data = {0};
	unsigned int      Row    = 0;
	unsigned int      ID    = 0;
	ListItemPara *item_para =NULL;
	if(NULL == widgetname || NULL == usrdata )
	{
		return (1);
	}
	item_para = (ListItemPara *)(usrdata);
	if(NULL == item_para)
		return GXCORE_ERROR;

	Row = item_para->sel;

	get_data.date_type = DVB_CA_ENTITLE_INFO;
	get_data.pos = Row; // 行

	for (ID = DVB_ENTITLE_PRODUCT_NUM;ID<=DVB_ENTITLE_PRODUCT_END_TIME;ID++)
	{
		get_data.ID = ID;
		item_para->string = app_cas_api_get_data(&get_data);

		if (ID < DVB_ENTITLE_PRODUCT_END_TIME)
		{
			/*
			 * 不是最后一列
			 */
			item_para= item_para->next;
			if(NULL == item_para)
				return GXCORE_ERROR;
		}
	}

	return 0;
}

SIGNAL_HANDLER  int app_entitle_list_change(const char* widgetname, void *usrdata)
{
	return 0;
}




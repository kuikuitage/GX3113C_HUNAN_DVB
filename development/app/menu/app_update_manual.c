/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app__update_manual.c
* Author    : 	wuqian
* Project   :	
* Type      :	
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   1.0  	2010.04.19		  lijq 	 			creation
*****************************************************************************/
#include "app.h"

#include <sys/ioctl.h>


#define STR_UPGRADE_USB_INVALIDE_PATH         "Invalid file path!"
#define STR_UPGRADE_USB_INVALIDE_FILE_SIZE    "Invalid upgrade file size!"
#define STR_UPGRADE_USB_INSERT_DEVICE         "Insert the device please"
#define STR_UPGRADE_USB_UPDATE_SUCCESS        "Upgrade successfully,reboot now!"
#define STR_UPGRADE_USB_DUMP_SUCCESS          "Dump successfully!"
//static event_list* sApp_Update_Timer = NULL;
static char *s_file_path = NULL;
static char *s_file_path_bak = NULL;
static int8_t start_flag = 0;

#define UPGRADE_SUFFIX "bin"

static GxUpdate_ProtocolOps* protocol_list[] = 
{
    &gxupdate_protocol_ts,
    &gxupdate_protocol_serial,
    &gxupdate_protocol_usb,
    NULL
};
static GxUpdate_PartitionOps* partition_list[] = 
{
    &gxupdate_partition_flash,
    &gxupdate_partition_file,
    NULL
};
GxUpdate_ProtocolOps**  gxupdate_protocol_list = protocol_list;
GxUpdate_PartitionOps** gxupdate_partition_list = partition_list;

typedef enum
{
	UPGRADE_TYPE_U2S,	
	UPGRADE_TYPE_DUMP
}UpgradeTypeSel;

typedef enum
{
	UPGRADE_SECT_ALL,
	UPGRADE_SECT_BOOTER,
	UPGRADE_SECT_LOADER,
	UPGRADE_SECT_KERNEL,
	UPGRADE_SECT_ROOTFS,
	UPGRADE_SECT_LOGO,
	
}UpgradeSectSel;




typedef struct
{
	UpgradeTypeSel type;//导入\导出
	UpgradeSectSel section;//分区
	char *path;//路径
}UpgradePara;

int check_usb_status(void)
{
    HotplugPartitionList* list = NULL;
    list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
    
    if((NULL != list)
        && (HOTPLUG_TYPE_USB == list->type)
        && (0 < list->partition_num))
    {
        return 1;
    }
    return 0;
}

static status_t app_get_partition_info(UpgradeSectSel section, GxUpdate_ConfigFlash *conf)
{
	GxPartitionTable *table_info = NULL;
	status_t ret = GXCORE_SUCCESS;
	int i;

	table_info = GxCore_Malloc(sizeof(GxPartitionTable));
	if(NULL == table_info)
		return  GXCORE_ERROR;

	memset(table_info,0,sizeof(GxPartitionTable));
	if(GxOem_PartitionTableGet(table_info) < 0)
	{
		GxCore_Free(table_info);
		table_info = NULL;
		return GXCORE_ERROR;
	}

	if(UPGRADE_SECT_ALL == section)
	{
		conf->start_addr = 0;
		conf->size = 0;
		APP_Printf("count [%d]\n",table_info->count);
//		for(i = 0; i<table_info->count-1; i++)
		for(i = 0; i<table_info->count; i++)
		{
			conf->size += table_info->tables[i].total_size;
			APP_Printf("%s \n",table_info->tables[i].name);
		}
		APP_Printf("total size = [0x%08x]\n",conf->size);
	}
	else
	{
		char *parti_name = NULL;

		switch(section)
		{
			case UPGRADE_SECT_BOOTER:
				parti_name = GX_PARTITION_BOOT;
				printf("GX_PARTITION_BOOT \n");
    			break;		
			case UPGRADE_SECT_LOADER:
				parti_name = GX_PARTITION_DOWNLOADER;
				printf("GX_PARTITION_DOWNLOADER \n");
    			break;
			case UPGRADE_SECT_KERNEL:
				parti_name = /*GX_PARTITION_V_OEM*/GX_PARTITION_KERNEL;
				printf("GX_PARTITION_KERNEL \n");
				break;
			case UPGRADE_SECT_ROOTFS:
				parti_name = GX_PARTITION_ROOT;
				printf("GX_PARTITION_ROOT \n");
    			break;		
			case UPGRADE_SECT_LOGO:
				parti_name = GX_PARTITION_LOGO;
				printf("GX_PARTITION_LOGO \n");
				break;

			default:
				ret = GXCORE_ERROR;
				break;
		}

		if(ret != GXCORE_ERROR)
		{
			for(i = 0; i < table_info->count; i++)
			{
				if(table_info->tables[i].name != NULL 
					&& 0 == strcmp(table_info->tables[i].name, parti_name))
				{
					
					conf->start_addr = table_info->tables[i].start_addr;
					conf->size = table_info->tables[i].total_size;
					printf("%s start=%x,size = %x\n",table_info->tables[i].name,conf->start_addr,conf->size);
				}
			}

			if(i >= table_info->count) //can't find partition
				ret = GXCORE_ERROR;
		}
	}

	GxCore_Free(table_info);
	table_info = NULL;
	
	return ret;
	
}
static status_t app_usb_upgrade_check_file_size(UpgradePara *ret_para)
{
	GxUpdate_ConfigFlash tem_config;
	GxFileInfo file_info;
	GxUpdate_ConfigFlash config;
	GxPartitionTable table_info;
	status_t ret = GXCORE_ERROR;
	
	if (NULL == ret_para)
	{
	    return GXCORE_ERROR;
	}
	if(UPGRADE_TYPE_DUMP == ret_para->type)
	{
	    return GXCORE_SUCCESS;
	}
	memset(&config,0,sizeof(GxUpdate_ConfigFlash));
	memset(&tem_config,0,sizeof(GxUpdate_ConfigFlash));
	memset(&file_info,0,sizeof(GxFileInfo));
	memset(&table_info,0,sizeof(GxPartitionTable));
	
	GxOem_PartitionTableGet(&table_info);
	GxCore_GetFileInfo(ret_para->path,&file_info);
	app_get_partition_info(ret_para->section,&config);

	do
	{
        if (file_info.size_by_bytes == config.size)
        {
            ret = GXCORE_SUCCESS;
			break;
        }
	    if (UPGRADE_SECT_KERNEL == ret_para->section)
	    {
	        app_get_partition_info(UPGRADE_SECT_ROOTFS,&tem_config);
			if (file_info.size_by_bytes == (config.size + tem_config.size))
			{
			    ret = GXCORE_SUCCESS;
			}
			break;
	    }
	}while(0);
//	APP_Printf("[check size]addr = %08x,size = %08x,file_size = %08x\n",	
//	config.start_addr,config.size,file_info.size_by_bytes);
	return ret;
}
static status_t app_usb_upgrade_check_file_path(UpgradePara *ret_para)
{
    status_t ret = GXCORE_SUCCESS;

    do{
	    if (NULL == ret_para->path)
	    {
	        ret = GXCORE_ERROR;
			break;
	    }
		if (UPGRADE_TYPE_U2S == ret_para->type)
		{
		    if (GxCore_FileExists(ret_para->path) == GXCORE_FILE_UNEXIST)
		    {				
				ret = GXCORE_ERROR;
				break;
		    }
		}
		else if (UPGRADE_TYPE_DUMP == ret_para->type)
		{
		    if (GxCore_FileExists(ret_para->path) == GXCORE_FILE_UNEXIST)
		    {
		        ret = GXCORE_ERROR;
				break;
		    }
		}
    }while(0);
	return ret;
}
#ifdef ECOS_OS
static status_t app_usb_upgrade_config(UpgradePara *ret_para)
{
    GxMsgProperty_UpdateOpen updateOpen;
    GxMsgProperty_UpdateProtocolSelect protSel;
    GxUpdate_ConfigUsbTerminalType type;
    GxMsgProperty_UpdatePartitionSelect partSel;
    GxUpdate_IoCtrl ctrl;
    GxUpdate_SelectUsbFile info;
    GxUpdate_ConfigFlash config;
    int32_t len = 0;
    int32_t state = GXUPDATE_OK - 1;
	status_t ret = GXCORE_SUCCESS;

    if(NULL == ret_para->path)
    {
  	 	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

        return GXCORE_ERROR;
    }

	/*Check file size for upgrade*/
	ret = app_usb_upgrade_check_file_size(ret_para);
	if (ret != GXCORE_SUCCESS)
	{
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_UPGRADE_USB_INVALIDE_FILE_SIZE, POPMSG_TYPE_OK);
		return GXCORE_ERROR;
	}
	/*Check file path for dump*/
   	ret = app_usb_upgrade_check_file_path(ret_para);
	if (ret != GXCORE_SUCCESS)
	{
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);
		return GXCORE_ERROR;
	}
	
    memset(&info, 0, sizeof(GxUpdate_SelectUsbFile));
    if(UPGRADE_TYPE_U2S== ret_para->type)
    {
        len = strlen(ret_para->path);
        if((len >= GXUPDATE_MAX_FILE_PATH_LENGTH) || (len <= 0))
        {
	     	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);
            return GXCORE_ERROR;
        }
        memcpy(info.file, ret_para->path, len);
        type.type = GXUPDATE_CLIENT;
    }
    else if(UPGRADE_TYPE_DUMP == ret_para->type)
    {
        len = strlen(ret_para->path);
        if((len >= (GXUPDATE_MAX_FILE_PATH_LENGTH-10)) || (len <= 0))
        {
	     	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

            return GXCORE_ERROR;
        }

        if(UPGRADE_SECT_ALL == ret_para->section)
    	{
            sprintf(info.file, "%s/dump_All.bin", ret_para->path);
    	}
    	else if(UPGRADE_SECT_BOOTER== ret_para->section)
    	{
            sprintf(info.file, "%s/dump_Booter.bin", ret_para->path);
    	}
    	else if(UPGRADE_SECT_KERNEL == ret_para->section)
    	{
            sprintf(info.file, "%s/dump_Kernel.bin", ret_para->path);
    	}
    	else if(UPGRADE_SECT_ROOTFS == ret_para->section)
    	{
            sprintf(info.file, "%s/dump_Root_fs.bin", ret_para->path);
    	}
    	else if(UPGRADE_SECT_LOADER== ret_para->section)
    	{
            sprintf(info.file, "%s/dump_Loader.bin", ret_para->path);
    	}
        else if(UPGRADE_SECT_LOGO== ret_para->section)
    	{
            sprintf(info.file, "%s/dump_Logo.bin", ret_para->path);
    	}
    	else
    	{
            sprintf(info.file, "%s/dump_All.bin", ret_para->path);
    	}
        type.type = GXUPDATE_SERVER;
    }
	
    memset(&updateOpen, 0, sizeof(GxMsgProperty_UpdateOpen));
    memcpy(&updateOpen, "NULL", sizeof(GxMsgProperty_UpdateOpen));
    app_send_msg(GXMSG_UPDATE_OPEN, &updateOpen);

    memset(&protSel,0,sizeof(GxMsgProperty_UpdateProtocolSelect));
    memcpy(&protSel, GXUPDATE_PROTOCOL_USB, sizeof(GxMsgProperty_UpdateProtocolSelect));
    app_send_msg(GXMSG_UPDATE_PROTOCOL_SELECT, &protSel);
    
    ctrl.key = GXUPDATE_USB_SELECT_TERMINAL_TYPE;
    ctrl.buf = &type;
    ctrl.size = sizeof(GxUpdate_ConfigUsbTerminalType);
    app_send_msg(GXMSG_UPDATE_PROTOCOL_CTRL, (void*)&ctrl);
/*
    ctrl.key = GXUPDATE_USB_SET_CRC; //crc
    ctrl.buf = NULL;
    ctrl.size = 0;
    app_send_msg(GXMSG_UPDATE_PROTOCOL_CTRL, (void*)&ctrl);
*/
    ctrl.key = GXUPDATE_SELECT_FILE_NAME;
    ctrl.buf = &info;
    ctrl.size = sizeof(GxUpdate_SelectUsbFile);
    app_send_msg(GXMSG_UPDATE_PROTOCOL_CTRL, (void*)&ctrl);
   
    state = GxUpdate_StreamGetStatus();
    if(state < GXUPDATE_OK)
    {
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Error occur!", POPMSG_TYPE_OK);

        return GXCORE_ERROR;
    }

    memset(&partSel,0,sizeof(GxMsgProperty_UpdatePartitionSelect));
    memcpy(&partSel,GXUPDATE_PARTITION_FLASH, sizeof(GxMsgProperty_UpdatePartitionSelect));
    app_send_msg(GXMSG_UPDATE_PARTITION_SELECT, &partSel);

    memset(&config,0,sizeof(GxUpdate_ConfigFlash));
    app_get_partition_info(ret_para->section, &config);

#if 1
    // TODO: Upgrade KERNEL and ROOT_FS with single file.
    if (UPGRADE_SECT_KERNEL == ret_para->section)
    {
        GxUpdate_ConfigFlash tem_config;
		GxFileInfo file_info;
		memset(&file_info,0,sizeof(GxFileInfo));
		GxCore_GetFileInfo(info.file,&file_info);
		memset(&tem_config,0,sizeof(GxUpdate_ConfigFlash));
		app_get_partition_info(UPGRADE_SECT_ROOTFS, &tem_config);
    	if(tem_config.size + config.size == file_info.size_by_bytes)
    	{
    	    config.size += tem_config.size;
    	}
//		APP_Printf("section addr = %08x,size = %08x,file_size = %08x\n",	
//		config.start_addr,config.size,file_info.size_by_bytes);
    }
#endif

    ctrl.key = GXUPDATE_CONFIG_FLASH;
    ctrl.buf = &config;
    ctrl.size = sizeof(GxUpdate_ConfigFlash);
    app_send_msg(GXMSG_UPDATE_PARTITION_CTRL, (void*)&ctrl);   

    if( type.type == GXUPDATE_CLIENT)
    {
         //app_flash_unlock();//unlock flash if flash locked
    }

    return GXCORE_SUCCESS;
    
}
#endif
#ifdef LINUX_OS
static event_list* sp_UpgradeTimeOut = NULL;
static uint32_t s_step_num = 0;
static uint32_t s_update_rate = 0;
int g_system_shell_ret_status = 0;
void usb_update_proc(void* userdata)
{
    UpgradePara *ret_para = (UpgradePara *)userdata;
    char precent_v[10];
    if(ret_para->type == UPGRADE_TYPE_U2S)
    {
	if(ret_para->section == UPGRADE_SECT_ALL)
	{
	    if(!(s_step_num % 35))
	    {
		s_update_rate++;
		if(s_update_rate>=100)
		    s_update_rate = 60;
		sprintf(precent_v,"%d",s_update_rate);
		GUI_SetProperty("update_manual_progbar", "value", &s_update_rate);	
		GUI_SetProperty("win_update_manual_text_prog_percent", "string",precent_v);
		reset_timer(sp_UpgradeTimeOut);
	    }
	}	
	else
	{
	    if((s_step_num % 40))
	    {
		s_update_rate++;
		if(s_update_rate>=100)
		    s_update_rate = 60;
		sprintf(precent_v,"%d",s_update_rate);
		GUI_SetProperty("update_manual_progbar", "value", &s_update_rate);	
		GUI_SetProperty("win_update_manual_text_prog_percent", "string",precent_v);
		reset_timer(sp_UpgradeTimeOut);
	    }
	}
    }
    if(ret_para->type == UPGRADE_TYPE_DUMP)
    {
	if(s_step_num % 40)
	{
	    s_update_rate++;
	    if(s_update_rate>=100)
		s_update_rate = 60;
	    sprintf(precent_v,"%d",s_update_rate);
	    GUI_SetProperty("update_manual_progbar", "value", &s_update_rate);	
	    GUI_SetProperty("win_update_manual_text_prog_percent", "string",precent_v);
	    reset_timer(sp_UpgradeTimeOut);
	}
    }
    s_step_num++;
}

void usb_update_ok(void* userdata)
{
    char precent_v[10];
    s_update_rate = 100;
    sprintf(precent_v,"%d",s_update_rate);
    GUI_SetProperty("update_manual_progbar", "value", (void*)&s_update_rate);
    GUI_SetProperty("win_update_manual_text_prog_percent", "string",precent_v);

    if(0 == g_system_shell_ret_status)
    {

    }
    else
    {

    }
    start_flag = 0;   
    remove_timer(sp_UpgradeTimeOut);
    sp_UpgradeTimeOut = NULL;

}

typedef void (*SYSTEM_SHELL_PROC)(void* userdata);
extern int system_shell(const char* s_cmd, int time_out, SYSTEM_SHELL_PROC step_proc, SYSTEM_SHELL_PROC finsh_proc, void* userdata);

static status_t app_usb_upgrade_config(UpgradePara *ret_para)
{
    //GxMsgProperty_UpdateOpen updateOpen;
    //GxMsgProperty_UpdateProtocolSelect protSel;
    //GxUpdate_ConfigUsbTerminalType type;
    //GxMsgProperty_UpdatePartitionSelect partSel;
    int32_t len = 0;
    //int32_t state = GXUPDATE_OK - 1;
    status_t ret = GXCORE_SUCCESS;
    char cmd_buf[512]={0};

    if(NULL == ret_para->path)
    {
  	 	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

        return GXCORE_ERROR;
    }

	/*Check file size for upgrade*/
//	ret = app_usb_upgrade_check_file_size(ret_para);
	if (ret != GXCORE_SUCCESS)
	{
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_FILE_SIZE, POPMSG_TYPE_OK);

		return GXCORE_ERROR;
	}
	/*Check file path for dump*/
   	ret = app_usb_upgrade_check_file_path(ret_para);
	if (ret != GXCORE_SUCCESS)
	{
		app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

		return GXCORE_ERROR;
	}
	
    if(UPGRADE_TYPE_U2S== ret_para->type)
    {
        len = strlen(ret_para->path);
        if((len >= GXUPDATE_MAX_FILE_PATH_LENGTH) || (len <= 0))
        {
	     	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

			return GXCORE_ERROR;
        }
    	if(UPGRADE_SECT_ALL == ret_para->section)
    	{
    	    sprintf(cmd_buf, "usb_update all \"%s\"", ret_para->path);
    	}
    	else if(UPGRADE_SECT_BOOTER== ret_para->section)
    	{
    	    sprintf(cmd_buf, "usb_update BOOT \"%s\"", ret_para->path);
    	}
    	else if(UPGRADE_SECT_KERNEL == ret_para->section)
    	{
    	    sprintf(cmd_buf, "usb_update KERNEL \"%s\"", ret_para->path);
    	}
    	else if(UPGRADE_SECT_ROOTFS == ret_para->section)
    	{
    	    sprintf(cmd_buf, "usb_update ROOTFS \"%s\"", ret_para->path);
    	}
    	else if(UPGRADE_SECT_LOADER== ret_para->section)
    	{
    	    sprintf(cmd_buf, "usb_update LOADER \"%s\"", ret_para->path);
    	}
    	else
    	{
    	    sprintf(cmd_buf, "usb_update all \"%s\"", ret_para->path);
    	}		
    }
    else if(UPGRADE_TYPE_DUMP == ret_para->type)
    {
        len = strlen(ret_para->path);
        if((len >= (GXUPDATE_MAX_FILE_PATH_LENGTH-10)) || (len <= 0))
        {
	     	app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_UPGRADE_USB_INVALIDE_PATH, POPMSG_TYPE_OK);

			return GXCORE_ERROR;
        }
        sprintf(ret_para->path, "%s/dump.bin", ret_para->path);
	if(UPGRADE_SECT_ALL == ret_para->section)
	{
	    sprintf(cmd_buf, "usb_dump all \"%s\"", ret_para->path);
	}
	else if(UPGRADE_SECT_BOOTER== ret_para->section)
	{
	    sprintf(cmd_buf, "usb_dump BOOT \"%s\"", ret_para->path);
	}
	else if(UPGRADE_SECT_KERNEL == ret_para->section)
	{
	    sprintf(cmd_buf, "usb_dump KERNEL \"%s\"", ret_para->path);
	}
	else if(UPGRADE_SECT_ROOTFS == ret_para->section)
	{
	    sprintf(cmd_buf, "usb_dump ROOTFS \"%s\"", ret_para->path);
	}
	else if(UPGRADE_SECT_LOADER== ret_para->section)
	{
	    sprintf(cmd_buf, "usb_dump LOADER \"%s\"", ret_para->path);
	}
	else
	{
	    sprintf(cmd_buf, "usb_dump all \"%s\"", ret_para->path);
	}
    }
    s_step_num = 0;
    s_update_rate = 0;

    g_system_shell_ret_status = 0; 
    start_flag = 1;
    system_shell(cmd_buf, 600000, usb_update_proc, usb_update_ok, ret_para);
    return GXCORE_SUCCESS;
    
}
#endif
/*
static  int timer_update_manual(void *userdata)
{		
	GUI_SetProperty("update_manual_data","string",app_get_local_time_string());		
	return 0;
}
*/
SIGNAL_HANDLER  int app_update_manual_create(const char* widgetname, void *usrdata)
{
	s_file_path =NULL;
	s_file_path_bak = NULL;
	start_flag = 0;
	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();


	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))		
	{
		#ifdef APP_HD
		GUI_SetProperty("win_update_manual_title", "img", "title_usbupdate.bmp");
    	GUI_SetProperty("win_update_manual_tip_image_ok", "img", "tips_confirm.bmp");

		#endif
	}
    else 
	{
		#ifdef APP_HD
    	GUI_SetProperty("win_update_manual_title", "img", "title_usbupdate_e.bmp");	
		GUI_SetProperty("win_update_manual_tip_image_ok", "img", "tips_confirm_e2.bmp");
	
    	#endif
	}

	app_set_win_create_flag(USB_UPDATE_WIN);


	return 0;
}

SIGNAL_HANDLER  int app_update_manual_destroy(const char* widgetname, void *usrdata)
{
	if(s_file_path_bak != NULL)
	{
		GxCore_Free(s_file_path_bak);
		s_file_path_bak = NULL;
	}

	s_file_path =NULL;
	app_start_all_monitor_filter();
	app_set_win_destroy_flag(USB_UPDATE_WIN);

	return 0;
}

SIGNAL_HANDLER  int app_update_manual_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;

	if(event->type == GUI_KEYDOWN)
	{	
		switch(event->key.sym)
		{			
			case KEY_EXIT:
				GUI_EndDialog("win_update_manual");					
				//GUI_EndDialog("win_system_menu");
				//GUI_EndDialog("win_main_menu");
				//app_exit_to_full_window();
				return EVENT_TRANSFER_STOP;
			case KEY_OK:
			case KEY_MENU:
				GUI_EndDialog("win_update_manual");				
				return EVENT_TRANSFER_STOP;	
			case KEY_POWER:
				return EVENT_TRANSFER_KEEPON;	
			default:
				return EVENT_TRANSFER_STOP;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER  int app_update_manual_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int value;
	int UpdateType;
	int UpdateSection = 0;
	popmsg_ret ret;
	WndStatus get_path_ret = WND_CANCLE;
	UpgradePara upgrade_para = {0};
	status_t ret1 = GXCORE_ERROR;

	if(event->type == GUI_KEYDOWN)
	{	
		APP_Printf("get key [%d]\n",event->key.sym);
		switch(event->key.sym)
		{
            case APP_KEY_RIGHT:
			case APP_KEY_LEFT:	
               GUI_GetProperty("update_manual_box", "select", &value);
               if(0 == value)
               {
                    char *str = NULL;
                    //GUI_GetProperty("update_manual_box_item1_combobox", "select", &UpdateType);
                    GUI_GetProperty("update_manual_box_item2_text2", "string", &str);
                    if(str != NULL)
                    {
                    	if(s_file_path_bak != NULL)
                    	{
                    		GxCore_Free(s_file_path_bak);
                    		s_file_path_bak = NULL;
                    	}
                    	s_file_path =NULL;
                        GUI_SetProperty("update_manual_box_item2_text2", "string", "Press OK");
                    }
                    break;
               }
			case APP_KEY_UP:
			case APP_KEY_DOWN:
			case APP_KEY_POWER:
			case APP_KEY_EXIT:
			case KEY_MENU:	
				if(start_flag)
				{
					return EVENT_TRANSFER_STOP;
				}
				break;	
			case KEY_OK:
				if(start_flag)
				{
					return EVENT_TRANSFER_STOP;
				}
				GUI_GetProperty("update_manual_box", "select", &value);
				GUI_GetProperty("update_manual_box_item1_combobox", "select", &UpdateType);
				GUI_GetProperty("update_manual_box_item4_combobox", "select", &UpdateSection);
				
				if(2 == value)//文件路径
				{
					if(check_usb_status() == 0)
					{
						ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_UPGRADE_USB_INSERT_DEVICE, POPMSG_TYPE_OK);
					}
					else
					{
						FileListParam file_para;
						memset(&file_para, 0, sizeof(file_para));

						file_para.cur_path = s_file_path_bak;
						file_para.dest_path = &s_file_path;
						file_para.suffix = UPGRADE_SUFFIX;
						if(0 == UpdateType)//更新
						{
							file_para.dest_mode = DEST_MODE_FILE;
							get_path_ret = app_get_file_path_dlg(&file_para);
						}
						else if(1 == UpdateType)//转出
						{
							file_para.dest_mode = DEST_MODE_DIR;
							get_path_ret = app_get_file_path_dlg(&file_para);
						}
						
						if(get_path_ret == WND_OK)
						{
							if(s_file_path != NULL)
							{
								int str_len = 0;	

                                printf("path:%s\n",s_file_path);

								GUI_SetProperty("update_manual_box_item2_text2", "string", s_file_path);
							////bak////
								if(s_file_path_bak != NULL)
								{
									GxCore_Free(s_file_path_bak);
									s_file_path_bak = NULL;
								}
							
								str_len = strlen(s_file_path);
								s_file_path_bak = (char *)GxCore_Malloc(str_len + 1);
								if(s_file_path_bak != NULL)
								{
									memcpy(s_file_path_bak, s_file_path, str_len);
									s_file_path_bak[str_len] = '\0';
								}								
							}
						}
					}
				}
				if(3 == value)//开始升级
				{
					if(check_usb_status() == 0)
					{
						ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_INSERT_DEVICE, POPMSG_TYPE_OK);

						return EVENT_TRANSFER_STOP;
					}
					else
					{
					    /*Upgrade OR Dump ?*/
						if(0 == UpdateType)//导入
						{
							upgrade_para.type = UPGRADE_TYPE_U2S;
						}
						else if(1 == UpdateType)//导出
						{
							upgrade_para.type = UPGRADE_TYPE_DUMP;
						}
						/*Which section ?*/
						if (0 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_ALL;
						}
						else if (1 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_BOOTER;
						}
						else if (2 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_LOADER;
						}
						else if (3 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_KERNEL;
						}
						else if (4 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_ROOTFS;
						}
						else if (5 == UpdateSection)
						{
							upgrade_para.section = UPGRADE_SECT_LOGO;
						}
						else
						{
							upgrade_para.section = UPGRADE_SECT_ALL;
						}
						
						upgrade_para.path = s_file_path;
						ret1 = app_usb_upgrade_config(&upgrade_para);
						if (GXCORE_SUCCESS != ret1)
						{
						    APP_Printf("[UPGRADE]Start upgrade error!\n");
						    return EVENT_TRANSFER_STOP;
						}
						
						if(0 == UpdateType)//导入
						{
							GUI_SetProperty("win_update_manual_upgrading_text", "string", "upgrading,don't poweroff or plug out usb!"); 
							GUI_SetProperty("win_update_manual_upgrading_text","state","show");
						}
					}
					if(ret1 == GXCORE_SUCCESS)
					{
						#ifdef ECOS_OS
						start_flag = 1;
						app_send_msg(GXMSG_UPDATE_START, NULL);
						#endif
					}
				}	
				return EVENT_TRANSFER_STOP;	
			default:
				return EVENT_TRANSFER_KEEPON;	
		}
	}
	
	return EVENT_TRANSFER_KEEPON;
}

int app_update_manual_service_msg(void* usrdata)
{
	GxMessage * msg;
	GxMsgProperty_UpdateStatus* p;
	uint32_t sel;
	uint8_t buf[50];
	char percent[10];	
	popmsg_ret ret;
	uint32_t rate;
	
	msg = (GxMessage*)usrdata;

	if(msg->msg_id == GXMSG_UPDATE_STATUS)
	{
		p = GxBus_GetMsgPropertyPtr(msg,GxMsgProperty_UpdateStatus);
		if(p->type == GXUPDATE_STATUS_PRERCENT)
		{
			APP_Printf("\n@@@@@@@@@percent: %d@@@@@@@@@@\n",p->percent);
			
			rate = (p->percent)/100;
			GUI_SetProperty("update_manual_progbar", "value", &rate);
			
			memset(percent,0,sizeof(percent));
			sprintf(percent,"%d%%",rate);
			GUI_SetProperty("win_update_manual_text_prog_percent", "string", percent);
			
		}
		else if(p->type == GXUPDATE_STATUS_ERROR)
		{
			if(p->error < 0)
			{
				memset(buf,0,sizeof(buf));
				sprintf((void*)buf,"Error:%d",p->error);
				rate = 0;
				GUI_SetProperty("update_manual_progbar", "value", &rate);
				memset(percent,0,sizeof(percent));
				sprintf(percent,"%d%%",rate);
				GUI_SetProperty("win_update_manual_text_prog_percent", "string", percent);
				app_send_msg(GXMSG_UPDATE_STOP,NULL);
				start_flag = 0;
				ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,(char*)buf, POPMSG_TYPE_OK);
			}
			else if(p->error == GXUPDATE_OK && start_flag)
			{
				
				rate = 100;
				APP_Printf("\n@@@@@@@@@ GXUPDATE_OK @@@@@@@@@@\n");
				#if 1
				GUI_SetProperty("update_manual_progbar", "value", &rate);
				memset(percent,0,sizeof(percent));
				sprintf(percent,"%d%%",rate);
				GUI_SetProperty("win_update_manual_text_prog_percent", "string", percent);
				GUI_SetProperty("update_manual_box","update",NULL);
				#endif
				GUI_GetProperty("update_manual_box_item1_combobox", "select", (void*)&sel);
				
				app_send_msg(GXMSG_UPDATE_STOP,NULL);
				if(sel == 0)
				{
//					ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_UPGRADE_USB_UPDATE_SUCCESS, POPMSG_TYPE_OK);
				}
				else
				{
					ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_UPGRADE_USB_DUMP_SUCCESS, POPMSG_TYPE_OK);
				}/**/
				if(sel == 0)
				{
//					GxCore_ThreadDelay(1000);
					restart();
				}
				else
				{
					rate = 0;				
					GUI_SetProperty("update_manual_progbar", "value", &rate);	
					memset(percent,0,sizeof(percent));
					sprintf(percent,"%d%%",rate);
					GUI_SetProperty("win_update_manual_text_prog_percent", "string", percent);
					start_flag = 0;
					GUI_SetProperty("update_manual_box","update",NULL);

					GUI_SetProperty("win_update_manual_upgrading_text", "string", " "); 
					GUI_SetProperty("win_update_manual_upgrading_text", "draw_now", NULL);
					GUI_SetProperty("win_update_manual_upgrading_text","state","hide");
					restart();
				}


			}
		}

	}

	return GXMSG_OK;
}

SIGNAL_HANDLER  int app_update_manual_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;

	event = (GUI_Event *)usrdata;

	switch(event->type)
	{
		case GUI_SERVICE_MSG:
			app_update_manual_service_msg(event->msg.service_msg);
			break;
		default:
			break;
	}

	return EVENT_TRANSFER_STOP;
}


#include "app.h"
#include "app_pvr.h"
#define PVR_PATH_LEN (16) // "/dev/usbxx" or "/dev/sdxx"


static HotplugPartitionList* partition_list = NULL;
extern int gi_FileCurSelect;

/*void system_setting_set_pvr_partition(int partitionNo)
{
    int init_value = partitionNo;
    GxBus_ConfigSetInt(PVR_PARTITION_KEY, init_value);
}*/

/*void system_pvr_partition_init(int *pvr_partition)
{
    if(NULL == pvr_partition)
        return;

    GxBus_ConfigGetInt(PVR_PARTITION_KEY, pvr_partition, PVR_PARTITION);
}*/
void app_create_disk_information_menu(void)
{

	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

	if((NULL != partition_list) && (0 < partition_list->partition_num))
	{
		GUI_CreateDialog("win_disk_information");
	}
	else
	{
		app_popmsg(POP_DIALOG_X_START,POP_DIALOG_Y_START,"Insert the device please",POPMSG_TYPE_OK);
		return;
	}
	return ;
}

SIGNAL_HANDLER int disk_information_create(const char* widgetname, void *usrdata)
{	
	GUI_SetFocusWidget("disk_information_listview");

	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

	if((NULL != partition_list) && (0 < partition_list->partition_num))
	{
		int sel = 0;

		gi_FileCurSelect = sel;
	}
	
	app_set_win_create_flag(PVR_MEDIA_WIN);
				
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int disk_information_destroy(const char* widgetname, void *usrdata)
{
	gi_FileCurSelect = -1;
	
	app_set_win_destroy_flag(PVR_MEDIA_WIN);
	return EVENT_TRANSFER_STOP;
}


SIGNAL_HANDLER int disk_information_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t sel = 0;

	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(find_virtualkey(event->key.sym))
		{		
			case STBK_EXIT:
				GUI_SetProperty("win_disk_information_title","state","hide");
				GUI_SetProperty("win_disk_information_title","draw_now",NULL);
				GUI_SetProperty("disk_information_listview","state","hide");
				GUI_SetProperty("disk_information_listview","draw_now",NULL);
				GUI_EndDialog("win_disk_information");
//				GUI_EndDialog("win_pvr_management");
//				GUI_EndDialog("win_media_center");
//				GUI_EndDialog("win_main_menu");
//				app_exit_to_full_window();
				return EVENT_TRANSFER_STOP;
			case STBK_MENU:
				GUI_SetProperty("win_disk_information_title","state","hide");
				GUI_SetProperty("win_disk_information_title","draw_now",NULL);
				GUI_SetProperty("disk_information_listview","state","hide");
				GUI_SetProperty("disk_information_listview","draw_now",NULL);
				GUI_EndDialog("win_disk_information");
				return EVENT_TRANSFER_STOP;
			
			case STBK_OK:
			{
				if(NULL == partition_list||0 == partition_list->partition_num)
				{
					return EVENT_TRANSFER_STOP;
				}
				GUI_GetProperty("disk_information_listview", "select", &sel);
				if(sel >= partition_list->partition_num)
				{
					GxBus_ConfigSet(PVR_PARTITION_KEY, partition_list->partition[0].dev_name);
				}
	//             	system_setting_set_pvr_partition(sel);
				GUI_SetProperty("disk_information_listview","update_all", NULL);
				GUI_SetProperty("disk_information_listview", "select", &sel);	
			}
			return EVENT_TRANSFER_STOP;

			case STBK_LEFT:
			case STBK_RIGHT:
				return EVENT_TRANSFER_STOP;
			default:
				break;
		}
	}
	return EVENT_TRANSFER_KEEPON;
}

SIGNAL_HANDLER int disk_information_listview_get_total(const char* widgetname, void *usrdata)
{
	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);
	if (NULL == partition_list)
		return 0;

	return partition_list->partition_num;
}

SIGNAL_HANDLER int disk_information_listview_get_data(const char* widgetname, void *usrdata)
{
	GxDiskInfo disk_info;
	ListItemPara *item_para;	
//	int partition = -1;
    int i = 0;
//  int num = 0;
    int32_t sel = 0;
    char partition[PVR_PATH_LEN] = {0};

	uint32_t temp_size = 0;
	static char pArry0[12] = {0};
	static char pArry1[12] = {0};
	static char pArry2[12] = {0};

	if(NULL == widgetname || NULL == usrdata)	
	{		
		return (1);	
	}	
	
	item_para = (ListItemPara *)(usrdata);
	partition_list = GxHotplugPartitionGet(HOTPLUG_TYPE_USB);

    if((NULL != partition_list) && (0 < partition_list->partition_num))
    { 
        GxBus_ConfigGet(PVR_PARTITION_KEY, partition, PVR_PATH_LEN, PVR_PARTITION);
	    for(i = 0; i < partition_list->partition_num; i++)
	    {
	        if(strcmp(partition, partition_list->partition[i].dev_name) == 0)
	        {
	            break;
	        }
	    }
	    if(i >= partition_list->partition_num)
	    {
	        i = 0;
	        GxBus_ConfigSet(PVR_PARTITION_KEY, partition_list->partition[i].dev_name);
			item_para->image = NULL;
	    }
		else
		{
			if(i == item_para->sel)
			{
//					item_para->image = "s_choice.bmp";
				item_para->image = NULL;
			}
			else
			{
				item_para->image = NULL;
			}
			
			item_para->string = NULL;
		}
    }
	
	item_para= item_para->next;
	item_para->string = partition_list->partition[item_para->sel].partition_name;

	GxCore_DiskInfo(partition_list->partition[item_para->sel].partition_entry, &disk_info);
	item_para= item_para->next;
	memset(pArry0, 0 ,sizeof(pArry0));
	temp_size = disk_info.used_size/1048576;
	sprintf((void*)pArry0,"%d M", temp_size);
	item_para->string = pArry0;
	
	item_para= item_para->next;
	memset(pArry1, 0 ,sizeof(pArry1));
	temp_size = disk_info.freed_size/1048576;
	sprintf((void*)pArry1,"%d M", temp_size);
	item_para->string = pArry1;
	
	item_para= item_para->next;
	memset(pArry2, 0 ,sizeof(pArry2));
	temp_size = disk_info.total_size/1048576;
	sprintf((void*)pArry2,"%d M", temp_size);
	item_para->string = pArry2;

	item_para= item_para->next;

	for(i = 0; i < sizeof(partition_list->partition[item_para->sel].fs_type); i++)
	{
		partition_list->partition[item_para->sel].fs_type[i] = toupper(partition_list->partition[item_para->sel].fs_type[i]);
	}
	
	item_para->string = partition_list->partition[item_para->sel].fs_type;

    GUI_GetProperty("disk_information_listview", "select", (void *)&sel);
    if(sel < 0)
    {
        sel = 0;
        GUI_SetProperty("disk_information_listview", "select", &sel);
    }

	gi_FileCurSelect = sel;

	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int disk_information_listview_draw(const char* widgetname, void *usrdata)
{

	//GUI_SetProperty("disk_information_listview", "select", &gi_FileCurSelect);	
	return 0;
}

void app_dm_list_hotplug_out(void)
{
	GUI_EndDialog("win_disk_information");
	GUI_SetProperty("win_disk_information", "draw_now", NULL);
}


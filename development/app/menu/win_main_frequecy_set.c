/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2010, All right reserved
******************************************************************************

******************************************************************************
* File Name :	win_search_result.c
* Author    : 	lijq
* Project   :	goxceed dvbc
* Type      :
******************************************************************************
* Purpose   :	模块头文件
******************************************************************************
* Release History:
VERSION	Date			  AUTHOR         Description
1.0  	2012.11.12		  zhouhm 	 			creation
*****************************************************************************/
#include "app.h"

extern float app_float_edit_str_to_value(const char *str);
SIGNAL_HANDLER int win_main_frequency_set_create(const char* widgetname, void *usrdata)
{
	search_dvbc_param tmp_param;
	char App_Fre[7];//频点
	char sApp_Sym[5];//符号率
    uint32_t sApp_Mod;//调制方式
	

	memset(&tmp_param,0,sizeof(search_dvbc_param));

	tmp_param.fre = app_flash_get_config_center_freq();
	memset(App_Fre,0,7);
	sprintf(App_Fre,"%03d.%d", tmp_param.fre/1000,(tmp_param.fre%1000)/100);
	GUI_SetProperty("win_main_frequency_value", "string", App_Fre); //frequency 

	tmp_param.symbol_rate= app_flash_get_config_center_freq_symbol_rate();
	tmp_param.qam= app_flash_get_config_center_freq_qam();

	memset(sApp_Sym,0,5);//符号率
	sprintf(sApp_Sym, "%04d", tmp_param.symbol_rate);
   	sApp_Mod = tmp_param.qam;

	GUI_SetProperty("win_main_frequency_symbol_value", "string", sApp_Sym);
	GUI_SetProperty("win_main_frequency_qam_value", "select", &sApp_Mod);

	GUI_SetProperty("win_main_frequency_set_time_text", "string", app_win_get_local_date_string());
#if DVB_DUAL_MODE
		int32_t widget_value = 0;
		
		widget_value = app_flash_get_config_dtmb_dvbc_switch();
		if(GXBUS_PM_SAT_1501_DTMB == widget_value)
		{
			GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","disable");
			GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","hide");
			GUI_SetProperty("win_main_frequency_qam_boxitem", "state","disable");
			GUI_SetProperty("win_main_frequency_qam_boxitem", "state","hide");
		}
		else
		{
			GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","enable");
			GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","show");
			GUI_SetProperty("win_main_frequency_qam_boxitem", "state","enable");
			GUI_SetProperty("win_main_frequency_qam_boxitem", "state","show");
		}

#else 
	if (DVB_DEMOD_DTMB == DVB_DEMOD_MODE)
	{
		GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","disable");
		GUI_SetProperty("win_main_frequency_symbol_boxitem", "state","hide");
		GUI_SetProperty("win_main_frequency_qam_boxitem", "state","disable");
		GUI_SetProperty("win_main_frequency_qam_boxitem", "state","hide");	
	}
#endif

	return 0;
}

SIGNAL_HANDLER  int win_main_frequency_set_destroy(const char* widgetname, void *usrdata)
{
	return 0;
}

SIGNAL_HANDLER int win_main_frequency_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	search_dvbc_param tmp_param;
	char *value;
	uint32_t value2;
	float edit_fre = 0;
    int sel;
	
	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
		case KEY_0:
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            GUI_GetProperty("win_main_frequency_box", "select", &value2);
            if (0 == value2)
            {
    			GUI_GetProperty("win_main_frequency_value", "string", &value);
    			edit_fre = app_float_edit_str_to_value(value);
				tmp_param.fre = 1000* edit_fre;
    			app_win_check_fre_vaild(FRE_MID,tmp_param.fre);
            }
            else if(1 == value2)
            {
    			GUI_GetProperty("win_main_frequency_symbol_value", "string", &value);
    			tmp_param.symbol_rate = atoi(value);
                app_win_check_sym_vaild(tmp_param.symbol_rate);
            }
			return EVENT_TRANSFER_STOP;
		case KEY_RECALL:
		case KEY_EXIT:
		case KEY_MENU:
			GUI_EndDialog("win_main_frequecy_set");
			return EVENT_TRANSFER_STOP;	   
		case KEY_LEFT:
		case KEY_RIGHT:
			return EVENT_TRANSFER_STOP;	
		case KEY_OK:	
			//GUI_GetProperty("win_main_frequency_box", "select", &value2);
			//if (0 == value2)
			{
				GUI_GetProperty("win_main_frequency_value", "string", &value);
				edit_fre = app_float_edit_str_to_value(value);
				tmp_param.fre = 1000* edit_fre;
				if (FALSE ==  app_win_check_fre_vaild(FRE_MID,tmp_param.fre))
				{
                    sel = 0;
                    GUI_SetProperty("win_main_frequency_box", "select", &sel);      
					return EVENT_TRANSFER_STOP;						
				}
				if (tmp_param.fre != app_flash_get_config_center_freq())
				{
					app_flash_save_config_center_freq(tmp_param.fre);
				}
			}
			//else if(1 == value2)
			{
				GUI_GetProperty("win_main_frequency_symbol_value", "string", &value);
				tmp_param.symbol_rate = atoi(value);
				if (FALSE ==  app_win_check_sym_vaild(tmp_param.symbol_rate))
				{
                    sel = 1;
                    GUI_SetProperty("win_main_frequency_box", "select", &sel);      
					return EVENT_TRANSFER_STOP; 					
				}
				if (tmp_param.symbol_rate != app_flash_get_config_center_freq_symbol_rate())
				{
					app_flash_save_config_center_freq_symbol_rate(tmp_param.symbol_rate);
				}
			}
			GUI_GetProperty("win_main_frequency_qam_value", "select", &value2);
			tmp_param.qam = value2;
			printf("qam = %d, flash qam = %d\n",tmp_param.qam,app_flash_get_config_center_freq_qam());
			if (tmp_param.qam!= app_flash_get_config_center_freq_qam())
			{
				app_flash_save_config_center_freq_qam(tmp_param.qam);
			}
	
			GUI_EndDialog("win_main_frequecy_set");			
			return EVENT_TRANSFER_STOP;				
		default:
			return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}


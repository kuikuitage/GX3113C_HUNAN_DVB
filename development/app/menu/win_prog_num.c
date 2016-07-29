#include "app.h"
#if DVB_PVR_FLAG
#include "app_pvr.h"
#endif

#define FULLSCREEN_WIN                  "win_full_screen"
#define PROGBAR_WIN                     "win_prog_bar"

#define PROGNUM_WIN                     "win_prog_num"
#define PROGNUM_TEXT                    "prog_num_text"

static event_list* timer_prognum = NULL;
static int32_t s_prog_num_digital = 0;
static int32_t s_prog_num_count = 0;
static event_list* timer_close_msg = NULL;

static int timer_prog_num_close_msg(void *userdata);

static status_t key_ok(void)
{
    /*play program*/
    // TODO: 
    //check s_prog_num_digital valid
    //play s_prog_num_digital

	uint16_t pos;
	Lcn_State_t lcn_flag = 0;
	GxBusPmViewInfo old_sysinfo;
	GxBusPmViewInfo sysinfo;
	GxBus_PmViewInfoGet(&old_sysinfo);
	lcn_flag = app_flash_get_config_lcn_flag();
	if(LCN_STATE_ON == lcn_flag)
	{
		if (FALSE == app_lcn_list_get_num_prog_pos(s_prog_num_digital,&pos))
			{
				app_play_set_msg_pop_type(MSG_POP_INVALID_PROG);
				GUI_EndDialog(PROGNUM_WIN);	
				return 0;
			}
		else
			{
				GxBus_PmViewInfoGet(&sysinfo);
				if (sysinfo.stream_type != old_sysinfo.stream_type)
					{
						if (GXBUS_PM_PROG_RADIO == sysinfo.stream_type)
							{
								app_play_stop();
							}
						else
							{
								if (GXBUS_PM_PROG_RADIO == old_sysinfo.stream_type)
									{
										app_play_stop();
									
									}							
							}
					}
				
			}
		s_prog_num_digital = pos+1;
	}

	if((s_prog_num_digital > app_prog_get_num_in_group())||(0 == s_prog_num_digital))
	{
		app_play_set_msg_pop_type(MSG_POP_INVALID_PROG);
        //create the timer to close the INVALID PROG msg.
        timer_close_msg = create_timer(timer_prog_num_close_msg, 5000, NULL,  TIMER_ONCE);

		GUI_EndDialog(PROGNUM_WIN);

	}
	else
	{
		if(s_prog_num_digital)
		{
			s_prog_num_digital--;
		}

#if DVB_PVR_FLAG
		if(1 == app_pvr_state_check(1,s_prog_num_digital))
		{
			GUI_EndDialog(PROGNUM_WIN);  
			return 0;
		}
#endif	
		
		app_prog_save_playing_pos_in_group(s_prog_num_digital);	
	    GUI_EndDialog(PROGNUM_WIN);  
		app_play_switch_prog_clear_msg();	/*创建信息条、密码框之前，清除上一节目提示，避免闪烁*/	
	    GUI_CreateDialog(PROGBAR_WIN);
		app_play_reset_play_timer(0);

	}



    return 0;
}

static status_t key_digital(unsigned short value)
{
    char buf[10] = {0};
    
    s_prog_num_digital  *= 10;
    s_prog_num_digital += value - KEY_0;
	s_prog_num_count++;
    switch(s_prog_num_count)
    	{
		case 1:
			snprintf((void*)buf, 9, "%d",s_prog_num_digital);
			break;
		case 2:
			snprintf((void*)buf, 9, "%02d",s_prog_num_digital);
			break;
		case 3:
			snprintf((void*)buf, 9, "%03d",s_prog_num_digital);
			break;
		default:
			snprintf((void*)buf, 9, "%d",s_prog_num_digital);
		 	break;
    	}

	GUI_SetProperty(PROGNUM_TEXT, "string", buf);

    if(3 <= s_prog_num_count)
    {
	GUI_SetProperty(PROGNUM_TEXT, "draw_now", NULL);
	GxCore_ThreadDelay(200);		 
        key_ok();
    }

    return 0;
}

static int timer_prog_num(void *userdata)
{
    timer_prognum = NULL;	

    key_ok();
    
	return 0;
}

static int timer_prog_num_close_msg(void *userdata)
{
    app_hide_prompt();

    timer_close_msg = NULL;

	return 0;
}

SIGNAL_HANDLER  int prog_num_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int prog_num_create(const char* widgetname, void *usrdata)
{
	uint32_t duration;
	int32_t config = 0;
	config = app_flash_get_config_bar_time();
	duration = config * 1000;
	timer_prognum = create_timer(timer_prog_num, duration, NULL,  TIMER_ONCE);
	
	s_prog_num_digital = 0;
       s_prog_num_count = 0;
	app_set_win_create_flag(PROG_NUM_WIN);

	return 0;
}

SIGNAL_HANDLER int prog_num_destroy(const char* widgetname, void *usrdata)
{

    remove_timer(timer_prognum);
    timer_prognum = NULL;
   app_set_win_destroy_flag(PROG_NUM_WIN);


     return 0;
}


SIGNAL_HANDLER int prog_num_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
    	case KEY_EXIT:
            GUI_EndDialog(PROGNUM_WIN);
    		return EVENT_TRANSFER_STOP;
                        
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
			key_digital(event->key.sym);
					if(timer_prognum)
					{	
						reset_timer(timer_prognum);
					}			
			break;		
            
		case KEY_OK:
			key_ok();
			break;				

        			
		default:
            GUI_EndDialog(PROGNUM_WIN);
            GUI_SendEvent(FULLSCREEN_WIN, event);
            return EVENT_TRANSFER_STOP;
			break;
	}
	
	return EVENT_TRANSFER_STOP;
}



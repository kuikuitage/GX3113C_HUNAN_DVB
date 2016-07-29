#ifdef ECOS_OS
#include "gxcore.h"
#include "gui_key.h"
#include "stb_panel.h"
#include "gxapp_sys_config.h"


static int32_t s_panel_timer = -1;
static uint8_t s_panel_conuter = 0;
extern GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock;
uint32_t s_panel_key = 0;
static struct CYG_SELINFO_TAG   sg_PanelSignal;

uint8_t gx_stb_panel_init(void)
{
//	bsp_printf("gx_stb_panel_init\n");
	return 0;

}
void bsp_front_panel_scan_manager()
{
	if (gs_PanelControlBlock.m_PanelScanMangerFun)
		(gs_PanelControlBlock.m_PanelScanMangerFun)();
	return ;
}

uint8_t bsp_front_panel_lock(void)
{
	if (NULL != gs_PanelControlBlock.m_PanelLockFun)
		{
			gs_PanelControlBlock.m_PanelLockFun();	
		}
	return 0;
}
uint8_t bsp_front_panel_unlock(void)
{
	if (NULL != gs_PanelControlBlock.m_PanelUnlockFun)
		{
			gs_PanelControlBlock.m_PanelUnlockFun();
		}
	return 0;
}

uint8_t bsp_front_panel_poweroff(void)
{
	if(gs_PanelControlBlock.m_PanelPowerOffFun)
	(gs_PanelControlBlock.m_PanelPowerOffFun)();
	return 0;
}
cyg_bool bsp_init_front_panel(struct cyg_devtab_entry *tab)
{
	gx_stb_panel_init();	
	if(gs_PanelControlBlock.m_PanelConfigFun != NULL)
		{
			gs_PanelControlBlock.m_PanelConfigFun();
		}
	if (gs_PanelControlBlock.m_PanelInitPinFun != NULL)
		{
			gs_PanelControlBlock.m_PanelInitPinFun();
		}
	if (gs_PanelControlBlock.m_PanelIsrFun !=NULL)
		{
			gs_PanelControlBlock.m_PanelIsrFun();
		}

	return 0;
}

uint8_t bsp_front_panel_set_signal(uint32_t wwww)
{
	if (NULL != gs_PanelControlBlock.m_PanelSetSignalValueFun)
		{
			gs_PanelControlBlock.m_PanelSetSignalValueFun(wwww);
		}
	return 0;
}
uint8_t bsp_front_panel_set_led_value(uint32_t wwww)
{
	if (NULL != gs_PanelControlBlock.m_PanelSetValueFun)
		(gs_PanelControlBlock.m_PanelSetValueFun)(wwww);

	return 0;
}

uint8_t bsp_front_panel_show_string(unsigned char *pString)
{
	if (NULL != gs_PanelControlBlock.m_PanelSetStringFun )
		{
			gs_PanelControlBlock.m_PanelSetStringFun(pString);	
		}

	return 0;
}
uint8_t bsp_front_panel_set_mode(GXPanel_ShowMode_t PanelMode)
{
	if (NULL != gs_PanelControlBlock.m_PanelShowModeFun)
		{
			gs_PanelControlBlock.m_PanelShowModeFun(PanelMode);
		}
	return 0;
}


uint8_t bsp_front_panel_stand_by(void)
{
	(gs_PanelControlBlock.m_PanelStandByFun)();
	return 0;
}
uint8_t bsp_front_panel_wake_up(void)
{
	(gs_PanelControlBlock.m_PanelWakeUpFun)();
	return 0;
}

static Cyg_ErrNo gx3113_panel_close(cyg_io_handle_t handle)
{
    bsp_printf("[panel]gx3113_panel_close\n");
    s_panel_conuter --;
    if(!s_panel_conuter)
    {
	    gx_rtc_timer_delete(s_panel_timer);
    }
	return ENOERR;
}

static Cyg_ErrNo gx3113_panel_ioctl(cyg_io_handle_t handle, cyg_uint32 key, void *buf)
{
	int gpio = 0;

	if ((PANEL_GPIO_HIGH == key)||(PANEL_GPIO_LOW == key)
		||(PANEL_STRING == key)||(PANEL_DATA == key)
		||(PANEL_READ_KEY == key))
		{
			if (buf == NULL) 
				return EIO;
			gpio = *(uint32_t*)buf;			
		}

	switch (key)
	{
		case PANEL_GPIO_HIGH:
			bsp_printf("PANEL_GPIO_HIGH %d\n",gpio);
			gx_gpio_setio(gpio, 1);
			gx_gpio_setlevel(gpio,1);			
			break;
		case PANEL_GPIO_LOW:
			bsp_printf("PANEL_GPIO_LOW %d\n",gpio);
			gx_gpio_setio(gpio, 1);
			gx_gpio_setlevel(gpio,0);			
			break;
		case PANEL_STRING:
#if 0
			bsp_front_panel_show_string(*(GXLED_Str_t*)(buf));
#else
			bsp_front_panel_show_string(buf);

#endif
			break;

		case PANEL_DATA:
//			bsp_printf(" gx3113_panel_ioctl PANEL_DATA \n");
#if 0
			bsp_front_panel_set_signal(*(uint32_t*)buf);
#else
			bsp_front_panel_set_led_value(*(uint32_t*)buf);

#endif
			break;

		case PANEL_READ_KEY:
//			bsp_printf(" gx3113_panel_ioctl PANEL_READ_KEY \n");
			*(uint32_t*)buf = s_panel_key;
			break;

		case PANEL_LOCK:
			bsp_front_panel_lock();
			//gx_stb_panel_ct1642_on();
			break;

		case PANEL_UNLOCK:
			bsp_front_panel_unlock();
			break;

		case PANEL_POWER_OFF:
			bsp_printf("PANEL_POWER_OFF\n");
			//gx_stb_panel_ct1642_set_led_dark();
            		bsp_front_panel_poweroff();
			//gx_stb_panel_ct1642_standby();
			//gx_stb_panel_ct1642_scan_manager();
			break;
		default:
			return EIO;
	}

	return ENOERR;
}

static Cyg_ErrNo gx3113_panel_open(struct cyg_devtab_entry **tab,
                       struct cyg_devtab_entry *sub_tab,
                       const char *name)
{
	//unsigned char initLedDisp = 0;
	
 	bsp_printf("[panel]gx3113_panel_open\n");
    if(!s_panel_conuter)
    {
	    s_panel_timer = gx_rtc_timer_create((gx_rtc_timer_callback)bsp_front_panel_scan_manager,0,4,4);
//		s_panel_timer = gx_rtc_timer_create((gx_rtc_timer_callback)bsp_front_panel_scan_manager,0,0,5);
    }
	
	if (s_panel_timer < 0)
	{

		bsp_printf("[panel]gx_rtc_timer_create failed!\n");
	}
	else
	{
		/*wangjian modify on 20141215 for boot disp led.*/
		#if 0
		if (0 == s_panel_conuter)
		{
			initLedDisp = GXLED_BOOT;
			bsp_front_panel_show_string(&initLedDisp);
		}
		#endif
	
       	s_panel_conuter++;
		bsp_printf("gx_rtc_timer_create s_panel_timer: %d\n", s_panel_timer);
	}
	return ENOERR;
}

static Cyg_ErrNo gx3113_panel_set_config(cyg_io_handle_t handle, cyg_uint32 key,
                                const void* buf, cyg_uint32* len)
{
	return ENOERR;
}

Cyg_ErrNo gx3113_panel_read(cyg_io_handle_t handle,
                         void *buffer, cyg_uint32 *len)
{
#define KEY_VALUE(value)    ((value&0xff00)<<16 | (value&0xff)<<8)
#define INTERVAL   (3)

	static uint32_t interval = 0;

	// slow down the key press
	interval++;
	if (interval != INTERVAL)
	{
		*(uint32_t*)buffer = KEY_VALUE(0x0);
		return ENOERR;
	}
	else
	{
		interval = 0;
	}
//	bsp_printf("gx3113_panel_read s_panel_key %x\n",s_panel_key);
	switch (s_panel_key)
	{
		case PANEL_KEY_EXIT:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_EXIT;
			break;
		case PANEL_KEY_UP:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_UP;
			break;
		case PANEL_KEY_DOWN:
//			s_panel_key = 0;
			*(uint32_t*)buffer =PANEL_KEY_DOWN;
			break;
		case PANEL_KEY_RIGHT:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_RIGHT;
			break;
		case PANEL_KEY_MENU:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_MENU;
			break;
		case PANEL_KEY_OK:
			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_OK;
			break;
		case PANEL_KEY_LEFT:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_LEFT;
			break;
		case PANEL_KEY_POWER:
//			s_panel_key = 0;
			*(uint32_t*)buffer = PANEL_KEY_POWER;
			break;
		default:
			*(uint32_t*)buffer = KEY_VALUE(0x0);
			break;
	}

	s_panel_key = 0;
	
	return ENOERR;
}

void poll_wakeup(void)
{
	cyg_selwakeup(&sg_PanelSignal);
}
static cyg_bool _panel_select(cyg_io_handle_t handle, cyg_uint32 which, CYG_ADDRWORD info)
{
   cyg_bool retval = false;
        
   switch(which)
   {
     case CYG_FREAD:
        if(s_panel_key == 0)
            cyg_selrecord( info, &sg_PanelSignal);
        else retval = true;
        break;
     case CYG_FWRITE:    //write - not support
        break;
     case 0:             //exceptions - not support
        break;
     default:
        break;
   }
     return retval;
}


EMPTY_DEVTAB_ENTRY(gx3113_panel);

static cyg_devio_table_t gx3113_panel_handlers =
{
	.read = gx3113_panel_read,
    .ioctl = gx3113_panel_ioctl,
	.close = gx3113_panel_close,
	.set_config = gx3113_panel_set_config,
	.select = _panel_select,
};

cyg_devtab_entry_t gx3113_panel_dev =
{
	.name = PANEL_NAME,
	.handlers = &gx3113_panel_handlers,
	.init = bsp_init_front_panel,
	.lookup = gx3113_panel_open,
};

void gx3113_panel_mod_init(void)
{
	char_dev_register(&gx3113_panel_dev, PANEL_NAME, NULL);
	cyg_selinit(&sg_PanelSignal);

}
#endif



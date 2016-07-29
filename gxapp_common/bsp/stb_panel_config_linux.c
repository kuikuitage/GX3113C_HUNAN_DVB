
#ifdef LINUX_OS
//#include "stb_panel_gpio.h"
#include <linux/gx_gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
//#include "stdio.h"




#include "stb_panel.h"
//#include <cyg/io/gx3110_gpio.h>
//#define OPEN_MMU    1


//static int out_status = 0;

GXPANEL_PUBLIC_Config_t g_PanelPublicConfig;

static void gx_stb_panel_cfg_multiplex(uint64_t nCfgPin)
{
	return;
}

static void gx_stb_panel_gpio_init(uint64_t nCfgPin)
{
	gx_stb_panel_cfg_multiplex(nCfgPin);
}

static void gx_stb_panel_deal_key(uint32_t nKeyIndex)
{
	printk("[panel] gx_stb_panel_deal_key s_panel_key :%d= %d \n", nKeyIndex,s_panel_key);
	return;	
}

void gx_stb_panel_set_gpio_output(int num)
{
	gx_gpio_setio(num,1);  
}

void gx_stb_panel_set_gpio_input(int num)
{
	gx_gpio_setio(num,0);  
}
void gx_stb_panel_set_gpio_level(int num,int level)
{
	gx_gpio_setlevel(num,level);

/*    if(level== GXPIO_BIT_HIGHT_LEVEL)
    {
	gx_gpio_setlevel(num,1);
    }
    else
    {
	gx_gpio_setlevel(num,0);
    }*/
}

void gx_stb_panel_get_gpio_level(int num,uint64_t *level)
{
    int lev;
    lev = gx_gpio_getlevel(num);
/*    if(lev == 1)
    {
	*level = GXPIO_BIT_HIGHT_LEVEL;
    } 
    else
    {
	*level = GXPIO_BIT_LOW_LEVEL;
    }*/

	*level = lev;
}

static void gx_stb_panel_gpio_open(uint8_t chGpio,GpioOutput_t GpioOutput)
{
//	return;
	if(GPIO_INPUT == GpioOutput)
	{
		gx_stb_panel_set_gpio_input(chGpio);
	}
	else
	{
		gx_stb_panel_set_gpio_output(chGpio);
		gx_stb_panel_set_gpio_level(chGpio, 1);
	}
}

void gx_stb_panel_public_config(void)
{
	g_PanelPublicConfig.m_PanelCfgMultiplexFun = (void *)gx_stb_panel_cfg_multiplex;
	g_PanelPublicConfig.m_PanelSetGpioOutPutFun = (void *)gx_stb_panel_set_gpio_output;
	g_PanelPublicConfig.m_PanelSetGpioInPutFun = (void *)gx_stb_panel_set_gpio_input;
	g_PanelPublicConfig.m_PanelSetGpioLevelFun = (void *)gx_stb_panel_set_gpio_level;
	g_PanelPublicConfig.m_PanelGetGpioLevelFun = (void *)gx_stb_panel_get_gpio_level;
	g_PanelPublicConfig.m_PanelInitGpioFun = (void *)gx_stb_panel_gpio_init;
	g_PanelPublicConfig.m_PanelOpenGpioFun = (void *)gx_stb_panel_gpio_open;
	g_PanelPublicConfig.m_PanelDealKey = (void *)gx_stb_panel_deal_key;
}

#endif


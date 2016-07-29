#ifdef ECOS_OS
#include "stb_panel.h"
//#include <cyg/io/gx3110_gpio.h>
#define OPEN_MMU    1

GXPANEL_PUBLIC_Config_t g_PanelPublicConfig;

static void gx_stb_panel_cfg_multiplex(uint64_t nCfgPin)
{
	return;
}

static void gx_stb_panel_set_gpio_level(uint8_t eport, uint8_t nLevel)
{
	int num = eport;
	int level = nLevel;
	gx_gpio_setlevel(num,level);
	return ;
}

static void gx_stb_panel_get_gpio_level(uint8_t eport, uint64_t *nLevel)
{
	int num = eport;

    int lev;
    lev = gx_gpio_getlevel(num);
	*nLevel = lev;
	return;
}

static void gx_stb_panel_set_gpio_output(uint8_t eport)
{
	int num = eport;
	int ret = -1;
	ret = gx_gpio_setio(num,1);  

	return;
}

static void gx_stb_panel_set_gpio_input(uint8_t eport)
{
	int num = eport;
	gx_gpio_setio(num,0); 

	return;
}

static void gx_stb_panel_gpio_init(uint64_t nCfgPin)
{
	gx_stb_panel_cfg_multiplex(nCfgPin);
}

static void gx_stb_panel_gpio_open(uint8_t chGpio,GpioOutput_t GpioOutput)
{
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

static void gx_stb_panel_deal_key(uint32_t nKeyIndex)
{
	return;	
	//bsp_printf("[panel] gx_stb_panel_deal_key s_panel_key :%d= %d \n", nKeyIndex,s_panel_key);
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


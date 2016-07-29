/*
 * =====================================================================================
 *
 *       Filename:  bsp_panel.c
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  2010年10月07日 22时20分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
/* ---------------------------------------------- */
#include "stb_panel.h"
#if (PANEL_TYPE == PANEL_TYPE_4KEY)
extern	void poll_wakeup(void);
#define GXPIO_BIT_HIGHT_LEVEL 1
#define GXPIO_BIT_LOW_LEVEL    0
extern uint32_t s_panel_key;

typedef struct GXPANEL_JUYING_Config_s
{
	uint8_t m_chPanelName[MAX_PANEL_NAME_LENGTH];

	uint8_t m_chKeyPin[STB_KEY_NUMBER];	

	uint32_t m_nKeyValue[4];

	GXPANEL_PUBLIC_Config_t m_tPanelPublicConfig;
}GXPANEL_JUYING_Config_t;

GXPANEL_JUYING_Config_t g_PanelJuYingConfig;



/* key */
#define GPIO_KEY_MENU   19
#define GPIO_KEY_DOWN   22
#define GPIO_KEY_UP     21
#define GPIO_KEY_OK      20

void gx_stb_panel_juying_send_keymsg(uint32_t nKeyIndex)
{
   // bsp_printf("+++++++++ key index %d\n", nKeyIndex);
	if((nKeyIndex!=0xff)&&(nKeyIndex<8))
		{
			s_panel_key = g_PanelJuYingConfig.m_nKeyValue[nKeyIndex];
			poll_wakeup();

	}
	
}


void gx_stb_panel_juying_key_scan(void)
{	
	uint64_t nKeyLevel[STB_KEY_NUMBER];
	static uint8_t KeyCount = 0;
	static uint8_t s_chPress = 0;
	uint32_t i;
	uint32_t utimes=0;
	
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJuYingConfig.m_chKeyPin[0], &nKeyLevel[0]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJuYingConfig.m_chKeyPin[1], &nKeyLevel[1]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJuYingConfig.m_chKeyPin[2], &nKeyLevel[2]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJuYingConfig.m_chKeyPin[3], &nKeyLevel[3]);

	if(
		((!nKeyLevel[0])||(!nKeyLevel[1])||(!nKeyLevel[2])|| (!nKeyLevel[3])) 
		&& (1 == s_chPress)
	   )	
	{	
		KeyCount++;
		if(KeyCount == 16)
		{	
			KeyCount = 144;
		}
		else if(KeyCount == 175)
		{	
			KeyCount = 144;
			s_chPress = 0;	
		}
		bsp_printf("KeyCount =%d\n",KeyCount);
		return;
	}
	else if(
		((!nKeyLevel[0])||(!nKeyLevel[1])||(!nKeyLevel[2])|| (!nKeyLevel[3])) 
		&& (0 == s_chPress)
		)	
	{
		bsp_printf("key pressed\n");
	}
	else if (
		((nKeyLevel[0])||(nKeyLevel[1])||(nKeyLevel[2])|| (nKeyLevel[3])) 
		&& (1 == s_chPress)
		)	
	{	
		KeyCount = 0;
		s_chPress = 0;	
		return;
	}
	else if(
		((nKeyLevel[0])||(nKeyLevel[1])||(nKeyLevel[2])|| (nKeyLevel[3])) 
		&& (0 == s_chPress)
		)	
	{	
	#if 1
		while(utimes<10)
			utimes++;
		if(utimes == 10)
			utimes =0;
	#endif
        return;
	}
	for(i = 0; i < STB_KEY_NUMBER; i++)
	{
		g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJuYingConfig.m_chKeyPin[i], &nKeyLevel[i]);
		if ((!(nKeyLevel[i])) && (s_chPress==0))//如果有按键
		{
			bsp_printf("\n  i:%d \n", i);
			gx_stb_panel_juying_send_keymsg(i);
			s_chPress = 1;
		}
	}
}





void gx_stb_panel_juying_scan_manager(void)
{
	static uint8_t m_chLedCnt;
	if(m_chLedCnt == 4 )
	{
		//printf("****_____****\n");
		gx_stb_panel_juying_key_scan();
		m_chLedCnt =0;
	}
	else
	{
		m_chLedCnt++;
	} 
	return;


}

uint32_t gx_stb_panel_juying_config(void)
{


	uint32_t nKeyValue[4]={PANEL_KEY_MENU
						, PANEL_KEY_DOWN
						, PANEL_KEY_UP
						, PANEL_KEY_OK
				};
	memset(g_PanelJuYingConfig.m_chPanelName, 0, sizeof(g_PanelJuYingConfig.m_chPanelName));
	sprintf((void*)g_PanelJuYingConfig.m_chPanelName, "%s", "JUYING");
	memcpy(g_PanelJuYingConfig.m_nKeyValue,nKeyValue,sizeof(nKeyValue));
	
	g_PanelJuYingConfig.m_chKeyPin[0] = GPIO_KEY_MENU;
	g_PanelJuYingConfig.m_chKeyPin[1] = GPIO_KEY_DOWN;
	g_PanelJuYingConfig.m_chKeyPin[2] = GPIO_KEY_UP;
	g_PanelJuYingConfig.m_chKeyPin[3] = GPIO_KEY_OK;
	
	gx_stb_panel_public_config();
	memcpy(&g_PanelJuYingConfig.m_tPanelPublicConfig
		, &g_PanelPublicConfig
		, sizeof(GXPANEL_PUBLIC_Config_t));

	
	return 0;

}

void gx_stb_panel_juying_isr(void)
{
	return ;

}
void gx_stb_panel_juying_init_pin(void)
{
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJuYingConfig.m_chKeyPin[0]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJuYingConfig.m_chKeyPin[1]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJuYingConfig.m_chKeyPin[2]);
	g_PanelJuYingConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJuYingConfig.m_chKeyPin[3]);

	return;
}

void gx_stb_panel_juying_lock(void)
{
	return;
}
void gx_stb_panel_juying_set_led_signal_value(uint32_t nLedValue)
{

	return;
}
void gx_stb_panel_juying_set_led_string(unsigned char* str)
{

	return;

}
void gx_stb_panel_juying_set_led_value(uint32_t nLedValue)
{

	return;
}
void gx_stb_panel_juying_unlock(void)
{
	return;
}
void gx_stb_panel_juying_show_mode(GXPanel_ShowMode_t PanelMode)
{

	return;
}

void gx_stb_panel_juying_power_off(void)
{
	return;
}
GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = gx_stb_panel_juying_config,
	.m_PanelIsrFun = gx_stb_panel_juying_isr,
	.m_PanelInitPinFun = gx_stb_panel_juying_init_pin,
	.m_PanelLockFun = gx_stb_panel_juying_lock,
	.m_PanelSetSignalValueFun = gx_stb_panel_juying_set_led_signal_value,
	.m_PanelSetStringFun = gx_stb_panel_juying_set_led_string,
	.m_PanelSetValueFun = gx_stb_panel_juying_set_led_value,
	.m_PanelUnlockFun = gx_stb_panel_juying_unlock,
	.m_PanelShowModeFun = gx_stb_panel_juying_show_mode,
	.m_PanelScanMangerFun = gx_stb_panel_juying_scan_manager,
	.m_PanelPowerOffFun = gx_stb_panel_juying_power_off,
};


#endif


/*****************************************************************************
* 						   CONFIDENTIAL								
*        Hangzhou GuoXin Science and Technology Co., Ltd.             
*                      (C)2008, All right reserved
******************************************************************************

******************************************************************************
* File Name :     bsp_panel.c
* Author    : 	hongg
* Project   :
* Type      :	
******************************************************************************
* Purpose   :	设备初始化入
******************************************************************************
* Release History:
  VERSION	Date			  AUTHOR         Description
   0.0  	2008.8.28	      hongg	         creation
*****************************************************************************/

/* Includes ----------------------------------------------------------------- */
#include "stb_panel.h"
//#ifdef PANEL_TYPE_JIZHONG
#if (PANEL_TYPE == PANEL_TYPE_JIZHONG)
extern	void poll_wakeup(void);

#define GXPIO_BIT_HIGHT_LEVEL 1
#define GXPIO_BIT_LOW_LEVEL    0
extern uint32_t s_panel_key;

typedef struct GXPANEL_JIZHONG_Config_s
{
	uint8_t m_chPanelName[MAX_PANEL_NAME_LENGTH];

	uint8_t m_chKeyPin[3];	//the kd pin

	uint32_t m_nKeyValue[8];

	uint8_t m_chLockPin;	//the lock pin

	GXPANEL_PUBLIC_Config_t m_tPanelPublicConfig;
}GXPANEL_JIZHONG_Config_t;

GXPANEL_JIZHONG_Config_t g_PanelJizhongConfig;



void gx_stb_panel_jizhong_send_keymsg(uint32_t nKeyIndex)
{
//	bsp_printf("+++++++++ key index %d\n", nKeyIndex);
	if((nKeyIndex!=0xff)&&(nKeyIndex<8))
		{
			s_panel_key = g_PanelJizhongConfig.m_nKeyValue[nKeyIndex];
			poll_wakeup();

	}
	
//	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelDealKey(nKeyIndex);
}
void gx_stb_panel_jizhong_key_scan(void)
{	
	#if 1
	uint64_t nKeyLevel[3];
	static uint8_t KeyCount = 0;
	static uint8_t s_chPress = 0;
	uint32_t i;
	#if 1
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJizhongConfig.m_chKeyPin[0], &nKeyLevel[0]);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJizhongConfig.m_chKeyPin[1], &nKeyLevel[1]);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJizhongConfig.m_chKeyPin[2], &nKeyLevel[2]);
	if((!(nKeyLevel[0]&&nKeyLevel[1]&&nKeyLevel[2])) && (1 == s_chPress))	
	{	
		KeyCount++;
		if(KeyCount == 0x10)
		{	KeyCount = 0x90;
		}
		else if(KeyCount == 0xaF)
		{	KeyCount = 0x90;
			s_chPress = 0;	
		}
		//bsp_printf("KeyCount =%d\n",KeyCount);
		return;
	}
	else if(((nKeyLevel[0]&&nKeyLevel[1]&&nKeyLevel[2]))&& (0 == s_chPress))	
	{
		//;//bsp_printf("key pressed\n");
	}
	else if (((nKeyLevel[0]&&nKeyLevel[1]&&nKeyLevel[2])) && (1 == s_chPress))	
	{	
		KeyCount = 0;
		s_chPress = 0;	
		return;
	}
	else if(((nKeyLevel[0]&&nKeyLevel[1]&&nKeyLevel[2]))&& (1 == s_chPress))	
	{	
        return;
	}
	#endif
	for(i = 0; i < 3; i++)
	{
		g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelJizhongConfig.m_chKeyPin[i], &nKeyLevel[i]);
		if ((!(nKeyLevel[i])) && (s_chPress==0))//如果有按键
		{
			//bsp_printf("\n##############[ct1642=0=] i:%d chKeyCnt:%d\n", i, chKeyCnt);
			gx_stb_panel_jizhong_send_keymsg(i);
			s_chPress = 1;
		}
	}
	#endif
}
void gx_stb_panel_jizhong_scan_manager(void)
{

	static uint8_t m_chLedCnt;
	if(m_chLedCnt == 3 )
	{
		//bsp_printf("****_____****\n");
		gx_stb_panel_jizhong_key_scan();
		m_chLedCnt =0;
	}
	else
	{
		m_chLedCnt++;
	} 
	return;
	
}
void gx_stb_panel_jizhong_isr(void)
{
	//bsp_printf("gx_stb_panel_jizhong_isr\n");
	//return;
}
void gx_stb_panel_jizhong_init_pin(void)
{
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJizhongConfig.m_chKeyPin[0]);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJizhongConfig.m_chKeyPin[1]);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelJizhongConfig.m_chKeyPin[2]);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelJizhongConfig.m_chLockPin);
	g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelJizhongConfig.m_chLockPin
										, GXPIO_BIT_HIGHT_LEVEL);
	bsp_printf("gx_stb_panel_jizhong_init_pin\n");
}
void gx_stb_panel_jizhong_lock(void)
{
		g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelJizhongConfig.m_chLockPin
										, GXPIO_BIT_LOW_LEVEL);
		bsp_printf("gx_stb_panel_jizhong_lock\n");
}
void gx_stb_panel_jizhong_set_led_signal_value(uint32_t nLedValue)
{
	bsp_printf("gx_stb_panel_jizhong_set_led_signal_value\n");
	//return;
}
void gx_stb_panel_jizhong_set_led_string(unsigned char* str)
{
	bsp_printf("gx_stb_panel_jizhong_set_led_string\n");
	//return;
}
void gx_stb_panel_jizhong_set_led_value(uint32_t nLedValue)
{
	bsp_printf("gx_stb_panel_jizhong_set_led_value\n");
	//return;
}
void gx_stb_panel_jizhong_unlock(void)
{
		g_PanelJizhongConfig.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelJizhongConfig.m_chLockPin
										, GXPIO_BIT_HIGHT_LEVEL);
		bsp_printf("gx_stb_panel_jizhong_unlock\n");
}
void gx_stb_panel_jizhong_show_mode(GXPanel_ShowMode_t PanelMode)
{
	//return;
}

void gx_stb_panel_jizhong_power_off(void)
{
	gx_stb_panel_jizhong_unlock	();
}

uint32_t gx_stb_panel_jizhong_config(void)
{
	uint32_t nKeyValue[8]={PANEL_KEY_UP
				, PANEL_KEY_DOWN
				, PANEL_KEY_POWER
				, PANEL_KEY_LEFT
				, PANEL_KEY_MENU
				, PANEL_KEY_OK
				, PANEL_KEY_DOWN
				, PANEL_KEY_UP
				};
	memset(g_PanelJizhongConfig.m_chPanelName, 0, sizeof(g_PanelJizhongConfig.m_chPanelName));
	sprintf((void*)g_PanelJizhongConfig.m_chPanelName, "%s", "JIZHONG");
	memcpy(g_PanelJizhongConfig.m_nKeyValue,nKeyValue,sizeof(nKeyValue));
	g_PanelJizhongConfig.m_chLockPin = PANEL_LOCK_GPIO//2;
	g_PanelJizhongConfig.m_chKeyPin[0] = 0;
	g_PanelJizhongConfig.m_chKeyPin[1] = 1;
	g_PanelJizhongConfig.m_chKeyPin[2] = 3;
	gx_stb_panel_public_config();
	memcpy(&g_PanelJizhongConfig.m_tPanelPublicConfig
		, &g_PanelPublicConfig
		, sizeof(GXPANEL_PUBLIC_Config_t));
	bsp_printf("gx_stb_panel_jizhong_config\n");
	return 0;
}

GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = gx_stb_panel_jizhong_config,
	.m_PanelIsrFun = gx_stb_panel_jizhong_isr,
	.m_PanelInitPinFun = gx_stb_panel_jizhong_init_pin,
	.m_PanelLockFun = gx_stb_panel_jizhong_lock,
	.m_PanelSetSignalValueFun = gx_stb_panel_jizhong_set_led_signal_value,
	.m_PanelSetStringFun = gx_stb_panel_jizhong_set_led_string,
	.m_PanelSetValueFun = gx_stb_panel_jizhong_set_led_value,
	.m_PanelUnlockFun = gx_stb_panel_jizhong_unlock,
	.m_PanelShowModeFun = gx_stb_panel_jizhong_show_mode,
	.m_PanelScanMangerFun = gx_stb_panel_jizhong_scan_manager,
	.m_PanelPowerOffFun = gx_stb_panel_jizhong_power_off,
};
#endif

#include "stb_panel.h"
//#include "stb_panel_gpio.h"
//#ifdef PANEL_TYPE_TM1635
#if (PANEL_TYPE == PANEL_TYPE_TM1635)
extern	void poll_wakeup(void);


#define GXPIO_BIT_HIGHT_LEVEL  	1
#define GXPIO_BIT_LOW_LEVEL    	0
#define LED_BIT_COUNT			4

//static uint8_t 			led_position = 0;
//tm1635
extern uint32_t s_panel_key;
typedef struct GXPANEL_TM1635_Config_s
{
	uint8_t m_chPanelName[MAX_PANEL_NAME_LENGTH];

	uint8_t m_chLedBit[8];

	uint8_t m_chLedCnt;	//the number of panel led
	uint8_t m_chCurLenCnt;//cur scan led
	uint8_t m_chShowMode;//GXPanel_ShowMode_t

	uint8_t m_chClkPin;	//the clk pin of hc164
	uint8_t m_chDataPin;	//the data pin of hc164

	uint8_t m_chKeyPin[2];	//the kd pin
	uint32_t m_nKeyValue[8];

	uint8_t m_chLockPinSelect;//lock led 片选
	uint8_t m_chLockPin;	//负极
	uint8_t m_chLedPin[4];//the led pin
	uint32_t m_nKeyAddr[8];//the led pin

	uint32_t m_nPortBaseAddr[PORT_BASE_ADDRESS_END];//the base address of port

	GXPANEL_PUBLIC_Config_t m_tPanelPublicConfig;
}GXPANEL_TM1635_Config_t;

GXPANEL_TM1635_Config_t g_PanelTM1635Config;
static uint8_t 			gs_chLedValue_1635[5]= {0,};
static uint8_t 			gs_chStringValue[256]= {0,};
static uint8_t 		    gs_chStringLen = 0;
static uint8_t 		    gs_chStringPos = 0;
static uint8_t 		    gs_chScroolSpeed = 8;
//static uint8_t 			s_Lockenable;

extern void gx_stb_panel_tm1635_send_keymsg(uint32_t nKeyIndex);
extern void gx_stb_panel_tm1635_lock(void);
extern void gx_stb_panel_tm1635_set_led_signal_value(uint32_t nLedValue);
extern void gx_stb_panel_tm1635_set_led_string(GXLED_Str_t LedStr);
extern void gx_stb_panel_tm1635_show_led_string(unsigned char *pStr);
extern void gx_stb_panel_tm1635_set_led_value(uint32_t nLedValue);
extern void gx_stb_panel_tm1635_unlock(void);
#define BIT_O_1635 (0<<0)//            0
#define BIT_A_1635 (1<<0)//            a
#define BIT_B_1635 (1<<1)//         -------
#define BIT_C_1635 (1<<2)//        |       |
#define BIT_D_1635 (1<<3)//    //f |       | b
#define BIT_E_1635 (1<<4)//         ---g---		
#define BIT_F_1635 (1<<5)//        |       | c
#define BIT_G_1635 (1<<6)//    //e |       |	
#define BIT_P_1635 (1<<7)//         ---d---   p

static uint8_t gs_chLedData_1635[LED_DATA_DARK+1]={
		(BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635),		//0 
		(BIT_B_1635|BIT_C_1635),								//1
		(BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635),			//2
		(BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635),			//3
		(BIT_B_1635|BIT_C_1635|BIT_F_1635|BIT_G_1635),					//4
		(BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635),			//5
		(BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635),		//6
		(BIT_A_1635|BIT_B_1635|BIT_C_1635),						//7
		(BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635),//8
		(BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635),		//9
		(BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635),			//E
		(BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635),					//F
		(BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635),			//N
		(BIT_A_1635|BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635),			//P
		0x00	,
		BIT_G,								
		//---
		0x00,//dark
		
};/*0,1,2,3,4,
5,6,7,8,9,E,F,N,P,T,dark*/

/*
static uint8_t gs_chAscii_1635[][2]={
{'0', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'1', (BIT_B_1635|BIT_C_1635)},
{'2', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'3', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'4', (BIT_B_1635|BIT_C_1635|BIT_F_1635|BIT_G_1635)},
{'5', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'6', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'7', (BIT_A_1635|BIT_B_1635|BIT_C_1635)},
{'8', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'9', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'a', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'A', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'b', (BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'B', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'c', (BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'C', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_D_1635)},
{'d', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'D', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'e', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'E', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'f', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'F', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'g', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'G', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_F_1635)},
{'h', (BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'H', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'i', (BIT_C_1635)},
{'I', (BIT_B_1635|BIT_C_1635)},
{'j', (BIT_C_1635|BIT_D_1635)},
{'J', (BIT_B_1635|BIT_C_1635|BIT_D_1635)},
{'j', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'k', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'K', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'l', (BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'L', (BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'m', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'M', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'n', (BIT_C_1635|BIT_E_1635|BIT_G_1635)},
{'N', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635)},
{'o', (BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'O', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'p', (BIT_A_1635|BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'P', (BIT_A_1635|BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'q', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_F_1635|BIT_G_1635)},
{'Q', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_P_1635)},
{'r', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'R', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'s', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'S', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'t', (BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'T', (BIT_A_1635|BIT_E_1635|BIT_F_1635)},
{'u', (BIT_C_1635|BIT_D_1635|BIT_E_1635)},
{'U', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'v', (BIT_C_1635|BIT_D_1635|BIT_E_1635)},
{'V', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'w', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'W', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'x', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'X', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'y', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'Y', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'z', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'Z', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{' ', (BIT_O_1635)},
{']', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635)},
{'[', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'-', (BIT_G)},
{'_', (BIT_A_1635)},
{'@', (BIT_B_1635)},
{'#', (BIT_C_1635)},
{'$', (BIT_D_1635)},
{'%', (BIT_A_1635)},
{'^', (BIT_E_1635)},
{'&', (BIT_F_1635)},
{'*', (BIT_G_1635)},
{'(', (BIT_P_1635)},
{')', (BIT_A_1635)},
};
//            a
//         -------
//        |       |
//    //f |       | b
//         ---g---		
//        |       | c
//    //e |       |	
//         ---d---   p

*/

uint8_t gs_chAscii_1635[][2]={
{'0', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'1', (BIT_B_1635|BIT_C_1635)},
{'2', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'3', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'4', (BIT_B_1635|BIT_C_1635|BIT_F_1635|BIT_G_1635)},
{'5', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'6', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'7', (BIT_A_1635|BIT_B_1635|BIT_C_1635)},
{'8', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'9', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'a', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'A', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'b', (BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'B', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'c', (BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'C', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_D_1635)},
{'d', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'D', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'e', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'E', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'f', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'F', (BIT_A_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'g', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'G', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_F_1635)},
{'h', (BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'H', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'i', (BIT_C_1635)},
{'I', (BIT_B_1635|BIT_C_1635)},
{'j', (BIT_C_1635|BIT_D_1635)},
{'J', (BIT_B_1635|BIT_C_1635|BIT_D_1635)},
{'j', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'k', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'K', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'l', (BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'L', (BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'m', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'M', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_G_1635)},
{'n', (BIT_C_1635|BIT_E_1635|BIT_G_1635)},
{'N', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635)},
{'o', (BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'O', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'p', (BIT_A_1635|BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'P', (BIT_A_1635|BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'q', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_F_1635|BIT_G_1635)},
{'Q', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_P_1635)},
{'r', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'R', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'s', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'S', (BIT_A_1635|BIT_C_1635|BIT_D_1635|BIT_F_1635|BIT_G_1635)},
{'t', (BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'T', (BIT_A_1635|BIT_E_1635|BIT_F_1635)},
{'u', (BIT_C_1635|BIT_D_1635|BIT_E_1635)},
{'U', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'v', (BIT_C_1635|BIT_D_1635|BIT_E_1635)},
{'V', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'w', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'W', (BIT_B_1635|BIT_C_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'x', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'X', (BIT_B_1635|BIT_C_1635|BIT_G_1635|BIT_E_1635|BIT_F_1635)},
{'y', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'Y', (BIT_B_1635|BIT_E_1635|BIT_F_1635|BIT_G_1635)},
{'z', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{'Z', (BIT_A_1635|BIT_B_1635|BIT_D_1635|BIT_E_1635|BIT_G_1635)},
{' ', (BIT_O_1635)},
{']', (BIT_A_1635|BIT_B_1635|BIT_C_1635|BIT_D_1635)},
{'[', (BIT_A_1635|BIT_D_1635|BIT_E_1635|BIT_F_1635)},
{'-', (BIT_G)},
{'_', (BIT_A_1635)},
{'@', (BIT_B_1635)},
{'#', (BIT_C_1635)},
{'$', (BIT_D_1635)},
{'%', (BIT_A_1635)},
{'^', (BIT_E_1635)},
{'&', (BIT_F_1635)},
{'*', (BIT_G_1635)},
{'(', (BIT_P_1635)},
{')', (BIT_A_1635)},
};

#if 0
static void gx_stb_panel_tm1635_init_led_data(void)
{
	#if 0
	gs_chLedData_1635[LED_DATA_0] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]);
	gs_chLedData_1635[LED_DATA_1] = (g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]);
	gs_chLedData_1635[LED_DATA_2] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_3] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_4] = (g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_5] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_6] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_7] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]);
	gs_chLedData_1635[LED_DATA_8] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_9] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_E] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_F] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_N] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]);
	gs_chLedData_1635[LED_DATA_P] = (g_PanelTM1635Config.m_chLedBit[BIT_A]
					| g_PanelTM1635Config.m_chLedBit[BIT_B]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	gs_chLedData_1635[LED_DATA_t] = (g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);	
	gs_chLedData_1635[LED_DATA_L] = (g_PanelTM1635Config.m_chLedBit[BIT_G]);

	gs_chLedData_1635[LED_DATA_b] = (g_PanelTM1635Config.m_chLedBit[BIT_C]
					| g_PanelTM1635Config.m_chLedBit[BIT_D]
					| g_PanelTM1635Config.m_chLedBit[BIT_E]
					| g_PanelTM1635Config.m_chLedBit[BIT_F]
					| g_PanelTM1635Config.m_chLedBit[BIT_G]);
	#endif
}
#endif

void panel_tm1635_set_portout(uint8_t port_num)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(port_num);

}

void panel_tm1635_set_portin(uint8_t port_num)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(port_num);
}

void Tm1635_DelayNTimes(uint8_t times)
{
	uint8_t i;

	for(i = 0; i < times; i++)
	{
		_NOP_;
	}
}


void CLR_CLOCK(void)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelTM1635Config.m_chClkPin, 
																GXPIO_BIT_LOW_LEVEL);
}

void SET_CLOCK(void)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelTM1635Config.m_chClkPin,
																	GXPIO_BIT_HIGHT_LEVEL);
}

void SET_DATA(void)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelTM1635Config.m_chDataPin,
																	GXPIO_BIT_HIGHT_LEVEL);
}

void CLR_DATA(void)
{
	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelTM1635Config.m_chDataPin,
																	GXPIO_BIT_LOW_LEVEL);
}

/*
*说明：点亮或关闭待机灯。
*输入参数：
*			u8Light: u8Light为1表示点亮，为0表示熄灭。
*输出参数:
*			无。
*		返回：
*		BLSM_NO_ERROR:成功。
*			错误号：失败。
*/
void FP_SetStandby(void)
{
	gs_chLedValue_1635[4] = BIT_A_1635;
    return;
}

void panel_tm1635_start(void)
{
    SET_CLOCK();
	//clk = 1;	//给芯片发送开始信号
    SET_DATA();//dio = 1;
    Tm1635_DelayNTimes(2);
    CLR_DATA();//dio = 0;
    Tm1635_DelayNTimes(2);
    CLR_CLOCK();//clk = 0;	//开始信号传送完
    Tm1635_DelayNTimes(1);
} 

void panel_tm1635_write(uint8_t wr_data)
{
	unsigned int i;

	//GXPIO_Data_t whether_key;
	
	panel_tm1635_set_portout(g_PanelTM1635Config.m_chDataPin);

	for(i=0;i<8;i++)//开始传送8位数据，每循环一次传送一位数据
	{
		CLR_CLOCK();//clk =0;
		Tm1635_DelayNTimes(2);
		if((wr_data & 0x1))
		{
			SET_DATA();
		}
		else
		{
			CLR_DATA();
		}
		wr_data >>= 1;//移位数据，低位在前!
		Tm1635_DelayNTimes(3);
		SET_CLOCK();//clk =1;
		Tm1635_DelayNTimes(2);
	}  
			 //8位数据传送完1
	CLR_CLOCK();//clk = 0;//判断芯片发过来的ACK应答信号
	Tm1635_DelayNTimes(2);
	SET_DATA();//dio = 1;
	Tm1635_DelayNTimes(2);
	SET_CLOCK();//clk = 1;
	Tm1635_DelayNTimes(2);

	panel_tm1635_set_portin(g_PanelTM1635Config.m_chDataPin);
	Tm1635_DelayNTimes(2);
	Tm1635_DelayNTimes(2);
	panel_tm1635_set_portout(g_PanelTM1635Config.m_chDataPin);
	Tm1635_DelayNTimes(2);
}

//---------结束信号-------------------
void panel_tm1635_stop(void)
{
    CLR_CLOCK();//clk = 0;
    Tm1635_DelayNTimes(2);
    CLR_DATA();//dio = 0;
    Tm1635_DelayNTimes(2);
    SET_CLOCK();//clk = 1;
    Tm1635_DelayNTimes(3);
    SET_DATA();//dio = 1;
}

//led显示
void font_pannel_led_scan(void)
{
	uint8_t a;

	SET_DATA();//dio = 1;
	SET_CLOCK();//clk = 1;
	panel_tm1635_start();    //调用开始信号
	panel_tm1635_write(0x44);//写命令40H（数据设置），固定地址模式
	panel_tm1635_stop();	  //调用结束信号，一个字节命令发送完毕，可以发送下一个命令
	Tm1635_DelayNTimes(4);
	for(a=0;a<5;a++)
	{
		panel_tm1635_start();	  //调用开始信号
		panel_tm1635_write(0xc0+a);//写起始地址命令（0C0H），地址从00H单元开始。
		panel_tm1635_write(gs_chLedValue_1635[a]);//给显示寄存器写显示数据，值可根据实参改变
		panel_tm1635_stop(); //将所有数据送完后，发结束信号，可以发送下一条命令
	}
	Tm1635_DelayNTimes(3);
	panel_tm1635_start();	  //调用开始信号
	panel_tm1635_write(0x8f); //送开屏命令，（8BH），亮度可以根据低三位调节。
	panel_tm1635_stop();	  //调用结束信号，一个字节命令发送完毕，可以发送下一个命令
}

#if 1
//---------读按键数据子程序-----------
uint32_t panel_tm1635_read(void)
{
	uint32_t x, key;
	uint64_t data;
    
	panel_tm1635_set_portin(g_PanelTM1635Config.m_chDataPin);
	Tm1635_DelayNTimes(2);
	CLR_CLOCK();//clk = 0;
	Tm1635_DelayNTimes(2);
	key = 0;
	for(x=0;x<8;x++) //读按键数据开始
	{
		CLR_CLOCK();//clk = 0;
		Tm1635_DelayNTimes(3);
        SET_CLOCK();//clk = 1;
		//data=panel_tm1635_get_data(DATA);
		Tm1635_DelayNTimes(1);
		g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelTM1635Config.m_chDataPin, &data);
		key = key<<1;
		if(data)
		key |= 1;        

		//SET_CLOCK();//clk = 1;
		Tm1635_DelayNTimes(2);
	}
    
	panel_tm1635_set_portout(g_PanelTM1635Config.m_chDataPin);
	Tm1635_DelayNTimes(2);
	CLR_CLOCK();//clk = 0;
	Tm1635_DelayNTimes(2);
	SET_DATA();//dio = 1;
	Tm1635_DelayNTimes(2);
	SET_CLOCK();//clk = 1;
	Tm1635_DelayNTimes(3);

	panel_tm1635_set_portin(g_PanelTM1635Config.m_chDataPin);	
	Tm1635_DelayNTimes(2);
	Tm1635_DelayNTimes(2);
	panel_tm1635_set_portout(g_PanelTM1635Config.m_chDataPin);
	Tm1635_DelayNTimes(2);//for the ack 应答信号
    return(key);
}

//----------判按键子程序-----------
void font_pannel_key_scan(void)
{
	static uint8_t	chOldKey = 0;
	static uint8_t 	chCnt = 0;
	static uint8_t 	chFirst = 0;
	//uint32_t		KeyValue=0;
	uint32_t 		key_VALUE=0;
	
	SET_DATA();
	SET_CLOCK();
	panel_tm1635_start();	        //调用开始信号
	panel_tm1635_write(0x42);        //送读按键命令（42H），
	key_VALUE = panel_tm1635_read();	//读8位按键数据，其中高三位为0，低5位为按键编码。编码表见说明书第5页。
	panel_tm1635_stop();             //发结束信号，按键读取完毕。

	if(chOldKey !=key_VALUE)
	{
		chCnt = 0;
		chFirst = 0;
		chOldKey = key_VALUE;
	}
	else
	{
		//bsp_printf("chFirst = %d\n",chFirst);
		if(0 == chFirst)
		{
			chCnt ++;

			if(chCnt == 40)
			{
				chCnt = 0;
				chFirst = 1;
			}
			else
			{
				return ;
			}
		}
		else 
		{
			chCnt ++;

			//bsp_printf("chCnt = %d\n",chCnt);
			if(chCnt == 20)
			{
				chCnt = 0;
			}
			else
			{
				return ;
			}
		}
	}
    
	if(key_VALUE != 0xff)
	{
		//bsp_printf("key == 0x%x\n",key_VALUE);
	}
	
	switch(key_VALUE)
	{
	case 0xef:
		//KeyValue = STB_KEY_OK;
		gx_stb_panel_tm1635_send_keymsg(6);
		break;
	case 0x4f:
		//KeyValue = STB_KEY_DOWN;
		gx_stb_panel_tm1635_send_keymsg(3);
		break;
	case 0x2f:
		//KeyValue = STB_KEY_RIGHT;
		gx_stb_panel_tm1635_send_keymsg(4);
		break;
	case 0xaf:
		//KeyValue = STB_KEY_UP;
		gx_stb_panel_tm1635_send_keymsg(2);
		break;
	case 0x8f:
		//KeyValue = STB_KEY_LEFT;
		gx_stb_panel_tm1635_send_keymsg(5);
		break;
	case 0x6f:
		//KeyValue = STB_KEY_MENU;
		gx_stb_panel_tm1635_send_keymsg(7);
		break;
	case 0xcf:
		//KeyValue = STB_KEY_EXIT;
		gx_stb_panel_tm1635_send_keymsg(1);
		break;
	default:
		return;
	}
}
#endif
void gx_stb_panel_tm1635_scan_manager(void)
{
	static uint8_t	SaveLedNum[5] = {0};
	uint8_t			count =0;
	static          int time_count = 0;

	time_count++;

	if(gs_chStringLen > 4)
	{
		if(0 == (time_count%gs_chScroolSpeed))
		{
			if(gs_chStringLen+3 >= gs_chStringPos)
			{
				switch(gs_chStringPos)
				{
					case 0:
						memset(gs_chLedValue_1635, 0x00, 3);
						memcpy(gs_chLedValue_1635+3, gs_chStringValue, 1);
						break;
					case 1:
						memset(gs_chLedValue_1635, 0x00, 2);
						memcpy(gs_chLedValue_1635+2, gs_chStringValue, 2);
						break;
					case 2:
						memset(gs_chLedValue_1635, 0x00, 1);
						memcpy(gs_chLedValue_1635+1, gs_chStringValue, 3);
						break;
					case 3:
						memcpy(gs_chLedValue_1635, gs_chStringValue, 4);
						break;
					default:
						memcpy(gs_chLedValue_1635, gs_chStringValue+gs_chStringPos-3, 4);
						break;
				}
				gs_chStringPos++;	

			}
			else
			{
				gs_chStringPos = 0;
				memset(gs_chLedValue_1635, 0x00, 3);
				memcpy(gs_chLedValue_1635+3, gs_chStringValue + gs_chStringPos, 1);
				gs_chStringPos++;
			}
		}
	}
	else
	{
		memcpy(gs_chLedValue_1635, gs_chStringValue, 4);
	}

	if((SaveLedNum[0] != gs_chLedValue_1635[0])||	\
		(SaveLedNum[1] != gs_chLedValue_1635[1])|| \
		(SaveLedNum[2] != gs_chLedValue_1635[2])|| \
		(SaveLedNum[3] != gs_chLedValue_1635[3]) || \
		(SaveLedNum[4] != gs_chLedValue_1635[4]))
	{
		for(count = 0; count < 4; count++)
		{
			SaveLedNum[count] = gs_chLedValue_1635[count];
			//bsp_printf("%d=%d\n", count, SaveLedNum[count]);
		}
        if(time_count > 240)
        {
            time_count = 0;
        }
		font_pannel_led_scan();
	}
	//font_pannel_led_scan();
	font_pannel_key_scan();
}

void gx_stb_panel_tm1635_send_keymsg(uint32_t nKeyIndex)
{
	//extern uint32_t s_panel_key;
	//s_panel_key = nKeyIndex;
//	bsp_printf("+++++++++ key index %d\n", nKeyIndex);
	if((nKeyIndex!=0xff)&&(nKeyIndex<8))
		{
			s_panel_key = g_PanelTM1635Config.m_nKeyValue[nKeyIndex];
			poll_wakeup();

	}
//	g_PanelTM1635Config.m_tPanelPublicConfig.m_PanelDealKey(nKeyIndex);
}

static int gx_stb_panel_tm1635_convert_acsii(unsigned char *pStringIn,
	                                  unsigned char *pStringOut,
	                                  unsigned int nStrLen)
{
	int i;
	int j;
	int nLen;
	int nTimeFlag = 0;
	unsigned char nStrBuffer[10];
	
	memcpy(nStrBuffer, pStringIn, 10);
	
	nLen = strlen((char*)nStrBuffer);
	
	if(nStrBuffer[2] == ':')//12:16
	{
		memcpy(nStrBuffer + 2, nStrBuffer + 3, nLen - 3);
		nTimeFlag = 1;
	}

	if(nLen > nStrLen)
	{
		nLen = nStrLen;
	}

	memset(pStringOut, 0x00, nStrLen);
	for(i = 0; i < nLen; i++)
	{
		for(j = 0; j < sizeof(gs_chAscii_1635)/2; j++)
		{
			if(nStrBuffer[i] == gs_chAscii_1635[j][0])
			{
				break;
			}
		}
		pStringOut[i] = gs_chAscii_1635[j][1];
	}

	if(1 == nTimeFlag)
	{
		pStringOut[1] |= BIT_P_1635;
	}
	return 0;
}

void gx_stb_panel_tm1635_show_led_string(unsigned char *pStr)
{
	gs_chStringLen = strlen((char*)pStr);
	gs_chStringPos = 0;
	memset(gs_chStringValue, 0x00, 256);
	gx_stb_panel_tm1635_convert_acsii(pStr, gs_chStringValue, 256);
}


void gx_stb_panel_tm1635_set_led_string(GXLED_Str_t LedStr)
{
	gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_DARK];
	
	switch(LedStr)
	{
	case GXLED_HIDE:
		gs_chLedValue_1635[0] = BIT_G_1635;
		gs_chLedValue_1635[1] = BIT_G_1635;
		gs_chLedValue_1635[2] = BIT_G_1635;
		gs_chLedValue_1635[3] = BIT_G_1635;			
		break;
	case GXLED_BOOT:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_t];
		gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_o];
		gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_o];
		gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_b];			
		break;
	case GXLED_INIT:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_t];
		gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_1];
		gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_N];
		gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_1];
		break;
	case GXLED_ON:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_N];
		gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_0];
		break;
	case GXLED_OFF:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_F];
		gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_F];
		gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_0];
		break;
	case GXLED_E:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_E];
		break;
	case GXLED_P:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_P];
		break;
	case GXLED_SEAR:
		gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_L];
		gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_L];
		gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_L];
		gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_L];
		break;
	default:
		break;
	}
}

void gx_stb_panel_tm1635_init_pin(void)
{
    uint64_t nCfgPin = 0;
    (g_PanelPublicConfig.m_PanelCfgMultiplexFun)(nCfgPin);
    //gx_stb_panel_tm1635_show_led_string("12345678");
	//gx_stb_panel_tm1635_init_led_data();
	//gx_gpio_setio(g_PanelTM1635Config.m_chClkPin, GX_GPIO_OUTPUT);
	//gx_gpio_setio(g_PanelTM1635Config.m_chDataPin, GX_GPIO_OUTPUT);
	//gx_gpio_setio(g_PanelTM1635Config.m_chLockPin, GX_GPIO_OUTPUT);
	//gx_gpio_setlevel(g_PanelTM1635Config.m_chLockPin, GXPIO_BIT_HIGHT_LEVEL);
}

void gx_stb_panel_tm1635_lock(void)
{
	gs_chLedValue_1635[4] |= BIT_B_1635;
}

void gx_stb_panel_tm1635_unlock(void)
{
	gs_chLedValue_1635[4] &= (~BIT_B_1635);


	SET_DATA();//dio = 1;
	SET_CLOCK();//clk = 1;
	panel_tm1635_start();    //调用开始信号
	panel_tm1635_write(0x44);//写命令40H（数据设置），固定地址模式
	panel_tm1635_stop();	  //调用结束信号，一个字节命令发送完毕，可以发送下一个命令
	Tm1635_DelayNTimes(4);
//	for(a=0;a<5;a++)
	{
		panel_tm1635_start();	  //调用开始信号
		panel_tm1635_write(0xc0+4);//写起始地址命令（0C0H），地址从00H单元开始。
		panel_tm1635_write(gs_chLedValue_1635[4]);//给显示寄存器写显示数据，值可根据实参改变
		panel_tm1635_stop(); //将所有数据送完后，发结束信号，可以发送下一条命令
	}
	Tm1635_DelayNTimes(3);
	panel_tm1635_start();	  //调用开始信号
	panel_tm1635_write(0x8f); //送开屏命令，（8BH），亮度可以根据低三位调节。
	panel_tm1635_stop();	
}

void gx_stb_panel_tm1635_stand_by(void)
{
	gs_chLedValue_1635[4] |= BIT_A_1635;
}

void gx_stb_panel_tm1635_wake_up(void)
{
	gs_chLedValue_1635[4] &= (~BIT_A_1635);
}

void gx_stb_panel_tm1635_show_mode(GXPanel_ShowMode_t PanelMode)
{
	g_PanelTM1635Config.m_chShowMode = PanelMode;
}

void gx_stb_panel_tm1635_power_off(void)
{
	return;
}

void gx_stb_panel_tm1635_set_led_signal_value(uint32_t nLedValue)
{
	uint32_t i;
	gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_DARK];
	
	if(nLedValue > 9999)
	{
		nLedValue = 9999;
	}

	for(i = 0; i < g_PanelTM1635Config.m_chLedCnt; i++)
	{
		gs_chLedValue_1635[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue_1635[i])/10;
	}
		
	for(i = 0; i < g_PanelTM1635Config.m_chLedCnt - 1; i++)
	{
		gs_chLedValue_1635[i] = gs_chLedValue_1635[gs_chLedValue_1635[i]];
	}

	gs_chLedValue_1635[g_PanelTM1635Config.m_chLedCnt-1] = gs_chLedData_1635[LED_DATA_P];
}

void gx_stb_panel_tm1635_set_led_value(uint32_t nLedValue)
{
# if 0
	int8_t i;
	

	gs_chLedValue_1635[0] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[1] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[2] = gs_chLedData_1635[LED_DATA_DARK];
	gs_chLedValue_1635[3] = gs_chLedData_1635[LED_DATA_DARK];

	if(nLedValue > 9999)
	{
		nLedValue = 9999;
		bsp_printf("nLedValue = %d\n", nLedValue);
	}

	for(i = (g_PanelTM1635Config.m_chLedCnt-1) ; i >= 0  ; i--)
	{
		gs_chLedValue_1635[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue_1635[i])/10;
		//bsp_printf("gs_chLedValue_1635[%d]= %d, nLedValue=%d\n",i,gs_chLedValue_1635[i], nLedValue);
	}
		
	for(i = 0; i < g_PanelTM1635Config.m_chLedCnt; i++)
	{
		gs_chLedValue_1635[i] =  gs_chLedData_1635[gs_chLedValue_1635[i]];
		//bsp_printf("gs_chLedValue_1635[%d]= %d\n",i,gs_chLedValue_1635[i]);
	}
#else
	int8_t i;
	char pStr[5] = {0};
	uint32_t temp = 0;
	for(i = 3 ; i >= 0  ; i--)
	{
		temp = (nLedValue)%10;
		pStr[i] = 0x30 + temp;
		nLedValue = (nLedValue -temp)/10;
	}
	
	gs_chStringLen = strlen((char*)pStr);
	gs_chStringPos = 0;
	memset(gs_chStringValue, 0x00, 256);
	gx_stb_panel_tm1635_convert_acsii((unsigned char *)pStr, (unsigned char *)gs_chStringValue, 256);
#endif
}

void gx_stb_panel_tm1635_isr(void)
{
	return;
}


uint32_t gx_stb_panel_tm1635_config(void)
{
	uint32_t nKeyValue[8]={
						0, 
						PANEL_KEY_EXIT, 
						PANEL_KEY_UP, 
						PANEL_KEY_DOWN, 
						PANEL_KEY_RIGHT, 
						PANEL_KEY_LEFT, 
						PANEL_KEY_OK, 
						PANEL_KEY_MENU};
	
	uint8_t chLedBit[8]={
					1<<0, 
					1<<1, 
					1<<2, 
					1<<3, 
					1<<4, 
					1<<5, 
					1<<6, 
					1<<7			
					};

	memset(g_PanelTM1635Config.m_chPanelName, 0, sizeof(g_PanelTM1635Config.m_chPanelName));
	sprintf((char*)g_PanelTM1635Config.m_chPanelName, "%s", "tm1635_sz");
	memcpy(g_PanelTM1635Config.m_chLedBit,chLedBit,sizeof(chLedBit));
	memcpy(g_PanelTM1635Config.m_nKeyValue,nKeyValue,sizeof(nKeyValue));
    /*

	g_PanelTM1635Config.m_chClkPin = 16;
	g_PanelTM1635Config.m_chDataPin = 15;
	g_PanelTM1635Config.m_chLockPin = 17;

    */

    g_PanelTM1635Config.m_chClkPin = PANEL_CLK_GPIO;//24;
	g_PanelTM1635Config.m_chDataPin = PANEL_DATA_GPIO;//23;
	g_PanelTM1635Config.m_chLockPin = PANEL_LOCK_GPIO;//25;

	g_PanelTM1635Config.m_chLedCnt = 4;
	g_PanelTM1635Config.m_chCurLenCnt = 0;

	gx_stb_panel_public_config();
	memcpy(&g_PanelTM1635Config.m_tPanelPublicConfig
		, &g_PanelPublicConfig
		, sizeof(GXPANEL_PUBLIC_Config_t));

	return 0;
}

GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = gx_stb_panel_tm1635_config,
	.m_PanelIsrFun = gx_stb_panel_tm1635_isr,
	.m_PanelInitPinFun = gx_stb_panel_tm1635_init_pin,
	.m_PanelLockFun = gx_stb_panel_tm1635_lock,
	.m_PanelSetSignalValueFun = gx_stb_panel_tm1635_set_led_signal_value,
	.m_PanelSetStringFun = gx_stb_panel_tm1635_show_led_string,
	.m_PanelSetValueFun = gx_stb_panel_tm1635_set_led_value,
	.m_PanelUnlockFun = gx_stb_panel_tm1635_unlock,
	.m_PanelStandByFun = gx_stb_panel_tm1635_stand_by,
	.m_PanelWakeUpFun = gx_stb_panel_tm1635_wake_up,
	.m_PanelScanMangerFun = gx_stb_panel_tm1635_scan_manager,
	.m_PanelPowerOffFun = gx_stb_panel_tm1635_power_off,
};
#endif




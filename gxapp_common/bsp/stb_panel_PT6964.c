#include "stb_panel.h"
//#ifdef PANEL_TYPE_PT6964
#if (PANEL_TYPE == PANEL_TYPE_PT6964)

extern	void poll_wakeup(void);
#if defined(LINUX_OS)
#include<linux/kernel.h>
#include <linux/gx_gpio.h>
#define PRESS_COUNT_MAX (2)
#define KEY_COUNT_MAX (1)
#elif defined(ECOS_OS)
#define PRESS_COUNT_MAX (15)
#define KEY_COUNT_MAX (10)
#endif

#define MANUFACTURE_CHUANGWEI 1

#define GXPIO_BIT_HIGHT_LEVEL 1
#define GXPIO_BIT_LOW_LEVEL    0
extern uint32_t s_panel_key;

typedef struct GXPANEL_PT6964_Config_s
{
	uint8_t m_chPanelName[MAX_PANEL_NAME_LENGTH];

	uint8_t m_chLedBit[8];

	uint8_t m_chLedCnt;	//the number of panel led
	uint8_t m_chCurLenCnt;//cur scan led
	uint8_t m_chShowMode;//GXPanel_ShowMode_t
	uint8_t m_chClkPin;	//the clk pin

	uint8_t m_chDataPin;	//the data pin
	uint8_t m_chStbPin;	//the stb pin
	uint8_t m_chSegmentLight;//0表示0点亮 共阳的；1表示1点亮 共阴的//一般为共阴

	uint8_t m_chGpioLockEn;//如果为GPIO口控制 lock 灯，该成员设为1
	uint8_t m_chLockPin;//如果为GPIO口控制 lock 灯，该位为GPIO口位

	uint8_t m_chStandbySegment;//standby灯由段控制
	uint8_t m_chLockSegment;//Lock灯由段控制

	uint32_t m_nKeyTranslateCode[8];//the led pin	  与m_nKeyValue[10]一一对应	
	uint32_t m_nKeyValue[8];
	uint8_t m_nLedAddr[8];//the led pin	
	//u8 m_nLockLedAddr[4];//锁定灯与待机灯的地址//固定的，不用单拉出来

	uint32_t m_nPortBaseAddr[PORT_BASE_ADDRESS_END];//the base address of port

	GXPANEL_PUBLIC_Config_t m_tPanelPublicConfig;
}GXPANEL_PT6964_Config_t;
GXPANEL_PT6964_Config_t g_PanelPT6964Config;

static uint8_t gs_chLedData[LED_DATA_DARK+1]={
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_E|BIT_F),		//0
		(BIT_B|BIT_C),								//1
		(BIT_A|BIT_B|BIT_D|BIT_E|BIT_G),			//2
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_G),			//3
		(BIT_B|BIT_C|BIT_F|BIT_G),					//4
		(BIT_A|BIT_C|BIT_D|BIT_F|BIT_G),			//5
		(BIT_A|BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),		//6
		(BIT_A|BIT_B|BIT_C),						//7
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),//8
		(BIT_A|BIT_B|BIT_C|BIT_D|BIT_F|BIT_G),		//9
		(BIT_A|BIT_D|BIT_E|BIT_F|BIT_G),			//E
		(BIT_A|BIT_E|BIT_F|BIT_G),					//F
		(BIT_A|BIT_B|BIT_C|BIT_E|BIT_F),			//N
		(BIT_A|BIT_B|BIT_E|BIT_F|BIT_G),			//P
	(BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),            //t
	(BIT_D|BIT_E|BIT_F),                        //L
	(BIT_C|BIT_D|BIT_E|BIT_F|BIT_G),            //b
	(BIT_C|BIT_D|BIT_E|BIT_G),                  //o
	0x00,                                       //HIDE
        (BIT_A|BIT_D|BIT_E|BIT_F),                  //C
	(BIT_B|BIT_C|BIT_D|BIT_E|BIT_F),            //U
	0x00					    //dark
	};/*0,1,2,3,4,5,6,7,8,9,E,F,N,P,T,dark*/
static uint16_t gs_chLedValue[4];
static uint8_t s_Lockenable;
static int32_t button_mask=0;
static int32_t iPressCount=0;
static uint32_t uKeyCodePrev=0xff;
static int32_t iKeyCount=0;
static uint8_t s_standby;
static int32_t stamp = 0;

/*关于控制PT6964的几个命令*/
#define WS_REFRESH_MAX         (10)
/*设置显示模式为:10: 6 digits, 12 segments*/
#define WS_MODE                	 (0x01)//(0x03)//(0x02)//
/*设置显示控制为:LED显示，Pulse width=14/16*/
#define WS_DISPLAY_CTL          (0x8f)
/*设置PT6964的读写模式为:递增地址写数据模式(LED显示)*/
#define WS_ADDRESS_INC          (0x40)
/*设置PT6964的读写模式为:固定地址写数据模式(LED显示)*/
#define WS_ADDRESS_FIX          (0x44)
/*设置PT6964的管位的地址(高两为为1，所以为C)*/
#define WS_ADDRESS_BASE       (0xc0)
/*设置PT6964的读写模式为:固定地址读数据模式(按键扫描)*/
#define WS_READ_KEY                (0x46)//(0x42)
/*mcu待机指令*/
#define WS_STANBY_CMD			(0x15)

#define FIRST_ADDR_LockorPower (0x0D)   /*Lock或Power灯的地址*/
#define TWO_ADDR_LockorPower (0x0D)   /*Lock或Power灯的地址*/
#define THREE_ADDR_LockorPower (0x0D)   /*Lock或Power灯的地址*/
#define FOUR_ADDR_LockorPower (0x0D)   /*Lock或Power灯的地址*/

/*typedef enum
{
	DRV_FP_PT_K1_MENU = 0xfdff,//0xC0000000,
	DRV_FP_PT_K2_LEFT = 0xfffe,//0x08000000,
	DRV_FP_PT_K3_DOWN = 0xf7ff, //0x00080000,
	DRV_FP_PT_K4_UP = 0xfeff, //0x00400000,
	DRV_FP_PT_K5_RIGHT = 0xfff7,//0x00004000,
	DRV_FP_PT_K6_OK= 0xefff,//0x00000800,
	DRV_FP_PT_K7_POWER = 0xfffd,
	DRV_FP_PT_K8_EXIT = 0xffff,
	DRV_FP_PT_K9_AUDIO = 0xffff

}DRV_FP_PT_SCANKEY_POSITION_e;*/

extern void gx_stb_panel_pt6964_send_keymsg(uint32_t nKeyIndex);
extern void gx_stb_panel_pt6964_set_led_string(unsigned char* str);
extern void gx_stb_panel_pt6964_lock(void);
extern void gx_stb_panel_pt6964_set_led_signal_value(uint32_t nLedValue);
extern void gx_stb_panel_pt6964_set_led_string(unsigned char* str);
extern void gx_stb_panel_pt6964_set_led_value(uint32_t nLedValue);
extern void gx_stb_panel_pt6964_unlock(void);
extern void gx_stb_panel_pt6964_standby(void);
void gx_stb_send_cmd_to_mcu(void);

inline void PT6964_DelayNTimes(uint8_t times)
{
	_NOP_
	#if 0
	uint8_t i;

	for(i = 0; i < times; i++)
	{
		_NOP_;
	}
	#endif
}
void PT6964_DelayNTimes_long(uint32_t times)
{
	volatile uint32_t i;
    volatile uint32_t j;

	for(i = 0; i < times; i++)
	{
        j = 18;
        do{
           j--;
        }while(j);
	}
}

static void gx_stb_panel_pt6964_init_led_data(void)
{
	gs_chLedData[LED_DATA_0] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_1] = (g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]);
	gs_chLedData[LED_DATA_2] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_3] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_4] = (g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_5] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_6] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_7] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]);
	gs_chLedData[LED_DATA_8] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_9] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_E] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_F] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_N] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_P] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_B]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
    gs_chLedData[LED_DATA_C] = (g_PanelPT6964Config.m_chLedBit[BIT_A]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_t] = (g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);
    gs_chLedData[LED_DATA_U] = (g_PanelPT6964Config.m_chLedBit[BIT_B]
                    | g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]);
	gs_chLedData[LED_DATA_L] = (g_PanelPT6964Config.m_chLedBit[BIT_G]);
	gs_chLedData[LED_DATA_b] = (g_PanelPT6964Config.m_chLedBit[BIT_C]
					| g_PanelPT6964Config.m_chLedBit[BIT_D]
					| g_PanelPT6964Config.m_chLedBit[BIT_E]
					| g_PanelPT6964Config.m_chLedBit[BIT_F]
					| g_PanelPT6964Config.m_chLedBit[BIT_G]);

}



/*******************************************************************************
  函数名　　　　:  gx_stb_panel_pt6964_send_data(u8 Data)
  函数功能　　　:  发送一个数据或命令
  输入参数　　　:  Data:要发送的数据或命令
*******************************************************************************/
static void gx_stb_panel_pt6964_send_data(uint8_t Data)
{
    uint8_t i=0;


	for (i = 0; i < 8; i++)
	{

		//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
											, GXPIO_BIT_LOW_LEVEL);

        _NOP_;

		if(Data & 0x1)
		{
			//GXBSP_WSSetDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);

		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
											, GXPIO_BIT_LOW_LEVEL);

		}

        _NOP_;

		Data >>= 1;

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
											, GXPIO_BIT_HIGHT_LEVEL);

        _NOP_;


	}

	//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
											, GXPIO_BIT_LOW_LEVEL);

	//GXBSP_WSSetDataOut(); //?
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
											, GXPIO_BIT_HIGHT_LEVEL);
	return;

}
/*******************************************************************************
  函数名　　　　:  gx_stb_panel_pt6964_send_data_cmd
  函数功能　　　:  发送一个命令和相应的数据
  输入参数　　　:
  　　Command:要发送的命令，告诉数据写给谁的
  　　*Data:指向要发送的数据的指针
  　　num:要发送的数据的个数
*********************************************************************************/
static void gx_stb_panel_pt6964_send_data_cmd(uint8_t Command, uint8_t *Data,uint8_t num)
{
	uint8_t  TempData=0;
	//u8  i =0;

	/*首先传送命令,STB*/
	//拉低STB
	//GXBSP_WSClearSTB();
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_LOW_LEVEL);



	gx_stb_panel_pt6964_send_data(Command);

	/*传送num个数据*/
	while(Data != NULL && num > 0)
	{
		TempData=*Data;

		gx_stb_panel_pt6964_send_data(TempData);

		num--;
		Data++;
	}

	//拉高STB
	//GXBSP_WSSetSTB() ;
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_HIGHT_LEVEL);


	_NOP_;
	_NOP_;

      //GXBSP_WSSetClock();//?
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
										, GXPIO_BIT_HIGHT_LEVEL);


      //GXBSP_WSSetDataOut() ;//?
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
										, GXPIO_BIT_HIGHT_LEVEL);
	_NOP_;
	_NOP_;

	return;

}

/*******************************************************************************
  函数名　　　　:  GXBSP_WSInit
  函数功能　　　:  PT6964初始化
*******************************************************************************/
static void gx_stb_panel_pt6964_init(void)
{
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chClkPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chDataPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chStbPin);

	/*首先设置为显示模式，4位13段*/
	gx_stb_panel_pt6964_send_data_cmd(WS_MODE,NULL,0);

    /*地址固定*/
   	gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_FIX,NULL,0);

	/*设置占空比，开显示*/
	gx_stb_panel_pt6964_send_data_cmd(WS_DISPLAY_CTL,NULL,0);
	return;

}
/*******************************************************************************
  函数名　　　　:  GXBSP_BoardInit
  函数功能　　　:  PT6964初始化
*******************************************************************************/
static void gx_stb_panel_pt6964_board_init(void)
{ 	uint16_t i;
	uint8_t uc_Data1[1]={0x00};

	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chClkPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chDataPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chStbPin);

	/*循环清空所有显示数据*/
	/*PT_REFRESH_MAX为10所以清空DIG1到DIG5控制的5组LED的*/
	for ( i = 0;i < WS_REFRESH_MAX;i++)
	{
	    /*设置对应的地址,并且清空该内存数据*/
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|i,uc_Data1,1);
	}

	return;

}

/*******************************************************************************
  函数名　　　　:  GXBSP_WSDisplayLedData
  函数功能　　　:  显示LED的数据
    输入参数　　　:
  　　*DisplayData:指向要显示的数据的指针

*******************************************************************************/

static void gx_stb_panel_pt6964_led_scan(uint16_t* DisplayData)
{
//	uint8_t data=0;
	uint8_t *data_tmp = (uint8_t*)DisplayData;

    /*首先设置为显示模式，4位13段*/
	gx_stb_panel_pt6964_send_data_cmd(WS_MODE,NULL,0);

	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[0],data_tmp,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[1],data_tmp+1,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[2],data_tmp+2,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[3],data_tmp+3,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[4],data_tmp+4,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[5],data_tmp+5,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[6],data_tmp+6,1);
	gx_stb_panel_pt6964_send_data_cmd(g_PanelPT6964Config.m_nLedAddr[7],data_tmp+7,1);

	#if 0
	DisplayData[4] = 0;
	if(s_Lockenable)
	{
		if(g_PanelPT6964Config.m_chSegmentLight)
			DisplayData[4] |= 1<<g_PanelPT6964Config.m_chLockSegment;
		else
			DisplayData[4] &= ~(1<<g_PanelPT6964Config.m_chLockSegment);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FIRST_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|TWO_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|THREE_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FOUR_ADDR_LockorPower,(DisplayData+4),1);
		return ;
	}
	else
	{
		if(g_PanelPT6964Config.m_chSegmentLight)
			DisplayData[4] &= ~(1<<g_PanelPT6964Config.m_chLockSegment);
		else
			DisplayData[4] |= 1<<g_PanelPT6964Config.m_chLockSegment;
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FIRST_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|TWO_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|THREE_ADDR_LockorPower,(DisplayData+4),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FOUR_ADDR_LockorPower,(DisplayData+4),1);
	}

	if(s_standby)
	{
		if(g_PanelPT6964Config.m_chSegmentLight)
			DisplayData[5] |= 1<<g_PanelPT6964Config.m_chStandbySegment;
		else
			DisplayData[5] &= ~(1<<g_PanelPT6964Config.m_chStandbySegment);

		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FIRST_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|TWO_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|THREE_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FOUR_ADDR_LockorPower,(DisplayData+5),1);
	}
	else
	{
		if(g_PanelPT6964Config.m_chSegmentLight)
			DisplayData[5] &= ~(1<<g_PanelPT6964Config.m_chStandbySegment);
		else
			DisplayData[5] |= 1<<g_PanelPT6964Config.m_chStandbySegment;
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FIRST_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|TWO_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|THREE_ADDR_LockorPower,(DisplayData+5),1);
		gx_stb_panel_pt6964_send_data_cmd(WS_ADDRESS_BASE|FOUR_ADDR_LockorPower,(DisplayData+5),1);
	}
	#endif
}

/*******************************************************************************
  函数名　　　　:  GXBSP_WSReceiveData
  函数功能　　　:  接收一个8位数据
  输入参数　　　:
  　　*ReceiveData:接收到的数据指针

*******************************************************************************/
static void gx_stb_panel_pt6964_receive_data(uint8_t* ReceiveData)
{
	uint8_t KeyData=0,i=0;
	uint16_t TempData=0;
	uint64_t TempRgValue=0;
	uint32_t key;

	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelPT6964Config.m_chDataPin);

	key = 0;
	for(i=0;i<8;i++)
	{
		TempData = 0;

		//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
									, GXPIO_BIT_LOW_LEVEL);

		PT6964_DelayNTimes(3);


		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelPT6964Config.m_chDataPin, &TempRgValue);
		#if 0
		if(TempRgValue & (((uint64_t)1)<<g_PanelPT6964Config.m_chDataPin))
		{
			TempData=1;
			bsp_printf("TempData=1\n");
		}
		else
		{
			TempData=0;
		}

		if (TempRgValue != 0)
		{
			bsp_printf("TempRgValue=%llx\n", TempRgValue);
		}
		#endif
		KeyData = KeyData<<1;
		if(TempRgValue)
		KeyData |= 1;

		//TempData <<= (7 - i);
		//KeyData |= TempData;
		PT6964_DelayNTimes(3);


		//GXBSP_WSSetClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
					, GXPIO_BIT_HIGHT_LEVEL);

		PT6964_DelayNTimes(3);
	}

	*ReceiveData = KeyData;

	return;
}

/*******************************************************************************
  函数名　　　　:  gx_stb_panel_pt6964_translate_key
  函数功能　　　:  从键位扫描数据到键功能值的转换
  输入参数　　　:  无
  　　renm_ScanData:键位扫描数据

*******************************************************************************/

static uint32_t gx_stb_panel_pt6964_translate_key(uint32_t renm_ScanData)
{
	if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[0])
	{
		//bsp_printf("DRV_FP_PT_K1_MENU\n");
		return (0);
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[1])
	{
		return (1);
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[2])
	{
		if (stamp == 1)
		{
			return (6);
		}
		else if (stamp == 0)
		{
			return (2);
		}
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[3])
	{
		return (3);
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[4])
	{
		if (stamp == 1)
		{
			return (5);
		}
		else if (stamp == 0)
		{
			return (4);
		}
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[5])
	{
		return (5);
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[6])
	{
		return (6);
	}
	else if (renm_ScanData == g_PanelPT6964Config.m_nKeyTranslateCode[7])
	{
		return (7);
	}
	else
	{
		//bsp_printf("0x%x\n", renm_ScanData);
		return (-1);
	}
			return (-1);

}

/*******************************************************************************
  函数名　　　　:  gx_stb_panel_pt6964_translate_key
  函数功能　　　:  获取按键并进行处理
  输入参数　　　:  无
*******************************************************************************/
static void gx_stb_panel_pt6964_process_key(void)
{
	int32_t i;
	uint32_t ui_ReceiveData = 0;
	uint32_t uKeyCode=-1;
	uint8_t uc_RecvByte;
	unsigned char bKeyPressed=0;
	//static u8 sShareKey = 0x01;
	//static u16 s_PowerDownActiveCnt=0;
	/*按键扫描*/
	//bsp_printf("ui_ReceiveData == 0x%x\n",ui_ReceiveData);

	stamp = 0;
	for (i = 0 ;i < 3;i++)
	{
		gx_stb_panel_pt6964_receive_data(&uc_RecvByte);
		//bsp_printf("uc_RecvByte == %#x\n",uc_RecvByte);
		ui_ReceiveData = (uint32_t)(ui_ReceiveData << 8) + (uint32_t)uc_RecvByte;
	}

	if(0 == ui_ReceiveData)
	{
		for(i = 0; i < 2; i++)
		{
			stamp = 1;
			gx_stb_panel_pt6964_receive_data(&uc_RecvByte);
			ui_ReceiveData = (uint32_t)(ui_ReceiveData << 8) + (uint32_t)uc_RecvByte;
		}
	}

	if(ui_ReceiveData!=0xffff||ui_ReceiveData!=0xffffff)
	{
        	//bsp_printf("\n == %x\n",ui_ReceiveData);

		if(ui_ReceiveData==button_mask)
		{
			iPressCount++;
		}
		else
		{
			iPressCount = 0;
			button_mask = ui_ReceiveData;
		}
	}
	else
	{
		iPressCount = 0;
		button_mask = 0;
		uKeyCodePrev = -1;
	}

	//bsp_printf("1  is  OK\n");

	if(iPressCount>=/*15*/PRESS_COUNT_MAX)/*去抖动*/
	{
		//if (ui_ReceiveData != 0)
		//{
		//	bsp_printf("ui_ReceiveData==0x%x\n",ui_ReceiveData);
		//}
		uKeyCode = gx_stb_panel_pt6964_translate_key(button_mask);

		bKeyPressed = FALSE;
		button_mask = 0;

		if(uKeyCode != -1)
		{
			if(uKeyCodePrev == uKeyCode)/*连续键处理*/
			{
				iKeyCount++;
				if(iKeyCount>=/*10*/KEY_COUNT_MAX)
				{
					iKeyCount = 0;
					bKeyPressed = TRUE;
				}
			}
			else
			{
				uKeyCodePrev = uKeyCode;
				iKeyCount = 0;
				bKeyPressed = TRUE;
			}
		}
		else
		{
			iPressCount = 0;
			button_mask = 0;
			uKeyCodePrev = -1;
		}
		//键值处理
		if(bKeyPressed)
		{
			gx_stb_panel_pt6964_send_keymsg(uKeyCode);
	    }
	}
}
#if 0
void gx_stb_panel_pt6964_scan_manager(void)
{
	static uint32_t count = 0;

	if(PANEL_SHOW_NORMAL == g_PanelPT6964Config.m_chShowMode)
	{

	}
	else if(PANEL_SHOW_TIME == g_PanelPT6964Config.m_chShowMode)
	{

	}
	else if(PANEL_SHOW_OFF == g_PanelPT6964Config.m_chShowMode)
	{
		gx_stb_panel_pt6964_set_led_string(GXLED_OFF);
		gx_stb_panel_pt6964_unlock();
		gx_stb_panel_pt6964_standby();
	}

	gx_stb_panel_pt6964_init();//要考虑热插拔的问题

	_NOP_;

	count ++;
	if(count > 30)
	{
		count = 0;
		gx_stb_panel_pt6964_led_scan(gs_chLedValue);
	}
	else
	{
		//GXBSP_WSClearSTB();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_LOW_LEVEL);

		gx_stb_panel_pt6964_send_data(WS_READ_KEY);

		_NOP_;//这个廷时不能少

		gx_stb_panel_pt6964_process_key();

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_HIGHT_LEVEL);
	}
	return;

}
#endif

#if 1
void gx_stb_panel_pt6964_scan_manager(void)
{
	#if 0
	static uint32_t count = 0;

	//gx_stb_panel_pt6964_init();

	_NOP_;

	count ++;
	if(count > 30)
	{
		count = 0;
	}
	else
	{
		//GXBSP_WSClearSTB();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chClkPin);
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chDataPin);
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chStbPin);

		//gx_stb_panel_pt6964_send_data(WS_READ_KEY);
		gx_stb_send_cmd_to_mcu();
	}
	#endif

	#if 1
	static uint32_t count = 0;
    if(PANEL_SHOW_NORMAL == g_PanelPT6964Config.m_chShowMode)
    {

    }
    else if(PANEL_SHOW_TIME == g_PanelPT6964Config.m_chShowMode)
    {

    }
    else if(PANEL_SHOW_OFF == g_PanelPT6964Config.m_chShowMode)
    {
        //gs_chLedValue[0] &= ~0x033b;
        //gs_chLedValue[1] &= ~0x033b;
        //gs_chLedValue[2] &= ~0x033b;
        //gs_chLedValue[3] &= ~0x033b;
        gx_stb_panel_pt6964_board_init();
		//gx_stb_panel_pt6964_led_scan(gs_chLedValue);
		//gx_stb_panel_pt6964_send_data_cmd(0x80,NULL,0);
        gx_stb_send_cmd_to_mcu();
        return;

    }

	gx_stb_panel_pt6964_init();

	_NOP_;

	count ++;
	if(count > 30)
	{
		count = 0;
		if (s_Lockenable == 1)
		{
			gs_chLedValue[0] |= 0x800;
		}
		else if (s_Lockenable == 0)
		{
			gs_chLedValue[0] &= 0x7ff;
		}
		gx_stb_panel_pt6964_led_scan(gs_chLedValue);
	}
	else
	{
		//GXBSP_WSClearSTB();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_LOW_LEVEL);

		gx_stb_panel_pt6964_send_data(WS_READ_KEY);

		PT6964_DelayNTimes(10);

		gx_stb_panel_pt6964_process_key();

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin
										, GXPIO_BIT_HIGHT_LEVEL);
	}
	#endif

	return;
}
#endif

void gx_stb_panel_pt6964_set_led_value(uint32_t nLedValue)
{
	uint32_t i;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];//
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];//LED_DATA_DARK   不用取反，在显示的时候取的
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];//
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];//

	if(nLedValue > 9999)
		nLedValue = 9999;

	for(i = 0; i < g_PanelPT6964Config.m_chLedCnt; i++)
	{
		gs_chLedValue[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue[i])/10;
	}
	#if 0
	for(i = 0; i < g_PanelPT6964Config.m_chLedCnt; i++)
	{
		if(g_PanelPT6964Config.m_chSegmentLight==1)
		{
			gs_chLedValue[i] = gs_chLedData[gs_chLedValue[i]];
		}
		else
		{
			gs_chLedValue[i] = (~gs_chLedData[gs_chLedValue[i]]);
		}
		//bsp_printf("====gs_chLedValue[%d]===%d\n",i,gs_chLedValue[i]);
	}
	#endif
	for(i = 0; i < 4; i++)
	{
		switch(gs_chLedValue[i])//For common positive LED
		{
			case 0: gs_chLedValue[i] = 0x0333; break;
			case 1: gs_chLedValue[i] = 0x0210; break;
			case 2: gs_chLedValue[i] = 0x0139; break;
			case 3: gs_chLedValue[i] = 0x0239; break;
			case 4: gs_chLedValue[i] = 0x021a; break;
			case 5: gs_chLedValue[i] = 0x022b; break;
			case 6: gs_chLedValue[i] = 0x032b; break;
			case 7: gs_chLedValue[i] = 0x0211; break;
			case 8: gs_chLedValue[i] = 0x033b; break;
			case 9: gs_chLedValue[i] = 0x023b; break;
		}
	}
}

void gx_stb_panel_pt6964_set_led_signal_value(uint32_t nLedValue)
{
	uint32_t i;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];

	if(nLedValue > 9999)
		nLedValue = 9999;

	for(i = 0; i < g_PanelPT6964Config.m_chLedCnt; i++)
	{
		gs_chLedValue[i] = (nLedValue)%10;
		nLedValue = (nLedValue - gs_chLedValue[i])/10;
	}

	for(i = 0; i < g_PanelPT6964Config.m_chLedCnt - 1; i++)
	{
		if(g_PanelPT6964Config.m_chSegmentLight==1)
		{
			gs_chLedValue[i] = gs_chLedData[gs_chLedValue[i]];
		}
		else
		{
			gs_chLedValue[i] = (~gs_chLedData[gs_chLedValue[i]]);
		}
	}

	if(g_PanelPT6964Config.m_chSegmentLight==1)
	{
		gs_chLedValue[g_PanelPT6964Config.m_chLedCnt-1] = gs_chLedData[LED_DATA_P];
	}
	else
	{
		gs_chLedValue[g_PanelPT6964Config.m_chLedCnt-1] = (~gs_chLedData[LED_DATA_P]);
	}
}

void gx_stb_panel_pt6964_set_led_string(unsigned char* str)
{
	unsigned char LedStr = *str;

	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];

	switch(LedStr)
	{
		case GXLED_BOOT:
			if(3 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_b];
			}
			else if(4 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_o];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_b];
			}
			break;
		case GXLED_INIT:
			if(3 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_N];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_1];
			}
			else if(4 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_t];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_1];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_N];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_1];
			}
			break;
		case GXLED_ON:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_N];
			gs_chLedValue[1] = gs_chLedData[LED_DATA_0];
			break;
		case GXLED_OFF:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_F];
			gs_chLedValue[1] = gs_chLedData[LED_DATA_F];
			gs_chLedValue[2] = gs_chLedData[LED_DATA_0];
			break;
		case GXLED_E:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_E];
			break;
		case GXLED_P:
			gs_chLedValue[0] = gs_chLedData[LED_DATA_P];
			break;
		case GXLED_SEAR:
			if(3 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_L];
			}
			else if(4 == g_PanelPT6964Config.m_chLedCnt)
			{
				gs_chLedValue[0] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[1] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[2] = gs_chLedData[LED_DATA_L];
				gs_chLedValue[3] = gs_chLedData[LED_DATA_L];
			}
			break;
		case GXLED_USB:
		case GXLED_HIDE:
			break;
	}
}

void gx_stb_panel_pt6964_lock(void)
{
	s_Lockenable = 1;
}

void gx_stb_panel_pt6964_unlock(void)
{
	s_Lockenable = 0;
}

void gx_stb_panel_pt6964_standby(void)
{
	
	s_standby = 1;
	gx_stb_send_cmd_to_mcu();
}

void gx_stb_panel_pt6964_on(void)
{
	s_standby = 0;
}

void gx_stb_panel_pt6964_show_mode(GXPanel_ShowMode_t PanelMode)
{
	g_PanelPT6964Config.m_chShowMode = PanelMode;
}



void gx_stb_panel_pt6964_send_keymsg(uint32_t nKeyIndex)
{
//	bsp_printf("+++++++++ key index %d\n", nKeyIndex);
	if((nKeyIndex!=0xff)&&(nKeyIndex<8))
		{
			s_panel_key = g_PanelPT6964Config.m_nKeyValue[nKeyIndex];
			poll_wakeup();

	}
//	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelDealKey(nKeyIndex);
}

void gx_stb_panel_pt6964_init_pin(void)
{
	//GXPIO_InitParams_t  init_params;
	//GXPIO_OpenParams_t 	open_params;
	uint64_t nCfgPin = 0;

	if(g_PanelPT6964Config.m_chGpioLockEn)
	{
		nCfgPin = (((uint64_t)1) << g_PanelPT6964Config.m_chClkPin)//37
				| (((uint64_t)1) << g_PanelPT6964Config.m_chDataPin)//36
				| (((uint64_t)1) << g_PanelPT6964Config.m_chStbPin)
				| (((uint64_t)1) << g_PanelPT6964Config.m_chLockPin);//33
	}
	else
	{
		nCfgPin = (((uint64_t)1) << g_PanelPT6964Config.m_chClkPin)//37
				| (((uint64_t)1) << g_PanelPT6964Config.m_chDataPin)//36
				| (((uint64_t)1) << g_PanelPT6964Config.m_chStbPin);//33
	}

	/*led灯初始化*/
	gx_stb_panel_pt6964_init_led_data();
//	(g_PanelPublicConfig.m_PanelCfgMultiplexFun)(nCfgPin);
//	(g_PanelPublicConfig.m_PanelCfgMultiplexFun)(nCfgPin);

	#if 0
	if (g_PanelPT6964Config.m_nPortBaseAddr[0] != 0
		&& g_PanelPT6964Config.m_nPortBaseAddr[1] == 0)
	{
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelInitGpioFun(nCfgPin);
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelPT6964Config.m_chClkPin, GPIO_OUTPUT);
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelPT6964Config.m_chDataPin, GPIO_OUTPUT);
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelOpenGpioFun(g_PanelPT6964Config.m_chStbPin, GPIO_OUTPUT);

		/*init_params.BaseAddress = g_PanelPT6964Config.m_nPortBaseAddr[0];
		init_params.IntNumber = 0;
		init_params.IntSource = 0;
		init_params.IntEn = 0;

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelInitGpioFun("FrontPanel", &init_params);

		open_params.ReservedBits =(1 << g_PanelPT6964Config.m_chClkPin)
					| (1 << g_PanelPT6964Config.m_chDataPin)
					| (1 << g_PanelPT6964Config.m_chStbPin);

		if(g_PanelPT6964Config.m_chGpioLockEn)
		{
			open_params.ReservedBits |= (1 << g_PanelPT6964Config.m_chLockPin);

			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin].Direction = GXPIO_BIT_OUTPUT;//Note:INput here
			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin].Pole = GXPIO_BIT_POSITIVE;
			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin].Level = GXPIO_BIT_HIGHT_LEVEL;
		}

		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin].Direction = GXPIO_BIT_INPUT;//Note:INput here
		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin].Level = GXPIO_BIT_HIGHT_LEVEL;

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelOpenGpioFun("FrontPanel", &open_params, &g_PanelPublicConfig.m_nGpioHandle);*/
	}
	else if (g_PanelPT6964Config.m_nPortBaseAddr[0] == 0
		&& g_PanelPT6964Config.m_nPortBaseAddr[1] != 0)
	{
		/*GPIO初始化*/
		/*init_params.BaseAddress = g_PanelPT6964Config.m_nPortBaseAddr[1];
		init_params.IntNumber = 0;
		init_params.IntSource = 0;
		init_params.IntEn = 0;

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelInitGpioFun("FrontPanel", &init_params);

		open_params.ReservedBits =(1 << (g_PanelPT6964Config.m_chClkPin-32))
					| (1 << (g_PanelPT6964Config.m_chDataPin-32))
					| (1 << (g_PanelPT6964Config.m_chStbPin-32));

		if(g_PanelPT6964Config.m_chGpioLockEn)
		{
			open_params.ReservedBits |= (1 << g_PanelPT6964Config.m_chLockPin);

			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin-32].Direction = GXPIO_BIT_OUTPUT;//Note:INput here
			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin-32].Pole = GXPIO_BIT_POSITIVE;
			open_params.BitCfg[g_PanelPT6964Config.m_chLockPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;
		}

		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin-32].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin-32].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chClkPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin-32].Direction = GXPIO_BIT_OUTPUT;
		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin-32].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chDataPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;

		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin-32].Direction = GXPIO_BIT_INPUT;//Note:INput here
		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin-32].Pole = GXPIO_BIT_POSITIVE;
		open_params.BitCfg[g_PanelPT6964Config.m_chStbPin-32].Level = GXPIO_BIT_HIGHT_LEVEL;

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelOpenGpioFun("FrontPanel", &open_params, &g_PanelPublicConfig.m_nGpioHandle);*/
	}
	else
	{
		//bsp_printf("[gx_stb_panel_pt6964_init_pin] panel gpio cfg error\n");
	}
	#endif
	gx_stb_panel_pt6964_board_init();
//	bsp_printf("gx_stb_panel_pt6964_init_pin\n");
}

void gx_stb_panel_pt6964_set_led_dark(void)
{
	gs_chLedValue[0] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[1] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[2] = gs_chLedData[LED_DATA_DARK];
	gs_chLedValue[3] = gs_chLedData[LED_DATA_DARK];
}

void gx_stb_panel_pt6964_power_off()
{
	gx_stb_panel_pt6964_show_mode(PANEL_SHOW_OFF);
	gx_stb_panel_pt6964_set_led_dark();
	gx_stb_panel_pt6964_unlock();
	gx_stb_panel_pt6964_scan_manager();
	gx_stb_panel_pt6964_standby();
}

void gx_stb_panel_pt6964_isr(void)
{
#if defined(LINUX_OS)
	create_bsp_timer(gx_stb_panel_pt6964_scan_manager,5);
#endif
	//char_dev_register(&gx3113_panel_dev, PANEL_NAME, NULL);
}

uint32_t gx_stb_panel_pt6964_config(void)
{
	uint32_t nKeyValue[8]={PANEL_KEY_UP
					, PANEL_KEY_DOWN
					, PANEL_KEY_LEFT
					, PANEL_KEY_RIGHT
					, PANEL_KEY_OK
					, PANEL_KEY_MENU
					, PANEL_KEY_EXIT
					, PANEL_KEY_POWER
					};

#ifdef MANUFACTURE_CHANGJIA1
	uint32_t nKeyValue[10]={PANEL_KEY_EXIT
					, PANEL_KEY_UP
					, PANEL_KEY_DOWN
					, PANEL_KEY_RIGHT
					, PANEL_KEY_MENU
					, PANEL_KEY_OK
					, PANEL_KEY_LEFT
					, PANEL_KEY_POWER
					, 0
					, 0
//					, PANEL_KEY_BACK
//					, PANEL_KEY_REC_PLAY
					};
#endif
#ifdef MANUFACTURE_CHANGJIA2
	uint32_t nKeyValue[10]={PANEL_KEY_DOWN
					, PANEL_KEY_LEFT
					, PANEL_KEY_UP
					, PANEL_KEY_POWER
					, PANEL_KEY_MENU
					, PANEL_KEY_EXIT
					, PANEL_KEY_RIGHT
					, PANEL_KEY_OK
					, 0
					, 0
//					, PANEL_KEY_BACK
//					, PANEL_KEY_REC_PLAY
					};
#endif

	uint8_t chLedBit[8]={1<<0
					, 1<<1
					, 1<<2
					, 1<<3
					, 1<<4
					, 1<<5
					, 1<<6
					, 1<<7
					};

	memset(g_PanelPT6964Config.m_chPanelName, 0, sizeof(g_PanelPT6964Config.m_chPanelName));
	sprintf((void*)g_PanelPT6964Config.m_chPanelName, "%s", "PT6964");//?
	memcpy(g_PanelPT6964Config.m_nKeyValue,nKeyValue,sizeof(nKeyValue));
	memcpy(g_PanelPT6964Config.m_chLedBit,chLedBit,sizeof(chLedBit));

	g_PanelPT6964Config.m_chClkPin = PANEL_CLK_GPIO;//35;
	g_PanelPT6964Config.m_chDataPin = PANEL_DATA_GPIO;//34;
	g_PanelPT6964Config.m_chStbPin= PANEL_STANDBY_GPIO;//36; 

#ifdef MANUFACTURE_DEMO
	g_PanelPT6964Config.m_nLedAddr[0]=0xc0;
	g_PanelPT6964Config.m_nLedAddr[1]=0xc2;
	g_PanelPT6964Config.m_nLedAddr[2]=0xc4;
	g_PanelPT6964Config.m_nLedAddr[3]=0xc6;
#endif
#ifdef MANUFACTURE_CHANGJIA1
	g_PanelPT6964Config.m_nLedAddr[0]=0xc0;
	g_PanelPT6964Config.m_nLedAddr[1]=0xc2;
	g_PanelPT6964Config.m_nLedAddr[2]=0xc4;
	g_PanelPT6964Config.m_nLedAddr[3]=0xc6;
#endif
#ifdef MANUFACTURE_CHANGJIA2
	g_PanelPT6964Config.m_nLedAddr[0]=0xc6;
	g_PanelPT6964Config.m_nLedAddr[1]=0xc4;
	g_PanelPT6964Config.m_nLedAddr[2]=0xc2;
	g_PanelPT6964Config.m_nLedAddr[3]=0xc0;
#endif
#ifdef MANUFACTURE_CHUANGWEI
	g_PanelPT6964Config.m_nLedAddr[0] = 0xc8;
	g_PanelPT6964Config.m_nLedAddr[1] = 0xc9;
	g_PanelPT6964Config.m_nLedAddr[2] = 0xc2;
	g_PanelPT6964Config.m_nLedAddr[3] = 0xc3;
	g_PanelPT6964Config.m_nLedAddr[4] = 0xc6;
	g_PanelPT6964Config.m_nLedAddr[5] = 0xc7;
	g_PanelPT6964Config.m_nLedAddr[6] = 0xc4;
	g_PanelPT6964Config.m_nLedAddr[7] = 0xc5;
#endif
	g_PanelPT6964Config.m_nKeyTranslateCode[0]=0x800000;
	g_PanelPT6964Config.m_nKeyTranslateCode[1]=0x100000;
	g_PanelPT6964Config.m_nKeyTranslateCode[2]=0x80;
	g_PanelPT6964Config.m_nKeyTranslateCode[3]=0x1000;
	g_PanelPT6964Config.m_nKeyTranslateCode[4]=0x10;
	g_PanelPT6964Config.m_nKeyTranslateCode[5]=0x10;
	g_PanelPT6964Config.m_nKeyTranslateCode[6]=0x80;
	g_PanelPT6964Config.m_nKeyTranslateCode[7]=0x8000;

#ifdef MANUFACTURE_DEMO
	g_PanelPT6964Config.m_chLockSegment=BIT_D;
#endif
#ifdef MANUFACTURE_CHANGJIA1
	g_PanelPT6964Config.m_chLockSegment=BIT_D;
#endif
#ifdef MANUFACTURE_CHANGJIA2
	g_PanelPT6964Config.m_chLockSegment=BIT_A;
#endif
	g_PanelPT6964Config.m_chStandbySegment=BIT_B;

	g_PanelPT6964Config.m_chGpioLockEn=0;
	g_PanelPT6964Config.m_chLockPin=0;

	g_PanelPT6964Config.m_chSegmentLight=1;

	g_PanelPT6964Config.m_chLedCnt = 4;
	g_PanelPT6964Config.m_chCurLenCnt = 0;

    g_PanelPT6964Config.m_nPortBaseAddr[PROT_BASE_LOW_ADDRESS] = 0xa0500000;
    g_PanelPT6964Config.m_nPortBaseAddr[PROT_BASE_HIGH_ADDRESS] = 0;

	gx_stb_panel_public_config();
	memcpy(&g_PanelPT6964Config.m_tPanelPublicConfig
		, &g_PanelPublicConfig
		, sizeof(GXPANEL_PUBLIC_Config_t));

	//com_gpio_port_clear(12);
//g	bsp_printf("gx_stb_panel_pt6964_config\n");
	return 0;
}

static inline void gx_send_to_mcu_data(uint8_t cmd)
{
	 uint8_t 		i=0;
	for (i = 0; i < 8; i++)
	{
		//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
																	, GXPIO_BIT_LOW_LEVEL);
		PT6964_DelayNTimes_long(13);

		if(cmd & 0x1)
		{
			//GXBSP_WSSetDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_HIGHT_LEVEL);
			//bsp_printf("cmd=1\n");
		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_LOW_LEVEL);
			//bsp_printf("cmd=0\n");
		}

		PT6964_DelayNTimes_long(10);

		cmd >>= 1;
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
						, GXPIO_BIT_HIGHT_LEVEL);

		PT6964_DelayNTimes_long(23);
	}
}

/*******************************************************************************
  函数名　　　　:  gx_stb_send_cmd_to_mcu(uint8_t Data)
  函数功能　　　:  发送一个命令到mcu
  输入参数　　　:  Data:要发送的数据或命令
*******************************************************************************/
int gx_stb_send_one_cmd_to_mcu_check(uint8_t cmd ,uint16_t data,uint8_t mask)
{
    uint8_t 		i=0;
	//uint8_t 		cmd = 0x15;
	//uint16_t		data;
    uint64_t        TempRgValue = 0;
    uint32_t        sendbuf = 0;
    sendbuf = mask<<24|data<<8|cmd;
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chClkPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chDataPin);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioOutPutFun(g_PanelPT6964Config.m_chStbPin);
	//起始位
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin,
																	GXPIO_BIT_HIGHT_LEVEL);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin,
																	GXPIO_BIT_HIGHT_LEVEL);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chStbPin,
																	GXPIO_BIT_HIGHT_LEVEL);

	PT6964_DelayNTimes_long(30);
	g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin,
																	GXPIO_BIT_LOW_LEVEL);
	PT6964_DelayNTimes_long(30);


	for (i = 0; i < 32; i++)
	{
		//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
																	, GXPIO_BIT_LOW_LEVEL);
		PT6964_DelayNTimes_long(13);

		if(sendbuf & 0x1)
		{
			//GXBSP_WSSetDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_HIGHT_LEVEL);
			//bsp_printf("cmd=1\n");
		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_LOW_LEVEL);
			//bsp_printf("cmd=0\n");
		}

		PT6964_DelayNTimes_long(10);

		sendbuf >>= 1;
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
						, GXPIO_BIT_HIGHT_LEVEL);

		PT6964_DelayNTimes_long(23);
	}
	#if 0
	//data = 0x0000;
	for (i = 0; i < 16; i++)
	{
		//GXBSP_WSClearClock();
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
																	, GXPIO_BIT_LOW_LEVEL);
		PT6964_DelayNTimes_long(13);

		if(cmd & 0x1)
		{
			//GXBSP_WSSetDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_HIGHT_LEVEL);
		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_LOW_LEVEL);
		}

		PT6964_DelayNTimes_long(10);

		cmd >>= 1;
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
						, GXPIO_BIT_HIGHT_LEVEL);

		PT6964_DelayNTimes_long(23);
	}
	#endif

	#if 0
	//cmd = 0xeb;
	for (i = 0; i < 8; i++)
	{
		//GXBSP_WSClearClock();

		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
																	, GXPIO_BIT_LOW_LEVEL);
		PT6964_DelayNTimes_long(13);

		if(mask & 0x1)
		{
			//GXBSP_WSSetDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_HIGHT_LEVEL);
			//bsp_printf("cmd=1\n");
		}
		else
		{
			//GXBSP_WSClearDataOut();
			g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chDataPin
						, GXPIO_BIT_LOW_LEVEL);
			//bsp_printf("cmd=0\n");
		}

		PT6964_DelayNTimes_long(10);

		mask >>= 1;
		g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioLevelFun(g_PanelPT6964Config.m_chClkPin
						, GXPIO_BIT_HIGHT_LEVEL);

        PT6964_DelayNTimes_long(23);

	}
	#endif
    //PT6964_DelayNTimes_long(10);
    TempRgValue = 0;
    g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelSetGpioInPutFun(g_PanelPT6964Config.m_chClkPin);
    g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelPT6964Config.m_chClkPin, &TempRgValue);
    i = 20;
    while(i)
    {
        g_PanelPT6964Config.m_tPanelPublicConfig.m_PanelGetGpioLevelFun(g_PanelPT6964Config.m_chClkPin, &TempRgValue);
        if(0==TempRgValue)
        {
            //bsp_printf("\n finish \n");
            return 0;

        }
        //bsp_printf(" =%d\n",TempRgValue);
        PT6964_DelayNTimes_long(10);

        i--;

    }

//    bsp_printf("err ");
    return -1;



}
void gx_stb_send_one_cmd_to_mcu(uint8_t cmd ,uint16_t data,uint8_t mask)
{
    int i=40;
    while(i)
    {
        PT6964_DelayNTimes_long(500);
        if(!gx_stb_send_one_cmd_to_mcu_check(cmd,data,mask))
            break;
        i--;
    }

}


void gx_stb_send_cmd_to_mcu(void)
{
#if 0
        //bsp_printf(" \n gx_stb_send_cmd_to_mcu \n");

		gx_stb_send_one_cmd_to_mcu(0x45,0x0b0b,0xa5);

		gx_stb_send_one_cmd_to_mcu(0x47,0x0b0b,0xa3);

		gx_stb_send_one_cmd_to_mcu(0x55,0x0b1b,0x85);

		gx_stb_send_one_cmd_to_mcu(0x57,0x0b1b,0x83);

		gx_stb_send_one_cmd_to_mcu(0x65,0x1b1b,0x65);

		gx_stb_send_one_cmd_to_mcu(0x67,0x0b1b,0x73);

		gx_stb_send_one_cmd_to_mcu(0x75,0x0b1b,0x65);

		gx_stb_send_one_cmd_to_mcu(0x77,0x0b1b,0x63);

		gx_stb_send_one_cmd_to_mcu(0x85,0x070f,0x65);

		gx_stb_send_one_cmd_to_mcu(0x87,0x2155,0x03);

#endif
		gx_stb_send_one_cmd_to_mcu(0x15,0x0000,0xeb);
}

GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = gx_stb_panel_pt6964_config,
	.m_PanelIsrFun = gx_stb_panel_pt6964_isr,
	.m_PanelInitPinFun = gx_stb_panel_pt6964_init_pin,
	.m_PanelLockFun = gx_stb_panel_pt6964_lock,
	.m_PanelSetSignalValueFun = gx_stb_panel_pt6964_set_led_signal_value,
	.m_PanelSetStringFun = gx_stb_panel_pt6964_set_led_string,
	.m_PanelSetValueFun = gx_stb_panel_pt6964_set_led_value,
	.m_PanelUnlockFun = gx_stb_panel_pt6964_unlock,
	.m_PanelShowModeFun = gx_stb_panel_pt6964_show_mode,
	.m_PanelScanMangerFun = gx_stb_panel_pt6964_scan_manager,
	.m_PanelPowerOffFun = gx_stb_panel_pt6964_power_off,
};
#endif

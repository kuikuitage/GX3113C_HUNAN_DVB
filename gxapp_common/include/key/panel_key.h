#ifndef __PANEL_KEY_H__
#define __PANEL_KEY_H__
#include "gxapp_sys_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************定义面板按键对应的遥控器键值******************/

#if (KEY_GONGBAN_NATIONALCHIP_NEW == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xbb444fb0
#define PANEL_KEY_UP    0xbb447788
#define PANEL_KEY_DOWN  0xbb44d728
#define PANEL_KEY_RIGHT 0xbb4437c8
#define PANEL_KEY_MENU  0xbb44cf30
#define PANEL_KEY_OK    0xbb4457a8
#define PANEL_KEY_LEFT  0xbb44b748
#define PANEL_KEY_POWER 0xbb44af50
#endif

#if (KEY_HUNAN_TAIHUI_NEW == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_UP    0xfdcbbf40
#define PANEL_KEY_DOWN  0xfdcb3fc0
#define PANEL_KEY_RIGHT 0xfdcba758
#define PANEL_KEY_MENU  0xfdcbd728
#define PANEL_KEY_OK    0xfdcb1fe0
#define PANEL_KEY_LEFT  0xfdcb8f70
#define PANEL_KEY_POWER 0xfdcb5da2
#endif

#if (KEY_RUNDE == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfb0405fa
#define PANEL_KEY_UP    0xfb04bf40
#define PANEL_KEY_DOWN  0xfb047f80
#define PANEL_KEY_RIGHT 0xfb045fa0
#define PANEL_KEY_MENU  0xfb046d92
#define PANEL_KEY_OK    0xfb04df20
#define PANEL_KEY_LEFT  0xfb04cf30
#define PANEL_KEY_POWER 0xfb04b748
#endif

#if (KEY_NEIMENG_CABLE == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0x58005800
#define PANEL_KEY_UP    0x55005000
#define PANEL_KEY_DOWN  0x44004f00
#define PANEL_KEY_RIGHT 0x52004900
#define PANEL_KEY_MENU  0x45005800
#define PANEL_KEY_OK    0x45004e00
#define PANEL_KEY_LEFT  0x4c004500
#define PANEL_KEY_POWER 0x50005700
#endif
#if (KEY_XINSIDA == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xff01d52a
#define PANEL_KEY_UP    0xff01857a
#define PANEL_KEY_DOWN  0xff01956a
#define PANEL_KEY_RIGHT 0xff01e51a
#define PANEL_KEY_MENU  0xff01659a
#define PANEL_KEY_OK    0xff01a55a
#define PANEL_KEY_LEFT  0xff0125da
#define PANEL_KEY_POWER 0xff01af50
#endif

#if (KEY_HEBEI == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xef98b748
#define PANEL_KEY_UP    0xef98d728
#define PANEL_KEY_DOWN  0xef989768
#define PANEL_KEY_RIGHT 0xef987788
#define PANEL_KEY_MENU  0xef988778
#define PANEL_KEY_OK    0xef9857a8
#define PANEL_KEY_LEFT  0xef9847b8
#define PANEL_KEY_POWER 0xef981de2
#endif

#if (KEY_YUENAN_KENENG_NEW == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_UP    0xfdcb3fc0
#define PANEL_KEY_DOWN  0xfdcbbf40
#define PANEL_KEY_RIGHT 0xfdcba758
#define PANEL_KEY_MENU  0xfdcbd728
#define PANEL_KEY_OK    0xfdcb1fe0
#define PANEL_KEY_LEFT  0xfdcb8f70
#define PANEL_KEY_POWER 0xfdcb5da2
#endif

#if (KEY_KENENG == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_UP    0xfdcb3fc0
#define PANEL_KEY_DOWN  0xfdcbbf40
#define PANEL_KEY_RIGHT 0xfdcba758
#define PANEL_KEY_MENU  0xfdcbd728
#define PANEL_KEY_OK    0xfdcb1fe0
#define PANEL_KEY_LEFT  0xfdcb8f70
#define PANEL_KEY_POWER 0xfdcb5da2
#endif

#if (KEY_ZHILING == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_UP    0xfdcb2dd2
#define PANEL_KEY_DOWN  0xfdcb8d72
#define PANEL_KEY_RIGHT 0xfdcb4db2
#define PANEL_KEY_MENU  0xfdcb659a
#define PANEL_KEY_OK    0xfdcbc738
#define PANEL_KEY_LEFT  0xfdcbcd32
#define PANEL_KEY_POWER 0xfdcb3fc0
#endif
#if (KEY_JINYA == REMOTE_TYPE)

#define PANEL_KEY_EXIT  0xbb444fb0
#define PANEL_KEY_UP    0xfe00ff00
#define PANEL_KEY_DOWN  0xfe007f80
#define PANEL_KEY_RIGHT 0xfe00bf40
#define PANEL_KEY_MENU  0xfe00a758
#define PANEL_KEY_OK    0xfe0007f8
#define PANEL_KEY_LEFT  0xfe003fc0
#define PANEL_KEY_POWER 0xfe00af50
#endif
#if (KEY_TAIHUI_CHONGQING == REMOTE_TYPE)/*Same as KEY_JINYA*/
#define PANEL_KEY_EXIT  0xfe00C738
#define PANEL_KEY_UP    0xfe00ff00
#define PANEL_KEY_DOWN  0xfe007f80
#define PANEL_KEY_RIGHT 0xfe00bf40
#define PANEL_KEY_MENU  0xfe00a758
#define PANEL_KEY_OK    0xfe0007f8
#define PANEL_KEY_LEFT  0xfe003fc0
#define PANEL_KEY_POWER 0xfe00af50
#endif
#if (KEY_SICHUAN == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0x58005800
#define PANEL_KEY_UP    0x44005000
#define PANEL_KEY_DOWN  0x53004400
#define PANEL_KEY_RIGHT 0x5a004a00
#define PANEL_KEY_MENU  0x45004800
#define PANEL_KEY_OK    0x45004e00
#define PANEL_KEY_LEFT  0x4e004f00
#define PANEL_KEY_POWER 0x50005700
#endif
#if (KEY_ANHUI == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xEF98F708
#define PANEL_KEY_UP    0xEF98D728
#define PANEL_KEY_DOWN  0xEF989768
#define PANEL_KEY_RIGHT 0xEF987788
#define PANEL_KEY_MENU  0xEF982DD2
#define PANEL_KEY_OK    0xEF9857A8
#define PANEL_KEY_LEFT  0xEF9847B8
#define PANEL_KEY_POWER 0xEF983DC2
#endif
#if (KEY_TONGHUI == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_UP    0xfdcb3fc0
#define PANEL_KEY_DOWN  0xfdcbbf40
#define PANEL_KEY_RIGHT 0xfdcba758
#define PANEL_KEY_MENU  0xfdcbd728
#define PANEL_KEY_OK    0xfdcb1fe0
#define PANEL_KEY_LEFT  0xfdcb8f70
#define PANEL_KEY_POWER 0xfdcb5da2
#endif

#if (KEY_NEIMENG_GX3113C == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0x58005800
#define PANEL_KEY_UP    0x44005000
#define PANEL_KEY_DOWN  0x53004400
#define PANEL_KEY_RIGHT 0x5a004a00
#define PANEL_KEY_MENU  0x45004800
#define PANEL_KEY_OK    0x45004e00
#define PANEL_KEY_LEFT  0x4e004f00
#define PANEL_KEY_POWER 0x50005700
#endif

#if (KEY_THINEWTEC == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xFD02C53A
#define PANEL_KEY_UP    0xFD022DD2
#define PANEL_KEY_DOWN  0xFD028D72
#define PANEL_KEY_RIGHT 0xFD024Db2
#define PANEL_KEY_MENU  0xFD02659A
#define PANEL_KEY_OK    0xFD02C738
#define PANEL_KEY_LEFT  0xFD02CD32
#define PANEL_KEY_POWER 0xFD023FC0
#endif

#if (KEY_TAOYUAN == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xFF00D52A
#define PANEL_KEY_UP    0xFF00857A
#define PANEL_KEY_DOWN  0xFF02956A
#define PANEL_KEY_RIGHT 0xFF00E51A
#define PANEL_KEY_MENU  0xFF0015EA
#define PANEL_KEY_OK    0xFF00A55A
#define PANEL_KEY_LEFT  0xFF0025DA
#define PANEL_KEY_POWER 0xFF00AF50
#endif
#if (KEY_TAIHUI == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0xfdcbc53a
#define PANEL_KEY_DOWN    0xfdcbbf40
#define PANEL_KEY_UP  0xfdcb3fc0
#define PANEL_KEY_RIGHT 0xfdcba758
#define PANEL_KEY_MENU  0xfdcbd728
#define PANEL_KEY_OK    0xfdcb1fe0
#define PANEL_KEY_LEFT  0xfdcb8f70
#define PANEL_KEY_POWER 0xfdcb5da2
#endif
#if (KEY_JIMO == REMOTE_TYPE)
#define PANEL_KEY_EXIT  0x7f405ca3
#define PANEL_KEY_DOWN    0x7f40b44b
#define PANEL_KEY_UP  0x7f40ac53
#define PANEL_KEY_RIGHT 0x7f407c83
#define PANEL_KEY_MENU  0x7f40946b
#define PANEL_KEY_OK    0x7f408c73
#define PANEL_KEY_LEFT  0x7f406699
#define PANEL_KEY_POWER 0x7f40c43b
#endif



	/*
* 其他遥控器定义
*/





#ifdef __cplusplus
}
#endif

#endif /* __PANEL_KEY_H__ */


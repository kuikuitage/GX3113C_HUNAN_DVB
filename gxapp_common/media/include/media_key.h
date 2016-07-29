#ifndef __MEDIA_KEY_H__
#define __MEDIA_KEY_H__

#include "gui_core.h"


#ifdef __cplusplus
extern "C" {
#endif


//player 
/*top*/
#define	APPK_POWER		GUIK_PRINT
#define	APPK_MODE		GUIK_SCROLLOCK
#define	APPK_MUTE		GUIK_HOME

/*TODO*/
#define	APPK_AV			GUIK_NUMLOCK
#define	APPK_YPBPR		GUIK_KP_DIVIDE
#define	APPK_HDMI		GUIK_KP_MULTIPLY
#define	APPK_VGA		GUIK_KP_MINUS

/*play function*/                                
#define	APPK_SUBT		GUIK_INSERT
#define	APPK_RATIO		GUIK_Y
#define	APPK_AUDIO		GUIK_DELETE
#define	APPK_SEEK		GUIK_END
#define	APPK_ROTATE		GUIK_PAGE_UP
#define	APPK_ZOOM		GUIK_PAGE_DOWN

/*play entry*/                                
#define	APPK_MOVIE		GUIK_F9
#define	APPK_MUSIC		GUIK_F10
#define	APPK_PIC		GUIK_F11
#define	APPK_TEXT		GUIK_F12
#define	APPK_FILE		GUIK_F
#define	APPK_DOWNLOAD	GUIK_D

/*play control*/                                
#define	APPK_PREVIOUS	GUIK_F1
#define	APPK_REW		GUIK_F3
#define	APPK_FF			GUIK_F2
#define	APPK_NEXT		GUIK_F4
#define	APPK_PLAY		GUIK_F5
#define	APPK_STOP		GUIK_F6

/*set*/                                
#define	APPK_BACK		GUIK_ESCAPE//GUIK_F7
#define	APPK_SET		GUIK_R//GUIK_F8
#define	APPK_VOLUP		GUIK_PLUS
#define	APPK_VOLDOWN	GUIK_MINUS


/*direction*/
#define	APPK_LEFT		GUIK_LEFT
#define	APPK_RIGHT		GUIK_RIGHT
#define	APPK_UP			GUIK_UP
#define	APPK_DOWN		GUIK_DOWN
#define	APPK_OK			GUIK_RETURN                            

//extern for media player
#define APPK_MENU       GUIK_M
#define APPK_PAUSE		GUIK_PAUSE
#define APPK_PAUSE_PLAY	GUIK_K
#define APPK_REPEAT     GUIK_F7
#define APPK_GREEN		GUIK_G
#define APPK_YELLOW		GUIK_Y


/*digital*/
#define	APPK_0			GUIK_0
#define	APPK_1			GUIK_1
#define	APPK_2			GUIK_2
#define	APPK_3			GUIK_3
#define	APPK_4			GUIK_4
#define	APPK_5			GUIK_5
#define	APPK_6			GUIK_6
#define	APPK_7			GUIK_7
#define	APPK_8			GUIK_8
#define	APPK_9			GUIK_9



#ifdef __cplusplus
}
#endif

#endif /* __MEDIA_KEY_H__ */


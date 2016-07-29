#ifndef _APP_FULL_KEYBOARD_H_
#define _APP_FULL_KEYBOARD_H_

#define WND_FULL_KEYBOARD			"wnd_full_keyboard"
#define TXT_FULL_KEYBOARD_INPUT 		"text_full_keyboard_input"
#define TXT_FULL_KEYBOARD_HINT 		"text_full_keyboard_hint"

#define BUF_LEN		65
#define KEY_NUM 		72
#define KEY_X			11
#define KEY_Y			5
#define KEY_CLR_NUM	11
#define KEY_DEL_NUM	23
#define KEY_CAPS_NUM	47
#define KEY_ESC_NUM	71

typedef void (* full_keyboard_proc)(void);

extern full_keyboard_proc s_full_keyboard_proc;
extern char s_full_keyboard_input[] ;

#endif

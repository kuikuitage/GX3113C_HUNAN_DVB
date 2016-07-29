//#include "app.h"
#include "file_view.h"
#include "play_text.h"

#define NOTEPAD					"text_view_notepad"

status_t play_text(const char* file)
{
	if(NULL == file) return GXCORE_ERROR;
	
	GUI_SetProperty(NOTEPAD, "file", (void*)file);
			
	return GXCORE_SUCCESS;
}


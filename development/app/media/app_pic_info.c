#include "app.h"


#define WIN_PIC_INFO        "win_pic_info"

#define TEXT_INFO0			"pic_info_text_info0"
#define TEXT_INFO1          	"pic_info_text_info1"
#define TEXT_INFO2		       "pic_info_text_info2"
#define TEXT_INFO3	              "pic_info_text_info3"
#define TEXT_INFO4                 "pic_info_text_info4"
#define TEXT_INFO5                 "pic_info_text_info5"
#define   BUTTON_OK                "pic_info_button_ok"   
#define WIN_PIC_VIEW             "win_pic_view"

SIGNAL_HANDLER  int pic_info_service(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	event = (GUI_Event *)usrdata;
	GUI_SendEvent(WIN_PIC_VIEW, event);
	
	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int pic_info_init(const char* widgetname, void *usrdata)
{	
	play_pic_info pic_info;	
	char string[50];
		
	play_pic_get_info(&pic_info);

	if(pic_info.exif.width&&pic_info.exif.height)	//alpha8 fixed
	{		
		memset(string,0,sizeof(string));
		sprintf(string, "%d x %d", pic_info.exif.width,pic_info.exif.height);		//alpha8 fixed
		GUI_SetProperty(TEXT_INFO0, "string", string);
	}	
	
	if(pic_info.exif.image_ext_info.DateTime)	
	{		
		GUI_SetProperty(TEXT_INFO1, "string", pic_info.exif.image_ext_info.DateTime);
	}
	
	if(pic_info.exif.image_ext_info.CameraMake)	
	{
		GUI_SetProperty(TEXT_INFO2, "string", pic_info.exif.image_ext_info.CameraMake);
	}

	if(pic_info.exif.image_ext_info.CameraModel)	
	{		
		GUI_SetProperty(TEXT_INFO3, "string", pic_info.exif.image_ext_info.CameraModel);
	}
	
	if(pic_info.exif.image_ext_info.FocalLength)	
	{		
		memset(string,0,sizeof(string));
		sprintf(string, "%f",pic_info.exif.image_ext_info.FocalLength);	
		GUI_SetProperty(TEXT_INFO4, "string", string);
	}

	
	if(pic_info.exif.image_ext_info.ExposureTime)	
	{		
		memset(string,0,sizeof(string));
		sprintf(string, "%f", pic_info.exif.image_ext_info.ExposureTime);	
		GUI_SetProperty(TEXT_INFO5, "string", string);
	}
	
	return 0;
}


SIGNAL_HANDLER int pic_info_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->type)
	{
		case GUI_MOUSEBUTTONDOWN:
			break;

		case GUI_KEYDOWN:
			switch(event->key.sym)
			{
				case APPK_LEFT:					
					break;
					
				case APPK_RIGHT:					
					break;				

				case APPK_OK:
					GUI_EndDialog(WIN_PIC_INFO);
					break;
					
				default:
					break;
			}
			break;
		default:
			break;
	}

	
	return EVENT_TRANSFER_KEEPON;
}




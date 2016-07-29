#include "app.h"
#include "app_pop.h"

#define WIN_PMP_SET				"win_pmp_set"
#define BOX_SETTING				"pmp_set_box"
#define COMBOBOX_LANGUAGE		"pmp_set_combo_language"
#define COMBOBOX_TVOUT			"pmp_set_combo_tvout"
#define COMBOBOX_VIDEO_FORMAT	"pmp_set_combo_video_format"
#define COMBOBOX_SCREEN_FORMAT	"pmp_set_combo_screen_format"
#define COMBOBOX_RESOLUTION_FORMAT	"pmp_set_boxitem_resolution"
#define COMBOBOX_POWER_ON_PLAY	"pmp_set_combo_power_on_play"
#define COMBOBOX_SAVE_TAGS		"pmp_set_combo_save_tags"
#define BUTTON_FAC_DEFAULT		"pmp_set_button_fac_default"

typedef enum
{
	OEM_OUTPUT_MODE_CVBS,
	OEM_OUTPUT_MODE_SCART,
	OEM_OUTPUT_MODE_VGA
}oem_output_mode;

typedef enum
{
	OEM_VIDEO_FORMAT_CVBS_AUTO,
	OEM_VIDEO_FORMAT_CVBS_PAL,
	OEM_VIDEO_FORMAT_CVBS_NTSC
}oem_video_format_cvbs;

typedef enum
{
	OEM_VIDEO_FORMAT_SCART_AUTO,
	OEM_VIDEO_FORMAT_SCART_PAL,
	OEM_VIDEO_FORMAT_SCART_NTSC
}oem_video_format_scart;

typedef enum
{
	OEM_VIDEO_FORMAT_VGA_AUTO,
	OEM_VIDEO_FORMAT_VGA_480P,
	OEM_VIDEO_FORMAT_VGA_576P
}oem_video_format_vga;

static void video_format_pmp_2_oem(pmpset_output_mode pmp_output)
{
	int32_t pmp_video_format = 0;
	int32_t oem_video_format = 0;

	pmp_video_format = pmpset_get_int(PMPSET_VIDEO_FORMAT);

	if(PMPSET_OUTPUT_MODE_RCA == pmp_output)
	{
		GUI_SetProperty(COMBOBOX_VIDEO_FORMAT, "content", (void*)"[Auto, PAL,  NTSC]");
		
		if(PMPSET_VIDEO_FORMAT_PAL == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_CVBS_PAL;
		}
		else if(PMPSET_VIDEO_FORMAT_NTSC_M == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_CVBS_NTSC;
		}
		else
		{
			oem_video_format = OEM_VIDEO_FORMAT_CVBS_AUTO;
		}			
	}
	else if(PMPSET_OUTPUT_MODE_SCART == pmp_output)
	{		
		GUI_SetProperty(COMBOBOX_VIDEO_FORMAT, "content", (void*)"[Auto, PAL,  NTSC]");
		
		if(PMPSET_VIDEO_FORMAT_PAL == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_SCART_PAL;
		}
		else if(PMPSET_VIDEO_FORMAT_NTSC_M == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_SCART_NTSC;
		}
		else
		{
			oem_video_format = OEM_VIDEO_FORMAT_SCART_AUTO;
		}			
	}
	/*else if(PMPSET_OUTPUT_MODE_VGA == pmp_output)
	{		
		GUI_SetProperty(COMBOBOX_VIDEO_FORMAT, "content", (void*)"[Auto, 480P, 576P]");
		
		if(PMPSET_VIDEO_FORMAT_VGA_480P == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_VGA_480P;
		}
		else if(PMPSET_VIDEO_FORMAT_VGA_576P == pmp_video_format)
		{
			oem_video_format = OEM_VIDEO_FORMAT_VGA_576P;
		}
		else
		{
			oem_video_format = OEM_VIDEO_FORMAT_VGA_AUTO;
		}			
	}*/

	GUI_SetProperty(COMBOBOX_VIDEO_FORMAT, "select", &oem_video_format);
	//GUI_SetProperty(COMBOBOX_VIDEO_FORMAT, "update", NULL);

}

static void video_format_oem_2_pmp(int32_t oem_video_format)
{
	int32_t oem_output_mode = 0;
	int32_t pmp_video_format = 0;


	GUI_GetProperty(COMBOBOX_TVOUT, "select", (void*)&oem_output_mode);

	if(OEM_OUTPUT_MODE_CVBS == oem_output_mode)
	{
		if(OEM_VIDEO_FORMAT_CVBS_PAL == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_PAL;
		}
		else if(OEM_VIDEO_FORMAT_CVBS_NTSC == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_NTSC_M;
		}
		else
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_AUTO;
		}
	}
	else if(OEM_OUTPUT_MODE_SCART == oem_output_mode)
	{
		if(OEM_VIDEO_FORMAT_SCART_PAL == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_PAL;
		}
		else if(OEM_VIDEO_FORMAT_SCART_NTSC == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_NTSC_M;
		}
		else
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_AUTO;
		}
	}
	else if(OEM_OUTPUT_MODE_VGA == oem_output_mode)
	{
		if(OEM_VIDEO_FORMAT_VGA_480P == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_VGA_480P;
		}
		else if(OEM_VIDEO_FORMAT_VGA_576P == oem_video_format)
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_VGA_576P;
		}
		else
		{
			pmp_video_format = PMPSET_VIDEO_FORMAT_AUTO;
		}
	}

	pmpset_set_int(PMPSET_VIDEO_FORMAT, pmp_video_format);
}




static void output_mode_pmp_2_oem(pmpset_output_mode output_mode)
{
	int32_t oem_output_mode = 0;
	
	if(PMPSET_OUTPUT_MODE_RCA == output_mode)
	{
		oem_output_mode = OEM_OUTPUT_MODE_CVBS;
	}
	else if(PMPSET_OUTPUT_MODE_SCART == output_mode)
	{		
		oem_output_mode = OEM_OUTPUT_MODE_SCART;
	}
	/*else if(PMPSET_OUTPUT_MODE_VGA == output_mode)
	{		
		oem_output_mode = OEM_OUTPUT_MODE_VGA;
	}*/

	GUI_SetProperty(COMBOBOX_TVOUT, "select", (void*)&oem_output_mode);

}

static void output_mode_oem_2_pmp(oem_output_mode oem_output_mode)
{
	int32_t pmp_output_mode = 0;
	
	if(OEM_OUTPUT_MODE_CVBS == oem_output_mode)
	{
		pmp_output_mode = PMPSET_OUTPUT_MODE_RCA;
	}
	else if(OEM_OUTPUT_MODE_SCART == oem_output_mode)
	{		
		pmp_output_mode = PMPSET_OUTPUT_MODE_SCART;
	}
	/*else if(OEM_OUTPUT_MODE_VGA == oem_output_mode)
	{		
		pmp_output_mode = PMPSET_OUTPUT_MODE_VGA;
	}*/

	pmpset_set_int(PMPSET_OUTPUT_MODE, pmp_output_mode);

	video_format_pmp_2_oem(pmp_output_mode);
}



SIGNAL_HANDLER int pmp_set_init(const char* widgetname, void *usrdata)
{
	int32_t value = 0;

	value = pmpset_get_int(PMPSET_LANG);
	GUI_SetProperty(COMBOBOX_LANGUAGE, "select", (void*)&value);
	
	value = pmpset_get_int(PMPSET_OUTPUT_MODE);
	output_mode_pmp_2_oem(value);

	video_format_pmp_2_oem(value);

	value = pmpset_get_int(PMPSET_ASPECT_RATIO);
	GUI_SetProperty(COMBOBOX_SCREEN_FORMAT, "select", (void*)&value);

	value = pmpset_get_int(PMPSET_SAVE_TAGS);
	GUI_SetProperty(COMBOBOX_SAVE_TAGS, "select", (void*)&value);

	return 0;
}



SIGNAL_HANDLER int pmp_set_destroy(const char* widgetname, void *usrdata)
{
	
	return 0;
}


static status_t key_left_right(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;
	
	GUI_GetProperty(BOX_SETTING, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 0:
			GUI_GetProperty(COMBOBOX_LANGUAGE, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_LANG, value_sel);
			GUI_SetProperty(WIN_PMP_SET, "update", NULL);
			break;
			
		case 1:
			GUI_GetProperty(COMBOBOX_TVOUT, "select", (void*)&value_sel);
			output_mode_oem_2_pmp(value_sel);
			break;
			
		case 2:
			GUI_GetProperty(COMBOBOX_VIDEO_FORMAT, "select", (void*)&value_sel);
			video_format_oem_2_pmp(value_sel);
			break;

		case 3:
			GUI_GetProperty(COMBOBOX_SCREEN_FORMAT, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_ASPECT_RATIO, value_sel);
			break;
			
		case 4:
			GUI_GetProperty(COMBOBOX_SAVE_TAGS, "select", (void*)&value_sel);
			pmpset_set_int(PMPSET_SAVE_TAGS, value_sel);
			break;
		
		default:
			break;
	}

	printf("[SET] pmp_set_keypress_ok item:%d, value:%d\n", item_sel, value_sel);
		
	return GXCORE_SUCCESS;
}

static status_t _ok_key(void)
{
	uint32_t item_sel = 0;
	uint32_t value_sel = 0;
    PopDlg  pop;
//	media_popmsg_ret ret=0;
	
	GUI_GetProperty(BOX_SETTING, "select", (void*)&item_sel);

	switch(item_sel)
	{
		
		case 5:
            memset(&pop, 0, sizeof(PopDlg));
            pop.type = POP_TYPE_YES_NO;
            pop.str = "Reset to factory?";
			pop.pos.x = POP_MEDIA_X_START;
			pop.pos.y = POP_MEDIA_Y_START;	
//			ret=media_popmsg(pop.pos.x, pop.pos.y, pop.str, MEDIA_POPMSG_TYPE_YESNO);
//			if(ret==MEDIA_POPMSG_RET_YES)

            if(popdlg_create(&pop) == POP_VAL_OK)
			{
				pmpset_set_int(PMPSET_FACTORY_DEFAULT, 0);
				pmp_set_init(NULL, NULL);
				GUI_SetProperty(WIN_PMP_SET, "update", NULL);
			}			
			break;

		default:
			break;
	}

	printf("[SET] pmp_set_keypress_ok item:%d, value:%d\n", item_sel, value_sel);
		
	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int pmp_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
		
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case APPK_OK:
			_ok_key();
			break;
		case APPK_LEFT:
		case APPK_RIGHT:
			key_left_right();
			break;
			
		default:
			break;
	}	
	return EVENT_TRANSFER_KEEPON;
}



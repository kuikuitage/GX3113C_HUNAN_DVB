#include "app.h"


#define PASSWDSET_TEXT_TIME                             "win_passwd_set_time_text"
#define PASSWDSET_BOX                                   "win_passwd_set_box"
#define PASSWDSET_EDIT_OLD_PASSWD                       "win_passwd_set_boxitem1_edit"
#define PASSWDSET_EDIT_NEW_PASSWD                       "win_passwd_set_boxitem2_edit"

#define STR_PASSWD_CHECK           	"Do you want to modify the password?"
#define STR_NEW_PASSED_NO_MODIFIED  "Please input new password!"//"New password no modify!"
#define STR_OLD_PASSED_NO_MODIFIED  "Please input old password!"//"New password no modify!"
#define STR_PASSED_SAME             "Same password,please input new password again!"//"New password no modify!"

#define STR_PASSWD_SUCCESS         	"Modify password success!"
#define STR_PASSWD_FAILURE         	"Modify password failure!"
#define STR_OLD_PASSWD_ERROR     	"Incorrect old password!"


static int passworkOk = 0;

static status_t key_exit(void)
{
    /*check modified?*/

    /*popmsg to save?*/

    /*save*/

    /*not save, reset param*/

    
	return GXCORE_SUCCESS;
}

static status_t key_lr(unsigned short value)
{
	uint32_t item_sel = 0;	
	//uint32_t value_sel = 0;
    
	GUI_GetProperty(PASSWDSET_BOX, "select", (void*)&item_sel);

	switch(item_sel)
	{
		case 1:{
			break;
			}

		case 2:{
			break;
			}


		default:
			break;
	}
		
	return GXCORE_SUCCESS;
}

static status_t key_ok(void)
{
    int32_t old_passwd_invalid = 0;
    char* new_passwd = NULL;
	char* old_passwd = NULL;
	int listview_sel = 0;
	popmsg_ret ret = 0;

	do{
			
	    GUI_GetProperty(PASSWDSET_EDIT_OLD_PASSWD, "string", &old_passwd);
	    GUI_GetProperty(PASSWDSET_EDIT_NEW_PASSWD, "string", &new_passwd);
	    APP_Printf("old[%s],new[%s]\n",old_passwd,new_passwd);

		/*Old password is valid ?*/
	    old_passwd_invalid = app_win_check_password_valid(PASSWDSET_EDIT_OLD_PASSWD, 4);
	    if(old_passwd_invalid)
		{
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_OLD_PASSWD_ERROR, POPMSG_TYPE_OK);

			GUI_SetProperty(PASSWDSET_EDIT_OLD_PASSWD,"clear",NULL);
			break;
		}

		/*New password is inputed ?*/
	    if(strlen((char*)new_passwd)!=4)
	    {
	 	   app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_NEW_PASSED_NO_MODIFIED,POPMSG_TYPE_OK);

		   break;
	    }

        if(!strcmp(old_passwd,new_passwd))
        {
            app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_PASSED_SAME,POPMSG_TYPE_OK);

            GUI_SetProperty(PASSWDSET_EDIT_NEW_PASSWD,"clear",NULL);
            break;
        }
		
		/*Are you sure to change password ?*/
        ret = app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,STR_PASSWD_CHECK, POPMSG_TYPE_YESNO);

        if(POPMSG_RET_YES == ret)
        {
        	app_flash_save_config_password(new_passwd);
			app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_PASSWD_SUCCESS, POPMSG_TYPE_OK);

			GUI_SetProperty(PASSWDSET_EDIT_OLD_PASSWD,"clear",NULL);
			GUI_SetProperty(PASSWDSET_EDIT_NEW_PASSWD,"clear",NULL);

			GUI_SetProperty(PASSWDSET_BOX,"select",(void*)&listview_sel);
			break;
        }
	    else
		{
			GUI_SetProperty(PASSWDSET_EDIT_OLD_PASSWD,"clear",NULL);
			GUI_SetProperty(PASSWDSET_EDIT_NEW_PASSWD,"clear",NULL);

			GUI_SetProperty(PASSWDSET_BOX,"select",(void*)&listview_sel);
			break;		
		}
	}while(0);
	return GXCORE_SUCCESS;
}



SIGNAL_HANDLER  int win_passwd_set_service(const char* widgetname, void *usrdata)
{
	
	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_passwd_set_create(const char* widgetname, void *usrdata)
{	
	char* osd_language=NULL;


    osd_language = app_flash_get_config_osd_language();
    if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
    {
#ifdef HD
    	    GUI_SetProperty("win_passwd_set_title", "img", "title_password.bmp");
    	GUI_SetProperty("win_passwd_set_tip_image_exit", "img", "tips_exit.bmp");
    	GUI_SetProperty("win_passwd_set_tip_image_ok", "img", "tips_confirm.bmp");
#endif
    }
    else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
    {
#ifdef HD
        GUI_SetProperty("win_passwd_set_title", "img", "title_password_e.bmp");
    	GUI_SetProperty("win_passwd_set_tip_image_exit", "img", "tips_exit_e.bmp");
    	GUI_SetProperty("win_passwd_set_tip_image_ok", "img", "tips_confirm_e2.bmp");
#endif
    }


	GUI_SetProperty(PASSWDSET_TEXT_TIME, "string", app_win_get_local_date_string());
	GUI_SetProperty(PASSWDSET_EDIT_OLD_PASSWD,"clear",NULL);
	GUI_SetProperty(PASSWDSET_EDIT_NEW_PASSWD,"clear",NULL);

	return 0;
}

SIGNAL_HANDLER int win_passwd_set_destroy(const char* widgetname, void *usrdata)
{
	passworkOk = 0;
	
	return 0;
}

SIGNAL_HANDLER int win_passwd_set_box_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t item_sel = 0;	
	char* old_passwd = NULL;
       int32_t old_passwd_invalid = 0;


	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
		case KEY_UP:
		case KEY_DOWN:
			GUI_GetProperty(PASSWDSET_BOX, "select", (void*)&item_sel);
			if (0 == item_sel)
				{
				    GUI_GetProperty(PASSWDSET_EDIT_OLD_PASSWD, "string", &old_passwd);

					/*Old password is valid ?*/
				    old_passwd_invalid = app_win_check_password_valid(PASSWDSET_EDIT_OLD_PASSWD, 4);
				    if(old_passwd_invalid)
					{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_OLD_PASSWD_ERROR, POPMSG_TYPE_OK);

                        GUI_SetProperty(PASSWDSET_EDIT_OLD_PASSWD,"clear",NULL);
						return EVENT_TRANSFER_STOP;
					}
				else
				{
					passworkOk = 1;
				}
				
			}
				return EVENT_TRANSFER_KEEPON;
		default:
			{
				uint32_t CurSel = 0;
				char *str = NULL;

				GUI_GetProperty(PASSWDSET_BOX, "select", (void*)&CurSel);
				if (0 == CurSel)
				{
					GUI_GetProperty(PASSWDSET_EDIT_OLD_PASSWD, "string", &str);

					if (1 == passworkOk)
					{
						break;
					}
					
					if (4 == strlen(str))
					{
						event->key.sym = KEY_DOWN;
						GUI_SendEvent(PASSWDSET_BOX, event); 

						return EVENT_TRANSFER_STOP;
					}
				}
			}
			break;
	}

	return EVENT_TRANSFER_KEEPON;
}


SIGNAL_HANDLER int win_passwd_set_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	uint32_t item_sel = 0;	
	char* old_passwd = NULL;
       int32_t old_passwd_invalid = 0;


	
	if(NULL == usrdata) return EVENT_TRANSFER_STOP;

	event = (GUI_Event *)usrdata;
	switch(event->key.sym)
	{
    	case KEY_EXIT:
    	case KEY_RECALL:
        case KEY_MENU:
            key_exit();
	     GUI_EndDialog("win_passwd_set");
    		return EVENT_TRANSFER_STOP;
                        
		case APPK_LEFT:
		case APPK_RIGHT:
			key_lr(event->key.sym);
			break;
		case KEY_UP:
		case KEY_DOWN:
			GUI_GetProperty(PASSWDSET_BOX, "select", (void*)&item_sel);
			if (0 == item_sel)
				{
				    GUI_GetProperty(PASSWDSET_EDIT_OLD_PASSWD, "string", &old_passwd);

					/*Old password is valid ?*/
				    old_passwd_invalid = app_win_check_password_valid(PASSWDSET_EDIT_OLD_PASSWD, 4);
				    if(old_passwd_invalid)
					{
						app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START, STR_OLD_PASSWD_ERROR, POPMSG_TYPE_OK);

						return EVENT_TRANSFER_STOP;
					}
					
				}
				return EVENT_TRANSFER_KEEPON;
		case APPK_OK:
			key_ok();
			break;

        			
		default:
			break;
	}

	
	return EVENT_TRANSFER_STOP;
}



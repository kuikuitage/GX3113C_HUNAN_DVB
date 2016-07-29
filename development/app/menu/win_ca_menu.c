#include "app.h"
#include "win_main_menu.h"

#define CA_BUTTON_ITEM_MAX      20
static int gCaMenuNum;
int CaMenuIndex = 0;
typedef struct ca_button_list2_item_t
{
	char* list2_content;//子菜单
	char* list2_window; //窗体名称(组件)
}ca_button_list2_item;

typedef struct ca_button_list1_t
{
	char* list1_content;//菜单
	ca_button_list2_item list2_item[CA_BUTTON_ITEM_MAX];
}ca_button_list1;

//主界面菜单
ca_button_list1 ca_menu_list =
{"CA Information",
	{
		{"ca info", "win_ca_info"},
	}
};
char* WIDGET_CA_MENU_BUTTON[13] = {
														"win_ca_menu_button1",
														"win_ca_menu_button2",
														"win_ca_menu_button3",
														"win_ca_menu_button4",
														"win_ca_menu_button5",
														"win_ca_menu_button6",
														"win_ca_menu_button7",
														"win_ca_menu_button8",
														"win_ca_menu_button9",
														"win_ca_menu_button10",
														"win_ca_menu_button11",
														"win_ca_menu_button12",
														"win_ca_menu_button13",
									};


static int ca_menu_init(void)
{
	int32_t dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
	switch(dvb_ca_flag)
	{
		case DVB_CA_TYPE_DVB:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_dvb_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Work Time";
			ca_menu_list.list2_item[1].list2_window = "win_dvb_cas_worktime";
			ca_menu_list.list2_item[2].list2_content = "Authorization info";
			ca_menu_list.list2_item[2].list2_window = "win_dvb_cas_operator";
			ca_menu_list.list2_item[3].list2_content = "Modify Pin";
			ca_menu_list.list2_item[3].list2_window = "win_dvb_cas_pin";
			ca_menu_list.list2_item[4].list2_content = "Paired Info";
			ca_menu_list.list2_item[4].list2_window = "win_dvb_cas_stb_pair";
			ca_menu_list.list2_item[5].list2_content = "Watch Level";
			ca_menu_list.list2_item[5].list2_window = "win_dvb_cas_watchlevel";
			ca_menu_list.list2_item[6].list2_content = "Email List";
			ca_menu_list.list2_item[6].list2_window = "win_dvb_cas_email";
			return 7;
		case DVB_CA_TYPE_CDCAS30:
			ca_menu_list.list2_item[0].list2_content = "Base Info";
			ca_menu_list.list2_item[0].list2_window = "win_cd_cas30_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_cd_cas30_entitle";
			ca_menu_list.list2_item[2].list2_content = "Paired Info";
			ca_menu_list.list2_item[2].list2_window = "win_cd_cas30_paried";
			ca_menu_list.list2_item[3].list2_content = "Modify Pin";
			ca_menu_list.list2_item[3].list2_window = "win_cd_cas30_pin";
			ca_menu_list.list2_item[4].list2_content = "Watch Level";
			ca_menu_list.list2_item[4].list2_window = "win_cd_cas30_rating";
			ca_menu_list.list2_item[5].list2_content = "Work Time";
			ca_menu_list.list2_item[5].list2_window = "win_cd_cas30_worktime";
			ca_menu_list.list2_item[6].list2_content = "Features Info";
			ca_menu_list.list2_item[6].list2_window = "win_cd_cas30_features";
			ca_menu_list.list2_item[7].list2_content = "Ippv Slot";
			ca_menu_list.list2_item[7].list2_window = "win_cd_cas30_ippv_slot";
			ca_menu_list.list2_item[8].list2_content = "Ippv Info";
			ca_menu_list.list2_item[8].list2_window = "win_cd_cas30_ippv_info";
			ca_menu_list.list2_item[9].list2_content = "SubMainCard";
			ca_menu_list.list2_item[9].list2_window = "win_cd_cas30_feed";
			ca_menu_list.list2_item[10].list2_content = "Detitle Info";
			ca_menu_list.list2_item[10].list2_window = "win_cd_cas30_detitle";
			ca_menu_list.list2_item[11].list2_content = "Card Update Info";
			ca_menu_list.list2_item[11].list2_window = "win_cd_cas30_cardupdate";
			ca_menu_list.list2_item[12].list2_content = "Email List";
			ca_menu_list.list2_item[12].list2_window = "win_cd_cas30_email";
			return 13;
		case DVB_CA_TYPE_BY:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_by_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_by_cas_entitle";
			ca_menu_list.list2_item[2].list2_content = "Email";
			ca_menu_list.list2_item[2].list2_window = "win_by_cas_email";
			return 3;
		case DVB_CA_TYPE_KN:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_kn_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_kn_cas_entitle";
			return 2;
		case DVB_CA_TYPE_XINSHIMAO:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_xinshimao_cas_baseinfo";
			return 1;
		case DVB_CA_TYPE_DIVITONE:
			ca_menu_list.list2_item[0].list2_content = "Card Info";
			ca_menu_list.list2_item[0].list2_window = "win_divitone_cas_cardinfo";
			ca_menu_list.list2_item[1].list2_content = "Email";
			ca_menu_list.list2_item[1].list2_window = "win_divitone_cas_email";
			ca_menu_list.list2_item[2].list2_content = "Work Time";
			ca_menu_list.list2_item[2].list2_window = "win_divitone_cas_worktime";
			ca_menu_list.list2_item[3].list2_content = "Watch Level";
			ca_menu_list.list2_item[3].list2_window = "win_divitone_cas_watchlevel";
			ca_menu_list.list2_item[4].list2_content = "Set Pin";
			ca_menu_list.list2_item[4].list2_window = "win_divitone_cas_pin";
			ca_menu_list.list2_item[5].list2_content = "Active Child";
			ca_menu_list.list2_item[5].list2_window = "win_divitone_cas_activechild";
			ca_menu_list.list2_item[6].list2_content = "Entitle Info";
			ca_menu_list.list2_item[6].list2_window = "win_divitone_cas_entitleinfo";
			return 7;
		case DVB_CA_TYPE_DSCAS50:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_desai_cas50_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Operator Info";
			ca_menu_list.list2_item[1].list2_window = "win_desai_cas50_operatorinfo";
			ca_menu_list.list2_item[2].list2_content = "Card Set Watchlevel";
			ca_menu_list.list2_item[2].list2_window = "win_desai_cas50_card_setwatchlevel";
			ca_menu_list.list2_item[3].list2_content = "Card Set Worktime";
			ca_menu_list.list2_item[3].list2_window = "win_desai_cas50_card_setworktime";
			ca_menu_list.list2_item[4].list2_content = "Modify Pin";
			ca_menu_list.list2_item[4].list2_window = "win_desai_cas50_changepin";
			ca_menu_list.list2_item[5].list2_content = "Purser Info";
			ca_menu_list.list2_item[5].list2_window = "win_desai_cas50_purser_info";
			ca_menu_list.list2_item[6].list2_content = "Authorization info";
			ca_menu_list.list2_item[6].list2_window = "win_desai_cas50_entitle_info";
			ca_menu_list.list2_item[7].list2_content = "Email";
			ca_menu_list.list2_item[7].list2_window = "win_desai_cas50_email";
			return 8;
		case DVB_CA_TYPE_QILIAN:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_qilian_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_qilian_cas_entitle";
			ca_menu_list.list2_item[2].list2_content = "Email";
			ca_menu_list.list2_item[2].list2_window = "win_qilian_cas_email";
			return 3;
			/*begin:add by chenth,移植迅驰CA*/
		case DVB_CA_TYPE_MG:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_mg_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Features Code";
			ca_menu_list.list2_item[1].list2_window = "win_mg_cas_featurescode";
			ca_menu_list.list2_item[2].list2_content = "SubMainCard";
			ca_menu_list.list2_item[2].list2_window = "win_mg_cas_submaincard";
			ca_menu_list.list2_item[3].list2_content = "Email";
			ca_menu_list.list2_item[3].list2_window = "win_mg_cas_email";
			return 4;
			/*end:add by chenth,移植迅驰CA*/
		case DVB_CA_TYPE_QZ:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_qz_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Set Host Slave";
			ca_menu_list.list2_item[1].list2_window = "win_qz_cas_masterslave";
			ca_menu_list.list2_item[2].list2_content = "Email";
			ca_menu_list.list2_item[2].list2_window = "win_qz_cas_email";
			return 3;
		case DVB_CA_TYPE_GOS:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_gos_cas_baseinfo";
			return 1;
		case DVB_CA_TYPE_GY:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_gy_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_gy_cas_entitle";
			ca_menu_list.list2_item[2].list2_content = "Paired Info";
			ca_menu_list.list2_item[2].list2_window = "win_gy_cas_paried";
			ca_menu_list.list2_item[3].list2_content = "Modify Pin";
			ca_menu_list.list2_item[3].list2_window = "win_gy_cas_pin";
			ca_menu_list.list2_item[4].list2_content = "Watch Level";
			ca_menu_list.list2_item[4].list2_window = "win_gy_cas_rating";
			ca_menu_list.list2_item[5].list2_content = "Email List";
			ca_menu_list.list2_item[5].list2_window = "win_gy_cas_email";
			return 6;
		case DVB_CA_TYPE_WF:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_wf_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization info";
			ca_menu_list.list2_item[1].list2_window = "win_wf_cas_entitle";
			ca_menu_list.list2_item[2].list2_content = "Email";
			ca_menu_list.list2_item[2].list2_window = "win_wf_cas_email";
			return 3;
		case DVB_CA_TYPE_DVT:
			ca_menu_list.list2_item[0].list2_content = "Authorization Information";
			ca_menu_list.list2_item[0].list2_window = "win_dvt_cas_authorize";
			ca_menu_list.list2_item[1].list2_content = "Smartcard Information";
			ca_menu_list.list2_item[1].list2_window = "win_dvt_cas_smart";
			ca_menu_list.list2_item[2].list2_content = "Password Setting";
			ca_menu_list.list2_item[2].list2_window = "win_dvt_cas_pin";
			ca_menu_list.list2_item[3].list2_content = "Lock Setting";
			ca_menu_list.list2_item[3].list2_window = "win_dvt_cas_rating";
			ca_menu_list.list2_item[4].list2_content = "Period Setting";
			ca_menu_list.list2_item[4].list2_window = "win_dvt_cas_worktime";
			ca_menu_list.list2_item[5].list2_content = "Mail Manage";
			ca_menu_list.list2_item[5].list2_window = "win_dvt_cas_email";
			return 6;
		case DVB_CA_TYPE_KP:/*天柏CA*/
			ca_menu_list.list2_item[0].list2_content = "Base Info";
			ca_menu_list.list2_item[0].list2_window = "win_kp_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Authorization Info";
			ca_menu_list.list2_item[1].list2_window = "win_kp_cas_entitle_info";
			ca_menu_list.list2_item[2].list2_content = "Modify Pin";
			ca_menu_list.list2_item[2].list2_window = "win_kp_cas_pin";
			ca_menu_list.list2_item[3].list2_content = "Watch Level";
			ca_menu_list.list2_item[3].list2_window = "win_kp_cas_watchlevel";
			ca_menu_list.list2_item[4].list2_content = "Work Time";
			ca_menu_list.list2_item[4].list2_window = "win_kp_cas_worktime";
			ca_menu_list.list2_item[5].list2_content = "Email";
			ca_menu_list.list2_item[5].list2_window = "win_kp_cas_email";
			//ca_menu_list[].list2_item[6].list2_content = "IPPV Slot";
			//ca_menu_list[].list2_item[6].list2_window = "win_kp_cas_ippv_slot";

			return 6;
			/*wangjian add on 20141217 for MGCAS3.1.2*/
		case DVB_CA_TYPE_MG312:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_mg_cas312_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Features Code";
			ca_menu_list.list2_item[1].list2_window = "win_mg_cas312_featurescode";
			ca_menu_list.list2_item[2].list2_content = "SubMainCard";
			ca_menu_list.list2_item[2].list2_window = "win_mg_cas312_submaincard";
			ca_menu_list.list2_item[3].list2_content = "Email";
			ca_menu_list.list2_item[3].list2_window = "win_mg_cas312_email";
			return 4;
			/*wangjian add end.*/
		case DVB_CA_TYPE_TR:
			ca_menu_list.list2_item[0].list2_content = "Base info";
			ca_menu_list.list2_item[0].list2_window = "win_tr_cas_baseinfo";
			ca_menu_list.list2_item[1].list2_content = "Modify Pin";
			ca_menu_list.list2_item[1].list2_window = "win_tr_cas_pin";
			ca_menu_list.list2_item[2].list2_content = "Watch Level";
			ca_menu_list.list2_item[2].list2_window = "win_tr_cas_rating";
			ca_menu_list.list2_item[3].list2_content = "Authorization info";
			ca_menu_list.list2_item[3].list2_window = "win_tr_cas_entitle";
			ca_menu_list.list2_item[4].list2_content = "IPP Info";
			ca_menu_list.list2_item[4].list2_window = "win_tr_cas_ippv_order";
			ca_menu_list.list2_item[5].list2_content = "Slot Info";
			ca_menu_list.list2_item[5].list2_window = "win_tr_cas_ippv_slot";
			ca_menu_list.list2_item[6].list2_content = "Ipp Bill";
			ca_menu_list.list2_item[6].list2_window = "win_tr_cas_ippv_bill";
			ca_menu_list.list2_item[7].list2_content = "MC-Manage";
			ca_menu_list.list2_item[7].list2_window = "win_tr_cas_mc_mgr";
			ca_menu_list.list2_item[8].list2_content = "Email";
			ca_menu_list.list2_item[8].list2_window = "win_tr_cas_email";
			ca_menu_list.list2_item[9].list2_content = "Announce";
			ca_menu_list.list2_item[9].list2_window = "win_tr_cas_announce_list";
			return 10;
			break;
		default:
			return 0;
			break;
	}

}

static void ca_menu_button_init(int buttonNum)
{
	int i;
	if(buttonNum<0||buttonNum >13)
	{
		return;
	}

	for (i=0;i<buttonNum;i++)
	{
		GUI_SetProperty(WIDGET_CA_MENU_BUTTON[i],"string",ca_menu_list.list2_item[i].list2_content);
		GUI_SetProperty(WIDGET_CA_MENU_BUTTON[i],"state","show");
	}
}
int win_ca_menu_change_button(int oldbuttonindex , int newbuttonindex)
{
	/*
	 * 主菜单未使用控件，控制选中button实现，改变选中、未选中图标
	 */
#ifdef APP_SD
	GUI_SetProperty(WIDGET_CA_MENU_BUTTON[oldbuttonindex], "state", "unfocus");
	GUI_SetProperty(WIDGET_CA_MENU_BUTTON[oldbuttonindex], "state", "show");

	GUI_SetProperty(WIDGET_CA_MENU_BUTTON[newbuttonindex], "state", "focus");
	GUI_SetProperty(WIDGET_CA_MENU_BUTTON[newbuttonindex], "state", "show");
#endif

	CaMenuIndex  = newbuttonindex;
	return 0;

}


SIGNAL_HANDLER int app_ca_info_menu_create(const char* widgetname, void *usrdata)
{
	gCaMenuNum = ca_menu_init();
	ca_menu_button_init(gCaMenuNum);
	CaMenuIndex = 0;
	char* osd_language=NULL;
	osd_language = app_flash_get_config_osd_language();
	if (0 == strcmp(osd_language,LANGUAGE_CHINESE))
	{
	//	GUI_SetProperty("win_ca_info_menu_image_title","img", "DS_CA_INFO_TITLE_CH.bmp");
	}
	else if(0 == strcmp(osd_language,LANGUAGE_ENGLISH))
	{
	//	GUI_SetProperty("win_ca_info_menu_image_title","img", "DS_CA_INFO_TITLE_ENG.bmp");
	}
	GUI_SetProperty(WIDGET_CA_MENU_BUTTON[0], "state", "focus");
	return 0;
}
SIGNAL_HANDLER int app_ca_info_menu_keypress(const char* widgetname, void *usrdata)
{
	GUI_Event *event = NULL;
	event = (GUI_Event *)usrdata;
	int ret = 0;
#ifdef CA_FLAG
	int capopmsg_flag = 0;
	int check_passwdFailed = 0;
#endif

	if(event->type == GUI_KEYDOWN)
	{
		switch(event->key.sym)
		{
			case KEY_RECALL:
			case KEY_MENU:
			case KEY_EXIT:
				GUI_EndDialog("win_ca_menu");
				return EVENT_TRANSFER_STOP;
			case KEY_UP:
				if(CaMenuIndex == 0 || CaMenuIndex==1)
				{
					win_ca_menu_change_button(CaMenuIndex,gCaMenuNum-1);
				}
				else
				{
					win_ca_menu_change_button(CaMenuIndex,CaMenuIndex-2);
				}
				return EVENT_TRANSFER_STOP;
			case KEY_DOWN:
				if( CaMenuIndex == gCaMenuNum-1 ||CaMenuIndex == gCaMenuNum-2)
				{
					win_ca_menu_change_button(CaMenuIndex,0);
				}else
				{
					win_ca_menu_change_button(CaMenuIndex,CaMenuIndex+2);
				}
				return EVENT_TRANSFER_STOP;

			case KEY_LEFT:
				if(CaMenuIndex == 0)
				{
					win_ca_menu_change_button(CaMenuIndex,gCaMenuNum-1);
				}else
				{
					win_ca_menu_change_button(CaMenuIndex,CaMenuIndex-1);
				}
				return EVENT_TRANSFER_STOP;
			case KEY_RIGHT:
				if(CaMenuIndex == gCaMenuNum-1)
				{
					win_ca_menu_change_button(CaMenuIndex,0);
				}else
				{
					win_ca_menu_change_button(CaMenuIndex,CaMenuIndex+1);
				}

				return EVENT_TRANSFER_STOP;
			case KEY_OK:
#ifdef CA_FLAG
				{
				int32_t dvb_ca_flag = app_flash_get_config_dvb_ca_flag();
				switch(dvb_ca_flag)
				{
					case DVB_CA_TYPE_DVB:
						if (strcasecmp("win_dvb_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_dvb_cas_operator", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_OPERATOR_INFO);
						}
						else if (strcasecmp("win_dvb_cas_stb_pair", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							//app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_dvb_cas_worktime", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);
						}
						else if (strcasecmp("win_dvb_cas_pin", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							//app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);
						}
						else if (strcasecmp("win_dvb_cas_watchlevel", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if (strcasecmp("win_dvb_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_CDCAS30:
						if (strcasecmp("win_cd_cas30_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_cd_cas30_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_cd_cas30_paried", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_PAIRED_INFO);
						}
						else if (strcasecmp("win_cd_cas30_pin", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);
						}
						else if (strcasecmp("win_cd_cas30_rating", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if (strcasecmp("win_cd_cas30_worktime", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);
						}
						else if (strcasecmp("win_cd_cas30_features", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_FETURE_INFO);
						}
						else if (strcasecmp("win_cd_cas30_ippv_slot", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO);
						}
						else if (strcasecmp("win_cd_cas30_ippv_info", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_PROG_INFO);
						}
						else if (strcasecmp("win_cd_cas30_feed", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_MOTHER_CARD_INFO);
						}
						else if (strcasecmp("win_cd_cas30_detitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_DETITLE_INFO);
						}
						else if (strcasecmp("win_cd_cas30_update", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CARD_UPDATE_INFO);
						}
						else if (strcasecmp("win_cd_cas30_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_BY:
						if (strcasecmp("win_by_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_by_cas_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_by_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_KN:
						//nonthing todo
						break;
					case DVB_CA_TYPE_XINSHIMAO:
						if (strcasecmp("win_xinshimao_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						break;
					case DVB_CA_TYPE_DIVITONE:
						if (strcasecmp("win_divitone_cas_cardinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_divitone_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						else if (strcasecmp("win_divitone_cas_worktime", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_WORK_TIME_INFO);
						}
						else if (strcasecmp("win_divitone_cas_entitleinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						break;
					case DVB_CA_TYPE_QILIAN:
						printf("CaMenuIndex=%d\n",CaMenuIndex);
						if(ca_menu_list.list2_item[CaMenuIndex].list2_window)
						{
							//printf("list2_windowl=%s\n",ca_menu_list[].list2_item[CaMenuIndex].list2_windowl);
							printf("list2_windowl=%s\n",ca_menu_list.list2_item[CaMenuIndex].list2_window);
						}
						else
							printf("windows null\n");
						if (strcasecmp("win_qilian_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_qilian_cas_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							printf("**win_qilian_cas_entitle**\n");
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_qilian_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_MG:
						if (strcasecmp("Email", ca_menu_list.list2_item[CaMenuIndex].list2_content) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						else
						{
							if(app_cas_api_init_data(DVB_CA_BASE_INFO)<0)
							{
								if(!strcasecmp("Features Code", ca_menu_list.list2_item[CaMenuIndex].list2_content)
										|| !strcasecmp("SubMainCard", ca_menu_list.list2_item[CaMenuIndex].list2_content))
								{
									capopmsg_flag = 1;
								}
							}
						}
						break;

					case DVB_CA_TYPE_QZ:
						{
							if(strcasecmp("Set Host Slave", ca_menu_list.list2_item[CaMenuIndex].list2_content) == 0)
							{
								extern int PasswdCheckOK();
								extern void win_SetName(char *win);
								win_SetName(ca_menu_list.list2_item[CaMenuIndex].list2_window);
								GUI_CreateDialog("win_password_check");
								if(!PasswdCheckOK())
								{
									check_passwdFailed = 1;
								}
							}

						}
						break;


					case DVB_CA_TYPE_GY:
						if (strcasecmp("win_gy_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_gy_cas_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_gy_cas_paried", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_PAIRED_INFO);
						}
						else if (strcasecmp("win_gy_cas_pin", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_CHANGE_PIN_INFO);
						}
						else if (strcasecmp("win_gy_cas_rating", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if (strcasecmp("win_gy_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_WF:
						if (strcasecmp("win_wf_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_wf_cas_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_wf_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_MG312:
						if (strcasecmp("Email", ca_menu_list.list2_item[CaMenuIndex].list2_content) == 0)
						{
							app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						else
						{
							if(app_cas_api_init_data(DVB_CA_BASE_INFO)<0)
							{
								if(!strcasecmp("Features Code", ca_menu_list.list2_item[CaMenuIndex].list2_content)
										|| !strcasecmp("SubMainCard", ca_menu_list.list2_item[CaMenuIndex].list2_content))
								{
									capopmsg_flag = 1;
								}
							}
						}
						break;
					case DVB_CA_TYPE_TR:
						if (strcasecmp("win_tr_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if(strcasecmp("win_tr_cas_rating", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_RATING_INFO);
						}
						else if(strcasecmp("win_tr_cas_entitle", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_order", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_PROG_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_slot", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO);
						}
						else if(strcasecmp("win_tr_cas_ippv_bill", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_DETITLE_INFO);
						}
						else if(strcasecmp("win_tr_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							//app_cas_api_init_data(DVB_CA_EMAIL_INFO);
						}
						break;
					case DVB_CA_TYPE_DVT:
						if (strcasecmp("win_dvt_cas_authorize", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_OPERATOR_INFO))
							{
#ifdef APP_SD
								ret = app_popmsg(210,180,"Access to information error",POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
								ret = app_popmsg(340,200,"Access to information error",POPMSG_TYPE_OK);
#endif
								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_smart", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_dvt_cas_rating", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_RATING_INFO))
							{
#ifdef APP_SD
								ret = app_popmsg(210,180,"Access to information error",POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
								ret = app_popmsg(340,200,"Access to information error",POPMSG_TYPE_OK);
#endif
								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_worktime", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_WORK_TIME_INFO))
							{
#ifdef APP_SD
								ret = app_popmsg(210,180,"Access to information error",POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
								ret = app_popmsg(340,200,"Access to information error",POPMSG_TYPE_OK);
#endif
								return EVENT_TRANSFER_STOP;
							}
						}
						else if (strcasecmp("win_dvt_cas_email", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							if (0 == app_cas_api_init_data(DVB_CA_EMAIL_INFO))
							{
								/*
#ifdef APP_SD
ret = app_popmsg(210,150,"Access to information error",POPMSG_TYPE_OK);
#endif
#ifdef APP_HD
ret = app_popmsg(340,200,"Access to information error",POPMSG_TYPE_OK);
#endif
return EVENT_TRANSFER_STOP;
*/
							}
						}
						break;
					case DVB_CA_TYPE_KP:
						if (strcasecmp("win_kp_cas_baseinfo", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_BASE_INFO);
						}
						else if (strcasecmp("win_kp_cas_entitle_info", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_ENTITLE_INFO);
						}
						else if (strcasecmp("win_kp_cas_ippv_slot", ca_menu_list.list2_item[CaMenuIndex].list2_window) == 0)
						{
							app_cas_api_init_data(DVB_CA_IPPV_SLOT_INFO);
						}
						break;
					default:
						break;
				}
				}
#endif
#ifdef CA_FLAG
				if(capopmsg_flag)
				{
					app_popmsg(POP_DIALOG_X_START, POP_DIALOG_Y_START,"Card not Ready!",POPMSG_TYPE_OK);

				}
				else if(check_passwdFailed)
				{
				}
				else
#endif
				{
					//进入子窗体
					GUI_CreateDialog(ca_menu_list.list2_item[CaMenuIndex].list2_window);
				}
				break;

				return EVENT_TRANSFER_STOP;
			default:
				return EVENT_TRANSFER_KEEPON;
		}
	}

	return EVENT_TRANSFER_KEEPON;
}



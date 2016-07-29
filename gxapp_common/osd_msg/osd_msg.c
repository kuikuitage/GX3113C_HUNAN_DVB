#include "osd_msg.h"
#include "gxcore.h"

osd_msg_t sOsdMsg;

static int _get_index(int  msg_no)
{
	int count = 0;
	int i = 0;

	if(0xff == msg_no)
	{
		return -1;
	}	    

	count = sOsdMsg.list.osd_msg_num;

	for(i = 0; i < count; i++)
	{
		if(sOsdMsg.list.msg[i].msg_no == msg_no)
		{
			return i;
		}		    
	}

	return -1;
}

static bool _select(osd_msg_pp_mode_t pp_mode,osd_msg_process_t* process_msg)
{
	int count = 0;
	int i = 0;
	int select_index = -1;
	int select_no = 0xff;

	if(pp_mode == OSD_MSG_PP_MODE_NONE)
		return false;

	count = sOsdMsg.list.osd_msg_num;
	for(i = 0; i < count; i++)
	{
		if((pp_mode == OSD_MSG_PP_MODE_FULL) ||
		   (pp_mode == OSD_MSG_PP_MODE_SMALL &&sOsdMsg.list.msg[i].content_short != NULL))
		{
			 if(false == sOsdMsg.list.msg[i].flag)
			 {
			 	continue;
			 }
			if(sOsdMsg.list.msg[i].msg_no < select_no)
			{
				select_index = i;
				select_no = sOsdMsg.list.msg[i].msg_no;
			}
		}
			
	}

	/* process timeout msg */
	GxTime time_now = { 0 };
	unsigned int time_now_ms = 0;
	GxCore_GetTickTime(&time_now);
	time_now_ms = time_now.seconds * 1000 + time_now.microsecs / 1000;
	
	for(i = 0; i < count; i++)
	{
		if(true == sOsdMsg.list.msg[i].flag && 0 < sOsdMsg.list.msg[i].disable_time_ms)
		{
			if(sOsdMsg.list.msg[i].disable_time_ms <= time_now_ms)
			{
				 sOsdMsg.list.msg[i].flag = false;
				 sOsdMsg.list.msg[i].disable_time_ms = 0;
			}
		}
	}
	
	if(-1 == select_index)
	{
		return false;
	}        
    
	process_msg->msg_no = sOsdMsg.list.msg[select_index].msg_no;
	if(pp_mode == OSD_MSG_PP_MODE_FULL)
	{
		process_msg->content = sOsdMsg.list.msg[select_index].content_long;  
	}
	else if(pp_mode == OSD_MSG_PP_MODE_SMALL)
	{
		process_msg->content = sOsdMsg.list.msg[select_index].content_short;  
	}

	return true;
}


void  osd_msg_init(void)
{
	memset( sOsdMsg.list.msg, 0, MAX_MSG_COUNT * sizeof(osd_msg_item_t) );
	sOsdMsg.list.osd_msg_num = 0;

	memset(sOsdMsg.display.window, 0, MAX_LEN_WINDOW_NAME);
	sOsdMsg.display.pp_mode = OSD_MSG_PP_MODE_NONE;
	sOsdMsg.display.msg_no = -1;
	sOsdMsg.display.content = NULL;

	return;
}


bool osd_msg_register(osd_msg_register_t* msg)
{
	if(MAX_MSG_COUNT <= sOsdMsg.list.osd_msg_num)
		return false;
	
	sOsdMsg.list.msg[sOsdMsg.list.osd_msg_num].msg_no = msg->msg_no;
	sOsdMsg.list.msg[sOsdMsg.list.osd_msg_num].content_short = msg->content_short;
	sOsdMsg.list.msg[sOsdMsg.list.osd_msg_num].content_long = msg->content_long;
	
	sOsdMsg.list.osd_msg_num++;
	return true;
}
bool osd_msg_update(osd_msg_register_t* msg)
{
    int index=-1;
	//int size;
	index = _get_index(msg->msg_no);
	if(-1 == index) 
		return false;
	else
	{
		///size=strlen(sOsdMsg.list.msg[index].content_short);
		//if(size)
		sprintf(sOsdMsg.list.msg[index].content_short,"%s",msg->content_short);
		//size=strlen(sOsdMsg.list.msg[index].content_long);
		//if(size)
		sprintf(sOsdMsg.list.msg[index].content_long,"%s",msg->content_long);
	}
	if(sOsdMsg.display.msg_no==msg->msg_no)//强制重新画。
		sOsdMsg.display.msg_no=-1;

    return true;
}
bool osd_msg_enable(int msg_no)
{
	int index = -1;

	index = _get_index(msg_no);
	if(-1 == index) 
	{
		return false;
	}

	sOsdMsg.list.msg[index].flag = true;
	sOsdMsg.list.msg[index].disable_time_ms = 0;

	return true;
}

bool osd_msg_enable_timeout(int msg_no, int timeout_ms)
{
	int index = -1;
	
	GxTime time_now = { 0 };
	unsigned int time_now_ms = 0;
	GxCore_GetTickTime(&time_now);
	time_now_ms = time_now.seconds * 1000 + time_now.microsecs / 1000;
	
	index = _get_index(msg_no);
	if(-1 == index) 
	{
		return false;
	}

	sOsdMsg.list.msg[index].flag = true;
	sOsdMsg.list.msg[index].disable_time_ms = time_now_ms + timeout_ms;

	return true;
}

bool osd_msg_disable(int msg_no)
{
	int index = -1;

	index = _get_index(msg_no);
	if(-1 == index) 
	{
		return false;
	}

	sOsdMsg.list.msg[index].flag = false;
	sOsdMsg.list.msg[index].disable_time_ms = 0;
	
	return true;
}

bool osd_msg_process(char* window, osd_msg_pp_mode_t pp_mode, osd_msg_show_cb show_cb, osd_msg_hide_cb hide_cb)
{	
	bool ret = false;
	osd_msg_process_t process_msg;

	if(NULL == window)
	{
		memset(sOsdMsg.display.window, 0, MAX_LEN_WINDOW_NAME);
		sOsdMsg.display.pp_mode = OSD_MSG_PP_MODE_NONE;
		//sOsdMsg.display.msg_no = -1;
		sOsdMsg.display.content = NULL;
		return true;
	}
	
	if(NULL == show_cb || NULL == hide_cb)
		return false;

	ret = _select(pp_mode, &process_msg);
	
	/*SHOW*/
	if(true == ret)
	{
		if(0 == strncmp(sOsdMsg.display.window, window, MAX_LEN_WINDOW_NAME) &&
			sOsdMsg.display.pp_mode == pp_mode)
		{
			if( sOsdMsg.display.msg_no != process_msg.msg_no)
			{
				sOsdMsg.display.msg_no = process_msg.msg_no;
				sOsdMsg.display.content = process_msg.content;
				show_cb(&process_msg);

			}
		}
		else
		{
			strncpy(sOsdMsg.display.window, window, MAX_LEN_WINDOW_NAME);
			sOsdMsg.display.pp_mode = pp_mode;
			sOsdMsg.display.msg_no = process_msg.msg_no;
			sOsdMsg.display.content = process_msg.content;
			show_cb(&process_msg);
		}
	}
	/*HIDE*/
	else
	{
		if(-1 != sOsdMsg.display.msg_no)
		{
			memset(sOsdMsg.display.window, 0, MAX_LEN_WINDOW_NAME);
			sOsdMsg.display.pp_mode = OSD_MSG_PP_MODE_NONE;
			sOsdMsg.display.msg_no = -1;
			sOsdMsg.display.content = NULL;
			hide_cb();
		}
	}

	return true;
}

bool osd_msg_check(int msg_no)
{
	int index = -1;

	index = _get_index(msg_no);
	if(-1 == index) 
		return false;

	return sOsdMsg.list.msg[index].flag;
}

int osd_msg_get_display_msg(void)
{
	int msg_no;
	
	msg_no = sOsdMsg.display.msg_no;
	return msg_no;
}



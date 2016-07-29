/*****************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2009, All right reserved
******************************************************************************

******************************************************************************
* File Name :	app_rss.c
* Author    : 	tony
* Project   :	
* Type      :	
******************************************************************************
* Purpose   :	
******************************************************************************
* Release History:
  VERSION	Date			 AUTHOR         Description
   0.0  	2012.09.25	      	   tony         	  creation
*****************************************************************************/

#include "app.h"
//#include "app_module.h"
//#include "app_msg.h"
//#include "app_send_msg.h"
//#include "full_screen.h"
//#include "app_pop.h"
//#include "app_book.h"
//#include "app_default_params.h"

//#define SUPPORT_NOLIMIT_EMAIL_NUM
#if defined(LINUX_OS)

#define MAX_PER_GET_EMAIL  256 



/**********************************************************************************************************
*													限制说明:
*											1,命令长度不能超过256 个字节。
*											2,路径名不能超过256 个字节。
**********************************************************************************************************/
#define RSS_ERR(express)  {\
							if(!express)\
							printf("%s,%s, %s,%s,%s\n",\
								#express,__FILE__,__FUNCTION__,__LINE__);\
}




#define RSS_DEBUG_SUPPORT
#ifdef RSS_DEBUG_SUPPORT
#define RSS_DEBUG(...)  {\
									printf("[RSS_DEBUG]");\
									printf(__VA_ARGS__);\
}
#else
#define RSS_DEBUG(...) 

#endif


#if 0
typedef enum
{
	BMP_TYPE,
	JPEG_TYPE,
	GIF_TYPE,
	PNG_TYPE,
	FILE_TYPE
}image_type;
#endif
static event_list* lastest_timer=(event_list *)-1;


//#define DEBUG_GUI
#define COMMAND_EXIT(...)	 \
	{ int i;\
		i=system("rss_exit");\
			lastest_timer=(event_list *)-1;\
	}
 
static	char rssflag=0;
static  int first_focus;

void reset_email(void);
void clear_Right_Interface(void);
void exit_email(void);
void enter_email(void);
void redraw_all_class2(void);
void clear_Right_Interface(void);
void hide_Class1_Interface(void);
void start_show_wating(void);
void stop_hide_wating(void);
popmsg_ret pop_rss_msg(char *context,int YesorNo);

static int starti=0;

#define MAX_CLASS_MENU_ITEM   256
#define MAX_STRING_NUM 		  256


#define MAX_EMAIL_TITLE       80

#define MAX_EMAIL_AUTHOR      80


#define MAX_EMAIL_CONTENT     1024
#define MAX_EMAIL_TIME        40



typedef struct string_recode
{
	int index;//暂时不用
    char string[MAX_STRING_NUM];
}Name_T;

typedef struct file_record
{
	int total;
	Name_T *property;
}File_Record_T;



#define MAX_PATH_NUM 256
typedef struct string_recode1
{
	int index;//暂时不用
    char string[MAX_STRING_NUM];
	char picpath[MAX_PATH_NUM];
}Name1_T;

typedef struct file_record1
{
	int total;
	Name1_T *property;
}File1_Record_T;


typedef struct email_recode
{
	int index;//为-1表示没有使用

    char title[MAX_EMAIL_TITLE+1];
	char time[MAX_EMAIL_TIME];
	char author[MAX_EMAIL_AUTHOR];
	char simp_content[MAX_EMAIL_CONTENT+1];	
}Email_property_T;


#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
#define MAX_EMAIL_SAVE_ITEM  256
#else
#define MAX_EMAIL_SAVE_ITEM  1024
#endif

typedef struct Emai_file_record
{
	int total;
	int type;//0:title,time,content.
			 //1:title,content,time
	Email_property_T *property;//>3*line可以保证没有网络的时候还可以看看
}Email_File_Record_T;



File_Record_T   	class0_file_Record;
File1_Record_T  	class1_file_Record;
Email_File_Record_T    email_file_Record;

int in_email;

/*****************************************************************************
	when item>num of page,will be error. this is a incomplete patch,i don't know item loc.
********************************************************************************/
void show_menu0_item_sel(void)
{
	int y,sel;
    
	GUI_GetProperty("wnd_rss_list_menu0", "select", &sel);
	printf("show sel sel22 is %d\n",sel);
	y=122+52*sel;
	if(sel<10)
	{
		GUI_SetProperty("wnd_rss_menu0_sel", "y",&y );
		GUI_SetProperty("wnd_rss_menu0_sel", "state","show");

		GUI_SetProperty("wnd_rss_text_sel", "y",&y);
		GUI_SetProperty("wnd_rss_text_sel", "string",class0_file_Record.property[sel].string);
		//GUI_SetProperty("wnd_rss_text_sel", "state","show");
	}


}
void hide_menu0_item_sel(void)
{
	GUI_SetProperty("wnd_rss_menu0_sel","state","hide");
	GUI_SetProperty("wnd_rss_text_sel" ,"state","hide");
	printf("hide wnd_rss_menu0_sel\n");
}

void enter_rss(void)
{ 
	rssflag=1;
	first_focus=0;
	RSS_DEBUG("tony test!!~~%s %d\n",__FUNCTION__,starti);
	RSS_DEBUG("%s %d\n",__FUNCTION__,starti);
	#ifndef SUPPORT_LANGUAGE_SELECT
	GUI_SetProperty("wnd_rss_image_config", "state","show" );
	GUI_SetProperty("wnd_rss_image_refresh", "state","hide" );
	GUI_SetProperty("wnd_rss_text_refresh", "string","Move" );
	#endif
	in_email=0;
	hide_menu0_item_sel();
	GUI_SetProperty("wnd_rss_text_switch", "string","Enter" );

	

	class0_file_Record.property=(Name_T *)malloc(sizeof(Name_T)*MAX_CLASS_MENU_ITEM);
	if(class0_file_Record.property==NULL)
	{
		RSS_DEBUG("class0 heap size is no enough!\n");
		return;
	}
	else
	{
		RSS_DEBUG("class0 alloc sucuss!\n");
	}
	class1_file_Record.property=(Name1_T *)malloc(sizeof(Name1_T)*MAX_CLASS_MENU_ITEM);
	if(class1_file_Record.property==NULL)
	{
		RSS_DEBUG("class1 heap size is no enough!\n");
		return;
	}
	else
	{
		RSS_DEBUG("class1 alloc sucuss!\n");
	}
	email_file_Record.property=(Email_property_T *)malloc(sizeof(Email_property_T)*MAX_EMAIL_SAVE_ITEM);
	if(email_file_Record.property==NULL)
	{
		RSS_DEBUG("heap size is no enough!\n");
		return;
	}
	else
	{
		RSS_DEBUG("email alloc sucuss!\n");
	}

	reset_email();
	clear_Right_Interface();
	
}
	
void exit_rss(void)
{
	rssflag=0;
	stop_hide_wating();
	COMMAND_EXIT();
	if(email_file_Record.property!=NULL)
	{
		free(email_file_Record.property);
		RSS_DEBUG("email free sucuss!\n");
	}
	if(class1_file_Record.property!=NULL)
	{
		free(class0_file_Record.property);
		RSS_DEBUG("class1 free sucuss!\n");
	}
	if(class0_file_Record.property!=NULL)
	{
		free(class1_file_Record.property);
		RSS_DEBUG("class0 free sucuss!\n");
	}
	return;
}

	
char get_rss_status(void)
{
	return rssflag;
}

typedef void ((*CALL_BACK_FIND_FILE)(char *p));


typedef struct callbackparam{

CALL_BACK_FIND_FILE timeout_file_call_back;
char  filepath[257];
event_list* timer;
int retry_times;
int cur_times;
int inter;


}CALLBACK_PARAM;

/******************************************************************************************************
此函数为超时等待数据模式为非阻塞模式
in :
	timeout:每多少MS 查询一次,
	retry_times:重复几次。
       timeout_file_call_back();如何处理。因为有的文件很大,可能需要用
       轮询方式，所以不能统一在这里处理。
*******************************************************************************************************/

static int timer_rss_count;

int search_file_timer(void *userdata) 
{
		CALLBACK_PARAM *p=(CALLBACK_PARAM *)userdata;
		RSS_DEBUG("%s %x ,%x,%s\n",__FUNCTION__,(int)p,(int)p->timer,p->filepath);
		RSS_DEBUG("%s timer count %d\n",__FUNCTION__,timer_rss_count);
		if(get_rss_status()==0||lastest_timer!=p->timer)
		{
			//RSS_DEBUG("exit release timer%s\n",p->filepath);
			remove_timer(p->timer);
			timer_rss_count--;
			free(p);	
			return 0;


		}
		p->cur_times++;
		if(--(p->retry_times)==0)
	    {
	   		timer_stop(p->timer);
			RSS_DEBUG("REMOVE TIMER %x\n",(int)p->timer);
		    if(0==access(p->filepath, F_OK))
		    {
		        
		    	RSS_DEBUG("find file %s time %d ms\n",p->filepath,p->cur_times*p->inter);
	    		p->timeout_file_call_back(p->filepath);
		    }
			else
			{
				RSS_DEBUG("no find file %s times is %d ms\n",p->filepath,p->cur_times*p->inter);
				p->timeout_file_call_back(NULL);
			}
			RSS_DEBUG("RELEASE TIMER %x\n",(int)p->timer);
			remove_timer(p->timer);
			free(p);	
			timer_rss_count--;
			RSS_DEBUG("RELEASE TIMER %x over\n",(int)p->timer);
		   	
		}
		else
		{
		  	if(0==access(p->filepath, F_OK))
		    {

				timer_stop(p->timer);
				RSS_DEBUG("REMOVE TIMER %x\n",(int)p->timer);
		    	RSS_DEBUG("find file %s time %d ms\n",p->filepath,p->cur_times*p->inter);
				p->timeout_file_call_back(p->filepath);
				RSS_DEBUG("RELEASE TIMER00 %x\n",(int)p->timer);
				remove_timer(p->timer);
				free(p);
				timer_rss_count--;
				RSS_DEBUG("RELEASE TIMER00 %x over\n",(int)p->timer);
				
			}	
			else
			{
				reset_timer(p->timer);
				
			}
		}
		return 0;


}





//退出的时候本来要释放TIMER。目前暂不释放。让其自动释放。
int system_rss(char *cmd,char *filepath,int timeout,int retry_times,CALL_BACK_FIND_FILE timeout_file_call_back)
{
		
		CALLBACK_PARAM *p;
		event_list * timer;
		int i;
		
		char cmd2[258];//256+&+\0
		if(strlen(filepath)>=256)
			return -1;
		p=malloc(sizeof(CALLBACK_PARAM));
		if(p==NULL)
			return -2;
		timer = create_timer(search_file_timer, timeout, p, TIMER_ONCE);
		
		if(timer==NULL)
		{
			free(p);
			return -3;
		}
		timer_stop(timer);
		strcpy(p->filepath,filepath);
		p->timer=timer;
		p->timeout_file_call_back=timeout_file_call_back;
		p->retry_times=retry_times;
		p->cur_times=0;
		p->inter=timeout;

		reset_timer(timer);
		lastest_timer=timer;
		timer_rss_count++;

		snprintf(cmd2,258,"%s%s",cmd,"&");
		RSS_DEBUG("send system invoke %s\n",cmd2);
		i=system(cmd2);


		return 0;
	

}






#define LANGUAGE_CH     "zh"
#define LANGUAGE_EN     "en"



void  get_lange(char **language)
{
	if(1)
	{
		*language=LANGUAGE_EN;
	}
}



void SEND_RSS_CMD(char *Path,char *class0,char *class1,char *item,
int timeout,int retry_times,CALL_BACK_FIND_FILE timeout_file_call_back)
{
    char cmd[257];
	char * language;
	get_lange(&language);
	snprintf(cmd,257,"%s %s %s \"%s\" \"%s\" \"%s\"","rss_get_channel",Path,language,class0,class1,item);
	system_rss(cmd,Path,timeout,retry_times,timeout_file_call_back);
}



#define ClASS0_PATH  "/tmp/rss/class0Rec.txt"
#define ClASS1_PATH  "/tmp/rss/class1Rec.txt"
#define ClASS2_PATH  "/tmp/rss/class2Rec.txt"
#define EMAIL_PATH   "/tmp/rss/email"




/*************************************************************************************
	此结构给class0-2使用
	限制名字和个数不大于256
**************************************************************************************/


int showitem[9]={
0,1,2,3,4,5,6,7,8
};
typedef struct show_class1
{
	int FOCUS_LOC;
	int total_num;
}show_class1_T;

show_class1_T Showstatus;

typedef void ((*Parse_File_call_back)(char *path,char *buf,int length,void *struct_buf));
void  parse_file_save(char *path,void *struct_buf,Parse_File_call_back callback);
void find_Email(int sel,Email_property_T **record);
void get_email_callback(char *path);

void get_class1_callback(char *path);
void redraw_all_class2_no_focus(void);


static event_list * waiting_timer=NULL;


int wating_timer(void *userdata) 
{
	int alu = GX_ALU_ROP_COPY_INVERT;

    RSS_DEBUG("%s %d\n",__FUNCTION__,starti);
	GUI_SetProperty("wnd_rss_waiting", "draw_gif", &alu);
	//reset_timer(waiting_timer);
	return 0;
}


void start_show_wating(void)
{
	int type;
	int alu = GX_ALU_ROP_COPY_INVERT;
	starti++;
	RSS_DEBUG("%s  @@@@!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",__FUNCTION__);

	GUI_SetProperty("wnd_rss_waiting", "init_gif_alu_mode", &alu);
	GUI_SetProperty("wnd_rss_waiting", "load_zoom_img", WORK_PATH"theme/image/youtube/loading.gif");
	GUI_GetProperty("wnd_rss_waiting", "img_type", &type);
	RSS_DEBUG("showwating is %d\n",type);
	if(type==GIF_TYPE)
	GUI_SetProperty("wnd_rss_waiting", "draw_gif", &alu);

	GUI_SetProperty("wnd_rss_waiting", "state", "show");
	if(waiting_timer==NULL)
	{
		waiting_timer=create_timer(wating_timer, 200, 0, TIMER_REPEAT);
	}
	else
	{
		reset_timer(waiting_timer);
		RSS_DEBUG("%sHAVE NO RELEASE TIMER!!\n",__FUNCTION__);


	}
	//reset_timer(waiting_timer);
}

void stop_hide_wating(void)
{
	if(waiting_timer==NULL)
		return;
	GUI_SetProperty("wnd_rss_waiting", "state", "hide");
	starti--;
	RSS_DEBUG("%s!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",__FUNCTION__);
	timer_stop(waiting_timer);
	remove_timer(waiting_timer);
	waiting_timer=NULL;
	
	
}

#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
void get_Index_space(int *index,int current_index,int start_index,int end_index)
{
	int i;
	int loc;

	for(i=0;i<MAX_EMAIL_SAVE_ITEM;i++)//如果已经存在，则更新
	{
		if(email_file_Record.property[i].index==current_index)
		{
		    *index=i;
			return ;

		}
	}
	for(i=0;i<MAX_EMAIL_SAVE_ITEM;i++)//不存在，找空的一个
	{
		if(email_file_Record.property[i].index==-1)
		{
			 *index=i;
			return ;
		}
	}
	loc=0;
	if(i==MAX_EMAIL_SAVE_ITEM)//no find ,delect   最远的部分
	{
		for(i=0;i<MAX_EMAIL_SAVE_ITEM;i++)//不存在，找最远的一个
		{
			if(abs(email_file_Record.property[i].index-start_index)>loc)
			{
				loc=email_file_Record.property[i].index;
			}
		}
		*index=i;
		return ;

	}

	
}
#endif

#define EMAIL_MAX_LINE  1024

void get_file_start_item(int *start_index,char *path)
{
	int i=0;
	int temp=0;
	RSS_DEBUG("%s start %s\n",__FUNCTION__,path);
	if(path==NULL)
	{
		//RSS_ERR((path!=NULL));
	}
	
	while(1)
	{

		
		if(path[i]>0x39||path[i]<0x30)
			i++;
		else  //找到start
			break;
			
	}
	while(1)
	{

		
		if(path[i]>0x39||path[i]<0x30)
			break;
		temp=((path[i]-0x30)+(temp<<8));
		i++;	
	}
	*start_index=temp*MAX_PER_GET_EMAIL;
	RSS_DEBUG("%s over start is %d\n",__FUNCTION__,*start_index);

}

///////目前看起来是GB编码，长度字符一定要兼容ANSI，否则分析出错。
///////是否linux下所以的回车都 是OX0A
void email_parse_File(char *path,char *buf,int length,Email_File_Record_T *struct_buf)
{
    char *pointer_head=buf;
	char *pointer_end=buf;
	int   elment_num=0;
	int   index=0;
	int start_index;
	int index_cur;
	int start_parse=0;
	int type=0;
	RSS_DEBUG("%s len is %d\n",__FUNCTION__,length);
	#if 0
	for(i=0;i<length;i++)
	{
		if(i%8==0)
		RSS_DEBUG("\n");
		RSS_DEBUG(" %x\t",buf[i]);
		
	}
	#endif
/*	while(1)//fenxi item_prio
	{
		length--;
		if(length==0)
		{
			
		   struct_buf->total=0;
		   return; 
		}
	   
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			break;
		}
		 pointer_end++;
	}
	RSS_DEBUG("%s111111 \n",__FUNCTION__);
	while(1)//fenxi houxu 
	{
		length--;
		if(length==0)
		{
				
			 struct_buf->total=0;
			 return; 
		}
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			break;
		}
		pointer_end++;
	}
*/
	RSS_DEBUG("%s\n",__FUNCTION__);

	if(*pointer_end==0x0a)
	{
		RSS_DEBUG("empty file\n");
		return;
	}

	while(1)//fenxi总个数
	{
	    length--;
		if(*pointer_end>0x39||*pointer_end<0x30)
		{
			RSS_DEBUG("length-err");
			return;
		}
		
		elment_num=(*pointer_end-0x30)+elment_num*10;
		
		if(length==0)
		{
			RSS_DEBUG("没有总个数元素!!!\n");
		    struct_buf->total=0;
			return;	
		}
		pointer_end++;
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			length--;
			break;
		}
	}

	RSS_DEBUG("email real num is %d\n",elment_num);
	if(elment_num<EMAIL_MAX_LINE)
		struct_buf->total=elment_num;
    else
		struct_buf->total=EMAIL_MAX_LINE;


	RSS_DEBUG("email save num is %d\n",elment_num);


	while(1)//fenxi类型
	{
	    length--;
		if(*pointer_end>0x39||*pointer_end<0x30)
		{
			RSS_DEBUG("type-err");
			return;
		}
		
		type=(*pointer_end-0x30)+type*10;
		
		if(length==0)
		{
			RSS_DEBUG("没有总个数元素!!!\n");
		    struct_buf->total=0;
			return;	
		}
		pointer_end++;
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			length--;
			break;
		}
	}
	struct_buf->type=type;
	RSS_DEBUG("email type is %d\n",type);
	#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
    start_index=0;
	end_index=4;
	#else
	get_file_start_item(&start_index,path);
	#endif
    index_cur=start_index;
	pointer_head=pointer_end;
	start_parse=0;
	 
	while(1)
	{
		int len;
		length--;
		if(*pointer_end==0X0A)
        {
           
			if(start_parse%3==0)//biaoti
			{   

			#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
			    get_Index_space(&index,index_cur,start_index,end_index);
			#else
				index=index_cur;//
			#endif
				
				len=((pointer_end-pointer_head)>=MAX_EMAIL_TITLE)?(MAX_EMAIL_TITLE-1):(pointer_end-pointer_head);
				memcpy(struct_buf->property[index].title,pointer_head,len);
				struct_buf->property[index].title[len]=0;//添加结束标志
				pointer_head=++pointer_end;
				struct_buf->property[index].index=index_cur++;
				//RSS_DEBUG("title index %d %s\n",index,struct_buf->property[index].title);
			}
			else if(start_parse%3==1)
			{
				if(type==0)
				{
					len=((pointer_end-pointer_head)>=MAX_EMAIL_CONTENT)?(MAX_EMAIL_CONTENT-1):(pointer_end-pointer_head);
					memcpy(struct_buf->property[index].simp_content,pointer_head,len);
					struct_buf->property[index].simp_content[len]=0;//添加结束标志
					pointer_head=++pointer_end;
					//RSS_DEBUG("conternt index %d %s\n",index,struct_buf->property[index].simp_content);
				}
				else if(type==1)
				{
					len=((pointer_end-pointer_head)>=MAX_EMAIL_TIME)?(MAX_EMAIL_TIME-1):(pointer_end-pointer_head);
					memcpy(struct_buf->property[index].time,pointer_head,len);
					struct_buf->property[index].time[len]=0;//添加结束标志
					pointer_head=++pointer_end;
					//RSS_DEBUG("time index %d %s\n",index,struct_buf->property[index].time);
				}
			}
			else if(start_parse%3==2)
			{
				if(type==1)
				{
					len=((pointer_end-pointer_head)>=MAX_EMAIL_CONTENT)?(MAX_EMAIL_CONTENT-1):(pointer_end-pointer_head);
					memcpy(struct_buf->property[index].simp_content,pointer_head,len);
					struct_buf->property[index].simp_content[len]=0;//添加结束标志
					pointer_head=++pointer_end;
					//RSS_DEBUG("conternt index %d %s\n",index,struct_buf->property[index].simp_content);
				}
				else if(type==0)
				{
					len=((pointer_end-pointer_head)>=MAX_EMAIL_TIME)?(MAX_EMAIL_TIME-1):(pointer_end-pointer_head);
					memcpy(struct_buf->property[index].time,pointer_head,len);
					struct_buf->property[index].time[len]=0;//添加结束标志
					pointer_head=++pointer_end;
					//RSS_DEBUG("time index %d %s\n",index,struct_buf->property[index].time);
				}
			}

	
			start_parse++;
			
		}
		else
		{
			pointer_end++;
		}
		if(length==0)
		{
			
			len=((pointer_end-pointer_head)>=MAX_EMAIL_CONTENT)?(MAX_EMAIL_CONTENT-1):(pointer_end-pointer_head);
			if((index+1)!=struct_buf->total)
			{
				RSS_DEBUG("file length compare error FILE give %d,parse %d!!\n",struct_buf->total,index);
				struct_buf->total=index+1;
			}

			if(len==0)
			{

				
				RSS_DEBUG("email 0xoa file over\n");	 
				return;
			}

			memcpy(struct_buf->property[index].simp_content,pointer_head,len);
			struct_buf->property[index].simp_content[len]=0;
			//RSS_DEBUG("%s\n",struct_buf->property[index].simp_content);
			RSS_DEBUG("EMAIL parse file over\n");   
			return;
		}

	}
    
}
void class_parse_File(char *path,char *buf,int length,File_Record_T *struct_buf)
{
    char *pointer_head=buf;
	char *pointer_end=buf;
	int   elment_num=0;
	int   index=0;
	RSS_DEBUG("parse file %x,%x\n",buf[0],buf[1]);
	if(*pointer_end==0x0a)
	{	
		RSS_DEBUG("空文件\n");
		return ;
	}
	while(1)
	{
	    length--;
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			break;
		}
	
		if(*pointer_end>0x39||*pointer_end<0x30)
		{
			RSS_DEBUG("length-err");
			return;
		}
		
		elment_num=(*pointer_end-0x30)+elment_num*10;
		
		if(length==0)
		{
			RSS_DEBUG("没有元素!!!\n");
		    struct_buf->total=0;
			return;	
		}
		pointer_end++;
		
	}
    RSS_DEBUG("elemet num is %d\n",elment_num);
	struct_buf->total=elment_num;
	pointer_head=pointer_end;
	
	while(1)
	{
		int len;
		length--;
		if(*pointer_end==0X0A)
        {
            if(index==MAX_CLASS_MENU_ITEM)
				break;
			len=((pointer_end-pointer_head)>=MAX_STRING_NUM)?(MAX_STRING_NUM-1):(pointer_end-pointer_head);
			memcpy(struct_buf->property[index].string,pointer_head,len);
			struct_buf->property[index].string[len]=0;//添加结束标志
			pointer_head=++pointer_end;
			RSS_DEBUG("%s\n",struct_buf->property[index].string);
			index++;
		}
		else
		{
			pointer_end++;
		}
		if(length==0)
		{
			if(index==MAX_CLASS_MENU_ITEM)
				break;
			
			len=((pointer_end-pointer_head)>=MAX_STRING_NUM)?(MAX_STRING_NUM-1):(pointer_end-pointer_head);
			if(len==0)
			{
				RSS_DEBUG("parse00 file over\n");	 
				return;
			}
			memcpy(struct_buf->property[index].string,pointer_head,len);
			struct_buf->property[index].string[len]=0;
			RSS_DEBUG("%s\n",struct_buf->property[index].string);
			RSS_DEBUG("parse file over\n");   
			return;
		}

	}
    
}


void class_parse_File1(char *path,char *buf,int length,File1_Record_T *struct_buf)
{
    char *pointer_head=buf;
	char *pointer_end=buf;
	int   elment_num=0;
	int   index=0;
	int   parse_time=0;
	RSS_DEBUG("parse file %x,%x\n",buf[0],buf[1]);
	if(*pointer_end==0x0a)
	{	
		RSS_DEBUG("空文件\n");
		return ;
	}
	while(1)
	{
	    length--;
		if(*pointer_end==0x0a)
		{	
			pointer_end++;
			break;
		}
	
		if(*pointer_end>0x39||*pointer_end<0x30)
		{
			RSS_DEBUG("length-err");
			return;
		}
		
		elment_num=(*pointer_end-0x30)+elment_num*10;
		
		if(length==0)
		{
			RSS_DEBUG("没有元素!!!\n");
		    struct_buf->total=0;
			return;	
		}
		pointer_end++;
		
	}
    RSS_DEBUG("elemet num is %d\n",elment_num);
	struct_buf->total=elment_num;
	pointer_head=pointer_end;

	parse_time=0;
	while(1)
	{
		int len;
		length--;
		if(*pointer_end==0X0A)
        {
            if(index==MAX_CLASS_MENU_ITEM)
				break;
			if(parse_time%2==0)
			{
			len=((pointer_end-pointer_head)>=MAX_STRING_NUM)?(MAX_STRING_NUM-1):(pointer_end-pointer_head);
			memcpy(struct_buf->property[index].string,pointer_head,len);
			struct_buf->property[index].string[len]=0;//添加结束标志
			pointer_head=++pointer_end;
			RSS_DEBUG("%s\n",struct_buf->property[index].string);
			
			}
			else
			{
				len=((pointer_end-pointer_head)>=MAX_PATH_NUM)?(MAX_PATH_NUM-1):(pointer_end-pointer_head);
				memcpy(struct_buf->property[index].picpath,pointer_head,len);
				struct_buf->property[index].picpath[len]=0;//添加结束标志
				pointer_head=++pointer_end;
				RSS_DEBUG("%s\n",struct_buf->property[index].picpath);
				index++;
			}
			parse_time++;
		}
		else
		{
			pointer_end++;
		}
		if(length==0)
		{
			 if(index==MAX_CLASS_MENU_ITEM)
				break;
			if(parse_time%2==0&&(pointer_end-pointer_head)!=0)
			{
				RSS_DEBUG("err illeage no pic!!!%d--%x--%x\n",(pointer_end-pointer_head),pointer_end[0],pointer_head[0]);
				return;
			}
			len=((pointer_end-pointer_head)>=MAX_PATH_NUM)?(MAX_PATH_NUM-1):(pointer_end-pointer_head);
			if(len==0)
			{
				RSS_DEBUG("parse00 file over\n");   
				return;
			}

			memcpy(struct_buf->property[index].picpath,pointer_head,len);
			struct_buf->property[index].picpath[len]=0;
			RSS_DEBUG("%s\n",struct_buf->property[index].picpath);
			RSS_DEBUG("parse file over\n");   
			return;
		}

	}
    
}





void find_Email(int sel,Email_property_T **record)
{
    int i;
	*record=NULL;
	for(i=0;i<email_file_Record.total;i++)
	{
		if(sel==email_file_Record.property[i].index)
		{
			//RSS_DEBUG("find_email sel is %d",sel);	
			*record=&email_file_Record.property[i];

		}
	}
}

void reset_email(void)
{
    int i;
	email_file_Record.total=0;
	for(i=0;i<MAX_EMAIL_SAVE_ITEM;i++)
		email_file_Record.property[i].index=-1;

}

#define NUM_PER_PAGE 6

#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
void load_next_pre_page(int page)
{

    char item[100];
	int sel,sel_0,sel_1,sel_2;
	RSS_DEBUG("%s page is %d\n",__FUNCTION__,page);
    GUI_GetProperty("wnd_rss_list_menu0", "select", &sel_0);
	GUI_GetProperty("wnd_rss_list_menu1", "select", &sel_1);
	GUI_GetProperty("wnd_rss_list_menu1", "select", &sel_2);
	if(0==access(EMAIL_NEXT_PATH, F_OK))
		remove(EMAIL_NEXT_PATH);//删除文件，如果原来的还在写可能就会失败。
	if(0==access(EMAIL_PREV_PATH, F_OK))
		remove(EMAIL_PREV_PATH);//删除文件，如果原来的还在写可能就会失败。
	


	if(email_file_Record.total==0)//
	{
		snprintf(item,100,"%d-%d",(1)*NUM_PER_PAGE,(2)*NUM_PER_PAGE-1);//第一个没来大到的时候也发个2，报错就抱也没关系
		SEND_RSS_CMD(EMAIL_NEXT_PATH,class0_file_Record.property[sel_0].string,
		class1_file_Record.property[sel_1].string,class2_file_Record.property[sel_2].string,item,1000,30,get_email_callback);
		return;
	}
	#ifndef DEBUG_GUI
	if(email_file_Record.total<=(page+1)*NUM_PER_PAGE)
		snprintf(item,100,"%d-%d",0,NUM_PER_PAGE-1);
	else	
	    snprintf(item,100,"%d-%d",(page+1)*NUM_PER_PAGE,(page+2)*NUM_PER_PAGE-1);
	RSS_DEBUG("total is %d,item is %s\n",class1_file_Record.total,item);
	SEND_RSS_CMD(EMAIL_NEXT_PATH,class0_file_Record.property[sel_0].string,
	class1_file_Record.property[sel_1].string,class2_file_Record.property[sel_2].string,item,1000,30,get_email_callback);
    if(page)
		snprintf(item,100,"%d-%d",(page-1)*NUM_PER_PAGE,(page)*NUM_PER_PAGE-1);
	else
		snprintf(item,100,"%d-%d",(class1_file_Record.total-1)*NUM_PER_PAGE,(class1_file_Record.total)*NUM_PER_PAGE-1);
	if(page)
	SEND_RSS_CMD(EMAIL_PREV_PATH,class0_file_Record.property[sel_0].string,
		class1_file_Record.property[sel_1].string,class2_file_Record.property[sel_2].string,item,1000,30,get_email_callback);
	
	#endif



}


#endif




void  parse_file_save(char *path,void *struct_buf,Parse_File_call_back callback)
{
	FILE * file;
    unsigned int length;
	char *buf;
	int i;
	if(path==NULL)
	{
		RSS_DEBUG("please check net or frash!!\n");
	}
	else
	{

	   
	   file=fopen(path,"r");
	   
	   if(file==NULL)
	   {
	        
			RSS_DEBUG("file no exist!!\n");
			return;
	   }
	   else
	   {
	        RSS_DEBUG("file 11 exist!!\n");
	   		fseek(file, 0L, SEEK_END);
			length = ftell(file); 
			if(length==0)//空文件
			{
				fclose(file);
				return;
			}	
			fseek(file, 0L, SEEK_SET);
			buf=malloc((length/2+1)*2);//分配偶数个，减少随片
			if(buf==NULL)
			{	
				fclose(file);
				return ;
			}
			i=fread(buf,length,1,file);
			RSS_DEBUG("%s %s %d,%d,%d",__FUNCTION__,path,buf[0],length,buf[length-1]);
			callback(path,buf,length,struct_buf);
		    free(buf);
			fclose(file);
	   }
	}
}
#define STR_ID_CONTINUE  "Net issue or No Data, Refresh?"


void get_email_callback(char *path)
{
	int sel1,sel2;
	Email_property_T  *record;

	RSS_DEBUG("email clallback!!\n");

	if(path)
	{
		parse_file_save(path,(void *)&email_file_Record,(Parse_File_call_back)email_parse_File);

		GUI_GetProperty("wnd_rss_list_email", "select", &sel1);
		RSS_DEBUG("file 222 exist!!%d\n",sel1);
		GUI_SetProperty("wnd_rss_list_email", "update_all", NULL);
		GUI_GetProperty("wnd_rss_list_email", "select", &sel2);
		//SEL当无数据的时候是-1。
		//在更新的时候指针会更新至0。
		RSS_DEBUG("file 333 exist!!%d\n",sel2);
		if(sel1==sel2&&sel1!=-1)
		{
				GUI_GetProperty("wnd_rss_list_email", "select", &sel2);
				GUI_SetProperty("wnd_rss_list_email", "active", &sel2);
				RSS_DEBUG("email changed--%d!!\n",sel2);
			
				#ifdef SUPPORT_NOLIMIT_EMAIL_NUM
			
				 if(sel2%NUM_PER_PAGE==0)
					load_next_pre_page(sel2/NUM_PER_PAGE);
			
				#endif
				
				find_Email(sel2,&record);
				//RSS_DEBUG("email record--%x!!\n",record);
				GUI_SetProperty("wnd_rss_title_sub", "string", " ");
				GUI_SetProperty("wnd_rss_email_content", "string", " ");
				GUI_SetProperty("wnd_rss_email_date", "string", " ");
				if(record==NULL)
				{
					
					return ;
				}
			
				//RSS_DEBUG("email record-!=NULL!!\n");
				GUI_SetProperty("wnd_rss_title_sub", "string", record->title);
				
				if(strlen(record->simp_content)==0)//如果没有标题的话,显示标题
				{
					GUI_SetProperty("wnd_rss_email_content", "string", record->title);
					RSS_DEBUG("email CONTERNT 0!!\n");
				}
				else
				{
					RSS_DEBUG("email CONTERNT !=0!!\n");
					GUI_SetProperty("wnd_rss_email_content", "string", record->simp_content);
				}
				RSS_DEBUG("content is%x %s!!\n",record->simp_content[0],record->simp_content);
				//GUI_SetProperty("wnd_rss_email_content", "state","show" );
				GUI_SetProperty("wnd_rss_email_date", "string", record->time);


		}
		
		stop_hide_wating();

	
		
	}
	else
	{
		stop_hide_wating();

		if( pop_rss_msg(STR_ID_CONTINUE,1) == POPMSG_RET_YES)
		{
			enter_email();
		}
		else
		{
			GUI_SetProperty("wnd_rss_email_content", "string", "Sorry, No data find !Please check network or retry!");
		}


	}
}


void get_class0_callback(char *path)
{
	int sel_0;
    RSS_DEBUG("class0 clallback!!\n");
	stop_hide_wating();
	if(path)
	{
		parse_file_save(path,&class0_file_Record,(Parse_File_call_back)class_parse_File);
		
		if(class0_file_Record.total)
		{
			
			GUI_GetProperty("wnd_rss_list_menu0", "select", &sel_0);
			RSS_DEBUG("sel_o is %d\n",sel_0);
			GUI_SetProperty("wnd_rss_list_menu0", "update_all", NULL);
			//GUI_GetProperty("wnd_rss_list_menu0", "select", &sel_0);
			RSS_DEBUG("sel_o00 is %d\n",sel_0);
			RSS_DEBUG("class0 clallback sel is %d !!!%s\n",sel_0,class0_file_Record.property[0].string);
			{
				//SEND_RSS_CMD(ClASS1_PATH,class0_file_Record.property[sel_0].string,
				//		"","",1000,30,get_class1_callback);
	
			}
			 RSS_DEBUG("class0 clallback sel 22!!!\n");
			
		}
		else
		{
			RSS_DEBUG("%s no valid data!!\n",path);

		}
	}	
	else  //超时没有拿到文件。
	{

			RSS_DEBUG("%s no receive!!\n",path);

	}
}


void init_item(void)
{
	int i;
	for(i=0;i<9;i++)
	showitem[i]=i;
}

void get_class1_callback(char *path)
{
	RSS_DEBUG("class1 clallback!!\n");
	class1_file_Record.total=0;//因为有可能给的文件无有效数据。
	if(path)
	{
		
		parse_file_save(path,(void *)&class1_file_Record,(Parse_File_call_back)class_parse_File1);
		Showstatus.total_num=class1_file_Record.total;
		Showstatus.FOCUS_LOC=0;
	}
	else   //无数据或者网络问题。
	{
		
	}
	
	init_item();
	redraw_all_class2_no_focus();//更新右边界面
	stop_hide_wating();
	
}

char focus_list_or_text;

void show_focus_email_content(void)
{
	int sel;
	if(focus_list_or_text==1)
	{
	   // GUI_SetFocusWidget("wnd_rss_list_menu0");
	    sel=-1;
	    GUI_SetProperty("wnd_rss_list_email", "active", &sel);
		GUI_SetProperty("wnd_rss_list_focus", "state", "hide");
		GUI_SetProperty("wnd_rss_content_focus", "state", "show");
		GUI_SetProperty("wnd_rss_text_switch", "string","List" );
	}
	else
	{
		//GUI_SetFocusWidget("wnd_rss_list_email");
		GUI_GetProperty("wnd_rss_list_email", "select", &sel);
	    GUI_SetProperty("wnd_rss_list_email", "active", &sel);
		GUI_SetProperty("wnd_rss_list_focus", "state", "show");
		GUI_SetProperty("wnd_rss_content_focus", "state", "hide");
		GUI_SetProperty("wnd_rss_text_switch", "string","Detail" );
	}	

}
//1 EMAIL
SIGNAL_HANDLER int app_rss_list_email_keypress(GuiWidget *widget, void *usrdata)
	{
		GUI_Event *event = NULL;
		int ret = EVENT_TRANSFER_KEEPON;
		int value=1;
		event = (GUI_Event *)usrdata;
		if(GUI_KEYDOWN ==  event->type)
		{
			switch(find_virtualkey(event->key.sym))
			{
					
				case STBK_OK:
					if(focus_list_or_text==0)
						focus_list_or_text=1;
					else
						focus_list_or_text=0;
					show_focus_email_content();
					ret = EVENT_TRANSFER_STOP;
					//RSS_DEBUG("enter rss ok!!!!\n");
					break;
				case STBK_RIGHT:
					//RSS_DEBUG("enter rss right!!!!\n");
					if(focus_list_or_text==0)
					{
						
						focus_list_or_text=1;
						show_focus_email_content();
						ret = EVENT_TRANSFER_STOP;
					}
					
					break;
				case STBK_UP:
					if(focus_list_or_text==1)
					{
						GUI_SetProperty("wnd_rss_email_content", "line_up", &value);
						ret = EVENT_TRANSFER_STOP;
					}
					//RSS_DEBUG("enter rss up!!!!\n");
					break;
				case STBK_DOWN:
					if(focus_list_or_text==1)
					{
						GUI_SetProperty("wnd_rss_email_content", "line_down", &value);
						ret = EVENT_TRANSFER_STOP;
					}
					//RSS_DEBUG("enter rss down!!!!\n");
					break;	
				case STBK_LEFT:
				
					//RSS_DEBUG("enter rss left!!!!\n");
					if(focus_list_or_text==1)
					{
						focus_list_or_text=0;
						show_focus_email_content();
						ret = EVENT_TRANSFER_STOP;
					}
					
					break;

				case STBK_MENU:
					exit_email();
					break;

				case KEY_PAGE_UP:
					if(focus_list_or_text==1)
					{
						
						GUI_SetProperty("wnd_rss_email_content", "page_up", &value);
						ret = EVENT_TRANSFER_STOP;

					}
					break;
				case KEY_PAGE_DOWN:
					if(focus_list_or_text==1)
					{
						
						GUI_SetProperty("wnd_rss_email_content", "page_down", &value);
						ret = EVENT_TRANSFER_STOP;
					}
					break;	

				case KEY_BLUE:
					enter_email();//刷新

					break;
		
				default:
				
					break;
			}		
		}
		return ret;
	}




SIGNAL_HANDLER int app_rss_list_email_get_total(GuiWidget *widget, void *usrdata)
{
	//RSS_DEBUG("email get total %d\n",email_file_Record.total);
	return email_file_Record.total;


}


SIGNAL_HANDLER int app_rss_list_email_get_data(GuiWidget *widget, void *usrdata)
{
	int sel;
	ListItemPara* item = NULL;
	//RSS_DEBUG("email get data\n");
	Email_property_T *record=NULL;
	item = (ListItemPara*)usrdata;
	sel=item->sel;
    find_Email(sel,&record);
	
	if(record==NULL)
	{
		
		item->x_offset = 6;
    	item->image = NULL;
		item->string = "";
		item = item->next;



		item->x_offset = 0;
    	item->image = NULL;
		item->string = "";
		item = item->next;

	
		return GXCORE_SUCCESS;
	}

	RSS_DEBUG("find record is  %s %s--sel %d\n",record->time,record->title,sel);
	item->x_offset = 6;
    item->image = NULL;
	item->string = record->title;
	item = item->next;



	item->x_offset = 0;
    item->image = NULL;
	item->string =record->time;
	item = item->next;


	
	return GXCORE_SUCCESS;
}


SIGNAL_HANDLER int app_rss_list_email_change(GuiWidget *widget, void *usrdata)
{
	int sel;
	Email_property_T *record;
	GUI_GetProperty("wnd_rss_list_email", "select", &sel);
	GUI_SetProperty("wnd_rss_list_email", "active", &sel);
	RSS_DEBUG("email changed--%d!!\n",sel);

	#ifdef SUPPORT_NOLIMIT_EMAIL_NUM

	 if(sel%NUM_PER_PAGE==0)
	 	load_next_pre_page(sel/NUM_PER_PAGE);

	#endif
	
	find_Email(sel,&record);
	//RSS_DEBUG("email record--%x!!\n",record);
	GUI_SetProperty("wnd_rss_title_sub", "string", " ");
	GUI_SetProperty("wnd_rss_email_content", "string", " ");
	GUI_SetProperty("wnd_rss_email_date", "string", " ");
	if(record==NULL)
	{
		
		return EVENT_TRANSFER_STOP;
	}

	//RSS_DEBUG("email record-!=NULL!!\n");
	GUI_SetProperty("wnd_rss_title_sub", "string", record->title);
	
	if(strlen(record->simp_content)==0)//如果没有标题的话,显示标题
	{
		GUI_SetProperty("wnd_rss_email_content", "string", record->title);
		RSS_DEBUG("email CONTERNT 0!!\n");
	}
	else
	{
	    RSS_DEBUG("email CONTERNT !=0!!\n");
		GUI_SetProperty("wnd_rss_email_content", "string", record->simp_content);
	}
	RSS_DEBUG("content is%x %s!!\n",record->simp_content[0],record->simp_content);
	//GUI_SetProperty("wnd_rss_email_content", "state","show" );
	GUI_SetProperty("wnd_rss_email_date", "string", record->time);
	return EVENT_TRANSFER_STOP;
}



void  left(void);
void  right(void);
void  up(void);
void  down(void);





/*****************************************************************************
*			定位第一个图片为可以返回主子菜单切换。
                     定位MENU键返回主菜单。
*
*****************************************************************************/


SIGNAL_HANDLER int app_rss_menu0_list_keypress(GuiWidget *widget, void *usrdata)
	{
		GUI_Event *event = NULL;
		int ret = EVENT_TRANSFER_KEEPON;
		static int oldsel;
		event = (GUI_Event *)usrdata;
		if(GUI_KEYDOWN ==  event->type)
		{
			switch(find_virtualkey(event->key.sym))
			{
				case STBK_MENU:
				//	RSS_DEBUG("enter rss exit111!!!!");
					first_focus=0;
				    GUI_SetProperty("wnd_rss_list_menu0", "active", &oldsel);
					redraw_all_class2_no_focus();//更新右边界面
					break;
				case STBK_OK:
					RSS_DEBUG("enter email ok!!!!");
					if(first_focus==1)
					{
						enter_email();
					}
					else if(Showstatus.total_num)
					{
						int sel=-1;
						first_focus=1;
						GUI_GetProperty("wnd_rss_list_menu0", "select", &oldsel);
						GUI_SetProperty("wnd_rss_list_menu0", "active", &sel);
						redraw_all_class2();
												
					}
					break;
				case STBK_LEFT:
					if(Showstatus.total_num)
					{
						if(first_focus==0)
						{
						
						}
						else if(showitem[0]==0&&(Showstatus.FOCUS_LOC==0))//在第一个上
						{
						
							first_focus=0;
							redraw_all_class2_no_focus();//更新右边界面
							GUI_SetProperty("wnd_rss_list_menu0", "active", &oldsel);

						}
						else
						{
							left();
							ret =EVENT_TRANSFER_STOP;
						}
						
					}
					break;
				case STBK_RIGHT:
					RSS_DEBUG("enter menu0 right!!!!");
					if(Showstatus.total_num)
					{
						if(first_focus==0)
						{
							int sel=-1;
							first_focus=1;
							GUI_GetProperty("wnd_rss_list_menu0", "select", &oldsel);
							GUI_SetProperty("wnd_rss_list_menu0", "active", &sel);
							redraw_all_class2();
							
						}
						else
						{
							right();
							ret =EVENT_TRANSFER_STOP;
						}
					}
					RSS_DEBUG("menu0 right\n");
					break;
				case STBK_UP:
					if(Showstatus.total_num)
					{
						if(first_focus==0)
						{
						
						}
						else
						{
							up();
							ret =EVENT_TRANSFER_STOP;
						}
					}
					RSS_DEBUG("menu0 UP\n");
					break;
				case STBK_DOWN:
					if(Showstatus.total_num)
					{
						if(first_focus==0)
						{
						
						}
						else
						{
							down();
							ret =EVENT_TRANSFER_STOP;
						}
					}
					RSS_DEBUG("menu0 UP\n");
					break;
				default:
				
					break;
			}		
		}
		return ret;
	}




SIGNAL_HANDLER int app_rss_menu0_list_get_total(GuiWidget *widget, void *usrdata)
{
	RSS_DEBUG("menu0 get total\n");
    #ifndef DEBUG_GUI
	return class0_file_Record.total;
	#else
	return 100;
	#endif
}
#ifdef DEBUG_GUI
char buf[32];
#endif
SIGNAL_HANDLER int app_rss_menu0_list_get_data(GuiWidget *widget, void *usrdata)
{
	ListItemPara* item = NULL;
	int sel;
	
	item = (ListItemPara*)usrdata;
	sel=item->sel;
	RSS_DEBUG("menu0 get data\n");
	item->x_offset = 0;
	item->image = NULL;
	#ifndef DEBUG_GUI
	item->string=class0_file_Record.property[sel].string;
	#else
	sprintf(buf,"sel is %d",sel);
	RSS_DEBUG(buf,"sel is %d", sel);
	item->string =buf;
	#endif
	item = item->next;

	return GXCORE_SUCCESS;
}

SIGNAL_HANDLER int app_rss_menu0_list_change(GuiWidget *widget, void *usrdata)
{
	int sel;
	//struct timeval tv_begin, tv_end; 
    if(first_focus==1)
    {
    	RSS_DEBUG("ERRR MENU0 CHAGED!!!\n");
		return EVENT_TRANSFER_STOP;
	}
	COMMAND_EXIT();
	GUI_GetProperty("wnd_rss_list_menu0", "select", &sel);
	RSS_DEBUG("menu0 changed--%d!!\n",sel);
    GUI_SetProperty("wnd_rss_list_menu0", "active", &sel);
	clear_Right_Interface();
	start_show_wating();
	class1_file_Record.total=0;//清除这个防止切换过去仍然有。
	#ifndef DEBUG_GUI
	//gettimeofday(&tv_begin, NULL);
	if(class0_file_Record.total>0)
		SEND_RSS_CMD(ClASS1_PATH,class0_file_Record.property[sel].string,"","",50,600,get_class1_callback);
	//gettimeofday(&tv_end, NULL);
	//RSS_DEBUG("timer after is %d us!!\n",1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + tv_end.tv_usec - tv_begin.tv_usec);
	#endif
	return EVENT_TRANSFER_STOP;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//1 



int  verfify_last_page(void)
{
	if(showitem[0]+8>=Showstatus.total_num-1)//最后一页
	{
		return 1;

	}
	return 0;
}
int  verfify_first_page(void)
{
	if(showitem[0]==0)//第一页
	{
		return 1;

	}
	return 0;
}


void  left(void)
{
  int i;
  if(Showstatus.total_num==0)
  	return;
	if((Showstatus.FOCUS_LOC%3)==0)//准备向左翻一页
	{
		if(showitem[0]==0)//第一页
				return ;///do_nothing		


		for(i=0;i<9;i++)
		{
						
			showitem[i]=showitem[i]-9;

		}
		Showstatus.FOCUS_LOC=0;
	}
	else
	{
		Showstatus.FOCUS_LOC-=1;
		RSS_DEBUG("loc is %d",Showstatus.FOCUS_LOC);

	}
	redraw_all_class2();

}


void  right(void)
{
  int i;
  if(Showstatus.total_num==0)
  	return;
  
	if((Showstatus.FOCUS_LOC%3)==2)//准备向右翻一页
	{
		if(showitem[0]+8>=Showstatus.total_num-1)//最后一页
				return ;///do_nothing		


		for(i=0;i<9;i++)
		{
			showitem[i]=showitem[i]+9;

		}
		Showstatus.FOCUS_LOC=0;
	}
	else
	{
		if(showitem[Showstatus.FOCUS_LOC]==(Showstatus.total_num-1))//最后一个。
			return;
		Showstatus.FOCUS_LOC+=1;

	}
	redraw_all_class2();

}


void  up(void)
{

  if(Showstatus.total_num==0)
  	return;
  
	if(Showstatus.FOCUS_LOC<3)
	{
		;//donothing
	}
	else
	{
		Showstatus.FOCUS_LOC-=3;

	}
	
	redraw_all_class2();

}



void  down(void)
{
	
	  if(Showstatus.total_num==0)
		return;
	  
		if(Showstatus.FOCUS_LOC>5)
		{
			;//donothing
		}
		else
		{
			if(showitem[Showstatus.FOCUS_LOC]+3<=Showstatus.total_num-1)
				Showstatus.FOCUS_LOC+=3;
		}
		
		redraw_all_class2();
}







void exit_email(void)
{
    int sel;
	GUI_SetFocusWidget("wnd_rss_list_menu0");
	email_file_Record.total=0;//清空
	hide_menu0_item_sel();
	#ifdef SUPPORT_LANGUAGE_SELECT
	GUI_SetProperty("wnd_rss_image_config", "state","show" );
	GUI_SetProperty("wnd_rss_text_refresh", "string","Config");
	#else
	GUI_SetProperty("wnd_rss_image_config", "state","show" );
	GUI_SetProperty("wnd_rss_text_refresh", "string","Move");
	#endif
	
	GUI_SetProperty("wnd_rss_image_refresh", "state","hide" );
	in_email=0;
	COMMAND_EXIT();
	stop_hide_wating();
	GUI_SetProperty("wnd_rss_title_sub", "string", " ");
	GUI_SetProperty("wnd_rss_email_content", "string", " ");
	GUI_SetProperty("wnd_rss_email_date", "string", " ");
	GUI_SetProperty("wnd_rss_list_email", "update_all", NULL);

	GUI_GetProperty("wnd_rss_list_email", "select", &sel);
	RSS_DEBUG("exit email sel %d",sel);
	GUI_SetProperty("wnd_rss_text_switch", "string","Enter" );
	redraw_all_class2();
}




void enter_email(void)
{
	
	char path[50];
	char page_line[50];
	int pathnum;
	int sel_0;
    
	if(Showstatus.total_num==0)
	   return;
	focus_list_or_text=0;
	in_email=1;
	show_focus_email_content();
	start_show_wating();
	show_menu0_item_sel();
	GUI_SetFocusWidget("wnd_rss_list_email");
	hide_Class1_Interface();
	GUI_SetProperty("wnd_rss_text_switch", "string","Detail" );
	GUI_SetProperty("wnd_rss_image_config", "state","hide" );
	GUI_SetProperty("wnd_rss_image_refresh", "state","show" );
	GUI_SetProperty("wnd_rss_text_refresh", "string","Refresh" );
	GUI_GetProperty("wnd_rss_list_menu0", "select", &sel_0);
	GUI_SetProperty("wnd_rss_email_content", "string"," " );
	GUI_SetProperty("wnd_rss_class1_name", "string", class1_file_Record.property[showitem[Showstatus.FOCUS_LOC]].string);
	pathnum=((EMAIL_MAX_LINE%MAX_PER_GET_EMAIL)==0)?(EMAIL_MAX_LINE/MAX_PER_GET_EMAIL)	\
						:((EMAIL_MAX_LINE/MAX_PER_GET_EMAIL)+1);
	{
		int i=0;
		//for(i=0;i<pathnum;i++)		
	//一次加载所有行，这个时候因为没有拿到行数，所以
	//脚本自己会分析是否和合理 
	//为什么不一次全部发，是为了防止时间太长导致UI显示缓慢。
		{
	
			sprintf(path,"%s_%d",EMAIL_PATH,i);
			sprintf(page_line,"%d",MAX_PER_GET_EMAIL);
			SEND_RSS_CMD(path,class0_file_Record.property[sel_0].string,
			class1_file_Record.property[showitem[Showstatus.FOCUS_LOC]].string,page_line,1000,30,get_email_callback);
	
		}
	}

}

void showPicture(char *path,int loc)
{
	char picname[40];
	int type;
	//char picname_test[80]="WORK_PATH/theme/image/youtube/loading.gif";
	RSS_DEBUG("%s",__FUNCTION__);
	snprintf(picname,40,"%s%d","wnd_rss_class1_",loc);
	RSS_DEBUG("pic name %s path is %s\n",picname,path);

//	while(1)
	//{
	//	if(path[i]==0d)
	//		break;
		//RSS_DEBUG("%x \t",path[i]);
	//	i++;
	//}
	//path[i-1]=0;

	//RSS_DEBUG("\n");
	GUI_SetProperty(picname, "load_zoom_img", path);
	GUI_GetProperty(picname, "img_type", &type);
	RSS_DEBUG("TPPE22 IS %d\n",type);
	if(type==GIF_TYPE)
		GUI_SetProperty(picname, "draw_gif", NULL);

	GUI_SetProperty(picname, "state", "show");
	
	//GUI_SetProperty("wnd_rss_class1_10", "load_img", "/usr/local/rss/logo/nbcsports-logo.png");
	//GUI_SetProperty("wnd_rss_class1_10", "state", "show");
	//GUI_SetProperty("wnd_rss_class1_0", "state", "show");

}
void hidePicture(char *path,int loc)
{
    char picname[40];
	snprintf(picname,40,"%s%d","wnd_rss_class1_",loc);
	GUI_SetProperty(picname, "state", "hide");
}
void showLoc(int Loc)
{
    char *string;
	char *picpath;
	char textname[40];
	string=class1_file_Record.property[showitem[Loc]].string;
	picpath=class1_file_Record.property[showitem[Loc]].picpath;
	snprintf(textname,40,"%s%d","wnd_rss_textclass1_",Loc);
	
	GUI_SetProperty(textname, "state", "show");
	GUI_SetProperty(textname, "string",string);
	showPicture(picpath,Loc);
}
void hideLoc(int Loc)
{

   char *string;
   char *picpath;
   char textname[40];
   string=class1_file_Record.property[showitem[Loc]].string;
   picpath=class1_file_Record.property[showitem[Loc]].picpath;
   snprintf(textname,40,"%s%d","wnd_rss_textclass1_",Loc);
   GUI_SetProperty(textname, "state", "hide");
   GUI_SetProperty(textname, "string","");
   hidePicture(picpath,Loc);
}


// 每次分类1 切换的时候肯定要将右边的画面归零。然后用等待符号等待新的数据归来。
void clear_Right_Interface(void)
{
	Showstatus.total_num=0;
	redraw_all_class2_no_focus();
}

void hide_Class1_Interface(void)
{
	GUI_SetProperty("wnd_rss_image_right", "state", "hide");
	GUI_SetProperty("wnd_rss_image_left",  "state", "hide");

	GUI_SetProperty("wnd_rss_imagefocus0", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus1", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus2", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus3", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus4", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus5", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus6", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus7", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus8", "state", "hide");



	GUI_SetProperty("wnd_rss_class1_0", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_1", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_2", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_3", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_4", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_5", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_6", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_7", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_8", "state", "hide");



	GUI_SetProperty("wnd_rss_textclass1_0", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_1", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_2", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_3", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_4", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_5", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_6", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_7", "state", "hide");
	GUI_SetProperty("wnd_rss_textclass1_8", "state", "hide");

	GUI_SetProperty("wnd_rss_class2bg", "state", "hide");
	GUI_SetProperty("wnd_rss_class1_page", "state", "hide");

	
}



void redraw_all_class2_no_focus(void)
{
	int i=0;
	char buf[40];
	GUI_SetProperty("wnd_rss_class2bg", "state", "show");
	GUI_SetProperty("wnd_rss_class1_page", "state", "show");
	GUI_SetProperty("wnd_rss_class1_page", "string","" );

	GUI_SetProperty("wnd_rss_image_right", "state", "hide");
	GUI_SetProperty("wnd_rss_image_left",  "state", "hide");

	GUI_SetProperty("wnd_rss_imagefocus0", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus1", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus2", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus3", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus4", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus5", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus6", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus7", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus8", "state", "hide");


	if(Showstatus.total_num==0)//一个数据都没有，清除右边的界面
	{
		for(i=0;i<9;i++)
			hideLoc(i);
		return;
	}
	if(!verfify_last_page())
	{
		for(i=0;i<9;i++)
		{
			showLoc(i);
		}
	}
	else
	{	
		int num;
		num=Showstatus.total_num-showitem[0];
		for(i=0;i<num;i++)
		{
			showLoc(i);
		}
		for(i=num;i<9;i++)
		{

			hideLoc(i);
		}

	}
	snprintf(buf,40,"[%d/%d]",showitem[Showstatus.FOCUS_LOC]+1,Showstatus.total_num);
	GUI_SetProperty("wnd_rss_class1_page", "string",buf );
	
}



void redraw_all_class2(void)
{
	int i=0;
	char buf[40];
	GUI_SetProperty("wnd_rss_class2bg", "state", "show");
	GUI_SetProperty("wnd_rss_class1_page", "state", "show");
	//GUI_SetProperty("wnd_rss_class1_page", "string","" );
	if(verfify_last_page())
	{
		GUI_SetProperty("wnd_rss_image_right", "state", "hide");
	}
	else
	{
		GUI_SetProperty("wnd_rss_image_right", "state", "show");
	}
	if(verfify_first_page())
	{
		GUI_SetProperty("wnd_rss_image_left", "state", "hide");
	}
	else
	{

		GUI_SetProperty("wnd_rss_image_left", "state", "show");
	}

	GUI_SetProperty("wnd_rss_imagefocus0", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus1", "state", "hide");	
	GUI_SetProperty("wnd_rss_imagefocus2", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus3", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus4", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus5", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus6", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus7", "state", "hide");
	GUI_SetProperty("wnd_rss_imagefocus8", "state", "hide");
	
    switch(Showstatus.FOCUS_LOC)
	{

		case 0:
			RSS_DEBUG("show 0 pic\n");
			GUI_SetProperty("wnd_rss_imagefocus0", "state", "show");	
			break;
		case 1:
			RSS_DEBUG("show 1 pic\n");
			GUI_SetProperty("wnd_rss_imagefocus1", "state", "show");	
			break;
		case 2:
			RSS_DEBUG("show 2 pic\n");
			GUI_SetProperty("wnd_rss_imagefocus2", "state", "show");	
			break;
		case 3:
			GUI_SetProperty("wnd_rss_imagefocus3", "state", "show");	
			break;

		case 4:
			GUI_SetProperty("wnd_rss_imagefocus4", "state", "show");	
			break;
		case 5:
			GUI_SetProperty("wnd_rss_imagefocus5", "state", "show");	
			break;
		case 6:
			GUI_SetProperty("wnd_rss_imagefocus6", "state", "show");	
			break;
		case 7:
			GUI_SetProperty("wnd_rss_imagefocus7", "state", "show");	
			break;
		case 8:
			GUI_SetProperty("wnd_rss_imagefocus8", "state", "show");	
			break;

	}

	//GUI_Exec();//防止载入图片速度快过其他显示，推出时候很怪。
	GUI_SetInterface("flush",NULL);
	if(!verfify_last_page())
	{
		for(i=0;i<9;i++)
		{
			showLoc(i);
		}
	}
	else
	{	
		int num;
		num=Showstatus.total_num-showitem[0];
		for(i=0;i<num;i++)
		{
			showLoc(i);
		}
		for(i=num;i<9;i++)
		{

			hideLoc(i);
		}

	}
	snprintf(buf,40,"[%d/%d]",showitem[Showstatus.FOCUS_LOC]+1,Showstatus.total_num);
	RSS_DEBUG("page buf is %s",buf);
	GUI_SetProperty("wnd_rss_class1_page", "string",buf );
	
}




SIGNAL_HANDLER int app_rss_create(GuiWidget *widget, void *usrdata)
{
    int sel=0;
	int sel_0;
	RSS_DEBUG("rss_creat!!!!tony test \n");
	enter_rss();
	class0_file_Record.total=0;
	GUI_SetProperty("wnd_rss_list_menu0", "update_all", NULL);//将listview指针复位为-1。否则可能再次进入的时候不能激活changed.
	#ifndef DEBUG_GUI
	SEND_RSS_CMD(ClASS0_PATH,"","","",50,600,get_class0_callback);
	#endif

	RSS_DEBUG("enter rss creathah!!!!\n");
	
	GUI_SetProperty("wnd_rss_list_menu0", "select", &sel);
	GUI_SetProperty("wnd_rss_list_menu1", "select", &sel);
	GUI_SetProperty("wnd_rss_list_menu2", "select", &sel);
	

	GUI_SetProperty("wnd_rss_list_menu0", "active", &sel);
	GUI_SetProperty("wnd_rss_list_menu1", "active", &sel);
    GUI_SetProperty("wnd_rss_list_menu2", "active", &sel);

	GUI_SetFocusWidget("wnd_rss_list_menu0");
	GUI_GetProperty("wnd_rss_list_menu0", "select", &sel_0);
	RSS_DEBUG("sel0333 is %d\n",sel_0);
	RSS_DEBUG("enter rss creathah!!!!\n");
	//GUI_Exec();//防止载入图片速度快过其他显示，推出时候很怪。
	GUI_SetInterface("flush",NULL);
	start_show_wating();

   	return EVENT_TRANSFER_STOP;
}

SIGNAL_HANDLER int app_rss_destroy(GuiWidget *widget, void *usrdata)
{
	exit_rss();

	return EVENT_TRANSFER_STOP;
}
#define STR_ID_ENSURE_EXIT  "Do you want to Exit Rss?"

SIGNAL_HANDLER int app_rss_keypress(GuiWidget *widget, void *usrdata)
{
	GUI_Event *event = NULL;
	int ret = EVENT_TRANSFER_STOP;	
	event = (GUI_Event *)usrdata;
	if(GUI_KEYDOWN ==  event->type)
	{
		switch(find_virtualkey(event->key.sym))
		{
			//case STBK_MENU:
			case STBK_EXIT:
				
				//popmsg_result=popmsg(600, 270, STR_ID_ENSURE_EXIT, POPMSG_TYPE_YESNO);
			
				RSS_DEBUG("exit keypress\n");
				#if 0
				if(in_email)
				{
					if(	pop_rss_msg(STR_ID_ENSURE_EXIT,0) == POPMSG_RET_YES)
					{
						GUI_EndDialog("wnd_rss");
					}
				}
				else
			    #endif
				{

					GUI_EndDialog("wnd_rss");

				}
				break;
				
			case STBK_OK:
				
				break;

			case STBK_LEFT:
				break;
			case STBK_RIGHT:
				
				break;

#ifdef RADIO_REC_BOOK
			case STBK_TV_RADIO:
				break;	
#endif			
			default:
			
				break;
		}		
	}
	return ret;
}
#endif




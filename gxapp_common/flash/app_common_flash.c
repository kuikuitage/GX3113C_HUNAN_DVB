/**
 *
 * @file        app_common_flash.c
 * @brief
 * @version     1.1.0
 * @date        10/17/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gxcore.h>
#include <assert.h>
#include "gxoem.h"
#include "app_common_flash.h"
#include "app_common_prog.h"
#include <sys/ioctl.h>
#include "fcntl.h"
#include "gxapp_sys_config.h"
#include "app_common_porting_stb_api.h"
#if DVB_MEDIA_FLAG
#include "pmp_setting.h"
#endif

#ifdef CA_FLAG
#ifdef DVB_CA_TYPE_CD_CAS30_FLAG
#include "CDCASS.h"
#endif
#endif

#ifdef ECOS_OS
extern void cyg_scheduler_lock(void);
extern void cyg_scheduler_unlock(void);


//---------------------------------------
#define IO_GET_CONFIG_FLASH_ERASE               0x600
/*IO_GET_CONFIG_FLASH_ERASE*/
struct flash_erase {
    uint32_t    offset;
    size_t      len;
    int         flasherr;
    uint32_t    err_address;
};
#endif



#define OEM_SOFTWARE_SECTION                    ("software")
#define OEM_UPDATE_SECTION                      ("update")
#define OEM_UPDATE_FLAG                         ("flag")
#define OEM_UPDATE_TYPE                         ("file_source")
#define OEM_UPDATE_DMX_PID                      ("dmx_pid")
#define OEM_UPDATE_DMX_TABLEID                      ("dmx_tableid")
#define OEM_UPDATE_DMX_SOURCE                      ("dmx_source")
#define OEM_UPDATE_DMX_FREQUENCY                      ("fe_frequency")
#define OEM_UPDATE_REPEAT_TIMES                      ("ota_repeat_times")
#define OEM_UPDATE_FE_MODULATION                      ("fe_modulation")
#define OEM_UPDATE_FE_SYMBOLRATE                      ("fe_symbolrate")
#define OEM_UPDATE_FE_WORKMODE                      ("fe_wokemode")
#define OEM_UPDATE_FE_TUNER_TYPE                      ("fe_tuner_type")
#define OEM_UPDATE_FE_DEMOD_TYPE                      ("fe_demod_type")
#define OEM_UPDATE_DMX_SERVICEID                    ("dmx_serviceid")  

#define OEM_SYSTEM_SECTION			            ("system")
#define OEM_DOWNLOADER                          ("downloader_version")
#define OEM_MANUFACTURE_ID                      ("manufacture_id")
#define OEM_PLATFORM_ID                             ("platform_Id")
#define OEM_HARDWARE_VERSION                    ("hardware_version")
#define OEM_SERIAL_NUMBER                       ("serial_number")

#define OEM_APP_VERSION                         ("application_version")
#define OEM_APP_UPDATE_VERSION                         ("application_update_version")
#define OEM_BOOTLOADER_VERSION                         ("bootloader_version")

static flash_config_default g_config_default = {0};

#if defined(LINUX_OS)
#include <sys/mman.h>
#define MTD_INFORMATON_PATH	 "/proc/mtd"
#define LINE_MAX_LENGTH		128
#define NAME_MAX_LENGTH 	32
#define MAXMTDCOUNT		    16 //最大mtd数量

#define FLASH_BASEADDRESS         (0xa0600000UL)
#define CMDLINE_PATH			   "/proc/cmdline"                                   
  
typedef struct MtdDevice_tag
{
	int32_t		Fd;	//文件描述符
	int32_t		Index;
	uint32_t	StartAddress;	//该mtd设备对应的起始地址
	uint32_t	Size;	//该mtd 设备的大小
	char	    DeviceName[NAME_MAX_LENGTH];	//设备名称
	char	    Name[NAME_MAX_LENGTH];			//通用名称

}MtdDevice;
//flash设备的数据结构
typedef struct FlashDevice_tag
{
	uint32_t	BaseAddress;	//flash设备的起始地址
	uint32_t	SectorSize;		//flash设备扇区的大小
	uint32_t	MtdCount;		//flash设备拥有的mtd个数
	MtdDevice   MtdDevice[MAXMTDCOUNT];		//mtd设备数组
}FlashDevice;
static int IsMinifs()
{
	FILE *fp = NULL;
	char *ps8Str=NULL;
	char as8Line[128];
	int s32Ret =0;
    static int isMinifs_flag = -1;

    if(isMinifs_flag>=0)
    {
        return isMinifs_flag;
    }
    
	if (NULL == (fp = fopen(CMDLINE_PATH, "r")))
	{
		printf("open %s failed\n",CMDLINE_PATH);
		goto fail;
	}

	while(fgets(as8Line, sizeof(as8Line), fp))
	{
		if((ps8Str=strstr(as8Line, "minifs")) != NULL)
		{
			s32Ret = 1;
			break;
		}
	}

	fclose(fp);
    isMinifs_flag = s32Ret;
    //printf("#####isMinifs_flag:%d\n",isMinifs_flag);
	return isMinifs_flag;

fail:
	return -1;
}


static int GetMtdInfo(FlashDevice *pDev)
{
    FILE *fp = NULL;
    char line[LINE_MAX_LENGTH];

    int32_t end = 0;
    int32_t tmp = 0;
    char tmp_mtdsize[24];
    char tmp_mtdname[24];
    char tmp_sectsize[24];
    char tmp_name[24];
    int32_t MtdStartAddress = FLASH_BASEADDRESS;
    
    if(NULL == pDev)
    {
        return -1;
    }

    if (NULL == (fp = fopen(MTD_INFORMATON_PATH, "r")))
    {
        printf("Read /proc/mtd failed\n");
        return -1;
    }
    
    memset(pDev,0,sizeof(FlashDevice));
    while(fgets(line, sizeof(line), fp))
    {
        //printf("%s\n", line);
        if (strstr(line, "mtd"))
        {
            if (pDev->MtdCount >= MAXMTDCOUNT - 1)
            {
                printf("Parrtion count > %d error\n",MAXMTDCOUNT);
                pDev->MtdCount = 0;
                break;
            }
            //从文件中读取设备名，设备大小，扇区大小，名称(以字节为单位)
            sscanf(line, "%s %s %s %s", tmp_mtdname, tmp_mtdsize, tmp_sectsize, tmp_name);
        
            end = strlen(tmp_mtdname) - 1;

            while (tmp_mtdname[end] != 'd')
            {
                end--;
            }

            tmp = end +1;
            pDev->MtdDevice[pDev->MtdCount].Index = atoi(&tmp_mtdname[tmp]);
            if(IsMinifs())
            {
                sprintf(pDev->MtdDevice[pDev->MtdCount].DeviceName
                            , "/dev/mtd%d", pDev->MtdDevice[pDev->MtdCount].Index);
            }
            else
            {
                
                //格式化设备名如/dev/mtdblock
                sprintf(pDev->MtdDevice[pDev->MtdCount].DeviceName
                            , "/dev/mtdblock%d", pDev->MtdDevice[pDev->MtdCount].Index);
                //printf("%s\n",pDev->MtdDevice[pDev->MtdCount].DeviceName);
            }
            
            pDev->MtdDevice[pDev->MtdCount].StartAddress = MtdStartAddress;
            //格式化设备名称
            strcpy(pDev->MtdDevice[pDev->MtdCount].Name, tmp_name);
            MtdStartAddress += strtol(tmp_mtdsize, NULL, 16);
            if (!pDev->SectorSize)
            {
                pDev->SectorSize = strtol(tmp_sectsize, NULL, 16);
                //printf("Flash sector size : 0x%x\n", pDev->SectorSize);
            }
            pDev->MtdDevice[pDev->MtdCount].Size = strtol(tmp_mtdsize, NULL, 16);
            /*
            printf("mtd%d: %s, start: %08x, size: 0x%x, name : %s\n"
                , pDev->MtdDevice[pDev->MtdCount].Index
                , pDev->MtdDevice[pDev->MtdCount].DeviceName
                , pDev->MtdDevice[pDev->MtdCount].StartAddress
                , pDev->MtdDevice[pDev->MtdCount].Size
                , pDev->MtdDevice[pDev->MtdCount].Name);
            */
            pDev->MtdCount ++;
        }
    }
    
    fclose(fp);
    if (0 == pDev->MtdCount)
    {
        return -1;
    }
    
    //pDev->MtdDevice[pDev->MtdCount].StartAddress = MtdStartAddress;

    return 0;
}

static MtdDevice * FindMtdDev_ByName(FlashDevice *pDev,const char *Name)
{
    MtdDevice *pmtd = NULL;
    int i;
    if(NULL == pDev || NULL == Name)
    {
        return NULL;
    }
    for(i = 0;i <pDev->MtdCount;i++)
    {
        pmtd = &pDev->MtdDevice[i];
        if(!strcmp(pmtd->Name,Name) 
           ||strstr(pmtd->Name,Name))
        {
            return pmtd;
        }
    }
    
    return NULL;
}

/*
* linux oem read ,write interface
*/

/*
* copy xxx.ini to /tmp/xxx.INI
*/
char oem_buf[64]={0};
int32_t app_flash_linux_partion_init(const char* partion_name)
{
	char  sizebuf[16]={0};
	char  namebuf[64]={0};
	FILE * fp; 
	int res; 
	char arg[256]={0}; 
	char buf[64]={0};
	
	if (NULL == partion_name)
		return -1;
	sprintf(arg,"cat /proc/mtd | busybox grep %s | awk '{print $2}'",partion_name);

	/*
	* read oem partion size
	*/
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
	 	printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
 	{
		 while(fgets(buf, sizeof(buf), fp)) 
		{ 
			printf("%s", buf); 
		}
		buf[strlen(buf)-1]='\0';
		 
		if ( (res = pclose(fp)) == -1) 
		{ 
			printf("close popen file pointer fp error!\n"); 
			return res;
		 }

		if (0 == strlen(buf))
			{
				printf("%s size no exist!\n",partion_name); 
				return -1;			
			}
		
		sprintf(sizebuf,"0x%s",buf);
	}

	memset(buf,0,sizeof(buf));
	memset(arg,0,sizeof(arg));

	/*
	* read partion name
	*/
	sprintf(arg,"cat /proc/mtd | busybox grep  %s | awk '{print $1}' | sed 's/://'",partion_name);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{
			 while(fgets(buf, sizeof(buf), fp)) 
			{ 
				printf("%s", buf); 
			}
			buf[strlen(buf)-1]='\0';
			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 

			if (0 == strlen(buf))
				{
					printf("%s dev: no exist!\n",partion_name); 
					return -1;			
				}

			sprintf(namebuf,"/dev/%s",buf);
		}

	memset(buf,0,sizeof(buf));
	memset(arg,0,sizeof(arg));

	/*
	* copy xxx.ini /tmp/xxx.INI
	*/
	//拷贝文件
	sprintf(arg,"mtd_debug read %s 0 %s /tmp/%s.INI",namebuf,sizebuf,partion_name);
    printf("namebuf = %s\n",namebuf);
    printf("sizebuf = %s\n",sizebuf);
    printf("partion_name = %s\n",partion_name);
    printf("arg = %s\n",arg);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}
	sprintf(arg,"sed -i 's/\r//g' /tmp/%s.INI",partion_name);
	printf("arg = %s\n",arg);
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}
	

	if ((fp = popen("sync", "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}
	
	return 0;
}

int32_t app_flash_linux_partion_save(const char* partion_name)
{
	char  sizebuf[16]={0};
	char  namebuf[64]={0};
	FILE * fp; 
	int res; 
	char arg[256]={0}; 
	char buf[64]={0}; 

	memset(buf,0,sizeof(buf));
	memset(arg,0,sizeof(arg));	
	sprintf(arg,"cat /proc/mtd | busybox grep %s | awk '{print $2}'",partion_name);
	printf("arg = %s\n",arg);

	/*
	* read oem partion size
	*/
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
	 	printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
 	{
		 while(fgets(buf, sizeof(buf), fp)) 
		{ 
			printf("%s", buf); 
		}
		buf[strlen(buf)-1]='\0';
		 
		if ( (res = pclose(fp)) == -1) 
		{ 
			printf("close popen file pointer fp error!\n"); 
			return res;
		 }

		if (0 == strlen(buf))
			{
				printf("%s size no exist!\n",partion_name); 
				return -1;			
			}

		sprintf(sizebuf,"0x%s",buf);
	}

	memset(buf,0,sizeof(buf));
	memset(arg,0,sizeof(arg));

	/*
	* read partion name
	*/
	sprintf(arg,"cat /proc/mtd | busybox grep  %s | awk '{print $1}' | sed 's/://'",partion_name);
	printf("arg = %s\n",arg);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{
			 while(fgets(buf, sizeof(buf), fp)) 
			{ 
				printf("%s", buf); 
			}
			buf[strlen(buf)-1]='\0';
			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 

			if (0 == strlen(buf))
				{
					printf("%s dev: no exist!\n",partion_name); 
					return -1;			
				}

	
			sprintf(namebuf,"/dev/%s",buf);
		}

	memset(buf,0,sizeof(buf));
	memset(arg,0,sizeof(arg));




    printf("namebuf = %s\n",namebuf);
    printf("sizebuf = %s\n",sizebuf);
    printf("partion_name = %s\n",partion_name);	

	/*
	* copy  /tmp/xxx.INI  xxx.ini
	*/
	sprintf(arg,"flash_eraseall %s",namebuf);
    printf("arg = %s\n",arg);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}
	
	sprintf(arg,"dd if=/tmp/%s.INI  of=/tmp/%s.INI.bak bs=64k skip=0 count=1 && sync"
, partion_name, partion_name);
	printf("arg = %s\n",arg);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}  

	sprintf(arg,"flashcp  /tmp/%s.INI.bak %s",partion_name,namebuf);
    printf("arg = %s\n",arg);
	
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}  
	return 0;
}


char* app_flash_linux_oem_read(const char* partion_name,const char* section, const char* parameter)
{
	FILE * fp; 
	int res; 
	char arg[256]={0}; 
	char buf[64]={0};

	if ((NULL == partion_name)||(NULL == section)||(NULL == parameter))
		return NULL;

	sprintf(arg,"sed '%s%s%s,$!d;/^%s=/!d;s///' /tmp/%s.INI","/^\\[",section,"\\]/",parameter,partion_name);
//	printf("arg = %s\n",arg);
	/*
	* read oem partion size
	*/
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
	 	printf("popen error: %s/n", strerror(errno)); 
		return NULL; 
	} 
	else
 	{
		 while(fgets(buf, sizeof(buf), fp)) 
		{ 
			printf("%s", buf); 
		}
		buf[strlen(buf)-1]='\0';
		 
		if ( (res = pclose(fp)) == -1) 
		{ 
			printf("close popen file pointer fp error!\n"); 
			return NULL;
		 }

		if (0 == strlen(buf))
			{
				printf("%s %s %s read NULL!\n",partion_name,section, parameter); 
				return NULL;			
			}

	}
	memset(oem_buf,0,sizeof(oem_buf));
	strcpy(oem_buf,buf);
	
	return oem_buf;
}

	
 int app_flash_linux_oem_write(const char* partion_name,const char* section, const char* parameter, const char* value)
{
	FILE * fp; 
	int res; 
	char arg[256]={0}; 

	if ((NULL == partion_name)||(NULL == section)||(NULL == parameter)||(NULL == value))
		return -1;

	sprintf(arg,"sed -i '/^\\[%s\\]/,/^\\[/ {/^\\[%s\\]/b;/^\\[/b;s/^%s*=.*/%s=%s/g;}' /tmp/%s.INI",section,section,parameter,parameter,value,partion_name);
	printf("arg = %s\n",arg);
	/*
	* write oem partion size
	*/
	if ((fp = popen(arg, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
	{	 
		if ( (res = pclose(fp)) == -1) 
		{ 
			printf("close popen file pointer fp error!\n"); 
			return -1;
		 }
	}

	if ((fp = popen("sync", "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
		{			 
			if ( (res = pclose(fp)) == -1) 
			{ 
				printf("close popen file pointer fp error!\n"); 
				return res;
			 } 
		}
	
	return 0;
}
#endif

/*
*   oem
*/

/*
* 获取bootloader 版本字符串
*/
char * app_flash_get_oem_bootversion_str(void)
{
	char* str=NULL;
#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_SOFTWARE_SECTION, OEM_BOOTLOADER_VERSION);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char bootversion[32]={0};
	str = app_flash_linux_oem_read("V_OEM",OEM_SOFTWARE_SECTION,OEM_BOOTLOADER_VERSION);
	if (NULL == str)
		return NULL;
	memset(bootversion,0,sizeof(bootversion));
	strcpy(bootversion,str);
	return bootversion;
#endif
	return str;
}

/*
* 获取软件 版本字符串
*/
char * app_flash_get_oem_softversion_str(void)
{
	char* str=NULL;

#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_SOFTWARE_SECTION, OEM_APP_VERSION);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char softversion[32]={0};
	memset(softversion,0,sizeof(softversion));
	str = app_flash_linux_oem_read("V_OEM",OEM_SOFTWARE_SECTION,OEM_APP_VERSION);
	if (NULL == str)
		return NULL;
	strcpy(softversion,str);
	return softversion;
#endif
	return str;
}

/*
* 获取区域 版本值
*/
char* app_flash_get_oem_regionversion_str(void)
{
	char* str;
#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_SOFTWARE_SECTION, OEM_APP_VERSION);
	if (str == NULL)
	{
		return 0;
	}
#elif defined(LINUX_OS)
	static char regionversion[32]={0};
	memset(regionversion,0,sizeof(regionversion));
	str = app_flash_linux_oem_read("V_OEM",OEM_SOFTWARE_SECTION,OEM_APP_VERSION);
	if (NULL == str)
		return NULL;
	strcpy(regionversion,str);
	return regionversion;
#endif

	return str;
}


/*
* 获取平台ID,该ID值为芯片厂家分配的ID
*/
uint32_t app_flash_get_oem_platform_id(void)
{
    char* str;

#if defined(ECOS_OS)
    str = GxOem_GetValue(OEM_SYSTEM_SECTION, OEM_PLATFORM_ID);
#elif defined(LINUX_OS)
    //str = app_flash_linux_oem_read("I_OEM",OEM_SYSTEM_SECTION,OEM_PLATFORM_ID);
	str = app_flash_linux_oem_read("V_OEM",OEM_SYSTEM_SECTION,OEM_PLATFORM_ID);
#endif

	if (str == NULL)
	{
		return 0;
	}

    return atoi(str);
}

/*
* 获取厂家ID值
*/
char* app_flash_get_oem_manufacture_id(void)
{

		char* str=NULL;
	
	
#if defined(ECOS_OS)
		str = GxOem_GetValue(OEM_SYSTEM_SECTION, OEM_MANUFACTURE_ID);
		if (str == NULL)
		{
			return NULL;
		}
#elif defined(LINUX_OS)
		static char manufacture[16]={0};
		memset(manufacture,0,sizeof(manufacture));
		//str = app_flash_linux_oem_read("I_OEM",OEM_SYSTEM_SECTION,OEM_MANUFACTURE_ID);
		str = app_flash_linux_oem_read("V_OEM",OEM_SYSTEM_SECTION,OEM_MANUFACTURE_ID);
		if (NULL == str)
			return NULL;
		strcpy(manufacture,str);
		return manufacture;
#endif	
		return str;

}

/*
* 获取硬件版本字符串xx.xx.xx.xx
*/
char *  app_flash_get_oem_hardware_version_str(void)
{
    char* str=NULL;


#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_SYSTEM_SECTION, OEM_HARDWARE_VERSION);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char hardwareversion[32]={0};
	memset(hardwareversion,0,sizeof(hardwareversion));
	//str = app_flash_linux_oem_read("I_OEM",OEM_SYSTEM_SECTION,OEM_HARDWARE_VERSION);
	str = app_flash_linux_oem_read("V_OEM",OEM_SYSTEM_SECTION,OEM_HARDWARE_VERSION);
	if (NULL == str)
		return NULL;
	strcpy(hardwareversion,str);
	return hardwareversion;
#endif

	
	return str;
}

/*
* 获取OEM序号字符串
*/
uint32_t app_flash_get_oem_serial_number(void)
{
    char* str;

#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_SYSTEM_SECTION, OEM_SERIAL_NUMBER);
#elif defined(LINUX_OS)
	//str = app_flash_linux_oem_read("I_OEM",OEM_SYSTEM_SECTION,OEM_HARDWARE_VERSION);
	str = app_flash_linux_oem_read("V_OEM",OEM_SYSTEM_SECTION,OEM_HARDWARE_VERSION);
#endif

	if (str == NULL)
	{
		return 0;
	}

    return atoi(str);
}

/*
* get demod type
*/
char *  app_flash_get_oem_fe_demod_type(void)
{
    char* str=NULL;


#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_DEMOD_TYPE);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char fedemodtype[32]={0};
	memset(fedemodtype,0,sizeof(fedemodtype));
	str = app_flash_linux_oem_read("V_OEM",OEM_UPDATE_SECTION, OEM_UPDATE_FE_DEMOD_TYPE);
	if (NULL == str)
		return NULL;
	strcpy(fedemodtype,str);
	return fedemodtype;
#endif

	
	return str;
}

/*
* get demux src
*/
char * app_flash_get_oem_demux_source(void)
{
	char* str=NULL;

#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_UPDATE_SECTION, OEM_UPDATE_DMX_SOURCE);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char demuxsource[32]={0};
	memset(demuxsource,0,sizeof(demuxsource));
	str = app_flash_linux_oem_read("V_OEM",OEM_UPDATE_SECTION, OEM_UPDATE_DMX_SOURCE);
	if (NULL == str)
		return NULL;
	strcpy(demuxsource,str);
	return demuxsource;
#endif

	
	return str;

}

/*
* get fe tuner type
*/
uint32_t app_flash_get_oem_fe_tuner_type(void)
{
	char* str=NULL;


#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_TUNER_TYPE);
#elif defined(LINUX_OS)
	str = app_flash_linux_oem_read("V_OEM",OEM_UPDATE_SECTION, OEM_UPDATE_FE_TUNER_TYPE);
#endif
	if (str == NULL)
	{
		return 0;
	}

    return atoi(str);

}



/*
* 保存升级PID、频点等OEM到FLASH
*/
void app_flash_save_oem(void)
{
#if defined(ECOS_OS)
	GxOem_Save();
#elif defined(LINUX_OS)
         app_flash_linux_partion_save("V_OEM");
#endif
	return ;
}

/*
* 设置升级描述子中分析到的升级软件版本xx.xx.xx.xx
* 保存需调用app_flash_save_oem()
*/
uint32_t app_flash_set_oem_soft_updateversion(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_SOFTWARE_SECTION, OEM_APP_UPDATE_VERSION, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_SOFTWARE_SECTION,OEM_APP_UPDATE_VERSION,Value);	
#endif
	
	return 0;
}

/*
* 设置升级描述子中分析到的升级PID
*/
uint32_t app_flash_set_oem_dmx_pid(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_DMX_PID, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_DMX_PID,Value);	
#endif	
	return 0;
}

/*
* 设置升级描述子中分析到的升级TABLEID
*/
uint32_t app_flash_set_oem_dmx_tableid(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_DMX_TABLEID, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_DMX_TABLEID,Value);	
#endif		
	return 0;
}

/*
* set demux source 
*/
uint32_t app_flash_set_oem_dmx_source(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_DMX_SOURCE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_DMX_SOURCE,Value);	
#endif		
	return 0;
}

/*
* set fe demod type 
*/
uint32_t app_flash_set_oem_fe_demod_type(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_DEMOD_TYPE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FE_DEMOD_TYPE,Value);	
#endif		
	return 0;
}

/*
 *设置升级描述子中分析到的serviceId
 */
uint32_t app_flash_set_oem_dmx_serviceId(char* Value)
{              
	if(NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION,OEM_UPDATE_DMX_SERVICEID,Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_DMX_SERVICEID,Value);
#endif
	return 0;
}              


/*
* 设置升级描述子中分析到的升级频率
*/
uint32_t app_flash_set_oem_dmx_frequency(char* Value)
{
	if (NULL == Value)
		return 0;
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_DMX_FREQUENCY, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_DMX_FREQUENCY,Value);	
#endif	
	return 0;
}

/*
* 设置升级描述子中分析到的允许重复升级最大失败次数
*/
uint32_t app_flash_set_oem_repeat_times(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_REPEAT_TIMES, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_REPEAT_TIMES,Value);	
#endif	

	
	return 0;
}

/*
* 设置升级描述子中分析到的调制方式
*/
uint32_t app_flash_set_oem_fe_modulation(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_MODULATION, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FE_MODULATION,Value);	
#endif	

	
	return 0;
}

/*
* 设置升级描述子中分析到的符号率
*/
uint32_t app_flash_set_oem_fe_symbolrate(char* Value)
{
	if (NULL == Value)
		return 0;
	
#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_SYMBOLRATE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FE_SYMBOLRATE,Value);	
#endif

	return 0;
}

/*
* 设置升级到的解调模式
*/
uint32_t app_flash_set_oem_fe_workmode(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_WORKMODE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FE_WORKMODE,Value);	
#endif

	
	return 0;
}

/*
* set tuner type
*/
uint32_t app_flash_set_oem_fe_tuner_type(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FE_TUNER_TYPE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FE_TUNER_TYPE,Value); 
#endif
	
	return 0;
}
/*
* 设置升级的方式
* Value - "usb" or "ts"
*/
uint32_t app_flash_set_oem_ota_update_type(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_TYPE, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_TYPE,Value); 
#endif

	
	return 0;
}

/*
* 设置是否需要升级的标志
* Value - "yes" or "no"
*/
uint32_t app_flash_set_oem_ota_flag(char* Value)
{
	if (NULL == Value)
		return 0;

#if defined(ECOS_OS)
	GxOem_SetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FLAG, Value);
#elif defined(LINUX_OS)
	app_flash_linux_oem_write("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FLAG,Value); 
#endif

	
	return 0;
}

/*
* 获取是否升级标志
*/
char *  app_flash_get_oem_ota_flag(void)
{
    char* str;


#if defined(ECOS_OS)
	str = GxOem_GetValue(OEM_UPDATE_SECTION, OEM_UPDATE_FLAG);
	if (str == NULL)
	{
		return NULL;
	}
#elif defined(LINUX_OS)
	static char otaflag[32]={0};
	str = app_flash_linux_oem_read("V_OEM",OEM_UPDATE_SECTION,OEM_UPDATE_FLAG);
	if (NULL == str)
		return NULL;
	memset(otaflag,0,sizeof(otaflag));
	strcpy(otaflag,str);
	return otaflag;
#endif

	return str;
}

/*
* config
*/

/*
* 开机设置默认参数
*/
int32_t app_flash_set_default_config_para(flash_config_default config_default)
{
	memcpy(&g_config_default,&config_default,sizeof(flash_config_default));
	g_config_default.default_password = config_default.default_password;
	g_config_default.default_osd_lang = config_default.default_osd_lang;
	g_config_default.default_subtitle_lang = config_default.default_subtitle_lang;
	g_config_default.default_teltext_lang = config_default.default_teltext_lang;
	return 0;
}

/*
* 保存默认参数
*/
int32_t app_flash_save_default_config_para(void)
{
	/*
	* 可修改/设置的参数，恢复出厂设置为默认值
	*/
	Video_Display_Screen_t display_screen;
	int32_t para = 0;
	
	display_screen = app_flash_get_config_video_display_screen();
	if (display_screen != g_config_default.default_video_display_screen)
		{
			app_flash_save_config_video_display_screen(g_config_default.default_video_display_screen);
		}
	
	para = app_flash_get_config_video_hdmi_mode();
	if (para != g_config_default.default_video_hdmi_mode)
		{
			app_flash_save_config_video_hdmi_mode(g_config_default.default_video_hdmi_mode);
		}
	
	para = app_flash_get_config_video_aspect();
	if (para != g_config_default.default_video_aspect)
	{
		app_flash_save_config_video_aspect(g_config_default.default_video_aspect);
	}

	para = app_flash_get_config_video_quiet_switch();
	if (para != g_config_default.default_video_quiet_switch)
	{
		app_flash_save_config_video_quiet_switch(g_config_default.default_video_quiet_switch);
	}

	para = app_flash_get_config_video_auto_adapt();
	if (para != g_config_default.default_video_quiet_switch)
	{
		app_flash_save_config_video_auto_adapt(g_config_default.default_video_auto_adapt);
	}

	para = app_flash_get_config_volumn_globle_flag();
	if (para != g_config_default.default_volumn_globle_flag)
	{
		app_flash_save_config_volumn_globle_flag(g_config_default.default_volumn_globle_flag);
	}

	para = app_flash_get_config_audio_volume();
	if (para != g_config_default.default_audio_volume)
	{
		app_flash_save_config_audio_volume(g_config_default.default_audio_volume);
	}

	para = app_flash_get_config_track_globle_flag();
	if (para != g_config_default.default_track_globle_flag)
	{
		app_flash_save_config_track_globle_flag(g_config_default.default_track_globle_flag);
	}

	para = app_flash_get_config_audio_track();
	if (para != g_config_default.default_audio_track)
	{
		app_flash_save_config_audio_track(g_config_default.default_audio_track);
	}

	para = app_flash_get_config_audio_audio_track();
	if (para != g_config_default.default_audio_audio_track)
	{
		app_flash_save_config_audio_audio_track(g_config_default.default_audio_audio_track);
	}	

	para = app_flash_get_config_audio_ac3_bypass();
	if (para != g_config_default.default_audio_ac3_bypass)
	{
		app_flash_save_config_audio_ac3_bypass(g_config_default.default_audio_ac3_bypass);
	}		

	para = app_flash_get_config_osd_trans();
	if (para != g_config_default.default_osd_trans)
	{
		app_flash_save_config_osd_trans(g_config_default.default_osd_trans);
	}

	para = app_flash_get_config_videocolor_brightness();
	if (para != g_config_default.default_videocolor_brightness)
	{
		app_flash_save_config_videocolor_brightness(g_config_default.default_videocolor_brightness);
	}

	para = app_flash_get_config_videocolor_saturation();
	if (para != g_config_default.default_videocolor_saturation)
	{
		app_flash_save_config_videocolor_saturation(g_config_default.default_videocolor_saturation);
	}	

	para = app_flash_get_config_videocolor_contrast();
	if (para != g_config_default.default_videocolor_contrast)
	{
		app_flash_save_config_videocolor_contrast(g_config_default.default_videocolor_contrast);
	}

	para = app_flash_get_config_bar_time();
	if (para != g_config_default.default_bar_time)
	{
		app_flash_save_config_bar_time(g_config_default.default_bar_time);
	}	

	para = app_flash_get_config_lcn_flag();
	if (para != g_config_default.default_lcn)
	{
		app_flash_save_config_lcn_flag(g_config_default.default_lcn);
	}	

	para = app_flash_get_config_timezone();
	if (para != g_config_default.default_timezone)
	{
		app_flash_save_config_timezone(g_config_default.default_timezone);
	}	

	para = app_flash_get_config_center_nit_fre_version();
	if (para != g_config_default.default_main_fre_nit_version)
	{
		app_flash_save_config_center_nit_fre_version(g_config_default.default_main_fre_nit_version);
	}	
#ifndef CUST_SHANXI
	para = app_flash_get_config_center_freq1();
	if (para != g_config_default.default_centre_fre1)
	{
		app_flash_save_config_center_freq1(g_config_default.default_centre_fre1);
	}
#endif
	para = app_flash_get_config_center_freq_symbol_rate1();
	if (para != g_config_default.default_centre_sym1)
	{
		app_flash_save_config_center_freq_symbol_rate1(g_config_default.default_centre_sym1);
	}

	para = app_flash_get_config_center_freq_qam1();
	if (para != g_config_default.default_centre_qam1)
	{
		app_flash_save_config_center_freq_qam1(g_config_default.default_centre_qam1);
	}	

#ifndef CUST_SHANXI
	para = app_flash_get_config_center_freq();
	if (para != g_config_default.default_centre_fre)
	{
		app_flash_save_config_center_freq(g_config_default.default_centre_fre);
	}	
#endif
	para = app_flash_get_config_center_freq_symbol_rate();
	if (para != g_config_default.default_centre_sym)
	{
		app_flash_save_config_center_freq_symbol_rate(g_config_default.default_centre_sym);
	}

	para = app_flash_get_config_center_freq_qam();
	if (para != g_config_default.default_centre_qam)
	{
		app_flash_save_config_center_freq_qam(g_config_default.default_centre_qam);
	}	

#ifndef CUST_SHANXI
	para = app_flash_get_config_manual_search_freq();
	if (para != g_config_default.default_manual_fre)
	{
		app_flash_save_config_manual_search_freq(g_config_default.default_manual_fre);
	}	
#endif
	para = app_flash_get_config_manual_search_symbol_rate();
	if (para != g_config_default.default_manual_sym)
	{
		app_flash_save_config_manual_search_symbol_rate(g_config_default.default_manual_sym);
	}

	para = app_flash_get_config_manual_search_qam();
	if (para != g_config_default.default_manual_qam)
	{
		app_flash_save_config_manual_search_qam(g_config_default.default_manual_qam);
	}

	para = app_flash_get_config_manual_search_bandwidth();
	if (para != g_config_default.default_manual_bandwidth)
	{
		app_flash_save_config_manual_search_bandwidth(g_config_default.default_manual_bandwidth);
	}

	para = app_flash_get_config_dtmb_dvbc_switch();
	if (para != g_config_default.default_dtmb_dvbc_switch)
	{
		app_flash_save_config_dtmb_dvbc_switch(g_config_default.default_dtmb_dvbc_switch);
	}

	para = app_flash_get_config_mute_flag();
	if (para != g_config_default.default_mute_flag)
	{
		app_flash_save_config_mute_flag(g_config_default.default_mute_flag);
	}

	para = app_flash_get_config_gx150x_mode_flag();
	if (para != g_config_default.default_dtmb_flag)
	{
		app_flash_save_config_gx150x_mode_flag(g_config_default.default_dtmb_flag);
	}	

/*	para = app_flash_get_config_dvb_ca_flag();
	if (para != g_config_default.default_dvb_ca)
	{
		app_flash_save_config_dvb_ca_flag(g_config_default.default_dvb_ca);
	}*/	

	para = app_flash_get_config_password_flag();
	if (para != g_config_default.default_password_flag)
	{
		app_flash_save_config_password_flag(g_config_default.default_password_flag);
	}

	para = app_get_fm_freq();
	if (para != g_config_default.default_fm_freq)
	{
		app_set_fm_freq(g_config_default.default_fm_freq);
	}	

	para = app_get_manual_search_flag();
	if (para != g_config_default.default_manualsearch_flag)
	{
		app_set_manual_search_flag(g_config_default.default_manualsearch_flag);
	}
	
	app_flash_save_config_password(g_config_default.default_password);
	app_flash_save_config_osd_language(g_config_default.default_osd_lang);
#if MEDIA_SUBTITLE_SUPPORT
	app_flash_save_config_subtitle_language(g_config_default.default_subtitle_lang);
	app_flash_save_config_teltext_language(g_config_default.default_teltext_lang);
#endif
	para = app_flash_get_config_pvrduration();
	if (para != g_config_default.default_pvr_duration)
	{
		app_flash_save_config_pvrduration(g_config_default.default_pvr_duration);
	}
#if (DVB_DEFINITION_TYPE == SD_DEFINITION)
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_INTERFACE, VIDEO_OUTPUT_RCA);
	GxBus_ConfigSetInt(PLAYER_CONFIG_VIDEO_RESOLUTION_RCA, VIDEO_OUTPUT_PAL);
#endif

	/*
	* 多媒体参数
	*/
#if DVB_MEDIA_FLAG
	pmpset_factory_default();
#endif

	/*
	* 其他需要特殊恢复的参数
	*/
	return 0;
}



/*
* 获取时区参数
*/
int32_t app_flash_get_config_timezone(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(TIMEZONE,&config,g_config_default.default_timezone);
	return config;
}

/*
* 设置时区参数
* config -- 时区值
*/
int32_t app_flash_save_config_timezone(int32_t config)
{
	GxBus_ConfigSetInt(TIMEZONE,config);
	return 0;
}

/*
* 获取是否使用lcn标志
*/
Lcn_State_t app_flash_get_config_lcn_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(LCN,&config,g_config_default.default_lcn);
	return config;
}

/*
* 设置是否使用lcn标志
* config -- 0
*/
int32_t app_flash_save_config_lcn_flag(Lcn_State_t config)
{
	int32_t lcn_flag = config;
	GxBus_ConfigSetInt(LCN,lcn_flag);
	return 0;
}

/*
* 获取是否移动私有排序*/
int32_t app_flash_get_user_list_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(USER_LIST,&config,g_config_default.default_user_list);
	return config;
}

#if MEDIA_SUBTITLE_SUPPORT
/*
* 获取teltext 语言类型
*/
char*  app_flash_get_config_teltext_language(void)
{
	static char osd_language[LANGUAGE_NAME_MAX+1]={0};

	memset(osd_language,0,LANGUAGE_NAME_MAX+1);
	 GxBus_ConfigGet(TELTEXT_LANG, osd_language,LANGUAGE_NAME_MAX,g_config_default.default_teltext_lang);
	 return osd_language;
}

/*
* 设置teltext 语言类型
*/
int32_t app_flash_save_config_teltext_language(const char* language)
{
	if (NULL == language)
		return 0;

	GxBus_ConfigSet(TELTEXT_LANG,language);
	return 0;
}

/*
* 获取subtitle 语言类型
*/
char*  app_flash_get_config_subtitle_language(void)
{
	static char osd_language[LANGUAGE_NAME_MAX+1]={0};

	memset(osd_language,0,LANGUAGE_NAME_MAX+1);
	 GxBus_ConfigGet(SUBTITLE_LANG, osd_language,LANGUAGE_NAME_MAX,g_config_default.default_subtitle_lang);
	 return osd_language;
}

/*
* 设置subtitle 语言类型
*/
int32_t app_flash_save_config_subtitle_language(const char* language)
{
	if (NULL == language)
		return 0;

	GxBus_ConfigSet(SUBTITLE_LANG,language);
	return 0;
}
#endif

/*
* 获取菜单语言类型
*/
char*  app_flash_get_config_osd_language(void)
{
	static char osd_language[LANGUAGE_NAME_MAX+1]={0};

	memset(osd_language,0,LANGUAGE_NAME_MAX+1);
	 GxBus_ConfigGet(OSD_LANG, osd_language,LANGUAGE_NAME_MAX,g_config_default.default_osd_lang);
	 return osd_language;
}

/*
* 设置菜单语言类型
*/
int32_t app_flash_save_config_osd_language(const char* language)
{
	if (NULL == language)
		return 0;

	GxBus_ConfigSet(OSD_LANG,language);
	return 0;
}


/*
* 获取ac3 bypass 音频输出开关状态
*/
int32_t app_flash_get_config_audio_ac3_bypass(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(AUDIO_AC3_BYPASS,&config,g_config_default.default_audio_ac3_bypass);
	return config;
}

/*
* 设置ac3 bypass 音频输出开关状态
*/
int32_t app_flash_save_config_audio_ac3_bypass(int32_t config)
{
	GxBus_ConfigSetInt(AUDIO_AC3_BYPASS,config);
	return 0;
}

/*
* 获取全局声道标志
*/
int32_t app_flash_get_config_track_globle_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(TRACK_GLOBLE_FLAG,&config,g_config_default.default_track_globle_flag);
	return config;
}

/*
* 设置全局声道标志
*/
int32_t app_flash_save_config_track_globle_flag(int32_t config)
{
	GxBus_ConfigSetInt(TRACK_GLOBLE_FLAG,config);
	return 0;
}


/*
* 获取视频声道(默认电视/广播声道非独立设置)
*/
int32_t app_flash_get_config_audio_track(void)
{
	if ((1 == app_flash_get_config_track_globle_flag())||(0 == GxBus_PmProgNumGet()))
		{
			/*获取全局声道*/
			int32_t config = 0;
			GxBus_ConfigGetInt(AUDIO_TRACK,&config,g_config_default.default_audio_track);
			return config;		 
		}
	else
		{
			/*获取当前节目声道*/
			return app_prog_get_cur_prog_audio_track()+1;
		}
	return 0;
}

/*
* 设置视频声道(默认电视/广播声道非独立设置)
*/
int32_t app_flash_save_config_audio_track(int32_t config)
{
	if ((1 == app_flash_get_config_track_globle_flag())||(0 == GxBus_PmProgNumGet()))
		{
			/*保存全局声道*/
			GxBus_ConfigSetInt(AUDIO_TRACK,config);			
		}
	else
		{
			/*保存当前节目声道*/
			app_prog_save_cur_prog_audio_track(config-1);
		}
	
	return 0;
}

/*
* 获取音频声道(默认电视/广播声道非独立设置)
*/
int32_t app_flash_get_config_audio_audio_track(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(AUDIO_AUDIO_TRACK,&config, g_config_default.default_audio_audio_track);
	return config;
}

/*
* 设置音频声道(默认电视/广播声道非独立设置)
*/
int32_t app_flash_save_config_audio_audio_track(int32_t config)
{
	GxBus_ConfigSetInt(AUDIO_AUDIO_TRACK,config);
	return 0;
}

int32_t app_flash_get_config_volumn_default_globle_flag(void)
{
	return g_config_default.default_volumn_globle_flag;
}

/*
* 获取全局音量标志
*/
int32_t app_flash_get_config_volumn_globle_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VOLUMN_GLOBLE_FLAG,&config,g_config_default.default_volumn_globle_flag);
	return config;
}

/*
* 设置全局音量标志
*/
int32_t app_flash_save_config_volumn_globle_flag(int32_t config)
{
	GxBus_ConfigSetInt(VOLUMN_GLOBLE_FLAG,config);
	return 0;
}

/*
* 获取音量大小，根据是否全局标志
* 读取全局音量或节目音量
*/
int32_t app_flash_get_config_audio_volume(void)
{
	if ((1 == app_flash_get_config_volumn_globle_flag())||(0 == GxBus_PmProgNumGet()))
		{
			/*获取全局音量*/
			int32_t config = 0;
			GxBus_ConfigGetInt(AUDIO_VOLUME,&config,g_config_default.default_audio_volume);
			return config;		
		}
	else
		{
			/*获取当前节目音量*/
			return app_prog_get_cur_prog_audio_volumn();
		}
	return 0;
}

/*
* 设置音量大小，根据是否全局标志
* 设置全局音量或节目音量
*/
int32_t app_flash_save_config_audio_volume(int32_t config)
{
	if ((1 == app_flash_get_config_volumn_globle_flag())||(0 == GxBus_PmProgNumGet()))
		{
			/*保存全局音量*/
			GxBus_ConfigSetInt(AUDIO_VOLUME,config);	
		}
	else
		{
			/*保存当前节目音量*/
			app_prog_save_cur_prog_audio_volumn(config);
		}
		return 0;	
}

/*
* 获取菜单透明度(菜单透明度与实际效果正常相反，需修改)
*/
int32_t app_flash_get_config_osd_trans(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(OSD_TRANS,&config,g_config_default.default_osd_trans);
	return config;
}

/*
* 设置菜单透明度(菜单透明度与实际效果正常相反，需修改)
*/
int32_t app_flash_save_config_osd_trans(int32_t config)
{
	GxBus_ConfigSetInt(OSD_TRANS,config);
	return 0;
}

int32_t app_flash_get_config_videocolor_brightness(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEOCOLOR_BRIGHTNESS,&config,g_config_default.default_videocolor_brightness);
	return config;	
}
int32_t app_flash_save_config_videocolor_brightness(int32_t config)
{
	GxBus_ConfigSetInt(VIDEOCOLOR_BRIGHTNESS,config);
	return 0;
}

int32_t app_flash_get_config_videocolor_saturation(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEOCOLOR_SATURATION,&config,g_config_default.default_videocolor_saturation);
	return config;		
}

int32_t app_flash_save_config_videocolor_saturation(int32_t config)
{
	GxBus_ConfigSetInt(VIDEOCOLOR_SATURATION,config);
	return 0;

}

int32_t app_flash_get_config_videocolor_contrast(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEOCOLOR_CONTRAST,&config,g_config_default.default_videocolor_contrast);
	return config;	

}

int32_t app_flash_save_config_videocolor_contrast(int32_t config)
{
	GxBus_ConfigSetInt(VIDEOCOLOR_CONTRAST,config);
	return 0;

}

/*
* 获取静音标志
*/
int32_t app_flash_get_config_mute_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(MUTE_FLAG,&config,g_config_default.default_mute_flag);
	return config;
}

/*
* 设置静音标志
*/
int32_t app_flash_save_config_mute_flag(int32_t config)
{
	GxBus_ConfigSetInt(MUTE_FLAG,config);
	return 0;
}

/*
* 获取信息条显示超时时间
*/
int32_t app_flash_get_config_bar_time(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(BAR_TIME,&config,g_config_default.default_bar_time);
	return config;
}

/*
* 设置信息条显示超时时间
*/
int32_t app_flash_save_config_bar_time(int32_t config)
{
	GxBus_ConfigSetInt(BAR_TIME,config);
	return 0;
}


/*
* 获取监测NIT表频点版本
*/
int32_t app_flash_get_config_center_nit_fre_version(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(MAIN_FREQ_NITVERSION,&config,g_config_default.default_main_fre_nit_version);
	return config;
}

/*
* 设置监测NIT表频点分析的版本
*/
int32_t app_flash_save_config_center_nit_fre_version(int32_t config)
{
	GxBus_ConfigSetInt(MAIN_FREQ_NITVERSION,config);
	return 0;
}

/*
* 获取是否设置密码标志
*/
int32_t app_flash_get_config_password_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(PASSWORD_FLAG,&config,g_config_default.default_password_flag);
	return config;
}

/*
* 设置是否使用密码标志
*/
int32_t app_flash_save_config_password_flag(int32_t config)
{
	GxBus_ConfigSetInt(PASSWORD_FLAG,config);
	return 0;
}

/*
* 获取视频制式
*/
int32_t app_flash_get_config_video_hdmi_mode(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEO_HDMI_MODE,&config,g_config_default.default_video_hdmi_mode);
	return config;
}

/*
* 保存视频制式
*/
int32_t app_flash_save_config_video_hdmi_mode(int32_t config)
{	
	GxBus_ConfigSetInt(VIDEO_HDMI_MODE,config);
	return 0;
}


/*
* 获取视频宽高比模式
*/
int32_t app_flash_get_config_video_aspect(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEO_ASPECT,&config,g_config_default.default_video_aspect);
	return config;
}

/*
* 保存视频宽高比模式
*/
int32_t app_flash_save_config_video_aspect(int32_t config)
{	
	GxBus_ConfigSetInt(VIDEO_ASPECT,config);
	return 0;
}

/*
* 获取视频输出宽高比
*/
Video_Display_Screen_t app_flash_get_config_video_display_screen(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEO_DISPLAY_SCREEN,&config,g_config_default.default_video_display_screen);
	return config;
}

/*
* 设置视频输出宽高比
*/
int32_t app_flash_save_config_video_display_screen(Video_Display_Screen_t config)
{
	int32_t display_screen = config;
	GxBus_ConfigSetInt(VIDEO_DISPLAY_SCREEN,display_screen);
	return 0;
}

int32_t app_flash_get_config_video_auto_adapt(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEO_AUTO_ADAPT,&config,g_config_default.default_video_auto_adapt);
	return config;
}

int32_t app_flash_save_config_video_auto_adapt(int32_t config)
{
	GxBus_ConfigSetInt(VIDEO_AUTO_ADAPT,config);
	return 0;
}

int32_t app_flash_get_config_video_quiet_switch(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(VIDEO_QUIET_SWITCH,&config,g_config_default.default_video_quiet_switch);
	return config;
}

int32_t app_flash_save_config_video_quiet_switch(int32_t config)
{
	GxBus_ConfigSetInt(VIDEO_QUIET_SWITCH,config);
	return 0;
}


uint32_t app_get_manual_search_flag(void)
{
	int32_t nFlag = 0;

	GxBus_ConfigGetInt(MANAULSEARCH, &nFlag, g_config_default.default_manualsearch_flag);

	return nFlag;
}

void app_set_manual_search_flag(int32_t flag)
{
	GxBus_ConfigSetInt(MANAULSEARCH, flag);
	return ;
}

uint32_t app_get_fm_freq(void)
{
	int32_t nStartFreq = 0;

	GxBus_ConfigGetInt(FM_FREQ, &nStartFreq, g_config_default.default_fm_freq);

	return nStartFreq;
}
uint32_t app_set_fm_freq(int32_t nFrequcncy)
{
	GxBus_ConfigSetInt(FM_FREQ, nFrequcncy);
	return 0;
}
uint32_t app_flash_get_config_center_freq(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MAIN_FREQ, &config, g_config_default.default_centre_fre);
	return config;
}

uint32_t app_flash_save_config_center_freq(int32_t nFrequcncy)
{
	GxBus_ConfigSetInt(MAIN_FREQ, nFrequcncy);
	return 0;
}

int32_t app_flash_get_config_center_freq_symbol_rate(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MAIN_FREQ_SYMRATE, &config,g_config_default.default_centre_sym);

	return config;
}

uint32_t app_flash_save_config_center_freq_symbol_rate(int32_t nValue)
{
	GxBus_ConfigSetInt(MAIN_FREQ_SYMRATE, nValue);
	return 0;
}
    
int32_t app_flash_get_config_center_freq_qam(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MAIN_FREQ_QAM, &config,g_config_default.default_centre_qam);

	return config;
}

uint32_t app_flash_save_config_center_freq_qam(int32_t nValue)
{
	GxBus_ConfigSetInt(MAIN_FREQ_QAM, nValue);
	return 0;
}

int32_t app_flash_get_config_center_freq_bandwidth(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MAIN_FREQ_BANDWIDTH, &config,DVB_CENTER_BANDWIDTH);

	return config;
}


uint32_t app_flash_save_config_center_freq_bandwidth(int32_t nValue)
{
	GxBus_ConfigSetInt(MAIN_FREQ_BANDWIDTH, nValue);
	return 0;
}

uint32_t app_flash_get_config_center_freq1(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(MAIN_FREQ1, &config, g_config_default.default_centre_fre1);
	return config;
}
uint32_t app_flash_save_config_center_freq1(int32_t nFrequcncy)
{
	GxBus_ConfigSetInt(MAIN_FREQ1, nFrequcncy);
	return 0;
}
int32_t app_flash_get_config_center_freq_symbol_rate1(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(MAIN_FREQ_SYMRATE1, &config,g_config_default.default_centre_sym1);
	return config;
}
uint32_t app_flash_save_config_center_freq_symbol_rate1(int32_t nValue)
{
	GxBus_ConfigSetInt(MAIN_FREQ_SYMRATE1, nValue);
	return 0;
}
int32_t app_flash_get_config_center_freq_qam1(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(MAIN_FREQ_QAM1, &config,g_config_default.default_centre_qam1);
	return config;
}
uint32_t app_flash_save_config_center_freq_qam1(int32_t nValue)
{
	GxBus_ConfigSetInt(MAIN_FREQ_QAM1, nValue);
	return 0;
}
int32_t app_flash_get_config_manual_search_freq(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MANUAL_SEARCH_FREQ, &config,g_config_default.default_manual_fre);

	return config;
}

uint32_t app_flash_save_config_manual_search_freq(int32_t nFrequcncy)
{
	GxBus_ConfigSetInt(MANUAL_SEARCH_FREQ, nFrequcncy);
	return 0;
}

int32_t app_flash_get_config_manual_search_symbol_rate(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MANUAL_SEARCH_SYMBOL, &config,g_config_default.default_manual_sym);

	return config;
}

uint32_t app_flash_save_config_manual_search_symbol_rate(int32_t nValue)
{
	GxBus_ConfigSetInt(MANUAL_SEARCH_SYMBOL, nValue);
	return 0;
}

int32_t app_flash_get_config_manual_search_qam(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MANUAL_SEARCH_QAM, &config,g_config_default.default_manual_qam);

	return config;
}

uint32_t app_flash_save_config_manual_search_qam(int32_t nValue)
{
	GxBus_ConfigSetInt(MANUAL_SEARCH_QAM, nValue);
	return 0;
}

int32_t app_flash_get_config_dtmb_dvbc_switch(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(DTMB_DVBC_SWITCH, &config,g_config_default.default_dtmb_dvbc_switch);

	return config;
}

uint32_t app_flash_save_config_dtmb_dvbc_switch(int32_t nValue)
{
	GxBus_ConfigSetInt(DTMB_DVBC_SWITCH, nValue);
	return 0;
}


int32_t app_flash_get_config_manual_search_bandwidth(void)
{
	int32_t config = 0;

	GxBus_ConfigGetInt(MANUAL_SEARCH_BANDWIDTH, &config,g_config_default.default_manual_bandwidth);

	return config;
}

uint32_t app_flash_save_config_manual_search_bandwidth(int32_t nValue)
{
	GxBus_ConfigSetInt(MANUAL_SEARCH_BANDWIDTH, nValue);
	return 0;
}



int32_t app_flash_get_config_pvrduration(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(PVR_DURATION_KEY, &config,g_config_default.default_pvr_duration);
	return config;
}

uint32_t app_flash_save_config_pvrduration(int32_t nValue)
{
	GxBus_ConfigSetInt(PVR_DURATION_KEY, nValue);
	return 0;
}

int32_t app_flash_save_config_password(const char * password)
{
	if (NULL == password)
		return 0;
	
	 GxBus_ConfigSet(PASSWORD, (const char*)password);
 	return 0;
}

int32_t app_flash_get_config_password(char *password,int32_t passwordlen)
{
	if (NULL == password)
		return 0;
	
	 GxBus_ConfigGet(PASSWORD, (char*)password,passwordlen,g_config_default.default_password);
	 return 0;

}

/*
* 获取当前CA类型(兼容双CA系统)
*/
int32_t app_flash_get_config_dvb_ca_flag(void)
{

	int32_t config = 0;
	GxBus_ConfigGetInt(DVB_CA,&config,g_config_default.default_dvb_ca);
/*
* check dvb ca flag valid or not
*/
#ifdef CA_FLAG
	switch(config)
		{
			case DVB_CA_TYPE_DVB:
#ifndef DVB_CA_TYPE_DVB_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_DVB\n");
#endif
				break;
			case DVB_CA_TYPE_BY:
#ifndef DVB_CA_TYPE_BY_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_BY\n");
#endif			
				break;
			case DVB_CA_TYPE_CDCAS30:
#ifndef DVB_CA_TYPE_CD_CAS30_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_CDCAS30\n");
#endif		
				break;
			case DVB_CA_TYPE_KN:
#ifndef DVB_CA_TYPE_KN_FLAG  
			printf("dvb ca error , config = DVB_CA_TYPE_KN\n");
#endif	
				break;
			case DVB_CA_TYPE_XINSHIMAO:
#ifndef DVB_CA_TYPE_XINSHIMAO_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_XINSHIMAO\n");	
#endif		
				break;		
			case DVB_CA_TYPE_ABV53:
#ifndef DVB_CA_TYPE_ABV_CAS53_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_ABV\n");
#endif			
							break;
			case DVB_CA_TYPE_DIVITONE:
#ifndef DVB_CA_TYPE_DIVITONE_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_DIVITONE\n");	
#endif		
				break;		
			case DVB_CA_TYPE_DSCAS50:
#ifndef DVB_CA_TYPE_DS_CAS50_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_DSCAS50\n");	
#endif		
				break;
			case DVB_CA_TYPE_DVT:
#ifndef DVB_CA_TYPE_DVT_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_DVT\n");	
#endif		
				break;
			case DVB_CA_TYPE_MG:
#ifndef DVB_CA_TYPE_MG_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_MG\n");	
#endif		
				break;
			case DVB_CA_TYPE_QZ:
#ifndef DVB_CA_TYPE_QZ_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_QZ\n");	
#endif		
				break;
			case DVB_CA_TYPE_GOS:
#ifndef DVB_CA_TYPE_GOS_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_GOS\n"); 
#endif		
				break;
			case DVB_CA_TYPE_GY:
#ifndef DVB_CA_TYPE_GY_CAS_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_GY\n");
#endif		
				break;
            case DVB_CA_TYPE_WF:
#ifndef DVB_CA_TYPE_WF_CA_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_WF\n");
#endif		
				break;  
			case DVB_CA_TYPE_KP:
#ifndef DVB_CA_TYPE_KP_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_KP\n");	
#endif		
				break;
			case DVB_CA_TYPE_MG312:
#ifndef DVB_CA_TYPE_MG312_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_MG312!!!\n");
#endif		            
				break;
 			case DVB_CA_TYPE_TR:
#ifndef DVB_CA_TYPE_TR_FLAG
			printf("dvb ca error , config = DVB_CA_TYPE_TR\n");
#endif		
				break;	
			default:
			printf("dvb ca error , unkown config = %d\n",config);
				break;
		}

	return config;
#else
	printf("fta application\n");
	return DVB_CA_TYPE_NONE;
#endif
}

/*
* 设置当前CA类型(兼容双CA系统)
*/
int32_t app_flash_save_config_dvb_ca_flag(int32_t config)
{
	GxBus_ConfigSetInt(DVB_CA,config);
	return 0;
}

int32_t app_flash_get_config_gx150x_mode_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(DTMB_MODE,&config,g_config_default.default_dtmb_flag);
	return config;
}

int32_t app_flash_save_config_gx150x_mode_flag(int32_t config)
{
	GxBus_ConfigSetInt(DTMB_MODE,config);
	return 0;
}

int32_t app_flash_get_config_sort_by_pat_flag(void)
{
	int32_t config = 0;
	GxBus_ConfigGetInt(SORT_PROG_BY_PAT,&config,g_config_default.default_sort_prog_by_pat);
	return config;
}




/*
* flash 读取序列号
*
*/

/*
* 
*获取序列号,不同方案序列号存储位置可能不同
* addr-序列号存储flash位置
*/
int32_t app_flash_get_serial_number(int32_t addr, char * serial,int32_t maxLen)
{
	int ret = 0;
   
	if (NULL == serial)
		return -1;
#ifdef ECOS_OS
    int fd;
    uint32_t serialaddr,len;
    if(GXCORE_SUCCESS != app_flash_partion_info_by_name("CAUSER",&serialaddr,&len,NULL))
    {
        printf("app_flash_partion_info_by_name failed.\n");
		return -1;
    }
    
    printf("app_flash_partion_info_by_name,addr:%#x,len:%#x \n",serialaddr,len);
    if(serialaddr==0 && len == 0)
    {
		return -1;
    }


	cyg_scheduler_lock();
	fd = open("/dev/flash/0/0", /*O_RDWR*/O_RDONLY);
	if (fd != 0)
	{
		ret = lseek(fd, serialaddr, SEEK_SET);	
		ret = read(fd,serial,maxLen);
		close(fd);

	}
	cyg_scheduler_unlock();
	return 0;
#elif defined(LINUX_OS)
		FILE *file;
		file = fopen("/tmp/SERIAL.INI", "r");
		if (file == NULL)
		{
			printf("----------file open  %s err.\n","/tmp/SERIAL.INI");
			return -1;
		}
		

		ret = fread(serial, 1, maxLen, file);
		if (ret != maxLen) 
		{
			printf("------read failed %d.\n", ret);
			fclose(file);
			return -1;
		}
		
		fclose(file);
		printf("serial = %s\n",serial);
		return 0;
#endif
}


/*
* 从FLASH中读取广告文件到内存中
* 广告播放从内存读取JPG等数据
*/
int32_t app_flash_get_ad_data_to_ddram_file(const char* flash_file_name,const char* file_name)
{
	handle_t ad_file;
	handle_t flash_ad_file;
	size_t   size;	
	char* Buf= NULL;

	if ((NULL == flash_file_name)||(NULL == file_name))
		return GXCORE_ERROR;

	 if (1 != GxCore_FileExists(flash_file_name))
		return GXCORE_ERROR;
	 	

	Buf = GxCore_Malloc(1024);
	if (NULL == Buf)
		{
			return GXCORE_ERROR;			
		}
	memset(Buf,0,1024);


	GxCore_FileDelete(file_name);
#ifdef ECOS_OS
	cyg_scheduler_lock();
#endif
	flash_ad_file = GxCore_Open(flash_file_name, "r");
	if (flash_ad_file > 0)
		{
			GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
			ad_file = GxCore_Open(file_name, "a+");
			GxCore_Seek(ad_file, 0, GX_SEEK_SET);
			size = GxCore_Read(flash_ad_file, Buf, 1, 1024);
			while((size>0)&&(GXCORE_ERROR!=size))
				{

		       		 size = GxCore_Write(ad_file, Buf, 1, size);	
					 size = GxCore_Read(flash_ad_file, Buf, 1, 1024);
				}

		        GxCore_Close(flash_ad_file);	
		        GxCore_Close(ad_file);			
		}
#ifdef ECOS_OS
	cyg_scheduler_unlock();
#endif
	GxCore_Free(Buf);
	Buf = NULL;
	return GXCORE_SUCCESS;
	
}

int32_t app_save_data_to_ddram_file(const char* data,uint32_t len,const char* file_name)
{
	handle_t ad_file;
	 size_t   size;			
	if ((NULL == data)||(NULL == file_name))
		return GXCORE_ERROR;

	/*首先删除以原有文件*/
	if (GxCore_FileExists(file_name) == 1)
	{
		GxCore_FileDelete(file_name);	
	}
#ifdef ECOS_OS
	cyg_scheduler_lock();
#endif
	ad_file = GxCore_Open(file_name, "a+");
	GxCore_Seek(ad_file, 0, GX_SEEK_SET);
	size = GxCore_Write(ad_file, (char*)data, 1, len);
	GxCore_Close(ad_file);
#ifdef ECOS_OS
	cyg_scheduler_unlock();
#endif
	return GXCORE_SUCCESS;
	
}


/*
* 过滤到的广告数据保存到FLASH文件
*/
int32_t app_flash_save_ad_data_to_flash_file(const char* data,uint32_t len,const char* flash_file_name)
{
	handle_t flash_ad_file;
	 size_t   size;			
	if ((NULL == data)||(NULL == flash_file_name))
		return GXCORE_ERROR;

	/*首先删除以原有文件*/
	{
		GxCore_FileDelete(flash_file_name);	
	}
#ifdef ECOS_OS
	cyg_scheduler_lock();
#endif
	flash_ad_file = GxCore_Open(flash_file_name, "a+");
	GxCore_Seek(flash_ad_file, 0, GX_SEEK_SET);
	size =  GxCore_Write(flash_ad_file, (void*)data, 1, len);	   
	GxCore_Close(flash_ad_file);
#ifdef ECOS_OS
	cyg_scheduler_unlock();
#endif
	return GXCORE_SUCCESS;
	
}

/*
* 过滤到的开机广告保存到FLASH中
* 默认开机广告小于64K 
*/
int32_t app_flash_save_logo_data(const char* data,uint32_t len)
{
#ifdef ECOS_OS  
	handle_t fd   ;
	GxPartitionTable *table_info = NULL;
	int i;
	uint32_t addr = 0;
	uint32_t max_size = 0;
	struct flash_erase erase;


	table_info = GxCore_Malloc(sizeof(GxPartitionTable));
	if(NULL == table_info)
		return	GXCORE_ERROR;

	memset(table_info,0,sizeof(GxPartitionTable));
	if(GxOem_PartitionTableGet(table_info) < 0)
	{
		GxCore_Free(table_info);
		table_info = NULL;
		return GXCORE_ERROR;
	}

	for(i = 0; i<table_info->count; i++)
	{
		if(table_info->tables[i].name != NULL 
			&& 0 == strcmp(table_info->tables[i].name, GX_PARTITION_LOGO))
			{
				addr = table_info->tables[i].start_addr;
				max_size = table_info->tables[i].total_size;
				printf("name:%s addr:0x%x total:0x%x\n", table_info->tables[i].name, addr,max_size);
				
				break;
			}

	}

	GxCore_Free(table_info);
	table_info = NULL;

	if ((0 == addr)&&(0 == max_size))
		{


			return GXCORE_ERROR;
		}
     
	if ((NULL == data)||(len > max_size))
		return GXCORE_ERROR;
	
	cyg_scheduler_lock();
	for (i = 0; i<max_size/0x10000; i++)
	{
		fd  = open("/dev/flash/0/0", O_RDWR/*O_RDONLY*/);
		lseek(fd, addr+i*0x10000, SEEK_SET);
		erase.offset    = addr+i*0x10000;
	    erase.len       = 0x10000;
	    if (ioctl(fd, IO_GET_CONFIG_FLASH_ERASE, &erase, sizeof(struct flash_erase)) < 0) 
         {
	        printf("%s%d: flash erase error!\n", __func__, __LINE__);
			close(fd);
			cyg_scheduler_unlock();
			return GXCORE_ERROR;
	    }
		close(fd);
	}
	
	fd   = open("/dev/flash/0/0", O_RDWR/*O_RDONLY*/);	
	lseek(fd, addr, SEEK_SET);						
	write(fd,data, len);								
	fsync(fd);
	close(fd);
	cyg_scheduler_unlock();
#endif
	return GXCORE_SUCCESS;
}


int32_t app_flash_partion_info_by_name(const char *part_name,uint32_t *paddr,uint32_t *plen,char *pdevName)
{
    status_t ret = GXCORE_SUCCESS;
    if(!part_name)
    {
        return GXCORE_ERROR;
    }
    
#ifdef ECOS_OS    
    GxPartitionTable *table_info = NULL;
	int i;
    if(!paddr || !plen)
    {
        return GXCORE_ERROR;
    }
    table_info = GxCore_Malloc(sizeof(GxPartitionTable));
	if(NULL == table_info)
		return  GXCORE_ERROR;
	memset(table_info,0,sizeof(GxPartitionTable));
	if(GxOem_PartitionTableGet(table_info) < 0)
	{
		GxCore_Free(table_info);
		table_info = NULL;
		return GXCORE_ERROR;
	}
	for(i = 0; i < table_info->count; i++)
	{
		if(table_info->tables[i].name != NULL 
			&& 0 == strcmp(table_info->tables[i].name, part_name))
		{
			
			*paddr = table_info->tables[i].start_addr;
			*plen = table_info->tables[i].total_size;
			 printf("%s start=%#x,size = %#x\n",part_name,*paddr,*plen);
             break;
		}
	}
    
    if(i >= table_info->count) //can't find partition
	    ret = GXCORE_ERROR;
    
    GxCore_Free(table_info);
#elif defined(LINUX_OS)
    FlashDevice Dev;
    MtdDevice *pmtdDev = NULL;
    if(GetMtdInfo(&Dev)<0)
    {
        return GXCORE_ERROR;
    }
    
    if((pmtdDev = FindMtdDev_ByName(&Dev,part_name))==NULL)
    {
       printf("FindMtdDev_ByName %s failed.\n",part_name);
       return GXCORE_ERROR;
    }
    if(paddr && plen)
    {
        *paddr = pmtdDev->StartAddress;
        *plen = pmtdDev->Size;
        printf("%s start=%#x,size = %#x\n",part_name,*paddr,*plen);
    }
    if(pdevName)
    {
        strcpy(pdevName,pmtdDev->DeviceName);
    }
#endif
    return ret;
}

int32_t app_file_init(int ReCreateFlag,const char *file_name,uint16_t UnitSize)
{
    handle_t file_h;
    FILE_INFO mfile = {0};
    uint32_t len = 0,size;
    if (NULL == file_name)
		return GXCORE_ERROR;

	if (GxCore_FileExists(file_name) == 1)
	{
        if(ReCreateFlag)
        {
		    GxCore_FileDelete(file_name);	
        }
        else
        {
		    return 1;//文件已经存在
        }
	}

    memset(&mfile,0,sizeof(FILE_INFO));
    mfile.u16Magic = FILE_MAGIC;
    mfile.u16UnitSize = UnitSize;
    mfile.u32Size = 0;
    snprintf(mfile.filename,sizeof(mfile.filename)-1,"%s",file_name);
    len = sizeof(FILE_INFO);

    file_h = GxCore_Open(mfile.filename, "w");
	GxCore_Seek(file_h, 0, GX_SEEK_SET);
	size =  GxCore_Write(file_h, (void *)&mfile, 1, len);	   
    if(size != len)
    {
        GxCore_Close(file_h);
        return -1;
    }
	GxCore_Close(file_h);

    return 0;
}

int32_t app_file_read(const char *file_name,uint8_t *buf,uint32_t maxlen,uint32_t *readlen)
{
    handle_t file_h;
    FILE_INFO mfile = {0};
    uint32_t size;
    uint8_t *pbuf = NULL;
    if (NULL == file_name || NULL == buf || NULL == readlen)
		return GXCORE_ERROR;

    *readlen = 0;
	if (GxCore_FileExists(file_name) != 1)
	{
		return GXCORE_ERROR;
	}


    file_h = GxCore_Open(file_name, "r");
	if (file_h > 0)
	{
		GxCore_Seek(file_h, 0, GX_SEEK_SET);
		size = GxCore_Read(file_h, &mfile, 1, sizeof(FILE_INFO));
        if((size>0)&&(GXCORE_ERROR != size))
        {
            if(mfile.u16Magic == FILE_MAGIC 
                && !strcmp(file_name,mfile.filename)
                && mfile.u32Size >0 
                && mfile.u16UnitSize >0 )
            {
                
int32_t i;
                 *readlen = 0;
                 pbuf = buf;

                 for(i=0;i<mfile.u32Size;i++)
                 {
                    if(*readlen + mfile.u16UnitSize <= maxlen)
                    {
                        size = GxCore_Read(file_h, pbuf, 1, mfile.u16UnitSize);
                        if((size>0)&&(GXCORE_ERROR != size))
                        {
                            pbuf += mfile.u16UnitSize;
                            *readlen += mfile.u16UnitSize;
                        }
                        else
                        {
                            GxCore_Close(file_h);
                            return GXCORE_ERROR;
                        }
                    }
                    else
                    {
                        break;
                    }
                 }
            }
        }
        
        GxCore_Close(file_h);			
	}

    return GXCORE_SUCCESS;
}

int32_t app_file_write(const char *file_name,const uint8_t *buf,uint32_t buflen)
{
    handle_t file_h;
    FILE_INFO mfile = {0};
    uint32_t size;
    
    if (NULL == file_name || NULL == buf)
		return GXCORE_ERROR;
    
    if (GxCore_FileExists(file_name) != 1)
	{
		return GXCORE_ERROR;
	}


    file_h = GxCore_Open(file_name, "r");
	if (file_h > 0)
	{
		GxCore_Seek(file_h, 0, GX_SEEK_SET);
		size = GxCore_Read(file_h, &mfile, 1, sizeof(FILE_INFO));
        if((size>0)&&(GXCORE_ERROR != size))
        {
             if(mfile.u16Magic != FILE_MAGIC 
                || strcmp(file_name,mfile.filename)
                || mfile.u16UnitSize == 0 )
            {
                GxCore_Close(file_h);	
                return GXCORE_ERROR;
            }
        }
        else
        {
            GxCore_Close(file_h);	
            return GXCORE_ERROR;
        }
        GxCore_Close(file_h);	
	}

    if(buflen%mfile.u16UnitSize != 0)
    {
        return GXCORE_ERROR;
    }

    mfile.u32Size = buflen/mfile.u16UnitSize;
    
    file_h = GxCore_Open(mfile.filename, "w");
	GxCore_Seek(file_h, 0, GX_SEEK_SET);
	size =  GxCore_Write(file_h, (void *)&mfile, 1, sizeof(FILE_INFO));	   
    if(size != sizeof(FILE_INFO))
    {
        GxCore_Close(file_h);
        return GXCORE_ERROR;
    }
    size =  GxCore_Write(file_h, (void *)buf, 1, buflen);	   
    if(size != buflen)
    {
        GxCore_Close(file_h);
        return GXCORE_ERROR;
    }
	GxCore_Close(file_h);
    return GXCORE_SUCCESS;
    
}



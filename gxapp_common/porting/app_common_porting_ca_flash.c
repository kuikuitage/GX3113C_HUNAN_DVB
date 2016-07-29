/*
* 此文件接口实现CA移植FLASH初始化、读、写公共接口
*/
#include <gxtype.h>
#include "app_common_flash.h"
#include "app_common_porting_ca_flash.h"
#include "app_common_porting_stb_api.h"
#include "gx_nvram.h"

#define CA_MAX_NUM (50)
handle_t nvram_handle[CA_MAX_NUM]={0};
#define CA_MAX_NVRAM_FILE_NAME_LEN (128)
//handle_t p_gsemFlash;

/*
* 初始化ca flash, 分配大小
* type --CA类型，兼容多CA系统
*/
void app_porting_ca_flash_init(uint32_t size)
{
	int32_t dvb_ca_flag ;
    char        file_name[CA_MAX_NVRAM_FILE_NAME_LEN]={0};
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();
	sprintf(file_name,"gxca_nvram_%d.dat",dvb_ca_flag);
	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return;
		}

	CAS_Dbg(" app_porting_ca_flash_init  size=0x%x\n",size);
	nvram_handle[dvb_ca_flag]= GxNvram_Open(file_name,size);
//	GxNvram_Clean(nvram_handle[dvb_ca_flag],0, size);
	return ;
}

/*
* 从FLASH中读取CA 数据
* type --CA类型，兼容多CA系统
*/
uint8_t app_porting_ca_flash_read_data(uint32_t Offset,uint8_t *pbyData,uint32_t *pdwLen)
{
	size_t Size = 0;
	int32_t dvb_ca_flag ;

	if ((NULL == pbyData)||(NULL == pdwLen))
		return 0;

	size_t readSize = (uint32_t)*pdwLen;
	dvb_ca_flag =  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return 0;
		}

	
	if (E_INVALID_HANDLE == nvram_handle[dvb_ca_flag])
		return 0;


	CAS_Dbg(" app_porting_ca_flash_read_data   Offset=0x%x  pdwLen=0x%x\n",Offset,*pdwLen);
	Size = GxNvram_Read(nvram_handle[dvb_ca_flag],Offset, pbyData, readSize);

	
	if( Size != readSize)
	{
		CAS_Dbg(" app_porting_ca_flash_read_data Size=0x%x readSize=0x%x failed \n",
			Size,readSize);

		return 0;
	}

	return 1;

}

/*
* CA数据写入FLASH
*/
uint8_t app_porting_ca_flash_write_data(uint32_t Offset,uint8_t *pbyData,uint32_t dwLen)
{

	size_t Size = 0;
	int32_t dvb_ca_flag ;
	size_t writeSize = dwLen;
	dvb_ca_flag=  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return 0;
		}
	
		
	if (E_INVALID_HANDLE == nvram_handle[dvb_ca_flag])
		return 0;


	if (NULL == pbyData)
	{
		CAS_Dbg(" app_porting_ca_flash_write_data   failed \n");
		return 0;
	}

	CAS_Dbg(" app_porting_ca_flash_write_data   Offset=0x%x  pdwLen=0x%x\n",Offset,dwLen);
		
	Size = GxNvram_Write(nvram_handle[dvb_ca_flag],Offset, pbyData, writeSize);

	CAS_Dbg("app_porting_ca_flash_write_data size=0x%x\n",Size);


	if(Size != writeSize)
	{
		CAS_Dbg("app_porting_ca_flash_write_data size not equal writeSize=0x%x size=0x%x\n",(unsigned int)writeSize,Size);
		return 0;
	}

	return 1;	
}

uint8_t app_porting_ca_flash_erase(uint32_t Offset,uint32_t dwLen)
{
	int32_t dvb_ca_flag ;
	dvb_ca_flag=  app_flash_get_config_dvb_ca_flag();

	if (dvb_ca_flag >= CA_MAX_NUM)
		{
			return 0;
		}
	
		
	if (E_INVALID_HANDLE == nvram_handle[dvb_ca_flag])
		return 0;

	CAS_Dbg(" app_porting_ca_flash_erase   Offset=0x%x  pdwLen=0x%x\n",Offset,dwLen);	
	GxNvram_Clean(nvram_handle[dvb_ca_flag],Offset, dwLen);

	return 1;	
}


//	GxNvram_Clean(nvram_handle[dvb_ca_flag],0, size);


void app_porting_ca_flash_sem_create(void)
{

	return;
}

void app_porting_ca_flash_sem_wait(void)
{

	return ;
}

void app_porting_ca_flash_sem_post(void)
{

	return ;
}




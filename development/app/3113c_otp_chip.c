#include "gxapp_sys_config.h"
#include <gxtype.h>
#if (GX_CHIP_3113C == DVB_CHIP_TYPE)
#include "devapi/gxotp_api.h"
#ifdef ECOS_OS
#include <gxcore.h>
#define GX_OTP_BASE     0x00F80000
#define MAX_LEN         0x40
#define CHIP3113C_BASE  0x3A3

#define OTP_CON_REG         (*(volatile unsigned int*)(GX_OTP_BASE + 0x80))  //OTP control register
#define OTP_CFG_REG         (*(volatile unsigned int*)(GX_OTP_BASE + 0x84))  //OTP config register
#define OTP_STA_REG         (*(volatile unsigned int*)(GX_OTP_BASE + 0x88))  //OTP state register



typedef unsigned char U8;
typedef unsigned int U32;


void otp_read_hard(U32 start_addr, int rd_num, U8 *data)
{
	GxOtp_Read(start_addr,data,rd_num);
	return;
}

void otp_write_hard(U32 start_addr, int wr_num, U8 *data)
{
	
	GxOtp_Write(start_addr,data,wr_num);
	
	return;
}


/* 0x3A3 - 0X3E3 */
void chip3113C_otp_test(void)
{
	int i = 0;
	unsigned char otp_buf_wr[MAX_LEN];
	unsigned char otp_buf_rd[MAX_LEN];
	unsigned int  start_addr;
	int           data_num;

    data_num = 4;
	start_addr = 0x381;
	otp_read_hard(start_addr, data_num, otp_buf_rd);

    printf("-----------------------read,len:%d\n",data_num);
    for (i = 0; i < data_num; i++)
	{
		printf("otp[0x%x]=%x\n", (start_addr+i), otp_buf_rd[i]);
	}
    
	data_num = MAX_LEN;
	start_addr = CHIP3113C_BASE;
	otp_read_hard(start_addr, data_num, otp_buf_rd);

    printf("-----------------------read,len:%d\n",data_num);
    for (i = 0; i < data_num; i++)
	{
		printf("otp[0x%x]=%x\n", (start_addr+i), otp_buf_rd[i]);
	}

	data_num = 6;
	otp_buf_wr[0] = 0x11;
	otp_buf_wr[1] = 0x22;
	otp_buf_wr[2] = 0x33;
	otp_buf_wr[3] = 0x44;
	otp_buf_wr[4] = 0x55;
	otp_buf_wr[5] = 0x66;

    printf("-----------------------write,len:%d\n",data_num);
    for (i = 0; i < data_num; i++)
	{
		printf("otp[0x%x]=%x\n", (start_addr+i), otp_buf_wr[i]);
	}
	/* write 0x3a3 - 0x3a8 addr */
	otp_write_hard(start_addr, data_num, otp_buf_wr); 

    data_num = MAX_LEN;
	otp_read_hard(start_addr, data_num, otp_buf_rd);
    printf("-----------------------read,len:%d\n",data_num);
	for (i = 0; i < data_num; i++)
	{
		printf("otp[0x%x]=%x\n", (start_addr+i), otp_buf_rd[i]);
	}

	printf("otp_operation is finish");
}

//0x368~36F
void chip3113C_chipId_test(void)
{
    int i = 0;
    #define CHIDLEN 8    
	unsigned char otp_chipid[CHIDLEN]={0};
    unsigned  int *p = NULL;

    printf("-----------------------read,len:%d\n",CHIDLEN);
    p = (unsigned  int *)otp_chipid;
    *p = *(volatile unsigned int *)0xa030a564;
    //printf("h=%X\n",*p);
    
    p = (unsigned  int *)(otp_chipid+4);
    *p = *(volatile unsigned int *)0xa030a560;
    //printf("l=%X\n",*p);

    printf("chipid:");
    for (i = 0; i < CHIDLEN; i++)
	{
		printf("%02X", otp_chipid[i]);
	}
    printf("\n");
}

int get_chipId(unsigned char *id,int len)
{
     //int i = 0;
#define CHIDLEN 8    
	unsigned char otp_chipid[CHIDLEN]={0};
    unsigned  int *p = NULL;

    if(!id || len <CHIDLEN)
    {
        return -1;
    }

    
    p = (unsigned  int *)otp_chipid;
    *p = *(volatile unsigned int *)0xa030a564;
    //printf("h=%X\n",*p);
    
    p = (unsigned  int *)(otp_chipid+4);
    *p = *(volatile unsigned int *)0xa030a560;
    //printf("l=%X\n",*p);
    memcpy(id,otp_chipid,CHIDLEN);
#if 0
    printf("\nchipid:");
    for (i = 0; i < CHIDLEN; i++)
	{
		printf("%02X", otp_chipid[i]);
	}
    printf("\n");
#endif

    return 1;
}

int get_licenceid(unsigned char *id,int len)
{
     //int i = 0;
#define CHIDLEN 8  
// todo
#if 0
    printf("\nchipid:");
    for (i = 0; i < CHIDLEN; i++)
	{
		printf("%02X", licenceid[i]);
	}
    printf("\n");
#endif

    return 1;
}

#endif
#endif




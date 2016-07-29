/*
 * =====================================================================================
 *
 *       Filename:  flash_chip.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  6/26/2014 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  chenth
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#ifdef ECOS_OS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "gxcore.h"

#define _IO_GET_CONFIG_FLASH_INFO              	0x609
#define _IO_GET_CONFIG_FLASH_OTP_READ			0x614
#define _IO_SET_CONFIG_FLASH_OTP_WRITE			0x615
#define _IO_SET_CONFIG_FLASH_OTP_ERASE			0x616
#define _IO_SET_CONFIG_FLASH_OTP_BLOCK_LOCK		0x617
#define _IO_GET_CONFIG_FLASH_OTP_BLOCK_STATUS	0x618
typedef struct {
	unsigned int               OTPAddress; 
	unsigned int               Len;
	unsigned char           *RamBuffer;    // Number of entries
} FlashOTPInfo_t;
typedef struct {
	unsigned int sector_size;   // Number of pages in a sector.
	unsigned int  sector_count;  // Number of sectors on device.  
	unsigned int  addr_width;
	unsigned int  jedec_id;      // 3 byte JEDEC identifier for this device.
	unsigned int  ext_id;    // 2 byte ext identifier for this device.
	void *priv;				//private parameter
	struct gxspi_range *prange;//pointer to spi_writeprotect_addrrange
	unsigned int  count;		 //sizeof(prange)/sizeof(struct gxspi_range) 
	unsigned int  param;		 //bottom or top protect support: &0x01 bottom;&0x08 top support.
}FlashGetconfigInfo_t;

void spi_otp_read(int fd, unsigned int addr, char *buf, int len)
{
	unsigned char *rbuf = (unsigned char*)buf;
	int err = 0;
	//int LenTemp = sizeof(FlashOTPInfo_t);
	FlashOTPInfo_t ReadInfo = {0};

	if((((addr&0xff) + len) > 256)
		&& (fd < 0)
		&& (rbuf == NULL)
		&& (((addr < 0x1000)|| (addr > 0x10ff))
			|| ((addr < 0x2000)|| (addr > 0x20ff))
			|| ((addr < 0x3000)|| (addr > 0x30ff))))
	{
		printf("\nOTP, read failed,wrong parameter\n");
		return ;
	}
	ReadInfo.OTPAddress = addr;
	ReadInfo.RamBuffer= rbuf;
	ReadInfo.Len = len;
	memset(rbuf,0,len);
	//err = ioctl(fd, _IO_GET_CONFIG_FLASH_OTP_READ, (void*)&ReadInfo, LenTemp);
	err = ioctl(fd, _IO_GET_CONFIG_FLASH_OTP_READ, (void*)&ReadInfo);
	if(err < 0)
	{
		printf("\nOTP, read failed\n");
		return ;
	}
	return ;
}

void spi_otp_write(int fd, unsigned int addr, char *buf, int len)
{
	unsigned char *wbuf = (unsigned char*)buf;
	int err;
	//int LenTemp = sizeof(FlashOTPInfo_t);
	FlashOTPInfo_t WriteInfo = {0};

	if((((addr&0xff) + len) > 256)
		&& (fd < 0)
		&& (wbuf == NULL)
		&& (((addr < 0x1000)|| (addr > 0x10ff))
			|| ((addr < 0x2000)|| (addr > 0x20ff))
			|| ((addr < 0x3000)|| (addr > 0x30ff))))
	{
		err = -1;
		printf("\nOTP, write failed,wrong parameter\n");
		return ;
	}
	WriteInfo.OTPAddress= addr;
	WriteInfo.RamBuffer= wbuf;
	WriteInfo.Len = len;
	//err = ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_WRITE, (void*)&WriteInfo, LenTemp);
	err = ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_WRITE, (void*)&WriteInfo);
	if(err < 0)
	{
		printf("\nOTP, write failed\n");
		return ;
	}
	return ;
}

void spi_otp_erase(int fd, unsigned int addr)
{
	unsigned int EraseBaseAddr = addr;
	//unsigned int len = 4;
	int err = 0;
	
	if((fd > 0)
		&&(((EraseBaseAddr >= 0x1000)&& (EraseBaseAddr <= 0x10ff))
			|| ((EraseBaseAddr >= 0x2000)&& (EraseBaseAddr <= 0x20ff))
			|| ((EraseBaseAddr >= 0x3000)&& (EraseBaseAddr <= 0x30ff))))
	{
		EraseBaseAddr &=~(0xff);
		printf("\nOTP,erase [start addr = 0x%x] \n", EraseBaseAddr);
		//err =  ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_ERASE, (void*)&EraseBaseAddr, len);
		err =  ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_ERASE, (void*)&EraseBaseAddr);
		if(err < 0)
		{
			printf("\nOTP, erase failed\n");
			return ;
		}
	}
	else
	{
		printf("\nOTP, erase failed,wrong parameter\n");
	}

	return ;
}

// BlockIndex is : 1, 2, 3
// the lock bit: bit3~bit5 (LB3 LB2 LB1) for three block
//"Bit define: SUS CMP LB3 LB2 LB1 (R) QE SRP1
void spi_otp_lock(int fd, int BlockIndex)
{
	int err = -1;
	char BlockFlag[3] = {1,2,4};
	//unsigned int  len = 1;
	if((fd <= 0)
		|| ((BlockIndex < 1) &&(BlockIndex > 3)))
	{
		printf("\nOTP,  lock status failed,parameter err\n");
		return ;
	}
	//err =  ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_BLOCK_LOCK, (void*)&BlockFlag[BlockIndex - 1], len);
	err =  ioctl(fd, _IO_SET_CONFIG_FLASH_OTP_BLOCK_LOCK, (void*)&BlockFlag[BlockIndex - 1]);
	if(err < 0)
	{
		printf("\nOTP, lock status failed\n");
		return ;
	}
	return ;
}

int  spi_otp_get_lock_status(int fd, int BlockIndex)
{// return 1 ,mean the bolck "BlockIndex" is locked, or is unlock
	int err = -1;
	char status = 0;
	//unsigned int len = 1;
	if((fd <= 0)
		|| ((BlockIndex < 1) &&(BlockIndex > 3)))
	{
		printf("\nOTP, get lock status failed, parameter err\n");
		return err;
	}
	//err =  ioctl(fd, _IO_GET_CONFIG_FLASH_OTP_BLOCK_STATUS, (void*)&status, len);
	err =  ioctl(fd, _IO_GET_CONFIG_FLASH_OTP_BLOCK_STATUS, (void*)&status);
	if(err < 0)
	{
		printf("\nOTP, get lock status failed\n");
		return err;
	}
	status = ((status)&(1<<(BlockIndex - 1))) > 0?1:0;
	return status;
}

void spi_otp_get_id(int fd)
{
	FlashGetconfigInfo_t info = {0};
	int err = -1;
	//int len = sizeof(FlashGetconfigInfo_t);
	if(fd <= 0)
	{
		printf("\nOTP, get flash info failed, parameter err\n");
		return ;
	}
	//err =  ioctl(fd, CYG_IO_GET_CONFIG_FLASH_INFO, (void*)&info, len);
	err =  ioctl(fd, _IO_GET_CONFIG_FLASH_INFO, (void*)&info);
	if(err < 0)
	{
		printf("\nOTP, get flash info failed\n");
		return ;
	}
	printf("\nOTP, flash jedec_id = %x\n", info.jedec_id);
}

int spi_otp_open(void)
{
	int fd = -1;
	fd = open("/dev/flash/0/0", 3);//O_RDWR
	if(fd <= 0)
		printf("\nOTP, open device failed\n");
	return fd;
}

void spi_otp_close(int fd)
{
	if(fd > 0)
		close(fd);
	else
		printf("\nOTP, wrong parameter for close\n");
}

#endif

#ifdef LINUX_OS
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define OTP_START_ADDR	(0x1000)
#define OTP_END_ADDR	(0x30ff)
#define OTP_BLOCK_SIZE	(256)
#define OTP_BLOCK_NUM	(3)// block1=0x1000~0x10ff, block2=0x2000~0x20ff, block3=0x3000~0x30ff

/*
 [0x9F] 00000000  ef 40 18                                        .@.
 [0x90] 00000000  ef 17                                           ..
 [0x4B] 00000000  d2 61 c0 00 27 35 43 29                         .a..'5C)
 [0xAB] 00000000  17                                              .
 */
struct spi_id {
	unsigned char	cmd;
	unsigned char 	code[8];	
	unsigned char	len;
};

#define SPI_ID              			_IOWR('o', 1, struct spi_id)
#define SPI_OTP_LOCK             		_IOW('o', 2,unsigned char)
#define SPI_OTP_LOCK_STATUS	_IOR('o', 3,unsigned char)
#define SPI_OTP_ERASE            		_IOW('o', 4,unsigned int)

static void dump(unsigned char *buf, int size)
{
    int len, i, j, c;
#define PRINT(...) do { printf(__VA_ARGS__); } while(0)

    for(i=0;i<size;i+=16) {
        len = size - i;
        if (len > 16)
            len = 16;
        PRINT("%08x ", i);
        for(j=0;j<16;j++) {
            if (j < len)
                PRINT(" %02x", buf[i+j]);
            else
                PRINT("   ");
        }
        PRINT(" ");
        for(j=0;j<len;j++) {
            c = buf[i+j];
            if (c < ' ' || c > '~')
                c = '.';
            PRINT("%c", c);
        }
        PRINT("\n");
    }
//	PRINT("\n\n");
#undef PRINT
}

static int check_otp_addr(int addr)
{
        if(addr < OTP_START_ADDR || addr > OTP_END_ADDR){
		fprintf(stderr,"otp start addr wrong!\n");
		return 1;
	}else{
		return 0;
	}
}
void spi_otp_dump(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	if((fd <= 0) ||(NULL == buf) || (len > (OTP_BLOCK_SIZE - (addr&0xff))) || (check_otp_addr(addr)))
	{
		fprintf(stderr,"otp dump parameter wrong!\n");
		return ;
	}
	
	fprintf(stderr,"read addr: 0x%4X\n", addr);
	lseek(fd,addr,SEEK_SET);
	memset(buf,0,len);
	ret = read(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"out of block ####ret = %d\n",ret);
	if(ret > 0)
		dump((unsigned char*)buf,ret);

}

void spi_otp_read(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	if((fd <= 0) ||(NULL == buf) || (OTP_BLOCK_SIZE - (addr&0xff)) || (check_otp_addr(addr)))
	{
		fprintf(stderr,"otp read parameter wrong!\n");
		return ;
	}
	
	lseek(fd,addr,SEEK_SET);
	memset(buf,0,len);
	ret = read(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"out of block ####ret = %d\n",ret);
}

void spi_otp_write(int fd, unsigned int addr, char *buf, int len)
{
	int ret = 0;
	if((fd <= 0) ||(NULL == buf) || (OTP_BLOCK_SIZE - (addr&0xff)) || (check_otp_addr(addr)))
	{
		fprintf(stderr,"otp write parameter wrong!\n");
		return ;
	}
	lseek(fd,addr,SEEK_SET);
	ret = write(fd,buf,len);
	if(ret != len)
		fprintf(stderr,"write opt error. \n");

}

void spi_otp_erase(int fd, unsigned int addr)
{
	
	int err = 0;

	if((fd <= 0)  || (check_otp_addr(addr)))
	{
		fprintf(stderr,"otp erase parameter wrong!\n");
		return ;
	}

	printf("erase addr: 0x%4X\n", addr);
	err = ioctl(fd,SPI_OTP_ERASE,&addr);
	if(err != 0)
		perror("SPI OTP ERASE Error\n");

}

void spi_otp_lock(int fd, int BlockIndex)
{//BlockIndex 1, 2, 3
	int err = 0;

	if((fd <= 0) ||(BlockIndex  <= 0) || (BlockIndex > 3))
	{
		fprintf(stderr,"otp lock block parameter wrong!\n");
		return ;
	}
	
	err = ioctl(fd,SPI_OTP_LOCK,&BlockIndex);
	if(err != 0)
		perror("SPI OTP Lock Error\n");

}


int  spi_otp_get_lock_status(int fd, int BlockIndex)
{//BlockIndex 1, 2, 3
	int err = 0;
	unsigned char status;

	if((fd <= 0) ||(BlockIndex  <= 0) || (BlockIndex > 3))
	{
		fprintf(stderr,"otp lock block parameter wrong!\n");
		return -1;
	}

	printf("Bit define: S15   S14  S13 S12  S11  S10  S9  S8\n");
	printf("Bit define: SUS CMP LB3 LB2 LB1 (R) QE SRP1\n");
	printf("                     ^   ^   ^\n");

	err = ioctl(fd,SPI_OTP_LOCK_STATUS,&status);
	if(err != 0)
		perror("SPI_OTP_LOCK_STATUS\n");
	else
		printf("otp lock status : 0x%02X\n",status);
	return(int) (((status >> 3) & (1<<(BlockIndex-1))) >> (BlockIndex-1));
	
}

void spi_otp_get_id(int fd)
{
	struct spi_id id;

	if(fd <= 0)
	{
		fprintf(stderr,"get flash id parameter wrong!\n");
		return ;
	}

	printf("Read IDs: JEDEC ID, Manufacture/Device ID, Unique ID\n");
	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x9f;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x90;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0x4b;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

	memset(&id,0,sizeof(struct spi_id));
	id.cmd = 0xab;
	ioctl(fd,SPI_ID,&id);
	dump(id.code,id.len);	

}

int spi_otp_open(void)
{
	int fd = -1;
	fd = open("/dev/spiotp0",O_RDWR);
	if (fd < 0) 
	{
		printf("/dev/spiotp0 open fail!\n");
	}
	return fd;
}

void spi_otp_close(int fd)
{
	if(fd < 0)
	{
		printf("/dev/spiotp0 close fail!\n");
		return ;
	}
	close(fd);
}


#endif

#ifdef _DEBUG
/*extern int spi_otp_open(void);
extern int spi_otp_close(int fd);
extern void spi_otp_read(int fd, unsigned int addr, char *buf, int len);
extern void spi_otp_write(int fd, unsigned int addr, char *buf, int len);
extern void spi_otp_erase(int fd, unsigned int addr);
extern int  spi_otp_get_lock_status(int fd, int block);
extern void spi_otp_lock(int fd, int block);
extern void spi_otp_get_id(int fd);
*/
void flash_otp_test(void)
{
	int OTPFD = -1;
	char buffer[256] = {0};
	int status = 0;
	int BlockNum = 1;
    int i;

    printf("#########################\n");
    printf("####flash otp test\n");
    printf("#########################\n");
	// open the device
	OTPFD = spi_otp_open();
	if(OTPFD <= 0)
	{
        printf("\nOTP, open failed\n");
        return ;
	}

    
	printf("\nOTP, open successfully\n");
	
	// read the otp block, bolck1~block3
	//block1
	spi_otp_read(OTPFD, 0x1000, buffer, 256);//0x1000~0x10ff
	//block2
	spi_otp_read(OTPFD, 0x2000, buffer, 256);//0x2000~0x20ff
	//block3
	spi_otp_read(OTPFD, 0x3000, buffer, 256);//0x3000~0x30ff
	printf("\nOTP, read successfully\n");
	
	// write
	buffer[0] = 0x01;
	buffer[1] = 0x02;
	buffer[2] = 0x03;
	buffer[3] = 0x04;
	spi_otp_write(OTPFD, 0x1000, buffer, 4);//0x1000~0x10ff
	printf("otp wirte->%x,len:%d\n",0x1000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");
    
	buffer[0] = 0x05;
	buffer[1] = 0x06;
	buffer[2] = 0x07;
	buffer[3] = 0x08;
	spi_otp_write(OTPFD, 0x2000, buffer, 4);//0x2000~0x20ff
	printf("otp wirte->%x,len:%d\n",0x2000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");
	buffer[0] = 0x09;
	buffer[1] = 0x0a;
	buffer[2] = 0x0b;
	buffer[3] = 0x0c;
	spi_otp_write(OTPFD, 0x3000, buffer, 4);//0x3000~0x30ff
	printf("otp wirte->%x,len:%d\n",0x3000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");
	printf("\nOTP, write successfully\n");


    spi_otp_read(OTPFD, 0x1000, buffer, 4);
    printf("otp read->%x,len:%d\n",0x1000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");

    spi_otp_read(OTPFD, 0x2000, buffer, 4);
    printf("otp read->%x,len:%d\n",0x2000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");

    spi_otp_read(OTPFD, 0x3000, buffer, 4);
    printf("otp read->%x,len:%d\n",0x3000,4);
    for(i = 0;i < 4;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");
    
	// erase
	spi_otp_erase(OTPFD, 0x1000);//erase 256bytes
	spi_otp_erase(OTPFD, 0x2000);
	spi_otp_erase(OTPFD, 0x3000);
	printf("\nOTP, erase successfully\n");
	
	//get lock status,
	status = spi_otp_get_lock_status(OTPFD, BlockNum);
	printf("\nOTP, block%d lock status = %d\n",BlockNum,status);
	BlockNum++;
	status = spi_otp_get_lock_status(OTPFD, BlockNum);
	printf("\nOTP, block%d lock status = %d\n",BlockNum,status);
	BlockNum++;
	status = spi_otp_get_lock_status(OTPFD, BlockNum);
	printf("\nOTP, block%d lock status = %d\n",BlockNum,status);
	printf("\nOTP, get lock status successfully\n");

    /*
	// set lock
	BlockNum = 1;// block1
	spi_otp_lock(OTPFD, BlockNum);
	BlockNum++;//block2
	spi_otp_lock(OTPFD, BlockNum);
	BlockNum++;//block3
	spi_otp_lock(OTPFD, BlockNum);
	printf("\nOTP, set block lock successfully\n");
	*/
	
	//get flash id
	spi_otp_get_id(OTPFD);
	printf("\nOTP, get flash id successfully\n");
	
	// close the device
	spi_otp_close(OTPFD);
	printf("\nOTP, close successfully\n");
}
#endif



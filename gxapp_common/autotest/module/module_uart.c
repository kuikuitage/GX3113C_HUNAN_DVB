/*
 * =====================================================================================
 *
 *       Filename:  autotest_uart.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2014 01:06:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Z.Z.R
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdarg.h>
#include "../include/common_autotest.h"

#define F_DBG       printf		

static int sg_UartFd				= -1;

// uart control
#ifdef ECOS_OS

void UartPrintf(const char *fmt, ...)
{
	va_list va;
	char Buff[512];

	if(sg_UartFd == -1)
	{
		F_DBG("%s",fmt);
		return;
	}
	memset(Buff,'\0',512);
	va_start(va, fmt);
	vsprintf(Buff, fmt, va);
	va_end(va);
	//write(sg_UartFd, Buff, 512);
	write(sg_UartFd, Buff, strlen(Buff));
	
	GxCore_ThreadDelay(10);
}

int uart_init(unsigned int Baudrate)
{
extern int print_enable;
	int Ret = 0;
	unsigned int BRate = Baudrate;
	if(sg_UartFd > 0)
	{
		F_DBG("\nTEST, uart init parameter error\n");
		return -1;
	}
	sg_UartFd = open("/dev/ser0",O_RDWR|O_NONBLOCK);
	if(sg_UartFd < 0)
	{
		F_DBG("\nTEST, uart open failed\n");
		return -1;
	}
	Ret = ioctl(sg_UartFd,0x0191/*CYG_IO_SET_CONFIG_SERIAL_BAUD_RATE*/, (void*)&BRate);
	if(Ret < 0)
	{
		UartPrintf("\nTEST, uart set baudrate failed\n");
		return -1;
	}
	print_enable = 0;
	return sg_UartFd;
}

int uart_receive(unsigned char *Buffer, unsigned int byteToRead)
{
	int RealByteRead = 0;
	if((Buffer == NULL) || (sg_UartFd < 0))
	{
		UartPrintf("\nTEST, uart set baudrate failed\n");
		return -1;
	}
	RealByteRead = read(sg_UartFd,Buffer,byteToRead);
	if(RealByteRead != byteToRead)
	{
	//	UartPrintf("\nTEST, uart read not enough data\n");
	}
	return RealByteRead;
}

int uart_send(unsigned char *Buffer, unsigned int byteToWrite)
{
	int RealByteGet = 0;
	if((Buffer == NULL) || (sg_UartFd <= 0))
	{
		return -1;
	}
	RealByteGet = write(sg_UartFd, Buffer, byteToWrite);
	if(RealByteGet != byteToWrite)
	{
		UartPrintf("\nTEST, uart send failed\n");
		return -1;
	}
	return RealByteGet;
}

int uart_close(void)
{
    extern int print_enable;
	if(sg_UartFd <= 0)
	{
		return -1;
	}
	close(sg_UartFd);
	sg_UartFd = -1;
	print_enable = 1;
	return 0;
}

#endif


#ifdef LINUX_OS
#include "termios.h"

static int set_termios(int fd,long speed,char databit,char stopbit,char oebit,struct termios *pstTtyattr)
{
  int err = -1;

  pstTtyattr->c_cflag |= CLOCAL | CREAD;
  switch(databit)
    {
  case 7:
	  pstTtyattr->c_cflag |= CS7;
	  break;
  case 8:
	  pstTtyattr->c_cflag |= CS8;
	  break;
  default:
	  pstTtyattr->c_cflag |= CS8;
	  break;
    }
  switch(stopbit)
    {
		case 1:
			pstTtyattr->c_cflag &= ~CSTOPB;
			break;
		case 2:
			pstTtyattr->c_cflag |= CSTOPB;
		  break;
		default:
		  pstTtyattr->c_cflag &= ~CSTOPB;
		  break;
    }
  switch(oebit)
    {
  case 'O'://odd
	  pstTtyattr->c_cflag |= PARENB;
	  pstTtyattr->c_cflag |= (INPCK | ISTRIP);
	  pstTtyattr->c_cflag |= PARODD;
	  break;
  case 'E'://even
	  pstTtyattr->c_cflag |= PARENB;
	  pstTtyattr->c_cflag |= (INPCK | ISTRIP);
	  pstTtyattr->c_cflag &= ~PARODD;
	  break;
  case 'N':
	  pstTtyattr->c_cflag &= ~PARODD;
	  break;
  default:
	  pstTtyattr->c_cflag &= ~PARODD;
	  break;
    }

  switch(speed)
    {
  case 2400:
	  cfsetispeed(pstTtyattr,B2400);
	  cfsetospeed(pstTtyattr,B2400);
	  break;
  case 4800:
	  cfsetispeed(pstTtyattr,B4800);
	  cfsetospeed(pstTtyattr,B4800);
	  break;
  case 9600:
	  cfsetispeed(pstTtyattr,B9600);
	  cfsetospeed(pstTtyattr,B9600);
	  break;
  case 57600:
	  cfsetispeed(pstTtyattr,B57600);
	  cfsetospeed(pstTtyattr,B57600);
	  break;
  case 115200:
//	  cfsetspeed(pstTtyattr,B115200);
	  cfsetispeed(pstTtyattr,B115200);
	  cfsetospeed(pstTtyattr,B115200);
	  break;
  default:
	  cfsetispeed(pstTtyattr,B9600);
	  cfsetospeed(pstTtyattr,B9600);
	  break;
    }
  pstTtyattr->c_cc[VTIME] = 0;
  pstTtyattr->c_cc[VMIN] = 0;
  tcflush(fd,TCIFLUSH);
  if((tcsetattr(fd,TCSANOW,pstTtyattr)) != 0)
  {
  	;
  }
  else
  {
	   err = 0;
  }
  return err;
}

void UartPrintf(const char *fmt, ...)
{
	va_list va;
	char buff[512];

	if(sg_UartFd == -1)
	{
		F_DBG("%s",fmt);
		return;
	}
	memset(buff,'\0',512);
	va_start(va, fmt);
	vsprintf(buff, fmt, va);
	va_end(va);
	write(sg_UartFd, buff, strlen(buff));
	GxCore_ThreadDelay(10);
}

//#define NORMAL_UART_SUPPORT
#ifdef NORMAL_UART_SUPPORT
#define KERNEL_DEBUG_PRI_CMD1		"echo 3 > /proc/sys/kernel/printk"	
#define KERNEL_DEBUG_PRI_CMD2		"echo 7 > /proc/sys/kernel/printk"	
#define KERNEL_DEBUG_PRI_CMD3		"rm -rf /dev/console;ln -s /dev/null /dev/console"	
#define KERNEL_DEBUG_PRI_CMD4		"rm -rf /dev/console;mknod /dev/console c 5 1"	
	

static int _kernel_printf_pri_adjust(int DiableFlag)
{
	if(DiableFlag)
    {
		system(KERNEL_DEBUG_PRI_CMD1);
	//	system(KERNEL_DEBUG_PRI_CMD3);
    }
    else
    {
		system(KERNEL_DEBUG_PRI_CMD2);
	//	system(KERNEL_DEBUG_PRI_CMD4);
    }
    return 0;
}

static int _user_printf_control(int DisableFlag)
{
	if(DisableFlag)
	{// disable the user printf output
		int fd = open("/dev/null",O_RDWR);
		if(fd < 0)
		{
			FACTORY_DBG("\nUART TEST, user printf control open NULL device failed...\n");
			return -1;
		}
		dup2(fd,0);
		dup2(fd,1);
		dup2(fd,2);
		if(fd > 2)
		    close(fd);		
	}
	else
	{// recover
		if(sg_UartFd == -1)
		{
			FACTORY_DBG("\nUART TEST, user printf control recover failed...\n");
			return -1;
		}
		dup2(sg_UartFd,0);
		dup2(sg_UartFd,1);
		dup2(sg_UartFd,2);
		close(sg_UartFd);
		sg_UartFd = -1;
	}
	return 0;
}

int uart_init(unsigned int Baudrate)
{
	int Ret = 0;
	unsigned int BRate = Baudrate;
	struct termios stDefaultTtyattr;
	
	if(sg_UartFd > 0)
	{
		F_DBG("\nTEST, uart init parameter error\n");
		return -1;
	}
	sg_UartFd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
	if(sg_UartFd < 0)
	{
		F_DBG("\nUART TEST, uart device open failed...\n");
		return -1;
	}
	// set baudrate
	tcgetattr(sg_UartFd,&stDefaultTtyattr);
	cfmakeraw(&stDefaultTtyattr);
	set_termios(sg_UartFd, BRate, 8, 1, 'N', &stDefaultTtyattr);
	
	_kernel_printf_pri_adjust(1);
	_user_printf_control(1);
	return sg_UartFd;
}

int uart_receive(unsigned char *Buffer, unsigned int byteToRead)
{
	int RealByteRead = 0;
	int RetLen = 0;
	if((Buffer == NULL) || (sg_UartFd < 0))
	{
		UartPrintf("\nTEST, uart receive parameter err\n");
		return -1;
	}

	ioctl(sg_UartFd,FIONREAD,&RealByteRead);
	if(RealByteRead > 0)
	{
		RetLen = read(sg_UartFd, Buffer, RealByteRead);
	}
	

	return RetLen;
}

int uart_send(unsigned char *Buffer, unsigned int byteToWrite)
{
	int RealByteGet = 0;
	if((Buffer == NULL) || (sg_UartFd <= 0))
	{
		return -1;
	}
	RealByteGet = write(sg_UartFd, Buffer, byteToWrite);
	if(RealByteGet != byteToWrite)
	{
		UartPrintf("\nTEST, uart send failed\n");
		return -1;
	}
	return RealByteGet;
}

int uart_close(void)
{
	if(sg_UartFd <= 0)
	{
		return -1;
	}
	_kernel_printf_pri_adjust(1);
	_user_printf_control(1);
	return 0;
}
#else

int uart_init(unsigned int Baudrate)
{
	struct termios stDefaultTtyattr;
	
	if(sg_UartFd > 0)
	{
		F_DBG("\nTEST, uart init parameter error\n");
		return -1;
	}
	sg_UartFd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
	if(sg_UartFd < 0)
	{
		F_DBG("\nUART TEST, uart device open failed...\n");
		return -1;
	}
	// set baudrate 
    tcgetattr(sg_UartFd,&stDefaultTtyattr);
	cfmakeraw(&stDefaultTtyattr);
	set_termios(sg_UartFd, Baudrate, 8, 1, 'N', &stDefaultTtyattr);
	
	return sg_UartFd;
}

int uart_receive(unsigned char *Buffer, unsigned int byteToRead)
{
	//int RealByteRead = 0;
	int RetLen = 0;
	if((Buffer == NULL) || (sg_UartFd < 0))
	{
		F_DBG("\nTEST, uart receive parameter err\n");
		return -1;
	}

	//ret = ioctl(sg_UartFd,FIONREAD,&RealByteRead);
	//if(RealByteRead > 0)
	{
		RetLen = read(sg_UartFd, Buffer, byteToRead);
	}
    //else
    //{
   //   printf("\nret = %d, len = %d\n", ret, RetLen);  
	//  UartPrintf("sds");
    //}

	

	return RetLen;
}

int uart_send(unsigned char *Buffer, unsigned int byteToWrite)
{
	int RealByteGet = 0;
	if((Buffer == NULL) || (sg_UartFd <= 0))
	{
		return -1;
	}
	RealByteGet = write(sg_UartFd, Buffer, byteToWrite);
	if(RealByteGet != byteToWrite)
	{
		F_DBG("\nTEST, uart send failed\n");
		return -1;
	}
	return RealByteGet;
}

int uart_close(void)
{
	if(sg_UartFd <= 0)
	{
		return -1;
	}
	close(sg_UartFd);
	sg_UartFd = -1;
	return 0;
}

#endif
#endif


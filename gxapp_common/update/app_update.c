/*
 * =====================================================================================
 *
 *       Filename:  app_update.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年08月05日 10时29分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include "gxcore.h"
#include "app_update.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


typedef struct
{
	handle_t serial;
	handle_t flash;
	int8_t *err;//实际应用过程中需要ui显示为了无ui时的调试先用err保存错状态
	int8_t status;//0 - succesee, -1 - err
}GUpdate;

GUpdate update;

extern unsigned char gx3113C_3113c_boot[];
extern unsigned int gx3113C_3113c_boot_len;

static int32_t serial_read(uint8_t *buf, int32_t size, uint32_t time_out)
{
	fd_set fds;
	struct timeval timeout;

	FD_ZERO(&fds);
	FD_SET(update.serial, &fds);

	timeout.tv_sec = time_out;
	timeout.tv_usec = 0;
		
	if (select(update.serial, &fds, NULL, NULL, &timeout) > 0) 
	{
		if (FD_ISSET(update.serial, &fds))
		{
			return read(update.serial, buf, size);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


static int32_t serial_write(uint8_t *buf, int32_t size)
{
	if (write(update.serial, buf, size) != size)
	{
		return -1;
	}
	return size;
}

static int32_t serial_update_wait(char *s, uint32_t time_out_s)
{
	int32_t pos = 0;
	uint8_t buf[1024];
	int32_t len = strlen((const char *)s);
	int32_t timeout_flag = 0;

	memset(buf, 0, 1024);
	while ( pos < len && pos < 1024) 
	{
		if (serial_read(buf + pos, 1, time_out_s) > 0) 
		{
			timeout_flag = 0;
			if (buf[pos] == '\r') 
			{
				buf[pos] = '\n';
			}
			if (s[pos] == buf[pos])
			{
				pos	++;
			}
		}
		else if (time_out_s != 0)
		{
			    return -1;
		}
	}

	return strncmp((const char*)buf, (const char*)s, len);
}
static void ctr_stdout(uint8_t flag)//0 - turn off; 1 - turn on
{
#ifdef ECOS_OS
	extern int print_enable;
	print_enable = flag;
#elif defined(LINUX_OS)
#endif
}

static void serial_update_exit(const char *s, int32_t status)
{
	if (s != NULL)
	{
		update.err = (int8_t *)GxCore_Strdup((const char*)s);
	}

	update.status = status;
	close(update.serial);
	update.serial = 0;
	close(update.flash);
	update.flash = 0;
	ctr_stdout(1);
	return;
}

static int32_t serial_update_start(void)
{
	ctr_stdout(0);
	fflush(stdout);
	update.serial = open(APP_UPDATE_SERIAL_DEVICE_NAME, O_RDWR);
	if (update.serial < 0)
	{
		serial_update_exit("open serial device err", -1);
		return -1;
	}
	return 0;
}

static int32_t serial_update_send(void *data, int32_t len)
{
	int32_t i, size;
	uint8_t buf[128];
	uint8_t *pdata = (uint8_t *)data;
	uint32_t crc = 0;
	uint32_t *magic = (uint32_t *)data;
	
	switch(*magic) 
	{
	case 0x3211:
		size = len > 8192 ? 8192 : len;
		break;
	default:
		size = len > 4096 ? 4096 : len;
		break;
	}

	size = size / 4;

	buf[0] = 'Y';
	buf[1] = (size >>  0) & 0xFF;
	buf[2] = (size >>  8) & 0xFF;
	buf[3] = (size >> 16) & 0xFF;
	buf[4] = (size >> 24) & 0xFF;

	if (serial_write(buf, 5) < 0)
	{
		serial_update_exit("send boot len err", -1);
		return -1;
	}

	for(i=0; i<len; i++)
	{
		crc += pdata[i]; 
	}

	size = size * 4 - 12;

	serial_write(pdata + 4, size);
	serial_write((uint8_t *)(&crc), 4);
	serial_write((uint8_t *)(&len), 4);
	serial_write((uint8_t *)"boot", 4);

	if (serial_update_wait("GET",6) <0)
	{
		serial_update_exit("wait client get err", -1);
		return -1;
	}

	while (1)
	{
		i = 0;
		while (i<len)
		{
			i += serial_write(pdata + i, MIN(len - i, 2048));
		}
		memset(buf,0,128);
		size = serial_read(buf, 1, 100);
		if (size < 1)
		{
			break;
		}

		if (buf[0] == 'O')
		{
			break;
		}
		else if (buf[0] == 'E')
		{
			//err agin;
		}
		else
		{
			serial_update_exit("send boot err", -1);
			return -1;
		}
	}
	return 0;
}

static int32_t serial_update_command(const char *cmd)
{
#define FILE_LEN  (8*1024*1024)
	uint32_t i = 0;
	uint32_t count = 0;
	uint32_t unit = 0;

	if (strcmp(cmd, "serialdown") == 0)
	{
		uint8_t new_cmd[128];
		uint8_t *data = NULL;

		memset(new_cmd, 0, 128);

		//0 -  写入的地址; 8*1024*1024 - 文件大小 移植时注意这两个值应该通过partiton模块获取table后再确定
		sprintf((char*)new_cmd, "serialdown %d %d", 0, FILE_LEN);
		
		update.flash = open(APP_UPDATE_FLASH_DEVICE_NAME, O_RDWR);
		if (update.flash < 0)
		{
			serial_update_exit("open flash err", -1);
			return -1;
		}
		data = GxCore_Malloc(FILE_LEN);
		if (data == NULL)
		{
			serial_update_exit("no memory read flash file", -1);
			return -1;
		}

		if (read(update.flash, data, FILE_LEN) != FILE_LEN)
		{
			serial_update_exit("read flash file err", -1);
			return -1;
		}
		
		serial_write(new_cmd, strlen((const char*)new_cmd));

		if (new_cmd[strlen((const char*)new_cmd) - 1] != '\n')
		{
			serial_write((uint8_t*)"\n", 1);
		}
		if (serial_update_wait("~sta~", 3) < 0) 
		{
			serial_update_exit("Failed to receive \"~sta~\" from stb in 3s", -1);
			return -1;
		}
		for (i = 0, count = 0, unit = (FILE_LEN / 100 + 1); i <= 100; i++)
		{
			if (unit == 0)
			{
				serial_write(data, FILE_LEN);
				break;
			}
			if((count + unit) <= FILE_LEN)
			{
				serial_write((void *)(data + count), unit);
				count += unit;
			}
			else
			{
				serial_write((void *)(data + count), FILE_LEN - count);
				count = FILE_LEN;
				break;
			}
		}
		if (serial_update_wait("~fin~", 3) < 0) 
		{
			serial_update_exit("Failed to receive \"~fin~\" from stb in 3s", -1);
			return -1;
		}
	}
	if (serial_update_wait("boot> ", 100) != 0) 
	{
		serial_update_exit("after send file,can't wait boot>", -1);
		return -1;
	}
	return 0;
}


int32_t  SerialUpdate(void)
{
	if (update.err != NULL)
	{
		GxCore_Free(update.err);
		update.err = NULL;
	}
	if (serial_update_start() < 0)
	{
		return -1;
	}
	if (serial_update_wait("X",100) <0)
	{
		serial_update_exit("wait client X time out", -1);
		return -1;
	}

	if (serial_update_send(gx3113C_3113c_boot, gx3113C_3113c_boot_len) <0)
	{
		return -1;
	}
	if (serial_update_wait("boot> ", 3) < 0) 
	{
		serial_update_exit("Before exec the first command, failed to receive \"boot>\" from stb in 3s", -1);
		return -1;
	}

	if (serial_update_command("serialdown") < 0)
	{
		return -1;
	}
	serial_update_exit("update ok", 0);
	return 0;
}


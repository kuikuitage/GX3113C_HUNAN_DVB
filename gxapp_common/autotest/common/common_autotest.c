#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/module_command.h"
#include "../include/common_autotest.h"

// 
static int __find_first_keyword(char *cmd, char *keyword)
{
	int len = 0;
	char buffer[MAX_CMDBUF_SIZE] = {0};
	if((cmd == NULL) || (keyword == NULL))
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	len = strlen(cmd);
	if((len == 0) || (len >= MAX_CMDBUF_SIZE))
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	if((cmd[0] < 'a') || (cmd[0] > 'z'))
		sscanf(cmd, "%*[^a-z]%s",buffer);
	else
		sscanf(cmd, "%s", buffer);
//	printf("\ncmd = %s, keyword = %s\n", cmd, buffer);
	len = strlen(buffer);
	if((len == 0) || (len >= MAX_CMDBUF_SIZE))
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	memcpy(keyword, buffer, len);
	return 0;
}

/***************************************************************************
* Function		:parse_line
* Description	:get command param num, command name and params
* Arguments		:[in]line
				 [in]argv[]
* Return 		: return param num -- argc
* Others		:argv[0]--name  argv[1]--params1  argv[2]--params2
****************************************************************************/
static int __parse_line (char *line, char *argv[])
{
	int nargs = 0;
	//log_printf("Line: %s|\n",line);
	while (nargs < CONFIG_SYS_MAXARGS) {
		 //skip any white space 
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if(*line == '\n'){
			*line = '\0';
		}

		if (*line == '\0') {	// end of line, no more args	
			argv[nargs] = NULL;
			return (nargs);
		}

		argv[nargs++] = line;	// begin of argument string	

		// find end of string 
		while (*line && (*line != ' ') && (*line != '\t') && (*line != '\n')) {
			++line;
		}

		if(*line == '\n'){
			*line = '\0';
		}

		if (*line == '\0' ) {	// end of line, no more args	
			argv[nargs] = NULL;
			return (nargs);
		}

		*line++ = '\0';		//terminate current arg	 
	}

	log_printf("** Too many args (max. %d) **\n", CONFIG_SYS_MAXARGS);

	return (nargs);
}

static int _auto_test_one_cmd_proccess(char *cmd, int cmdlen)
{
	signed int ret = 0;
	char buffer[MAX_CMDBUF_SIZE] = {0};
	char *argv[CONFIG_SYS_MAXARGS + 1] = {0};
	int argc = 0;
	cmd_tbl_t *cmd_ops = NULL;
		
	ret = __find_first_keyword(cmd,buffer);
	if(ret < 0)
	{
		// cmd is wrong
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		auto_test_reply("find_firt_keywords", "failed: the cmd is not support!", 2, cmd);
        goto err;
		return -1;
	}
	cmd_ops = find_cmd(buffer);
	if(cmd_ops == NULL)
	{
		// cmd is wrong, cmd is not support
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		auto_test_reply("find_cmd", "failed: the cmd is not support!", 2, cmd);
		goto err;
        return -1;
	}
	if(cmd_ops->cmd != NULL)
	{
		memset(buffer, 0, MAX_CMDBUF_SIZE);
		memcpy(buffer, cmd, strlen(cmd));
		argc = __parse_line(buffer, argv);
		if((argc == 0) || (argc >= CONFIG_SYS_MAXARGS))
		{
			// cmd is wrong, cmd is not support
			log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
			auto_test_reply(cmd_ops->name, "failed: parameter is wrong!", 2, NULL);
			goto err;
            return -1;
		}
#ifdef ECOS_OS
        // for gx uart module, reset the uart receive fifo
        {// NOTICE: the register address is different from different chips
            int i = 1000;
            *(volatile unsigned int*)0xa0400010 |=(1<<4);
            while(i--);
            *(volatile unsigned int*)0xa0400010 &=~(1<<4);
        }
#endif
		ret = cmd_ops->cmd(cmd_ops, 0, argc, argv, cmd);
		if(ret < 0)
		{
			// cmd execute failed
			log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
			goto err;
            return -1;
		}
	}
	else
	{
		// cmd is wrong, cmd is not support
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		auto_test_reply(cmd_ops->name, "failed: have no function to deal with the cmd!", 1, NULL);
		goto err;
        return -1;
	}
	return 0;

err:
#ifdef ECOS_OS
    // for gx uart module, reset the uart receive fifo
    {// NOTICE: the register address is different from different chips
        int i = 1000;
        *(volatile unsigned int*)0xa0400010 |=(1<<4);
        while(i--);
        *(volatile unsigned int*)0xa0400010 &=~(1<<4);
    }
#endif

    return -1;
}


// export

int auto_test_reply(char *cmd, char *str, unsigned int error_code, char *result)
{
#define REPLAY_CMD_LEN  MAX_CMDBUF_SIZE*2
	char buffer[REPLAY_CMD_LEN] = {0};
    int ret = 0;
	if((cmd == NULL) || (str == NULL))
	{
		log_printf("\nreply failed!!!\n");
		
	}
	if(result != NULL)
	{
		sprintf(buffer, "code:%d ", error_code);
		strcat(buffer, "data: ");
		strcat(buffer, result);
		strcat(buffer, " message: ");
	}
	else
	{
		sprintf(buffer, "code:%d message: ", error_code);
	}
    strcat(buffer, "reply ");
	strcat(buffer, cmd);
	strcat(buffer, " ");
	if((strlen(buffer) + strlen(str)) >= (REPLAY_CMD_LEN - 2))
	{
		char *p = NULL;
		p = strchr(str,':');
		if(p == NULL)
		{
			strcat(buffer, "failed");
		}
		else
		{
			memcpy(buffer+strlen(buffer), str, (REPLAY_CMD_LEN - strlen(buffer)));
		}
			
	}
	else
	{
		strcat(buffer, str);
	}
	strcat(buffer, "\r\n");

	ret = uart_send((unsigned char*)buffer,strlen(buffer));
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}


int auto_test_protocol_data_receive(unsigned char* MemBuffer, int MemSize,
											 int *pReadPos,int *pWritePos,
											 unsigned char *ReceiveData,int ReceiveSize)
{
	if((MemBuffer == NULL) 
		|| (ReceiveData == NULL) 
		|| (pReadPos == NULL)
		|| (pWritePos == NULL)
		|| (MemSize*2 < (ReceiveSize + *pWritePos)) 
		|| (MemSize < ReceiveSize)
		|| (ReceiveSize <= 0))
	{
		log_printf("\nPROTOCOL, parameter error\n");
		return -1;
	}

	if((*pWritePos + ReceiveSize) > MemSize)
	{
		memcpy((MemBuffer + *pWritePos), ReceiveData, (MemSize - *pWritePos));
		memcpy(MemBuffer,(ReceiveData + (MemSize - *pWritePos)),(ReceiveSize - (MemSize - *pWritePos)));
		*pWritePos = ReceiveSize - (MemSize - *pWritePos);
		if(*pWritePos >= *pReadPos)
		{
			*pWritePos = 0;
			*pReadPos = 0;
			log_printf("\nPROTOCOL, buff is full\n");
			return -2;
		}
	}
	else
	{
		memcpy((MemBuffer + *pWritePos), ReceiveData, ReceiveSize);
		if((*pWritePos + ReceiveSize) == MemSize)
			*pWritePos = 0;
		else
			*pWritePos +=  ReceiveSize;
	}
	return 0;
}

int auto_test_protocol_data_dealwith(unsigned char* MemBuffer, int MemSize,
											 int *pReadPos,int *pWritePos,
											 int ByteToDo)
{
	char *pData 							= NULL;
	char *pTemp 							= NULL;
	char *pTempParser						= NULL;
	char *p									= NULL;
	int ByteToDealWith 						= ByteToDo;
	int RealDoSize 							= 0;
	int Ret									= 0;
	int Length								= 0;
	int i									= 0;

	if((MemBuffer == NULL)
		|| (pReadPos == NULL)
		|| (pWritePos == NULL)
		|| (MemSize < *pReadPos)
		|| (MemSize < ByteToDealWith)
		|| (ByteToDealWith <= 0))
	{
		log_printf("\nSK PROTOCOL, parameter error\n");
		return -1;
	}
	if(*pReadPos == *pWritePos)
	{
		return -1;
	}
	log_printf("\nPROTOCOL, Read = %d, Write = %d\n",*pReadPos,*pWritePos);

	if(*pReadPos > *pWritePos)
	{
		if(((MemSize - *pReadPos) + *pWritePos) < ByteToDealWith)
			ByteToDealWith = (MemSize - *pReadPos) + *pWritePos;
	}
	else
	{
		if((*pWritePos - *pReadPos) < ByteToDealWith)
			ByteToDealWith = *pWritePos - *pReadPos;
	}

	pTemp = calloc(1,ByteToDealWith+1);
	if(pTemp == NULL)
	{
		UartPrintf("PROTOCOL, calloc error\n");
		return -1;
	}

	pTempParser = calloc(1,ByteToDealWith+1);
	if(pTempParser == NULL)
	{
		log_printf("PROTOCOL, calloc error\n");
		GxCore_Free(pTemp);
		return -1;
	}

	if((*pReadPos + ByteToDealWith) <= MemSize)
	{
		pData = (char*)MemBuffer + *pReadPos;
		memcpy(pTemp,pData,ByteToDealWith);
	}
	else
	{
		pData = (char*)MemBuffer + *pReadPos;
		memcpy(pTemp,pData,(MemSize - *pReadPos));
		pData = (char*)MemBuffer;
		memcpy((pTemp + (MemSize - *pReadPos)),pData,(ByteToDealWith - (MemSize - *pReadPos)));
	}
	// do the data
	pData = pTemp;
	p = strstr((char*)pData,CMD_END_FLAG);
	if(p == NULL)
	{
		//pTemp += ByteToDealWith;
		log_printf("PROTOCOL, bad data...\n");
		goto BACK;
	}
	while(1)
	{
		memset(pTempParser, 0, ByteToDealWith+1);
		Length = p - pData + (strlen(CMD_END_FLAG));
		memcpy(pTempParser,pData,Length);

		for(i = 0; i < Length; i++)
		{
			if(pTempParser[i] == '\r')
				pTempParser[i] = ' ';
		}
		Ret = _auto_test_one_cmd_proccess(pTempParser,Length);
		if(Ret < 0)
		{
			//log_printf("\nPROTOCOL, parser cmd failed...\n");
			//break;
		}
		pData += Length;
		p = strstr((char*)pData,CMD_END_FLAG);
		if(p == NULL)
		{
			log_printf("\nPROTOCOL, not enough data to deal with...\n");
			break;
		}
	}

BACK:	
	// successfull, set the read position
	RealDoSize = pData - pTemp;
	if((*pReadPos + RealDoSize) > MemSize)
	{
		*pReadPos = (*pReadPos + RealDoSize) - MemSize;
	}
	else if((*pReadPos + RealDoSize) == MemSize)
	{
		*pReadPos = 0;
	}
	else
	{
		*pReadPos += RealDoSize;
	}
	GxCore_Free(pTemp);
	GxCore_Free(pTempParser);
	return 0;
}



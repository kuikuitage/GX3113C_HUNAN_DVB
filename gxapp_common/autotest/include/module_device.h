#ifndef __MODULE_DEVICE_H_
#define __MODULE_DEVICE_H_

#define MAX_DEVICE_NUM 		                    (16)


typedef int (*UserCommandParse)(char *Command, void **Out1, void **Out2);
typedef struct UserCommand_s
{
	char *CommandName;
	UserCommandParse ParseFunction;
}UserCommad_t;

typedef struct UserDefineFunc_s
{
	char CmdNum;
	UserCommad_t Cmds[8];
}UserDefineFunc_t;




typedef struct dev_tbl_s	dev_tbl_t;

struct dev_tbl_s {
	char 	*name; /* device name, such as dvbs, dvbt and so on.*/
	int     (*lock_tp)(void *TpInfo);//(TpInfo_t *TpInfo)
    int 	(*play)(void *ProgInfo);//(ProgInfo_t *ProgInfo)
	int 	(*parse)(char *Commond,void **ppInOut1, void **ppInOut2);
	UserDefineFunc_t UserDefineFuncs;
	char	*usage;		/* Usage message	(short)	*/
};

dev_tbl_t *find_dev (const char *cmd);
int register_dev(dev_tbl_t *dev_item);
#endif
	

#ifndef __MODULE_COMMAND_H_
#define __MODULE_COMMAND_H_

#define CMD_END_FLAG			"\n"
#define MAX_CMDBUF_SIZE         256
#define CONFIG_SYS_MAXARGS      16
#define MAX_COMMAND_NUM 		(32)

typedef struct cmd_tbl_s	cmd_tbl_t;

struct cmd_tbl_s {
	char	*name;		/* Command Name			*/
	int	 maxargs;	/* maximum number of arguments	*/
	int	 repeatable;	/* autorepeat allowed?		*/
				/* Implementation function	*/
	signed int (*cmd)(struct cmd_tbl_s *,int, int,char *[], char *);
	char	*usage;		/* Usage message	(short)	*/
};

/* common/command.c */
extern cmd_tbl_t *find_cmd(const char *cmd);
extern int  register_cmd(cmd_tbl_t  *cmd_item);

#endif


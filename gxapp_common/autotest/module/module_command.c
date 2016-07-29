#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/module_command.h"
#include "../include/common_autotest.h"

cmd_tbl_t  __command_list[MAX_COMMAND_NUM];
unsigned int __command_list_used = 0;

/***************************************************************************
 * Function 	:find_cmd_tbl
 * Description 	:
 * Arguments 	:
 * Return 		:
 * Others 		:
 **************************************************************************/
cmd_tbl_t *find_cmd_tbl (const char *cmd, cmd_tbl_t *table, int table_len)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = table;	/*Init value */
	int len;
	int n_found = 0;

	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = strlen(cmd);
	for (cmdtp = table;
	     cmdtp != table + table_len;
	     cmdtp++) {
		if (strcmp (cmd, cmdtp->name) == 0) {
			if (len == strlen (cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}

	return NULL;	/* not found or ambiguous command */
}

cmd_tbl_t *find_cmd (const char *cmd)
{
	
	int len = __command_list_used; 
	
	return find_cmd_tbl(cmd, __command_list, len);
}

int register_cmd(cmd_tbl_t *cmd_item)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdnew;

	int len = 0;

	if( __command_list_used + 1 >= MAX_COMMAND_NUM){
		log_printf("[TestCommand] command list is full when registering.\n");
		return -1;
	}

	//printf("name:%s  index:%d\n",cmd_item->name,__command_list_used);

	len = strlen(cmd_item->name);

	for(cmdtp = __command_list; cmdtp != &__command_list[__command_list_used]; cmdtp++){

		if(cmdtp->name == NULL){
			break;
		}
		if (strcmp (cmd_item->name, cmdtp->name) == 0) {
			log_printf("[TestCommand] find same command name when registering.\n");
			return -1;
		}
	}
	
	cmdnew = __command_list + __command_list_used;
	memcpy(cmdnew, cmd_item, sizeof(cmd_tbl_t));
	printf("Register command :%s\n",cmdnew->name );

	__command_list_used++;

	return 0;
}


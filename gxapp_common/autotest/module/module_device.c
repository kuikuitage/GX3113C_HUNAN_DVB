#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/module_device.h"
#include "../include/common_autotest.h"

dev_tbl_t  __demod_list[MAX_DEVICE_NUM];
unsigned int __demod_list_used = 0;

/***************************************************************************
 * Function 	:find_dev_tbl
 * Description 	:
 * Arguments 	:
 * Return 		:
 * Others 		:
 **************************************************************************/
dev_tbl_t *find_dev_tbl (const char *cmd, dev_tbl_t *table, int table_len)
{
	dev_tbl_t *devtp;
	dev_tbl_t *devtp_temp = table;	/*Init value */
	int len;
	int n_found = 0;

	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = strlen(cmd);
	for (devtp = table;
	     devtp != table + table_len;
	     devtp++) {
		if (strcmp (cmd, devtp->name) == 0) {
			if (len == strlen (devtp->name))
				return devtp;	/* full match */
			devtp_temp = devtp;	/* abbreviated device? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return devtp_temp;
	}
	return NULL;	/* not found or ambiguous device */
}

dev_tbl_t *find_dev (const char *cmd)
{
	int len = __demod_list_used; 
	
	return find_dev_tbl(cmd, __demod_list, len);
}

int register_dev(dev_tbl_t *dev_item)
{
	dev_tbl_t *devtp;
	dev_tbl_t *devnew;
	int len = 0;

	if( __demod_list_used + 1 >= MAX_DEVICE_NUM){
		//log_printf("[TestCommand] device list is full when registering.\n");
		return -1;
	}
	//printf("name:%s  index:%d\n",cmd_item->name,__command_list_used);

	len = strlen(dev_item->name);
	for(devtp = __demod_list; devtp != &__demod_list[__demod_list_used]; devtp++){

		if(devtp->name == NULL){
			break;
		}
		if (strcmp (dev_item->name, devtp->name) == 0) {
			//log_printf("[TestCommand] find same device name when registering.\n");
			return -1;
		}
	}
	
	devnew = __demod_list + __demod_list_used;
	memcpy(devnew, dev_item, sizeof(dev_tbl_t));
 	//log_printf("Register device :%s\n",devnew->name );
	__demod_list_used++;
	return 0;
}


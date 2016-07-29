#ifndef __LIB_NET_H__
#define __LIB_NET_H__
#include <gxcore.h>
#include "gxtype.h"


#define NET_FREE(s) if(s){GxCore_Free(s);s=NULL;}


int net_init(void);
unsigned char* net_get(char* url);


#endif


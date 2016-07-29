#ifndef __APP_POPMSG_H__
#define __APP_POPMSG_H__

#include "app.h"


#ifdef __cplusplus
extern "C" {
#endif

//common string
#define STR_CHK_SAVE         "Do you want to save?"


typedef enum
{
	POPMSG_TYPE_YESNO,
	POPMSG_TYPE_OK,
	POPMSG_TYPE_MINI_YESNO,
	POPMSG_TYPE_PASSWORD
}popmsg_type;

typedef enum
{
	POPMSG_RET_NONE,
	POPMSG_RET_OK,
	POPMSG_RET_YES,
	POPMSG_RET_NO
}popmsg_ret;




popmsg_ret app_popmsg(uint32_t x, uint32_t y, const char* context, popmsg_type type);



// TODO: later
typedef enum
{
	POPMSG_TYPE_URGENT_MSG,
	POPMSG_TYPE_PPC_MSG
}popmsg_type_no_exec;
void app_popmsg_no_exec(uint16_t x, uint16_t y, 
	                    const char* header, 
	                    const char* title, 
	                    const char* context, 
	                    popmsg_type_no_exec type);
void app_end_popmsg_no_exec(popmsg_type_no_exec type);
void app_set_popmsg_yes_timeout(int32_t time);
#ifdef __cplusplus
}
#endif

#endif /* __APP_POPMSG_H__ */


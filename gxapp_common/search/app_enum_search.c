/**
 *
 * @file        app_enum_search.c
 * @brief
 * @version     1.1.0
 * @date        03/31/2016 
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#include <gxtype.h>
#include "app_common_search.h"

static search_fre_list enum_fre_list ;
uint16_t app_enum_search_fre_list_init(void)
{
	uint16_t i = 0;
	uint16_t freArrayCount =0;	
	memset(&enum_fre_list,0,sizeof(search_fre_list));
	if ( (0 == strncmp("runde",DVB_CUSTOM,strlen(DVB_CUSTOM)))
	   &&(0 == strncmp("yunnan_dtmb",DVB_MARKET,strlen(DVB_MARKET))))
		{
			freArrayCount = 9;
			enum_fre_list.app_fre_array[0]=115000;	
			enum_fre_list.app_fre_array[1]=123000;	
			enum_fre_list.app_fre_array[2]=131000;
			enum_fre_list.app_fre_array[3]=139000;
			enum_fre_list.app_fre_array[4]=147000;
			enum_fre_list.app_fre_array[5]=155000;
			enum_fre_list.app_fre_array[6]=322000;
			enum_fre_list.app_fre_array[7]=330000;
			enum_fre_list.app_fre_array[8]=338000;

			for (i = 0; i< freArrayCount; i++)
				{
					enum_fre_list.app_qam_array[i]=3;
					enum_fre_list.app_symb_array[i]=6875;					
				}
	
					  
			enum_fre_list.num = freArrayCount;
			enum_fre_list.nit_flag = GX_SEARCH_NIT_DISABLE;			
		}

	

	return enum_fre_list.num;
}

search_fre_list* app_enum_search_get_fre_list(void)
{
	return &enum_fre_list;	
}


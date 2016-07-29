#include "cJSON.h"
int    cJSON_GetArraySize(cJSON *array)
{
	if (array)
	{
		cJSON *c=array->child;int i=0;while(c)i++,c=c->next;return i;
	}
	return 0;
}
cJSON *cJSON_GetArrayItem(cJSON *array,int item)
{
	if (array)
	{
		cJSON *c=array->child;  while (c && item>0) item--,c=c->next; return c;
	}
	return NULL;
}
cJSON *cJSON_GetObjectItem(cJSON *object,const char *string)
{
	if (object)
	{
		cJSON *c=object->child; while (c && cJSON_strcasecmp(c->string,string)) c=c->next; return c;
	}
	return NULL;
}
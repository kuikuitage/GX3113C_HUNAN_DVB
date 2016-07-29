#include "app_id_manage.h"
#include "gxapp_sys_config.h"
#include "demod_enum.h"
#if (DVB_DEMOD_MODE == DVB_DEMOD_DVBS)

#define DB_ID_UNEXIST   (0)

extern AppIdOps g_AppIdOps;

enum
{
	COL_POS = 0,
	COL_ID,
	COL_TOTAL
};

enum
{
	ID_UNEXIST,
	ID_EXIST
};

typedef struct
{
	int32_t     manage_mode;
	uint32_t*   array;
	uint32_t    realCnt;
	uint32_t    totalCnt;
}IdArray;

typedef struct id_manage    IdManage;
struct id_manage
{
    AppIdOps*       id_ops;
    IdArray*        id_arr;

    IdManage*      prior;
    IdManage*      next;
};

IdManage* GxIdManage = NULL;

IdManage* cls_to_obj(AppIdOps* id_ops)
{
    IdManage* p = GxIdManage;

    if ((p == NULL)
        || (id_ops == NULL))
    {
        return NULL;
    }	

    while(p->next != NULL)
    {
        if (p->id_ops == id_ops)
        {
            return p;
        }
        p = p->next;
    }

    if (p->id_ops == id_ops)
    {
        return p;
    }

    return NULL;
}

status_t open_ids_array(AppIdOps* id_ops, int32_t manage_mode, uint32_t createCnt)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id != NULL) 
		&& (id->id_arr != NULL)
		&& (0 < createCnt)
		&& (NULL ==  id->id_arr->array))
	{
		id->id_arr->manage_mode = manage_mode;
		id->id_arr->array = GxCore_Malloc(createCnt*sizeof(int32_t));
		if(NULL != id->id_arr->array)
		{
			memset(id->id_arr->array,0,createCnt*sizeof(int32_t));
			id->id_arr->realCnt = 0; 
			id->id_arr->totalCnt = createCnt;
			return GXCORE_SUCCESS;
		}
		else
		{
			printf("GxCore_Malloc error \n");
			return GXCORE_ERROR;
		}
	}
	else
	{
		printf("the count of array is invalid\n");
		return GXCORE_ERROR;
	}
}

status_t  close_ids_array(AppIdOps* id_ops)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id != NULL) 
		&& (id->id_arr != NULL)
		&& (id->id_arr->array != NULL))
	{
		GxCore_Free(id->id_arr->array);
		id->id_arr->array = NULL;
		id->id_arr->realCnt = 0;
		id->id_arr->totalCnt = 0;
		return GXCORE_SUCCESS;
	}
	else
	{
		printf("the array have already freed \n");
		return GXCORE_ERROR;
	}
}


// 1-exist   0-unexist
uint32_t check_id_exist(AppIdOps* id_ops, uint32_t pos_or_id)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id == NULL) 
		|| (id->id_arr == NULL) 
		|| (id->id_arr->array == NULL) )
	{
		return ID_UNEXIST;
	}

	if (id->id_arr->manage_mode == MANAGE_SAME_LIST)
	{
		// pos, database have no member id is 0
		if ((pos_or_id < id->id_arr->totalCnt)
			&& (id->id_arr->array[pos_or_id] != DB_ID_UNEXIST))  
		{
			return ID_EXIST;
		}
	}
	else if (id->id_arr->manage_mode == MANAGE_DIFF_LIST)
	{
		int i; 
		
		for (i=0; i<id->id_arr->realCnt; i++)
		{
			// id
			if (id->id_arr->array[i] == pos_or_id) 
			{
				return ID_EXIST;
			}
		}
	}
	else {}
	
	return ID_UNEXIST;
}


status_t add_id_into_array(AppIdOps* id_ops, uint32_t pos, uint32_t idv)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id == NULL) 
		|| (id->id_arr == NULL) 
		|| (id->id_arr->array == NULL))
	{
		return GXCORE_ERROR;
	}
	
	if (id->id_arr->manage_mode == MANAGE_SAME_LIST)
	{
		// database have no member id is 0
		if ((pos < id->id_arr->totalCnt)
			&& (id->id_arr->array[pos] == DB_ID_UNEXIST))
		{
			id->id_arr->array[pos] = idv;
			id->id_arr->realCnt++;	
			return GXCORE_SUCCESS;
		}
	}
	else if (id->id_arr->manage_mode == MANAGE_DIFF_LIST)
	{
		// member unexist, add
		if ((id->id_arr->realCnt < id->id_arr->totalCnt)
			&& (check_id_exist(id_ops, idv) == ID_UNEXIST))
		{
			id->id_arr->array[id->id_arr->realCnt] = idv;
			id->id_arr->realCnt++;	
			return GXCORE_SUCCESS;
		}
	}
	else {}

	return GXCORE_ERROR;
}

status_t del_from_array(AppIdOps* id_ops, uint32_t pos)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id == NULL) 
		|| (id->id_arr == NULL) 
		|| (id->id_arr->array == NULL)
		|| (pos >= id->id_arr->totalCnt))
	{
		printf("[id] del err pos\n");
		return GXCORE_ERROR;
	}

	if (id->id_arr->manage_mode == MANAGE_SAME_LIST)
	{
		// database have no member id is 0
		if (id->id_arr->array[pos] != DB_ID_UNEXIST) 
		{
			id->id_arr->array[pos] = DB_ID_UNEXIST;
			id->id_arr->realCnt--;	

			return GXCORE_SUCCESS;
		}
	}
	else if (id->id_arr->manage_mode == MANAGE_DIFF_LIST)
	{
		int32_t i;
		
		// database have no member id is 0
		if (id->id_arr->array[pos] != DB_ID_UNEXIST) 
		{
			for (i=pos; i<id->id_arr->realCnt; i++)
			{
				id->id_arr->array[i] = id->id_arr->array[i+1];
			}
			
			id->id_arr->realCnt--;	
			return GXCORE_SUCCESS;
		}
	}
	else{}
	
	return GXCORE_ERROR;
}

// especial for flag get
status_t get_id_pos_array(AppIdOps* id_ops, uint32_t* pos_buf)
{
	int32_t i = 0;
	int32_t j = 0;
	IdManage *id = cls_to_obj(id_ops);

	if ((pos_buf == NULL)
		|| (id == NULL) 
		|| (id->id_arr == NULL) 
		|| (id->id_arr->array == NULL))
	{
		return GXCORE_ERROR;
	}

	if (id->id_arr->manage_mode == MANAGE_SAME_LIST)
	{
		for (i=0; i<id->id_arr->totalCnt; i++)
		{
			if (id->id_arr->array[i] != DB_ID_UNEXIST) 
			{
				pos_buf[j] = i;
			}
		}
		return GXCORE_SUCCESS;
	}
	else if (id->id_arr->manage_mode == MANAGE_DIFF_LIST)
	{
		for (i=0; i<id->id_arr->realCnt; i++)
		{
			pos_buf[j] = i;
		}
		return GXCORE_SUCCESS;
	}

	return GXCORE_ERROR;
}

status_t get_id_array(AppIdOps* id_ops, uint32_t *id_buf)
{
	int32_t i;
	int32_t j = 0;

	IdManage *id = cls_to_obj(id_ops);

	if ((id_buf == NULL)
		|| (id == NULL) 
		|| (id->id_arr == NULL) 
		|| (id->id_arr->array == NULL))
	{
		return GXCORE_ERROR;
	}

	if (id->id_arr->manage_mode == MANAGE_SAME_LIST)
	{
		for (i=0; i<id->id_arr->totalCnt; i++)
		{
			if (id->id_arr->array[i] != DB_ID_UNEXIST) 
			{
				id_buf[j++] = id->id_arr->array[i];
			}
		}
		return GXCORE_SUCCESS;
	}
	else if (id->id_arr->manage_mode == MANAGE_DIFF_LIST)
	{
		memcpy(id_buf, id->id_arr->array, id->id_arr->realCnt*sizeof(int32_t));
	}
	else {}

	return GXCORE_ERROR;
}


uint32_t get_id_total(AppIdOps* id_ops)
{
	IdManage *id = cls_to_obj(id_ops);

	if ((id == NULL) 
		|| (id->id_arr == NULL)
		|| (id->id_arr->array == NULL))
	{
		return 0;
	}

	return id->id_arr->realCnt;
}

AppIdOps* new_id_ops(void)
{
    IdManage* p = NULL;

    if (GxIdManage == NULL)
    {
        GxIdManage = GxCore_Malloc(sizeof(IdManage));
        if (GxIdManage == NULL)  return NULL;

        GxIdManage->prior = NULL;
        GxIdManage->next = NULL;
        p = GxIdManage;
    }
    else
    {
        IdManage* pri = NULL;

        p = GxIdManage;
        while(p->next != NULL)
        {
            p = p->next;
        }
            
        p->next = GxCore_Malloc(sizeof(IdManage));
        if (p->next == NULL)    return NULL;

        pri = p;
        p = p->next;
        p->prior = pri;
        p->next = NULL;
    }
    
    p->id_ops = GxCore_Malloc(sizeof(AppIdOps));
    if (p->id_ops == NULL)
    {
        GxCore_Free(p->next);
        return NULL;
    }

    p->id_arr = GxCore_Malloc(sizeof(IdArray));
    if (p->id_arr == NULL)
    {
        GxCore_Free(p->id_ops);
        GxCore_Free(p->next);
        return NULL;
    }

    memcpy(p->id_ops, &g_AppIdOps, sizeof(AppIdOps));
    memset(p->id_arr, 0, sizeof(IdArray));

    return p->id_ops;
    
}

void del_id_ops(AppIdOps* id_ops)
{
    IdManage* p = NULL;

    close_ids_array(id_ops);

    p = cls_to_obj(id_ops);
    if (p == NULL)  return;
   
    if (p->prior == NULL)
    {
        if (p->next != NULL)
        {
            GxIdManage = p->next;
            GxIdManage->prior = NULL;
        }
        else
        {
            // first & no next
            GxIdManage = NULL;
        }
    }
    else if (p->next == NULL)
    {
        p->prior->next = NULL; 
    }
    else
    {
        p->prior->next = p->next;
        p->next->prior = p->prior;
    }
    
    GxCore_Free(p->id_ops);
    GxCore_Free(p->id_arr);
    GxCore_Free(p);
}


AppIdOps g_AppIdOps = {
	.id_open = open_ids_array,
	.id_add = add_id_into_array,
	.id_delete = del_from_array,
	.id_close = close_ids_array,
	.id_check = check_id_exist,
	.id_pos_get = get_id_pos_array,
	.id_get = get_id_array,
	.id_total_get = get_id_total,
};

#endif

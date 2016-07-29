#ifndef __APP_DATA_TYPE_H__
#define __APP_DATA_TYPE_H__


#include "module/pm/gxprogram_manage_berkeley.h"


/**
 * node type
 */
typedef enum
{
	NODE_SAT,
	NODE_TP,
	NODE_TP_ALL,
	NODE_PROG,
	NODE_CAS,
	NODE_FAV_GROUP,
}GxNodeType;
#ifdef ECOS_OS
#define DEFAULT_DB_PATH	NULL
#else
#define DEFAULT_DB_PATH	"/home/gx/default_data.db"
#endif
#define TIMEOUT_PAT 5000
#define TIMEOUT_PMT 7000
#define TIMEOUT_SDT 7000
#define TIMEOUT_NIT 5000
/**
 * node type
 */
typedef enum
{
	CHK_TP_IN_SAT,
	NODE_PROG_IN_TP,
}GxCheckType;

/**
 * use the node type to get the right node number
 */
typedef struct
{
	GxNodeType node_type;
	uint32_t node_num;
	uint16_t sat_id;
}GxMsgProperty_NodeNumGet;

/**
 * use node pos or node id to get the right node
 */
typedef struct
{
	GxNodeType node_type;
	union{
		uint32_t pos; // when use position , default number value is 1
		uint32_t id;
	};
	union{
		GxBusPmDataSat		    sat_data;
		GxBusPmDataTP  		    tp_data;
		GxBusPmDataProg 	    prog_data;
		GxBusPmDataCasInfot 	cas_info;
		GxBusPmDataFavItem  	fav_item;
	};	
}GxMsgProperty_NodeByPosGet, GxMsgProperty_NodeByIdGet;


typedef struct
{
	uint16_t tp_id;
	uint16_t ts_id; 
	uint16_t service_id;
	uint16_t original_id;
	uint16_t tuner;
}GxMsgProg;

typedef struct
{
	uint16_t sat_id;
	uint16_t fre;
	uint16_t symbol;
	GxBusPmTpPolar polar;
	uint16_t tp_id;
}GxMsgTp;


typedef struct
{
	GxCheckType chkType;
	union{
		GxMsgTp tpData;
		GxMsgProg progData;
	};
}GxChkData;

/**
 * use node pos or node id to get the right node
 */
typedef struct
{
	GxNodeType node_type;
	uint32_t pos;
	uint32_t number;
	union{
		GxBusPmDataSat		    *sat_array;
		GxBusPmDataTP  		    *tp_array;
		GxBusPmDataProg 	    *prog_array;
		GxBusPmDataCasInfot 	*cas_array;
		GxBusPmDataFavItem  	*fav_array;
	};	
}GxMsgProperty_MultiNodeByPosGet;

/**
 * use the node type to operate the right node
 */
typedef struct
{
	GxNodeType node_type;
	union{
		GxBusPmDataSat		    sat_data;
		GxBusPmDataTP  		    tp_data;
		GxBusPmDataProg 	    prog_data;
		GxBusPmDataFavItem  	fav_item;
	};	
}GxMsgProperty_NodeAdd, GxMsgProperty_NodeModify;

/**
 * use the node type to delete the right node which id is in id_array
 */
typedef struct
{
	GxNodeType node_type;	// only support NODE_SAT, NODE_TP, NODE_PROG
	uint32_t *id_array;	// need alloc the id buffer by app & remember to GxCore_Free it 
	uint32_t num;
}GxMsgProperty_NodeDelete;

/**
 * use the node type to delete the right node which id is in id_array
 */
typedef struct
{
	GxNodeType node_type;	// only support NODE_PROG
	uint32_t *id_array;	// need alloc the id buffer by app & remember to GxCore_Free it 
	uint32_t num;
	uint32_t target_id;	// move the id in id_array ahead of target_id
}GxMsgProperty_NodeMove;



/**
 * get PM 's url
 */
typedef struct
{
	GxBusPmDataProg	prog_info;
	int8_t *url;	// need alloc memory by application 
	uint32_t size;	//indicate the size of url 
}GxMsgProperty_GetUrl;

/**
 * copy all data from src tuner to dst tuner
 */
typedef struct 
{
	GxBusPmDataSatTuner src_tuner;
	GxBusPmDataSatTuner dst_tuner;
}GxMsgProperty_TunerCopy;


/**
 * get PM 's url by id or position
 */
typedef struct
{
	union{
		uint32_t pos; // when use position , default number value is 1
		uint32_t id;
	};
	int8_t *url;	// need alloc memory by application 
	uint32_t size;	//indicate the size of url 
}GxMsgProperty_byPosGetUrl,GxMsgProperty_byIDGetUrl;


/**
 * play the PM  by id or position
 */
typedef struct
{
	union{
		uint32_t pos; // when use position , default number value is 1
		uint32_t id;
	};
}GxMsgProperty_byPosPlayPM,GxMsgProperty_byIDPlayPM;

/** * get PM 's time */
typedef struct
{	
	int8_t *player_name;		
	uint64_t cur_time;
	uint64_t total_time;
}GxMsgProperty_GetPlayTime;

typedef struct
{
	uint32_t sel_in_all;
	uint32_t sel_in_t1;
	uint32_t sel_in_t2;
}SatSelStatus;

#endif


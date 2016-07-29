/*
 * mad_cfg.h
 *
 *  Created on: 2016-6-23
 *      Author: Brant
 */

#ifndef _M_AD_CFG_H_
#define _M_AD_CFG_H_

/*
 * Add header file which you need always include here.
 */
//#include "usifext.h"


/***********************************************************************************
 *                                   constant                                      *
 ***********************************************************************************/
#define M_AD_SYS_VERSION				(1)

#define M_AD_HEADER						("M-AD")

#define M_AD_DEBUG_INFO					(1)
#define M_AD_DEBUG_WARNING				(2)
#define M_AD_DEBUG_ERROR				(3)
#define M_AD_DEBUG_NONE					(4)


/***********************************************************************************
 *                                     config                                      *
 ***********************************************************************************/
#ifndef ENABLE_M_AD_TS
#define ENABLE_M_AD_TS					(1)
#endif
#ifndef ENABLE_M_AD_NET
#define ENABLE_M_AD_NET					(0)
#endif

#define ENABLE_TEST_M_AD_TS				(0)
#define ENABLE_TEST_M_AD_NET			(0)

#define M_AD_PLACE_NUM					(9)

#define M_AD_DEBUG_LEVEL				(M_AD_DEBUG_INFO)

#define M_AD_TS_RUN_IN_BKG				(1)



/***********************************************************************************
 *                                  auto define                                    *
 ***********************************************************************************/
#ifndef ENABLE_M_AD
#define ENABLE_M_AD			(ENABLE_M_AD_TS || ENABLE_M_AD_NET)
#endif
#define ENABLE_TEST_M_AD	(ENABLE_TEST_M_AD_TS || ENABLE_TEST_M_AD_NET)

#define ENABLE_DEBUG_INF	(M_AD_DEBUG_LEVEL<=M_AD_DEBUG_INFO)
#define ENABLE_DEBUG_WRN	(M_AD_DEBUG_LEVEL<=M_AD_DEBUG_WARNING)
#define ENABLE_DEBUG_ERR	(M_AD_DEBUG_LEVEL<=M_AD_DEBUG_ERROR)
#define ENABLE_DEBUG_M_AD	(ENABLE_DEBUG_INF||ENABLE_DEBUG_WRN||ENABLE_DEBUG_ERR)

#if ENABLE_DEBUG_INF
#define MAD_INF(fmt, args...)		printf("MAD INF: "fmt, ##args)
#else
#define MAD_INF(fmt, args...)
#endif

#if ENABLE_DEBUG_WRN
#define MAD_WRN(fmt, args...)		printf("MAD WRN: "fmt, ##args)
#else
#define MAD_WRN(fmt, args...)
#endif

#if ENABLE_DEBUG_ERR
#define MAD_ERR(fmt, args...)		printf("MAD ERR: "fmt, ##args)
#else
#define MAD_ERR(fmt, args...)
#endif

#endif /* _M_AD_CFG_H_ */

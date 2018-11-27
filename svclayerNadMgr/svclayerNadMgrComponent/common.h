/*===================================================

                  COMMON HEADER


GENERAL DESCRIPTION




Copyright (c) 2018 by Digen, Incorporated.  All Rights Reserved.
Digen Confidential and Proprietary.
=====================================================*/

/*====================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------


*/

#ifndef __COMMON_H
#define __COMMON_H

#include "legato.h"

#include "cust.h"
#include "interfaces.h"




#define TELEPHONY_MGR_VERSION "0000000"

#define ECALL_TYPE		0
#define BCALL_TYPE		1
#define CCALL_TYPE		2
#define NCALL_TYPE		0xff

#ifdef FEATURE_SIERRA_MODEM

#define CALL_END 				LE_MCC_EVENT_TERMINATED
#define CALL_SEUTP				LE_MCC_EVENT_SETUP
#define CALL_DIALING			LE_MCC_EVENT_ORIGINATING
#define CALL_INCOMMING			LE_MCC_EVENT_INCOMING
#define CALL_ALERTING			LE_MCC_EVENT_ALERTING
#define CALL_ACTIVE				LE_MCC_EVENT_CONNECTED
#define CALL_HOLDING			LE_MCC_EVENT_ON_HOLD
#define CALL_WAITING			LE_MCC_EVENT_ON_HOLD
#define CALL_IDLE				CALL_END

#else
#define CALL_END				0  // CALL_END
#define CALL_DIALING			1
#define CALL_INCOMMING			2
#define CALL_ALERTING			3
#define CALL_ACTIVE				4
#define CALL_HOLDING			5
#define CALL_WAITING			6
#define CALL_IDLE		CALL_END
#endif





#define FALSE 		0
#define TRUE 		1



typedef unsigned char byte;
typedef signed int BOOL;
typedef uint8_t uint8;

#define EXCEPTION_INIT \
uint32_t err_code = 0; \
BOOL err_catch = FALSE;

#define THROW() \
{\
  err_code = __LINE__; \
  goto _error;\
}

#define CATCH_ERR()\
  if( 0 ) \
  { \
  goto _error;\
  _error: \
    err_catch = TRUE; \
    LOGI( "%s ERR >> %d\n",__FILE__, (int)err_code );\
  } \
  if(err_catch)



#define TCU_LOG 	0x00
#define MCU_LOG		0x01
#define AVN_LOG		0x02


#ifdef FEATURE_DG_LOG_MSG
#define LOGI(...) \
            logSysLog(TCU_LOG,__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOGI(fmt,...) \
		printf("%s : %s ..%d : "fmt"\n",__TIME__,__FILE__, __LINE__,##__VA_ARGS__)
#endif

#ifdef FEATURE_DG_LOG_MSG
#define LOGM(fmt,...) \
            logSysLog(MCU_LOG,__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOGM(fmt,...) \
		printf("%s : %s ..%d : "fmt"\n",__TIME__,__FILE__, __LINE__,##__VA_ARGS__)
#endif

#ifdef FEATURE_DG_LOG_MSG
#define LOGA(fmt,...) \
            logSysLog(AVN_LOG,__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOGA(fmt,...) \
		printf("%s : %s ..%d : "fmt"\n",__TIME__,__FILE__, __LINE__,##__VA_ARGS__)
#endif

#if !defined(ARR_SIZE)
#define  ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) )
#endif


//AVN <-> TCU string max count
#define MAX_STRING_CNT	200

#endif //__COMMON_H



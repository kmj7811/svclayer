/*===================================================

                  MESSAGE INTERFACE HEADER


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

#ifndef __MSG_IF_H
#define __MSG_IF_H

#include "common.h"

////JUST FOR TEST /////////////
#define NAD2IF_MSGQ_KEY					0x5000
#define IF2NAD_MSGQ_KEY					0x5001

//Msg Q data type
#define NAD_TO_IF_TYPE						0x5000
#define IF_TO_NAD_TYPE						0x5001


////////////////////////////////////////////
#define MAX_DIR		20
typedef struct
{
  byte call_type;
}start_call_type_t;
typedef struct
{
  byte call_status;
  byte number[MAX_DIR];
}call_status_t;
typedef struct
{
  byte fail_reason;
}call_failure_t;
typedef struct
{
  byte status;
}nw_status_t;
typedef struct
{
  byte collide;
}collide_info_t;
typedef struct
{
  double latitude;
  double longitude;
  int altitude;
  int heading;
  int gpsSpeed;
  int gpsFix;
  byte gpsType;  //0: WGS84, 1: BESSEL
}gps_info_indi_t;
///////////////////////////////////////////
typedef struct
{
	long  data_type;
	byte  cmd;
	union {
		start_call_type_t start_call;
		call_status_t call_status;
		call_failure_t call_failure;
		nw_status_t nw_status;
		collide_info_t collide;
		gps_info_indi_t gps_info;

	}data;

}msg_t;
///////////////////////////////



typedef enum {
	START_CALL_REQ =0x00,
	CALL_STATUS_INDI,
	CALL_END_INDI,
	CALL_FAILURE_INDI,

	DATA_NW_STATUS_INDI = 0x30,

	GPS_INFO_INDI = 0x60,

    APP_NAD_CALL_STATUS = 0xC0,

	COLLIDE_INFO_INDI = 0xF0,

    ID_MAX /* MAX */
}cmd_type;




/////////////////////////////////////////////////////////////
#define CALL_STATUS_INCOMMING			0x01
#define CALL_STATUS_ORIGIN				0x02
#define CALL_STATUS_RING				0x03
#define CALL_STATUS_CONNECTED			0x04
#define CALL_STATUS_TERMINATED			0x05
/////////////////////////////////////////////////////////////

#endif //__MSG_IF_H



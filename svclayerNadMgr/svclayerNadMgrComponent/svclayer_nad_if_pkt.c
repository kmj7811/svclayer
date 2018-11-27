/*===================================================

                  SVCLAYER NAD TO IF PACKET


GENERAL DESCRIPTION

  Process Msg from MCU  to TCU  IF MGR thread 
  Make Response command


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

/*=====================================================

                     INCLUDE FILES FOR MODULE

=====================================================*/
#include "svclayer_nad_if_pkt.h"
#include "svclayer_nad_if_pkt.h"

#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include "common.h"
#include "msg_if.h"


int send_msgq_nad_to_if(msg_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = NAD_TO_IF_TYPE;
  qid = msgget( (key_t)NAD2IF_MSGQ_KEY, IPC_CREAT | 0666);

  if(qid<0)
  {
    LOGI("send_msgq_to_appif msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_appif msgsnd error\n");
    ret = -1;
  }

  LOGI("send_msgq() key(%d) ret =%d",NAD2IF_MSGQ_KEY, ret);

  return ret;
}

/*
 typedef enum {
	START_CALL_REQ =0x00,
	CALL_STATUS_INDI,
	CALL_END_INDI,
	CALL_FAILURE_INDI,

	DATA_NW_STATUS_INDI = 0x30,

	GPS_INFO_INDI = 0x60,

    APP_NAD_CALL_STATUS = 0xC0,

	COLLIDE_INFO_INDI = 0xF0,

*/
BOOL send_start_call_indi(byte btn_type)
{
    int result = true;
    msg_t send_data;

    memset(&send_data, 0, sizeof(send_data));

    send_data.cmd = START_CALL_REQ;
    send_data.data.start_call.call_type = btn_type;
    LOGI("send a send_start_call_indi btn(%d)",btn_type);

    result = send_msgq_nad_to_if(send_data);
    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = false;
    }
    return result;
}

byte call_status;
byte number[MAX_DIR];


BOOL send_start_status_indi(byte call_status, char* dir)
{
    int result = true;
    msg_t send_data;
    char num[MAX_DIR];

    memset(&send_data, 0, sizeof(send_data));

    if(dir == NULL)
    {
    	LOGI("No dial Number");
    	result = false;
    	return result;
    }

    send_data.cmd = START_CALL_REQ;
    send_data.data.call_status.call_status = call_status;
    LOGI("send a send_start_status_indi status(%d)",call_status);

    result = send_msgq_nad_to_if(send_data);
    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = false;
    }
    return result;
}

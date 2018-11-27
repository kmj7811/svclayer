/*===================================================

                  PROCESS APP PACKET


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
#include "svclayer_process_nad_pkt.h"
#include "svclayer_process_nad_pkt.h"

#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include "common.h"
#include "msg_if.h"

typedef struct
{  
	cmd_type cmd;
	handler_type handler;
} cmd_processor_type;


static BOOL dummy(msg_t *rcv_data);
static BOOL process_start_call_req(msg_t *rcv_data);
static BOOL process_call_status_indi(msg_t *rcv_data);
static BOOL process_call_end_indi(msg_t *rcv_data);
static BOOL process_call_failure_indi(msg_t *rcv_data);
static BOOL process_nw_status_indi(msg_t *rcv_data);
static BOOL process_gps_location_info_indi(msg_t *rcv_data);
static BOOL process_collide_info_indi(msg_t *rcv_data);



cmd_processor_type handler_tbl[] =
{
    {START_CALL_REQ,        				process_start_call_req},
    {CALL_STATUS_INDI,        				process_call_status_indi},
    {CALL_END_INDI,			          		process_call_end_indi},
    {CALL_FAILURE_INDI,          			process_call_failure_indi},
    {DATA_NW_STATUS_INDI,         	 		process_nw_status_indi},
    {GPS_INFO_INDI,          				process_gps_location_info_indi},
    {COLLIDE_INFO_INDI,          			process_collide_info_indi},
    {ID_MAX,          						dummy}
};



BOOL process_start_call_req(msg_t *rcv_data)
{
    BOOL res = true;
    byte btn_type = 0;

    LOGI("Received CALL BTN Indi");

    if(rcv_data == NULL)
    {
    	LOGI("No Rcv Data");
    	return false;
    }

    btn_type = rcv_data->data.start_call.call_type;
    LOGI("BTN call type is %d", btn_type);
    return res;
}

BOOL process_call_status_indi(msg_t *rcv_data)
{
    BOOL res = true;
    byte call_status;
    char dir[MAX_DIR];

    LOGI("Received Call Status Indi");

    if(rcv_data == NULL)
    {
    	LOGI("No Rcv Data");
    	return false;
    }
    memset(dir,0,sizeof(dir));

    call_status = rcv_data->data.call_status.call_status;
    memcpy(dir,rcv_data->data.call_status.number,MAX_DIR);

    LOGI("Rcv call status is %d", call_status);
    LOGI("Rcv call dir is %s", dir);

    return res;
}


BOOL process_call_end_indi(msg_t *rcv_data)
{
    BOOL res = true;
    return res;
}
BOOL process_call_failure_indi(msg_t *rcv_data)
{
    BOOL res = true;
    return res;
}
BOOL process_nw_status_indi(msg_t *rcv_data)
{
    BOOL res = true;
    return res;
}
BOOL process_gps_location_info_indi(msg_t *rcv_data)
{
    BOOL res = true;
    return res;
}
BOOL process_collide_info_indi(msg_t *rcv_data)
{
    BOOL res = true;
    return res;
}
static BOOL dummy(msg_t *rcv_data)
{
  LOGI("dummy called");
  return TRUE;
}


handler_type handler_get(cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(handler_tbl); i++)
  {
    if(cmd == handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}



//Process the packet from APP side
int process_nadMsg_pkt(msg_t* pkt)
{
    EXCEPTION_INIT
    BOOL ret = 0;

    handler_type handler = NULL;
	
    if(pkt == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("APP Pkt CMDL(0x%x) Received",pkt->cmd);

	
    handler = handler_get(pkt->cmd);
    if(!handler)
    {
      LOGI("%s", "can't process cmd since there is no handler to process it\n");
      THROW();
    }
    handler(pkt);

#endif

    CATCH_ERR()
    {
        ret = FALSE;
    }    
    
    
    return ret;	
}

int send_msgq_if_to_nad(msg_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = IF_TO_NAD_TYPE;
  qid = msgget( (key_t)IF2NAD_MSGQ_KEY, IPC_CREAT | 0666);

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

  LOGI("send_msgq() key(%d) ret =%d",IF2NAD_MSGQ_KEY, ret);

  return ret;
}
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


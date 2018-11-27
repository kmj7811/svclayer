/*===================================================

                  PROCESS AVN PACKET H


GENERAL DESCRIPTION

  Process Msg from MCU  to TCU  IF MGR  thread 
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

#ifndef __PROCESS_APP_PKT_H
#define __PROCESS_APP_PKT_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"
#include "common.h"

#define DEFAULT_PKT_LENGTH	6   //sof + cmdidh + cmdidl + lengthH + lengthL + eof

typedef BOOL (*handler_type)(msg_t *rcv_data);
int process_nadMsg_pkt(msg_t* pkt);


#endif //__PROCESS_APP_PKT_H



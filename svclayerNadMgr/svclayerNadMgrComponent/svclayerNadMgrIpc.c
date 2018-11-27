/*===================================================

                  SERVICE LAYER NAD MGR IPC File


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

#include "legato.h"
#include "interfaces.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "svclayerNadMgrIpc.h"
#include "stCall.h"

bool process_call_start_req(const char* message)
{
	bool res = true;
	char dial_string[MAX_DIAL_STRING];

	if( message == NULL)
	{
		res = false;
        LE_INFO("No Dial Digit");
		return res;
	}

	memcpy(dial_string,message,MAX_DIAL_STRING);
	res = st_call_MOCall(dial_string);

	if(res == false)
	{
		LE_INFO("Fail to Orig Call");
	}
	return res;
}
bool process_call_accep_req()
{
	bool res = true;
	LE_INFO("process_call_accep_req");
	st_call_Accept();
	return res;

}
bool process_call_end_req()
{
	bool res = true;
	LE_INFO("process_call_end_req");
	res =  st_call_hangup();
	return res;

}
void process_svclayerIfMsg(int value, const char* message)
{
	bool res = true;
    switch(value)
	{
        case CALL_START_REQ:
        	res = process_call_start_req(message);
        	if(res == false)
        	{
        		//Send Call failure indicator to APP
        		LE_INFO("Send Fail Indicator to APP");
        	}
        	break;
        case CALL_ACCEPT_REQ:
        	process_call_accep_req();
        	break;
        case CALL_END_REQ:
        	process_call_end_req();
        	break;
        default:
        	break;
	}
}
le_result_t svclayerNad_SetCommand1
(
    int32_t value1
)
{
	LE_INFO("test_API_SendCommand1 command : %d",value1);

	if(value1 == 1)
	{
		LE_INFO(" value 1 command");
		//send_test_data_one();
	}
	if(value1 == 2)
	{
		LE_INFO(" value 2 command");
		//send_test_data_two();
	}
	return LE_OK;
}
le_result_t svclayerNad_SetCommand2
(
	const char* LE_NONNULL message
)
{
	LE_INFO("test_API_SendCommand2 command : %s",message);

	return LE_OK;
}

/// For Call related IPC
le_result_t svclayerNad_SetCommand3
(
	int32_t value1,
	const char* LE_NONNULL message
)
{
	LE_INFO("svclayerNad_SetCommand3 command(0x%x) string: %s",value1, message);
	process_svclayerIfMsg(value1, message);

	return LE_OK;
}
le_result_t svclayerNad_GetCommand1
(
    char* message, size_t messageSize
)
{

#if 1
	char temp[100];
	int i = 2;
	memset(temp,0,100);
	sprintf(temp,"test num %d",i);
	LE_INFO("Tx String is %s",temp);


	memcpy(message, temp, 100);
#endif
	LE_INFO("unit_Test_GetCommand1");
	return LE_OK;
}




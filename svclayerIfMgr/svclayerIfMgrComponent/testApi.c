/*===================================================

                  Unit Test Reciever File


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


#include "svclayerIfMgrComponent.h"
//==================================================
//app runProc svclayerIfMgr unitTestAPI --exe=unitTestAPI
//app runProc svclayerIfMgr unitTestAPI --exe=unitTestAPI -- 1
//            app name  proc name(executable)
//==================================================



le_result_t test_API_SendCommand1
(
    int32_t value1
)
{
	LE_INFO("test_API_SendCommand1 command : %d",value1);

	if(value1 == 1)
	{
		LE_INFO(" value 1 command");
		//send_test_data_one();
		test_api();
	}
	if(value1 == 2)
	{
		LE_INFO(" value 2 command");
		test_api2();
	}
	if(value1 == 3)
	{
		LE_INFO(" value 3 command");
		test_api3();
	}
	if(value1 == 4)
	{
		LE_INFO(" value 4 command");
		test_api4();
	}
	if(value1 == 5)
	{
		LE_INFO(" value 5 command");
		test_api5();
	}
	return LE_OK;
}

le_result_t test_API_GetCommand1
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


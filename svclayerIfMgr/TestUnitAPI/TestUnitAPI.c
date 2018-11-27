#include "legato.h"
#include "interfaces.h"

COMPONENT_INIT
{
	LE_INFO("TEST  API");
	size_t argNum = le_arg_NumArgs();

	int i=0;
	char value[100] ;
	char temp[100];

	memset(value,0,100);
	memset(temp,0,100);

	const char * arg[3]={NULL,NULL,NULL};

	LE_INFO("stTestComp argNum[%d]",argNum);

	if( argNum > 3)
	{
		LE_ERROR("Too many arg");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < argNum ; i++)
	{
		arg[i] = le_arg_GetArg(i);
		LE_INFO("unitTestComp arg [%s]",arg[i]);
	}

	//le_result_t unit_Test_API_GetCommand1	( char* message,    ///< [OUT]	 size_t messageSize	    ///< [IN]);
	//le_result_t unit_Test_API_SendCommand1(  int32_t value1      ///< [IN]);
	if( argNum == 0)
	{
		test_API_GetCommand1(value,100);
		sprintf(temp,"Rcvd String value from App is %s\n",value);
		printf("----- %s\n", value);
	}
	else if( argNum == 1 )
	{
		test_API_SendCommand1(atoi(arg[0]));
	}

	exit(EXIT_SUCCESS);
}

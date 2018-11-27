/*
 * stDataConnect.c
 *
 *  Created on: 2018. 8. 29.
 *      Author: Gwanghee Lee
 */


#include "legato.h"
#include "interfaces.h"
#include "stDataConnect.h"
#include "stNet.h"

#define LE_PROFILE_INDEX	1


#define PLMN_INFO_SIZE		50
static char 				plmn_info[PLMN_INFO_SIZE]={0};

//static le_mdc_ProfileRef_t _leProfileRef;

void st_data_SetAPN(le_mdc_ProfileRef_t inProfileRef);

static void st_DataConnectionStateHandler
(
    const char *intfName,
    bool isConnected,
    void *contextPtr
);

static le_data_RequestObjRef_t _ConnectionRef=NULL;

void st_get_Home_MccMnc()
{
	char mcc[PLMN_INFO_SIZE]={0};
    char mnc[PLMN_INFO_SIZE]={0};

	le_sim_GetHomeNetworkMccMnc(LE_SIM_EXTERNAL_SLOT_1, mcc, LE_MRC_MCC_BYTES,
	                                      mnc, LE_MRC_MNC_BYTES);
	sprintf(plmn_info,"%s%s",mcc,mnc);
	LE_INFO("mccmnc: %s",plmn_info);
}
void st_Data_Init()
{
	LE_INFO("--------------Data Init-------------------");
	//temp open for test
    le_data_SetCellularProfileIndex(LE_PROFILE_INDEX);
    ///////////////////////

#if 0 //아래 사용 하게 되면 모뎀에 기 저장된 APN 을 override ..lte-internet.sktelecom.com 으로 override 한다..
	st_get_Home_MccMnc();

    _leProfileRef = le_mdc_GetProfile(LE_PROFILE_INDEX);

	st_data_SetAPN(_leProfileRef);
#endif

	le_data_AddConnectionStateHandler(&st_DataConnectionStateHandler, NULL);
	st_Data_le_Connect();
}




static void st_DataConnectionStateHandler
(
    const char *intfName,
    bool isConnected,
    void *contextPtr
)
{

    if (isConnected)
    {
        //WaitingForConnection = false;
        LE_INFO("Interface %s connected.", intfName);
        if( strlen(intfName) != 0 && intfName != NULL)
        {
        }
    }
    else
    {
        LE_INFO("Interface %s disconnected.", intfName);
        //st_app_ResultByDataMng(ST_APP_IPC_RSP_CATEGORY_PDP_RESULT,ST_APP_IPC_RSP_RESULT_DISCONNECTED,0,"");
    }
}



bool st_Data_le_Connect()
{
    le_data_SetTechnologyRank(0,LE_DATA_CELLULAR);

#if 0
	if( _ConnectionRef != NULL)
	{
		LE_ERROR("st_Data_StartConnection Already Connected");
		return true;
	}
#endif
	//if( techType == LE_DATA_CELLULAR )
	{
		LE_INFO("le_data_Request");

	    _ConnectionRef = le_data_Request();
	    if( _ConnectionRef != NULL)
	    {
	    	//LE_INFO("FINISH CONNECTING");
	    	return true;
	    }
	}

	return false;
}

bool st_Data_Disconnect()
{
	if( _ConnectionRef == NULL )
	{
	    LE_INFO("st_Data_Disconnect : Already Disconnected");
	    st_DataConnectionStateHandler("",false,NULL);
	}
	else
	{
	    LE_INFO("st_Data_Disconnect : DISCONNECTING");
	    le_data_Release(_ConnectionRef);
	    _ConnectionRef=NULL;

	    LE_INFO("st_Data_Disconnect : Disconnected");
	}

	return true;
}

void st_data_SetAPN(le_mdc_ProfileRef_t inProfileRef)
{
	char tempApnString[100];
#if 0
	if( !strcmp(st_net_GetMncMcc(),MNCMCC_KOREA_KT))
	{
		le_mdc_SetAPN(inProfileRef, "lte.ktfwing.com");
		//le_mdc_SetPDP(inProfileRef, LE_MDC_PDP_IPV4);
	}
	else if( !strcmp(st_net_GetMncMcc(),MNCMCC_KOREA_SK))
		le_mdc_SetAPN(inProfileRef, "lte.sktelecom.com");
	else
	{
#if (RELEASE_VERSION ==1)
		le_mdc_SetAPN(inProfileRef, "bjhmgc01.clfu.njm2mapn"); // need to be update
#else
		le_mdc_SetAPN(inProfileRef, "3gnet"); // need to be update
#endif
		//le_mdc_SetPDP(inProfileRef, LE_MDC_PDP_IPV4V6);
	}
#endif

#if 1
	if( !strcmp(plmn_info,MNCMCC_KOREA_KT))
	{
		le_mdc_SetAPN(inProfileRef, "lte.ktfwing.com");
		//le_mdc_SetPDP(inProfileRef, LE_MDC_PDP_IPV4);
	}
	else if( !strcmp(plmn_info,MNCMCC_KOREA_SK))
		le_mdc_SetAPN(inProfileRef, "lte.sktelecom.com");
	else
	{
#if (RELEASE_VERSION ==1)
		le_mdc_SetAPN(inProfileRef, "bjhmgc01.clfu.njm2mapn"); // need to be update
#else
		le_mdc_SetAPN(inProfileRef, "3gnet"); // need to be update
#endif
		//le_mdc_SetPDP(inProfileRef, LE_MDC_PDP_IPV4V6);
	}
#endif

	le_mdc_GetAPN(inProfileRef, tempApnString, 100);
	LE_INFO("Set APN Info %s", tempApnString);
}


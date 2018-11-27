/*
 * ccuNet.c
 *
 *  Created on: Jul 13, 2017
 *      Author: topgun
 */

/*
 * STAPPNetwork.c
 *
 *  Created on: Jun 20, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "interfaces.h"
#include "stNet.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NETWORK_NAME_SIZE	50
#define NETWORK_TEMP_SIZE	200

//static char * st_net_ratBitMaskString(le_mrc_RatBitMask_t statePtr);
static const char * st_net_mrcRatString(le_mrc_Rat_t statePtr);
static const char * st_net_NetRegString(le_mrc_NetRegState_t statePtr);
static const char * st_net_CellNetStatusString(le_cellnet_State_t statePtr);
static const char * st_net_LteBandString(le_mrc_LteBandBitMask_t BandMask);

static le_mrc_BandBitMask_t _netBandMask;
static le_mrc_LteBandBitMask_t _netLteBandMaskCap;
static le_mrc_LteBandBitMask_t _netLteBandMaskPref;

static le_cellnet_State_t	_netCellnetStatus;
static le_mrc_NetRegState_t _netRegStatus;
static le_mrc_Rat_t			_netRatStatus;
static uint32_t 			_netRssiValue;
static bool					_netPossibleToUse;
static char 				_netNameDig[NETWORK_NAME_SIZE]={0};
static char 				_netNameStr[NETWORK_NAME_SIZE]={0};
static char 				_netTempRetBuffer[NETWORK_TEMP_SIZE];

//void st_net_SetApn(char * inMCC, char * inMNC);

char * st_net_GetMncMcc()
{
	return _netNameDig;
}


bool st_net_GetPossibleToUse()
{
	le_mrc_GetNetRegState(&_netRegStatus);

	if( _netRegStatus == LE_MRC_REG_HOME || _netRegStatus == LE_MRC_REG_ROAMING)
		_netPossibleToUse = true;
	else
		_netPossibleToUse = false;

	return _netPossibleToUse;
}

le_mrc_NetRegState_t st_net_GetRegStatus()
{
	//le_mrc_GetNetRegState(&_netRegStatus);

	return _netRegStatus;
}

uint32_t st_net_SignalStrength()
{

	//res = le_mrc_GetNetRegState(&state);
	le_mrc_GetSignalQual(&_netRssiValue);

	return _netRssiValue;
}

uint32_t st_net_GetRatStatus()
{
	LE_INFO("RAT : %s",st_net_mrcRatString(_netRatStatus));
	return _netRatStatus;
}

/****************************HANDLER************************************************/
static void st_net_CellNetHandler(le_cellnet_State_t state,void* contextPtr)
{
	_netCellnetStatus = state;
	LE_INFO("[NETWORK] STATUS : %s",st_net_CellNetStatusString(_netCellnetStatus) );
}

static void st_net_mrcNetRegHandler(le_mrc_NetRegState_t state,void* contextPtr)
{
	_netRegStatus = state;
	LE_INFO("st_net_mrcNetRegHandler : %s", st_net_NetRegString(_netRegStatus));
}

static void st_net_mrcRatHandler(le_mrc_Rat_t state,void* contextPtr)
{
	_netRatStatus = state;
	LE_INFO("%s", st_net_mrcRatString(_netRatStatus));
}

#if 0
static void st_net_mrcSignalStrengthHandler(int32_t state,void* contextPtr)
{
	LE_INFO("st_net_mrcSignalStrengthHandler RSSI : [%d]", state);
	//_netRssiValue = state;
}
#endif

static void Testle_mrc_GetSignalMetrics()
{
    le_result_t   res;
    le_mrc_Rat_t  rat;
    int32_t rxLevel = 0;
    uint32_t er = 0;
    int32_t ecio = 0;
    int32_t rscp = 0;
    int32_t sinr = 0;
    int32_t rsrq = 0;
    int32_t rsrp = 0;
    int32_t snr = 0;
    int32_t io = 0;

    le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();
    LE_ASSERT(metricsRef != NULL);

    rat = le_mrc_GetRatOfSignalMetrics(metricsRef);
    LE_INFO("RAT of signal metrics is %d",rat);
    switch(rat)
    {
        case LE_MRC_RAT_GSM:
            res = le_mrc_GetGsmSignalMetrics(metricsRef, &rxLevel, &er);
            LE_ASSERT(res == LE_OK);
            LE_INFO("GSM metrics rxLevel.%ddBm, er.%d", rxLevel, er);
            break;

        case LE_MRC_RAT_UMTS:
            res = le_mrc_GetUmtsSignalMetrics(metricsRef, &rxLevel, &er, &ecio, &rscp, &sinr);
            LE_ASSERT(res == LE_OK);
            LE_INFO("UMTS metrics rxLevel.%ddBm, er.%d, ecio.%010.1fdB, rscp.%ddBm, sinr.%ddB",
                    rxLevel, er, ((double)ecio/10), rscp, sinr);
            break;

        case LE_MRC_RAT_LTE:

            res = le_mrc_GetLteSignalMetrics(metricsRef, &rxLevel, &er, &rsrq, &rsrp, &snr);
            LE_ASSERT(res == LE_OK);
            LE_INFO("LTE metrics rxLevel.%ddBm, er.%d, rsrq.%010.1fdB, "
                    "rsrp.%010.1fdBm, snr.%010.1fdB",
                    rxLevel, er, ((double)rsrq/10), ((double)rsrp/10), ((double)snr/10));
            break;

        case LE_MRC_RAT_CDMA:
            res = le_mrc_GetCdmaSignalMetrics(metricsRef,  &rxLevel, &er, &ecio, &sinr, &io);
            LE_ASSERT(res == LE_OK);
            LE_INFO("CDMA metrics rxLevel.%ddBm, er.%d, ecio.%010.1fdB, "
                    "sinr.%ddB, io.%ddBm",
                    rxLevel, er, ((double)ecio/10), sinr, io);
            break;

        default:
            LE_FATAL("Unknown RAT!");
            break;
    }

    le_mrc_DeleteSignalMetrics(metricsRef);
}

void st_net_GetAllInformaion()
{
	//le_result_t           res;
	//le_cellnet_State_t statePtr;
    //le_mrc_TdScdmaBandBitMask_t bandTdscmaMask;
    //le_onoff_t powerPtr;


	/***********************************************************************/
	//le_mrc_NetRegState_t netRegStatus;
	le_mrc_GetNetRegState(&_netRegStatus);
	le_mrc_AddNetRegStateEventHandler(st_net_mrcNetRegHandler,NULL);
	/***********************************************************************/

	/***********************************************************************/
	//Cell net Info :
	le_cellnet_AddStateEventHandler(st_net_CellNetHandler,NULL);
	if( _netRegStatus == LE_MRC_REG_HOME)
		le_cellnet_GetNetworkState(&_netCellnetStatus);
	/***********************************************************************/

	/***********************************************************************/
	le_mrc_GetRadioAccessTechInUse(&_netRatStatus);
	if( _netRegStatus == LE_MRC_REG_HOME)
		le_mrc_AddRatChangeHandler(st_net_mrcRatHandler,NULL);
	/***********************************************************************/

	/***********************************************************************/
	//uint32_t _netRssiValue;

	le_mrc_GetSignalQual(&_netRssiValue);
	//le_mrc_AddSignalStrengthChangeHandler(ratPtr,0,0,st_net_mrcSignalStrengthHandler,NULL);
	/***********************************************************************/

	/***********************************************************************/
	le_mrc_GetCurrentNetworkName(_netNameStr, NETWORK_NAME_SIZE);

	char mcc[NETWORK_NAME_SIZE]={0};
    char mnc[NETWORK_NAME_SIZE]={0};
#if 1
	le_mrc_GetCurrentNetworkMccMnc(mcc,NETWORK_NAME_SIZE,mnc,NETWORK_NAME_SIZE);
	sprintf(_netNameDig,"%s%s",mcc,mnc);
#endif


	/***********************************************************************/
	if( _netRegStatus == LE_MRC_REG_HOME)
	{
		le_mrc_GetBandPreferences(&_netBandMask);

		le_mrc_GetLteBandCapabilities(&_netLteBandMaskCap);
		le_mrc_GetLteBandPreferences(&_netLteBandMaskPref);
		Testle_mrc_GetSignalMetrics();
	}

	/***********************************************************************/

	LE_INFO("NETWORK STATUS : %s",st_net_CellNetStatusString(_netCellnetStatus));
	LE_INFO("NETWORK TECH   : %s",st_net_mrcRatString(_netRatStatus));
	LE_INFO("NETWORK REGI   : %s",st_net_NetRegString(_netRegStatus));
	LE_INFO("RSSI           : %d",_netRssiValue);
	LE_INFO("NETNAME STRING : %s",_netNameStr);
	LE_INFO("NETNAME DIGIT  : %s (MCC : %s, MNC : %s)",_netNameDig,mcc,mnc);
	if( _netRegStatus == LE_MRC_REG_HOME)
	{
		LE_INFO("LteBandMask(c) : %s",st_net_LteBandString(_netLteBandMaskCap));
		LE_INFO("LteBandMask(p) : %s",st_net_LteBandString(_netLteBandMaskPref));
		//LE_INFO("BandMask       : %s",st_net_LteBandString(_netBandMask));
	}
}

/*
void st_net_SetApn(char * inMCC, char * inMNC)
{
	le_mdc_ProfileRef_t profileValue;
	uint32_t profileIndex;
	uint32_t numberOfProfiles = le_mdc_NumProfiles();

	le_mdc_GetProfileFromApn("lte.ktfwing.com",&profileValue);
	profileIndex = le_mdc_GetProfileIndex(profileValue);

	LE_DEBUG("le_mdc_NumProfiles [%d] getIndex[%d]",numberOfProfiles,profileIndex);

#if 0
	if( !strcmp(inMNC,"450") && !strcmp(inMCC,"08"))
	{
		le_mdc_SetAPN(profileRef, apnStr);
	}
#endif
}
*/

static const char * st_net_LteBandString(le_mrc_LteBandBitMask_t BandMask)
{
	memset(_netTempRetBuffer,0x00,NETWORK_TEMP_SIZE);

	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_1)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_1   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_2)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_2   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_3)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_3   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_4)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_4   ");
	}

	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_5)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_5   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_6)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_6   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_7)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_7   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_8)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_8   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_9)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_9   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_10)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_10   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_11)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_11   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_12)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_12   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_13)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_13   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_14)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_14   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_17)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_17   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_18)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_18   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_19)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_19   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_20)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_20   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_21)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_21   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_24)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_24   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_25)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_25   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_33)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_33   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_34)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_34   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_35)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_35   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_36)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_36   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_37)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_37   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_38)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_38   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_39)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_39   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_40)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_40   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_41)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_41   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_42)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_42   ");
	}
	if( BandMask & LE_MRC_BITMASK_LTE_BAND_E_UTRA_OP_BAND_43)
	{
		sprintf(&_netTempRetBuffer[strlen(_netTempRetBuffer)],"%s","LTE_BAND_43   ");
	}

	return _netTempRetBuffer;
}

static const char * st_net_CellNetStatusString(le_cellnet_State_t statePtr)
{
	switch(statePtr)
	{
	case LE_CELLNET_RADIO_OFF :
        return " LE_CELLNET_RADIO_OFF : The radio is switched-off.";

	case LE_CELLNET_REG_EMERGENCY :
        return "LE_CELLNET_REG_EMERGENCY : Only emergency calls are allowed.";

	case LE_CELLNET_REG_HOME :
        return "LE_CELLNET_REG_HOME : Registered, home network.";

	case LE_CELLNET_REG_ROAMING :
        return "LE_CELLNET_REG_ROAMING : Registered to a roaming network.";

	case LE_CELLNET_REG_UNKNOWN :
        return "LE_CELLNET_REG_UNKNOWN : Unknown state.";

	case LE_CELLNET_SIM_ABSENT :
        return "LE_CELLNET_SIM_ABSENT : No SIM card available.";

	default:
		break;
	}
	return "";
}

static const char * st_net_NetRegString(le_mrc_NetRegState_t statePtr)
{
	switch(statePtr)
	{
	case LE_MRC_REG_NONE :
        return "LE_MRC_REG_NONE";

	case LE_MRC_REG_HOME :
        return "LE_MRC_REG_HOME";

	case LE_MRC_REG_SEARCHING :
        return "LE_MRC_REG_SEARCHING";

	case LE_MRC_REG_DENIED :
        return "LE_MRC_REG_DENIED";

	case LE_MRC_REG_ROAMING :
        return "LE_MRC_REG_ROAMING";
	default:
		break;
	}
	return "LE_MRC_REG_UNKNOWN";
}

static const char * st_net_mrcRatString(le_mrc_Rat_t statePtr)
{
	switch(statePtr)
	{
	case LE_MRC_RAT_GSM :
        return "LE_MRC_RAT_GSM";

	case LE_MRC_RAT_UMTS :
        return "LE_MRC_RAT_UMTS";

	case LE_MRC_RAT_LTE :
        return "LE_MRC_RAT_LTE";

	case LE_MRC_RAT_CDMA :
        return "LE_MRC_RAT_CDMA";

	default:
		break;
	}
	return "LE_MRC_RAT_UNKNOWN";
}

#if 0 //NOT USED FUNCTION
static char * st_net_ratBitMaskString(le_mrc_RatBitMask_t statePtr)
{
	switch(statePtr)
	{
	case LE_MRC_BITMASK_RAT_GSM :
        return "LE_MRC_BITMASK_RAT_GSM";

	case LE_MRC_BITMASK_RAT_UMTS :
        return "LE_MRC_BITMASK_RAT_UMTS";

	case LE_MRC_BITMASK_RAT_CDMA :
        return "LE_MRC_BITMASK_RAT_CDMA";

	case LE_MRC_BITMASK_RAT_ALL :
        return "LE_MRC_BITMASK_RAT_ALL";

	default:
		break;
	}
	return "LE_MRC_RAT_UNKNOWN";
}
#endif

#ifdef __cplusplus
}
#endif


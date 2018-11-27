/*
 * stMobileCall.c
 *
 *  Created on: Aug 8, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "interfaces.h"
#include "stCall.h"
#include "stUartMgr.h"
#include "stUartHexCmd.h"

#define ACCEP_INCOMMiNG_CALL	1
static le_onoff_t ClirStatus = LE_ON;

le_mcc_CallRef_t _moCallRef;
le_mcc_CallRef_t _mtCallRef;

le_mcc_Event_t _callEvent=0x0FF;

static bool hanguping = false;

//Temp_audio
static le_audio_StreamRef_t MdmRxAudioRef = NULL;
static le_audio_StreamRef_t MdmTxAudioRef = NULL;
static le_audio_StreamRef_t FeInRef = NULL;
static le_audio_StreamRef_t FeOutRef = NULL;
//static le_audio_StreamRef_t FileAudioRef = NULL;
static le_audio_ConnectorRef_t AudioInputConnectorRef = NULL;
static le_audio_ConnectorRef_t AudioOutputConnectorRef = NULL;

char mtNum[20] = {0,};


static void MyCallEventHandler(le_mcc_CallRef_t  callRef,le_mcc_Event_t callEvent,void* contextPtr);
static void ConnectAudioToPcm();
static void connectToMic(); //Temp_audio

void st_call_init()
{
	LE_INFO("st_call_init");
	le_mcc_AddCallEventHandler(MyCallEventHandler, NULL);
	ConnectAudioToPcm();
	connectToMic();

}

bool st_call_getCallStatus()
{
	if( _moCallRef != NULL)
	{
		return true;
	}

	if( _mtCallRef != NULL)
	{
		return true;
	}

	return false;
}

static void MyCallEventHandler
(
    le_mcc_CallRef_t  callRef,
    le_mcc_Event_t callEvent,
    void* contextPtr
)
{
    //le_result_t         res;
    static bool firstConnectCall = true;
    le_mcc_TerminationReason_t term=0xFF;
    _callEvent = callEvent;
    LE_INFO("MyCallEventHandler [%d]",callEvent);

    if (callEvent == LE_MCC_EVENT_ALERTING)
    {
        LE_INFO("===================LE_MCC_EVENT_ALERTING.============================");
        if (firstConnectCall)
        {
            LE_INFO("---!!!! PLEASE CHECK ON THE REMOTE SIDE IF THE PHONE NUMBER IS %s !!!!---",
                            ((ClirStatus == LE_ON) ? "HIDED" : "DISPLAYED"));
            LE_INFO("---!!!! PLEASE HANG UP ON THE REMOTE SIDE !!!!---");
        }
    }
    else if (callEvent == LE_MCC_EVENT_CONNECTED)
    {
        LE_INFO("Check MyCallEventHandler passed, event is LE_MCC_EVENT_CONNECTED.");
        if (firstConnectCall)
        {
            LE_INFO("---!!!! PLEASE TERMINATE THE CALL on THE REMOTE SIDE !!!!---");
            firstConnectCall = false;
        }
    }
    else if (callEvent == LE_MCC_EVENT_TERMINATED)
    {
        LE_INFO("Check MyCallEventHandler passed, event is LE_MCC_EVENT_TERMINATED.");
        //st_uart_setCallStatus(D_TCU_AVN_DATA_NAD_CALL_STATUS_CALL_IDLE);

        hanguping = false;

        term = le_mcc_GetTerminationReason(callRef);
        int32_t code = le_mcc_GetPlatformSpecificTerminationCode(callRef);
        switch(term)
        {
            case LE_MCC_TERM_NETWORK_FAIL:
                LE_ERROR("Termination reason is LE_MCC_TERM_NETWORK_FAIL");
                //exit(EXIT_FAILURE);
                break;

            case LE_MCC_TERM_UNASSIGNED_NUMBER:
                LE_ERROR("Termination reason is LE_MCC_TERM_UNASSIGNED_NUMBER");
                //exit(EXIT_FAILURE);
                break;

            case LE_MCC_TERM_USER_BUSY:
                LE_ERROR("Termination reason is LE_MCC_TERM_USER_BUSY");
                //exit(EXIT_FAILURE);
                break;

            case LE_MCC_TERM_LOCAL_ENDED:
                LE_INFO("Termination reason is LE_MCC_TERM_LOCAL_ENDED");
                //LE_INFO("mccTest Sequence SUCCESS");
                //LE_INFO("mccTest test exit");
                //le_mcc_Delete(callRef);
                //exit(EXIT_SUCCESS);
                break;

            case LE_MCC_TERM_REMOTE_ENDED:
                LE_INFO("Termination reason is LE_MCC_TERM_REMOTE_ENDED");
                LE_INFO("---!!!! PLEASE CREATE AN INCOMING CALL !!!!---");
                break;

            case LE_MCC_TERM_NO_SERVICE:
                LE_INFO("Termination reason is LE_MCC_TERM_NO_SERVICE");
                //exit(EXIT_FAILURE);
                break;

            case LE_MCC_TERM_OPERATOR_DETERMINED_BARRING:
                LE_INFO("Termination reason is LE_MCC_TERM_OPERATOR_DETERMINED_BARRING");
                //exit(EXIT_FAILURE);
                break;

            case LE_MCC_TERM_UNDEFINED:
                LE_ERROR("Termination reason is LE_MCC_TERM_UNDEFINED");
                LE_ERROR("---!!!! PLEASE CREATE AN INCOMING CALL !!!!---");
                break;

            default:
                LE_ERROR("Termination reason is %d", term);
                //exit(EXIT_FAILURE);
                break;
        }
        //DisconnectAllAudio();
        LE_INFO("Termination code is 0x%X", code);
        if( _mtCallRef != NULL)
        {
            le_mcc_Delete(_mtCallRef);
            _mtCallRef = NULL;

        }
        if( _moCallRef != NULL)
        {
            le_mcc_Delete(_moCallRef);
            _moCallRef = NULL;

        }
    }
    else if (callEvent == LE_MCC_EVENT_INCOMING)
    {
        LE_INFO("LE_MCC_EVENT_INCOMING.Call Number");
        _mtCallRef = callRef;
        st_call_GetMTNum(mtNum);
        LE_INFO("--- > MT Num is %s", mtNum);
        LE_INFO("_mtCallRef is %p",_mtCallRef);

//#if (ACCEP_INCOMMING_CALL == 0)
//        le_mcc_HangUp(callRef);
//#else
//        le_mcc_Answer(callRef);
//#endif
    }
    else if (callEvent == LE_MCC_EVENT_ORIGINATING)
    {
        LE_INFO("Check MyCallEventHandler passed, event is LE_MCC_EVENT_ORIGINATING.");
    }
    else if (callEvent == LE_MCC_EVENT_SETUP)
    {
        LE_INFO("Check MyCallEventHandler passed, event is LE_MCC_EVENT_SETUP.");
    }
    else
    {
        LE_ERROR("Check MyCallEventHandler failed, unknowm event %d.", callEvent);
        hanguping = false;
    }

    st_uart_setCallStatus(callEvent,term);
}


bool st_call_MOCall(char * callNumber)
{
	le_result_t callret = LE_OK;
	LE_INFO("MO CALL : %s", callNumber );

	if( hanguping == true)
	{
		LE_ERROR("Already Call Disconnecting");
		return false;
	}

	if( _moCallRef != NULL)
	{
		LE_ERROR("Already Try to connect call");
		return false;
	}

	_moCallRef= le_mcc_Create(callNumber);
	if( _moCallRef == NULL)
	{
		LE_INFO("le_mcc_Create : NULL");
        _mtCallRef = NULL;
        _moCallRef = NULL;
        return false;
	}
	callret = le_mcc_Start(_moCallRef);
	LE_INFO("le_mcc_Start : %d [%s]",callret,callNumber);
	if( callret < 0)
	{
        _mtCallRef = NULL;
        _moCallRef = NULL;
        return false;
	}
	return true;
}

void st_call_GetMTNum(char * telPtr)
{
    le_mcc_GetRemoteTel(_mtCallRef,telPtr,18);
}

bool st_call_hangup()
{
	le_result_t ret = LE_OK;

	LE_INFO("Current Call Ref MO:%p MT:%p",_moCallRef,_mtCallRef);
	ret = le_mcc_HangUpAll();
	LE_INFO("st_call_hangup ret[%d]",ret);
	return true;
}

void st_call_Accept()
{
	if( _mtCallRef != NULL)
	{
	    le_mcc_Answer(_mtCallRef);
	    _mtCallRef = NULL;
	}
}

#if 1
void ConnectAudioToPcm
(
    void
)
{
    le_result_t res;

    MdmRxAudioRef = le_audio_OpenModemVoiceRx();
    LE_ERROR_IF((MdmRxAudioRef==NULL), "GetRxAudioStream returns NULL!");
    MdmTxAudioRef = le_audio_OpenModemVoiceTx();
    LE_ERROR_IF((MdmTxAudioRef==NULL), "GetTxAudioStream returns NULL!");

    // Redirect audio to the PCM interface.
    FeOutRef = le_audio_OpenPcmTx(0);
    LE_ERROR_IF((FeOutRef==NULL), "OpenPcmTx returns NULL!");
    FeInRef = le_audio_OpenPcmRx(0);
    LE_ERROR_IF((FeInRef==NULL), "OpenPcmRx returns NULL!");

    AudioInputConnectorRef = le_audio_CreateConnector();
    LE_ERROR_IF((AudioInputConnectorRef==NULL), "AudioInputConnectorRef is NULL!");
    AudioOutputConnectorRef = le_audio_CreateConnector();
    LE_ERROR_IF((AudioOutputConnectorRef==NULL), "AudioOutputConnectorRef is NULL!");

    if (MdmRxAudioRef && MdmTxAudioRef && FeOutRef && FeInRef &&
        AudioInputConnectorRef && AudioOutputConnectorRef)
    {
        res = le_audio_Connect(AudioInputConnectorRef, FeInRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect PCM RX on Input connector!");
        res = le_audio_Connect(AudioInputConnectorRef, MdmTxAudioRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect mdmTx on Input connector!");
        res = le_audio_Connect(AudioOutputConnectorRef, FeOutRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect PCM TX on Output connector!");
        res = le_audio_Connect(AudioOutputConnectorRef, MdmRxAudioRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect mdmRx on Output connector!");
    }
}
#endif

#if 0
void ConnectAudioToCodec
(
    void
)
{
    le_result_t res;

    MdmRxAudioRef = le_audio_OpenModemVoiceRx();
    LE_ERROR_IF((MdmRxAudioRef==NULL), "GetRxAudioStream returns NULL!");
    MdmTxAudioRef = le_audio_OpenModemVoiceTx();
    LE_ERROR_IF((MdmTxAudioRef==NULL), "GetTxAudioStream returns NULL!");

    // Redirect audio to the in-built Microphone and Speaker.
    FeOutRef = le_audio_OpenSpeaker();
    LE_ERROR_IF((FeOutRef==NULL), "OpenSpeaker returns NULL!");
    FeInRef = le_audio_OpenMic();
    LE_ERROR_IF((FeInRef==NULL), "OpenMic returns NULL!");

    AudioInputConnectorRef = le_audio_CreateConnector();
    LE_ERROR_IF((AudioInputConnectorRef==NULL), "AudioInputConnectorRef is NULL!");
    AudioOutputConnectorRef = le_audio_CreateConnector();
    LE_ERROR_IF((AudioOutputConnectorRef==NULL), "AudioOutputConnectorRef is NULL!");

    if (MdmRxAudioRef && MdmTxAudioRef && FeOutRef && FeInRef &&
        AudioInputConnectorRef && AudioOutputConnectorRef)
    {
        res = le_audio_Connect(AudioInputConnectorRef, FeInRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect Mic on Input connector!");
        res = le_audio_Connect(AudioInputConnectorRef, MdmTxAudioRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect mdmTx on Input connector!");
        res = le_audio_Connect(AudioOutputConnectorRef, FeOutRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect Speaker on Output connector!");
        res = le_audio_Connect(AudioOutputConnectorRef, MdmRxAudioRef);
        LE_ERROR_IF((res!=LE_OK), "Failed to connect mdmRx on Output connector!");
    }
}

#endif

void connectToMic()
{
	static le_audio_StreamRef_t     micAudioRef = NULL;
	static le_audio_ConnectorRef_t  audioInputConnectorRef = NULL;

	micAudioRef = le_audio_OpenMic();

	audioInputConnectorRef = le_audio_CreateConnector();
	le_audio_Connect(audioInputConnectorRef, micAudioRef);

}

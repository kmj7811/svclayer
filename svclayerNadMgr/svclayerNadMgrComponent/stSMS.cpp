/*
 * stSMS.c
 *
 *  Created on: Sep 19, 2017
 *      Author: ghlee
 */

#include "legato.h"
#include "interfaces.h"
#include "stSMS.h"

#define MAX_PHONE_NUM	30

#define TON_UNKNOWN			 0
#define TON_INTERNATIONAL	1
#define TON_NATIONAL		2
#define TON_NETWORK			3
#define TON_SUBSCRIBER		4
#define TON_ALPHANUMERIC	5
#define TON_ABBREVIATED		6

#define MAX_SMS_ADDR_DIG               (0x14)

#define BCD_ASTSK                      0xa         /* (                              */
#define BCD_PND                        0xb         /* #                              */
#define BCD_A                          0xc         /* a                              */
#define BCD_B                          0xd         /* b                              */
#define BCD_C                          0xe         /* c                              */
#define BCD_RES                        0xf         /* reserved                       */


static le_sms_RxMessageHandlerRef_t RxHdlrRef;
static le_sms_FullStorageEventHandlerRef_t FullStorageHdlrRef;

//--------------------------------------------------------------------------------------------------
/**
 * Handler function for SMS message reception.
 *
 */
//--------------------- -----------------------------------------------------------------------------
le_result_t st_sms_mtReceiver();
le_result_t st_sms_MonitorStorage();

void st_sms_Init()
{
    st_sms_mtReceiver();
    st_sms_MonitorStorage();
}

static void st_sms_rxMessageHandler(le_sms_MsgRef_t msgRef,void*contextPtr)
{
    le_result_t  res;
    le_sms_Format_t smsformat;
    char         tel[LE_MDMDEFS_PHONE_NUM_MAX_BYTES];
    char         timestamp[LE_SMS_TIMESTAMP_MAX_BYTES] = {0};

    char         textAsciiSms[LE_SMS_TEXT_MAX_BYTES] = {0};
    size_t		 textAsciiSmsLength=LE_SMS_TEXT_MAX_BYTES;

    uint16_t     textUcsSms[LE_SMS_UCS2_MAX_CHARS] = {0};
    size_t		 textUcsSmsLength=LE_SMS_UCS2_MAX_CHARS;

    uint8_t         textBinarySms[LE_SMS_BINARY_MAX_BYTES] = {0};
    size_t		 textBinarySmsLength=LE_SMS_BINARY_MAX_BYTES;

    uint8_t         textPDUSms[LE_SMS_PDU_MAX_BYTES] = {0};
    size_t		 textPDUSmsLength=LE_SMS_PDU_MAX_BYTES;

    LE_INFO("================================RECV NEW SMS===================================================");

    res = le_sms_GetSenderTel(msgRef, tel, sizeof(tel));
    if(res != LE_OK)
    {
        LE_ERROR("le_sms_GetSenderTel has failed (res.%d)!", res);
    }
    else
    {
        LE_INFO("Message is received from %s.", tel);
    }

    res = le_sms_GetTimeStamp(msgRef, timestamp, sizeof(timestamp));
    if(res != LE_OK)
    {
        LE_ERROR("le_sms_GetTimeStamp has failed (res.%d)!", res);
    }
    else
    {
        LE_INFO("Message timestamp is %s.", timestamp);
    }

    smsformat = le_sms_GetFormat(msgRef);

    if ( smsformat == LE_SMS_FORMAT_TEXT )
    {
        LE_INFO("A New SMS message is received(LE_SMS_FORMAT_TEXT)");

        res = le_sms_GetText(msgRef, textAsciiSms, textAsciiSmsLength);
        if(res != LE_OK)
            LE_ERROR("le_sms_GetText has failed (res.%d)!", res);

        LE_INFO("Message content: \"%s\"", textAsciiSms);
    }
    else if ( smsformat == LE_SMS_FORMAT_UCS2)
    {
        LE_INFO("A New SMS message is received(LE_SMS_FORMAT_UCS2)");

        res = le_sms_GetUCS2(msgRef, textUcsSms, &textUcsSmsLength);
        if(res != LE_OK)
            LE_ERROR("le_sms_GetText has failed (res.%d)!", res);

        LE_DUMP((const uint8_t *)textUcsSms,textUcsSmsLength);
    }
    else if ( smsformat == LE_SMS_FORMAT_BINARY)
    {
        LE_INFO("A New SMS message is received(LE_SMS_FORMAT_BINARY)");

        res = le_sms_GetBinary(msgRef, textBinarySms, &textBinarySmsLength);
        if(res != LE_OK)
            LE_ERROR("le_sms_GetBinary has failed (res.%d)!", res);

        LE_DUMP(textBinarySms,textBinarySmsLength);
    }
    else if ( smsformat == LE_SMS_FORMAT_PDU)
    {
        LE_INFO("A New SMS message is received(LE_SMS_FORMAT_PDU)");

        res = le_sms_GetPDU(msgRef, textPDUSms, &textPDUSmsLength);
        if(res != LE_OK)
            LE_ERROR("le_sms_GetBinary has failed (res.%d)!", res);

        LE_DUMP(textPDUSms,textPDUSmsLength);
    }

    else
    {
        LE_WARN("Warning! Unknow Schema Message! [%d]",smsformat);
        goto GOTO_FINISH_SMS;
    }

    LE_INFO("Message content: \"%s\"", textAsciiSms);

    GOTO_FINISH_SMS:

#if 1
    res = le_sms_DeleteFromStorage(msgRef);
    if(res != LE_OK)
        LE_ERROR("le_sms_DeleteFromStorage has failed (res.%d)!", res);
    else
        LE_INFO("the message has been successfully deleted from storage.");
#endif

    le_sms_Delete(msgRef);
}

//--------------------------------------------------------------------------------------------------
/**
 * This function simply sends a text message.
 *
 * @return LE_FAULT  The function failed.
 * @return LE_OK     The function succeed.
 */
//--------------------------------------------------------------------------------------------------
#define SMS_TYPE_BINARY

le_result_t st_sms_moSendMessage(uint8_t * inSendData, size_t inSendLength)
{
    le_result_t           res;
    le_sms_MsgRef_t      myMsg;

    le_sms_ConnectService();

    myMsg = le_sms_Create();

    if (!myMsg)
    {
        LE_ERROR("SMS message creation has failed!");
        return LE_FAULT;
    }

   	le_sms_SetTimeout(myMsg,10);

#if defined (SMS_TYPE_BINARY)

    res = le_sms_SetDestination(myMsg, "+821030146506");
    if (res != LE_OK)
    {
        LE_ERROR("le_sms_SetDestination has failed (res.%d)!", res);
        return LE_FAULT;
    }

    res = le_sms_SetBinary(myMsg,inSendData , inSendLength);
#else
    res = le_sms_SetDestination(myMsg, "13031658973");

    if (res != LE_OK)
    {
        LE_ERROR("le_sms_SetDestination has failed (res.%d)!", res);
        return LE_FAULT;
    }

    res = le_sms_SetText(myMsg, "1234567");
#endif

    if (res != LE_OK)
    {
        LE_ERROR("le_sms_SetText has failed (res.%d)!", res);
        return LE_FAULT;
    }

    res = le_sms_Send(myMsg);

    if (res != LE_OK)
    {
    	int flatformErrorCode = 0;
    	le_sms_ErrorCode3GPP2_t errorCode3gpp2;
        le_sms_ErrorCode_t rpCausePtr;
            ///< [OUT] Radio Protocol cause code.
        le_sms_ErrorCode_t tpCausePtr;

        LE_ERROR("le_sms_Send has failed (res.%d)!", res);
        errorCode3gpp2 = le_sms_Get3GPP2ErrorCode(myMsg);
        LE_ERROR("errorCode3gpp2 : (res.%d)!", errorCode3gpp2);

        le_sms_GetErrorCode(myMsg,&rpCausePtr,&tpCausePtr);
        LE_ERROR("errorCode : (res.%d, %d)!", rpCausePtr,tpCausePtr);

        flatformErrorCode = le_sms_GetPlatformSpecificErrorCode(myMsg);
        LE_ERROR("flatformErrorCode : (res.%d)!", flatformErrorCode);
        return LE_FAULT;
    }
    else
    {
        //LE_INFO("\"%s\" has been successfully sent to %s.", textPtr, destinationPtr);
        LE_INFO(" has been successfully sent to.");
    }

    le_sms_Delete(myMsg);

    le_sms_DisconnectService();
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Handler function for SMS storage full message indication.
 *
 */
//--------------------- -----------------------------------------------------------------------------
static void StorageMessageHandler(le_sms_Storage_t  storage,void* contextPtr)
{
    LE_INFO("A Full storage SMS message is received. Type of full storage %d", storage);
}

//--------------------------------------------------------------------------------------------------
/**
 * This function installs an handler for message reception.
 *
 * @return LE_FAULT  The function failed.
 * @return LE_OK     The function succeed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t st_sms_mtReceiver
(
    void
)
{
    RxHdlrRef = le_sms_AddRxMessageHandler(st_sms_rxMessageHandler, NULL);
    if (!RxHdlrRef)
    {
        LE_ERROR("le_sms_AddRxMessageHandler has failed!");
        return LE_FAULT;
    }
    else
    {
        return LE_OK;
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * This function installs an handler for storage message indication.
 *
 * @return LE_FAULT  The function failed.
 * @return LE_OK     The function succeed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t st_sms_MonitorStorage(void)
{
    FullStorageHdlrRef = le_sms_AddFullStorageEventHandler(StorageMessageHandler, NULL);
    if (!FullStorageHdlrRef)
    {
        LE_ERROR("le_sms_AddFullStorageEventHandler has failed!");
        return LE_FAULT;
    }
    else
    {
        return LE_OK;
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * This function remove the handler for message reception.
 *
 */
//--------------------------------------------------------------------------------------------------
void smsmt_HandlerRemover(void)
{
    le_sms_RemoveRxMessageHandler(RxHdlrRef);
}

//--------------------------------------------------------------------------------------------------
/**
 * This function remove the handler for storage message indication.
 *
 */
//--------------------------------------------------------------------------------------------------
void smsmt_StorageHandlerRemover(void)
{
    le_sms_RemoveFullStorageEventHandler(FullStorageHdlrRef);
}

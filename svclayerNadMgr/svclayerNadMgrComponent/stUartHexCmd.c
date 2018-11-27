/*
 * stUartATCmd.c
 *
 *  Created on: Jul 31, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "interfaces.h"
#include "stUartMgr.h"
#include "stUartHexCmd.h"
#include "stCall.h"

typedef struct
{
	uint8_t e_callNumber[100];
	uint8_t b_callNumber[100];
	uint8_t c_callNumber[100];
	uint8_t a_callNumber[100];
}call_number_t;

void st_uart_sendCallStatusResponse();
void st_uart_moCallSendStatusReport(uint8_t inCallStatus,uint8_t inFailCause);
uint8_t moCallType=D_AVN_TCU_CALL_START_NONE;
//uint8_t _callProgressInd=D_TCU_AVN_MO_CALL_CONNETING;
uint8_t _callStatusInd=D_TCU_AVN_DATA_NAD_CALL_STATUS_CALL_IDLE;
bool _mtCallFlag=false;

call_number_t _callNumber;

#define ST_PROV_HANDLE_NAME 		"/home/callDest"

char * _defaultDesNum=
{
"\
e_call:00000000000\r\n\
b_call:00000000000\r\n\
c_call:00000000000\r\n\
a_call:00000000000\r\n"

};

size_t flh_FileSize(char * filePath)
{
	struct stat fileStatBuf;
	int ret = stat(filePath,&fileStatBuf);

	if(ret < 0)
	{
		LE_INFO("flh_FileSize ret [%d]",ret);
		return ret;
	}

	LE_INFO("flh_FileSize %s = [%ld]",filePath,fileStatBuf.st_size);
	return fileStatBuf.st_size;
}

void st_app_uartSetDest()
{
	FILE * fileHanlder;
	size_t llongRes;
	int fileSize=0;

	memset((void *) &_callNumber, 0x00, sizeof(call_number_t));

	fileSize = flh_FileSize(ST_PROV_HANDLE_NAME);
	LE_INFO("st_app_uartSetDest flh_FileSize size=[%d]",fileSize);

	if(fileSize < 0)
	{
		fileHanlder = fopen ( ST_PROV_HANDLE_NAME, "w");

		llongRes=fwrite ((void *)_defaultDesNum,1,strlen(_defaultDesNum),fileHanlder);
		if( llongRes < 0)
			LE_INFO( "[st_prov_Init] adl_flhWrite[%s] FAIL res=[%d]",ST_PROV_HANDLE_NAME,llongRes);

		fclose(fileHanlder);
	}
	else
	{
		char readFileBuffer[500];
		int readFileSize=0;

		memset(readFileBuffer,0x00,500);
		readFileSize = flh_FileSize(ST_PROV_HANDLE_NAME);
		fileHanlder = fopen ( ST_PROV_HANDLE_NAME, "r");
		llongRes=fread( (void *)readFileBuffer,1,readFileSize,fileHanlder);
		LE_INFO("st_app_uartSetDest fwrite res=[%d]",llongRes);
		fclose(fileHanlder);

		char seperateLine[100];
		char seperateLineName[100];
		char seperateLineNum[100];
		int readFileindex=0;
		int seperateLinendex=0;
		//int startIndex=0;
		//int endIndex=0;

		memset(seperateLine,0x00,100);
		memset(seperateLineName,0x00,100);
		memset(seperateLineNum,0x00,100);

		while(readFileBuffer[readFileindex] != 0x00)
		{
			if( readFileBuffer[readFileindex] == '\r' && readFileBuffer[readFileindex+1] == '\n')
			{
				readFileindex+=2;
				//LE_INFO(" LINE -> %s",seperateLine);

				seperateLinendex=0;
				while(seperateLine[seperateLinendex] != 0x00)
				{
					if( seperateLine[seperateLinendex] == ':')
					{
						memcpy(seperateLineName,seperateLine,seperateLinendex);
						memcpy(seperateLineNum,&seperateLine[seperateLinendex+1],strlen(readFileBuffer)-seperateLinendex+1-2);

						if( !strcmp(seperateLineName,"e_call") )
						{
							memcpy((void *)_callNumber.e_callNumber,seperateLineNum,strlen((const char *)seperateLineNum));
						}
						else if( !strcmp(seperateLineName,"b_call") )
						{
							memcpy((void *)_callNumber.b_callNumber,seperateLineNum,strlen((const char *)seperateLineNum));
						}
						else if( !strcmp(seperateLineName,"c_call") )
						{
							memcpy((void *)_callNumber.c_callNumber,seperateLineNum,strlen((const char *)seperateLineNum));
						}
						else if( !strcmp(seperateLineName,"a_call") )
						{
							memcpy((void *)_callNumber.a_callNumber,seperateLineNum,strlen((const char *)seperateLineNum));
						}
						memset(seperateLine,0x00,100);
						memset(seperateLineName,0x00,100);
						memset(seperateLineNum,0x00,100);
						seperateLinendex = 0;
						break;
					}

					seperateLinendex++;
				}
			}

			if( readFileBuffer[seperateLinendex] != ' ')
				seperateLine[seperateLinendex] = readFileBuffer[readFileindex];

			readFileindex++;
			seperateLinendex++;
		}

		LE_INFO("E_CALL : %s",_callNumber.e_callNumber);
		LE_INFO("B_CALL : %s",_callNumber.b_callNumber);
		LE_INFO("C_CALL : %s",_callNumber.c_callNumber);
		LE_INFO("A_CALL : %s",_callNumber.a_callNumber);
	}
}

bool st_app_uart_HexPharser(char * inReadData,int inReadByte)
{
	uint8_t headerID;
	uint16_t cmdID0;
	uint16_t dataSize=0;
	uint8_t sendBuffer[50];
	uint16_t sendIndex=0;
	int readIndex=0;

	//int index=0;

	headerID = inReadData[readIndex++];
	if( headerID != D_DATATYPE_SOF )
	{
		LE_ERROR("INVALID DATA FROM MCU");
		return false;
	}

	cmdID0 = ( ((inReadData[readIndex++] << 7) & 0xFF00));
	cmdID0 |= inReadData[readIndex++];
	LE_INFO("CMD : %x",cmdID0);

	dataSize = ( ((inReadData[readIndex++] << 7) & 0xFF00) );
	dataSize |= inReadData[readIndex++];
	LE_INFO("dataSize : %x",dataSize);

	if( cmdID0 == D_AVN_TCU_CALL_START)
	{
		moCallType = inReadData[readIndex++];
		st_uart_callStart(moCallType);
	}
	else if( cmdID0 == D_AVN_TCU_CALL_END)
	{
		LE_INFO("D_AVN_TCU_CALL_END");
		st_call_hangup();
	}
	else if( cmdID0 == D_TCU_AVN_CALL_ACCEPT)
	{
		st_call_Accept();
	}
	else if( cmdID0 == D_AVN_TCU_DATA_NAD_CALL_STATUS)
	{
		LE_INFO("D_AVN_TCU_DATA_NAD_CALL_STATUS");
		//ser_BodyStartSendCCUData(1);
		st_uart_sendCallStatusResponse();
	}
	else if( cmdID0 == D_AVN_TCU_DATA_NAD_SERVICE_STATUS)
	{
		LE_INFO("D_AVN_TCU_DATA_NAD_SERVICE_STATUS");
		memset(sendBuffer,0x00,50);
		sendBuffer[sendIndex++]=D_DATATYPE_SOF;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=D_TCU_AVN_DATA_NAD_SERVICE_STATUS;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=D_DATATYPE_EOF;
	}
	else if( cmdID0 == D_AVN_TCU_DATA_NAD_STAIC_INF_REQ)
	{
		LE_INFO("D_TCU_AVN_DATA_NAD_STAIC_INF_REQ");
		memset(sendBuffer,0x00,50);
		sendBuffer[sendIndex++]=D_DATATYPE_SOF;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=D_TCU_AVN_DATA_NAD_STAIC_INF_REQ;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=D_DATATYPE_EOF;
	}
	else if( cmdID0 == D_AVN_TCU_DATA_NAD_RF_INFO_REQ)
	{
		uint8_t networkType;
		le_mrc_Rat_t ratType;
		LE_INFO("D_AVN_TCU_DATA_NAD_RF_INFO_REQ");
		le_mrc_GetRadioAccessTechInUse(&ratType);

		switch((size_t)ratType)
		{
		case LE_MRC_RAT_GSM :
			networkType = D_TCU_AVN_DATA_NAD_RF_INFO_REQ_GSM;
			break;
		case LE_MRC_RAT_CDMA :
			networkType = D_TCU_AVN_DATA_NAD_RF_INFO_REQ_WCDMA;
			break;
		case LE_MRC_RAT_UMTS :
			networkType = D_TCU_AVN_DATA_NAD_RF_INFO_REQ_WCDMA;
			break;
		case LE_MRC_RAT_LTE :
			networkType = D_TCU_AVN_DATA_NAD_RF_INFO_REQ_LTE;
			break;
		}

		memset(sendBuffer,0x00,50);
		sendBuffer[sendIndex++]=D_DATATYPE_SOF;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=D_TCU_AVN_DATA_NAD_RF_INFO_REQ;
		sendBuffer[sendIndex++]=0x00;
		sendBuffer[sendIndex++]=0x01;
		sendBuffer[sendIndex++]=networkType;
		sendBuffer[sendIndex++]=D_DATATYPE_EOF;
	}
	else
	{
		LE_ERROR("UNDEFINED COMMAND");
		return false;
	}

	return true;
}

void st_uart_setCallStatus(uint8_t inCallStatus,uint8_t inFailCause)
{
    if (inCallStatus == LE_MCC_EVENT_ALERTING)
    {
    }
    else if (inCallStatus == LE_MCC_EVENT_CONNECTED)
    {
    	_callStatusInd = D_TCU_AVN_DATA_NAD_CALL_STATUS_CALL_CONNECTED;
		st_uart_moCallSendStatusReport(D_TCU_AVN_MO_CALL_CONNETED,inFailCause);
    }
    else if (inCallStatus == LE_MCC_EVENT_TERMINATED)
    {
    	_callStatusInd = D_TCU_AVN_DATA_NAD_CALL_STATUS_CALL_IDLE;
		st_uart_moCallSendStatusReport(D_TCU_AVN_MO_CALL_END,inFailCause);

    	moCallType = D_AVN_TCU_CALL_START_NONE;

    	switch(inFailCause)
        {
            case LE_MCC_TERM_NETWORK_FAIL:
                break;
            case LE_MCC_TERM_UNASSIGNED_NUMBER:
                break;
            case LE_MCC_TERM_USER_BUSY:
                break;
            case LE_MCC_TERM_LOCAL_ENDED:
                break;
            case LE_MCC_TERM_REMOTE_ENDED:
                break;
            case LE_MCC_TERM_NO_SERVICE:
                break;
            case LE_MCC_TERM_OPERATOR_DETERMINED_BARRING:
                break;
            case LE_MCC_TERM_UNDEFINED:
                break;
            default:
                break;
        }
    }
    else if (inCallStatus == LE_MCC_EVENT_INCOMING)
    {
    }
    else if (inCallStatus == LE_MCC_EVENT_ORIGINATING)
    {
    	_callStatusInd = D_TCU_AVN_DATA_NAD_CALL_STATUS_CALL_ORIG;
    	//send call report
    	st_uart_moCallSendStatusReport(D_TCU_AVN_MO_CALL_CONNETING,inFailCause);
    }
    else if (inCallStatus == LE_MCC_EVENT_SETUP)
    {
    }
    else
    {
    }
}

void st_uart_sendCallStatusResponse()
{
	char sendBuffer[50];
	uint16_t sendIndex=0;

	memset(sendBuffer,0x00,50);
	sendBuffer[sendIndex++]=D_DATATYPE_SOF;
	sendBuffer[sendIndex++]=0x00;
	sendBuffer[sendIndex++]=D_TCU_AVN_DATA_NAD_CALL_STATUS;
	sendBuffer[sendIndex++]=0x00;
	sendBuffer[sendIndex++]=0x01;
	sendBuffer[sendIndex++]=_callStatusInd;
	sendBuffer[sendIndex++]=D_DATATYPE_EOF;

	st_app_uart_write(sendBuffer, sendIndex);
}

void st_uart_moCallSendStatusReport(uint8_t inCallStatus,uint8_t inFailCause)
{
	char sendBuffer[50];
	uint16_t sendIndex=0;

	memset(sendBuffer,0x00,50);
	sendBuffer[sendIndex++]=D_DATATYPE_SOF;
	sendBuffer[sendIndex++]=0x00;
	sendBuffer[sendIndex++]=inCallStatus;
	sendBuffer[sendIndex++]=0x00;
	if( inCallStatus == D_TCU_AVN_MO_CALL_CONNETING || inCallStatus == D_TCU_AVN_MO_CALL_FAILIND)
		sendBuffer[sendIndex++]=0x01;
	else
		sendBuffer[sendIndex++]=0x00;

	LE_INFO("st_uart_moCallSendStatusReport = %d",moCallType);
	if( inCallStatus == D_TCU_AVN_MO_CALL_CONNETING)
		sendBuffer[sendIndex++]=moCallType;
	else if( inCallStatus == D_TCU_AVN_MO_CALL_FAILIND)
		sendBuffer[sendIndex++]=inFailCause;

	sendBuffer[sendIndex++]=D_DATATYPE_EOF;

	st_app_uart_write(sendBuffer, sendIndex);
}

void st_uart_callStart(uint8_t inMoCallType)
{
	uint8_t * callNumber;

	switch(inMoCallType)
	{
	case D_AVN_TCU_CALL_START_E :
		LE_INFO("D_AVN_TCU_CALL_START_E");
		callNumber=_callNumber.e_callNumber;
		break;
	case D_AVN_TCU_CALL_START_B :
		LE_INFO("D_AVN_TCU_CALL_START_B");
		callNumber=_callNumber.b_callNumber;
		break;
	case D_AVN_TCU_CALL_START_C :
		LE_INFO("D_AVN_TCU_CALL_START_C");
		callNumber=_callNumber.c_callNumber;
		break;
	case D_AVN_TCU_CALL_START_A :
		LE_INFO("D_AVN_TCU_CALL_START_A");
		callNumber=_callNumber.a_callNumber;
		break;
	default :
		break;
	}
	moCallType = inMoCallType;

	if( st_call_getCallStatus() == false)
	{
		st_call_MOCall((char *)callNumber);
	}
	else
		st_call_hangup();
}

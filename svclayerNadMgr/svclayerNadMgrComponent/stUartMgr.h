/*
 * stUartMgr.h
 *
 *  Created on: Jul 25, 2017
 *      Author: topgun
 */

#ifndef STUARTMANAGER_STUARTMGR_H_
#define STUARTMANAGER_STUARTMGR_H_



void st_app_uart_Init();
size_t st_app_uart_write(char *inBuffer,int inLength);
//int8_t mcu_UartFcmEnd(char * inResultString);
//void st_app_UartSendResult(uint8_t inType,uint8_t inResult);
//void st_app_UartSendString(uint8_t inType,char *inString);
//void st_app_UartSendHexData(uint8_t inType,char *inData,int32_t inLength);


#endif /* STUARTMANAGER_STUARTMGR_H_ */

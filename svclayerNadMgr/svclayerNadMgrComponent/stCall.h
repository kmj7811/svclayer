/*
 * stMobileCall.h
 *
 *  Created on: Aug 8, 2017
 *      Author: topgun
 */

#ifndef STCCUMOBILE_STMOBILECALL_H_
#define STCCUMOBILE_STMOBILECALL_H_


void st_call_init();
bool st_call_MOCall(char * callNumber);
bool st_call_hangup();
void st_call_Accept();
void st_call_GetMTNum(char * telPtr);
bool st_call_getCallStatus();

#endif /* STCCUMOBILE_STMOBILECALL_H_ */

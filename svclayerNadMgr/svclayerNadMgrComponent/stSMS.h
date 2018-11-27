/*
 * stSMS.h
 *
 *  Created on: Sep 19, 2017
 *      Author: ghlee
 */

#ifndef STAPPCOMP_STMOBILE_STSMS_H_
#define STAPPCOMP_STMOBILE_STSMS_H_

#ifdef __cplusplus
extern "C"
{
#endif

//le_result_t smsmt_Receiver(void);
//le_result_t smsmt_MonitorStorage(void);
//void smsmt_HandlerRemover(void);
//void smsmt_StorageHandlerRemover(void);

void st_sms_Init();
le_result_t st_sms_moSendMessage(uint8_t * inSendData, size_t inSendLength);
//bool st_sms_moSendMessage(void *context);

void TestSMSPharser();

#ifdef __cplusplus
}
#endif

#endif /* STAPPCOMP_STMOBILE_STSMS_H_ */

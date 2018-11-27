/*
 * stNet.h
 *
 *  Created on: 2018. 10. 22.
 *      Author: Gwanghee Lee
 */

#ifndef STNET_H_
#define STNET_H_

#include "interfaces.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MNCMCC_CHINA_UNICOM ""
#define MNCMCC_KOREA_KT		"45008"
#define MNCMCC_KOREA_SK		"45005"

void st_net_GetAllInformaion();
//uint8_t st_net_GetNetStatus();
uint32_t st_net_mrcSignalStrength();
//uint8_t st_net_GetRegState();

char * st_net_GetMncMcc();
uint32_t st_net_GetRatStatus();
le_mrc_NetRegState_t st_net_GetRegStatus();

#ifdef __cplusplus
}
#endif

#endif /* STNET_H_ */

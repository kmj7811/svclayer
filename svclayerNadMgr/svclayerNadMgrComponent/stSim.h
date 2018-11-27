/*
 * stSim.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Gwanghee Lee
 */

#ifndef STMOBILE_STSIM_H_
#define STMOBILE_STSIM_H_

#include "legato.h"

#ifdef __cplusplus
extern "C"
{
#endif

void st_sim_init();

uint8_t st_sim_GetStatus();
bool st_sim_isReady();
char * st_sim_phoneNumber();
char * st_sim_GetIccid();

#ifdef __cplusplus
}
#endif

#endif /* STMOBILE_STSIM_H_ */

/*
 * stCCUSim.c
 *
 *  Created on: Aug 7, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "interfaces.h"

static char phoneNumberStr[50];
static char phoneIccidStr[50];

static char * st_sim_CheckIccid();

void st_sim_init()
{
	st_sim_CheckIccid();
	//st_sim_phoneNumber();
}

char * st_sim_GetIccid()
{
	return phoneIccidStr;
}

static char * st_sim_CheckIccid()
{
	le_sim_Id_t simId;
	simId = le_sim_GetSelectedCard();

	le_sim_GetICCID(simId,phoneIccidStr,50);
	LE_INFO("%s",phoneIccidStr);
	return phoneIccidStr;
}

char * st_sim_phoneNumber()
{
	le_sim_Id_t simId;

	simId = le_sim_GetSelectedCard();
	le_sim_GetSubscriberPhoneNumber(simId,phoneNumberStr,30);

	LE_INFO("st_sim_phoneNumber [%s]",phoneNumberStr);
	if( !strncmp(phoneNumberStr,"86",2))
		return &phoneNumberStr[2];
	else
		return phoneNumberStr;

}

bool st_sim_isReady()
{
	return le_sim_IsReady(LE_SIM_EXTERNAL_SLOT_1);
}

uint8_t st_sim_GetStatus()
{
	uint8_t lretValue=0;
	le_sim_States_t simStatus;

	simStatus = le_sim_GetState(LE_SIM_EXTERNAL_SLOT_1);
#if 0
	if( simStatus == LE_SIM_INSERTED)
		lretValue = 0;
	else if( simStatus == LE_SIM_READY )
		lretValue = 1;
	else
		lretValue = 2; //ERROR

#else
	lretValue = simStatus;
#endif

	return lretValue;
}


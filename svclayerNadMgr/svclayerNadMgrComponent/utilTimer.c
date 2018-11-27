/*
 * st_timer_Component.c
 *
 *  Created on: Jun 20, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "utilTimer.h"

le_timer_Ref_t st_timer_Start(char * inTimerName,size_t inIntervalSec,size_t inIntervalMic, bool inRepeat, le_timer_ExpiryHandler_t inTimerHndler)
{
    le_timer_Ref_t timerRef;
    le_result_t res;
    le_clk_Time_t interval = { inIntervalSec, inIntervalMic };

    //create timer
    timerRef = le_timer_Create(inTimerName);
    LE_FATAL_IF(timerRef == NULL, "timer ref is NULL");

    //set interval to 30 seconds
    res = le_timer_SetInterval(timerRef, interval);
    //LE_FATAL_IF(res != LE_OK, "set interval to %d seconds: %d ,micsecond [%ld]", inIntervalSec,inIntervalMic, res);

    //set repeat to infinite
    res = le_timer_SetRepeat(timerRef, inRepeat);
    LE_FATAL_IF(res != LE_OK, "set repeat to infinite: %d", res);

    //set handler for the timer expiration
    res = le_timer_SetHandler(timerRef, inTimerHndler);
    LE_FATAL_IF(res != LE_OK, "set handler to TimerExpirationAction: %d", res);

    //start timer
    //LE_INFO("st_timer_Start [%s]",inTimerName);
    res = le_timer_Start(timerRef);
    LE_FATAL_IF(res != LE_OK, "Unable to start timer: %d", res);

    return timerRef;
}

void st_timer_ReStart(le_timer_Ref_t timerRef)
{
	le_timer_Restart(timerRef);
}

le_result_t st_timer_Stop(le_timer_Ref_t timerRef)
{
	le_result_t result;
	result = le_timer_Stop(timerRef);
	LE_DEBUG("st_timer_Stop [%d]",result);
	free( le_timer_GetContextPtr( timerRef ) );
	if( result == LE_OK)
	{
		le_timer_Delete(timerRef);
		//LE_DEBUG("le_timer_Delete [%d]",result);
	}

	return result;
}

void st_timer_Remove(le_timer_Ref_t timerRef)
{
	le_timer_Delete(timerRef);
}

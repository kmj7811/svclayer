/*
 * st_timer_Component.h
 *
 *  Created on: Jun 20, 2017
 *      Author: topgun
 */

#ifndef ST_UTIL_COMPONENT_ST_TIMER_COMPONENT_H_
#define ST_UTIL_COMPONENT_ST_TIMER_COMPONENT_H_

#define TIMER_1SECS	1000000

typedef enum {
	ST_TIMER_REPEAT, // repeat
	ST_TIMER_ONESHOT // one shot
}st_timer_repeat_t;

le_timer_Ref_t st_timer_Start(char * inTimerName,size_t inIntervalSec,size_t inIntervalMic, bool inRepeat, le_timer_ExpiryHandler_t inTimerHndler);
le_result_t st_timer_Stop(le_timer_Ref_t timerRef);
void st_timer_Remove(le_timer_Ref_t timerRef);
void st_timer_ReStart(le_timer_Ref_t timerRef);

#endif /* ST_UTIL_COMPONENT_ST_TIMER_COMPONENT_H_ */

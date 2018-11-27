/*
 * stGpio.c
 *
 *  Created on: 2018. 9. 4.
 *      Author: Gwanghee Lee
 */
#include "legato.h"
#include "interfaces.h"
#include "stUartMgr.h"
#include "stCall.h"
#include "utilTimer.h"
#include "stUartHexCmd.h"
#include "stDataConnect.h"
#include "stGpio.h"

#define GPIO_THREAD 0

#define GPIO_PIN_22 1
#define GPIO_PIN_32 1
#define GPIO_PIN_33 1

#if (GPIO_PIN_22 == 1)
static le_buttonGpio22_ChangeEventHandlerRef_t ref22;

static void Pin22ChangeCallback(bool state, void *ctx){
    LE_INFO("State 22 gpio change %s", state?"TRUE":"FALSE");

    if( state == true)
    {
		LE_INFO("GPIO 22 E_CALL PRESSED Call state %d",st_call_getCallStatus());
		if( st_call_getCallStatus() == false)
			st_uart_callStart(D_AVN_TCU_CALL_START_E);
#if 1
		else
			st_call_hangup();
#else
		else
		{
			LE_INFO("Call Progressing..Ignore second BTN input");
		}
#endif
    }
}
#endif

#if GPIO_PIN_32
static le_buttonGpio32_ChangeEventHandlerRef_t ref32;

static void Pin32ChangeCallback(bool state, void *ctx){
    LE_INFO("State 32 gpio change %d", state);

    if( state == true)
    {
		//LE_INFO("GPIO 32 C_CALL PRESSED");
		LE_INFO("GPIO 32 C_CALL PRESSED Call state %d",st_call_getCallStatus());
		if( st_call_getCallStatus() == false)
			st_uart_callStart(D_AVN_TCU_CALL_START_C);
#if 1
		else
			st_call_hangup();
#else
		else
		{
			LE_INFO("Call Progressing..Ignore second BTN input");
		}
#endif
    }
}
#endif

#if GPIO_PIN_33
static le_buttonGpio33_ChangeEventHandlerRef_t ref33;

static void Pin33ChangeCallback(bool state, void *ctx){
    LE_INFO("State 33 gpio change %d", state);

    if( state == true)
    {
		//LE_INFO("GPIO 33 B_CALL PRESSED");
		LE_INFO("GPIO 33 B_CALL PRESSED Call state %d",st_call_getCallStatus());
		if( st_call_getCallStatus() == false)
			st_uart_callStart(D_AVN_TCU_CALL_START_B);
#if 1
		else
			st_call_hangup();
#else
		else
		{
			LE_INFO("Call Progressing..Ignore second BTN input");
		}
#endif
    }
}
#endif

void stGpioInit()
{
#if (GPIO_PIN_22 == 1)
    le_buttonGpio22_EnablePullUp();
	le_buttonGpio22_SetInput(LE_BUTTONGPIO22_ACTIVE_LOW);
    ref22 = le_buttonGpio22_AddChangeEventHandler(LE_BUTTONGPIO22_EDGE_FALLING, Pin22ChangeCallback, NULL, 0);
    LE_INFO("Pin22 ref22: %x", (uint32_t)ref22);
#endif

#if (GPIO_PIN_32 == 1)
    le_buttonGpio32_EnablePullUp();
	le_buttonGpio32_SetInput(LE_BUTTONGPIO32_ACTIVE_LOW);
    ref32 = le_buttonGpio32_AddChangeEventHandler(LE_BUTTONGPIO32_EDGE_FALLING, Pin32ChangeCallback, NULL, 0);
    LE_INFO("Pin32 ref32: %x", (uint32_t)ref32);
#endif

#if (GPIO_PIN_33 == 1)
    le_buttonGpio33_EnablePullUp();
	le_buttonGpio33_SetInput(LE_BUTTONGPIO33_ACTIVE_LOW);
    ref33 = le_buttonGpio33_AddChangeEventHandler(LE_BUTTONGPIO33_EDGE_FALLING, Pin33ChangeCallback, NULL, 0);
    LE_INFO("Pin33 ref33: %x", (uint32_t)ref33);
#endif
}

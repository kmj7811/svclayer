#include "legato.h"
#include "interfaces.h"

#include "stUartMgr.h"
#include "stCall.h"
#include "utilTimer.h"
#include "stUartHexCmd.h"
#include "stDataConnect.h"
#include "stGpio.h"
#include "stNet.h"

COMPONENT_INIT
{
    LE_INFO("---Service Layer NAD MGR Start ----");
    st_app_uartSetDest();
    st_app_uart_Init(); //No more need AVN interface
    st_call_init();
    st_Data_Init();
    stGpioInit();
    st_net_GetAllInformaion();
}

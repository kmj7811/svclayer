/*
 * stUartMgr.c
 *
 *  Created on: Jul 25, 2017
 *      Author: topgun
 */

#include "legato.h"
#include "stUartMgr.h"
#include "interfaces.h"
#include <termios.h>
#include "le_fdMonitor.h"
#include "stUartHexCmd.h"
#include "stCall.h"

/*DEFINE*/
#define UART_DATA_TOTAL	0

//#define D_ST_UART_PORT_NAME "/dev/ttyHSL1"
#define D_ST_UART_PORT_NAME "/dev/ttyHS0"
#define REMAINED_DATA_BUFFER	1000


/*PRE-DEFINED EXTERN*/
int st_app_uart_Open();

/*PRE-DEFINED STATIC*/
static int st_uart_v_fileDescrip;
static void st_uart_DataHndler(int fd, short events);

void st_app_uart_Init()
{
	st_uart_v_fileDescrip = 0;

	st_uart_v_fileDescrip = st_app_uart_Open();

	/*le_fdMonitor_Ref_t fdMonitor = */le_fdMonitor_Create("Serial Port",      // Name for diagnostics
													st_uart_v_fileDescrip,                 // fd to monitor
													st_uart_DataHndler,  // Handler function
													POLLIN);            // Monitor readability
}

static void st_uart_DataHndler(int fd, short events)
{
	int bytesRead=0;

    if (events & POLLIN)    // Data available to read?
    {
        char buff[1024];
        //char callNumber[50];
        memset(buff,0x00,1024);

        bytesRead = read(fd, buff, sizeof(buff));

#if 1
        int i = 0;
        printf("Rcvd data is %d bytes",bytesRead);
        for(i = 0; i < bytesRead; i++)
        {
        	printf("Data[%d] = %c[0x%d]",i,buff[i],(uint8_t)buff[i]);
        }
#endif
        LE_DUMP((uint8_t *)buff,bytesRead);
        st_app_uart_HexPharser(buff,bytesRead);
    }

    if (events & POLLOUT)    // Data available to read?
    {
    	LE_DEBUG("st_uart_read POLLOUT");
    }
}

size_t st_app_uart_write(char *inBuffer,int inLength)
{
	size_t writeResult=0;

#if 1 //(ST_APP_TEST_UART_DEBUG_MODE)
	LE_DEBUG("=================st_app_uart_write inLength[%d]=================",inLength);
	LE_DUMP((uint8_t*)inBuffer,inLength);
	LE_DEBUG("================================================================");
#endif
	if( st_uart_v_fileDescrip == 0)
	{
		LE_ERROR("Need to open UART");
		return 0;
	}
	writeResult = write (st_uart_v_fileDescrip, inBuffer, inLength);

	return (writeResult);
}

int st_app_uart_Open()
{
	int fd=-1;
	struct termios  newtio;

    fd = open (D_ST_UART_PORT_NAME, O_RDWR|O_NONBLOCK);

    if (fd < 0)
    {
    	LE_ERROR("Error UART opening %s: %s\n", D_ST_UART_PORT_NAME, strerror(errno));
        return fd;
    }
    else
    {
        LE_INFO("UART Open Success");
    	memset(&newtio, 0, sizeof(newtio));

    	newtio.c_iflag = IGNPAR;  // non-parity    newtio.c_oflag = 0;
    	newtio.c_oflag = 0;
    	newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts
    	newtio.c_cflag |= B115200;
    	newtio.c_cc[VTIME] = 10;
    	newtio.c_cc[VMIN]  = 0;

    	tcflush  ( fd, TCIFLUSH );
    	tcsetattr( fd, TCSANOW, &newtio );

    	return fd;
    }
	return fd;

}

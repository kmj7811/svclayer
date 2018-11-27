#include "legato.h"
#include "interfaces.h"

#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <execinfo.h>
#include <signal.h>

#include "msg_if.h"
#include "common.h"
#include "svclayer_process_nad_pkt.h"


int mExitThread=0;


void test_api()
{
	int value = 1;
	printf("test_api\n");
	svclayerNad_SetCommand1(value);
}
void test_api2()
{
	char message[10];
    char *value = "test_msg";

    memset(message,0,sizeof(message));
    memcpy(message, value,10);
	svclayerNad_SetCommand2(message);

}
void test_api3()
{
	int value = 0x00;  //orig call
	char *orig_string = "01024647811";
	char message[100];

	memset(message,0,sizeof(message));
	memcpy(message,orig_string,11);

	svclayerNad_SetCommand3(value,message);
}
void test_api4()
{
	int value = 0x01;  //accept call
	char* dummy = "dummy";
	svclayerNad_SetCommand3(value,dummy);
}
void test_api5()
{
	int value = 0x02;  //end call
	char* dummy = "dummy";
	svclayerNad_SetCommand3(value,dummy);
}




static void* mThreadFn
(
    void* context
)
{
    int msqid;
    bool bReturn = true;
    msg_t rcv_data;

    LOGI("--- mThreadFn Start\n");

    msqid = msgget( (key_t)NAD2IF_MSGQ_KEY, IPC_CREAT | 0666);

    if(msqid<0)
    {
      LOGI("mThreadFn msgget error\n");
      return(void*)-1;
    }


    while(mExitThread!=1)
    {

        memset(&rcv_data,0,sizeof(msg_t));
        if (msgrcv( msqid, (void *)&rcv_data, sizeof(msg_t)- sizeof(long), NAD2IF_MSGQ_KEY, 0) == -1)
        {
          LOGI("SENDDAT_KEY msgrcv fail\n");
          usleep(10000);
          continue;
        }

        bReturn = process_nadMsg_pkt(&rcv_data);
        if(!bReturn)
        {
        	LOGI("fail to process app msg");
        }

    }
    le_thread_CleanupLegatoThreadData();

    LOGI("---mThreadFn Error -- Exit\n");
    return (void*)-1;
    return NULL;
}

void start_receiverThread()
{
    le_thread_Ref_t   mThreadRef;
    mThreadRef = le_thread_Create("msvcReceiverThread",mThreadFn,NULL);
    le_thread_Start(mThreadRef);
    LE_INFO("----- Start msvcReceiverThread ----------");
}

COMPONENT_INIT
{

    LE_INFO("-----Service Layer Interface Mgr ---- ");
    start_receiverThread();


}

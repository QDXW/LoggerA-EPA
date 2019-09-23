#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/socket.h>
#include "PortType.h"
#include "../Ethernet/EthernetExtern.h"
#include "../include/ConstDef.h"

PortType sPortType;
int ProMsgID;

/*****************************************************************************
* Description: 		init net parameter
* Parameters:
* Returns:
*****************************************************************************/
BOOL initPort()
{
	int res=1;
	int i=0;
	int sockid;
	int msgid = 0;

	key_t portKey=0x00001010;
	key_t ServerKey = 0x10101010;
	ProMsgID = 0;
	while(ProMsgID == 0)
	{
		ProMsgID=msgget(ServerKey++,IPC_CREAT|0666);
	}
	DbgPrintf("initPort-----ProMsgID=%d\n",ProMsgID);
	for(i=0;i<MaxPortType;i++)
	{
		switch(i)
		{
            case EnetPortType:  //net port type
                sockid=socket(AF_INET,SOCK_TYPE,0);
                while(msgid == 0)
                {
                    msgid=msgget(portKey++,IPC_CREAT|0666);    //net msg list,use for upload
                }

                if(sockid==-1)
                {
                    res=0;
                }
                break;
            case UartPortType:  //uart type
                break;
            case UsbPortType:	//USB type
                break;
		}
	}
	return res;
}

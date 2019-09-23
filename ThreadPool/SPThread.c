#include "stdio.h"
#include "../include/DataTypeDef.h"
#include "SPThread.h"
#include "../include/ConstDef.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <pthread.h>
#include "SPThreadPool.h"
#include "../EnetPortPrs/Port_FunExtern.h"
//#include"../GetSetFun/GetSet_PrsExtern.h"
#include <sys/signal.h>

extern Struct_ThreadPool g_ThreadPool;      //threadpool Global variables

volatile static UINT4 g_nMsgQueueKeytoken = 2345;

/*****************************************************************************
* Description:      获取消息队列序号
* Parameters:
* Returns:           序号
*****************************************************************************/
UINT4 GetMsgQueueKeytoken()
{
    g_nMsgQueueKeytoken += 1;
    return g_nMsgQueueKeytoken;
}

/*****************************************************************************
* Description:      获取消息编号
* Parameters:
* Returns:           编号
*****************************************************************************/
INT4 GetMsgID()
{
    INT4 nMsgID=0;
    while(0 == nMsgID)
    {
        nMsgID = msgget((key_t)GetMsgQueueKeytoken(),IPC_CREAT|0660);
    }
    return nMsgID;
}

/*****************************************************************************
* Description:      创建消息处理线程
* Parameters:        nType:类型：
                     nPos:1:Server处理线程 2:Client处理线程 3:AP处理线程
* Returns:
*****************************************************************************/
BOOL CreateThread(int nType,int nPos)
{
    BOOL bRes = FALSE;
    Struct_SPThreadInofPtr pInfo = NULL;//*pInfoPtr;
    sigset_t signal_mask;

    sigemptyset(&signal_mask);
    sigaddset(&signal_mask,SIGPIPE);
    int rc = pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
    if(rc !=0)
    {
        printf("block sigpipe error\r\n");
    }

    switch(nType)
    {
        case TYPE_THREAD_GETSET:
            pInfo = &g_ThreadPool.arrThreadType_GetSet[nPos];
            pthread_mutex_init(&pInfo->mutex,NULL);//mutex init
            pInfo->nMessageCount = 0;//msg num set 0
            pInfo->nKeytoken = GetMsgQueueKeytoken();//get msg keyID
            pInfo->nMsgQueueID = GetMsgID();//creat msg
            //pInfo->pThreadFunction = Fun_Thread_GetSet;
            printf("TYPE_THREAD_GETSET pInfo->nMsgQueueID =%d,nKeytoken=%d\n",pInfo->nMsgQueueID,pInfo->nKeytoken);
            break;
        case TYPE_THREAD_PORT:
            pInfo = &g_ThreadPool.arrThreadType_Port[nPos];
            pthread_mutex_init(&pInfo->mutex,NULL);//mutex init
            pInfo->nMessageCount = 0;   //msg num set 0
            pInfo->nKeytoken = GetMsgQueueKeytoken();//get msg keyID
            pInfo->nMsgQueueID = GetMsgID();//creat msg
            pInfo->pThreadFunction = Fun_Thread_Port;
            printf("TYPE_THREAD_PORT pInfo->nMsgQueueID =%d,nKeytoken=%d\n",pInfo->nMsgQueueID,pInfo->nKeytoken);
            break;
        case TYPE_THREAD_AP:
            pInfo = &g_ThreadPool.arrThreadType_Port[nPos];
            pthread_mutex_init(&pInfo->mutex,NULL);//mutex init
            pInfo->nMessageCount = 0;   //msg num set 0
            pInfo->nKeytoken = GetMsgQueueKeytoken();//get msg keyID
            pInfo->nMsgQueueID = GetMsgID();//creat msg
            pInfo->pThreadFunction = Fun_Thread_Port;
            printf("TYPE_THREAD_AP pInfo->nMsgQueueID =%d,nKeytoken=%d\n",pInfo->nMsgQueueID,pInfo->nKeytoken);
            break;
        default:
            pInfo->pThreadFunction = NULL;
            break;
    }

    if(pInfo->pThreadFunction != NULL)
    {
        bRes = pthread_create(&pInfo->thread_t_ID,NULL,pInfo->pThreadFunction,(void*)nPos) == 0?TRUE:FALSE;
    }
    return bRes;
}


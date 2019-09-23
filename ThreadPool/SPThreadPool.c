#include "stdio.h"
#include "../include/DataTypeDef.h"
#include "SPThread.h"
#include "../include/ConstDef.h"
#include "SPThreadPool.h"
#include "SPThreadExtern.h"
#include <pthread.h>
#include "../interface/common.h"

Struct_ThreadPool g_ThreadPool;//thread Global variables

/*****************************************************************************
* Description:      初始化消息处理线程池
* Parameters:
* Returns:
*****************************************************************************/
void InitThreadPool()
{
    int nPos = 0;
    Struct_SPThreadInofPtr pThreadInfo = NULL;
    for(nPos = 0;nPos < INIT_GETSETThread_COUNT;nPos ++)
    {
        pThreadInfo = &g_ThreadPool.arrThreadType_GetSet[nPos];
        pThreadInfo->nThreadType = TYPE_THREAD_GETSET;
        CreateThread(TYPE_THREAD_GETSET,nPos);
        //thread hanging
    }

    nPos = 0;
    pThreadInfo = NULL;
    for(nPos = 0;nPos < INIT_PORTThread_COUNT;nPos ++)
    {
        pThreadInfo = &g_ThreadPool.arrThreadType_Port[nPos];
        pThreadInfo->nThreadType = TYPE_THREAD_PORT;
        CreateThread(TYPE_THREAD_PORT,nPos);
        //thread hanging
    }

    nPos = 0;
    pThreadInfo = NULL;
    for(nPos = 0;nPos < INIT_APThread_COUNT;nPos ++)
    {
        pThreadInfo = &g_ThreadPool.arrThreadType_AP[nPos];
        pThreadInfo->nThreadType = TYPE_THREAD_AP;
        CreateThread(TYPE_THREAD_AP,nPos);
        //thread hanging
    }
    g_ThreadPool.nThreadCount_GetSet = INIT_GETSETThread_COUNT;
//  g_ThreadPool.nThreadCount_Alarm = INIT_ALARMThread_COUNT;
    g_ThreadPool.nThreadCount_Port=INIT_PORTThread_COUNT;
    g_ThreadPool.nThreadCount_Port=INIT_APThread_COUNT;
    DbgPrintf("init success!\n");
}

/*****************************************************************************
* Description:      获取线程池线程
* Parameters:        nType:获取的线程类型
                     pThreadInfoPtr:
* Returns:
*****************************************************************************/
BOOL GetThread(UINT4 nType,Struct_SPThreadInofPtr *pThreadInfoPtr)
{
    BOOL bRes = FALSE;
    UINT4 nThreadCount=0;
    UINT4 nMaxThreadCount=0;
    Struct_SPThreadInofPtr pThreadInfo = NULL;
    switch(nType)
    {
        case TYPE_THREAD_GETSET:
            nThreadCount = g_ThreadPool.nThreadCount_GetSet;//working pthread num
            pThreadInfo = &g_ThreadPool.arrThreadType_GetSet[0];
            nMaxThreadCount = MAX_GETSETThread_COUNT;
            break;
        case TYPE_THREAD_ALARM:
            nThreadCount = g_ThreadPool.nThreadCount_Alarm;
            pThreadInfo = &g_ThreadPool.arrThreadType_Alarm[0];
            nMaxThreadCount = MAX_ALARMThread_COUNT;
            break;
        case TYPE_THREAD_PORT:
            nThreadCount = g_ThreadPool.nThreadCount_Port;
            pThreadInfo = &g_ThreadPool.arrThreadType_Port[0];
            nMaxThreadCount = MAX_PORTThread_COUNT;
            break;
    }
    int nPos = 0;
    pthread_mutex_lock(&pThreadInfo->mutex);
    int nMessageCount = pThreadInfo->nMessageCount;//the command num in pthread
    pthread_mutex_unlock(&pThreadInfo->mutex);
    Struct_SPThreadInofPtr pThreadInof_Relaxed = pThreadInfo;
    pThreadInfo += 1;//point to next pthread in the pthreadpool
    //look for the most relax pthread in the pthreadpool,pThreadInof_Relaxed point to this pthread
    if(nMessageCount!=0)
    {
        for(nPos = 1; nPos < nThreadCount; nPos++)
        {
        pthread_mutex_unlock(&pThreadInfo->mutex);
        if(nMessageCount > pThreadInfo->nMessageCount);
            {
                nMessageCount = pThreadInfo->nMessageCount;
                pThreadInof_Relaxed = pThreadInfo;
                if(nMessageCount == 0)
                {
                //if no command in msg,then break the loop
                pthread_mutex_unlock(&pThreadInfo->mutex);
                break;
                }
            }
        pthread_mutex_unlock(&pThreadInfo->mutex);
        pThreadInfo += 1;
        }
    }
    if(nMessageCount == 0 || nThreadCount == nMaxThreadCount)
    {
        //if pthread all working or the command num is 0 in msg,return the pthread that pThreadInof_Relaxed pointed
        bRes = TRUE;
        *pThreadInfoPtr = pThreadInof_Relaxed;
    }
    else
    {
        //new thread
        pThreadInof_Relaxed = pThreadInfo;
        pThreadInof_Relaxed->nThreadType = nType;
        CreateThread(nType,nThreadCount);     //BOOL CreateThread(Struct_SPThreadInofPtr* pInfoPtr)
        switch(nType)
        {
            case TYPE_THREAD_GETSET:
                g_ThreadPool.nThreadCount_GetSet += 1;
                break;
            case TYPE_THREAD_ALARM:
                g_ThreadPool.nThreadCount_Alarm += 1;
                break;
            case TYPE_THREAD_PORT:
                g_ThreadPool.nThreadCount_Port += 1;
                break;
        }
        *pThreadInfoPtr = pThreadInof_Relaxed;
        bRes = TRUE;
    }
    return bRes;
}

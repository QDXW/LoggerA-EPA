#ifndef SPTHREAD_H_
#define SPTHREAD_H_

#include "../include/DataTypeDef.h"
#include <pthread.h>
#include <semaphore.h>

#define TYPE_THREAD_GETSET	1
#define TYPE_THREAD_ALARM       2
#define TYPE_THREAD_PORT        3
#define TYPE_THREAD_AP        4
typedef void* (*Fun_Thread)(void *);
typedef struct	_SPThreadInfo
{
	UINT4	nThreadType;//thread type
	pthread_mutex_t mutex;//mutex
	pthread_t thread_t_ID;//thread ID
	UINT4 nKeytoken;//msg keyID
	INT4 nMsgQueueID;//msg flag
	Fun_Thread pThreadFunction;//thread work fun
	UINT4 nMessageCount;//msg queue count
}Struct_SPThreadInfo,*Struct_SPThreadInofPtr;
#endif /*SPTHREAD_H_*/

#ifndef SPTHREADPOOL_
#define SPTHREADPOOL_

#include "../include/DataTypeDef.h"
#include "SPThread.h"

#define MAX_GETSETThread_COUNT 	5
#define INIT_GETSETThread_COUNT 	1
#define MAX_ALARMThread_COUNT 		5
#define INIT_ALARMThread_COUNT 	1
#define MAX_PORTThread_COUNT 		5
#define INIT_PORTThread_COUNT 	2
#define MAX_APThread_COUNT 		5
#define INIT_APThread_COUNT 	2

typedef struct _ThreadPool
{
	//the thread num in threadpool
	UINT4 nThreadCount_GetSet;
	UINT4 nThreadCount_Alarm;
	UINT4 nThreadCount_Port;
	UINT4 nThreadCount_AP;
	//set/get threadpool
	Struct_SPThreadInfo arrThreadType_GetSet[MAX_GETSETThread_COUNT];
	//threadpool alarm
	Struct_SPThreadInfo arrThreadType_Alarm[MAX_ALARMThread_COUNT];
	//port info threadpool
	Struct_SPThreadInfo arrThreadType_Port[MAX_PORTThread_COUNT];
	Struct_SPThreadInfo arrThreadType_AP[MAX_PORTThread_COUNT];
}Struct_ThreadPool,*Struct_ThreadPoolPtr;

#endif /*SPTHREADPOOL_*/

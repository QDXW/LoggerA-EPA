#ifndef PROTOCOLPOOLEXTERN_H_
#define PROTOCOLPOOLEXTERN_H_
#include "SPThreadPool.h"
extern void InitThreadPool();
extern BOOL GetThread(UINT4 nType,Struct_SPThreadInofPtr *pThreadInfoPtr);
#endif

#ifndef TOOLEXTERN_H_
#define TOOLEXTERN_H_
#include "../include/DataTypeDef.h"

extern void *Deamon_Monitor();
extern UINT4 Transform(char *pIn,UINT4 nLth,BOOL bReverse,char **pOut);
extern UINT2 CalculateCRC( char *pBuffer, UINT4 nLen);
extern UINT2 CalculateFileCRC( char *pBuffer, UINT4 nLen, UINT2 file_type);

//extern UINT2 htons(UINT2 nData);
//extern UINT2 ntohs(UINT2 nData);
//extern UINT4 htonl(UINT4 nData);
//extern UINT4 ntohl(UINT4 nData);
#endif

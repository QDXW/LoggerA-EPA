#ifndef PROTOCOLEXTERN_H_
#define PROTOCOLEXTERN_H_
#include "ProtocolDef.h"
extern UINT4 UnPackingAPData(UINT4 nTypeComm,char* pData,UINT4 nLength,Struct_APPtr* pAPPtr);
extern UINT4 PackingAPData(Struct_APPtr pAP,char** pBuffer);
extern UINT4 SlipRecvData(UINT4 nTypeComm,char* pData,UINT4 nLength,UINT4 nUsefulLength);
#endif /*PROTOCOLEXTERN_H_*/

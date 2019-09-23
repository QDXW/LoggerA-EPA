#ifndef SPMessageDef_H
#define SPMessageDef_H
#include"../include/ConstDef.h"
#include"../include/DataTypeDef.h"
#pragma pack(push, 1)
//getset alarm msg
typedef struct _PortID
{
	INT4 Port_MsgID;
	INT4 Port_TypeID;
}Struct_PortID,*Struct_PortIDPtr;

//port msg
typedef struct _Port_Message
{
	unsigned long lMessageType;
	UINT4 nDateLength;
	Struct_PortID portid;
	char PortMsgBuf[MaxMsgBufLength];
}Struct_Port_Msg,*Struct_Port_MsgPrt;

typedef struct _Server_Message
{
	unsigned long lMessageType;
	int length;
	char ServerMsgBuf[MaxMsgBufLength];
}Struct_Server_Msg,*Struct_Server_MsgPrt;
#pragma pack (pop)
#endif

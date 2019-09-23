#ifndef ConstDef
#define ConstDef
//#define NULL	0
#define TRUE	1
#define FALSE	0
#define MYFIFO "./myfifo"
#define XMLName "RemoteInfo.xml"
#define MaxMsgBufLength 256 //msg queue length
#define RemotePortNum 6666	//remote port
#define MAX_Lisn_QUE_NUM 255	//listen queue num
#define SOCK_TYPE SOCK_STREAM	//net protol type
#define Port_Msg_Length(DataType) (sizeof(DataType)-sizeof(long))
//monitor Variable
#endif

/*
 * main.h
 *
 *  Created on: 2010-9-9
 *      Author: vv
 */
#ifndef PORTTYPE_H_
#define PORTTYPE_H_
#define MaxPortType   3
#define EnetPortType  0
#define UartPortType  1
#define UsbPortType   2
#include"../include/DataTypeDef.h"
typedef void (* Port_Prs)(int sockid,int msgid);
typedef void (* Port_Update)(int msgid);
typedef struct PortType
{
	int porttype;
	union Portfd
	{
		int sockfd;	//net socketID
	}protid;
	INT4 NetPort_Msg_id;	//msg ID(be used to save the massage that be upload by netport)
	Port_Prs Port_Prs_Fun;
}PortType[MaxPortType];
#endif

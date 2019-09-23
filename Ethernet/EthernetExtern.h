/******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : EthernetExtern.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef ETHERNETEXTERN_H_
#define ETHERNETEXTERN_H_
#include"../include/DataTypeDef.h"
#include "../interface/interfaceExtern.h"

extern int g_ethernet_connect_status;                //Ethernet connect status  1:connect   0: disconnect
extern struct sThirdPartyStationInfo gThirdPartyStationInfo;
extern int g_ethernet_connect_sub;                   //Channel 1 socket client connect status  1:connect   0: disconnect
extern int g_nRemotesockfd;
extern int g_nRemotesockfd_sub;
extern int g_ethernet_sockfd;                        //Ethernet server socket fd

extern UINT8 SysNetInit();
extern unsigned char GetDomainNameIP(unsigned char *pDomainName);
extern UINT8 EthernetSend(UINT8 *buffer,UINT8 length,UINT8 mode,UINT8 channel_id);
extern void *EthernetThreadMasterChannel();
extern void *EthernetThreadSlaveChannel();
extern void *EthernetServerThreadSlaveChannel();

#endif

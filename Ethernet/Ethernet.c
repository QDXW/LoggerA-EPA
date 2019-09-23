/******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : Ethernet.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "../include/DataTypeDef.h"
#include "../protocal/ProtocolDef.h"
#include "../interface/interfaceExtern.h"
#include "../INI/libini.h"
#include "../TPMsgDef/TPMsgDef.h"
#include "../ThreadPool/SPThreadPoolExtern.h"
#include "../SGCC/store.h"

#define CLIENT_MODE     0
#define SERVER_MODE     1

#define MAIN_CHANNEL    0
#define SUB_CHANNEL     1

int g_nRemotesockfd = 0;                          //Channel 0 socket client fd
int g_nRemotesockfd_sub = 0;                      //Channel 1 socket client fd
int g_ethernet_connect = 0;                       //Channel 0 socket client connect status  1:connect   0: disconnect
int g_ethernet_connect_sub = 0;                   //Channel 1 socket client connect status  1:connect   0: disconnect
struct sThirdPartyStationInfo gThirdPartyStationInfo; //Third party station information
int g_ethernet_sockfd=0;                          //Ethernet server socket fd
int g_ethernet_connect_status = 0;                //Ethernet connect status  1:connect   0: disconnect
UINT8 gLocalIP[4]={192,168,1,1};                  //local ip address
UINT8 g_ethernet_lock = 0;                        //ethernet mutex

extern int ProMsgID;
extern UINT8 gDeviceIPSetFlag;
extern UINT8 gMainDeviceStatus;
extern UINT8 gMainDeviceConfig;
extern UINT8 gDeviceIPSetTempFlag;
extern UINT8 gSocketMode;
extern UINT8 gDeviceStationBuild;
extern UINT8 gServerIP[4];
extern UINT8 gConnectDeviceMaxNum;
extern UINT16 gMainDeviceID;

/*****************************************************************************
* Function     : SysNetInit()
* Description  : 设置系统网络信息
* Input        : None
* Output       : None
* Return       : 0:set success 1:set fail
* Note(s)      :
* Contributor  : 2018年11月8日        Andre
*****************************************************************************/
UINT8 SysNetInit()
{
    char aRouteSet[100]={0},aIpSet[100]={0},aIPTemp[4]={0},routeflag=0;
    char *pRoute="route add -net %d.%d.%d.0 netmask 255.255.255.0 gw %d.%d.%d.1 eth0";
    char dns_path[50]="/etc/resolv.conf";

    gDeviceIPSetTempFlag=1;          /*配置设备处于网络信息配置状态*/
    if(g_nRemotesockfd>1)             /*如果有连接,则关闭连接*/
    {
        shutdown(g_nRemotesockfd,2);
        //TestLogTimeFileWrite();
        //TestLogStringFileWrite("Disconnect Server:Init Device IP\n",strlen("Disconnect Server:Init Device IP\n"));
        close(g_nRemotesockfd);
    }
    /*配置IP信息*/
    system("ifconfig eth0 down");/*关闭网络设备*/
    sleep(1);
    E2promRead((UINT8 *)&gDeviceIPSetFlag,DeviceIPSetAddr,1);/*获取网络信息配置标志 1:已配置 0:未配置*/
    if(gDeviceIPSetFlag!=1)        /*未配置过网络信息则使用默认信息*/
    {
        system("ifconfig eth0 192.168.1.176");
        aIPTemp[0]=192;
        aIPTemp[1]=168;
        aIPTemp[2]=1;
        aIPTemp[3]=176;
    }
    else/*从存储器中获取网络信息*/
    {
        E2promRead((void *)aIPTemp,DeviceIP_E2P,4);
        sprintf(aIpSet,DeviceIPRegex,aIPTemp[0],aIPTemp[1],aIPTemp[2],aIPTemp[3]);
        DbgPrintf("Set Eth0 IP %s\r\n",aIpSet);
        system(aIpSet);
    }

    gLocalIP[0]=aIPTemp[0];
    gLocalIP[1]=aIPTemp[1];
    gLocalIP[2]=aIPTemp[2];
    gLocalIP[3]=aIPTemp[3];

char dns_value[50]="nameserver 10.11.1.100\r\n";


    FILE *fd=fopen(dns_path,"w+");
    fwrite(dns_value,strlen(dns_value),1,fd);
    fclose(fd);

    system("ifconfig eth0 up");/*打开网络设备*/
    sleep(3);

    /*配置路由*/
    E2promRead((UINT8 *)&routeflag,RouteConfigAddr,1);

    if((gServerIP[0]!=aIPTemp[0])
        ||(gServerIP[1]!=aIPTemp[1])
        ||(gServerIP[2]!=aIPTemp[2]))
    {
        sprintf(aRouteSet,pRoute,gServerIP[0],gServerIP[1],gServerIP[2],aIPTemp[0],aIPTemp[1],aIPTemp[2]);
        DbgPrintf("%s\r\n",aRouteSet);
        system(aRouteSet);
        routeflag=1;
        E2promWrite((UINT8 *)&routeflag,RouteConfigAddr,1);
    }

    gDeviceIPSetTempFlag=0;
    DbgPrintf("TTT Set Logger IP Address Finished TTT\r\n");
    return 0;
}

/*****************************************************************************
* Function     : GetDomainNameIP()
* Description  : 域名转换成IP地址
* Input        : *pDomainName: domain name
* Output       : None
* Return       : 0: fail  1:success
* Note(s)      :
* Contributor  : 2018年11月7日        Andre
*****************************************************************************/
UINT8 GetDomainNameIP(UINT8 *pDomainName)
{
    char host[50],aDomainNameTemp[30];
    UINT8 i;
    struct hostent *he;
    struct in_addr **addr_list;
    UINT8 *pServerIPStart,*pServerIPEnd;

    memcpy(aDomainNameTemp,pDomainName,strlen(aDomainNameTemp));
    /*if((strstr(pDomainName,".com")==NULL)
        &&(strstr(pDomainName,".cn")==NULL)
        &&(strstr(pDomainName,".net")==NULL))
    {*/
         /*无法查找到.com .cn .net的时候,作为IP字符串直接进行处理*/
    /*    strcpy(host,pDomainName);
    }
    else
    {*/
        if ((he = gethostbyname((void *)pDomainName)) == NULL)   //*调用DNS函数直接进行转换*/
        {
            DbgPrintf("gethostbyname error\r\n");
            sleep(3);
            return 0;
        }
        DbgPrintf("Official name is: %s\n", he->h_name);
        DbgPrintf("IP addresses: ");
        addr_list = (struct in_addr **)he->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++)
        {
            DbgPrintf("%s \r\n", inet_ntoa(*addr_list[i]));
        }
        if(addr_list[0] != NULL)
            strcpy(host,inet_ntoa(*addr_list[0]));    //*使用第一个IP地址*/
    //}
    /*获取IP的IP地址的4个数据*/
    pServerIPStart=(void *)host;
    pServerIPEnd = (void *)strstr((void *)pServerIPStart,".");
    memset(gServerIP,0,sizeof(gServerIP));
    for(i=0;i<(pServerIPEnd-pServerIPStart);i++)
    {
        gServerIP[0] += HsCharToInt(pServerIPStart[i])*pow(10,pServerIPEnd-pServerIPStart-i-1);
    }

    pServerIPStart=pServerIPEnd+1;
    pServerIPEnd = (void *)strstr((void *)pServerIPStart,".");
    for(i=0;i<(pServerIPEnd-pServerIPStart);i++)
    {
        gServerIP[1] += HsCharToInt(pServerIPStart[i])*pow(10,pServerIPEnd-pServerIPStart-i-1);
    }

    pServerIPStart=pServerIPEnd+1;
    pServerIPEnd = (void *)strstr((void *)pServerIPStart,".");
    for(i=0;i<(pServerIPEnd-pServerIPStart);i++)
    {
        gServerIP[2] += HsCharToInt(pServerIPStart[i])*pow(10,pServerIPEnd-pServerIPStart-i-1);
    }

    pServerIPStart=pServerIPEnd+1;
    while(*pServerIPEnd!=0)
    {
        pServerIPEnd++;
    }
    for(i=0;i<(pServerIPEnd-pServerIPStart);i++)
    {
        gServerIP[3] += HsCharToInt(pServerIPStart[i])*pow(10,pServerIPEnd-pServerIPStart-i-1);
    }
    E2promWrite(gServerIP,StationIPAddr,4);
    return 1;
}

/*****************************************************************************
* Function     : EthernetSend()
* Description  : 以太网发送数据
* Input        : buffer:Data
                 length:data length
                 mode: 0 client  1 server
                 channel_id: 0 main channel  1 sub channel
* Output       : None
* Return       : 0:fail 1:success
* Note(s)      :
* Contributor  : 2018年11月8日        Andre
*****************************************************************************/
UINT8 EthernetSend(UINT8 *buffer,UINT8 length,UINT8 mode,UINT8 channel_id)
{
    int i, sendnum=0;
    UINT8 prefix[50]={0};

    //printf("[EthernetSend]get lock\n");

    while(g_ethernet_lock!=0)
    {
        usleep(5000);
        //printf("[EthernetSend]g_ethernet_lock=%d\n",g_ethernet_lock);
    }

    g_ethernet_lock=1;

    if((mode == CLIENT_MODE) && (channel_id == MAIN_CHANNEL))
    {
        sendnum = send(g_nRemotesockfd,buffer, length,0);
        sprintf((void *)prefix,"[Ethernet Client 0]");
    }
    else if((mode == CLIENT_MODE) && (channel_id == SUB_CHANNEL))
    {
        sendnum = send(g_nRemotesockfd_sub,buffer, length,0);
        sprintf((void *)prefix,"[Ethernet Client 1]");
    }
    else if((mode == SERVER_MODE) && (channel_id == MAIN_CHANNEL))
    {
        //sendnum = send(g_ethernet_sockfd,buffer, length,0);
        sprintf((void *)prefix,"[Ethernet Server 0]");
    }
    else if((mode == SERVER_MODE) && (channel_id == SUB_CHANNEL))
    {
        sendnum = send(g_ethernet_sockfd,buffer, length,0);
        sprintf((void *)prefix,"[Ethernet Server 1]");
    }

    g_ethernet_lock=0;

    if(sendnum==-1)                          //send fail
    {
        NorthPrintf("%sSend fail!\r\n", prefix);
        return 0;
    }
    else                                     //send success
    {
        NorthPrintf("%sSend msg:", prefix);
        for(i=0;i<length;i++)
        {
            NorthPrintf("%02X ",buffer[i]);
}
        NorthPrintf("\r\n");
        return 1;
    }
}

/*****************************************************************************
* Function     : EthernetAddRoute()
* Description  : 以太网添加路由
* Input        : *IP:ip address
* Output       : None
* Return       : None
* Note(s)      : route add default gw 192.168.1.1
                 route add -host 176.16.12.66 gw 10.11.10.1
                 route add -net 192.168.3.0 netmask 255.255.255.0 gw 192.168.2.1
* Contributor  : 2018年11月8日        Andre
*****************************************************************************/
void EthernetAddRoute(char *IP)
{
    DbgPrintf("[EthernetAddRoute]Add Route Start\r\n");
    char aRouteSet[100],n=255;
    char *pRoute="route add -net %d.%d.%d.0 netmask %d.%d.%d.0 gw %d.%d.%d.1 eth0";
    //char *pDefaultRoute="route add default gw %d.%d.%d.1";
    unsigned long pIP=inet_addr(IP);
    unsigned char *aIP=(unsigned char *)&pIP;

    if((aIP[0]==gLocalIP[0])&&(aIP[1]==gLocalIP[1])&&(aIP[2]==gLocalIP[2]))
    {
        DbgPrintf("[EthernetAddRoute]Same NetWork \r\n");
        DbgPrintf("[EthernetAddRoute]Add Route End\r\n");
        return ;
    }

    /*if(gLocalIP[0] < 64)
        sprintf(aRouteSet,pRoute,aIP[0],aIP[1],aIP[2],n,m,m,gLocalIP[0],gLocalIP[1],gLocalIP[2]);
    else if(gLocalIP[0] < 128)
        sprintf(aRouteSet,pRoute,aIP[0],aIP[1],aIP[2],n,n,m,gLocalIP[0],gLocalIP[1],gLocalIP[2]);
    else*/
        sprintf(aRouteSet,pRoute,aIP[0],aIP[1],aIP[2],n,n,n,gLocalIP[0],gLocalIP[1],gLocalIP[2]);

    DbgPrintf("[EthernetAddRoute]Static Route Add %s\r\n",aRouteSet);
    system(aRouteSet);
    DbgPrintf("[EthernetAddRoute]Add Route End\r\n");
}

/*****************************************************************************
* Function     : EthernetThreadMasterChannel()
* Description  : 以太网客户端主通道线程
* Input        : None
* Output       : None
* Return       : None
* Note(s)      :
* Contributor  : 2018年11月7日        Andre
*****************************************************************************/
void *EthernetThreadMasterChannel()
{
    int clientfd=0,res,i;
    int msgid=0;
    struct sockaddr_in client_addr;
    struct hostent *he;
    struct in_addr **addr_list;
    key_t portKey=0x01001010;
    int sin_size;
    int readnum;
    int connectRst = 1;
    Struct_Port_Msg sPortMsg;                        //TPMsgDef.h
    char host[50];
    unsigned long nBlock=1;
    struct timeval timeout;
    fd_set fdr,fdw;
    UINT16 nPort=12434;
    UINT8 aPortBuf[20],aServerHostName[30];
    UINT8 led_time_count=0;
    Struct_SPThreadInofPtr pThread = NULL;

    DbgPrintf("THREAD-----Ethernet Client Master Channel Init!\r\n");

    while(1)
    {
        //非有线模式下进行等待
        gIsReportInfo = _NO;
        while(gSocketMode!=0)
        {
            usleep(5000);
            DbgPrintf("[Ethernet Client 0]Ethernet Client Master Channel Quit!\r\n");
            return 0;
        }

        //配置网络信息时进行等待
        while((gDeviceIPSetTempFlag==1))
        {
            sleep(2);
            DbgPrintf("[Ethernet Client 0]Set Device IP Now.....\r\n");
            continue;
        }
        /*while(gMainDeviceStatus==0)
        {
            printf("Device Ethernet Server Busy:Status=%02d\r\n",gMainDeviceStatus);
            sleep(3);
            continue;
        }*/

        //建立Socket连接
        memset(host,0,sizeof(host));
        LedSet(1,0); //关闭连接成功指示灯
        do{
            clientfd = socket(AF_INET,SOCK_STREAM,0);  //获取Socket ID
            DbgPrintf("[Ethernet Client 0]socket init = %d \r\n",clientfd);
        }while(clientfd<1);
        usleep(5000);
        g_ethernet_connect=0;
        g_nRemotesockfd = 0;
        while(msgid == 0)
        {
            msgid=msgget(portKey++,IPC_CREAT|0666);
        }

        E2promRead((UINT8 *)&nPort,ServerDomainNamePortAddr,2);
        bzero(&client_addr,sizeof(struct sockaddr_in));
        client_addr.sin_family=AF_INET;
        client_addr.sin_port=htons(nPort);   //绑定端口
        DbgPrintf("[Ethernet Client 0]port = %d\r\n",nPort);

        //bind server ip
        if(gMainDeviceConfig==1)  //已保存网络信息
        {
            E2promRead(aServerHostName,ServerDomainNameAddr,30);
            printf("%s\r\n", aServerHostName);

            if ((he = gethostbyname((void *)aServerHostName)) == NULL) //调用DNS函数解析域名
            {
                DbgPrintf("[Ethernet Client 0]gethostbyname error\r\n");
                sleep(3);
                continue;
            }
            DbgPrintf("[Ethernet Client 0]Official name is: %s\n", he->h_name);
            DbgPrintf("[Ethernet Client 0]IP addresses: ");
            addr_list = (struct in_addr **)he->h_addr_list;
            for(i = 0; addr_list[i] != NULL; i++)
            {
                DbgPrintf("%s \r\n", inet_ntoa(*addr_list[i]));
            }
            if(addr_list[0] != NULL)
                strcpy(host,inet_ntoa(*addr_list[0]));
        }
        else
        {
            strcpy(host,"182.150.21.253"); //未保存信息时采用默认信息
        }
        DbgPrintf("[Ethernet Client 0]Server IP = %s\r\n",host);
        inet_aton(host,&client_addr.sin_addr);
        sin_size=sizeof(struct sockaddr_in);
        if(clientfd < 1)
        {
            DbgPrintf("[Ethernet Client 0]client = %d\r\n",clientfd);
            shutdown(clientfd,2);
            sleep(3);
            continue;
        }
        ioctl(clientfd,FIONBIO,&nBlock); //配置为非阻塞模式进行连接
        EthernetAddRoute(host);

        DbgPrintf("[Ethernet Client 0]waiting to connect server\n");
        if((connectRst=connect(clientfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr)))!=0)
        {
            if(errno != EINPROGRESS)
            {
                DbgPrintf("[Ethernet Client 0]the connect error!\n");
                close(clientfd);
                sleep(2);
                continue;
            }
        }

        FD_ZERO(&fdr);
        FD_ZERO(&fdw);
        FD_SET(clientfd,&fdr);
        FD_SET(clientfd,&fdw);

        timeout.tv_sec=5;
        timeout.tv_usec=0;
        res = select(clientfd+1,&fdr,&fdw,NULL,&timeout);
        if(res==0)
        {
            DbgPrintf("[Ethernet Client 0]connect timeout\r\n");
            shutdown(clientfd,2);
            close(clientfd);
            continue;
        }
        else if(res==1)
        {
            DbgPrintf("[Ethernet Client 0]connect success\r\n");
        }
        else
        {
            DbgPrintf("[Ethernet Client 0]connect error\r\n");
            close(clientfd);
            sleep(5);
            continue;
        }
        nBlock = 0;
        ioctl(clientfd,FIONBIO,&nBlock);  //配置为阻塞模式进行读取数据

        //连接成功后等待读取数据
        g_nRemotesockfd = clientfd;
        LedSet(1,1);
        DbgPrintf("****************************************************\n");
        DbgPrintf("[Ethernet Client 0]the remote connect clientfd=%d\n",g_nRemotesockfd);
        DbgPrintf("****************************************************\n");

        memset(aPortBuf,0,sizeof(aPortBuf));
        sprintf((void *)aPortBuf,TestLogServerPortRegex,nPort);
        TestLogStringFileWrite(aPortBuf,strlen((void *)aPortBuf));
        g_ethernet_connect = 1;
        sleep(1);

        //与平台建立连接后，上报设备信息
        ReportDeviceInfoToServer();  //上报数采信息
        sleep(2);
        SendFrameToPlatform(gMainDeviceID,DEVICE_MAXNUMBER_CMD,S_R_InfoReport,(UINT8 *)&gConnectDeviceMaxNum,sizeof(gConnectDeviceMaxNum));

        while(1)
        {
            bzero(sPortMsg.PortMsgBuf,MaxMsgBufLength);
            readnum=recv(g_nRemotesockfd, sPortMsg.PortMsgBuf, MaxMsgBufLength, 0); //接收Socket信息
            if(readnum==-1)
            {
                DbgPrintf("[Ethernet Client 0]the read error!\n");
                shutdown(g_nRemotesockfd,2);
                //TestLogTimeFileWrite();
                //TestLogStringFileWrite("Disconnect Server:recv<0\n",strlen("Disconnect Server:recv<0\n"));
                g_nRemotesockfd = 0;
                sleep(5);
                break;
            }
            else if(0 == readnum)
            {
                DbgPrintf("[Ethernet Client 0]the read timeout!\n");
                shutdown(g_nRemotesockfd,2);
                //TestLogTimeFileWrite();
                //TestLogStringFileWrite("Disconnect Server:recv=0\n",strlen("Disconnect Server:recv=0\n"));
                g_nRemotesockfd = 0;
                break;
            }
            else
            {
                led_time_count++;
                if(led_time_count == 3)
                {
                    LedSet(2,0);
                    LedSet(4,0);
                    LedSet(3,0);
                    LedSet(4,1);
                    LedSet(3,1);
                    led_time_count = 0;
                }

                DbgPrintf("[Ethernet Client 0]the client receive:%d\n", readnum);
                sPortMsg.lMessageType=3;
                sPortMsg.nDateLength=readnum;
                sPortMsg.portid.Port_MsgID=msgid;
                sPortMsg.portid.Port_TypeID=clientfd;
                //printf("[Ethernet Client 0]Enet_Monitor.Port_MsgID=%d,Enet_Monitor.Port_TypeID=%d\n",sPortMsg.portid.Port_MsgID, sPortMsg.portid.Port_TypeID);
                if(GetThread(3,&pThread))                //获取消息处理线程
                {
                    if(msgsnd(pThread->nMsgQueueID, (void *)&sPortMsg, Port_Msg_Length(Struct_Port_Msg), 0) == -1)
                    {
                        //perror("message posted!\n");
                        g_nRemotesockfd = 0;
                        DbgPrintf("[Ethernet Client 0]Error 1!!!");
                        break;
                    }
                    //printf("[Ethernet Client 0]GetThread(3,&pThread)pThread->nMsgQueueID is %d\n",pThread->nMsgQueueID);
                }
                else
                {
                    //printf("[Ethernet Client 0]Get Port Thread error!\r\n");
                    g_nRemotesockfd = 0;
                    DbgPrintf("[Ethernet Client 0]Error 2!!!");
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
* Function     : EthernetThreadSlaveChannel()
* Description  : 以太网客户端次通道线程
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : 此通道在主通道建立之后再建立,通过select和状态EINPROGRESS实现
                 socket 连接超时判断
* Contributor  : 2018年11月12日        Andre
*****************************************************************************/
void *EthernetThreadSlaveChannel()
{
    int clientfd=0,res=0,readnum=0,i;
    struct sockaddr_in client_addr;
    struct hostent *he;
    struct in_addr **addr_list;
    char host[50]={0};
    unsigned long nBlock=1;
    struct timeval timeout;
    fd_set fdr,fdw;
    UINT16 nPort=12434;
    UINT8 aPortBuf[20]={0},aServerHostName[30]={0},buf[256]={0};

    DbgPrintf("THREAD-----Ethernet Client Slave Channel Init!\r\n");

    while(1)
    {
        g_ethernet_connect_sub=0;
        g_nRemotesockfd_sub = 0;

        if((gModeSub!=2)||(gSocketModeSub!=1)||(gModemSlaveChannelSwitch==0)||(gMainDeviceStatus!=10)||(gModemSlaveConfigFlag!= 1))
        {
            usleep(500);
            DbgPrintf("[Ethernet Client 1]Ethernet Client Slave Channel Quit!\r\n");
            return 0;
        }

        //主通道连接未完成时进行等待
        if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0)) ||
          ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER)))
        {
            sleep(5);
            DbgPrintf("[Ethernet Client 1]Waiting for master channel ready\r\n");
            continue;
        }

        //配置网络信息时进行等待
        while((gDeviceIPSetTempFlag==1))
        {
            sleep(2);
            DbgPrintf("[Ethernet Client 1]Set Device IP Now.....\r\n");
            continue;
        }

        //建立Socket连接
        memset(host,0,sizeof(host));
        do{
            clientfd = socket(AF_INET,SOCK_STREAM,0);  //获取Socket ID
            DbgPrintf("[Ethernet Client 1]socket init = %d \r\n",clientfd);
        }while(clientfd<1);
        usleep(5000);

        E2promRead((UINT8 *)&nPort,SlaveDomainNamePortAddr,2);
        bzero(&client_addr,sizeof(struct sockaddr_in));
        client_addr.sin_family=AF_INET;
        client_addr.sin_port=htons(nPort);   //绑定端口
        DbgPrintf("[Ethernet Client 1]port = %d\r\n",nPort);

        //bind server ip
        E2promRead(aServerHostName,SlaveDomainNameAddr,30);
        if ((he = gethostbyname((void *)aServerHostName)) == NULL) //调用DNS函数解析域名
        {
            DbgPrintf("gethostbyname error\r\n");
            sleep(3);
            continue;
        }
        DbgPrintf("[Ethernet Client 1]Official name is: %s\n", he->h_name);
        DbgPrintf("[Ethernet Client 1]IP addresses: ");
        addr_list = (struct in_addr **)he->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++)
        {
            DbgPrintf("%s \r\n", inet_ntoa(*addr_list[i]));
        }
        if(addr_list[0] != NULL)
            strcpy(host,inet_ntoa(*addr_list[0]));

        DbgPrintf("[Ethernet Client 1]Server IP = %s\r\n",host);
        inet_aton(host,&client_addr.sin_addr);

        if(clientfd < 1)
        {
            DbgPrintf("[Ethernet Client 1]client = %d\r\n",clientfd);
            shutdown(clientfd,2);
            sleep(3);
            continue;
        }

        ioctl(clientfd,FIONBIO,&nBlock); //配置为非阻塞模式进行连接
        EthernetAddRoute(host);

        DbgPrintf("[Ethernet Client 1]waiting to connect server\n");
        if(connect(clientfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr))!=0) //connect()成功返回0，出错返回-1。
        {
            if(errno != EINPROGRESS)     // EINPROGRESS means the connecting of the socket fd is established.
            {
                DbgPrintf("[Ethernet Client 1]the connect error!\n");
                close(clientfd);
                sleep(2);
                continue;
            }
        }

        FD_ZERO(&fdr);
        FD_ZERO(&fdw);
        FD_SET(clientfd,&fdr);
        FD_SET(clientfd,&fdw);

        timeout.tv_sec=5;
        timeout.tv_usec=0;
        res = select(clientfd+1,&fdr,&fdw,NULL,&timeout); // ">0" means sockfd ready to read, "=0" means timeout cause retrun, "<0" means error.
        if(res==0)
        {
            DbgPrintf("[Ethernet Client 1]connect timeout\r\n");
            shutdown(clientfd,2);
            close(clientfd);
            continue;
        }
        else if(res==1)
        {
            DbgPrintf("[Ethernet Client 1]connect success\r\n");
        }
        else
        {
            DbgPrintf("[Ethernet Client 1]connect error\r\n");
            close(clientfd);
            sleep(5);
            continue;
        }
        nBlock = 0;
        ioctl(clientfd,FIONBIO,&nBlock);  //配置为阻塞模式进行读取数据

        //连接成功后等待读取数据
        g_nRemotesockfd_sub = clientfd;
        DbgPrintf("****************************************************\n");
        DbgPrintf("[Ethernet Client 1]the remote connect clientfd=%d\n",g_nRemotesockfd_sub);
        DbgPrintf("****************************************************\n");

        memset(aPortBuf,0,sizeof(aPortBuf));
        sprintf((void *)aPortBuf,TestLogServerPortRegex,nPort);
        TestLogStringFileWrite(aPortBuf,strlen((void *)aPortBuf));
        g_ethernet_connect_sub = 1;
        sleep(1);

        //与平台建立连接后，上报设备信息
        ReportDeviceInfoToThirdPartyServer(SOCKETMODE_LAN);  //上报数采信息
        sleep(2);
        SendFrameToThirdPartyPlatform(gMainDeviceID,DEVICE_MAXNUMBER_CMD,S_R_InfoReport,(UINT8 *)&gConnectDeviceMaxNum,sizeof(gConnectDeviceMaxNum),SOCKETMODE_LAN);
        gTypeGroupPointTran=NULL;
        gDLinkDeviceInfoCount = 0;

        while((gModeSub == 2) && (gSocketModeSub == 1) && (g_ethernet_connect_sub == 1))
        {
            bzero(buf,MaxMsgBufLength);
            readnum=recv(g_nRemotesockfd_sub, buf, MaxMsgBufLength, 0); //接收Socket信息
            if(readnum==-1)
            {
                DbgPrintf("[Ethernet Client 1]the read error!\n");
                shutdown(g_nRemotesockfd_sub,2);
                //TestLogTimeFileWrite();
                //TestLogStringFileWrite("Disconnect Server:recv<0\n",strlen("Disconnect Server:recv<0\n"));
                g_nRemotesockfd_sub = 0;
                sleep(5);
                break;
            }
            else if(0 == readnum)
            {
                shutdown(g_nRemotesockfd_sub,2);
                //TestLogTimeFileWrite();
                //TestLogStringFileWrite("Disconnect Server:recv=0\n",strlen("Disconnect Server:recv=0\n"));
                g_nRemotesockfd_sub = 0;
                break;
            }
            else
            {
                DbgPrintf("[Ethernet Client 1]the client receive:%d\n", readnum);
                gSocketHeartChannel1Count=0;
                AddMsgToSGCCReceiveBuff(buf,readnum);
                if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0)) ||
                  ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER))||
                  (gModemSlaveChannelSwitch==0)||(gMainDeviceStatus!=10)||
                  (gSocketModeSub!=1)||(gModeSub != 2))
                {
                    shutdown(g_nRemotesockfd_sub,2);
                    g_nRemotesockfd_sub = 0;
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
* Function     : EthernetServerThreadSlaveChannel()
* Description  : 以太网服务器线程
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : INADDR_ANY 表示服务器自动填充本机IP地址
* Contributor  : 2018年11月7日        Andre
*****************************************************************************/
void *EthernetServerThreadSlaveChannel()
{
    UINT8 res=0;

    DbgPrintf("THREAD-----Ethernet Server Slave Channel Init!\r\n");

    res=ReadThirdPartyFile();                                    //read third party config file
    PrintThirdPartyList();
    EthernetAddRoute((void *)gThirdPartyStationInfo.nIP);        //add route

    while((res)&&(gModeSub == 1)&&(gSocketModeSub==1)&&(gModemSlaveChannelSwitch==1)&&(gModemSlaveConfigFlag==1)&&(gMainDeviceStatus==10))
    {
        struct sockaddr_in server_addr, client_addr;
        int sockfd, clientfd=0, on;
        socklen_t sin_size;
        struct linger lig;

        /*建立Socket Server并监听*/
        sockfd=socket(AF_INET,SOCK_TYPE,0);                      //get socket id
        bzero(&server_addr,sizeof(struct sockaddr_in));          //clear struct server_addr
        server_addr.sin_family=AF_INET;                          //set address family as IPv4 protocol
        server_addr.sin_addr.s_addr=htonl(INADDR_ANY);           //set IP address
        server_addr.sin_port=htons(gThirdPartyStationInfo.nPort); //set port
        lig.l_onoff=1;
        lig.l_linger=2;
        setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char*)&lig, sizeof(struct linger));
        on=1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        //pthread_mutex_lock(&ethernetsem);

        while(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) //bind port with local address
        {
            DbgPrintf("[Ethernet Server 1]the bind error!\r\n");
            sleep(10);
            if((gModeSub!=1)||(gSocketModeSub!=1)||(gModemSlaveChannelSwitch==0)||(gModemSlaveConfigFlag==0)||(gMainDeviceStatus!=10))
            {
                usleep(500);
                break;
            }
        }
        //pthread_mutex_unlock(&ethernetsem);
        DbgPrintf("[Ethernet Server 1]TCP server port: %d\r\n", ntohs(server_addr.sin_port));

        while(1)
        {
            if((gModeSub!=1)||(gSocketModeSub!=1)||(gModemSlaveChannelSwitch==0)||(gModemSlaveConfigFlag==0)||(gMainDeviceStatus!=10))
            {
                usleep(500);
                DbgPrintf("[Ethernet Server 1]Ethernet Server Slave Channel Quit!\r\n");
                break;
            }

            //do not connect southern devices until northern connect is built
            if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0)) ||
              ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER)))
            {
                sleep(5);
                DbgPrintf("[Ethernet Server 1]Waiting for master channel ready!\r\n");
                continue;
            }

            /*监听连接*/
            //pthread_mutex_lock(&ethernetsem);
            if(listen(sockfd,MAX_Lisn_QUE_NUM)==-1)              //listen this socket. if failed, return -1
            {
                DbgPrintf("[Ethernet Server 1]the listen error!\r\n"); //show err
                //pthread_mutex_unlock(&ethernetsem);
                break;
            }
            else
            {
                DbgPrintf("[Ethernet Server 1]waiting accept from device!\r\n");
                sin_size=sizeof(struct sockaddr_in);             //get size of struct sockaddr_in(in byte)
                /*建立连接*/
                if((clientfd=accept(sockfd,(struct sockaddr *)(& client_addr),&sin_size))==-1) //create a new socket for receive and send data
                {
                    /*连接建立失败,关闭连接*/
                    DbgPrintf("[Ethernet Server 1]the connect error!\r\n"); //show err
                    sleep(1);
                    shutdown(sockfd,2);                          //shutdown this socket
                    close(sockfd);                               //close this socket
                    //pthread_mutex_unlock(&ethernetsem);
                    break;
                }
                else
                {
                    int readnum;
                    UINT8 aRecvBuf[1024],client_address[20]={0};

                    memcpy(client_address,inet_ntoa(client_addr.sin_addr),20);
                    DbgPrintf("[Ethernet Server 1]connect %s:%d successful\n",client_address,ntohs(client_addr.sin_port));

                    DbgPrintf("[Ethernet Server 1]APP clientfd=%d\n",clientfd);
                    //pthread_mutex_unlock(&ethernetsem);
                    g_ethernet_connect_status = 1;
                    g_ethernet_sockfd = clientfd;
                    while(1)
                    {
                        if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0))||
                          ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER))||
                          (gModeSub != 1)||(gSocketModeSub != 1)||(gModemSlaveChannelSwitch == 0)||
                          (gMainDeviceStatus!=10)||(gModemSlaveConfigFlag != 1))
                            break;
                        bzero(aRecvBuf,1024);                   //clear receive buffer
                        //pthread_mutex_lock(&ethernetsem);
                        readnum=recv(g_ethernet_sockfd,aRecvBuf,1024,0);  //receive data from Ethernet port
                        //pthread_mutex_unlock(&ethernetsem);
                        if(readnum==-1)                          //receive fail
                        {
                            usleep(1000);
                            continue;
                        }
                        else if(0 == readnum)                    //no data
                        {
                            usleep(1000);
                            DbgPrintf("[Ethernet Server 1]the client quit!\r\n");
                            break;
                        }
                        else                                     //receive success
                        {
                            int i= 0;

                            NorthPrintf("[Ethernet Server 1]the client receive:"); //show data
                            for(i=0; i<readnum; i++)
                            {
                                NorthPrintf("%02X ", aRecvBuf[i]);    //show data
                            }
                            NorthPrintf("\r\n");

                            gSocketHeartChannel1Count=0;
                            AddMsgToSGCCReceiveBuff(aRecvBuf,readnum);
                        }
                        usleep(5000);
                    }
                    g_ethernet_connect_status = 0;
                    break;
                }
            }
        }
        shutdown(sockfd,SHUT_RDWR);
        shutdown(g_ethernet_sockfd,SHUT_RDWR);
        DbgPrintf("[Ethernet Server 1]Socket Shutdown!\r\n");
        close(sockfd);                                            //close this socket
        close(g_ethernet_sockfd);
        DbgPrintf("[Ethernet Server 1]Socket Close!\r\n");
        usleep(1000);
    }
    if(res==0)
    {
        DbgPrintf("[Ethernet Server 1]Read file /mnt/flash/OAM/thirdparty/config.ini fail!\r\n");
        DbgPrintf("[Ethernet Server 1]Please check config file and restart logger!!!\r\n");
    }

    //如果品联平台未确认断链告警，重新上报
        //if()
        {

        }
    DbgPrintf("[Ethernet Server 1]Ethernet Server Slave Channel Quit!\r\n");
    return 0;
}


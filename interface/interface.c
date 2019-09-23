/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : interface.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <string.h>
#include <sys/socket.h>
#include "common.h"
#include "uartExtern.h"
#include "exporthelper.h"
#include "../protocal/ProtocolDef.h"
#include "../XML/xml.h"
#include "../INI/libini.h"
#include "../Module/Module.h"
#include "../Ethernet/EthernetExtern.h"
#include "../SGCC/store.h"
#include "../protocal/ToolExtern.h"
#include "../Alarm/alarmExtern.h"

UINT8 gMainDeviceName[20];                   //数采名称
UINT8 gMainDeviceSN[20];                     //数采ESN
UINT8 gMainDeviceModel[20];                  //数采类型
UINT8 gMainDeviceType[20];                   //数采型号
UINT8 gMainDeviceStatus=0;                   //Logger Init Flag   1:init success 0:null
UINT8 gMainDeviceIP[4];
UINT8 gMainDeviceConfig;                     //Logger Build Station Status 1:build ok 0:null
UINT8 dYTPiontCount = 0;

UINT8 gStationID[32];                        //Station ID
UINT8 gStationName[20];                      //Station Name
UINT32 gStationLongitude;                    //StationLongitude
UINT32 gStationLatitude;                     //StationLatitude

UINT8 gDeviceIPSetFlag=0;                    //Device IP Set flag  1:set 0:null
UINT8 gDeviceIPSetTempFlag=0;                //Device IP Config Status 1:in config thread 0:normal
UINT8 gServerIP[4];                          //Socket Server IP
UINT32 gDeviceOldVer=0;                      //legacy version
UINT8 gDeviceStationBuild=0;                 //station build status 2:connect server 4:upload station information 6:start build station

UINT8 g_TransSta=1;                          //socket work status 0,stop 1:start 3:call 4:call end 5:DD call 6:DD call end 7:call with time 8:call with time end
UINT16 gMainDeviceID=1;                      //logger device ID
UINT16 gRecvCount=0;                         //recv packet count
UINT16 gSendCount=0;                         //send packet count

UINT8 gConnectDeviceNum=0;                   //the number of south device
UINT8 gConnectDeviceNumBuf=0;                //the number of south device buffer
UINT8 gConnectDeviceMaxNum=40;               //maximum number of south device
UINT8 gWirelessNetMode=0;                    //module network mode
UINT8 gDeviceTypeNum=0;                      //the type number of south device
UINT8 gDeviceTypeNumBuf=0;                   //the type number of south device
UINT16 gDeviceTypeBuf[40];                   //the type number of south device
UINT16 gYXPointNum=0;                        //the total number of 104 YX point
UINT16 gYXPointNumBuf=0;                     //the total number of 104 YX point
UINT16 gYCPointNum=0;                        //the total number of 104 YC point
UINT16 gYCPointNumBuf=0;                     //the total number of 104 YC point
UINT16 gYKPointNum=0;                        //the total number of 104 YK point
UINT16 gYKPointNumBuf=0;                     //the total number of 104 YK point
UINT16 gSDPointNum=0;                        //the total number of 104 SD point
UINT16 gSDPointNumBuf=0;                     //the total number of 104 SD point
UINT16 gDDPointNum=0;                        //the total number of 104 DD point
UINT16 gDDPointNumBuf=0;                     //the total number of 104 DD point

UINT16 gYCPointCount=0;                      //the count of 104 YC point in call process
UINT16 gYCTimePointCount=0;                  //the count of 104 YC point in time call process
UINT16 gYXPointCount=0;                      //the count of 104 YX point in call process
UINT16 gYXTimePointCount=0;                  //the count of 104 YX point in time call process
UINT16 gDDPointCount=0;                      //the count of 104 DD point in call process
UINT16 gDDTimePointCount=0;                  //the count of 104 DD point in time call process
UINT8 gTimeCallBackTime[7];                  //time of the call point

sDeviceInfo gDeviceInfo[MAXDEVICE];          //information of the south device
sDeviceInfo gDeviceInfoBuf[MAXDEVICE];       //information of the south device

UINT8 gBufCount=0;                           // S/U frame count in recv buffer
UINT8 gIBufCount=0;                          // I frame count in recv buffer
s104Point aPointBuf[0x6601];                 // 104 point data buffer
UINT32 aPointTimeBuf[0x6601];                // 104 point record data buffer
UINT8 gPhone[11];                            //alarm upload phone number
UINT8 gDLinkDeviceInfoCount=0;               //遍历最大支持设备数时的计数
UINT8 gDLinkDeviceCount=0;                   //下联设备信息获取计数

UINT8 gMasterReportSwitch=0;                 //Master channel mode 0:normal  1:only control(not report)
UINT8 gSocketMode=1;                         //Socket module 0:ethernet 1:modem mode
UINT8 gModeSub=0;                            //Sub channel mode 1:server 2:client
UINT8 gSocketModeSub=0;                      //Sub channel socket mode 1:ethernet 2:modem mode
UINT8 gSocketHeartChannel0Count=0;           //socket channel 0 error count
UINT8 gSocketHeartChannel1Count=0;           //socket channel 1 error count
UINT8 gModuleChannel0InitFlag=0;             //Channel 0 Init Flag: 0:no Connection  1:Modem Config Over 2:Modem Connect to server
UINT8 gModuleChannel1InitFlag=0;             //Channel 1 Init Flag: 0:no Connection  1:Modem Config Over 2:Modem Connect to server
UINT8 gModemNetStatus=0;                     //Net Status bit0 0:Normal 1:Connect Success  bit4 0:No query 1:Must Report to APP
UINT8 gModemSlaveChannelSwitch = 0;          //Sub channel switch 0:close 1:open
UINT8 gModemSlaveConfigFlag = 0;             //Sub channel config flag 0:not yet 1:already
UINT8 gSocketHeartChannel1Flag = 0;          //Socket heartbeat slave channel flag 0:not exist 1:exist

UINT32 gUpdataCount=0;                       //recv update packet number
UINT8  gUpdataModeFlag=0;                    //0:normal 1:in update mode
UINT8  gDt1000UpdataFlag=0;                  //0:normal 1:in DT1000 update mode

UINT32 gIVLicenseCount=0;
UINT8  gIVLicenseStatus=0;                   //0:normal  FF:report all device license info
UINT8  gIVCRCHi=0xFF;
UINT8  gIVCRCLo=0xFF;
UINT16 g_export_file_crc =0x00;
UINT16 g_import_file_crc =0x00;
UINT8  gIVLicenseDevice=0;
sIVStruct gDeviceIVInfo[MAXDEVICE];

UINT8 gIVScanfUpload[5];
UINT8 gIVScanfPoint[40];
UINT8 gIVScanfCount=0;
UINT8 gTotalCallFlag=0;
UINT8 gTimeTotalCallFlag=0;
UINT8 gEleTotalCallFlag=0;
UINT8 DeviceSdSum=0;//YT point sum

struct sTypeGroup *gTypeHead=NULL;           //head of point table list
struct sTypeGroup *gTypeHeadBuf=NULL;        //head of point table list
struct sTypeGroup *gTypeGroupPoint=NULL;     //point of point table type list
struct sTypeGroup *gTypeGroupPointBuf=NULL;  //point of point table type list
struct sTypeParam *gTypeParamPoint=NULL;     //point of point table param list
struct sTypeParam *gTypeParamPointBuf=NULL;  //point of point table param list
struct sTypeGroup *gTypeGroupPointTran=NULL;
struct sTypeParam *gTypeParamPointTran=NULL;
UINT16 gTypeCount=0;                         //point table number

struct sSocketRecvBuf *gRecvBufHead=NULL;    //the head of Recv S/U frame list
struct sSocketRecvBuf *gRecvBufPoint=NULL;   //point of Recv frame list
struct sSocketRecvBuf *gRecvIBufHead=NULL;   //the head of Recv I frame list
struct sSocketRecvBuf *gRecvIBufPoint=NULL;  //point of Recv I fram list

UINT8 load_point_table_flag=0;               //点表传输标志位

//以下三个全局变量 暂时不用，存在于TimeoutThread线程中
UINT8 gTimeCount = 0xFF;
UINT8 gLogFileBufferFrame[SYS_FRAME_LEN] = {0}; //APDU:暂存帧该帧，以备重传
UINT8 g_export_file_path[50];
UINT8 g_import_file_path[50];
extern struct sDt1000Update gDt1000Update;

//以下用于三方平台
struct sThirdPartySinglePoint *gThirdPartySinglePointHead=NULL;     //head of point table list
struct sThirdPartySinglePoint *gThirdPartySinglePointPoint=NULL;    //point of point table list
//struct sThirdPartySubPoint   *gThirdPartySinglePointPoint=NULL;   //point of single point list
UINT16 gThirdPartyPointTableCount=0;         //number of point table
UINT16 gPointTableList[MAX_DEVICE_TYPE]={0};

extern UINT8 gMBQueryDeviceCountBuf;
extern struct sTypeGroup *gTypeGroupPointMB;
extern struct sTypeGroup *psPointTable;
extern sDeviceInfo sPlcInfoBuf[2];//PLC
extern UINT8 PlcC7Buff0[84];
extern UINT8 PlcC7Buff1[84];
extern UINT8 PLC0ConnectiveStatus;
extern UINT8 PLC1ConnectiveStatus;
extern UINT8 gUploadHWDeviceFlag;

/*****************************************************************************
* Function     : DriverInit()
* Description  : Config logger parameters
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年12月12日        Andre
*****************************************************************************/
void DriverInit(void)
{
    UINT8 nTemp;
    int i;

    pthread_mutex_init(&e2promSem,NULL);
    pthread_mutex_init(&threadFileSem,NULL);
    pthread_mutex_init(&socketbufsem,NULL);
    pthread_mutex_init(&APsem,NULL);
    pthread_mutex_init(&alarmsem,NULL);
    pthread_mutex_init(&modemsem,NULL);
    pthread_mutex_init(&Uartsem,NULL);
    pthread_mutex_init(&logsem,NULL);
    pthread_mutex_init(&fpgaSem,NULL);
    pthread_mutex_init(&LogApartSem,NULL);
    pthread_mutex_init(&sgcc_recv_buff_sem,NULL);
    pthread_mutex_init(&sgcc_send_buff_sem,NULL);


    printf("\r\nSystem boot start!\r\n");
    printf("Software Version = %X\r\n",SYS_OAM_VERSION);
    printf("Dubug Version = %04X\r\n",SYS_DEBUG_VERSION);

#ifdef __DEBUG__
    printf("*******************************************************************\r\n");
    printf("\r\nCaution!!!This is debug version!!!\r\n");
    printf("\r\nCaution!!!This is debug version!!!\r\n");
    printf("\r\nCaution!!!This is debug version!!!\r\n");
    printf("*******************************************************************\r\n");
#endif

    RTCGet();
    LedSet(3,0);
    LedSet(4,0);
    memset(gDeviceInfo,0,sizeof(gDeviceInfo));
    memset(aPointBuf,0,sizeof(aPointBuf));
    for(i=1;i<0x6601;i++)
        aPointBuf[i].nValue=0xFFFFFFFF;
    E2promRead(gDeviceAlarmBuf,AlarmStatusAddr,10);
    if((gDeviceAlarmBuf[5]==0xff) && (gDeviceAlarmBuf[6]==0xff) && (gDeviceAlarmBuf[7]==0xff) && (gDeviceAlarmBuf[8]==0xff) &&(gDeviceAlarmBuf[9]==0xff))
        memset(gDeviceAlarmBuf,0,sizeof(gDeviceAlarmBuf));

    ModulePowerSet(0);
    E2promRead((UINT8 *)&gMainDeviceID,DeviceAddress,2);
    if((gMainDeviceID==0) || (gMainDeviceID==65535))
        gMainDeviceID=1;

    memset(gMainDeviceName,0,sizeof(gMainDeviceName));
    E2promRead((UINT8 *)&gMainDeviceName,DeviceNameAddr,20);

    memset(gMainDeviceSN,0,sizeof(gMainDeviceSN));
    E2promRead((UINT8 *)&gMainDeviceSN,DeviceSNAddr,20);

    memset(gMainDeviceModel,0,sizeof(gMainDeviceModel));
    E2promRead((UINT8 *)&gMainDeviceModel,DeviceModelAddr,20);

    memset(gMainDeviceType,0,sizeof(gMainDeviceType));
    E2promRead((UINT8 *)&gMainDeviceType,DeviceTypeAddr,20);

    E2promRead((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
    if(gMainDeviceStatus>DEVSTATUS_NORMAL_WORKMODE)
        gMainDeviceStatus=DEVSTATUS_NEW_DEVICE;
    if(gMainDeviceStatus==1)
        gMainDeviceStatus=DEVSTATUS_NORMAL_WORKMODE;
    E2promRead((UINT8 *)&gMainDeviceIP,DeviceIP_E2P,4);
    if(gMainDeviceIP[0]==255)
    {
        gMainDeviceIP[0]=192;
        gMainDeviceIP[1]=168;
        gMainDeviceIP[2]=1;
        gMainDeviceIP[3]=176;
    }
    E2promRead((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
    if(gMainDeviceConfig>1)
        gMainDeviceConfig=0;

    E2promRead(gPhone,DevicePhoneAddr,11);
    if(gPhone[0]==255)
    {
        gPhone[0]=0;
    }
    printf("************************Logger  Information*************************\r\n");
    printf("ID = %d   IP = %d.%d.%d.%d\r\n",DeviceAddress,gMainDeviceIP[0],gMainDeviceIP[1],gMainDeviceIP[2],gMainDeviceIP[3]);

    for(i=0;i<20;i++)
    {
        if(gMainDeviceName[i]==0xFF)
            gMainDeviceName[i]=0;
        //printf("0x%02X ",gMainDeviceSN[i]);
        if(gMainDeviceSN[i]==0xFF)
            gMainDeviceSN[i]=0;
        if(gMainDeviceModel[i]==0xFF)
            gMainDeviceModel[i]=0;
        if(gMainDeviceType[i]==0xFF)
            gMainDeviceType[i]=0;
    }
    if(gMainDeviceName[0]!=0)
        printf("Name =%s\r\n",gMainDeviceName);
    else
        printf("Name = NULL\r\n");
    if(gMainDeviceSN[0]!=0)
    {
        UINT8 aMacCmd[50],aMacTemp[50]="ifconfig eth0 hw ether %X:%X:%X:%X:%X:%X";
        UINT8 aSNBuf[6];

        printf("SN   =%s\r\n",gMainDeviceSN);
        memset(aSNBuf,0,sizeof(aSNBuf));
        system("ifconfig eth0 down");
        sleep(1);
        switch(gMainDeviceSN[15])
        {
            case 'R':
            case 'N':
            case 'r':
                gConnectDeviceMaxNum=40;
                break;
            case 'E':
            case 'e':
                gConnectDeviceMaxNum=30;
                break;
            case 'F':
            case 'f':
                gConnectDeviceMaxNum=15;
                break;
            case 'T':
            case 't':
                gConnectDeviceMaxNum=5;
                break;
            default:
                gConnectDeviceMaxNum=40;
        }
        switch(gMainDeviceSN[16])
        {
            case 'D':
            case 'd':
                gWirelessNetMode=Net_Mode_4G;
                break;
            default:
                gWirelessNetMode=Net_Mode_3G;
        }
        if(strlen((void *)gMainDeviceSN)<8)
        {
            UINT8 nSNCount=0;

            for(nSNCount=0;nSNCount<strlen((void *)gMainDeviceSN);nSNCount++)
            {
                aSNBuf[nSNCount]=gMainDeviceSN[nSNCount];
            }
        }
        else
        {
            memcpy(aSNBuf,(UINT8 *)&gMainDeviceSN[strlen((void *)gMainDeviceSN)-8],6);
        }
        aSNBuf[0] &= 0xFE;
        sprintf((void *)aMacCmd,(void *)aMacTemp,aSNBuf[0],aSNBuf[1],aSNBuf[2],aSNBuf[3],aSNBuf[4],aSNBuf[5]);
        system((void *)aMacCmd);
        sleep(1);
    }
    else
        printf("SN   = NULL\r\n");
    if(gMainDeviceModel[0]!=0)
        printf("Model=%s\r\n",gMainDeviceModel);
    else
        printf("Model= NULL\r\n");
    if(gMainDeviceType[0]!=0)
        printf("Type =%s\r\n",gMainDeviceType);
    else
        printf("Type = NULL\r\n");

    memset(gStationID,0,sizeof(gStationID));
    E2promRead((UINT8 *)&gStationID,StationIDAddr,32);

    memset(gStationName,0,sizeof(gStationName));
   // E2promRead((UINT8 *)&gStationName,StationNameAddr,20);

    E2promRead((UINT8 *)&gStationLongitude,StationLongitudeAddr,4);
    E2promRead((UINT8 *)&gStationLatitude,StationLatitudeAddr,4);
    E2promRead(gServerIP,StationIPAddr,4);
    printf("************************Station Information*************************\r\n");
    for(i=0;i<20;i++)
    {
        if(gStationName[i]==0xFF)
            gStationName[i]=0;
    }
    for(i=0;i<32;i++)
    {
        if(gStationID[i]==0xFF)
            gStationID[i]=0;
    }
    if(gStationID[0]!=0)
        printf("ID =%s\r\n",gStationID);
    else
        printf("ID = NULL\r\n");
    /*if(gStationID[0]!=0)
        printf("Name =%s\r\n",gStationName);
    else
        printf("Name = NULL\r\n");*/
    memset(gDeviceIVInfo,0,sizeof(gDeviceIVInfo));
    memset(gIVScanfPoint,0,sizeof(gIVScanfPoint));
    E2promRead((UINT8 *)&gDeviceIPSetFlag,DeviceIPSetAddr,1);
    E2promRead((UINT8 *)&gDeviceOldVer,DeviceSwOldVerAddr,3);
    if(gDeviceOldVer==0xFFFFFF)
        gDeviceOldVer=0;
    E2promRead((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
    if((gConnectDeviceNum>40)||(gMainDeviceStatus!=DEVSTATUS_NORMAL_WORKMODE))
        gConnectDeviceNum=0;
    WifiInit();
    sleep(3);
    E2promRead(&nTemp,DevicePointConfigAddr,1);
    printf("************************Device  Information************************\r\n");
    printf("Device Number = %d\r\n",gConnectDeviceNum);
    sleep(3);
    SysNetInit();
    memset(gDeviceInfoBuf,0,sizeof(gDeviceInfoBuf));

    if(gMainDeviceStatus==DEVSTATUS_NORMAL_WORKMODE)
    {
        ReadTagBaseFile();
        ReadTagInfoFile();
        DLinkParamGet();
        ReadPointTableFile();
        InitDeviceIecInfo();

        E2promRead((UINT8 *)&gYXPointNum,DeviceYXNumberAddr,2);
        if(gYXPointNum==0xFFFF)
            gYXPointNum=0;
        E2promRead((UINT8 *)&gYCPointNum,DeviceYCNumberAddr,2);
        if(gYCPointNum==0xFFFF)
            gYCPointNum=0;
        E2promRead((UINT8 *)&gYKPointNum,DeviceYKNumberAddr,2);
        if(gYKPointNum==0xFFFF)
            gYKPointNum=0;
        E2promRead((UINT8 *)&gSDPointNum,DeviceSDNumberAddr,2);
        if(gSDPointNum==0xFFFF)
            gSDPointNum=0;
        E2promRead((UINT8 *)&gDDPointNum,DeviceDDNumberAddr,2);
        if(gDDPointNum==0xFFFF)
            gDDPointNum=0;

        printf("YX Num=%d  YC Num=%d  YK Num=%d  SD Num=%d  DD Num=%d\r\n",gYXPointNum,gYCPointNum,gYKPointNum,gSDPointNum,gDDPointNum);
        printf("*******************************************************************\r\n");

    }
    nPointRecordFd=0;
    nLogRecordFd=0;
    if(access(TestLogRecordFile,F_OK)==0)
    {
        nTestLogRecordFd=(int)fopen(TestLogRecordFile,"w+");
        fclose((void *)nTestLogRecordFd);
    }
    if(access("/mnt/flash/OAM/log/comlog",F_OK)==0)
        system("rm /mnt/flash/OAM/log/comlog");
    ReadAlarmFile();

}

/*****************************************************************************
* Function     : NorthernHeartbeatThreadMasterChannel()
* Description  : 主通道心跳线程
* Input        : None
* Output       : None
* Return       :
* Note(s)      : 改为30min重连      2019.4.25
* Contributor  : 2018年10月15日        Andre
*****************************************************************************/
void *NorthernHeartbeatThreadMasterChannel()
{
    DbgPrintf("THREAD-----SocketHeart Master Channel Init!\r\n");

    while(1)
    {
        if(((gSocketMode==0)&&(g_nRemotesockfd>0))
            ||((gSocketMode==1)&&(gModuleChannel0InitFlag>0)))
        {
            if(gSocketHeartChannel0Count>18)
            {
                if((gSocketMode==0)&&(g_nRemotesockfd>0))
                {
                    shutdown(g_nRemotesockfd,2);
                    g_nRemotesockfd=0;
                    gLocationReport++;
                    DbgPrintf("[Northern heartbeat 0]Reinit ethernet socket channel 0\r\n");
                    gSocketHeartChannel0Count=0;
                }
                else if((gSocketMode==1)&&(gModuleChannel0InitFlag>0))
                {
                    pthread_mutex_lock(&modemsem);
                    sleep(1);
                    gModuleChannel0InitFlag=0;
                    gLocationReport++;
                    DbgPrintf("[Northern heartbeat 0]Reinit modem socket channel 0\r\n");
                    gSocketHeartChannel0Count=0;
                    pthread_mutex_unlock(&modemsem);
                }
                else
                    gSocketHeartChannel0Count=0;
            }
            else
                gSocketHeartChannel0Count++;
        }
        sleep(100);
    }
}

/*****************************************************************************
* Function     : NorthernHeartbeatThreadSlaveChannel()
* Description  : 模块次通道心跳线程
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月15日        Andre
*****************************************************************************/
void *NorthernHeartbeatThreadSlaveChannel()
{
    DbgPrintf("THREAD-----SocketHeart Slave Channel Init!\r\n");
    gSocketHeartChannel1Flag=1;

    while(1)
    {
        if(((gSocketModeSub==2)&&(gModeSub==2)&&(gModuleChannel1InitFlag>0))
            ||((gSocketModeSub==1)&&(gModeSub==2)&&(g_ethernet_connect_sub>0))
            ||((gSocketModeSub==1)&&(gModeSub==1)&&(g_ethernet_connect_status>0)))
        {
            //DbgPrintf("[Northern heartbeat 1]gSocketHeartChannel1Count=%d\r\n",gSocketHeartChannel1Count);
            if(gSocketHeartChannel1Count>120)
            {
                if((gSocketModeSub==2)&&(gModeSub==2)&&(gModuleChannel1InitFlag>0))
                {
                    pthread_mutex_lock(&modemsem);
                    sleep(1);
                    gModuleChannel1InitFlag=0;
                    //gPlatformDisConnect=1;
                    DbgPrintf("[Northern heartbeat 1]Reinit modem socket channel 1\r\n");
                    gSocketHeartChannel1Count=0;
                    pthread_mutex_unlock(&modemsem);
                }
                else if((gSocketModeSub==1)&&(gModeSub==2)&&(g_ethernet_connect_sub>0))
                {
                    shutdown(g_nRemotesockfd_sub,2);
                    close(g_nRemotesockfd_sub);
                    g_ethernet_connect_sub=0;
                    g_nRemotesockfd_sub=0;
                    DbgPrintf("[Northern heartbeat 1]Reinit ethernet socket channel 1\r\n");
                    gSocketHeartChannel1Count=0;
                }
                else if((gSocketModeSub==1)&&(gModeSub==1)&&(g_ethernet_connect_status>0))
                {
                    shutdown(g_ethernet_sockfd,2);
                    close(g_ethernet_sockfd);
                    g_ethernet_connect_status=0;
                    g_ethernet_sockfd=0;
                    DbgPrintf("[Northern heartbeat 1]Reinit ethernet server channel 1\r\n");
                    gSocketHeartChannel1Count=0;
                }
                else
                    gSocketHeartChannel1Count=0;
            }
            else
                gSocketHeartChannel1Count++;
        }
        if((gModemSlaveChannelSwitch==0)||(gModemSlaveConfigFlag==0)||(gMainDeviceStatus!=10))
        {
            usleep(500);
            DbgPrintf("[Northern heartbeat 1]Northern Heartbeat Slave Channel Quit!\r\n");
            gSocketHeartChannel1Flag=0;
            return 0;
        }
        sleep(1);
    }
}

/*****************************************************************************
* Function     : NorthernProcessingThreadMaster()
* Description  : 北向主通道处理线程
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : 处理线程动态分配,心跳线程不关闭
* Contributor  : 2018年11月29日        Andre
*****************************************************************************/
void *NorthernProcessingThreadMaster()
{
    pthread_t Ethernet_Thread_Master,Modem_Thread_Master,Heart_Thread_Master;

    E2promRead((UINT8 *)&gSocketMode,DeviceSocketModeAddr,1);    //read master channnel network mode, default is wired
    if(gSocketMode>1)
    {
        gSocketMode=0;
        E2promWrite((UINT8 *)&gSocketMode,DeviceSocketModeAddr,1);
    }

    E2promRead((UINT8 *)&gMasterReportSwitch, MasterReportSwitch, 1); //read master channnel report switch, default is normal
    if(gMasterReportSwitch>1)
    {
        gMasterReportSwitch=0;
        E2promWrite((UINT8 *)&gMasterReportSwitch, MasterReportSwitch, 1);
    }

    pthread_create(&Heart_Thread_Master, NULL, NorthernHeartbeatThreadMasterChannel, (void *)NULL);

    while(1)
    {
        if(gSocketMode == 0)
        {
            pthread_create(&Ethernet_Thread_Master, NULL, EthernetThreadMasterChannel, (void *)NULL);

            while(gSocketMode == 0)
            {
                sleep(5);
            }
        }
        else if(gSocketMode == 1)
        {
            pthread_create(&Modem_Thread_Master, NULL, ModuleThreadMasterChannel, (void *)NULL);

            while(gSocketMode == 1)
            {
                sleep(5);
            }
        }
        usleep(1000);
    }
}

/*****************************************************************************
* Function     : NorthernProcessingThreadSlave()
* Description  : 北向从通道处理线程
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : 处理线程动态分配，从通道关闭时关闭心跳线程
* Contributor  : 2018年11月29日        Andre
*****************************************************************************/
void *NorthernProcessingThreadSlave()
{
    pthread_t Ethernet_Thread_Slave,Modem_Thread_Slave,Heart_Thread_Slave,Ethernet_Server_Slave;

    E2promRead((UINT8 *)&gModemSlaveChannelSwitch, SlaveSwitch, 1); //read sub channnel switch, default is open
    if(gModemSlaveChannelSwitch>1)
    {
        gModemSlaveChannelSwitch=1;
        E2promWrite((UINT8 *)&gModemSlaveChannelSwitch, SlaveSwitch, 1);
    }

    E2promRead((UINT8 *)&gModeSub,SlaveMode,1);                  //read sub channnel mode, default is client
    if((gModeSub != 1) && (gModeSub != 2))
    {
        gModeSub=2;
        E2promWrite((UINT8 *)&gModeSub, SlaveMode, 1);
    }

    E2promRead((UINT8 *)&gSocketModeSub, SlaveNetMode, 1);       //read sub channnel network mode, default is wireless
    if((gSocketModeSub != 1) && (gSocketModeSub != 2))
    {
        gSocketModeSub=2;
        E2promWrite((UINT8 *)&gSocketModeSub, SlaveNetMode, 1);
    }

    E2promRead((UINT8 *)&gModemSlaveConfigFlag, SlaveNetInfoConfigAddr, 1); //read sub channel config flag, default is close
    if((gModemSlaveConfigFlag != 0) && (gModemSlaveConfigFlag != 1))
    {
        gModemSlaveConfigFlag=0;
        E2promWrite((UINT8 *)&gModemSlaveConfigFlag, SlaveNetInfoConfigAddr, 1);
    }

    while(1)
    {
        while(gModemSlaveConfigFlag != 1)
        {
//            DbgPrintf("[NorthernProcessingThreadSlave]Have not Config Slave Channel Network Info yet!\r\n");
            sleep(10);
        }

        while(gModemSlaveChannelSwitch != 1)
        {
//            DbgPrintf("[NorthernProcessingThreadSlave]Slave Channel Switch Off!\r\n");
            sleep(10);
        }

        while(gMainDeviceStatus != 10)
        {
//            DbgPrintf("[NorthernProcessingThreadSlave]Device Status is not Normal Workmode!\r\n");
            sleep(10);
        }

        if(gSocketHeartChannel1Flag == 0)
            pthread_create(&Heart_Thread_Slave, NULL, NorthernHeartbeatThreadSlaveChannel, (void *)NULL);

        if((gModeSub==2)&&(gSocketModeSub==2)&&(gModemSlaveConfigFlag==1)&&(gModemSlaveChannelSwitch==1))
        {
            pthread_create(&Modem_Thread_Slave, NULL, ModuleThreadSlaveChannel, (void *)NULL);

            while((gModeSub==2)&&(gSocketModeSub==2)&&(gModemSlaveChannelSwitch==1)&&(gMainDeviceStatus==10))
            {
                sleep(1);
            }
        }
        else if((gModeSub==2)&&(gSocketModeSub==1)&&(gModemSlaveConfigFlag==1)&&(gModemSlaveChannelSwitch==1))
        {
            pthread_create(&Ethernet_Thread_Slave, NULL, EthernetThreadSlaveChannel, (void *)NULL);

            while((gModeSub==2)&&(gSocketModeSub==1)&&(gModemSlaveChannelSwitch==1)&&(gMainDeviceStatus==10))
            {
                sleep(1);
            }
        }
        else if((gModeSub==1)&&(gSocketModeSub==1)&&(gModemSlaveConfigFlag==1)&&(gModemSlaveChannelSwitch==1))
        {
            pthread_create(&Ethernet_Server_Slave, NULL, EthernetServerThreadSlaveChannel, (void *)NULL);

            while((gModeSub==1)&&(gSocketModeSub==1)&&(gModemSlaveChannelSwitch==1)&&(gMainDeviceStatus==10))
            {
                sleep(1);
            }
        }
        sleep(1);
    }
}

/*****************************************************************************
* Description:      Clear point table in ram
* Parameters:
* Returns:
*****************************************************************************/
void TypePointClear(void)
{
    struct sTypeGroup *pGroupPoint,*pGroupPointTemp;
    struct sTypeParam *pParamPoint,*pParamPointTemp;

    pGroupPoint=gTypeHead;
    while(pGroupPoint!=NULL)
    {
        pParamPoint=pGroupPoint->pParamNext;
        pParamPointTemp=pGroupPoint->pParamNext;
        while(pParamPoint!=NULL)
        {
            pParamPointTemp=pParamPoint;
            pParamPoint=pParamPoint->pNext;
            pGroupPoint->pParamNext=pParamPoint;
            free(pParamPointTemp);
        }
        pGroupPointTemp=pGroupPoint;
        pGroupPoint=pGroupPoint->pNext;
        gTypeHead=pGroupPoint;
        free(pGroupPointTemp);
    }
    gTypeHead=NULL;
    gTypePointClearFlag=0;
}

/*****************************************************************************
* Description:      add Point table base information to list
* Parameters:        nTypeID:table number
                     nProtocolTypeID:device protocol type 0:huawei modbus 1:modbus
* Returns:
*****************************************************************************/
void TypeGroupAdd(UINT8 nAddMode,UINT16 nTypeID,UINT8 nProtocolTypeID)
{
    struct sTypeGroup *pPoint;

    pPoint=(struct sTypeGroup*)malloc(sizeof(struct sTypeGroup));
    pPoint->nTypeID=nTypeID;
    pPoint->nProtocalTypeID=nProtocolTypeID;
    pPoint->pNext=NULL;
    pPoint->pParamNext=NULL;
//  printf("555 Create Table of Device\r\n");
    if(nAddMode)
    {
        if(gTypeHead==NULL)
        {
            gTypeHead=pPoint;
            gTypeGroupPoint=pPoint;
        }
        else
        {
            gTypeGroupPoint->pNext=pPoint;
            gTypeGroupPoint=pPoint;
        }
        gTypeParamPoint=NULL;
    }
    else
    {
        if(gTypeHeadBuf==NULL)
        {
            gTypeHeadBuf=pPoint;
            gTypeGroupPointBuf=pPoint;
        }
        else
        {
            gTypeGroupPointBuf->pNext=pPoint;
            gTypeGroupPointBuf=pPoint;
        }
        gTypeParamPointBuf=NULL;
    }
}

/*****************************************************************************
* Description:      add Point table point information to list
* Parameters:        nAddr:modbus address
                     nLen:reg length
                     nType:point type
                     nDataType:point data type
* Returns:
*****************************************************************************/
void TypeParamAdd(UINT8 nAddMode,UINT16 nAddr,UINT8 nLen,UINT8 nType,UINT8 nDataType)
{
    struct sTypeParam *pPoint;

    pPoint=(struct sTypeParam*)malloc(sizeof(struct sTypeParam));
    pPoint->nMBAddr=nAddr;
    pPoint->nLen=nLen;
    pPoint->nType=nType;
    pPoint->nDataType = nDataType;
    pPoint->pNext=NULL;
//  printf("555 Create Devicce of Point\r\n");
    if(nAddMode)
    {
        if(gTypeParamPoint==NULL)
        {
            gTypeGroupPoint->pParamNext=pPoint;
            gTypeParamPoint=pPoint;
        }
        else
        {
            gTypeParamPoint->pNext=pPoint;
            gTypeParamPoint=pPoint;
        }
    }
    else
    {
        if(gTypeParamPointBuf==NULL)
        {
            gTypeGroupPointBuf->pParamNext=pPoint;
            gTypeParamPointBuf=pPoint;
        }
        else
        {
            gTypeParamPointBuf->pNext=pPoint;
            gTypeParamPointBuf=pPoint;
        }
    }
}

/*****************************************************************************
* Function     : SinglePointAdd()
* Description  : 添加三方单点信息到链表
* Input        : type：third party single point type
                 _104_addr: third party single point 104 address
                 number_of_sub_point: number of third party sub point
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年11月7日        Andre
*****************************************************************************/
void SinglePointAdd(UINT8 type,UINT32 _104_addr,UINT8 number_of_sub_point)
{
    struct sThirdPartySinglePoint *pPoint;

    pPoint=(struct sThirdPartySinglePoint*)malloc(sizeof(struct sThirdPartySinglePoint));
    pPoint->nType=type;
    pPoint->n104Addr=_104_addr;
    pPoint->nNumberOfSubPoint=number_of_sub_point;
    pPoint->pNext=NULL;
    pPoint->pSubPoint=(struct sThirdPartySubPoint*)malloc(sizeof(struct sThirdPartySubPoint)*number_of_sub_point);

    if(gThirdPartySinglePointHead==NULL)
    {
        gThirdPartySinglePointHead=pPoint;
        gThirdPartySinglePointPoint=pPoint;
    }
    else
    {
        gThirdPartySinglePointPoint->pNext=pPoint;
        gThirdPartySinglePointPoint=pPoint;
    }
}

/*****************************************************************************
* Function     : SubPointAdd()
* Description  : 添加三方子点信息到数组
* Input        : addr：third party sub point inverter address
                 modbus: third party sub point modbus address
                 num: third party sub point ID
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年11月7日        Andre
*****************************************************************************/
void SubPointAdd(UINT8 addr,UINT32 modbus,UINT8 num)
{
    gThirdPartySinglePointPoint->pSubPoint[num].nAddr=addr;
    gThirdPartySinglePointPoint->pSubPoint[num].nModbus=modbus;
}

/*****************************************************************************
* Description:      exchange 104 YK point to modbus device ID and modbus address
* Parameters:        nAddr:modbus address
                     pAddr:return modbus address
* Returns:           modbus device ID
*****************************************************************************/
UINT8 YKGetDeviceID(UINT16 nAddr,UINT8 *pAddr)
{
    UINT8 i,nMaxDeviceID=0;
    UINT16 nMaxAddrValue=0;
    struct sTypeParam *pTypeParaTemp=NULL;
    struct sTypeGroup *pTypeGroupTemp=NULL;

    for(i=0;i<MAXDEVICE;i++)
    {
        if(gDeviceInfo[i].nInUse==1)
        {
            if(gDeviceInfo[i].nYKAddr<=nAddr)
            {
                if(gDeviceInfo[i].nYKAddr>nMaxAddrValue)
                {
                    nMaxDeviceID=i;
                    nMaxAddrValue=gDeviceInfo[i].nYKAddr;
                }
            }
        }
    }
    gPointTablePossessFlag|=(1<<6);
    pTypeGroupTemp = gTypeHead;
    while(pTypeGroupTemp!=NULL)
    {
        if(pTypeGroupTemp->nTypeID==gDeviceInfo[nMaxDeviceID].nType)
        {
            UINT8 nPointCount;

            nPointCount=nAddr-gDeviceInfo[nMaxDeviceID].nYKAddr;
            pTypeParaTemp=pTypeGroupTemp->pParamNext;

            while(pTypeParaTemp!=NULL)
            {
                if(pTypeParaTemp->nType==4)
                {
                    if(nPointCount==0)
                    {
                        memcpy(pAddr,(UINT8 *)&pTypeParaTemp->nMBAddr,2);
                        return nMaxDeviceID;
                    }
                    else
                    {
                        nPointCount--;
                        pTypeParaTemp=pTypeParaTemp->pNext;
                    }
                }
                else
                {
                    pTypeParaTemp=pTypeParaTemp->pNext;
                }
            }
        }
        else
        {
            pTypeGroupTemp=pTypeGroupTemp->pNext;
        }
    }
    gPointTablePossessFlag&=~(1<<6);
    return 0;
}

/*****************************************************************************
* Description:      write socket
* Parameters:        aBuf:Data
                     nLen:data length
* Returns:
*****************************************************************************/
void Socket_Send(UINT8 *aBuf,UINT8 nLen)
{
    if((gSocketMode==0)&&(g_nRemotesockfd>0))
    {
        UINT8 i;

        printf("send msg:");
        for(i=0;i<nLen;i++)
            printf("%02X ",aBuf[i]);
        printf("\r\n");

        if(nLen>6)
            LogRecordFileWrite(2,aBuf,nLen);

        EthernetSend(aBuf, nLen,0,0);
    }
    else if((gSocketMode==1)&&(gModuleChannel0InitFlag > 0))
    {
        pthread_mutex_lock(&modemsem);
        ModemSend(0,aBuf,nLen);
        /*if(nLen>6)
            LogRecordFileWrite(6,aBuf,nLen);*/
        pthread_mutex_unlock(&modemsem);
        usleep(200000);
    }
}

/*****************************************************************************
* Description:write data to Com
* Parameters:nCom       the Number of interface COM
             *pValue    the Data to be transfered
             nLen       the Length of Data
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void ComWrite(UINT8 nCom,UINT8 *pValue,UINT8 nLen)
{
    UINT8 nPreValueTemp,nFPGAValue,i,nRecvLen;
    UINT8 aMBSendBuf[256],aMBRecvBuf[256];
    UINT16 nCRC;

    memset(aMBSendBuf,0,sizeof(aMBSendBuf));
    memset(aMBRecvBuf,0,sizeof(aMBRecvBuf));
    memcpy(aMBSendBuf,pValue,nLen);
//    pthread_mutex_lock(&Uartsem);
    if(nUartFd == 0)
    {
        nUartFd = UartOper(2,9600);
    }
    nPreValueTemp = FpgaRead(3);
    do
    {
        if(nCom!=1)
            nFPGAValue = 0;
        else
            nFPGAValue = 1;
        FpgaWrite(3,nFPGAValue);
    }while((FpgaRead(0x03))!=nFPGAValue);
    nCRC = CRC16(aMBSendBuf,nLen);
    memcpy((UINT8 *)&aMBSendBuf[nLen],(UINT8 *)&nCRC,2);
    DbgPrintf("COM Send:");
    for(i=0;i<nLen+2;i++)
    {
        DbgPrintf("%02X ",aMBSendBuf[i]);
    }
    DbgPrintf("\r\n");
    GPIOSet(2,19,1);
    usleep(10);
    writeDev(nUartFd,aMBSendBuf,nLen+2);
    GPIOSet(2,19,0);
    nRecvLen=readDev(nUartFd,aMBRecvBuf);
    usleep(10);
    do
    {
        FpgaWrite(0x03,nPreValueTemp);
    }
    while((FpgaRead(0x03))!=nPreValueTemp);
    usleep(10);
    GPIOSet(2,19,1);
//    pthread_mutex_unlock(&Uartsem);
}

/*****************************************************************************
* Description:Send modbus command (set value)to South device
* Parameters:nDeviceID    DeviceID
             nAddr        addr of a register
             nValue       value to be set to the South device
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void ComNormalWrite(UINT8 nDeviceID,UINT16 nAddr,UINT16 nValue)
{
    UINT8 aMBSendBuf[8]={0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};

    aMBSendBuf[0]=nDeviceID;
    aMBSendBuf[2]=nAddr>>8;
    aMBSendBuf[3]=nAddr&0x00FF;
    aMBSendBuf[4]=nValue>>8;
    aMBSendBuf[5]=nValue&0x00FF;
    if((nDeviceID==249)||(nDeviceID==250))
        ComWrite(nDeviceID-248,aMBSendBuf,6);
    else
        ComWrite(gDeviceInfo[nDeviceID].nDownlinkPort,aMBSendBuf,6);
    return;
}

/*****************************************************************************
* Description:read data from  interface COM
* Parameters:nCom       the Number of interface COM
             *pDataBuf  none
             nLen       length of data  which would be read
             *pRecvBuf  to store the data which would be read
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
int ComRead(UINT8 nCom,UINT8 *pDataBuf,UINT8 nLen,UINT8 *pRecvBuf)
{
    UINT8 nPreValueTemp,nFPGAValue,i;
    int nRecvLen=0;
    UINT8 aMBSendBuf[256];
    UINT8 aMBRecvBuf[256];
    UINT16 nCRC;

    memset(aMBSendBuf,0,sizeof(aMBSendBuf));
    memcpy(aMBSendBuf,pDataBuf,nLen);
    if(nUartFd == 0)
    {
        nUartFd = UartOper(2,9600);
    }
    nPreValueTemp = FpgaRead(3);
    do
    {
        if(nCom!=1)
            nFPGAValue = 0;
        else
            nFPGAValue = 1;
        FpgaWrite(3,nFPGAValue);
    }while((FpgaRead(0x03))!=nFPGAValue);
    nCRC = CRC16(aMBSendBuf,nLen);
    memcpy((UINT8 *)&aMBSendBuf[nLen],(UINT8 *)&nCRC,2);
    DbgPrintf("COM Send:");
    for(i=0;i<nLen+2;i++)
    {
        DbgPrintf("%02X ",aMBSendBuf[i]);
}
    DbgPrintf("\r\n");
    GPIOSet(2,19,1);
    writeDev(nUartFd,aMBSendBuf,nLen+2);
    GPIOSet(2,19,0);
    nRecvLen=readDev(nUartFd,aMBRecvBuf);
    usleep(10);
    do
    {
        FpgaWrite(0x03,nPreValueTemp);
    }
    while((FpgaRead(0x03))!=nPreValueTemp);
    usleep(10);
    GPIOSet(2,19,1);
    memcpy(pRecvBuf,aMBRecvBuf,nRecvLen);
    /*if(nRecvLen>0)
    {
        memcpy(pDataBuf,(UINT8 *)&aMBRecvBuf[3],aMBRecvBuf[2]-2);
    }*/
    return nRecvLen;
}

/*****************************************************************************
* Description:Send a modbus command to read data from interface COM
* Parameters:nDeviceID  Device ID
             nAddr      addr of a register
             nLen       length of data  which would be read
             *pRecvBuf  to store the data which would be read
* Returns: length of read data
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
int ComNormalRead(UINT8 nDeviceID,UINT16 nAddr,UINT8 nLen,UINT8 *pRecvBuf)
{
    int nRecvLen=0;
    UINT8 aMBSendBuf[8]={0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00};
    UINT8 aMBRecvBuf[256];

    aMBSendBuf[0]=nDeviceID;
    aMBSendBuf[2]=nAddr>>8;
    aMBSendBuf[3]=nAddr&0x00FF;
    aMBSendBuf[4]=nLen>>8;
    aMBSendBuf[5]=nLen&0x00FF;
    nRecvLen=ComRead(gDeviceInfo[nDeviceID].nDownlinkPort,aMBSendBuf,6,aMBRecvBuf);
    memcpy(pRecvBuf,aMBRecvBuf,nRecvLen);

    return nRecvLen;
}

/*****************************************************************************
* Description:      send packet to APP
* Parameters:        aBuf:Data
                     nLen:data length
* Returns:
*****************************************************************************/
void AP_Send(UINT8 *aBuf,UINT8 nLen)
{
    int i;

    DbgPrintf("AP Send:\r\n");
    //printf("%S\r\n",aBuf);
    for(i=0;i<nLen;i++)
    {
        DbgPrintf("%02X ",aBuf[i]);
    }
    DbgPrintf("\r\n");
    pthread_mutex_lock(&APsem);
    LogRecordFileWrite(4,aBuf,nLen);
    write(nAPFd, aBuf, nLen);
    pthread_mutex_unlock(&APsem);
}

/*****************************************************************************
* Description:      send normal frame to platform
* Parameters:        aBuf:Data
                     nLen:data length
* Returns:
*****************************************************************************/
void FramePacketSend(UINT8 *aBuf,UINT8 nLen)
{
    UINT8 aSendBuf[SYS_FRAME_LEN];
    UINT16 nValue;

    aSendBuf[0]=PACKETHEAD;
    aSendBuf[1]=nLen+4;
    nValue=gSendCount*2;
    memcpy(&aSendBuf[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&aSendBuf[4],(UINT8 *)&nValue,2);
    memcpy(&aSendBuf[6],aBuf,nLen);
    Socket_Send(aSendBuf,nLen+6);
    gSendCount++;
}

/*****************************************************************************
* Description:      send S frame to platform
* Parameters:
* Returns:
*****************************************************************************/
void SendSFramePacket(void)
{
    UINT8 aSendBuf[6]={0x68,0x04,0x01,0x00,0x00,0x00};
    UINT16 nCount;

    nCount=(gRecvCount<<1);
    memcpy(&aSendBuf[4],(UINT8 *)&nCount,2);
    Socket_Send(aSendBuf,6);
}

/*****************************************************************************
* Description:      send update resend frame to platform
* Parameters:
* Returns:
*****************************************************************************/
void SendUpdataResendPacket(void)
{
    UINT8 aBuf[18];
    UINT16 nPacketValue;
    UINT32 nPacketNum=0;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=0x10;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    aBuf[6]=0xB4;
    aBuf[7]=0x00;
    aBuf[8]=0x83;
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);
    nPacketNum = gUpdataCount+1;
    memcpy((UINT8 *)&aBuf[15],(UINT8 *)&nPacketNum,3);
    ModemSend(0,aBuf,18);
}

/*****************************************************************************
* Description:      send S Frame to App
* Parameters:
* Returns:
*****************************************************************************/
void SendSFramePacketToAPP(void)
{
    UINT8 aSendBuf[6]={0x68,0x04,0x01,0x00,0x00,0x00};
    UINT16 nCount;

    nCount=(gRecvCount<<1);
    memcpy(&aSendBuf[4],(UINT8 *)&nCount,2);
    AP_Send(aSendBuf,6);
}

/*****************************************************************************
* Description:      send single value frame to app
* Parameters:        nCmdID:frame command ID
                     nFlag:send reason
                     nValue:send value
* Returns:
*****************************************************************************/
void SendSingleFrameToAPP(UINT16 nCmdID,UINT16 nFlag,UINT8 nValue)
{
    UINT8 aBuf[16];
    UINT16 nPacketValue;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=0x0E;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    memcpy((UINT8 *)&aBuf[6],(UINT8 *)&nCmdID,1);
    aBuf[7]=0x01;
    memcpy((UINT8 *)&aBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);
    aBuf[15]=nValue;
    AP_Send(aBuf,16);
}

/*****************************************************************************
* Description:      send single value frame to platform
* Parameters:        nCmdID:frame command ID
                     nFlag:send reason
                     nValue:send value
* Returns:
*****************************************************************************/
void SendSingleFrameToPlatform(UINT16 nCmdID,UINT16 nFlag,UINT8 nValue)
{
    UINT8 aBuf[16];
    UINT16 nPacketValue;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=0x0E;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    memcpy((UINT8 *)&aBuf[6],(UINT8 *)&nCmdID,1);
    aBuf[7]=0x01;
    memcpy((UINT8 *)&aBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);
    aBuf[15]=nValue;
    Socket_Send(aBuf,16);
}

/*****************************************************************************
* Description:      send normal frame to platform
* Parameters:        nCmdID:frame command ID
                     nFlag:send reason
                     nValue:send value buffer
                     nLen:send value buffer length
* Returns:
*****************************************************************************/
void SendFrameToPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen)
{
    UINT8 aBuf[256];
    UINT16 nPacketValue;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=nLen+13;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    memcpy((UINT8 *)&aBuf[6],(UINT8 *)&nCmdID,1);
    aBuf[7]=0x01;
    memcpy((UINT8 *)&aBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);
    if((nDeviceID>0)&&(nDeviceID<MAXDEVICE))
        aBuf[12]=nDeviceID;
    memcpy((UINT8 *)&aBuf[15],aValue,nLen);
    Socket_Send(aBuf,nLen+15);
}

/*****************************************************************************
* Function     : SendFrameToThirdPartyPlatform()
* Description  : send normal frame to third party platform
* Input        : nDeviceID: Device ID
                 nCmdID: frame command ID
                 nFlag: send reason
                 nValue: send value buffer
                 nLen: send value buffer length
                 mode: 0 ethernet  1 modem
* Output       : None
* Return       : None
* Note(s)      :
* Contributor  : 2018年11月13日        Andre
*****************************************************************************/
void SendFrameToThirdPartyPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen,UINT8 mode)
{
    UINT8 aBuf[256];
    UINT16 nPacketValue;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=nLen+13;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    memcpy((UINT8 *)&aBuf[6],(UINT8 *)&nCmdID,1);
    aBuf[7]=0x01;
    memcpy((UINT8 *)&aBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);
    if((nDeviceID>0)&&(nDeviceID<MAXDEVICE))
        aBuf[12]=nDeviceID;
    memcpy((UINT8 *)&aBuf[15],aValue,nLen);

    if(mode==1)
    {
        pthread_mutex_lock(&modemsem);
        usleep(200000);
        ModemSend(1,aBuf,nLen+15);
        pthread_mutex_unlock(&modemsem);
    }
    else if(mode==0)
    {
        EthernetSend(aBuf, nLen+15, 0, 1);
    }
}

/*****************************************************************************
* Description:      set device information into platform
* Parameters:
* Returns:
*****************************************************************************/
void SendDeviceInfoToServer(void)
{
    UINT8  aSendBuf[100];
    UINT16 nValue,nCmd=0x00C0,nFlag=S_R_Set;
    UINT32 nVersion;

    InitialLoggerInf();
    memset(aSendBuf,0,sizeof(aSendBuf));
    aSendBuf[0]=0x68;
    aSendBuf[1]=0x65;
    nValue=gSendCount*2;
    memcpy(&aSendBuf[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&aSendBuf[4],(UINT8 *)&nValue,2);
    memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCmd,1);
    aSendBuf[7]=0x01;
    memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&gMainDeviceID,2);
    memcpy((UINT8 *)&aSendBuf[15],gMainDeviceName,20);
    memcpy((UINT8 *)&aSendBuf[15+20],gMainDeviceSN,20);
    memcpy((UINT8 *)&aSendBuf[15+40],gMainDeviceModel,20);
    memcpy((UINT8 *)&aSendBuf[15+60],gMainDeviceType,20);
    memcpy((UINT8 *)&aSendBuf[15+80],gMainDeviceIP,4);
    aSendBuf[15+84]=gMainDeviceStatus/10;
    nVersion=SYS_OAM_VERSION;
    memcpy((UINT8 *)&aSendBuf[15+85],(UINT8 *)&nVersion,3);
    Socket_Send(aSendBuf,103);
    TestLogTimeFileWrite();
    if(gSocketMode==0)
        TestLogTypeFileWrite(2);
    else
        TestLogTypeFileWrite(6);
    TestLogStringFileWrite((void *)"I Frame ",strlen("I Frame "));
    TestLogStringFileWrite((void *)"Report Device Information to Platform\n",strlen("Report Device Information to Platform\n"));
}

/*****************************************************************************
* Description:      report device information to platform
* Parameters:
* Returns:
*****************************************************************************/
void ReportDeviceInfoToServer(void)
{
    UINT8 aSendBuf[100];
    UINT16 nValue,nCmd=0x00C0,nFlag=S_R_InfoReport;
    UINT32 nVersion;

    InitialLoggerInf();
    memset(aSendBuf,0,sizeof(aSendBuf));
    aSendBuf[0]=0x68;
    aSendBuf[1]=0x65;
    nValue=gSendCount*2;
    memcpy(&aSendBuf[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&aSendBuf[4],(UINT8 *)&nValue,2);
    memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCmd,1);
    aSendBuf[7]=0x01;
    memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&gMainDeviceID,2);
    memcpy((UINT8 *)&aSendBuf[15],gMainDeviceName,20);
    memcpy((UINT8 *)&aSendBuf[15+20],gMainDeviceSN,20);
    memcpy((UINT8 *)&aSendBuf[15+40],gMainDeviceModel,20);
    memcpy((UINT8 *)&aSendBuf[15+60],gMainDeviceType,20);
    memcpy((UINT8 *)&aSendBuf[15+80],gMainDeviceIP,4);
    aSendBuf[15+84]=gMainDeviceStatus == 10 ? 0x01 : 0x00;
    nVersion=SYS_OAM_VERSION;
    memcpy((UINT8 *)&aSendBuf[15+85],(UINT8 *)&nVersion,3);
    Socket_Send(aSendBuf,103);
}

/*****************************************************************************
* Function     : ReportDeviceInfoToThirdPartyServer()
* Description  : report device information to third party platform
* Input        : mode:  0 ethernet  1 modem
* Output       : None
* Return       : None
* Note(s)      :
* Contributor  : 2018年11月13日        Andre
*****************************************************************************/
void ReportDeviceInfoToThirdPartyServer(UINT8 mode)
{
    UINT8 aSendBuf[120];
    UINT16 nValue,nCmd=0x00C0,nFlag=S_R_InfoReport;
    UINT32 nVersion;

    InitialLoggerInf();
    memset(aSendBuf,0,sizeof(aSendBuf));
    aSendBuf[0]=0x68;
    aSendBuf[1]=0x65;
    nValue=gSendCount*2;
    memcpy(&aSendBuf[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&aSendBuf[4],(UINT8 *)&nValue,2);
    memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCmd,1);
    aSendBuf[7]=0x01;
    memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nFlag,2);
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&gMainDeviceID,2);
    memcpy((UINT8 *)&aSendBuf[15],gMainDeviceName,20);
    memcpy((UINT8 *)&aSendBuf[15+20],gMainDeviceSN,20);
    memcpy((UINT8 *)&aSendBuf[15+40],gMainDeviceModel,20);
    memcpy((UINT8 *)&aSendBuf[15+60],gMainDeviceType,20);
    memcpy((UINT8 *)&aSendBuf[15+80],gMainDeviceIP,4);
    aSendBuf[15+84]=gMainDeviceStatus == 10 ? 0x01 : 0x00;
    nVersion=SYS_OAM_VERSION;
    memcpy((UINT8 *)&aSendBuf[15+85],(UINT8 *)&nVersion,3);

    if(mode==1)
    {
        pthread_mutex_lock(&modemsem);
        usleep(200000);
        ModemSend(1,aSendBuf,103);
        pthread_mutex_unlock(&modemsem);
    }
    else if(mode==0)
    {
        EthernetSend(aSendBuf, 103, 0, 1);
    }
}

/*****************************************************************************
* Description:      send U frame to platform
* Parameters:
* Returns:
*****************************************************************************/
void SendUFramePacket(UINT8 nMode,UINT8 nValue)
{
    UINT8 aSendBuf[6]={0x68,0x04,0x03,0x00,0x00,0x00};

    aSendBuf[2] |= nValue;
    if(nMode==0)
        Socket_Send(aSendBuf,6);
    else
        AP_Send(aSendBuf,6);
}

/*****************************************************************************
* Description:      Send all point value to platform
* Parameters:
* Returns:
*****************************************************************************/
void TotalCallPacket()
{
    UINT8 aBuf[SYS_FRAME_LEN];
    UINT32 nAddrTemp=0;
    UINT16 i;

    memset(aBuf,0,sizeof(aBuf));
    if(gTotalCallFlag==1)
    {
        if((gYXPointCount!=0xFFFF) && (gYXPointNum!=0))
        {
            UINT8 nPointCount=0;

            aBuf[0]=0x01;
            aBuf[2]=0x14;
            aBuf[3]=0x00;
            memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
            for(i=gYXPointCount;i<gYXPointNum;i++)
            {
                if(((i-gYXPointCount+1)*4+12)>=255)
                {
                    break;
                }
                //if(aPointBuf[0x0001+i].nStatus==1)
                //    continue;
                nAddrTemp=0x0001+i;
                memcpy((UINT8 *)&aBuf[6+(i-gYXPointCount)*4],(UINT8 *)&nAddrTemp,2);
                memcpy((UINT8 *)&aBuf[6+(i-gYXPointCount)*4+3],(UINT8 *)&aPointBuf[0x0001+i].nValue,1);
                //printf("YXXXXXX  addr =   %d   value = %d\r\n",nAddrTemp,aPointBuf[0x0001+i].nValue);
                aBuf[1]=0x00+(i-gYXPointCount+1);
                nPointCount++;
            }
            aBuf[1]=nPointCount;
            TestLogTimeFileWrite();
            if(gSocketMode==0)
                TestLogTypeFileWrite(2);
            else
                TestLogTypeFileWrite(6);
            TestLogStringFileWrite((void *)"I Frame ",strlen("I Frame "));
            TestLogStringFileWrite((void *)"Upload CallBack YX Information\n",strlen("Upload CallBack YX Information\n"));
            if(nPointCount!=0)
                FramePacketSend(aBuf,6+4*(i-gYXPointCount));
            gYXPointCount=i;
            if(gYXPointCount==gYXPointNum)
            {
                gYXPointCount=0xFFFF;
            }
        }
        else if(gYCPointCount!=0xFFFF && gYCPointNum!=0)
        {
            UINT8 nPointCount=0;

            aBuf[0]=0x0D;
            aBuf[2]=0x14;
            aBuf[3]=0x00;
            memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
            nAddrTemp=0x4001+gYCPointCount;
            memcpy(&aBuf[6],(UINT8 *)&nAddrTemp,3);
            aBuf[8]=0x00;
            for(i=gYCPointCount;i<gYCPointNum;i++)
            {
                if(((i-gYCPointCount+1)*5+15)>=255)
                {
                    break;
                }
                //if(aPointBuf[0x4001+i].nStatus==1)
                 //   break;
                memcpy((UINT8 *)&aBuf[9+(i-gYCPointCount)*5],(UINT8 *)&aPointBuf[0x4001+i].nValue,4);
                //printf("YCCCCCC  addr =   %d   value = %d\r\n",0x4001+i,aPointBuf[0x4001+i].nPreValue);
                aBuf[9+(i-gYCPointCount)*5+4]=0;
                aBuf[1]=0x80+(i-gYCPointCount+1);
                nPointCount++;
            }
            aBuf[1]=0x80+nPointCount;
            FramePacketSend(aBuf,9+5*(i-gYCPointCount));
            gYCPointCount=i;
            if(gYCPointCount==gYCPointNum)
            {
                gYCPointCount=0xFFFF;
                gTotalCallFlag=2;
            }
        }
        else if(gYCPointNum==0)
        {
            gYCPointCount=0xFFFF;
            gTotalCallFlag=2;
        }
    }
    else if(gTotalCallFlag==2)
    {
        aBuf[0]=0x64;
        aBuf[1]=0x01;
        aBuf[2]=0x0A;
        aBuf[3]=0x00;
        memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
        aBuf[6]=0x00;
        aBuf[7]=0x00;
        aBuf[8]=0x00;
        aBuf[9]=0x14;
        TestLogTimeFileWrite();
        if(gSocketMode==0)
            TestLogTypeFileWrite(2);
        else
            TestLogTypeFileWrite(6);
        TestLogStringFileWrite((void *)"I Frame ",strlen("I Frame "));
        TestLogStringFileWrite((void *)"Upload CallBack Finish\n",strlen("Upload CallBack Finish\n"));
        FramePacketSend(aBuf,10);
        gTotalCallFlag=0;
        gYCPointCount=0;
    }
    else if(gEleTotalCallFlag==1)
    {
        if(gDDPointCount!=0xFFFF && gDDPointNum!=0)
        {
            UINT8 nPointCount=0;

            aBuf[0]=0x0F;
            aBuf[2]=0x14;
            aBuf[3]=0x00;
            memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
            for(i=gDDPointCount;i<gDDPointNum;i++)
            {
                if(((i-gDDPointCount+1)*8+12)>=255)
                {
                    break;
                }
                if(aPointBuf[0x6401+i].nStatus==1)
                    continue;
                nAddrTemp=0x6401+i;
                memcpy((UINT8 *)&aBuf[6+(i-gDDPointCount)*8],(UINT8 *)&nAddrTemp,3);
                memcpy((UINT8 *)&aBuf[6+(i-gDDPointCount)*8+3],(UINT8 *)&aPointBuf[0x6401+i].nValue,4);
                aBuf[6+(i-gDDPointCount)*8+7]=0;
                aBuf[1]=0x00+(i-gDDPointCount+1);
                nPointCount++;
            }
            aBuf[1]=nPointCount;
            TestLogTimeFileWrite();
            if(gSocketMode==0)
                TestLogTypeFileWrite(2);
            else
                TestLogTypeFileWrite(6);
            TestLogStringFileWrite((void *)"I Frame ",strlen("I Frame "));
            TestLogStringFileWrite((void *)"Upload CallBack DD Information\n",strlen("Upload CallBack DD Information\n"));
            FramePacketSend(aBuf,6+8*(i-gDDPointCount));
            gDDPointCount=i;
            if(gDDPointCount==gDDPointNum)
            {
                gDDPointCount=0xFFFF;
                gEleTotalCallFlag=2;
            }
        }
        else if(gDDPointNum==0)
        {
            gDDPointCount=0xFFFF;
            gEleTotalCallFlag=2;
        }
    }
    else if(gEleTotalCallFlag==2)
    {
        aBuf[0]=0x65;
        aBuf[1]=0x01;
        aBuf[2]=0x0A;
        aBuf[3]=0x00;
        memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
        aBuf[6]=0x00;
        aBuf[7]=0x00;
        aBuf[8]=0x00;
        aBuf[9]=0x05;
        TestLogTimeFileWrite();
        if(gSocketMode==0)
            TestLogTypeFileWrite(2);
        else
            TestLogTypeFileWrite(6);
        TestLogStringFileWrite((void *)"I Frame ",strlen("I Frame "));
        TestLogStringFileWrite((void *)"Upload Electrical CallBack Finish\n",strlen("Upload Electrical CallBack Finish\n"));
        FramePacketSend(aBuf,10);
        gEleTotalCallFlag=0;
    }
}

/*****************************************************************************
* Description:      send all point value to platform with time
* Parameters:
* Returns:
*****************************************************************************/
void TotalTimeCallPacket()
{
    UINT8 aBuf[SYS_FRAME_LEN];
    UINT32 nAddrTemp=0;
    UINT16 i;

    memset(aBuf,0,sizeof(aBuf));
    if(gTimeTotalCallFlag==1)
    {
        if((gYXTimePointCount!=0xFFFF) && (gYXPointNum!=0))
        {
            aBuf[0]=0x1E;
            aBuf[2]=0x14;
            aBuf[3]=0x00;
            memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
            for(i=gYXTimePointCount;i<gYXPointNum;i++)
            {
                if(((i-gYXTimePointCount+1)*4+19)>=255)
                {
                    break;
                }
                nAddrTemp=0x0001+i;
                memcpy((UINT8 *)&aBuf[6+(i-gYXTimePointCount)*4],(UINT8 *)&nAddrTemp,2);
                memcpy((UINT8 *)&aBuf[6+(i-gYXTimePointCount)*4+3],(UINT8 *)&aPointTimeBuf[0x0001+i],1);
                //printf("YXXXXXX  addr =   %d   value = %d\r\n",nAddrTemp,aPointTimeBuf[0x0001+i]);
                aBuf[1]=0x00+(i-gYXTimePointCount+1);
            }
            memcpy((UINT8 *)&aBuf[6+4*(i-gYXTimePointCount)],gTimeCallBackTime,7);
            FramePacketSend(aBuf,6+4*(i-gYXTimePointCount)+7);
            gYXTimePointCount=i;
            if(gYXTimePointCount==gYXPointNum)
            {
                gYXTimePointCount=0xFFFF;
            }
        }
        else if(gYCTimePointCount!=0xFFFF && gYCPointNum!=0)
        {
            aBuf[0]=0x24;
            aBuf[2]=0x14;
            aBuf[3]=0x00;
            memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
            nAddrTemp=0x4001+gYCTimePointCount;
            memcpy(&aBuf[6],(UINT8 *)&nAddrTemp,3);
            aBuf[8]=0x00;
            for(i=gYCTimePointCount;i<gYCPointNum;i++)
            {
                if(((i-gYCTimePointCount+1)*5+22)>=255)
                {
                    break;
                }
                memcpy((UINT8 *)&aBuf[9+(i-gYCTimePointCount)*5],(UINT8 *)&aPointTimeBuf[0x4001+i],4);
                //printf("YCCCCCC  addr =   %d   value = %d\r\n",0x4001+i,aPointTimeBuf[0x4001+i]);
                aBuf[9+(i-gYCTimePointCount)*5+4]=0;
                aBuf[1]=0x80+(i-gYCTimePointCount+1);
            }
            memcpy((UINT8 *)&aBuf[9+5*(i-gYCTimePointCount)],gTimeCallBackTime,7);
            FramePacketSend(aBuf,9+5*(i-gYCTimePointCount)+7);
            gYCTimePointCount=i;
            if(gYCTimePointCount==gYCPointNum)
            {
                gYCTimePointCount=0xFFFF;
                gTimeTotalCallFlag=2;
            }
        }
        else if(gYCPointNum==0)
        {
            gYCTimePointCount=0xFFFF;
            gTimeTotalCallFlag=2;
        }
    }
    else if(gTimeTotalCallFlag==2)
    {
        aBuf[0]=0xBE;
        aBuf[1]=0x01;
        aBuf[2]=0x0A;
        aBuf[3]=0x00;
        memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
        aBuf[6]=0x00;
        aBuf[7]=0x00;
        aBuf[8]=0x00;
        aBuf[9]=0x14;
        memcpy((UINT8 *)&aBuf[10],gTimeCallBackTime,7);
        FramePacketSend(aBuf,17);
        gTimeTotalCallFlag=0;
        gYCTimePointCount=0;
    }
}

/*****************************************************************************
* Description:      获取下联设备信息
* Parameters:        nGetMode:   0:Set Platform  从下联设备信息缓存列表中获取
                                 1:Report    从下联设备信息列表中获取
* Returns:           0: 找到设备信息  1： 没有南向设备信息
*****************************************************************************/
UINT8 GetDownLinkDeviceInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen)
{
    UINT8 nCount=0,nDeviceCount=0;

    do
    {
        gDLinkDeviceInfoCount++;
        //printf("glinkdevicecout=======%d\r\n",gDLinkDeviceInfoCount);
        //printf("TTT GetDownLinkDeviceInfo: Count = %d, DeviceNum = %d\r\n", gDLinkDeviceInfoCount, gConnectDeviceNum);

        if(gDLinkDeviceInfoCount>=MAXDEVICE)
        {
            if(nCount==0)
            {
                gDLinkDeviceInfoCount=0;
                return 1;
            }
            else
            {
                *pLen = nCount;
                return 0;
            }
        }
        if(nGetMode)
        {
            if(gDeviceInfo[gDLinkDeviceInfoCount].nInUse==1)
            {
                nDeviceCount++;
                pBuf[nCount] = gDLinkDeviceInfoCount;
                memcpy((UINT8 *)&pBuf[nCount+1],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].aESN,20);
                memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nPointTableNo,2);
                pBuf[nCount+23]=gDeviceInfo[gDLinkDeviceInfoCount].nDownlinkPort;
                pBuf[nCount+24]=gDeviceInfo[gDLinkDeviceInfoCount].nProtocolType;
                //memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&nTemp,2);
                nCount += 25;
            }
            else
            {
                DbgPrintf("Warnning: Device No. %d is not in Use TTT", gDLinkDeviceInfoCount);
            }
        }
        else
        {
            if(gDeviceInfoBuf[gDLinkDeviceInfoCount].nInUse==1)
            {
                DbgPrintf("device id %d\r\n",gDLinkDeviceInfoCount);
                nDeviceCount++;
                pBuf[nCount] = gDLinkDeviceInfoCount;
                memcpy((UINT8 *)&pBuf[nCount+1],(UINT8 *)&gDeviceInfoBuf[gDLinkDeviceInfoCount].aESN,20);
                memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&gDeviceInfoBuf[gDLinkDeviceInfoCount].nPointTableNo,2);
                pBuf[nCount+23]=gDeviceInfoBuf[gDLinkDeviceInfoCount].nDownlinkPort;
                pBuf[nCount+24]=gDeviceInfoBuf[gDLinkDeviceInfoCount].nProtocolType;
                //memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&nTemp,2);
                nCount += 25;
                gDeviceInfoBuf[gDLinkDeviceInfoCount].nInUse=0;
            }
            else
            {
                DbgPrintf("Warnning: Device buffer No. %d is not in Use TTT", gDLinkDeviceInfoCount);
            }
        }
    }while(((nDeviceCount < 9)&&(nDeviceCount>0)) || (nDeviceCount==0));
    *pLen=nCount;

    return 0;
}

UINT8 GetDownLinkDevicePointInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen)
{
    UINT8 nCount=0,nDeviceCount=0;

    do
    {
        gDLinkDeviceInfoCount++;

        if(gDLinkDeviceInfoCount>=MAXDEVICE)
        {
            if(nCount==0)
            {
                gDLinkDeviceInfoCount=0;
                return 1;
            }
            else
            {
                *pLen = nCount;
                return 0;
            }
        }
        if(nGetMode)
        {
            if(gDeviceInfo[gDLinkDeviceInfoCount].nInUse==1)
            {
                nDeviceCount++;
                pBuf[nCount] = gDLinkDeviceInfoCount;
                memcpy((UINT8 *)&pBuf[nCount+3],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nType,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nType));
                memcpy((UINT8 *)&pBuf[nCount+5],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nYXAddr,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nYXAddr));
                memcpy((UINT8 *)&pBuf[nCount+7],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nYCAddr,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nYCAddr));
                memcpy((UINT8 *)&pBuf[nCount+9],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nYKAddr,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nYKAddr));
                memcpy((UINT8 *)&pBuf[nCount+11],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nSDAddr,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nSDAddr));
                memcpy((UINT8 *)&pBuf[nCount+13],(UINT8 *)&gDeviceInfo[gDLinkDeviceInfoCount].nDDAddr,sizeof(gDeviceInfo[gDLinkDeviceInfoCount].nDDAddr));

                nCount += 15;
            }
            else
            {
                DbgPrintf("Warnning: Device No. %d is not in Use TTT", gDLinkDeviceInfoCount);
            }
        }
        else
        {
            if(gDeviceInfoBuf[gDLinkDeviceInfoCount].nInUse==1)
            {
                DbgPrintf("device id %d\r\n",gDLinkDeviceInfoCount);
                nDeviceCount++;
                pBuf[nCount] = gDLinkDeviceInfoCount;
                memcpy((UINT8 *)&pBuf[nCount+1],(UINT8 *)&gDeviceInfoBuf[gDLinkDeviceInfoCount].aESN,20);
                memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&gDeviceInfoBuf[gDLinkDeviceInfoCount].nPointTableNo,2);
                pBuf[nCount+23]=gDeviceInfoBuf[gDLinkDeviceInfoCount].nDownlinkPort;
                pBuf[nCount+24]=gDeviceInfoBuf[gDLinkDeviceInfoCount].nProtocolType;
                //memcpy((UINT8 *)&pBuf[nCount+21],(UINT8 *)&nTemp,2);
                nCount += 25;
                gDeviceInfoBuf[gDLinkDeviceInfoCount].nInUse=0;
            }
            else
            {
                DbgPrintf("Warnning: Device buffer No. %d is not in Use TTT", gDLinkDeviceInfoCount);
            }
        }
    }while(((nDeviceCount < 15)&&(nDeviceCount>0)) || (nDeviceCount==0));
    *pLen=nCount;

    return 0;
}

UINT8 GetDownLinkDevicePointParamInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen)
{
    UINT8 nCount=0;

    memcpy((UINT8 *)&pBuf[0], (UINT8 *)&gTypeGroupPointTran->nTypeID,sizeof(gTypeGroupPointTran->nTypeID));
    pBuf[3] = gTypeGroupPointTran->nProtocalTypeID;
    nCount += 4;
    if(gTypeParamPointTran!=NULL)
    {
        DbgPrintf("addr = %X  count = %d\r\n",gTypeParamPointTran->nMBAddr, nCount);
    }
    else
    {
        DbgPrintf("count = %d\r\n",nCount);
    }

    while((gTypeParamPointTran!=NULL)&&(nCount<210))
    {
        memcpy((UINT8 *)&pBuf[nCount], (UINT8 *)&gTypeParamPointTran->nMBAddr,sizeof(gTypeParamPointTran->nMBAddr));
        pBuf[nCount+2] = gTypeParamPointTran->nType;
        pBuf[nCount+3] = gTypeParamPointTran->nLen;
        pBuf[nCount+4] = gTypeParamPointTran->nDataType;
        nCount += 5;
        gTypeParamPointTran = gTypeParamPointTran->pNext;
    }
    *pLen = nCount;
    return 1;
}

UINT8 GetDownLinkDevicePointTable(UINT16 point_number)
{
    //gPointTablePossessFlag|=(1<<7);
    if(point_number == 0)
    {
        gTypeGroupPointTran = gTypeHead;
    }
    else
    {
        gTypeGroupPointTran = gTypeGroupPointTran->pNext;
    }
    while(gTypeGroupPointTran != NULL)
    {
        gTypeParamPointTran = gTypeGroupPointTran->pParamNext;
        if(gTypeParamPointTran != NULL)
            return gTypeGroupPointTran->nTypeID;
        else
            gTypeGroupPointTran = gTypeGroupPointTran->pNext;
    }
    //gPointTablePossessFlag&=~(1<<7);
    return 0;
}

/*****************************************************************************
* Description:      deal recv frame and response to platform
* Parameters:
* Returns:
*****************************************************************************/
UINT8 PackMainFunction(UINT8 nProtocol,UINT8 *aBuf,UINT8 nLen)
{
    sAPDU sMessage;
    UINT8 SendBuf[SYS_FRAME_LEN];
    UINT8 i=0,nFrameType,j;      //FrameType   0:I  1:S  2:U
    UINT8 nRes=0,aLogBuf[256];
    static UINT8 CRCErrcount = 0;

    memcpy(SendBuf,aBuf,nLen);
    //APCI
    for(j=0;j<nLen;j++)
    {
        NorthPrintf("%02X ",SendBuf[j]);
    }
    NorthPrintf("\r\n");
    sMessage.mAPCI.nHead=aBuf[i++];
    sMessage.mAPCI.nPktLen=aBuf[i++];
    if((aBuf[i]&0x01) == 0)
    {
        //I Frame
        nFrameType=0;
        sMessage.mAPCI.nSendCtrl=(aBuf[i++]>>1);
        sMessage.mAPCI.nSendCtrl+=aBuf[i++];
        sMessage.mAPCI.nRecvCtrl=(aBuf[i++]>>1);
        sMessage.mAPCI.nRecvCtrl+=aBuf[i++];
    }
    else
    {
        if((aBuf[i]&0x02)==0)
        {
            //S Frame
            nFrameType=1;
            sMessage.mAPCI.nRecvCtrl=(aBuf[i++]>>1);
            sMessage.mAPCI.nRecvCtrl+=aBuf[i++];
            DbgPrintf("S:Status=%d SendCount=%02d RecvCount=%02d\r\n",g_TransSta,gSendCount,gRecvCount);
            if(sMessage.mAPCI.nRecvCtrl!=gRecvCount)
            {
                /*close socket*/
            }
			SendSFramePacket();
            return 0;
        }
        else
        {
            //U Frame
            TestLogTimeFileWrite();
            TestLogTypeFileWrite(1);
            TestLogStringFileWrite((void *)"U Frame ",strlen("U Frame "));
            nFrameType=2;
            DbgPrintf("U:Status=%d SendCount=%02d RecvCount=%02d\r\n",g_TransSta,gSendCount,gRecvCount);
            if(((aBuf[i]>>2)&0x03)==0x01)//Start Command
            {
                if(g_TransSta==1)//Stop->Start
                {
                    g_TransSta=0;
                    DbgPrintf("U:Stop->Start\r\n");
                    TestLogStringFileWrite((void *)"Stop->Start",strlen("Stop->Start"));
                }
                SendUFramePacket(0,0x0B);
            }
            else if(((aBuf[i]>>4)&0x03)==0x01)//Stop Command
            {
                if(g_TransSta==0)//Start->Stop
                {
                    /*check unconfirm I Frame*/
                    g_TransSta=1;
                    DbgPrintf("U:Start->Stop\r\n");
                    TestLogStringFileWrite((void *)"Start->Stop",strlen("Start->Stop"));
                }
                SendUFramePacket(0,0x23);
            }
            else if(((aBuf[i]>>6)&0x03)==0x01)//Test Command
            {
                //g_TransSta=2;
                DbgPrintf("U:Test\r\n");
                TestLogStringFileWrite((void *)"Heart Test",strlen("Heart Test"));
                SendUFramePacket(0,0x83);
            }
            TestLogStringFileWrite((void *)"\n",strlen("\n"));
            return 0;
        }
    }
    if(gSocketMode == SOCKETMODE_LAN)
        LogRecordFileWrite(1,SendBuf,nLen);
    else
        LogRecordFileWrite(5,SendBuf,nLen);
    sMessage.mASDU.nCmdID=aBuf[i++];
    sMessage.mASDU.nMessNum=(aBuf[i++]&0x7F);
    sMessage.mASDU.nSendFlag=aBuf[i++];
    sMessage.mASDU.nSendFlag+=aBuf[i++]*256;
    sMessage.mASDU.nPublicAddr=aBuf[i++];
    sMessage.mASDU.nPublicAddr+=aBuf[i++]*256;
    memcpy((UINT8 *)&sMessage.mASDU.nPublicAddr,(UINT8 *)&aBuf[i],3);
    DbgPrintf("I:Status=%d SendCount=%02d RecvCount=%02d Command ID=0x%02X\r\n",g_TransSta,gSendCount,gRecvCount,sMessage.mASDU.nCmdID);
    switch(sMessage.mASDU.nCmdID)
    {
        case C_SC_NA://0x2D
        case C_DC_NA://0x2E
        {
            UINT16 nPreValueTemp=0,nFPGAValue=0;

            if((SendBuf[15]&0xF0)==0 && SendBuf[8]!=0x08)
            {
                UINT8 aMBSendBuf[8]={0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};
                UINT16 nAddr,nRecvLen,nCRC;
                UINT8 aMBAddr[2];

                memcpy((UINT8 *)&nAddr,(UINT8 *)&SendBuf[12],2);
                aMBSendBuf[0] = YKGetDeviceID(nAddr,aMBAddr);

                if(gDeviceInfo[aMBSendBuf[0]].nEndian==1)
                {
                    aMBSendBuf[3] = aMBAddr[1];
                    aMBSendBuf[2] = aMBAddr[0];
                }
                else
                {
                    aMBSendBuf[2] = aMBAddr[1];
                    aMBSendBuf[3] = aMBAddr[0];
                }
                DbgPrintf("YK Device ID= %d    addr=%02X%02X\r\n",aMBSendBuf[0],aMBSendBuf[2],aMBSendBuf[3]);
                aMBSendBuf[5] = SendBuf[15]&0x0F;

                pthread_mutex_lock(&Uartsem);
                if(nUartFd == 0)
                {
                    nUartFd = UartOper(2,9600);
                }
                nPreValueTemp=FpgaRead(3);
                do
                {
                    if(gDeviceInfo[aMBSendBuf[0]].nDownlinkPort!=1)
                        nFPGAValue = 0;
                    else
                        nFPGAValue = 1;
                    FpgaWrite(3,nFPGAValue);
                }while((FpgaRead(0x03))!=nFPGAValue);
                nCRC = CRC16(aMBSendBuf,6);
                if(gDeviceInfo[aMBSendBuf[0]].nEndian==1)
                    memcpy((UINT8 *)&aMBSendBuf[6],(UINT8 *)&nCRC,2);
                else
                {
                    aMBSendBuf[7]=(unsigned char)(nCRC>>8);
                    aMBSendBuf[6]=(unsigned char)nCRC;
                }
                DbgPrintf("COM Send:");
                for(i=0;i<8;i++)
                {
                    DbgPrintf("%02X ",aMBSendBuf[i]);
                }
                DbgPrintf("\r\n");
                GPIOSet(2,19,1);
                usleep(10);
                writeDev(nUartFd,aMBSendBuf,8);
                usleep(10);
                GPIOSet(2,19,0);
                nRecvLen=readDev(nUartFd,aMBSendBuf);
                usleep(10);
                do
                {
                    FpgaWrite(0x03,nPreValueTemp);
                }
                while((FpgaRead(0x03))!=nPreValueTemp);
                usleep(10);
                GPIOSet(2,19,1);
                pthread_mutex_unlock(&Uartsem);
            }
            SendBuf[8]=SendBuf[8]+1;
            Socket_Send(SendBuf,nLen);
            break;
        }
        case C_IC_NA://0x64
        {
            UINT16 nValue;

            if(gUpdataModeFlag)
                break;

            TestLogStringFileWrite((void *)"CallBack Confirm\n",strlen("CallBack Confirm\n"));
            SendSFramePacket();
            //HsSecDelay(1);
            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[8]=S_R_Confirm;
            if(gMasterReportSwitch!=1)
            {
                Socket_Send(SendBuf,nLen);
                gTotalCallFlag=1;
                gYXPointCount=0;
                gYCPointCount=0;
                gSendCount++;
            }
            break;
        }
        case C_CI_NA://0x65
        {
            UINT16 nValue;

            if(SendBuf[15]==0x05)
            {
                if(gUpdataModeFlag)
                    break;
                //g_TransSta=5;
                gEleTotalCallFlag=1;
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_Confirm;
                Socket_Send(SendBuf,nLen);
                gDDPointCount=0;
                gSendCount++;
                TestLogStringFileWrite((void *)"Electrical CallBack Start\n",strlen("Electrical CallBack Start\n"));
            }
            else if(SendBuf[15]==0x45)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_Confirm;
                Socket_Send(SendBuf,nLen);
                gSendCount++;
                TestLogStringFileWrite((void *)"Electrical CallBack Confirm\n",strlen("Electrical CallBack Confirm\n"));
            }
            break;
        }
        case C_CS_NA:						//0x67	对时
        {
            UINT16 nValue;
            UINT8 nMin,nHour,nDate,nMout,nYear,i;
            UINT16 nSec;
            char *aCmd="date -s %02d%02d%02d%02d20%02d.%02d";
            char aSetcmd[50];
            UINT8 aTimeBuf[6];
            UINT8 MB_send_buf[11];
            EPOCHTIME posix_time;
            struct sTypeGroup *gTypeGroupPoint=NULL;
            struct sTypeParam *gTypeParamPoint=NULL;

            gIsReportInfo = _YES;
            memset(aSetcmd,0,sizeof(aSetcmd));
            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[8]=S_R_Confirm;
            Socket_Send(SendBuf,nLen);
            memcpy((UINT8 *)&nSec,(UINT8 *)&SendBuf[15],2);
            nSec = nSec /1000;
            nMin = SendBuf[17];
            nHour= SendBuf[18];
            nDate = SendBuf[19] & 0x1F;
            nMout = SendBuf[20];
            nYear = SendBuf[21];
            aTimeBuf[0]=0x20;
            aTimeBuf[1]=nYear/10*16+nYear%10;
            aTimeBuf[2]=nMout/10*16+nMout%10;
            aTimeBuf[3]=nDate/10*16+nDate%10;
            aTimeBuf[4]=nHour/10*16+nHour%10;
            aTimeBuf[5]=nMin/10*16+nMin%10;
            aTimeBuf[6]=nSec/10*16+nSec%10;
            sprintf(aSetcmd,aCmd,nMout,nDate,nHour,nMin,nYear,nSec);
            system(aSetcmd);
            RTCSet(aTimeBuf);
            TestLogStringFileWrite((void *)"System Time Set Confirm\n",strlen("System Time Set Confirm\n"));
            gIVLicenseStatus=0xFF;
            posix_time = time((time_t *)NULL);
            DbgPrintf("posix_time = %ld\n",posix_time);
            for(i = 0; i < MAXDEVICE; i++)
            {
                if(gDeviceInfo[i].nInUse == _YES)
                {
                    gPointTablePossessFlag|=(1<<8);
                    gTypeGroupPoint = gTypeHead;
                    while(gTypeGroupPoint != NULL)
                    {
                        if(gTypeGroupPoint->nTypeID != gDeviceInfo[i].nType)
                            gTypeGroupPoint = gTypeGroupPoint->pNext;
                        else
                            break;
                    }
                    if(gTypeGroupPoint == NULL)
                        continue;

                    if(gTypeGroupPoint->nProtocalTypeID != Type_Huawei_Modbus)
                    {
                        continue;
                    }
                    gTypeParamPoint = gTypeGroupPoint->pParamNext;
                    while(gTypeParamPoint != NULL)
                    {
                        if(gTypeParamPoint->nMBAddr != 40000)
                            gTypeParamPoint = gTypeParamPoint->pNext;
                        else
                            break;
                    }
//                    if((gTypeParamPoint != NULL) && (gTypeParamPoint->nDataType == Type_Data_EPOCHTIME))
                    if(gTypeParamPoint != NULL)
                    {
                        MB_send_buf[0] = i;
                        MB_send_buf[1] = 0x10;
                        MB_send_buf[2] = 0x9C;
                        MB_send_buf[3] = 0x42;//DT1000 device
                        MB_send_buf[4] = 0x00;
                        MB_send_buf[5] = 0x02;
                        MB_send_buf[6] = 0x04;
                        MB_send_buf[7] = (unsigned char) (posix_time >> 24);
                        MB_send_buf[8] = (unsigned char) (posix_time >> 16);
                        MB_send_buf[9] = (unsigned char) (posix_time >> 8);
                        MB_send_buf[10] = (unsigned char) posix_time;
                        pthread_mutex_lock(&Uartsem);
                        ComWrite(gDeviceInfo[i].nDownlinkPort,MB_send_buf,11);
                        pthread_mutex_unlock(&Uartsem);
                    }
                }
            }
            gPointTablePossessFlag&=~(1<<8);
            break;
        }
        case UPDATA_CMD://0xB4
        {
            switch(sMessage.mASDU.nSendFlag)
            {
                UINT16 nValue;

                case S_R_BeginToTrans :
                {
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_BeginToTransConfirm;
                    SendBuf[15]=0x81;
                    Socket_Send(SendBuf,nLen);
                    TestLogStringFileWrite((void *)"Updata Start\n",strlen("Updata Start\n"));
                    gUpdataCount=0xFFFF;
                    UpdataFileOpen();
                    gUpdataModeFlag=1;
                    break;
                }
                case S_R_DataTrans:
                {
                    UINT8 aUpdataTemp[255];
                    UINT32 nUpdataCountTemp=0;
                    UINT16 nCRC,nPacketCRC;

                    if(gUpdataModeFlag)
                    {
                        nCRC = CalculateCRC((char *)&SendBuf[15],nLen-17);
                        memcpy((UINT8 *)&nPacketCRC,(UINT8 *)&SendBuf[nLen-2],2);
                        if(nCRC!=nPacketCRC)
                        {
                            DbgPrintf("Packet CRC Error = 0x%04X\r\n",nCRC);
                            SendBuf[1] = 0x10;
                            nValue=gSendCount*2;
                            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                            nValue=gRecvCount*2;
                            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                            SendBuf[8] = 0x83;
                            SendBuf[12] = 0x01;
                            SendBuf[13] = 0x00;
                            SendBuf[14] = 0x00;
                            nUpdataCountTemp=gUpdataCount+1;
                            memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                            Socket_Send(SendBuf,18);
                        }
                        else
                        {
                            memset(aUpdataTemp,0,sizeof(aUpdataTemp));
                            SendBuf[1]=0x0E;
                            nValue=gSendCount*2;
                            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                            nValue=gRecvCount*2;
                            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                            memcpy(aUpdataTemp,(UINT8 *)&SendBuf[15],nLen-17);
                            SendBuf[15]=0x01;
                            memcpy((UINT8 *)&nUpdataCountTemp,(UINT8 *)&SendBuf[12],3);
                            if(gUpdataCount==0xFFFF)
                            {
                                memcpy((UINT8 *)&gUpdataCount,(UINT8 *)&SendBuf[12],3);
                                FileWrite(nUpdataFd,aUpdataTemp,nLen-17);
                                Socket_Send(SendBuf,16);
                            }
                            else
                            {
                                if((gUpdataCount+1)==nUpdataCountTemp)
                                {
                                    FileWrite(nUpdataFd,aUpdataTemp,nLen-17);
                                    Socket_Send(SendBuf,16);
                                    gUpdataCount=nUpdataCountTemp;
                                }
                                else //if((gUpdataCount+1)<nUpdataCountTemp)
                                {
                                    SendBuf[1] = 0x10;
                                    SendBuf[8] = 0x83;
                                    SendBuf[12] = 0x01;
                                    SendBuf[13] = 0x00;
                                    SendBuf[14] = 0x00;
                                    nUpdataCountTemp=gUpdataCount+1;
                                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                                    Socket_Send(SendBuf,18);
                                    DbgPrintf("Updata-----packet number error pre=0x%02X,now=0x%02X\r\n",gUpdataCount,nUpdataCountTemp);
                                }
                                /*else if((gUpdataCount+1)>nUpdataCountTemp)
                                {
                                    SendBuf[1] = 0x10;
                                    SendBuf[8] = 0x83;
                                    SendBuf[12] = 0x01;
                                    SendBuf[13] = 0x00;
                                    SendBuf[14] = 0x00;
                                    nUpdataCountTemp=gUpdataCount+1;
                                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                                    Socket_Send(SendBuf,18);
                                    printf("Updata-----packet number error pre=0x%02X,now=0x%02X\r\n",gUpdataCount,nUpdataCountTemp);
                                }*/
                            }
                        }
                    }
                    break;
                }
                case S_R_TransComplete:
                {
                    UINT32 nRunVersion;
                    UINT8 nUpdateError=0;

                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_TransComplConfirm;
                    UpdataFileSave();
                    SendBuf[15] = S_R_TransComplConfirm;
                    Socket_Send(SendBuf,nLen);
                    nRunVersion = SYS_OAM_VERSION;
                    E2promWrite((UINT8 *)&nRunVersion,DeviceSwOldVerAddr,3);
                    sleep(1);
                    if((access("/mnt/flash/OAM/updatatemp.gz.tar",F_OK)!=-1))
                    {
                        system("rm /mnt/flash/OAM/OAM_PRE");

                        system("mv /mnt/flash/OAM/OAM /mnt/flash/OAM/OAM_PRE");

                       // system("cd /mnt/flash/OAM");

                        do{
                            system("tar -zxvf /mnt/flash/OAM/updatatemp.gz.tar -C /mnt/flash/OAM");
                            sleep(1);
                            nUpdateError++;
                            if(nUpdateError>3)
                                break;
                        }while(access("/mnt/flash/OAM/OAM",F_OK)==-1);
                        //system("rm /mnt/flash/OAM/updatatemp.gz.tar");
                        if(access("/mnt/flash/OAM/OAM",F_OK)==-1)
                            system("mv /mnt/flash/OAM/OAM_PRE /mnt/flash/OAM/OAM");

                        system("chmod 777 /mnt/flash/OAM/OAM");
                        system("ls /mnt/flash/OAM/");
                        system("reboot -f");
                    }
                    gUpdataModeFlag=0;
                    break;
                }
            }
            break;
        }
        case DEVICE_IP_CMD://0xB5
        {
            UINT16 nValue;
            UINT8 routeflag=0;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device IP:Value=%d.%d.%d.%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                E2promRead((UINT8 *)&SendBuf[15],DeviceIP_E2P,4);
                Socket_Send(SendBuf,nLen);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15],SendBuf[16],SendBuf[17],SendBuf[18]);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                DbgPrintf("TTT Get IP Set Command: B5 TTT\r\n");
                UINT8 aRegexBuf[256]="Set Device IP:Value=%d.%d.%d.%d\n";
                UINT8 aIPReadTemp[4];

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15],SendBuf[16],SendBuf[17],SendBuf[18]);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                E2promRead(aIPReadTemp,DeviceIP_E2P,4);
                DbgPrintf("TTT Send IP Set Command: B5 ACK TTT\r\n");
                Socket_Send(SendBuf,nLen);

                if((aIPReadTemp[0]!=SendBuf[15])
                    ||(aIPReadTemp[1]!=SendBuf[16])
                    ||(aIPReadTemp[2]!=SendBuf[17])
                    ||(aIPReadTemp[3]!=SendBuf[18]))
                {
                    memcpy(gMainDeviceIP,(UINT8 *)&SendBuf[15],4);
                    E2promWrite((UINT8 *)&SendBuf[15],DeviceIP_E2P,4);
                    gDeviceIPSetFlag=1;
                    E2promWrite((UINT8 *)&gDeviceIPSetFlag,DeviceIPSetAddr,1);
                    routeflag=1;
                    E2promWrite((UINT8 *)&routeflag,RouteConfigAddr,1);
                    SysNetInit();
                }
            }
            break;
        }
        case MODBUS_ENDIAN_CMD://0xB6
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                TestLogStringFileWrite((void *)"Get Downlink Device Endian",strlen("Get Downlink Device Endian"));
                for(i=0;i<((nLen-15)/2);i++)
                {
                    SendBuf[15+i*2+1]=gDeviceInfo[SendBuf[15+i*2]].nEndian;
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*2],SendBuf[15+i*2+1]);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                TestLogStringFileWrite((void *)"Set Downlink Device Endian",strlen("Get Downlink Device Endian"));
                for(i=0;i<((nLen-15)/2);i++)
                {
                    gDeviceInfo[SendBuf[15+i*2]].nEndian=SendBuf[15+i*2+1];
                    E2promWrite((UINT8 *)&gDeviceInfo[SendBuf[15+i*2]].nEndian,DeviceTypeInfoAddr+(SendBuf[15+i*2]-1)*5+4,1);
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*2],SendBuf[15+i*2+1]);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Downlink Device Endian Set Confirm\n",strlen("Downlink Device Endian Set Confirm\n"));
            }
            break;
        }
        case MODBUS_RATE_CMD://0xB7
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                TestLogStringFileWrite((void *)"Get Downlink Device Baudrate",strlen("Get Downlink Device Baudrate"));
                for(i=0;i<((nLen-15)/4);i++)
                {
                    memcpy((UINT8 *)&SendBuf[15+i*4+1],(UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,3);
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*4],gDeviceInfo[SendBuf[15+i*4]].nRate);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Downlink Device Rate Query Response\n",strlen("Downlink Device Rate Query Response\n"));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                TestLogStringFileWrite((void *)"Set Downlink Device Baudrate",strlen("Get Downlink Device Baudrate"));
                for(i=0;i<((nLen-15)/4);i++)
                {
                    memcpy((UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,(UINT8 *)&SendBuf[15+i*4+1],3);
                    E2promWrite((UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,DeviceTypeInfoAddr+(SendBuf[15+i*4]-1)*5,4);
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*4],gDeviceInfo[SendBuf[15+i*4]].nRate);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Downlink Device Rate Set Confirm\n",strlen("Downlink Device Rate Set Confirm\n"));
            }
            break;
        }
        case DEVICE_ADDR_CMD://0xB8
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device Address,Value=%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gMainDeviceID,2);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,gMainDeviceID);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]="Get Device Address,Value=%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy((UINT8 *)&gMainDeviceID,(UINT8 *)&SendBuf[15],2);
                E2promWrite((UINT8 *)&SendBuf[15],DeviceAddress,2);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,gMainDeviceID);
                /*E2promRead((UINT8 *)&gMainDeviceID,DeviceAddress,2);
                if((gMainDeviceID==0) || (gMainDeviceID==65535))
                    {
                         gMainDeviceID=1;
                    }*/
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_VERSION_CMD://0xB9
        {
            UINT16 nValue;
            UINT32 nVersion;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device Software Version,Local Value=%06d,Old Value=%06d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                nVersion=SYS_OAM_VERSION;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nVersion,3);
                memcpy((UINT8 *)&SendBuf[18],(UINT8 *)&gDeviceOldVer,3);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,nVersion,gDeviceOldVer);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_LOOPBACK_CMD:/** 0xBA 版本回滚 */
        {
            UINT32 nRunVersion;
            UINT16 nValue;

            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[8]=S_R_SetConfirm;
            Socket_Send(SendBuf,nLen);
            if((access("/mnt/flash/OAM/OAM_PRE",F_OK)!=-1))
            {
                nRunVersion = SYS_OAM_VERSION;
                E2promWrite((UINT8 *)&nRunVersion,DeviceSwOldVerAddr,3);
                system("mv /mnt/flash/OAM/OAM /mnt/flash/OAM/OAM_PRE_TEMP");
                system("mv /mnt/flash/OAM/OAM_PRE /mnt/flash/OAM/OAM");
                system("mv /mnt/flash/OAM/OAM_PRE_TEMP /mnt/flash/OAM/OAM_PRE");
                system("chmod 777 /mnt/flash/OAM/OAM");
                system("reboot -f");
            }
            break;
        }
        case POINT_LOAD_CMD:/**0xBB 点表导入（自动导表） */
        {
            UINT16 nValue;
            //printf("Point Table\r\n");
            switch(sMessage.mASDU.nSendFlag)
            {
                case 0x88:/** 传输原因： 导入全局信息 */
                {
                    gConnectDeviceNumBuf=SendBuf[15]; /** 设置设备总数*/

                    /** 设置遥信、遥测、遥控、设点、电度各自的总数 */
                    memcpy((UINT8 *)&gYXPointNumBuf,(UINT8 *)&SendBuf[16],2);
                    memcpy((UINT8 *)&gYCPointNumBuf,(UINT8 *)&SendBuf[18],2);
                    memcpy((UINT8 *)&gYKPointNumBuf,(UINT8 *)&SendBuf[20],2);
                    memcpy((UINT8 *)&gSDPointNumBuf,(UINT8 *)&SendBuf[22],2);
                    memcpy((UINT8 *)&gDDPointNumBuf,(UINT8 *)&SendBuf[24],2);

                    gTypeCount = 0;
                    gDeviceTypeNumBuf = 0;
                    TestLogStringFileWrite((void *)"Get Point Table Global Information From Platform\n",strlen("Get Point Table Global Information From Platform\n"));
                    break;
                }
                case 0x89:/** 传输原因： 导入设备类型信息 */
                {
                    UINT8 nParamCount,nParaLen,nParaType,nDataType;
                    UINT16 nAddr,nTableNo;

                    memcpy((UINT8 *)&nTableNo,(UINT8 *)&SendBuf[12],2);
                    if(gTypeCount!=nTableNo)
                    {
                        memcpy((UINT8 *)&gTypeCount,(UINT8 *)&SendBuf[12],2);
                        //gTypeCount=SendBuf[12];
                        gDeviceTypeBuf[gDeviceTypeNumBuf]=gTypeCount;
                        DbgPrintf("point table %d  = %d\r\n",gDeviceTypeNumBuf,gTypeCount);
                        TypeGroupAdd(0,gTypeCount,SendBuf[15]);
                        gTypeParamPoint=NULL;
                        gDeviceTypeNumBuf++;
                    }
                   nParamCount=16;
                    while(nParamCount<nLen)
                    {
                        memcpy((UINT8 *)&nAddr,(UINT8 *)&SendBuf[nParamCount],2);
                        nParaType=SendBuf[nParamCount+2];
                        nParaLen=SendBuf[nParamCount+3];
                        nDataType = SendBuf[nParamCount+4];
                        TypeParamAdd(0,nAddr,nParaLen,nParaType,nDataType);
                        nParamCount += 5;
                    }
                    TestLogStringFileWrite((void *)"Get Point Table Information From Platform\n",strlen("Get Point Table Information From Platform\n"));
                    break;
                }
                case 0x8A:/** 传输原因： 导入设备信息 */
                {
                    UINT8 nDeviceCount;


                    nDeviceCount=12;
                    while(nDeviceCount<nLen)
                    {
                        UINT8 nDeviceID;

                        nDeviceID = SendBuf[nDeviceCount];
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nType,(UINT8 *)&SendBuf[nDeviceCount+3],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nPointTableNo,(UINT8 *)&SendBuf[nDeviceCount+3],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nYXAddr,(UINT8 *)&SendBuf[nDeviceCount+5],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nYCAddr,(UINT8 *)&SendBuf[nDeviceCount+7],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nYKAddr,(UINT8 *)&SendBuf[nDeviceCount+9],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nSDAddr,(UINT8 *)&SendBuf[nDeviceCount+11],2);
                        memcpy((UINT8 *)&gDeviceInfoBuf[nDeviceID].nDDAddr,(UINT8 *)&SendBuf[nDeviceCount+13],2);
                        gDeviceInfoBuf[nDeviceID].nInUse=1;
                        nDeviceCount +=15;
                    }
                    TestLogStringFileWrite((void *)"Get Device 104 Information From Platform\n",strlen("Get Device 104 Information From Platform\n"));
                    break;
                }
            }

            /** 跟平台回复确认 */
            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[7]=0x01;
            SendBuf[8]=sMessage.mASDU.nSendFlag+0x10;/** 传输原因 */
            Socket_Send(SendBuf,nLen);

            break;
        }
        case REPORT_PHONE_CMD://0xBC
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gPhone,11);
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Phone Number",strlen("Get Phone Number"));
                TestLogStringFileWrite(gPhone,11);
                TestLogStringFileWrite((void *)"\n",1);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 *aReboot=(void *)"15967127443";
                UINT8 *aCmd=(void *)"13801878566";
                UINT8 *aUpdataPhone=(void *)"13801112223";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gPhone,(UINT8 *)&SendBuf[15],11);
                Socket_Send(SendBuf,nLen);
                if(memcmp(gPhone,aReboot,11)==0)
                {
                    system("reboot -f");
                }
                else if(memcmp(gPhone,aCmd,11)==0)
                {
                    system("rm -rf /mnt/flash/OAM/record");
                    system("mkdir /mnt/flash/OAM/record");
                    if(nPointRecordFd!=0)
                    {
                        fclose((void *)nPointRecordFd);
                        nPointRecordFd=0;
                    }
                    break;
                }
                else if(memcmp(gPhone,aUpdataPhone,11)==0)
                {
                    gUpdataModeFlag=0;
                    break;
                }
                else if((gPhone[0]==0x31)&&(gPhone[1]==0x31)&&(gPhone[2]==0x30))//PLC
                {
                    UINT8 nComNum = gPhone[3];

                    nComNum -=0x30;
                    pthread_mutex_lock(&Uartsem);
                    if(gPhone[4]==0x31)//PLC reboot
                    {
                        ComNormalWrite(249+nComNum,43100,1);
                    }
                    else if(gPhone[4]==0x30)
                    {
                        if(gPhone[5]!=0x39)
                        {
                            ComNormalWrite(249+nComNum,43114,gPhone[5]-0x30);
                        }
                        if(gPhone[6]!=0x39)
                        {
                            ComNormalWrite(249+nComNum,43116,gPhone[6]-0x30);
                        }
                        if((gPhone[7]!=0x39)||(gPhone[8]!=0x39)||(gPhone[9]!=0x39))
                        {
                            ComNormalWrite(249+nComNum,47001,(gPhone[7]-0x30)*100+(gPhone[8]-0x30)*10+(gPhone[9]-0x30));
                        }
                        if(gPhone[10]!=0x39)
                        {
                            ComNormalWrite(249+nComNum,47002,gPhone[10]-0x30);
                        }
                    }
                    pthread_mutex_unlock(&Uartsem);
                }
                else if((gPhone[0]==0x31)&&(gPhone[1]==0x33)&&(gPhone[2]==0x31))//PLC
                {
                    UINT8 nComNum = gPhone[6];
                    UINT8 nDeviceID;

                    nComNum -=0x30;
                    nDeviceID = (gPhone[7]-0x30)*10+(gPhone[8]-0x30);
                    pthread_mutex_lock(&Uartsem);
                    ComNormalWrite(nDeviceID,40015+gPhone[9]-0x30,gPhone[10]-0x30);
                    pthread_mutex_unlock(&Uartsem);
                }
                else
                    E2promWrite((UINT8 *)&SendBuf[15],DevicePhoneAddr,11);
                //Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Set Phone Number",strlen("Set Phone Number"));
                TestLogStringFileWrite(gPhone,11);
                TestLogStringFileWrite((void *)"\n",1);
            }
            break;
        }
        case CALLBACK_TIME_CMD://0xBE
        {
            UINT16 nValue;

            if(gMasterReportSwitch!=1)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                if(sMessage.mASDU.nSendFlag==S_R_Active)
                {
                    UINT8 aCallBackTime[7];

                    SendBuf[8]=S_R_Confirm;
                    Socket_Send(SendBuf,nLen);
                    gYXTimePointCount=0;
                    gYCTimePointCount=0;
                    gSendCount++;
                    memcpy(aCallBackTime,(UINT8 *)&SendBuf[16],7);
                    if(GetPointRecord(aCallBackTime)==1)
                        gTimeTotalCallFlag=1;
                    else
                    {
                        UINT8 aBuf[20];

                        aBuf[0]=0xBE;
                        aBuf[1]=0x01;
                        aBuf[2]=0x0A;
                        aBuf[3]=0x00;
                        memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
                        aBuf[6]=0x00;
                        aBuf[7]=0x00;
                        aBuf[8]=0x00;
                        aBuf[9]=0x14;
                        memcpy((UINT8 *)&aBuf[10],(UINT8 *)&SendBuf[16],7);
                        FramePacketSend(aBuf,17);
                    }
                }
                else  if(sMessage.mASDU.nSendFlag==S_R_TimeCallBackEnd)
                {
                    if(SendBuf[15]==0x00)
                    {
                        //SendBuf[15] = GetPointRecordStatus();
                        SendBuf[15] = 1;
                        Socket_Send(SendBuf,nLen);
                    }
                    else if(SendBuf[15]==0x8F)
                    {

                    }
                }
            }
            break;
        }
        case ALARM_REPORT_CMD://0xBF
        {
            UINT8 nAlarmCount=0,i,nDeviceIDTemp,nAlarmIDTemp,nExternID,nAlarmStatus;
            UINT16 nModbusAddr;

            nAlarmCount = SendBuf[7]&0x7F;
            for(i=0;i<nAlarmCount;i++)
            {
                nDeviceIDTemp = SendBuf[15+i*6];
                nAlarmStatus = SendBuf[15+i*6+5];
                if(nDeviceIDTemp==249)
                {
                    if(nAlarmStatus==1)
                    {
                        PLC0ConnectiveStatus=4;
                    }
                    if(nAlarmStatus==0)
                    {
                        PLC0ConnectiveStatus=1;
                    }
                    DbgPrintf("[PLC]AlarmStatus is %d ,PLC Status is %d ",nAlarmStatus,PLC0ConnectiveStatus);
                    continue;
                }
                if(nDeviceIDTemp==250)
                {
                    if(nAlarmStatus==1)
                    {
                        PLC1ConnectiveStatus=4;
                    }
                    if(nAlarmStatus==0)
                    {
                        PLC1ConnectiveStatus=1;
                    }
                    DbgPrintf("[PLC]AlarmStatus is %d ,PLC Status is %d ",nAlarmStatus,PLC1ConnectiveStatus);
                    continue;
                }
                nAlarmIDTemp = SendBuf[15+i*6+1];
                memcpy((UINT8 *)&nModbusAddr,(UINT8 *)&SendBuf[15+i*6+2],2);
                nExternID = SendBuf[15+i*6+4];
                nAlarmStatus = SendBuf[15+i*6+5];
                AlarmConfirmMasterChannel(nDeviceIDTemp,nAlarmIDTemp,nModbusAddr,nExternID,nAlarmStatus);
            }
            break;
        }
        case DEVICE_BASE_INFO_CMD:/** 0xC0 设备基本信息 */
        {
            UINT16 nValue;
            UINT32 nVersion;
            //InitialLoggerInf();
            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                DbgPrintf("TTT C0: Read Device Basic Information with %02X\r\n", S_R_Query);
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceName,20);
                memcpy((UINT8 *)&SendBuf[15+20],gMainDeviceSN,20);
                memcpy((UINT8 *)&SendBuf[15+40],gMainDeviceModel,20);
                memcpy((UINT8 *)&SendBuf[15+60],gMainDeviceType,20);
                memcpy((UINT8 *)&SendBuf[15+80],gMainDeviceIP,4);
                SendBuf[15+84] = gMainDeviceStatus == 10 ? 0x01 : 0x00;
                nVersion=SYS_OAM_VERSION;
                memcpy((UINT8 *)&SendBuf[15+85],(UINT8 *)&nVersion,3);
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Device Information\n",strlen("Get Information Query\n"));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_SetConfirm)
            {
                if(gMainDeviceStatus!=DEVSTATUS_NEW_DEVICE)
                {
                //SendSingleFrameToAPP(0xC3,0x008B,0x04);
                    if(gDeviceStationBuild<4)
                    {
                        SendBuf[1]=73;
                        SendBuf[6]=STATION_INFO_CMD;
                        nValue=gSendCount*2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[7]=0x01;
                        SendBuf[8]=S_R_Set;
                        memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gStationID,32);
                        memcpy((UINT8 *)&SendBuf[15+32],(UINT8 *)&gStationName,20);
                        memcpy((UINT8 *)&SendBuf[15+52],(UINT8 *)&gStationLongitude,4);
                        memcpy((UINT8 *)&SendBuf[15+56],(UINT8 *)&gStationLatitude,4);
                        Socket_Send(SendBuf,75);
                        TestLogStringFileWrite((void *)"Upload Station Information to Platform\n",strlen("Upload Station Information to Platform\n"));

                        SendSingleFrameToAPP(0xC3,0x008B,0x04);
                        gDeviceStationBuild=4;
                    }
                }
            }
            break;
        }
        case STATION_INFO_CMD:/** 0xC1 站点基本信息 */
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gStationID,32);
                memcpy((UINT8 *)&SendBuf[15+32],(UINT8 *)&gStationName,20);
                memcpy((UINT8 *)&SendBuf[15+52],(UINT8 *)&gStationLongitude,4);
                memcpy((UINT8 *)&SendBuf[15+56],(UINT8 *)&gStationLatitude,4);
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Station Information\n",strlen("Get Station Information\n"));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_SetConfirm)
            {
                //SendSingleFrameToAPP(0xC3,0x008B,0x06);

                SendBuf[1]=14;
                SendBuf[6]=STATION_BUILD_CMD;
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_TableBuild;
                SendBuf[15]=1;
                Socket_Send(SendBuf,16);
                TestLogStringFileWrite((void *)"Build Station Start\n",strlen("Build Station Start\n"));
                //sleep(5);
                //SendSingleFrameToAPP(0x00C3,0x008B,0x08);
                SendSingleFrameToAPP(0xC3,0x008B,0x06);
                gDeviceStationBuild=6;
            }
            break;
        }
        case SERVER_INFO_CMD://0xC2
        {
            break;
        }
        case STATION_BUILD_CMD://0xC3
        {
             if(SendBuf[15]==0x08)
            {
                TestLogStringFileWrite((void *)"Build Station Finish\n",strlen("Build Station Finish\n"));
                SendSFramePacket();
                for(i=1;i<=40;i++)
                    gDeviceInfo[i].nInUse=0;
                gConnectDeviceNum=0;
                sleep(5);
                SendSingleFrameToAPP(0xC3,0x008B,0x08);
            }
            break;
        }
        case DOWNLINK_DEVICE_INFO_CMD:/** 0xC4 下联设备信息 */
        {
            UINT16 nValue;
            UINT8 aTemp[255],nLenTemp=0,nRes=0;

            if((sMessage.mASDU.nSendFlag & 0xFF) == S_R_Query)
            {
                nRes = GetDownLinkDeviceInfo(1,aTemp,&nLenTemp);
                if(nRes == 0)/** 找到设备信息: 发送一帧设备信息报文 */
                {
                    SendBuf[1] = nLenTemp + 13;
                    nValue = gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue = gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[7] = 0x01;
                    SendBuf[8] = S_R_QueryConfirm;
                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&aTemp,nLenTemp);
                    Socket_Send(SendBuf,nLenTemp+15);
                    TestLogStringFileWrite((void *)"Report Downlink Device to Platform\n",strlen("Report Downlink Device to Platform\n"));
                }
                else/** 设备信息为空 */
                {
                    //printf("TTT DownLink Device Info is empty [X] TTT\r\n");
                    SendBuf[1] = 14;
                    nValue = gSendCount * 2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue = gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[7]  = 0x01;
                    SendBuf[8]  = S_R_InfoReportFinish;
                    SendBuf[15] = S_R_InfoReportFinish;
                    Socket_Send(SendBuf,16);
                    TestLogStringFileWrite((void *)"Report Downlink Device Finish\n",strlen("Report Downlink Device Finish\n"));
                }
            }
            else if((sMessage.mASDU.nSendFlag & 0xFF) == S_R_SetConfirm)
            {
                if(gMainDeviceStatus==DEVSTATUS_DEVINFO_RESPONSE)
                    nRes=GetDownLinkDeviceInfo(1,aTemp,&nLenTemp);
                else if(gMainDeviceStatus==DEVSTATUS_DEVINFO_SET)
                    nRes=GetDownLinkDeviceInfo(0,aTemp,&nLenTemp);
                if(nRes == 0)
                {
                    //gMainDeviceStatus=DEVSTATUS_DEVINFO_SET;
                    SendBuf[1]=nLenTemp+13;
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[7]=0x01;
                    SendBuf[8]=S_R_Set;
                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&aTemp,nLenTemp);
                    Socket_Send(SendBuf,nLenTemp+15);
                    TestLogStringFileWrite((void *)"Set Downlink Device\n",strlen("Set Downlink Device\n"));
                }
                else if(nRes == 1)
                {
                    if(gMainDeviceStatus==DEVSTATUS_DEVINFO_SET)
                    {
                        SendSingleFrameToAPP(0xC5,0x008C,0x02);
                        SendBuf[1]=0x0E;
                        SendBuf[6]=LOAD_POINT_TABLE_CMD;
                        nValue=gSendCount*2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[7]=0x01;
                        SendBuf[8]=S_R_TableLoad;
                        SendBuf[15]=1;
                        Socket_Send(SendBuf,16);
                        load_point_table_flag = 1;
                        TestLogStringFileWrite((void *)"Set Downlink Device Finish\n",strlen("Set Downlink Device Finish\n"));
                    }
                    else if(gMainDeviceStatus==DEVSTATUS_DEVINFO_RESPONSE)
                    {
                        gMainDeviceStatus=DEVSTATUS_DEVINFO_EXISTS;
                    }
                }
            }
            else if((sMessage.mASDU.nSendFlag & 0xFF) == S_R_Set) /** 平台发送数采下联设备信息 */
            {
                UINT8 nLoopCount;

                nValue = gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue = gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8] = S_R_SetConfirm;

                nLoopCount=15;

                while(nLoopCount<nLen)
                {
                    if(gConnectDeviceNumBuf<gConnectDeviceMaxNum)
                    {
                        gConnectDeviceNumBuf++;
                        //gDeviceInfoBuf[SendBuf[nLoopCount]].nInUse=1;
                        memcpy(gDeviceInfoBuf[SendBuf[nLoopCount]].aESN,(UINT8 *)&SendBuf[nLoopCount+1],20);
                        memcpy((UINT8 *)&gDeviceInfoBuf[SendBuf[nLoopCount]].nPointTableNo,(UINT8 *)&SendBuf[nLoopCount+21],2);
                        gDeviceInfoBuf[SendBuf[nLoopCount]].nDownlinkPort=SendBuf[nLoopCount+23];
                        gDeviceInfoBuf[SendBuf[nLoopCount]].nProtocolType=SendBuf[nLoopCount+24];
                        nLoopCount+=25;
                    }
                    else
                    {
                        SendSingleFrameToPlatform(0xC5,0x008C,0x07);//提示下联设备超出限制
                        break;
                    }
                }
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Downlink Device Set Confirm\n",strlen("Downlink Device Set Confirm\n"));
            }
            break;
        }
        case LOAD_POINT_TABLE_CMD:/** 0xC5 导表指令 */
        {

            if(SendBuf[15] == 1)/** 导表启动 */
            {
                load_point_table_flag = 1;
                gDLinkDeviceInfoCount = 0;
                gDLinkDeviceCount = 0;
                TestLogStringFileWrite((void *)"Report Downlink Device to Platform\n",strlen("Report Downlink Device to Platform\n"));
                E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
                gMainDeviceStatus=DEVSTATUS_DEVINFO_RESPONSE;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
                SendBuf[2] = (gSendCount * 2) & 0xFF;
                SendBuf[3] = ((gSendCount * 2) >> 8) & 0xFF;
                SendBuf[4] = (gRecvCount * 2) & 0xFF;
                SendBuf[5] = ((gRecvCount * 2) >> 8) & 0xFF;
                Socket_Send(SendBuf, 16);
                break;
            }
            else if(SendBuf[15] == 4)/** 导表完成 */
            {
                if(load_point_table_flag == 1)
                {
                    load_point_table_flag = 0;
                    if ((gMainDeviceStatus >= 2) && (gMainDeviceStatus <= 5))
                    {
                        AP_Send(SendBuf,16);
                    }
                    AlarmDeleteAll();
                    TestLogStringFileWrite((void *)"Download Point Table Finish\n",strlen("Download Point Table Finish\n"));
                    SendSFramePacket();
                    memcpy(gDeviceInfo,gDeviceInfoBuf,sizeof(gDeviceInfo));
                    memset(gDeviceInfoBuf,0,sizeof(gDeviceInfoBuf));
                    TagBaseFileWrite();
                    TagInfoFileWrite();
                    InitDeviceIecInfo();

                    gTypePointClearFlag=1;
                }
            }
            break;
        }
        case COMMUNICATION_MODE_CMD://0xC6
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_QueryConfirm;
                SendBuf[15]=gSocketMode;
                Socket_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Socket Mode",strlen("Get Socket Mode"));
                TestLogStringFileWrite((void *)"\n",1);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 nOldSocketMode=gSocketMode;

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_SetConfirm;
                E2promWrite((UINT8 *)&SendBuf[15],DeviceSocketModeAddr,1);
                Socket_Send(SendBuf,nLen);
                sleep(2);
                gSocketMode = SendBuf[15];
                if(nOldSocketMode!=gSocketMode)
                {
                    if(gSocketMode==1)
                    {
                        shutdown(g_nRemotesockfd,2);
                        g_nRemotesockfd = 0;
                    }
                }
                TestLogStringFileWrite((void *)"Set Socket Mode",strlen("Set Socket Mode"));
                TestLogStringFileWrite((void *)"\n",1);
            }
            break;
        }
        case UPLOAD_HW_DEVICE_CMD:
        {
            if((sMessage.mASDU.nSendFlag==S_R_SetConfirm) && (gUploadHWDeviceFlag == 1))
			{
                gMainDeviceStatus=DEVSTATUS_DEVINFO_RESPONSE;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
            }
            break;
        }
        case SERVER_IP_CMD://            0xC8
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                E2promRead((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                E2promRead((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                E2promRead((UINT8 *)&SendBuf[15+30+2],MasterReportSwitch,1);
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                E2promWrite((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                E2promWrite((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                gMasterReportSwitch=SendBuf[15+30+2];
                E2promWrite((UINT8 *)&SendBuf[15+30+2],MasterReportSwitch,1);
                /*Close Connect*/
                gMainDeviceConfig=1;
                E2promWrite((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
                Socket_Send(SendBuf,nLen);
                GetDomainNameIP((UINT8 *)&SendBuf[15]);
                if(gSocketMode==0)
                {
                    shutdown(g_nRemotesockfd,2);
                    close(g_nRemotesockfd);
                    g_nRemotesockfd=0;
                    SysNetInit();
                }
                else
                {
                    gModuleChannel0InitFlag=0;
                }

            }
            break;
        }

        //日志上传
        case LOG_UPLOAD:                                           //0xCA
        {
            UINT16 nValue;
            UINT8 day;                                             //日志的当月天数
            static UINT32 totalPackageNumber;                      //总包数量
            static UINT32 packageSequnce = 0;                      //82帧中到传输包序号
            static UINT8 logFileBufferFrame[SYS_FRAME_LEN];        //APDU:暂存帧该帧，以备重传
            switch(sMessage.mASDU.nSendFlag)
            {
                // 1.接收到来自平台到日志上传开始命令
                case S_R_BeginToTrans:
                {
                    //解析平台到log天数
                    day = SendBuf[15];
                    nValue=gSendCount*2;
                    SendBuf[1] = 0x10;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_BeginToTransConfirm;
                    //获取日志压缩包到总包数量
                    totalPackageNumber = startCompressLogOfDay(day);
                    SendBuf[15] = totalPackageNumber & 0xFF;
                    SendBuf[16] = (totalPackageNumber >> 8) & 0xFF;
                    SendBuf[17] = (totalPackageNumber >> 16) & 0xFF;

                    // 1.检测日志文件，是否准备完毕
                    if(totalPackageNumber > 0)
                    {
                        //日志文件准备OK
                        Socket_Send(SendBuf,0x12);
                        DbgPrintf("LOG UPLOAD START: totalPackageNumber = %d\n", totalPackageNumber);
                        /** reset frame number */
                        packageSequnce = 0;
                        //TestLogStringFileWrite("LOG UPLOAD START\n",strlen("LOG UPLOAD START\n"));
                    }
                    else
                    {
                        DbgPrintf("totalPackageNumber = 0\n");
                        //TestLogStringFileWrite("LOG FILES DOES NOT EXIT\n",strlen("LOG FILES DOES NOT EXIT\n"));
                    }
                    break;
                }

                // 2.接收到来自平台到准备接收上传数据命令或者平台成功接收命令
                case S_R_DataTrans:
                {
                    UINT8 logFileWholeFrame[SYS_FRAME_LEN] = {0};  //APDU完整包:固定15字节+自定义数据+CRC
                    UINT8 logFileData[SYS_LOG_FILES_LEN] = {0};    //自定义数据部分,200字节一帧
                    UINT8 logFileFixedLenData[FIXED_LENGTH] ={0};  //APDU前15字节固定字节
                    UINT16 CRCValue;                               //CRC校验值
                    UINT8 transFlag = SendBuf[15];                 //transFlag:0-传输启动; 1-传输成功; 2-CRC失败

                    //传输启动或者传输成功,并且不为最后一帧
                    if((transFlag == TRANSFER_START || transFlag == TRANSFER_OK) && (packageSequnce < totalPackageNumber))
                    {
                        DbgPrintf("Send log package frame[%d]:\n", packageSequnce);
                        //组帧:fixed frame
                        memcpy(logFileFixedLenData,SendBuf,15);
                        logFileFixedLenData[1] = 0xD7;
                        nValue=gSendCount*2;
                        memcpy(&logFileFixedLenData[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&logFileFixedLenData[4],(UINT8 *)&nValue,2);
                        logFileFixedLenData[12] = packageSequnce & 0xFF;
                        logFileFixedLenData[13] = (packageSequnce>>8) & 0xFF;
                        logFileFixedLenData[14] = (packageSequnce>>16) & 0xFF;

                        //发送日志文件到数据帧，直到最后一帧发送完毕后，发送完成命令给平台
                        if(GetLogFrameData((void *)logFileData, packageSequnce++) != 0)
                        {
                            /*  0x82帧封包   */
                            memcpy(logFileWholeFrame, logFileFixedLenData, FIXED_LENGTH);
                            memcpy(&logFileWholeFrame[FIXED_LENGTH], logFileData, SYS_LOG_FILES_LEN);
                            //CRC校验:只校验自定义数据部分:200 bytes
                            CRCValue = CalculateCRC((void *)logFileData,SYS_LOG_FILES_LEN);
                            logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN] = CRCValue & 0xFF;
                            logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN + 1] = (CRCValue>>8) & 0xFF;


                            //暂存帧,提供重传包数据
                            memcpy(logFileBufferFrame, logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);

                            //发送数据包
                            Socket_Send(logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                            //TestLogStringFileWrite(logFileWholeFrame,FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                            DbgPrintf("%s\n", logFileWholeFrame);

                            //将发送包清空
                            bzero(logFileData, SYS_LOG_FILES_LEN);
                            bzero(logFileWholeFrame, SYS_FRAME_LEN);
                        }
                    }

                    //CRC校验失败,重传
                    else if(transFlag == CRC_FAILED)
                    {
                        DbgPrintf("CRC failed, resend log package frame[%d]:\n", packageSequnce);
                        Socket_Send(logFileBufferFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                        DbgPrintf("%s\n", logFileBufferFrame);
                        //TestLogStringFileWrite("resend a package of log zip package",strlen("resend a package of log zip package"));
                    }

                    //传输完最后一帧,发送结束命令
                    else if((transFlag == TRANSFER_START || transFlag == TRANSFER_OK) && (packageSequnce == totalPackageNumber))
                    {
                        nValue=gSendCount*2;
                        SendBuf[1] = 0x0e;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_TransComplete;
                        SendBuf[12] = totalPackageNumber & 0xFF;
                        SendBuf[13] = (totalPackageNumber>>8) & 0xFF;
                        SendBuf[14] = (totalPackageNumber >> 16) & 0XFF;
                        SendBuf[15] = S_R_TransComplete;

                        Socket_Send(SendBuf, 0x10);
                        //一次日志上传成功后，包序号清0
                        packageSequnce = 0;
                        DbgPrintf("log upload OK!");
                        system("rm -f /mnt/flash/OAM/log/xxExportFile.tar.gz");
                    }
                    break;
                }

                case S_R_DataRetran:
                {
                    UINT8 logFileWholeFrame[SYS_FRAME_LEN];          //APDU:固定15字节+自定义数据
                    UINT8 logFileData[SYS_LOG_FILES_LEN];            //自定义数据部分,200字节一帧
                    UINT8 logFileFixedLenData[FIXED_LENGTH];         //15字节固定长度
                    UINT16 CRCValue;

                    //重传序号
                    UINT32 resendPackageSequnce;
                    resendPackageSequnce = SendBuf[15] + (SendBuf[16]<<8) + (SendBuf[17]<<16);

                    //组帧:fixed frame
                    memcpy(logFileFixedLenData,SendBuf,15);
                    logFileFixedLenData[1] = 0x10;
                    nValue=gSendCount*2;
                    memcpy(&logFileFixedLenData[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&logFileFixedLenData[4],(UINT8 *)&nValue,2);

                    //build package sequence number
                    logFileFixedLenData[12] = SendBuf[15];
                    logFileFixedLenData[13] = SendBuf[16];
                    logFileFixedLenData[14] = SendBuf[17];

                    if((resendPackageSequnce > totalPackageNumber) || (resendPackageSequnce < 0))
                    {
                        DbgPrintf("resendPackageSequence is error!");
                        break;
                    }
                    // 根据包序号获取重传包
                    if(GetLogFrameData((void *)logFileData, resendPackageSequnce) != 0)
                    {
                        memcpy(logFileWholeFrame, logFileFixedLenData, FIXED_LENGTH);
                        memcpy(&logFileWholeFrame[FIXED_LENGTH], logFileData, SYS_LOG_FILES_LEN);

                        //CRC校验:只校验自定义数据部分:200 bytes
                        CRCValue = CalculateCRC((void *)logFileData,SYS_LOG_FILES_LEN);
                        logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN] = CRCValue & 0xFF;
                        logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN + 1] = (CRCValue>>8) & 0xFF;

                        //发送数据包
                        Socket_Send(logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                        DbgPrintf("Resend log frame[%d] OK\n", resendPackageSequnce);
                        DbgPrintf("%s\n", logFileWholeFrame);
                        //TestLogStringFileWrite(logFileWholeFrame,FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);

                    }

                    break;
                }

            }
            break;
        }

        /**
        ** 数采发送端带超时机制到日志导出
        */
        /*
        //日志上传
        case LOG_UPLOAD:                //0xCA
        {
            UINT16 nValue;
            UINT8 day;                                  //日志的当月天数
            static UINT32 totalPackageNumber = 0;       //总包数量
            static UINT32 packageSequnce = 0;
            //static UINT8 logFileBufferFrame[SYS_FRAME_LEN] = {0};        //APDU:暂存帧该帧，以备重传

            //加互斥锁
            pthread_mutex_lock(&logUploadSem);
            switch(sMessage.mASDU.nSendFlag)
            {
                //接收到来自平台到日志上传开始命令
                case S_R_BeginToTrans:
                {
                    //解析平台到log天数
                    day = SendBuf[15];
                    nValue=gSendCount*2;
                    SendBuf[1] = 0x10;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_BeginToTransConfirm;
                    //获取日志压缩包到总包数量
                    totalPackageNumber = startCompressLogOfDay(day);
                    SendBuf[15] = totalPackageNumber & 0xFF;
                    SendBuf[16] = (totalPackageNumber>>8) & 0xFF;
                    SendBuf[17] = (totalPackageNumber>>16) & 0xFF;

                    // 1.检测日志文件，是否准备完毕
                    if(totalPackageNumber > 0)
                    {
                        //日志文件准备OK
                        Socket_Send(SendBuf,0x12);
                        printf("LOG UPLOAD START: totalPackageNumber = ");
                        printf("%d\n", totalPackageNumber);
                        TestLogStringFileWrite("LOG UPLOAD START\n",strlen("LOG UPLOAD START\n"));
                    } else
                    {
                        printf("totalPackageNumber = 0\n");
                        TestLogStringFileWrite("LOG FILES DOES NOT EXIT\n",strlen("LOG FILES DOES NOT EXIT\n"));
                    }
                    break;
                }

                // 2.接收到来自平台到准备接收上传数据命令或者平台成功接收命令
                case S_R_DataTrans:
                {
                    UINT8 logFileWholeFrame[SYS_FRAME_LEN] = {0};        //APDU:固定15字节+自定义数据+CRC
                    UINT8 logFileData[SYS_LOG_FILES_LEN] = {0};          //自定义数据部分,200字节一帧
                    UINT8 logFileFixedLenData[FIXED_LENGTH] ={0};        //15字节固定长度
                    UINT16 CRCValue;
                    UINT8 transFlag = SendBuf[15];                       //transFlag:0-传输启动; 1-传输成功; 2-CRC失败
                    UINT32 recvPacketSequence = 0x00;                    //上一帧回来的帧序号

                    //传输启动或者传输成功,并且不为最后一帧
                    if((transFlag == 0 || transFlag == 1) && (packageSequnce < totalPackageNumber))
                    {
                            if(transFlag == 0)
                            {
                                //启动传输命令回来的响应帧序号
                                recvPacketSequence = 0x00;
                            }
                            else if(transFlag == 1)
                            {
                                //数据帧回来的响应帧序号
                                recvPacketSequence = SendBuf[12] + SendBuf[13]<<8 + SendBuf[14]<<16;
                            }
                            //判断在30s超时范围内是否收正确收到启动命令的响应
                            if((transFlag == 0) && (recvPacketSequence == packageSequnce) && (gTimeCount > 0) && (gTimeCount <= 30))
                            {
                                //启动传输命令发送成功，取消重传
                                printf("Start transferring cmd successfully...\n");
                                printf("recv packet sequence[%d]\n", recvPacketSequence);
                                gTimeCount = 0xFF;
                            }
                            //判断在30s超时范围内是否收正确收到数据包的响应
                            if((transFlag == 1) && (recvPacketSequence == (packageSequnce -1)) && (gTimeCount > 0) && (gTimeCount <= 30))
                            {
                                //数据包正常发送成功，取消重传
                                printf("Transfer data packet successfully...");
                                printf("recv packet sequence[%d]\n", recvPacketSequence);
                                gTimeCount = 0xFF;
                            }
                            printf("Send log package frame[%d]:\n", packageSequnce);
                            //组帧:fixed frame
                            memcpy(logFileFixedLenData,SendBuf,15);
                            logFileFixedLenData[1] = 0xD7;
                            nValue=gSendCount*2;
                            memcpy(&logFileFixedLenData[2],(UINT8 *)&nValue,2);
                            nValue=gRecvCount*2;
                            memcpy(&logFileFixedLenData[4],(UINT8 *)&nValue,2);
                            logFileFixedLenData[12] = packageSequnce & 0xFF;
                            logFileFixedLenData[13] = (packageSequnce>>8) & 0xFF;
                            logFileFixedLenData[14] = (packageSequnce>>16) & 0xFF;

                            //发送日志文件到数据帧，直到最后一帧发送完毕后，发送完成命令给平台
                            if(getFrameData(logFileData, packageSequnce++) != 0)
                            {
                                memcpy(logFileWholeFrame, logFileFixedLenData, FIXED_LENGTH);
                                memcpy(&logFileWholeFrame[FIXED_LENGTH], logFileData, SYS_LOG_FILES_LEN);
                                //CRC校验:只校验自定义数据部分:200 bytes
                                CRCValue = CalculateCRC(logFileData,SYS_LOG_FILES_LEN);
                                logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN] = CRCValue % 0xFF;
                                logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN + 1] = (CRCValue>>8) & 0xFF;

                                //发送数据包
                                Socket_Send(logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                                gTimeCount = 30;
                                gTimeoutResendLen = (FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                                //暂存帧
                                memcpy(gLogFileBufferFrame, logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                                printf("%s\n", logFileWholeFrame);
                                //将发送包清空
                                bzero(logFileData, SYS_LOG_FILES_LEN);
                                bzero(logFileWholeFrame, SYS_FRAME_LEN);
                            }
                    }

                    //CRC校验失败,重传
                    else if(transFlag == 2)
                    {
                        printf("CRC failed, resend log package frame[%d]:\n", packageSequnce);
                        Socket_Send(gLogFileBufferFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                        printf("%s\n", gLogFileBufferFrame);
                        TestLogStringFileWrite("resend a package of log zip package",strlen("resend a package of log zip package"));
                    }

                    //传输完最后一帧,发送结束命令
                    else if((transFlag == 0 || transFlag == 1) && (packageSequnce == totalPackageNumber))
                    {
                        nValue=gSendCount*2;
                        SendBuf[1] = 0x0e;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_TransComplete;
                        SendBuf[12] = totalPackageNumber & 0xFF;
                        SendBuf[13] = (totalPackageNumber>>8) & 0xFF;
                        SendBuf[14] = (totalPackageNumber >> 16) & 0XFF;
                        SendBuf[15] = S_R_TransComplete;

                        Socket_Send(SendBuf, 0x10);
                        //一次日志上传成功后，包序号清0
                        packageSequnce = 0;
                        printf("log upload OK!");
                    }

                    break;
                }

                case S_R_DataRetran:
                {
                    UINT8 logFileWholeFrame[SYS_FRAME_LEN];        //APDU:固定15字节+自定义数据
                    UINT8 logFileData[SYS_LOG_FILES_LEN];          //自定义数据部分,200字节一帧
                    UINT8 logFileFixedLenData[FIXED_LENGTH];       //15字节固定长度
                    UINT16 CRCValue;

                    //重传序号
                    UINT32 resendPackageSequnce;
                    resendPackageSequnce = SendBuf[15] + SendBuf[16]*256 + SendBuf[17]*65536;

                    //组帧:fixed frame
                    memcpy(logFileFixedLenData,SendBuf,15);
                    logFileFixedLenData[1] = 0x10;
                    nValue=gSendCount*2;
                    memcpy(&logFileFixedLenData[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&logFileFixedLenData[4],(UINT8 *)&nValue,2);
                    //package sequnce number
                    logFileFixedLenData[12] = SendBuf[15];
                    logFileFixedLenData[13] = SendBuf[16];
                    logFileFixedLenData[14] = SendBuf[17];

                    if((resendPackageSequnce > totalPackageNumber) || (resendPackageSequnce < 0))
                    {
                        printf("resendPackageSequnce is error!");
                        break;
                    }
                    // 根据包序号获取重传包
                    if(getFrameData(logFileData, resendPackageSequnce) != 0)
                    {
                        memcpy(logFileWholeFrame, logFileFixedLenData, FIXED_LENGTH);
                        memcpy(&logFileWholeFrame[FIXED_LENGTH], logFileData, SYS_LOG_FILES_LEN);

                        //CRC校验:只校验自定义数据部分:200 bytes
                        CRCValue = CalculateCRC(logFileData,SYS_LOG_FILES_LEN);
                        logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN] = CRCValue % 256;
                        logFileWholeFrame[FIXED_LENGTH + SYS_LOG_FILES_LEN + 1] = CRCValue / 256;

                        //发送数据包
                        Socket_Send(logFileWholeFrame, FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);
                        printf("resend log frame[%d] OK\n", resendPackageSequnce);
                        printf("%s\n", logFileWholeFrame);
                        //TestLogStringFileWrite(logFileWholeFrame,FIXED_LENGTH + SYS_LOG_FILES_LEN + CRC_LENGTH);

                    }

                    break;
                }

            }
            pthread_mutex_unlock(&logUploadSem);
            break;
        }

        */
        case NETWORK_CONFIG_CMD:   //   0xCE
        {
            UINT16 nValue;
            UINT32 channel_id=0;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy((UINT8 *)&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy((UINT8 *)&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&channel_id,(UINT8 *)&SendBuf[12],3);
                DbgPrintf("channel = %d\r\n",channel_id);
                if(channel_id == 0)
                {
                    E2promRead((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                    E2promRead((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                    SendBuf[47]=0x02;
                    SendBuf[48]=gSocketMode+1;
                }
                else if(channel_id == 1)
                {
                    E2promRead((UINT8 *)&SendBuf[15],SlaveDomainNameAddr,30);
                    E2promRead((UINT8 *)&SendBuf[15+30],SlaveDomainNamePortAddr,2);
                    E2promRead((UINT8 *)&SendBuf[15+30+2],SlaveMode,1);
                    E2promRead((UINT8 *)&SendBuf[15+30+2+1],SlaveNetMode,1);
                }
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy((UINT8 *)&channel_id,(UINT8 *)&SendBuf[12],3);
                if(channel_id==0)
                {
                    E2promWrite((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                    E2promWrite((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                    /*Close Connect*/
                    gMainDeviceConfig=1;
                    E2promWrite((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
                    Socket_Send(SendBuf,nLen);
                    GetDomainNameIP((UINT8 *)&SendBuf[15]);
                    if(gSocketMode==0)
                    {
                        shutdown(g_nRemotesockfd,2);
                        close(g_nRemotesockfd);
                        g_nRemotesockfd=0;
                        SysNetInit();
                    }
                    else
                    {
                        gModuleChannel0InitFlag=0;
                    }
                }
                else if(channel_id==1)
                {
                    E2promWrite((UINT8 *)&SendBuf[15],SlaveDomainNameAddr,30);
                    E2promWrite((UINT8 *)&SendBuf[15+30],SlaveDomainNamePortAddr,2);
                    E2promWrite((UINT8 *)&SendBuf[15+30+2],SlaveMode,1);
                    E2promWrite((UINT8 *)&SendBuf[15+30+2+1],SlaveNetMode,1);
                    gModeSub=SendBuf[15+30+2];
                    gSocketModeSub=SendBuf[15+30+2+1];

                    gModemSlaveConfigFlag=1;
                    E2promWrite((UINT8 *)&gModemSlaveConfigFlag,SlaveNetInfoConfigAddr,1);
                    Socket_Send(SendBuf,nLen);
                    if(gModeSub == 2)
                        GetDomainNameIP((UINT8 *)&SendBuf[15]);
                    if(gSocketModeSub == 2)
                        gModuleChannel1InitFlag=0;
                    else if(gSocketModeSub == 1)
                        g_ethernet_connect_sub = 0;
                }
            }
            break;
        }
        case NETWORK_SLAVE_SWITCH_CMD:
        {
            UINT16 nValue;
            UINT32 channel_id=0;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&channel_id,(UINT8 *)&SendBuf[12],3);
                if(channel_id == 0)
                    SendBuf[15] = 1;
                else
                    SendBuf[15] = gModemSlaveChannelSwitch & 0x01;
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy((UINT8 *)&channel_id,(UINT8 *)&SendBuf[12],3);
                if(channel_id != 0)
                {
                    gModemSlaveChannelSwitch = SendBuf[15];
                    E2promWrite((UINT8 *)&gModemSlaveChannelSwitch, SlaveSwitch, 1);
                }
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case ESN_CMD://                  0xD0
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceSN,20);
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceSN,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceSN,DeviceSNAddr,20);
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_TYPE_CMD://          0xD1
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag == S_R_Query)
            {
                UINT8 info[21] = {0};
                memcpy(info, gMainDeviceType, 20);
                DbgPrintf("TTT D1: Query Device Type -> %s TTT\r\n", info);

                nValue = gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceType,20);
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 info[21] = {0};
                memcpy(info, gMainDeviceType, 20);
                DbgPrintf("TTT D1: Set Device Type Before -> %s TTT\r\n", info);

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceType,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceType,DeviceTypeAddr,20);
                Socket_Send(SendBuf,nLen);

                memset(info, 0, 20);
                memcpy(info, gMainDeviceType, 20);
                DbgPrintf("TTT D1: Set Device Type After -> %s TTT\r\n", info);
            }
            break;
        }
        case DEVICE_MODEL_CMD://         0xD2
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 info[21] = {0};
                memcpy(info, gMainDeviceModel, 20);
                DbgPrintf("TTT D2: Query Device Model -> %s TTT\r\n", info);

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceModel,20);
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 info[21] = {0};
                memcpy(info, gMainDeviceModel, 20);
                DbgPrintf("TTT D2: Set Device Model Before -> %s TTT\r\n", info);

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceModel,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceModel,DeviceModelAddr,20);
                Socket_Send(SendBuf,nLen);

                memset(info, 0, 20);
                memcpy(info, gMainDeviceModel, 20);
                DbgPrintf("TTT D2: Set Device Model After -> %s TTT\r\n", info);
            }
            break;
        }
        case DEVICE_NAME_CMD://          0xD3
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 info[21] = {0};
                memcpy(info, gMainDeviceName, 20);
                DbgPrintf("TTT D3: Query Device Name -> %s TTT\r\n", info);

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gMainDeviceName,20);
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 info[21] = {0};

                memcpy(info, gMainDeviceName, 20);
                DbgPrintf("TTT D3: Set Device Name Before -> %s TTT\r\n", info);

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceName,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceName,DeviceNameAddr,20);
                Socket_Send(SendBuf,nLen);

                memset(info, 0, 20);
                memcpy(info, gMainDeviceName, 20);
                DbgPrintf("TTT D3: Set Device Name Before -> %s TTT\r\n", info);
            }
            break;
        }
        case DEVICE_MODIFY_CMD:/** 0xD4 南向设备修改 */
        {
            UINT8 dev_addr  = SendBuf[12];
            UINT8 newEsn[20] = {0};
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                memcpy(newEsn, SendBuf + 15, 20);

                //printf("XXX Receive South Device Change Response XXX\r\n");
                DbgPrintf("Device Address: %d\r\n", dev_addr);
                DbgPrintf("New ESN: %s\r\n", newEsn);

                if ((0 < dev_addr) && (dev_addr <= gConnectDeviceNum))
                {
                    UINT8 send_to_sgcc_buff[255];

                    memset(send_to_sgcc_buff, 0 ,sizeof(send_to_sgcc_buff));
                    memcpy(send_to_sgcc_buff, SendBuf, nLen);
                    AddMsgToSGCCSendBuff(send_to_sgcc_buff,nLen);
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_SetConfirm;
                    memset(gDeviceInfo[dev_addr].aESN, 0, sizeof(gDeviceInfo[dev_addr].aESN));
                    memcpy(gDeviceInfo[dev_addr].aESN,  newEsn, 20);
                    Socket_Send(SendBuf,nLen);
                    TagBaseFileWrite();
                }
                else
                {
                    DbgPrintf("Error: Illegal Device Number\r\n");
                }
            }
            break;
        }
        case DEVICE_DELETE_CMD://  0xD5 南向设备删除 */
        {
            DbgPrintf("XXX South Device Delete XXX\r\n");
            if (sMessage.mASDU.nSendFlag == 0x06)
            {
                if (SendBuf[15] == 0x81)
                {/**预置*/
                    UINT16 nValue = gSendCount * 2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue = gRecvCount * 2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8] = 0x07;

                    DbgPrintf("XXX Send Pre-Config: ");
                    Socket_Send(SendBuf, nLen);
                }
                else if (SendBuf[15] == 0x01)
                {/** 设置 */
                    UINT32 dev_id = SendBuf[12]; /**下联设备二级地址低8位*/
                    UINT8 send_to_sgcc_buff[255];

                    if(dev_id==249)
                    {
                        sPlcInfoBuf[0].nInUse=0;
                        //memset(PlcC7Buff0,0,84);
                        PLC0ConnectiveStatus=0;
                    }
                    else if(dev_id==250)
                    {
                        sPlcInfoBuf[1].nInUse=0;
                        //memset(PlcC7Buff1,0,84);
                        PLC1ConnectiveStatus=0;
                    }
                    else if(gDeviceInfo[dev_id].nInUse==_YES)
                    {
                        gConnectDeviceNum--;
                        E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
                        gDeviceInfo[dev_id].nInUse = _NO;
                    }

                    memset(send_to_sgcc_buff, 0 ,sizeof(send_to_sgcc_buff));
                    memcpy(send_to_sgcc_buff, SendBuf, nLen);
                    send_to_sgcc_buff[8]  = 0xA2;
                    AddMsgToSGCCSendBuff(send_to_sgcc_buff,nLen);

                    UINT16 nValue = gSendCount * 2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue = gRecvCount * 2;
                    memcpy(&SendBuf[4], (UINT8 *)&nValue, 2);
                    SendBuf[8]  = 0x07;
                    SendBuf[12] = dev_id & 0xFF;
                    SendBuf[13] = (dev_id >> 8) & 0xFF;
                    SendBuf[14] = (dev_id >> 16) & 0xFF;
                    DbgPrintf("XXX Send Config: ");
                    Socket_Send(SendBuf, nLen);
                    TagBaseFileWrite();
                }
            }
            break;
        }
        case DEVICE_MAXNUMBER_CMD:
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                SendBuf[15]=gConnectDeviceMaxNum;
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case IV_SCANF_INFO_CMD:
        {
            UINT16 nValue;
            UINT8 i,aRecvBuf[256];

            if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                pthread_mutex_lock(&Uartsem);
                if(ComNormalRead(SendBuf[15],32126,2,aRecvBuf)!=-1)
                {
                    SendBuf[8]=S_R_SetConfirm;
                    for(i=0;i<((nLen-15)/2);i++)
                    {
                        gIVScanfPoint[SendBuf[15+i*2]-1] = SendBuf[15+i*2+1];
                        ComNormalWrite(SendBuf[15+i*2],42779,SendBuf[15+i*2+1]);
                    }
                }
                else
                {
                    SendBuf[8]=S_R_Error;
                    SendBuf[15]=S_R_Error;
                    SendBuf[1]=0x0E;
                    nLen = 16;
                }

                pthread_mutex_unlock(&Uartsem);
                Socket_Send(SendBuf,nLen);
                //gggggTest=0;
            }
            break;
        }
        case IV_SCANF_STATUS_CMD:
        {
            UINT16 nValue;
            UINT8 aRecvBuf[10];

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                //if(gggggTest)
                    //aRecvBuf[3] = 0x02;
                //else
                    //aRecvBuf[3] = 0x08;
                //gggggTest=1;
                pthread_mutex_lock(&Uartsem);
                ComNormalRead(SendBuf[15],32287,1,aRecvBuf);
                pthread_mutex_unlock(&Uartsem);
                SendBuf[16] = aRecvBuf[4];
                SendBuf[17] = aRecvBuf[3];
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case IV_SCANF_TRANS_CMD:
        {
            UINT16 nValue;
            UINT8 i;

            switch(sMessage.mASDU.nSendFlag)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);

                case S_R_BeginToTrans:
                {
                    /*nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_SetConfirm;*/
                    for(i=0;i<(nLen-15);i++)
                    {
                        gDeviceIVInfo[SendBuf[15+i]].nIVScanfLoadFlag = 1;
                    }
                    //Socket_Send(SendBuf,nLen);
                    break;
                }
                case S_R_DataTrans:
                {
                    UINT8 aFilePath[50],aFileReadBuf[200];
                    int nFileReadLen=0;

                    if(SendBuf[16]==0)
                    {
                        gIVScanfCount=0;
                    }
                    else if(SendBuf[16]==1)
                    {
                        gIVScanfCount++;
                    }
                    sprintf((void *)aFilePath,"/mnt/flash/OAM/IV_%02d.bin",SendBuf[15]);
                    nFileReadLen = GetFileData((void *)aFilePath,(void *)aFileReadBuf,gIVScanfCount,SYS_LOG_FILES_LEN);
                    if(nFileReadLen>0)
                    {
                        UINT16 nReadCRC=0;

                        memcpy((UINT8 *)&SendBuf[12],(UINT8 *)&gIVScanfCount,2);
                        memcpy((UINT8 *)&SendBuf[16],aFileReadBuf,nFileReadLen);
                        nReadCRC = CRC16(aFileReadBuf,nFileReadLen);
                        memcpy((UINT8 *)&SendBuf[16+nFileReadLen],(UINT8 *)&nReadCRC,2);
                        SendBuf[1] = nFileReadLen+14+2;
                        Socket_Send(SendBuf,nFileReadLen+16+2);
                    }
                    else
                    {
                        SendBuf[8] = S_R_TransComplete;
                        SendBuf[16] = S_R_TransComplete;
                        Socket_Send(SendBuf,nLen);
                    }
                    break;
                }
                case S_R_DataRetran:
                {
                    UINT8 aFilePath[50],aFileReadBuf[200];
                    int nFileReadLen=0;

                    memcpy((UINT8 *)&gIVScanfCount,(UINT8 *)&SendBuf[16],2);
                    sprintf((void *)aFilePath,"/mnt/flash/OAM/IV_%02d.bin",SendBuf[15]);
                    nFileReadLen = GetFileData((void *)aFilePath,(void *)aFileReadBuf,gIVScanfCount,SYS_LOG_FILES_LEN);
                    if(nFileReadLen>0)
                    {
                        UINT16 nReadCRC=0;

                        SendBuf[8] = S_R_DataTrans;
                        memcpy((UINT8 *)&SendBuf[12],(UINT8 *)&gIVScanfCount,2);
                        memcpy((UINT8 *)&SendBuf[16],aFileReadBuf,nFileReadLen);
                        nReadCRC = CRC16(aFileReadBuf,nFileReadLen);
                        memcpy((UINT8 *)&SendBuf[16+nFileReadLen],(UINT8 *)&nReadCRC,2);
                        SendBuf[1] = nFileReadLen+14+2;
                        Socket_Send(SendBuf,nFileReadLen+15+2);
                    }
                    break;
                }
                case S_R_DataStop:
                {
                    SendBuf[8]++;
                    gDeviceIVInfo[SendBuf[12]].nIVScanfLoadFlag = 0;
                    Socket_Send(SendBuf,nLen);
                    break;
                }
            }
        }
        case IV_LICE_DEL_CMD:
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aComWrite[30];

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                aComWrite[0] = SendBuf[12];
                aComWrite[1] = 0x10;
                aComWrite[2] = 42795/256;
                aComWrite[3] = 42795%256;
                aComWrite[4] = 0;
                aComWrite[5] = 10;
                aComWrite[6] = 20;
                memcpy((UINT8 *)&aComWrite[7],(UINT8 *)&SendBuf[15],20);
                pthread_mutex_lock(&Uartsem);
                ComWrite(gDeviceInfo[SendBuf[12]].nDownlinkPort,aComWrite,27);
                pthread_mutex_unlock(&Uartsem);
                gDeviceIVInfo[SendBuf[12]].nIVLicenseReportLoadFlag=1;
                gDeviceIVInfo[SendBuf[12]].nIVLicenseReportLoadCount=0;
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case IV_LICE_TRANS_CMD://0xE6
        {
            switch(sMessage.mASDU.nSendFlag)
            {
                UINT16 nValue;

                case S_R_BeginToTrans :
                {
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);

                    SendBuf[8]=S_R_BeginToTransConfirm;
                    SendBuf[15]=0x81;
                    gIVLicenseCount=0xFFFF;
                    gIVLicenseDevice=SendBuf[12];
                    LicenseFileOpen(gIVLicenseDevice);

                    Socket_Send(SendBuf,nLen);
                    break;
                }
                case S_R_DataTrans:
                {
                    UINT8 aUpdataTemp[255];
                    UINT32 nUpdataCountTemp=0;
                    UINT16 nCRC,nPacketCRC;

                    nCRC = CalculateCRC((void *)&SendBuf[15],nLen-17);
                    memcpy((UINT8 *)&nPacketCRC,(UINT8 *)&SendBuf[nLen-2],2);
                    if(nCRC!=nPacketCRC)
                    {
                        DbgPrintf("Packet CRC Error = 0x%04X\r\n",nCRC);
                        SendBuf[1] = 0x10;
                        nValue=gSendCount*2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = 0x83;
                        SendBuf[12] = 0x01;
                        SendBuf[13] = 0x00;
                        SendBuf[14] = 0x00;
                        nUpdataCountTemp=gIVLicenseCount+1;
                        memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                        Socket_Send(SendBuf,18);
                    }
                    else
                    {
                        memset(aUpdataTemp,0,sizeof(aUpdataTemp));
                        SendBuf[1]=0x0E;
                        nValue=gSendCount*2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue=gRecvCount*2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        memcpy(aUpdataTemp,(UINT8 *)&SendBuf[15],nLen-17);
                        SendBuf[15]=0x01;
                        memcpy((UINT8 *)&nUpdataCountTemp,(UINT8 *)&SendBuf[12],3);
                        if(gIVLicenseCount==0xFFFF)
                        {
                            memcpy((UINT8 *)&gIVLicenseCount,(UINT8 *)&SendBuf[12],3);
                            LicenseFileWrite(aUpdataTemp,nLen-17);
                            Socket_Send(SendBuf,16);
                        }
                        else
                        {
                            if((gIVLicenseCount+1)==nUpdataCountTemp)
                            {
                                LicenseFileWrite(aUpdataTemp,nLen-17);
                                Socket_Send(SendBuf,16);
                                gIVLicenseCount=nUpdataCountTemp;
                            }
                            else //if((gUpdataCount+1)<nUpdataCountTemp)
                            {
                                SendBuf[1] = 0x10;
                                SendBuf[8] = 0x83;
                                SendBuf[12] = 0x01;
                                SendBuf[13] = 0x00;
                                SendBuf[14] = 0x00;
                                nUpdataCountTemp=gIVLicenseCount+1;
                                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                                Socket_Send(SendBuf,18);
                                DbgPrintf("IV License-----packet number error pre=0x%02X,now=0x%02X\r\n",gIVLicenseCount,nUpdataCountTemp);
                            }
                                /*else if((gUpdataCount+1)>nUpdataCountTemp)
                                {
                                    SendBuf[1] = 0x10;
                                    SendBuf[8] = 0x83;
                                    SendBuf[12] = 0x01;
                                    SendBuf[13] = 0x00;
                                    SendBuf[14] = 0x00;
                                    nUpdataCountTemp=gUpdataCount+1;
                                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nUpdataCountTemp,3);
                                    Socket_Send(SendBuf,18);
                                    printf("Updata-----packet number error pre=0x%02X,now=0x%02X\r\n",gUpdataCount,nUpdataCountTemp);
                                }*/
                        }
                    }
                    break;
                }
                case S_R_DataStop:
                {
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[1] = 0x0E;
                    SendBuf[8] = S_R_DataStopConfirm;
                    Socket_Send(SendBuf,nLen);
                    for(i=1;i<MAXDEVICE;i++)
                    {
                        gDeviceIVInfo[i].nIVLicenseLoadFlag =0;
                    }
                    break;
                }
                case S_R_TransComplete:
                {
                    UINT8 i;

                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_TransComplConfirm;
                    LicenseFileSave();
                    SendBuf[15] = S_R_TransComplConfirm;
                    Socket_Send(SendBuf,nLen);
                    system("tar -zxvf /mnt/flash/OAM/license.tar.gz -C /mnt/flash/OAM");
                    for(i=1;i<=40;i++)
                    {
                        UINT8 aPathBuf[100],aRecvBuf[256];

                        sprintf((void *)aPathBuf,"/mnt/flash/OAM/%d.dat",i);
                        if(access((void *)aPathBuf,F_OK)!=-1)
                        {
                            pthread_mutex_lock(&Uartsem);
                            if(ComNormalRead(i,33107,64,aRecvBuf)==-1)
                            {
                                UINT8 nErrorValue=0xEE;

                                pthread_mutex_unlock(&Uartsem);
                                SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                continue;
                            }
                            else
                                gDeviceIVInfo[i].nIVLicenseLoadFlag=1;
                            pthread_mutex_unlock(&Uartsem);
                        }
                    }
                    DbgPrintf("License File Trans finish\r\n");
                    break;
                }
            }
            break;
        }
        case DEVICE_RESET_CMD:
        {
            UINT8 nAlarmDeviceCount,nAlarmPointCount;

            if((SendBuf[15] & 0xF0) == 0 && SendBuf[8] != 0x08)
            {
                gMainDeviceStatus = 0;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
                gConnectDeviceNum = 0;
                E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
                memset(gDeviceInfo, 0, sizeof(gDeviceInfo));
                gDLinkDeviceInfoCount=0;
                SendBuf[8] = SendBuf[8]+1;
                Socket_Send(SendBuf,nLen);
                memset((UINT8 *)&aAlarmInfo,0,sizeof(aAlarmInfo));
                for(nAlarmDeviceCount=0;nAlarmDeviceCount<40;nAlarmDeviceCount++)
                {
                    for(nAlarmPointCount=0;nAlarmPointCount<20;nAlarmPointCount++)
                    {
                        E2promWrite((UINT8 *)&aAlarmInfo[nAlarmDeviceCount][nAlarmPointCount],AlarmInfoAddr+(nAlarmDeviceCount*20+nAlarmPointCount)*2,2);
                    }
                }
                //close slave channel
                gModemSlaveConfigFlag=0;
                E2promWrite((UINT8 *)&gModemSlaveConfigFlag, SlaveNetInfoConfigAddr, 1);
                system("rm /mnt/flash/OAM/xml/alarm.xml");
                sleep(2);
                system("reboot -f");
            }
            else
            {
                SendBuf[8] = SendBuf[8]+1;
                Socket_Send(SendBuf,nLen);
            }

            break;
        }
        case DEVICE_REBOOT_CMD:
        {
            if (SendBuf[15] == 0x81)
            {/**预置*/
                UINT16 nValue = gSendCount * 2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue = gRecvCount * 2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8] = 0x07;

                Socket_Send(SendBuf, nLen);
            }
            else if (SendBuf[15] == 0x01)
            {/** 设置 */
                UINT16 nValue = gSendCount * 2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue = gRecvCount * 2;
                memcpy(&SendBuf[4], (UINT8 *)&nValue, 2);
                SendBuf[8]  = 0x07;

                Socket_Send(SendBuf, nLen);

                sleep(3);
                system("reboot -f");
            }

        }break;
        case C_SE_NC:
        {
            break;
        }

        case DEVICE_REGISTER_CMD:					//遥调0xD9
        {
			if(SendBuf[8]==0x90)					//查询
            {
                DeviceSdSum=SendBuf[7] - 0x80;
				InquireYT(SendBuf);
            }
            else if(SendBuf[8]==0x92)				//设置遥调参数
            {
                SetYT(SendBuf,DeviceSdSum);
            }
            break;
        }

        case FILE_TRANS_CMD:						//文件传输
        {
        	UINT8  aRecvBuf[256] = {0};		 		 	 //485接收缓存
            switch(sMessage.mASDU.nSendFlag)
            {
                UINT16 nValue;
                static UINT32 total_package_num;
                static UINT32 export_package_serial_num;
                static UINT32 import_package_serial_num;

                case S_R_BeginToTrans:
                {
                    UINT32 total_file_length = 0;

                    //file export
                    if(SendBuf[1] == 0x0f)
                    {
                        UINT16 point_num = 0;

                        //frame analysis
                        memset(g_export_file_path,0,sizeof(g_export_file_path));
                        switch(SendBuf[12])
                        {
                            case UPDATA_FILE:
                            {
                                memset(g_export_file_path,0,sizeof(g_export_file_path));
                                total_file_length = 0;
                                total_package_num = 0;
                                break;
                            }
                            case POINT_FILE:
                            {
                                memcpy((UINT8 *)&point_num, &SendBuf[15], 2);
                                sprintf((void *)g_export_file_path, POINT_FILE_PATH, point_num);
                                total_file_length = StartCompressFile((void *)g_export_file_path);
                                total_package_num = total_file_length % SYS_FILES_LEN == 0? total_file_length / SYS_FILES_LEN : 1 + total_file_length / SYS_FILES_LEN;
                                break;
                            }
                            case DEVICE_INFO_FILE:
                            {
                                sprintf((void *)g_export_file_path,DEVICE_FILE_PATH);
                                total_file_length = StartCompressFile((void *)g_export_file_path);
                                total_package_num = total_file_length % SYS_FILES_LEN == 0? total_file_length / SYS_FILES_LEN : 1 + total_file_length / SYS_FILES_LEN;
                                break;
                            }
                            case ALARM_FILE:
                            {
                                sprintf((void *)g_export_file_path,ALARM_FILE_PATH);
                                total_file_length = StartCompressFile((void *)g_export_file_path);
                                total_package_num = total_file_length % SYS_FILES_LEN == 0? total_file_length / SYS_FILES_LEN : 1 + total_file_length / SYS_FILES_LEN;
                                break;
                            }
                            case IV_DATA_FILE:
                            {
                                memset(g_export_file_path,0,sizeof(g_export_file_path));
                                total_file_length = 0;
                                total_package_num = 0;
                                break;
                            }
                            case IV_LICENSE_FILE:
                            {
                                memset(g_export_file_path,0,sizeof(g_export_file_path));
                                total_file_length = 0;
                                total_package_num = 0;
                                break;
                            }
                            case HISTORY_FILE:
                            {
                                memset(g_export_file_path,0,sizeof(g_export_file_path));
                                total_file_length = 0;
                                total_package_num = 0;
                                break;
                            }
                            case SOUTH_RECORD:
							{
								UINT8 CRCErrcount = 0;
								UINT16 rCRC;
								total_package_num = 0;
								export_package_serial_num = 0;
								dSouth_Record_Information = SendBuf[16] | (SendBuf[15] << 8);
								if((dSouth_Record_Information && 0xFF) && (dSouth_Record_Information && 0xFF) < 32)	//高八位Modbus地址  低八位月日期   同时也是南向导出标志位
								{
									SendBuf[0] = (dSouth_Record_Information&0xFF00)>>8;
									SendBuf[1] = 0x1D;
									SendBuf[2] = 0x33;
									SendBuf[3] = dSouth_Record_Information&0xFF;
									rCRC = CRC16(SendBuf,4);
									SendBuf[4]=rCRC&0xFF;
									SendBuf[5]=rCRC>>8;
									SouthCmdTask(SendBuf,6,aRecvBuf,gDeviceInfo[SendBuf[0]].nDownlinkPort);
									rCRC = (aRecvBuf[11]<<8) | aRecvBuf[10];

									if(CRC16(aRecvBuf,10) == rCRC)
									{
										total_file_length = (aRecvBuf[5]<<24)|(aRecvBuf[6]<<16)|
												(aRecvBuf[7]<<8)|(aRecvBuf[8]);

										total_package_num = total_file_length%200 == 0? total_file_length/200 : 1 + total_file_length/200;
									}
									else
									{
										DbgPrintf("CRC Inspection failure!!!\r\n");
										total_file_length = 0;
										total_package_num = 0;
										export_package_serial_num = 0;
										dSouth_Record_Information = 0;
									}
								}
								DbgPrintf("SOUTH RECORD Package Length:%d--total package num:%d!!!\r\n",total_file_length,total_package_num);
								break;
							}

                            default:
                            {
                                memset(g_export_file_path,0,sizeof(g_export_file_path));
                                total_file_length = 0;
                                total_package_num = 0;
                                break;
                            }
                        }

                        //response
                        SendBuf[0] = 0x68;
                        SendBuf[1] = 0x12;
                        nValue = gSendCount * 2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount * 2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_BeginToTransConfirm;
                        SendBuf[15] = total_file_length & 0xff;
                        SendBuf[16] = (total_file_length >> 8) & 0xff;
                        SendBuf[17] = (total_file_length >> 16) & 0xff;
                        SendBuf[18] = (total_file_length >> 24) & 0xff;
                        SendBuf[19] = dSouth_Record_Information?SYS_LOG_FILES_LEN:SYS_FILES_LEN;       //default length of file data in a package frame

                        //confirm file existence
                        if(total_package_num > 0)
                        {
                            Socket_Send(SendBuf,20);
                            DbgPrintf("[FILE UPLOAD] START---uploading file %s, file length = %d bytes\n",
                                g_export_file_path, total_file_length);
                            //reset frame number
                            export_package_serial_num= 0;
                            gUpdataModeFlag = 1;
                        }
                        else
                        {
                            DbgPrintf("[FILE UPLOAD] START FAIL---file not exist\n");
                        }

                        break;
                    }
                    if(SendBuf[1] == 0x0e)
                    {
                        memset(g_export_file_path,0,sizeof(g_export_file_path));
                        switch(SendBuf[12])
                        {
                            case LOG_FILE:
                            {
                                sprintf((void *)g_export_file_path,LOG_FILE_PATH,SendBuf[15]);
                                total_file_length = StartCompressFile((void *)g_export_file_path);
                                total_package_num = total_file_length % SYS_FILES_LEN == 0? total_file_length / SYS_FILES_LEN : 1 + total_file_length / SYS_FILES_LEN;
                                break;
                            }
                            case RECORD_FILE:
                            {
                                sprintf((void *)g_export_file_path,RECORD_FILE_PATH,SendBuf[15]);
                                total_file_length = StartCompressFile((void *)g_export_file_path);
                                total_package_num = total_file_length % SYS_FILES_LEN == 0? total_file_length / SYS_FILES_LEN : 1 + total_file_length / SYS_FILES_LEN;
                                break;
                            }
                        }
                        //response
                        SendBuf[1] = 0x12;
                        nValue = gSendCount * 2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount * 2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_BeginToTransConfirm;
                        SendBuf[15] = total_file_length & 0xff;
                        SendBuf[16] = (total_file_length >> 8) & 0xff;
                        SendBuf[17] = (total_file_length >> 16) & 0xff;
                        SendBuf[18] = (total_file_length >> 24) & 0xff;
                        SendBuf[19] = SYS_FILES_LEN;       //default length of file data in a package frame

                        //confirm file existence
                        if(total_package_num > 0)
                        {
                            //raedy to send
                            Socket_Send(SendBuf,20);
                            DbgPrintf("[FILE UPLOAD] START---uploading file %s, file length = %d bytes\n",
                                g_export_file_path, total_file_length);
                            //reset frame number
                            export_package_serial_num= 0;
                        }
                        else
                        {
                            DbgPrintf("[FILE UPLOAD] START FAIL---file not exist\n");
                        }
                        break;
                    }

                    //file import
                    if(SendBuf[1] == 0x13)
                    {
                        UINT32 total_file_length;
                        FileOpen();

                        //response
                        nValue = gSendCount * 2;
                        memcpy(&SendBuf[2], (UINT8 *)&nValue, 2);
                        nValue = gRecvCount * 2;
                        memcpy(&SendBuf[4], (UINT8 *)&nValue, 2);

                        //raedy to send
                        Socket_Send(SendBuf,21);
                        memcpy((UINT8 *)&total_file_length, &SendBuf[16], 4);
                        DbgPrintf("[FILE DOWNLOAD] START---downloading file length = %d bytes\n", total_file_length);
                        //reset frame number
                        import_package_serial_num = 0xffffffff;

                        break;
                    }

					//file import update
					if(SendBuf[1] == 0x25)
					{
					    UINT32 total_file_length;
						FileOpen();

						//response
						nValue = gSendCount * 2;
						memcpy(&SendBuf[2], (UINT8 *)&nValue, 2);
						nValue = gRecvCount * 2;
						memcpy(&SendBuf[4], (UINT8 *)&nValue, 2);
						memcpy(&gDt1000Update.vertion[0], &SendBuf[17], 17);
						memcpy((UINT8 *)&gDt1000Update.nDataLen, &SendBuf[34], 4);

						DbgPrintf("DT1000_Vertion: ");
						UINT8 i;
						for(i=0;i<17;i++)
						{
							DbgPrintf("%c",gDt1000Update.vertion[i]);
						}
						DbgPrintf("\nFileLen:%d	 ",gDt1000Update.nDataLen);
						DbgPrintf("\r\n");
                        SendBuf[8]=S_R_BeginToTransConfirm;
						//raedy to send
						Socket_Send(SendBuf,39);
						//gDt1000UpdataFlag=1;
						//reset frame number
						import_package_serial_num = 0xffffffff;
						break;
					}
                }

                case S_R_BeginToTransConfirm:
                {
                	UINT8  data_temp[FIXED_LENGTH];
                	UINT16 rCRC;
                	UINT8  file_whole_frame[SYS_FRAME_LEN] = {0};      //APDU packet: fixed data + customized data + CRC

					if((dSouth_Record_Information && 0xFF) && (dSouth_Record_Information && 0xFF) < 32)	//高八位Modbus地址  低八位本月日期
					{
						data_temp[0] = (dSouth_Record_Information&0xFF00)>>8;
						data_temp[1] = 0x2D;
						data_temp[2] = 0x33;
						data_temp[3] = dSouth_Record_Information&0xFF;
						data_temp[4] = (export_package_serial_num+1)>>8;
						data_temp[5] = (export_package_serial_num+1);
						rCRC = CRC16(data_temp,6);
						data_temp[6]=rCRC&0xFF;
						data_temp[7]=rCRC>>8;
						memset(aRecvBuf,0,sizeof(aRecvBuf));
						SouthCmdTask(data_temp,8,aRecvBuf,gDeviceInfo[data_temp[0]].nDownlinkPort);
						rCRC = (aRecvBuf[aRecvBuf[6]+8]<<8) | aRecvBuf[aRecvBuf[6]+7];

						if(CRC16((void *)aRecvBuf,207) == rCRC)
						{
							//fixed data framing
							memcpy(data_temp,SendBuf,15);
							data_temp[1] = aRecvBuf[6] + FIXED_LENGTH;
							nValue = gSendCount *2;
							memcpy(&data_temp[2],(UINT8 *)&nValue,2);
							nValue = gRecvCount *2;
							memcpy(&data_temp[4],(UINT8 *)&nValue,2);
							data_temp[8] = 0X82;
							data_temp[12] = export_package_serial_num & 0xff;
							data_temp[13] = (export_package_serial_num >> 8) & 0xff;
							data_temp[14] = (export_package_serial_num >> 16) & 0xff;
							if(export_package_serial_num <= total_package_num)
							{
								memcpy(file_whole_frame, data_temp, FIXED_LENGTH);
								memcpy(&file_whole_frame[FIXED_LENGTH], &aRecvBuf[7],aRecvBuf[6]);
								//CRC calculate
								rCRC = CalculateCRC((void *)&aRecvBuf[7], aRecvBuf[6]);
								file_whole_frame[FIXED_LENGTH + aRecvBuf[6]] = rCRC & 0xff;
								file_whole_frame[FIXED_LENGTH + aRecvBuf[6] + 1] = (rCRC >> 8) & 0xff;
								//send package frame
								DbgPrintf("[FILE SOUTH RECORD] TRANSMISSION  Length:%d---Send package %d\n",aRecvBuf[6],export_package_serial_num);
								Socket_Send(file_whole_frame, FIXED_LENGTH + aRecvBuf[6] + 2);
							}
							CRCErrcount = 0;
						}
						else
						{
							CRCErrcount++;
							if(CRCErrcount > 2)
							{
								dSouth_Record_Information = 0;
								gUpdataModeFlag=0;
							}
							DbgPrintf("CRC Inspection failure!!!\r\n");
						}
					}
					break;
                }

                case S_R_DataTrans:
                {
                	UINT8  file_whole_frame[SYS_FRAME_LEN] = {0};      //APDU packet: fixed data + customized data + CRC
					UINT8  file_data[SYS_FILES_LEN] = {0};             //customized data: 200 bytes
					UINT8  data_temp[FIXED_LENGTH];
					UINT16 rCRC;
                	if(SendBuf[1] == 0x10 && dSouth_Record_Information)
                	{
						data_temp[0] = (dSouth_Record_Information&0xFF00)>>8;
						data_temp[1] = 0x2D;
						data_temp[2] = 0x33;
						data_temp[3] = dSouth_Record_Information&0xFF;
						export_package_serial_num++;
						data_temp[4] = (export_package_serial_num+1)>>8;
						data_temp[5] = (export_package_serial_num+1);
						rCRC = CRC16(data_temp,6);
						data_temp[6]=rCRC&0xFF;
						data_temp[7]=rCRC>>8;
						if(export_package_serial_num < total_package_num)
						{
							for(i = 0;i < 3;i++)
							{
								memset(aRecvBuf,0,sizeof(aRecvBuf));
								SouthCmdTask(data_temp,8,aRecvBuf,gDeviceInfo[data_temp[0]].nDownlinkPort);
								rCRC = (aRecvBuf[aRecvBuf[6]+8]<<8) | aRecvBuf[aRecvBuf[6]+7];
								if(CRC16((void *)aRecvBuf,207) == rCRC)
								{
									dSouth_Record_Information = 0;
									break;
								}
							}

							if(CRC16((void *)aRecvBuf,207) == rCRC)
							{
								//fixed data framing
								memcpy(data_temp,SendBuf,15);
								data_temp[1] = aRecvBuf[6] + FIXED_LENGTH;
								nValue = gSendCount *2;
								memcpy(&data_temp[2],(UINT8 *)&nValue,2);
								nValue = gRecvCount *2;
								memcpy(&data_temp[4],(UINT8 *)&nValue,2);
								data_temp[8] = (aRecvBuf[6] == 200)?0X82:0x86;
								data_temp[12] = export_package_serial_num & 0xff;
								data_temp[13] = (export_package_serial_num >> 8) & 0xff;
								data_temp[14] = (export_package_serial_num >> 16) & 0xff;
								memcpy(file_whole_frame, data_temp, FIXED_LENGTH);
								memcpy(&file_whole_frame[FIXED_LENGTH], &aRecvBuf[7],aRecvBuf[6]);
								//CRC calculate
								rCRC = CalculateCRC((void *)&aRecvBuf[7], aRecvBuf[6]);
								file_whole_frame[FIXED_LENGTH + aRecvBuf[6]] = rCRC & 0xff;
								file_whole_frame[FIXED_LENGTH + aRecvBuf[6] + 1] = (rCRC >> 8) & 0xff;
								//send package frame
								DbgPrintf("[FILE SOUTH RECORD] TRANSMISSION length:%d --send package %d\n",aRecvBuf[6],export_package_serial_num);
								Socket_Send(file_whole_frame, FIXED_LENGTH + aRecvBuf[6] + 2);
							}
							else
							{
								CRCErrcount++;
								if(CRCErrcount > 2)
								{
									dSouth_Record_Information = 0;
									gUpdataModeFlag=0;
								}
								DbgPrintf("CRC Inspection failure -- send package %d!!!\r\n",export_package_serial_num);
								export_package_serial_num--;
							}
						}
                		else
                		{
							memcpy(data_temp,SendBuf,15);
							data_temp[1] = FIXED_LENGTH;
							nValue = gSendCount *2;
							memcpy(&data_temp[2],(UINT8 *)&nValue,2);
							nValue = gRecvCount *2;
							memcpy(&data_temp[4],(UINT8 *)&nValue,2);
							data_temp[8] = 0x86;
							data_temp[12] = export_package_serial_num & 0xff;
							data_temp[13] = (export_package_serial_num >> 8) & 0xff;
							data_temp[14] = (export_package_serial_num >> 16) & 0xff;

							//CRC calculate
							rCRC = CalculateCRC((void *)data_temp,FIXED_LENGTH);
							data_temp[FIXED_LENGTH] = rCRC & 0xff;
							data_temp[FIXED_LENGTH+ 1] = (rCRC >> 8) & 0xff;
							//send package frame
							DbgPrintf("[FILE SOUTH RECORD] TRANSMISSION length:%d --send package %d\n",data_temp[1],export_package_serial_num);
							Socket_Send(data_temp, FIXED_LENGTH + 2);
							dSouth_Record_Information = 0;
                		}
						break;
					}
                	else if(SendBuf[1] == 0x10)		//export file
                    {
                        export_package_serial_num = SendBuf[15] + (SendBuf[16] << 8) + (SendBuf[17] << 16);

                        //fixed data framing
                        memcpy(data_temp,SendBuf,15);
                        data_temp[1] = 0xef;
                        nValue = gSendCount *2;
                        memcpy(&data_temp[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount *2;
                        memcpy(&data_temp[4],(UINT8 *)&nValue,2);
                        data_temp[12] = export_package_serial_num & 0xff;
                        data_temp[13] = (export_package_serial_num >> 8) & 0xff;
                        data_temp[14] = (export_package_serial_num >> 16) & 0xff;

                        //customized data framing and send
                        if(export_package_serial_num < total_package_num)
                        {
                            if(GetFileData(EXPORT_FILE_PATH, (void *)file_data, export_package_serial_num, SYS_FILES_LEN) != 0)
                            {
                                memcpy(file_whole_frame, data_temp, FIXED_LENGTH);
                                memcpy(&file_whole_frame[FIXED_LENGTH], file_data, SYS_FILES_LEN);
                                //CRC calculate
                                rCRC = CalculateCRC((void *)file_data, SYS_FILES_LEN);
                                file_whole_frame[FIXED_LENGTH + SYS_FILES_LEN] = rCRC & 0xff;
                                file_whole_frame[FIXED_LENGTH + SYS_FILES_LEN + 1] = (rCRC >> 8) & 0xff;
                                //send package frame
                                DbgPrintf("[FILE UPLOAD] TRANSMISSION---send package %d\n",export_package_serial_num);
                                Socket_Send(file_whole_frame, FIXED_LENGTH + SYS_FILES_LEN + CRC_LENGTH);
                            }
                        }
                        else
                        {
                            DbgPrintf("[FILE UPLOAD] TRANSMISSION FAIL---do not have package %d, total package number is %d\n",export_package_serial_num,total_package_num);
                        }
                        break;
                    }

                    //import file
                    if(SendBuf[1] >= 0x12)
                    {
                        UINT8  file_data_temp[255];
                        UINT32 import_package_serial_num_temp = 0;
                        UINT16 crc, crc_reference;
                        crc = CalculateCRC((void *)&SendBuf[15], nLen - 17);
                        memcpy((UINT8 *)&crc_reference, (UINT8 *)&SendBuf[nLen-2], 2);
                        if(crc != crc_reference)
                        {
                            SendBuf[1] = 0x10;
                            nValue = gSendCount *2;
                            memcpy(&SendBuf[2],(UINT8 *)&nValue, 2);
                            nValue = gRecvCount *2;
                            memcpy(&SendBuf[4],(UINT8 *)&nValue, 2);
                            SendBuf[8] = 0x83;
                            SendBuf[12] = 0x01;
                            SendBuf[13] = 0x00;
                            SendBuf[14] = 0x00;
                            import_package_serial_num_temp = import_package_serial_num + 1;
                            memcpy((UINT8 *)&SendBuf[15], (UINT8 *)&import_package_serial_num_temp, 3);
                            Socket_Send(SendBuf, 18);
                            DbgPrintf("[FILE DOWNLOAD] TRANSMISSION FAIL---crc check fail, retransmission package %d\n",
                                import_package_serial_num_temp);
                        }
                        else
                        {
                            memset(file_data_temp, 0, sizeof(file_data_temp));
                            SendBuf[1] = 0x10;
                            nValue = gSendCount *2;
                            memcpy(&SendBuf[2],(UINT8 *)&nValue, 2);
                            nValue = gRecvCount *2;
                            memcpy(&SendBuf[4],(UINT8 *)&nValue, 2);
                            memcpy(file_data_temp, &SendBuf[15], nLen-17);
                            SendBuf[15] = SendBuf[12];
							SendBuf[16] = SendBuf[13];
							SendBuf[17] = SendBuf[14];
                            memcpy((UINT8 *)&import_package_serial_num_temp, &SendBuf[12], 3);
                            if((import_package_serial_num == 0xffffffff) && (import_package_serial_num_temp == 0))
                            {
                                import_package_serial_num = 0;
                                memcpy((UINT8 *)&import_package_serial_num, &SendBuf[12], 3);
                                FileWrite(n_file_fd, file_data_temp, nLen-17);

								SendBuf[12] = 0x00;
							    SendBuf[13] = 0x00;
							    SendBuf[14] = 0x00;
                                Socket_Send(SendBuf, 18);
                                g_import_file_crc = 0;
                                g_import_file_crc = CalculateFileCRC((void *)file_data_temp, nLen-17, 1);
                                DbgPrintf("[FILE DOWNLOAD] TRANSMISSION---receive package %d\n", import_package_serial_num_temp);
                            }
                            else
                            {
                                if((import_package_serial_num+1) == import_package_serial_num_temp)
                                {
                                    FileWrite(n_file_fd, file_data_temp, nLen-17);
									SendBuf[12] = 0x00;
							        SendBuf[13] = 0x00;
							        SendBuf[14] = 0x00;
                                    Socket_Send(SendBuf, 18);
                                    g_import_file_crc = CalculateFileCRC((void *)file_data_temp, nLen-17, 1);
                                    DbgPrintf("[FILE DOWNLOAD] TRANSMISSION---receive package %d\n",import_package_serial_num_temp);
                                    import_package_serial_num++;
                                }
                                else
                                {
                                    SendBuf[1] = 0x10;
                                    nValue = gSendCount *2;
                                    memcpy(&SendBuf[2],(UINT8 *)&nValue, 2);
                                    nValue = gRecvCount *2;
                                    memcpy(&SendBuf[4],(UINT8 *)&nValue, 2);
                                    SendBuf[8] = 0x83;
                                    SendBuf[12] = 0x01;
                                    SendBuf[13] = 0x00;
                                    SendBuf[14] = 0x00;
                                    import_package_serial_num_temp = import_package_serial_num + 1;
                                    memcpy((UINT8 *)&SendBuf[15], (UINT8 *)&import_package_serial_num_temp, 3);
                                    Socket_Send(SendBuf, 18);
                                    DbgPrintf("[FILE DOWNLOAD] TRANSMISSION FAIL---package number error, retransmission package %d\n",
                                        import_package_serial_num_temp);
                                }
                            }
                        }
                        break;
                    }
                }
                case S_R_DataStop:
                {
                    //file export
                    if(SendBuf[15] == 0x01)
                    {
                    	dSouth_Record_Information = 0;
                        //framing and response
                        SendBuf[1] = 0x0e;
                        nValue = gSendCount *2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount *2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_DataStopConfirm;
                        SendBuf[16] = S_R_DataStopConfirm;
                        Socket_Send(SendBuf, 17);
                        DbgPrintf("[FILE UPLOAD] TRANSMISSION STOP\n");
                        break;
                    }

                    //file import
                    if(SendBuf[15] == S_R_DataStop)
                    {
                        SendBuf[1] = 0x0e;
                        nValue = gSendCount *2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount *2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_DataStopConfirm;
                        SendBuf[15] = S_R_DataStopConfirm;
                        Socket_Send(SendBuf, 16);
                        DbgPrintf("[FILE DOWNLOAD] TRANSMISSION STOP\n");
                        break;
                    }
                }
                case S_R_TransComplete:
                {
                    UINT8  file_data[SYS_FILES_LEN] = {0};
                    UINT32 package_serial_num = 0;
                    //file export
                    if(SendBuf[1] == 0x0e)
                    {
                    	dSouth_Record_Information = 0;
                        //framing and response
                        SendBuf[1] = 0x0f;
                        nValue = gSendCount *2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount *2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_TransComplConfirm;
                        SendBuf[12] = total_package_num & 0xff;
                        SendBuf[13] = (total_package_num >> 8) & 0xff;
                        SendBuf[14] = (total_package_num >> 16) & 0xff;

                        //file crc calculate(CRC-CCITT(XModem))
                        while(GetFileData(EXPORT_FILE_PATH, (void *)file_data, package_serial_num, SYS_FILES_LEN) != 0)
                        {
                            if(package_serial_num == 0)
                                g_import_file_crc = 0;
                            g_import_file_crc = CalculateFileCRC((void *)file_data, SYS_FILES_LEN, 1);
                            package_serial_num++;
                        }

                        //file crc calculate(CRC-16(Modbus))
                        /*g_file_crc_high = 0xff;
                        g_file_crc_low = 0xff;
                        while(getFileData(EXPORT_FILE_PATH, file_data, package_serial_num, SYS_FILES_LEN) != 0)
                        {
                            file_crc = FileCRC16(file_data, SYS_FILES_LEN);
                            g_file_crc_high = (file_crc >> 8) & 0xff;
                            g_file_crc_low = file_crc & 0xff;
                            package_serial_num++;
                        }
                        SendBuf[15] = g_file_crc_low;
                        SendBuf[16] = g_file_crc_high;*/

                        memcpy(&SendBuf[15], (UINT8 *)&g_export_file_crc, 2);
                        Socket_Send(SendBuf, 17);
                        DbgPrintf("[FILE UPLOAD] TRANSMISSION COMPLETE\n");
                        break;
                    }

                    //file import
                    if(SendBuf[1] == 0x0f)
                    {
                        UINT16 crc_reference;
                        UINT8  cmd_line[50] = {0};
                        UINT8  error_count = 0;
						UINT8  import_file_parent_directory[50] = {0};
                        UINT32 run_version = 0;

                        SendBuf[1] = 0x0e;
                        nValue = gSendCount *2;
                        memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                        nValue = gRecvCount *2;
                        memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                        SendBuf[8] = S_R_TransComplConfirm;
                        FileSave();

                        //crc check

                        /*//file crc calculate(CRC-CCITT(XModem))
                        while(getFileData(IMPORT_FILE_PATH, file_data, package_serial_num, SYS_FILES_LEN) != 0)
                        {
                            if(package_serial_num == 0)
                                g_import_file_crc = 0;
                            g_import_file_crc = CalculateFileCRC(file_data, SYS_FILES_LEN, 1);
                            package_serial_num++;
                            printf("g_import_file_crc = %x",g_import_file_crc);
                        }*/

                        //file crc calculate(CRC-16(Modbus))
                        /*g_file_crc_high = 0xff;
                        g_file_crc_low = 0xff;
                        while(getFileData(EXPORT_FILE_PATH, file_data, package_serial_num, SYS_FILES_LEN) != 0)
                        {
                            file_crc = FileCRC16(file_data, SYS_FILES_LEN);
                            g_file_crc_high = (file_crc >> 8) & 0xff;
                            g_file_crc_low = file_crc & 0xff;
                            package_serial_num++;
                        }
                        SendBuf[15] = g_file_crc_low;
                        SendBuf[16] = g_file_crc_high;*/

                        memcpy((UINT8 *)&crc_reference, (UINT8 *)&SendBuf[15], 2);
                        //printf("g_import_file_crc = %x\n",g_import_file_crc);
                        //printf("crc_reference = %x\n",crc_reference);
                        if(g_import_file_crc == crc_reference)
                        {
                            //response0D
                            Socket_Send(SendBuf, 17);
                            DbgPrintf("[FILE DOWNLOAD] TRANSMISSION COMPLETE\n");
                            //unzip and cover
                            if((access(IMPORT_FILE_PATH, F_OK) != -1))
                            {
                                if(strstr(g_import_file_path, "/mnt/flash/OAM/OAM") != NULL)
                                {
                                    run_version = SYS_OAM_VERSION;
                                    E2promWrite((UINT8 *)&run_version, DeviceSwOldVerAddr, 3);
                                    sleep(1);
                                }
                                sprintf(g_import_file_path,DT1000_FILE_PATH);
                                sprintf(cmd_line, "rm %s", g_import_file_path);
                                DbgPrintf("cmd_line = %s\n", cmd_line);
                                system(cmd_line);

                                /*memset(cmd_line, 0, sizeof(cmd_line));
                                sprintf(cmd_line, "mv %s %s", g_import_file_path, g_import_file_path);
                                DbgPrintf("cmd_line = %s\n", cmd_line);
                                system(cmd_line);*/


                                memset(cmd_line, 0, sizeof(cmd_line));
                                if(strstr(g_import_file_path, "/mnt/flash/OAM/point/") != NULL)
                                    sprintf(import_file_parent_directory, "/mnt/flash/OAM/point");
                                else if(strstr(g_import_file_path, "/mnt/flash/OAM/log/") != NULL)
                                         sprintf(import_file_parent_directory, "/mnt/flash/OAM/log");
                                     else if(strstr(g_import_file_path, "/mnt/flash/OAM/record/") != NULL)
                                              sprintf(import_file_parent_directory, "/mnt/flash/OAM/record");
                                          else if(strstr(g_import_file_path, "/mnt/flash/OAM/xml/") != NULL)
                                                   sprintf(import_file_parent_directory, "/mnt/flash/OAM/xml");
                                               else if(strstr(g_import_file_path, "/mnt/flash/OAM/OAM") != NULL)
                                                        sprintf(import_file_parent_directory, "/mnt/flash/OAM");
                                                    else
                                                        sprintf(import_file_parent_directory, "/mnt/flash/OAM");
                                sprintf(cmd_line, "mv %s %s", IMPORT_FILE_PATH, g_import_file_path);
                                DbgPrintf("cmd_line = %s\n", cmd_line);
                                system(cmd_line);
                                /*do{
                                    system((void *)cmd_line);
                                    sleep(2);
                                    error_count++;
                                    if(error_count > 3)
                                        break;
                                }while(access(g_import_file_path, F_OK) == -1);*/

                                if(access(g_import_file_path, F_OK) == -1)
                                {
                                    memset(cmd_line, 0, sizeof(cmd_line));
                                    sprintf(cmd_line, "mv %s_pre %s", g_import_file_path, g_import_file_path);
                                    DbgPrintf("cmd_line = %s\n", cmd_line);
                                    system(cmd_line);
                                }

								UINT32 total_file_length;
                                sprintf(g_import_file_path,DT1000_FILE_PATH);
								DbgPrintf("g_import_file_path:%s\n",g_import_file_path);
								total_file_length=GetFileLength(g_import_file_path);
								DbgPrintf("total_file_length:%d\n",total_file_length);
								gDt1000Update.nDataLen=total_file_length;
								DbgPrintf("nDataLen:%d\n",gDt1000Update.nDataLen);

                                memset(cmd_line, 0, sizeof(cmd_line));
                                sprintf(cmd_line, "chmod 777 %s", g_import_file_path);
                                DbgPrintf("cmd_line = %s\n", cmd_line);
                                system(cmd_line);

								gDt1000UpdataFlag=1;
								DbgPrintf("gDt1000UpdataFlag: %d\n", gDt1000UpdataFlag);



                                if(strstr(g_import_file_path, "/mnt/flash/OAM/OAM") != NULL)
                                {
                                    system("ls /mnt/flash/OAM/");
                                    system("reboot -f");
                                }
                            }
                            if(strstr(g_import_file_path, "/mnt/flash/OAM/OAM") != NULL)
                            {
                                gUpdataModeFlag = 0;
                            }
                        }
                        else
                        {
                            if(strstr(g_import_file_path, "/mnt/flash/OAM/OAM") != NULL)
                            {
                                gUpdataModeFlag = 0;
                            }
                            DbgPrintf("FILE DOWNLOAD: TRANSMISSION COMPLETE FAIL---crc check fail\n");
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
    return nRes;
}

/*****************************************************************************
* Description:      deal recv frame and response to app
* Parameters:
* Returns:
*****************************************************************************/
UINT8 APPackMainFunction(UINT8 nProtocol,UINT8 *aBuf,UINT8 nLen)
{
    sAPDU sMessage;
    UINT8 SendBuf[SYS_FRAME_LEN];
    UINT8 i=0,nFrameType;        //FrameType   0:I  1:S  2:U
    UINT8 nRes=0,aLogBuf[256];

    memcpy(SendBuf,aBuf,nLen);
    //APCI
    /*for(j=0;j<nLen;j++)
    {
        printf("%02X ",SendBuf[j]);
    }
    printf("\r\n");*/


    sMessage.mAPCI.nHead=aBuf[i++];
    sMessage.mAPCI.nPktLen=aBuf[i++];
    if((aBuf[i]&0x01)==0)
    {
        //I Frame
        nFrameType=0;
        sMessage.mAPCI.nSendCtrl=(aBuf[i++]>>1);
        sMessage.mAPCI.nSendCtrl+=aBuf[i++];
        sMessage.mAPCI.nRecvCtrl=(aBuf[i++]>>1);
        sMessage.mAPCI.nRecvCtrl+=aBuf[i++];
    }
    else
    {
        if((aBuf[i]&0x02)!=0)
        {
            //U Frame
            TestLogTimeFileWrite();
            TestLogTypeFileWrite(1);
            TestLogStringFileWrite((void *)"U Frame ",strlen("U Frame "));
            nFrameType=2;
            DbgPrintf("U:Status=%d SendCount=%02d RecvCount=%02d\r\n",g_TransSta,gSendCount,gRecvCount);
            if(((aBuf[i]>>6)&0x03)==0x01)//Test Command
            {
                //g_TransSta=2;
                DbgPrintf("U:Test\r\n");
                TestLogStringFileWrite((void *)"Heart Test",strlen("Heart Test"));
                SendUFramePacket(1,0x83);
            }
            TestLogStringFileWrite((void *)"\n",strlen("\n"));
            return 0;
        }
    }
    //ASDU
    LogRecordFileWrite(3,SendBuf,nLen);
    sMessage.mASDU.nCmdID=aBuf[i++];
    sMessage.mASDU.nMessNum=(aBuf[i++]&0x7F);
    sMessage.mASDU.nSendFlag=aBuf[i++];
    sMessage.mASDU.nSendFlag+=aBuf[i++]*256;
    sMessage.mASDU.nPublicAddr=aBuf[i++];
    sMessage.mASDU.nPublicAddr+=aBuf[i++]*256;
    memcpy((UINT8 *)&sMessage.mASDU.nPublicAddr,(UINT8 *)&aBuf[i],3);
    DbgPrintf("AP I:Status=%d Flag=%04X Command ID=0x%02X\r\n",g_TransSta,sMessage.mASDU.nSendFlag,sMessage.mASDU.nCmdID);
    TestLogTimeFileWrite();
    TestLogTypeFileWrite(4);
    switch(sMessage.mASDU.nCmdID)
    {
        case DEVICE_IP_CMD://0xB5
        {
            UINT16 nValue;
            UINT8 routeflag=0;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device IP:Value=%d.%d.%d.%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                E2promRead((UINT8 *)&SendBuf[15],DeviceIP_E2P,4);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15],SendBuf[16],SendBuf[17],SendBuf[18]);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]="Set Device IP:Value=%d.%d.%d.%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                if((gMainDeviceIP[0]!=SendBuf[15])
                    ||(gMainDeviceIP[1]!=SendBuf[16])
                    ||(gMainDeviceIP[2]!=SendBuf[17])
                    ||(gMainDeviceIP[3]!=SendBuf[18]))
                {
                    memcpy(gMainDeviceIP,(UINT8 *)&SendBuf[15],4);
                    E2promWrite((UINT8 *)&SendBuf[15],DeviceIP_E2P,4);
                    gDeviceIPSetFlag=1;
                    E2promWrite((UINT8 *)&gDeviceIPSetFlag,DeviceIPSetAddr,1);
                    routeflag=1;
                    E2promWrite((UINT8 *)&routeflag,RouteConfigAddr,1);
                    SysNetInit();
                }
                sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15],SendBuf[16],SendBuf[17],SendBuf[18]);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case MODBUS_ENDIAN_CMD://0xB6
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                TestLogStringFileWrite((void *)"Get Downlink Device Endian",strlen("Get Downlink Device Endian"));
                for(i=0;i<((nLen-15)/2);i++)
                {
                    SendBuf[15+i*2+1]=gDeviceInfo[SendBuf[15+i*2]].nEndian;
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*2],SendBuf[15+i*2+1]);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                AP_Send(SendBuf,nLen);
                SendBuf[8]=S_R_InfoReport;
                Socket_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                TestLogStringFileWrite((void *)"Set Downlink Device Endian",strlen("Get Downlink Device Endian"));
                for(i=0;i<((nLen-15)/2);i++)
                {
                    gDeviceInfo[SendBuf[15+i*2]].nEndian=SendBuf[15+i*2+1];
                    E2promWrite((UINT8 *)&gDeviceInfo[SendBuf[15+i*2]].nEndian,DeviceTypeInfoAddr+(SendBuf[15+i*2]-1)*5+4,1);

                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*2],SendBuf[15+i*2+1]);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                AP_Send(SendBuf,nLen);
                SendBuf[8]=S_R_InfoReport;
                Socket_Send(SendBuf,nLen);
           }
            break;
        }
        case MODBUS_RATE_CMD://0xB7
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                TestLogStringFileWrite((void *)"Get Downlink Device Baudrate",strlen("Get Downlink Device Baudrate"));
                for(i=0;i<((nLen-15)/4);i++)
                {
                    memcpy((UINT8 *)&SendBuf[15+i*4+1],(UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,3);
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*4],gDeviceInfo[SendBuf[15+i*4]].nRate);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]=",Device No.%d Value=%d";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                TestLogStringFileWrite((void *)"Set Downlink Device Baudrate",strlen("Set Downlink Device Baudrate"));
                for(i=0;i<((nLen-15)/4);i++)
                {
                    memcpy((UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,(UINT8 *)&SendBuf[15+i*4+1],3);
                    E2promWrite((UINT8 *)&gDeviceInfo[SendBuf[15+i*4]].nRate,DeviceTypeInfoAddr+(SendBuf[15+i*4]-1)*5,4);
                    sprintf((void *)aLogBuf,(void *)aRegexBuf,SendBuf[15+i*4],gDeviceInfo[SendBuf[15+i*4]].nRate);
                    TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                }
                TestLogStringFileWrite((void *)"\n",strlen("\n"));
                AP_Send(SendBuf,nLen);
                SendBuf[8]=S_R_InfoReport;
                Socket_Send(SendBuf,nLen);

            }
            break;
        }
        case DEVICE_ADDR_CMD://0xB8
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device Address,Value=%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gMainDeviceID,2);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,gMainDeviceID);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 aRegexBuf[256]="Set Device Address,Value=%d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy((UINT8 *)&gMainDeviceID,(UINT8 *)&SendBuf[15],2);
                E2promWrite((UINT8 *)&SendBuf[15],DeviceAddress,2);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,gMainDeviceID);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                AP_Send(SendBuf,nLen);
                SendBuf[8]=S_R_InfoReport;
                Socket_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_VERSION_CMD://0xB9
        {
            UINT16 nValue;
            UINT32 nVersion;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                UINT8 aRegexBuf[256]="Get Device Software Version,Local Value=%06d,Old Value=%06d\n";

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                nVersion=SYS_OAM_VERSION;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&nVersion,3);
                memcpy((UINT8 *)&SendBuf[18],(UINT8 *)&gDeviceOldVer,3);
                sprintf((void *)aLogBuf,(void *)aRegexBuf,nVersion,gDeviceOldVer);
                TestLogStringFileWrite(aLogBuf,strlen((void *)aLogBuf));
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_LOOPBACK_CMD:/** 0xBA 版本回滚 */
        {
            UINT32 nRunVersion;
            UINT16 nValue;

            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[8]=S_R_SetConfirm;
            AP_Send(SendBuf,nLen);
            if((access("/mnt/flash/OAM/OAM_PRE",F_OK)!=-1))
            {
                nRunVersion = SYS_OAM_VERSION;
                E2promWrite((UINT8 *)&nRunVersion,DeviceSwOldVerAddr,3);
                system("mv /mnt/flash/OAM/OAM /mnt/flash/OAM/OAM_PRE_TEMP");
                system("mv /mnt/flash/OAM/OAM_PRE /mnt/flash/OAM/OAM");
                system("mv /mnt/flash/OAM/OAM_PRE_TEMP /mnt/flash/OAM/OAM_PRE");
                system("chmod 777 /mnt/flash/OAM/OAM");
                system("reboot -f");
            }
            break;
        }
        case POINT_LOAD_CMD:/**0xBB 点表导入 */
        {
            break;
        }
        case REPORT_PHONE_CMD://0xBC
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gPhone,11);
                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Phone Number",strlen("Get Phone Number"));
                TestLogStringFileWrite(gPhone,11);
                TestLogStringFileWrite((void *)"\n",1);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy((UINT8 *)&gPhone,(UINT8 *)&SendBuf[15],11);
                if((gPhone[0]==0x31)&&(gPhone[1]==0x33)&&(gPhone[2]==0x31))//PLC
                {
                    UINT8 nComNum = gPhone[6];
                    UINT8 nDeviceID;

                    nComNum -=0x30;
                    nDeviceID = (gPhone[7]-0x30)*10+(gPhone[8]-0x30);
                    pthread_mutex_lock(&Uartsem);
                    ComNormalWrite(nDeviceID,40015+gPhone[9]-0x30,gPhone[10]-0x30);
                    pthread_mutex_unlock(&Uartsem);
                }
                else
                    E2promWrite((UINT8 *)&SendBuf[15],DevicePhoneAddr,11);
                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Set Phone Number",strlen("Get Phone Number"));
                TestLogStringFileWrite(gPhone,11);
                TestLogStringFileWrite((void *)"\n",1);
            }
            break;
        }
        case MODEM_STATUS_CMD://0xBD
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Active)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_Confirm;
                /** 只有数采跟平台连接成功后，才返回1 */
                if(gSocketMode == 0 && g_nRemotesockfd != 0)// Ethernet Mode
                {
                    SendBuf[15]=1;
                }
                else if (gSocketMode != 0)
                {
                    SendBuf[15] = gModemNetStatus&0x0F;
                    gModemNetStatus = gModemNetStatus | 0x10;
                }else{
                    SendBuf[15] = 0;
                }
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_BASE_INFO_CMD:/** 0xC0 设备基本信息 */
        {
            UINT16 nValue;
            UINT32 nVersion;
            UINT8 routeflag;
            //InitialLoggerInf();
            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceName,20);
                memcpy((UINT8 *)&SendBuf[15+20],gMainDeviceSN,20);
                memcpy((UINT8 *)&SendBuf[15+40],gMainDeviceModel,20);
                memcpy((UINT8 *)&SendBuf[15+60],gMainDeviceType,20);
                memcpy((UINT8 *)&SendBuf[15+80],gMainDeviceIP,4);
                nVersion=SYS_OAM_VERSION;
                memcpy((UINT8 *)&SendBuf[15+85],(UINT8 *)&nVersion,3);
                SendBuf[15+84] = 0;
                TestLogStringFileWrite((void *)"Get Device Information\n",strlen("Get Device Information\n"));
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;

                memcpy(gMainDeviceName,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceName,DeviceNameAddr,20);

                memcpy(gMainDeviceSN,(UINT8 *)&SendBuf[15+20],20);
                E2promWrite(gMainDeviceSN,DeviceSNAddr,20);

                memcpy(gMainDeviceModel,(UINT8 *)&SendBuf[15+40],20);
                E2promWrite(gMainDeviceModel,DeviceModelAddr,20);

                memcpy(gMainDeviceType,(UINT8 *)&SendBuf[15+60],20);
                E2promWrite(gMainDeviceType,DeviceTypeAddr,20);

                TestLogStringFileWrite((void *)"Set Device Information\n",strlen("Set Device Information\n"));
                if((gMainDeviceIP[0]!=SendBuf[15+80])
                    ||(gMainDeviceIP[1]!=SendBuf[15+81])
                    ||(gMainDeviceIP[2]!=SendBuf[15+82])
                    ||(gMainDeviceIP[3]!=SendBuf[15+83]))
                {
                    //memcpy(gMainDeviceIP,(UINT8 *)&SendBuf[15+80],4);
                    E2promWrite(gMainDeviceIP,DeviceIP_E2P,4);
                    //gDeviceIPSetTempFlag=1;
                }
                gDeviceIPSetFlag=1;
                E2promWrite((UINT8 *)&gDeviceIPSetFlag,DeviceIPSetAddr,1);
                routeflag=1;
                gModuleChannel0InitFlag=0;
                E2promWrite((UINT8 *)&routeflag,RouteConfigAddr,1);
                gMainDeviceStatus=1;
                gDeviceStationBuild=0;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
                system("rm -rf /mnt/flash/OAM/record");
                system("mkdir /mnt/flash/OAM/record");
                if(nPointRecordFd!=0)
                {
                    fclose((void *)nPointRecordFd);
                    nPointRecordFd=0;
                }
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case STATION_INFO_CMD://0xC1
        {
            /*UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gStationID,32);
                memcpy((UINT8 *)&SendBuf[15+32],(UINT8 *)&gStationName,20);
                memcpy((UINT8 *)&SendBuf[15+52],(UINT8 *)&gStationLongitude,4);
                memcpy((UINT8 *)&SendBuf[15+56],(UINT8 *)&gStationLatitude,4);
                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite("Get Station Information\n",strlen("Get Station Information\n"));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;

                memcpy((UINT8 *)&gStationID,(UINT8 *)&SendBuf[15],32);
                E2promWrite((UINT8 *)&gStationID,StationIDAddr,32);

                memcpy((UINT8 *)&gStationName,(UINT8 *)&SendBuf[15+32],20);
                E2promWrite((UINT8 *)&gStationName,StationNameAddr,20);

                memcpy((UINT8 *)&gStationLongitude,(UINT8 *)&SendBuf[15+52],4);
                E2promWrite((UINT8 *)&gStationLongitude,StationLongitudeAddr,4);

                memcpy((UINT8 *)&gStationLatitude,(UINT8 *)&SendBuf[15+56],4);
                E2promWrite((UINT8 *)&gStationLatitude,StationLatitudeAddr,4);

                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite("Set Station Information\n",strlen("Set Station Information\n"));

                gMainDeviceConfig=0;
                E2promWrite((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
            }*/
            break;
        }
        case SERVER_INFO_CMD://0xC2
        {
            /*UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                E2promRead((UINT8 *)&SendBuf[15],StationIPAddr,4);

                E2promRead((UINT8 *)&SendBuf[15+4],StationPortAddr,2);
                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite("Get Server Information\n",strlen("Get Server Information\n"));
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 routeflag;
                UINT8 aDomainName[30],nTemp[30]="%d.%d.%d.%d";

                memset(aDomainName,0,sizeof(aDomainName));
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;

                E2promWrite((UINT8 *)&SendBuf[15],StationIPAddr,4);
                sprintf(aDomainName,nTemp,SendBuf[15],SendBuf[16],SendBuf[17],SendBuf[18]);
                E2promWrite(aDomainName,ServerDomainNameAddr,30);
                memcpy(gServerIP,(UINT8 *)&SendBuf[15],4);
                E2promWrite((UINT8 *)&SendBuf[15+4],StationPortAddr,2);
                E2promWrite((UINT8 *)&SendBuf[15+4],ServerDomainNamePortAddr,2);
                TestLogStringFileWrite("Set Server Information\n",strlen("Set Server Information\n"));
                if(gSocketMode==0)
                {
                    shutdown(g_nRemotesockfd,2);
                    close(g_nRemotesockfd);
                    SysNetInit();
                    g_nRemotesockfd=0;
                    TestLogTimeFileWrite();
                    TestLogStringFileWrite("Disconnect Server:Init Device IP\n",strlen("Disconnect Server:Init Device IP\n"));
                    //routeflag=1;
                    //E2promWrite((UINT8 *)&routeflag,RouteConfigAddr,1);
                }
                AP_Send(SendBuf,nLen);
                gMainDeviceConfig=1;
                E2promWrite((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
            }*/
            break;
        }
        case STATION_BUILD_CMD://0xC3
        {
             /*if(SendBuf[15]==0x01)
            {
                gMainDeviceStatus=2;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
                if(gSocketMode==0)
                {
                    if(g_nRemotesockfd==0)
                    {
                        gMainDeviceStatus=3;
                    }
                    else
                    {
                        SendDeviceInfoToServer();
                        SendSingleFrameToAPP(0xC3,0x008B,0x02);//connect success
                    }
                }
                else
                {
                    gMainDeviceStatus=3;
                    if(gModuleChannel0InitFlag>0)
                    {
                        SendDeviceInfoToServer();
                        SendSingleFrameToAPP(0xC3,0x008B,0x02);
                    }
                }
            }*/
            break;
        }
        case DOWNLINK_DEVICE_INFO_CMD:/** 0xC4 下联设备信息 */
        {
            UINT16 nValue;
            UINT8 aTemp[255],nLenTemp,nRes;

            if((sMessage.mASDU.nSendFlag & 0xFF) == S_R_Query)
            {
                nRes=GetDownLinkDeviceInfo(1,aTemp,&nLenTemp);
                if(nRes==0)
                {
                    SendBuf[1]=nLenTemp+13;
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_InfoReport;
                    memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&aTemp,nLenTemp);
                    sleep(1);
                    AP_Send(SendBuf,nLenTemp+15);
                    TestLogStringFileWrite((void *)"Report Downlink Device to APP\n",strlen("Report Downlink Device to APP\n"));
                }
                else
                {
                    SendBuf[1] = 14;
                    nValue = gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_InfoReportFinish;
                    SendBuf[15]=S_R_InfoReportFinish;
                    sleep(1);
                    AP_Send(SendBuf,16);
                    TestLogStringFileWrite((void *)"Report Downlink Device Finish\n",strlen("Report Downlink Device Finish\n"));
                }
            }
            else if((sMessage.mASDU.nSendFlag & 0xFF) == S_R_Set)
            {
                    UINT8 nLoopCount;
                    DbgPrintf("TTT Get C4 Set Command from APP with Flag: %X TTT\r\n", sMessage.mASDU.nSendFlag);
                    gMainDeviceStatus=DEVSTATUS_DEVINFO_GET;
                    nValue=gSendCount*2;
                    memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                    nValue=gRecvCount*2;
                    memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                    SendBuf[8]=S_R_SetConfirm;

                    nLoopCount=15;
                    /** 如果是从二级地址1开始设置，则清空之前的设备信息 */
                    /* #ifndef USE_NEW_C4_FORAPP
                    if(SendBuf[nLoopCount]==1)
                    {
                        for(k=1;k<=MAXDEVICE;k++)
                            gDeviceInfo[k].nInUse=0;
                        gConnectDeviceNum=0;
                    }
                    #else
                    if (sMessage.mASDU.nSendFlag == (S_R_Set | (0x01 << 8)))
                    {
                        for(k=1;k<=MAXDEVICE;k++)
                            gDeviceInfo[k].nInUse=0;
                        gConnectDeviceNum=0;
                    }
                    #endif*/

                    while(nLoopCount<nLen)
                    {
                        if(gConnectDeviceNumBuf<gConnectDeviceMaxNum)
                        {
                            gConnectDeviceNumBuf++;
                            gDeviceInfoBuf[SendBuf[nLoopCount]].nInUse=1;
                            memcpy(gDeviceInfoBuf[SendBuf[nLoopCount]].aESN,(UINT8 *)&SendBuf[nLoopCount+1],20);
                            memcpy((UINT8 *)&gDeviceInfoBuf[SendBuf[nLoopCount]].nPointTableNo,(UINT8 *)&SendBuf[nLoopCount+21],2);
                            gDeviceInfoBuf[SendBuf[nLoopCount]].nDownlinkPort=SendBuf[nLoopCount+23];
                            gDeviceInfoBuf[SendBuf[nLoopCount]].nProtocolType=SendBuf[nLoopCount+24];
                            nLoopCount+=25;
                        }
                        else
                        {
                            SendSingleFrameToAPP(0xC5,0x008C,0x07);//提示下联设备超出限制
                            break;
                        }
                    }
                    //TagBaseFileWrite();/** 把设备基本信息写入到base.xml文件中*/
                    AP_Send(SendBuf,nLen);
                    TestLogStringFileWrite((void *)"Downlink Device Set Confirm\n",strlen("Downlink Device Set Confirm\n"));
            }
            break;
        }
        case LOAD_POINT_TABLE_CMD:/** 0xC5 导表指令 */
        {
            UINT16 nValue;
            UINT8 aTemp[255],nLenTemp,nRes;

            gDLinkDeviceInfoCount=0;
            gDLinkDeviceCount=0;
            TestLogStringFileWrite((void *)"Report Downlink Device to Platform\n",strlen("Report Downlink Device to Platform\n"));
            E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
            DbgPrintf("Device Number=%d\r\n",gConnectDeviceNum);
            SendSFramePacketToAPP();
            gMainDeviceStatus=DEVSTATUS_DEVINFO_SET;
            nRes=GetDownLinkDeviceInfo(0,aTemp,&nLenTemp);
            SendBuf[1]=nLenTemp+13;
            nValue=gSendCount*2;
            memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
            nValue=gRecvCount*2;
            memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
            SendBuf[6]=DOWNLINK_DEVICE_INFO_CMD;
            SendBuf[8]=S_R_Set;
            //#ifndef USE_NEW_C4_SERVER
            SendBuf[9]=0x00;
            //#else
            //SendBuf[9]=0x01;
            //#endif
            //LedSet(3,0);
            //LedSet(4,0);
            //LedSet(5,0);
            memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&aTemp,nLenTemp);
            Socket_Send(SendBuf,nLenTemp+15);
            break;
        }
        case COMMUNICATION_MODE_CMD://0xC6
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_QueryConfirm;
                SendBuf[15]=gSocketMode;
                AP_Send(SendBuf,nLen);
                TestLogStringFileWrite((void *)"Get Socket Mode",strlen("Get Socket Mode"));
                TestLogStringFileWrite((void *)"\n",1);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                UINT8 nOldSocketMode=gSocketMode;

                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[7]=0x01;
                SendBuf[8]=S_R_SetConfirm;
                gSocketMode = SendBuf[15];
                E2promWrite((UINT8 *)&SendBuf[15],DeviceSocketModeAddr,1);
                AP_Send(SendBuf,nLen);
                if(nOldSocketMode!=gSocketMode)
                {
                    if(gSocketMode==1)
                    {
                        shutdown(g_nRemotesockfd,2);
                        g_nRemotesockfd = 0;
                    }
                }
                TestLogStringFileWrite((void *)"Set Socket Mode",strlen("Set Socket Mode"));
                TestLogStringFileWrite((void *)"\n",1);
            }
            break;
        }
        case SERVER_IP_CMD://            0xC8
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                E2promRead((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                E2promRead((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                E2promWrite((UINT8 *)&SendBuf[15],ServerDomainNameAddr,30);
                E2promWrite((UINT8 *)&SendBuf[15+30],ServerDomainNamePortAddr,2);
                gMainDeviceConfig=1;
                E2promWrite((UINT8 *)&gMainDeviceConfig,DeviceConfigAddr,1);
                GetDomainNameIP((UINT8 *)&SendBuf[15]);
                /*Close Connect*/
                AP_Send(SendBuf,nLen);
                if(gSocketMode==0)
                {
                    shutdown(g_nRemotesockfd,2);
                    close(g_nRemotesockfd);
                    g_nRemotesockfd=0;
                    SysNetInit();
                }
                else
                {
                    gModuleChannel0InitFlag=0;
                }
            }
            break;
        }
        case ESN_CMD://                  0xD0
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceSN,20);
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceSN,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceSN,DeviceSNAddr,20);
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_TYPE_CMD://          0xD1
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceType,20);
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceType,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceType,DeviceTypeAddr,20);
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_MODEL_CMD://         0xD2
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],gMainDeviceModel,20);
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceModel,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceModel,DeviceModelAddr,20);
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_NAME_CMD://          0xD3
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                memcpy((UINT8 *)&SendBuf[15],(UINT8 *)&gMainDeviceName,20);
                AP_Send(SendBuf,nLen);
            }
            else if(sMessage.mASDU.nSendFlag==S_R_Set)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_SetConfirm;
                memcpy(gMainDeviceName,(UINT8 *)&SendBuf[15],20);
                E2promWrite(gMainDeviceName,DeviceNameAddr,20);
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_MAXNUMBER_CMD:
        {
            UINT16 nValue;

            if(sMessage.mASDU.nSendFlag==S_R_Query)
            {
                nValue=gSendCount*2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue=gRecvCount*2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8]=S_R_QueryConfirm;
                SendBuf[15]=gConnectDeviceMaxNum;
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_RESET_CMD:
        {
            UINT8 nAlarmDeviceCount,nAlarmPointCount;

            if((SendBuf[15] & 0xF0) == 0 && SendBuf[8] != 0x08)
            {
                gMainDeviceStatus = 0;
                E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
                gConnectDeviceNum = 0;
                E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);
                memset(gDeviceInfo, 0, sizeof(gDeviceInfo));
                gDLinkDeviceInfoCount=0;
                SendBuf[8] = SendBuf[8]+1;
                AP_Send(SendBuf,nLen);
                memset((UINT8 *)&aAlarmInfo,0,sizeof(aAlarmInfo));
                for(nAlarmDeviceCount=0;nAlarmDeviceCount<40;nAlarmDeviceCount++)
                {
                    for(nAlarmPointCount=0;nAlarmPointCount<20;nAlarmPointCount++)
                    {
                        E2promWrite((UINT8 *)&aAlarmInfo[nAlarmDeviceCount][nAlarmPointCount],AlarmInfoAddr+(nAlarmDeviceCount*20+nAlarmPointCount)*2,2);
                    }
                }
                //close slave channel
                gModemSlaveConfigFlag=0;
                E2promWrite((UINT8 *)&gModemSlaveConfigFlag, SlaveNetInfoConfigAddr, 1);
                sleep(2);
                system("reboot -f");
            }
            else
            {
                SendBuf[8] = SendBuf[8]+1;
                AP_Send(SendBuf,nLen);
            }
            break;
        }
        case DEVICE_REBOOT_CMD:
        {
            if (SendBuf[15] == 0x81)
            {/**预置*/
                UINT16 nValue = gSendCount * 2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue = gRecvCount * 2;
                memcpy(&SendBuf[4],(UINT8 *)&nValue,2);
                SendBuf[8] = 0x07;
                Socket_Send(SendBuf, nLen);
            }
            else if (SendBuf[15] == 0x01)
            {/** 设置 */
                UINT16 nValue = gSendCount * 2;
                memcpy(&SendBuf[2],(UINT8 *)&nValue,2);
                nValue = gRecvCount * 2;
                memcpy(&SendBuf[4], (UINT8 *)&nValue, 2);
                SendBuf[8]  = 0x07;

                AP_Send(SendBuf, nLen);

                sleep(3);
                system("reboot -f");
            }
        }break;
    }
    return nRes;
}

/*****************************************************************************
* Description: instead  by SouthQuery below
* Parameters:
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void SetYT(UINT8 *Msg,UINT8 DeviceSdSum)
{
    DbgPrintf("[YT] Handle YT SET TASK!!! \r\n"); //根据 设备 ID、MB地址及浮点数设置对应YT命令到对应逆变器

    UINT16 uMbAddr,u104Addr,nValue,uCrc;
	UINT8 uSdMBCount,uSdCount,uYtSum,i,uDeviceID;
	UINT8 aRecvBuf[256];
	UINT8 uModbusCmd[13];
	UINT8 nValueTemp[2];
	UINT8 uSdSum=0;

	union U32_F_Char_Convert U32_F_Char;
	struct sTypeGroup *gTypeGroupPoint=NULL;
	struct sTypeParam *gTypeParamPoint=NULL;
    UINT8 uDeviceCount;
    UINT16 uDevice104Addr;

	uDevice104Addr=Msg[12]|Msg[13]<<8;
	for(uDeviceCount=0;uDeviceCount<MAXDEVICE;uDeviceCount++)
	{
		 if(gDeviceInfo[uDeviceCount].nInUse==1)
         {
              if((gDeviceInfo[uDeviceCount].nSDAddr <= uDevice104Addr) && (uDevice104Addr < (gDeviceInfo[uDeviceCount].nSDAddr+DeviceSdSum)))
              {
				  uDeviceID=uDeviceCount;
				  break;
			  }
		 }
	}
    DbgPrintf("uDeviceID:%d\r\n",uDeviceID);
	//uDeviceID=Msg[10];//设备地址

	gTypeGroupPoint = gTypeHead;

	 while(gTypeGroupPoint != NULL)
    {
        if(gTypeGroupPoint->nTypeID != gDeviceInfo[uDeviceID].nType)
        {
			gTypeGroupPoint = gTypeGroupPoint->pNext;
		}
        else
		{
			break;
		}
    }
    if(gTypeGroupPoint == NULL)
        return;

    if(gTypeGroupPoint->nProtocalTypeID != Type_Huawei_Modbus)
    {
		return;
    }
	gTypeParamPoint = gTypeGroupPoint->pParamNext;

   	while(gTypeParamPoint != NULL)
	{
		if(gTypeParamPoint->nType == Type_104_SD)
		{
			 uSdSum++;
			 gTypeParamPoint = gTypeParamPoint->pNext;
		}
		else
		{
			gTypeParamPoint = gTypeParamPoint->pNext;
		}
	}

    DbgPrintf("SdSum:%d\r\n",uSdSum);

	uYtSum=Msg[7] & 0x7f;
	for(i=0;i<uYtSum;i++)
	{
		uSdMBCount=0;

		u104Addr = (UINT16*)((Msg[13+i*7]<<8) | Msg[12+i*7]);
        uSdCount = (u104Addr-0x6201)%uSdSum;

		gTypeGroupPoint = gTypeHead;
		gTypeParamPoint = gTypeGroupPoint->pParamNext;
		while(gTypeParamPoint != NULL)
		{
			if(gTypeParamPoint->nType == Type_104_SD)
			{
                if(uSdCount == uSdMBCount)
                {
					uMbAddr=gTypeParamPoint->nMBAddr;

					if(gTypeParamPoint->nLen==2)
                    {
						U32_F_Char.c[0]=Msg[15+i*7];
						U32_F_Char.c[1]=Msg[16+i*7];
						U32_F_Char.c[2]=Msg[17+i*7];
						U32_F_Char.c[3]=Msg[18+i*7];

						U32_F_Char.u = U32_F_Char.f;

						uModbusCmd[0]=uDeviceID;
						uModbusCmd[1]=0x10;
						uModbusCmd[2]=uMbAddr>>8;
						uModbusCmd[3]=uMbAddr&0xFF;
						uModbusCmd[4]=0x00;
						uModbusCmd[5]=0x02;
						uModbusCmd[6]=0x04;
						uModbusCmd[7]=U32_F_Char.c[3];
						uModbusCmd[8]=U32_F_Char.c[2];
						uModbusCmd[9]=U32_F_Char.c[1];
						uModbusCmd[10]=U32_F_Char.c[0];
						uCrc = CRC16(&uModbusCmd[0],11);
						memcpy((UINT8 *)&uModbusCmd[11],(UINT8 *)&uCrc,2);

						SouthCmdTask(uModbusCmd,13,aRecvBuf,gDeviceInfo[uDeviceID].nDownlinkPort);
						DbgPrintf("########DeviceID:%d MbAddr: %d Value: %08X #########\r\n",uDeviceID,uMbAddr,U32_F_Char.u);
					}
					else
					{
						U32_F_Char.c[3]=Msg[18+i*7];
						U32_F_Char.c[2]=Msg[17+i*7];
						U32_F_Char.c[1]=Msg[16+i*7];
						U32_F_Char.c[0]=Msg[15+i*7];

						U32_F_Char.u=U32_F_Char.f;

						uModbusCmd[0]=uDeviceID;
						uModbusCmd[1]=0x06;
						uModbusCmd[2]=uMbAddr>>8;
						uModbusCmd[3]=uMbAddr&0xFF;

						nValueTemp[0]=((UINT16)(U32_F_Char.u))>>8;
						nValueTemp[1]=((UINT16)(U32_F_Char.u))&0xFF;
						memcpy(&uModbusCmd[4],(UINT8 *)&nValueTemp,2);
						uCrc = CRC16(&uModbusCmd[0],6);
						memcpy((UINT8 *)&uModbusCmd[6],(UINT8 *)&uCrc,2);
						SouthCmdTask(uModbusCmd,8,aRecvBuf,gDeviceInfo[uDeviceID].nDownlinkPort);
						DbgPrintf("########DeviceID:%d MbAddr: %d Value: %d #########\r\n",uDeviceID,uMbAddr,(UINT16)(U32_F_Char.u));
					}

					break;
				}
				gTypeParamPoint = gTypeParamPoint->pNext;
				uSdMBCount++;
			}
			else
			{
				gTypeParamPoint = gTypeParamPoint->pNext;
			}
		}

	}

	nValue=gSendCount*2;
    memcpy(&Msg[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&Msg[4],(UINT8 *)&nValue,2);
    Msg[8]=S_R_SetConfirm;
    Socket_Send(Msg, Msg[1]+2);
}
/*****************************************************************************
* Description: YT Parameter inquire
* Parameters:
* Returns: none
* Staff&Date: Xu Zongxiang 2019.5.15
*****************************************************************************/
void InquireYT(UINT8 *Msg)
{
    DbgPrintf("[YT] Handle YT INQUIRE TASK!!!\r\n");//根据 设备 ID、MB地址及浮点数设置对应YT命令到对应逆变器

    UINT16 nValue = 1;
    UINT8 uDeviceID = 0,uYtDotSum,dYtregisterSum = 0,Valid_YtDotSum = 0;
	UINT8 SendMessage[SYS_FRAME_LEN];
	UINT8 i = 1,j = 0;
	struct sTypeGroup *gTypeGroupPoint=NULL;
	struct sTypeParam *gTypeParamPoint=NULL;
    UINT16 u104Addr;
    dYTPiontCount = 0;

    uYtDotSum = Msg[7] - 0x80;
	u104Addr = Msg[12]|(Msg[13]<<8);
	memcpy(SendMessage,Msg,Msg[1]+2);

	struct sTypeParam *uYtDotpoint = (struct sTypeParam *)malloc(uYtDotSum*sizeof(struct sTypeParam));
	if(uYtDotpoint == NULL)
	{
		return ;
	}

	/**********************根据104地址找到所有的Modbus地址****************************/
	for(i = 0;i < uYtDotSum;i++)
	{
		gTypeParamPoint = GetDeviceID(u104Addr++,&uDeviceID);
		if(gTypeParamPoint == NULL)
			return ;
		memcpy(&uYtDotpoint[Valid_YtDotSum].nMBAddr,gTypeParamPoint,sizeof(struct sTypeParam));

		if(uYtDotpoint[i].nType == Type_104_SD)
			Valid_YtDotSum++;
	}

	/*********************************冒泡**************************************/
	for (j = 0; j < Valid_YtDotSum - 1; j++)
	{
		for (i = 0; i < Valid_YtDotSum - 1 - j; i++)
		{
			if(uYtDotpoint[i].nMBAddr > uYtDotpoint[i+1].nMBAddr)
			{
				memcpy(gTypeParamPoint,&uYtDotpoint[i].nMBAddr,sizeof(struct sTypeParam));
				memcpy(&uYtDotpoint[i].nMBAddr,&uYtDotpoint[i+1].nMBAddr,sizeof(struct sTypeParam));
				memcpy(&uYtDotpoint[i+1].nMBAddr,gTypeParamPoint,sizeof(struct sTypeParam));
			}
		}
	}

	/********************************查询分段*************************************/
	for(i = 0;i < Valid_YtDotSum;i++)
	{
		dYtregisterSum += uYtDotpoint[i].nLen;
		if((i == (Valid_YtDotSum-1)) || ((uYtDotpoint[i].nMBAddr+uYtDotpoint[i].nLen) != uYtDotpoint[i+1].nMBAddr))
		{
//			DbgPrintf("处理地址：%d - nValue:%d\r\n",i,nValue);
			SouthParamQuery(&uYtDotpoint[i - (nValue - 1)],uDeviceID,dYtregisterSum,SendMessage,((i == (uYtDotSum-1))?1:0));
			nValue = 0;
			dYtregisterSum = 0;
		}
		nValue++;
	}

	/******************************包发送***************************************/
	nValue=gSendCount*2;
	memcpy(&SendMessage[2],(UINT8 *)&nValue,2);
	nValue=gRecvCount*2;
	memcpy(&SendMessage[4],(UINT8 *)&nValue,2);
	SendMessage[8]=S_R_QueryConfirm;
	Socket_Send(SendMessage, Msg[1]+2);
}

/*****************************SouthRecord_SendtoPlatform***********************/
void SouthParamQuery(struct sTypeParam *uYtDotpoint,UINT8 uDeviceID,UINT8 dYtregisterSum,
		UINT8 *SendMessage,UINT8 Flag)
{
	UINT8 i = 0,nCount = 0;
	UINT8 uModbusCmd[8],aRecvBuf[256];
	UINT16 uMbAddr,uCrc;
	UINT32 nValueTemp;
	union uIEEE754 uIE;

	uMbAddr = uYtDotpoint->nMBAddr;
	uModbusCmd[0]=uDeviceID;
	uModbusCmd[1]=0x03;
	uModbusCmd[2]=uMbAddr>>8;
	uModbusCmd[3]=uMbAddr&0xFF;
	uModbusCmd[4]=0x00;
	uModbusCmd[5]=dYtregisterSum;					//register sum
	uCrc = CRC16(uModbusCmd,6);
	uModbusCmd[6]=uCrc&0xFF;
	uModbusCmd[7]=uCrc>>8;
	SouthCmdTask(uModbusCmd,8,aRecvBuf,gDeviceInfo[uDeviceID].nDownlinkPort);

	while(dYtregisterSum)
	{
		if((uYtDotpoint->nLen)== 2)		//表计点表时间数据类型是UINT32,华为是EPOCHTIME
		{
			dYtregisterSum -= 2;
			nValueTemp=0;
			for(i=0;i<2;i++)
			{
			   nValueTemp=nValueTemp+(aRecvBuf[3+nCount]<<(8*((2-i)*2-1)))
									+(aRecvBuf[4+nCount]<<(8*((2-i)*2-2)));
			   nCount=nCount+2;
			}
			uIE.nIndex=nValueTemp;
//			DbgPrintf("处理地址：%d - Data:%X - DataLength:%d\r\n",uYtDotpoint->nMBAddr,nValueTemp,uYtDotpoint->nLen);
			memcpy((UINT8 *)&SendMessage[15+dYTPiontCount*4],uIE.nChar,4);
			dYTPiontCount++;
		}
		else
		{
			dYtregisterSum -= 1;
			nValueTemp=0;
			nValueTemp=nValueTemp+(aRecvBuf[3+nCount]<<8)+(aRecvBuf[4+nCount]);
			nCount=nCount+2;
			uIE.nIndex=nValueTemp;
//			DbgPrintf("处理地址：%d - Data:%X - DataLength:%d\r\n",uYtDotpoint->nMBAddr,nValueTemp,uYtDotpoint->nLen);
			memcpy((UINT8 *)&SendMessage[15+dYTPiontCount*4],uIE.nChar,4);
			dYTPiontCount++;
		}
		uYtDotpoint = uYtDotpoint->pNext;
	}
	if(Flag)
		dYTPiontCount = 0;
	return ;
}

/*****************************************************************************
* Description: instead  by SouthQuery below
* Parameters:
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void SouthCmdTask(UINT8 *aSendBuf,UINT8 aSendLen, UINT8 *aRecvBuf,UINT8 uDeviceId)
{
    UINT8 nErrorCount = 0;
    int nRecvLen;

    do
    {
        int i;
        UINT16 nCRC,nFPGAValue,k;
        UINT16 nPreValueTemp=0;

        nRecvLen = -1;
        pthread_mutex_lock(&Uartsem);
        if(nUartFd == 0)
        {
            nUartFd = UartOper(2,9600);
        }
       nPreValueTemp=FpgaRead(3);
        do
        {
            if(uDeviceId!=1)
            {
                nFPGAValue = 0;
                LedSet(4,1);
            }
            else
            {
                nFPGAValue = 1;
                LedSet(3,1);
            }
            FpgaWrite(3,nFPGAValue);
        }while((FpgaRead(0x03))!=nFPGAValue);

        memset(aRecvBuf,0,sizeof(aRecvBuf));
        GPIOSet(2,19,1);		/*南向COM处于发送状态*/
        DbgPrintf("COM.%d Send:",2-nFPGAValue);
        for(k=0;k<aSendLen;k++)
        {
        	DbgPrintf("%02X ",aSendBuf[k]);
        }
        DbgPrintf("\r\n");

        writeDev(nUartFd,aSendBuf,aSendLen);
        if(aSendBuf[0]!=0xFF && aSendBuf[1]==0x1C)
        {
            usleep(500000);
        }
        GPIOSet(2,19,0);		/*南向COM处于接收状态*/

        if(dSouth_Record_Information)
        	usleep(500000);
        else
        	usleep(500);

        nRecvLen=readDev(nUartFd,aRecvBuf);

        do
        {
            FpgaWrite(0x03,nPreValueTemp);
        }
        while((FpgaRead(0x03))!=nPreValueTemp);
        pthread_mutex_unlock(&Uartsem);
        usleep(10);
        GPIOSet(2,19,1);

    }while((aSendBuf[0]!=0xFF) && (nRecvLen == 0) && ((nErrorCount++)<3));
}

/*****************************************************************************
* Description: Handle  south device Query request from platform
* Parameters:*Msg   Query Information
             uLen   length of query information
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void SouthQueryTask(UINT8 *Msg,UINT8 uLen)
{
    DbgPrintf("[Register-Query]Send Modbus Command");
    UINT8 uModbusCmd[8]={0xF9,0x03,0x00,0x00,0x00,0x01,0x00,0x00};
    UINT8 uDeviceID;
    UINT8 uMbAddr[2];
    //UINT8 uValue[2];
    UINT8 uTemp;
    UINT8 uRegisterNum=(uLen-15)/4;
    UINT8 uCount;
    UINT8 i;
    UINT8 nDownlinkPort;
    int nRecvLen;
    UINT16 nCRC,nFPGAValue,k;
    UINT8 aRecvBuf[256];
    uDeviceID=Msg[12];//设备ID
    if(uDeviceID!=250)
    {
        uModbusCmd[0]=uDeviceID;
    }
    //Msg[15]开始为寄存器地址
    for(uCount=0;uCount<uRegisterNum;uCount++)
    {
        memcpy(uMbAddr,&Msg[15+uCount*4],2);
        //memcpy(uValue,&Msg[17+uCount*4],2);
        uModbusCmd[2]=uMbAddr[1];
        uModbusCmd[3]=uMbAddr[0];
        //memcpy(&uModbusCmd[4],uValue,2);
        nCRC=CRC16(uModbusCmd,6);
        uModbusCmd[6] = (UINT8)nCRC;
        uModbusCmd[7] = (UINT8)(nCRC>>8);
        nRecvLen = -1;

        pthread_mutex_lock(&Uartsem);
       do
        {
            if(uDeviceID<42)
            {
                nDownlinkPort=gDeviceInfo[uDeviceID].nDownlinkPort;
            }
            else
            {
                nDownlinkPort=uDeviceID-249+1;
            }
            if(gDeviceInfo[uDeviceID].nDownlinkPort==1)
            {
                nFPGAValue = 1;
                LedSet(4,1);
            }
            else
            {
                nFPGAValue = 2;
                LedSet(3,1);
            }
            FpgaWrite(3,nFPGAValue);
        }while((FpgaRead(0x03))!=nFPGAValue);
        memset(aRecvBuf,0,sizeof(aRecvBuf));
        GPIOSet(2,19,1);/*南向COM处于发送状态*/
        usleep(1000);
        DbgPrintf("[Register]COM.%d Send:",nFPGAValue);
        for(k=0;k<8;k++)
        {
            DbgPrintf("%02X ",uModbusCmd[k]);
        }
        DbgPrintf("\r\n");
        writeDev(nUartFd,uModbusCmd,8);
        //usleep(100);
        GPIOSet(2,19,0);
        nRecvLen=readDevEsn(nUartFd,aRecvBuf);
        DbgPrintf("[Register]Recieve : ");
        for(i=0;i<255;i++)
        {
            DbgPrintf("%02X",aRecvBuf[i]);
}
        DbgPrintf("\r\n");
        uTemp=aRecvBuf[3];
        aRecvBuf[3]=aRecvBuf[4];
        aRecvBuf[4]=uTemp;
        memcpy(&Msg[17+(uCount*4)],&aRecvBuf[3],2);
        usleep(1000);
        pthread_mutex_unlock(&Uartsem);
    }


    /**回包 不影响操作**/
    Msg[8]=Msg[8]+1;
    DbgPrintf("[Register-Query]Send Retrospect to Platform");
    Socket_Send(Msg, uLen);
}

/*****************************************************************************
* Description: Handle  south device Set request from platform
* Parameters:*Msg   Set Information
             uLen   length of set information
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void SouthSetTask(UINT8 *Msg,UINT8 uLen)
{
    DbgPrintf("[Register-Set]Send Modbus Command");
    UINT8 uModbusCmd[8]={0xF9,0x06,0x00,0x00,0x00,0x00,0x00,0x00};
    UINT8 uModbusRecieve[8]={0};
    UINT8 uDeviceID;
    UINT8 uMbAddr[2];
    UINT8 uValue[2];
    UINT8 uTemp;
    UINT8 uRegisterNum=(uLen-15)/4;
    UINT8 uCount;
    UINT8 i;
    UINT8 nDownlinkPort;
    int nRecvLen;
    UINT16 nCRC,nFPGAValue,k;
    UINT8 aRecvBuf[256];
    uDeviceID=Msg[12];//设备ID
    if(uDeviceID!=250)
    {
        uModbusCmd[0]=uDeviceID;
    }
    //Msg[15]开始为寄存器地址
    for(uCount=0;uCount<uRegisterNum;uCount++)
    {
        memcpy(uMbAddr,&Msg[15+uCount*4],2);
        memcpy(uValue,&Msg[17+uCount*4],2);
        uTemp=uValue[0];
        uValue[0]=uValue[1];
        uValue[1]=uTemp;
        uModbusCmd[2]=uMbAddr[1];
        uModbusCmd[3]=uMbAddr[0];
        memcpy(&uModbusCmd[4],uValue,2);
        nCRC=CRC16(uModbusCmd,6);
        uModbusCmd[6] = (UINT8)nCRC;
        uModbusCmd[7] = (UINT8)(nCRC>>8);
        nRecvLen = -1;
        pthread_mutex_lock(&Uartsem);
       do
        {
            if(uDeviceID<42)
            {
                nDownlinkPort=gDeviceInfo[uDeviceID].nDownlinkPort;
            }
            else
            {
                nDownlinkPort=uDeviceID-249+1;
            }
            if(nDownlinkPort==1)
            {
                nFPGAValue = 1;
                LedSet(4,1);
            }
            else
            {
                nFPGAValue = 2;
                LedSet(3,1);
            }
            FpgaWrite(3,nFPGAValue);
        }while((FpgaRead(0x03))!=nFPGAValue);
        memset(aRecvBuf,0,sizeof(aRecvBuf));
        GPIOSet(2,19,1);/*南向COM处于发送状态*/
        usleep(1000);
        DbgPrintf("[Register]COM.%d Send:",nFPGAValue);
        for(k=0;k<8;k++)
        {
            DbgPrintf("%02X ",uModbusCmd[k]);
        }
        DbgPrintf("\r\n");
        writeDev(nUartFd,uModbusCmd,8);
        //usleep(100);
        GPIOSet(2,19,0);
        nRecvLen=readDevEsn(nUartFd,aRecvBuf);
        DbgPrintf("[Register]Recieve : ");
        for(i=0;i<255;i++)
        {
            DbgPrintf("%02X",aRecvBuf[i]);
        }
        DbgPrintf("\r\n");
        usleep(1000);
        pthread_mutex_unlock(&Uartsem);
        memcpy(uModbusRecieve,&aRecvBuf[0],8);
        if(memcmp(uModbusCmd,uModbusRecieve,8)!=0)
        {

            Msg[8]=0xee;
            DbgPrintf("[YT]Failed Setting,Send Reason is %d\r\n",Msg[8]);
            break;
        }
    }
    /**回包 不影响操作**/
    if(Msg[8]!=0xee)
    {
        Msg[8]=Msg[8]+1;
    }
    DbgPrintf("[Register-Set]Send Retrospect to Platform");
    Socket_Send(Msg, uLen);
}

/*****************************************************************************
* Description: reborn Logger Information
* Parameters:none
* Returns: none
* Staff&Date: Liujing 2018.5.22
*****************************************************************************/
void InitialLoggerInf()
{
    DbgPrintf("[iClean]InitialLoggerInf\r\n");
    memset(gMainDeviceName,0,sizeof(gMainDeviceName));
    E2promRead((UINT8 *)&gMainDeviceName,DeviceNameAddr,20);

    memset(gMainDeviceSN,0,sizeof(gMainDeviceSN));
    E2promRead((UINT8 *)&gMainDeviceSN,DeviceSNAddr,20);

    memset(gMainDeviceModel,0,sizeof(gMainDeviceModel));
    E2promRead((UINT8 *)&gMainDeviceModel,DeviceModelAddr,20);

    memset(gMainDeviceType,0,sizeof(gMainDeviceType));
    E2promRead((UINT8 *)&gMainDeviceType,DeviceTypeAddr,20);

    E2promRead((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
    if(gMainDeviceStatus>DEVSTATUS_NORMAL_WORKMODE)
        gMainDeviceStatus=DEVSTATUS_NEW_DEVICE;
    if(gMainDeviceStatus==1)
        gMainDeviceStatus=DEVSTATUS_NORMAL_WORKMODE;
    E2promRead((UINT8 *)&gMainDeviceIP,DeviceIP_E2P,4);
    if(gMainDeviceIP[0]==255)
    {
        gMainDeviceIP[0]=192;
        gMainDeviceIP[1]=168;
        gMainDeviceIP[2]=1;
        gMainDeviceIP[3]=176;
    }
}

/*****************************************************************************
* Description:      send SofeVersion to Platform
* Parameters:        nCmdID:frame command ID
                     nFlag:send reason
                     nValue:send value buffer
                     nLen:send value buffer length
* Returns:

SendFrameToPlatform(i,UPLOAD_HW_DEVICE_CMD,S_R_InfoReport,(UINT8 *)&aDownLinkDevice[i],uDownLinkDeviceLen);

*****************************************************************************/
void SendSofeVersionToPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen)
{
    UINT8 aBuf[256];
    UINT16 nPacketValue;

    memset(aBuf,0,sizeof(aBuf));
    aBuf[0]=0x68;
    aBuf[1]=nLen+16;
    nPacketValue=gSendCount*2;
    memcpy((UINT8 *)&aBuf[2],(UINT8 *)&nPacketValue,2);
    nPacketValue=gRecvCount*2;
    memcpy((UINT8 *)&aBuf[4],(UINT8 *)&nPacketValue,2);
    memcpy((UINT8 *)&aBuf[6],(UINT8 *)&nCmdID,1);//C7
    aBuf[7]=0x01;
    memcpy((UINT8 *)&aBuf[8],(UINT8 *)&nFlag,2);//92 00
    //gDeviceID=1;//Test
    memcpy((UINT8 *)&aBuf[10],(UINT8 *)&gMainDeviceID,2);//01 00
    aBuf[12]=0;
    aBuf[13]=0;
    aBuf[14]=0;
    if((nDeviceID>0)&&(nDeviceID<MAXDEVICE))
        aBuf[15]=nDeviceID;
    aBuf[16]=gDeviceInfo[nDeviceID].nDownlinkPort;//port
    aBuf[17]=nLen;
    memcpy((UINT8 *)&aBuf[18],aValue,nLen);
    Socket_Send(aBuf,nLen+15+3);
}

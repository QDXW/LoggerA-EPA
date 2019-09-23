/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : Module.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <sys/socket.h>
#include "Module.h"
#include "../interface/uartExtern.h"
#include "../EnetPortPrs/Port_FunExtern.h"
#include "../interface/exporthelper.h"
#include "../SGCC/store.h"

#define  MI_START            7
#define  MI_BOOT             8
#define  MI_AT               9            // 发送AT
#define  MI_NO_ECHO          10           // 关回显
#define  MI_SOFTWARE_VERSION 11           // 显示软件版本号

#define  MI_GET_CSQ          12           // 获取信号强度
#define  MI_FIND_NETWORK     13           // 查询网络状态
#define  MI_REGISTER         14           // 注册网络
#define  MI_DEACT_PDP        15           // 关闭PDP场景
#define  MI_ACT_PDP          16           // 激活PDP场景
#define  MI_TLS1             17           // 以下为TLS相关
#define  MI_TLS2             18
#define  MI_TLS3             19
#define  MI_TLS4             20
#define  MI_TLS5             21
#define  MI_TLS6             22
#define  MI_TLS7             23

#define  MI_CONFIG_GSM       30           // 配置网络为GSM（需重新找网）
#define  MI_LOCATION         31           // 获取基站定位（需在GSM网络下进行）
#define  MI_CONFIG_AUTO      32           // 配置网络为自动选网（需重新找网）
#define  MI_CLOSE_SERVER     33           // 关闭socket连接
#define  MI_OPEN_SERVER      34           // 建立socket连接

UINT16 gLocationReport=0;                 // 0: not report yet  others: already reported
int nModemFd=-1;

extern UINT8 gModemNetStatus;             //Net Status bit0 0:Normal 1:Connect Success  bit4 0:No query 1:Must Report to APP
extern UINT8 gLocationInfo[30];
extern UINT8 gSocketMode;                 //Socket module 0:ethernet 1:modem mode
extern UINT8 gMainDeviceStatus;           //Logger Init Flag   1:init success 0:null
extern UINT16 gMainDeviceID;              //logger device ID
extern UINT8 gConnectDeviceMaxNum;        //最大连接设备数
extern UINT16 gRecvCount;                 //recv packet count
extern UINT16 gSendCount;                 //send packet count
extern UINT8  gUpdataModeFlag;            //0:normal 1:in update mode
extern int g_nRemotesockfd;               //Socket Client fd
extern UINT8 gModemSlaveChannelSwitch;

extern void ReadAlarmFile();

/*****************************************************************************
* Function     : ModulePowerSet()
* Description  : 配置AP和3G模块的电源开关
* Input        : nStatus 0:work mode 1:shut down
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月12日        Andre
*****************************************************************************/
void ModulePowerSet(UINT16 nStatus)
{
    GPIOSet(1,30,nStatus);
}

/*****************************************************************************
* Function     : ModelEnable()
* Description  : 复位无线模块
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月12日        Andre
*****************************************************************************/
void ModelEnable()
{
    GPIOSet(1,20,1);
    usleep(100000);
    GPIOSet(1,20,0);
}

/*****************************************************************************
* Function     : ModelRFEnable()
* Description  : 配置3G模块射频开关
* Input        : nStatus 1:work mode 0:shut down
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月12日        Andre
*****************************************************************************/
void ModelRFEnable(UINT16 nStatus)
{
    GPIOSet(1,22,nStatus);
}

/*****************************************************************************
* Function     : ModuleInit()
* Description  : 3G/4G模块的初始化
* Input        : uStep: initial from this step
                 uChannel: channel id
                 uEncryptionMode: 0:no encryption 1:SSL3.0  2:TLS1.0  3:TLS1.1
                                  4:TLS1.2
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月12日        Andre
*****************************************************************************/
UINT8 ModuleInit(UINT8 uStep,UINT8 uChannel,UINT8 uEncryptionMode)
{
    UINT8 aBuf[512],nTemp=27,finish_flag=0;
    int rnum;
    UINT8 nErrorCount=0,nResetCount=10;

    while(1)
    {
        switch(uStep)
        {
            case MI_START:
                LedSet(2,0);
                LedSet(1,0);
                gModemNetStatus=gModemNetStatus & 0xF0;

            case MI_BOOT:
                if(nResetCount>5)
                {
                    ModulePowerSet(1);
                    sleep(10);
                    ModulePowerSet(0);
                    sleep(3);
                    nResetCount=0;
                }
                nErrorCount=0;
                ModelEnable(0);
                sleep(5);
                ModelRFEnable(1);
                write(nModemFd,&nTemp,1);
                sleep(1);

            case MI_AT:
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT\r\n",4);
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_NO_ECHO:
                memset(aBuf,0,sizeof(aBuf));
                writeModem((void *)"ATE0\r\n",6);
                rnum = readModem(aBuf);
                sleep(1);

            case MI_SOFTWARE_VERSION:

                memset(aBuf,0,sizeof(aBuf));    //request software revision
                writeModem((void *)"AT+GMR\r\n",8);
                rnum = readModem(aBuf);
                sleep(1);

            case MI_GET_CSQ:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+CSQ\r\n",8);
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_FIND_NETWORK:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+CGREG?\r\n",11);
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while((strstr((void *)aBuf,"0,0") != NULL) && (strstr((void *)aBuf,"OK") == NULL));
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_REGISTER:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                writeModem((void *)"AT+QICSGP=1\r\n",13);
                rnum = readModem(aBuf);
                if(strstr((void *)aBuf,"+QICSGP: 1")==NULL)
                {
                    writeModem((void *)"AT+QICSGP=1,1,\"UNINET\",\"\",\"\",1\r\n",32);
                    do{
                        rnum = readModem(aBuf);
                        sleep(3);
                        if(strstr((void *)aBuf,"OK")==NULL)
                        {
                            writeModem((void *)"AT+QICSGP=1\r\n",13);
                            rnum = readModem(aBuf);
                            sleep(1);
                        }
                        if((nErrorCount++)>=10)
                            break;
                    }while(strstr((void *)aBuf,"OK")==NULL);
                }
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }
                DbgPrintf("Modem register finish\r\n");

            case MI_DEACT_PDP:
                memset(aBuf,0,sizeof(aBuf));
                nErrorCount=0;
                do{
                    writeModem((void *)"AT+QIDEACT=1\r\n",14);
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=30)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=30)
                {
                    nResetCount++;
                    continue;
                }
                sleep(5);
                DbgPrintf("Modem deact finish\r\n");

            case MI_ACT_PDP:
                memset(aBuf,0,sizeof(aBuf));
                nErrorCount=0;
                writeModem((void *)"AT+QIACT?\r\n",11);
                rnum = readModem(aBuf);
                if(strstr((void *)aBuf,"+QIACT:")==NULL)
                {
                    DbgPrintf("Modem no activated,activating now\r\n");
                    writeModem((void *)"AT+QIACT=1\r\n",12);
                    memset(aBuf,0,sizeof(aBuf));
                    rnum = readModem(aBuf);
                    if(strstr((void *)aBuf,"ERROR")!=NULL)
                    {
                        nResetCount++;
                        continue;
                    }
                    memset(aBuf,0,sizeof(aBuf));
                    do{
                        writeModem((void *)"AT+QIACT?\r\n",11);
                        rnum = readModem(aBuf);
                        if((nErrorCount++)>=80)
                            break;
                        sleep(2);
                    }while((strstr((void *)aBuf,"+QIACT:")==NULL)&&(strstr((void *)aBuf,"ERROR")==NULL));
                    if((nErrorCount>=80)||(strstr((void *)aBuf,"ERROR")!=NULL))
                    {
                        nResetCount++;
                        continue;
                    }
                }
                LedSet(2,1);
                sleep(5);

            case MI_TLS1:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"sslversion\",0,1\r\n",29);
                    DbgPrintf("MI_TLS1:AT+QSSLCFG=\"sslversion\",0,1\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS2:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"ciphersuite\",0,0xFFFF\r\n",35);
                    DbgPrintf("MI_TLS2:AT+QSSLCFG=\"ciphersuite\",0,\"0XFFFF\"\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS3:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"cacert\",0,\"UFS:ca-cert.pem\"\r\n",41);
                    DbgPrintf("MI_TLS3:AT+QSSLCFG=\"cacert\",0,\"UFS:ca-cert.pem\"\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS4:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"clientcert\",0,\"UFS:client.pem\"\r\n",44);
                    DbgPrintf("MI_TLS4:AT+QSSLCFG=\"clientcert\",0,\"UFS:client.pem\"\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS5:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"clientkey\",0,\"UFS:ckey.pem\"\r\n",41);
                    DbgPrintf("MI_TLS5:AT+QSSLCFG=\"clientkey\",0,\"UFS:ckey.pem\"\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS6:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"seclevel\",0,0\r\n",27);
                    DbgPrintf("MI_TLS6:AT+QSSLCFG=\"seclevel\",0,0\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_TLS7:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    writeModem((void *)"AT+QSSLCFG=\"ignorelocaltime\",1\r\n",32);
                    DbgPrintf("MI_TLS7:AT+QSSLCFG=\"ignorelocaltime\",1\r\n");
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=10)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=10)
                {
                    nResetCount++;
                    continue;
                }

            case MI_CONFIG_GSM:
                if(gLocationReport == 0)
                {
                    nErrorCount=0;
                    memset(aBuf,0,sizeof(aBuf));
                    do{
                        writeModem((void *)"AT+QCFG=\"nwscanmode\",1\r\n",24);
                        rnum = readModem(aBuf);
                        sleep(1);
                        if((nErrorCount++)>=10)
                            break;
                    }while(strstr((void *)aBuf,"OK")==NULL);
                    if(nErrorCount>=10)
                    {
                        nResetCount++;
                        continue;
                    }
                    DbgPrintf("Network is GSM only now\r\n");
                    sleep(5);
                }

            case MI_LOCATION:
                if(gLocationReport == 0)
                {
                    memset(aBuf,0,sizeof(aBuf));
                    nErrorCount=0;
                    do{
                        writeModem((void *)"AT+QCELLLOC=1\r\n",15);
                        rnum = readModem(aBuf);
                        sleep(5);
                        nErrorCount++;
                        if(nErrorCount>12)
                            break;
                    }while(strstr((void *)aBuf,"+QCELLLOC")==NULL);
                    //if(nErrorCount>=12)
                    //{
                     //   nResetCount++;
                      //  continue;
                    //}
                    if(strstr((void *)aBuf,"+QCELLLOC") != NULL)
                    {
                        UINT8 *pLocationHead,*pLocationEnd;

                        pLocationHead=(void *)strstr((void *)aBuf,": ");
                        pLocationHead +=2;
                        pLocationEnd = (void *)strstr((void *)pLocationHead,"\r\n");
                        memset(gLocationInfo,0,sizeof(gLocationInfo));
                        memcpy(gLocationInfo,pLocationHead,pLocationEnd-pLocationHead);
                    }
                }

            case MI_CONFIG_AUTO:
                if(gLocationReport == 0)
                {
                    nErrorCount=0;
                    memset(aBuf,0,sizeof(aBuf));
                    do{
                        writeModem((void *)"AT+QCFG=\"nwscanmode\",0\r\n",24);
                        rnum = readModem(aBuf);
                        sleep(1);
                        if((nErrorCount++)>=10)
                            break;
                    }while(strstr((void *)aBuf,"OK")==NULL);
                    if(nErrorCount>=10)
                    {
                        nResetCount++;
                        continue;
                    }
                    DbgPrintf("Network is auto now\r\n");
                    sleep(1);
                }

            case MI_CLOSE_SERVER:
                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                do{
                    UINT8 *pCmd=(void *)"AT+QICLOSE=%d\r\n";
                    UINT8 aCmd[100];

                    sprintf((void *)aCmd,(void *)pCmd,uChannel);
                    writeModem(aCmd,strlen((void *)aCmd));
                    rnum = readModem(aBuf);
                    sleep(1);
                    if((nErrorCount++)>=30)
                        break;
                }while(strstr((void *)aBuf,"OK")==NULL);
                if(nErrorCount>=30)
                {
                    nResetCount++;
                    continue;
                }
                DbgPrintf("Socket close finish\r\n");
                sleep(1);

                gModemNetStatus=gModemNetStatus | 0x01;
                memset(aBuf,0,sizeof(aBuf));

                if((gModemNetStatus&0xF0)&&(gSocketMode==1))
                {
                    SendSingleFrameToAPP(MODEM_STATUS_CMD,S_R_InfoReport,1);
                }
                sleep(1);

            case MI_OPEN_SERVER:
            {
                UINT8 *pDoname0Cmd=(void *)"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0\r\n";
                UINT8 *pDoname1Cmd=(void *)"AT+QIOPEN=1,1,\"TCP\",\"%s\",%d,0,0\r\n"; //(void *)"AT+QSSLOPEN=1,0,1,\"%s\",%d,0\r\n";
                UINT8 aConnetCmd[100];
                UINT8 aServerHostName[30];
                UINT16 nPort;

                nErrorCount=0;
                memset(aBuf,0,sizeof(aBuf));
                if(uChannel==0)
                {
                    do
                    {
                        //UINT8 *pConnetCmd=(void *)"AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,0\r\n";

                        pthread_mutex_lock(&modemsem);
                        E2promRead((UINT8 *)&nPort,ServerDomainNamePortAddr,2);
                        //E2promRead(aServerIP,StationIPAddr,4);
                        E2promRead(aServerHostName,ServerDomainNameAddr,30);
                        //sprintf(aConnetCmd,pConnetCmd,aServerIP[0],aServerIP[1],aServerIP[2],aServerIP[3],nPort);
                        sprintf((void *)aConnetCmd,(void *)pDoname0Cmd,aServerHostName,nPort);
                        //sprintf((void *)aConnetCmd,(void *)pDoname0Cmd,"182.150.21.245",12123);
                        DbgPrintf("%s\r\n",aConnetCmd);
                        TestLogTimeFileWrite();
                        TestLogStringFileWrite(aConnetCmd,strlen((void *)aConnetCmd));
                        writeModem(aConnetCmd,strlen((void *)aConnetCmd));
                        sleep(2);
                        rnum = readModem(aBuf);
                        pthread_mutex_unlock(&modemsem);
                        sleep(3);
                        if((nErrorCount++)>=30)
                            break;
                        if(gSocketMode==0)
                            return 0;
                    }while(strstr((void *)aBuf,"0,0")==NULL);
                }
                else if(uChannel==1)
                {
                    //UINT8 *pConnetCmd=(void *)"AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,0\r\n";
                    do
                    {
                        pthread_mutex_lock(&modemsem);
                        E2promRead((UINT8 *)&nPort,SlaveDomainNamePortAddr,2);
                        //E2promRead(aServerIP,StationIPAddr,4);
                        E2promRead(aServerHostName,SlaveDomainNameAddr,30);
                        //sprintf(aConnetCmd,pConnetCmd,aServerIP[0],aServerIP[1],aServerIP[2],aServerIP[3],nPort);
                        sprintf((void *)aConnetCmd,(void *)pDoname1Cmd,aServerHostName,nPort);
                        //sprintf((void *)aConnetCmd,(void *)pDoname1Cmd,"117.139.166.27",12235);
                        DbgPrintf("%s\r\n",aConnetCmd);
                        TestLogTimeFileWrite();
                        TestLogStringFileWrite(aConnetCmd,strlen((void *)aConnetCmd));
                        writeModem(aConnetCmd,strlen((void *)aConnetCmd));
                        sleep(6);
                        rnum = readModem(aBuf);
                        pthread_mutex_unlock(&modemsem);
                        sleep(6);
                        if((gSocketMode==0) || (gModuleChannel0InitFlag==0) || (gModemSlaveChannelSwitch == 0) || (gMainDeviceStatus!=10))
                            return 0;
                        if((nErrorCount++)>=30)
                            break;
                    }while(strstr((void *)aBuf,"1,0")==NULL);
                }
                if(nErrorCount>=30)
                {
                    nResetCount++;
                    return 0;
                }
            }
            default:
                finish_flag=1;
                break;
        }
        if(finish_flag==1)
            break;
    }
    nResetCount=0;
    if(uChannel==0)
        gModuleChannel0InitFlag=1;
    DbgPrintf("INIT OK\r\n");
    LedSet(1,1);

    /** 读取SIM卡号信息 */
   // UINT8 *pSimPoint,*pSimEnd,nSimCount=0;
    UINT8 *pEnd,*pSim,*p;

    memset(aBuf,0,sizeof(aBuf));
    do{
        writeModem((void *)"AT+CIMI\r\n",9);
        rnum = readModem(aBuf);
        sleep(1);
    }while(strstr((void *)aBuf,"OK")==NULL);
    DbgPrintf("MMM Get SIM Card CIMI Info: %s MMM\r\n", aBuf);

    /** 清空sim card信息数组 */
    memset(sim_card_id,0,sizeof(sim_card_id));
    /** 跳过开头的非数字字符*/
    //pSimPoint=aBuf;
    /*  do
    {
        pSimEnd = strstr(pSimPoint,"\r\n");
        if((pSimEnd==NULL)||((pSimEnd-pSimPoint)<5))
        {
            pSimPoint = pSimEnd+strlen("\r\n");
            nSimCount = nSimCount+pSimEnd-pSimPoint+2;
            continue;
        }
        for(i=0;i<pSimEnd-pSimPoint;i++)
        {
            if((pSimPoint[i]<'0')||(pSimPoint[i]>'9'))
            {
                break;
            }
            nSimCount++;
        }
        if(nSimCount!=(pSimEnd-pSimPoint))
        {
            pSimPoint = pSimEnd+strlen("\r\n");
            nSimCount = nSimCount+(pSimPoint-pSimEnd-i)+2;
            continue;
        }
        else
        {
            memcpy(sim_card_id,pSimPoint,pSimEnd-pSimPoint);
            break;
        }
    }while(nSimCount<rnum);*/
    pSim=aBuf;
    p=aBuf;
    DbgPrintf("[SIM]End OK\n");
    /*while(p!=NULL)
    {
        pEnd=p;
        p= strstr(p,"OK");
    }*/
    pEnd=(void *)strstr((void *)p,"OK");
    DbgPrintf("[SIM]pSim\n");
    while((pSim+15+2+strlen("\r\n"))!=pEnd)
    {
        pSim++;
    }
    memcpy(sim_card_id,pSim,15);
    DbgPrintf("[SIM]SIm is : %s\n",sim_card_id);

    return 1;
}

/*****************************************************************************
* Function     : ModuleThreadMasterChannel()
* Description  : 模块主通道线程
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年10月15日        Andre
*****************************************************************************/
void *ModuleThreadMasterChannel()
{
    UINT8 nRecvCountTemp=0,nPower=0,led_time_count=0;
    UINT8 power_blink_mode=0;  //0:weak signal 1:normal signal 2:strong signal
    UINT8 aRecvBuf[1024];
    UINT16 power_time_count=0;

    DbgPrintf("THREAD-----Module Client Master Channel Init!\r\n");

    while(1)
    {
        nModemFd = UartOper(1,115200);

        gModuleChannel0InitFlag=0;

        if(gSocketMode != 1)
        {
            usleep(5000);
            DbgPrintf("[Channel 0]Module Client Master Channel Quit!\r\n");
            return 0;
        }


        while(gSocketMode == 1)
        {
            //gModemRecvFlag=0;
            gIsReportInfo = _NO;
            if(ModuleInit(MI_START,0,0)==0)
                break;
            gModemNetStatus = gModemNetStatus & 0x0F;
            gSocketHeartChannel0Count=0;
            sleep(1);
            DbgPrintf("[Channel 0]Device Status=%d\r\n",gMainDeviceStatus);

            //SendDeviceInfoToServer();
            /** 与平台建立连接后，上报设备信息 */

            ReportDeviceInfoToServer();
            sleep(2);
//            gIsReportInfo = _YES;
            SendFrameToPlatform(gMainDeviceID,DEVICE_MAXNUMBER_CMD,S_R_InfoReport,(UINT8 *)&gConnectDeviceMaxNum,sizeof(gConnectDeviceMaxNum));
            gModuleChannel0InitFlag=2;
//            ReadAlarmFile();
            SendFrameToPlatform(gMainDeviceID,DEVICE_LOCATION_CMD,S_R_InfoReport,gLocationInfo,sizeof(gLocationInfo));

            while((gModuleChannel0InitFlag>0)&&(gSocketMode==1))
            {
                UINT8 *pPoint,*pEnd;
                UINT16 nLen,i;
                UINT32 nErrorCount=0;
                UINT8 aTemp[1024];
                int rnum=0;

                //time synchronize
                if(power_blink_mode == 0)
                {
                    led_time_count += 4;
                    power_time_count += 4;
                }
                else if(power_blink_mode == 1)
                {
                    led_time_count += 2;
                    power_time_count += 2;
                }
                else
                {
                    led_time_count++;
                    power_time_count++;
                }

                //COM LED control
                if(led_time_count >= 2)
                {
                    LedSet(4,0);
                    LedSet(3,0);
                    LedSet(4,1);
                    LedSet(3,1);
                    led_time_count = 0;
                }

                //signal power acquire and report
                if(power_time_count >= 140)    //one minute
                {
                    UINT8 aBuf[512],rnum = 0;
                    UINT8 *pCSQPoint,*pCSQEnd;
                    UINT8 error_count = 0;

                    memset(aBuf,0,sizeof(aBuf));
                    nPower = 0;
                    pthread_mutex_lock(&modemsem);
                    sleep(1);
                    do{
                        writeModem((void *)"AT+CSQ\r\n",8);
                        rnum = readModem(aBuf);
                        sleep(1);
                        if((error_count++) >= 10)
                            break;
                    }while(strstr((void *)aBuf,"OK")==NULL);
                    pthread_mutex_unlock(&modemsem);
                    pCSQPoint=(void *)strstr((void *)aBuf,"+CSQ: ");
                    pCSQPoint+=strlen("+CSQ: ");
                    pCSQEnd=(void *)strstr((void *)pCSQPoint,",");
                    for(i=0;i<pCSQEnd-pCSQPoint;i++)
                    {
                        nPower = nPower+(pCSQPoint[i]-0x30)*(int)pow(10,pCSQEnd-pCSQPoint-i-1);
                    }

                    SendFrameToPlatform(gMainDeviceID,MODEM_SIGNAL_PWR_CMD,S_R_InfoReport,(UINT8 *)&nPower,sizeof(nPower));
                    power_time_count = 0;
                }

                //modem LED blink control
                if(nPower <= 12)
                {
                    LedSet(2,0);
                    sleep(1);
                    LedSet(2,1);
                    power_blink_mode = 0;
                    usleep(700000);
                }
                else if(nPower <= 21)
                {
                    LedSet(2,0);
                    usleep(500000);
                    LedSet(2,1);
                    power_blink_mode = 1;
                    usleep(200000);
                }
                else
                {
                    LedSet(2,1);
                    power_blink_mode = 2;
                    usleep(200000);
                }

                pthread_mutex_lock(&modemsem);
                //usleep(200000);
                memset(aRecvBuf,0,sizeof(aRecvBuf));
                nRecvCountTemp=0;
                if((gModuleChannel0InitFlag==0)||(gSocketMode!=1))
                {
                    pthread_mutex_unlock(&modemsem);
                    break;
                }
                writeModem((void *)"AT+QIRD=0,1024\r\n",16);
                //usleep(5000);
                //rnum = readModem(aRecvBuf);
                nErrorCount=0;
                while(1)
                {
                    usleep(500);
                    memset(aTemp,0,sizeof(aTemp));
                    rnum = read(nModemFd,aTemp,sizeof(aTemp));
                    if(rnum<=0)
                    {
                        nErrorCount++;
                        if(nErrorCount>1000)
                            break;
                    }
                    else
                    {
                        //printf("Modem Recv = %s\r\n",aTemp);
                        memcpy((UINT8 *)&aRecvBuf[nRecvCountTemp],aTemp,rnum);
                        nRecvCountTemp += rnum;
                        pPoint = (void *)strstr((void *)aRecvBuf,"+");
                        if(pPoint!=NULL)
                        {
                            pPoint = (void *)strstr((void *)pPoint,"+QIRD: ");
                            if(pPoint!=NULL)
                            {
                                pPoint += strlen("+QIRD: ");
                                pEnd=(void *)strstr((void *)pPoint,"\r\n");
                                if(pEnd==NULL)
                                {
                                    continue;
                                }
                                nLen = 0;
                                for(i=0;i<pEnd-pPoint;i++)
                                {
                                    nLen = nLen+(pPoint[i]-0x30)*pow(10,pEnd-pPoint-i-1);
                                }
                                pPoint = pEnd+nLen+2;
                                pPoint = (void *)strstr((void *)pPoint,"OK");
                                if(pPoint!=NULL)
                                {
                                    nRecvCountTemp = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(nErrorCount>1000)
                {
                    DbgPrintf("[Channel 0]Receive Modem Error number = %d Data = ",nRecvCountTemp);
                    for(i=0;i<nRecvCountTemp;i++)
                    {
                        DbgPrintf("%c",aRecvBuf[i]);
                    }
                    DbgPrintf("\r\n");
                    pthread_mutex_unlock(&modemsem);
                    usleep(1000);
                    continue;
                }
                if(strstr((void *)aRecvBuf,"+QIRD: ")!=NULL)
                {
                    UINT8 aBufTemp[256];
                    UINT16 nPacketLen,nLenCount;
                    UINT8 aBuf[512];
                    UINT16 nLen=0;

                    pPoint=aRecvBuf;
                    pPoint=(void *)strstr((void *)pPoint,"+QIRD: ");
                    pPoint += strlen("+QIRD: ");
                    pEnd=(void *)strstr((void *)pPoint,"\r\n");
                    pthread_mutex_unlock(&modemsem);
                    if(pEnd==NULL)
                    {
                        usleep(1000);
                        continue;
                    }
                    for(i=0;i<pEnd-pPoint;i++)
                        nLen = nLen+(pPoint[i]-0x30)*pow(10,pEnd-pPoint-i-1);
                    if(nLen!=0)
                    {
                        NorthPrintf("[Channel 0]Modem RecvData %d Byte=",nLen);
                        pPoint=pEnd+2;
                        for(i=0;i<nLen;i++)
                        {
                            NorthPrintf("%02X ",pPoint[i]);
                        }
                        NorthPrintf("\r\n");
                        memcpy(aBuf,pPoint,nLen);
                        rnum = nLen;
                        nLenCount=0;
                        while(rnum!=nLenCount)
                        {
                            memset(aBufTemp,0,sizeof(aBufTemp));
                            nPacketLen = aBuf[nLenCount+1];
                            memcpy(aBufTemp,(UINT8 *)&aBuf[nLenCount],nPacketLen+2);
                            gSocketHeartChannel0Count=0;
                            if((aBufTemp[0]==0x68)&&(aBufTemp[1]>=4))
                            {
                                if((aBufTemp[2]&0x01)==0)
                                    RecvBufferAdd(1,aBufTemp,nPacketLen+2);
                                else
                                    RecvBufferAdd(0,aBufTemp,nPacketLen+2);
                            }
                            else
                            {
                                if(gUpdataModeFlag==1)
                                {
                                    SendUpdataResendPacket();
                                }
                                break;
                            }
                            nLenCount = nLenCount+nPacketLen+2;
                        }
                    }
                }
                else if(strstr((void *)aRecvBuf,"closed")!=NULL)
                {
                    //pthread_mutex_unlock(&modemsem);
                    DbgPrintf("[Channel 0]Modem close\r\n");
                    break;
                }
                else
                {
                    DbgPrintf("[Channel 0]Modem error %s\r\n",aRecvBuf);
                }
                /*if(strstr(aRecvBuf,"+QIURC")!=NULL)
                {
                    gModemRecvFlag=1;
                }*/
                //pthread_mutex_unlock(&modemsem);
                usleep(1000);
            }
            TestLogTimeFileWrite();
            TestLogStringFileWrite((void *)"Modem DisConnect\n",strlen("Modem DisConnect\n"));
        }
        closeDev(nModemFd);
    }
}

/*****************************************************************************
* Function     : ModuleThreadSlaveChannel()
* Description  : 模块次通道线程
* Input        : None
* Output       : None
* Return       :
* Note(s)      : 此通道在主通道建立之后再建立，故初始化只需要从AT+QIOPEN开始
* Contributor  : 2018年10月15日        Andre
*****************************************************************************/
void *ModuleThreadSlaveChannel()
{
    UINT8 nRecvCountTemp=0;
    UINT8 aRecvBuf[1024];

    DbgPrintf("THREAD-----Module Client Slave Channel Init!\r\n");

    while(1)
    {
        gModuleChannel1InitFlag=0;

        if((gModeSub!=2)||(gSocketModeSub!=2)||(gModemSlaveChannelSwitch==0)||(gMainDeviceStatus!=10)||(gModemSlaveConfigFlag!=1))
        {
            usleep(500);
            DbgPrintf("[Channel 1]Module Client Slave Channel Quit!\r\n");
            return 0;
        }
        if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0)) ||
          ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER)))
        {
            sleep(2);
            DbgPrintf("[Channel 1]Waiting for master channel ready\r\n");
            continue;
        }
        if(nModemFd==-1)
            nModemFd = UartOper(1,115200);

        while((gModeSub == 2)&&(gSocketModeSub == 2))
        {
            //gModemRecvFlag=0;
            if(((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd<=0)) ||
              ((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag!=MODEMFLAG_CONNECT_SERVER)))
                break;
            if((gSocketMode==SOCKETMODE_3G)&&(gModuleChannel0InitFlag==MODEMFLAG_CONNECT_SERVER))
            {
                if(ModuleInit(MI_CLOSE_SERVER,1,0)==0)
                    break;
            }
            if((gSocketMode==SOCKETMODE_LAN)&&(g_nRemotesockfd>0))
            {
                if(ModuleInit(MI_BOOT,1,0)==0)
                    break;
            }

            gSocketHeartChannel1Count=0;
            sleep(1);
            DbgPrintf("[Channel 1]Device Status=%d\r\n",gMainDeviceStatus);

            //SendDeviceInfoToServer();

            /** 与平台建立连接后，上报设备信息 */
            ReportDeviceInfoToThirdPartyServer(SOCKETMODE_3G);
            sleep(2);
//            gIsReportInfo = _YES;
            SendFrameToThirdPartyPlatform(gMainDeviceID,DEVICE_MAXNUMBER_CMD,S_R_InfoReport,(UINT8 *)&gConnectDeviceMaxNum,sizeof(gConnectDeviceMaxNum),SOCKETMODE_3G);
            gModuleChannel1InitFlag=2;
            gTypeGroupPointTran=NULL;
            gDLinkDeviceInfoCount = 0;

            while((gModuleChannel1InitFlag > 0)&&(gModeSub == 2)&&(gSocketModeSub == 2))
            {
                UINT8 *pPoint,*pEnd;
                UINT16 nLen,i;
                UINT32 nErrorCount=0;
                UINT8 aTemp[1024];
                int rnum=0;

                pthread_mutex_lock(&modemsem);
                //usleep(200000);
                memset(aRecvBuf,0,sizeof(aRecvBuf));
                nRecvCountTemp=0;
                if((gModuleChannel1InitFlag == 0)||(gModeSub != 2)||(gSocketModeSub != 2))
                {
                    pthread_mutex_unlock(&modemsem);
                    break;
                }
                writeModem((void *)"AT+QIRD=1,1024\r\n",16);//writeModem((void *)"AT+QSSLRECV=1,1024\r\n",20);
                //usleep(5000);
                //rnum = readModem(aRecvBuf);
                nErrorCount=0;
                while((gModeSub == 2)&&(gSocketModeSub == 2))
                {
                    usleep(500);
                    memset(aTemp,0,sizeof(aTemp));
                    rnum = read(nModemFd,aTemp,sizeof(aTemp));
                    if(rnum<=0)
                    {
                        nErrorCount++;
                        if(nErrorCount>1000)
                        {
                            break;
                        }
                    }
                    else
                    {
                        //printf("[Channel 1]Modem Recv = %s\r\n",aTemp);
                        memcpy((UINT8 *)&aRecvBuf[nRecvCountTemp],aTemp,rnum);
                        nRecvCountTemp += rnum;
                        pPoint = (void *)strstr((void *)aRecvBuf,"+");
                        if(pPoint!=NULL)
                        {
                            pPoint = (void *)strstr((void *)pPoint,"+QIRD: ");//"+QSSLRECV: "
                            if(pPoint!=NULL)
                            {
                                pPoint += strlen("+QIRD: ");
                                pEnd=(void *)strstr((void *)pPoint,"\r\n");
                                if(pEnd==NULL)
                                {
                                    continue;
                                }
                                nLen = 0;
                                for(i=0;i<pEnd-pPoint;i++)
                                {
                                    nLen = nLen+(pPoint[i]-0x30)*pow(10,pEnd-pPoint-i-1);
                                }
                                pPoint = pEnd+nLen+2;
                                pPoint = (void *)strstr((void *)pPoint,"OK");
                                if(pPoint!=NULL)
                                {
                                    nRecvCountTemp = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(nErrorCount>1000)
                {
                    DbgPrintf("[Channel 1]Receive Modem Error number = %d Data = ",nRecvCountTemp);
                    for(i=0;i<nRecvCountTemp;i++)
                    {
                        DbgPrintf("%c",aRecvBuf[i]);
                    }
                    DbgPrintf("\r\n");
                    pthread_mutex_unlock(&modemsem);
                    usleep(1000);
                    if(gModuleChannel0InitFlag==0)
                        break;
                    continue;
                }
                if(strstr((void *)aRecvBuf,"+QIRD: ")!=NULL)
                {
                    UINT8 aBufTemp[256];
                    UINT16 nPacketLen,nLenCount;
                    UINT8 aBuf[512];
                    UINT16 nLen=0;

                    pPoint=aRecvBuf;
                    pPoint=(void *)strstr((void *)pPoint,"+QIRD: ");
                    pPoint += strlen("+QIRD: ");
                    pEnd=(void *)strstr((void *)pPoint,"\r\n");
                    if(pEnd==NULL)
                    {
                        pthread_mutex_unlock(&modemsem);
                        usleep(1000);
                        continue;
                    }
                    for(i=0;i<pEnd-pPoint;i++)
                        nLen = nLen+(pPoint[i]-0x30)*pow(10,pEnd-pPoint-i-1);
                    if(nLen!=0)
                    {
                        NorthPrintf("[Channel 1]Modem RecvData %d Byte=",nLen);
                        pPoint=pEnd+2;
                        for(i=0;i<nLen;i++)
                        {
                            NorthPrintf("%02X ",pPoint[i]);
                        }
                        NorthPrintf("\r\n");
                        memcpy(aBuf,pPoint,nLen);
                        rnum = nLen;
                        nLenCount=0;
                        while(rnum!=nLenCount)
                        {
                            memset(aBufTemp,0,sizeof(aBufTemp));
                            nPacketLen = aBuf[nLenCount+1];
                            memcpy(aBufTemp,(UINT8 *)&aBuf[nLenCount],nPacketLen+2);
                            gSocketHeartChannel1Count=0;
                            if((aBufTemp[0]==0x68)&&(aBufTemp[1]>=4))
                            {
                                if((aBufTemp[2]&0x01)==0)
                                    AddMsgToSGCCReceiveBuff(aBufTemp,nPacketLen+2);
                                else
                                    AddMsgToSGCCReceiveBuff(aBufTemp,nPacketLen+2);
                            }
                            else
                            {
                                if(gUpdataModeFlag==1)
                                {
                                    //SendUpdataResendPacket();
                                }
                                break;
                            }
                            nLenCount = nLenCount+nPacketLen+2;
                        }
                    }
                }
                else if(strstr((void *)aRecvBuf,"closed")!=NULL)
                {
                    pthread_mutex_unlock(&modemsem);
                    DbgPrintf("[Channel 1]Modem close\r\n");
                    break;
                }
                else
                {
                    DbgPrintf("[Channel 1]Modem error %s\r\n",aRecvBuf);
                }
                /*if(strstr(aRecvBuf,"+QIURC")!=NULL)
                {
                    gModemRecvFlag=1;
                }*/
                pthread_mutex_unlock(&modemsem);
                usleep(1000);
            }
            TestLogTimeFileWrite();
            TestLogStringFileWrite((void *)"Modem DisConnect\n",strlen("Modem DisConnect\n"));
        }
    }
}

/*****************************************************************************
* Function     : UploadCertificate()
* Description  : 更新模块加密证书
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年11月1日        Andre
*****************************************************************************/
void UploadCertificate()
{
    char aBuf[512]={0},*file_data,at_cmd[50]={0},cmd_line[50]={0};
    UINT8 error_count=0;
    int rnum=0,file_size=0;
    //UINT8 i=0;

    if(access(CA_FILE_PATH, F_OK) == 0)
    {
        memset(aBuf,0,sizeof(aBuf));
        memset(at_cmd,0,sizeof(at_cmd));
        sprintf(at_cmd,"AT+QFDEL=\"UFS:*\"\r\n");
        DbgPrintf("[Upload Certificate] send at cmd: %s",at_cmd);
        pthread_mutex_lock(&modemsem);
        do{
            writeModem((void *)at_cmd,sizeof(at_cmd));
            rnum = readModem((void *)aBuf);
            sleep(1);
            if((error_count++) >= 10)
                break;
        }while(strstr((void *)aBuf,"OK")==NULL);
        pthread_mutex_unlock(&modemsem);
        sleep(1);

        file_size=GetFileLength(CA_FILE_PATH);
        error_count=0;
        memset(aBuf,0,sizeof(aBuf));
        memset(at_cmd,0,sizeof(at_cmd));
        sprintf(at_cmd,"AT+QFUPL=\"UFS:ca-cert.pem\",%d\r\n",file_size);
        DbgPrintf("[Upload Certificate] send at cmd: %s",at_cmd);
        pthread_mutex_lock(&modemsem);
        do{
            writeModem((void *)at_cmd,sizeof(at_cmd));
            rnum = readModem((void *)aBuf);
            sleep(1);
            if((error_count++) >= 10)
            {
                pthread_mutex_unlock(&modemsem);
                return;
            }
        }while(strstr((void *)aBuf,"CONNECT")==NULL);

        error_count=0;
        memset(aBuf,0,sizeof(aBuf));

        file_data=(char *)malloc(sizeof(char)*file_size);
        GetFileData(CA_FILE_PATH, file_data, 0, file_size);
        writeModem((void *)file_data,file_size);
        DbgPrintf("[writeModem]%s\r\n",file_data);
        free(file_data);
        while(strstr((void *)aBuf,"OK")==NULL)
        {
            rnum = readModem((void *)aBuf);
            usleep(1000);
            if((error_count++) >= 3)
            {
                pthread_mutex_unlock(&modemsem);
                return;
            }
        }
        pthread_mutex_unlock(&modemsem);

        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "rm -rf %s", CERT_FOLDER_PATH);
        DbgPrintf("[Upload Certificate] cmd_line = %s\n", cmd_line);
        system(cmd_line);

        //printf("[Upload Certificate] finish\r\n");
    }
    else
    {
        DbgPrintf("[Upload Certificate] no file %s\r\n", CA_FILE_PATH);
    }
}


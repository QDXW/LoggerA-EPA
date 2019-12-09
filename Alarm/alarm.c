/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : alarm.c
* Author            : Andre
* Date First Issued : 2019.2.18
* Version           :
* Description       : 告警相关函数
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <malloc.h>
#include "../protocal/ProtocolDef.h"
#include "../interface/interfaceExtern.h"
#include "../SGCC/store.h"
#include "../XML/xml.h"
#include "../Ethernet/EthernetExtern.h"

UINT8 gDeviceAlarmBuf[10];                   //alarm of the south device ,every bit means one device total 80 devices
UINT16 aAlarmInfo[40][20];
UINT16 gDeviceAlarm=0;
struct sAlarmGroup *gAlarmHead=NULL;         //the head of alarm list
struct sAlarmGroup *gAlarmPoint=NULL;        //point of alarm list

/*****************************************************************************
* Function     : AlarmAdd()
* Description  : add alarm to list
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
* Output       : None
* Return       : alarm error count
* Note(s)      :
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
UINT8 AlarmAdd(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue)
{
    struct sAlarmGroup *pPoint;

    pthread_mutex_lock(&alarmsem);
    pPoint=gAlarmHead;
    while(pPoint!=NULL)
    {
        if((pPoint->nDeviceID==nDeviceID) && (pPoint->nAlarmID==nAlarmID) && (pPoint->nModbusAddr==nAddr) && (pPoint->nAlarmExternID == nExternValue))
        {
            /** 如果该设备已经被删除，则不上报告警 */
            if (gDeviceInfo[nDeviceID].nInUse == _NO)
            {
                break;
            }

            if(pPoint->nAlarmID == Alarm_Link_Error)
            {
                if(pPoint->nAlarmCount <= ALARM_SCANF_MAXNUMBER)
                    pPoint->nAlarmCount++;
                else if(pPoint->nUploadStatusMasterChannel != ALARM_NOT_REPORT)
                {
                    pPoint->nAlarmCount = 0;
                }
            }
            break;
        }
        else
        {
            pPoint=pPoint->pNext;
        }
    }

    if(pPoint==NULL)
    {
        gDeviceAlarmBuf[(nDeviceID-1)/8] = gDeviceAlarmBuf[(nDeviceID-1)/8] | (0x01<<((nDeviceID-1)%8));
        E2promWrite(gDeviceAlarmBuf,AlarmStatusAddr,10);
        pPoint=(struct sAlarmGroup*)malloc(sizeof(struct sAlarmGroup));
        pPoint->nDeviceID=nDeviceID;
        pPoint->nAlarmID=nAlarmID;
        pPoint->nModbusAddr=nAddr;
        pPoint->nAlarmExternID=nExternValue;
        pPoint->nUploadStatusMasterChannel=ALARM_NOT_REPORT;
        pPoint->nUploadStatusSlaveChannel=ALARM_NOT_REPORT_SLAVE;
        if(nAlarmID == Alarm_Link_Error)
        {
            pPoint->nAlarmCount = 1;
        }
        /*else if((nAlarmID==Alarm_Device_Error)&&(nAddr==0x0001)&&(nExternValue==0))
        {
            pPoint->nUploadStatusMasterChannel=2;
        }*/
        else
        {
            pPoint->nAlarmCount = 11;
            pPoint->nUploadStatusMasterChannel = 2;
        }
        pPoint->pNext=NULL;
        pPoint->pPre=NULL;
        if(gAlarmHead==NULL)
        {
            gAlarmHead=pPoint;
            gAlarmPoint=pPoint;
        }
        else
        {
            gAlarmPoint->pNext=pPoint;
            pPoint->pPre=gAlarmPoint;
            gAlarmPoint=pPoint;
        }
    }
    DbgPrintf("[Alarm Add]Alarm!!!!!!!---------No.%d Device Alarm No.%d,MBAddr=%d,bit %d is Count %d\r\n",nDeviceID,nAlarmID,nAddr,nExternValue,pPoint->nAlarmCount);
    pthread_mutex_unlock(&alarmsem);
    return pPoint->nAlarmCount;
}

/*****************************************************************************
* Function     : AlarmRemove()
* Description  : Delete alarm from alarm link list
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
* Output       : None
* Return       : None
* Note(s)      : 此函数调用较少且功能与AlarmDelete部分重叠，后期考虑修改
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void AlarmRemove(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue)
{
    struct sAlarmGroup *pPoint;

    pthread_mutex_lock(&alarmsem);
    pPoint=gAlarmHead;
    if(pPoint==NULL)
    {
        pthread_mutex_unlock(&alarmsem);
        return;
    }
    while(pPoint!=NULL)
    {
        //DbgPrintf("Alarm remove = %d alarmID = %d modbus = %d alarmextern = %d nAlarmCount = %d nUploadStatusMasterChannel = %d\r\n",
                   //pPoint->nDeviceID,pPoint->nAlarmID,pPoint->nModbusAddr,pPoint->nAlarmExternID, pPoint->nAlarmCount, pPoint->nUploadStatusMasterChannel);
        if((pPoint->nDeviceID==nDeviceID)
           && (pPoint->nAlarmID==nAlarmID)
           && (pPoint->nModbusAddr==nAddr)
           && (pPoint->nAlarmExternID==nExternValue))
        {
            /** 如果该设备已经被删除，则不上报告警 */
            if (gDeviceInfo[nDeviceID].nInUse == _NO)
            {
                break;
            }

            if(pPoint->nUploadStatusMasterChannel==ALARM_NOT_REPORT)
            {
                if((pPoint->pPre==NULL)&&(pPoint->pNext==NULL))
                {
                    gAlarmHead=NULL;
                }
                else if(pPoint->pPre==NULL)
                {
                    gAlarmHead=pPoint->pNext;
                    gAlarmHead->pPre=NULL;
                }
                else if(pPoint->pNext==NULL)
                {
                    gAlarmPoint=pPoint->pPre;
                    gAlarmPoint->pNext=NULL;
                }
                else
                {
                    pPoint->pPre->pNext=pPoint->pNext;
                    pPoint->pNext->pPre=pPoint->pPre;
                }
                free(pPoint);
            }
            /*else if(pPoint->nUploadStatusMasterChannel==ALARM_REPORT_CONFIRMFLAG)
            {
                pPoint->nAlarmCount=0;
            }*/
            break;
        }
        else
        {
            pPoint=pPoint->pNext;
        }
    }
    if(pPoint != NULL)
    {
        DbgPrintf("[Alarm Remove]Remove Alarm!---------No.%d Device Alarm No.%d,MBAddr=%d,bit %d is Count %d\r\n",nDeviceID,nAlarmID,nAddr,nExternValue,pPoint->nAlarmCount);
    }
    /*pPoint=gAlarmHead;
    while(pPoint!=NULL)
    {
        printf("Alarm remove = %d alarmID = %d modbus = %d alarmextern = %d nAlarmCount = %d\r\n",pPoint->nDeviceID,pPoint->nAlarmID,pPoint->nModbusAddr,pPoint->nAlarmExternID, pPoint->nAlarmCount);
        pPoint=pPoint->pNext;
    }*/
    pthread_mutex_unlock(&alarmsem);
    return;
}

/*****************************************************************************
* Function     : AlarmReload()
* Description  : reload alarm linked list
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
* Output       : None
* Return       : alarm error count
* Note(s)      : 从通道断连后告警状态清0，恢复重新上报
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
UINT8 AlarmReload(UINT8 nDeviceID, UINT8 nAlarmID, UINT16 nAddr, UINT8 nExternValue)
{
    struct sAlarmGroup *pPoint;

    pthread_mutex_lock(&alarmsem);
    gDeviceAlarmBuf[(nDeviceID - 1) / 8] = gDeviceAlarmBuf[(nDeviceID - 1) / 8] | (0x01 << ((nDeviceID - 1) % 8));
    E2promWrite(gDeviceAlarmBuf, AlarmStatusAddr, 10);
    pPoint = (struct sAlarmGroup *)malloc(sizeof(struct sAlarmGroup));
    pPoint->nDeviceID = nDeviceID;
    pPoint->nAlarmID = nAlarmID;

    if((nAlarmID == Alarm_Device_Error) && (nAddr == 0x0001) && (nExternValue == 0))
        gDeviceAlarm=gDeviceAlarm |0x0001;
    pPoint->nModbusAddr=nAddr;
    pPoint->nAlarmExternID=nExternValue;

    if(nAlarmID == Alarm_Link_Error)
        pPoint->nAlarmCount = ALARM_SCANF_MAXNUMBER;
    else
        pPoint->nAlarmCount = 11;
	if((nAlarmID == Alarm_Device_Error) && (nAddr == 0x0001) && (nExternValue == 0))
	{
		pPoint->nUploadStatusMasterChannel = ALARM_REPORT_CONFIRMFLAG;
	}
	else
	{
	    //if(pPoint->nUploadStatusMasterChannel<ALARM_REPORT_CONFIRMFLAG)
	    pPoint->nUploadStatusMasterChannel = 1;//必须为1，0的话恢复不上报
	    //if((gModuleChannel1InitFlag==0) && (pPoint->nUploadStatusSlaveChannel<=ALARM_REPORT_CONFIRM_SLAVE))
	    pPoint->nUploadStatusSlaveChannel = ALARM_NOT_REPORT_SLAVE;
	}
    pPoint->pNext = NULL;
    pPoint->pPre = NULL;
    if(gAlarmHead == NULL)
    {
        gAlarmHead = pPoint;
        gAlarmPoint = pPoint;
    }
    else
    {
        gAlarmPoint->pNext = pPoint;
        pPoint->pPre = gAlarmPoint;
        gAlarmPoint = pPoint;
    }
    DbgPrintf("[Alarm Reload]Alarm!!!!!!!---------No.%d Device Alarm No.%d,MBAddr=%d,bit %d is Count %d,pPoint->nUploadStatusMasterChannel is %d,pPoint->nUploadStatusSlaveChannel is %d\r\n"
        , nDeviceID, nAlarmID, nAddr, nExternValue, pPoint->nAlarmCount, pPoint->nUploadStatusMasterChannel, pPoint->nUploadStatusSlaveChannel);
    pthread_mutex_unlock(&alarmsem);
    return pPoint->nAlarmCount;
}

/*****************************************************************************
* Function     : AlarmDelete()
* Description  : delete alarm from list
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
* Output       : None
* Return       : None
* Note(s)      : 告警节点只有恢复确认之后才能删除，删除在AlarmConfirm函数里
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void AlarmDelete(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue)
{
    struct sAlarmGroup *pPoint;

    pthread_mutex_lock(&alarmsem);
    pPoint=gAlarmHead;
    while(pPoint!=NULL)
    {
        if((pPoint->nDeviceID==nDeviceID) && (pPoint->nAlarmID==nAlarmID) && (pPoint->nModbusAddr==nAddr) && (pPoint->nAlarmExternID)==nExternValue)
        {
            DbgPrintf("[AlarmDelete]Alarm Delete DeviceID=%d,AlarmID=%d,address=0x%04X,externID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternValue);
            if((pPoint->nAlarmID==Alarm_Link_Error) && (pPoint->nAlarmCount<=ALARM_SCANF_MAXNUMBER) && (pPoint->nUploadStatusMasterChannel == ALARM_NOT_REPORT))
            {
                pthread_mutex_unlock(&alarmsem);
                AlarmRemove(nDeviceID,nAlarmID,nAddr,nExternValue);
                return;
            }

            if(pPoint->nUploadStatusMasterChannel<=ALARM_REPORT_CONFIRMFLAG)
            {
                UINT8 aSendBuf[256];
                UINT16 nCmdID,nPacketValue,nFlag;

                if((pPoint->nUploadStatusSlaveChannel>ALARM_NOT_REPORT_SLAVE) && (pPoint->nUploadStatusSlaveChannel<ALARM_NOT_RECOVER_SLAVE))
                    pPoint->nUploadStatusSlaveChannel=ALARM_NOT_RECOVER_SLAVE;
                memset(aSendBuf,0,sizeof(aSendBuf));
                aSendBuf[0]=0x68;
                aSendBuf[1]=0x13;
                nPacketValue=gSendCount*2;
                memcpy((UINT8 *)&aSendBuf[2],(UINT8 *)&nPacketValue,2);
                nPacketValue=gRecvCount*2;
                memcpy((UINT8 *)&aSendBuf[4],(UINT8 *)&nPacketValue,2);
                nCmdID = ALARM_REPORT_CMD;
                memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCmdID,1);
                aSendBuf[7]=1;
                nFlag=S_R_Active;
                memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nFlag,2);
                memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&gMainDeviceID,2);

                aSendBuf[15]=nDeviceID;
                aSendBuf[16]=nAlarmID;
                memcpy((UINT8 *)&aSendBuf[17],(UINT8 *)&nAddr,2);
                aSendBuf[19]=nExternValue;
                aSendBuf[20]=0;
                Socket_Send(aSendBuf,21);
                //AddMsgToSGCCSendBuff(aSendBuf,21);
            }

            UINT8 nDeviceTypeTemp;
            nDeviceTypeTemp=gDeviceInfo[pPoint->nDeviceID].nType;
            struct sTypeGroup *pTypeGroupTemp=NULL;
            struct sTypeParam *pTypeParamTemp=NULL;
            UINT8 nYXCount=0,nYCCount=0,nYKCount=0,nSDCount=0,nDDCount=0;
            UINT8 nLoopCount=0;

            gPointTablePossessFlag|=(1<<1);
            pTypeGroupTemp=gTypeHead;
            while(pTypeGroupTemp!=NULL)
            {

                if(pTypeGroupTemp->nTypeID==nDeviceTypeTemp)
                {
                    pTypeParamTemp=pTypeGroupTemp->pParamNext;
                    while(pTypeParamTemp!=NULL)
                    {
                        switch(pTypeParamTemp->nType)
                        {
                            case 1:nYCCount++;break;
                            case 2:nYXCount++;break;
                            case 3:nDDCount++;break;
                            case 4:nYKCount++;break;
                            case 5:nSDCount++;break;
                        }
                        pTypeParamTemp=pTypeParamTemp->pNext;
                    }
                    break;
                }
                else
                {
                    pTypeGroupTemp=pTypeGroupTemp->pNext;
                }
            }
            gPointTablePossessFlag&=~(1<<1);

            for(nLoopCount=0;nLoopCount<nYCCount;nLoopCount++)
            {
                aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYCAddr+nLoopCount].nStatus=0;
            }
            for(nLoopCount=0;nLoopCount<nYXCount;nLoopCount++)
            {
                aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYXAddr+nLoopCount].nStatus=0;
            }
            for(nLoopCount=0;nLoopCount<nDDCount;nLoopCount++)
            {
                aPointBuf[gDeviceInfo[pPoint->nDeviceID].nDDAddr+nLoopCount].nStatus=0;
            }
            //free(pPoint);
            break;
        }
        else
        {
            pPoint=pPoint->pNext;
        }
    }
    pthread_mutex_unlock(&alarmsem);
}

/*****************************************************************************
* Function     : AlarmDeleteAll()
* Description  : delete all alarm from list
* Input        : None
* Output       : None
* Return       : None
* Note(s)      :
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void AlarmDeleteAll()
{
    struct sAlarmGroup *pPoint;

    pthread_mutex_lock(&alarmsem);
    while(gAlarmHead!=NULL)
    {
        pPoint=gAlarmHead;
        gAlarmHead=pPoint->pNext;
        free(pPoint);
    }
    AlarmFileWrite();
    pthread_mutex_unlock(&alarmsem);
}

/*****************************************************************************
* Function     : AlarmConfirmMasterChannel()
* Description  : confirm alarm when recv confirm packet from master platform
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
                 nAlarmStatus: 0-recover  1-occur
* Output       : None
* Return       : None
* Note(s)      : 告警节点只有主、从通道都确认过之后才能删除
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void AlarmConfirmMasterChannel(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternID,UINT8 nAlarmStatus)
{
    struct sAlarmGroup *pPoint,*pLoopPoint;

    pthread_mutex_lock(&alarmsem);
    pPoint=gAlarmHead;
    while(pPoint!=NULL)
    {
        if((pPoint->nDeviceID==nDeviceID) && (pPoint->nAlarmID==nAlarmID) && (pPoint->nModbusAddr==nAddr) && (pPoint->nAlarmExternID)==nExternID)
        {
            if(nAlarmStatus==1)
            {
                DbgPrintf("[Master Channel]Alarm Confirm DeviceID=%d,AlarmID=%d,address=0x%04X,externID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternID);
                if(pPoint->nUploadStatusMasterChannel < ALARM_REPORT_CONFIRMFLAG)
                    pPoint->nUploadStatusMasterChannel = ALARM_REPORT_CONFIRMFLAG;
            }
            else
            {
                DbgPrintf("[Master Channel]Alarm Repair Confirm DeviceID=%d,AlarmID=%d,address=0x%04X,externID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternID);
                pPoint->nUploadStatusMasterChannel=ALARM_RECOVER_CONFIRMFLAG;

                DbgPrintf("[Master Channel]pPoint->nUploadStatusSlaveChannel=%d\r\n",pPoint->nUploadStatusSlaveChannel);
                if((pPoint->nUploadStatusSlaveChannel==ALARM_NOT_REPORT_SLAVE)||(pPoint->nUploadStatusSlaveChannel==ALARM_RECOVER_CONFIRM_SLAVE)
                    ||(pPoint->nAlarmID==Alarm_Device_Error))
                {
                    DbgPrintf("[Master Channel]Delete alarm node!!\r\n");
                    aAlarmInfo[nDeviceID-1][nAddr%10000] &= ~(0x0001<<nExternID);
                    if((nAlarmID==Alarm_Device_Error)&&(nAddr== 0x0001)&&(nExternID==0))
                    {
                        gDeviceAlarm = gDeviceAlarm&0xFFFE;
                        E2promWrite((UINT8 *)&gDeviceAlarm,AlarmLoggerAddr,2);
                    }
                    if((pPoint->pPre==NULL)&&(pPoint->pNext==NULL))
                    {
                        gAlarmHead=NULL;
                        LedSet(5,0);
                    }
                    else if(pPoint->pPre==NULL)
                    {
                        gAlarmHead=pPoint->pNext;
                        gAlarmHead->pPre=NULL;
                    }
                    else if(pPoint->pNext==NULL)
                    {
                        gAlarmPoint=pPoint->pPre;
                        gAlarmPoint->pNext=NULL;
                    }
                    else
                    {
                        pPoint->pPre->pNext=pPoint->pNext;
                        pPoint->pNext->pPre=pPoint->pPre;
                    }
                    pLoopPoint = gAlarmHead;
                    while(pLoopPoint!=NULL)
                    {
                        if(pLoopPoint->nDeviceID==nDeviceID)
                        {
                            break;
                        }
                        else
                        {
                            pLoopPoint=pLoopPoint->pNext;
                        }
                    }
                    if(pLoopPoint==NULL)
                    {
                        gDeviceAlarmBuf[(nDeviceID-1)/8] = gDeviceAlarmBuf[(nDeviceID-1)/8] & (~(0x01<<((nDeviceID-1)%8)));
                        E2promWrite(gDeviceAlarmBuf,AlarmStatusAddr,10);
                    }
                    free(pPoint);
                    AlarmFileWrite();
                }
            }
            break;
        }
        else
        {
            pPoint=pPoint->pNext;
        }
    }
    pthread_mutex_unlock(&alarmsem);
}

/*****************************************************************************
* Function     : AlarmConfirmSlaveChannel()
* Description  : confirm alarm when recv confirm packet from slave platform
* Input        : nDeviceID:south device ID
                 nAlarmID:Alarm type ID
                 nAddr:modbus address
                 nExternValue:offset bit in modbus address
                 nAlarmStatus: 0-recover  1-occur
* Output       : None
* Return       : None
* Note(s)      : 告警节点只有主、从通道都确认过之后才能删除
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void AlarmConfirmSlaveChannel(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternID,UINT8 nAlarmStatus)
{
    struct sAlarmGroup *pPoint,*pLoopPoint;

    pthread_mutex_lock(&alarmsem);
    pPoint=gAlarmHead;
    while(pPoint!=NULL)
    {
        DbgPrintf("[AlarmConfirmSlaveChannel]nDeviceID=%d,nAlarmID=%d,nAddr=%d,nExternID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternID);
        DbgPrintf("[AlarmConfirmSlaveChannel]pPoint->nDeviceID=%d,pPoint->nAlarmID=%d,pPoint->nModbusAddr=%d,pPoint->nAlarmExternID=%d\r\n",pPoint->nDeviceID,pPoint->nAlarmID,pPoint->nModbusAddr,pPoint->nAlarmExternID);
        if((pPoint->nDeviceID==nDeviceID) && (pPoint->nAlarmID==nAlarmID)&& (pPoint->nModbusAddr==nAddr) && (pPoint->nAlarmExternID)==nExternID)
        {
            if(nAlarmStatus==1)
            {
                if(pPoint->nUploadStatusSlaveChannel<ALARM_REPORT_CONFIRM_SLAVE)
                {
                    DbgPrintf("[AlarmConfirmSlaveChannel]Alarm Confirm DeviceID=%d,AlarmID=%d,address=0x%04X,externID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternID);
                    pPoint->nUploadStatusSlaveChannel=ALARM_REPORT_CONFIRM_SLAVE;
                }
                break;
            }
            else
            {
                if((pPoint->nUploadStatusSlaveChannel>=ALARM_NOT_RECOVER_SLAVE)&&(pPoint->nUploadStatusSlaveChannel<ALARM_RECOVER_CONFIRM_SLAVE))
                {
                    DbgPrintf("[AlarmConfirmSlaveChannel]Alarm Recover Confirm DeviceID=%d,AlarmID=%d,address=0x%04X,externID=%d\r\n",nDeviceID,nAlarmID,nAddr,nExternID);
                    pPoint->nUploadStatusSlaveChannel=ALARM_RECOVER_CONFIRM_SLAVE;

                    DbgPrintf("[AlarmConfirmSlaveChannel]pPoint->nUploadStatusMasterChannel=%d\r\n",pPoint->nUploadStatusMasterChannel);
                    if((pPoint->nUploadStatusMasterChannel==ALARM_NOT_REPORT)||(pPoint->nUploadStatusMasterChannel==ALARM_RECOVER_CONFIRMFLAG))
                    {
                        DbgPrintf("[AlarmConfirmSlaveChannel]Delete alarm node!!\r\n");
                        aAlarmInfo[nDeviceID-1][nAddr%10000] &= ~(0x0001<<nExternID);
                        if((nAlarmID==Alarm_Device_Error)&&(nAddr== 0x0001)&&(nExternID==0))
                        {
                            gDeviceAlarm = gDeviceAlarm&0xFFFE;
                            E2promWrite((UINT8 *)&gDeviceAlarm,AlarmLoggerAddr,2);
                        }
                        if((pPoint->pPre==NULL)&&(pPoint->pNext==NULL))
                        {
                            gAlarmHead=NULL;
                            LedSet(5,0);
                        }
                        else if(pPoint->pPre==NULL)
                        {
                            gAlarmHead=pPoint->pNext;
                            gAlarmHead->pPre=NULL;
                        }
                        else if(pPoint->pNext==NULL)
                        {
                            gAlarmPoint=pPoint->pPre;
                            gAlarmPoint->pNext=NULL;
                        }
                        else
                        {
                            pPoint->pPre->pNext=pPoint->pNext;
                            pPoint->pNext->pPre=pPoint->pPre;
                        }
                        pLoopPoint = gAlarmHead;
                        while(pLoopPoint!=NULL)
                        {
                            if(pLoopPoint->nDeviceID==nDeviceID)
                            {
                                break;
                            }
                            else
                            {
                                pLoopPoint=pLoopPoint->pNext;
                            }
                        }
                        if(pLoopPoint==NULL)
                        {
                            gDeviceAlarmBuf[(nDeviceID-1)/8] = gDeviceAlarmBuf[(nDeviceID-1)/8] & (~(0x01<<((nDeviceID-1)%8)));
                            E2promWrite(gDeviceAlarmBuf,AlarmStatusAddr,10);
                        }
                        free(pPoint);
                        AlarmFileWrite();
                    }
                }
                break;
            }
        }
        else
        {
            pPoint=pPoint->pNext;
        }
    }
    pthread_mutex_unlock(&alarmsem);
}

/*****************************************************************************
* Function     : AlarmThreadMasterChannel()
* Description  : alarm dealing thread for master channel
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : 此线程只发送告警，发送告警恢复在AlarmDelete，后续考虑改为最新方案
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void *AlarmThreadMasterChannel()
{
    struct sAlarmGroup *pPoint=NULL;
    UINT8 aSendBuf[256], nCmdID, nLen=0;
    UINT16 nPacketValue, nFlag;

    while(1)
    {
        if(gIsReportInfo == _NO)
        {
            sleep(2);
            continue;
        }
        memset(aSendBuf, 0, sizeof(aSendBuf));
        aSendBuf[0] = 0x68;
        nPacketValue = gSendCount * 2;
        memcpy((UINT8 *)&aSendBuf[2], (UINT8 *)&nPacketValue, 2);
        nPacketValue = gRecvCount * 2;
        memcpy((UINT8 *)&aSendBuf[4], (UINT8 *)&nPacketValue, 2);
        nCmdID = ALARM_REPORT_CMD;
        memcpy((UINT8 *)&aSendBuf[6], (UINT8 *)&nCmdID, 1);
        nFlag=S_R_Active;
        memcpy((UINT8 *)&aSendBuf[8], (UINT8 *)&nFlag, 2);
        memcpy((UINT8 *)&aSendBuf[10], (UINT8 *)&gMainDeviceID, 2);

        nLen = 15;
        pthread_mutex_lock(&alarmsem);
        pPoint = gAlarmHead;

        while((pPoint != NULL) && (nLen < 235))
        {
//        	DbgPrintf("nAlarmCount = %d\n\n",pPoint->nAlarmCount);
            if((pPoint->nAlarmID == Alarm_Link_Error) && (pPoint->nAlarmCount < ALARM_SCANF_MAXNUMBER))
            {
                pPoint = pPoint->pNext;
                continue;
            }

//            DbgPrintf("nUploadStatusMasterChannel = %d\n\n",pPoint->nUploadStatusMasterChannel);
            if(pPoint->nUploadStatusMasterChannel <= ALARM_REPORT_MAXNUMBER)
            {
                aSendBuf[nLen] = pPoint->nDeviceID;
                aSendBuf[nLen+1] = pPoint->nAlarmID;
                memcpy((UINT8 *)&aSendBuf[nLen+2], (UINT8 *)&pPoint->nModbusAddr, 2);
                aSendBuf[nLen+4] = pPoint->nAlarmExternID;
                aSendBuf[nLen+5] = 1;
                nLen += 6;

//                DbgPrintf("nAlarmID = %d,nUploadStatusMasterChannel = %d\n\n", pPoint->nAlarmID, pPoint->nUploadStatusMasterChannel);
                if((pPoint->nAlarmID == Alarm_Link_Error) && (pPoint->nUploadStatusMasterChannel < 2))
                {
                    UINT8 nDeviceTypeTemp=gDeviceInfo[pPoint->nDeviceID].nType;
                    struct sTypeGroup *pTypeGroupTemp=NULL;
                    struct sTypeParam *pTypeParamTemp=NULL;
                    UINT16 nYXCount=0,nYCCount=0,nYKCount=0,nSDCount=0,nDDCount=0;
                    UINT16 nLoopCount=0;

                    pPoint->nUploadStatusMasterChannel = 2;
                    gPointTablePossessFlag|=(1<<3);
                    pTypeGroupTemp=gTypeHead;
                    while(pTypeGroupTemp!=NULL)
                    {
                        if(pTypeGroupTemp->nTypeID==nDeviceTypeTemp)
                        {
                            pTypeParamTemp=pTypeGroupTemp->pParamNext;
                            while(pTypeParamTemp!=NULL)
                            {
                                switch(pTypeParamTemp->nType)
                                {
                                    case 1:nYCCount++; /*DbgPrintf("  nYCCount:%d\n",nYCCount);*/ break;
                                    case 2:nYXCount++;/*DbgPrintf("  nYXCount:%d\n",nYXCount);*/break;
                                    case 3:nDDCount++;break;
                                    case 4:nYKCount++;break;
                                    case 5:nSDCount++;break;
                                }
                                pTypeParamTemp=pTypeParamTemp->pNext;
                            }
                            break;
                        }
                        else
                        {
                            pTypeGroupTemp=pTypeGroupTemp->pNext;
                        }
                    }
                    gPointTablePossessFlag&=~(1<<3);

//                    DbgPrintf("YX=%d,YC=%d,YK=%d\r\n",nYXCount,nYCCount,nYKCount);
                    for(nLoopCount=0;nLoopCount<nYCCount;nLoopCount++)
                    {
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYCAddr+nLoopCount].nValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYCAddr+nLoopCount].nPreValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYCAddr+nLoopCount].nStatus=1;
                    }
                    for(nLoopCount=0;nLoopCount<nYXCount;nLoopCount++)
                    {
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYXAddr+nLoopCount].nValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYXAddr+nLoopCount].nPreValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYXAddr+nLoopCount].nStatus=1;
                    }
                    for(nLoopCount=0;nLoopCount<nDDCount;nLoopCount++)
                    {
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nDDAddr+nLoopCount].nValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nDDAddr+nLoopCount].nPreValue=0xFFFFFFFF;
                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nDDAddr+nLoopCount].nStatus=1;
                    }
//                    for(nLoopCount=0;nLoopCount<nYKCount;nLoopCount++){
//                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYKAddr+nLoopCount].nValue=0xFFFFFFFF;
//                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nYKAddr+nLoopCount].nPreValue=0xFFFFFFFF;
//                    }
//                    for(nLoopCount=0;nLoopCount<nSDCount;nLoopCount++){
//                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nSDAddr+nLoopCount].nValue=0xFFFFFFFF;
//                        aPointBuf[gDeviceInfo[pPoint->nDeviceID].nSDAddr+nLoopCount].nPreValue=0xFFFFFFFF;
//                    }
                }
            }
            pPoint=pPoint->pNext;
        }
        pthread_mutex_unlock(&alarmsem);
        if(nLen != 15)
        {
            aSendBuf[1] = nLen - 2;
            aSendBuf[7] = (nLen - 15) / 6 + 128;
//            printf("gMasterReportSwitch = %d\r\n", gMasterReportSwitch);
            if(gMasterReportSwitch != 1)
                Socket_Send(aSendBuf, nLen);
            //if(g_ethernet_connect_status != 1)
                //AddMsgToSGCCSendBuff(aSendBuf,nLen);
            LedSet(5, 1);
            AlarmFileWrite();
        }
        LedSet(5,0);
        pPoint=gAlarmHead;
        while(pPoint!=NULL)
        {
            if(gDeviceInfo[pPoint->nDeviceID].nInUse == _YES)
                LedSet(5,1);
//            DbgPrintf("[AlarmThreadMasterChannel]pPoint->nDeviceID = %d, pPoint->nAlarmID = %d, pPoint->nModbusAddr = %d\n\n\n", pPoint->nDeviceID, pPoint->nAlarmID, pPoint->nModbusAddr);
            pPoint=pPoint->pNext;
        }
        sleep(30);
    }
}

/*****************************************************************************
* Function     : AlarmThreadSlaveChannel()
* Description  : alarm dealing thread for slave channel
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : 此线程为最新告警方案示例,从通道暂时10次上报,之后修改结构体
* Contributor  : 2019年2月22日        Andre
*****************************************************************************/
void *AlarmThreadSlaveChannel()
{
    struct sAlarmGroup *pPoint=NULL;
    UINT8 aSendBuf[256],nCmdID,nLen=0;
    UINT16 nPacketValue,nFlag;

    while(1)
    {
        if(gModuleChannel1InitFlag == 0)
        {
            //DbgPrintf("[AlarmThreadSlaveChannel]wait!!!\n");
            sleep(5);
            continue;
        }
        memset(aSendBuf,0,sizeof(aSendBuf));
        aSendBuf[0]=0x68;
        nPacketValue=gSendCount*2;
        memcpy((UINT8 *)&aSendBuf[2],(UINT8 *)&nPacketValue,2);
        nPacketValue=gRecvCount*2;
        memcpy((UINT8 *)&aSendBuf[4],(UINT8 *)&nPacketValue,2);
        nCmdID = ALARM_REPORT_CMD;
        memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCmdID,1);
        nFlag=S_R_Active;
        memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nFlag,2);
        memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&gMainDeviceID,2);

        nLen = 15;
        DbgPrintf("[AlarmThreadSlaveChannel]test!!!\n");
        pthread_mutex_lock(&alarmsem);
        pPoint=gAlarmHead;

        DbgPrintf("[AlarmThreadSlaveChannel]start!!!\n");
        while((pPoint!=NULL)&&(nLen<235))
        {
            DbgPrintf("[AlarmThreadSlaveChannel]pPoint->nAlarmCount=%d,pPoint->nUploadStatusMasterChannel=%d,pPoint->nUploadStatusSlaveChannel=%d!!!\n",
                pPoint->nAlarmCount,pPoint->nUploadStatusMasterChannel,pPoint->nUploadStatusSlaveChannel);

            if((pPoint->nAlarmID==Alarm_Link_Error) && (pPoint->nAlarmCount<=ALARM_SCANF_MAXNUMBER))
            {
                pPoint=pPoint->pNext;
                continue;
            }

            if(pPoint->nUploadStatusSlaveChannel<ALARM_REPORT_CONFIRM_SLAVE)
            {
                pPoint->nUploadStatusSlaveChannel=ALARM_REPORT_SLAVE;
                aSendBuf[nLen]=pPoint->nDeviceID;
                aSendBuf[nLen+1]=pPoint->nAlarmID;
                memcpy((UINT8 *)&aSendBuf[nLen+2],(UINT8 *)&pPoint->nModbusAddr,2);
                aSendBuf[nLen+4]=pPoint->nAlarmExternID;
                aSendBuf[nLen+5]=1;
                nLen +=6;
                pPoint=pPoint->pNext;
            }
            else if((pPoint->nUploadStatusSlaveChannel>ALARM_REPORT_CONFIRM_SLAVE)&&(pPoint->nUploadStatusSlaveChannel<ALARM_RECOVER_CONFIRM_SLAVE))
            {
                pPoint->nUploadStatusSlaveChannel=ALARM_RECOVER_SLAVE;
                aSendBuf[nLen]=pPoint->nDeviceID;
                aSendBuf[nLen+1]=pPoint->nAlarmID;
                memcpy((UINT8 *)&aSendBuf[nLen+2],(UINT8 *)&pPoint->nModbusAddr,2);
                aSendBuf[nLen+4]=pPoint->nAlarmExternID;
                aSendBuf[nLen+5]=0;
                nLen +=6;
                pPoint=pPoint->pNext;
            }
            else
                pPoint=pPoint->pNext;
        }
        pthread_mutex_unlock(&alarmsem);
        if(nLen!=15)
        {
            aSendBuf[1]=nLen-2;
            aSendBuf[7]=(nLen-15)/6+128;
            if(gModuleChannel1InitFlag == 2)
                AddMsgToSGCCSendBuff(aSendBuf,nLen);
        }
        DbgPrintf("[AlarmThreadSlaveChannel]sleep 30s!!!\n");
        sleep(30);
    }
}


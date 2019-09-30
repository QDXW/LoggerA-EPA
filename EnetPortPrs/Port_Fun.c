/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : Port_Fun.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/ConstDef.h"
#include "../include/DataTypeDef.h"
#include "../protocal/ProtocolDef.h"
#include "../protocal/ToolExtern.h"
#include "../TPMsgDef/TPMsgDef.h"
#include "../ThreadPool/SPThreadExtern.h"
#include "../ThreadPool/SPThreadPoolExtern.h"
#include "../ThreadPool/SPThreadPool.h"
#include "../interface/interfaceExtern.h"

extern Struct_ThreadPool g_ThreadPool;      //threadpool global identification
extern int g_nRemotesockfd;
extern UINT8 gBufCount;
extern UINT8 gIBufCount;
extern UINT8 g_TransSta;
extern UINT16 gSendCount;
extern UINT8 gSocketMode;
extern struct sSocketRecvBuf *gRecvBufHead;
extern struct sSocketRecvBuf *gRecvBufPoint;
extern struct sSocketRecvBuf *gRecvIBufHead;
extern struct sSocketRecvBuf *gRecvIBufPoint;
extern UINT8 gTotalCallFlag;
extern UINT8 gTimeTotalCallFlag;
extern UINT8 gEleTotalCallFlag;
extern void TotalCallPacket();
extern void TotalTimeCallPacket();

iec104DeviceInfo gDeviceiec104Info[MAXDEVICE];

/*****************************************************************************
* Description:  ��ʼ��ÿһ̨�豸��104��Ϣ������
* Parameters:	void
* Returns:      void
* Author:       Donfulee
* Date:         2019/8/26
*****************************************************************************/
void InitDeviceIecInfo(void)
{
	int k=0;
	UINT8 yxCount=0,ycCount=0,sdCount=0,ykCount=0,ddCount=0,alarmCount=0;
	struct sTypeGroup *typeGroupPoint=NULL;
	struct sTypeParam *typeParamPoint=NULL;

	memset(gDeviceiec104Info,0,sizeof(iec104DeviceInfo)*MAXDEVICE);
	for(;k<MAXDEVICE;k++)
	{
		if(gDeviceInfo[k].nInUse)
		{
			typeGroupPoint=gTypeHead;
            while(typeGroupPoint!=NULL)
            {

                if(typeGroupPoint->nTypeID!=gDeviceInfo[k].nType)
                    typeGroupPoint=typeGroupPoint->pNext;
                else
                    break;
            }

			/*δ�ҵ��豸��Ӧ�ĵ��*/
            if(typeGroupPoint==NULL)
            {
				gDeviceiec104Info[k].uInuse=0;
                continue;
            }

			typeParamPoint=typeGroupPoint->pParamNext;
			gDeviceiec104Info[k].uInuse=1;
			gDeviceiec104Info[k].deviceID=k;
			while(NULL!=typeParamPoint)
			{
				if(Type_104_YX==typeParamPoint->nType)
				{
					yxCount++;
				}
				else if(Type_104_YC==typeParamPoint->nType)
				{
					ycCount++;
				}
				else if(Type_104_YK==typeParamPoint->nType)
				{
					ykCount++;
				}
				else if(Type_104_DD==typeParamPoint->nType)
				{
					ddCount++;
				}
				else if(Type_104_SD==typeParamPoint->nType)
				{
					sdCount++;
				}
				else if(Type_104_Alarm==typeParamPoint->nType)
				{
					alarmCount++;
				}
				typeParamPoint=typeParamPoint->pNext;
			}

			gDeviceiec104Info[k].nYXSum=yxCount;
			gDeviceiec104Info[k].nYCSum=ycCount;
			gDeviceiec104Info[k].nYKSum=ykCount;
			gDeviceiec104Info[k].nSDSum=sdCount;
			gDeviceiec104Info[k].nDDSum=ddCount;
			gDeviceiec104Info[k].nAlarmSum=alarmCount;

            yxCount=0;
            ycCount=0;
            ykCount=0;
            sdCount=0;
            ddCount=0;
            alarmCount=0;
		}
		else
		{
		    memset(&gDeviceiec104Info[k],0,sizeof(iec104DeviceInfo));
		}
	}
}

/*****************************************************************************
* Description:       ����104��ַ�����豸��ַ��modbus��ַ����Ϣ������
* Parameters:        Iec104Addr:104��ַ
					 nAddr:�豸��ַ
                     pAddr:modbus��ַ
* Returns:           ��Ϣ������
*****************************************************************************/
struct sTypeParam *GetDeviceID(UINT16 Iec104Addr,UINT8 *nAddr)
{
    UINT8 nPointCount=0,nType=0xFF,nType1=0xFF;
    struct sTypeParam *typeParaTemp=NULL;
    struct sTypeGroup *typeGroupTemp=NULL;
    int j=0;

	/*����104��ַ�ķ�Χ�ж���Ϣ������*/
	if((Iec104Addr>=0x0001)&&(Iec104Addr<0x4001))
	{
		nType  = Type_104_YX;
	}
	else if((Iec104Addr>=0x4001)&&(Iec104Addr<0x5001))
	{
		nType  = Type_104_YC;
		nType1 = 0xFF;
	}
	else if((Iec104Addr>=0x6001)&&(Iec104Addr<0x6101))
	{
		nType  = Type_104_YK;
	}
	else if((Iec104Addr>=0x6201)&&(Iec104Addr<0x6401))
	{
		nType  = Type_104_SD;
		nType1 = 0x08;
	}

	for(j=1;j<MAXDEVICE;j++)
	{
		if(1!=gDeviceInfo[j].nInUse)
		{
			continue;
		}

		if(Type_104_YX==nType)
		{
			if((Iec104Addr>=gDeviceInfo[j].nYXAddr)&&(Iec104Addr<(gDeviceInfo[j].nYXAddr+gDeviceiec104Info[j].nYXSum)))
			{
				nPointCount=Iec104Addr-gDeviceInfo[j].nYXAddr;
				break;
			}
		}
		else if(Type_104_YC==nType)
		{
			if((Iec104Addr>=gDeviceInfo[j].nYCAddr)&&(Iec104Addr<(gDeviceInfo[j].nYCAddr+gDeviceiec104Info[j].nYCSum)))
			{
				nPointCount=Iec104Addr-gDeviceInfo[j].nYCAddr;
				break;
			}
		}
		else if(Type_104_YK==nType)
		{
			if((Iec104Addr>=gDeviceInfo[j].nYKAddr)&&(Iec104Addr<(gDeviceInfo[j].nYKAddr+gDeviceiec104Info[j].nYKSum)))
			{
				nPointCount=Iec104Addr-gDeviceInfo[j].nYKAddr;
				break;
			}
		}
		else if(Type_104_SD==nType)
		{
			if((Iec104Addr>=gDeviceInfo[j].nSDAddr)&&(Iec104Addr<(gDeviceInfo[j].nSDAddr+gDeviceiec104Info[j].nSDSum)))
			{
				nPointCount=Iec104Addr-gDeviceInfo[j].nSDAddr;
				break;
			}
		}
	}

	if(j>=MAXDEVICE)
		return 0xFF;

	/*�����豸���*/
	typeGroupTemp = gTypeHead;
	while(NULL != typeGroupTemp)
	{
		if(typeGroupTemp->nTypeID != gDeviceInfo[j].nType)
			typeGroupTemp=typeGroupTemp->pNext;
		else
		{
			break;
		}
	}

	/*�豸���Ϊ��*/
	if(NULL == typeGroupTemp)
		return NULL;
	typeParaTemp=typeGroupTemp->pParamNext;				/*��ȡ���ĵ�һ���źŵ�*/
	/*�����豸���,����ƫ����,��ȡ104��ַ��Ӧ����Ϣ������*/
    while(NULL != typeParaTemp)
    {
        if((typeParaTemp->nType==nType)||(typeParaTemp->nType==nType1))
        {
            if(nPointCount==0)
            {
//                memcpy(MBAddr,(UINT8 *)&(typeParaTemp->nMBAddr),2);
				*nAddr = j;
//				if(Type_104_SD==nType)
//				{
//					if(1==typeParaTemp->nLen)
//						return nType;
//					else
//						return nType1;
//				}
//				else
//                	return typeParaTemp->nType;
				return typeParaTemp;
            }
            --nPointCount;
        }
       	typeParaTemp=typeParaTemp->pNext;
    }
	return NULL;
}

/*****************************************************************************
* Description: 		�����յ����ݴ������ݴ洢����
* Parameters:        nType: 1:I֡  0:S/U֡
                     aBuf:  ���յ�����
                     nLen:  ���յ����ݳ���
* Returns:
*****************************************************************************/
void RecvBufferAdd(UINT8 nType,UINT8 *aBuf,UINT8 nLen)
{
	UINT8 j;
	struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    pRecvPoint=(struct sSocketRecvBuf *)malloc(sizeof(struct sSocketRecvBuf));/*�������ݿռ�*/
    memcpy(pRecvPoint->aDataBuf,aBuf,nLen);
    pRecvPoint->nLen=nLen;
    pRecvPoint->nType=nType;
    pRecvPoint->pNext=NULL;
    pRecvPoint->pPre=NULL;
    if(gRecvBufHead==NULL)/*�洢�����޻�������,ֱ�ӽ����ݼ���洢��*/
    {
        gRecvBufHead=pRecvPoint;
        gRecvBufPoint=pRecvPoint;
    }
    else/*��������,�����ݷ���洢��ĩβ*/
    {
        gRecvBufPoint->pNext=pRecvPoint;
        pRecvPoint->pPre=gRecvBufPoint;
        gRecvBufPoint=pRecvPoint;
    }
    gBufCount++;/*�洢�����ݼ����ۼ�*/
    NorthPrintf("Save %d Byte:",pRecvPoint->nLen);
    for(j=0;j<pRecvPoint->nLen;j++)
    {
        NorthPrintf("%02X ",pRecvPoint->aDataBuf[j]);
    }
    NorthPrintf("to Buffer No.%d\r\n",gBufCount);
    pthread_mutex_unlock(&socketbufsem);
}

/*****************************************************************************
* Description:         ���������ݴ洢���е�����
* Parameters:
* Returns:
*****************************************************************************/
void RecvBufferDeal(void)
{
    UINT8 nRes=0,nFlag=0;
    StrSocketRecvInfo aDataTemp;
    struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    nFlag=0;/*0:δ��ȡ������ 1:��ȡ������*/
    pRecvPoint = gRecvBufHead;
    while(pRecvPoint!=NULL)/*�����ݴ洢����Ϊ��ʱ*/
    {
        //if((g_TransSta==0)||(pRecvPoint->nType==0))/*����������״̬Ϊ��������ʱ*/
        //if(pRecvPoint->nType==0)
        {
            DbgPrintf("Buffer %03d Byte to be Deal:",pRecvPoint->nLen);
            gBufCount--;/*�洢�����ݼ�������*/
            memcpy((UINT8 *)&aDataTemp.aDataBuf,(UINT8 *)&pRecvPoint->aDataBuf,pRecvPoint->nLen);
            aDataTemp.nType=pRecvPoint->nType;
            aDataTemp.nLen=pRecvPoint->nLen;
            if((pRecvPoint->pPre==NULL)&&(pRecvPoint->pNext==NULL))/*��������Ϊ�洢�����һ������ʱ*/
            {
                gRecvBufHead=NULL;
                gRecvBufPoint=NULL;
            }
            else if(pRecvPoint->pPre==NULL)/*��������Ϊ�洢�����ݶ��е�һ��*/
            {
                gRecvBufHead=pRecvPoint->pNext;
                gRecvBufHead->pPre=NULL;
            }
            else if(pRecvPoint->pNext==NULL)/*��������Ϊ�洢�����ݶ���ĩβ*/
            {
                gRecvBufPoint=pRecvPoint->pPre;
                gRecvBufPoint->pNext=NULL;
            }
            else/*��������Ϊ�洢�����ݶ����м�*/
            {
                pRecvPoint->pPre->pNext=pRecvPoint->pNext;
                pRecvPoint->pNext->pPre=pRecvPoint->pPre;
            }
            nFlag=1;
            free(pRecvPoint);
            break;
        }
        /*else if((g_TransSta!=1))//����ʱ����������
        {
            pRecvPoint=pRecvPoint->pNext;
        }*/
    }
    pthread_mutex_unlock(&socketbufsem);
    if(nFlag)
    {
        nRes=PackMainFunction(aDataTemp.nType,aDataTemp.aDataBuf,aDataTemp.nLen);/*���ô�����*/
    }
}

/*****************************************************************************
* Description:     �߳�:��ⱱ�����ݴ洢��������
* Parameters:
* Returns:
*****************************************************************************/
void *SocketRecvbufThread(void)
{
    while(1)
    {
        /**
        *  ��Pro_Monitor�߳��л�ͨ����̫�����Կͻ��˵��������ƽ̨������ɹ���g_nRemotesockfd����һ����0ֵ.
        */
        if((g_nRemotesockfd!=0) ||(gSocketMode!=0))/*����ģʽ���������� ���� ����ģʽ*/
        {
            RecvBufferDeal();/*�������ݴ洢������*/
            if((gTotalCallFlag>0)||(gEleTotalCallFlag>0))/*��������״̬*/
            {
                //if((gSendCount>0)&&(gSendCount%8!=0))
                {
                    TotalCallPacket();/*�������ٴ���*/
                }
            }
            if(gTimeTotalCallFlag>0)/*���ڲ���״̬*/
            {
                {
                    TotalTimeCallPacket();/*���в��ɴ���*/
                }
            }
        }
        usleep(100);
    }
}

/*****************************************************************************
* Description: �߳�:����Socket���յ�������
* Parameters:
* Returns:
*****************************************************************************/
void *Fun_Thread_Port(void *param)
{
    int nPos = (int)param;
    Struct_SPThreadInofPtr pThreadInfo = &g_ThreadPool.arrThreadType_Port[nPos]; //get thread info
    Struct_Port_Msg message;                     //get message from port  TPMsgDef.h
    int pid=pThreadInfo->nMsgQueueID;
    UINT8 aTemp[SYS_FRAME_LEN];

    while(1)
    {
        bzero(message.PortMsgBuf, MaxMsgBufLength);
        //get info from port queue
        if((msgrcv(pid,(void *)&message,Port_Msg_Length(Struct_Port_Msg),0,0))<0)  /*��ȡ�����̴߳�����������*/
        {
            DbgPrintf("logic_prs msgrcv error!\n");
            exit(1);
        }

        int nLen;
        nLen = 0;
        do
        {
            UINT8 nTempLen;

            memset(aTemp,sizeof(aTemp),0);
            nTempLen=message.PortMsgBuf[nLen+1];
            memcpy(aTemp,(UINT8 *)&message.PortMsgBuf[nLen],nTempLen+2);
            gSocketHeartChannel0Count=0;
            if((aTemp[2]&0x01)==0)/*I֡�Ĵ���*/
            {
                RecvBufferAdd(1,aTemp,nTempLen+2);
            }
            else/*S֡�Ĵ���*/
            {
                RecvBufferAdd(0,aTemp,nTempLen+2);
            }
            nLen = nLen+nTempLen+2;
        }while(nLen<message.nDateLength);
    }
}

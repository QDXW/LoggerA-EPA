/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : xml.c
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "xml.h"
#include "../protocal/ProtocolDef.h"
#include "../interface/common.h"
#include "../Alarm/alarmExtern.h"

extern sDeviceInfo gDeviceInfo[MAXDEVICE];
extern struct sTypeGroup *gTypeHead;
extern struct sTypeGroup *gTypeGroupPoint;
extern struct sTypeParam *gTypeParamPoint;
extern UINT8 gDeviceTypeNum;
extern struct sAlarmGroup *gAlarmHead;
extern UINT16 gDeviceAlarm;
extern UINT16 aAlarmInfo[40][20];

/*****************************************************************************
* Description:      获取文件大小
* Parameters:       path:文件路径
* Returns:          文件大小
*****************************************************************************/
unsigned long GetFileSize(const char *path)
{
    unsigned long filesize=-1;
    struct stat statbuf;

    if(stat(path,&statbuf)<0)
    {
        return filesize;
    }
    else
    {
        filesize=statbuf.st_size;
    }
    return filesize;
}

/*****************************************************************************
* Description:      获取设备基本信息
* Parameters:
* Returns:
*****************************************************************************/
void ReadTagBaseFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;

    fTagBasefd=open(TagBaseFilePath,O_RDONLY);
    DbgPrintf("ReadTagBaseFile-----Get File size=");
    filesize =GetFileSize(TagBaseFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)//文件大于0时进行读取
    {
        pHead=malloc(filesize);//申请文件数据缓存
        pPoint=pHead;//指针指向缓存首部
        if(read(fTagBasefd,pHead,filesize)>0)//读取文件信息
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//检测是否还有设备信息存在
            {
                int nDeviceID;

                //获取设备ID
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);
                if(strstr((void *)pPoint,TagUseHead)==NULL)
                {
                    DbgPrintf("There is no PointAddrHead");
                    return;
                }
                //获取设备使用状态 0:未使用 1:使用
                pPoint = (void *)strstr((void *)pPoint,TagUseHead);
                pPoint +=strlen(TagUseHead);
                if(strstr((void *)pPoint,TagUseEnd)==NULL)
                {
                    DbgPrintf("There is no TagUseEnd");
                    return;
                }
                gDeviceInfo[nDeviceID].nInUse=pPoint[0]-0x30;
                if(((pPoint[0]-0x30)==1) && (nDeviceID<=40))//当设备使用中,并且设备编号不超过40时
                {
                    int i;

                    gDeviceInfo[nDeviceID].nInUse=1;
                    if(strstr((void *)pPoint,TagESNHead)==NULL)
                    {
                        DbgPrintf("There is no TagESNHead");
                        return;
                    }
                    //获取设备ESN
                    pPoint = (void *)strstr((void *)pPoint,TagESNHead);
                    pPoint +=strlen(TagESNHead);
                    if(strstr((void *)pPoint,TagESNEnd)==NULL)
                    {
                        DbgPrintf("There is no TagESNEnd");
                        return;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagESNEnd);
                    memset(gDeviceInfo[nDeviceID].aESN,0,21);
                    if(pEnd-pPoint>20)
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aESN,pPoint,20);
                    else
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aESN,pPoint,pEnd-pPoint);
                    if(strstr((void *)pPoint,TagTableHead)==NULL)
                    {
                        DbgPrintf("There is no TagTableHead");
                        return;
                    }
                    //获取设备点表编号
                    pPoint = (void *)strstr((void *)pPoint,TagTableHead);
                    pPoint +=strlen(TagTableHead);
                    if(strstr((void *)pPoint,TagTableEnd)==NULL)
                    {
                        DbgPrintf("There is no TagTableEnd");
                        return;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagTableEnd);
                    for(i=0;i<pEnd-pPoint;i++)
                    {
                        gDeviceInfo[nDeviceID].nPointTableNo=gDeviceInfo[nDeviceID].nPointTableNo+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                    }

                    //获取设备连接端口信息
                    if((strstr((void *)pPoint,TagPortHead)!=NULL)&&(strstr((void *)pPoint,TagPortEnd)!=NULL))
                    {
                        pPoint = (void *)strstr((void *)pPoint,TagPortHead);
                        pPoint +=strlen(TagPortHead);
                        gDeviceInfo[nDeviceID].nDownlinkPort=HsCharToInt(pPoint[0]);
                    }
                    //获取设备协议类型信息
                    if((strstr((void *)pPoint,TagProtocolHead)!=NULL)&&(strstr((void *)pPoint,TagProtocolEnd)!=NULL))
                    {
                        pPoint = (void *)strstr((void *)pPoint,TagProtocolHead);
                        pPoint +=strlen(TagProtocolHead);
                        gDeviceInfo[nDeviceID].nProtocolType=HsCharToInt(pPoint[0]);
                    }
                    //获取逆变器软件版本号
                    if((strstr((void *)pPoint,TagSoftVsersionHead)==NULL))
                    {
                        DbgPrintf("There is no TagSoftVsersionHead");
                        continue;
                    }
                    pPoint = (void *)strstr((void *)pPoint,TagSoftVsersionHead);
                    pPoint +=strlen(TagSoftVsersionHead);
                    if(strstr((void *)pPoint,TagSoftVsersionEnd)==NULL)
                    {
                        DbgPrintf("There is no TagSoftVsersionEnd");
                        continue;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagSoftVsersionEnd);
                    memset(gDeviceInfo[nDeviceID].aSofeVersion,0,21);
                    if(pEnd-pPoint>20)
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aSofeVersion,pPoint,20);
                    else
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aSofeVersion,pPoint,pEnd-pPoint);
                }
            }
        }
        free(pHead);
    }
    close(fTagBasefd);
}

/*****************************************************************************
* Description:      获取设备点表基本信息
* Parameters:
* Returns:
*****************************************************************************/
void ReadTagInfoFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;

    fTagInfofd=open(TagInfoFilePath,O_RDONLY);
    DbgPrintf("ReadTagInfoFile-----Get File size=");
    filesize =GetFileSize(TagInfoFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)//文件大于0时进行读取
    {
        pHead=malloc(filesize);//申请缓存空间
        pPoint=pHead;
        if(read(fTagBasefd,pHead,filesize)>0)//获取文件数据
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//检测是否还有设备信息存在
            {
                int nDeviceID;

                //获取设备编号
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);//Get DeviceID
                if((gDeviceInfo[nDeviceID].nInUse==1) && (nDeviceID<=40))//当设备使用中,并且设备编号不超过40时
                {
                    int i;

                    gDeviceInfo[nDeviceID].nInUse=1;

                    //获取设备类型编号,等同于设备点表编号
                    pPoint = (void *)strstr((void *)pPoint,TagTypeHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagTypeHead);

                    pEnd = (void *)strstr((void *)pPoint,TagTypeEnd);
                    for(i=0;i<pEnd-pPoint;i++)
                    {
                        gDeviceInfo[nDeviceID].nType=gDeviceInfo[nDeviceID].nType+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                    }

                    //获取设备遥信起始地址
                    pPoint = (void *)strstr((void *)pPoint,TagYXHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYXHead);

                    gDeviceInfo[nDeviceID].nYXAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //获取设备遥测起始地址
                    pPoint = (void *)strstr((void *)pPoint,TagYCHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYCHead);

                    gDeviceInfo[nDeviceID].nYCAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //获取设备遥控起始地址
                    pPoint = (void *)strstr((void *)pPoint,TagYKHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYKHead);

                    gDeviceInfo[nDeviceID].nYKAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //获取设备设点起始地址
                    pPoint = (void *)strstr((void *)pPoint,TagSDHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagSDHead);

                    gDeviceInfo[nDeviceID].nSDAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //获取设备电度起始地址
                    pPoint = (void *)strstr((void *)pPoint,TagDDHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagDDHead);

                    gDeviceInfo[nDeviceID].nDDAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);
                }
            }
        }
        free(pHead);
    }
    close(fTagBasefd);
}

/*****************************************************************************
* Description:      保存设备基本信息
* Parameters:
* Returns:
*****************************************************************************/
void TagBaseFileWrite(void)
{
    UINT8 nDeviceCount;
    UINT8 aBuf[50];
    FILE *fd=fopen(TagBaseFilePath,"w+");

    //写入文件头
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",FileHead);
    fwrite(aBuf,1,strlen((void *)aBuf),fd);
    //写入XML文件结构体
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);

    for(nDeviceCount=1;nDeviceCount<=40;nDeviceCount++)
    {
        if(gDeviceInfo[nDeviceCount].nInUse==0)
        {
            continue;
        }
        //写入设备编号
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入使用状态
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagUseRegex,1);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入ESN
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagESNRegex,gDeviceInfo[nDeviceCount].aESN);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备类型编号
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagTableRegex,gDeviceInfo[nDeviceCount].nType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备连接COM口
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagPortRegex,gDeviceInfo[nDeviceCount].nDownlinkPort);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备协议类型
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagProtocolRegex,gDeviceInfo[nDeviceCount].nProtocolType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入逆变器软件版本号
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagSoftVsersionRegex,gDeviceInfo[nDeviceCount].aSofeVersion);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);

        //写入本设备结束节点
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);

        //保存设备大小端
        gDeviceInfo[nDeviceCount].nEndian=0;
        E2promWrite((UINT8 *)&gDeviceInfo[nDeviceCount].nEndian,DeviceTypeInfoAddr+(nDeviceCount-1)*5+4,1);
        //保存设备波特率
        gDeviceInfo[nDeviceCount].nRate=9600;
        E2promWrite((UINT8 *)&gDeviceInfo[nDeviceCount].nRate,DeviceTypeInfoAddr+(nDeviceCount-1)*5,4);
    }
    //写入XML文件结构体尾
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);
    fclose(fd);
}

/*****************************************************************************
* Description:      保存设备点表基本信息
* Parameters:
* Returns:
*****************************************************************************/
void TagInfoFileWrite(void)
{
    UINT8 nDeviceCount;
    UINT8 aBuf[50];
    FILE *fd=fopen(TagInfoFilePath,"w+");

    //写入文件头
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",FileHead);
    fwrite(aBuf,1,strlen((void *)aBuf),fd);

    //写入XML文件结构体
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);

    for(nDeviceCount=1;nDeviceCount<=40;nDeviceCount++)
    {
        if(gDeviceInfo[nDeviceCount].nInUse==0)
        {
            continue;
        }
        //写入设备编号
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备类型编号
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagTypeRegex,gDeviceInfo[nDeviceCount].nType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备遥信起始地址
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYXRegex,gDeviceInfo[nDeviceCount].nYXAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备遥测起始地址
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYCRegex,gDeviceInfo[nDeviceCount].nYCAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备遥控起始地址
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYKRegex,gDeviceInfo[nDeviceCount].nYKAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备设点起始地址
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagSDRegex,gDeviceInfo[nDeviceCount].nSDAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备电度起始地址
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagDDRegex,gDeviceInfo[nDeviceCount].nDDAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //写入设备结束节点
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
    }
    //写入XML文件结束结构体
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);
    fclose(fd);
}

/*****************************************************************************
* Description:      保存点表信息
* Parameters:
* Returns:
*****************************************************************************/
void PointInfoFileWrite(void)
{
    UINT8 aBuf[50];
    FILE *fd;
    struct sTypeGroup *pTypeGroup=NULL;
    struct sTypeParam *pTypeParam=NULL;

    pTypeGroup=gTypeHead;//指向点表缓存区
    while(pTypeGroup!=NULL)//点表类型不为空
    {
        pTypeParam=pTypeGroup->pParamNext;//获取该点表第一个信号点
        if(pTypeParam!=NULL)
        {
            UINT8 nDeviceCount=0;//信号点计数
            //根据点表编号建立点表文件
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,PointFilePath,pTypeGroup->nTypeID);
            fd=fopen((void *)aBuf,"w+");
            //写入文件头
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",FileHead);
            fwrite(aBuf,1,strlen((void *)aBuf),fd);
            //写入点表协议类型
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,ProtocolRegex,pTypeGroup->nProtocalTypeID);
            fwrite(aBuf,1,strlen((void *)aBuf),fd);
            //写入XML文件结构体
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
            fwrite(aBuf,strlen((void *)aBuf),1,fd);
            while(pTypeParam!=NULL)
            {
                //写入信号点编号
                nDeviceCount++;
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //写入信号点MODBUS地址
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointAddrRegex,pTypeParam->nMBAddr);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //写入信号点寄存器长度
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointLenRegex,pTypeParam->nLen);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //写入信号点对应104规约类型
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointTypeRegex,pTypeParam->nType);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //写入信号点数据类型
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointDataTypeRegex,pTypeParam->nDataType);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //写入信号点结束节点
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                pTypeParam=pTypeParam->pNext;
            }
            //写入XML文件结构体结束
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
            fwrite(aBuf,strlen((void *)aBuf),1,fd);
            fclose(fd);
            pTypeGroup=pTypeGroup->pNext;
        }
    }
}

/*****************************************************************************
* Description:      获取点表信息
* Parameters:
* Returns:
*****************************************************************************/
void ReadPointTableFile(void)
{
    UINT8 i,j;
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    UINT8 nPointFlag=0;

    E2promRead((UINT8 *)&gDeviceTypeNum,TypeNumberAddr,1);//获取数采点表数
    if(gDeviceTypeNum==255)
        gDeviceTypeNum=0;
    DbgPrintf("TypeNum = %d\r\n",gDeviceTypeNum);
    //逐一获取全部点表的信息
    E2promRead((UINT8 *)&nPointFlag,UpdateRestoreAddr,1);//获取点表编号
    for(i=0;i<gDeviceTypeNum;i++)
    {
        UINT16 nTableNum;
        UINT8 nTableProtocolType=0;
        UINT8 aBuf[50];

        E2promRead((UINT8 *)&nTableNum,0x6A5+i*2,2);//获取点表编号
        if(nTableNum==0xFFFF)
            continue;
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,PointFilePath,nTableNum);
        //printf("PointFilePath = %s\r\n", aBuf);
        fTypePointfd=open((void *)aBuf,O_RDONLY);
        DbgPrintf("ReadPointTypeFile-----Get File size=");
        filesize =GetFileSize((void *)aBuf);//获取点表文件大小
        DbgPrintf("%ld\r\n",filesize);
        if(filesize>0)
        {
            pHead=malloc(filesize);
            pPoint=pHead;
            if(read(fTypePointfd,pHead,filesize)>0)
            {
                //获取点表协议类型
                if((strstr((void *)pPoint,ProtocolHead)!=NULL)&&(strstr((void *)pPoint,ProtocolEnd)!=NULL))
                {
                    pPoint = (void *)strstr((void *)pPoint,ProtocolHead);
                    pPoint +=strlen(ProtocolHead);
                    pEnd = (void *)strstr((void *)pPoint,ProtocolEnd);
                    for(j=0;j<(pEnd-pPoint);j++)
                    {
                        nTableProtocolType = nTableProtocolType+HsCharToInt(pPoint[j])*pow(10,(pEnd-pPoint)-j-1);
                    }
                }
                else
                    nTableProtocolType=1;
                TypeGroupAdd(1,nTableNum,nTableProtocolType);
                //printf("ADD Point Table %d\r\n",nTableNum);
                while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//逐一获取点表每个信号点的信息
                {
                    UINT8 nParaLen,nParaType,nParaDataType;
                    UINT16 nAddr=0;
                    if(strstr((void *)pPoint,">")==NULL)
                    {
                        DbgPrintf("There is no TagEnd >");
                        return;
                    }
                    //写入信号点MODBUS地址
                    pPoint +=strlen("<Tag id=\"");
                    if(strstr((void *)pPoint,PointAddrHead)==NULL)
                    {
                        DbgPrintf("There is no PointAddrHead");
                        return;
                    }
                    pPoint = (void *)strstr((void *)pPoint,PointAddrHead);
                    if(strstr((void *)pPoint,PointAddrEnd)==NULL)
                    {
                        DbgPrintf("There is no PointAddrEnd");
                        return;
                    }
                    pPoint +=strlen(PointAddrHead);
                    pEnd = (void *)strstr((void *)pPoint,PointAddrEnd);
                    for(j=0;j<(pEnd-pPoint);j++)
                    {
                        nAddr = nAddr+HsCharToInt(pPoint[j])*pow(10,(pEnd-pPoint)-j-1);
                    }
                    if(strstr((void *)pPoint,PointLenHead)==NULL)
                    {
                        DbgPrintf("There is no PointLenHead");
                        return;
                    }
                    //获取信号点的寄存器长度
                    pPoint = (void *)strstr((void *)pPoint,PointLenHead);
                    pPoint +=strlen(PointLenHead);
                    if(strstr((void *)pPoint,PointLenEnd)==NULL)
                    {
                        DbgPrintf("There is no PointLenEnd");
                        return;
                    }
                    nParaLen=HsCharToInt(pPoint[0])*pow(10,1)+HsCharToInt(pPoint[1])*pow(10,0);
                    if(strstr((void *)pPoint,PointTypeHead)==NULL)
                    {
                        DbgPrintf("There is no PointTypeHead");
                        return;
                    }
                    //获取信号点对应104的类型
                    pPoint = (void *)strstr((void *)pPoint,PointTypeHead);
                    pPoint +=strlen(PointTypeHead);
                    if(strstr((void *)pPoint,PointTypeEnd)==NULL)
                    {
                        DbgPrintf("There is no PointTypeEnd");
                        return;
                    }
                    nParaType=HsCharToInt(pPoint[0]);
                    if(strstr((void *)pPoint,PointDataTypeHead)==NULL)
                    {
                        DbgPrintf("There is no PointDataTypeHead");
                        return;
                    }
                    //写入信号点数据类型
                    pPoint = (void *)strstr((void *)pPoint,PointDataTypeHead);
                    pPoint +=strlen(PointDataTypeHead);
                    if(strstr((void *)pPoint,PointDataTypeEnd)==NULL)
                    {
                        DbgPrintf("There is no PointDataTypeEnd");
                        return;
                    }
                    nParaDataType=HsCharToInt(pPoint[0]);
                    /*printf("{%d,%d,%d},",nAddr,nParaLen,nParaType);*/
                    TypeParamAdd(1,nAddr,nParaLen,nParaType,nParaDataType);
                }
            }
            free(pHead);
        }
        close(fTypePointfd);
    }
    /*
    pTypeGroup = gTypeHead;
    while(pTypeGroup!=NULL)
    {
        pTypeParam=pTypeGroup->pParamNext;
        if(pTypeParam!=NULL)
        {
            while(pTypeParam!=NULL)
            {
                pTypeParam=pTypeParam->pNext;
            }
            pTypeGroup=pTypeGroup->pNext;
        }
    }*/
    gTypeGroupPoint=NULL;
}

void AlarmFileWrite(void)
{
    UINT8 aBuf[50];
    FILE *fd;
    struct sAlarmGroup *pPoint;

    //printf("1\r\n");
    pPoint = gAlarmHead;
    fd=fopen(AlarmFilePath,"w+");
    if(fd != 0)
    {
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",FileHead);
        fwrite(aBuf,1,strlen((void *)aBuf),fd);
        while(pPoint!=NULL)
        {
            if((pPoint->nUploadStatusMasterChannel>=2)&&(pPoint->nUploadStatusMasterChannel<=5))
            {
                //printf("%d %d %d %d\r\n",pPoint->nDeviceID,pPoint->nAlarmID,pPoint->nModbusAddr,pPoint->nAlarmExternID);
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,TagIDRegex,pPoint->nDeviceID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmTypeRegex,pPoint->nAlarmID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmAddrRegex,pPoint->nModbusAddr);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmOffsetRegex,pPoint->nAlarmExternID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
            }
            pPoint=pPoint->pNext;
        }
        //printf("add end\r\n");
        fclose(fd);
    }
    else
    {
        DbgPrintf("[Alarm]Open alarm file fail!!!\r\n");
    }
}

void ReadAlarmFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    int fd;
    struct sAlarmGroup *pAlarmBufPoint;

    pthread_mutex_lock(&alarmsem);
    while(gAlarmHead!=NULL)
    {
        pAlarmBufPoint=gAlarmHead;
        gAlarmHead=pAlarmBufPoint->pNext;
        free(pAlarmBufPoint);
    }
    pthread_mutex_unlock(&alarmsem);

    memset(aAlarmInfo,0,sizeof(aAlarmInfo));
    if((access(AlarmFilePath,F_OK)==-1))
    {
        gAlarmHead=NULL;
        return;
    }
    fd=open(AlarmFilePath,O_RDONLY);
    DbgPrintf("ReadAlarmInfoFile-----Get File size=");
    filesize =GetFileSize(AlarmFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)
    {

        pHead=malloc(filesize);
        pPoint=pHead;
        if(read(fd,pHead,filesize)>0)
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)
            {
                UINT8 i,nDeviceID=0,nAlarmID=0,nOffSet=0;
                UINT16 nAlarmAddr;

                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);
                if(strstr((void *)pPoint,AlarmTypeHead)==NULL)
                {
                    DbgPrintf("There is no AlarmTypeHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmTypeHead);
                pPoint +=strlen(AlarmTypeHead);
                if(strstr((void *)pPoint,AlarmTypeEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmTypeEnd");
                    return;
                }
                pEnd = (void *)strstr((void *)pPoint,AlarmTypeEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nAlarmID=nAlarmID+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }
                if(strstr((void *)pPoint,AlarmAddrHead)==NULL)
                {
                    DbgPrintf("There is no AlarmAddrHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmAddrHead);
                pPoint +=strlen(AlarmAddrHead);
                if(strstr((void *)pPoint,AlarmAddrEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmAddrEnd");
                    return;
                }
                nAlarmAddr=HsCharToInt(pPoint[0])*pow(16,3)
                            +HsCharToInt(pPoint[1])*pow(16,2)
                            +HsCharToInt(pPoint[2])*pow(16,1)
                            +HsCharToInt(pPoint[3])*pow(16,0);
                if(strstr((void *)pPoint,AlarmOffsetHead)==NULL)
                {
                    DbgPrintf("There is no AlarmOffsetHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmOffsetHead);
                pPoint +=strlen(AlarmOffsetHead);
                if(strstr((void *)pPoint,AlarmOffsetEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmOffsetEnd");
                    return;
                }
                pEnd = (void *)strstr((void *)pPoint,AlarmOffsetEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nOffSet=nOffSet + HsCharToInt(pPoint[i]) * pow(10, pEnd - pPoint - i - 1);
                }
                AlarmReload(nDeviceID,nAlarmID,nAlarmAddr,nOffSet);
                if(nAlarmID == 2)
                {
                	switch(nAlarmAddr/100)
					{
						case 407:
							aAlarmInfo[nDeviceID-1][nAlarmAddr%10] &= ~(0x0001<<nOffSet);
							break;

						case 655:
							aAlarmInfo[nDeviceID-1][11] &= ~(0x0001<<nOffSet);
							break;
						default:
							aAlarmInfo[nDeviceID-1][12] &= ~(0x0001<<nOffSet);
							break;
					}
                }
//                    aAlarmInfo[nDeviceID-1][nAlarmAddr%10000] |=  (0x0001<<nOffSet);
            }
        }
        free(pHead);
    }
    close(fd);
}

void ReloadAlarmFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    int fd;
    struct sAlarmGroup *pAlarmBufPoint;

    memset(aAlarmInfo,0,sizeof(aAlarmInfo));
    if((access(AlarmFilePath,F_OK)==-1))
    {
        gAlarmHead=NULL;
        return;
    }
    fd=open(AlarmFilePath,O_RDONLY);
    DbgPrintf("ReadAlarmInfoFile-----Get File size=");
    filesize =GetFileSize(AlarmFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)
    {
        pHead=malloc(filesize);
        pPoint=pHead;
        if(read(fd,pHead,filesize)>0)
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)
            {
                UINT8 i,nDeviceID=0,nAlarmID=0,nOffSet=0;
                UINT16 nAlarmAddr;
                UINT8 nReloadFlag=0;

                pAlarmBufPoint=gAlarmHead;
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);

                pPoint = (void *)strstr((void *)pPoint,AlarmTypeHead);
                pPoint +=strlen(AlarmTypeHead);
                pEnd = (void *)strstr((void *)pPoint,AlarmTypeEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nAlarmID=nAlarmID+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }

                pPoint = (void *)strstr((void *)pPoint,AlarmAddrHead);
                pPoint +=strlen(AlarmAddrHead);
                nAlarmAddr=HsCharToInt(pPoint[0])*pow(16,3)
                            +HsCharToInt(pPoint[1])*pow(16,2)
                            +HsCharToInt(pPoint[2])*pow(16,1)
                            +HsCharToInt(pPoint[3])*pow(16,0);

                pPoint = (void *)strstr((void *)pPoint,AlarmOffsetHead);
                pPoint +=strlen(AlarmOffsetHead);
                pEnd = (void *)strstr((void *)pPoint,AlarmOffsetEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nOffSet=nOffSet+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }
                while(pAlarmBufPoint!=NULL)
                {
                    if((pAlarmBufPoint->nDeviceID==nDeviceID)&&(pAlarmBufPoint->nAlarmID==nAlarmID)
                        &(pAlarmBufPoint->nModbusAddr==nAlarmAddr)&&(pAlarmBufPoint->nAlarmExternID==nOffSet))
                    {
                        nReloadFlag = 1;
                        break;
                    }
                    else
                    {
                        pAlarmBufPoint=pAlarmBufPoint->pNext;
                    }
                }
                if(nReloadFlag == 0)
                    AlarmReload(nDeviceID,nAlarmID,nAlarmAddr,nOffSet);
                if(nAlarmID == 2)
                {
					switch(nAlarmAddr/100)
					{
						case 407:
							aAlarmInfo[nDeviceID-1][nAlarmAddr%10] &= ~(0x0001<<nOffSet);
							break;

						case 655:
							aAlarmInfo[nDeviceID-1][11] &= ~(0x0001<<nOffSet);
							break;
						default:
							aAlarmInfo[nDeviceID-1][12] &= ~(0x0001<<nOffSet);
							break;
					}
				}
//                    aAlarmInfo[nDeviceID-1][nAlarmAddr%10000] |=  (0x0001<<nOffSet);
            }
        }
        free(pHead);
    }
    close(fd);
}

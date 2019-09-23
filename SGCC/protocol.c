/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : protocol.c
* Author            : Alex
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "protocol.h"
#include "store.h"
#include "../interface/interfaceExtern.h"
#include "../Ethernet/EthernetExtern.h"
#include "../Alarm/alarmExtern.h"

UINT8  sgcc_s_packet[6] = {0x68, 0x04, 0x01, 0x00, 0x00, 0x00};
UINT8  sgcc_u_packet[6] = {0x68, 0x04, 0x83, 0x00, 0x00, 0x00};
UINT8  gThirdPartyTotalCallFlag = 0;
UINT16 gThirdPartyYXPointCount=0;
UINT16 gThirdPartyYCPointCount=0;
UINT16 gYXPointSumTemp=0;
UINT16 gYCPointSumTemp=0;

extern UINT16 gMainDeviceID;
extern UINT8 gConnectDeviceNum;
extern UINT16 gYXPointNum;
extern UINT16 gYCPointNum;
extern UINT16 gYKPointNum;
extern UINT16 gSDPointNum;
extern UINT16 gDDPointNum;
extern struct sTypeGroup *gTypeGroupPointTran;
extern struct sTypeParam *gTypeParamPointTran;
extern UINT8 gStationID[32];                        //Station ID
extern UINT8 gStationName[20];                      //Station Name
extern UINT32 gStationLongitude;                    //StationLongitude
extern UINT32 gStationLatitude;                     //StationLatitude
extern UINT8 gDLinkDeviceInfoCount;
extern UINT16 gSendCount;
extern UINT16 gRecvCount;

union
{
    UINT8 integer[4];
    float float_number;
}_104_value;

UINT8 PacketNorthMsgToSendArray(struct MsgSGCCStruct sgcc_msg_buff, UINT8 *send_buff)
{
    UINT8 send_buff_tmp[SGCC_MSG_MAX_LENGTH];

    memset(send_buff_tmp, 0, sizeof(send_buff_tmp));
    send_buff_tmp[SGCC_PROTOCOL_HEAD_ORIGIN] = 0x68;
    send_buff_tmp[SGCC_PROTOCOL_LENGTH_ORIGIN] = sgcc_msg_buff.msg_analysis.length;
    //packet_number_buff = g_north_msg_send_number << 1;
    //memcpy((uint8 *)&send_buff_tmp[SGCC_PROTOCOL_SEND_ORIGIN], (uint8 *)&packet_number_buff, sizeof(g_north_msg_send_number));
    //packet_number_buff = g_north_msg_receive_number << 1;
    //memcpy((uint8 *)&send_buff_tmp[SGCC_PROTOCOL_RECEVIE_ORIGIN], (uint8 *)&packet_number_buff, sizeof(g_north_msg_receive_number));
    //if(++g_north_msg_send_number > NORTH_PACKET_MAX_NUMBER)
    //    g_north_msg_send_number = 0;
    send_buff_tmp[SGCC_PROTOCOL_CMD_ORIGIN] = sgcc_msg_buff.msg_analysis.cmd_id;
    send_buff_tmp[SGCC_PROTOCOL_DETERMINER_ORIGIN] = sgcc_msg_buff.msg_analysis.determiner;
    memcpy((UINT8 *)&send_buff_tmp[SGCC_PROTOCOL_CAUSE_ORIGIN], (UINT8 *)&sgcc_msg_buff.msg_analysis.transmission_cause,sizeof(sgcc_msg_buff.msg_analysis.transmission_cause));
    memcpy((UINT8 *)&send_buff_tmp[SGCC_PROTOCOL_ADDRESS_ORIGIN], (UINT8 *)&sgcc_msg_buff.msg_analysis.address,sizeof(sgcc_msg_buff.msg_analysis.address));
    memcpy((UINT8 *)&send_buff_tmp[SGCC_PROTOCOL_MSG_ORIGIN], sgcc_msg_buff.msg_analysis.data_buff,sgcc_msg_buff.msg_analysis.data_length);
    memcpy(send_buff, send_buff_tmp, sgcc_msg_buff.msg_analysis.length+2);
    return sgcc_msg_buff.msg_analysis.length + 2;
}

UINT8 PacketSGCCMsgWithProtocol(UINT8 cmd_id, UINT8 determiner, UINT16 transmission_cause, UINT16 address, UINT8 *msg_data, UINT8 data_length, UINT8 *packet_data)
{
    struct MsgSGCCStruct sgcc_msg_buff;
    UINT8 send_buff[SGCC_MSG_MAX_LENGTH], send_msg_length;

    sgcc_msg_buff.msg_analysis.length = SGCC_PROTOCOL_HEAD_LENGTH + data_length - 2;
    sgcc_msg_buff.msg_analysis.send_msg_number = 0;
    sgcc_msg_buff.msg_analysis.receive_msg_number = 0;
    sgcc_msg_buff.msg_analysis.cmd_id = cmd_id;
    sgcc_msg_buff.msg_analysis.determiner = determiner;
    sgcc_msg_buff.msg_analysis.transmission_cause = transmission_cause;
    sgcc_msg_buff.msg_analysis.address = address;
    sgcc_msg_buff.msg_analysis.data_length = data_length;
    memcpy(sgcc_msg_buff.msg_analysis.data_buff, msg_data, data_length);
    sgcc_msg_buff.packet_type = SGCC_PACKET_TYPE_I;
    send_msg_length = PacketNorthMsgToSendArray(sgcc_msg_buff, send_buff);
    memcpy(packet_data, send_buff, send_msg_length);
    return send_msg_length;
}

/*****************************************************************************
* Description:      send S frame to third party platform
* Parameters:
* Returns:
*****************************************************************************/
void ThirdPartySendSFramePacket(void)
{
    UINT8 aSendBuf[6]={0x68,0x04,0x01,0x00,0x00,0x00};
    UINT16 nCount;

    nCount=(gRecvCount<<1);
    memcpy(&aSendBuf[4],(UINT8 *)&nCount,2);
    AddMsgToSGCCSendBuff(aSendBuf,6);
}

/*****************************************************************************
* Description:      南向104协议处理
* Parameters:       p_south_device_temp:南向设备
                    recv_msg_struct:接收的104
                    response_buff:应答报文
* Returns:          应答报文长度
*****************************************************************************/
UINT8 DealPacket(struct MsgSGCCStruct recv_msg_struct, UINT8 *response_buff)
{
    UINT8 response_length = 6;
    //UINT16 send_number = 0, recv_number = 0;
    UINT8 upload_buff[SGCC_MSG_MAX_LENGTH];

    if(recv_msg_struct.packet_type == SGCC_PACKET_TYPE_S)
    {
        memcpy(response_buff, sgcc_s_packet, sizeof(sgcc_s_packet));
        response_length = sizeof(sgcc_s_packet);
        return response_length;
    }
    else if(recv_msg_struct.packet_type == SGCC_PACKET_TYPE_U)//只应答U帧
    {
        memcpy(response_buff, sgcc_u_packet, sizeof(sgcc_u_packet));
        response_length = sizeof(sgcc_u_packet);
        if(recv_msg_struct.msg_analysis.data_buff[0] == 0x43)
        {
            response_buff[2] = 0x83;
        }
        else if(recv_msg_struct.msg_analysis.data_buff[0] == 0x07)
        {
            response_buff[2] = 0x0B;
        }
        else
            response_length = 0;
        return response_length;
    }
    else if(recv_msg_struct.packet_type == SGCC_PACKET_TYPE_I)
    {
        switch(recv_msg_struct.msg_analysis.cmd_id)
        {

            case C_IC_NA://0x64 总召
            {
                UINT8  upload_finish_data[4] = {0,0,0,S_R_Reponse};
                UINT8  nLenTemp=0;

                if(gUpdataModeFlag)
                    break;

                gThirdPartyTotalCallFlag=1;
                gThirdPartyYXPointCount=0;
                gThirdPartyYCPointCount=0;
                ThirdPartySendSFramePacket();

                nLenTemp = PacketSGCCMsgWithProtocol(C_IC_NA,
                                                      1,
                                                      S_R_Confirm,
                                                      gMainDeviceID,
                                                      upload_finish_data,
                                                      4,
                                                      upload_buff);//发送总召起始报文
                AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                break;
            }
            case C_CS_NA://0x67 对时
            {
                UINT8 aTemp[255],nLenTemp=0;

                memset(aTemp, 0, sizeof(aTemp));
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_Active)
                {
                    memcpy(aTemp,recv_msg_struct.msg_analysis.data_buff, recv_msg_struct.msg_analysis.data_length);
                    nLenTemp = PacketSGCCMsgWithProtocol(C_CS_NA,
                                                         1,
                                                         S_R_Confirm,
                                                         gMainDeviceID,
                                                         aTemp,
                                                         recv_msg_struct.msg_analysis.data_length,
                                                         upload_buff);
                    AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                }
                break;
            }
            case POINT_LOAD_CMD://0xBB 下联设备Point信息
            {
                UINT8 aTemp[255],nLenTemp=0;
                UINT16 nRes;

                memset(aTemp, 0, sizeof(aTemp));
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_ReportGlobleInfo)
                {
                    aTemp[3] = gConnectDeviceNum;
                    memcpy((UINT8 *)&aTemp[3+1],(UINT8 *)&gYXPointNum, sizeof(gYXPointNum));
                    memcpy((UINT8 *)&aTemp[3+3],(UINT8 *)&gYCPointNum, sizeof(gYCPointNum));
                    memcpy((UINT8 *)&aTemp[3+5],(UINT8 *)&gYKPointNum, sizeof(gYKPointNum));
                    memcpy((UINT8 *)&aTemp[3+7],(UINT8 *)&gSDPointNum, sizeof(gSDPointNum));
                    memcpy((UINT8 *)&aTemp[3+9],(UINT8 *)&gDDPointNum, sizeof(gDDPointNum));
                    nLenTemp = PacketSGCCMsgWithProtocol(POINT_LOAD_CMD,
                                                         1,
                                                         S_R_ReportGlobleInfo,
                                                         gMainDeviceID,
                                                         aTemp,
                                                         14,
                                                         upload_buff);
                    AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                }
                else if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_ReportPointTable)
                {
                    if(gTypeGroupPointTran == NULL)
                    {
                        DbgPrintf("step 1\r\n");
                        nRes = GetDownLinkDevicePointTable(0);
                    }
                    else if(gTypeParamPointTran == NULL)
                    {
                        DbgPrintf("step 2\r\n");
                        nRes = GetDownLinkDevicePointTable(gTypeGroupPointTran->nTypeID);
                    }
                    else
                        nRes = gTypeGroupPointTran->nTypeID;
                    DbgPrintf("step res %d\r\n", nRes);
                    if(nRes != 0)/** 找到设备信息: 发送一帧设备信息报文 */
                    {
                        nRes = GetDownLinkDevicePointParamInfo(1,aTemp,&nLenTemp);
                        nLenTemp = PacketSGCCMsgWithProtocol(POINT_LOAD_CMD,
                                                             1,
                                                             S_R_ReportPointTable,
                                                             gMainDeviceID,
                                                             aTemp,
                                                             nLenTemp,
                                                             upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                    else/** 设备信息为空 */
                    {
                        UINT8 upload_finish_data[4] = {0,0,0,S_R_InfoReportFinish};
                        nLenTemp = PacketSGCCMsgWithProtocol(POINT_LOAD_CMD,
                                                             1,
                                                             S_R_InfoReportFinish,
                                                             gMainDeviceID,
                                                             upload_finish_data,
                                                             4,
                                                             upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                }
                else if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_ReportDLinkDeviceInfo)
                {
                    nRes = GetDownLinkDevicePointInfo(1,aTemp,&nLenTemp);
                    if(nRes == 0)/** 找到设备信息: 发送一帧设备信息报文 */
                    {
                        nLenTemp = PacketSGCCMsgWithProtocol(POINT_LOAD_CMD,
                                                             1,
                                                             S_R_ReportDLinkDeviceInfo,
                                                             gMainDeviceID,
                                                             aTemp,
                                                             nLenTemp,
                                                             upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                    else/** 设备信息为空 */
                    {
                        UINT8 upload_finish_data[4] = {0,0,0,S_R_InfoReportFinish};
                        nLenTemp = PacketSGCCMsgWithProtocol(POINT_LOAD_CMD,
                                                             1,
                                                             S_R_InfoReportFinish,
                                                             gMainDeviceID,
                                                             upload_finish_data,
                                                             4,
                                                             upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                }
                break;
            }
            case ALARM_REPORT_CMD://0xBF
            {
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_Confirm)
                {
                    UINT8 nAlarmCount=0,i,nDeviceIDTemp,nAlarmIDTemp,nExternID,nAlarmStatus;
                    UINT16 nModbusAddr;

                    DbgPrintf("[AlarmConfirmSlaveChannel]get alarm confirm frame!!\n");
                    nAlarmCount = recv_msg_struct.msg_analysis.determiner&0x7F;
                    DbgPrintf("[AlarmConfirmSlaveChannel]nAlarmCount=%d\n",nAlarmCount);
                    for(i=0;i<nAlarmCount;i++)
                    {
                        nDeviceIDTemp = recv_msg_struct.msg_analysis.data_buff[3+(i*6)];                    
                        nAlarmIDTemp = recv_msg_struct.msg_analysis.data_buff[3+(i*6+1)];
                        memcpy((UINT8 *)&nModbusAddr,(UINT8 *)&recv_msg_struct.msg_analysis.data_buff[3+(i*6+2)],2);
                        nExternID = recv_msg_struct.msg_analysis.data_buff[3+(i*6+4)];
                        nAlarmStatus = recv_msg_struct.msg_analysis.data_buff[3+(i*6+5)];
                        AlarmConfirmSlaveChannel(nDeviceIDTemp,nAlarmIDTemp,nModbusAddr,nExternID,nAlarmStatus);
                    }
                }
                break;
            }
            case STATION_INFO_CMD://0xC1 站点基本信息
            {
                UINT8 aTemp[255],nLenTemp=0;

                memset(aTemp, 0, sizeof(aTemp));
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_Query)
                {
                    memcpy((UINT8 *)&aTemp[3],gStationID, sizeof(gStationID));
                    memcpy((UINT8 *)&aTemp[3+32],gStationName, sizeof(gStationName));
                    memcpy((UINT8 *)&aTemp[3+32+20],(UINT8 *)&gStationLongitude, sizeof(gStationLongitude));
                    memcpy((UINT8 *)&aTemp[3+32+20+4],(UINT8 *)&gStationLatitude, sizeof(gStationLatitude));
                    nLenTemp = PacketSGCCMsgWithProtocol(DOWNLINK_DEVICE_INFO_CMD,
                                                         1,
                                                         S_R_QueryConfirm,
                                                         gMainDeviceID,
                                                         aTemp,
                                                         3+32+20+4+4,
                                                         upload_buff);
                    AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                }
                break;
            }
            case DOWNLINK_DEVICE_INFO_CMD:// 0xC4 下联设备信息
            {
                UINT8 aTemp[255],nLenTemp=0;
                UINT16 nRes;

                memset(aTemp, 0, sizeof(aTemp));
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_Query)
                {
                    nRes = GetDownLinkDeviceInfo(1,aTemp,&nLenTemp);
                    if(nRes == 0)/** 找到设备信息: 发送一帧设备信息报文 */
                    {
                        UINT8 aTempBuf[255];

                        memset(aTempBuf, 0, sizeof(aTempBuf));
                        memcpy((UINT8 *)&aTempBuf[3], aTemp, nLenTemp);
                        nLenTemp = PacketSGCCMsgWithProtocol(DOWNLINK_DEVICE_INFO_CMD,
                                                                      1,
                                                                      S_R_QueryConfirm,
                                                                      gMainDeviceID,
                                                                      aTempBuf,
                                                                      nLenTemp+3,
                                                                      upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                    else/** 设备信息为空 */
                    {
                        gDLinkDeviceInfoCount = 0;
                        UINT8 upload_finish_data[4] = {0,0,0,S_R_InfoReportFinish};
                        nLenTemp = PacketSGCCMsgWithProtocol(DOWNLINK_DEVICE_INFO_CMD,
                                                                      1,
                                                                      S_R_InfoReportFinish,
                                                                      gMainDeviceID,
                                                                      upload_finish_data,
                                                                      4,
                                                                      upload_buff);
                        AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                    }
                }
                break;
            }
            case NETWORK_CONFIG_CMD://0xCE 多通道网络配置
            {
                UINT8 aTemp[255],nLenTemp=0;

                memset(aTemp, 0, sizeof(aTemp));
                if((recv_msg_struct.msg_analysis.transmission_cause & 0xFF) == S_R_Query)
                {
                    UINT8 aServerHostName[30];
                    UINT16 nPort;
                    UINT8 nMode = 2;
                    UINT8 nLink = 2;

                    if(recv_msg_struct.msg_analysis.data_buff[0] == 0)
                    {
                        E2promRead((UINT8 *)&nPort,ServerDomainNamePortAddr,2);
                        E2promRead(aServerHostName,ServerDomainNameAddr,30);
                    }
                    else if(recv_msg_struct.msg_analysis.data_buff[0] == 1)
                    {
                        E2promRead((UINT8 *)&nPort,SlaveDomainNamePortAddr,2);
                        E2promRead(aServerHostName,SlaveDomainNameAddr,30);
                        E2promRead((UINT8 *)&nMode,SlaveMode,1);
                        E2promRead((UINT8 *)&nLink,SlaveNetMode,1);
                    }
                    aTemp[0] = recv_msg_struct.msg_analysis.data_buff[0];
                    memcpy((UINT8 *)&aTemp[3],aServerHostName, sizeof(aServerHostName));
                    memcpy((UINT8 *)&aTemp[3+30],(UINT8 *)&nPort, sizeof(nPort));
                    aTemp[3+30+2] = nMode;
                    aTemp[3+30+2+1] = nLink;
                    nLenTemp = PacketSGCCMsgWithProtocol(NETWORK_CONFIG_CMD,
                                                         1,
                                                         S_R_QueryConfirm,
                                                         gMainDeviceID,
                                                         aTemp,
                                                         3+30+2+1+1,
                                                         upload_buff);
                    AddMsgToSGCCSendBuff(upload_buff,nLenTemp);
                }
                break;
            }
        }
    }
    return 0;
}

/*****************************************************************************
* Description:      解析接收报文为104报文
* Parameters:       pipe:通信通道
                    msg_data:接收数据
                    data_length:接收数据长度
                    recv_msg_struct:转换后的104报文结构体
* Returns:          应答报文长度
*****************************************************************************/
UINT8 DecodeMsg(UINT8 *msg_data, UINT8 data_length, struct MsgSGCCStruct *recv_msg_struct)
{
    struct MsgSGCCStruct *north_msg_buff;
    UINT16 msg_packet_number = 0;

    north_msg_buff = (struct MsgSGCCStruct *)malloc(sizeof(struct MsgSGCCStruct));
    //memset((uint8 *)&north_msg_buff, 0, sizeof(struct Msg104Struct));
    north_msg_buff->msg_analysis.length = msg_data[SGCC_PROTOCOL_LENGTH_ORIGIN];
    memcpy((UINT8 *)&msg_packet_number, (UINT8 *)&msg_data[SGCC_PROTOCOL_SEND_ORIGIN], sizeof(msg_packet_number));
    if((msg_packet_number & 0x0001) == SGCC_PACKET_TYPE_I)
    {
        /*I帧处理*/
        north_msg_buff->packet_type = SGCC_PACKET_TYPE_I;
        north_msg_buff->msg_analysis.send_msg_number = msg_packet_number;
        memcpy((UINT8 *)&msg_packet_number, (UINT8 *)&msg_data[SGCC_PROTOCOL_RECEVIE_ORIGIN], sizeof(msg_packet_number));
        north_msg_buff->msg_analysis.receive_msg_number = msg_packet_number;
        memcpy((UINT8 *)&north_msg_buff->msg_analysis.cmd_id, (UINT8 *)&msg_data[SGCC_PROTOCOL_CMD_ORIGIN], sizeof(north_msg_buff->msg_analysis.cmd_id));
        north_msg_buff->msg_analysis.determiner = msg_data[SGCC_PROTOCOL_DETERMINER_ORIGIN];
        memcpy((UINT8 *)&north_msg_buff->msg_analysis.transmission_cause, (UINT8 *)&msg_data[SGCC_PROTOCOL_CAUSE_ORIGIN], sizeof(north_msg_buff->msg_analysis.transmission_cause));
        memcpy((UINT8 *)&north_msg_buff->msg_analysis.address, (UINT8 *)&msg_data[SGCC_PROTOCOL_ADDRESS_ORIGIN], sizeof(north_msg_buff->msg_analysis.address));
        north_msg_buff->msg_analysis.data_length = data_length - SGCC_PROTOCOL_HEAD_LENGTH;
        memcpy(north_msg_buff->msg_analysis.data_buff, (UINT8 *)&msg_data[SGCC_PROTOCOL_MSG_ORIGIN], north_msg_buff->msg_analysis.data_length);
    }
    else if((msg_packet_number & 0x0003) == SGCC_PACKET_TYPE_S)
    {
        /*S帧处理*/
        north_msg_buff->packet_type = SGCC_PACKET_TYPE_S;
        north_msg_buff->msg_analysis.send_msg_number = 0;
        memcpy((UINT8 *)&msg_packet_number, (UINT8 *)&msg_data[SGCC_PROTOCOL_RECEVIE_ORIGIN], sizeof(msg_packet_number));
        north_msg_buff->msg_analysis.receive_msg_number = msg_packet_number >> 2;
    }
    else if((msg_packet_number & 0x0003) == SGCC_PACKET_TYPE_U)
    {
        /*U帧处理*/
        north_msg_buff->packet_type = SGCC_PACKET_TYPE_U;
        north_msg_buff->msg_analysis.data_length = 1;
        north_msg_buff->msg_analysis.data_buff[0] = msg_data[SGCC_PROTOCOL_SEND_ORIGIN];
    }
    memcpy(recv_msg_struct, north_msg_buff, sizeof(struct MsgSGCCStruct));
    free(north_msg_buff);
    return 1;
}

/*****************************************************************************
* Description:      总召处理线程
* Parameters:
* Returns:
*****************************************************************************/
void *TotalCallThread()
{
    UINT8 aBuf[SYS_FRAME_LEN];
    UINT32 nAddrTemp=0;
    UINT16 i;
    UINT8 aSendBuf[SYS_FRAME_LEN];
    UINT32 nValue;

    aSendBuf[0]=0x68;
    nValue=gSendCount*2;
    memcpy(&aSendBuf[2],(UINT8 *)&nValue,2);
    nValue=gRecvCount*2;
    memcpy(&aSendBuf[4],(UINT8 *)&nValue,2);

    while(1)
    {
        if(gThirdPartyTotalCallFlag == 0)
        {
            if(g_ethernet_connect_status == 1)
                gThirdPartySinglePointPoint=gThirdPartySinglePointHead;
            usleep(10);
            continue;
        }

        memset(aBuf,0,sizeof(aBuf));
        if(gThirdPartyTotalCallFlag==1)
        {
            if((gModuleChannel1InitFlag == 2) || (g_ethernet_connect_sub == 1))
            {
                if((gThirdPartyYXPointCount != 0xFFFF) && (gYXPointNum != 0))
                {
                    UINT8 nPointCount=0;

                    aBuf[0]=0x01;
                    aBuf[2]=0x14;
                    aBuf[3]=0x00;
                    memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
                    for(i = gThirdPartyYXPointCount;i<gYXPointNum;i++)
                    {
                        if(((i-gThirdPartyYXPointCount+1)*4+12)>=255)
                        {
                            break;
                        }
                        nAddrTemp=0x0001+i;
                        memcpy((UINT8 *)&aBuf[6+(i-gThirdPartyYXPointCount)*4],(UINT8 *)&nAddrTemp,2);
                        memcpy((UINT8 *)&aBuf[6+(i-gThirdPartyYXPointCount)*4+3],(UINT8 *)&aPointBuf[0x0001+i].nValue,1);
                        aBuf[1]=0x00+(i-gThirdPartyYXPointCount+1);
                        nPointCount++;
                    }
                    aBuf[1]=nPointCount;
                    if(nPointCount!=0)
                    {
                        memcpy(&aSendBuf[6],aBuf,6+4*(i-gThirdPartyYXPointCount));
                        aSendBuf[1]=6+4*(i-gThirdPartyYXPointCount)+6-2;
                        AddMsgToSGCCSendBuff(aSendBuf,6+4*(i-gThirdPartyYXPointCount)+6);
                    }
                    gThirdPartyYXPointCount=i;
                    if(gThirdPartyYXPointCount==gYXPointNum)
                    {
                        gThirdPartyYXPointCount=0xFFFF;
                    }
                }
                else if((gThirdPartyYCPointCount!=0xFFFF) && (gYCPointNum!=0))
                {
                    UINT8 nPointCount=0;

                    aBuf[0]=0x0D;
                    aBuf[2]=0x14;
                    aBuf[3]=0x00;
                    memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
                    nAddrTemp=0x4001+gThirdPartyYCPointCount;
                    memcpy(&aBuf[6],(UINT8 *)&nAddrTemp,3);
                    aBuf[8]=0x00;
                    for(i=gThirdPartyYCPointCount;i<gYCPointNum;i++)
                    {
                        if(((i-gThirdPartyYCPointCount+1)*5+15)>=255)
                        {
                            break;
                        }
                        memcpy((UINT8 *)&aBuf[9+(i-gThirdPartyYCPointCount)*5],(UINT8 *)&aPointBuf[0x4001+i].nValue,4);
                        aBuf[9+(i-gThirdPartyYCPointCount)*5+4]=0;
                        aBuf[1]=0x80+(i-gThirdPartyYCPointCount+1);
                        nPointCount++;
                    }
                    aBuf[1]=0x80+nPointCount;
                    memcpy(&aSendBuf[6],aBuf,9+5*(i-gThirdPartyYCPointCount));
                    aSendBuf[1]=9+5*(i-gThirdPartyYCPointCount)+6-2;
                    AddMsgToSGCCSendBuff(aSendBuf,9+5*(i-gThirdPartyYCPointCount)+6);

                    gThirdPartyYCPointCount=i;

                    if(gThirdPartyYCPointCount >= gYCPointNum)
                    {
                        gThirdPartyYCPointCount=0xFFFF;
                        gThirdPartyTotalCallFlag=2;
                    }
                }
                else if(gYCPointNum == 0)
                {
                    gThirdPartyYCPointCount=0xFFFF;
                    gThirdPartyTotalCallFlag=2;
                }
            }
            else if(g_ethernet_connect_status == 1)
            {
                UINT8 j;
                struct sThirdPartySubPoint *ThirdPartySubPointTemp=NULL;
                struct sTypeGroup *TypeGroupPointMBTemp=NULL;
                struct sTypeParam *TypeParamPointMBtemp=NULL;

                if((gThirdPartyYXPointCount != 0xFFFF) && (gYXPointSumTemp != 0))
                {
                    UINT8 nPointCount=0,YX_104_value_temp=0;

                    aBuf[0]=0x01;
                    aBuf[2]=0x14;
                    aBuf[3]=0x00;
                    memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);

                    for(i=gThirdPartyYXPointCount;i<gYXPointSumTemp;i++)
                    {
                        if(((i-gThirdPartyYXPointCount+1)*4+12)>=255)
                        {
                            break;
                        }

                        if(gThirdPartySinglePointPoint == NULL)
                            break;

                        while(gThirdPartySinglePointPoint->nType != Type_104_YX)
                        {
                            if(gThirdPartySinglePointPoint->pNext != NULL)
                                gThirdPartySinglePointPoint=gThirdPartySinglePointPoint->pNext;
                            else
                                break;
                        }

                        nAddrTemp=gThirdPartySinglePointPoint->n104Addr;

                        ThirdPartySubPointTemp=gThirdPartySinglePointPoint->pSubPoint;
                        YX_104_value_temp=0;
                        for(j=0;j<gThirdPartySinglePointPoint->nNumberOfSubPoint;j++)
                        {
                            UINT16 offset=0,_104_addr=0;

                            /*通过设备点表号查找对应点表*/
                            gPointTablePossessFlag|=(1<<9);
                            TypeGroupPointMBTemp=gTypeHead;

                            if(TypeGroupPointMBTemp == NULL)
                                break;

                            while(TypeGroupPointMBTemp->nTypeID != gDeviceInfo[ThirdPartySubPointTemp[j].nAddr].nPointTableNo)
                            {
                                if(TypeGroupPointMBTemp->pNext != NULL)
                                    TypeGroupPointMBTemp=TypeGroupPointMBTemp->pNext;
                                else
                                    break;
                            }


                            /*通过设备各自104起始地址和所处的偏移量计算本信号点在104点表中的位置*/
                            TypeParamPointMBtemp=TypeGroupPointMBTemp->pParamNext;

                            if(TypeParamPointMBtemp == NULL)
                                break;

                            while(TypeParamPointMBtemp->nMBAddr != ThirdPartySubPointTemp[j].nModbus)
                            {
                                if(TypeParamPointMBtemp->nType==Type_104_YX)
                                    offset++;

                                if(TypeParamPointMBtemp->pNext != NULL)
                                    TypeParamPointMBtemp=TypeParamPointMBtemp->pNext;
                                else
                                    break;
                            }
                            
                            gPointTablePossessFlag&=~(1<<9);
                            _104_addr=gDeviceInfo[ThirdPartySubPointTemp[j].nAddr].nYXAddr+offset;//计算得出104地址
                            YX_104_value_temp=(UINT8)aPointBuf[_104_addr].nValue;
                            DbgPrintf("YX%d,sub%d,_104_addr=%d,nValue=%d\r\n",i+1,j+1,_104_addr,YX_104_value_temp);
                        }
                        memcpy((UINT8 *)&aBuf[6+(i-gThirdPartyYXPointCount)*4],(UINT8 *)&nAddrTemp,2);
                        memcpy((UINT8 *)&aBuf[6+(i-gThirdPartyYXPointCount)*4+3],(UINT8 *)&YX_104_value_temp,1);
                        aBuf[1]=0x00+(i-gThirdPartyYXPointCount+1);
                        nPointCount++;
                        gThirdPartySinglePointPoint=gThirdPartySinglePointPoint->pNext;
                    }
                    aBuf[1]=nPointCount;
                    if(nPointCount!=0)
                    {
                        memcpy(&aSendBuf[6],aBuf,6+4*(i-gThirdPartyYXPointCount));
                        aSendBuf[1]=6+4*(i-gThirdPartyYXPointCount)+6-2;
                        AddMsgToSGCCSendBuff(aSendBuf,6+4*(i-gThirdPartyYXPointCount)+6);
                    }
                    gThirdPartyYXPointCount=i;
					
                    if(gThirdPartyYXPointCount==gYXPointSumTemp)
                    {
                        gThirdPartyYXPointCount=0xFFFF;
                        gThirdPartySinglePointPoint=gThirdPartySinglePointHead;
                    }
                }
                else if((gThirdPartyYCPointCount!=0xFFFF) && (gYCPointSumTemp!=0))
                {
                    UINT8 nPointCount=0;
                    float YC_104_value_temp=0;

                    aBuf[0]=0x0D;
                    aBuf[2]=0x14;
                    aBuf[3]=0x00;
                    memcpy(&aBuf[4],(UINT8 *)&gMainDeviceID,2);
                    nAddrTemp=0x4001+gThirdPartyYCPointCount;
                    memcpy(&aBuf[6],(UINT8 *)&nAddrTemp,3);
                    aBuf[8]=0x00;
                    for(i=gThirdPartyYCPointCount;i<gYCPointSumTemp;i++)
                    {
                        if(((i-gThirdPartyYCPointCount+1)*5+15)>=255)
                        {
                            break;
                        }

                        if(gThirdPartySinglePointPoint == NULL)
                            break;

                        while(gThirdPartySinglePointPoint->nType != Type_104_YC)
                        {
                            if(gThirdPartySinglePointPoint->pNext != NULL)
                                gThirdPartySinglePointPoint=gThirdPartySinglePointPoint->pNext;
                            else
                                break;
                        }

                        ThirdPartySubPointTemp=gThirdPartySinglePointPoint->pSubPoint;
                        YC_104_value_temp=0;
                        for(j=0;j<gThirdPartySinglePointPoint->nNumberOfSubPoint;j++)
                        {
                            UINT16 offset=0,_104_addr=0;

                            /*通过设备点表号查找对应点表*/
                            gPointTablePossessFlag|=(1<<10);
                            TypeGroupPointMBTemp=gTypeHead;

                            if(TypeGroupPointMBTemp == NULL)
                                break;

                            while(TypeGroupPointMBTemp->nTypeID != gDeviceInfo[ThirdPartySubPointTemp[j].nAddr].nPointTableNo)
                            {
                                if(TypeGroupPointMBTemp->pNext != NULL)
                                    TypeGroupPointMBTemp=TypeGroupPointMBTemp->pNext;
                                else
                                    break;
                            }

                            /*通过设备各自104起始地址和所处的偏移量计算本信号点在104点表中的位置*/
                            TypeParamPointMBtemp=TypeGroupPointMBTemp->pParamNext;

                            if(TypeParamPointMBtemp->pNext == NULL)
                                break;

                            while(TypeParamPointMBtemp->nMBAddr != ThirdPartySubPointTemp[j].nModbus)
                            {
                                if(TypeParamPointMBtemp->nType==Type_104_YC)
                                    offset++;

                                if(TypeParamPointMBtemp->pNext != NULL)
                                    TypeParamPointMBtemp=TypeParamPointMBtemp->pNext;
                                else
                                    break;
                            }

                            _104_addr=gDeviceInfo[ThirdPartySubPointTemp[j].nAddr].nYCAddr+offset;//计算得出104地址

                            switch(TypeParamPointMBtemp->nDataType)
                            {
                                case Type_Data_INT16:
                                case Type_Data_INT32:
                                case Type_Data_UINT16:
                                case Type_Data_UINT32:
                                case Type_Data_EPOCHTIME:
                                {
                                    memcpy(_104_value.integer,(UINT8 *)&aPointBuf[_104_addr].nValue,4);
                                    break;
                                }
                                case Type_Data_FLOAT:
                                {
                                    _104_value.float_number=aPointBuf[_104_addr].nValue;
                                    break;
                                }
                            }
                            gPointTablePossessFlag&=~(1<<10);
                            YC_104_value_temp+=_104_value.float_number;
                            DbgPrintf("YC%d,sub%d,_104_addr=%d,nValue=%f\r\n",i+1,j+1,_104_addr,YC_104_value_temp);
                        }
                        memcpy((UINT8 *)&aBuf[9+(i-gThirdPartyYCPointCount)*5],(UINT8 *)&YC_104_value_temp,4);
                        aBuf[9+(i-gThirdPartyYCPointCount)*5+4]=0;
                        aBuf[1]=0x80+(i-gThirdPartyYCPointCount+1);
                        nPointCount++;
                        gThirdPartySinglePointPoint=gThirdPartySinglePointPoint->pNext;
                    }
                    aBuf[1]=0x80+nPointCount;
                    memcpy(&aSendBuf[6],aBuf,9+5*(i-gThirdPartyYCPointCount));
                    aSendBuf[1]=9+5*(i-gThirdPartyYCPointCount)+6-2;
                    AddMsgToSGCCSendBuff(aSendBuf,9+5*(i-gThirdPartyYCPointCount)+6);

                    gThirdPartyYCPointCount=i;

                    if(gThirdPartyYCPointCount >= gYCPointSumTemp)
                    {
                        gThirdPartyYCPointCount=0xFFFF;
                        gThirdPartyTotalCallFlag=2;
                    }
                }
                else if(gYCPointSumTemp == 0)
                {
                    gThirdPartyYCPointCount=0xFFFF;
                    gThirdPartyTotalCallFlag=2;
                }
            }
        }
        else if(gThirdPartyTotalCallFlag == 2)
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

            memcpy(&aSendBuf[6],aBuf,10);
            aSendBuf[1]=0x0E;
            AddMsgToSGCCSendBuff(aSendBuf,16);
            gThirdPartyTotalCallFlag=0;
            gThirdPartyYCPointCount=0;
        }
        usleep(10);
    }
}


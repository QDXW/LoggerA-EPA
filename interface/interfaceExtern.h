#ifndef INTERFACEEXTERN_H_
#define INTERFACEEXTERN_H_
#include "common.h"
#include "../include/DataTypeDef.h"

/*******************************设备的104信息点总数结构*******************************/
typedef struct
{
	UINT8 uInuse; 			/*设备是否可用*/
	UINT8 deviceID; 		/*设备地址*/
    UINT8 nYXSum; 			/*设备遥信104总数*/
    UINT8 nYCSum; 			/*设备遥测104总数*/
    UINT8 nYKSum; 			/*设备遥控104总数*/
    UINT8 nSDSum; 			/*设备设点104总数*/
    UINT8 nDDSum; 			/*设备电度104总数*/
    UINT8 nAlarmSum; 		/*设备告警104总数*/
}iec104DeviceInfo;

//extern struct iec104DeviceInfo gDeviceiec104Info[MAXDEVICE];

extern UINT16 g_export_file_crc;
extern UINT16 g_import_file_crc;
extern UINT16 gRecvCount;                         //recv packet count
extern UINT8  gUpdataModeFlag;                    //0:normal 1:in update mode
extern UINT8 gDt1000UpdataFlag;

extern s104Point aPointBuf[0x6601];               //104 point data buffer
extern UINT32 aPointTimeBuf[0x6601];              //104 point record data buffer
extern UINT8 gTimeCallBackTime[7];                //time of the call point
extern UINT8 gModuleChannel0InitFlag;
extern UINT8 gModemSlaveChannelSwitch;
extern UINT8 gModuleChannel1InitFlag;             //Channel 1 Init Flag: 0:no Connection  1:Modem Config Over 2:Modem Connect to server
extern struct sThirdPartySinglePoint *gThirdPartySinglePointHead; //head of point table list
extern struct sThirdPartySinglePoint *gThirdPartySinglePointPoint; //point of point table list
extern struct sTypeGroup *gTypeHead;              //head of point table list
extern sDeviceInfo gDeviceInfo[MAXDEVICE];        //information of the south device
extern s104Point aPointBuf[0x6601];               //104 point data buffer
extern UINT8 gSocketModeSub;                      //Sub channel socket module 1:ethernet 2:modem mode
extern UINT8 gModeSub;                            //Sub channel mode 1:server 2:client
extern UINT8 gModemSlaveConfigFlag;               //Sub channel config flag 0:not yet 1:already
extern UINT8 gSocketHeartChannel0Count;           //socket channel 0 error count
extern UINT8 gSocketHeartChannel1Count;           //socket channel 1 error count
extern UINT8 gWirelessNetMode;                    //module network mode
extern UINT16 gMainDeviceID;                      //logger device ID
extern UINT16 gRecvCount;                         //recv packet count
extern UINT16 gSendCount;                         //send packet count
extern UINT8 gMasterReportSwitch; 
extern struct sTypeGroup *gTypeGroupPointTran;
extern UINT8 gDLinkDeviceInfoCount;
extern UINT16 dSouth_Record_Information;

extern UINT8 g_import_file_path[50];

extern int SouthCmdTask(UINT8 *aSendBuf,UINT8 aSendLen, UINT8 *aRecvBuf,UINT8 uDeviceId);
extern void DriverInit(void);
extern UINT8 SysNetInit(void);
extern void LedSet(UINT8 nBus,UINT16 nStatus);
extern void SinglePointAdd(UINT8 type,UINT32 _104_addr,UINT8 number_of_sub_point);
extern void SubPointAdd(UINT8 addr,UINT32 modbus,UINT8 num);
extern void ReportDeviceInfoToThirdPartyServer(UINT8 mode);
extern void SendFrameToThirdPartyPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen,UINT8 mode);
extern UINT8 GetDownLinkDevicePointTable(UINT16 point_number);
extern UINT8 GetDownLinkDevicePointParamInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen);
extern UINT8 GetDownLinkDevicePointInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen);
extern UINT8 GetDownLinkDeviceInfo(UINT8 nGetMode,UINT8 *pBuf,UINT8 *pLen);
extern void AP_Send(UINT8 *aBuf,UINT8 nLen);
extern int ComNormalRead(UINT8 nDeviceID,UINT16 nAddr,UINT8 nLen,UINT8 *pRecvBuf);
extern void ComWrite(UINT8 nCom,UINT8 *pValue,UINT8 nLen);
extern void *NorthernProcessingThreadMaster();
extern void *NorthernProcessingThreadSlave();
extern void SouthParamQuery(struct sTypeParam *uYtDotpoint,UINT8 uDeviceID,UINT8 dYtregisterSum,
		UINT8 *SendMessage,UINT8 Flag);

#endif

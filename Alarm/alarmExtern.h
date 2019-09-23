/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : alarmExtern.h
* Author            : Andre
* Date First Issued : 2019.2.18
* Version           :
* Description       : 告警相关定义
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include "../protocal/ProtocolDef.h"

extern UINT8 gDeviceAlarmBuf[10];
extern UINT16 aAlarmInfo[40][20];
extern struct sAlarmGroup *gAlarmHead;         //the head of alarm list

extern UINT8 AlarmAdd(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue);
extern void AlarmRemove(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue);
extern UINT8 AlarmReload(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue);
extern void AlarmDelete(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternValue);
extern void AlarmDeleteAll();
extern void AlarmConfirmMasterChannel(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternID,UINT8 nAlarmStatus);
extern void AlarmConfirmSlaveChannel(UINT8 nDeviceID,UINT8 nAlarmID,UINT16 nAddr,UINT8 nExternID,UINT8 nAlarmStatus);
extern void *AlarmThreadMasterChannel();
extern void *AlarmThreadSlaveChannel();


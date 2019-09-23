/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : ProtocolDef.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef PROTOCOLDEF_H_
#define PROTOCOLDEF_H_
#include <memory.h>
#include "../include/DataTypeDef.h"
#include "../include/ConstDef.h"
#include "../interface/common.h"
#pragma pack(push, 1)

typedef struct StrRemoteInfor
{
    UINT1 nEquipmentID;
    int fdSocket;
}__attribute__((__packed__)) StrRemoteInfor,*StrRemoteInforPtr;

typedef struct StrSmsInfor
{
    UINT1 nDataFlag;
    UINT1 aDataBuf[512];
    UINT1 nLen;
    UINT1 aPhoneNum[20];
}__attribute__((__packed__)) StrSmsInfor;

typedef struct StrSocketRecvInfo
{
    UINT1 nLen;
    UINT1 nType;
    UINT1 nFlag;
    UINT1 aDataBuf[256];
}__attribute__((__packed__)) StrSocketRecvInfo,*StrSocketRecvInfoPtr;

struct sSocketRecvBuf
{
    UINT1 nLen;
    UINT1 nType;
    UINT1 aDataBuf[256];
    struct sSocketRecvBuf *pNext;
    struct sSocketRecvBuf *pPre;
};

typedef struct
{
    UINT8 nInUse;/** 0:设备不可用 1：设备可用*/
    UINT8 nEndian;/** 大小端, 1:小端 0:大端 */
    UINT16 nPointTableNo; /** 电表编号 */
    UINT16 nType;
    UINT16 nYXAddr;
    UINT16 nYCAddr;
    UINT16 nYKAddr;
    UINT16 nSDAddr;
    UINT16 nDDAddr;
    UINT32 nRate;   /** 传输波特率 */
    UINT8 aESN[21]; /** 设备ESN号 */
    UINT8 nProtocolType;/** 协议类型, 1:华为MODBUS 0:标准MODBUS */
    UINT8 nDownlinkPort;/** 设备连接到数采的COM口, 1代表COM1, 2代表COM2*/
    UINT8 aSofeVersion[21];//逆变器软件版本号
}sDeviceInfo,*ptrsDeviceInfo;


typedef struct
{
    UINT8 nCmdID;
    UINT8 nMessNum;                 //variable structure qualifier
    UINT16 nSendFlag;
    UINT16 nPublicAddr;
    UINT32 nMessAddr;
}sASDU,*ptrsASDU;

typedef struct
{
    UINT8 nHead;
    UINT8 nPktLen;
    UINT16 nSendCtrl;
    UINT16 nRecvCtrl;
}sAPCI,*ptrsAPCI;

typedef struct
{
    sAPCI mAPCI;
    sASDU mASDU;
}sAPDU,*ptrsAPDU;

typedef struct
{
    UINT8 nLen;
    UINT8 nStatus;
    UINT8 nEnable;
    UINT32 nPreValue;
    UINT32 nValue;
}s104Point,*ptrs104Point;

typedef struct
{
    UINT8 nIVLicenseLoadFlag;
    UINT8 nIVLicenseReportLoadFlag;
    UINT8 nIVLicenseReportLoadCount;
    UINT8 nIVScanfLoadFlag;
}sIVStruct,*ptrsIVStruct;

union uIEEE754
{
    float nIndex;
    UINT8 nChar[4];
};
extern union uIEEE754 uIE;

union U32_F_Char_Convert
{
    float    f;
    UINT32   u;
    INT32    i;
    UINT16   u16;
    INT16    i16;
    UINT8    c[4];
};

extern union U32_F_Char_Convert U32_F_Char;

typedef struct
{
    UINT16 nMBAddr;
    UINT8  nLen;
    UINT8  nType;
}sTypePoint,*ptrsTypePoint;

struct sTypeParam
{
    UINT16 nMBAddr;
    UINT8  nLen;
    UINT8  nType;
    UINT8  nDataType;
    struct sTypeParam *pNext;
};

struct sTypeGroup
{
    UINT16 nTypeID;
    UINT8  nProtocalTypeID;
    struct sTypeGroup *pNext;
    struct sTypeParam *pParamNext;
};

struct sAlarmGroup
{
    UINT8 nDeviceID;
    UINT8 nAlarmID;
    UINT16 nModbusAddr;
    UINT8 nAlarmExternID;
    UINT8 nAlarmCount;                  //告警计数，只用于断链告警
    UINT8 nUploadStatusMasterChannel;   //主通道：      0:告警未上报       1、2、3:告警已上报，未确认            5:告警已上报，已确认
                                        //6:告警恢复已上报，已确认
    UINT8 nUploadStatusSlaveChannel;    //从通道：      0:告警未上报       1:告警已上报，未确认           2:告警已上报，已确认
                                        //3:告警恢复未上报 4:告警恢复已上报，未确认 5:告警恢复已上报，已确认
    struct sAlarmGroup *pNext;
    struct sAlarmGroup *pPre;
};

//以下为针对三方平台定义的结构体 Andre 2018.10.09
struct sThirdPartyStationInfo           //三方站点信息，包括IP、端口、站点编号
{
    UINT8  nIP[20];
    UINT16 nPort;
    UINT16 nStationID;
};

struct sThirdPartySinglePoint           //三方单点信息，包括104地址、子点数
{
    UINT8  nType;
    UINT16 n104Addr;
    UINT8  nNumberOfSubPoint;
    struct sThirdPartySinglePoint *pNext;
    struct sThirdPartySubPoint *pSubPoint;
};

struct sThirdPartySubPoint              //三方子点信息，包括逆变器地址、modbus地址
{
    UINT8  nAddr;
    UINT32 nModbus;
};

struct sThirdPartyFrameBuffer
{
    UINT8 msg[255];
    UINT8 msg_length;
    struct sThirdPartyFrameBuffer *p_next;
};

//以下为针对能源项目定义的结构体 Andre 2018.11.12
struct sSouthernAlarm
{
    UINT16 upper_limit;
    UINT16 lower_limit;
};

struct sDt1000Update
{
    UINT8   updata_mark;      // 升级标志0xAA升级；0xBB回滚；0x55无
    UINT8   vertion[17];      // 程序版本
    UINT8   uDevAddr;        //设备地址
    UINT8   uData;           // 日期
    UINT32  nDataLen;
    UINT16  frame_sum;        // 升级总帧数，256字节一帧
    UINT16  CRC;              // 数据CRC校验
};

//struct sDt1000Update *gDt1000Update=NULL;

#pragma pack (pop)
#endif /*PROTOCOLDEF_H_*/

/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : common.h
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../protocal/ProtocolDef.h"

/*
typedef	char		INT8;
typedef	short		INT16;
typedef	int		    INT32;
typedef	long long	INT64;
typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned int	UINT32;
typedef	unsigned long long UINT64;
*/


#define __DEBUG__

#ifdef __DEBUG__
#define DbgPrintf printf
#define NorthPrintf DbgPrintf
#else
#define DbgPrintf usleep(1000);printf
#define NorthPrintf
#endif


/*数据类型*/
#define INT8                     signed char
#define INT16                    signed short
#define INT32                    signed int
#define UINT8                    unsigned char
#define UINT16                   unsigned short
#define UINT32                   unsigned int
#define EPOCHTIME                long
/*EEPROM地址定义*/
#define DeviceIP_E2P             0x0001      //4Byte
#define DeviceNumberAddr         0x0005      //1Byte
#define DeviceTypeNumberAddr     0x0006      //1Byte
#define DeviceYXNumberAddr       0x0007      //2Byte
#define DeviceYCNumberAddr       0x0009      //2Byte
#define DeviceYKNumberAddr       0x000B      //2Byte
#define DeviceSDNumberAddr       0x000D      //2Byte
#define DeviceDDNumberAddr       0x000F      //2Byte
#define DevicePointConfigAddr    0x0011      //1Byte
#define DeviceAddress            0x0012      //2Byte
#define DeviceOldVersionAddr     0x0014      //20Byte
#define DevicePhoneAddr          0x0028      //11Byte
#define DeviceSNAddr             0x0033      //20Byte
#define DeviceNameAddr           0x0047      //20Byte
#define DeviceModelAddr          0x005B      //20Byte
#define DeviceTypeAddr           0x006F      //20Byte
#define DeviceStatusAddr         0x0083      //1Byte
#define DeviceSocketModeAddr     0x0084      //1Byte
//#define StationIDAddr            0x0084      //20Byte
#define StationNameAddr          0x0098      //20Byte
#define StationLongitudeAddr     0x00AC      //4Byte
#define StationLatitudeAddr      0x00B0      //4Byte
#define StationIPAddr            0x00B4      //4Byte null
#define StationPortAddr          0x00B8      //4Byte null
#define DeviceIPSetAddr          0x00BC      //1Byte
#define DeviceConnectServerAddr  0x00BD      //1Byte
#define TypeNumberAddr           0x00BE      //1Byte
#define DeviceConfigAddr         0x00BF      //1Byte
#define DeviceSwOldVerAddr       0x00C0      //3Byte
#define RouteConfigAddr          0x00C3      //1Byte
#define StationIDAddr            0x00C4      //32Byte
#define ServerDomainNameAddr     0x00E4      //30Byte
#define ServerDomainNamePortAddr 0x0102      //2Byte
#define AlarmStatusAddr          0x0104      //10Byte
#define AlarmLoggerAddr          0x010E      //2Byte
#define UpdateRestoreAddr        0x0110      //1Byte
#define SlaveDomainNameAddr      0x0111      //30Byte
#define SlaveDomainNamePortAddr  0x012F      //2Byte
#define SlaveMode                0x0131      //1Byte
#define SlaveNetMode             0x0132      //1Byte
#define SlaveSwitch              0x0133      //1Byte
#define SlaveNetInfoConfigAddr   0x0134      //1Byte
#define MasterReportSwitch       0x0135      //1Byte


//Third Party Server Domain Name and Port
#define ThirdPartyServer2DomainNameAddr     0x0140      //30Byte
#define ThirdPartyServer2DomainNamePortAddr 0x015E      //2Byte
#define ThirdPartyServer3DomainNameAddr     0x0160      //30Byte
#define ThirdPartyServer3DomainNamePortAddr 0x017E      //2Byte


//Device Type Info
#define DeviceTypeInfoAddr          0x03E9   //1001-1400  4Byte(Rate)+1Byte(Endian)...1Byte+info......
//#define TypeTableInfoAddr         0x0579   //1401-1480  80Byte6a5
#define TypeTableInfoAddr           0x06A5

//#define AlarmInfoAddr             0x067D   //1661-3260  1600Byte745
#define AlarmInfoAddr               0x0745

#define SYS_OAM_VERSION             0xA11774 //版本号
#define SYS_DEBUG_VERSION           0x0002   //debug版本号
#define SYS_FRAME_LEN               256
#define MAXDEVICE                   41
#define MAX_DEVICE_TYPE             8        //最大接入设备类型数
#define MAXRECVBUF                  10
#define SYS_LOG_FILES_LEN           200
#define SYS_FILES_LEN               224
#define FIXED_LENGTH                15
#define CRC_LENGTH                  2
#define TRANSFER_START              0
#define TRANSFER_OK                 1
#define CRC_FAILED                  2

/*104*/
#define PACKETHEAD                  0x68

/*104命令标识*/
#define M_SP_NA                     0x01//单点信息
#define M_DP_NA                     0x03//双点信息
#define M_ST_NA                     0x05
#define M_BO_NA                     0x07
#define M_ME_NA                     0x09
#define M_ME_NB                     0x0B
#define M_ME_NC                     0x0D
#define M_IT_NA                     0x0F
#define M_PS_NA                     0x14
#define M_ME_ND                     0x15
#define M_SP_TB                     0x1E
#define M_DP_TB                     0x1F
#define M_ST_TB                     0x20
#define M_BO_TB                     0x21
#define M_ME_TD                     0x22
#define M_ME_TE                     0x23
#define M_ME_TF                     0x24
#define M_IT_TB                     0x25
#define M_EP_TD                     0x26
#define M_EP_TE                     0x27
#define M_EP_TF                     0x28

#define C_SC_NA                     0x2D
#define C_DC_NA                     0x2E
#define C_RC_NA                     0x2F
#define C_SE_NA                     0x30
#define C_SE_NB                     0x31
#define C_SE_NC                     0x32
#define C_BO_NA                     0x33
#define C_SC_TA                     0x3A
#define C_DC_TA                     0x3B
#define C_RC_TA                     0x3C
#define C_SE_TA                     0x3D
#define C_SE_TB                     0x3E
#define C_SE_TC                     0x3F
#define C_BO_TA                     0x40

#define M_EI_NA                     0x46

#define C_IC_NA                     0x64//总召唤
#define C_CI_NA                     0x65
#define C_RD_NA                     0x66
#define C_CS_NA                     0x67
#define C_RP_NA                     0x69
#define C_TS_TA                     0x6B

#define P_ME_NA                     0x6E
#define P_ME_NB                     0x6F
#define P_ME_NC                     0x70
#define P_AC_NA                     0x71

#define F_FR_NA                     0x78
#define F_SR_NA                     0x79
#define F_SC_NA                     0x7A
#define F_LS_NA                     0x7B
#define F_AF_NA                     0x7C
#define F_SG_NA                     0x7D
#define F_DR_NA                     0x7E
#define F_SC_NB                     0x7F

#define UPDATA_CMD                  0xB4
#define DEVICE_IP_CMD               0xB5
#define MODBUS_ENDIAN_CMD           0xB6
#define MODBUS_RATE_CMD             0xB7
#define DEVICE_ADDR_CMD             0xB8
#define DEVICE_VERSION_CMD          0xB9
#define DEVICE_LOOPBACK_CMD         0xBA  /** 版本回滚 */
#define POINT_LOAD_CMD              0xBB  /** 点表导入 */
#define REPORT_PHONE_CMD            0xBC
#define MODEM_STATUS_CMD            0xBD
#define CALLBACK_TIME_CMD           0xBE
#define ALARM_REPORT_CMD            0xBF
#define DEVICE_BASE_INFO_CMD        0xC0  /** 设备基本信息 */
#define STATION_INFO_CMD            0xC1  /** 站点基本信息 */
#define SERVER_INFO_CMD             0xC2  /** 平台基本信息 */
#define STATION_BUILD_CMD           0xC3  /** 建站指令 */
#define DOWNLINK_DEVICE_INFO_CMD    0xC4  /** 下联设备信息 */
#define LOAD_POINT_TABLE_CMD        0xC5  /** 导表指令 */
#define COMMUNICATION_MODE_CMD      0xC6
#define UPLOAD_HW_DEVICE_CMD        0xC7
#define SERVER_IP_CMD               0xC8
#define SERVER_PORT_CMD             0xC9
#define LOG_UPLOAD                  0xCA
#define DEVICE_LOCATION_CMD         0xCB
#define NETWORK_CONFIG_CMD          0xCE
#define NETWORK_SLAVE_SWITCH_CMD    0xCF
#define ESN_CMD                     0xD0
#define DEVICE_TYPE_CMD             0xD1
#define DEVICE_MODEL_CMD            0xD2
#define DEVICE_NAME_CMD             0xD3
#define DEVICE_MODIFY_CMD           0xD4 /** 南向设备修改 */
#define DEVICE_DELETE_CMD           0xD5
#define SIM_INFO_REPORT_CMD         0xD6 /** SIM卡信息上报 */
#define MODEM_SIGNAL_PWR_CMD        0xD7
#define DEVICE_MAXNUMBER_CMD        0xD8
#define DEVICE_REGISTER_CMD         0xD9/** 遥调在用 */

#define IV_SCANF_INFO_CMD           0xE0
#define IV_SCANF_STATUS_CMD         0xE1
#define IV_SCANF_TRANS_CMD          0xE2
#define IV_LICE_INFO_CMD            0xE3
#define IV_LICE_DEL_CMD             0xE4
#define IV_LICE_STATUS_CMD          0xE5
#define IV_LICE_TRANS_CMD           0xE6 /** IV*/

#define FILE_TRANS_CMD              0xF0 /** File transmission */
#define DEVICE_RESET_CMD            0xFA /** 恢复出厂*/
#define DEVICE_REBOOT_CMD           0xFB /** 设备复位： 重启*/

/** 手机APP C4命令使用最新的格式
***  即，原因码高8位使用0x01表示起始帧,0x00表示后续帧
***  如果使用旧方案，注释掉下面的宏
*/
//#define USE_NEW_C4_FORAPP
/** 服务器 C4命令使用新的格式
*/
//#define USE_NEW_C4_SERVER

/*传输原因*/
#define S_R_Upload                  0x03//突发上传
#define S_R_Init                    0x04//初始化
#define S_R_Requst                  0x05//请求、被请求
#define S_R_Active                  0x06//激活
#define S_R_Confirm                 0x07//激活确认
#define S_R_Stop                    0x08//停止
#define S_R_StopConf                0x09//停止确认
#define S_R_AcitveEnd               0x0a//激活结束
#define S_R_Reponse                 0x14//响应总召唤
#define S_R_BeginToTrans            0x80
#define S_R_BeginToTransConfirm     0x81
#define S_R_DataTrans               0x82//数据传输
#define S_R_DataRetran              0x83//数据重传
#define S_R_DataStop                0x84//数据停止
#define S_R_DataStopConfirm         0x85//数据停止确认
#define S_R_TransComplete           0x86//传输完成
#define S_R_TransComplConfirm       0x87//传输完成确认
#define S_R_LoadGlobleInfo          0x88
#define S_R_LoadDLinkDeviceType     0x89
#define S_R_LoadDLinkDeviceInfo     0x8A
#define S_R_TableBuild              0x8B
#define S_R_TableLoad               0x8C
#define S_R_ExternStop              0x8D
#define S_R_ExternStopConfirm       0x8E
#define S_R_TimeCallBackEnd         0x8F
#define S_R_Query                   0x90
#define S_R_QueryConfirm            0x91
#define S_R_Set                     0x92
#define S_R_SetConfirm              0x93
#define S_R_InfoReport              0x94
#define S_R_InfoReportFinish        0x95
#define S_R_ReportGlobleInfo	    0xA8
#define S_R_ReportPointTable	    0xA9
#define S_R_ReportDLinkDeviceInfo	0xAA
#define S_R_Error                   0xEE

/*104类型编号*/
#define Type_104_YC         0x01
#define Type_104_YX         0x02
#define Type_104_YK         0x04
#define Type_104_SD         0X07   //设点即遥调
#define Type_104_DD         0x06
#define Type_104_Alarm      0x09


/*MODBUS数据类型*/
#define Type_Data_UINT16    0x01
#define Type_Data_STRING    0x02
#define Type_Data_UINT32    0x03
#define Type_Data_INT16     0x04
#define Type_Data_INT32     0x05
#define Type_Data_NULLDATA  0x06
#define Type_Data_EPOCHTIME 0x07
#define Type_Data_BIT       0x08
#define Type_Data_FLOAT     0x09

/*MODBUS功能代码*/
#define Code_Modbus_Read    0x03

/*协议类型*/
#define Type_Huawei_Modbus   0x01
#define Type_Standard_Modbus 0x02

/*设备参数*/
#define HS_I2C               "/dev/hs_i2c"        /*I2C */
#define HS_E2PROM            "/dev/at24"          /*e2prom */
#define HS_FPGA              "/dev/fpga"          /*FPGA */
#define HS_PLL               "/dev/lmx2531"       /*PLL */
#define HS_ATT               "/dev/pe4302"        /*ATT */
#define HS_AD                "/dev/ad6642"
#define HS_DA                "/dev/ad9122"
#define HS_CLK               "/dev/ad9523"
#define HS_DAC               "/dev/tlv5631"
#define HS_GPIO              "/dev/hsgpio"
#define Up_Data_File_Path    "/mnt/flash/OAM/updatatemp.gz.tar"
#define EXPORT_FILE_PATH     "/mnt/flash/OAM/ExportFile.tar.gz"
#define IMPORT_FOLDER_PATH   "/mnt/flash/OAM/filetemp"
#define IMPORT_FILE_PATH     "/mnt/flash/OAM/filetemp/filetemp.tar.gz"
#define IMPORT_CONFIG_FILE   "/mnt/flash/OAM/filetemp/config.ini"
#define UPDATE_FILE_PATH     "/mnt/flash/OAM/OAM"
#define DT1000_FILE_PATH     "/mnt/flash/OAM/DT1000.bin"
#define POINT_FILE_PATH      "/mnt/flash/OAM/point/%d.xml"
#define LOG_FILE_PATH        "/mnt/flash/OAM/log/%d.bin"
#define RECORD_FILE_PATH     "/mnt/flash/OAM/record/%02d.bin"
#define DEVICE_FILE_PATH     "/mnt/flash/OAM/xml/base.xml"
#define ALARM_FILE_PATH      "/mnt/flash/OAM/xml/alarm.xml"
#define CERT_FOLDER_PATH     "/mnt/flash/OAM/cert"
#define CA_FILE_PATH         "/mnt/flash/OAM/cert/ca.pem"
#define CC_FILE_PATH         "/mnt/flash/OAM/cert/cc.pem"
#define Ck_FILE_PATH         "/mnt/flash/OAM/cert/ck.pem"
#define THIRD_PARTY_FOLDER   "/mnt/flash/OAM/thirdparty"

/*I2C参数*/
#define E2PROM_CHIP_ADDR     0xA0                    /*e2prom CHIP */
#define E2PROM_PAGE_SIZE     0x20
#define I2C_CHIP_ID          0x01
#define I2C_ADDR             0x02
#define I2C_ADDR_LEN         0x03
#define I2C_DATA_LEN         0x04
#define I2C_FPGA_SEL         0x05
#define DAC_CHIP_ADDR        0x9E

#define ERROR_LOW            5
#define ERROR_OVER           6
#define ERROR_UNDETECT       7

#define CARRY_DAY            7 //2012.01.04
#define CARRY_TIME           5 //2012.01.04

#define HS_DOUBLE_ZERO       0.0
#define HS_FLOAT_ZERO        0.0

#define ServerIPRegex        "%d.%d.%d.%d"
#define DeviceIPRegex        "ifconfig eth0 %d.%d.%d.%d"

#define PointRecordFileRegex "/mnt/flash/OAM/record/%02d.bin"
#define PointRecordHead      "104Addr    Device        Modbus     Value\n"
#define PointRecordHeadRegex "%d-%02d-%02d %02d:%02d:%02d\n"
#define PointRecordRegex     "0x%04X       %02d          %05d      %d\n"

#define LogRecordFile        "/mnt/flash/OAM/log/log"
#define LogApartFilePath	 "/mnt/flash/OAM/xml/LogApart.txt"
/**TA: the path where log files store*/
#define LogRecodeFilePath    "/mnt/flash/OAM/log/"
#define LogComRecordFile     "/mnt/flash/OAM/log/comlog"
#define TestLogRecordFile    "/mnt/flash/OAM/log/testlog"
/**TA: the path where data binary files store*/
#define DataRecordFilePath   "/mnt/flash/OAM/record/"
#define LogRecordFileRegex   "0x%d0x%d0x%d0x%d0x%d0x%d"
#define TestLogRecordFileRegex   "Device ID=%d CmdID=0x%02X MessNum=%d TransFlag=0x%02X Value="
#define TestLogServerPortRegex   " Port=%d\n"

/** gMainDeviceStatus设备状态取值 2-5(APP开站) 6-9(华为设备自发现)*/
#define DEVSTATUS_NEW_DEVICE        0  /** 新设备，还未开站 */
//APP
#define DEVSTATUS_DEVINFO_GET       2
//Platform
#define DEVSTATUS_DEVINFO_SET       3
// 华为设备自发现
#define DEVSTATUS_DEVINFO_UPLOAD    6  /** 开始上报下联设备 */
#define DEVSTATUS_DEVINFO_RESPONSE  7  /** 上报下联设备应答，准备上报下一帧 */
#define DEVSTATUS_DEVINFO_EXISTS    8
#define DEVSTATUS_DEVINFO_LOADEND   9  /** 完成下联设备上报 */
#define DEVSTATUS_NORMAL_WORKMODE   10 /** 表示完成导表及开站 */

/*告警编号*/
#define Alarm_Link_Error            0x01
#define Alarm_DownLinkDevice_Error  0x02
#define Alarm_Device_Error          0x03

//告警计数
#define ALARM_NOT_COUNT             0  //无需计数，用于非断连告警
#define ALARM_SCANF_MAXNUMBER       10 //最大重查次数，第11次上报

//主通道告警上报状态
#define ALARM_NOT_REPORT            0
#define ALARM_REPORT_MAXNUMBER      3
#define ALARM_REPORT_CONFIRMFLAG    5
#define ALARM_RECOVER_CONFIRMFLAG   6

//从通道告警上报状态
#define ALARM_NOT_REPORT_SLAVE      0
#define ALARM_REPORT_SLAVE          1
#define ALARM_REPORT_CONFIRM_SLAVE  2
#define ALARM_NOT_RECOVER_SLAVE     3
#define ALARM_RECOVER_SLAVE         4
#define ALARM_RECOVER_CONFIRM_SLAVE 5

/** gSocketMode当前网络的工作模式 */
#define SOCKETMODE_LAN 0 /** 通过网口连接平台 */
#define SOCKETMODE_3G  1 /** 通过3G模块连接平台 */

/** gModuleChannel0InitFlag 3G模块工作状态 */
#define MODEMFLAG_UNINITIALIZED  0 /** 未初始化 */
#define MODEMFLAG_CONFIGED_ONLY  1 /** 配置完成，但是没有连接 */
#define MODEMFLAG_CONNECT_SERVER 2 /** 成功连接到平台服务器 */

#define _BIG_ENDIAN    0
#define _LITTLE_ENDIAN 1

#define _NO  0
#define _YES 1

/** Modbus协议类型 */
#define PTYPE_STAND_MODBUS  0 /** 标准 */
#define PTYPE_HUAWEI_MODBUS 1 /** 华为 */

/**PLC**/
#define PLCDetect           0 //从未连接
#define PLCConnect          1 //已经连接
#define PLCCutdown          2 //连接后断开
#define PLCAlarm            3 //上报告警
#define PLCAlarmConfirm     4 //告警确认

#define UPDATA_FILE             0x01     //升级文件
#define UPDATA_ICLEANLOGGER     0x00     //数采升级文件
#define UPDATA_DT1000           0x01     //表计升级文件

#define POINT_FILE              0x02     //点表文件
#define LOG_FILE                0x03     //北向日志
#define RECORD_FILE             0x04     //南向日志
#define DEVICE_INFO_FILE        0x05     //设备信息
#define ALARM_FILE              0x06     //告警记录

#define IV_DATA_FILE            0x20     //IV曲线导出
#define IV_LICENSE_FILE         0x21     //IV License导入
#define HISTORY_FILE            0x22     //逆变器历史数据
#define SOUTH_RECORD            0x33     //南向记录

#define CERT_FILE               0x50     //证书文件
#define THIRDPARTYCONFIG_FILE   0x51     //三方平台配置文件

/** 模块网络模式 */
#define Net_Mode_3G         0
#define Net_Mode_4G         1

int nCLKfd;
int nADfd;
int nDAfd;
int nLMXfd;
int nPEfd;
int nFPGAfd;
int nE2fd;
int nI2Cfd;
int nDACFd;
int nGPIOfd;
int nAPFd;
int nUpdataFd;
int nLicenseFd;
int nIVScanfFd;
int nPointRecordFd;
int nLogRecordFd;
int nTestLogRecordFd;
int nComLogRecordFd;
int nUartFd;
int n_file_fd;
int nLogApartFd;

/**
 * TA: Record current opened log file day number [1,31]
 * log file format:  gCurrentDayNum.log
 * example: 16.log (store 16th day's log file of one month)
 */
unsigned char gCurrentDayNum;
unsigned char gCurrentMonthNum;
/**
 * 是否已经将数采设备的信息发送给平台
 * 通过C0指令实现
*/
volatile unsigned char gIsReportInfo;
/** SIM 信息*/
unsigned char sim_card_id[20];
volatile unsigned char hasReportSIM;

pthread_mutex_t e2promSem;
pthread_mutex_t threadFileSem;
pthread_mutex_t APsem;
pthread_mutex_t socketbufsem;
pthread_mutex_t alarmsem;
pthread_mutex_t modemsem;
pthread_mutex_t Uartsem;
pthread_mutex_t logsem;
pthread_mutex_t logUploadSem;
pthread_mutex_t fpgaSem;
pthread_mutex_t LogApartSem;
pthread_mutex_t sgcc_send_buff_sem;
pthread_mutex_t sgcc_recv_buff_sem;

UINT8 aAlarmSwitch[145];
struct sSouthernAlarm *gSouthernAlarm;                //南向数据告警阈值，用于能源项目
extern UINT8 gTypePointClearFlag;
extern UINT32 gPointTablePossessFlag;

INT32 E2promWrite(UINT8 * paraValue,UINT16 addrValue,UINT8 count);
INT32 E2promRead(UINT8 * paraValue,UINT16 addrValue,UINT8 count);

void FpgaWrite(UINT16 nAddr,UINT8 nValue);
UINT8 FpgaRead(UINT16 nAddr);

void FpgaInit(void);
UINT8 SysNetInit(void);
void SysTimeInit();

UINT8 RTCGet();
UINT8 RTCSet(UINT8 *pParaValue);

void Socket_Send(UINT8 *aBuf,UINT8 nLen);

void DLinkParamGet();
void LedSet(UINT8 nBus,UINT16 nStatus);
void WIFIEnable(UINT16 nStatus);
void WifiInit(void);
void SaveThreadInfoInit();
void SaveThreadInfo(UINT8 *pThreadName);
void *LocalThread();
void *SouthUpdatethread();

void *APThread();
void *SocketHeartThread();
void *ScanfDeviceThread();
void *PowerAlarmThread();
void *ModemScanfThread();
void *IVThread();
void *TestThread();
void SouthBroadcastUpdata();
void SouthDiscovery();

void SouthSingleUpdata();

UINT8 HsCharToInt(UINT8 nValue);
UINT8 PackMainFunction(UINT8 nProtocol,UINT8 *aBuf,UINT8 nLen);
UINT8 APPackMainFunction(UINT8 nProtocol,UINT8 *aBuf,UINT8 nLen);
void SendDeviceInfoToServer(void);
void SendUpdataResendPacket(void);
void ReportDeviceInfoToServer(void);
void SendSingleFrameToAPP(UINT16 nCmdID,UINT16 nFlag,UINT8 nValue);
void SendSingleFrameToPlatform(UINT16 nCmdID,UINT16 nFlag,UINT8 nValue);
void SendFrameToPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen);
void TypeGroupAdd(UINT8 nAddMode,UINT16 nTypeID,UINT8 nProtocolTypeID);
void TypeParamAdd(UINT8 nAddMode,UINT16 nAddr,UINT8 nLen,UINT8 nType,UINT8 nDataType);
void TypePointClear(void);
void LogRecordFileWrite(UINT8 nStatus,UINT8 *aRecvBuf,UINT8 nLen);
void LogComRecordFileWrite(UINT8 nStatus,UINT8 *aRecvBuf,UINT8 nLen);
void TestLogStringFileWrite(UINT8 *aRecvBuf,UINT8 nLen);
void TestLogTimeFileWrite();
void TestLogTypeFileWrite(UINT8 nStatus);
UINT8 YKGetDeviceID(UINT16 nAddr,UINT8 *pAddr);
void LVScanfFileOpen(UINT8 nDeviceID);
void LicenseFileOpen(UINT8 nDeviceID);

UINT8 GetPointRecord(UINT8 *aTimeBuf);
UINT8 GetPointRecordStatus(void);

void FileWrite(int fd,UINT8 *aBuf,UINT8 nLen);

//void pn_delay(unsigned int nMillisecond);

unsigned short CRC16 (unsigned char * puchMsg,unsigned short usDataLen );
UINT8 PlcScanDev(UINT8 uCom,UINT8 *uDeviceCharacter);//南向扫描前单独查询PLC信息
UINT8 PlcModbusSend(UINT8 nPort,UINT8 nSecAddr,UINT8 *aEsnTemp,UINT8 *aRecvBuf);//0 代表COM1   1代表COM2
void PlcC7SendtoPlatform();
void PlcAlarmEraseSend(UINT8 uCom);
void PlcAlarmActSend(UINT8 uCom);
UINT8 PlcGetAlarmStatus();
void PlcIni();


void HandleYT(UINT8 *Msg);//根据下发数据设置YT到对应寄存器
int SouthCmdTask(UINT8 *aSendBuf,UINT8 aSendLen,UINT8 *aRecvBuf,UINT8 uDeviceId);//遥调南向收发


void SouthQueryTask(UINT8 *Msg,UINT8 uLen);
void SouthSetTask(UINT8 *Msg,UINT8 uLen);
void InitialLoggerInf();//处理c0时数采信息跑飞
void SendSofeVersionToPlatform(UINT16 nDeviceID,UINT16 nCmdID,UINT16 nFlag,UINT8 *aValue,UINT8 nLen);//SoftVersion Update
void GPIOSet(UINT8 nLine,UINT8 nPin,UINT8 nValue);
void MoveFile(UINT8 type, char *file_name, char *path);
void UpdataFileOpen();
void UpdataFileSave();
void LicenseFileWrite(UINT8 *aBuf,UINT8 nLen);
void LicenseFileSave();
void FileOpen();
void FileSave();
void *PeriodicResetThread();


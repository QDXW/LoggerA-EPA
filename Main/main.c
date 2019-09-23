/******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : main.c
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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/signal.h>

#include "PortType.h"
#include "PortTypeExtern.h"
#include "../include/ConstDef.h"
#include "../interface/interfaceExtern.h"
#include "../protocal/ToolExtern.h"
#include "../Ethernet/EthernetExtern.h"
#include "../EnetPortPrs/Port_FunExtern.h"
#include "../TPMsgDef/TPMsgDef.h"
#include "../ThreadPool/SPThreadPoolExtern.h"
#include "../Module/Module.h"
#include "../SGCC/store.h"
#include "../SGCC/protocol.h"
#include "../Alarm/alarmExtern.h"

extern PortType sPortType;
char g_AppPath[255];

/*****************************************************************************
* Function     : HsReboot()
* Description  : reboot OAM when OAM exit
* Input        : None
* Output       : None
* Return       : None
* Note(s)      : called before this program normally exit
* Contributor  : 2018年11月13日        Andre
*****************************************************************************/
void HsReboot(void)
{
    // call program named "file" to clean up.
    system("/mnt/flash/OAM/file");
}

/*****************************************************************************
* Function     : main()
* Description  : main function
* Input        : None
* Output       : None
* Return       : None
* Note(s)      :
* Contributor  : 2018年11月13日        Andre
*****************************************************************************/
int main(int   argc,   char*   argv[])
{
    pthread_t North_Thread_Master,North_Thread_Slave,Local_thread,Deamon_thread,SocketRecv_Thread,
              Scanf_Thread,APP_Thread,TotalCall_Thread,Alarm_Thread_Master,Alarm_Thread_Slave,
              PowerAlarm_Thread,ModemScanf_Thread,IV_Thread,SGCCRecv_Thread,SGCCSend_Thread,Reset_Thread,South_Update_thread;
    int nPos = 0;
    Struct_PortID spm;

    gCurrentDayNum = 0;
    gIsReportInfo  = _NO;
    hasReportSIM   = _NO;

    /**
    ** TA: Exit Function, called before this program normal exit
    */
    atexit(HsReboot);
    sprintf(g_AppPath,"%s",argv[0]);
    for(nPos = strlen(g_AppPath)-1;nPos > 0; nPos--)
    {
        if(g_AppPath[nPos] != '/')
            g_AppPath[nPos] = 0;
        else
            break;
    }

    /*I2Cfd open in this place,in order to avoid the open device error when the fd is same with FPGA fd*/
    /**TA: which device dose the I2C interface connected? EEPROM and RTC*/
    nI2Cfd = open(HS_I2C,O_RDWR);//2011.01.04
    if(nI2Cfd < 0)//2011.01.04
    {
        close(nI2Cfd);//2011.01.04
        nI2Cfd = open(HS_I2C,O_RDWR);
        DbgPrintf("INTERFACE-----open e2prom or i2c device error!~\n");//2011.01.04
    }

    /*FPGAfd open in this place,in order to avoid the open device error when the fd is same with I2Cfd.2012.01.04*/
    /** Connect CPLD, switch UART interface COM1 and COM2 */
    nFPGAfd = open(HS_FPGA,O_RDWR);//2011.01.04
    if(nFPGAfd < 0)//2011.01.04
    {
        close(nFPGAfd);//2011.01.04
        /**TA: Why here need open again?
        * if open failed, does the code below can work normally?
        */
        nFPGAfd = open(HS_FPGA,O_RDWR);
        DbgPrintf("INTERFACE-----open fpga device error!~\n");//2011.01.04
    }

    /**
    * TA: Create deamon thread Deamon_Monitor, which used for sending IPC messages
    * Implemented at protocal/Tool.c
    */
    pthread_create(&Deamon_thread,NULL,Deamon_Monitor,(void *)NULL); //deamon msg thread init
    DbgPrintf("#######create Deamon_Monitor####\n");

    /**
    * TA: Init driver
    * Implemented at interface/interface.c
    */
    DriverInit();
    InitThreadPool();  //pthreadpool init

    /**
    * TA: Init Ports: socket, Uart and USB
    * But Uart and USB init code is empty
    */
    system("cat /proc/interrupts");
    initPort();

    spm.Port_MsgID = sPortType[EnetPortType].NetPort_Msg_id;
    spm.Port_TypeID = sPortType[EnetPortType].protid.sockfd;/** TA: protid spells wrong? (portid)*/

    DbgPrintf("#############  North  Thread  Master ####################\r\n");
    pthread_create(&North_Thread_Master, NULL, (void *)NorthernProcessingThreadMaster, (void *)NULL);

    DbgPrintf("#############  North  Thread  Slave  ####################\r\n");
    pthread_create(&North_Thread_Slave, NULL, (void *)NorthernProcessingThreadSlave, (void *)NULL);

    DbgPrintf("#############  Socket Buffer Thread  ####################\r\n");
    /** process data recieved from socket */
    pthread_create(&SocketRecv_Thread, NULL, (void *)SocketRecvbufThread, (void *)NULL);

    DbgPrintf("#############  RS485  Server Thread  ####################\r\n");
    /** modbus handler for down-link device communication */
    pthread_create(&Local_thread,NULL,LocalThread,(void *)NULL);

	DbgPrintf("#############  South  Update Thread  ####################\r\n");
    /** modbus handler for down-link device communication */
    pthread_create(&South_Update_thread,NULL,SouthUpdatethread,(void *)NULL);

    DbgPrintf("#############  APP    Server Thread  ####################\r\n");
    /** APP connect to Logger, no buffer */
    pthread_create(&APP_Thread, NULL, APThread, (void *)NULL);

    DbgPrintf("#############  Alarm  Thread  Master ####################\r\n");
    pthread_create(&Alarm_Thread_Master, NULL, AlarmThreadMasterChannel, (void *)NULL);

    DbgPrintf("#############  Alarm  Thread  Slave  ####################\r\n");
    pthread_create(&Alarm_Thread_Slave, NULL, AlarmThreadSlaveChannel, (void *)NULL);

    DbgPrintf("#############  Total Call Thread  ####################\r\n");
    pthread_create(&TotalCall_Thread, NULL, TotalCallThread, (void *)NULL);

    DbgPrintf("#############  Device  Scanner Thread  ####################\r\n");
    /** if new device, scan down-link hua-wei device; if old device, detect new add device */
    pthread_create(&Scanf_Thread, NULL, ScanfDeviceThread, (void *)NULL);

    DbgPrintf("#############  Power  Alarm  Thread  ####################\r\n");
    /** detecting power off event */
    pthread_create(&PowerAlarm_Thread, NULL, PowerAlarmThread, (void *)NULL);

    DbgPrintf("#############  Modem Information Thread  ####################\r\n");
    /** detecting modem information */
    pthread_create(&ModemScanf_Thread, NULL, ModemScanfThread, (void *)NULL);

    //DbgPrintf("#############  IV     Thread  ####################\r\n");
    //pthread_create(&IV_Thread, NULL, IVThread, (void *)NULL);

    DbgPrintf("#############  SGCC   Receive Thread  ####################\r\n");
    pthread_create(&SGCCRecv_Thread, NULL, SGCCSocketReceiveThread, (void *)NULL);

    DbgPrintf("#############  SGCC   Send    Thread  ####################\r\n");
    pthread_create(&SGCCSend_Thread, NULL, SendSGCCMsgThread, (void *)NULL);

    DbgPrintf("#############  Periodic  Reset  Thread  ####################\r\n");
    pthread_create(&Reset_Thread, NULL, PeriodicResetThread, (void *)NULL);

    while(1)
    {
        sleep(30);
    }
}

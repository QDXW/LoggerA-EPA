#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "common.h"

int speed_arr[]={B115200,B38400, B19200, B9600, B4800, B2400, B1200, B300};
int name_arr[] = {115200,38400,  19200,  9600,  4800,  2400,  1200,  300};

UINT16 dSouth_Record_Information = 0;
extern int nModemFd;
extern void hs_udelay(unsigned int udelayTime);
extern sDeviceInfo gDeviceInfo[MAXDEVICE];
extern UINT8 gMBQueryDeviceCountBuf;

/******************UART properties******************************
no parity       8bit    Option.c_cflag &= ~PARENB;
                        Option.c_cflag &= ~CSTOPB;
                        Option.c_cflag &= ~CSIZE;
                        Option.c_cflag |= ~CS8;
odd parity(Odd) 7bit    Option.c_cflag |= ~PARENB;
                        Option.c_cflag &= ~PARODD;
                        Option.c_cflag &= ~CSTOPB;
                        Option.c_cflag &= ~CSIZE;
                        Option.c_cflag |= ~CS7;
even parity(Even) 7bit  Option.c_cflag &= ~PARENB;
                        Option.c_cflag |= ~PARODD;
                        Option.c_cflag &= ~CSTOPB;
                        Option.c_cflag &= ~CSIZE;
                        Option.c_cflag |= ~CS7;
Space parity    7bit    Option.c_cflag &= ~PARENB;
                        Option.c_cflag &= ~CSTOPB;
                        Option.c_cflag &= &~CSIZE;
                        Option.c_cflag |= CS8;
*******************************************************************/
//fd         int     UART fd
//databits   short   7 or 8
//parity     char    type£ºN,E,O,S
//stopbits   short   1 or 2

/*****************************************************************************
* Description:      read data from uart
* Parameters:        id:uart file id
                     buffer:read data buffer
* Returns:           read data length
*****************************************************************************/
int readDev(int id, unsigned char *buffer)
{
	int rnum = -1;
	int fd = id;
	int i;
	unsigned char nLen=0,nLenAll=0;
	unsigned char aTemp[255];
	int nErrorCount=0;

	while(rnum<=0)
	{
		rnum = read(fd,aTemp,255);

		if(rnum<=0)
		{
			usleep(500);

			nErrorCount++;
			if(nErrorCount>100)
			{
				DbgPrintf("Receive 485 Packet NULL\r\n");
				return -1;
			}
			continue;
		}
		else
		{
			nErrorCount=0;
			memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
			DbgPrintf("recv:",rnum);
			for(i=0;i<rnum;i++)
			{
				DbgPrintf("%02X ",buffer[i]);
			}
			DbgPrintf("\r\n");
			LogComRecordFileWrite(1,buffer,rnum);
			if((nLen+rnum)<3)
			{
				nLen +=rnum;
				rnum =-1;
				continue;
			}
			else
			{
				nLen +=rnum;
				if((buffer[1]&0x80)!=0)
				{
					return nLen;
				}
				if(buffer[1]==0x41)
					nLenAll=buffer[3]+6;
				else if((buffer[1]==0x06) || (buffer[1]==0x10))
					nLenAll=nLen;
				else
					nLenAll=buffer[2]+5;
				while(nLen<nLenAll)
				{
					rnum = read(fd,aTemp,255);
					if(rnum<=0)
					{
						usleep(200);

						nErrorCount++;
						if(nErrorCount>200)
						{
							DbgPrintf("Receive 485 Packet\r\n");
							return -1;
						}
						continue;
					}
					else
					{
//						DbgPrintf("Resend 485 Packet:\r\n");
						for(i=0;i<rnum;i++)
						{
							DbgPrintf("%02X ",buffer[i]);
						}
						nErrorCount=0;
						memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
						nLen +=rnum;
					}
				}
				DbgPrintf("\r\n");
				break;
			}
		}
	}
	return nLen;
}

/*****************************************************************************
* Description:      write data to uart
* Parameters:        id:uart file id
                     buffer:write data buffer
                     length:write data length
* Returns:           write length
*****************************************************************************/
int writeDev(int id,unsigned char *buffer, int length)
{
    int wnum;
    UINT8 buf[256];
    UINT8 uDeviceID=buffer[0];
    int uSeconds;
    read(id,buf,256);

    if(gDeviceInfo[uDeviceID].nRate!=4800)
    {
        if(length>=10)
        {
            uSeconds=25*length;
        }
        else
        {
            uSeconds=200;
        }
    }
    else
    {
        if(length>=10)
        {
            uSeconds=25*length;
        }
        else
        {
            uSeconds=200;//2100*length;
        }
    }
    wnum=write(id,buffer,length);
    fsync(id);
    if(gDeviceInfo[uDeviceID].nRate==4800)
    {
        hs_udelay(uSeconds);
    }
    else
    {
        hs_udelay(uSeconds * 42);
    }
    return wnum;
}

/*****************************************************************************
* Description:      read esn from uart
* Parameters:        id:uart file id
                     buffer:read data buffer
* Returns:           read data length
*****************************************************************************/
int readDevEsn(int id, unsigned char *buffer)
{
    int rnum = -1;
    int fd = id;
    int i;
    unsigned char nLen=0,nLenAll=0;
    unsigned char aTemp[256];
    int nErrorCount=0;

    while(rnum<=0)
    {
        usleep(500);
        rnum = read(fd,aTemp,255);
        if(rnum<=0)
        {
            nErrorCount++;
            if(nErrorCount>100)
            {
                DbgPrintf("Receive 485 Packet NULL\r\n");
                return -1;
            }
            continue;
        }
        else
        {
            memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
            DbgPrintf("recv:");
            for(i=0;i<rnum;i++)
            {
                DbgPrintf("%02X ",buffer[i]);
            }
            DbgPrintf("\r\n");
            if((nLen+rnum)<3)
            {
                nLen +=rnum;
                rnum =-1;
                continue;
            }
            else
            {
                nLen +=rnum;
                if(((buffer[1]&0x80)!=0)||(buffer[1]!=0x3B))
                {
                    return -1;
                }
                if(buffer[6]==0x87)
                    nLenAll=buffer[12]+15;
                else
                    nLenAll=buffer[10]+13;
                while(nLen<nLenAll)
                {
                    rnum = read(fd,&aTemp,256);
                    if(rnum<=0)
                    {
                        usleep(300);
                        nErrorCount++;
                        if(nErrorCount>100)
                        {
                            DbgPrintf("Recv len= %d,Alllen=%d\r\n",nLen,nLenAll);
                            DbgPrintf("Receive 485 Packet\r\n");
                            return -1;
                        }
                        continue;
                    }
                    else
                    {
                       memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
                       nLen +=rnum;
                    }
                }
                break;
            }
        }
    }
    /*printf("recv:");
    for(i=0;i<nLenAll;i++)
    {
        printf("%02X ",buffer[i]);
    }
    printf("\r\n");*/
    return nLenAll;
}

/*****************************************************************************
* Description:      read data from AP module
* Parameters:        id:uart file id
                     buffer:read data buffer
* Returns:           read data length
*****************************************************************************/
int readAP(int id, unsigned char *buffer)
{
    int rnum = -1;
    int fd = id;
    unsigned char nLen=0,nLenAll=0;
    unsigned char aTemp[255];

    //fcntl(fd,F_SETFL,0);

    while(rnum<=0)
    {
        usleep(500);
        pthread_mutex_lock(&APsem);
        rnum = read(fd,aTemp,255);
        pthread_mutex_unlock(&APsem);
        if(rnum<=0)
        {
            continue;
        }
        else
        {
            memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
            if((nLen+rnum)<2)
            {
                nLen +=rnum;
                rnum =-1;
                continue;
            }
            else
            {
                nLen +=rnum;
                nLenAll=buffer[1]+2;
                while(nLen<nLenAll)
                {
                    pthread_mutex_lock(&APsem);
                    rnum = read(fd,&aTemp,255);
                    pthread_mutex_unlock(&APsem);
                    if(rnum<=0)
                        continue;
                    else
                    {
                       memcpy((unsigned char *)&buffer[nLen],aTemp,rnum);
                       nLen +=rnum;
                    }
                }
                break;
            }
        }
    }
    /*printf("recv:");
    for(i=0;i<nLenAll;i++)
    {
        printf("%02X ",buffer[i]);
    }
    printf("\r\n");*/
    return nLenAll;
}

/*****************************************************************************
* Description:      write data to AP module
* Parameters:        id:uart file id
                     buffer:write data buffer
                     length:write data length
* Returns:           write data length
*****************************************************************************/
int writeAP(int id,unsigned char *buffer, int length)
{
    int wnum,len=0;
    int fd = id;

    pthread_mutex_lock(&APsem);
    wnum=write(fd,buffer,len);
    pthread_mutex_unlock(&APsem);
    return wnum;
}

/*****************************************************************************
* Description:      write data to Modem uart
* Parameters:        buffer:data buffer
                     len:data length
* Returns:
*****************************************************************************/
int writeModem(unsigned char *buffer,UINT16 nLen)
{
    //printf("Modem Send : %s\r\n",buffer);
    write(nModemFd,buffer,nLen);
    return 0;
}

/*****************************************************************************
* Description:      read data from Modem uart
* Parameters:        buffer:data buffer
* Returns:           data length
*****************************************************************************/
int readModem(unsigned char *buffer)
{
    int rnum = -1;
    int nErrorCount;
    unsigned char aTemp[512];

    memset(aTemp,0,sizeof(aTemp));
    nErrorCount=0;
    while(rnum<=0)
    {
        usleep(500);
        rnum = read(nModemFd,aTemp,512);
        if(rnum<=0)
        {
            nErrorCount++;
            if(nErrorCount>200)
                return -1;
            continue;
        }
        else
        {
            DbgPrintf("Modem Recv = %s\r\n",aTemp);
            memcpy(buffer,aTemp,512);
            /*if(strstr(buffer,"+QIURC")!=NULL)
                gModemRecvFlag=1;*/
        }
    }
    /*if(strstr(buffer,"OK")==NULL){
        printf("recv:");
        for(i=0;i<rnum;i++)
        {
            printf("%02X ",buffer[i]);
        }
        printf("\r\n");*/
    //}
    return rnum;
}

/*****************************************************************************
* Description:      Send message to platform with Modem
* Parameters:        aSendbuf:data buffer
                     len:data length
* Returns:
*****************************************************************************/
void ModemSend(UINT8 socket_id,UINT8 *aSendbuf,UINT16 nLen)
{
    UINT8 aBuf[50]="AT+QISEND=%d,%d\r\n",aBufTemp[50]="AT+QISEND=%d,%d\r\n"; //"AT+QSSLSEND=%d,%d\r\n";
    UINT8 aRecvBuf[512],aCmd[50];
    UINT8 nEsc=27,nErrorResendCount=0,nSendOKFlag=0;
    UINT8 rnum,i;
    int nErrorCount=0;

    while(nErrorResendCount<3)
    {
        NorthPrintf("[Channel %d]Send Mess :",socket_id);
        for(i=0;i<nLen;i++)
        {
            NorthPrintf("%02X ",aSendbuf[i]);
        }
        NorthPrintf("\r\n");

        //write(nModemFd,&nEsc,1);
        //pn_delay(1000);

        if(socket_id==0)
        {
            sprintf((void *)aCmd,(void *)aBuf,socket_id, nLen);
        }
        else if(socket_id==1)
        {
            sprintf((void *)aCmd,(void *)aBufTemp,socket_id, nLen);
        }
        else
        {
            DbgPrintf("[ModemSend]Unavailable socket ID\r\n");
        }

        nErrorCount=0;
        writeModem(aCmd,strlen((void *)aCmd));
        do{
            memset(aRecvBuf,0,sizeof(aRecvBuf));
            rnum = readModem(aRecvBuf);
            usleep(100);
            if(strstr((void *)aRecvBuf,">")==NULL)
            {
                nErrorCount++;
                DbgPrintf("[Channel %d]send error %d times in step.1\r\n",socket_id,++nErrorCount);
            }
            if(nErrorCount>3)
            {
                write(nModemFd,&nEsc,1);
                sleep(1);
                break;
            }
            if((strstr((void *)aRecvBuf,"closed")!=NULL)||(strstr((void *)aRecvBuf,"ERROR")!=NULL))
            {
                write(nModemFd,&nEsc,1);
                sleep(1);
                return;
            }
        }while(strstr((void *)aRecvBuf,">")==NULL);
        if(nErrorCount>3)
        {
            nErrorResendCount++;
            continue;
        }
        write(nModemFd,aSendbuf,nLen);

        //printf("testtest %d\r\n",nLen);
        usleep(500);
        nErrorCount=0;
        do{
            memset(aRecvBuf,0,sizeof(aRecvBuf));
            rnum = readModem(aRecvBuf);
            if(nErrorCount>5)
            {
                nErrorResendCount++;
                break;
            }
            nErrorCount++;
            if((strstr((void *)aRecvBuf,"closed")!=NULL)||(strstr((void *)aRecvBuf,"ERROR")!=NULL))
            {
                write(nModemFd,&nEsc,1);
                return;
            }
        }while(strstr((void *)aRecvBuf,"SEND OK")==NULL);
        if(strstr((void *)aRecvBuf,"SEND OK")!=NULL)
        {
            nSendOKFlag=1;
            break;
        }
    }
    if(nSendOKFlag)
    {
        DbgPrintf(" [Channel %d]OK\r\n",socket_id);
    }
    else
    {
        DbgPrintf(" [Channel %d]ERROR\r\n",socket_id);
    }
    write(nModemFd,&nEsc,1);
    usleep(5000);
}

/*****************************************************************************
* Description:      close uart device
* Parameters:        fd:uart file id
* Returns:
*****************************************************************************/
void closeDev(int fd)
{
    close(fd);
    fd=0;
}


/*****************************************************************************
* Description:      open uart device
* Parameters:        ttys:uart device number
                     speed:boundrate
* Returns:           uart file id
*****************************************************************************/
int UartOper(int ttyS, int speed)
{
    char *Dev[11]= {"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3"};
    int speed_arr[]= {B115200,B38400, B19200, B9600, B4800, B2400, B1200, B300};
    int name_arr[] = {115200,38400,  19200,  9600,  4800,  2400,  1200,  300};
    struct termios old_cfg,new_cfg;
//    struct serial_struct serial;
    int i;
    int fd,ret;

    if((ttyS==1)||(ttyS==3))
        fd = open(Dev[ttyS], O_RDWR | O_NOCTTY | O_NDELAY);         //| O_NOCTTY | O_NDELAY
    else
        fd = open(Dev[ttyS], O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd)
    {
        DbgPrintf("Can't Open Serial Port");
        return -1;
    }

    if(tcgetattr(fd, &old_cfg) != 0)
    {
        DbgPrintf("tcgetattr\n");
        return -1;
    }
    new_cfg = old_cfg;
    cfmakeraw(&new_cfg);//defualt
    new_cfg.c_cflag |= CLOCAL | CREAD;//active option
    //set speed
    tcflush(fd, TCIOFLUSH);
    for ( i= 0;  i < sizeof(speed_arr)/sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            //printf(" speed=%d\n",name_arr[i]);
            cfsetispeed(&new_cfg, speed_arr[i]);
            cfsetospeed(&new_cfg, speed_arr[i]);
        }
    }//for(speed)
    //set data length 8 bit
    new_cfg.c_cflag &= ~CSIZE;
    new_cfg.c_cflag |= CS8;
    //parity bit
    if(speed == 4800)
    {
        new_cfg.c_cflag &=~ PARENB;
        new_cfg.c_iflag &=~ INPCK;
    }
    else
    {
        new_cfg.c_cflag &=~ PARENB;
        new_cfg.c_iflag &=~ INPCK;
    }
    new_cfg.c_cflag &=~ PARENB;
    new_cfg.c_iflag &=~ INPCK;
    //1bit stop bit
    new_cfg.c_cflag &=~ CSTOPB;
    //none data stream control
    new_cfg.c_cflag &=~ CRTSCTS;
    new_cfg.c_iflag &=~ IXON;
    new_cfg.c_iflag &=~ IXOFF;
    //min byte and wait time
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 2;

    tcflush(fd,TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &new_cfg) != 0)
    {
        DbgPrintf("tcsetattr\n");
        return -1;
    }
    return fd;
}

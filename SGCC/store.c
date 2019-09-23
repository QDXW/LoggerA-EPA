/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : store.c
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
#include <pthread.h>
#include "../protocal/ProtocolDef.h"
#include "../interface/common.h"
#include "../interface/uartExtern.h"
#include "../Ethernet/EthernetExtern.h"

struct sThirdPartyFrameBuffer *p_sgcc_send_msg_head, *p_sgcc_send_msg_point, *p_sgcc_recv_msg_head, *p_sgcc_recv_msg_point;

extern UINT8 gModuleChannel1InitFlag;
extern int g_ethernet_connect_sub;

/*****************************************************************************
* Description: 		清除发送缓存区
* Parameters:
* Returns:
*****************************************************************************/
void ClearMsgFromSGCCSendBuff(void)
{
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    while(p_sgcc_send_msg_buff != NULL)
    {
        p_sgcc_send_msg_head = p_sgcc_send_msg_buff->p_next;
        p_sgcc_send_msg_buff->p_next = NULL;
        free(p_sgcc_send_msg_buff);
        p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    }
    p_sgcc_send_msg_head = NULL;
    p_sgcc_send_msg_point = NULL;
    pthread_mutex_unlock(&sgcc_send_buff_sem);
}

/*****************************************************************************
* Description: 		添加发送报文至发送缓存区
* Parameters:		pipe:发送通道
					p_msg_buff:发送报文
					length:发送报文长度
* Returns:
*****************************************************************************/
UINT8 AddMsgToSGCCSendBuff(UINT8 *p_msg_buff, UINT8 length)
{
    UINT8 res = 1,i;
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = (struct sThirdPartyFrameBuffer *)malloc(sizeof(struct sThirdPartyFrameBuffer));//申请链表节点空间
    p_sgcc_send_msg_buff->msg_length = length;
    memset(p_sgcc_send_msg_buff->msg, 0, sizeof(p_sgcc_send_msg_buff->msg));
    memcpy(p_sgcc_send_msg_buff->msg, p_msg_buff, length);
    p_sgcc_send_msg_buff->p_next = NULL;

    NorthPrintf("**add send buff**");
    for(i =0 ;i<length;i++)
    {
        NorthPrintf("%02X ",p_msg_buff[i]);
    }
    NorthPrintf("\r\n");
    if(p_sgcc_send_msg_head == NULL)
    {
        /*空链表*/
        p_sgcc_send_msg_head = p_sgcc_send_msg_buff;
        p_sgcc_send_msg_point = p_sgcc_send_msg_buff;
    }
    else
    {
        /*非空链表加入链表尾部*/
        p_sgcc_send_msg_point->p_next = p_sgcc_send_msg_buff;
        p_sgcc_send_msg_point = p_sgcc_send_msg_buff;
    }
    pthread_mutex_unlock(&sgcc_send_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		从北向发送缓存区中获取待发送的报文
* Parameters:		p_msg_buff:发送报文
					p_length:发送报文长度
* Returns:          0:无数据      其他值:发送的socket id
*****************************************************************************/
INT32 GetMsgFromSGCCSendBuff(UINT8 *p_msg_buff,UINT8 *p_length)
{
    INT32 res = 0;
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    while(p_sgcc_send_msg_buff != NULL)
    {
        if(1)
        {
            memcpy(p_msg_buff, p_sgcc_send_msg_buff->msg, p_sgcc_send_msg_buff->msg_length);
            memcpy(p_length, (UINT16 *)&p_sgcc_send_msg_buff->msg_length, sizeof(p_sgcc_send_msg_buff->msg_length));
            if(p_sgcc_send_msg_buff->p_next == NULL)
            {
                p_sgcc_send_msg_head = NULL;
                p_sgcc_send_msg_point = NULL;
            }
            else
            {
                p_sgcc_send_msg_head = p_sgcc_send_msg_buff->p_next;
            }
            free(p_sgcc_send_msg_buff);
            break;
        }
    }
    pthread_mutex_unlock(&sgcc_send_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		添加数据至缓存区中
* Parameters:		p_msg_buff:发送报文
					p_length:发送报文长度
* Returns:          1:添加成功
*****************************************************************************/
UINT8 AddMsgToSGCCReceiveBuff(UINT8 *p_msg_buff,UINT8 length)
{
    UINT8 res = 1;
    struct sThirdPartyFrameBuffer *p_sgcc_recv_msg_buff;

    pthread_mutex_lock(&sgcc_recv_buff_sem);
    p_sgcc_recv_msg_buff = (struct sThirdPartyFrameBuffer *)malloc(sizeof(struct sThirdPartyFrameBuffer));
    p_sgcc_recv_msg_buff->msg_length = length;
    memset(p_sgcc_recv_msg_buff->msg, 0, sizeof(p_sgcc_recv_msg_buff->msg));
    memcpy(p_sgcc_recv_msg_buff->msg, p_msg_buff, length);
    p_sgcc_recv_msg_buff->p_next = NULL;
    if(p_sgcc_recv_msg_head == NULL)
    {
        p_sgcc_recv_msg_head = p_sgcc_recv_msg_buff;
        p_sgcc_recv_msg_point = p_sgcc_recv_msg_buff;
    }
    else
    {
        p_sgcc_recv_msg_point->p_next = p_sgcc_recv_msg_buff;
        p_sgcc_recv_msg_point = p_sgcc_recv_msg_buff;
    }
    pthread_mutex_unlock(&sgcc_recv_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		从接收缓存区中获取已接收的报文
* Parameters:		p_msg_buff:发送报文
					p_length:发送报文长度
* Returns:          0:无数据      其他值:接收的socket id
*****************************************************************************/
UINT8 GetMsgFromSGCCReceiveBuff(UINT8 *p_msg_buff,UINT8 *p_length)
{
    UINT8 res = 0;
    struct sThirdPartyFrameBuffer *p_sgcc_recv_msg_buff;

    pthread_mutex_lock(&sgcc_recv_buff_sem);
    p_sgcc_recv_msg_buff = p_sgcc_recv_msg_head;
    if(p_sgcc_recv_msg_buff != NULL)
    {
        memcpy(p_msg_buff, p_sgcc_recv_msg_buff->msg, p_sgcc_recv_msg_buff->msg_length);
       	memcpy(p_length, (UINT8 *)&p_sgcc_recv_msg_buff->msg_length, sizeof(p_sgcc_recv_msg_buff->msg_length));
        if(p_sgcc_recv_msg_buff->p_next == NULL)
        {
            p_sgcc_recv_msg_head = NULL;
            p_sgcc_recv_msg_point = NULL;
        }
        else
        {
            p_sgcc_recv_msg_head = p_sgcc_recv_msg_buff->p_next;
        }
        free(p_sgcc_recv_msg_buff);
        res = 1;
    }
    pthread_mutex_unlock(&sgcc_recv_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		线程-接收报文处理线程
* Parameters:
* Returns:
*****************************************************************************/
void *SGCCSocketReceiveThread()
{
    UINT8 recv_buff[2000];
    UINT8 recv_length = 0;

    while(1)
    {
        if(GetMsgFromSGCCReceiveBuff(recv_buff, (UINT8 *)&recv_length))
        {
        	/**缓存中存在待处理报文*/
            UINT8 recv_msg_count = 0;

            while(recv_msg_count < recv_length)
            {
                struct MsgSGCCStruct recv_msg_struct;
                UINT8 send_temp[255], recv_temp[255];
                UINT8 send_length_temp = 0, recv_length_temp = 0, i;

                while(recv_msg_count <= recv_length)
                {
                    if(recv_buff[recv_msg_count] == 0x68)//报文缓存中找到起始符0x68
                        break;
                    else
                        recv_msg_count++;
                }
                if((recv_buff[recv_msg_count + 1] + 2) > (recv_length - recv_msg_count))//包长度大于缓存剩余数据
                    break;
                recv_length_temp = recv_buff[recv_msg_count + 1] + 2;
                memcpy(recv_temp, (UINT8 *)&recv_buff[recv_msg_count], recv_length_temp);
                //if(recv_length != recv_length_temp)
                {
                    NorthPrintf("**South recv**");
                    for(i =0 ;i<recv_length_temp;i++)
                    {
                        NorthPrintf("%02X ",recv_temp[i]);
                    }
                    NorthPrintf("\r\n");
                }
                DecodeMsg(recv_temp, recv_length_temp, (struct MsgSGCCStruct *)&recv_msg_struct);//解析报文
                send_length_temp = DealPacket(recv_msg_struct, send_temp);//处理报文
                recv_msg_count += recv_length_temp;
                if(send_length_temp >=6)
                    AddMsgToSGCCSendBuff(send_temp, send_length_temp);
            }
        }
        //usleep(50000);
        usleep(10);
    }
    pthread_testcancel();
}

void *SendSGCCMsgThread()
{
    while(1)
    {
        UINT8 socket_send_buff[SGCC_MSG_MAX_LENGTH], socket_send_length=0;

        if(1)//连接正常
        {
            GetMsgFromSGCCSendBuff(socket_send_buff, &socket_send_length);//从缓存区中获取报文
            if(socket_send_length>0)
            {

                if(gModuleChannel1InitFlag == 2)
                {
                    pthread_mutex_lock(&modemsem);
                    ModemSend(1,socket_send_buff,socket_send_length);
                    pthread_mutex_unlock(&modemsem);
                }
                else if(g_ethernet_connect_sub == 1)
                {
                    EthernetSend(socket_send_buff, socket_send_length,0,1);
                }
                else if(g_ethernet_connect_status == 1)
                {
                    EthernetSend(socket_send_buff, socket_send_length,1,1);
                }
            }
        }
        usleep(1000);
    }
}


/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : store.h
* Author            : Alex
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef STORE_H_
#define STORE_H_

extern void *SGCCSocketReceiveThread();
extern void *SendSGCCMsgThread();
extern UINT8 AddMsgToSGCCSendBuff(UINT8 *p_msg_buff, UINT8 length);
extern UINT8 AddMsgToSGCCReceiveBuff(UINT8 *p_msg_buff,UINT8 length);

#endif

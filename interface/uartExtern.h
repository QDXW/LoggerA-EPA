#ifndef UARTEXTERN_H_
#define UARTEXTERN_H_

#define COM_dev_ttys0   2
#define COM_speed       115200
#define COM_databits    8
#define COM_parity      'N'
#define COM_stopbits    1
#define COM_ReadLength  6


extern UINT16 dSouth_Record_Information;

extern int readDev(int id, unsigned char *buffer);
extern int readDevEsn(int id, unsigned char *buffer);
extern int readAP(int id, unsigned char *buffer);
extern int writeAP(int id,unsigned char *buffer, int length);
extern int writeDev(int id,unsigned char *buffer, int length);
extern int readModem(unsigned char *buffer);
extern int writeModem(unsigned char *buffer,UINT16 nLen);
extern void ModemSend(UINT8 socket_id,UINT8 *aSendbuf,UINT16 nLen);
extern void closeDev(int fd);
extern int UartOper(int ttyS, int speed);
#endif

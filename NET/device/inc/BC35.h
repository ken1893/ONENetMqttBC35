#ifndef _BC35_H_
#define _BC35_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void BC35_Init(void);

void BC35_Sockets(void);

void BC35_Clear(void);

void BC35_SendData(unsigned char *data, unsigned short len);

unsigned char *BC35_GetIPD(unsigned short timeOut);

#endif

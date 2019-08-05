/*
***********************************************************************************************
*                                           uC/OS-View
*
*                            (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : OS_VIEW.H
* Version    : V1.33
* Programmer : KEN
***********************************************************************************************
*/

#ifdef   COM_GLOBALS
#define  COM_EXT
#else
#define  COM_EXT  extern
#endif

//#include  <stm32f10x.h>
#include "config.h"

#ifndef  COM_H
#define  COM_H

#define  RX_BUF_SIZE        128u          // 串口接收最大处理长度

//#define  RX_STATE_SD0              0                /* waiting for start first  start delimiter (SD0)  */
//#define  RX_STATE_SD1              1                /* waiting for start second start delimiter (SD1)  */
#define  RX_STATE_ID               0                /* waiting for End ID (End ID)  */
#define  RX_STATE_LEN              1                /* waiting for len byte  根据功能码确定长度        */
#define  RX_STATE_DATA             4                /* waiting for data                                */
#define  RX_STATE_CHKSUM           5                /* waiting for checksum                            */
#define  RX_STATE_ED               6                /* waiting for end delimiter                       */
#define  RX_STATE_DATA1            7                /* waiting for data                                */


#define  PROTOCOL_RX_SD0        0xED                /* Start delimiters                                */
#define  PROTOCOL_RX_SD1        0x8C
#define  PROTOCOL_RX_ED         0x8D                /* End   delimiter                                 */
                               /* Outbound packets (to PC)                        */
#define  PROTOCOL_TX_SD0        0x8C                /* Start delimiters                                */
#define  PROTOCOL_TX_SD1        0xED
#define  PROTOCOL_TX_ED         0x8D                /* End   delimiter                                 */

// 迪文
#define  PROTOCOL_WAITING     0
#define  PROTOCOL_DI_S0       0x5a   // 迪文帧头0
#define  PROTOCOL_DI_S1       0xa5   // 迪文帧头1
#define  PROTOCOL_DI_LEN      1
#define  PROTOCOL_CMD         0x0c   // cmd

#define  ADD_ENERGY           0x1000
#define  ADD_TEMP             0x1004
#define  ADD_POWER            0x1006
#define  ADD_VOLT             0x1008
#define  ADD_CURRENT          0x100a
#define  ADD_LEAK             0x100e
#define  ADD_PF               0x1010 

COM_EXT  uint8_t              pkt_Flag;      // uart 485
COM_EXT  uint8_t              pk1t_Flag;     // uart1

COM_EXT  uint8_t              Rx1State;      // uart1 接收窗口数据状态
COM_EXT  uint8_t              RxState;        // 接收窗口数据状态
COM_EXT  uint8_t              RxRemainLen;
COM_EXT  uint8_t              Rx1RemainLen;   // uart1 remain length

COM_EXT  uint8_t              RxChkSum;

COM_EXT  uint8_t              RxBuf[RX_BUF_SIZE];  // 接收缓存字符串
COM_EXT  uint8_t              Rx1Buf[RX_BUF_SIZE];  // uart1 接收缓存字符串

COM_EXT  uint8_t              RxBufCnt;
COM_EXT  uint8_t              Rx1BufCnt;            // uart1

COM_EXT  uint8_t              RxRdIx;
COM_EXT  uint32_t             RxCtr;
COM_EXT  uint16_t             RxPktCtr;

COM_EXT  uint8_t    			    TxLen; 
COM_EXT  uint8_t				      TxEnable_Flag;	// 允许发送标志位

COM_EXT  uint8_t              TxBuf[50];      // MAX 50 BYTE

COM_EXT  uint8_t              MulLen;
COM_EXT  uint16_t             RxTimeCnt;      // 接收计时 timer
COM_EXT  uint16_t             Rx1TimeCnt;      // 接收计时 timer
COM_EXT  uint8_t              ComStart_Flag;  // 等待一帧数据起始字节

COM_EXT struct crcstruct
{
	unsigned char crcH;
	unsigned char crcL;
}crcs;

void cpytex(uint8_t *dst, const uint8_t *src);    // 

COM_EXT void  RxHandler (uint8_t rx_data);         // modbus handle
COM_EXT void  U1RxISRHandler(uint8_t rx_data);     // diwi   handle 
COM_EXT void Send_diwi(uint16_t val,uint16_t add);
COM_EXT void Send_diwi4(uint16_t val,uint16_t val2,uint16_t add);


COM_EXT void  RxISRHandler(void);
COM_EXT void  TxISRHandler(void);

COM_EXT void UartDeal(uint8_t suquence);     // 处理接收数据

COM_EXT void Ask_pros(uint8_t suquence);     // 设备查询
COM_EXT void SW_Con(uint8_t suquence,uint8_t sta);  // 开关控制

COM_EXT void EndRequest(uint8_t deviceid,uint16_t cmd,uint16_t value);


#endif



/*
*********************************************************************************************************
*                                           uC/OS-App com.c
*
*                                     (c) Copyright 2015, Ken
*                                       All Rights Reserved
*
*
* Filename   : com.C
* Version    : V1.01
* Programmer : Ken
*********************************************************************************************************
*/

#define  COM_GLOBALS
#include <config.h>
#include "com.h"
#include "usart.h"
//#include "EEPROM.h"



/* CRC 高位字节值表 */ 
const unsigned char auchCRCHi[256] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 

/* CRC低位字节值表*/ 
const unsigned char auchCRCLo[256] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
};


void  RxISRHandler(void);                  /* C code to process character received           */

static  void  RxBufClr (void);
static  void  RxStoINT8U (uint8_t rx_data);

void  RxIntDis (void);
void  RxIntEn (void);
struct crcstruct crc16(unsigned char *puchMsg, unsigned int usDataLen);

void ModbusPro(unsigned char com);

// MODBUS CMD MODE 
void Read_InputRegister(void);     // 读输入寄存器
void Read_HoldingRegister(void);   // 讀保持寄存器
void Write_SingleHolding(uint8_t suquence,uint8_t cmd,uint16_t RegAddr,uint16_t val);    // 写单个保持寄存器   0x06
void Write_MultiHolding(void);	   // 写多个保持寄存器   

void Send_CommonReg(uint8_t* pBuffer, uint8_t Num);	   // 发送装载数据并计算校验

/************************************************************************************
** 函数名称: crc16
** 功能描述: 对接收数据进行crc校验
** 输　入: 地址指针 *puchMsg, 数据长度 usDataLen
** 输　出: crcstruct (crc结构体)
** 全局变量: 无
** 调用模块:
*************************************************************************************/
struct crcstruct crc16(unsigned char *puchMsg, unsigned int usDataLen) 
{ 
	unsigned char uchCRCHi = 0xFF;   //高CRC字节初始化  
	unsigned char uchCRCLo = 0xFF;   //低CRC字节初始化  
	unsigned int uIndex;             //CRC循环中的索引 
	struct crcstruct temp;   //返回值
	 
	while(usDataLen--)       // 传输消息缓冲区  
	{ 
		uIndex = uchCRCHi ^ *puchMsg++;             // 计算CRC 
		uchCRCHi = uchCRCLo ^ (auchCRCHi[uIndex]); 
		uchCRCLo = auchCRCLo[uIndex]; 
	} 	

	temp.crcH = uchCRCHi;
	temp.crcL = uchCRCLo;

	return(temp);
}

/********************************************************************************
*  UartDeal(void)
*  函数功能: 处理串口接收
*********************************************************************************/
void UartDeal(uint8_t suquence)
{
	unsigned char numTemp = 0;    // 数据个数
	struct crcstruct temp;
	
	RxIntDis();			  // 关接收中断
		
  switch(RxBuf[1])
	{
		case 0x04:                    // 获取输入寄存器
			numTemp = 29;               // 单相设备输入寄存器
			break;
		
		case 0x03:                    // Holding
			//numTemp = 26;
			break;
	  
		case 0x06:
			numTemp = 6;
			break;
		
		case 0x10:
			numTemp = MulLen;
			MulLen = 0;	
			break;
	}
				
	temp = crc16(RxBuf,numTemp);	
	if(temp.crcH == RxBuf[numTemp] && temp.crcL == RxBuf[numTemp + 1]){
			ModbusPro(RxBuf[1]);    // 处理modbus命令
	}
		
	RxBufClr();		   
	memset(RxBuf,0,sizeof(RxBuf));    // clear
	RxIntEn();                  // 开接收中断

}

//------------------------------------向终端发送请求---------------------------------
//
//
void EndRequest(uint8_t deviceid,uint16_t add,uint16_t value)
{
	uint8_t i;
	struct crcstruct temp;
	
	#ifdef RS485
		UARTSEND;
	#endif
	
	TxBuf[0] = deviceid;   // id
	
	TxBuf[1] = 0x06;                // cmd  input 输入寄存器
	
	TxBuf[2] = (add >> 8) & 0xff;   // start add H
	TxBuf[3] = add & 0xff;          // add L
	
	TxBuf[4] = (value >> 8) & 0xff;   // start add H
	TxBuf[5] = value & 0xff;          // add L
	
	temp = crc16(TxBuf,6);
	
	TxBuf[6] = temp.crcH;
	TxBuf[7] = temp.crcL;
	  
	TxLen = 8;
	
	TxEnable_Flag = 1;       // 允许发送

	if(TxEnable_Flag == 1){
		for(i=0;i< TxLen;i++){
			USART_SendData(USART_485,TxBuf[i]);	
			while(USART_GetFlagStatus(USART_485, USART_FLAG_TXE) == RESET);
		}
		#ifdef RS485
		USART_ITConfig(USART_485,USART_IT_TC,ENABLE);           // enable发送完成中断	
		#endif
		
		TxEnable_Flag = 0;       // 发送完毕，关闭发送使能
	}
}

//------------------------------------串口发送---------------------------------------
//   调用主动发送
//
//
/////////////////////////////////////////////////////////////////////////////////////
void Ask_pros(uint8_t suquence)
{
	uint8_t i;
	struct crcstruct temp;
	
	#ifdef RS485
		UARTSEND;
	#endif
	
	TxBuf[0] = Device[suquence][0];   // id
	
	TxBuf[1] = 0x04;   // cmd  input 输入寄存器
	
	TxBuf[2] = 0x00;   // start add
	TxBuf[3] = 0x00;   
	
	switch(Device[suquence][1])
	{
		case 0:
			TxBuf[4] = 0x00;   // count
	    TxBuf[5] = 0x0d;
			break;
		
		case 1:
			break;
		
		default:break;
	}
	temp = crc16(TxBuf,6);
	
	TxBuf[6] = temp.crcH;
	TxBuf[7] = temp.crcL;
	  
	TxLen = 8;
	
	TxEnable_Flag = 1;       // 允许发送

	if(TxEnable_Flag == 1){
		for(i=0;i< TxLen;i++){
			USART_SendData(USART_485,TxBuf[i]);	
			while(USART_GetFlagStatus(USART_485, USART_FLAG_TXE) == RESET);
		}
		#ifdef RS485
		USART_ITConfig(USART_485,USART_IT_TC,ENABLE);           // enable发送完成中断	
		#endif
		
		TxEnable_Flag = 0;       // 发送完毕，关闭发送使能
	}
}


void SW_Con(uint8_t suquence,uint8_t sta)
{
	Write_SingleHolding(suquence,0x06,0x0000,(uint16_t)sta);
}


/********************************************************************************
*  ModbusPro(unsigned char com)
*  函数功能: 处理串行modbus协议
*********************************************************************************/
void ModbusPro(unsigned char com)
{
	switch(com)
		{
			case 0x04:
				Read_InputRegister();    // 读输入寄存器
			break;
			
			case 0x03:
				Read_HoldingRegister();  // 讀保持寄存器
			break;
			
			case 0x06:                 // 寫單個保持寄存器
				//Write_SingleHolding();
			break; 

			case 0x10:                 // 写多个寄存器
				Write_MultiHolding();	
			break;
				
			default:break;
		}
}

/********************************************************************************
*  Read_InputRegister(void)
*  函数功能: 讀輸入寄存器
*********************************************************************************/
void Read_InputRegister(void)
{
	//SinglePhase[0].RegI
	uint8_t i,counttemp;
	
	counttemp = RxBuf[2] / 2;
	
	//SinglePhase[0].RegI[0] = RxBuf[(i * 2) + 3];
	//SinglePhase[0].RegI[0] = (SinglePhase[0].RegI[0] << 8) + RxBuf[(i * 2) + 4];
	
	for(i=0;i<counttemp;i++)
	{
		SinglePhase[0].RegI[i] = RxBuf[(i * 2) + 3];
	  SinglePhase[0].RegI[i] = (SinglePhase[0].RegI[i] << 8) + RxBuf[(i * 2) + 4];
	}
	switch(RxBuf[0])
	{
		case 1:
			break;
	}
	
	UsartPrintf(USART_DEBUG, "{\"PowerAll\":%d,\"Temp\":%d,\"Power\":%d,\"Vol\":%d,\"Cur\":%d,\"Sta\":3}",SinglePhase[0].RegS.EnegerL,SinglePhase[0].RegS.Temp,SinglePhase[0].RegS.Power,SinglePhase[0].RegS.Volt,SinglePhase[0].RegS.Current);
	UsartPrintf(USART_DEBUG, "Input successed:%d,\r\n",RxBuf[2]);
	UsartPrintf(USART_DEBUG,(char *)RxBuf);
}


/********************************************************************************
*  Read_HoldingRegister(void)
*  函数功能: 讀保持寄存器
*********************************************************************************/
void Read_HoldingRegister()
{
	
	switch(RxBuf[0])
	{
		case 1:
			break;
	}
	UsartPrintf(USART_DEBUG, "Holding successed:%d,\r\n",TxBuf[2]);
}

/********************************************************************************
*  Write_SingleHolding(void)
*  函数功能: 写單個保持寄存器成功
*********************************************************************************/
void Write_SingleHolding(uint8_t suquence,uint8_t cmd,uint16_t RegAddr,uint16_t val)
{
	uint8_t i;
	struct crcstruct temp;
	
	#ifdef RS485
		UARTSEND;
	#endif
	
	TxBuf[0] = Device[suquence][0];   // id
	
	TxBuf[1] = cmd;   // cmd  input 输入寄存器
	
	TxBuf[2] = (RegAddr >> 8)&0xff;   // start add
	TxBuf[3] = RegAddr & 0xff;   
	
	switch(Device[suquence][1])
	{
		case 0:              // 智能开关设备
			TxBuf[4] = (val >> 8)&0xff;   // control  switch or register
	    TxBuf[5] = val & 0xff;
			break;
		
		case 1:
			break;
		
		default:break;
	}
	temp = crc16(TxBuf,6);
	
	TxBuf[6] = temp.crcH;
	TxBuf[7] = temp.crcL;
	  
	TxLen = 8;
	
	TxEnable_Flag = 1;       // 允许发送

	if(TxEnable_Flag == 1){
		for(i=0;i< TxLen;i++){
			USART_SendData(USART_485,TxBuf[i]);	
			while(USART_GetFlagStatus(USART_485, USART_FLAG_TXE) == RESET);
		}
		#ifdef RS485
		USART_ITConfig(USART_485,USART_IT_TC,ENABLE);           // enable发送完成中断	
		#endif
		
		TxEnable_Flag = 0;       // 发送完毕，关闭发送使能
	}
	
	UsartPrintf(USART_DEBUG, "setting successed\r\n");
	
}

/********************************************************************************
*  Write_MultiHolding(void)
*  函数功能: 写多个保持寄存器
*********************************************************************************/
void Write_MultiHolding(void)	   // 写多个保持寄存器
{
	
}



/*$PAGE*/
/*
*********************************************************************************************************
*                                              Rx Handler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/
void  RxHandler (uint8_t rx_data)
{
	switch(RxState){
		case RX_STATE_ID:
			if(rx_data == Device[askTimes][0]){          
				RxTimeCnt = 1;                        // set rec timer
				RxState = RX_STATE_LEN;	
				RxBufClr();
				RxStoINT8U(rx_data);     // 
			}
			break;

		case RX_STATE_LEN:
			if(rx_data == 0x03 || rx_data == 0x04){
				RxState = RX_STATE_DATA;
				RxStoINT8U(rx_data);
				//RxRemainLen = (((RxBuf[4] << 8) + RxBuf[5]) << 1) + 2;     // *2 + 1		  // 寄存器數量
				RxRemainLen = 29;
			}else if(rx_data == 0x10){
				RxState = RX_STATE_DATA1;
				RxStoINT8U(rx_data);
				RxRemainLen = 5;	 // 先接收5个
			}
			else if(rx_data == 0x06)      // 参数设置，命令
			{
				RxState = RX_STATE_DATA;
				RxStoINT8U(rx_data);
				RxRemainLen = 6;	 // 接收6个
			}
			else{
				RxState = RX_STATE_ID;	 // can't handle the CMD
			}
			break;

		case RX_STATE_DATA:
				RxStoINT8U(rx_data);
				if (--RxRemainLen == 0) {
					pkt_Flag = 1;			 // 有数据包需要处理,在下一个 uctsk_CYCSend中解析数据			
                 	RxState = RX_STATE_ID;	 // rec end
            }
			break;

		case RX_STATE_DATA1:
				RxStoINT8U(rx_data);
				if (--RxRemainLen == 0) {
					RxRemainLen = rx_data + 2;   // 剩余寄存器数量和两个校验位
					MulLen = rx_data + 7;		 	
                 	RxState = RX_STATE_DATA;;	 // end
				}	
			break;	
	}
}

//-----------------------------------------------------------------------------------------
// copy rec char
void cpytex(uint8_t *dst, const uint8_t *src)
{
	uint8_t i;
	
	for(i=0;i<8;i++){
			dst[i] = src[i];
		}
}


/*
*********************************************************************************************************
*                                Rx/Tx Communication handler for uC/OS-View
*
* Description: This function is  called in stm32f10x_it.c 
*********************************************************************************************************
*/ 																				 
void  RxISRHandler (void)
{
    if(USART_GetITStatus(USART_485, USART_IT_RXNE) != RESET){
                      /* Read one byte from the receive data register         */
        RxHandler( USART_ReceiveData(USART_485) & 0xFF );       // call RxHandler
		//USART_SendData(USART1,USART_ReceiveData(USART1) & 0xFF );		       // for debug test

        USART_ClearITPendingBit(USART_485, USART_IT_RXNE);          /* Clear the USART Receive interrupt                   */
    }
}

void TxISRHandler(void)
{
	if(USART_GetITStatus(USART_485, USART_IT_TC) != RESET) 
		{
			#ifdef RS485
			UARTRECEIVE;        // set the 485 Received;
			#endif
		  
			//USART_SendData(USART1,USART_ReceiveData(USART1) & 0xFF );		       // for debug test
			USART_ClearITPendingBit(USART_485, USART_IT_TC);          /* Clear the USART TC interrupt                   */
			USART_ITConfig(USART_485,USART_IT_TC,DISABLE);            // 关闭发送完成中断
    }
}


/*
*********************************************************************************************************
*                                           CLEAR Rx BUFFER
*********************************************************************************************************
*/
static  void  RxBufClr (void)
{
    RxBufCnt = 0;
    RxRdIx   = 0;
}

/*
*********************************************************************************************************
*                                        STORE DATA IN Rx BUFFER
*
* Description: This routine is called whenever a valid byte has been received.
*********************************************************************************************************
*/

static  void  RxStoINT8U (uint8_t rx_data)
{
    if (RxBufCnt < RX_BUF_SIZE) {
        RxBuf[RxBufCnt++] = rx_data;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       Disable Rx Interrupts
*********************************************************************************************************
*/

void  RxIntDis (void)
{
    USART_ITConfig(USART_485, USART_IT_RXNE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Rx Interrupts
*********************************************************************************************************
*/

void  RxIntEn (void)
{
    USART_ITConfig(USART_485, USART_IT_RXNE, ENABLE);
}

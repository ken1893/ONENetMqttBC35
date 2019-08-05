/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	usart.c
	*	日期： 		2016-11-23
	*	版本： 		V1.0
	*	说明： 		单片机串口外设初始化，格式化打印
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "com.h"

//C库
#include <stdarg.h>
#include <string.h>

// 485 DE Initial
void DE_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* 使能GPIOE时钟 */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);	  // PE4 DE

  //  LED1 -> PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			            // 输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------
//	函数名称：	Usart1_Init
//	函数功能：	串口1初始化
//	入口参数：	baud：设定的波特率
//	返回参数：	无
//	说明：		TX-PA9		RX-PA10
//-------------------------------------------------------------------------
void Usart1_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //PA9	TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_9;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    //PA10	RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = baud;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// 无硬件流控
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						        // 接收和发送
    usart_initstruct.USART_Parity = USART_Parity_No;									//无校验
    usart_initstruct.USART_StopBits = USART_StopBits_1;								//1位停止位
    usart_initstruct.USART_WordLength = USART_WordLength_8b;					//8位数据位
    USART_Init(USART1, &usart_initstruct);

    USART_Cmd(USART_DEBUG, ENABLE);												// 使能串口

    USART_ITConfig(USART_DEBUG, USART_IT_RXNE, ENABLE);		// 使能接收中断

    nvic_initstruct.NVIC_IRQChannel = USART1_IRQn;        // DEBUG OR DISPLAY
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	函数名称：	Usart2_Init  
//	函数功能：	串口2初始化
//	入口参数：	baud：设定的波特率
//	返回参数：	无
//	说明：		TX-PA2		RX-PA3
//-------------------------------------------------------------------------
void Usart2_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //PA2	TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    //PA3	RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = baud;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			// 接收和发送
    usart_initstruct.USART_Parity = USART_Parity_No;									// 无校验
    usart_initstruct.USART_StopBits = USART_StopBits_1;								// 1位停止位
    usart_initstruct.USART_WordLength = USART_WordLength_8b;					// 8位数据位
    USART_Init(USART2, &usart_initstruct);
		
    USART_Cmd(USART_485, ENABLE);												  // 使能串口

    USART_ITConfig(USART_485,USART_IT_RXNE, ENABLE);			// 使能接收中断

    nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;        // 485
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	函数名称：	Usart2_Init
//	函数功能：	串口2初始化
//	入口参数：	baud：设定的波特率
//	返回参数：	无
//	说    明：	TX-PA2		RX-PA3
//-------------------------------------------------------------------------
void Usart3_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpio_initstruct;
    USART_InitTypeDef usart_initstruct;
    NVIC_InitTypeDef nvic_initstruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //PB10	TXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_initstruct);

    //PB11	RXD
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_initstruct);

    usart_initstruct.USART_BaudRate = baud;
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// 无硬件流控
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						        // 接收和发送
    usart_initstruct.USART_Parity = USART_Parity_No;									              // 无校验
    usart_initstruct.USART_StopBits = USART_StopBits_1;								              // 1位停止位
    usart_initstruct.USART_WordLength = USART_WordLength_8b;							          // 8位数据位
    USART_Init(USART3, &usart_initstruct);

    USART_Cmd(USART3, ENABLE);														//使能串口

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);				//使能接收中断

    nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;        // bc35
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	函数名称：	Usart_SendString
//	函数功能：	串口数据发送
//	入口参数：	USARTx：串口组
//				str：要发送的数据
//				len：数据长度
//	返回参数：	无
//	说明：		
//-------------------------------------------------------------------------
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//发送数据
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }
}

//-------------------------------------------------------------------------
//	函数名称：	UsartPrintf
//	函数功能：	格式化打印
//	入口参数：	USARTx：串口组
//				fmt：不定长参
//	返回参数：	无
//	说明：		
//-------------------------------------------------------------------------
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

//-------------------------------------------------------------------------
//	函数名称：	USART1_IRQHandler
//	函数功能：	DEBUG串口1收发中断
//	入口参数：	无
//	返回参数：	无
//	说明：		
//-------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
    {
                      /* Read one byte from the receive data register         */
			   
        U1RxISRHandler(USART_ReceiveData(USART1) & 0xFF );       // call RxHandler
			  //USART_ClearITPendingBit(USART_485, USART_IT_RXNE);          /* Clear the USART Receive interrupt                   */
			
        //USART_ClearFlag(USART1, USART_FLAG_RXNE);
			  USART_ClearITPendingBit(USART1, USART_IT_RXNE);          //Clear the USART Receive interrupt 
    }
}

//-------------------------------------------------------------------------
//	函数名称：	USART3_IRQHandler
//	函数功能：	串口3收发中断
//	入口参数：	无
//	返回参数：	无
//	说明：		  原处理UART3，接收gps处理字符串
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
//	函数名称：	USART2_IRQHandler
//	函数功能：	485串口2收发中断
//	入口参数：	无
//	返回参数：	无
//	说明：
//-------------------------------------------------------------------------
void USART2_IRQHandler(void)   
{
  RxISRHandler();
	TxISRHandler();
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	#ifdef RS485
		UARTSEND;
	#endif
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART_485, (uint8_t)ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART_485, USART_FLAG_TC) == RESET)
  {}
		
	#ifdef RS485
		USART_ITConfig(USART_485,USART_IT_TC,ENABLE);           // 使能发送完成中断
		UARTRECEIVE;
	#endif

  return ch;
}




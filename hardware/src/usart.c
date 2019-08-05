/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	usart.c
	*	���ڣ� 		2016-11-23
	*	�汾�� 		V1.0
	*	˵���� 		��Ƭ�����������ʼ������ʽ����ӡ
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "com.h"

//C��
#include <stdarg.h>
#include <string.h>

// 485 DE Initial
void DE_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* ʹ��GPIOEʱ�� */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);	  // PE4 DE

  //  LED1 -> PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			            // ���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//-------------------------------------------------------------------------
//	�������ƣ�	Usart1_Init
//	�������ܣ�	����1��ʼ��
//	��ڲ�����	baud���趨�Ĳ�����
//	���ز�����	��
//	˵����		TX-PA9		RX-PA10
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
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// ��Ӳ������
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						        // ���պͷ���
    usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
    usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
    usart_initstruct.USART_WordLength = USART_WordLength_8b;					//8λ����λ
    USART_Init(USART1, &usart_initstruct);

    USART_Cmd(USART_DEBUG, ENABLE);												// ʹ�ܴ���

    USART_ITConfig(USART_DEBUG, USART_IT_RXNE, ENABLE);		// ʹ�ܽ����ж�

    nvic_initstruct.NVIC_IRQChannel = USART1_IRQn;        // DEBUG OR DISPLAY
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	�������ƣ�	Usart2_Init  
//	�������ܣ�	����2��ʼ��
//	��ڲ�����	baud���趨�Ĳ�����
//	���ز�����	��
//	˵����		TX-PA2		RX-PA3
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
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			// ���պͷ���
    usart_initstruct.USART_Parity = USART_Parity_No;									// ��У��
    usart_initstruct.USART_StopBits = USART_StopBits_1;								// 1λֹͣλ
    usart_initstruct.USART_WordLength = USART_WordLength_8b;					// 8λ����λ
    USART_Init(USART2, &usart_initstruct);
		
    USART_Cmd(USART_485, ENABLE);												  // ʹ�ܴ���

    USART_ITConfig(USART_485,USART_IT_RXNE, ENABLE);			// ʹ�ܽ����ж�

    nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;        // 485
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	�������ƣ�	Usart2_Init
//	�������ܣ�	����2��ʼ��
//	��ڲ�����	baud���趨�Ĳ�����
//	���ز�����	��
//	˵    ����	TX-PA2		RX-PA3
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
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// ��Ӳ������
    usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						        // ���պͷ���
    usart_initstruct.USART_Parity = USART_Parity_No;									              // ��У��
    usart_initstruct.USART_StopBits = USART_StopBits_1;								              // 1λֹͣλ
    usart_initstruct.USART_WordLength = USART_WordLength_8b;							          // 8λ����λ
    USART_Init(USART3, &usart_initstruct);

    USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);				//ʹ�ܽ����ж�

    nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;        // bc35
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_initstruct);
}

//-------------------------------------------------------------------------
//	�������ƣ�	Usart_SendString
//	�������ܣ�	�������ݷ���
//	��ڲ�����	USARTx��������
//				str��Ҫ���͵�����
//				len�����ݳ���
//	���ز�����	��
//	˵����		
//-------------------------------------------------------------------------
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;

    for(; count < len; count++)
    {
        USART_SendData(USARTx, *str++);									//��������
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
    }
}

//-------------------------------------------------------------------------
//	�������ƣ�	UsartPrintf
//	�������ܣ�	��ʽ����ӡ
//	��ڲ�����	USARTx��������
//				fmt����������
//	���ز�����	��
//	˵����		
//-------------------------------------------------------------------------
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;

    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
    va_end(ap);

    while(*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

//-------------------------------------------------------------------------
//	�������ƣ�	USART1_IRQHandler
//	�������ܣ�	DEBUG����1�շ��ж�
//	��ڲ�����	��
//	���ز�����	��
//	˵����		
//-------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
    {
                      /* Read one byte from the receive data register         */
			   
        U1RxISRHandler(USART_ReceiveData(USART1) & 0xFF );       // call RxHandler
			  //USART_ClearITPendingBit(USART_485, USART_IT_RXNE);          /* Clear the USART Receive interrupt                   */
			
        //USART_ClearFlag(USART1, USART_FLAG_RXNE);
			  USART_ClearITPendingBit(USART1, USART_IT_RXNE);          //Clear the USART Receive interrupt 
    }
}

//-------------------------------------------------------------------------
//	�������ƣ�	USART3_IRQHandler
//	�������ܣ�	����3�շ��ж�
//	��ڲ�����	��
//	���ز�����	��
//	˵����		  ԭ����UART3������gps�����ַ���
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
//	�������ƣ�	USART2_IRQHandler
//	�������ܣ�	485����2�շ��ж�
//	��ڲ�����	��
//	���ز�����	��
//	˵����
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
		USART_ITConfig(USART_485,USART_IT_TC,ENABLE);           // ʹ�ܷ�������ж�
		UARTRECEIVE;
	#endif

  return ch;
}




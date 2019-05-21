#ifndef _USART_H_
#define _USART_H_

#include "stm32f10x.h"
#include <stdio.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
#define RS485     // uart口为RS485
	
#define DE_OFF                       GPIOE->BSRR = 0x10
#define DE_ON                        GPIOE->BRR  = 0x10
	
#define UARTSEND 	   DE_OFF
#define UARTRECEIVE  DE_ON

#define USART_DEBUG		USART1		// 调试打印所使用的串口组
#define USART_BC35    USART3
#define USART_485		  USART2
	
void DE_Init(void);      // 485 DE Init

void Usart1_Init(unsigned int baud);      // for串口debug
void Usart3_Init(unsigned int baud);      // for BC35 com
void Usart2_Init(unsigned int baud);      // for 485/GPS

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
	



#endif

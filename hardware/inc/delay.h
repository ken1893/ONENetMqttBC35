#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"

void delay_tms(__IO uint32_t nTime);
void SysTick_init(void);

void DelayUs(unsigned short us);

#endif

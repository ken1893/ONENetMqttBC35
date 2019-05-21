/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	delay.c
	*
	*	˵���� 		����systick������ʽ��ʱ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#define DELAY_GLOBALS

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"
#include "config.h"

//delayͷ�ļ�
#include "delay.h"


//-------------------------------------------------------------------------
// * @brief initializes delay.
// * @param wSystemCoreClock comes from system core clock.
// * @retval None
//-------------------------------------------------------------------------
void SysTick_init(void)
{
	// The SysTick calibration value is set to 6000, which gives a reference time base of 1 ms with the SysTick clock set to 6 MHz (max f HCLK /8)
	if(SysTick_Config(SystemCoreClock / 100))   // 1000 1ms    100 10ms
  { 
    /* Capture error */ 
    while (1);
  }
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void delay_tms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}



void TaskTimer(void)
{
	if(TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
	
	TaskSystic++;
	Task1Flag = 1;
	
	switch(TaskSystic)
	{
		case 10:
			Task2Flag = 1;
			break;
		
		case 20:
			Task2Flag = 1;
			break;
		
		case 30:
			Task2Flag = 1;
			break;
		
		case 40:
			Task2Flag = 1;
			break;
		
		case 50:
			Task2Flag = 1;
			break;
		
		case 60:
			Task2Flag = 1;
			break;
		
		case 70:
			Task2Flag = 1;
			break;
		
		case 80:
			Task2Flag = 1;
			break;
		
		case 90:
			Task2Flag = 1;
			break;
		
		case 100:
			Task3Flag = 1;
			TaskSystic = 0;
			break;
	}
}


//-------------------------------------------------------------------------
//	�������ƣ�	DelayUs
//	�������ܣ�	΢�뼶��ʱ
//	��ڲ�����	us����ʱ��ʱ��
//	���ز�����	��
//	˵����		��ʱ��(21MHz)�����ʱ798915us
//-------------------------------------------------------------------------
void DelayUs(unsigned short us)
{
	uint32_t i; 
	for(i=0;i<us;i++) 
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();
		//__NOP();__NOP();__NOP();__NOP();__NOP();
		//__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}



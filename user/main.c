/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
        *	作者： 		
        *	日期： 		2018-05-08
	*	版本： 		V1.0
	*	说明： 		接入onenet，上传数据和命令控制
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#define MAIN_GLOBALS

//单片机头文件
#include "stm32f10x.h"
#include "config.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "BC35.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

																		
// State Machine function prototypes
//------------------------------------
// Alpha states
void A0(void);	//state A0
void B0(void);	//state B0
void C0(void);	//state C0

// A branch states  10ms 
void A1(void);	//state A1   deal onenet REC control
void A2(void);	//state A2   onenet reconnect
void A3(void);	//state A3   

// B branch states   100ms
void B1(void);	//state B1   (receive)SERIALS COMmunication
void B2(void);	//state B2
void B3(void);	//state B3

// C branch states
void C1(void);	//state C1    timing onenet send
void C2(void);	//state C2    485 ask the terminal
void C3(void);	//state C3    strategy rules

// Variable declarations
void (*Alpha_State_Ptr)(void);     // Base States pointer
void (*A_Task_Ptr)(void);     // State pointer A branch    10ms
void (*B_Task_Ptr)(void);     // State pointer B branch    100ms
void (*C_Task_Ptr)(void);     // State pointer C branch

void LED_GPIO_Configuration(void);
static void WDG_Configuration(void);

unsigned char *dataPtr = NULL;
unsigned short timeCount = 0;	 // 发送间隔变量
unsigned char  StatusOne = 0;  

uint16_t led_counter = 0;    // for leddisplay

//----------------------------------------------------------------------------
//	函数名称：	Hardware_Init
//	函数功能：	硬件初始化
//	入口参数：	无
//	返回参数：	无
//	说    明：  初始化单片机功能以及外接设备
//----------------------------------------------------------------------------
void Hardware_Init(void)
{
	  LED_GPIO_Configuration();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	    // 中断控制器分组设置
	  WDG_Configuration();

	  SysTick_init();           // systick初始化

    Usart1_Init(115200);      // 串口1，打印信息用
	
    Usart3_Init(9600);        // 串口2，驱动BC35用，网络模块驱动
    Usart2_Init(9600);        // 串口2，485主模式，设备通讯
	  DE_Init();                // 485 DE
}

/*******************************************************************************
* Function Name  : LED_GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void LED_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);	   // PE13 LED

  //  LED1 -> PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			     // Êä³ö
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}

static void WDG_Configuration(void)
{
  /* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency
     dispersion) -------------------------------------------------------------*/
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_256);

  /* Set counter reload value to 349 */
  IWDG_SetReload(349);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  //IWDG_Enable();
}

void Para_Init(void)
{
	Task1Flag = 0;
	Task2Flag = 0;
	Task3Flag = 0;
	
	dog_flag = 0;   
  NetStatus = NONET; 
	
	Device[0][0] = 1;   // Device 0 ID 
}

//-------------------------------------------------------------------------------
//	函数名称：	main
//	函数功能：	
//	入口参数：	无
//	返回参数：	0
//	说明：		
//-------------------------------------------------------------------------------
int main(void)
{
    Hardware_Init();				       // 初始化外围硬件
	  Para_Init();                   // 初始化变量
	
	  OPERATING_MODE = NORMAL;       // 正常工作模式，带显示
	
	  LEDON;

    BC35_Init();				           // 初始化bc35
	
	  BC35_Sockets();                // TCP
	
    while(OneNet_DevLink())			   // 接入OneNET
			delay_tms(50);

		LEDOFF;

		// Tasks State-machine init
	  Alpha_State_Ptr = &A0; 
	  A_Task_Ptr = &A1;
	  B_Task_Ptr = &B1;
	  C_Task_Ptr = &C1;
		
		/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    //IWDG_Enable();

    while(1)
    {
			// State machine entry & exit point
			(*Alpha_State_Ptr)();     // jump to an Plpha State (A0,B0,...)
    }
}


//--------------------------------FRAME WORK---------------------------------
void A0(void)
{
	// loop rate synchronizer for A-tasks
	if(Task1Flag == 1)
	{
		Task1Flag = 0;     // clear Tasktimer flag
		
		//-----------------------------------------------------------
		(*A_Task_Ptr)();		// jump to an A Task (A1,A2,A3,...)
		//-----------------------------------------------------------
	}
	Alpha_State_Ptr = &B0;
}


void B0(void)
{
	// loop rate synchronizer for B-tasks
	if(Task2Flag == 1)
	{
		Task2Flag = 0;     // clear Tasktimer flag
		
		//-----------------------------------------------------------
		(*B_Task_Ptr)();		// jump to a B Task (B1,B2,B3,...)
		//-----------------------------------------------------------
	}
	Alpha_State_Ptr = &C0;   // Allow C state tasks
}

void C0(void)
{
	// loop rate synchronizer for C-tasks
	if(Task3Flag == 1)
	{
		Task3Flag = 0;				// clear flag

		//-----------------------------------------------------------
		(*C_Task_Ptr)();		// jump to a C Task (C1,C2,C3,...)
		//-----------------------------------------------------------
	}

	Alpha_State_Ptr = &A0;	// Back to State A0
}


//=================================================================================
//	A - TASKS (executed in every 10 msec)
//=================================================================================
//--------------------------------------------------------
void A1(void) // SPARE (not used)  deal onenet REC
//--------------------------------------------------------
{
  dataPtr = BC35_GetIPD(10);
  if(dataPtr != NULL)
		OneNet_RevPro(dataPtr);
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//-----------------------------------------------------------------
void A2(void) // SPARE (onenet reconnect) 
//-----------------------------------------------------------------
{	
	//
	if(dog_flag == 0)     // if everything is OK 
	{
		// IWDG_ReloadCounter();     // wed dog
	}
	
	if(NetStatus == ONENETOFF)  // 接入OneNET
	{		        
		  LEDOFF;
			BC35_Sockets();         // TCP
			delay_tms(20);          
			OneNet_DevLink();       
	}
	else 
	{
		SetLED(&led_counter);  // led display
	}
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A3
	A_Task_Ptr = &A3;
	//-------------------
}

//-----------------------------------------
void A3(void) // SPARE (not used)
//-----------------------------------------
{

	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}


//=================================================================================
//	B - TASKS (executed in every 100 msec)
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void) // SERIALS COMmunication
//----------------------------------------
{
	// deal the uart com
	// UART 485
	if(RxTimeCnt >= 1){    // UART 485
			RxTimeCnt++;
			if(RxTimeCnt >= 3){	  // 判断超时
				RxTimeCnt = 0;
				RxState = 0;	
				
				#ifdef RS485
				//UARTRECEIVE;
				#endif
			}
		}
	
		//  UART1 
		if(Rx1TimeCnt >= 1){    // 
			Rx1TimeCnt++;
			if(Rx1TimeCnt >= 3){	  // 判断超时
				Rx1TimeCnt = 0;
				Rx1State = PROTOCOL_WAITING;	

			}
		}
	
	if(pkt_Flag == 1){    // 有数据包需要处理
			UartDeal(askTimes);       // deal 485
			pkt_Flag = 0;     // 包处理完毕
		}
	
	if(pk1t_Flag == 1)
	{
		switch(Rx1Buf[3])    // dwin address 
		{
			case 0x22:
				//UsartPrintf(USART_DEBUG, "OPEN\r\n");
			  SW_Con(0,SW_ON);
				break;
			
			case 0x20:
				//UsartPrintf(USART_DEBUG, "CLOSE\r\n");
			  SW_Con(0,SW_OFF);
				break;
			
			default:break;
		}
		pk1t_Flag = 0;     // 包处理完毕
	}

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;	
	//-----------------
}

//----------------------------------------
void B2(void) //  SPARE
//----------------------------------------
{
	// deal with the Switch

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void) //  SPARE
//----------------------------------------
{
	//
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;	
	//-----------------
}


//=================================================================================
//	C - TASKS (executed in every 1000 msec)
//=================================================================================

//--------------------------------- USER ------------------------------------------

//----------------------------------------
void C1(void) 	// timing onenet send
//----------------------------------------
{
	//  循环发送字段  3second a time 
  if(++timeCount >= 30)		// 发送间隔 120s
  {
		timeCount = 0;
					  
    OneNet_SendData();		// OneNet_SendData 发送数据	
    BC35_Clear();
  }
	else if(timeCount == 1 || timeCount == 7 || timeCount == 14)    // 演示设备,一分钟获取3次数据
	{
		
	}
	
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C2
	C_Task_Ptr = &C2;	
	//-----------------
}

//----------------------------------------
void C2(void) //  SPARE  485 ask the terminal
	//strategy rules
//----------------------------------------
{
	Ask_pros(0);       // ask for terminal
	
	if(OPERATING_MODE == NORMAL)    // 带屏幕显示，主动发送数据
	{
		//Send_diwi(uint16_t val,uint16_t add);
		
		Send_diwi4(SinglePhase[0].RegS.EnegerH,SinglePhase[0].RegS.EnegerL,ADD_ENERGY);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.Temp,ADD_TEMP);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.Power,ADD_POWER);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.Volt,ADD_VOLT);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.Current,ADD_CURRENT);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.Leak,ADD_LEAK);
		delay_tms(1);
		Send_diwi(SinglePhase[0].RegS.PF,ADD_PF);
		delay_tms(1);
	}
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C3;	
	//-----------------
}


//-----------------------------------------
void C3(void) //  SPARE
//-----------------------------------------
{
	
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------
}

//-----------------------------------------
//-----------------------------------------
//-----------------------------------------
uint8_t SetLED(uint16_t *ls)
{
	switch(*ls)
	{
		case L1ON:
			LEDON;
			break;
		
		case L2OFF:
			LEDOFF;
			break;
		
		case L3ON:
			LEDON;
			break;
		
		case L4OFF:
			LEDOFF;
			break;
		
		case LEND:
			*ls = 0;
		break;
		
		default:break;
	}
	
	*ls = *ls + 1;
	
	return 0;
}


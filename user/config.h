
#ifndef __CONFIG_H
#define __CONFIG_H

//#define RS485

#include    "string.h"
#include    "stm32f10x.h"
#include    "com.h" 
#include    "stdlib.h"

#define enable_irq()   __enable_irq();
#define disable_irq()  __disable_irq();

#define ActionReg    0x00      
#define RESTARTTIMES 10 

// display for one led
#define LEDON        GPIO_ResetBits(GPIOE , GPIO_Pin_13)    
#define LEDOFF       GPIO_SetBits(GPIOE , GPIO_Pin_13)

// �豸����
enum ActionType {
    WAITING,           // ϵͳ�ȴ�
    SW_ON = 1,         // ��բ    1
	  SW_OFF,            // ��բ    2
    SW_BLOCK,          // �ű��̵ּ�������  3
	
	  STOP            // STOP                 9
};

// ����״̬
enum NETSTA {
    NONET,            // ������
    SIMON = 1,        // sim������    1
	  ONENETON,         // onenet����   2
    ONENETOFF         // onenet����   3
};


enum BOOTMODE{
	BOOTOFF,
	BOOTON = 1,         // ��⵽���µ��ն��豸����ʱĬ��Ϊ��
	BOOTDONE,
	DEBUGMODE,
	NORMAL
};

enum LEDACT{          // LED ACTION
	L1ON = 1,
	L2OFF = 3,
	L3ON = 5,
	L4OFF = 7,
	LEND = 30
};


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#ifdef MAIN_GLOBALS
#define MAIN_EXT
#else
#define MAIN_EXT extern
#endif

MAIN_EXT uint8_t Task1Flag;
MAIN_EXT uint8_t Task2Flag;
MAIN_EXT uint8_t Task3Flag;


union HOLDREG_U
{
	unsigned int RegI[2];
	struct HONDREG_S
	{
    unsigned int  Action;
		// not for write
		unsigned int  End_ID;

	}RegS;
};

union INPUTREG_U
{
	unsigned int RegI[13];  // 
	struct INPUTREG_S
	{
		unsigned int  EnegerH;		   //   0
		unsigned int  EnegerL;		   //   1
		
	  unsigned int  Temp;	 	        //  2
	  unsigned int  Power;	 	      //  3
	  unsigned int  Volt;	 	        //  4
	  unsigned int  Current;	 	    //  5
	  unsigned int  Status;         // 6
	  unsigned int  Leak;           // 7
	  unsigned int  PF;             // 8
	  unsigned int  PowerAll;       // 9
	  unsigned int  PeakC;          // 10
	  unsigned int  PeakV;          // 11
	  unsigned int  PeakP;	        // 12

	}RegS;
};

MAIN_EXT union HOLDREG_U Holding;    

MAIN_EXT union INPUTREG_U SinglePhase[12];     // �����豸,��485�����豸��������

// �����ݶ���
typedef struct
{
	uint8_t  EnegerAll;		  // �ܵ���
	uint8_t  Temp;	 	      // �¶�
	uint8_t  Power;	 	      // ����
	uint8_t  Volt;	 	      // ��ѹ
	uint8_t  Current;	 	    // ����
	uint8_t  Status;         
	uint8_t  Leak;
	uint8_t  PF;
	uint8_t  PowerAll;
	uint8_t  PeakC;
	uint8_t  PeakV;
	uint8_t  PeakP;	                 
} Meter_TypeDef;

MAIN_EXT Meter_TypeDef Meter_Data;
MAIN_EXT uint8_t Device[12][3];     // �豸���б�    ����485�豸�޶��豸��������12̨����һ�������豸ID���ڶ��������豸����,�����������豸����״̬
MAIN_EXT uint8_t DeviceCount;       // �豸���� notused

MAIN_EXT uint16_t OPERATING_MODE;    // ����ģʽ

MAIN_EXT uint16_t RecordAction;          // ��¼��һ������
MAIN_EXT uint16_t Action_Flag;           // ������־


MAIN_EXT uint8_t RECNetStatus;      // BC35 REC status
MAIN_EXT uint8_t NetStatus;         // ONENET STATUS
MAIN_EXT uint8_t ErrorTimes;        // ������������,��¼10������
MAIN_EXT uint8_t dog_flag;          // ι����־

MAIN_EXT uint8_t askTimes;          // ?? ��ѯ���У����豸���÷�ʱ��Ѷ��ʽ��ȡ�ӻ�����

MAIN_EXT uint8_t SetLED(uint16_t *ls);    // LED for status

#ifdef DELAY_GLOBALS
#define DELAY_EXT
#else
#define DELAY_EXT extern
#endif

DELAY_EXT __IO uint32_t TimingDelay;
DELAY_EXT __IO uint16_t TaskSystic;

DELAY_EXT void TaskTimer(void);

#endif


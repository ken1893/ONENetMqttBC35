
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

// 设备动作
enum ActionType {
    WAITING,           // 系统等待
    SW_ON = 1,         // 分闸    1
	  SW_OFF,            // 合闸    2
    SW_BLOCK,          // 磁保持继电器高阻  3
	
	  STOP            // STOP                 9
};

// 网络状态
enum NETSTA {
    NONET,            // 无网络
    SIMON = 1,        // sim卡上线    1
	  ONENETON,         // onenet上线   2
    ONENETOFF         // onenet离线   3
};


enum BOOTMODE{
	BOOTOFF,
	BOOTON = 1,         // 检测到有新的终端设备上线时默认为打开
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

MAIN_EXT union INPUTREG_U SinglePhase[12];     // 单相设备,单485网关设备数量控制

// 表数据定义
typedef struct
{
	uint8_t  EnegerAll;		  // 总电量
	uint8_t  Temp;	 	      // 温度
	uint8_t  Power;	 	      // 功率
	uint8_t  Volt;	 	      // 电压
	uint8_t  Current;	 	    // 电流
	uint8_t  Status;         
	uint8_t  Leak;
	uint8_t  PF;
	uint8_t  PowerAll;
	uint8_t  PeakC;
	uint8_t  PeakV;
	uint8_t  PeakP;	                 
} Meter_TypeDef;

MAIN_EXT Meter_TypeDef Meter_Data;
MAIN_EXT uint8_t Device[12][3];     // 设备序列表    单个485设备限定设备连接数量12台，第一个参数设备ID，第二个参数设备类型,第三个参数设备联网状态
MAIN_EXT uint8_t DeviceCount;       // 设备数量 notused

MAIN_EXT uint16_t OPERATING_MODE;    // 工作模式

MAIN_EXT uint16_t RecordAction;          // 记录上一个动作
MAIN_EXT uint16_t Action_Flag;           // 动作标志


MAIN_EXT uint8_t RECNetStatus;      // BC35 REC status
MAIN_EXT uint8_t NetStatus;         // ONENET STATUS
MAIN_EXT uint8_t ErrorTimes;        // 故障重启次数,记录10次重启
MAIN_EXT uint8_t dog_flag;          // 喂狗标志

MAIN_EXT uint8_t askTimes;          // ?? 问询序列，本设备采用分时闻讯方式获取从机数据

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


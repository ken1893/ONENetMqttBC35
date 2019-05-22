/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	BC35.c
	*	日期： 		2017-10-20
	*	版本： 		V1.0
	*	说明： 		BC35驱动
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "BC35.h"
//#include "oled.h"
//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "HEXSTR.h"

#include "config.h"

#define BC35_ONENET_INFO "AT+NSOCO=2,183.230.40.39,6002\r\n"  //"AT+IPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"

unsigned char BC35_buf[128];        // 接收模块的反馈信息

unsigned short BC35_cnt = 0, BC35_cntPre = 0;

//==========================================================
//	函数名称：	BC35_Clear
//	函数功能：	清空缓存
//	入口参数：	无
//	返回参数：	无
//	说明：		
//==========================================================
void BC35_Clear(void)
{
    memset(BC35_buf, 0, sizeof(BC35_buf));   // 接收模块的反馈信息clear
    BC35_cnt = 0;
}

//==========================================================
//	函数名称：	BC35_WaitRecive
//	函数功能：	等待接收完成
//	入口参数：	无
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool BC35_WaitRecive(void)
{
    if(BC35_cnt == 0) 						// 如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if(BC35_cnt == BC35_cntPre)   // 如果上一次的值和这次相同，则说明接收完毕
    {
        BC35_cnt = 0;							// 清0接收计数
        return REV_OK;						// 返回接收完成标志
    }
		
    BC35_cntPre = BC35_cnt;     //置为相同
		
    //UsartPrintf(USART_DEBUG,"Rec:BC35_cntPre,%d;BC35_cnt,%d;",BC35_cntPre,BC35_cnt); 		//for test
		
    return REV_WAIT;							// 返回接收未完成标志
}

//==========================================================
//	函数名称：	BC35_SendCmd
//	函数功能：	发送命令
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//	返回参数：	0-成功	1-失败
//	说明：		
//==========================================================
_Bool BC35_SendCmd(char *cmd, char *res)
{
    unsigned char timeOut = 252;    // 

    Usart_SendString(USART_BC35, (unsigned char *)cmd, strlen((const char *)cmd));
	
    delay_tms(20);
    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					// 如果收到数据
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);        // print usartdebug
            if(strstr((const char *)BC35_buf, res) != NULL)		// 如果检索到关键词
            {
                BC35_Clear();                                 // 清空缓存 成功反馈0
							  ErrorTimes = 0;   // 故障代码清零
                return 0;
            }
						else if(strstr((const char *)BC35_buf, "ERROR") != NULL) // check error
						{
							
							ErrorTimes++;
							UsartPrintf(USART_DEBUG,"ERROR HAPPENED %d times,NEED TO Check!!!\r\n",ErrorTimes);
							
							if(ErrorTimes >= RESTARTTIMES)
							{
								UsartPrintf(USART_DEBUG,"ERROR CRASHED,RESTART NOW!!!!!!\r\n");
								/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
                IWDG_Enable();    // restart
							}
						}
        }
        //delay_tms(1);
    }
    return 1;
}

//==========================================================
//	函数名称：	BC35_SendData
//	函数功能：	发送数据
//	入口参数：	data：数据
//			 len：长度
//	返回参数：	无
//	说明：		
//==========================================================
void BC35_SendData(unsigned char *data, unsigned short len)
{
    char HTTP_BufSendAT[500];
    memset(HTTP_BufSendAT, 0, sizeof(HTTP_BufSendAT));
    sprintf((char*)(HTTP_BufSendAT),"%s,%d,%s\r\n","AT+NSOSD=2",len,(char *)data);
    UsartPrintf(USART_DEBUG,(char *)HTTP_BufSendAT);
    if(!BC35_SendCmd(HTTP_BufSendAT, "OK"))				//收到‘>’时可以发送数据
    {
        delay_tms(50);
        Usart_SendString(USART_BC35,"AT+NSORF=2,100\r\n", 16);	//发送设备连接请求数据
        if(BC35_WaitRecive() == REV_OK)							//如果收到数据
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
        }
        delay_tms(50);
        //		UsartPrintf(USART_DEBUG,(char *)HTTP_BufSend);
        UsartPrintf(USART_DEBUG,"Send OK\r\n");
        //		BC35_SendCmd("AT+QIRD=0,1500\r\n","+QIRD:");
    }
}

//==========================================================
//	函数名称：	BC35_GetIPD
//	函数功能：	获取平台返回的数据
//	入口参数：	timeOut等待的时间(乘以10ms)
//	返回参数：	平台返回的原始数据
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *BC35_GetIPD(unsigned short timeOut)
{
    char *ptrIPD;
	  char *RecNum;
  	int num;
	
	  char cmdbuf[20];
    memset(cmdbuf, 0, sizeof(cmdbuf));
		
	  //strcat(buf, text);
	
    do
    {
        if(BC35_WaitRecive() == REV_OK)						// 如果接收完成
        {
					UsartPrintf(USART_DEBUG,"receive signal is:\r\n");
          UsartPrintf(USART_DEBUG,(char *)BC35_buf);
					
					if(strstr((char *)BC35_buf, ",6002,") != NULL)
					{
						NetStatus = 0;      // receive data
						ptrIPD = strstr((char *)BC35_buf, ",6002,");
					}
					else if(strstr((char *)BC35_buf, "+NSONMI:") != NULL)
					{
						NetStatus = 1;      // receive data flag
						ptrIPD = strstr((char *)BC35_buf, "+NSONMI:");
					}
					else 
					{
						UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
						NetStatus = 0xff;				  // 处理完成
						return NULL;
					}
					  
					switch(NetStatus)
				  {
							case 0:     // 等待连接
								  SetLED(1);

									UsartPrintf(USART_DEBUG, "收到服务器数据\r\n");
							
									UsartPrintf(USART_DEBUG,(char *)BC35_buf);		
                  UsartPrintf(USART_DEBUG, "接收完成\r\n");		
									
							    return (unsigned char *)(ptrIPD);
                  NetStatus = 0xff;				  // 处理完成			
								break;
							
									// +NSONMI:2,47
							case 1:      // 接收
								  
							    SetLED(0);
					
									RecNum = ptrIPD + 10;
									num = atoi(RecNum);
									UsartPrintf(USART_DEBUG, "RecNUM:%s;intnum:%d\r\n",RecNum,num);
										
									sprintf(cmdbuf, "AT+NSORF=2,%d\r\n",num);
										
									Usart_SendString(USART_BC35,(unsigned char *)cmdbuf, 15);	//发送设备连接请求数据
										
									if(BC35_WaitRecive() == REV_OK)							//如果收到数据
									{
											UsartPrintf(USART_DEBUG,(char *)BC35_buf);
									}
									delay_tms(50);
										
									UsartPrintf(USART_DEBUG,"Receive data ready!\r\n");
									NetStatus = 0xff;				  // 处理完成
								break;
							
							default:
								break;
						}
        }
        delay_tms(1);												// 延时等待
    }while(timeOut--);
		
    return NULL;														// 超时还未找到，返回空指针
}

//==========================================================
//	函数名称：	BC35_Init
//	函数功能：	初始化BC35
//	入口参数：	无
//	返回参数：	无
//	说明：		
//==========================================================
void BC35_Init(void)
{
		//                1
    BC35_Clear();                          
    //delay_tms(100);
	  
	  //                2
    UsartPrintf(USART_DEBUG, "1.ATE0\r\n");     // 关闭回显
    while(BC35_SendCmd("ATE0\r\n", "OK"))
        delay_tms(50);
    
		//                3                         // 设置移动终端错误报告
    while(BC35_SendCmd("AT+CMEE=1\r\n", "OK"))  // 保SIM卡PIN码解锁，返回READY，表示解锁成功
        delay_tms(50);
    
		//                4                         // 测试指令，返回ok
    UsartPrintf(USART_DEBUG, "2.AT\r\n");
    while(BC35_SendCmd("AT\r\n", "OK"))	        // 看下卡状态是否OK 能否获取到卡号
        delay_tms(50);
    
		//                5                         // 查询模块序列号
    UsartPrintf(USART_DEBUG, "3.AT+CGSN=1\r\n");// 获取IMEI
    while(BC35_SendCmd("AT+CGSN=1\r\n", "OK"))
        delay_tms(50);
    
		//                9                   
    BC35_Clear();
		//                                            查询
    if(!BC35_SendCmd("AT+NBAND?\r\n","+NBAND:5"))// 判断是否为+NBAND:5
    { 
			UsartPrintf(USART_DEBUG, "BAND:5 REG[..]\r\n");  // 是否是电信的
    }
    else
    { 
			UsartPrintf(USART_DEBUG, "BAND:8 REG[..]\r\n");  // 否则就是移动联通
    }

    delay_tms(50);
    
		//                 10     查询SIM的IMSI值
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "4.AT+CIMI\r\n");
    if(!BC35_SendCmd("AT+CIMI\r\n","OK"))
    {
			UsartPrintf(USART_DEBUG, "NBSIMCARD [OK]\r\n");
    }
    else
    {
			UsartPrintf(USART_DEBUG, "NBSIMCARD [NO]\r\n");
    }
    delay_tms(50);
    
		//                 11                            查询信号强度
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "5.AT+CSQ\r\n");
    while(!BC35_SendCmd("AT+CSQ\r\n","CSQ:99,99"))             // 激活
        delay_tms(50);

    //	UsartPrintf(USART_DEBUG, "8. AT+QIACT?\r\n");
    //	while(BC35_SendCmd("AT+QIACT?\r\n","OK"))
    //		DelayXms(500);
		
		//                 12                            设置网络激活操作
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "6.AT+CGATT=1\r\n");
    BC35_SendCmd("AT+CGATT=1\r\n","OK");                   //激活状态
    delay_tms(50);
    
		//                 13
    UsartPrintf(USART_DEBUG, "7.AT+CGATT?\r\n");     // 查询网络激活状态
    while(BC35_SendCmd("AT+CGATT?\r\n","CGATT:1"))         //激活状态
    {
        UsartPrintf(USART_DEBUG, "Regedit PDP[..]\r\n");
        delay_tms(50);
    }
    
		//                 24
    UsartPrintf(USART_DEBUG, "8.AT+CGPADDR\r\n");     // 查询模块获取的IP
    BC35_SendCmd("AT+CGPADDR\r\n","OK");              // 激活状态
    delay_tms(50);

    UsartPrintf(USART_DEBUG, "9.AT+NSOCR=STREAM,6,35001,1\r\n");        //  TCP
    while(BC35_SendCmd("AT+NSOCR=STREAM,6,35001,1\r\n","OK"))           //必须为单连接，不然平台IP都连不上
    {
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        delay_tms(100);
    }

    UsartPrintf(USART_DEBUG, "-----------------\r\n");
    UsartPrintf(USART_DEBUG, BC35_ONENET_INFO);
    while(BC35_SendCmd(BC35_ONENET_INFO,"OK"))				// 连接到服务器
    {
        UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        delay_tms(50);
        UsartPrintf(USART_DEBUG, "AT+NSOCR=STREAM,6,35001,1\r\n");
        while(BC35_SendCmd("AT+NSOCR=STREAM,6,35001,1\r\n","OK"))	// 必须为单连接，不然平台IP都连不上
        {
            UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
            BC35_SendCmd("AT+NSOCL=1\r\n","OK");
            delay_tms(50);
        }
        delay_tms(50);
    }
    UsartPrintf(USART_DEBUG, "=====BC35 Init OK=====\r\n");
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//	函数功能：	串口2收发中断
//	入口参数：	无
//	返回参数：	无
//	说明:
//==========================================================
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART_BC35, USART_IT_RXNE) != RESET) // 接收中断
    {
        if(BC35_cnt >= sizeof(BC35_buf))	BC35_cnt = 0;   // 防止串口被刷爆
        BC35_buf[BC35_cnt++] = USART_BC35->DR;

        USART_ClearFlag(USART_BC35, USART_FLAG_RXNE);
    }
}

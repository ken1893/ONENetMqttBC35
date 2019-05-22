/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	BC35.c
	*	���ڣ� 		2017-10-20
	*	�汾�� 		V1.0
	*	˵���� 		BC35����
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "BC35.h"
//#include "oled.h"
//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "HEXSTR.h"

#include "config.h"

#define BC35_ONENET_INFO "AT+NSOCO=2,183.230.40.39,6002\r\n"  //"AT+IPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"

unsigned char BC35_buf[128];        // ����ģ��ķ�����Ϣ

unsigned short BC35_cnt = 0, BC35_cntPre = 0;

//==========================================================
//	�������ƣ�	BC35_Clear
//	�������ܣ�	��ջ���
//	��ڲ�����	��
//	���ز�����	��
//	˵����		
//==========================================================
void BC35_Clear(void)
{
    memset(BC35_buf, 0, sizeof(BC35_buf));   // ����ģ��ķ�����Ϣclear
    BC35_cnt = 0;
}

//==========================================================
//	�������ƣ�	BC35_WaitRecive
//	�������ܣ�	�ȴ��������
//	��ڲ�����	��
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool BC35_WaitRecive(void)
{
    if(BC35_cnt == 0) 						// ������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
        return REV_WAIT;

    if(BC35_cnt == BC35_cntPre)   // �����һ�ε�ֵ�������ͬ����˵���������
    {
        BC35_cnt = 0;							// ��0���ռ���
        return REV_OK;						// ���ؽ�����ɱ�־
    }
		
    BC35_cntPre = BC35_cnt;     //��Ϊ��ͬ
		
    //UsartPrintf(USART_DEBUG,"Rec:BC35_cntPre,%d;BC35_cnt,%d;",BC35_cntPre,BC35_cnt); 		//for test
		
    return REV_WAIT;							// ���ؽ���δ��ɱ�־
}

//==========================================================
//	�������ƣ�	BC35_SendCmd
//	�������ܣ�	��������
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//	���ز�����	0-�ɹ�	1-ʧ��
//	˵����		
//==========================================================
_Bool BC35_SendCmd(char *cmd, char *res)
{
    unsigned char timeOut = 252;    // 

    Usart_SendString(USART_BC35, (unsigned char *)cmd, strlen((const char *)cmd));
	
    delay_tms(20);
    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					// ����յ�����
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);        // print usartdebug
            if(strstr((const char *)BC35_buf, res) != NULL)		// ����������ؼ���
            {
                BC35_Clear();                                 // ��ջ��� �ɹ�����0
							  ErrorTimes = 0;   // ���ϴ�������
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
//	�������ƣ�	BC35_SendData
//	�������ܣ�	��������
//	��ڲ�����	data������
//			 len������
//	���ز�����	��
//	˵����		
//==========================================================
void BC35_SendData(unsigned char *data, unsigned short len)
{
    char HTTP_BufSendAT[500];
    memset(HTTP_BufSendAT, 0, sizeof(HTTP_BufSendAT));
    sprintf((char*)(HTTP_BufSendAT),"%s,%d,%s\r\n","AT+NSOSD=2",len,(char *)data);
    UsartPrintf(USART_DEBUG,(char *)HTTP_BufSendAT);
    if(!BC35_SendCmd(HTTP_BufSendAT, "OK"))				//�յ���>��ʱ���Է�������
    {
        delay_tms(50);
        Usart_SendString(USART_BC35,"AT+NSORF=2,100\r\n", 16);	//�����豸������������
        if(BC35_WaitRecive() == REV_OK)							//����յ�����
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
//	�������ƣ�	BC35_GetIPD
//	�������ܣ�	��ȡƽ̨���ص�����
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//	���ز�����	ƽ̨���ص�ԭʼ����
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
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
        if(BC35_WaitRecive() == REV_OK)						// ����������
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
						NetStatus = 0xff;				  // �������
						return NULL;
					}
					  
					switch(NetStatus)
				  {
							case 0:     // �ȴ�����
								  SetLED(1);

									UsartPrintf(USART_DEBUG, "�յ�����������\r\n");
							
									UsartPrintf(USART_DEBUG,(char *)BC35_buf);		
                  UsartPrintf(USART_DEBUG, "�������\r\n");		
									
							    return (unsigned char *)(ptrIPD);
                  NetStatus = 0xff;				  // �������			
								break;
							
									// +NSONMI:2,47
							case 1:      // ����
								  
							    SetLED(0);
					
									RecNum = ptrIPD + 10;
									num = atoi(RecNum);
									UsartPrintf(USART_DEBUG, "RecNUM:%s;intnum:%d\r\n",RecNum,num);
										
									sprintf(cmdbuf, "AT+NSORF=2,%d\r\n",num);
										
									Usart_SendString(USART_BC35,(unsigned char *)cmdbuf, 15);	//�����豸������������
										
									if(BC35_WaitRecive() == REV_OK)							//����յ�����
									{
											UsartPrintf(USART_DEBUG,(char *)BC35_buf);
									}
									delay_tms(50);
										
									UsartPrintf(USART_DEBUG,"Receive data ready!\r\n");
									NetStatus = 0xff;				  // �������
								break;
							
							default:
								break;
						}
        }
        delay_tms(1);												// ��ʱ�ȴ�
    }while(timeOut--);
		
    return NULL;														// ��ʱ��δ�ҵ������ؿ�ָ��
}

//==========================================================
//	�������ƣ�	BC35_Init
//	�������ܣ�	��ʼ��BC35
//	��ڲ�����	��
//	���ز�����	��
//	˵����		
//==========================================================
void BC35_Init(void)
{
		//                1
    BC35_Clear();                          
    //delay_tms(100);
	  
	  //                2
    UsartPrintf(USART_DEBUG, "1.ATE0\r\n");     // �رջ���
    while(BC35_SendCmd("ATE0\r\n", "OK"))
        delay_tms(50);
    
		//                3                         // �����ƶ��ն˴��󱨸�
    while(BC35_SendCmd("AT+CMEE=1\r\n", "OK"))  // ��SIM��PIN�����������READY����ʾ�����ɹ�
        delay_tms(50);
    
		//                4                         // ����ָ�����ok
    UsartPrintf(USART_DEBUG, "2.AT\r\n");
    while(BC35_SendCmd("AT\r\n", "OK"))	        // ���¿�״̬�Ƿ�OK �ܷ��ȡ������
        delay_tms(50);
    
		//                5                         // ��ѯģ�����к�
    UsartPrintf(USART_DEBUG, "3.AT+CGSN=1\r\n");// ��ȡIMEI
    while(BC35_SendCmd("AT+CGSN=1\r\n", "OK"))
        delay_tms(50);
    
		//                9                   
    BC35_Clear();
		//                                            ��ѯ
    if(!BC35_SendCmd("AT+NBAND?\r\n","+NBAND:5"))// �ж��Ƿ�Ϊ+NBAND:5
    { 
			UsartPrintf(USART_DEBUG, "BAND:5 REG[..]\r\n");  // �Ƿ��ǵ��ŵ�
    }
    else
    { 
			UsartPrintf(USART_DEBUG, "BAND:8 REG[..]\r\n");  // ��������ƶ���ͨ
    }

    delay_tms(50);
    
		//                 10     ��ѯSIM��IMSIֵ
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
    
		//                 11                            ��ѯ�ź�ǿ��
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "5.AT+CSQ\r\n");
    while(!BC35_SendCmd("AT+CSQ\r\n","CSQ:99,99"))             // ����
        delay_tms(50);

    //	UsartPrintf(USART_DEBUG, "8. AT+QIACT?\r\n");
    //	while(BC35_SendCmd("AT+QIACT?\r\n","OK"))
    //		DelayXms(500);
		
		//                 12                            �������缤�����
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "6.AT+CGATT=1\r\n");
    BC35_SendCmd("AT+CGATT=1\r\n","OK");                   //����״̬
    delay_tms(50);
    
		//                 13
    UsartPrintf(USART_DEBUG, "7.AT+CGATT?\r\n");     // ��ѯ���缤��״̬
    while(BC35_SendCmd("AT+CGATT?\r\n","CGATT:1"))         //����״̬
    {
        UsartPrintf(USART_DEBUG, "Regedit PDP[..]\r\n");
        delay_tms(50);
    }
    
		//                 24
    UsartPrintf(USART_DEBUG, "8.AT+CGPADDR\r\n");     // ��ѯģ���ȡ��IP
    BC35_SendCmd("AT+CGPADDR\r\n","OK");              // ����״̬
    delay_tms(50);

    UsartPrintf(USART_DEBUG, "9.AT+NSOCR=STREAM,6,35001,1\r\n");        //  TCP
    while(BC35_SendCmd("AT+NSOCR=STREAM,6,35001,1\r\n","OK"))           //����Ϊ�����ӣ���Ȼƽ̨IP��������
    {
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        delay_tms(100);
    }

    UsartPrintf(USART_DEBUG, "-----------------\r\n");
    UsartPrintf(USART_DEBUG, BC35_ONENET_INFO);
    while(BC35_SendCmd(BC35_ONENET_INFO,"OK"))				// ���ӵ�������
    {
        UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        delay_tms(50);
        UsartPrintf(USART_DEBUG, "AT+NSOCR=STREAM,6,35001,1\r\n");
        while(BC35_SendCmd("AT+NSOCR=STREAM,6,35001,1\r\n","OK"))	// ����Ϊ�����ӣ���Ȼƽ̨IP��������
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
//	�������ƣ�	USART2_IRQHandler
//	�������ܣ�	����2�շ��ж�
//	��ڲ�����	��
//	���ز�����	��
//	˵��:
//==========================================================
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART_BC35, USART_IT_RXNE) != RESET) // �����ж�
    {
        if(BC35_cnt >= sizeof(BC35_buf))	BC35_cnt = 0;   // ��ֹ���ڱ�ˢ��
        BC35_buf[BC35_cnt++] = USART_BC35->DR;

        USART_ClearFlag(USART_BC35, USART_FLAG_RXNE);
    }
}

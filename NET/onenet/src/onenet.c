/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*	���ڣ� 		2017-05-08
	*	�汾�� 		V1.1
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"
#include "MQTTData.h"
#include "config.h"

//�����豸
#include "BC35.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
//C��
#include <string.h>
#include <stdio.h>


//#define PROID		"154131"
#define PROID   "204115"

//#define AUTH_INFO	"csicbc35"
#define AUTH_INFO	"czfrBC35M2"    // M2
//#define AUTH_INFO "czfrBC35M1"  // M1

//#define DEVID		"41058860"
#define DEVID   "514552697"   // M2
//#define DEVID "514531952"   // M1


extern unsigned char esp8266_buf[128];

char HTTP_BufHEX[450];     //HTTP���Ļ�����
u8 http_len=0;
void hextostr(uint8 *pkt,uint8 *data,uint32 len)
{
    u8 i;
    *pkt = 0;
    for(i=0;i<len;i++)
    {
        if(data[i]/16<10)
            pkt[2*i]=data[i]/16+0x30;
        else
            pkt[2*i]=data[i]/16+0x37;
        if(data[i]%16<10)
            pkt[2*i+1]=data[i]%16+0x30;
        else
            pkt[2*i+1]=data[i]%16+0x37;
    }
}

//==========================================================
//	�������ƣ�	OneNet_DevLink
//	�������ܣ�	��onenet��������
//	��ڲ�����	��
//	���ز�����	1-�ɹ�	0-ʧ��
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���
		
    unsigned char *dataPtr;
    _Bool status = 1;

		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                "PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                , PROID, AUTH_INFO, DEVID);
		}

		// //	��ڲ�����	user���û�������ƷID
   //				password�����룺��Ȩ��Ϣ��apikey
   //				devid���豸ID
   //				cTime�����ӱ���ʱ��     
   //				clean_session��������Ϣ�����־
   //				qos���ط���־
   //				will_topic���쳣����topic
   //				will_msg���쳣������Ϣ
   //				will_retain����Ϣ���ͱ�־
   //				mqttPacket����ָ��

    if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 255, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
        ClearRAM((u8 *)HTTP_BufHEX,500);
        http_len = mqtt_connect_message(HTTP_BufHEX,PROID,AUTH_INFO,DEVID);

        //		hextostr(mqttPacket._dataHEX,mqttPacket._data,mqttPacket._len);
			  if(OPERATING_MODE == DEBUGMODE)
	      {
          UsartPrintf(USART_DEBUG,(char *)HTTP_BufHEX);
				}
        BC35_SendData((u8 *)HTTP_BufHEX,http_len);		// �ϴ�ƽ̨
			
        dataPtr = BC35_GetIPD(250);									  // �ȴ�ƽ̨��Ӧ

				if(OPERATING_MODE == DEBUGMODE)
	      {
          UsartPrintf(USART_DEBUG, (char *)dataPtr);
				}
        if(dataPtr != NULL)
        {
            //			if(strstr((char *)dataPtr,"20020000"))
            //			{
            status = 0;
					  NetStatus = ONENETON;
            //			}
            //			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
            //			{
            //				switch(MQTT_UnPacketConnectAck(dataPtr))
            //				{
            //					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
            //					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
            //					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
            //					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
            //					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
            //					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
            //					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
            //				}
            //			}
        }

        MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
    }
    else
		{
			if(OPERATING_MODE == DEBUGMODE)
	    {
        UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
			}
		}

    return status;
}


unsigned char OneNet_FillBuf(char *buf)
{
    char text[200];
    memset(text, 0, sizeof(text));
	  
		sprintf(text, "{\"PowerAll\":%d,\"Temp\":%d,\"Power\":%d,\"Vol\":%d,\"Cur\":%d,\"Sta\":%d,\"PF\":%d,\"VA\":%d}",SinglePhase[0].RegS.EnegerL,SinglePhase[0].RegS.Temp,SinglePhase[0].RegS.Power,SinglePhase[0].RegS.Volt,SinglePhase[0].RegS.Current,SinglePhase[0].RegS.Status,SinglePhase[0].RegS.PF,SinglePhase[0].RegS.PowerAll);
	  strcat(buf, text);
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Xg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.x);
    //	strcat(buf, text);
    //
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Yg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.y);
    //	strcat(buf, text);

    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Zg\",\"datapoints\":[{\"value\":%0.2f}]}", adxl362Info.z);
    //	strcat(buf, text);

		/* ---------------------------------------------------------------------------------------
		
		//char json[]="{\"datastreams\":[{\"id\":\"temp\",\"datapoints\":[{\"value\":%d}]},{\"id\":\"humi\",\"datapoints\":[{\"value\":%d}]}]}";
			
			memset(text, 0, sizeof(text));
			sprintf(text, "{\"id\":\"Sta\",\"datapoints\":[{\"value\":%d}]},",Meter_Data.Status);
			strcat(buf, text);

			memset(text, 0, sizeof(text));
			sprintf(text, "{\"id\":\"Temp\",\"datapoints\":[{\"value\":%d}]}",Meter_Data.Temp);
			strcat(buf, text);
		---------------------------------------------------------------------------------------*/
		
    //		//LED�ƿ���
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"LED\",\"datapoints\":[{\"value\":%d}]},",1);
    //	strcat(buf, text);
    //GPS������
		/* ---------------------------------------------------------------------------------------
			memset(text, 0, sizeof(text));
			if(strlen(weiString)>0 &&  strlen(jingString)>0)
					sprintf(text, "{\"id\":\"GPS\",\"datapoints\":[{\"value\":{\"lon\":%s,\"lat\":%s}}]}",jingString,weiString);
			else
					sprintf(text, "{\"id\":\"GPS\",\"datapoints\":[{\"value\":{\"lon\":%s,\"lat\":%s}}]}","108.112393","22.449897");
			strcat(buf, text);
			
		---------------------------------------------------------------------------------------*/

			//strcat(buf, "]}");
		
    return strlen(buf);
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//	��ڲ�����	type���������ݵĸ�ʽ
//	���ز�����	��
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};									// Э���

    char buf[300];
    char bufhex[300];
		
    short body_len = 0, i = 0;

		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
		}

    memset(buf, 0, sizeof(buf));
    memset(bufhex, 0, sizeof(bufhex));
		
		// ���� json�ֶΣ�datastreams����
		
    body_len = OneNet_FillBuf(buf);						// ���onenet�������ݣ���ȡ��ǰ��Ҫ���͵����������ܳ���
		
		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, buf);
		}
		
    delay_tms(50);
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, FORMAT_TYPE3, &mqttPacket) == 0)						// ���
        {
            for(;i < body_len; i++)
                mqttPacket._data[mqttPacket._len++] = buf[i];           // �����͵����ݰ����뵽mqttpacket����
					
            mqtt_SendData_message(bufhex,buf,body_len);
					
					  if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "bufhex:%s\r\n", bufhex);
						}
					
            delay_tms(50);
					
            BC35_SendData((u8 *)bufhex,strlen((const char*)bufhex)/2);										//�ϴ����ݵ�ƽ̨
						
						if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
						}
            MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
        }
        else
				{
					if(OPERATING_MODE == DEBUGMODE)
	        {
            UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
					}
				}
    }
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//	�������ܣ�	ƽ̨�������ݼ��
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//	���ز�����	��
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���

    char *req_payload = NULL;
    char *cmdid_topic = NULL;

    unsigned short req_len = 0;

    unsigned char type = 0;

    short result = 0;

    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;

    type = MQTT_UnPacketRecv(cmd);
		
    switch(type)
    {
    case MQTT_PKT_CMD:															// �����·�

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
        if(result == 0)
        {
						if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
						}

            if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
            {
								if(OPERATING_MODE == DEBUGMODE)
	              {
                  UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
								}
                BC35_SendData(mqttPacket._data, mqttPacket._len);				//�ظ�����
                MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
            }
        }

        break;

		case MQTT_PKT_PUBACK:														// ����Publish��Ϣ��ƽ̨�ظ���Ack

                    if(MQTT_UnPacketPublishAck(cmd) == 0)
										{
											if(OPERATING_MODE == DEBUGMODE)
	                    {
                        UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
											}
										}
                    break;

		default:
                    result = -1;
                    break;
                }

    BC35_Clear();										//��ջ���

    if(result == -1)
        return;

    dataPtr = strchr(req_payload, '}');					//����'}'

    if(dataPtr != NULL && result != -1)					//����ҵ���
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
}

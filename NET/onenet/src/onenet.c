/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*	日期： 		2017-05-08
	*	版本： 		V1.1
	*	说明： 		与onenet平台的数据交互接口层
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"
#include "MQTTData.h"
#include "config.h"

//网络设备
#include "BC35.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
//C库
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

char HTTP_BufHEX[450];     //HTTP报文缓存区
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
//	函数名称：	OneNet_DevLink
//	函数功能：	与onenet创建连接
//	入口参数：	无
//	返回参数：	1-成功	0-失败
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包
		
    unsigned char *dataPtr;
    _Bool status = 1;

		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                "PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                , PROID, AUTH_INFO, DEVID);
		}

		// //	入口参数：	user：用户名：产品ID
   //				password：密码：鉴权信息或apikey
   //				devid：设备ID
   //				cTime：连接保持时间     
   //				clean_session：离线消息清除标志
   //				qos：重发标志
   //				will_topic：异常离线topic
   //				will_msg：异常离线消息
   //				will_retain：消息推送标志
   //				mqttPacket：包指针

    if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 255, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
        ClearRAM((u8 *)HTTP_BufHEX,500);
        http_len = mqtt_connect_message(HTTP_BufHEX,PROID,AUTH_INFO,DEVID);

        //		hextostr(mqttPacket._dataHEX,mqttPacket._data,mqttPacket._len);
			  if(OPERATING_MODE == DEBUGMODE)
	      {
          UsartPrintf(USART_DEBUG,(char *)HTTP_BufHEX);
				}
        BC35_SendData((u8 *)HTTP_BufHEX,http_len);		// 上传平台
			
        dataPtr = BC35_GetIPD(250);									  // 等待平台响应

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
            //					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
            //					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
            //					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
            //					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
            //					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
            //					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
            //					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
            //				}
            //			}
        }

        MQTT_DeleteBuffer(&mqttPacket);								//删包
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
		
    //		//LED灯控制
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"LED\",\"datapoints\":[{\"value\":%d}]},",1);
    //	strcat(buf, text);
    //GPS的数据
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
//	函数名称：	OneNet_SendData
//	函数功能：	上传数据到平台
//	入口参数：	type：发送数据的格式
//	返回参数：	无
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};									// 协议包

    char buf[300];
    char bufhex[300];
		
    short body_len = 0, i = 0;

		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
		}

    memset(buf, 0, sizeof(buf));
    memset(bufhex, 0, sizeof(bufhex));
		
		// 填入 json字段，datastreams内容
		
    body_len = OneNet_FillBuf(buf);						// 填充onenet发送数据，获取当前需要发送的数据流的总长度
		
		if(OPERATING_MODE == DEBUGMODE)
	  {
      UsartPrintf(USART_DEBUG, buf);
		}
		
    delay_tms(50);
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, FORMAT_TYPE3, &mqttPacket) == 0)						// 封包
        {
            for(;i < body_len; i++)
                mqttPacket._data[mqttPacket._len++] = buf[i];           // 将发送的数据包填入到mqttpacket变量
					
            mqtt_SendData_message(bufhex,buf,body_len);
					
					  if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "bufhex:%s\r\n", bufhex);
						}
					
            delay_tms(50);
					
            BC35_SendData((u8 *)bufhex,strlen((const char*)bufhex)/2);										//上传数据到平台
						
						if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
						}
            MQTT_DeleteBuffer(&mqttPacket);															//删包
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
//	函数名称：	OneNet_RevPro
//	函数功能：	平台返回数据检测
//	入口参数：	dataPtr：平台返回的数据
//	返回参数：	无
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包

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
    case MQTT_PKT_CMD:															// 命令下发

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
        if(result == 0)
        {
						if(OPERATING_MODE == DEBUGMODE)
	          {
              UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
						}

            if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
            {
								if(OPERATING_MODE == DEBUGMODE)
	              {
                  UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
								}
                BC35_SendData(mqttPacket._data, mqttPacket._len);				//回复命令
                MQTT_DeleteBuffer(&mqttPacket);									//删包
            }
        }

        break;

		case MQTT_PKT_PUBACK:														// 发送Publish消息，平台回复的Ack

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

    BC35_Clear();										//清空缓存

    if(result == -1)
        return;

    dataPtr = strchr(req_payload, '}');					//搜索'}'

    if(dataPtr != NULL && result != -1)					//如果找到了
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				//转为数值形式
    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
}

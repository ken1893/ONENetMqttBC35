#include "MQTTData.h"
#include "HEXSTR.h"
void ClearRAM(u8* ram,u32 n)
{
    u32 i;
    for (i = 0;i < n;i++)
    {
        ram[i] = 0x00;
    }
}

//构建MQTT连接包 
u16 mqtt_connect_message(char *mqtt_message,char *PROID,char *AUTH_INFO,char *DEVID)
{
    //	char HTTP_Buf[400];     //HTTP报文缓存区
    char str[500];
    char strHEX[500];
    u16 PROID_length = strlen(PROID);
    u16 DEVID_length = strlen(DEVID);
    u16 AUTH_INFO_length = strlen(AUTH_INFO);
    u16 MQTT_length = strlen("MQTT");
    u16 packetLen;
    ClearRAM((u8*)str,200);   //清空commandAT数据包缓存，发送命令包含ip端口和长度
    ClearRAM((u8*)strHEX,200); //清空locationHex数据包缓存，转换为hex 需要发送的真实数据
    packetLen = 12;//固定的数据头长度
    if(PROID_length > 0)
        packetLen = packetLen + 2 + PROID_length;               // 2字节字符串长度+utf8字符串
    if(DEVID_length > 0)
        packetLen = packetLen+ 2 + DEVID_length;
    if(AUTH_INFO_length > 0)
        packetLen = packetLen+ 2 + AUTH_INFO_length;

    strncpy((char*)(mqtt_message + strlen((char*)mqtt_message)),"10",2);    // 
    sprintf(strHEX,"%02x",(packetLen-2));

    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);   //清空commandAT数据包缓存，发送命令包含ip端口和长度
    ClearRAM((u8*)strHEX,200); //清空locationHex数据包缓存，转换为hex 需要发送的真实数据
		
    strcat(mqtt_message,"00044D51545404C00100");
		
    //装载设备ID到数据包		
    sprintf(str, "%02x%02x",((0xff00&(DEVID_length))>>8),(0xff&DEVID_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",DEVID);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    //装载产品ID到数据包
    sprintf(str, "%02x%02x",((0xff00&(PROID_length))>>8),(0xff&PROID_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",PROID);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    //装载产品ID到数据包
    sprintf(str, "%02x%02x",((0xff00&(AUTH_INFO_length))>>8),(0xff&AUTH_INFO_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",AUTH_INFO);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    strcat(mqtt_message,"\r\n");
    return packetLen;
}

//-------------------------------------------------------------------------------------
//
//----------------------------------构建MQTT数据包-------------------------------------
//
//-------------------------------------------------------------------------------------
u16 mqtt_SendData_message(char *mqtt_messageHEX,char *SendData,short length)
{
    char str[200];
    char strHEX[200];
    u16 packetLen = length + 8;
    u16 SendData_length = strlen(SendData);
	
    strncpy((char*)(mqtt_messageHEX + strlen((char*)mqtt_messageHEX)),"30",2);
	
    sprintf(strHEX,"%02x",(packetLen));
	
    strcat(mqtt_messageHEX,strHEX);
    ClearRAM((u8*)str,200);         // 清空commandAT数据包缓存，发送命令包含ip端口和长度
    ClearRAM((u8*)strHEX,200);      // 清空locationHex数据包缓存，转换为hex 需要发送的真实数据
	
	 //
    strcat(mqtt_messageHEX,"000324647003");        // ????  标题开始/01 null/00  ETX (end of text)正文结束/03	$/24  d/64  p/70  
	  
	  
    //装载json数据到数据包
    sprintf(str, "%02x%02x",((0xff00&(SendData_length))>>8),(0xff&SendData_length));
	
    strcat(mqtt_messageHEX,str);
    ClearRAM((u8*)str,200);
		
		// 
    sprintf(str, "%s",SendData);          // copy SendData to str
    Str2Hex((char*)str,(char*)strHEX);    // convert str to strHEX
		
    strcat(mqtt_messageHEX,strHEX);       // 拼接strHEX到mqtt_messageHex尾部
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    return packetLen;
}

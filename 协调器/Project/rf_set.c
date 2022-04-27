#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "LCD.h"
#include "sensor_drv/sensor.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
/*****点对点通讯地址设置******/
#define RF_CHANNEL                17         // 频道 11~26
#define PAN_ID                    0x0001     //网络id 
#define MY_ADDR                   0x0007     //本机模块地址
#define SEND_ADDR                 0x0009     //发送地址
/**************************************************/

#define LED1 P1_0
#define LED2 P1_1

unsigned char LcdDataTem[5]={':','0','0','C','\0'};
unsigned char LcdDataHum[7]={':','0','0','R','H','%','\0'};
unsigned char LcdDataTur[7]={':','0','0','R','H','%','\0'};

static unsigned char RfRxData[128];
static unsigned char RfTxData[128];

static unsigned char RxData[128];
static unsigned char TxData[128];

static basicRfCfg_t basicRfConfig;
// 无线RF初始化
void ConfigRf_Init(void)
{
    basicRfConfig.panId       =   PAN_ID;
    basicRfConfig.channel     =   RF_CHANNEL;
    basicRfConfig.myAddr      =   MY_ADDR;
    basicRfConfig.ackRequest  =   TRUE;
    while(basicRfInit(&basicRfConfig) == FAILED);
    basicRfReceiveOn();
}

void led_init()
{
  P1SEL &=~0x03;
  P1DIR |=0x03;
  LED1=0;
  LED2=0;
}

void main(void)
{
    int i;
    halBoardInit();
    ConfigRf_Init();
    led_init();
    LCD_Init();
    while(1)
    {
      if(basicRfPacketIsReady())
      {
        LED2=!LED2;
        int len=basicRfReceive(RfRxData,128,0);
        halUartWrite(RfRxData,len);
      }
      for(i=0;i<2;i++)
      {
        LcdDataTem[1+i]=RfRxData[i];
      }
      for(i=0;i<2;i++)
      {
        LcdDataHum[1+i]=RfRxData[2+i];
      }
      for(i=0;i<2;i++)
      {
        LcdDataTur[1+i]=RfRxData[5+i];
      }
      if(RfRxData[4]=='1')
      {
        RfTxData[0]=1+0x30;
        basicRfSendPacket(SEND_ADDR,RfTxData,1);
      }
      else
      {
        RfTxData[0]=0+0x30;
        basicRfSendPacket(SEND_ADDR,RfTxData,1);
      }
      LCD_P16x16Ch(0,2,4);
      LCD_P16x16Ch(0,3,5);
      LCD_P16x16Ch(0,4,6);
      LCD_P16x16Ch(0,5,7);
      
      LCD_P8x16Str(1,8,LcdDataTur);
      LCD_P16x16Ch(1,0,8);
      LCD_P16x16Ch(1,1,9);
      LCD_P16x16Ch(1,2,10);
      LCD_P16x16Ch(1,3,11);
      
      LCD_P8x16Str(2,4,LcdDataTem);
      LCD_P16x16Ch(2,0,0);
      LCD_P16x16Ch(2,1,1);
      
      LCD_P8x16Str(3,4,LcdDataHum);
      LCD_P16x16Ch(3,0,2);
      LCD_P16x16Ch(3,1,3);
      
    }
}
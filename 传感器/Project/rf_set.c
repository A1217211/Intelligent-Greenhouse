#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "sensor_drv/sensor.h"
#include "dht11.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
/*****点对点通讯地址设置******/
#define RF_CHANNEL                17         // 频道 11~26
#define PAN_ID                    0x0001     //网络id 
#define MY_ADDR                   0x0009     //本机模块地址
#define SEND_ADDR                 0x0007     //发送地址
/**************************************************/

#define LED1 P1_0
#define LED2 P1_1
#define LED3 P1_4

static unsigned char RfRxData[128];
//static unsigned char RfTxData[128];

//static unsigned char RxData[128];
//static unsigned char TxData[128];
unsigned char Temp[10];
unsigned char TurHum;
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

int adc_get()//adc转换
{
  unsigned int value;
  APCFG|=0x20;
  P0SEL|=0x20;
  P0DIR&=~0x20;
  ADCCON3=0x35;
  ADCCON1|=0x30;
  ADCCON1|=0x40;
  while(!(ADCCON1&0x80));
  value=ADCL>>2;
  value|=((unsigned int)ADCH<<6)>>2;
  return((value));
}

unsigned char GetTurHum()
{
  int adc=0;
  float vol=0.0;
  unsigned char TurHum=0;
  adc=adc_get();
  
  adc=4096-adc;
  
  vol=(float)((float)adc)/4096;
  TurHum=vol*100;
  return TurHum;
}

void led_init()
{
  P1SEL &=~0x13;
  P1DIR |=0x13;
  LED1=0;
  LED2=0;
  LED3=0;
}

void tr_init()
{
  P0SEL &=~0x60;
  P0DIR &=~0x60;
}

void delay_ms(unsigned int n)//延迟函数
{
  unsigned int i,j;
  for(i=0;i<n;i++)
  {
    for(j=0;j<1100;j++);
  }
}
void main(void)
{
    halBoardInit();
    ConfigRf_Init();
    led_init();
    tr_init();
    P0SEL &= 0x7f;
    P0SEL &=~0x01;
    P0DIR |=0x01;
    while(1)
    {
      memset(Temp,0,10);
      DHT11();
      TurHum=GetTurHum();
      Temp[0]=wendu+0x30;
      Temp[1]=wendu_ge+0x30;
      Temp[2]=shidu+0x30;
      Temp[3]=shidu_ge+0x30;
      Temp[4]=(P0_6>0?0:1)+0x30;//0低湿度，1高湿度
      Temp[5]=(TurHum/10)+0x30;
      Temp[6]=(TurHum%10)+0x30;
      halUartWrite(Temp,7);
      basicRfSendPacket(SEND_ADDR ,Temp,7);
      if(basicRfPacketIsReady())
      {
        LED2=!LED2;
        basicRfReceive(RfRxData,128,0);
        if(RfRxData[0]=='1')
        {
          P0_0=1;
        }
        else
        {
          P0_0=0;
        }
      }
      LED1=!LED1;
      delay_ms(1000);
    }
}
/**************************************************************************************************
  Filename:      sh10.c

  Description:    sh10 driver 
                  
**************************************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include "ioCC2530.h"
#include "hal_defs.h"
//#include "hal_types.h"
#include "get_adc.h"


/******************************************************************************
 * GLOBAL VARIABLES
 */
void hal_adc_Init(void);
 uint16 readVoltage(void);
//static volatile uint32 value;

void hal_adc_Init(void)
{
	    APCFG  |=1;   
        P0SEL  |= (1 << (0));	
        P0DIR  &= ~(1 << (0));	
    
}
/******************************************************************************
 * 名称       get_adc
 * 功能       读取A/D值
 * 入口参数   无
 * 出口参数   16位电压值，分辨率为10mV, 如0x0102表示2.58V
 *****************************************************************************/
uint16 get_adc(void)
{
   int32 value;
   hal_adc_Init(); // ADC初始化
   ADCIF = 0;   //清ADC 中断标志
   //采用基准电压avdd5:3.3V，通道0，启动AD转化
   ADCCON3 = (0x80 | 0x10 | 0x00);
   while ( !ADCIF )
   {
       ;  //等待AD转化结束
   }
   value = ADCH;
   value = value<< 8;
   value |= ADCL;
   if(value&0x8000)
       value=0;
  // AD值转化成电压值
  // 0 表示 0V ，32768 表示 3.3V
  // 电压值 = (value*3.3)/32768 （V)
  value = (value * 330);
  value = value >> 15;   // 除以32768
  // 返回分辨率为0.01V的电压值
  return (uint16)value;
}
#define ACH1    0
#define ACH2    4
#define ACH3    5
#define ACH4    6

void hal_adc4_Init(void);
void hal_adc4_Init(void)
{
	    APCFG  |= (1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4);   
        P0SEL  |= (1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4);	
        P0DIR  &= ~((1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4));	
    
}
#define AD4MA 32768*150*4/3300
#define AD20MA 32768*150*20/3300
//return 10AD值，0---0v, 3ff---3.3v
uint16 adC_ch(uint8 ch);
uint16 adC_ch(uint8 ch)
{
    int32 value;
    ADCIF = 0;   //清ADC 中断标志
   //采用基准电压avdd5:3.3V，通道ch，启动AD转化
   ADCCON3 = (0x80 | 0x10 | (ch&0x0f) );
   while ( !ADCIF )
   {
       ;  //等待AD转化结束
   }
   value = ADCH;
   value = value<< 8;
   value |= ADCL;
   if(value&0x8000)
       value=0;
   value= value>>5;
   return (uint16)value;
   /*
  // AD值转化成电压值
  // 0 表示 0V ，32768 表示 3.3V
  // 电流 0--4ma, 255--20ma, 电阻150r
  value = (value - AD4MA)*255/(AD20MA - AD4MA);
  if(value<0)
      return    0;
  if(value>255)
      return    255;
  return    (uint8)value&0xff;*/
}
/******************************************************************************
 * 名称       get_adc4
 * 功能       读取四通道A/D值 ch0 ch4 ch5 ch6 
 * 入口参数   无
 * 出口参数   
 *****************************************************************************/
void get_adc4ch(uint8 *buf)
{
   uint16 value;
   hal_adc4_Init(); // ADC初始化
   value=adC_ch(ACH1);
   *buf=value;
   buf++;
   *buf=value>>8;
   buf++;
   value=adC_ch(ACH2);
   *buf=value;
   buf++;
   *buf=value>>8;
   buf++;
   value=adC_ch(ACH3);
   *buf=value;
   buf++;
   *buf=value>>8;
   buf++;
   value=adC_ch(ACH4);
   *buf=value;
   buf++;
   *buf=value>>8;
}


























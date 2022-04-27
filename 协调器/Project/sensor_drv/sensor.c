/**************************************************************************************************
  Filename:       sensor.c  */



#include "hal_defs.h"
//#include "hal_types.h"
#include "hal_mcu.h"
#include "ioCC2530.h"
//#include "sh10.h"
#include "get_adc.h"
#include "sensor.h"
#include "dma_itg.h"
#include "bma020.h"
//#include "UART_PRINT.h"
//#include "OnBoard.h"

 
/******************************************************************************
 * GLOBAL VARIABLES
 */
void hal_LightSensor_Init(void);
static uint16 readVoltage(void);

uint16 get_guangdian_ad(void);
void get_4channel_ad(uint8 *buf);
uint8 get_rentihongwai(void);
void get_wendu_shidu(int16 *temp, int16 *hum);
//static volatile uint32 value;

void hal_LightSensor_Init(void)
{
	APCFG  |=1;   
        P0SEL  |= (1 << (0));	
        P0DIR  &= ~(1 << (0));	
        P0INP |=1;         
}
/******************************************************************************
 * @fn          readVoltage
 *
 * @brief       read voltage from ADC
 *
 * @param       none
 *              
 * @return      voltage
 *******/
static uint16 readVoltage(void)
{

   uint32 value;
   int16 value1;
   hal_LightSensor_Init(); 
  // Clear ADC interrupt flag 
  ADCIF = 0;

  ADCCON3 = (0x80 | 0x10 | 0x00);
  //ADCCON3 = (0x80 | 0x10 | 0x0c);
  //采用基准电压avdd5:3.3V
  //  Wait for the conversion to finish 
  while ( !ADCIF );

  // Get the result
  value1 = ADCH;
  value1 = value1<< 8;
  value1 |= ADCL;
  if(value1<0)
      value1=0;
  value=value1;

  // value now contains measurement of 3.3V
  // 0 indicates 0V and 32767 indicates 3.3V
  // voltage = (value*3.3)/32767 volts
  // we will multiply by this by 10 to allow units of 0.01 volts
  value = value >> 6;   // divide first by 2^6
  value = (uint32)(value * 330);
  value = value >> 9;   // ...and later by 2^9...to prevent overflow during multiplication

  return (uint16)value;
 
}

/******************************************************************************
 * @fn          readsensor
 *
 * @brief       read readsensor 传感器采样
 *
 * @param       sensorid
 *              
 * @return      sensor value, len
 *******/
uint8 readsensor(uint8 sensorid, char *val)
{
       if(sensorid == 0x01)
       {    //温湿度
           //call_sht11(tmp1,tmp2);
           
           return 4;    
       } 
       if(sensorid == 0x02)
       {    // G_SENSOR
           BMA_Pooling((int16*)val, (int16*)(val+2),(int16*)(val+4));
           return 6;    
       } 
       if(sensorid == 0x03)
       {    //陀螺仪
            ITG_Pooling((int16*)val, (int16*)(val+2),(int16*)(val+4));
            return 6;    
       } 
        if(sensorid == 0x4)
       {    //人体 p0.1 
            APCFG &= ~(1<<1);
            P0SEL  &= ~(1 << (1));
            P0DIR  &= ~(1 << (1));	
            *val=0;
            if( P0_1==0 )
               *val=0x01; 
            return 2;    
       } 
       if(sensorid == 0x5)
       {    //四通道 ad  
            get_adc4ch(val);
            return 8;    
       } 
       if(sensorid == 0x6)
       {    //光照  adc 或其他  
            *(int16*)val=get_adc();
            return 2;    
       } 
}
uint16 get_guangdian_ad(void)
{
  return get_adc();
}
void get_4channel_ad(uint8 *buf)
{
  get_adc4ch(buf);
}
//1 youren
uint8 get_rentihongwai(void)
{
  uint8 val;
  APCFG &= ~(1<<1);
  P0SEL  &= ~(1 << (1));
  P0DIR  &= ~(1 << (1));	
  val=0;
  if( P0_1==0 )
     val=0x01; 
  return val;
}

void get_wendu_shidu(int16 *temp, int16 *hum)
{
  int16 tmp1,tmp2;
  call_sht11 (&tmp1,&tmp2);
  *temp = tmp1;
  *hum = tmp2;
}
//1 dongzuo
void set_relay(uint8 status)
{
  if(status == 1){
    HAL_TURN_ON_DC1();
  }else{
    HAL_TURN_OFF_DC1();
  }
}
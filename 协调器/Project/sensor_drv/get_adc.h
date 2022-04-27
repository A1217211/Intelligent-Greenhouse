/*************************************************************************************************
  Filename:      adc.h
 
 *****/


#ifndef SIMPLE_adc_H
#define SIMPLE_adc_H


extern  void hal_adc_Init(void); 
extern  uint16 get_adc(void);
extern  void get_adc4ch(uint8 *buf);
#endif



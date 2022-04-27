/**************************************************************************************************
  Filename:      
 
 
**************************************************************************************************/


#ifndef SENSOR_H
#define SENSOR_H

/******************************************************************************
 * CONSTANTS
 */


/******************************************************************************
 * PUBLIC FUNCTIONS
 */
extern uint8 readsensor(uint8 sensorid, char *val);

extern uint16 get_guangdian_ad(void);
extern void get_4channel_ad(uint8 *buf);
extern uint8 get_rentihongwai(void);
extern void get_wendu_shidu(int16 *temp, int16 *hum);
extern void set_relay(uint8 status);
#endif // 





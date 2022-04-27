#ifndef _FLASH_DATA_H
#define _FLASH_DATA_H


// Function prototype for the multiple words write function 
extern void writeWordM( uint8 pg, uint16 offset, uint8 *buf, uint16 cnt );

// Function prototype for the word write function 
extern void writeWord( uint8 pg, uint16 offset, uint8 *buf );

// Function prototype for the buffer write function 
extern void writeBuf( uint8 dstPg, uint16 dstOff, uint16 len, uint8 *buf );

// Function prototype for delay Function
void delay(int time );

#endif
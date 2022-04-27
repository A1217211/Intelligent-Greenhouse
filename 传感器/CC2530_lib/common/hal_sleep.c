#include <hal_sleep.h>

void SET_POWER_MODE(unsigned char mode)
{                                      
	if(mode == 0)         
		SLEEPCMD &= ~0x03; 
	else 
	if(mode == 3)  
		SLEEPCMD |= 0x03; 
	else 
	{ 
		SLEEPCMD &= ~0x03; 
		SLEEPCMD |= mode;  
	} 
	
	PCON |= 0x01;                            
	asm("NOP");                              
}

void Set_ST_Period(unsigned char sec) 
{ 
	unsigned long sleepTimer = 0; 

	sleepTimer |= ST0; 
	sleepTimer |= (unsigned long)ST1 << 8; 
	sleepTimer |= (unsigned long)ST2 << 16; 

	sleepTimer += ((unsigned long)sec * (unsigned long)32768);

	ST2 = (unsigned char)(sleepTimer >> 16); 
	ST1 = (unsigned char)(sleepTimer >> 8); 
	ST0 = (unsigned char) sleepTimer; 
}

void Init_SLEEP_TIMER(void) 
{ 	
	EA = 1;  
	STIE = 1; 
	STIF = 0; 
} 


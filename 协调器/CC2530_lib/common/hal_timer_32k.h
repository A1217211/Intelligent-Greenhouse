//-------------------------------------------------------------------
// Filename: hal_timer_32k.h
// Description:  hal 32KHz timer library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
#define TIMER_32K_CLK_FREQ    32768  

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void halTimer32kInit(uint16 cycles);
void halTimer32kRestart(void);
void halTimer32kIntConnect(ISR_FUNC_PTR isr);
void halTimer32kIntEnable(void);
void halTimer32kIntDisable(void);

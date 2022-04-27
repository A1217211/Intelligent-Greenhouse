//-------------------------------------------------------------------
// Filename: hal_mcu.h
// Description: hal mcu library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void halMcuInit(void);
void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
void halMcuSetLowPowerMode(uint8 mode);
void halMcuReset(void);
uint8 halMcuGetResetCause(void);

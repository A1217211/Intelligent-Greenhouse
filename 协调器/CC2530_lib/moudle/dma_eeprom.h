//-------------------------------------------------------------------
// Filename: dma_eeprom.h
// Description: dma_eeprom module library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------

void M3_Init(void);
void M3_WriteEEPROM(uint8 addr, uint8 ch);
uint8 M3_ReadEEPROM(uint8 addr);

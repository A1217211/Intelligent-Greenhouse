//-------------------------------------------------------------------
// Filename:  dma_bma.h
// Description: dma_bma module library header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
extern uint16 BMA_X_Value;
extern uint16 BMA_Y_Value;
extern uint16 BMA_Z_Value;

void M3_Init(void);
//void M3_WriteEEPROM(uint8 addr, uint8 ch);
uint8 M3_ReadEEPROM(uint8 addr);

void BMA_Pooling(void);

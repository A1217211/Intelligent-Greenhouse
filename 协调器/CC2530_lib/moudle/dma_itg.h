#define ACK 1
#define noACK 0
#define measure_temp_H 0x1B 
#define measure_temp_L 0x1C
#define measure_X_H 0x1D 
#define measure_X_L 0x1E 
#define measure_Y_H 0x1F 
#define measure_Y_L 0x20
#define measure_Z_H 0x21 
#define measure_Z_L 0x22 


#define     uchar    unsigned   char
#define     uint     unsigned   int
#define     ulong    unsigned   long

extern uint16 BMA_X_Value,BMA_Y_Value,BMA_Z_Value;
extern uint8 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
extern uint8 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;



void M3_Init(void);          
void BMA_Pooling(void);
void M3_WriteEEPROM(uint8 addr, uint8 ch);

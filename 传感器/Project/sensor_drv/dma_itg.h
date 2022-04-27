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
#define     uint     unsigned   short
#define     ulong    unsigned   long


  
extern void  ITG_Pooling(int16* X_Value, int16* Y_Value, int16* Z_Value  );


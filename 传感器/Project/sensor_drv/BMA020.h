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


/******************************************************************************
 * 名称       BMA_Pooling
 * 功能       读取bma020的X,Y Z的重力加速度值
 * 入口参数 (int16 *BMA_X_Value,int16 *BMA_Y_Value,int16 *BMA_Z_Value)
            *BMA_X_Value：X的重力加速度值存放地址
            *BMA_Y_Value：Y的重力加速度值存放地址
            *BMA_Z_Value：Z的重力加速度值存放地址
            加速度值为16位有符号数，分辨率为 2/512 (g), g=9.8m/s
 * 出口参数  无  
 *****************************************************************************/       
extern void BMA_Pooling(int16 *BMA_X_Value,int16 *BMA_Y_Value,int16 *BMA_Z_Value);


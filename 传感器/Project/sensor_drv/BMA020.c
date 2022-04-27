//-------------------------------------------------------------------
// Filename: dma_bma.c
// Description:  dma_bma module library (I2C)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
/******************************************************************************
 * INCLUDES
 */
#include "ioCC2530.h"
//#include "hal_types.h"
#include "hal_defs.h"
#include "BMA020.h"



#define BM(n)      (1 << (n))

#define MCU_IO_TRISTATE   1             
#define MCU_IO_PULLUP     2
#define MCU_IO_PULLDOWN   3

#define MCU_IO_OUTPUT(port, pin, val)  MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_OUTPUT_PREP(port, pin, val)  st( P##port##SEL &= ~BM(pin); P##port##_##pin## = val; P##port##DIR |= BM(pin); )
#define MCU_IO_INPUT(port, pin, func)  MCU_IO_INPUT_PREP(port, pin, func)
#define MCU_IO_INPUT_PREP(port, pin, func)  st( P##port##SEL &= ~BM(pin); P##port##DIR &= ~BM(pin); switch (func) { case MCU_IO_PULLUP: P##port##INP &= ~BM(pin); P2INP &= ~BM(port + 5); break; case MCU_IO_PULLDOWN: P##port##INP &= ~BM(pin); P2INP |= BM(port + 5); break; default: P##port##INP |= BM(pin); break; } )
#define MCU_IO_GET(port, pin)          MCU_IO_GET_PREP(port, pin)
#define MCU_IO_GET_PREP(port, pin)          (P##port## & BM(pin))



#define HAL_BOARD_IO_SHT_SCK_PORT           1
#define HAL_BOARD_IO_SHT_SCK_PIN             6
#define HAL_BOARD_IO_SHT_SDA_PORT           1
#define HAL_BOARD_IO_SHT_SDA_PIN             5

#define HAL_BOARD_IO_E2PROM_SCL_PORT  HAL_BOARD_IO_SHT_SCK_PORT
#define HAL_BOARD_IO_E2PROM_SCL_PIN    HAL_BOARD_IO_SHT_SCK_PIN


#define HAL_E2PROM_SCK_DIR_OUT()          MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,0)
#define HAL_E2PROM_SCK_SET()                  MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,1)
#define HAL_E2PROM_SCK_CLR()                  MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,0)
#define HAL_E2PROM_SCL_DIR_OUT        HAL_E2PROM_SCK_DIR_OUT
#define HAL_E2PROM_SCL_SET                 HAL_E2PROM_SCK_SET
#define HAL_E2PROM_SCL_CLR                HAL_E2PROM_SCK_CLR

#define HAL_E2PROM_SDA_SET()                 MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,1)
#define HAL_E2PROM_SDA_CLR()                 MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,0)
#define HAL_E2PROM_SDA_DIR_OUT()         MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,1)
#define HAL_E2PROM_SDA_DIR_IN()           MCU_IO_INPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,2)
#define HAL_E2PROM_SDA_VAL()                MCU_IO_GET(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN)   
#define halMcuWaitUs  McuWaitUs
#define NOP()    asm("NOP")
//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
static void McuWaitUs2(uint16 usec);
void McuWaitUs2(uint16 usec)
{
    usec >>= 1;
    while (usec--)
    {
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        //NOP();
    }
}
//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
static void M2_I2cStart(void);
void M2_I2cStart(void)//在SCL保持高电平期间，SDA由高变低，为起始信号
{
    HAL_E2PROM_SCL_SET();  //SCL=1

    HAL_E2PROM_SDA_SET();  //SDA=1
    McuWaitUs2(2);
    HAL_E2PROM_SDA_CLR();  //SDA=0
    McuWaitUs2(2);

    HAL_E2PROM_SCL_CLR();  //SCL=0
    McuWaitUs2(2);
}

//-------------------------------------------------------------------
static void M2_I2cStop(void);
void M2_I2cStop(void)//在SCL保持高电平期间，SDA由低变高，为停止信号
{
    HAL_E2PROM_SCL_SET();  //SCL=1
    McuWaitUs2(2);
    
    HAL_E2PROM_SDA_CLR();  //SDA=0
    McuWaitUs2(2);

    HAL_E2PROM_SDA_SET();  //SDA=1
    McuWaitUs2(2);
    
    HAL_E2PROM_SCL_CLR();  //SCL=0
    McuWaitUs2(2);
}

//-------------------------------------------------------------------
static void M2_I2cAck(void);
void M2_I2cAck(void)//当SDA=0时产生应答信号
{
    HAL_E2PROM_SCL_SET();
    McuWaitUs2(2);
    
    HAL_E2PROM_SDA_CLR();
    McuWaitUs2(2);
    
    HAL_E2PROM_SCL_CLR();
    McuWaitUs2(2);
}

//-------------------------------------------------------------------
static void M2_I2cNack(void);
void M2_I2cNack(void)//当SDA=1时产生非应答信号
{
    HAL_E2PROM_SCL_SET();
    McuWaitUs2(2);
    
    HAL_E2PROM_SDA_SET();
    McuWaitUs2(2);
    
    HAL_E2PROM_SCL_CLR();
    McuWaitUs2(2);
}

//-------------------------------------------------------------------
static void M2_I2cWrite(uint8 dat);
void M2_I2cWrite(uint8 dat)//在传送数据时，数据线保持稳定不能有跳变
{
    uint8 i;

    for (i = 0; i < 8; i++)
    {
        if (dat &0x80)
        {
            HAL_E2PROM_SDA_SET();
        }
        else
        {
            HAL_E2PROM_SDA_CLR();
        }
        HAL_E2PROM_SCL_SET();//当时钟线为高电平(SCL=1)时开始传送数据
        dat = dat << 1;
        McuWaitUs2(2);
        HAL_E2PROM_SCL_CLR();//传送完后释放总线（SCL=0）
        McuWaitUs2(2);
    }
}

//-------------------------------------------------------------------
static uint8 M2_I2cRead(void);
uint8 M2_I2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_E2PROM_SDA_SET();//在读数据之前，把数据线拉高（datesheet上有介绍）,然后把数据线端口对CPU设置成输入端口
    HAL_E2PROM_SDA_DIR_IN();
    McuWaitUs2(2);

    for (cnt = 0; cnt < 8; cnt++)
    {
        HAL_E2PROM_SCL_SET();
        res <<= 1;
        if (HAL_E2PROM_SDA_VAL())//cpu获取SDA端口上的值
        {
            res |= 0x01;
        }
        McuWaitUs2(2);
        HAL_E2PROM_SCL_CLR();
        McuWaitUs2(2);
    }
    HAL_E2PROM_SDA_DIR_OUT();//cpu获取完一个字节后，再将SDA端口设置成输出，且置高此端口
    return res;
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
static void M2_Init(void);
void M2_Init(void)//初始化 SDA=1、SCL=1
{
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCL_PORT, HAL_BOARD_IO_E2PROM_SCL_PIN,1);
}


//-------------------------------------------------------------------
static void M2_WriteEEPROM(uint8 addr, uint8 ch);
void M2_WriteEEPROM(uint8 addr, uint8 ch)//往EEPROM里写数据（先写地址，在写数据）
{
    M2_I2cStart();
    
    M2_I2cWrite(0x70);//写器件地址，对于AT24C02来说，高四位固定为1010,后三位没有意义，最后一位为0时写，为1时读
    M2_I2cAck();
    
    M2_I2cWrite(addr);
    M2_I2cAck();
    
    M2_I2cWrite(ch); //data
    M2_I2cAck();
    
    M2_I2cStop();
   // halMcuWaitMs(5);
}

//-------------------------------------------------------------------
static uint8 M3_ReadEEPROM(uint8 addr);
uint8 M3_ReadEEPROM(uint8 addr)
{
    uint8 res;
    
    M2_I2cStart();
    
    M2_I2cWrite(0x70); //器件设置为写状态
    M2_I2cAck();
    
    M2_I2cWrite(addr);
    M2_I2cAck();

    
    M2_I2cStart();
    
    M2_I2cWrite(0x71); //器件设置为写状态
    M2_I2cAck();
      
    res = M2_I2cRead();
    
    M2_I2cNack();
    M2_I2cStop();

    return res;
}


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
void BMA_Pooling(int16 *BMA_X_Value,int16 *BMA_Y_Value,int16 *BMA_Z_Value)
{
    uint16 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
    uint16 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;
    int16 value;
    M2_Init();
    BMA_XL_Value = M3_ReadEEPROM(0x02);
    BMA_XM_Value = M3_ReadEEPROM(0x03);
    BMA_YL_Value = M3_ReadEEPROM(0x04);
    BMA_YM_Value = M3_ReadEEPROM(0x05);
    BMA_ZL_Value = M3_ReadEEPROM(0x06);
    BMA_ZM_Value = M3_ReadEEPROM(0x07);
    value = (BMA_XM_Value << 8) | ((BMA_XL_Value&0xc0));
    *BMA_X_Value= value/64;
    value = (BMA_YM_Value << 8) | ((BMA_YL_Value&0xc0));
    *BMA_Y_Value= value/64;
    value = (BMA_ZM_Value << 8) | ((BMA_ZL_Value&0xc0));
    *BMA_Z_Value= value/64;
}


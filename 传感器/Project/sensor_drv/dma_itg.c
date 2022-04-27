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
#include "hal_defs.h"
//#include "hal_types.h"
#include "dma_itg.h"



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



/***********新板板IO设置********************/
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
#define halMcuWaitUs  ITGMcuWaitUs
#define NOP()    asm("NOP")
//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
static void ITGMcuWaitUs(uint16 usec);
void ITGMcuWaitUs(uint16 usec)
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
//--------- write a byte and checks the ack signal----------------- 
static void M3_I2cWrite(uint8 dat);
void M3_I2cWrite(uint8 dat)//在传送数据时，数据线保持稳定不能有跳变
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
        HAL_E2PROM_SCK_SET();//当时钟线为高电平(SCL=1)时开始传送数据
        dat = dat << 1;
        halMcuWaitUs(2);
        HAL_E2PROM_SCK_CLR();//传送完后释放总线（SCL=0）
        halMcuWaitUs(2);
    }
    
}

//--------- read a byte and checks the ack signal----------------- 
static uint8 M3_I2cRead(void);
uint8 M3_I2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_E2PROM_SDA_SET();//在读数据之前，把数据线拉高（datesheet上有介绍）,然后把数据线端口对CPU设置成输入端口
    HAL_E2PROM_SDA_DIR_IN();
    halMcuWaitUs(2);

    for (cnt = 0; cnt < 8; cnt++)
    {
        res <<= 1;
        if (HAL_E2PROM_SDA_VAL())//cpu获取SDA端口上的值
        {
            res |= 0x01;
        }
        HAL_E2PROM_SCK_SET();
        halMcuWaitUs(2);
        HAL_E2PROM_SCK_CLR();
        halMcuWaitUs(2);
    }
 // MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SDA_PORT, HAL_BOARD_IO_E2PROM_SDA_PIN,!ack);   /*当ack＝1时拉低数据线*/
  //  HAL_E2PROM_SDA_SET();
    
    HAL_E2PROM_SCK_SET();                            /*clk #9 for ack*/
    halMcuWaitUs(1);                  /*pulswith approx 1 us */
    HAL_E2PROM_SCK_CLR();
    HAL_E2PROM_SDA_SET();
  
    return res;
}
static void M3_I2cStart(void);
void M3_I2cStart(void)//在SCL保持高电平期间，SDA由高变低，为起始信号
{
    HAL_E2PROM_SCK_SET();  //SCL=1

    HAL_E2PROM_SDA_SET();  //SDA=1
    halMcuWaitUs(2);
    HAL_E2PROM_SDA_CLR();  //SDA=0
    halMcuWaitUs(2);

    HAL_E2PROM_SCK_CLR();  //SCL=0
}

//-------------------------------------------------------------------
static void M3_I2cStop(void);
void M3_I2cStop(void)//在SCL保持高电平期间，SDA由低变高，为停止信号
{    
      
   HAL_E2PROM_SCK_SET();  //SCL=1

    HAL_E2PROM_SDA_CLR();  //SDA=0
    halMcuWaitUs(2);

    HAL_E2PROM_SDA_SET();  //SDA=1
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCK_CLR();  //SCL=0
}

//-------------------------------------------------------------------
static void M3_I2cAck(void);
void  M3_I2cAck(void)
{
    HAL_E2PROM_SCK_SET();     
    HAL_E2PROM_SDA_SET();
    HAL_E2PROM_SCK_CLR();
}

//-------------------------------------------------------------------
static void M3_I2cNAck(void);
void M3_I2cNAck(void)
{
    HAL_E2PROM_SCK_SET();
    HAL_E2PROM_SDA_CLR(); 
    HAL_E2PROM_SCK_CLR();
}


static void M3_WriteEEPROM(uint8 addr, uint8 ch);
void M3_WriteEEPROM(uint8 addr, uint8 ch)//往EEPROM里写数据（先写地址，在写数据）
{
    M3_I2cStart();
    
    M3_I2cWrite(0xD2);//写器件地址，对于AT24C02来说，高四位固定为1010,后三位没有意义，最后一位为0时写，为1时读
    M3_I2cAck();
    
    M3_I2cWrite(addr);
    M3_I2cAck();
    
    M3_I2cWrite(ch); //data
    M3_I2cAck();
    
    M3_I2cStop();
}
static uint8 M3_ReadEEPROM(uint8 addr);
uint8 M3_ReadEEPROM(uint8 addr)
{
    uint8 res; 
    
    M3_I2cStart();    
    M3_I2cWrite(0xD2); //器件设置为写状态
    M3_I2cAck();     
    M3_I2cWrite(addr);
    M3_I2cAck();
    
    M3_I2cStart();    
    M3_I2cWrite(0xD3); //器件设置为读状态
    M3_I2cAck();  
    
    res = M3_I2cRead();  
    M3_I2cNAck();
    M3_I2cStop();

    return res;
}
static void M3_Init(void);
void M3_Init(void)//初始化 SDA=1、SCL=1
{
    static char initflg=0;
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCL_PORT, HAL_BOARD_IO_E2PROM_SCL_PIN,1);
    if(initflg==0)
        {
             M3_WriteEEPROM(0x16,0x18);
            initflg=1 ;           
        }
}

/*******************************************************************
 * 名称      ITG_Pooling
 * 功能      读取itg_3200的X,Y Z的角速度
 * 入口参数  (int16* X_Value, int16* Y_Value, int16* Z_Value  )
            *X_Value：X轴的16位角速度存放地址
            *Y_Value：Y轴的16位角速度存放地址
            *Z_Value：Z轴的16位角速度存放地址
            角速度值为16位有符号数，单位为 度/S
 * 出口参数  无  
 *****************************************************************/
void  ITG_Pooling(int16* X_Value, int16* Y_Value, int16* Z_Value  )
{
    int8 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
    int8 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;
    int16 num_x1=0,num_y1=0,num_z1=0;
    int16 num_x=0,num_y=0,num_z=0;
    uint8 i = 0;
    M3_Init();
    for(i=0;i<10;i++)
   {
        while (!(M3_ReadEEPROM(0x1A) & 0x01)); 
        BMA_XM_Value = M3_ReadEEPROM(0x1D); 
        halMcuWaitUs(5);
        BMA_XL_Value = M3_ReadEEPROM(0x1E); 
        halMcuWaitUs(5);
        BMA_YM_Value = M3_ReadEEPROM(0x1F);
        halMcuWaitUs(5);
        BMA_YL_Value = M3_ReadEEPROM(0x20);
        halMcuWaitUs(5);       
        BMA_ZM_Value = M3_ReadEEPROM(0x21);
        halMcuWaitUs(5);
        BMA_ZL_Value = M3_ReadEEPROM(0x22);
        halMcuWaitUs(5);
        num_x1 = (BMA_XM_Value << 8) | BMA_XL_Value;
        num_y1 = (BMA_YM_Value << 8) | BMA_YL_Value;
        num_z1 = (BMA_ZM_Value << 8) | BMA_ZL_Value;
        num_x = num_x + num_x1;
        num_y = num_y + num_y1;
        num_z = num_z + num_z1;
   }
    // 换算成 度/S
    *X_Value = (int16)(num_x/10/14.375);
    *Y_Value = (int16)(num_y/10/14.375);
    *Z_Value = (int16)(num_z/10/14.375);
}





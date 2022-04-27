//-------------------------------------------------------------------
// Filename: dma_bma.c
// Description:  dma_bma module library (I2C)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "dma_bma.h"

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
void M3_I2cStart(void)//在SCL保持高电平期间，SDA由高变低，为起始信号
{
    HAL_E2PROM_SCL_SET();  //SCL=1

    HAL_E2PROM_SDA_SET();  //SDA=1
    halMcuWaitUs(2);
    HAL_E2PROM_SDA_CLR();  //SDA=0
    halMcuWaitUs(2);

    HAL_E2PROM_SCL_CLR();  //SCL=0
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
void M3_I2cStop(void)//在SCL保持高电平期间，SDA由低变高，为停止信号
{
    HAL_E2PROM_SCL_SET();  //SCL=1
    halMcuWaitUs(2);
    
    HAL_E2PROM_SDA_CLR();  //SDA=0
    halMcuWaitUs(2);

    HAL_E2PROM_SDA_SET();  //SDA=1
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCL_CLR();  //SCL=0
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
void M3_I2cAck(void)//当SDA=0时产生应答信号
{
    HAL_E2PROM_SCL_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SDA_CLR();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCL_CLR();
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
void M3_I2cNack(void)//当SDA=1时产生非应答信号
{
    HAL_E2PROM_SCL_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SDA_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCL_CLR();
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
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
        HAL_E2PROM_SCL_SET();//当时钟线为高电平(SCL=1)时开始传送数据
        dat = dat << 1;
        halMcuWaitUs(2);
        HAL_E2PROM_SCL_CLR();//传送完后释放总线（SCL=0）
        halMcuWaitUs(2);
    }
}

//-------------------------------------------------------------------
uint8 M3_I2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_E2PROM_SDA_SET();//在读数据之前，把数据线拉高（datesheet上有介绍）,然后把数据线端口对CPU设置成输入端口
    HAL_E2PROM_SDA_DIR_IN();
    halMcuWaitUs(2);

    for (cnt = 0; cnt < 8; cnt++)
    {
        HAL_E2PROM_SCL_SET();
        res <<= 1;
        if (HAL_E2PROM_SDA_VAL())//cpu获取SDA端口上的值
        {
            res |= 0x01;
        }
        halMcuWaitUs(2);
        HAL_E2PROM_SCL_CLR();
        halMcuWaitUs(2);
    }
    HAL_E2PROM_SDA_DIR_OUT();//cpu获取完一个字节后，再将SDA端口设置成输出，且置高此端口
    return res;
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M3_Init(void)//初始化 SDA=1、SCL=1
{
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCL_PORT, HAL_BOARD_IO_E2PROM_SCL_PIN,1);
}


//-------------------------------------------------------------------
void M3_WriteEEPROM(uint8 addr, uint8 ch)//往EEPROM里写数据（先写地址，在写数据）
{
    M3_I2cStart();
    
    M3_I2cWrite(0x70);//写器件地址，对于AT24C02来说，高四位固定为1010,后三位没有意义，最后一位为0时写，为1时读
    M3_I2cAck();
    
    M3_I2cWrite(addr);
    M3_I2cAck();
    
    M3_I2cWrite(ch); //data
    M3_I2cAck();
    
    M3_I2cStop();
    halMcuWaitMs(5);
}

//-------------------------------------------------------------------
uint8 M3_ReadEEPROM(uint8 addr)
{
    uint8 res;
    
    M3_I2cStart();
    
    M3_I2cWrite(0x70); //器件设置为写状态
    M3_I2cAck();
    
    M3_I2cWrite(addr);
    M3_I2cAck();

    
    M3_I2cStart();
    
    M3_I2cWrite(0x71); //器件设置为写状态
    M3_I2cAck();
      
    res = M3_I2cRead();
    
    M3_I2cNack();
    M3_I2cStop();

    return res;
}

uint16 BMA_X_Value,BMA_Y_Value,BMA_Z_Value;

void BMA_Pooling(void)
{
    uint16 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
    uint16 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;
    
    BMA_XL_Value = M3_ReadEEPROM(0x02);
    BMA_XM_Value = M3_ReadEEPROM(0x03);
    BMA_YL_Value = M3_ReadEEPROM(0x04);
    BMA_YM_Value = M3_ReadEEPROM(0x05);
    BMA_ZL_Value = M3_ReadEEPROM(0x06);
    BMA_ZM_Value = M3_ReadEEPROM(0x07);
    
    BMA_X_Value = (BMA_XM_Value << 2) + ((BMA_XL_Value&0xc0)>>6);
    BMA_Y_Value = (BMA_YM_Value << 2) + ((BMA_YL_Value&0xc0)>>6);
    BMA_Z_Value = (BMA_ZM_Value << 2) + ((BMA_ZL_Value&0xc0)>>6);
}

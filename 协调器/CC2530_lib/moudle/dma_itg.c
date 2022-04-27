#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "dma_itg.h"



//--------- write a byte and checks the ack signal----------------- 
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
void M3_I2cAck(void)
{
    HAL_E2PROM_SCK_SET();
    HAL_E2PROM_SDA_SET();
    HAL_E2PROM_SCK_CLR();
}

//-------------------------------------------------------------------
void M3_I2cNAck(void)
{
    HAL_E2PROM_SCK_SET();
    HAL_E2PROM_SDA_CLR(); 
    HAL_E2PROM_SCK_CLR();
}

void M3_Init(void)//初始化 SDA=1、SCL=1
{
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCK_PORT, HAL_BOARD_IO_E2PROM_SCK_PIN,1);
}

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

uint16 BMA_X_Value,BMA_Y_Value,BMA_Z_Value;
uint8 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
uint8 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;

void BMA_Pooling(void)
{
 //   uint16 BMA_XM_Value,BMA_YM_Value,BMA_ZM_Value;
 //   uint16 BMA_XL_Value,BMA_YL_Value,BMA_ZL_Value;
    uint16 num_x1=0,num_y1=0,num_z1=0;
    uint16 num_x=0,num_y=0,num_z=0;
    uint8 i = 0;
    
    for(i=0;i<10;i++)
   {
        while (!(M3_ReadEEPROM(0x1A) & 0x01));
        
        BMA_XM_Value = M3_ReadEEPROM(0x1D); //1d  1b
        halMcuWaitUs(5);
        BMA_XL_Value = M3_ReadEEPROM(0x1E); //1e  1c
        halMcuWaitUs(5);
        BMA_YM_Value = M3_ReadEEPROM(0x1F);
        halMcuWaitUs(5);
        BMA_YL_Value = M3_ReadEEPROM(0x20);
        halMcuWaitUs(5);       
        BMA_ZM_Value = M3_ReadEEPROM(0x21);
        halMcuWaitUs(5);
        BMA_ZL_Value = M3_ReadEEPROM(0x22);
        halMcuWaitUs(5);
        
    
        num_x1 = (BMA_XM_Value << 8) + BMA_XL_Value;
        num_y1 = (BMA_YM_Value << 8) + BMA_YL_Value;
        num_z1 = (BMA_ZM_Value << 8) + BMA_ZL_Value;
        
        num_x = num_x + num_x1;
        num_y = num_y + num_y1;
        num_z = num_z + num_z1;
   }
    BMA_X_Value = num_x/10;
    BMA_Y_Value = num_y/10;
    BMA_Z_Value = num_z/10;
    
    halMcuWaitMs(1);
}






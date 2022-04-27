#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "dma_itg.h"



//--------- write a byte and checks the ack signal----------------- 
void M3_I2cWrite(uint8 dat)//�ڴ�������ʱ�������߱����ȶ�����������
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
        HAL_E2PROM_SCK_SET();//��ʱ����Ϊ�ߵ�ƽ(SCL=1)ʱ��ʼ��������
        dat = dat << 1;
        halMcuWaitUs(2);
        HAL_E2PROM_SCK_CLR();//��������ͷ����ߣ�SCL=0��
        halMcuWaitUs(2);
    }
    
}

//--------- read a byte and checks the ack signal----------------- 
uint8 M3_I2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_E2PROM_SDA_SET();//�ڶ�����֮ǰ�������������ߣ�datesheet���н��ܣ�,Ȼ��������߶˿ڶ�CPU���ó�����˿�
    HAL_E2PROM_SDA_DIR_IN();
    halMcuWaitUs(2);

    for (cnt = 0; cnt < 8; cnt++)
    {
        res <<= 1;
        if (HAL_E2PROM_SDA_VAL())//cpu��ȡSDA�˿��ϵ�ֵ
        {
            res |= 0x01;
        }
        HAL_E2PROM_SCK_SET();
        halMcuWaitUs(2);
        HAL_E2PROM_SCK_CLR();
        halMcuWaitUs(2);
    }
 // MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SDA_PORT, HAL_BOARD_IO_E2PROM_SDA_PIN,!ack);   /*��ack��1ʱ����������*/
  //  HAL_E2PROM_SDA_SET();
    
    HAL_E2PROM_SCK_SET();                            /*clk #9 for ack*/
    halMcuWaitUs(1);                  /*pulswith approx 1 us */
    HAL_E2PROM_SCK_CLR();
    HAL_E2PROM_SDA_SET();
  
    return res;
}

void M3_I2cStart(void)//��SCL���ָߵ�ƽ�ڼ䣬SDA�ɸ߱�ͣ�Ϊ��ʼ�ź�
{
    HAL_E2PROM_SCK_SET();  //SCL=1

    HAL_E2PROM_SDA_SET();  //SDA=1
    halMcuWaitUs(2);
    HAL_E2PROM_SDA_CLR();  //SDA=0
    halMcuWaitUs(2);

    HAL_E2PROM_SCK_CLR();  //SCL=0
}

//-------------------------------------------------------------------
void M3_I2cStop(void)//��SCL���ָߵ�ƽ�ڼ䣬SDA�ɵͱ�ߣ�Ϊֹͣ�ź�
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

void M3_Init(void)//��ʼ�� SDA=1��SCL=1
{
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCK_PORT, HAL_BOARD_IO_E2PROM_SCK_PIN,1);
}

void M3_WriteEEPROM(uint8 addr, uint8 ch)//��EEPROM��д���ݣ���д��ַ����д���ݣ�
{
    M3_I2cStart();
    
    M3_I2cWrite(0xD2);//д������ַ������AT24C02��˵������λ�̶�Ϊ1010,����λû�����壬���һλΪ0ʱд��Ϊ1ʱ��
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
    M3_I2cWrite(0xD2); //��������Ϊд״̬
    M3_I2cAck();     
    M3_I2cWrite(addr);
    M3_I2cAck();
    
    M3_I2cStart();    
    M3_I2cWrite(0xD3); //��������Ϊ��״̬
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






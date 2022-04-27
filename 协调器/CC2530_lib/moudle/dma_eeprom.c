//-------------------------------------------------------------------
// Filename: dma_eeprom.c
// Description: dma_eeprom module library (I2C)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "dma_eeprom.h"

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
void M3_I2cStart(void)//��SCL���ָߵ�ƽ�ڼ䣬SDA�ɸ߱�ͣ�Ϊ��ʼ�ź�
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
void M3_I2cStop(void)//��SCL���ָߵ�ƽ�ڼ䣬SDA�ɵͱ�ߣ�Ϊֹͣ�ź�
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
void M3_I2cAck(void)//��SDA=0ʱ����Ӧ���ź�
{
    HAL_E2PROM_SCL_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SDA_CLR();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCL_CLR();
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
void M3_I2cNack(void)//��SDA=1ʱ������Ӧ���ź�
{
    HAL_E2PROM_SCL_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SDA_SET();
    halMcuWaitUs(2);
    
    HAL_E2PROM_SCL_CLR();
    halMcuWaitUs(2);
}

//-------------------------------------------------------------------
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
        HAL_E2PROM_SCL_SET();//��ʱ����Ϊ�ߵ�ƽ(SCL=1)ʱ��ʼ��������
        dat = dat << 1;
        halMcuWaitUs(2);
        HAL_E2PROM_SCL_CLR();//��������ͷ����ߣ�SCL=0��
        halMcuWaitUs(2);
    }
}

//-------------------------------------------------------------------
uint8 M3_I2cRead(void)
{
    uint8 res = 0;
    uint8 cnt;

    HAL_E2PROM_SDA_SET();//�ڶ�����֮ǰ�������������ߣ�datesheet���н��ܣ�,Ȼ��������߶˿ڶ�CPU���ó�����˿�
    HAL_E2PROM_SDA_DIR_IN();
    halMcuWaitUs(2);

    for (cnt = 0; cnt < 8; cnt++)
    {
        HAL_E2PROM_SCL_SET();
        res <<= 1;
        if (HAL_E2PROM_SDA_VAL())//cpu��ȡSDA�˿��ϵ�ֵ
        {
            res |= 0x01;
        }
        halMcuWaitUs(2);
        HAL_E2PROM_SCL_CLR();
        halMcuWaitUs(2);
    }
    HAL_E2PROM_SDA_DIR_OUT();//cpu��ȡ��һ���ֽں��ٽ�SDA�˿����ó���������øߴ˶˿�
    return res;
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M3_Init(void)//��ʼ�� SDA=1��SCL=1
{
    HAL_E2PROM_SDA_DIR_OUT();
    MCU_IO_OUTPUT(HAL_BOARD_IO_E2PROM_SCL_PORT, HAL_BOARD_IO_E2PROM_SCL_PIN,1);
}


//-------------------------------------------------------------------
void M3_WriteEEPROM(uint8 addr, uint8 ch)//��EEPROM��д���ݣ���д��ַ����д���ݣ�
{
    M3_I2cStart();
    
    M3_I2cWrite(0xa0);//д������ַ������AT24C02��˵������λ�̶�Ϊ1010,����λû�����壬���һλΪ0ʱд��Ϊ1ʱ��
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
    
    M3_I2cWrite(0xa0); //��������Ϊд״̬
    M3_I2cAck();
    
    M3_I2cWrite(addr);
    M3_I2cAck();

    
    M3_I2cStart();
    
    M3_I2cWrite(0xa1); //��������Ϊд״̬
    M3_I2cAck();
      
    res = M3_I2cRead();
    
    M3_I2cNack();
    M3_I2cStop();

    return res;
}


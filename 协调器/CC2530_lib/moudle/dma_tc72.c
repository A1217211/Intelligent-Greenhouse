#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
#include "dma_tc72.h"

uint16 TC72_Value = 0;

void Write_Tc72(uint8 ch)
{
    uint8 i, cmd;

    cmd = ch;
    for (i = 0; i < 8; i++)
    {
        HAL_TC72_SCL_CLR();
        if (cmd &0x80)
        {
            HAL_TC72_SDI_SET();
        }
        else
        {
            HAL_TC72_SDI_CLR();
        }
        cmd <<= 1;
        HAL_TC72_SCL_SET();
    }
    HAL_TC72_SDI_SET();
}

uint16 Read_Tc72(void)
{
    uint8 i, ret;

    HAL_TC72_SDI_CLR();
    for (i = 0; i < 8; i++)
    {
        HAL_TC72_SCL_CLR();
        ret <<= 1;
        HAL_TC72_SCL_SET();
        if (HAL_TC72_SDO_VAL())
            ret |= 0x01;
    }
    HAL_TC72_SDI_SET();
    return ret;
}



//-------------------------------------------------------------------
void TC72_Start(void)
{
    HAL_TC72_SSS_SET();
    Write_Tc72(0x80);
    Write_Tc72(0x15);
    HAL_TC72_SSS_CLR();
}

//-------------------------------------------------------------------
void TC72_Read(void)
{
    uint16 m,n;
    HAL_TC72_SSS_SET();
    
    Write_Tc72(0x02);
    halMcuWaitUs(200);
    m = Read_Tc72();
    halMcuWaitUs(200);
    
    Write_Tc72(0x01);
    halMcuWaitUs(200);
    n = Read_Tc72();
    halMcuWaitUs(200);
    
    TC72_Value = (m<<8)+n;
    HAL_TC72_SSS_CLR();
}

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void TC72_Init(void)
{
    MCU_IO_INPUT(HAL_BOARD_IO_TC72_SDO_PORT, HAL_BOARD_IO_TC72_SDO_PIN, MCU_IO_PULLUP);
    MCU_IO_OUTPUT(HAL_BOARD_IO_TC72_SDI_PORT, HAL_BOARD_IO_TC72_SDI_PIN,1);
    MCU_IO_OUTPUT(HAL_BOARD_IO_TC72_SCL_PORT, HAL_BOARD_IO_TC72_SCL_PIN,1);
    MCU_IO_OUTPUT(HAL_BOARD_IO_TC72_SSS_PORT, HAL_BOARD_IO_TC72_SSS_PIN,1);
}

//-------------------------------------------------------------------
uint16 TC72_ReadTemp(void)
{
    TC72_Start();
    halMcuWaitMs(2);
    TC72_Read();
    return TC72_Value;
}



//-------------------------------------------------------------------
// Filename: M190.c
// Description: hal M190 library 可變電阻(A/D)
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "hal_adc.h"
#include "dma_tgs2602.h"

//-------------------------------------------------------------------
// @fn      M190_Init
// @brief    Set up port M190 ad
// @return  none
//-------------------------------------------------------------------
void dma_tgs2602_Init(void)
{
    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_ADC_PORT, HAL_BOARD_IO_ADC_CH);

}

//-------------------------------------------------------------------
// @fn      M190_GetValue
// @brief   Get this M190 module value
// @param   none
// @return  none
//-------------------------------------------------------------------
uint16 dma_tgs2602_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, HAL_BOARD_IO_ADC_CH);
    return adcValue;
}

//-------------------------------------------------------------------
// Filename: dma_m4.c
// Description: dma_m4 library (A/D)
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
#include "dma_m4.h"

//-------------------------------------------------------------------
// @fn      dma_m4_Init
// @brief    Set up port dma_m4 ad
// @return  none
//-------------------------------------------------------------------
void dma_m4_Init(void)
{
    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_ADC_PORT, HAL_BOARD_IO_ADC_CH);

}

//-------------------------------------------------------------------
// @fn      dma_m4_GetValue
// @brief   Get this dma_m4 module value
// @param   none
// @return  none
//-------------------------------------------------------------------
uint16 dma_m4_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, HAL_BOARD_IO_ADC_CH);
    return adcValue;
}

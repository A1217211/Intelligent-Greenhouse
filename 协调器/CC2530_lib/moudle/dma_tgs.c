//-------------------------------------------------------------------
// Filename: dma_tgs.c
// Description: dma_tgs library  (A/D)
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
#include "dma_tgs.h"

//-------------------------------------------------------------------
// @fn      dma_tgs_Init
// @brief    Set up port dma_tgs ad
// @return  none
//-------------------------------------------------------------------
void dma_tgs_Init(void)
{
    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_ADC_PORT, HAL_BOARD_IO_ADC_CH);

}

//-------------------------------------------------------------------
// @fn      dma_tgs_GetValue
// @brief   Get this dma_tgs module value
// @param   none
// @return  none
//-------------------------------------------------------------------
uint16 dma_tgs_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, HAL_BOARD_IO_ADC_CH);
    return adcValue;
}

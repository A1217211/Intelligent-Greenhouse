//-------------------------------------------------------------------
// Filename: dma_ad590.c
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
#include "dma_ad590.h"

//-------------------------------------------------------------------
// @fn       dma_ad590_Init
// @brief    Set up port ad590 ad
// @return  none
//-------------------------------------------------------------------
void dma_ad590_Init(void)
{
    // Analog input
    MCU_IO_PERIPHERAL(HAL_BOARD_IO_ADC_PORT, HAL_BOARD_IO_ADC_CH);

}

//-------------------------------------------------------------------
// @fn      dma_ad590_GetValue
// @brief   Get this ad590 module value
// @param   none
// @return  none
//-------------------------------------------------------------------
uint16 dma_ad590_GetValue(void)
{
    uint16 adcValue;
    adcValue = adcSampleSingle(ADC_REF_AVDD, ADC_12_BIT, HAL_BOARD_IO_ADC_CH);
    return adcValue;
}

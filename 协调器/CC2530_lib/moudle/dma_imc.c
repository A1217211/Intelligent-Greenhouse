//-------------------------------------------------------------------
// Filename: dma_imc.c
// Description: dma_imc library
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_digio.h"
#include "dma_imc.h"

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uint16 M1_ImcValue;

static const digioConfig pinM1_Imc = 
{
    HAL_BOARD_IO_INT_PORT, HAL_BOARD_IO_INT_PIN, BV(HAL_BOARD_IO_INT_PIN), HAL_DIGIO_INPUT, 0
};

//-------------------------------------------------------------------
// LOCAL FUNCTIONS
//-------------------------------------------------------------------
/*static void M1_ImcISR(void)
{
    M1_ImcValue++;
}*/

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void M1_Init(void)
{
    M1_ImcValue = 0;
    halDigioConfig(&pinM1_Imc);
    MCU_IO_INPUT(HAL_BOARD_IO_INT_PORT, HAL_BOARD_IO_INT_PIN, MCU_IO_TRISTATE); //MCU_IO_TRISTATE MCU_IO_PULLUP
    halDigioIntSetEdge(&pinM1_Imc, HAL_DIGIO_INT_RISING_EDGE);
  //  halDigioIntConnect(&pinM1_Imc, &M1_ImcISR);
}

//-------------------------------------------------------------------
uint16 M1_GetValue(void)
{
   M1_ImcValue = HAL_INT_VAL(); 
   return M1_ImcValue;
}

//-------------------------------------------------------------------
void M1_Enable(void)
{
    M1_ImcValue = 0;
    halDigioIntEnable(&pinM1_Imc);
}

//-------------------------------------------------------------------
void M1_Disable(void)
{
    halDigioIntDisable(&pinM1_Imc);
}


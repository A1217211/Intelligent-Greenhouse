//-------------------------------------------------------------------
// Filename: hal_dc.c
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "dma_dc.h"

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn          halDcStart
// @brief       Turn DC on.
// @param       uint8 id - led to set
// @return      none
//-------------------------------------------------------------------
void halDcStart(uint8 dc_num)
{
    switch (dc_num)
    {
        case 1:
            HAL_DC_START_1();
            break;
        case 2:
            HAL_DC_START_2();
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------
// @fn          halDcStop
// @brief       Turn LED off.
// @param       uint8 id - led to clear
// @return      none
//-------------------------------------------------------------------
void halDcStop(uint8 dc_num)
{
    switch (dc_num)
    {
        case 1:
            HAL_DC_STOP_1();
            break;
        case 2:
            HAL_DC_STOP_2();
            break;
        default:
            break;
    }
}


//-------------------------------------------------------------------

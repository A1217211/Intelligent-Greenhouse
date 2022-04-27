#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_mcu.h"
#include "hal_digio.h"
#include "hal_int.h"
#include "hal_board.h"
#include "hal_uart.h"

void halBoardInit(void)
{
    halMcuInit();
    halUartInit(115200);
    halIntOn();
}




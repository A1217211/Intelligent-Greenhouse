//-------------------------------------------------------------------
// Filename: hal_uart.c
// Description: This file contains the interface to the H/W UART driver.
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_board.h"
#include "hal_uart1.h"

//-------------------------------------------------------------------
// MACROS
//-------------------------------------------------------------------
#define HAL_UART1_ISR_RX_AVAIL() \
(uart1Cfg.rxTail >= uart1Cfg.rxHead) ? \
(uart1Cfg.rxTail - uart1Cfg.rxHead) : \
(HAL_UART1_ISR_RX_MAX - uart1Cfg.rxHead + uart1Cfg.rxTail)

#define HAL_UART1_ISR_TX_AVAIL() \
(uart1Cfg.txHead > uart1Cfg.txTail) ? \
(uart1Cfg.txHead - uart1Cfg.txTail - 1) : \
(HAL_UART1_ISR_TX_MAX - uart1Cfg.txTail + uart1Cfg.txHead - 1)

//-------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------
// U0CSR - USART Control and Status Register.
#define CSR_MODE                   0x80
#define CSR_RE                     0x40
#define CSR_SLAVE                  0x20
#define CSR_FE                     0x10
#define CSR_ERR                    0x08
#define CSR_RX_BYTE                0x04
#define CSR_TX_BYTE                0x02
#define CSR_ACTIVE                 0x01

// U0UCR - USART UART Control Register.
#define UCR_FLUSH                  0x80
#define UCR_FLOW                   0x40
#define UCR_D9                     0x20
#define UCR_BIT9                   0x10
#define UCR_PARITY                 0x08
#define UCR_SPB                    0x04
#define UCR_STOP                   0x02
#define UCR_START                  0x01

#define UTX1IE                     0x08
#define U1RX_TX                    0xC0

#define HAL_UART1_PERCFG_BIT       0x02

#define HAL_UART1_ISR_RX_MAX        128
#define HAL_UART1_ISR_TX_MAX        HAL_UART1_ISR_RX_MAX

typedef struct
{
    uint8 rxBuf[HAL_UART1_ISR_RX_MAX];
    uint8 rxHead;
    volatile uint8 rxTail;
    uint8 rxShdw;

    uint8 txBuf[HAL_UART1_ISR_TX_MAX];
    volatile uint8 txHead;
    uint8 txTail;
    uint8 txMT;
} uartCfg_t;

//-------------------------------------------------------------------
// LOCAL VARIABLES
//-------------------------------------------------------------------
uartCfg_t uart1Cfg;

//-------------------------------------------------------------------
// @fn      halUartInit
// @brief   Initialize the UART
// @param   none
// @return  none
//-------------------------------------------------------------------
void halUart1Init(uint32 baud)
{
    // UART Configuration
    PERCFG |= HAL_UART1_PERCFG_BIT; // 设置 UART1 I/O 位置在P1口上.

    P1SEL |= U1RX_TX;              // 使能 UART1 的Tx（P1.6）和Rx （P1.7）外部设备功能引脚.
   // ADCCFG &= ~U0RX_TX;          // Make sure ADC doesnt use this.
    U1CSR = CSR_MODE;              // UART 模式.
    U1UCR = UCR_FLUSH;             // 清除单元.

    // Only supporting subset of baudrate for code size - other is possible.
    // 38400
    //U1BAUD = 59;
    //U1GCR = 10;
    //9600
    //U1BAUD = 59;
    //U1GCR = 8;
    switch(baud)
    {
        case 2400 : 
            U1BAUD = 59; U1GCR = 6; break;
        case 4800 :
            U1BAUD = 59; U1GCR = 7; break;
        case 9600 : 
            U1BAUD = 59; U1GCR = 8; break;
        case 14400 :
            U1BAUD = 216; U1GCR = 8; break;
        case 19200 : 
            U1BAUD = 59; U1GCR = 9; break;
        case 28800 :
            U1BAUD = 216; U1GCR = 9; break;
        case 38400 : 
            U1BAUD = 59; U1GCR = 10; break;
        case 57600 :
            U1BAUD = 216; U1GCR = 10; break;
        case 76800 : 
            U1BAUD = 59; U1GCR = 11; break;
        case 115200 :
            U1BAUD = 216; U1GCR = 11; break;
        case 230400 : 
            U1BAUD = 216; U1GCR = 12; break;
        default :
            U1BAUD = 59; U1GCR = 10; break;            
    }

    // 8 bits/char; no parity; 1 stop bit; stop bit hi.
    U1UCR = UCR_STOP;  //停止位为高电平（1）
    U1CSR |= CSR_RE;   //UART1 接收器使能
    URX1IE = 1;        //UART1 接收中断使能
    U1DBUF = 0;        //数据缓冲器清0

    uart1Cfg.rxHead = 0;
    uart1Cfg.rxTail = 0;
    uart1Cfg.txHead = 0;
    uart1Cfg.txTail = 0;
    uart1Cfg.rxShdw = 0;
    uart1Cfg.txMT = 0;

}

//-------------------------------------------------------------------
// @fn      halUartRead
// @brief   Read a buffer from the UART
// @param   buf  - valid data buffer at least 'len' bytes in size
//          len  - max length number of bytes to copy to 'buf'
// @return  length of buffer that was read
//-------------------------------------------------------------------
uint16 halUart1Read(uint8 *buf, uint16 len)
{
    uint16 cnt = 0;

    while ((uart1Cfg.rxHead != uart1Cfg.rxTail) && (cnt < len))
    {
        *buf++ = uart1Cfg.rxBuf[uart1Cfg.rxHead++];
        if (uart1Cfg.rxHead >= HAL_UART1_ISR_RX_MAX)
        {
            uart1Cfg.rxHead = 0;
        }
        cnt++;
    }

    return cnt;
}

//-------------------------------------------------------------------
// @fn      halUartWrite
// @brief   Write a buffer to the UART.
// @param   buf - pointer to the buffer that will be written, not freed
//          len - length of
// @return  length of the buffer that was sent
//-------------------------------------------------------------------
uint16 halUart1Write(uint8 *buf, uint16 len)
{
    uint16 cnt;

    // Accept "all-or-none" on write request.
    if (HAL_UART1_ISR_TX_AVAIL() < len)
    {
        return 0;
    }

    for (cnt = 0; cnt < len; cnt++)
    {
        uart1Cfg.txBuf[uart1Cfg.txTail] =  *buf++;
        uart1Cfg.txMT = 0;

        if (uart1Cfg.txTail >= HAL_UART1_ISR_TX_MAX - 1)
        {
            uart1Cfg.txTail = 0;
        }
        else
        {
            uart1Cfg.txTail++;
        }

        // Keep re-enabling ISR as it might be keeping up with this loop due to other ints.
        IEN2 |= UTX1IE;
    }

    return cnt;
}

//-------------------------------------------------------------------
// @fn      halUartRxLen()
// @brief   Calculate Rx Buffer length - the number of bytes in the buffer.
// @param   none
// @return  length of current Rx Buffer
//-------------------------------------------------------------------
uint16 halUart1RxLen(void)
{
    return HAL_UART1_ISR_RX_AVAIL();
}

//-------------------------------------------------------------------
// @fn      halUartSuspend
// @brief   Suspend UART hardware before entering PM mode 1, 2 or 3.
// @param   None
// @return  None
//-------------------------------------------------------------------
void halUart1Suspend(void)
{
    U1CSR &= ~CSR_RE;
}

//-------------------------------------------------------------------
// @fn      halUartResume
// @brief   Resume UART hardware after exiting PM mode 1, 2 or 3.
// @param   None
// @return  None
//-------------------------------------------------------------------
void halUart1Resume(void)
{
    U1UCR |= UCR_FLUSH;
    U1CSR |= CSR_RE;
}

//-------------------------------------------------------------------
// @fn      halUartRxIsr
// @brief   UART Receive Interrupt
// @param   None
// @return  None
//-------------------------------------------------------------------
HAL_ISR_FUNCTION(halUart1RxIsr, URX1_VECTOR)
{
    uint8 tmp = U1DBUF;
    uart1Cfg.rxBuf[uart1Cfg.rxTail] = tmp;

    // Re-sync the shadow on any 1st byte received.
    if (uart1Cfg.rxHead == uart1Cfg.rxTail)
    {
        uart1Cfg.rxShdw = ST0;
    }

    if (++uart1Cfg.rxTail >= HAL_UART1_ISR_RX_MAX)
    {
        uart1Cfg.rxTail = 0;
    }
}

//-------------------------------------------------------------------
// @fn      halUartTxIsr
// @brief   UART Transmit Interrupt
// @param   None
// @return  None
//-------------------------------------------------------------------
HAL_ISR_FUNCTION(halUart1TxIsr, UTX1_VECTOR)
{
    if (uart1Cfg.txHead == uart1Cfg.txTail)
    {
        IEN2 &= ~UTX1IE;
        uart1Cfg.txMT = 1;
    }
    else
    {
        UTX1IF = 0;
        U1DBUF = uart1Cfg.txBuf[uart1Cfg.txHead++];

        if (uart1Cfg.txHead >= HAL_UART1_ISR_TX_MAX)
        {
            uart1Cfg.txHead = 0;
        }
    }
}

//-------------------------------------------------------------------
// Filename: hal_rf.c
// Description: CC2530 radio interface.
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include "hal_defs.h"
#include "hal_mcu.h"
#include "hal_int.h"
#include "util.h"
#include "hal_rf.h"

//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
// CC2530 RSSI Offset
#define RSSI_OFFSET                  73

// Various radio settings
#define AUTO_ACK                   0x20
#define AUTO_CRC                   0x40

// TXPOWER values
#define CC2530_TXPOWER_MIN_3_DBM   0xB5 
#define CC2530_TXPOWER_0_DBM       0xD5
#define CC2530_TXPOWER_4_DBM       0xF5

#define HAL_RF_TXPOWER_MIN_3_DBM  0
#define HAL_RF_TXPOWER_0_DBM      1
#define HAL_RF_TXPOWER_4_DBM	  2

// RF interrupt flags
#define IRQ_TXDONE                 0x02
#define IRQ_RXPKTDONE              0x40

// Selected strobes
#define ISRXON()                st(RFST = 0xE3;)
#define ISTXON()                st(RFST = 0xE9;)
#define ISTXONCCA()             st(RFST = 0xEA;)
#define ISRFOFF()               st(RFST = 0xEF;)
#define ISFLUSHRX()             st(RFST = 0xEC;)
#define ISFLUSHTX()             st(RFST = 0xEE;)



//-------------------------------------------------------------------
// GLOBAL DATA
//-------------------------------------------------------------------

static const menuItem_t pPowerSettings[] = 
{
    "-3dBm", HAL_RF_TXPOWER_MIN_3_DBM, "0dBm", HAL_RF_TXPOWER_0_DBM, "4dBm", HAL_RF_TXPOWER_4_DBM
};

const menu_t powerMenu = 
{
    pPowerSettings, N_ITEMS(pPowerSettings)
};

//-------------------------------------------------------------------
// LOCAL DATA
//-------------------------------------------------------------------

static ISR_FUNC_PTR pfISR = NULL;

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn      halRfInit
// @brief   Power up, sets default tuning settings, enables autoack, enables random generator.
// @param   none
// @return  SUCCESS always (for interface compatibility)
//-------------------------------------------------------------------
uint8 halRfInit(void)
{
    // Enable auto ack and auto crc
    FRMCTRL0 |= (AUTO_ACK | AUTO_CRC);

    // Write non-default register values
    // Tuning settings
    TXPOWER = CC2530_TXPOWER_0_DBM; // TX output power ~0 dBm

    // Recommended RX settings
    TXFILTCFG = 0x09;
    AGCCTRL1 = 0x15;
    FSCAL1 = 0x00;

    // Enable random generator -> Not implemented yet

    // Enable CC2591 with High Gain Mode -> Not implemented yet

    // Enable RX interrupt
    halRfEnableRxInterrupt();

    return SUCCESS;
}

//-------------------------------------------------------------------
// @fn      halRfGetChipId
// @brief   Get chip id
// @param   none
// @return  uint8 - result
//-------------------------------------------------------------------
uint8 halRfGetChipId(void)
{
    return CHIPID;
}

//-------------------------------------------------------------------
// @fn      halRfGetChipVer
// @brief   Get chip version
// @param   none
// @return  uint8 - result
//-------------------------------------------------------------------
uint8 halRfGetChipVer(void)
{
    // return major revision (4 upper bits)
    return CHVER >> 4;
}

//-------------------------------------------------------------------
// @fn      halRfGetRssiOffset
// @brief   Return RSSI Offset
// @param   none
// @return  uint8 - RSSI offset
//-------------------------------------------------------------------
uint8 halRfGetRssiOffset(void)
{
    return RSSI_OFFSET;
}

//-------------------------------------------------------------------
// @fn      halRfSetChannel
// @brief   Set RF channel in the 2.4GHz band. The Channel must be in the range 11-26,
//          11= 2005 MHz, channel spacing 5 MHz.
// @param   channel - logical channel number
// @return  none
//-------------------------------------------------------------------
void halRfSetChannel(uint8 channel)
{
    FREQCTRL = (MIN_CHANNEL + (channel - MIN_CHANNEL) *CHANNEL_SPACING);
}

//-------------------------------------------------------------------
// @fn      halRfSetShortAddr
// @brief   Write short address to chip
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfSetShortAddr(uint16 shortAddr)
{
    SHORT_ADDR0 = LO_UINT16(shortAddr);
    SHORT_ADDR1 = HI_UINT16(shortAddr);
}

//-------------------------------------------------------------------
// @fn      halRfSetPanId
// @brief   Write PAN Id to chip
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfSetPanId(uint16 panId)
{
    PAN_ID0 = LO_UINT16(panId);
    PAN_ID1 = HI_UINT16(panId);
}


//-------------------------------------------------------------------
// @fn      halRfSetPower
// @brief   Set TX output power
// @param   uint8 power - power level: TXPOWER_MIN_4_DBM, TXPOWER_0_DBM, TXPOWER_4_DBM
// @return  uint8 - SUCCESS or FAILED
//-------------------------------------------------------------------
uint8 halRfSetTxPower(uint8 power)
{
    uint8 n;

    switch (power)
    {
        case HAL_RF_TXPOWER_MIN_3_DBM:
            n = CC2530_TXPOWER_MIN_3_DBM;
            break;
        case HAL_RF_TXPOWER_0_DBM:
            n = CC2530_TXPOWER_0_DBM;
            break;
        case HAL_RF_TXPOWER_4_DBM:
            n = CC2530_TXPOWER_4_DBM;
            break;

        default:
            return FAILED;
    }

    // Set TX power
    TXPOWER = n;

    return SUCCESS;
}

//-------------------------------------------------------------------
// @fn      halRfWriteTxBuf
// @brief   Write to TX buffer
// @param   uint8* pData - buffer to write
//          uint8 length - number of bytes
// @return  none
//-------------------------------------------------------------------
void halRfWriteTxBuf(uint8 *pData, uint8 length)
{
    uint8 i;

    ISFLUSHTX(); // Making sure that the TX FIFO is empty.

    RFIRQF1 = ~IRQ_TXDONE; // Clear TX done interrupt

    // Insert data
    for (i = 0; i < length; i++)
    {
        RFD = pData[i];
    }
}


//-------------------------------------------------------------------
// @fn      halRfAppendTxBuf
// @brief   Write to TX buffer
// @param   uint8* pData - buffer to write
//          uint8 length - number of bytes
// @return  none
//-------------------------------------------------------------------
void halRfAppendTxBuf(uint8 *pData, uint8 length)
{
    uint8 i;

    // Insert data
    for (i = 0; i < length; i++)
    {
        RFD = pData[i];
    }
}

//-------------------------------------------------------------------
// @fn      halRfReadRxBuf
// @brief   Read RX buffer
// @param   uint8* pData - data buffer. This must be allocated by caller.
//          uint8 length - number of bytes
// @return  none
//-------------------------------------------------------------------
void halRfReadRxBuf(uint8 *pData, uint8 length)
{
    // Read data
    while (length > 0)
    {
        *pData++ = RFD;
        length--;
    }
}


//-------------------------------------------------------------------
// @fn      halRfReadMemory
// @brief   Read RF device memory
// @param   uint16 addr - memory address
//          uint8* pData - data buffer. This must be allocated by caller.
//          uint8 length - number of bytes
// @return  Number of bytes read
//-------------------------------------------------------------------
uint8 halRfReadMemory(uint16 addr, uint8 *pData, uint8 length)
{
    return 0;
}


//-------------------------------------------------------------------
// @fn      halRfWriteMemory
// @brief   Write RF device memory
// @param   uint16 addr - memory address
//          uint8* pData - data buffer. This must be allocated by caller.
//          uint8 length - number of bytes
// @return  Number of bytes written
//-------------------------------------------------------------------
uint8 halRfWriteMemory(uint16 addr, uint8 *pData, uint8 length)
{
    return 0;
}

//-------------------------------------------------------------------
// @fn      halRfTransmit
// @brief   Transmit frame with Clear Channel Assessment.
// @param   none
// @return  uint8 - SUCCESS or FAILED
//-------------------------------------------------------------------
uint8 halRfTransmit(void)
{
    uint8 status;

    ISTXON(); // Sending

    // Waiting for transmission to finish
    while (!(RFIRQF1 &IRQ_TXDONE))
        ;

    RFIRQF1 = ~IRQ_TXDONE;
    status = SUCCESS;

    return status;
}

//-------------------------------------------------------------------
// @fn      halRfReceiveOn
// @brief   Turn receiver on
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfReceiveOn(void)
{
    ISFLUSHRX(); // Making sure that the TX FIFO is empty.
    ISRXON();
}

//-------------------------------------------------------------------
// @fn      halRfReceiveOff
// @brief   Turn receiver off
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfReceiveOff(void)
{
    ISRFOFF();
    ISFLUSHRX(); // Making sure that the TX FIFO is empty.
}


//-------------------------------------------------------------------
// @fn      halRfDisableRxInterrupt
// @brief   Clear and disable RX interrupt.
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfDisableRxInterrupt(void)
{
    // disable RXPKTDONE interrupt
    RFIRQM0 &= ~BV(6);
    // disable general RF interrupts
    IEN2 &= ~BV(0);
}


//-------------------------------------------------------------------
// @fn      halRfEnableRxInterrupt
// @brief   Enable RX interrupt.
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfEnableRxInterrupt(void)
{
    // enable RXPKTDONE interrupt
    RFIRQM0 |= BV(6);
    // enable general RF interrupts
    IEN2 |= BV(0);
}

//-------------------------------------------------------------------
// @fn      halRfRxInterruptConfig
// @brief   Configure RX interrupt.
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfRxInterruptConfig(ISR_FUNC_PTR pf)
{
    uint8 x;
    HAL_INT_LOCK(x);
    pfISR = pf;
    HAL_INT_UNLOCK(x);
}


//-------------------------------------------------------------------
// @fn      halRfWaitTransceiverReady
// @brief   Wait until the transciever is ready (SFD inactive).
// @param   none
// @return  none
//-------------------------------------------------------------------
void halRfWaitTransceiverReady(void)
{
    // Wait for SFD not active and TX_Active not active
    while (FSMSTAT1 &(BV(1) | BV(5)))
        ;
}


//-------------------------------------------------------------------
// @fn          rfIsr
// @brief       Interrupt service routine that handles RFPKTDONE interrupt.
// @param       none
// @return      none
//-------------------------------------------------------------------
HAL_ISR_FUNCTION(rfIsr, RF_VECTOR)
{
    uint8 x;

    HAL_INT_LOCK(x);

    if (RFIRQF0 &IRQ_RXPKTDONE)
    {
        if (pfISR)
        {
            (*pfISR)(); // Execute the custom ISR
        }
        S1CON = 0; // Clear general RF interrupt flag
        RFIRQF0 &= ~IRQ_RXPKTDONE; // Clear RXPKTDONE interrupt
    }
    HAL_INT_UNLOCK(x);
}

//-------------------------------------------------------------------

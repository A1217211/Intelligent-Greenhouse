//-------------------------------------------------------------------
// Filename:     hal_rf.h
// Description:  HAL radio interface header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
// Chip ID's
#define HAL_RF_CHIP_ID_CC1100               0x00
#define HAL_RF_CHIP_ID_CC1110               0x01
#define HAL_RF_CHIP_ID_CC1111               0x11
#define HAL_RF_CHIP_ID_CC2420               0x02
#define HAL_RF_CHIP_ID_CC2500               0x80
#define HAL_RF_CHIP_ID_CC2510               0x81
#define HAL_RF_CHIP_ID_CC2511               0x91
#define HAL_RF_CHIP_ID_CC2550               0x82
#define HAL_RF_CHIP_ID_CC2520               0x84
#define HAL_RF_CHIP_ID_CC2430               0x85
#define HAL_RF_CHIP_ID_CC2431               0x89
#define HAL_RF_CHIP_ID_CC2530               0xA5
#define HAL_RF_CHIP_ID_CC2531               0xB5
#define HAL_RF_CHIP_ID_CC2540               0x8D

// IEEE 802.15.4 defined constants (2.4 GHz logical channels)
#define MIN_CHANNEL 				        11    // 2405 MHz
#define MAX_CHANNEL                         26    // 2480 MHz
#define CHANNEL_SPACING                     5     // MHz

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
// Generic RF interface
uint8 halRfInit(void);
uint8 halRfSetTxPower(uint8 power);
uint8 halRfTransmit(void);

uint8 halRfGetChipId(void);
uint8 halRfGetChipVer(void);
uint8 halRfGetRssiOffset(void);

void halRfWriteTxBuf(uint8 *pData, uint8 length);
void halRfReadRxBuf(uint8 *pData, uint8 length);
void halRfWaitTransceiverReady(void);
uint8 halRfReadMemory(uint16 addr, uint8 *pData, uint8 length);
uint8 halRfWriteMemory(uint16 addr, uint8 *pData, uint8 length);

void halRfReceiveOn(void);
void halRfReceiveOff(void);
void halRfDisableRxInterrupt(void);
void halRfEnableRxInterrupt(void);
void halRfRxInterruptConfig(ISR_FUNC_PTR pfISR);

// IEEE 802.15.4 specific interface
void halRfSetChannel(uint8 channel);
void halRfSetShortAddr(uint16 shortAddr);
void halRfSetPanId(uint16 PanId);

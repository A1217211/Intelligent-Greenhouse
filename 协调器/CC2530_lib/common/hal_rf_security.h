//-------------------------------------------------------------------
// Filename:     hal_rf_security.h
// Description:  HAL radio interface header file for security operations.
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
// IEEE 802.15.4 security (AES-128)
#define KEY_LENGTH                     16
#define NONCE_LENGTH                   16

#define SECURITY_CONTROL               0x06     // MIC-64

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
// CCM security interface, IEEE 802.15.4
void halRfSecurityInit(uint8 *key, uint8 *nonceRx, uint8 *nonceTx);
uint8 halRfReadRxBufSecure(uint8 *pData, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
void halRfWriteTxBufSecure(uint8 *pData, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
void halRfIncNonceTx(void);

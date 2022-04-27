//-------------------------------------------------------------------
// Filename: basic_rf.h
// Description:  Basic RF library header file
//-------------------------------------------------------------------
//    The "Basic RF" library contains simple functions for packet transmission and
//    reception with the IEEE 802.15.4 compliant radio devices. The intention of this
//    library is to demonstrate how the radio devices are operated, and not to provide
//    a complete and fully-functional packet protocol. The protocol uses 802.15.4 MAC compliant data
//    and acknowledgment packets, however it contains only a small subset of  the
//    802.15.4 standard:
//    - Association, scanning nor beacons are not implemented
//    - No defined coordinator/device roles (peer-to-peer, all nodes are equal)
//    - Waits for the channel to become ready, but does not check CCA twice
//    (802.15.4 CSMA-CA)
//    - Does not retransmit packets
//
//    INSTRUCTIONS:
//    Startup:
//    1. Create a basicRfCfg_t structure, and initialize the members:
//    2. Call basicRfInit() to initialize the packet protocol.
//
//    Transmission:
//    1. Create a buffer with the payload to send
//    2. Call basicRfSendPacket()
//
//    Reception:
//    1. Check if a packet is ready to be received by highger layer with
//    basicRfPacketIsReady()
//    2. Call basicRfReceive() to receive the packet by higher layer
//
//    FRAME FORMATS:
//    Data packets (without security):
//    [Preambles (4)][SFD (1)][Length (1)][Frame control field (2)]
//    [Sequence number (1)][PAN ID (2)][Dest. address (2)][Source address (2)]
//    [Payload (Length - 2+1+2+2+2)][Frame check sequence (2)]
//
//    Acknowledgment packets:
//    [Preambles (4)][SFD (1)][Length = 5 (1)][Frame control field (2)]
//    [Sequence number (1)][Frame check sequence (2)]
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------
typedef struct
{
    uint16 myAddr;     // 本机地址
    uint16 panId;      // 网络ID
    uint8 channel;     // 通信信道， 11~26
    uint8 ackRequest;   //应答信号
    #ifdef SECURITY_CCM
        uint8 *securityKey;
        uint8 *securityNonce;
    #endif 
} basicRfCfg_t;

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS


//-------------------------------------------------------------------
//-------------------------------------------------------------------
// @fn          basicRfInit
// @brief       Initialise basic RF datastructures. Sets channel, short address and
//             PAN id in the chip and configures interrupt on packet reception
// @param       pRfConfig - pointer to BASIC_RF_CONFIG struct.
//                          This struct must be allocated by higher layer
//              txState - file scope variable that keeps tx state info
//              rxi - file scope variable info extracted from the last incoming frame
// @return      none
//-------------------------------------------------------------------
uint8 basicRfInit(basicRfCfg_t *pRfConfig);

//-------------------------------------------------------------------
// @fn          basicRfSendPacket
// @brief       Send packet
// @param       destAddr - destination short address
//              pPayload - pointer to payload buffer. This buffer must be
//                         allocated by higher layer.
//             length - length of payload
//              txState - file scope variable that keeps tx state info
//              mpdu - file scope variable. Buffer for the frame to send
// @return      basicRFStatus_t - SUCCESS or FAILED
//-------------------------------------------------------------------
uint8 basicRfSendPacket(uint16 destAddr, uint8 *pPayload, uint8 length);

//-------------------------------------------------------------------
// @fn          basicRfPacketIsReady
// @brief       Check if a new packet is ready to be read by next higher layer
// @param       none
// @return      uint8 - TRUE if a packet is ready to be read by higher layer
//-------------------------------------------------------------------
uint8 basicRfPacketIsReady(void);

//-------------------------------------------------------------------
// @fn          basicRfReceive
// @brief       Copies the payload of the last incoming packet into a buffer
// @param       pRxData - pointer to data buffer to fill. This buffer must be
//                        allocated by higher layer.
//              len - Number of bytes to read in to buffer
//              rxi - file scope variable holding the information of the last
//                    incoming packet
// @return      uint8 - number of bytes actually copied into buffer
//-------------------------------------------------------------------
uint8 basicRfReceive(uint8 *pRxData, uint16 len, int16 *pRssi);

//-------------------------------------------------------------------
// @fn          basicRfReceiveOn
// @brief       Turns on receiver on radio
// @param       txState - file scope variable
// @return      none
//-------------------------------------------------------------------
void basicRfReceiveOn(void);

//-------------------------------------------------------------------
// @fn          basicRfReceiveOff
// @brief       Turns off receiver on radio
// @param       txState - file scope variable
// @return      none
//-------------------------------------------------------------------
void basicRfReceiveOff(void);



#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "hal_uart1.h"


/**************************************************************************************
参数： 
       1.  *dst -- 目的数组的首地址，也就是最后存放数据的地方
       2.  dststart -- 从目的数组的第 dststart 位开始存放数据
       3.  *src -- 源数据的首地址，也就是数据的来源
       4.  srcstart -- 从源数组的第 srcstart 位开始取数据
       5.  len -- 需要从源数组取 len 字节的数据存放到目的数组中
例子： 
       MyByteCopy(buf1, 5, buf2, 3, 4)；
       从数组 buf2 的第3个字节开始往后的4个字节（buf2[3]、buf2[4]、buf2[5]、buf2[6]）
       复制到 buf1[5]、buf1[6]、buf1[7]、buf1[8]中。
***************************************************************************************/
void MyByteCopy(uint8 *dst, int dststart, uint8 *src, int srcstart, int len)
{
    int i;
    for(i=0; i<len; i++)
    {
        dst[dststart+i] = src[srcstart+i];
    }
}

/**********************************************************
参数：
      1. *uRxData -- 串口0接收数据所存放的地方
      2. uRxlen -- 串口0接收数据的初始长度（一般为0）
***********************************************************/
uint16 RecvUartData(uint8 *uRxData, uint16 uRxlen)
{   
    uint16 r_UartLen = 0;
    uint8 r_UartBuf[128]; 
     
    r_UartLen = halUartRxLen();
    halMcuWaitMs(5);
    while(r_UartLen > 0)
    {
        r_UartLen = halUartRead(r_UartBuf, sizeof(r_UartBuf));
        MyByteCopy(uRxData, uRxlen, r_UartBuf, 0, r_UartLen);
        uRxlen += r_UartLen;
        halMcuWaitMs(5);   //这里的延迟非常重要，因为这是串口连续读取数据时候需要有一定的时间间隔
        r_UartLen = halUartRxLen();       
    }   
    return uRxlen;
}

/**********************************************************
参数：
      1. *uRxData -- 串口1接收数据所存放的地方
      2. uRxlen -- 串口1接收数据的初始长度（一般为0）
***********************************************************/
uint16 RecvUart1Data(uint8 *uRxData, uint16 uRxlen)
{   
    uint16 r_Uart1Len = 0;
    uint8 r_Uart1Buf[128]; 
     
    r_Uart1Len = halUart1RxLen();
    halMcuWaitMs(5);
    while(r_Uart1Len > 0)
    {
        r_Uart1Len = halUart1Read(r_Uart1Buf, sizeof(r_Uart1Buf));
        MyByteCopy(uRxData, uRxlen, r_Uart1Buf, 0, r_Uart1Len);
        uRxlen += r_Uart1Len;
        halMcuWaitMs(5);   //这里的延迟非常重要，因为这是串口连续读取数据时候需要有一定的时间间隔
        r_Uart1Len = halUart1RxLen();       
    }   
    return uRxlen;
}
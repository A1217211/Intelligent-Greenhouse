#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "hal_uart1.h"


/**************************************************************************************
������ 
       1.  *dst -- Ŀ��������׵�ַ��Ҳ������������ݵĵط�
       2.  dststart -- ��Ŀ������ĵ� dststart λ��ʼ�������
       3.  *src -- Դ���ݵ��׵�ַ��Ҳ�������ݵ���Դ
       4.  srcstart -- ��Դ����ĵ� srcstart λ��ʼȡ����
       5.  len -- ��Ҫ��Դ����ȡ len �ֽڵ����ݴ�ŵ�Ŀ��������
���ӣ� 
       MyByteCopy(buf1, 5, buf2, 3, 4)��
       ������ buf2 �ĵ�3���ֽڿ�ʼ�����4���ֽڣ�buf2[3]��buf2[4]��buf2[5]��buf2[6]��
       ���Ƶ� buf1[5]��buf1[6]��buf1[7]��buf1[8]�С�
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
������
      1. *uRxData -- ����0������������ŵĵط�
      2. uRxlen -- ����0�������ݵĳ�ʼ���ȣ�һ��Ϊ0��
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
        halMcuWaitMs(5);   //������ӳٷǳ���Ҫ����Ϊ���Ǵ���������ȡ����ʱ����Ҫ��һ����ʱ����
        r_UartLen = halUartRxLen();       
    }   
    return uRxlen;
}

/**********************************************************
������
      1. *uRxData -- ����1������������ŵĵط�
      2. uRxlen -- ����1�������ݵĳ�ʼ���ȣ�һ��Ϊ0��
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
        halMcuWaitMs(5);   //������ӳٷǳ���Ҫ����Ϊ���Ǵ���������ȡ����ʱ����Ҫ��һ����ʱ����
        r_Uart1Len = halUart1RxLen();       
    }   
    return uRxlen;
}
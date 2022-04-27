#include "hal_defs.h"
#include "hal_uart.h"
#include "hal_board.h"
#include "UART_PRINT.h"
#include <stdio.h>
#include <string.h>
#include	<stdarg.h>


void WaitUs(uint16 usec)
{
    usec >>= 1;
    while (usec--)
    {
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        //NOP();
    }
}
/**********************************************************
func: ���մ������ݰ�
������
      1. *buf -- ���ڽ�����������ŵĵط�
      2. maxlen -- ���ڽ������ݵ���󳤶�
retuen: uint16 ���յ������ݳ���
***********************************************************/
uint16  uart_read(uint8 *buf, uint16 maxlen)
{
    uint16 slen=0; 
    uint8 dtime=0;
    while(dtime<20)
    {    
        while(HalUARTRead(buf+slen, 1))
        {
            slen++;
            if(slen>=maxlen)
                return  slen;
            dtime=0;
        }
        WaitUs(30);     // Wait 30us; 
        dtime++;
    }    
   return  slen; 
}
/*****
��printf��ʽ��ӡ����ַ���

***/
void	printf_str( char *buf, char *fmt,... )
{
    va_list ap;
    va_start(ap,fmt);
    vsprintf(buf,fmt,ap);
    va_end(ap);
}
/********/
void	uart_printf(char *fmt,...)
{
    va_list ap;
    char str[256];

    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    UartWrite(str,strlen(str));
    va_end(ap);
}

void	uart_enter(void)
{
    
       uart_printf("\r\n");
      
}

void	uart_datas(void * fmt,uint16 len)
{
    char * ptr;
    ptr=fmt;
    while(len--)
    {
        uart_printf("%02x ",*ptr );
        ptr++;
    }
     uart_enter();   
}

uint8   lrc_checksum(uint8* buf, uint8 len)
{
   uint8    lrc=0;
   while(len--)
   {
        lrc+=*buf;
        buf++;
   }
   lrc =~lrc;
   lrc++;
   return   lrc;
}

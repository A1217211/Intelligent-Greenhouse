#include<ioCC2530.h>

#define D1 P1_0
#define D2 P1_1

void led_init(void);
void led_test(void);
void halWait(unsigned char wait);

void main()
{
  led_init();
  while(1)
  {
    led_test();
  }
}

void led_init(void)
{
  P1SEL&=~0x03;
  P1DIR|=0x03;
  D1=1;
  D2=1;
}

void led_test(void)
{
  D1=0;
  halWait(250);
  D1=1;
  halWait(250);
  
  D2=0;
  halWait(250);
  D2=1;
  halWait(250);
}

void halWait(unsigned char wait)
{
  unsigned long largeWait;
  
  if(wait==0)
  {return;}
  largeWait=((unsigned short)(wait<<7));
  largeWait+=114*wait;
  
  largeWait=(largeWait>>(CLKCONCMD&0x07));
  while(largeWait--);
  
  return;
}
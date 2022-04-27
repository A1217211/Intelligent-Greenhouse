#include <ioCC2530.h>
#include "DHT11.h"


#define DATA_PIN P0_7
//��ʪ�ȶ���
uchar ucharFLAG,uchartemp;
char shidu,shidu_ge,wendu,wendu_ge;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;


//��ʱ����
void Delay_us() //1 us��ʱ
{
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");    
    asm("nop");
    asm("nop");
    asm("nop");    
    asm("nop"); 
}

void Delay_10us() //10 us��ʱ
{
  #if 0  
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();
  Delay_us();  
  #else
    int i = 10;
    while(i--);  
#endif  
}

void Delay_ms(uint Time)//n ms��ʱ
{
    unsigned char i;
    while(Time--)
    {
        for(i=0;i<100;i++)
            Delay_10us();
    }
}

//��ʪ�ȴ���
void COM(void)    // ��ʪд��
{     
    uchar i;      
    for(i=0;i<8;i++)
    {
        ucharFLAG=2;
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);
        if(ucharFLAG==1)break;
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp;
    }    
}

void DHT11(void)   //��ʪ��������
{
    DATA_PIN=0;
    Delay_ms(20);  //>18MS
    DATA_PIN=1;
    P0DIR &= ~0x80; //��������IO�ڷ���
    Delay_10us();
    Delay_10us(); 
    Delay_10us();
    Delay_10us();
    if(!DATA_PIN)//���if�н���ȥ����һ������һֱ��������ԭ���������ʱ������û�о�ȷ
    {
        ucharFLAG=2;
        while((!DATA_PIN)&&ucharFLAG++);
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);
        COM();
        ucharRH_data_H_temp=ucharcomdata;
        COM();
        ucharRH_data_L_temp=ucharcomdata;
        COM();
        ucharT_data_H_temp=ucharcomdata;
        COM();
        ucharT_data_L_temp=ucharcomdata;
        COM();
        ucharcheckdata_temp=ucharcomdata;
        DATA_PIN=1;
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);//����
        if(uchartemp==ucharcheckdata_temp)
        {
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
        }
         wendu=ucharT_data_H/10;
         wendu_ge=ucharT_data_H%10;
        
         shidu=ucharRH_data_H/10;
         shidu_ge=ucharRH_data_H%10;
     }
     else //û�óɹ���ȡ������0
     {
         wendu=2;
         wendu_ge=1;
         shidu=1;
         shidu_ge=5;
     }
    P0DIR |= 0x80; //IO����Ҫ�������� 
}
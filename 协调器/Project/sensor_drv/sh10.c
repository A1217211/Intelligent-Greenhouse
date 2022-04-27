/**************************************************************************************************
  Filename:      sh10.c

  Description:    sh10 driver 
                  
**************************************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include "ioCC2530.h"
#include "hal_defs.h"
#include "sh10.h"

#define BM(n)      (1 << (n))

#define MCU_IO_TRISTATE   1             
#define MCU_IO_PULLUP     2
#define MCU_IO_PULLDOWN   3

#define MCU_IO_OUTPUT(port, pin, val)  MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_OUTPUT_PREP(port, pin, val)  st( P##port##SEL &= ~BM(pin); P##port##_##pin## = val; P##port##DIR |= BM(pin); )
#define MCU_IO_INPUT(port, pin, func)  MCU_IO_INPUT_PREP(port, pin, func)
#define MCU_IO_INPUT_PREP(port, pin, func)  st( P##port##SEL &= ~BM(pin); P##port##DIR &= ~BM(pin); switch (func) { case MCU_IO_PULLUP: P##port##INP &= ~BM(pin); P2INP &= ~BM(port + 5); break; case MCU_IO_PULLDOWN: P##port##INP &= ~BM(pin); P2INP |= BM(port + 5); break; default: P##port##INP |= BM(pin); break; } )
#define MCU_IO_GET(port, pin)          MCU_IO_GET_PREP(port, pin)
#define MCU_IO_GET_PREP(port, pin)          (P##port## & BM(pin))


#if defined (BOARD_OLD)
/***********旧板IO设置********************/
#define HAL_BOARD_IO_SHT_SCK_PORT           2
#define HAL_BOARD_IO_SHT_SCK_PIN             4
#define HAL_BOARD_IO_SHT_SDA_PORT           2
#define HAL_BOARD_IO_SHT_SDA_PIN             3
#else
/***********新板板IO设置********************/
#define HAL_BOARD_IO_SHT_SCK_PORT           1
#define HAL_BOARD_IO_SHT_SCK_PIN             6
#define HAL_BOARD_IO_SHT_SDA_PORT           1
#define HAL_BOARD_IO_SHT_SDA_PIN             5
#endif


#define HAL_SHT_SCK_DIR_OUT()          MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,0)
#define HAL_SHT_SCK_SET()                  MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,1)
#define HAL_SHT_SCK_CLR()                  MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SCK_PORT, HAL_BOARD_IO_SHT_SCK_PIN,0)

#define HAL_SHT_SDA_SET()                 MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,1)
#define HAL_SHT_SDA_CLR()                 MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,0)
#define HAL_SHT_SDA_DIR_OUT()         MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,1)
#define HAL_SHT_SDA_DIR_IN()           MCU_IO_INPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,2)
#define HAL_SHT_SDA_VAL()                MCU_IO_GET(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN)     




#define ACK 1
#define noACK 0
#define measure_temp 0x03 //测量温度命令
#define measure_humi 0x05 //测量湿度命令
#define RESET        0x1e //软启动

#define     uchar    unsigned   char
#define     uint     unsigned   int
#define     ulong    unsigned   long

//------------------------数据结构体定义---------------------------
typedef union                      //保存所测得的温度＆湿度值
{ 
	uint  i;
	float f;
} value;
//WENSHIDU S;






/******************************************************************************
 * LOCAL FUNCTIONS
 */

void SHT11_DELAY(uint16 usec);
char write_byte(unsigned char value); 	  
char read_byte(unsigned char ack);		
void transstart(void);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sth11(float *p_humidity ,float *p_temperature);


#define SENSOR_SHT10





#if defined SENSOR_SHT10

void SHT11_DELAY(uint16 usec)
{
	usec >>= 1;
	
	while (usec--)
	{
		asm("NOP"); 
		asm("NOP");  
		asm("NOP");  
		asm("NOP");
		
		asm("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
		
		asm("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
		
		asm("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
	}	
}




char write_byte(unsigned char value)
{ 
  	unsigned char  i,error=0;

	for (i=0x80;i>0;i/=2)           
	{ 
		if (i & value) 
			HAL_SHT_SDA_SET();           
		else 
			HAL_SHT_SDA_CLR();

		HAL_SHT_SCK_SET();               
		SHT11_DELAY(1);                    
		HAL_SHT_SCK_CLR();
	}
	
	HAL_SHT_SDA_SET();                 
	HAL_SHT_SDA_DIR_IN();
	HAL_SHT_SCK_SET();               
	error=HAL_SHT_SDA_VAL();         
	HAL_SHT_SCK_CLR();

	return error;                   
}

char read_byte(unsigned char ack)
{ 
	unsigned char i,val=0;

	HAL_SHT_SDA_SET();                
	HAL_SHT_SDA_DIR_IN();
	for (i=0x80;i>0;i/=2)            
	{ 
		HAL_SHT_SCK_SET();               
		if (HAL_SHT_SDA_VAL()) 
			val=(val | i);               
		HAL_SHT_SCK_CLR();
	}
	MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,!ack);  
	HAL_SHT_SCK_SET();                            
	SHT11_DELAY(1);                             
	HAL_SHT_SCK_CLR();						    
	HAL_SHT_SDA_SET();                         

	return val;
}

void transstart(void)
{
	HAL_SHT_SCK_SET();
	SHT11_DELAY(1); 
	HAL_SHT_SDA_CLR();
	SHT11_DELAY(1);  
	HAL_SHT_SCK_CLR();
	SHT11_DELAY(2); 
	HAL_SHT_SCK_SET();
	SHT11_DELAY(1); 

	HAL_SHT_SDA_SET(); 
	SHT11_DELAY(1); 
	HAL_SHT_SCK_CLR();
	SHT11_DELAY(2); 
	HAL_SHT_SDA_CLR();	
}

void connectionreset(void)
{  
	unsigned char i; 
	HAL_SHT_SDA_SET(); 
	HAL_SHT_SCK_CLR();                   
	for(i=0;i<9;i++)                    
	{ 
		HAL_SHT_SCK_SET();
		HAL_SHT_SCK_CLR();
	}
	transstart();                   
}

char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
{ 
	unsigned char error=0;
	unsigned int i;

	connectionreset();
	switch(mode)
	{                    
		case 0	: error+=write_byte(measure_temp); break;
		case 1	: error+=write_byte(measure_humi); break;
		default     : break;	 
	}
	
	HAL_SHT_SDA_DIR_IN();
	for (i=0;i<65535;i++)
	{
		SHT11_DELAY(10); 
		if(HAL_SHT_SDA_VAL()== 0) break;
	}                               
	if(HAL_SHT_SDA_VAL())
	{
		error+=1;
	}
	
	*(p_value+1)  =read_byte(ACK);   
	*(p_value)=read_byte(ACK);       
	*p_checksum =read_byte(noACK);          
	return error;
}
/******************************************************************************
 * 名称       calc_sth11
 * 功能       计算温湿度值
 * 入口参数   float *p_humidity ,float *p_temperature
                湿度                  温度
 * 出口参数   无
 *****************************************************************************/
void calc_sth11(float *p_humidity ,float *p_temperature)      
{ 
	const float C1=-4.0;              
	const float C2=+0.0405;         
	const float C3=-0.0000028;      
	const float T1=+0.01;             
	const float T2=+0.00008;           
	float rh=*p_humidity;            
	float t=*p_temperature;          
	float rh_lin;                     
	float rh_true;                    
	float t_C;                        
	t_C=t*0.01 - 40;                    
	rh_lin=C3*rh*rh + C2*rh + C1;       
	rh_true=(t_C-25)*(T1+T2*rh)+rh_lin; 
	if(rh_true>100)rh_true=100;         
	if(rh_true<0.1)rh_true=0.1;         
	*p_temperature=t_C;                 
	*p_humidity=rh_true;                
}

/******************************************************************************
 * 名称       call_sht11
 * 功能       读取SH1x温湿度值
 * 入口参数   void call_sht11(unsigned int *tem_val,unsigned int *hum_val)
            *tem_val:温度存放地址，0.1度分辨率
            *hum_val:湿度存放地址，0.1分辨率
 * 出口参数  无  
 *****************************************************************************/
void call_sht11(unsigned int *tem_val,unsigned int *hum_val)
{ 
	value humi_val,temp_val;
	unsigned char error = 0,checksum;
        *tem_val=0;
        *hum_val=0;
        //启动SH1x的湿度测量
       	error+=s_measure((unsigned char*) &humi_val.i,&checksum,1); 
        //启动SH1x的温度测量
	error+=s_measure((unsigned char*) &temp_val.i,&checksum,0);  
	if(error!=0)
	{   //测试错误，进行软复位
	    connectionreset();                                       
	}
	else
	{   //测试数据处理
            humi_val.f=(float)humi_val.i;                    
	    temp_val.f=(float)temp_val.i;
            //计算温湿度实际值
	    calc_sth11(&humi_val.f,&temp_val.f);
            //转化成0.1的分辨率
            *tem_val=temp_val.f*10;
            *hum_val=humi_val.f*10;
	}    
}

#endif


























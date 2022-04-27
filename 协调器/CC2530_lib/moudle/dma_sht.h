

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
{ uint  i;
  float f;
} value;

typedef struct __WENSHIDU__
{
	uchar gewei;
	uchar shiwei;
	uchar DateString1[4];
       // uchar DateString2[8];
}WENSHIDU;

WENSHIDU S;


/***************************函数声明*******************************/
char write_byte(unsigned char value); 	  
char read_byte(unsigned char ack);		
void transstart(void);
void connectionreset(void);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sth11(float *p_humidity ,float *p_temperature);
void DateToStr(WENSHIDU *Time,float datax,float datax1);
void call_sht11(void);  




//--------- write a byte and checks the ack signal----------------- 
char write_byte(uchar value)
{ 
  uchar i,error=0;

  for (i=0x80;i>0;i/=2)             /*连续向右移动8位*/
  { 
    if (i & value) 
      HAL_SHT_SDA_SET();            /*把相应的位送数据线*/
    else 
      HAL_SHT_SDA_CLR();
    
    HAL_SHT_SCK_SET();               /*时序脉冲，应严格按着此标准*/
    halMcuWaitUs(1);                       /*大约 1 us*/	
    HAL_SHT_SCK_CLR();
  }
  HAL_SHT_SDA_SET();                 /*释放数据线*/
  HAL_SHT_SDA_DIR_IN();
  HAL_SHT_SCK_SET();                 /*第9位作为响应位*/
  error=HAL_SHT_SDA_VAL();          /*检测响应情况，如果有响应数据线就会被SHT10拉低*/
  HAL_SHT_SCK_CLR();
  
  return error;                     /*返回1表示没响应*/
}

//--------- read a byte and checks the ack signal----------------- 
char read_byte(uchar ack)
{ 
  uchar i,val=0;

  HAL_SHT_SDA_SET();                 /*释放数据线*/
  HAL_SHT_SDA_DIR_IN();
  for (i=0x80;i>0;i/=2)             /*连续向右移动8位*/
  { 
    HAL_SHT_SCK_SET();               /*clk for SENSI-BUS*/  
    if (HAL_SHT_SDA_VAL()) 
      val=(val | i);                 /*read bit */ 
    HAL_SHT_SCK_CLR();
  }
  MCU_IO_OUTPUT(HAL_BOARD_IO_SHT_SDA_PORT, HAL_BOARD_IO_SHT_SDA_PIN,!ack);   /*当ack＝1时拉低数据线*/
  HAL_SHT_SCK_SET();                            /*clk #9 for ack*/
   halMcuWaitUs(1);                  /*pulswith approx 1 us */
  HAL_SHT_SCK_CLR();						    
  HAL_SHT_SDA_SET();                           /*释放数据线*/
  
  return val;
}


void transstart(void)
{
  HAL_SHT_SCK_SET();
   halMcuWaitUs(1);
  HAL_SHT_SDA_CLR();
   halMcuWaitUs(1);  
  HAL_SHT_SCK_CLR();
    halMcuWaitUs(2);
  HAL_SHT_SCK_SET();
   halMcuWaitUs(1);
  
  HAL_SHT_SDA_SET(); 
   halMcuWaitUs(1);
  HAL_SHT_SCK_CLR();
    halMcuWaitUs(2);
  HAL_SHT_SDA_CLR();	
}

void connectionreset(void)
{  
  unsigned char i; 
  
  HAL_SHT_SDA_SET(); 
  HAL_SHT_SCK_CLR();                    //Initial state

  for(i=0;i<9;i++)                     //9 SCK cycles
  { 
    HAL_SHT_SCK_SET();
    HAL_SHT_SCK_CLR();
  }
  
  transstart();                   //transmission start
}



char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
{ 
  unsigned error=0;
  uint i;

  connectionreset();
  switch(mode)
  {                     //发送寄存器地址给传感器
    case 0	: error+=write_byte(measure_temp); break;
    case 1	: error+=write_byte(measure_humi); break;
    default     : break;	 
  }
  HAL_SHT_SDA_DIR_IN();
  for (i=0;i<35000;i++)
  {
    halMcuWaitUs(1);
    if(HAL_SHT_SDA_VAL()== 0) break;
  }                                 //等待拉低数据线
  if(HAL_SHT_SDA_VAL()) error+=1;         
  *(p_value+1)  =read_byte(ACK);    //读高字节           
  *(p_value)=read_byte(ACK);        //读低字节         
  *p_checksum =read_byte(noACK);          

  return error;
}


void calc_sth11(float *p_humidity ,float *p_temperature)      
{ 
  const float C1=-4.0;              //for 12 Bit 
  const float C2=+0.0405;           //for 12 Bit 
  const float C3=-0.0000028;        //for 12 Bit 
  const float T1=+0.01;             //for 14 Bit @ 5V 
  const float T2=+0.00008;           //for 14 Bit @ 5V 	

  float rh=*p_humidity;             //rh:      Humidity [Ticks] 12 Bit
  float t=*p_temperature;           //*t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     //rh_lin:  Humidity linear
  float rh_true;                    //rh_true: Temperature compensated humidity 
  float t_C;                        //t_C   :  Temperature [癈] 

  t_C=t*0.01 - 40;                     //calc. temperature from ticks to [癈]
  rh_lin=C3*rh*rh + C2*rh + C1;        //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;  //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;          //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;          //the physical possible range

  *p_temperature=t_C;                  //return temperature [癈]
  *p_humidity=rh_true;                 //return humidity[%RH]
}


void DateToStr(WENSHIDU *Time,float datax,float datax1)
{
    uint i;

    i=(int)datax;
	Time->DateString1[0] = i;
    i=(int)((datax-i)*10); 
	Time->DateString1[1] = i;

    i=(int)datax1;
	Time->DateString1[2] = i;
    i=(int)((datax1-i)*10); 
        Time->DateString1[3] = i;

}



void call_sht11(void)
{
    
    value humi_val,temp_val;
    unsigned char error = 0,checksum;

    
    error+=s_measure((unsigned char*) &humi_val.i,&checksum,1);   //measure humidity
    error+=s_measure((unsigned char*) &temp_val.i,&checksum,0);   //measure temperature
    if(error!=0) 
        connectionreset();                                        //如果发生错误，系统复位
    else
    {
        humi_val.f=(float)humi_val.i;                     //converts integer to float
        temp_val.f=(float)temp_val.i;                     //converts integer to float
        calc_sth11(&humi_val.f,&temp_val.f);              //calculate humidity,temperature
        DateToStr(&S,temp_val.f,humi_val.f);
    }
}




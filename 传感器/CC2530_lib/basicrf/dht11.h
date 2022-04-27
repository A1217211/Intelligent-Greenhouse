#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;


extern void Delay_ms(unsigned int xms);	//延时函数
extern void COM(void);                  // 温湿写入
extern void DHT11(void);                //温湿传感启动

extern char shidu,shidu_ge,wendu,wendu_ge;
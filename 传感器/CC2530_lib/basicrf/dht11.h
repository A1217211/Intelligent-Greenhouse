#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int  uint;


extern void Delay_ms(unsigned int xms);	//��ʱ����
extern void COM(void);                  // ��ʪд��
extern void DHT11(void);                //��ʪ��������

extern char shidu,shidu_ge,wendu,wendu_ge;
#include <ioCC2530.h>

#define CRYSTAL 0
#define RC 1

void SET_MAIN_CLOCK_SOURCE(unsigned char source);
void SET_LOW_CLOCK(unsigned char source);
void SET_POWER_MODE(unsigned char mode);
void Set_ST_Period(unsigned char sec);
void Init_SLEEP_TIMER(void);

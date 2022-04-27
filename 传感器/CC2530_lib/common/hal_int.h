//-------------------------------------------------------------------
// Filename: hal_int.h
// Description: HAL interrupt control header file
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// MACROS
//-------------------------------------------------------------------
#define HAL_INT_ON(x)      st( EA = 1; )
#define HAL_INT_OFF(x)     st( EA = 0; )
#define HAL_INT_LOCK(x)    st( (x) = EA; EA = 0; )
#define HAL_INT_UNLOCK(x)  st( EA = (x); )

typedef unsigned short istate_t;

//-------------------------------------------------------------------
// GLOBAL FUNCTIONS
//-------------------------------------------------------------------
void halIntOn(void);
void halIntOff(void);
uint16 halIntLock(void);
void halIntUnlock(uint16 key);

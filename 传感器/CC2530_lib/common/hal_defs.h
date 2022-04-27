//-------------------------------------------------------------------
// Filename: hal_defs.h
// Description: HAL defines
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------
#include <ioCC2530.h>

//-------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------
typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed long int32;
typedef unsigned long uint32;

typedef void(*ISR_FUNC_PTR)(void);
typedef void(*VFPTR)(void);

//-------------------------------------------------------------------
// CONSTANTS AND DEFINES
//-------------------------------------------------------------------
#define TRUE 1
#define FALSE 0
#define NULL (void *)0

#define SUCCESS 0
#define FAILED  1

//-------------------------------------------------------------------
// MACROS
//-------------------------------------------------------------------
#define BV(n)      (1 << (n))

#define BM(n)      (1 << (n))

#define BF(x,b,s)  (((x) & (b)) >> (s))

#define MIN(n,m)   (((n) < (m)) ? (n) : (m))

#define MAX(n,m)   (((n) < (m)) ? (m) : (n))

#define ABS(n)     (((n) < 0) ? -(n) : (n))

// uint32 processing 
#define BREAK_UINT32( var, ByteNum ) (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) ((uint32)((uint32)((Byte0) & 0x00FF) + ((uint32)((Byte1) & 0x00FF) << 8) + ((uint32)((Byte2) & 0x00FF) << 16) + ((uint32)((Byte3) & 0x00FF) << 24)))

#define HI_UINT32(a) ((uint16) (((uint32)(a)) >> 16))
#define LO_UINT32(a) ((uint16) ((uint32)(a)))

// uint16 processing 
#define BUILD_UINT16(loByte, hiByte) ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((uint16)(a) >> 8) & 0xFF)
#define LO_UINT16(a) ((uint16)(a) & 0xFF)

// uint16 processing 
#define BUILD_UINT8(hiByte, loByte) ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((uint8)(a) >> 4) & 0x0F)
#define LO_UINT8(a) ((uint8)(a) & 0x0F)

//-------------------------------------------------------------------
//  This macro is for use by other macros to form a fully valid C statement.
//  Without this, the if/else conditionals could show unexpected behavior.
//
//  For example, use...
//    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
//  instead of ...
//    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
//  or
//    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
//  The last macro would not behave as expected in the if/else construct.
//  The second to last macro will cause a compiler error in certain uses
//  of if/else construct
//
//  It is not necessary, or recommended, to use this macro where there is
//  already a valid C statement.  For example, the following is redundant...
//    #define CALL_FUNC()   st(  func();  )
//  This should simply be...
//    #define CALL_FUNC()   func()
//
// (The while condition below evaluates false without generating a
//  constant-controlling-loop type of warning on most compilers.)
//-------------------------------------------------------------------
#define st(x)      do { x } while (__LINE__ == -1)

//-------------------------------------------------------------------
// Compiler abstraction
//-------------------------------------------------------------------
#define _PRAGMA(x) _Pragma(#x)

#define FAR
#define NOP()  asm("NOP")

#define HAL_MCU_LITTLE_ENDIAN()  __LITTLE_ENDIAN__
#define HAL_ISR_FUNC_DECLARATION(f,v) _PRAGMA(vector=v) __near_func __interrupt void f(void)
#define HAL_ISR_FUNC_PROTOTYPE(f,v) _PRAGMA(vector=v) __near_func __interrupt void f(void)
#define HAL_ISR_FUNCTION(f,v) HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)

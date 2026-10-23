#ifndef H2_PORT_CRT_FLOAT_H
#define H2_PORT_CRT_FLOAT_H
/* The compiler's own <float.h> plus the Microsoft CRT floating-point control
   calls the recovered source uses. The guest leaves the host's floating-point
   environment alone: the control word reads back as the x87 default
   (round to nearest, 53-bit precision, all exceptions masked) and writes are
   accepted without effect. */
#include_next <float.h>

#define _MCW_EM 0x0008001fu
#define _MCW_RC 0x00000300u
#define _MCW_PC 0x00030000u
#define _RC_NEAR 0x00000000u
#define _RC_CHOP 0x00000300u
#define _PC_24 0x00020000u
#define _PC_53 0x00010000u
#define _PC_64 0x00000000u
#define _EM_INVALID 0x00000010u
#define _EM_DENORMAL 0x00080000u
#define _EM_ZERODIVIDE 0x00000008u
#define _EM_OVERFLOW 0x00000004u
#define _EM_UNDERFLOW 0x00000002u
#define _EM_INEXACT 0x00000001u

#ifdef __cplusplus
extern "C" {
#endif
static inline unsigned int _control87(unsigned int value, unsigned int mask)
{
    (void)value;
    (void)mask;
    return _PC_53 | _RC_NEAR | _MCW_EM;
}
static inline unsigned int _controlfp(unsigned int value, unsigned int mask)
{
    return _control87(value, mask);
}
static inline unsigned int _clearfp(void) { return 0; }
static inline unsigned int _statusfp(void) { return 0; }
#ifdef __cplusplus
}
#endif

#endif

/**
 * @file	macros.h
 * @Author	turgay (turgay@dtsis.com)
 * @date	20131001
 * @brief	kullanisli macro tanimlari
 *
 * v2.0
 */

#include <stdint.h>

/* flags macros */
#define ISSET(stat, flg)        ((((stat) & (flg)) == (flg)) ? TRUE : FALSE)
#define ISNSET(stat, flg)       ((((stat) & (flg)) != (flg)) ? TRUE : FALSE)
#define ISNOTSET(stat, flg)     ISNSET(stat, flg)

#define SET(stat, flg)          (stat) |= (flg)
#define CLEAR(stat, flg)        (stat) &= ~(flg)
#define TOGLE(stat, flg)        { if (ISSET(stat, flg)) { CLEAR(stat, flg); } else { SET(stat, flg);} }

/* !flags macros */

#ifndef UNUSED
#define UNUSED(a)		(void)(a)
#endif


/* src url: http://cgit.haiku-os.org/haiku/plain/src/system/libroot/posix/glibc/include/arch/x86/bits/byteswap.h
 */
/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#ifdef __BIG_ENDIAN
 #define U32_LE(v)      (unsigned       int)(__rev(v))
 #define U16_LE(v)      (unsigned short int)(__rev(v) >> 16)
 #define U32_BE(v)      (unsigned       int)(v)
 #define U16_BE(v)      (unsigned short int)(v)
#else
 #define U32_BE(v)      (unsigned       int)(__rev(v))
 #define U16_BE(v)      (unsigned short int)(__rev(v) >> 16)
 #define U32_LE(v)      (unsigned       int)(v)
 #define U16_LE(v)      (unsigned short int)(v)
#endif

#define ABS(x)              ((x)<0 ? (x)*(-1) : x )

#define B0(x)               ((x) & 0xFF)
#define B1(x)               (((x)>>8) & 0xFF)
#define B2(x)               (((x)>>16) & 0xFF)
#define B3(x)               (((x)>>24) & 0xFF)

/* eof */

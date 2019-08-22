/***************************************************************************************************
 * v:1.1 - 20100726 - Turgay ÇALIÞKAN
 *  [+] I8,I16,U32,I64 tanýmlarýný ekledim.
 *  [-] "RTL.H" ý çýkardým.
 *  [+] uint_8, uint16 tanýmlarýný ekledim.
 *
 **************************************************************************************************/

#ifndef __TYPE_H__
#define __TYPE_H__

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif


//#define U8                    uint8_t
//#define S8                    int8_t
//#define U16                   uint16_t
//#define S16                   int16_t
//#define U32                   uint32_t
//#define S32                   int32_t
//#define U64                   uint64_t
//#define S64                   int64_t

typedef signed char     		S8;
typedef unsigned char   		U8;
typedef short           		S16;
typedef unsigned short  		U16;
typedef int             		S32;
typedef unsigned int    		U32;
typedef long long       		S64;
typedef unsigned long long 		U64;
typedef unsigned char   		BIT;
typedef unsigned int    		BOOL;


#define DWORD                 U32
#define BYTE                  U8
#define UCHAR                 U8
//#define CHAR                  S8
#define USHORT                U16
#define SHORT                 S16
#define ULONG                 U64
#define LONG                  S64
#define DWORD                 U32
#define UINT                  U32
//#define BOOL                  U8

/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#ifdef __cplusplus
  #define     __I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     __I     volatile const          /*!< defines 'read only' permissions      */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */


///#include "RTL.h"
//typedef unsigned char       U8;
//typedef signed char         S8;
//typedef unsigned short      U16;
//typedef signed short        S16;
//typedef unsigned int        U32;
//typedef signed int          S32;
//typedef unsigned long long  U64;
//typedef signed long long    S64;
//
//typedef signed char         I8;
//typedef signed short        I16;
//typedef signed int          I32;
//typedef signed long long    I64;
//
//
//typedef unsigned char       u8;
//typedef signed char         s8;
//typedef unsigned short      u16;
//typedef signed short        s16;
//typedef unsigned int        u32;
//typedef signed int          s32;
//typedef unsigned long long  u64;
//typedef signed long long    s64;
//
//typedef unsigned char       UCHAR;
//typedef char                CHAR;
//typedef unsigned short      USHORT;
//typedef short               SHORT;
//typedef unsigned long       ULONG;
//typedef long                LONG;
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;
//typedef unsigned long       DWORD;
//typedef unsigned int        UINT;
//typedef unsigned int        BOOL;
//
//typedef unsigned char       uint_8;
//typedef unsigned short      uint_16;

///*type definitions*/
//typedef struct// _TIME
//{
//	U8 Hour;
//	U8 Minute;
//	U8 Second;
//	U8 Day;
//	U8 Month;
//	U8 Year;
//}TIMEDATE;
//
//
///*type definitions*/
//typedef __packed struct
//{
//	U8 Hour;
//	U8 Minute;
//	U8 Second;
//	U8 Day;
//	U8 Month;
//	U8 Year;
//} TDateTime;


#endif  /* __TYPE_H__ */








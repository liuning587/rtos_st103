/**
 ******************************************************************************
 * @file       types.h
 * @version    V0.0.1
 * @brief      API C source file of types.
 * @details    This file including all API functions's declare of types.
 * @copy       Copyrigth(C), 2012-2015.
 *
 ******************************************************************************
 */

#ifndef TYPES_H
#define TYPES_H


/*-----------------------------------------------------------------------------
Section: Includes
-----------------------------------------------------------------------------*/
/* None */

/*-----------------------------------------------------------------------------
Section: Macro Definitions
-----------------------------------------------------------------------------*/
#ifndef OK
#define OK       0
#endif
#ifndef ERROR
//#define ERROR    (-1)
#endif
#ifndef IMPORT
#define IMPORT extern
#endif
#ifndef LOCAL
#define LOCAL static
#endif
#ifndef FAST
#define FAST  register
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#define     __I     volatile const            /*!< defines 'read only' permissions      */
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */


/*-----------------------------------------------------------------------------
Section: Type Definitions
-----------------------------------------------------------------------------*/
typedef char                int8;
typedef short int           int16;
typedef long int            int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned long int   uint32;
typedef unsigned long long  uint64;

typedef unsigned char       byte;
typedef unsigned char       bits;
typedef unsigned char       uchar;

typedef float               fp32;
typedef double              fp64;

typedef char                INT8;
typedef short int           INT16;
typedef long int            INT32;
typedef long long           INT64;

typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned long int   UINT32;
typedef unsigned long long  UINT64;

typedef unsigned char       BYTE;
typedef unsigned char       BITS;
typedef unsigned char       UCHAR;

typedef char               _int8_t;
typedef unsigned char      _uint8_t;

typedef short              _int16_t;
typedef unsigned short     _uint16_t;

typedef int                _int32_t;
typedef unsigned int       _uint32_t;

typedef long long          _int64_t;
typedef unsigned long long _uint64_t;
#if 0
typedef _int8_t      int8_t;
typedef _uint8_t     uint8_t;

typedef _int16_t     int16_t;
typedef _uint16_t    uint16_t;

#ifdef __INT24_DEFINED
typedef _int24_t     int24_t;
typedef _uint24_t    uint24_t;
#endif

typedef _int32_t     int32_t;
typedef _uint32_t    uint32_t;

#ifdef __INT64_DEFINED
typedef _int64_t     int64_t;
typedef _uint64_t    uint64_t;
#endif
#endif
#ifndef BOOL
typedef uint8 BOOL;
#endif
#ifndef bool
typedef uint8 bool;
#endif

/* size_t is used for sizes of objects.
 * ssize_t is used for a count of bytes or an error indication.
 */
typedef unsigned char tBoolean;

typedef enum  Boolean {
   false = 0,
   true  = 1
} boolean;
typedef signed long time_t;

#define WAIT_FOREVER  0

#define FOREVER	for (;;)

#define FAR

#define max(x, y)	(((x) < (y)) ? (y) : (x))
#define min(x, y)	(((x) < (y)) ? (x) : (y))
#define isascii(c)	((unsigned) (c) <= 0177)
#define toascii(c)	((c) & 0177)
#define BITS(x,y) (((x)>>(y))&0x01)   /* 判断某位是否为1 */
#define SETBITS(x,y,n) (x) = (n) ? ((x)|(1 << (y))) : ((x) &(~(1 << (y))));
#define MAX(x, y)   (((x) < (y)) ? (y) : (x))
#define MIN(x, y)   (((x) < (y)) ? (x) : (y))
#define BCD2HEX(x) (((x) >> 4) * 10 + ((x) & 0x0F))       /*20H -> 20*/
#define HEX2BCD(x) (((x) % 10) + ((((x) / 10) % 10) << 4))  /*20 -> 20H*/

#define LMMSB(x) (((x)>>32)&0xff)
#define LLLSB(x) ((x) & 0x0F)
#define LLNLSB(x) (((x) >> 4) & 0x0F)
#define LLNLHB(x) (((x) >> 8) & 0x0f)
#define BYTESWAP(x) ((MSB((x)) | (LSB((x)) << 8)))
#define BITS(x,y) (((x)>>(y))&0x01)   /* 判断某位是否为1 */
#define SETBITS(x,y,n) (x) = (n) ? ((x)|(1 << (y))) : ((x) &(~(1 << (y))));
#define INVERSE(x,y)    ((x)=(x)^(1<<(y)))  /* 给某位置反 */
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#define OFFSET(structure, member)	/* byte offset of member in structure*/\
		((int)&(((structure *) 0) -> member))

#define MEMBER_SIZE(structure, member)	/* size of a member of a structure */\
		(sizeof (((structure *) 0) -> member))


#ifdef __cplusplus
typedef void 		(*OSFUNCPTR) (void *);     /* ptr to function returning int */
typedef int 		(*FUNCPTR) (...);     /* ptr to function returning int */
typedef void 		(*VOIDFUNCPTR) (...); /* ptr to function returning void */
typedef double 		(*DBLFUNCPTR) (...);  /* ptr to function returning double*/
typedef float 		(*FLTFUNCPTR) (...);  /* ptr to function returning float */
typedef void (*VOIDFUNCPTRBOOL)(boolean);

#else
typedef void 		(*OSFUNCPTR) (void *);     /* ptr to function returning int */
typedef int 		(*FUNCPTR) ();	   /* ptr to function returning int */
typedef void 		(*VOIDFUNCPTR) (); /* ptr to function returning void */
typedef double 		(*DBLFUNCPTR) ();  /* ptr to function returning double*/
typedef float 		(*FLTFUNCPTR) ();  /* ptr to function returning float */
typedef void (*VOIDFUNCPTRBOOL)(boolean b);

#endif			/* _cplusplus */

typedef union {
    unsigned long longValue;
    unsigned char array[4];
    struct{unsigned short high,low;} shortValue;
    struct{unsigned char highest,higher,middle,low;}charValue;
}U_UINT32;

typedef union
{
     unsigned long LongValue;
     unsigned char Array[4];
     struct{unsigned short High,Low;} IntValue;
     struct{unsigned char Highest,Higher,Middle,Low;}CharValue;
}Long_Char;

typedef int STATUS;

#endif /* TYPES_H */

/*-------------------------------End of types.h------------------------------*/


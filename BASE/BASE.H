#ifndef	INCLUDED_BASE_H
#define INCLUDED_BASE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<STDIO.H>
#include	<STDLIB.H>
#ifdef __VBCC__
#include	<stddef.h>
#endif

/* ###################################################################################
#  PLATFORM DEFINITIONS
################################################################################### */


#ifdef	__GNUC__
#define	dGODLIB_COMPILER_GCC
#endif

#ifdef	WIN32

#define	dGODLIB_PLATFORM_WIN

#else

#define	dGODLIB_PLATFORM_ATARI

#ifdef __AHCC__

#define dGODLIB_COMPILER_AHCC

#endif

#endif


#if	defined(dSYSTEM_SDL)

#define	dGODLIB_SYSTEM_SDL

#elif defined(WIN32)

#define	dGODLIB_SYSTEM_D3D

#else

#define	dGODLIB_SYSTEM_TOS

#endif


#if (defined(__i386__) || defined(__i386)) || \
     defined(__ia64__) || defined(WIN32) || \
    (defined(__alpha__) || defined(__alpha)) || \
     defined(__arm__) || \
    (defined(__mips__) && defined(__MIPSEL__)) || \
     defined(__SYMBIAN32__) || \
     defined(__x86_64__) || \
     defined(__LITTLE_ENDIAN__)

#define dGODLIB_ENDIAN_LITTLE

#else

#define dGODLIB_ENDIAN_BIG

#endif

#ifdef	dGODLIB_PLATFORM_ATARI

#define	mSTEEM_BREAK	*(U32*)0x200L = 1;

#else

#define	mSTEEM_BREAK

#endif


/* ###################################################################################
#  TYPEDEFS
################################################################################### */

#if defined(WIN32) && !defined(dGODLIB_COMPILER_GCC)

#pragma warning (disable: 4115)
#pragma warning (disable: 4201)
#pragma warning (disable: 4996)
#pragma warning (disable: 4121)
#pragma warning (disable: 4127)
/*
#pragma warning (disable: 4244)
#pragma warning (disable: 4245)
*/
#endif

#ifdef dGODLIB_COMPILER_GCC

#include	<stdint.h>

typedef uint8_t					U8;
typedef int8_t					S8;
typedef uint16_t 				U16;
typedef int16_t				 	S16;
typedef uint32_t				U32;
typedef int32_t				 	S32;
typedef	float					F32;
typedef float                   FP32;

#pragma pack(2)

#elif defined(WIN32)

typedef unsigned char			U8;
typedef signed char				S8;
typedef unsigned short 			U16;
typedef signed short          	S16;
typedef unsigned int	        U32;
typedef signed int            	S32;
typedef	float					F32;
typedef float                   FP32;

#elif defined( dGODLIB_COMPILER_AHCC )

typedef unsigned char	U8;
typedef signed char		S8;
typedef unsigned int	U16;
typedef signed int		S16;
typedef unsigned long	U32;
typedef signed long		S32;
#define F32				float;
#define FP32			float;

#else

typedef unsigned char	U8;
typedef signed char		S8;
typedef unsigned int	U16;
typedef signed int		S16;
typedef unsigned long	U32;
typedef signed long		S32;
typedef	float			F32;
typedef float			FP32;

#endif


#ifdef	dGODLIB_ENDIAN_LITTLE


typedef	union
{
struct { S8 b0,b1; } b;
S16					 w;
} uS16;


typedef	union
{
struct { U8 b0,b1; } b;
U16					 w;
} uU16;


typedef union
{
struct {S8 b0,b1,b2,b3;} b;
struct {S16 w0,w1;} 	 w;
S32 					 l;
} uS32;


typedef union
{
struct {U8 b0,b1,b2,b3;} b;
struct {U16 w0,w1;} 	 w;
U32						 l;
} uU32;


#else


typedef	union
{
struct { S8 b1,b0; } b;
S16					 w;
} uS16;


typedef	union
{
struct { U8 b1,b0; } b;
U16					 w;
} uU16;


typedef union
{
struct {S8 b3,b2,b1,b0;} b;
struct {S16 w1,w0;} 	 w;
S32 					 l;
} uS32;


typedef union
{
struct {U8 b3,b2,b1,b0;} b;
struct {U16 w1,w0;} 	 w;
U32						 l;
} uU32;


#endif

#ifdef dGODLIB_COMPILER_GCC
#define stricmp strcasecmp
#define strcmpi strcasecmp
#endif

/* ###################################################################################
#  MACROS
################################################################################### */

#define	mSTRING_TO_U32(a,b,c,d)	( (((U32)a)<<24) | (((U32)b)<<16) | (((U32)c)<<8) | ((U32)d) )
#ifdef __VBCC__
/*#define	mOFFSET( _aType, _aMember )	((U32)offsetof( _aType, _aMember ))*/
/*#define	mOFFSET( _aType, _aMember )	((U32)__offsetof( _aType, _aMember ))*/
#define mOFFSET( _aType, _aMember ) 0
#else
#define	mOFFSET( _aType, _aMember )	( (U32) &(((_aType*)0)->_aMember) )
#endif

#define	mARRAY_COUNT( _aArray )	( sizeof(_aArray) / sizeof(_aArray[0]) )

#ifdef dGODLIB_PLATFORM_WIN
#define	mENUM(_a)	enum _a
#else
#define	mENUM(_a)	U8
#endif

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	U32		ID;
	char *	pString;
} sTagString;

typedef struct
{
	U32		ID;
	U32		Value;
} sTagValue;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

char *	sTagString_GetpString( const U32 aID, const sTagString * apTagStrings, const U32 aLimit );
U32		sTagString_GetID( const char * apString, const sTagString * apTagStrings, const U32 aLimit );
U8		sTagString_StringExists( const char * apString, const sTagString * apTagStrings, const U32 aLimit );
U8		sTagString_IDExists( const U32 aID, const sTagString * apTagStrings, const U32 aLimit );

U32		sTagValue_GetValue( const U32 aID,    const sTagValue * apTagValues, const U32 aLimit );
U32		sTagValue_GetID(    const U32 aValue, const sTagValue * apTagValues, const U32 aLimit );

#ifdef	dGODLIB_ENDIAN_LITTLE

#define	Endian_FromLittleU16( apU16 )
#define	Endian_FromLittleU32( apU32 )

#define	Endian_FromBigU16( apU16 )		Endian_Twiddle_U16( (U8*)apU16 )
#define	Endian_FromBigU32( apU32 )		Endian_Twiddle_U32( (U8*)apU32 )

#define	Endian_ReadLittleU16( apU16, aValue )			( aValue = *apU16 )
#define	Endian_ReadLittleU32( apU32, aValue )			( aValue = *apU32 )

#define	Endian_WriteLittleU16( apU16, aValue )			( *apU16 = aValue )
#define	Endian_WriteLittleU32( apU16, aValue )			( *apU32 = aValue )

#define	Endian_ReadBigU16( apU16, aValue )				( aValue = (U16)((((*apU16)>>8) & 0xFF) | ((*apU16)<<8)))
#define	Endian_ReadBigU32( apU16, aValue )				( aValue = (((*apU16)>>24) & 0xFFL) | (((*apU16)>>8)&0xFF00L) | (((*apU16)<<8)&0xFF0000L) | (((*apU16)<<24)&0xFF000000L) )

#define	Endian_WriteBigU16( apU16, aValue )				( *apU16 = (U16)((((aValue)>>8) & 0xFF) | ((aValue)<<8)) )
#define	Endian_WriteBigU32( apU16, aValue )				( *apU16 = (((aValue)>>24L) & 0xFFL) | (((aValue)>>8L)&0xFF00L) | (((aValue)<<8L)&0xFF0000L) | (((aValue)<<24L)&0xFF000000L) )

#else

#define	Endian_FromLittleU16( apU16 )	Endian_Twiddle_U16( (U8*)apU16 )
#define	Endian_FromLittleU32( apU32 )	Endian_Twiddle_U32( (U8*)apU32 )

#define	Endian_FromBigU16( apU16 )
#define	Endian_FromBigU32( apU32 )

#define	Endian_ReadBigU16( apU16, aValue )			( aValue = *apU16 )
#define	Endian_ReadBigU32( apU32, aValue )			( aValue = *apU32 )

#define	Endian_WriteBigU16( apU16, aValue )			( *apU16 = aValue )
#define	Endian_WriteBigU32( apU32, aValue )			( *apU32 = aValue )

#define	Endian_ReadLittleU16( apU16, aValue )				( aValue = (((*apU16)>>8) & 0xFF) | ((*apU16)<<8) )
#define	Endian_ReadLittleU32( apU16, aValue )				( aValue = (((*apU16)>>24) & 0xFFL) | (((*apU16)>>8)&0xFF00L) | (((*apU16)<<8)&0xFF0000L) | (((*apU16)<<24)&0xFF000000L) )

#define	Endian_WriteLittleU16( apU16, aValue )				( *apU16 = (((aValue)>>8) & 0xFF) | ((aValue)<<8) )
#define	Endian_WriteLittleU32( apU16, aValue )				( *apU16 = (((*aValue)>>24) & 0xFFL) | (((*aValue)>>8)&0xFF00L) | (((*aValue)<<8)&0xFF0000L) | (((*aValue)<<24)&0xFF000000L) )

#endif

void	Endian_Twiddle_U16( U8 * apU16 );
void	Endian_Twiddle_U32( U8 * apU32 );


S16		EndianSwap_S16( U8 * apS16 );
U16		EndianSwap_U16( U8 * apU16 );
S32		EndianSwap_S24( U8 * apS24 );
U32		EndianSwap_U24( U8 * apU24 );
S32		EndianSwap_S32( U8 * apS32 );
U32		EndianSwap_U32( U8 * apU16 );

S32		AsciiToS32( const char * apTxt );


/* ################################################################################ */

#endif	/* INCLUDED_BASE_H */

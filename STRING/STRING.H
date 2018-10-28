#ifndef	INCLUDED_STRING_H
#define	INCLUDED_STRING_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

/*
enum
{
	eString_DynamicAllocFlag = 0x80000000
};
*/

#define eString_DynamicAllocFlag 0x80000000

typedef	struct	sString
{
	char *	mpChars;
	U32		mCharCountAndDynamicFlag;
} sString;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

/*
This should really only be used for parsing.
Look at using inplace fixed strings

	INIT - initialise data structure

*/


void		String_Init( sString * apString, const char * apChars );
void		String_Create2( sString * apString, const char * apChars0, const char * apChars1 );
void		String_DeInit( sString * apString );
void		String_Alloc( sString * apString, U32 aCharCount );


/*
	Accessors : get information about string
*/
#define		String_GetLength( aString )		((aString)->mCharCountAndDynamicFlag & ~eString_DynamicAllocFlag)
#define		String_GetCharCount( aString )	String_GetLength( aString )
#define		String_IsDynamic( aString )		((aString)->mCharCountAndDynamicFlag & eString_DynamicAllocFlag)


void		String_Copy(sString * apDst, const sString * apSrc);

void		String_Set(sString * apString, const char * apChars);
void		String_Set2(sString * apString, const char * apChars0, const char * apChars1);

void		String_SetStatic(sString * apString, const char * apChars, U32 aLength );
void		String_SetStaticNT(sString * apString, const char * apChars );

U8			String_IsEqual(const sString * apString0, const sString * apString1);
U8			String_IsEqualNT( const sString * apString0, const char * apString1 );

void		String_Prepend(sString * apString, const char * apChars);
void		String_Append( sString * apString, const char * apChars );
void		String_Cat( sString * apDst, const sString * apSrc0, const sString * apSrc1 );

void		String_CharInsert( sString * apString, const U16 aIndex, const U8 aChar );
void		String_CharRemove( sString * apString, const U16 aIndex );

void		String_QuoteTrim( sString * apString );
S32			String_ToS32( sString * apString );

#define		String_Action( apString, aAction )	{ if((apString)->mpChars) { char old =(apString)->mpChars[ String_GetLength(apString)]; (apString)->mpChars[ String_GetLength(apString)] = 0; aAction(apString); (apString)->mpChars[ String_GetLength(apString)] = old; } }

/*
This API is unsafe. Let's deprecate.
*/

void		String_StrAppend( char * apDst, const char * apAdd );
void		String_StrAppend2( char * apDst, const char * apAdd0, const char * apAdd1 );
void		String_StrCpy( char * apDst, const char * apSrc );
void		String_StrCpy2( char * apDst, const char * apSrc, const U32 aDstLen );
void		String_StrCat( char * apDst, const char * apSrc0, const char * apSrc1 );

/*
Const utitility functions acting on immutable data
*/

U8			String_StrCmp( const char * apStr0, const char * apStr1 );
U8			String_StrCmpi( const char * apStr0, const char * apStr1 );
U32			String_StrLen( const char * apString );
S32			String_ToValue( const char * apString );

const sTagString *	sTagString_GetFromString( const sString * apString, const sTagString * apTagStrings, const U32 aLimit );

/* ################################################################################ */

#endif	/* INCLUDED_STRING_H */

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"STRING.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

void	String_SetCharCount( sString * apString, U32 aCount )
{
	apString->mCharCountAndDynamicFlag = aCount;
}

void	String_Alloc(sString * apString, U32 aCount )
{
	apString->mpChars = (char*)mMEMCALLOC( aCount + 1 );
	apString->mCharCountAndDynamicFlag = aCount | eString_DynamicAllocFlag;
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Init( const char * apChars )
* ACTION   : String_Init
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Init( sString * apString, const char * apChars )
{
	if( apChars )
	{
		String_Alloc(apString, String_StrLen(apChars));
		String_StrCpy( apString->mpChars, apChars );
	}
	else
	{
		apString->mCharCountAndDynamicFlag = 0;
		apString->mpChars    = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Create2( const char * apChars0,const char * apChars1 )
* ACTION   : String_Create2
* CREATION : 06.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Create2( sString * apString, const char * apChars0,const char * apChars1 )
{
	if( apChars0 && apChars1 )
	{
		U32 lCount = String_StrLen( apChars0 ) + String_StrLen( apChars1 );
		String_Alloc(apString, lCount);
		String_StrCat( apString->mpChars, apChars0, apChars1 );
	}
	else
	{
		String_SetCharCount( apString, 0 );
		apString->mpChars    = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_DeInit( sString * apString )
* ACTION   : String_DeInit
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_DeInit( sString * apString )
{
	if( apString->mpChars )
	{
		if (apString->mCharCountAndDynamicFlag & eString_DynamicAllocFlag)
		{
			mMEMFREE(apString->mpChars);
		}
		apString->mpChars = 0;
	}
	apString->mCharCountAndDynamicFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Prepend( sString * apString,const char * apChars )
* ACTION   : String_Prepend
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Prepend(sString * apString, const char * apChars)
{
	if (apString && apChars)
	{
		U32 lCount = String_GetCharCount(apString) + String_StrLen(apChars);
		char * lpDst = (char*)mMEMCALLOC(lCount + 1);
		String_StrCat(lpDst, apChars, apString->mpChars );
		String_DeInit(apString);
		apString->mpChars = lpDst;
		apString->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Append( sString * apString,const char * apChars )
* ACTION   : String_Append
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Append( sString * apString,const char * apChars )
{
	if( apString && apChars )
	{
		U32 lCount = String_GetCharCount( apString ) + String_StrLen( apChars );
		char * lpDst = (char*)mMEMCALLOC(lCount+1);
		String_StrCat( lpDst, apString->mpChars, apChars );
		String_DeInit(apString);
		apString->mpChars = lpDst;
		apString->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Cat( sString * apDst,const sString * apSrc0,const sString * apSrc1 )
* ACTION   : String_Cat
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Cat( sString * apDst,const sString * apSrc0,const sString * apSrc1 )
{
	if( apDst && apSrc0 && apSrc1 )
	{
		const char * lpSrc0 = apSrc0->mpChars;
		const char * lpSrc1 = apSrc1->mpChars;
		if( lpSrc0 && lpSrc1 )
		{
			U32		lCount = String_GetCharCount(apSrc0) + String_GetCharCount(apSrc1);
			char *	lpDst;
			lpDst = mMEMCALLOC(lCount + 1);
			String_StrCat(lpDst, lpSrc0, lpSrc1);
			String_DeInit(apDst);
			apDst->mpChars = lpDst;
			apDst->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_CharInsert( sString * apString,const U16 aIndex,const U8 aChar )
* ACTION   : String_CharInsert
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_CharInsert( sString * apString,const U16 aIndex,const U8 aChar )
{
	U16		i;
	char *	lpChars;
	char *	lpDst;
	char *	lpSrc;

	if( apString )
	{
		U32 lCount = String_GetCharCount( apString );
		lpChars = (char*)mMEMCALLOC( lCount + 2 );
		lpDst   = lpChars;
		lpSrc   = apString->mpChars;

		i = 0;
		for( i=0; i<lCount; i++ )
		{
			if( aIndex == i )
			{
				*lpDst++ = aChar;
			}
			*lpDst++ = *lpSrc++;
		}

		if( aIndex >= lCount )
		{
			*lpDst++ = aChar;
		}
		*lpDst++ = 0;

		mMEMFREE( apString->mpChars );
		String_SetCharCount( apString, lCount + 1 );
		apString->mpChars = lpChars;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_CharRemove( sString * apString,const U16 aIndex )
* ACTION   : String_CharRemove
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_CharRemove( sString * apString,const U16 aIndex )
{
	U16		i;
	char *	lpChars;
	char *	lpDst;
	char *	lpSrc;

	if( apString )
	{
		U32 lCount = String_GetCharCount( apString );
		if( aIndex < lCount )
		{
			lpChars = (char*)mMEMCALLOC( lCount );
			lpDst   = lpChars;
			lpSrc   = apString->mpChars;

			i = 0;
			for( i=0; i<lCount; i++ )
			{
				if( aIndex != i )
				{
					*lpDst++ = *lpSrc;
				}
				lpSrc++;
			}

			*lpDst++ = 0;

			mMEMFREE( apString->mpChars );
			String_SetCharCount( apString, lCount - 1 );
			apString->mpChars = lpChars;
		}
	}
}

void		String_QuoteTrim( sString * apString )
{
	U32 length = String_GetLength( apString );
	if( ( length > 1 ) && ( '"' == apString->mpChars[ 0 ] || ( '\'' == apString->mpChars[ 0 ] ) ) && ( apString->mpChars[ 0 ] == apString->mpChars[ length - 1 ] ) )
	{
		length -= 2;
		if( String_IsDynamic( apString ) )
		{
			U32 i;
			for( i = 1; i < length; i++ )
				apString->mpChars[ i-1 ] = apString->mpChars[ i ];
			apString->mCharCountAndDynamicFlag = length | eString_DynamicAllocFlag;
		}
		else
		{
			apString->mpChars++;
			apString->mCharCountAndDynamicFlag = length;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Copy( sString * apDst,const sString * apSrc )
* ACTION   : String_Copy
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Copy( sString * apDst,const sString * apSrc )
{
	if( apDst && apSrc )
	{
		U32 lCount = String_GetCharCount( apSrc );
		const char * lpSrc = apSrc->mpChars;
		char * lpDst = (char*)mMEMCALLOC(lCount + 1);
		String_StrCpy( lpDst, lpSrc );

		String_DeInit(apDst);
		apDst->mpChars = lpDst;
		apDst->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Set( sString * apString,const char * apChars )
* ACTION   : String_Set
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Set( sString * apString,const char * apChars )
{
	if( apString && apChars )
	{
		if (apChars)
		{
			U32 lCount = String_StrLen(apChars);
			char * lpDst = (char*)mMEMCALLOC(lCount + 1);
			String_StrCpy(lpDst, apChars);

			String_DeInit(apString);
			apString->mpChars = lpDst;
			apString->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
		}
		else
		{
			String_DeInit(apString);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Set2( sString * apString,const char * apChars,const char * apChars )
* ACTION   : String_Set2
* CREATION : 06.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Set2( sString * apString,const char * apChars0,const char * apChars1 )
{
	if( apString )
	{
		if (apChars0 || apChars1)
		{
			U32 lCount = String_StrLen(apChars0) + String_StrLen(apChars1);
			char * lpDst = (char*)mMEMCALLOC(lCount+1);
			String_StrCat(lpDst, apChars0, apChars1);

			String_DeInit(apString);
			apString->mpChars = lpDst;
			apString->mCharCountAndDynamicFlag = lCount | eString_DynamicAllocFlag;
		}
		else
		{
			String_DeInit(apString);
		}
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_SetStatic(sString * apString, const char * apChars, U32 aLength)
* ACTION   : Creates string pointing to a static already existing string
* CREATION : 30.09.2018 PNK
*-----------------------------------------------------------------------------------*/

void		String_SetStatic(sString * apString, const char * apChars, U32 aLength)
{
	String_DeInit(apString);
	apString->mpChars = (char*)apChars;
	apString->mCharCountAndDynamicFlag = aLength;
}

void		String_SetStaticNT(sString * apString, const char * apChars)
{
	String_SetStatic(apString, apChars, String_StrLen(apChars));
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_IsEqual(const sString * apString0, const sString * apString1)
* ACTION   : Compares two (not null terminated) strings
* CREATION : 30.09.2018 PNK
*-----------------------------------------------------------------------------------*/

U8		String_IsEqual(const sString * apString0, const sString * apString1)
{
	if (apString0)
	{
		const char * lpSrc0 = apString0->mpChars;
		const char * lpSrc1;
		U32 lCount = String_GetLength(apString0);
		if (!apString1)
			return 0;
		if (lCount != String_GetLength(apString1))
			return 0;
		lpSrc1 = apString1->mpChars;
		while (lCount--)
		{
			if (*lpSrc0++ != *lpSrc1++)
				return 0;
		}
	}
	else if (apString1)
	{
		return(0);
	}
	return(1);
}

U8			String_IsEqualNT( const sString * apString0, const char * apString1 )
{
	if( apString0 )
	{
		const char * lpSrc0 = apString0->mpChars;
		const char * lpSrc1;
		U32 lCount = String_GetLength( apString0 );
		if( !apString1 )
			return 0;
		lpSrc1 = apString1;
		while( lCount-- )
		{
			if( *lpSrc0++ != *lpSrc1++ )
				return 0;
		}
		return 0 == *lpSrc1;
	}
	else if( apString1 )
	{
		return( 0 );
	}
	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrAppend( char * apDst, const char * apAdd )
* ACTION   : Appends apAdd to apDst
* CREATION : 26.08.2018 PNK
*-----------------------------------------------------------------------------------*/

void		String_StrAppend( char * apDst, const char * apAdd )
{
	if( apDst && apAdd )
	{
		while( *apDst ) apDst++;
		while( *apAdd ) *apDst++ = *apAdd++;
		*apDst++ =0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrAppend2( char * apDst, const char * apAdd0, const char * apAdd1 )
* ACTION   : Appends apAdd to apDst
* CREATION : 26.08.2018 PNK
*-----------------------------------------------------------------------------------*/

void		String_StrAppend2( char * apDst, const char * apAdd0, const char * apAdd1 )
{
	if( apDst && apAdd0 && apAdd1 )
	{
		while( *apDst ) apDst++;
		while( *apAdd0 ) *apDst++ = *apAdd0++;
		while( *apAdd1 ) *apDst++ = *apAdd1++;
		*apDst++ =0;
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrLen( const char * apString )
* ACTION   : String_StrLen
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	String_StrLen( const char * apString )
{
	U32	lSize;

	lSize = 0;
	if( apString )
	{
		while( *apString++ )
		{
			lSize++;
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrCpy( char * apDst,const char * apSrc )
* ACTION   : String_StrCpy
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_StrCpy( char * apDst,const char * apSrc )
{
	if( apDst && apSrc )
	{
		while( *apSrc )
		{
			*apDst++ = *apSrc++;
		}
		*apDst++ = 0;
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrCpy2( char * apDst, const char * apSrc, const U32 aDstLen )
* ACTION   : String_StrCpy2
* CREATION : 3.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	String_StrCpy2( char * apDst, const char * apSrc, const U32 aDstLen )
{
	U32	lIndex;
	if( apDst && apSrc )
	{
		lIndex = 1;
		while( (*apSrc) && (lIndex<aDstLen) )
		{
			*apDst++ = *apSrc++;
			lIndex++;
		}
		*apDst++ = 0;
	}
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : String_StrCat( char * apDst,const char * apSrc0,const char * apSrc1 )
* ACTION   : String_StrCat
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_StrCat( char * apDst,const char * apSrc0,const char * apSrc1 )
{
	if( apDst && apSrc0 && apSrc1 )
	{
		while( *apSrc0 )
		{
			*apDst++ = *apSrc0++;
		}
		while( *apSrc1 )
		{
			*apDst++ = *apSrc1++;
		}
		*apDst++ = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 String_StrCmp(char * apStr0,char * apStr1)
* ACTION   : String_StrCmp
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 String_StrCmp(const char * apStr0,const char * apStr1)
{
	U8	lVal0,lVal1;

	if( apStr0 && apStr1 )
	{
		while( *apStr0 && *apStr1 )
		{
			lVal0 = *apStr0++;
			lVal1 = *apStr1++;

			if( lVal0 != lVal1 )
				return( 1 );
		}
		if( *apStr0 != *apStr1 )
			return 1;
	}
	else if( apStr0 || apStr1 )
	{
		return( 1 );
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 String_StrCmpi(char * apStr0,char * apStr1)
* ACTION   : String_StrCmpi
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 String_StrCmpi(const char * apStr0,const char * apStr1)
{
	U8	lVal0,lVal1;

	if( apStr0 && apStr1 )
	{
		while( *apStr0 && *apStr1 )
		{
			lVal0 = *apStr0++;
			lVal1 = *apStr1++;

			if( (lVal0 >= 'a') && (lVal0 <='z') )
				lVal0 = (U8)(lVal0 + ( 'A'-'a' ));

			if( (lVal1 >= 'a') && (lVal1 <='z') )
				lVal1 = (U8)(lVal1 + ( 'A'-'a' ));

			if( lVal0 != lVal1 )
				return( 1 );
		}
	}
	else if( apStr0 || apStr1 )
	{
		return( 1 );
	}
	else
	{
		return( 0 );
	}

	return( (U8)(*apStr0 | * apStr1) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_ToValue( const char * apString )
* ACTION   : String_ToValue
* CREATION : 18.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S32	String_ToValue( const char * apString )
{
	S32	lVal;
	S32	lSign;
	U8	lMode;

	while( *apString == ' ' )
	{
		apString++;
	}

	if( *apString == '-' )
	{
		lSign = -1;
		apString++;
	}
	else
	{
		lSign = 1;
	}

	lMode = 0;
	if( *apString == '$' )
	{
		lMode = 1;
		apString++;
	}
	if( apString[ 1 ] == 'x' )
	{
		lMode = 1;
		apString += 2;
	}

	lVal = 0;
	if( lMode )
	{
		while(
				( (*apString >= '0') && (*apString <= '9') )
			||	( (*apString >= 'a') && (*apString <= 'f') )
			||	( (*apString >= 'A') && (*apString <= 'F') )
			)
		{
			lVal *= 16L;
			if( (*apString >= '0') && (*apString <= '9') )
			{
				lVal += *apString - '0';
			}
			else if( (*apString >= 'a') && (*apString <= 'f') )
			{
				lVal += (*apString - 'a')+10;
			}
			else if( (*apString >= 'A') && (*apString <= 'F') )
			{
				lVal += (*apString - 'A')+10;
			}
			apString++;
		}
	}
	else
	{
		while( (*apString >= '0') && (*apString <= '9') )
		{
			lVal *= 10L;
			lVal += *apString - '0';
			apString++;
		}
	}

	lVal *= lSign;
	return( lVal );
}


const sTagString *	sTagString_GetFromString( const sString * apString, const sTagString * apTagStrings, const U32 aLimit )
{
	const sTagString * end = &apTagStrings[ aLimit ];

	for( ; apTagStrings < end; apTagStrings++ )
		if( String_IsEqualNT( apString, apTagStrings->pString ) )
			return apTagStrings;

	return 0;
}

/* ################################################################################ */

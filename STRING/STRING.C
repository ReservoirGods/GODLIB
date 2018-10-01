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

/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Create( const char * apChars )
* ACTION   : String_Create
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Create( sString * apString, const char * apChars )
{
	if( apChars )
	{
		String_SetCharCount( apString, String_StrLen( apChars ) );
		apString->mpChars    = (char*)mMEMCALLOC( String_GetLength( apString ) + 1 );
		String_StrCpy( apString->mpChars, apChars );
	}
	else
	{
		String_SetCharCount( apString, 0 );
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
		String_SetCharCount( apString, lCount );
		apString->mpChars    = (char*)mMEMCALLOC( String_GetCharCount( apString ) + 1 );
		String_StrCat( apString->mpChars, apChars0, apChars1 );
	}
	else
	{
		String_SetCharCount( apString, 0 );
		apString->mpChars    = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Destroy( sString * apString )
* ACTION   : String_Destroy
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Destroy( sString * apString )
{
	if( apString->mpChars )
	{
		mMEMFREE( apString->mpChars );
		apString->mpChars = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Append( sString * apString,const char * apChars )
* ACTION   : String_Append
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Append( sString * apString,const char * apChars )
{
	char *	lpChars;

	if( apString && apChars )
	{
		U32 lCount = String_GetCharCount( apString ) + String_StrLen( apChars );
		lpChars               = apString->mpChars;
		String_SetCharCount( apString, lCount );
		apString->mpChars     = (char*)mMEMCALLOC( String_GetCharCount(apString) + 1 );
		String_StrCat( apString->mpChars, lpChars, apChars );
		mMEMFREE( lpChars );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Cat( sString * apDst,const sString * apSrc0,const sString * apSrc1 )
* ACTION   : String_Cat
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Cat( sString * apDst,const sString * apSrc0,const sString * apSrc1 )
{
	char *	lpSrc0;
	char *	lpSrc1;
	char *	lpDst;
	U32		lSize;

	if( apDst && apSrc0 && apSrc1 )
	{
		lpSrc0 = apSrc0->mpChars;
		lpSrc1 = apSrc1->mpChars;
		if( lpSrc0 && lpSrc1 )
		{
			lpDst  = apDst->mpChars;
			lSize  = String_GetCharCount( apSrc0 ) + String_GetCharCount( apSrc1 );
			String_SetCharCount( apDst, lSize );
			apDst->mpChars    = (char*)mMEMCALLOC( String_GetCharCount( apDst )+1 );
			String_StrCat( apDst->mpChars, lpSrc0, lpSrc1 );
			if( lpDst )
			{
				mMEMFREE( lpDst );
			}
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


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Clone( const sString * apString )
* ACTION   : String_Clone
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sString *	String_Clone( const sString * apString )
{
	sString *	lpString = 0;

	if( apString )
	{
		lpString = mMEMCALLOC( sizeof( sString ) );
		if( lpString )
			String_Create( lpString, apString->mpChars );
	}

	return( lpString );
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
		if( apDst->mpChars )
		{
			mMEMFREE( apDst->mpChars );
		}
		apDst->mpChars    = (char*)mMEMCALLOC( lCount + 1 );
		String_SetCharCount( apDst, lCount );
		String_StrCpy( apDst->mpChars, apSrc->mpChars );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Prepend( sString * apString,const char * apChars )
* ACTION   : String_Prepend
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Prepend( sString * apString,const char * apChars )
{
	char *	lpChars;

	if( apString && apChars )
	{
		U32 lCount = String_GetCharCount( apString ) + String_StrLen( apChars );
		lpChars               = apString->mpChars;
		String_SetCharCount( apString, lCount );
		apString->mpChars     = (char*)mMEMCALLOC( lCount + 1 );
		String_StrCat( apString->mpChars, apChars, lpChars );
		mMEMFREE( lpChars );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Update( sString * apString,const char * apChars )
* ACTION   : String_Update
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Update( sString * apString,const char * apChars )
{
	char *	lpChars;
	U32		lCount;

	if( apString && apChars )
	{
		lCount = String_StrLen( apChars );
		if( lCount == String_GetCharCount( apString ) )
		{
			String_StrCpy( apString->mpChars, apChars );
		}
		else
		{
			lpChars              = apString->mpChars;
			String_SetCharCount( apString, lCount );
			apString->mpChars    = (char*)mMEMCALLOC( String_GetCharCount(apString) + 1 );
			String_StrCpy( apString->mpChars, apChars );

			if( lpChars )
			{
				mMEMFREE( lpChars );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : String_Update2( sString * apString,const char * apChars,const char * apChars )
* ACTION   : String_Update2
* CREATION : 06.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	String_Update2( sString * apString,const char * apChars0,const char * apChars1 )
{
	char *	lpChars;
	U32		lCount;

	if( apString && apChars0 && apChars1 )
	{
		lCount  = String_StrLen( apChars0 );
		lCount += String_StrLen( apChars1 );
		if( lCount == String_GetCharCount(apString) )
		{
			String_StrCat( apString->mpChars, apChars0, apChars1 );
		}
		else
		{
			lpChars              = apString->mpChars;
			String_SetCharCount( apString, lCount );
			apString->mpChars    = (char*)mMEMCALLOC( String_GetCharCount(apString) + 1 );
			String_StrCat( apString->mpChars, apChars0, apChars1 );

			if( lpChars )
			{
				mMEMFREE( lpChars );
			}
		}
	}

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
			{
				return( 1 );
			}
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
			{
				lVal0 = (U8)(lVal0 + ( 'A'-'a' ));
			}
			if( (lVal1 >= 'a') && (lVal1 <='z') )
			{
				lVal1 = (U8)(lVal1 + ( 'A'-'a' ));
			}

			if( lVal0 != lVal1 )
			{
				return( 1 );
			}
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


/* ################################################################################ */


/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: JSON.C
::
:: JSON Parser
::
:: [c] 2018 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"JSON.H"

#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\LEXER\LEXER.H>
#include	<GODLIB\LINKLIST\GOD_LL.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\REFLECT\REFLECT.H>


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_Parse( sString * apSrc, sElementCollectionJSON  * apCollection )
* ACTION   : parses JSON text file into an array of elements
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_Parse( sString * apSrc, sElementCollectionJSON  * apCollection )
{
	U32				elementCount = 0;
	U32				index = 0;
	U16				depth = 0;
	sElementJSON *	pElements = 0;
	sLexerContext	lexer;

	Lexer_Init( &lexer, apSrc );
	while( Lexer_GetNextToken( &lexer ) )
		elementCount++;
/*	Lexer_DeInit( &lexer );*/

	if( elementCount )
	{
		pElements = mMEMCALLOC( sizeof( sElementJSON ) * elementCount );

		Lexer_Init( &lexer, apSrc );
		while( Lexer_GetNextToken( &lexer ) )
			pElements[ index++ ].mToken = lexer.mToken;
/*		Lexer_DeInit( &lexer );*/

		for( index = 0; index < elementCount;index++ )
		{
			char c = pElements[ index ].mToken.mpChars[ 0 ];
			pElements[ index ].mDepth = depth;
			if( '{' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ObjectBegin;
				depth++;
			}
			else if( '[' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ArrayBegin;
				depth++;

			}
			else if( '}' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ObjectEnd;
				depth--;
				pElements[ index ].mDepth = depth;
			}
			else if( ']' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_ArrayEnd;
				depth--;
				pElements[ index ].mDepth = depth;
			}
			else if( ':' == c || ',' == c )
			{
				pElements[ index ].mTypeFlags = eTypeJSON_Seperator;
			}
			else
			{
				U32 i2;
				for( i2 = index + 1; i2 < elementCount; i2++ )
				{
					if( ':' == pElements[ i2 ].mToken.mpChars[ 0 ] || '[' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
					}
					else if( '{' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_ObjectName;
						break;
					}
					else if( ',' == pElements[ i2 ].mToken.mpChars[ 0 ] || '}' == pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_PropertyValue;
						break;
					}
					else if( ':' != pElements[ i2 ].mToken.mpChars[ 0 ] && '[' != pElements[ i2 ].mToken.mpChars[ 0 ] )
					{
						pElements[ index ].mTypeFlags = eTypeJSON_PropertyName;
						break;
					}
				}
			}

		}
	}

	apCollection->mCount = elementCount;
	apCollection->mpElements = pElements;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_Destroy( sElementCollectionJSON * apCollection )
* ACTION   : release memory associated with JSON parsing
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_Destroy( sElementCollectionJSON * apCollection )
{
	mMEMFREE( apCollection->mpElements )
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementsTrim( const sElementCollectionJSON * apCollection )
* ACTION   : removes quotes from JSON keys/values
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementsTrim( const sElementCollectionJSON * apCollection )
{
	U32 i;
	for( i = 0; i < apCollection->mCount; i++ )
	{
		String_QuoteTrim( &apCollection->mpElements[ i ].mToken );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementsToStruct( const sElementCollectionJSON * apCollection, const struct sReflectType * apType, U32 * apIndex, void * apDest )
* ACTION   : convertes JSON elements into a concrete data structure based on TYPE system
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementsToStruct( const sElementCollectionJSON * apCollection, const struct sReflectType * apType, U32 * apIndex, void * apDest )
{
	U16 depth = apCollection->mpElements[ *apIndex ].mDepth;
	sString * lpPropName = 0;

	for( ; *apIndex < apCollection->mCount; (*apIndex)++ )
	{
		sElementJSON * lpE = &apCollection->mpElements[ *apIndex ];
		if( ( eTypeJSON_ObjectEnd == lpE->mTypeFlags ) && ( depth == lpE->mDepth ) )
			break;

		if( eTypeJSON_PropertyName == lpE->mTypeFlags )
			lpPropName = &lpE->mToken;

		if( eTypeJSON_PropertyValue == lpE->mTypeFlags && lpPropName )
			Reflect_SetData( apType, lpPropName, &lpE->mToken, apDest );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection )
* ACTION   : helper function to walk a JSON structure
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	JSON_ElementWalker_Init( sElementCollectionWalkerJSON * apWalker, const sElementCollectionJSON * apCollection )
{
	apWalker->mIndex = 0;
	String_Init( &apWalker->mObjectName, 0 );
	apWalker->mpCollection = apCollection;
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker )
* ACTION   : retrieves next object from JSON structure
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

U8		JSON_ElementWalker_GetNextObject( sElementCollectionWalkerJSON * apWalker )
{
	apWalker->mIndex++;
	for( ;apWalker->mIndex < apWalker->mpCollection->mCount; apWalker->mIndex++ )
	{
		const sElementJSON * lpE = &apWalker->mpCollection->mpElements[ apWalker->mIndex ];
		if( eTypeJSON_ObjectBegin == lpE->mTypeFlags )
			return 1;
		if( eTypeJSON_ObjectName == lpE->mTypeFlags )
			apWalker->mObjectName = lpE->mToken;
	}
	return 0;
}

/* ################################################################################ */
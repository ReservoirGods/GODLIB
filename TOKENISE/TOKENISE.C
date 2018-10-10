/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"TOKENISE.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/HASHLIST/HASHLIST.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<STRING.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eTOKENISER_MODE_FINDCHUNK,
	eTOKENISER_MODE_FINDOPEN,
	eTOKENISER_MODE_CHUNK,
	eTOKENISER_MODE_FINDCLOSE,

	eTOKENISER_MODE_LIMIT
};


/* ###################################################################################
#  DATA
################################################################################### */


typedef	struct	sTokeniser
{
	const char *				mpText;
	U32							mTextSize;
	U32							mOffset;
	const sTokeniserHandler *	mpHandlers;
	const sTokeniserHandler *	mpHandler;
	U16							mMode;
	U16							mHandlerCount;
	U16							mHandlerIndex;
} sTokeniser;


typedef	struct sTokeniserLinkListItem
{
	U32								mData;
	struct sTokeniserLinkListItem *	mpNext;
} sTokeniserLinkListItem;

sTokeniserBuilder *	gpTokeniserBuilder;
sString 			gTokeniserNameSpace;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

const sTokeniserHandler *	Tokeniser_GetHandler( sTokeniser * apTokeniser, const char * apString );
S32							TokeniserArgs_GetInt( const sTokeniserArgs * apArgs );
U8 *						Tokeniser_GetListItem( const sTokeniserChunkContainer * apChunkContainer, const char * apName );
void						Tokeniser_GrabData( const sTokeniserStructMember * apMember, const sTokeniserArgs * apArgs, void * apDst );
void						Tokeniser_ShowType(const U16 aType,const U16 aSubType, const U32 aCountOffset, sTokeniserTagStringList * apList, void * apData,char * apString);
void						Tokeniser_ShiftPtr( U32 * apPtr, sTokeniserBuilder * apBuilder );

void						Tokeniser_Pass1_Init( const char * apTitle, const U16 aIndex, const char * apName );
void						Tokeniser_Pass1_DeInit( void );
void						Tokeniser_Pass1_Var( sTokeniserArgs * apArgs );

void						Tokeniser_Pass2_Init( const char * apTitle, const U16 aIndex, const char * apName );
void						Tokeniser_Pass2_DeInit( void );
void						Tokeniser_Pass2_Var( sTokeniserArgs * apArgs );

void						Tokeniser_Pass3_Init( const char * apTitle, const U16 aIndex, const char * apName );
void						Tokeniser_Pass3_DeInit( void );
void						Tokeniser_Pass3_Var( sTokeniserArgs * apArgs );


sTokeniserHandler	gTokenesierPass1 =
{
	Tokeniser_Pass1_Init,
	0,
	0,
	0,
	"*"
};

sTokeniserHandler	gTokenesierPass2 =
{
	Tokeniser_Pass2_Init,
	Tokeniser_Pass2_DeInit,
	Tokeniser_Pass2_Var,
	0,
	"*"
};

sTokeniserHandler	gTokenesierPass3 =
{
	Tokeniser_Pass3_Init,
	Tokeniser_Pass3_DeInit,
	Tokeniser_Pass3_Var,
	0,
	"*"
};

void	StringList_Validate( void );

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_Parse( const char * apText,const U32 aSize,const sTokeniserHandler * apHandlers,const U32 aHandlerCount )
* ACTION   : Tokeniser_Parse
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_Parse( const char * apText,const U32 aSize,const sTokeniserHandler * apHandlers,const U16 aHandlerCount )
{
	sTokeniserArgs	lArgs;
	sTokeniser		lTokeniser;
	sString 		lName;

	lTokeniser.mpText        = apText;
	lTokeniser.mOffset       = 0;
	lTokeniser.mTextSize     = aSize;
	lTokeniser.mHandlerCount = aHandlerCount;
	lTokeniser.mpHandlers    = apHandlers;
	lTokeniser.mpHandler     = 0;
	lTokeniser.mMode         = eTOKENISER_MODE_FINDCHUNK;
	lTokeniser.mHandlerCount = aHandlerCount;
	String_Init( &gTokeniserNameSpace,  "" );
	String_Init( &lName, "NAME" );

	while( lTokeniser.mOffset < lTokeniser.mTextSize )
	{
		lTokeniser.mOffset = TokeniserArgs_Build( &lArgs, lTokeniser.mpText, lTokeniser.mOffset, lTokeniser.mTextSize );
		switch( lTokeniser.mMode )
		{
		case	eTOKENISER_MODE_FINDCHUNK:
			if( lArgs.mArgCount > 2 )
			{
				if( (!String_StrCmpi( lArgs.mpStrings[0], "<")) &&
					(!String_StrCmpi( lArgs.mpStrings[ lArgs.mArgCount-1], ">")) )
				{
					if( !String_StrCmpi( "NAMESPACE", lArgs.mpStrings[ 1 ] ) )
					{
						if( (lArgs.mArgCount >= 6) &&
							(!String_StrCmpi( lArgs.mpStrings[2], "name" )) &&
							(!String_StrCmpi( lArgs.mpStrings[3], "=")) )
						{
							String_Set( &gTokeniserNameSpace,  lArgs.mpStrings[4] );
						}
						else
						{
							String_Set( &gTokeniserNameSpace,  "" );
						}
					}
					else
					{
						if( (lArgs.mArgCount >= 6) &&
							(!String_StrCmpi( lArgs.mpStrings[2], "name" )) &&
							(!String_StrCmpi( lArgs.mpStrings[3], "=")) )
						{
							String_Set2( &lName, gTokeniserNameSpace.mpChars, lArgs.mpStrings[4] );
						}
						else
						{
							String_Set2( &lName, gTokeniserNameSpace.mpChars, "none" );
						}
						lTokeniser.mpHandler = Tokeniser_GetHandler( &lTokeniser, lArgs.mpStrings[1] );
						if( (lTokeniser.mpHandler) && (lTokeniser.mpHandler->mpOnChunkInit) )
						{
							lTokeniser.mpHandler->mpOnChunkInit( lArgs.mpStrings[1], lTokeniser.mHandlerIndex, lName.mpChars );
						}

						lTokeniser.mMode = eTOKENISER_MODE_FINDOPEN;
					}
				}
			}
			break;

		case	eTOKENISER_MODE_FINDOPEN:
			if( lArgs.mArgCount )
			{
				if( lArgs.mpStrings[ 0 ][ 0 ] == '{' )
				{
					lTokeniser.mMode = eTOKENISER_MODE_CHUNK;
				}
			}
			break;

		case	eTOKENISER_MODE_CHUNK:
			if( lArgs.mArgCount )
			{
				if( lArgs.mpStrings[ 0 ][ 0 ] == '}' )
				{
					if( lTokeniser.mpHandler )
					{
						if( lTokeniser.mpHandler->mpOnChunkDeInit )
						{
							lTokeniser.mpHandler->mpOnChunkDeInit();
						}
					}
					lTokeniser.mMode = eTOKENISER_MODE_FINDCHUNK;
				}
				else
				{
					if( lArgs.mArgCount >= 3 )
					{
						if( lArgs.mpStrings[ 1 ][ 0 ] == '=' )
						{
							if( lTokeniser.mpHandler )
							{
								if( lTokeniser.mpHandler->mpOnVar )
								{
									lTokeniser.mpHandler->mpOnVar( &lArgs );
								}
							}
						}
					}

					if( lTokeniser.mpHandler->mpOnLine )
					{
						lTokeniser.mpHandler->mpOnLine( &lArgs );
					}
				}
			}
			break;

		case	eTOKENISER_MODE_FINDCLOSE:
			if( lArgs.mArgCount )
			{
				if( lArgs.mpStrings[ 0 ][ 0 ] == '}' )
				{
					lTokeniser.mMode     = eTOKENISER_MODE_FINDCHUNK;
					lTokeniser.mpHandler = 0;
				}
			}
			break;
		}
	}

	String_DeInit( &lName );
	String_DeInit( &gTokeniserNameSpace );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetInt( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetInt
* CREATION : 06.02.2004 PNK
*-----------------------------------------------------------------------------------*/

S32	TokeniserArgs_GetInt( const sTokeniserArgs * apArgs )
{
	S32		lVal;
	S32		lNum;
	U16		lNegFlag;
	U16		lHexFlag;
	U8 *	lpString;

	lVal = 0;
	if( apArgs->mArgCount >= 3 )
	{
		lpString = (U8*)apArgs->mpStrings[ 2 ];
		lNegFlag = 0;
		while( (*lpString) && (*lpString<'-') )
		{
			lpString++;
		}
		if( '-' == *lpString )
		{
			lNegFlag = 1;
		}
		else
		{
			lNegFlag = 0;
		}

		lHexFlag = 0;

		while( (*lpString) && (*lpString<'0') )
		{
			if( '$' == *lpString )
			{
				lHexFlag = 1;
			}
			lpString++;
		}
		if( (lpString[ 0 ] == '0') && (lpString[ 1 ] == 'x') )
		{
			lHexFlag = 1;
		}

		lNum = 0;

		if( lHexFlag )
		{
			while( (*lpString) &&
				( ((*lpString>='0') && (*lpString<='9')) ||
				  ((*lpString>='a') && (*lpString<='f')) ||
				  ((*lpString>='A') && (*lpString<='F')) )
				)
			{
				lNum  = *lpString++;
				if( (*lpString>='0') && (*lpString<='9') )
				{
					lNum -= '0';
				}
				if( (*lpString>='a') && (*lpString<='f') )
				{
					lNum -= ('a'-10);
				}
				if( (*lpString>='A') && (*lpString<='F') )
				{
					lNum -= ('A'-10);
				}
				lVal *= 16L;
				lVal += lNum;
			}
		}
		else
		{
			while( (*lpString) && (*lpString>='0') && (*lpString<='9') )
			{
				lNum  = *lpString++;
				lNum -= '0';
				lVal *= 10L;
				lVal += lNum;
			}
		}

		if( lNegFlag )
		{
			lVal = -lVal;
		}
	}

	return( lVal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetS32( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetS32
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

S32	TokeniserArgs_GetS32( const sTokeniserArgs * apArgs )
{
	return( TokeniserArgs_GetInt( apArgs ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetS16( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetS16
* CREATION : 31.01.2004 PNK
*-----------------------------------------------------------------------------------*/

S16	TokeniserArgs_GetS16( const sTokeniserArgs * apArgs )
{
	return( (S16)TokeniserArgs_GetS32( apArgs ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetU16( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetU16
* CREATION : 31.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	TokeniserArgs_GetU16( const sTokeniserArgs * apArgs )
{
	return( (U16)TokeniserArgs_GetU32( apArgs ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetU32( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetU32
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	TokeniserArgs_GetU32( const sTokeniserArgs * apArgs )
{
	return( (U32)TokeniserArgs_GetInt( apArgs ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetFP32( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_GetFP32
* CREATION : 06.02.2004 PNK
*-----------------------------------------------------------------------------------*/

FP32	TokeniserArgs_GetFP32( const sTokeniserArgs * apArgs )
{
	FP32	lVal;

	lVal = 0.f;

	if( apArgs->mArgCount >= 3 )
	{
		lVal = (FP32)atof( apArgs->mpStrings[ 2 ] );
	}

	return( lVal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetTagID( const sTokeniserArgs * apArgs,const sTagString * apTags,const U32 aTagLimit )
* ACTION   : TokeniserArgs_GetTagID
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	TokeniserArgs_GetTagID( const sTokeniserArgs * apArgs,const sTagString * apTags,const U32 aTagLimit )
{
	U32	lVal;

	if( apArgs->mArgCount >= 3 )
	{
		lVal = sTagString_GetID( apArgs->mpStrings[ 2 ], apTags, aTagLimit );
	}
	else
	{
		lVal = 0;
	}

	return( lVal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_GetTagValue( const sTokeniserArgs * apArgs,const sTagValue * apTags,const U32 aTagLimit )
* ACTION   : TokeniserArgs_GetTagValue
* CREATION : 30.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	TokeniserArgs_GetTagValue( const sTokeniserArgs * apArgs,const sTagValue * apTags,const U32 aTagLimit )
{
	U32	lVal;

	if( apArgs->mArgCount >= 3 )
	{
		lVal = atoi( apArgs->mpStrings[ 2 ] );
		lVal = sTagValue_GetValue( lVal, apTags, aTagLimit );
	}
	else
	{
		lVal = 0;
	}

	return( lVal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 TokeniserArgs_Build(sTokeniserArgs * apArgs,const char * apText,U32 aOffset,const U32 aTextSize)
* ACTION   : TokeniserArgs_Build
* CREATION : 02.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32 TokeniserArgs_Build(sTokeniserArgs * apArgs,const char * apText,U32 aOffset,const U32 aTextSize)
{
	U16	lStringOff;
	U16	lArgLen;
	U8	lExitFlag;
	U8	lChar;
	U8	lQuote;


	apArgs->mArgCount = 0;
	apArgs->mStringSpace[ 0 ] = 0;
	apArgs->mpStrings[ 0 ] = &apArgs->mStringSpace[ 0 ];

	lQuote           = 0;
	lStringOff       = 0;
	lExitFlag        = 0;
	lArgLen          = 0;
	apArgs->mLineLen = 0;

	while( !lExitFlag )
	{
		if( lStringOff >= eTOKEN_STRINGSPACE_LIMIT )
		{
			lExitFlag = 1;
		}
		else if( aOffset >= aTextSize )
		{
			lExitFlag = 1;
		}
		else
		{
			lChar = apText[ aOffset ];
			aOffset++;

			if( apArgs->mLineLen < eTOKEN_STRINGSPACE_LIMIT )
			{
				apArgs->mLine[ apArgs->mLineLen ] = lChar;
				apArgs->mLineLen++;
			}

			switch( lChar )
			{
			case	0:
			case	10:
			case	13:
				lExitFlag = 1;
				break;
			case	'>':
			case	'<':
			case	'}':
			case	'{':
			case	'=':
				if( lQuote )
				{
					apArgs->mStringSpace[ lStringOff++ ] = lChar;
					lArgLen++;
				}
				else
				{
					if( lArgLen )
					{
						apArgs->mStringSpace[ lStringOff++ ] = 0;
						lArgLen = 0;
					}
					apArgs->mpStrings[ apArgs->mArgCount ] = &apArgs->mStringSpace[ lStringOff ];
					apArgs->mStringSpace[ lStringOff++ ] = lChar;
					apArgs->mStringSpace[ lStringOff++ ] = 0;
					apArgs->mArgCount++;
				}
				break;

			case	8:
			case	9:
			case	' ':
			case	',':
				if( lQuote )
				{
					apArgs->mStringSpace[ lStringOff++ ] = lChar;
					lArgLen++;
				}
				else if( lArgLen )
				{
					apArgs->mStringSpace[ lStringOff++ ] = 0;
					lArgLen = 0;
				}
				break;

			case	'"':
			case	'\'':
				if( lQuote == lChar )
				{
					lQuote = 0;
					if( lArgLen )
					{
						apArgs->mStringSpace[ lStringOff ] = 0;
						lStringOff++;
						lArgLen = 0;
					}
				}
				else
				{
					lQuote = lChar;
				}
				break;
			default:
				if( !lArgLen )
				{
					apArgs->mpStrings[ apArgs->mArgCount ] = &apArgs->mStringSpace[ lStringOff ];
					apArgs->mArgCount++;
				}
				apArgs->mStringSpace[ lStringOff++ ] = lChar;
				lArgLen++;
				break;
			}
		}
	}

	if( apArgs->mLineLen < eTOKEN_STRINGSPACE_LIMIT )
	{
		apArgs->mLine[ apArgs->mLineLen ] = 0;
	}

	if( lArgLen )
	{
		apArgs->mStringSpace[ lStringOff ] = 0;
	}

	return( aOffset );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_IsChunkHeader( const sTokeniserArgs * apArgs )
* ACTION   : TokeniserArgs_IsChunkHeader
* CREATION : 24.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	TokeniserArgs_IsChunkHeader( const sTokeniserArgs * apArgs )
{
	U8	lRes;

	lRes = 0;

	if( apArgs )
	{
		if( apArgs->mArgCount >= 6 )
		{
			if( ( apArgs->mpStrings[0][0] == '<' ) &&
				( apArgs->mpStrings[3][0] == '=' ) &&
				( apArgs->mpStrings[5][0] == '>' ) )
			{
				lRes = 1;
			}
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetHandler( sTokeniser * apTokeniser,const char * apString )
* ACTION   : Tokeniser_GetHandler
* CREATION : 01.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserHandler *	Tokeniser_GetHandler( sTokeniser * apTokeniser,const char * apString )
{
	apTokeniser->mHandlerIndex = 0;
	apTokeniser->mpHandler     = 0;

	while( (apTokeniser->mHandlerIndex < apTokeniser->mHandlerCount) && (!apTokeniser->mpHandler) )
	{
		if( !String_StrCmpi( "*", apTokeniser->mpHandlers[ apTokeniser->mHandlerIndex ].mpChunkName ) )
		{
			apTokeniser->mpHandler = &apTokeniser->mpHandlers[ apTokeniser->mHandlerIndex ];
		}
		else if( !String_StrCmpi( apString, apTokeniser->mpHandlers[ apTokeniser->mHandlerIndex ].mpChunkName ) )
		{
			apTokeniser->mpHandler = &apTokeniser->mpHandlers[ apTokeniser->mHandlerIndex ];
		}
		else
		{
			apTokeniser->mHandlerIndex++;
		}
	}

	return( apTokeniser->mpHandler );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void TokeniserArrayList_Init(sTokeniserArrayList * apList)
* ACTION   : TokeniserArrayList_Init
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void TokeniserArrayList_Init(sTokeniserArrayList * apList)
{
	if( apList )
	{
		apList->mCount   = 0;
		apList->mpArrays = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void TokeniserArrayList_DeInit(sTokeniserArrayList * apList)
* ACTION   : TokeniserArrayList_DeInit
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void TokeniserArrayList_DeInit(sTokeniserArrayList * apList)
{
	sTokeniserArray *	lpArray;
	sTokeniserArray *	lpArrayNext;

	if( apList )
	{
		lpArray = apList->mpArrays;
		while( lpArray )
		{
			lpArrayNext = lpArray->mpNext;
			mMEMFREE( lpArray );
			lpArray     = lpArrayNext;
		}
		apList->mCount   = 0;
		apList->mpArrays = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sTokeniserArray * TokeniserArrayList_ArrayCreate(sTokeniserArrayList * apList)
* ACTION   : TokeniserArrayList_ArrayCreate
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sTokeniserArray * TokeniserArrayList_ArrayCreate(sTokeniserArrayList * apList)
{
	sTokeniserArray *	lpArray;

	lpArray = (sTokeniserArray*)mMEMCALLOC( sizeof(sTokeniserArray) );

	if( apList && lpArray )
	{
		lpArray->mpNext  = apList->mpArrays;
		apList->mpArrays = lpArray;
		apList->mCount++;
	}

	return( lpArray );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void TokeniserArrayList_ArrayDestroy(sTokeniserArrayList * apList,sTokeniserArray * apArray)
* ACTION   : TokeniserArrayList_ArrayDestroy
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void TokeniserArrayList_ArrayDestroy(sTokeniserArrayList * apList,sTokeniserArray * apArray)
{
	sTokeniserArray *	lpArray;
	sTokeniserArray *	lpArrayNext;
	sTokeniserArray **	lppArrayLast;

	if( apList )
	{
		lpArray      = apList->mpArrays;
		lppArrayLast =&apList->mpArrays;
		while( lpArray )
		{
			lpArrayNext = lpArray->mpNext;
			if( lpArray == apArray )
			{
				*lppArrayLast = lpArrayNext;
				mMEMFREE( lpArray );
				return;
			}
			lppArrayLast =&lpArray->mpNext;
			lpArray      = lpArrayNext;
		}
		apList->mCount   = 0;
		apList->mpArrays = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 TokeniserArray_GetSize(sTokeniserArray * apArray)
* ACTION   : TokeniserArray_GetSize
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32 TokeniserArray_GetSize(sTokeniserArray * apArray)
{
	U32	lSize;

	if( apArray )
	{
		lSize  = apArray->mCount;
		lSize *= apArray->mItemSize;
	}
	else
	{
		lSize = 0;
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 TokeniserArrayList_GetSize(sTokeniserArrayList * apList)
* ACTION   : TokeniserArrayList_GetSize
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32 TokeniserArrayList_GetSize(sTokeniserArrayList * apList)
{
	U32					lSize;
	sTokeniserArray *	lpArray;

	lSize = 0;

	if( apList )
	{
		lpArray = apList->mpArrays;
		while( lpArray )
		{
			lSize += TokeniserArray_GetSize( lpArray );
			lpArray = lpArray->mpNext;
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void TokeniserArrayList_Serialise(sTokeniserArrayList * apList,void * apDst)
* ACTION   : TokeniserArrayList_Serialise
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void TokeniserArrayList_Serialise(sTokeniserArrayList * apList,void * apDst)
{
	U32					lSize;
	U32					lOff;
	sTokeniserArray *	lpArray;
	U8 *				lpMem;

	if( apList )
	{
		lpMem   = (U8*)apDst;
		lpArray = apList->mpArrays;
		lOff    = 0;
		while( lpArray )
		{
			lSize   = TokeniserArray_GetSize( lpArray );
			if( lSize )
			{
				Memory_Copy( lSize, lpArray->mpItems, &lpMem[ lOff ] );
				mMEMFREE( lpArray->mpItems );
				lpArray->mpItems = &lpMem[ lOff ];
				lOff += lSize;
			}
			lpArray = lpArray->mpNext;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_Process( const char * apText,const U32 aSize,sTokeniserInfo * apInfo )
* ACTION   : Tokeniser_Process
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sTokeniserBuilder *	Tokeniser_Init( const char * apText, const U32 aSize, sTokeniserInfo * apInfo )
{
	sTokeniserChunkContainer *	lpCon;
	sTokeniserChunkDef *		lpChunkDef;
	sTokeniserStructMember *	lpMembers;
	sTokeniserArray *			lpArray;
	U32							lOff;
	U32							lSize;
	U16							i;


	gpTokeniserBuilder = (sTokeniserBuilder*)mMEMCALLOC( sizeof(sTokeniserBuilder) );

	gpTokeniserBuilder->mpInfo = apInfo;
	gpTokeniserBuilder->mpText = apText;
	gpTokeniserBuilder->mpInfo = apInfo;

	String_Init( &gpTokeniserBuilder->mChunkName, "none" );
	String_Init( &gpTokeniserBuilder->mChunkTitle, "none" );
/*	gpTokeniserBuilder->mpChunkTitleName = String_Init( "none" );
*/
	StringList_Init( &gpTokeniserBuilder->mStringList );

	gpTokeniserBuilder->mpChunkContainers = (sTokeniserChunkContainer*)mMEMCALLOC( sizeof(sTokeniserChunkContainer) * apInfo->mChunkCount );
	gpTokeniserBuilder->mpContainer       = (U8*)mMEMCALLOC( apInfo->mContainerSize );

	Tokeniser_Parse( apText, aSize, &gTokenesierPass1, 1 );

	lpChunkDef = apInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lpCon              = &gpTokeniserBuilder->mpChunkContainers[ lpChunkDef->mIndex ];
			lpCon->mIndex      = 0;
			if( lpCon->mCount )
			{
				lpCon->mpHashes    = (U32*)mMEMCALLOC( lpCon->mCount * 4 );
				lSize = lpChunkDef->mStructSize;
				lSize *= lpCon->mCount;
				lpCon->mpData      = (U8*)mMEMCALLOC( lSize );
				lpCon->mpDataMax   = &lpCon->mpData[ lSize ];
				lpCon->mStructSize = lpChunkDef->mStructSize;

				lpMembers = lpChunkDef->mpStructDef;
				if( lpMembers )
				{
					while( lpMembers->mpString )
					{
						if( 1 == lpMembers->mArrayFlag )
						{
							lOff = 0;
							for( i=0; i<lpCon->mCount; i++ )
							{
								lpArray         = TokeniserArrayList_ArrayCreate( &gpTokeniserBuilder->mArrayList );
								lpArray->mCount = 0;
								switch( lpMembers->mType )
								{
								case	eTOKENISER_TYPE_CHAR:
								case	eTOKENISER_TYPE_U8:
								case	eTOKENISER_TYPE_S8:
									lpArray->mItemSize = 1;
									break;
								case	eTOKENISER_TYPE_U16:
								case	eTOKENISER_TYPE_S16:
									lpArray->mItemSize = 2;
									break;
								case	eTOKENISER_TYPE_U32:
								case	eTOKENISER_TYPE_S32:
								case	eTOKENISER_TYPE_FP32:
								default:
									lpArray->mItemSize = 4;
								}
								*(sTokeniserArray**)&lpCon->mpData[ lpMembers->mOffset + lOff ] = lpArray;
								lOff += lpChunkDef->mStructSize;
							}
						}
						lpMembers++;
					}
				}
			}
			lpChunkDef++;
		}
	}


	Tokeniser_Parse( apText, aSize, &gTokenesierPass2, 1 );

	lpChunkDef = apInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lpCon              = &gpTokeniserBuilder->mpChunkContainers[ lpChunkDef->mIndex ];
			lpCon->mIndex      = 0;
			lpChunkDef++;
		}
	}

	Tokeniser_Parse( apText, aSize, &gTokenesierPass3, 1 );

	String_DeInit( &gpTokeniserBuilder->mChunkName );
	String_DeInit( &gpTokeniserBuilder->mChunkTitle );
/*	String_DeInit( gpTokeniserBuilder->mpChunkTitleName );*/

	return( gpTokeniserBuilder );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_DeInit(sTokeniserBuilder * apBuilder)
* ACTION   : Tokeniser_DeInit
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_DeInit(sTokeniserBuilder * apBuilder)
{
	sTokeniserChunkContainer *	lpCon;
	sTokeniserChunkDef *		lpChunkDef;


	lpChunkDef = apBuilder->mpInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lpCon = &apBuilder->mpChunkContainers[ lpChunkDef->mIndex ];
			if( lpCon->mpHashes )
			{
				mMEMFREE( lpCon->mpHashes );
				lpCon->mpHashes = 0;
			}
			if( lpCon->mpData )
			{
				mMEMFREE( lpCon->mpData );
				lpCon->mpData = 0;
			}
			lpChunkDef++;
		}
	}

	mMEMFREE( apBuilder->mpContainer );
	mMEMFREE( apBuilder->mpChunkContainers );

	StringList_DeInit( &apBuilder->mStringList );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void * Tokeniser_Serialise(sTokeniserBuilder * apBuilder)
* ACTION   : Tokeniser_Serialise
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void * Tokeniser_Serialise(sTokeniserBuilder * apBuilder)
{
	U32							lSize;
	U32							lArraysSize;
	U32							lChunkSize;
	U32							lChunksSize;
	U32							lHeaderSize;
	U32							lStringsSize;
	U16							lCount;
	U16							i,j;
	U8 *						lpItem;
	U8 *						lpData;
	U8 *						lpMem;
	U32							lOff;
	sString *					lpString;
	sTokeniserArray *			lpArray;
	sTokeniserChunkDef *		lpChunkDef;
	sTokeniserStructMember *	lpMembers;

	lHeaderSize  = apBuilder->mpInfo->mContainerSize;
	lStringsSize = StringList_GetStringsSize( &apBuilder->mStringList );
	lArraysSize  = TokeniserArrayList_GetSize( &apBuilder->mArrayList );


	lChunksSize = 0;
	lpChunkDef  = apBuilder->mpInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lChunksSize += lpChunkDef->mStructSize * apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mCount;
			lpChunkDef++;
		}
	}

	lSize  = lHeaderSize;
	lSize += lArraysSize;
	lSize += lChunksSize;
	lSize += lStringsSize;


	lpMem = (U8*)mMEMCALLOC( lSize );
	if( !lpMem )
		return 0;

	lOff  = 0;

	Memory_Copy( apBuilder->mpInfo->mContainerSize, apBuilder->mpContainer, lpMem );
	lOff = lHeaderSize;


	lpChunkDef  = apBuilder->mpInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lChunkSize = lpChunkDef->mStructSize * apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mCount;
			lpData     = apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mpData;
			Memory_Copy( lChunkSize, lpData, &lpMem[ lOff ] );

			apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mRelocOffset  = (S32)&lpMem[ lOff ];
			apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mRelocOffset -= (S32)lpData;

			*(U32*)&lpMem[ lpChunkDef->mPtrOffset   ] = (U32)&lpMem[ lOff ];
			*(U16*)&lpMem[ lpChunkDef->mCountOffset ] = (U16)apBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mCount;
			lOff      += lChunkSize;
			lpChunkDef++;
		}
	}

	TokeniserArrayList_Serialise( &apBuilder->mArrayList, &lpMem[ lOff ] );
	lOff += lArraysSize;
	DebugLog_Printf0( "stringlist serialise" );
	StringList_Validate();
	StringList_StringsSerialiseTo( &apBuilder->mStringList, &lpMem[ lOff ] );


	lpChunkDef  = apBuilder->mpInfo->mpChunkDefs;
	if( lpChunkDef )
	{
		while( lpChunkDef->mpChunkName )
		{
			lpData = *(U8**)&lpMem[ lpChunkDef->mPtrOffset   ];
			lCount = *(U16*)&lpMem[ lpChunkDef->mCountOffset ];
			for( i=0; i<lCount; i++ )
			{
				lpMembers = lpChunkDef->mpStructDef;
				while( lpMembers->mpString )
				{
					if( 1 == lpMembers->mArrayFlag )
					{
						lpArray = *(sTokeniserArray**)&lpData[ lpMembers->mOffset ];
						lpItem  = lpArray->mpItems;
						*(U8**)&lpData[ lpMembers->mOffset ] = lpItem;
						for( j=0; j<lpArray->mCount; j++ )
						{
							switch( lpMembers->mType )
							{
							case	eTOKENISER_TYPE_CHUNKNAME:
							case	eTOKENISER_TYPE_STRING:
								lpString = *(sString**)lpItem;
								if( lpString )
								{
									*(char**)lpItem = lpString->mpChars;
								}
								break;
							case	eTOKENISER_TYPE_REFERENCE:
								Tokeniser_ShiftPtr( (U32*)lpItem, apBuilder );
								break;
							}
							lpItem += lpArray->mItemSize;
						}
						*(U16*)&lpData[ lpMembers->mCountOffset ] = lpArray->mCount;
					}
					else if( !lpMembers->mArrayFlag )
					{
						switch( lpMembers->mType )
						{
						case	eTOKENISER_TYPE_CHUNKNAME:
						case	eTOKENISER_TYPE_STRING:
							lpString = *(sString**)&lpData[ lpMembers->mOffset ];
							if( lpString )
							{
								*(char**)&lpData[ lpMembers->mOffset ] = lpString->mpChars;
							}
							break;
						case	eTOKENISER_TYPE_REFERENCE:
							if( *(U32*)&lpData[ lpMembers->mOffset ] )
							{
								*(S32*)&lpData[ lpMembers->mOffset ] += apBuilder->mpChunkContainers[ lpMembers->mSubType ].mRelocOffset;
							}
							break;
						}
					}
					lpMembers++;
				}
				lpData += lpChunkDef->mStructSize;
			}
			lpChunkDef++;
		}
	}

	return( lpMem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_ShiftPtr(U32 * apPtr,sTokeniserBuilder * apBuilder)
* ACTION   : Tokeniser_ShiftPtr
* CREATION : 27.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_ShiftPtr(U32 * apPtr,sTokeniserBuilder * apBuilder)
{
	U32	lVal;
	U16	i;

	if( apPtr )
	{
		lVal = *apPtr;
		if( lVal )
		{
			for( i=0; i<apBuilder->mpInfo->mChunkCount; i++ )
			{
				if( (lVal >= (U32)apBuilder->mpChunkContainers[ i ].mpData) &&
					(lVal <  (U32)apBuilder->mpChunkContainers[ i ].mpDataMax) )
				{
					lVal  = (U32)(lVal + apBuilder->mpChunkContainers[ i ].mRelocOffset);
					*apPtr= lVal;
					return;
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_Pass1_Init( const char * apTitle,const U16 aIndex,const char * apName )
* ACTION   : Tokeniser_Pass1_Init
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_Pass1_Init( const char * apTitle,const U16 aIndex,const char * apName )
{
	const sTokeniserChunkDef *	lpChunkDef;

	(void)aIndex;


	String_Set( &gpTokeniserBuilder->mChunkName, apName );
	String_Set( &gpTokeniserBuilder->mChunkTitle, apTitle );
/*
	String_Set( gpTokeniserBuilder->mpChunkTitleName, apTitle );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, "::" );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, apName );
*/
	gpTokeniserBuilder->mChunkHashCurrent = HashList_BuildHash( apName );

	lpChunkDef = Tokeniser_GetChunkDef( gpTokeniserBuilder->mpInfo->mpChunkDefs, apTitle );
	gpTokeniserBuilder->mpChunkDefCurrent = lpChunkDef;
	if( lpChunkDef )
	{
		gpTokeniserBuilder->mpChunkContainers[ lpChunkDef->mIndex ].mCount++;
	}
	else
	{
		if( gpTokeniserBuilder->mpInfo->mOnWarning )
		{
			sString lString;
			String_Init( &lString, "WARNING: Unknown chunk" );
			String_Cat( &lString, &lString, &gpTokeniserBuilder->mChunkTitle );
			String_Append( &lString, "::" );
			String_Cat( &lString, &lString, &gpTokeniserBuilder->mChunkName );
/*			gpTokeniserBuilder->mpInfo->mOnWarning( "Chunk Not Found" );*/
			gpTokeniserBuilder->mpInfo->mOnWarning( lString.mpChars );
			String_DeInit( &lString );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass1_DeInit(void)
* ACTION   : Tokeniser_Pass1_DeInit
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass1_DeInit(void)
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass1_Var(sTokeniserArgs * apArgs)
* ACTION   : Tokeniser_Pass1_Var
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass1_Var(sTokeniserArgs * apArgs)
{
	(void)apArgs;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_Pass2_Init( const char * apTitle,const U16 aIndex,const char * apName )
* ACTION   : Tokeniser_Pass2_Init
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_Pass2_Init( const char * apTitle,const U16 aIndex,const char * apName )
{
	const sTokeniserChunkDef *	lpChunkDef;
	sTokeniserChunkContainer *	lpCon;

	(void)aIndex;


	String_Set( &gpTokeniserBuilder->mChunkName, apName );
	String_Set( &gpTokeniserBuilder->mChunkTitle, apTitle );

/*	String_Set( gpTokeniserBuilder->mpChunkTitleName, apTitle );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, "::" );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, apName );
*/
	gpTokeniserBuilder->mChunkHashCurrent = HashList_BuildHash( apName );

	lpChunkDef = Tokeniser_GetChunkDef( gpTokeniserBuilder->mpInfo->mpChunkDefs, apTitle );
	gpTokeniserBuilder->mpChunkDefCurrent = lpChunkDef;
	if( lpChunkDef )
	{
		lpCon                            = &gpTokeniserBuilder->mpChunkContainers[ lpChunkDef->mIndex ];
		lpCon->mpHashes[ lpCon->mIndex ] = HashList_BuildHash( apName );
		gpTokeniserBuilder->mpDataItem   = lpCon->mpData;
		gpTokeniserBuilder->mpDataItem  +=((U32)lpCon->mIndex *lpChunkDef->mStructSize);
		lpCon->mIndex++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_Pass2_DeInit( void )
* ACTION   : Tokeniser_Pass2_DeInit
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_Pass2_DeInit( void )
{
	sTokeniserStructMember *		lpMembers;
	sTokeniserArray *				lpArray;
	U8 *							lpData;
	U32								lSize;


	if( gpTokeniserBuilder->mpChunkDefCurrent )
	{
		lpMembers = gpTokeniserBuilder->mpChunkDefCurrent->mpStructDef;
		lpData    = gpTokeniserBuilder->mpDataItem;
		while( lpMembers->mpString )
		{
			if( 1 == lpMembers->mArrayFlag )
			{
				lpArray = *(sTokeniserArray**)&lpData[ lpMembers->mOffset ];
				lSize   = lpArray->mCount;
				lSize  *= lpArray->mItemSize;
				if( lSize )
				{
					lpArray->mpItems = (U8*)mMEMCALLOC( lSize );
				}
			}
			else
			{
				switch( lpMembers->mType )
				{
				case	eTOKENISER_TYPE_CONSTANT:
					switch( lpMembers->mSubType )
					{
					case	eTOKENISER_TYPE_CHAR:
					case	eTOKENISER_TYPE_U8:
						*(U8*)&lpData[ lpMembers->mOffset ] = (U8)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_S8:
						*(S8*)&lpData[ lpMembers->mOffset ] = (S8)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_U16:
						*(U16*)&lpData[ lpMembers->mOffset ] = (U16)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_S16:
						*(S16*)&lpData[ lpMembers->mOffset ] = (S16)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_U32:
						*(U32*)&lpData[ lpMembers->mOffset ] = (U32)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_S32:
						*(S32*)&lpData[ lpMembers->mOffset ] = (S32)lpMembers->mCountOffset;
						break;
					case	eTOKENISER_TYPE_FP32:
						*(FP32*)&lpData[ lpMembers->mOffset ] = (FP32)lpMembers->mCountOffset;
						break;
					}
					break;
				case	eTOKENISER_TYPE_CHUNKHASH:
					*(U32*)&lpData[ lpMembers->mOffset ] = gpTokeniserBuilder->mChunkHashCurrent;
					break;
				case	eTOKENISER_TYPE_CHUNKNAME:
					*(sString**)&lpData[ lpMembers->mOffset ] = StringList_ItemCreate( &gpTokeniserBuilder->mStringList, gpTokeniserBuilder->mChunkName.mpChars )->mpString;
					break;
				}
			}
			lpMembers++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass2_Var(sTokeniserArgs * apArgs)
* ACTION   : Tokeniser_Pass2_Var
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass2_Var(sTokeniserArgs * apArgs)
{
	sTokeniserStructMember *		lpMembers;
	sTokeniserArray *				lpArray;
	U8 *							lpData;


	if( gpTokeniserBuilder->mpChunkDefCurrent )
	{
		lpMembers = gpTokeniserBuilder->mpChunkDefCurrent->mpStructDef;
		lpData    = gpTokeniserBuilder->mpDataItem;
		while( lpMembers->mpString )
		{
			if( !String_StrCmpi( apArgs->mpStrings[ 0 ], lpMembers->mpString ) )
			{
				if( lpMembers->mArrayFlag )
				{
					lpArray = *(sTokeniserArray**)&lpData[ lpMembers->mOffset ];
					lpArray->mCount++;
				}
			}
			lpMembers++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass3_Init(const char * apTitle,const U16 aIndex,const char * apName)
* ACTION   : Tokeniser_Pass3_Init
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass3_Init(const char * apTitle,const U16 aIndex,const char * apName)
{
	const sTokeniserChunkDef *	lpChunkDef;
	sTokeniserChunkContainer *	lpCon;

	(void)aIndex;


	String_Set( &gpTokeniserBuilder->mChunkName, apName );
	String_Set( &gpTokeniserBuilder->mChunkTitle, apTitle );

/*	String_Set( gpTokeniserBuilder->mpChunkTitleName, apTitle );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, "::" );
	String_Append( gpTokeniserBuilder->mpChunkTitleName, apName );
*/

	gpTokeniserBuilder->mChunkHashCurrent = HashList_BuildHash( apName );

	lpChunkDef = Tokeniser_GetChunkDef( gpTokeniserBuilder->mpInfo->mpChunkDefs, apTitle );
	gpTokeniserBuilder->mpChunkDefCurrent = lpChunkDef;
	if( lpChunkDef )
	{
		lpCon                            = &gpTokeniserBuilder->mpChunkContainers[ lpChunkDef->mIndex ];
		lpCon->mpHashes[ lpCon->mIndex ] = HashList_BuildHash( apName );
		gpTokeniserBuilder->mpDataItem     = lpCon->mpData;
		gpTokeniserBuilder->mpDataItem    +=((U32)lpCon->mIndex *lpChunkDef->mStructSize);
		Assert( gpTokeniserBuilder->mpDataItem < lpCon->mpDataMax );
		lpCon->mIndex++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass3_DeInit(void)
* ACTION   : Tokeniser_Pass3_DeInit
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass3_DeInit(void)
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Pass3_Var(sTokeniserArgs * apArgs)
* ACTION   : Tokeniser_Pass3_Var
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Pass3_Var(sTokeniserArgs * apArgs)
{
	sTokeniserStructMember *		lpMembers;
	sTokeniserArray *				lpArray;
	U32								lOff;
	U8 *							lpData;
	U8								lFoundFlag;

	if( gpTokeniserBuilder->mpChunkDefCurrent )
	{
		lpMembers  = gpTokeniserBuilder->mpChunkDefCurrent->mpStructDef;
		lpData     = gpTokeniserBuilder->mpDataItem;
		lFoundFlag = 0;
		while( lpMembers->mpString )
		{
			if( !String_StrCmpi( apArgs->mpStrings[ 0 ], lpMembers->mpString ) )
			{
				lFoundFlag = 1;
				if( lpMembers->mArrayFlag )
				{
					lpArray = *(sTokeniserArray**)&lpData[ lpMembers->mOffset ];
					lOff    = lpArray->mItemIndex * lpArray->mItemSize;
					Tokeniser_GrabData( lpMembers, apArgs, &lpArray->mpItems[ lOff ] );
					lpArray->mItemIndex++;
				}
				else
				{
					Tokeniser_GrabData( lpMembers, apArgs, &lpData[ lpMembers->mOffset ] );
				}
			}
			lpMembers++;
		}
		if( !lFoundFlag )
		{
			if( gpTokeniserBuilder->mpInfo->mOnWarning )
			{
				sString lString;
				String_Init( &lString, "WARNING: couldn't find var " );
				String_Append( &lString, apArgs->mpStrings[ 0 ] );
				String_Append( &lString, " in chunk " );
				String_Append( &lString, gpTokeniserBuilder->mChunkTitle.mpChars );
				String_Append( &lString, "::" );
				String_Append( &lString, gpTokeniserBuilder->mChunkName.mpChars );
				gpTokeniserBuilder->mpInfo->mOnWarning( lString.mpChars );
				String_DeInit( &lString );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_MemberBuild( const sTokeniserStructMember * apMembers,const sTokeniserArgs * apArgs,void * apStruct )
* ACTION   : Tokeniser_MemberBuild
* CREATION : 06.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_MemberBuild( const sTokeniserStructMember * apMembers,const sTokeniserArgs * apArgs,void * apStruct )
{
	const sTokeniserStructMember *		lpMember;
	U8 * lpDst;

	lpMember = apMembers;
	lpDst    = (U8*)apStruct;
	while( lpMember->mpString )
	{
		if( !String_StrCmpi( apArgs->mpStrings[ 0 ], lpMember->mpString ) )
		{
			Tokeniser_GrabData( lpMember, apArgs, &lpDst[ lpMember->mOffset ] );
			return;
		}
		lpMember++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_GrabData(sTokeniserStructMember * apMember,const sTokeniserArgs * apArgs,void * apDst)
* ACTION   : Tokeniser_GrabData
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Tokeniser_GrabData( const sTokeniserStructMember * apMember, const sTokeniserArgs * apArgs, void * apDst )
{
	const sTokeniserTagStringList *	lpList;
	const sTokeniserTagString *		lpTag;
	sString 	lRefName;
	U32			i;
	char	lString[ 256 ];


	switch( apMember->mType )
	{
	case	eTOKENISER_TYPE_NONE:
		break;
	case	eTOKENISER_TYPE_CHAR:
		*(U8*)apDst = apArgs->mpStrings[ 2 ][ 0 ];
		break;
	case	eTOKENISER_TYPE_U8:
		*(U8*)apDst = (U8)TokeniserArgs_GetU16( apArgs );
		break;
	case	eTOKENISER_TYPE_S8:
		*(S8*)apDst = (S8)TokeniserArgs_GetS16( apArgs );
		break;
	case	eTOKENISER_TYPE_U16:
		*(U16*)apDst = TokeniserArgs_GetU16( apArgs );
		break;
	case	eTOKENISER_TYPE_S16:
		*(S16*)apDst = TokeniserArgs_GetS16( apArgs );
		break;
	case	eTOKENISER_TYPE_U32:
		*(U32*)apDst = TokeniserArgs_GetU32( apArgs );
		break;
	case	eTOKENISER_TYPE_S32:
		*(S32*)apDst = TokeniserArgs_GetS32( apArgs );
		break;
	case	eTOKENISER_TYPE_FP32:
		*(FP32*)apDst = TokeniserArgs_GetFP32( apArgs );
		break;

	case	eTOKENISER_TYPE_CHUNKHASH:
		*(U32*)apDst = gpTokeniserBuilder->mChunkHashCurrent;
		break;
	case	eTOKENISER_TYPE_CHUNKNAME:
		*(sString**)apDst = StringList_ItemCreate( &gpTokeniserBuilder->mStringList, gpTokeniserBuilder->mChunkName.mpChars )->mpString;
		break;

	case	eTOKENISER_TYPE_REFERENCE:
		String_Create2( &lRefName, gTokeniserNameSpace.mpChars, apArgs->mpStrings[ 2 ] );
		*(U32*)apDst = (U32)Tokeniser_GetListItem( &gpTokeniserBuilder->mpChunkContainers[ apMember->mSubType ], lRefName.mpChars );
		if( 0 == *(U32*)apDst )
		{
			sprintf( lString, "REFERENCE not found %s in %s::%s", apArgs->mpStrings[ 2 ], gpTokeniserBuilder->mChunkTitle.mpChars, gpTokeniserBuilder->mChunkName.mpChars );
			if( gpTokeniserBuilder->mpInfo->mOnWarning )
			{
				gpTokeniserBuilder->mpInfo->mOnWarning( lString );
			}

		}
		String_DeInit( &lRefName );
		break;

	case	eTOKENISER_TYPE_CHARMAP:
		i = 0;
		while( apArgs->mLine[ i ] && ('=' != apArgs->mLine[ i ]) && (i<apArgs->mLineLen) )
		{
			i++;
		}
		if( '=' == apArgs->mLine[ i ] )
		{
			i++;
			while( (' ' == apArgs->mLine[ i ]) && (i<apArgs->mLineLen) )
			{
				i++;
			}
			if( gpTokeniserBuilder )
			{
				*(sString**)apDst = StringList_ItemCreate( &gpTokeniserBuilder->mStringList, &apArgs->mLine[ i ] )->mpString;
			}
			else
			{
				*(char**)apDst = (char*)mMEMALLOC( strlen( &apArgs->mLine[ i ] ) + 1 );
				strcpy( *(char**)apDst, &apArgs->mLine[ i ] );
			}
		}
		break;

	case	eTOKENISER_TYPE_STRING:
		if( gpTokeniserBuilder )
		{
			*(sString**)apDst = StringList_ItemCreate( &gpTokeniserBuilder->mStringList, apArgs->mpStrings[ 2 ] )->mpString;
		}
		else
		{
			*(char**)apDst = (char*)mMEMALLOC( strlen(apArgs->mpStrings[2]) + 1 );
			strcpy( *(char**)apDst, apArgs->mpStrings[2] );
		}
		break;

	case	eTOKENISER_TYPE_TAGVALUE:
		lpList = Tokeniser_GetTagStringList( gpTokeniserBuilder->mpInfo->mpTagStringList, apMember->mCountOffset );
		if( lpList )
		{
			lpTag  = Tokeniser_GetTagString( lpList->mpTags, apArgs->mpStrings[ 2 ] );
			if( lpTag )
			{
				switch( apMember->mSubType )
				{
				case	eTOKENISER_TYPE_CHAR:
				case	eTOKENISER_TYPE_U8:
					*(U8*)apDst = (U8)lpTag->mID;
					break;
				case	eTOKENISER_TYPE_S8:
					*(S8*)apDst = (S8)lpTag->mID;
					break;
				case	eTOKENISER_TYPE_U16:
					*(U16*)apDst = (U16)lpTag->mID;
					break;
				case	eTOKENISER_TYPE_S16:
					*(S16*)apDst = (S16)lpTag->mID;
					break;
				case	eTOKENISER_TYPE_U32:
					*(U32*)apDst = lpTag->mID;
					break;
				case	eTOKENISER_TYPE_S32:
					*(S32*)apDst = (S32)lpTag->mID;
					break;
				case	eTOKENISER_TYPE_FP32:
					*(FP32*)apDst = (FP32)lpTag->mID;
					break;
				}
			}
		}
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetStructMember( const sTokeniserStructMember * apList,const char * apString )
* ACTION   : Tokeniser_GetStructMember
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserStructMember *	Tokeniser_GetStructMember( const sTokeniserStructMember * apList,const char * apString )
{
	if( apList )
	{
		while( apList->mpString )
		{
			if( !String_StrCmpi( apList->mpString, apString ) )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetChunkDef( const sTokeniserChunkDef * apList,const char * apString )
* ACTION   : Tokeniser_GetChunkDef
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserChunkDef *	Tokeniser_GetChunkDef( const sTokeniserChunkDef * apList,const char * apString )
{
	if( apList )
	{
		while( apList->mpChunkName )
		{
			if( !String_StrCmpi( apList->mpChunkName, apString ) )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : const sTokeniserChunkDef * Tokeniser_GetChunkDefFromIndex(const sTokeniserChunkDef * apList,const U16 aIndex)
* ACTION   : Tokeniser_GetChunkDefFromIndex
* CREATION : 24.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserChunkDef * Tokeniser_GetChunkDefFromIndex(const sTokeniserChunkDef * apList,const U16 aIndex)
{
	if( apList )
	{
		while( apList->mpChunkName )
		{
			if( aIndex == apList->mIndex )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetTagString( const sTokeniserTagString * apList,const char * apString )
* ACTION   : Tokeniser_GetTagString
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserTagString *	Tokeniser_GetTagString( const sTokeniserTagString * apList,const char * apString )
{
	if( apList )
	{
		while( apList->mpString )
		{
			if( !String_StrCmpi( apList->mpString, apString ) )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : const sTokeniserTagString * Tokeniser_GetTagStringFromID(const sTokeniserTagString * apList,const U32 aID)
* ACTION   : Tokeniser_GetTagStringFromID
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserTagString * Tokeniser_GetTagStringFromID(const sTokeniserTagString * apList,const U32 aID)
{
	if( apList )
	{
		while( apList->mpString )
		{
			if( aID == apList->mID )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetTagStringList( const sTokeniserTagStringList * apList,const U32 aID )
* ACTION   : Tokeniser_GetTagStringList
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

const sTokeniserTagStringList *	Tokeniser_GetTagStringList( const sTokeniserTagStringList * apList,const U32 aID )
{
	if( apList )
	{
		while( apList->mpTags )
		{
			if( aID == apList->mID )
			{
				return( apList );
			}
			apList++;
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Tokeniser_GetListItem( const sTokeniserChunkContainer * apChunkContainer,const char * apName )
* ACTION   : Tokeniser_GetListItem
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 *	Tokeniser_GetListItem( const sTokeniserChunkContainer * apChunkContainer,const char * apName )
{

	U8 *	lpItem;
	U32		lHash;
	U16		i;

	lpItem = 0;
	if( apChunkContainer && apName )
	{
		i     = 0;
		lHash = HashList_BuildHash( apName );
		while( (i<apChunkContainer->mCount) && (!lpItem) )
		{
			if( apChunkContainer->mpHashes[ i ] == lHash )
			{
				lpItem  = apChunkContainer->mpData;
				lpItem +=(apChunkContainer->mStructSize * (U32)i);
			}
			i++;
		}
	}

	return( lpItem );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_ShowInfo(sTokeniserBuilder * apBuilder)
* ACTION   : Tokeniser_ShowInfo
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_ShowInfo( U8 * apData, sTokeniserInfo * apInfo )
{
	const sTokeniserChunkDef *		lpChunkDef;
	const sTokeniserStructMember *	lpMember;
	U16 						lCount;
	U16							lArrayCount;
	U16							i,j;
	U8 *						lpItems;
	U8 *						lpArrayItem;
	char						lPair[ 512 ];
	char						lString[ 512 ];

	DebugLog_Printf0( "Tokeniser_ShowInfo()" );

	if( apInfo && apInfo->mOnWarning )
	{
		lpChunkDef = apInfo->mpChunkDefs;
		while( lpChunkDef->mpChunkName )
		{
			lCount  = *(U16*)&apData[ lpChunkDef->mCountOffset ];
			lpItems = *(U8**)&apData[ lpChunkDef->mPtrOffset   ];

			apInfo->mOnWarning( "=================================" );
			sprintf( lString, "CHUNK : %s : COUNT : %d", lpChunkDef->mpChunkName, lCount );
			apInfo->mOnWarning( lString );

			for( i=0; i<lCount; i++ )
			{
				apInfo->mOnWarning( " " );
				sprintf( lString, "*  %s [ %d ] %p *", lpChunkDef->mpChunkName, i, lpItems );
				apInfo->mOnWarning( lString );

				lpMember = lpChunkDef->mpStructDef;
				while( lpMember->mpString )
				{
					lString[ 0 ] = 0;
					if( 1 == lpMember->mArrayFlag )
					{
						lArrayCount = *(U16*)&lpItems[ lpMember->mCountOffset ];
						lpArrayItem = *(U8**)&lpItems[ lpMember->mOffset ];
						for( j=0; j<lArrayCount; j++ )
						{
							Tokeniser_ShowType( lpMember->mType, lpMember->mSubType, lpMember->mCountOffset, apInfo->mpTagStringList, lpArrayItem, lString );
							switch( lpMember->mType )
							{
							case	eTOKENISER_TYPE_CHAR:
							case	eTOKENISER_TYPE_U8:
							case	eTOKENISER_TYPE_S8:
								lpArrayItem += 1;
								break;
							case	eTOKENISER_TYPE_U16:
							case	eTOKENISER_TYPE_S16:
								lpArrayItem += 2;
								break;
							case	eTOKENISER_TYPE_U32:
							case	eTOKENISER_TYPE_S32:
							case	eTOKENISER_TYPE_FP32:
							case	eTOKENISER_TYPE_CONSTANT:
							case	eTOKENISER_TYPE_CHUNKHASH:
							case	eTOKENISER_TYPE_CHUNKNAME:
							case	eTOKENISER_TYPE_REFERENCE:
							case	eTOKENISER_TYPE_STRING:
							case	eTOKENISER_TYPE_TAGVALUE:
								lpArrayItem += 4;
								break;
							}
							sprintf( lPair, " %s %d : %s", lpMember->mpString, j, lString );
							apInfo->mOnWarning( lPair );
					}
					}
					else
					{
						Tokeniser_ShowType( lpMember->mType, lpMember->mSubType, lpMember->mCountOffset, apInfo->mpTagStringList, &lpItems[ lpMember->mOffset ], lString );
						sprintf( lPair, "%s : %s", lpMember->mpString, lString );
						apInfo->mOnWarning( lPair );
					}

					lpMember++;
				}
				lpItems += lpChunkDef->mStructSize;
			}
			apInfo->mOnWarning( "\n" );

			lpChunkDef++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_ShowType(const U16 aType,void * apData,char * apString)
* ACTION   : Tokeniser_ShowType
* CREATION : 27.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_ShowType(const U16 aType,const U16 aSubType, const U32 aCountOffset, sTokeniserTagStringList * apList, void * apData,char * apString)
{
	U32								lU32;
	const sTokeniserTagString *		lpTag;
	const sTokeniserTagStringList *	lpList;
	char *						lpString;

	switch( aType )
	{
	case	eTOKENISER_TYPE_CHAR:
		sprintf( apString, "%c", *(char*)apData );
		break;
	case	eTOKENISER_TYPE_U8:
		sprintf( apString, "%d", *(U8*)apData );
		break;
	case	eTOKENISER_TYPE_S8:
		sprintf( apString, "%d", *(S8*)apData );
		break;
	case	eTOKENISER_TYPE_U16:
		sprintf( apString, "%d", *(U16*)apData );
		break;
	case	eTOKENISER_TYPE_S16:
		sprintf( apString, "%d", *(S16*)apData );
		break;
	case	eTOKENISER_TYPE_U32:
		sprintf( apString, "%ld", *(U32*)apData );
		break;
	case	eTOKENISER_TYPE_S32:
		sprintf( apString, "%ld", *(S32*)apData );
		break;
	case	eTOKENISER_TYPE_FP32:
		sprintf( apString, "%f", *(FP32*)apData );
		break;

	case	eTOKENISER_TYPE_CHUNKNAME:
	case	eTOKENISER_TYPE_STRING:
		lpString = *(char**)apData;
		if( lpString )
		{
			strcpy( apString, lpString );
		}
		break;

	case	eTOKENISER_TYPE_CONSTANT:
		switch( aSubType )
		{
		case	eTOKENISER_TYPE_CHAR:
			sprintf( apString, "%c", *(char*)apData );
			break;
		case	eTOKENISER_TYPE_U8:
			sprintf( apString, "%d", *(U8*)apData );
			break;
		case	eTOKENISER_TYPE_S8:
			sprintf( apString, "%d", *(S8*)apData );
			break;
		case	eTOKENISER_TYPE_U16:
			sprintf( apString, "%d", *(U16*)apData );
			break;
		case	eTOKENISER_TYPE_S16:
			sprintf( apString, "%d", *(S16*)apData );
			break;
		case	eTOKENISER_TYPE_U32:
			sprintf( apString, "%ld", *(U32*)apData );
			break;
		case	eTOKENISER_TYPE_S32:
			sprintf( apString, "%ld", *(S32*)apData );
			break;
		case	eTOKENISER_TYPE_FP32:
			sprintf( apString, "%f", *(FP32*)apData );
			break;
		}
		break;

	case	eTOKENISER_TYPE_CHUNKHASH:
		sprintf( apString, "%lx", *(U32*)apData );
		break;

	case	eTOKENISER_TYPE_REFERENCE:
		sprintf( apString, "%lx", *(U32*)apData );
		break;

	case	eTOKENISER_TYPE_TAGVALUE:
		lU32 = 0;
		switch( aSubType )
		{
		case	eTOKENISER_TYPE_CHAR:
		case	eTOKENISER_TYPE_U8:
			lU32 = *(U8*)apData;
			break;
		case	eTOKENISER_TYPE_S8:
			lU32 = *(S8*)apData;
			break;
		case	eTOKENISER_TYPE_U16:
			lU32 = *(U16*)apData;
			break;
		case	eTOKENISER_TYPE_S16:
			lU32 = *(S16*)apData;
			break;
		case	eTOKENISER_TYPE_U32:
			lU32 = *(U32*)apData;
			break;
		case	eTOKENISER_TYPE_S32:
			lU32 = *(U32*)apData;
			break;
		case	eTOKENISER_TYPE_FP32:
			lU32 = (U32)*(FP32*)apData;
			break;
		}
		lpList = Tokeniser_GetTagStringList( apList, aCountOffset );
		if( lpList )
		{
			lpTag = Tokeniser_GetTagStringFromID( lpList->mpTags, lU32 );
		}
		else
		{
			lpTag = 0;
		}

		if( lpTag )
		{
			strcpy( apString, lpTag->mpString );
		}
		else
		{
			sprintf( apString, "%ld", lU32 );
		}
		break;

	case	eTOKENISER_TYPE_NONE:
	default:
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Delocate(U8 * apData,sTokeniserInfo * apInfo)
* ACTION   : Tokeniser_Delocate
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Delocate(U8 * apData,sTokeniserInfo * apInfo)
{
	const sTokeniserChunkDef *		lpChunkDef;
	const sTokeniserStructMember *	lpMember;
	U16 							lCount;
	U16								i,j;
	U16								lArrayCount;
	U32 *							lpArray;
	U8 *							lpItems;

	DebugLog_Printf0( "Tokeniser_Delocate()" );

	if( apData && apInfo )
	{
		lpChunkDef = apInfo->mpChunkDefs;
		while( lpChunkDef->mpChunkName )
		{
			lCount  = *(U16*)&apData[ lpChunkDef->mCountOffset ];
			lpItems = *(U8**)&apData[ lpChunkDef->mPtrOffset   ];

			*(U32*)&apData[ lpChunkDef->mPtrOffset ] -= (U32)apData;

			for( i=0; i<lCount; i++ )
			{
				lpMember = lpChunkDef->mpStructDef;
				while( lpMember->mpString )
				{
					switch( lpMember->mArrayFlag )
					{
					case	1:
						lpArray                               = *(U32**)&lpItems[ lpMember->mOffset ];
						*(U32*)&lpItems[ lpMember->mOffset ] -= (U32)apData;
						if( lpArray )
						{
							lArrayCount = *(U16*)&lpItems[ lpMember->mCountOffset ];
							for( j=0; j<lArrayCount; j++ )
							{
								switch( lpMember->mType )
								{
								case	eTOKENISER_TYPE_CHUNKNAME:
								case	eTOKENISER_TYPE_REFERENCE:
								case	eTOKENISER_TYPE_STRING:
									if( lpArray[ j ] )
									{
										lpArray[ j ] -= (U32)apData;
									}
									break;
								}
							}
						}
						break;
					case	0:
						switch( lpMember->mType )
						{
						case	eTOKENISER_TYPE_CHUNKNAME:
						case	eTOKENISER_TYPE_REFERENCE:
						case	eTOKENISER_TYPE_STRING:
							if( *(U32*)&lpItems[ lpMember->mOffset ] )
							{
								*(U32*)&lpItems[ lpMember->mOffset ] -= (U32)apData;
							}
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}

					lpMember++;
				}
				lpItems += lpChunkDef->mStructSize;
			}
			lpChunkDef++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Tokeniser_Relocate(U8 * apData,sTokeniserInfo * apInfo)
* ACTION   : Tokeniser_Relocate
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void Tokeniser_Relocate(U8 * apData,sTokeniserInfo * apInfo)
{
	const sTokeniserChunkDef *		lpChunkDef;
	const sTokeniserStructMember *	lpMember;
	U16 							lCount;
	U16								i,j;
	U16								lArrayCount;
	U32 *							lpArray;
	U8 *							lpItems;

	DebugLog_Printf0( "Tokeniser_Relocate()" );

	if( apData && apInfo )
	{
		lpChunkDef = apInfo->mpChunkDefs;
		while( lpChunkDef->mpChunkName )
		{
			*(U32*)&apData[ lpChunkDef->mPtrOffset ] += (U32)apData;

			lCount  = *(U16*)&apData[ lpChunkDef->mCountOffset ];
			lpItems = *(U8**)&apData[ lpChunkDef->mPtrOffset   ];

			for( i=0; i<lCount; i++ )
			{
				lpMember = lpChunkDef->mpStructDef;
				while( lpMember->mpString )
				{
					switch( lpMember->mArrayFlag )
					{
					case	1:
						*(U32*)&lpItems[ lpMember->mOffset ] += (U32)apData;
						lpArray = *(U32**)&lpItems[ lpMember->mOffset ];
						if( lpArray )
						{
							lArrayCount = *(U16*)&lpItems[ lpMember->mCountOffset ];
							for( j=0; j<lArrayCount; j++ )
							{
								switch( lpMember->mType )
								{
								case	eTOKENISER_TYPE_CHUNKNAME:
								case	eTOKENISER_TYPE_REFERENCE:
								case	eTOKENISER_TYPE_STRING:
									if( lpArray[ j ] )
									{
										lpArray[ j ] += (U32)apData;
									}
									break;
								}
							}
						}
						break;
					case	0:
						switch( lpMember->mType )
						{
						case	eTOKENISER_TYPE_CHUNKNAME:
						case	eTOKENISER_TYPE_REFERENCE:
						case	eTOKENISER_TYPE_STRING:
							if( *(U32*)&lpItems[ lpMember->mOffset ] )
							{
								*(U32*)&lpItems[ lpMember->mOffset ] += (U32)apData;
							}
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}

					lpMember++;
				}
				lpItems += lpChunkDef->mStructSize;
			}
			lpChunkDef++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : TokeniserArgs_MemberBuild( const sTokeniserArgs * apArgs,sTokeniserMember * apMembers,void * apStruct )
* ACTION   : TokeniserArgs_MemberBuild
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	TokeniserArgs_MemberBuild( const sTokeniserArgs * apArgs,sTokeniserMember * apMembers,void * apStruct )
{
	sTokeniserMember *	lpMember;
	U8 *				lpStruct;

	lpStruct = (U8*)apStruct;

	if( apArgs->mArgCount >= 3 )
	{
		S32	lVal;

		lpMember = apMembers;
		lVal     = 0;

		while( (lpMember->mpString) && (String_StrCmpi( lpMember->mpString, apArgs->mpStrings[0] ) ) )
		{
			lpMember++;
		}
		if( lpMember->mpString )
		{
			if( lpMember->mValue )
			{
				sTagString *	lpTags;

				lpTags = (sTagString*)lpMember->mValue;
				while( (lpTags->pString) && (String_StrCmpi( apArgs->mpStrings[2], lpTags->pString ) ) )
				{
					lpTags++;
				}
				if( lpTags->pString )
				{
					lVal = lpTags->ID;
				}
			}
			else
			{
				lVal = String_ToValue( apArgs->mpStrings[ 2 ] );
			}
			switch( lpMember->mType )
			{
			case	eTOKENISER_TYPE_U8:
				lpStruct[ lpMember->mOffset ] = (U8)lVal;
				break;
			case	eTOKENISER_TYPE_S8:
				lpStruct[ lpMember->mOffset ] = (S8)lVal;
				break;
			case	eTOKENISER_TYPE_U16:
				*(U16*)&lpStruct[ lpMember->mOffset ] = (U16)lVal;
				break;
			case	eTOKENISER_TYPE_S16:
				*(S16*)&lpStruct[ lpMember->mOffset ] = (S16)lVal;
				break;
			case	eTOKENISER_TYPE_U32:
				*(U32*)&lpStruct[ lpMember->mOffset ] = (U32)lVal;
				break;
			case	eTOKENISER_TYPE_S32:
				*(S32*)&lpStruct[ lpMember->mOffset ] = (S32)lVal;
				break;
			case	eTOKENISER_TYPE_FP32:
				*(FP32*)&lpStruct[ lpMember->mOffset ] = TokeniserArgs_GetFP32( apArgs );
				break;

			case	eTOKENISER_TYPE_FIX8_8:
				{
					FP32	lFloat;
					S32		lInt,lFrac;

					lFloat  = TokeniserArgs_GetFP32( apArgs );
					lInt    = (S32)lFloat;
					lFloat -= lInt;
					lFloat *= 256.f;
					lFrac   = (S32)lFloat;

					lInt  <<= 8L;
					lInt   |= lFrac;
					*(U16*)&lpStruct[ lpMember->mOffset ] = (U16)lInt;
				}
				break;

			case	eTOKENISER_TYPE_FIX16_16:
				{
					FP32	lFloat;
					S32		lInt,lFrac;

					lFloat  = TokeniserArgs_GetFP32( apArgs );
					lInt    = (S32)lFloat;
					lFloat -= lInt;
					lFloat *= 65536.f;
					lFrac   = (S32)lFloat;

					lInt  <<= 16L;
					lInt   |= lFrac;
					*(S32*)&lpStruct[ lpMember->mOffset ] = (S32)lInt;
				}
				break;

			case	eTOKENISER_TYPE_CHAR:
				lpStruct[ lpMember->mOffset ] = apArgs->mpStrings[ 2 ][ 0 ];
				break;
			case	eTOKENISER_TYPE_CHARMAP:
				break;
			case	eTOKENISER_TYPE_CONSTANT:
				break;
			case	eTOKENISER_TYPE_CHUNKHASH:
				break;
			case	eTOKENISER_TYPE_CHUNKNAME:
				break;
			case	eTOKENISER_TYPE_REFERENCE:
			case	eTOKENISER_TYPE_STRING:
				{
					char * lpStr;

					lpStr = (char*)mMEMCALLOC( String_StrLen( apArgs->mpStrings[ 2 ] ) + 1 );
					*(char**)&lpStruct[ lpMember->mOffset ] = lpStr;
					String_StrCpy( lpStr, apArgs->mpStrings[ 2 ] );
				}
				break;
			case	eTOKENISER_TYPE_TAGVALUE:
				break;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : StringList_Validate( void )
* ACTION   : StringList_Validate
* CREATION : 17.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	StringList_Validate( void )
{
	sString *			lpString;
	sStringList *		lpList;
	sStringListItem *	lpItem;

	DebugLog_Printf0( "StringList Validate" );

	lpList = &gpTokeniserBuilder->mStringList;
	lpItem = lpList->mpItems;
	while( lpItem )
	{
		lpString = lpItem->mpString;
		if( (!lpString->mpChars) || ((U32)lpString->mpChars > 0xFF0000L) )
		{
			DebugLog_Printf1( "StringList validate error %p", lpString->mpChars );
		}
		lpItem   = lpItem->mpNext;
	}
	DebugLog_Printf0( "StringList Validate done" );
}


/* ################################################################################ */

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: LEXER.C
::
:: Functions for tokenising strings based on whitespace divisions
::
:: [c] 2018 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"LEXER.H"

#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\STRING\STRING.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Lexer_Init(sLexerContext * apContext, const sString * apString)
* ACTION   : inits data structure. must be called before lexing
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	Lexer_Init(sLexerContext * apContext, const sString * apString)
{
	apContext->mLineIndex = 0;
	apContext->mOffset = 0;
	apContext->mSource = *apString;
	apContext->mSeperators[ 0 ] = 0;
	apContext->mSeperators[ 1 ] = 0xFC00FF7AL;
	apContext->mSeperators[ 2 ] = 0x78000001L;
	apContext->mSeperators[ 3 ] = 0xF8000001L;
	String_Init(&apContext->mToken, 0);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Lexer_Init2(sLexerContext * apContext, const char * apString)
* ACTION   : inits data structure, absed on null terminated string must be called before lexing.
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	Lexer_Init2(sLexerContext * apContext, const char * apString)
{
	apContext->mLineIndex = 0;
	apContext->mOffset = 0;
	apContext->mSeperators[ 0 ] = 0;
	apContext->mSeperators[ 1 ] = 0xFC00FF7AL;
	apContext->mSeperators[ 2 ] = 0x78000001L;
	apContext->mSeperators[ 3 ] = 0xF8000001L;
	String_Init( &apContext->mSource, apString);
	String_Init( &apContext->mToken, 0);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Lexer_DeInit(sLexerContext * apContext)
* ACTION   : ensures string deinit
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

void	Lexer_DeInit(sLexerContext * apContext)
{
	String_DeInit( &apContext->mSource);
	String_DeInit(&apContext->mToken);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8	Lexer_GetNextToken( sLexerContext * apContext )
* ACTION   : retrieves next token from buffer
* CREATION : 06.10.18 PNK
*-----------------------------------------------------------------------------------*/

U8	Lexer_GetNextToken( sLexerContext * apContext )
{
	U32 lOff;
	U32 lMask;
	U32 lLength = String_GetLength( &apContext->mSource );
	char * lpChars = &apContext->mSource.mpChars[ apContext->mOffset ];
	char * lpEnd = &apContext->mSource.mpChars[lLength ];

	apContext->mTokenFlag = 0;

	/* skip whitespace before token */
	for (; lpChars < lpEnd && (*lpChars<=' '); lpChars++)
	{
		if ((10 == *lpChars) || (13 == *lpChars))
		{
			apContext->mLineIndex++;
			if ((lpChars!=apContext->mSource.mpChars) && (lpChars[-1] != *lpChars) && (lpChars[-1] == 10))
				apContext->mLineIndex--;
		}
	}

	/* adjust token sto skip over whitespace */
	apContext->mOffset = lpChars - apContext->mSource.mpChars;

	lMask   = 1;
	lMask <<= (*lpChars & 31);

	if( apContext->mSeperators[ *lpChars>>5 ] & lMask )
	{
		lpChars++;
	}
	else
	{
		/* read token until whitespace */
		for( ;lpChars < lpEnd; lpChars++ )
		{
			char c = *lpChars;
			if( '\'' == c || '"' == c )
				while( lpChars < lpEnd && c != *++lpChars );
			else if( c<=' ')
				break;
			else
			{
				lMask = 1;
				lMask <<= (*lpChars & 31);
				if( apContext->mSeperators[ *lpChars >> 5 ] & lMask )
					break;
			}
		}
	}

	/* absolute offset of token end*/
	lOff = lpChars - apContext->mSource.mpChars;
	if( lOff > apContext->mOffset )
	{
		/* set new token */
		String_SetStatic( &apContext->mToken, &apContext->mSource.mpChars[ apContext->mOffset ], lOff - apContext->mOffset );
		apContext->mOffset = lOff;
		apContext->mTokenFlag = 1;
	}

	return( (U8)apContext->mTokenFlag );
}

void	Lexer_SetSeperators( sLexerContext * apContext, const char * apSeps )
{
	while( *apSeps )
	{
		(void)apContext;
	}
}


/* ################################################################################ */

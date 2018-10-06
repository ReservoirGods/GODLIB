#include	"LEXER.H"

#include	<GODLIB\STRING\STRING.H>

void	Lexer_Init(sLexerContext * apContext, const sString * apString)
{
	apContext->mLineIndex = 0;
	apContext->mOffset = 0;
	apContext->mSource = *apString;
	String_Init(&apContext->mToken, 0);
}

void	Lexer_Init2(sLexerContext * apContext, const char * apString)
{
	apContext->mLineIndex = 0;
	apContext->mOffset = 0;
	String_Init( &apContext->mSource, apString);
	String_Init( &apContext->mToken, 0);
}

void	Lexer_DeInit(sLexerContext * apContext)
{
	String_DeInit(&apContext->mToken);
}

U8	Lexer_GetNextToken( sLexerContext * apContext )
{
	U8 lRes = 0;
	U32 lOff;
	U32 lLength = String_GetLength( &apContext->mSource );
	char * lpChars = &apContext->mSource.mpChars[ apContext->mOffset ];
	char * lpEnd = &apContext->mSource.mpChars[lLength ];

	apContext->mTokenFlag = 0;

	for (; lpChars < lpEnd && (' ' == *lpChars) || ('\t' == *lpChars) || (10 == *lpChars) || (13 == *lpChars); lpChars++)
	{
		if ((10 == *lpChars) || (13 == *lpChars))
		{
			apContext->mLineIndex++;
			if ((lpChars!=apContext->mSource.mpChars) && (lpChars[-1] != *lpChars) && (lpChars[-1] == 10))
				apContext->mLineIndex--;
		}
	}

	apContext->mOffset = lpChars - apContext->mSource.mpChars;

	for( ;lpChars<lpEnd; lpChars++ )
	{
		char c = *lpChars;
		if( '\'' == c || '"' == c )
		{
			while (lpChars < lpEnd && c != *++lpChars);
			if (lpChars < lpEnd)
				lpChars++;
		}
		else if( ' ' == c || 9 == c || 10 ==c || 13 == c)
			break;
	}

	lOff = lpChars - apContext->mSource.mpChars;
	if( lOff > apContext->mOffset )
	{
		String_SetStatic( &apContext->mToken, &apContext->mSource.mpChars[ apContext->mOffset ], lOff - apContext->mOffset );
		apContext->mOffset = lOff;
		apContext->mTokenFlag = 1;
		lRes = 1;
	}

	return( lRes );
}
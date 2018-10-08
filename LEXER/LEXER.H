#ifndef	INCLUDED_LEXER_H
#define	INCLUDED_LEXER_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB\STRING\STRING.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sLexerContext
{
	sString mSource;
	sString mToken;
	U32		mOffset;
	U32		mLineIndex;
	U16		mTokenFlag;
	U32		mSeperators[ 4 ];
} sLexerContext;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Lexer_Init(sLexerContext * apContext, const sString * apString);
void	Lexer_Init2(sLexerContext * apContext, const char * apString);
void	Lexer_DeInit(sLexerContext * apContext);

void	Lexer_SetSeperators( sLexerContext * apContext, const char * apSeps );
U8		Lexer_GetNextToken( sLexerContext * apContext );


/* ################################################################################ */

#endif	/*	INCLUDED_LEXER_H	*/

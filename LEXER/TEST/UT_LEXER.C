/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"..\LEXER.H"

#include	<GODLIB\UNITTEST\UNITTEST.H>
#include	<STDARG.H>


/* ###################################################################################
#  CODE
################################################################################### */


void Lexer_TokeniseValidate( const char * aString, ... )
{
	sLexerContext lContext;
	const char * lpArg=0;
	va_list list;
	va_start(list,aString);

	Lexer_Init2(&lContext, aString);

	while( Lexer_GetNextToken(&lContext))
	{
		sString stringCorrect;
		lpArg = va_arg(list, char*);
		GOD_UNIT_TEST_EXPECT((lpArg), "lexer tokenise mismatch");
		if (!lpArg)
			break;

		String_Init(&stringCorrect,0);
		String_SetStaticNT(&stringCorrect, lpArg);
		if( !( String_IsEqual( &stringCorrect, &lContext.mToken ) ))
		{
			DebugLog_Printf4( "correct: -%s-[%ld] token: -%s-[%ld]\n", stringCorrect.mpChars, String_GetLength( &stringCorrect ), lContext.mToken.mpChars, String_GetLength( &lContext.mToken ) );
		}
		GOD_UNIT_TEST_EXPECT((String_IsEqual(&stringCorrect,&lContext.mToken)), "lexer tokenise mismatch");
	}
	if (!lContext.mTokenFlag)
	{
		lpArg = va_arg(list, char*);
		if (lpArg)
			printf("lexer arg: %p\n", lpArg);
		GOD_UNIT_TEST_EXPECT((!lpArg), "lexer tokenise mismatch");
	}
	Lexer_DeInit(&lContext);

	va_end(list);
}


GOD_UNIT_TEST( Lexer )
{

	Lexer_TokeniseValidate("", 0L);
	Lexer_TokeniseValidate("single", "single", 0L);
	Lexer_TokeniseValidate("two words", "two", "words", 0L);
	Lexer_TokeniseValidate(" startspace", "startspace", 0L);
	Lexer_TokeniseValidate("trailspace ", "trailspace", 0L);
	Lexer_TokeniseValidate(" bothspace ", "bothspace", 0L);
	Lexer_TokeniseValidate(" some \"text in quote marks\" here", "some", "\"text in quote marks\"", "here", 0L);
	Lexer_TokeniseValidate( "  other 'quote marks'", "other", "'quote marks'", 0L );
	Lexer_TokeniseValidate( "  inside 'a quote \"in a quote\"'", "inside", "'a quote \"in a quote\"'", 0L );
	Lexer_TokeniseValidate( "line\nbreaks", "line", "breaks", 0L );
	Lexer_TokeniseValidate( "another\rbreak", "another", "break", 0L );
	Lexer_TokeniseValidate( "\tdo\tyou\tlike\ttabs?", "do", "you", "like", "tabs", "?", 0L );
	Lexer_TokeniseValidate( " this   is 'some'\n\r\t mixed \n\"up stuff\"dont\tyou think?", "this", "is", "'some'", "mixed", "\"up stuff\"dont", "you", "think", "?", 0L );
	Lexer_TokeniseValidate( "!@#$%",  "!", "@", "#", "$", "%", 0L );

	/* validate all symbols */
	{
		const char *lpSymbolsAll = "!#$%&()*+,-./:;<=>?@[\\]^`{|}~";
		const char *lpNonSymbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

		const char * lpS = (const char*)lpSymbolsAll;
		char * lp0 = "abc";
		char * lp1 = "two bwords";
		Lexer_TokeniseValidate( lp0, "abc", 0L );

		for( ;*lpS; lpS++ )
		{
			char lChar[ 2 ] = { 0,0 };
			lp0[ 1 ] = *lpS;
			lChar[ 0 ] = *lpS;
			Lexer_TokeniseValidate( lp0, "a", lChar, "c", 0L );
			lp1[ 4 ] = *lpS;
			Lexer_TokeniseValidate( lp1, "two", lChar, "words", 0L );
		}

		for( lpS=lpNonSymbols;*lpS; lpS++ )
		{
/*			char lChar[ 2 ] = { 0,0 };*/
			lp0[ 1 ] = *lpS;
/*			lChar[ 0 ] = *lpS;*/
			Lexer_TokeniseValidate( lp0, lp0, 0L );
			lp1[ 4 ] = *lpS;
			Lexer_TokeniseValidate( lp1, "two", &lp1[4], 0L );
		}
	}
}


/* ################################################################################ */
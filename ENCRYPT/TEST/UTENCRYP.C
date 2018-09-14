/*extern int	gNoWarningOnEmptyTranslationUnit;*/

#include	<GODLIB/UNITTEST/UNITTEST.H>
#include	<GODLIB/ENCRYPT/ENCRYPT.H>

GOD_UNIT_TEST( Encrypt )
{
	U8	lSrc[ 32 ];
	U8	lDst[ 32 ];
	U16 i;
	U16 lSame = 0;
	U32 lKey;

	for( i = 0;i < sizeof(lSrc); i++ )
		lSrc[ i ] = (U8)( GOD_UNIT_TEST_RAND( 0xFF ) );

	lKey = 56;
	for( i = 0;i < 32; i++ )
		lDst[ i ] = lSrc[ i ];
	
	Encrypt_Scramble( lDst, sizeof( lSrc ), lKey );

	for( i = 0;i < 32; i++ )
		lSame += lDst[ i ] == lSrc[ i ] ? 1 : 0;

	GOD_UNIT_TEST_EXPECT( lSame < 4, ( "not scrambling enough" ) );

	Encrypt_DeScramble( lDst, sizeof( lSrc ), lKey );
	for( i = 0;i < 32; i++ )
	{
		GOD_UNIT_TEST_EXPECT( lSrc[i]==lDst[i], ( "not scrambling enough" ) );
	}

}
#include	<GODLIB\UNITTEST\UNITTEST.H>

sTagString	gBaseTestTagStrings[]=
{
	{	0x14756,	"hello"	},
	{   0x80032,	"world" },
	{   33,			"sausage" },
	{   0xFFFFFF,	"time" },
};
sTagValue	gBaseTestTagValues[ 16 ];


GOD_UNIT_TEST( Base )
{
	U16		i;
	U8 lA1[ 1 ] = { 1 };
	U8 lA2[ 2 ] = { 2,1 };
	U8 lA3[ 3 ] = { 3,2,1 };
	U8 lA4[ 4 ] = { 4,3,2,1 };

	U16 lU16_1[ 1 ] = { 1 };
	U16 lU16_2[ 2 ] = { 2,1 };
	U16 lU16_3[ 3 ] = { 3,2,1 };
	U16 lU16_4[ 4 ] = { 4,3,2,1 };

	/* we defer some of these comparisons to avoid compiler warnings about unreachable code for static checks */

	lA1[ 0 ] = mARRAY_COUNT( lA1 );
	lA2[ 0 ] = mARRAY_COUNT( lA2 );
	lA3[ 0 ] = mARRAY_COUNT( lA3 );
	lA4[ 0 ] = mARRAY_COUNT( lA4 );

	GOD_UNIT_TEST_ASSERT( lA1[ 0 ] == 1, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lA2[ 0 ] == 2, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lA3[ 0 ] == 3, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lA4[ 0 ] == 4, "array count failure" );

	lU16_1[ 0 ] = mARRAY_COUNT( lU16_1 );
	lU16_2[ 0 ] = mARRAY_COUNT( lU16_2 );
	lU16_3[ 0 ] = mARRAY_COUNT( lU16_3 );
	lU16_4[ 0 ] = mARRAY_COUNT( lU16_4 );

	GOD_UNIT_TEST_ASSERT( lU16_1[ 0 ] == 1, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lU16_2[ 0 ] == 2, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lU16_3[ 0 ] == 3, "array count failure" );
	GOD_UNIT_TEST_ASSERT( lU16_4[ 0 ] == 4, "array count failure" );

	lA1[ 0 ] = sizeof( S8 );
	lA2[ 0 ] = sizeof( U8 );
	lA3[ 0 ] = sizeof( S16 );
	lA4[ 0 ] = sizeof( U16 );

	lU16_1[ 0 ] = sizeof( S32 );
	lU16_2[ 0 ] = sizeof( U32 );
	lU16_3[ 0 ] = sizeof( F32 );
	lU16_4[ 0 ] = sizeof( FP32 );

	GOD_UNIT_TEST_ASSERT( lA1[0] == 1, "sizeof(S8) failure" );
	GOD_UNIT_TEST_ASSERT( lA2[0] == 1, "sizeof(U8) failure" );
	GOD_UNIT_TEST_ASSERT( lA3[0] == 2, "sizeof(S16) failure" );
	GOD_UNIT_TEST_ASSERT( lA4[0] == 2, "sizeof(U16) failure" );
	GOD_UNIT_TEST_ASSERT( lU16_1[0] == 4, "sizeof(S32) failure" );
	GOD_UNIT_TEST_ASSERT( lU16_2[0] == 4, "sizeof(U32) failure" );
	GOD_UNIT_TEST_ASSERT( lU16_3[0] == 4, "sizeof(F32) failure" );
	GOD_UNIT_TEST_ASSERT( lU16_4[0] == 4, "sizeof(FP32) failure" );

	lA1[ 0 ] = sizeof( uU16 );
	lA2[ 0 ] = sizeof( uS16 );
	lA3[ 0 ] = sizeof( uU32 );
	lA4[ 0 ] = sizeof( uS32 );

	GOD_UNIT_TEST_EXPECT( lA1[0] == 2, "sizeof(uU16) failure" );
	GOD_UNIT_TEST_EXPECT( lA2[0] == 2, "sizeof(uS16) failure" );

	GOD_UNIT_TEST_EXPECT( lA3[0] == 4, "sizeof(uU32) failure" );
	GOD_UNIT_TEST_EXPECT( lA4[0] == 4, "sizeof(uS32) failure" );

	{
		uU16	lU16;
		uS16	lS16;
		uU32	lU32;
		uS32	lS32;

		for( i = 0; i < 16; i++ )
		{
			U8 lB0 = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			U8 lB1 = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			U8 lB2 = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			U8 lB3 = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			U16 lW0 = lB1;
			U16 lW1 = lB3;
			U32 lL0;
			lW0 <<= 8;
			lW0 += lB0;
			lW1 <<= 8;
			lW1 += lB2;
			lL0 = lW1;
			lL0 <<= 16;
			lL0 += lW0;

			lU32.l = lL0;
			lS32.l = lL0;
			lU16.w = lW0;
			lS16.w = lW0;
			GOD_UNIT_TEST_EXPECT( lU32.w.w0 == lW0, "uU32.w0 failure" );
			GOD_UNIT_TEST_EXPECT( lU32.w.w1 == lW1, "uU32.w1 failure" );
			GOD_UNIT_TEST_EXPECT( (U16)lS32.w.w0 == lW0, "uS32.w0 failure" );
			GOD_UNIT_TEST_EXPECT( (U16)lS32.w.w1 == lW1, "uS32.w1 failure" );

			GOD_UNIT_TEST_EXPECT( (U8)lS32.b.b0 == lB0, "uS32.b0 failure" );
			GOD_UNIT_TEST_EXPECT( (U8)lS32.b.b1 == lB1, "uS32.b1 failure" );
			GOD_UNIT_TEST_EXPECT( (U8)lS32.b.b2 == lB2, "uS32.b2 failure" );
			GOD_UNIT_TEST_EXPECT( (U8)lS32.b.b3 == lB3, "uS32.b3 failure" );

			GOD_UNIT_TEST_EXPECT( lU32.b.b0 == lB0, "uU32.b0 failure" );
			GOD_UNIT_TEST_EXPECT( lU32.b.b1 == lB1, "uU32.b1 failure" );
			GOD_UNIT_TEST_EXPECT( lU32.b.b2 == lB2, "uU32.b2 failure" );
			GOD_UNIT_TEST_EXPECT( lU32.b.b3 == lB3, "uU32.b3 failure" );

			GOD_UNIT_TEST_EXPECT( (U8)lS16.b.b0 == lB0, "uS16.b0 failure" );
			GOD_UNIT_TEST_EXPECT( (U8)lS16.b.b1 == lB1, "uS16.b1 failure" );

			GOD_UNIT_TEST_EXPECT( lU16.b.b0 == lB0, "uU16.b0 failure" );
			GOD_UNIT_TEST_EXPECT( lU16.b.b1 == lB1, "uU16.b1 failure" );
		}

	}

	for( i = 0; i < mARRAY_COUNT( gBaseTestTagValues ); i++ )
	{
		U32 lAdd = 0;
		gBaseTestTagValues[ i ].ID = GOD_UNIT_TEST_RAND( 0xFFFF ) + lAdd;
		gBaseTestTagValues[ i ].Value = GOD_UNIT_TEST_RAND( 0xFFFF ) + lAdd;
		lAdd += 0x20000;
	}

	for( i = 0; i < mARRAY_COUNT( gBaseTestTagValues ); i++ )
	{
		U32 lVal = sTagValue_GetValue( gBaseTestTagValues[ i ].ID, gBaseTestTagValues, mARRAY_COUNT( gBaseTestTagValues ) );
		U32 lID = sTagValue_GetID( gBaseTestTagValues[ i ].Value, gBaseTestTagValues, mARRAY_COUNT( gBaseTestTagValues ) );
		GOD_UNIT_TEST_EXPECT( lVal == gBaseTestTagValues[ i ].Value, "sTagValue getvalue" );
		GOD_UNIT_TEST_EXPECT( lID == gBaseTestTagValues[ i ].ID, "sTagValue getid" );
	}

	for( i = 0; i < mARRAY_COUNT( gBaseTestTagStrings ); i++ )
	{
		char * lpString = sTagString_GetpString( gBaseTestTagStrings[ i ].ID, gBaseTestTagStrings, mARRAY_COUNT( gBaseTestTagStrings ) );
		U32 lID = sTagString_GetID( gBaseTestTagStrings[ i ].pString, gBaseTestTagStrings, mARRAY_COUNT( gBaseTestTagStrings ) );
		U8 lStringExist = sTagString_StringExists( gBaseTestTagStrings[ i ].pString, gBaseTestTagStrings, mARRAY_COUNT( gBaseTestTagStrings ) );
		U8 lTagExist = sTagString_IDExists( gBaseTestTagStrings[ i ].ID, gBaseTestTagStrings, mARRAY_COUNT( gBaseTestTagStrings ) );

		GOD_UNIT_TEST_EXPECT( lpString == gBaseTestTagStrings[ i ].pString, "sTagString getstring()" );
		GOD_UNIT_TEST_EXPECT( lID == gBaseTestTagStrings[ i ].ID, "sTagString getID()" );
		GOD_UNIT_TEST_EXPECT( !!lStringExist, "sTagString stringExists" );
		GOD_UNIT_TEST_EXPECT( !!lTagExist, "sTagString IDExists" );
	}

	{
		U32 lVal = mSTRING_TO_U32( 'R', 'G', 'O', 'D' );
		GOD_UNIT_TEST_EXPECT( lVal == 0x52474f44, "string to u32" );
	}

	{
		char lNumSpace[ 32 ];
		for( i=0; i<16; i++ )
		{ 
			S32	lVal0 = GOD_UNIT_TEST_RAND( 0xFFFF );
			S32 lVal1 = 0;
			lVal0 <<= 16;
			lVal0 |= GOD_UNIT_TEST_RAND( 0xFFFF );

			if( !i ) 
				lVal0 = 0;

			sprintf( lNumSpace, "%ld", lVal0 );
			lVal1 = AsciiToS32( lNumSpace );
			GOD_UNIT_TEST_EXPECT( lVal0 == lVal1, "ascii to s32" );

			sprintf( lNumSpace, "0x%lx", lVal0 );
			lVal1 = AsciiToS32( lNumSpace );
			GOD_UNIT_TEST_EXPECT( lVal0 == lVal1, "ascii to s32" );

			sprintf( lNumSpace, "0x%lX", lVal0 );
			lVal1 = AsciiToS32( lNumSpace );
			GOD_UNIT_TEST_EXPECT( lVal0 == lVal1, "ascii to s32" );

			sprintf( lNumSpace, "$%lx", lVal0 );
			lVal1 = AsciiToS32( lNumSpace );
			GOD_UNIT_TEST_EXPECT( lVal0 == lVal1, "ascii to s32" );
		}		
	}

	{
		U8	bytes[ 4 ];
		U16	r16;
		U32	r32;
		U16	w16;
		U32	w32;
		U16 t16;
		U32	t32;

		for( i = 0; i < 16; i++ )
		{
			bytes[ 0 ] = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			bytes[ 1 ] = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			bytes[ 2 ] = (U8)GOD_UNIT_TEST_RAND( 0xFF );
			bytes[ 3 ] = (U8)GOD_UNIT_TEST_RAND( 0xFF );

			r16 = bytes[ 1 ];
			r16 <<= 8;
			r16 |= bytes[ 0 ];

			t16 = bytes[ 0 ];
			t16 <<= 8;
			t16 |= bytes[ 1 ];

			r32 = bytes[ 3 ];
			r32 <<= 8;
			r32 |= bytes[ 2 ];
			r32 <<= 8;
			r32 |= bytes[ 1 ];
			r32 <<= 8;
			r32 |= bytes[ 0 ];

			t32 = bytes[ 0 ];
			t32 <<= 8;
			t32 |= bytes[ 1 ];
			t32 <<= 8;
			t32 |= bytes[ 2 ];
			t32 <<= 8;
			t32 |= bytes[ 3 ];

			w16 = r16;
			w32 = r32;

			Endian_Twiddle_U16( (U8*)&w16 );
			Endian_Twiddle_U32( (U8*)&w32 );

			GOD_UNIT_TEST_EXPECT( t16 == w16, "Endian twiddle U16" );
			GOD_UNIT_TEST_EXPECT( t32 == w32, "Endian twiddle U32" );


		}
	}

}
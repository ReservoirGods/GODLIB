#include	<GODLIB/UNITTEST/UNITTEST.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>

typedef struct sLLTestNode
{
	U32		mValue;
	struct sLLTestNode * mpNext;
} sLLTestNode;

GOD_UNIT_TEST( LinkedList )
{
	sLLTestNode	nodes[ 32 ];
	sLLTestNode * pHead = 0;
	sLLTestNode * pFound = 0;
	U16 i = 0, j;

	for( i = 0; i < 32; i++ )
		nodes[ i ].mValue = i;


	for( i=0; i<32; i++ )
	{ 
		GOD_LL_INSERT( pHead, mpNext, &nodes[i] );
		GOD_UNIT_TEST_EXPECT( pHead == &nodes[ i ], "insert head failed" );

		for( j = 1; j < i; j++ )
		{
			GOD_UNIT_TEST_EXPECT( nodes[j].mpNext == &nodes[ j-1 ], "insert head failed" );
		}
	}

	pHead = 0;
	for( i = 0; i < 32; i++ )
	{
		GOD_LL_INSERT_TAIL( sLLTestNode, pHead, mpNext, &nodes[ i ] );
		GOD_UNIT_TEST_EXPECT( pHead == &nodes[ 0 ], "insert tail failed" );

		for( j = 1; j < i; j++ )
		{
			GOD_UNIT_TEST_EXPECT( nodes[ j-1 ].mpNext == &nodes[ j ], "insert tail failed" );
		}
	}

	pHead = 0;
	pFound = 0;
	for( i = 0; i < 32; i++ )
	{
		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_ASSERT( 0 == pFound, "found item that isn't in list" );
		
		GOD_LL_INSERT( pHead, mpNext, &nodes[ i ] );

		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_ASSERT( pFound, "couldn't find item in list" );
	}

	/* remove heads linear */
	pHead = 0;
	for( i = 0; i < 32; i++ ) GOD_LL_INSERT( pHead, mpNext, &nodes[ 31-i ] );

	for( i = 0; i < 32; i++ )
	{
		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_EXPECT( pFound, "heads removal problem" );
		GOD_UNIT_TEST_EXPECT( pHead == &nodes[ i ], "removal problem" );

		GOD_LL_REMOVE( sLLTestNode, pHead, mpNext, &nodes[ i ] );

		GOD_UNIT_TEST_EXPECT( 0 == nodes[ i ].mpNext, "removal problem" );
		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_EXPECT( 0 == pFound, "heads removal problem" );
	}

	/* remove tails linear */
	pHead = 0;
	for( i = 0; i < 32; i++ ) GOD_LL_INSERT( pHead, mpNext, &nodes[ i ] );

	for( i = 0; i < 32; i++ )
	{
		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_EXPECT( pFound, "tails removal problem" );
		GOD_UNIT_TEST_EXPECT( pHead == &nodes[ 31 ], "tails removal problem" );

		GOD_LL_REMOVE( sLLTestNode, pHead, mpNext, &nodes[ i ] );

		GOD_UNIT_TEST_EXPECT( 0 == nodes[ i ].mpNext, "tails removal problem" );
		GOD_LL_FIND( pHead, mpNext, mValue, i, pFound );
		GOD_UNIT_TEST_EXPECT( 0 == pFound, "tails heads removal problem" );
	}

	/* remove central linear */
	pHead = 0;
	for( i = 0; i < 32; i++ ) GOD_LL_INSERT( pHead, mpNext, &nodes[ i ] );

	for( i = 0; i < 32; i++ )
	{
		j = ( i + 16 ) & 31;
		GOD_LL_FIND( pHead, mpNext, mValue, j, pFound );
		GOD_UNIT_TEST_EXPECT( pFound, "central removal problem" );

		GOD_LL_REMOVE( sLLTestNode, pHead, mpNext, &nodes[ j ] );
		GOD_UNIT_TEST_EXPECT( 0 == nodes[ j ].mpNext, "removal problem" );
		GOD_LL_FIND( pHead, mpNext, mValue, j, pFound );
		GOD_UNIT_TEST_EXPECT( 0 == pFound, "central removal problem" );
	}


}
/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB\UNITTEST\UNITTEST.H>

#include	<GODLIB\HASHTREE\HASHTREE.H>

/* ###################################################################################
#  DATA
################################################################################### */

/* the tree we will be using for all these tests */
sHashTree	gHashTreeUT;


/* a set of variables to read/write, one for each integral type */

#define	GOD_UNIT_TEST_TYPE_ACTION( aType )	\
	aType	gHashTreeTestVar_##aType;
	GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION


/* class containint details on each of our variables */

typedef struct sHashTreeVarDef
{
	const char * mpName;
	U32			mSize;
	void *		mpData;
	sHashTreeVar 	mVar;
	sHashTreeVarClient  mClient;
}sHashTreeVarDef;


/* define initial types, names, sizes and pointers to variables */

sHashTreeVarDef gHashTreeVarDef[] =
{
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )	\
		{ "ROOT\\TestVar_"#aType, sizeof(aType), &gHashTreeTestVar_##aType, 0, 0 },
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
};


/* structure for tracking callbacks */

typedef struct sHashTreeVarTrack
{
	U16	mInitCount;
	U16	mDeInitCount;
	U16	mWriteCount;
	U32 mWriteValue;
}sHashTreeVarTrack;

sHashTreeVarTrack	gHashTreeVarTracks[ dGOD_UNIT_TEST_TYPE_LIMIT ];


/* ###################################################################################
#  CODE
################################################################################### */


/* generic read of data based on type size*/

U32	HashTreeUT_GetValue( sHashTreeVarClient * apClient )
{
	U32 lVal = 0;
	switch( apClient->mpVar->mDataSize )
	{
	case 1:
		lVal = *(U8*)apClient->mpVar->mpData;
		break;
	case 2:
		lVal = *(U16*)apClient->mpVar->mpData;
		break;
	case 4:
		lVal = *(U32*)apClient->mpVar->mpData;
		break;
	}
	return lVal;
}


/* callbacks on node events*/

void	HashTreeUT_OnInit( sHashTreeVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gHashTreeVarTracks[ apClient->mUserData ].mInitCount++;
}

void	HashTreeUT_OnDeInit( sHashTreeVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gHashTreeVarTracks[ apClient->mUserData ].mDeInitCount++;
}

void	HashTreeUT_OnWrite( sHashTreeVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gHashTreeVarTracks[ apClient->mUserData ].mWriteCount++;
	gHashTreeVarTracks[ apClient->mUserData ].mWriteValue = HashTreeUT_GetValue( apClient );
}


GOD_UNIT_TEST( HashTree )
{
	U16 i,j;
	HashTree_Init( &gHashTreeUT );

	/* init tree variaables */

	for( i = 0; i < mARRAY_COUNT( gHashTreeVarDef ); i++ )
	{
		HashTree_Var_Init( &gHashTreeVarDef[ i ].mVar, &gHashTreeUT, gHashTreeVarDef[i].mpName, gHashTreeVarDef[ i ].mSize, gHashTreeVarDef[ i ].mpData );
/*		GOD_UNIT_TEST_ASSERT( gHashTreeVarDef[ i ].mVar.mpVar, "couldn't init has tree var" );*/
	}

	/* test that reading/writing return same value */

	for( j = 0; j < 16; j++ )
	{
		i = 0;
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )															\
		{																							\
			aType lVal0 = (aType)GOD_UNIT_TEST_RAND32();											\
			aType lVal1 = 0;																		\
			HashTree_VarWrite( &gHashTreeVarDef[ i ].mVar, &lVal0 );								\
			HashTree_VarRead( &gHashTreeVarDef[ i ].mVar, &lVal1, gHashTreeVarDef[ i ].mSize );		\
			i++;																					\
		}
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
	}


	/* init callbacks */

	for( i = 0; i < mARRAY_COUNT( gHashTreeVarTracks ); i++ )
	{
		gHashTreeVarTracks[ i ].mInitCount = 0;
		gHashTreeVarTracks[ i ].mDeInitCount = 0;
		gHashTreeVarTracks[ i ].mWriteCount = 0;
		gHashTreeVarTracks[ i ].mWriteValue = 0;

		gHashTreeVarDef[ i ].mClient.mfOnDeInit = HashTreeUT_OnDeInit;
		gHashTreeVarDef[ i ].mClient.mfOnInit = HashTreeUT_OnInit;
		gHashTreeVarDef[ i ].mClient.mUserData = i;

		HashTree_VarClient_Init( &gHashTreeVarDef[ i ].mClient, &gHashTreeUT, gHashTreeVarDef[ i ].mpName, HashTreeUT_OnWrite );
	}


	/* test write callbacks */

	for( j = 0; j < 16; j++ )
	{
		i = 0;
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )																						\
		{																														\
			U16 lCountOld = gHashTreeVarTracks[ i ].mWriteCount;																\
			aType lTemp = (aType)GOD_UNIT_TEST_RAND32();																		\
			HashTree_VarWrite( &gHashTreeVarDef[ i ].mVar, &lTemp );															\
			GOD_UNIT_TEST_EXPECT( lCountOld + 1 == gHashTreeVarTracks[ i ].mWriteCount, "write callback not triggered" );		\
			GOD_UNIT_TEST_EXPECT( lTemp == (aType)gHashTreeVarTracks[ i ].mWriteValue, "write callback not getting value" );	\
			i++;																												\
		}
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
	}

	/* deinit and test deinit callbacks */

	for( i = 0; i < mARRAY_COUNT( gHashTreeVarDef ); i++ )
	{
		U16 lCountOld = gHashTreeVarTracks[ i ].mDeInitCount;
		HashTree_Var_DeInit( &gHashTreeVarDef[ i ].mVar, &gHashTreeUT );
		GOD_UNIT_TEST_EXPECT( lCountOld + 1 == gHashTreeVarTracks[ i ].mDeInitCount, "deinit callback not triggered" );
	}

	/* unregister callbacks */

	for( i = 0; i < mARRAY_COUNT( gHashTreeVarTracks ); i++ )
	{
		HashTree_VarClient_DeInit( &gHashTreeVarDef[ i ].mClient, &gHashTreeUT );
	}

	/* destroy tree */

	HashTree_DeInit( &gHashTreeUT );
}


/* ################################################################################ */
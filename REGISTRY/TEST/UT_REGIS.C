/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB\UNITTEST\UNITTEST.H>

#include	<GODLIB\REGISTRY\REGISTRY.H>

/* ###################################################################################
#  DATA
################################################################################### */

/* the tree we will be using for all these tests */
sRegistry	gRegistryUT;


/* a set of variables to read/write, one for each integral type */

#define	GOD_UNIT_TEST_TYPE_ACTION( aType )	\
	aType	gRegistryTestVar_##aType;
	GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION


/* class containint details on each of our variables */

typedef struct sRegistryVarDef
{
	const char * mpName;
	U32			mSize;
	void *		mpData;
	sRegistryVar *	mpVar;
	sRegistryVarClient * mpClient;
}sRegistryVarDef;


/* define initial types, names, sizes and pointers to variables */

sRegistryVarDef gRegistryVarDef[] =
{
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )	\
		{ "ROOT\\TestVar_"#aType, sizeof(aType), &gRegistryTestVar_##aType, 0, 0 },
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
};


/* structure for tracking callbacks */

typedef struct sRegistryVarTrack
{
	U16	mInitCount;
	U16	mDeInitCount;
	U16	mWriteCount;
	U32 mWriteValue;
}sRegistryVarTrack;

sRegistryVarTrack	gRegistryVarTracks[ dGOD_UNIT_TEST_TYPE_LIMIT ];


/* ###################################################################################
#  CODE
################################################################################### */


/* generic read of data based on type size*/

U32	RegistryUT_GetValue( sRegistryVarClient * apClient )
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

void	RegistryUT_OnInit( sRegistryVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gRegistryVarTracks[ apClient->mUserData ].mInitCount++;
}

void	RegistryUT_OnDeInit( sRegistryVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gRegistryVarTracks[ apClient->mUserData ].mDeInitCount++;
}

void	RegistryUT_OnWrite( sRegistryVarClient * apClient )
{
	GOD_UNIT_TEST_ASSERT( apClient->mUserData < dGOD_UNIT_TEST_TYPE_LIMIT, "user data out of range" );
	gRegistryVarTracks[ apClient->mUserData ].mWriteCount++;
	gRegistryVarTracks[ apClient->mUserData ].mWriteValue = RegistryUT_GetValue( apClient );
}


GOD_UNIT_TEST( Registry )
{
	U16 i,j;
	Registry_Init( &gRegistryUT );

	/* init tree variaables */

	for( i = 0; i < mARRAY_COUNT( gRegistryVarDef ); i++ )
	{
		gRegistryVarDef[ i ].mpVar = Registry_VarInit( &gRegistryUT, gRegistryVarDef[i].mpName, gRegistryVarDef[ i ].mSize, gRegistryVarDef[ i ].mpData );
		GOD_UNIT_TEST_ASSERT( gRegistryVarDef[ i ].mpVar, "couldn't init has tree var" );
	}

	/* test that reading/writing return same value */

	for( j = 0; j < 16; j++ )
	{
		i = 0;
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )														\
		{																						\
			aType lVal0 = (aType)GOD_UNIT_TEST_RAND32();										\
			aType lVal1 = 0;																	\
			Registry_VarWrite( gRegistryVarDef[ i ].mpVar, &lVal0 );							\
			Registry_VarRead( gRegistryVarDef[ i ].mpVar, &lVal1, gRegistryVarDef[ i ].mSize );	\
			i++;																				\
		}
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
	}


	/* init callbacks */

	for( i = 0; i < mARRAY_COUNT( gRegistryVarTracks ); i++ )
	{
		gRegistryVarTracks[ i ].mInitCount = 0;
		gRegistryVarTracks[ i ].mDeInitCount = 0;
		gRegistryVarTracks[ i ].mWriteCount = 0;
		gRegistryVarTracks[ i ].mWriteValue = 0;

		gRegistryVarDef[ i ].mpClient = Registry_VarClientRegister( &gRegistryUT, gRegistryVarDef[ i ].mpName, RegistryUT_OnWrite, RegistryUT_OnInit, RegistryUT_OnDeInit, i );
	}


	/* test write callbacks */

	for( j = 0; j < 16; j++ )
	{
		i = 0;
#define	GOD_UNIT_TEST_TYPE_ACTION( aType )																						\
		{																														\
			U16 lCountOld = gRegistryVarTracks[ i ].mWriteCount;																\
			aType lTemp = (aType)GOD_UNIT_TEST_RAND32();																		\
			Registry_VarWrite( gRegistryVarDef[ i ].mpVar, &lTemp );															\
			GOD_UNIT_TEST_EXPECT( lCountOld + 1 == gRegistryVarTracks[ i ].mWriteCount, "write callback not triggered" );		\
			GOD_UNIT_TEST_EXPECT( lTemp == (aType)gRegistryVarTracks[ i ].mWriteValue, "write callback not getting value" );	\
			i++;																												\
		}
		GOD_UNIT_TEST_TYPES()
#undef	GOD_UNIT_TEST_TYPE_ACTION
	}

	/* deinit and test deinit callbacks */

	for( i = 0; i < mARRAY_COUNT( gRegistryVarDef ); i++ )
	{
		U16 lCountOld = gRegistryVarTracks[ i ].mDeInitCount;
		Registry_VarDeInit( &gRegistryUT, gRegistryVarDef[ i ].mpVar );
		GOD_UNIT_TEST_EXPECT( lCountOld + 1 == gRegistryVarTracks[ i ].mDeInitCount, "deinit callback not triggered" );
	}

	/* unregister callbacks */

	for( i = 0; i < mARRAY_COUNT( gRegistryVarTracks ); i++ )
	{
		Registry_VarClientUnRegister( &gRegistryUT, gRegistryVarDef[ i ].mpClient );
	}

	/* destroy tree */

	Registry_DeInit( &gRegistryUT );
}


/* ################################################################################ */
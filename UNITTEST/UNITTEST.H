#ifndef	INCLUDED_UNITTEST_H
#define	INCLUDED_UNITTEST_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB\BASE\BASE.H>
#include	<GODLIB\ASSERT\ASSERT.H>
#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\RANDOM\RANDOM.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sUnitTestContext
{
	const char *		mpTestName;
	U16					mTestCount;
	U16					mErrorCount;
	sRandomSeed			mSeed;
} sUnitTestContext;

extern sUnitTestContext  gUnitTestContext;

#define	GOD_UNIT_TEST( _aName )						\
	void	UnitTest_Action_##_aName( void );		\
	void	UnitTest##_aName( void )				\
	{												\
		printf( "Testing: %s\n", #_aName );			\
		gUnitTestContext.mTestCount = 0;			\
		gUnitTestContext.mErrorCount = 0;			\
		gUnitTestContext.mpTestName = #_aName;		\
		UnitTest_Action_##_aName();					\
	}												\
	void	UnitTest_Action_##_aName( void )


#define GOD_UNIT_TEST_TYPES()			\
	GOD_UNIT_TEST_TYPE_ACTION( U8 )		\
	GOD_UNIT_TEST_TYPE_ACTION( S8 )		\
	GOD_UNIT_TEST_TYPE_ACTION( U16 )	\
	GOD_UNIT_TEST_TYPE_ACTION( S16 )	\
	GOD_UNIT_TEST_TYPE_ACTION( U32 )	\
	GOD_UNIT_TEST_TYPE_ACTION( S32 )	

#define	dGOD_UNIT_TEST_TYPE_LIMIT	6

#define	GOD_UNIT_TEST_INVOKE( _aName )		{ void UnitTest##_aName( void); { UnitTest##_aName(); } }
#define GOD_UNIT_TEST_RAND( _aNum )			RandomSeed_GetClamped( &gUnitTestContext.mSeed, _aNum )
#define GOD_UNIT_TEST_RAND32()				RandomSeed_GetClamped32( &gUnitTestContext.mSeed )

#define	GOD_UNIT_TEST_EXPECT( _aValue, _aMsg ) { if( !( _aValue ) ) { DebugLog_Printf3( " %s @ %s (%d)\n", _aMsg, __FILE__, __LINE__ ); gUnitTestContext.mErrorCount++; } }
#define	GOD_UNIT_TEST_ASSERT( _aValue, _aMsg ) { if( !( _aValue ) ) { DebugLog_Printf3( " %s @ %s (%d)\n", _aMsg, __FILE__, __LINE__ ); gUnitTestContext.mErrorCount++; return; } }


#define	GOD_UNIT_TEST_GUARDED_STRUCT( _aType, _aName )	U8 _aName[ sizeof(_aType) + 32]; { U16 _i; for( _i=0;_i<16;_i++) _aName[ _i ] = _aName[ sizeof(_aType)+_i+16] = (U8)(0xE0 + _i);  }
#define	GOD_UNIT_TEST_GUARDED_STRUCT_BEGIN( _aType, _aName )	(_aType*)(&_aName [16])
#define	GOD_UNIT_TEST_GUARDED_STRUCT_CHECK2( _aType, _aName )	{ U16 _i; for( _i=0;_i<16;_i++) GOD_UNIT_TEST_EXPECT( _aName[ _i ] == _aName[ sizeof(_aType)+_i+16] == (U8)(0xE0 + _i), "scribbler" );  }
#define	GOD_UNIT_TEST_GUARDED_STRUCT_CHECK( _aType, _aName )	{ U16 _i; for( _i=0;_i<16;_i++) { GOD_UNIT_TEST_EXPECT( _aName[ _i ] == (U8)(0xE0 + _i), "scribbler" ); GOD_UNIT_TEST_EXPECT( _aName[ _i + sizeof(_aType) + 16 ] == (U8)(0xE0 + _i), "scribbler" ); }  }


/* ################################################################################ */

#endif	/*	INCLUDED_SCREEN_H	*/

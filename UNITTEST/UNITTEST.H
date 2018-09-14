#ifndef	INCLUDED_UNITTEST_H
#define	INCLUDED_UNITTEST_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB\BASE\BASE.H>
#include	<GODLIB\ASSERT\ASSERT.H>
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


#define	GOD_UNIT_TEST_INVOKE( _aName )		{ void UnitTest##_aName( void); { UnitTest##_aName(); } }
#define GOD_UNIT_TEST_RAND( _aNum )			RandomSeed_GetClamped( &gUnitTestContext.mSeed, _aNum )

#define	GOD_UNIT_TEST_EXPECT( _aValue, _aMsg ) { if( !( _aValue ) ) { printf( " %s\n", _aMsg ); gUnitTestContext.mErrorCount++; } }
#define	GOD_UNIT_TEST_ASSERT( _aValue, _aMsg ) { if( !( _aValue ) ) { printf( " %s\n", _aMsg ); gUnitTestContext.mErrorCount++; return; } }

/* ################################################################################ */

#endif	/*	INCLUDED_SCREEN_H	*/

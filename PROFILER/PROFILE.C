/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PROFILE.H"

#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#ifdef	dGODLIB_PLATFORM_WIN
#include	<Windows.h>
#endif


/* ###################################################################################
#  DATA
################################################################################### */

U32	gProfilerHiTideTag;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Profile_Init( sProfile * apProfile )
* ACTION   : Profile_Init
* CREATION : 09.05.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Profile_Init( sProfile * apProfile )
{
	Memory_Clear( sizeof(sProfile), apProfile );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Profile_CliPrint( sProfile * apProfile,const char * apName )
* ACTION   : Profile_CliPrint
* CREATION : 09.05.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Profile_CliPrint( sProfile * apProfile,const char * apName )
{
	if( apProfile )
	{
		if( apProfile->mHitCount )
		{
			apProfile->mAverage = apProfile->mTotal / apProfile->mHitCount;
		}
		Cli_PrintfLine5( "%s %ld %ld %ld %ld", apName, apProfile->mCurrent, apProfile->mAverage, apProfile->mHiTide, apProfile->mHiTideTag );
	}}

U32		Profile_GetCPUCycleCount( void )
{
	U32				lRes;
#ifdef	dGODLIB_PLATFORM_WIN
	LARGE_INTEGER  lCount;
	LARGE_INTEGER  lFreq;
	LARGE_INTEGER	lScale;

	QueryPerformanceCounter( &lCount );
	QueryPerformanceFrequency( &lFreq );

	lScale.QuadPart = lFreq.QuadPart / 8000000;	/* rescale to 8mz */
	if( lScale.QuadPart )
	{
		lCount.QuadPart /= lScale.QuadPart;
	}

	lRes = (U32)lCount.QuadPart;
#else
	lRes = 0;
#endif
	return( lRes );
}


/* ################################################################################ */

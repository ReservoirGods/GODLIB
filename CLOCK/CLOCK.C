/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CLOCK.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>


#ifdef	dGODLIB_SYSTEM_D3D
#include	<d3dx9.h>
#endif
#ifdef dGODLIB_PLATFORM_WIN
#include	<windows.h>
#endif

#include	<TIME.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8	Result;
	U8	Rem;
} sClockDiv;


/* ###################################################################################
#  DATA
################################################################################### */

sClockDiv	gClockDiv60[ 512 ];
sClockDiv	gClockDiv200[ 512 ];
sTime		gClockTime;
U32			gClockTicks;
U16			gClockSubTicks;
U32			gClockTickAdd;
U16			gClockSubTickAdd;
U8			gClockFrameRate;
U32			gClockStartTicks;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Clock_Cli_GameSeconds( const char * apArgs );
void	Clock_Cli_Info( const char * apArgs );

extern	void		Clock_TimeVbl( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_Init( void )
* ACTION   : Clock_Init
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_Init( void )
{
	uU32	lVal;
	U16		i;

	for( i=0; i<512; i++ )
	{
		gClockDiv60[ i ].Result = (U8)(i/60);
		gClockDiv60[ i ].Rem    = (U8)(i%60);

		gClockDiv200[ i ].Result = (U8)(i/200);
		gClockDiv200[ i ].Rem    = (U8)(i%200);
	}

	gClockFrameRate     = (U8)System_GetRefreshRate()->w.w1;

	lVal.w.w1           = 200;
	lVal.w.w0           = 0;

	lVal.l             /= System_GetRefreshRate()->w.w1;

	gClockSubTickAdd = lVal.w.w0;
	gClockTickAdd    = (U32)lVal.w.w1;

	gClockTicks      = 0;
	gClockSubTicks   = 0;

	gClockStartTicks = 0;
#ifdef	dGODLIB_PLATFORM_WIN
	gClockStartTicks = GetTickCount();
#endif

	Vbl_AddCall( Clock_TimeVbl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_DeInit( void )
* ACTION   : Clock_DeInit
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_DeInit( void )
{
	Vbl_RemoveCall( Clock_TimeVbl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_Update( sClock * apClock )
* ACTION   : Clock_Update
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_Update( sClock * apClock )
{
/*#ifdef	dGODLIB_PLATFORM_WIN*/
#if 0
	S32	lNewTicks;
	S32	lTicks;

	lNewTicks        = GetTickCount();
	lTicks           = lNewTicks - gClockStartTicks;
	gClockStartTicks = lNewTicks;
	gClockTicks     += (lTicks/5);

	lTicks /= 8;
	if( (lTicks + gClockTime.mMicroSeconds) > 128 )
	{
		gClockTime.mMicroSeconds = 0;
		gClockTime.mSeconds++;
		if( gClockTime.mSeconds > 59 )
		{
			gClockTime.mSeconds = 0;
			gClockTime.mHours++;
		}
	}
	else
	{
		gClockTime.mMicroSeconds = (U8)(gClockTime.mMicroSeconds + lTicks);
	}

#endif

	if( apClock->mState == eCLOCK_STATE_ACTIVE )
	{
		Time_Sub( &gClockTime,                   &apClock->mStartTime,   &apClock->mElapsedTime );
		if( *(U32*)&apClock->mCountDownStartTime )
		{
			if( Time_ToU32(&apClock->mElapsedTime) >= Time_ToU32(&apClock->mCountDownStartTime) )
			{
				*(U32*)&apClock->mCountDownTime = 0;
			}
			else
			{
				Time_Sub( &apClock->mCountDownStartTime, &apClock->mElapsedTime, &apClock->mCountDownTime );
			}
		}
		else
		{
			*(U32*)&apClock->mCountDownTime = 0;
		}

		apClock->mElapsedTicks = gClockTicks - apClock->mStartTicks;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_Start( sClock * apClock )
* ACTION   : Clock_Start
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_Start( sClock * apClock )
{
	apClock->mState                       = eCLOCK_STATE_ACTIVE;
	apClock->mStartTime                   = gClockTime;
	apClock->mPauseTime                   = apClock->mStartTime;

	apClock->mElapsedTime.mHours          = 0;
	apClock->mElapsedTime.mMinutes        = 0;
	apClock->mElapsedTime.mSeconds        = 0;
	apClock->mElapsedTime.mMicroSeconds   = 0;

	apClock->mCountDownTime.mHours        = apClock->mCountDownStartTime.mHours;
	apClock->mCountDownTime.mMinutes      = apClock->mCountDownStartTime.mMinutes;
	apClock->mCountDownTime.mSeconds      = apClock->mCountDownStartTime.mSeconds;
	apClock->mCountDownTime.mMicroSeconds = apClock->mCountDownStartTime.mMicroSeconds;

	apClock->mStartTicks                  = gClockTicks;
	apClock->mPauseTicks                  = apClock->mStartTicks;
	apClock->mElapsedTicks                = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_Stop( sClock * apClock )
* ACTION   : Clock_Stop
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_Stop( sClock * apClock )
{
	apClock->mState = eCLOCK_STATE_INACTIVE;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_Pause( sClock * apClock )
* ACTION   : Clock_Pause
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_Pause( sClock * apClock )
{
	apClock->mPauseTime  = gClockTime;
	apClock->mState      = eCLOCK_STATE_PAUSE;
	apClock->mPauseTicks = gClockTicks;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Clock_UnPause( sClock * apClock )
* ACTION   : Clock_UnPause
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Clock_UnPause( sClock * apClock )
{
	sTime	lTime;

	Time_Sub( &gClockTime,         &apClock->mPauseTime, &lTime             );
	Time_Add( &apClock->mStartTime, &lTime,             &apClock->mStartTime );
	apClock->mStartTicks += (gClockTicks - apClock->mPauseTicks);
	apClock->mState       = eCLOCK_STATE_ACTIVE;

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Time_ToU32( sTime * apTime )
* ACTION   : Time_ToU32
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U32	Time_ToU32( sTime * apTime )
{
	U32	lTicks;

	lTicks  = apTime->mMicroSeconds;
	lTicks += (U32)apTime->mSeconds * 200L;
	lTicks += (U32)apTime->mMinutes * 60L * 200L;
	lTicks += (U32)apTime->mHours   * 60L * 60L * 200L;

	return( lTicks );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Time_FromU32( sTime * apTime,U32 aTicks )
* ACTION   : Time_FromU32
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Time_FromU32( sTime * apTime,U32 aTicks )
{
	U32	lTicks;

	lTicks            = aTicks / (60L * 60L * 200L);
	apTime->mHours    = (U8)lTicks;
	aTicks           -= lTicks * (60L * 60L * 200L);

	lTicks            = aTicks / (60L * 200L);
	apTime->mMinutes  = (U8)lTicks;
	aTicks           -= lTicks * (60L * 200L);

	lTicks            = aTicks / 200L;
	apTime->mSeconds  = (U8)lTicks;
	aTicks           -= lTicks * 200L;

	apTime->mMicroSeconds = (U8)aTicks;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Time_Add( sTime * apSrc0,sTime * apSrc1,sTime * apDst )
* ACTION   : Time_Add
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Time_Add( sTime * apSrc0,sTime * apSrc1,sTime * apDst )
{
	U16	lWord;

	lWord  = apSrc0->mMicroSeconds;
	lWord  = (U16)(lWord + apSrc1->mMicroSeconds);

	apDst->mMicroSeconds = gClockDiv200[ lWord ].Rem;

	lWord = gClockDiv200[ lWord ].Result;
	lWord = (U16)(lWord + apSrc0->mSeconds);
	lWord = (U16)(lWord + apSrc1->mSeconds);

	apDst->mSeconds = gClockDiv60[ lWord ].Rem;

	lWord = gClockDiv60[ lWord ].Result;
	lWord = (U16)(lWord + apSrc0->mMinutes);
	lWord = (U16)(lWord + apSrc1->mMinutes);

	apDst->mMinutes = gClockDiv60[ lWord ].Rem;

	lWord = gClockDiv60[ lWord ].Result;
	lWord = (U16)(lWord + apSrc0->mHours);
	lWord = (U16)(lWord + apSrc1->mHours);

	apDst->mHours = gClockDiv60[ lWord ].Rem;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Time_Sub( sTime * apSrc0,sTime * apSrc1,sTime * apDst )
* ACTION   : Time_Sub
* CREATION : 08.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Time_Sub( sTime * apSrc0,sTime * apSrc1,sTime * apDst )
{
	S16	lWord;
	S16	lFlow;

	lWord = apSrc0->mMicroSeconds;
	lWord = (S16)(lWord - apSrc1->mMicroSeconds);

	if( lWord < 0 )
	{
		lWord += 200;
		lFlow  = 1;
	}
	else
	{
		lFlow  = 0;
	}

	apDst->mMicroSeconds = gClockDiv200[ lWord ].Rem;

	lWord = apSrc0->mSeconds;
	lWord = (S16)(lWord - apSrc1->mSeconds);
	lWord = (S16)(lWord -lFlow);

	if( lWord < 0 )
	{
		lWord += 60;
		lFlow  = 1;
	}
	else
	{
		lFlow = 0;
	}
	apDst->mSeconds = gClockDiv60[ lWord ].Rem;

	lWord = apSrc0->mMinutes;
	lWord = (S16)(lWord-apSrc1->mMinutes);
	lWord = (S16)(lWord-lFlow);

	if( lWord < 0 )
	{
		lWord += 60;
		lFlow  = 1;
	}
	else
	{
		lFlow = 0;
	}
	apDst->mMinutes = gClockDiv60[ lWord ].Rem;

	lWord = apSrc0->mHours;
	lWord = (S16)(lWord-apSrc1->mHours);
	lWord =(S16)(lWord-lFlow);

	apDst->mHours = (U8)lWord;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Time_GetAbsTime( void )
* ACTION   : Time_GetAbsTime
* CREATION : 12.02.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Time_GetAbsTime( void )
{
	U32	lCombine;
#ifdef	dGODLIB_PLATFORM_WIN2
	lCombine = 0;
#else
	time_t		lTimeT;
	struct tm * lpTime;
	U32	lTime;
	U32	lDate;

	time( &lTimeT );
	lpTime = gmtime( &lTimeT );

	lTime  = 0;
	lTime  = (lpTime->tm_sec>>1) & 31L;
	lTime |= (lpTime->tm_min & 63L) << 5L;
	lTime |= (lpTime->tm_hour & 31L) << 11L;

	lDate  =(lpTime->tm_mday & 31);
	lDate |=(((lpTime->tm_mon+1) & 15)<<5L);
	lDate |=(((lpTime->tm_year-80) & 127)<<9L);

/*	DebugLog_Printf1( "tm_mday: %d\n", lpTime->tm_mday );
	DebugLog_Printf1( "tm_mon: %d\n", lpTime->tm_mon );
	DebugLog_Printf1( "tm_year: %d\n", lpTime->tm_year );

	DebugLog_Printf1( "lTime: %lx\n", lTime );
	DebugLog_Printf1( "lDate: %lx\n", lDate );*/

	lCombine = (lDate<<16L) | lTime;
#endif
	return( lCombine );
}

#ifndef dGODLIB_PLATFORM_ATARI

ULARGE_INTEGER	gClockBaseTime = { 0 };

void		Clock_TimeVbl( void ) 
{
#if 0
	SYSTEMTIME sysTime;
	FILETIME fileTime;
	ULARGE_INTEGER	timeBits;

	if( 0 == gClockBaseTime.QuadPart )
	{
		GetSystemTime( &sysTime );
		SystemTimeToFileTime( &sysTime, &fileTime );
		gClockBaseTime.LowPart = fileTime.dwLowDateTime;
		gClockBaseTime.HighPart = fileTime.dwHighDateTime;
	}

	GetSystemTime( &sysTime );
	SystemTimeToFileTime( &sysTime, &fileTime );
	timeBits.LowPart = fileTime.dwLowDateTime;
	timeBits.HighPart = fileTime.dwHighDateTime;

	timeBits.QuadPart -= gClockBaseTime.QuadPart;

	fileTime.dwHighDateTime = timeBits.HighPart;
	fileTime.dwLowDateTime = timeBits.LowPart;

	FileTimeToSystemTime( &fileTime, &sysTime );

	gClockTime.mMicroSeconds = (U8)sysTime.wMilliseconds;
	gClockTime.mSeconds = (U8)sysTime.wSecond;
	gClockTime.mMinutes = (U8)sysTime.wMinute;
	gClockTime.mHours = (U8)sysTime.wHour;

#else
	gClockTime.mMicroSeconds++;
	if( gClockTime.mMicroSeconds >= gClockFrameRate )
	{
		gClockTime.mMicroSeconds=0;
		gClockTime.mSeconds++;
		if( gClockTime.mSeconds >= 60 )
		{
			gClockTime.mSeconds = 0;
			gClockTime.mMinutes++;
			if( gClockTime.mMinutes >= 60 )
			{
				gClockTime.mMinutes = 0;
				gClockTime.mHours++;
			}
		}
	}
#endif
}

#endif // !GODLIB_PLATFORM_ATARI

/* ################################################################################ */

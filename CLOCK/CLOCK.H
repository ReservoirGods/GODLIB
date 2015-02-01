#ifndef	INCLUDED_CLOCK_H
#define	INCLUDED_CLOCK_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */


enum
{
	eCLOCK_STATE_INACTIVE,
	eCLOCK_STATE_ACTIVE,
	eCLOCK_STATE_PAUSE
};

typedef struct
{
	U8	mHours;
	U8	mMinutes;
	U8	mSeconds;
	U8	mMicroSeconds;
} sTime;

typedef	struct
{
	U16			mState;
	U32			mDuration;
	sTime		mStartTime;
	sTime		mPauseTime;
	sTime		mElapsedTime;
	sTime		mCountDownStartTime;
	sTime		mCountDownTime;
	U32			mStartTicks;
	U32			mPauseTicks;
	U32			mElapsedTicks;
} sClock;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Clock_Init( void );
void	Clock_DeInit( void );

void	Clock_Update( sClock * apClock );
void	Clock_Start( sClock * apClock );
void	Clock_Stop( sClock * apClock );
void	Clock_Pause( sClock * apClock );
void	Clock_UnPause( sClock * apClock );

U32		Time_ToU32(   sTime * apTime );
void	Time_FromU32( sTime * apTime, U32 aTicks );

void	Time_Add( sTime * apSrc0, sTime * apSrc1, sTime * apDst );
void	Time_Sub( sTime * apSrc0, sTime * apSrc1, sTime * apDst );

U32		Time_GetAbsTime( void );


/* ################################################################################ */

#endif	/*	INCLUDED_CLOCK_H	*/

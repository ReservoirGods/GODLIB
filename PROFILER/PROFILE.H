#ifndef	INCLUDED_PROFILE_H
#define	INCLUDED_PROFILE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct	sProfile
{
	U32	mHitCount;
	U32	mCurrent;
	U32	mTotal;
	U32	mAverage;
	U32	mHiTide;
	U32	mHiTideTag;
} sProfile;

extern	U32	gProfilerHiTideTag;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U32		Profile_GetCPUCycleCount( void );
void	Profile_Init( sProfile * apProfile );
void	Profile_CliPrint( sProfile * apProfile, const char * apName );

#define	Profile_SetHiTidetag( _aT )	gProfilerHiTideTag = _aT;


#ifdef	dGODLIB_PLATFORM_ATARI
/*
#define	Profile_CpuBegin( _aP )	(_aP)->mCurrent = *(U32*)0xFFC10CL

#define	Profile_CpuEnd( _aP )								\
	(_aP)->mCurrent = *(U32*)0xFFC10CL - (_aP)->mCurrent;	\
	if( (S32)(_aP)->mCurrent >= 0 )                 \
	{														\
		(_aP)->mTotal  += (_aP)->mCurrent;						\
		(_aP)->mHitCount++;										\
		if( (_aP)->mCurrent > (_aP)->mHiTide )					\
		{														\
			(_aP)->mHiTide = (_aP)->mCurrent;					\
		(_aP)->mHiTideTag = gProfilerHiTideTag;					\
		}													\
	}
*/

#define	Profile_CpuBegin( _aP )
#define	Profile_CpuEnd( _aP )


#else

#define	Profile_CpuBegin( _aP )	(_aP)->mCurrent = Profile_GetCPUCycleCount();

#define	Profile_CpuEnd( _aP )								\
	(_aP)->mCurrent = Profile_GetCPUCycleCount() - (_aP)->mCurrent;			\
	(_aP)->mTotal  += (_aP)->mCurrent;						\
	(_aP)->mHitCount++;										\
	if( (_aP)->mCurrent > (_aP)->mHiTide )					\
	{														\
		(_aP)->mHiTide = (_aP)->mCurrent;					\
		(_aP)->mHiTideTag = gProfilerHiTideTag;					\
	}

#endif


/* ################################################################################ */

#endif	/* INCLUDED_PROFILE_H */

#ifndef	INCLUDED_VBL_H
#define	INCLUDED_VBL_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/MFP/MFP.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dVBL_MAX_CALLS	64


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8		mLockFlag;
	U8		mTimerBScanLine;
	U16		mHbiCounterStart;
	U16		mHbiCounter;
	U16		mCallCount;
	void	(*mfHbi)();
	void	(*mfTimerBFunc)();
	void	(*mfVideoFunc)();
	void 	(*mfCalls[ dVBL_MAX_CALLS ])(void);
} sVBL;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Vbl_Init( void );
void	Vbl_DeInit( void  );
U8		Vbl_AddCall( void (* apFunc)(void) );
U8		Vbl_RemoveCall( void (* apFunc)(void) );
U32		Vbl_GetCounter( void );
void	Vbl_WaitVbl( void );
void	Vbl_WaitVbls( const U16 aCount );
void	Vbl_InstallTimerB( sMfpTimer * apTimer );
void	Vbl_InstallHbi( sMfpTimer * apTimer );
sVBL *	Vbl_GetpVbl( void );
void	Vbl_SetVideoFunc( void (* apFunc)() );
void	Vbl_CallsProcess( void );


/* ################################################################################ */

#endif	/*	INCLUDED_VIDEO_H */

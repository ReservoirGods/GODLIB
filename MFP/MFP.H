#ifndef	INCLUDED_MFP_H
#define	INCLUDED_MFP_H


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/CLOCK/CLOCK.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum 
{
	eMFP_TIMER_A,
	eMFP_TIMER_B,
	eMFP_TIMER_C,
	eMFP_TIMER_D,
	
	eMFP_TIMER_LIMIT
};


enum
{
	eMFP_HANDLER_CENTRONICSBUSY,
	eMFP_HANDLER_RS232DCD,
	eMFP_HANDLER_RS232CTS,
	eMFP_HANDLER_BLITTERDONE,
	eMFP_HANDLER_TIMERD,
	eMFP_HANDLER_TIMERC,
	eMFP_HANDLER_IKBD,
	eMFP_HANDLER_FDC,
	eMFP_HANDLER_TIMERB,
	eMFP_HANDLER_SENDERROR,
	eMFP_HANDLER_SENDBUFFEREMPTY,
	eMFP_HANDLER_RECEIVEERROR,
	eMFP_HANDLER_RECEIVEBUFFERFULL,
	eMFP_HANDLER_TIMERA,
	eMFP_HANDLER_RS232RINGDETECT,
	eMFP_HANDLER_MONODETECT,

	eMFP_HANDLER_LIMIT
};

enum
{
	eMFP_DIV_STOP	= 0,
	eMFP_DIV_4		= 1,
	eMFP_DIV_10		= 2,
	eMFP_DIV_16		= 3,
	eMFP_DIV_50		= 4,
	eMFP_DIV_64		= 5,
	eMFP_DIV_100	= 6,
	eMFP_DIV_200	= 7
};

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	void	(*mfTimerFunc)(void);
	U16		mFreq;
	U8		mMode;
	U8		mData;
	U8		mMask;
	U8		mEnable;
} sMfpTimer;


typedef struct sMfpRegDump
{
	void	(*mfHandler[ eMFP_HANDLER_LIMIT ])( void );
	U8		mRegisters[ 0x30 ];
} sMfpRegDump;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Mfp_Init( void );
void			Mfp_DeInit( void );

void			Mfp_InstallTimerA( sMfpTimer * apTimer );
void			Mfp_InstallTimerB( sMfpTimer * apTimer );
void			Mfp_InstallTimerC( sMfpTimer * apTimer );
void			Mfp_InstallTimerD( sMfpTimer * apTimer );

void			Mfp_GetTimerA( sMfpTimer * apTimer );
void			Mfp_GetTimerB( sMfpTimer * apTimer );
void			Mfp_GetTimerC( sMfpTimer * apTimer );
void			Mfp_GetTimerD( sMfpTimer * apTimer );

void			Mfp_InstallGPI7( void (*apHandler)(void), U8 aPinLowHighFlag );

void			Mfp_HookIntoTimerC( sMfpTimer * apTimer );
void			Mfp_HookDisableTimerC( void );
void			Mfp_HookEnableTimerC( void );
void			Mfp_DisableSystemTimerC( void );

sTime *			Mfp_GetpTime( void );
void			Mfp_SetTime( sTime * apTime );
sMfpRegDump *	Mfp_GetpSavedRegisters( void );

U32				Mfp_GetCounter200hz( void );


/* ################################################################################ */

#endif	/* INCLUDED_MFP_H */

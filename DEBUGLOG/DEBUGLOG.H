#ifndef	INCLUDED_DEBUGLOG_H
#define	INCLUDED_DEBUGLOG_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<STRING.H>
#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  VARIABLE PROTOTYPES
################################################################################### */

extern	char gDebugLogString[ 1024 ];


/* ###################################################################################
#  MACROS
################################################################################### */

enum
{
	eDebugLog_File   = (1<<0),
	eDebugLog_Screen = (1<<1),
	eDebugLog_Debugger  = (1<<2),
};

#ifdef	dDEBUGLOG

void	DebugLog_Init( U32 aTargets, const char * apFileName );
void	DebugLog_DeInit( void );
void	DebugLog_AddString( const char * apString );

#define	DebugLog_Printf0(S)				DebugLog_AddString( S )
#define	DebugLog_Printf1(S,A)			sprintf( gDebugLogString, S, A ); DebugLog_AddString( gDebugLogString )
#define	DebugLog_Printf2(S,A,B)			sprintf( gDebugLogString, S, A, B ); DebugLog_AddString( gDebugLogString )
#define	DebugLog_Printf3(S,A,B,C)		sprintf( gDebugLogString, S, A, B, C ); DebugLog_AddString( gDebugLogString )
#define	DebugLog_Printf4(S,A,B,C,D)		sprintf( gDebugLogString, S, A, B, C, D ); DebugLog_AddString( gDebugLogString )
#define	DebugLog_Printf5(S,A,B,C,D,E)	sprintf( gDebugLogString, S, A, B, C, D, E ); DebugLog_AddString( gDebugLogString )

#else

#define	DebugLog_Init( a,b );
#define	DebugLog_DeInit();
#define	DebugLog_AddString( a );

#define	DebugLog_Printf0(S)
#define	DebugLog_Printf1(S,A)
#define	DebugLog_Printf2(S,A,B)
#define	DebugLog_Printf3(S,A,B,C)
#define	DebugLog_Printf4(S,A,B,C,D)
#define	DebugLog_Printf5(S,A,B,C,D,E)

#endif


/* ################################################################################ */

#endif	/*	INCLUDED_DEBUGLOG_H	*/

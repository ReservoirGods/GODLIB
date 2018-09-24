/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: DEBUGLOG.C
::
:: Functions for saving out debug text
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"DEBUGLOG.H"

#ifdef	dDEBUGLOG

#include	<STRING.H>
#include	<GODLIB/FILE/FILE.H>

#ifdef	dGODLIB_PLATFORM_WIN
#include	<d3dx9.h>
#include	<mmsystem.h>
#endif


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dDEBUGLOG_TO_FILE	0
#define	dDEBUGLOG_TO_STEEM	1
#define	dDEBUGLOG_TO_SCREEN	0


/* ###################################################################################
#  VARIABLES
################################################################################### */

char 			gDebugLogString[ 1024 ];
sFileHandle	gDebugLogFileHandle = 0;


/* ###################################################################################
#  CODE
################################################################################### */

#if		(dDEBUGLOG_TO_FILE)


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_Init( char * apFileName )
* ACTION   : creates a file for debug logging
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_Init( char * apFileName )
{
	gDebugLogFileHandle = File_Create( apFileName );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_DeInit()
* ACTION   : closes debug log file
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_DeInit()
{
	File_Close( gDebugLogFileHandle );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_AddString( const char * apString )
* ACTION   : adds a string to the debug log file
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_AddString( const char * apString )
{
	U32	lLen;

	if( !apString )
	{
		return;
	}

	lLen = strlen( apString );

	if( lLen )
	{
		if( gDebugLogFileHandle )
		{
			File_Write( gDebugLogFileHandle, lLen, apString );
		}
	}
}


#elif	(dDEBUGLOG_TO_STEEM)


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_Init( char * apFileName )
* ACTION   : inits STEEM debugging
* CREATION : 05.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_Init( char * apFileName )
{
	if( apFileName )
	{
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_DeInit()
* ACTION   : deinits STEEM debugging
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_DeInit()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_AddString( const char * apString )
* ACTION   : prints string to steem browser
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_AddString( const char * apString )
{
	(void)apString;
#ifdef	dGODLIB_PLATFORM_ATARI
	*(U32*)0xFFFFC1F0L = (U32)apString;
#else
	printf( apString );
#endif
}


#elif	(dDEBUGLOG_TO_SCREEN)


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_Init( char * apFileName )
* ACTION   : inits STEEM debugging
* CREATION : 05.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_Init( char * apFileName )
{
	if( apFileName )
	{
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_DeInit()
* ACTION   : deinits STEEM debugging
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_DeInit()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_AddString( char * apString )
* ACTION   : prints string to steem browser
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_AddString( const char * apString )
{
#ifdef	dGODLIB_PLATFORM_WIN
	OutputDebugString( apString );
	OutputDebugString( "\n" );
#endif
	printf( apString );
	printf( "\n" );
}


#else


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_Init( char * apFileName )
* ACTION   : does nothing
* CREATION : 05.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_Init( char * apFileName )
{
	(void)apFileName;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_DeInit( void )
* ACTION   : does nothing
* CREATION : 05.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_DeInit()
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_AddString( char * apString )
* ACTION   : does nothing
* CREATION : 05.01.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_AddString( const char * apString )
{
	(void)apString;
}


/* ################################################################################ */

#endif

#endif

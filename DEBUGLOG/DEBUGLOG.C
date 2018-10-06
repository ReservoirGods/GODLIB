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


/* ###################################################################################
#  VARIABLES
################################################################################### */

char 			gDebugLogString[ 1024 ];
U32			gDebugLogTargets = 0;
sFileHandle	gDebugLogFileHandle = 0;


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_Init( char * apFileName )
* ACTION   : creates a file for debug logging
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_Init( U32 aTargets, const char * apFileName )
{
	gDebugLogTargets = aTargets;
	if( aTargets & eDebugLog_File )
	{
		gDebugLogFileHandle = File_Create( apFileName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : DebugLog_DeInit()
* ACTION   : closes debug log file
* CREATION : 06.00.99 PNK
*-----------------------------------------------------------------------------------*/

void	DebugLog_DeInit()
{
	if( gDebugLogTargets & eDebugLog_File )
	{
		File_Close( gDebugLogFileHandle );
	}
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
		return;

	for( lLen = 0; apString[ lLen ]; lLen++ );

	if( lLen )
	{
		if( (gDebugLogTargets & eDebugLog_File) && gDebugLogFileHandle )
		{
			File_Write( gDebugLogFileHandle, lLen, apString );
		}
		if( gDebugLogTargets & eDebugLog_Screen )
		{
			printf( apString );
		}
		if( gDebugLogTargets & eDebugLog_Debugger )
		{
#ifdef	dGODLIB_PLATFORM_ATARI
			*(U32*)0xFFFFC1F0L = (U32)apString;
#elif defined(dGODLIB_PLATFORM_WIN)
			OutputDebugString( apString );
			OutputDebugString( "\n" );
#endif
		}
	}
}



/* ################################################################################ */

#endif

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

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/FILE/FILE.H>

#ifdef	dGODLIB_PLATFORM_WIN
#include	<d3dx9.h>
#include	<mmsystem.h>
#endif


/* ###################################################################################
#  DEFINES
################################################################################### */

U16		DebugLog_IsSTEEM( void );

/* ###################################################################################
#  VARIABLES
################################################################################### */

char 			gDebugLogString[ 1024 ];
U32			gDebugLogTargets = 0;
U16			gDebugLogCanWriteSTEEM = 0;
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
	GODLIB_ASSERT( 0 == gDebugLogFileHandle );

	gDebugLogTargets = aTargets;
	gDebugLogCanWriteSTEEM = DebugLog_IsSTEEM();
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
			printf( "%s", apString );
		}
		if( gDebugLogTargets & eDebugLog_Debugger )
		{
#ifdef	dGODLIB_PLATFORM_ATARI
			if( gDebugLogCanWriteSTEEM )
				*(U32*)0xFFFFC1F0L = (U32)apString;
#elif defined(dGODLIB_PLATFORM_WIN)
			OutputDebugString( apString );
			OutputDebugString( "\n" );
#endif
		}
	}
}

#ifndef dGODLIB_PLATFORM_ATARI

U16		DebugLog_IsSTEEM( void )
{
	return( 0 );
}

#else

U16 	gpSteemCheck[] =
{
	0x48E7,0x7FFE,			/* movem.l d1-a6,-(a7) */
	0x2C3C,0x456D,0x753F,	/* move.l #'Emu?',d6*/
	0x2E06,					/* move.l d6,d7*/
	0x3F3C,0x0025,			/* move.l #$25,-(a7)*/
	0x4E4E,					/* trap 14 */
	0x548F,					/* addq.l #2,a7 */
	0x7000,					/* moveq #0,d0 */
	0xBCBC,0x5354,0x4565,	/* cmp.l #'STEe',d6 */
	0x660A,					/* bne.s .not*/
	0xBEBC,0x6D45,0x6E67,	/* cmp.l #'mEng',d6 */
	0x6602,					/* bne.s .not */
	0x7001,					/* moveq #1,d0 */
	0x4CDF,0x7FFE,			/* move.l (a7)+,d1-a6 */
	0x4e75					/* RTS */
};

typedef U16	(*fIsSteem )( void );

U16		DebugLog_IsSTEEM( void )
{
	/* C doesn't like us converting data pointers to function pointers, so let's hack around this */
	fIsSteem lFunc;
	U16 ** lppF = (U16**)&lFunc;
	*lppF = gpSteemCheck;

	return( lFunc() );
}
#endif // !dGODLIB_ATARI


/* ################################################################################ */

#endif

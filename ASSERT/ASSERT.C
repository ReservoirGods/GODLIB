/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: ASSERT.C
::
:: Assertion functions
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ASSERT.H"
#include	<GODLIB/KERNEL/KERNEL.H>
#ifdef	dGODLIB_PLATFORM_WIN
#include	"Windows.h"
#endif

/* ###################################################################################
#  DATA
################################################################################### */

#ifdef	dAssertTXT

char gAssertString[ 1024 ];

#ifdef	dGODLIB_PLATFORM_WIN
void	Assert_Display( const char * apString )
{
/*	MessageBox( NULL, apString, "ASSERT", MB_ABORTRETRYIGNORE ); */
	Kernel_InputBufferSave();
	OutputDebugStr( apString );
	__asm int 3;
}
#endif

char *	Assert_GetpString( void )
{
	return( &gAssertString[ 0 ] );
}


/* ################################################################################ */

#endif

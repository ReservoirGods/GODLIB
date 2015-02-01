/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: VBL.C
::
:: VBL routine handlers
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"VBL.H"

#include	<GODLIB/DEBUG/DEBUG.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>


/* ###################################################################################
#  VARIABLES
################################################################################### */

void	(*gVblOldHandler)();
sVBL	gVbl;
U32		gVblCounter = 0;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	void	(*Vbl_GetHandler( void ) ) ();
extern	void	Vbl_SetHandler( void (*apHandler)() );

extern	void	Vbl_DummyFunc( void );
extern	void	Vbl_Handler( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_Init( void )
* ACTION   : saves old vbl handler / sets machine specific handler
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_Init()
{
	gVblOldHandler   = Vbl_GetHandler();
	gVbl.mLockFlag    = 0;
	gVbl.mCallCount   = 0;
	gVbl.mfTimerBFunc = 0;

	gVbl.mfVideoFunc  = Vbl_DummyFunc;

	Vbl_SetHandler( Vbl_Handler );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_DeInit( void )
* ACTION   : restores old vbl handler
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_DeInit()
{
	Vbl_SetHandler( gVblOldHandler );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_AddCall( void (*apCall)() )
* ACTION   : installs a FUNCTION to be called every vbl
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		Vbl_AddCall( void (*apCall)(void) )
{
	if( gVbl.mCallCount >= dVBL_MAX_CALLS )
	{
		DebugLog_Printf0( "Vbl_AddCall() : error too many vbl queue functions\n" );
		return( 0 );
	}

	gVbl.mfCalls[ gVbl.mCallCount ] = apCall;
	gVbl.mCallCount++;

	return( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_RemoveCall( void (*apCall)() )
* ACTION   : removes a FUNCTION on vbl list
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		Vbl_RemoveCall( void (*apCall)(void) )
{
	U16	i;

	i = 0;
	while( (i < gVbl.mCallCount) && (gVbl.mfCalls[i] != apCall) )
	{
		i++;
	}

	if( i < gVbl.mCallCount )
	{
		gVbl.mCallCount--;
		for( ; i<gVbl.mCallCount; i++ )
		{
			gVbl.mfCalls[ i ] = gVbl.mfCalls[ i + 1 ];
		}
		return( 1 );
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_InstallTimerB( sMfpTimer * apTimer )
* ACTION   : Vbl_InstallTimerB
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_InstallTimerB( sMfpTimer * apTimer )
{
#ifdef	dGODLIB_PLATFORM_ATARI
	*(U8*)0xFFFFFA1BL    = 0;
	Mfp_InstallTimerB( apTimer );
#endif
	gVbl.mTimerBScanLine = apTimer->mData;
	gVbl.mfTimerBFunc    = apTimer->mfTimerFunc;

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_InstallHbi( sMfpTimer * apTimer )
* ACTION   : Vbl_InstallHbi
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_InstallHbi( sMfpTimer * apTimer )
{
	gVbl.mHbiCounterStart = apTimer->mData;
	gVbl.mfHbi            = apTimer->mfTimerFunc;
	System_SetIML( 1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_GetpVbl( void )
* ACTION   : Vbl_GetpVbl
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

sVBL *	Vbl_GetpVbl( void )
{
	return( &gVbl );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_WaitVbls( const U16 aCount )
* ACTION   : Vbl_WaitVbls
* CREATION : 03.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_WaitVbls( const U16 aCount )
{
	U16	i;

	for( i=0; i<aCount; i++ )
	{
		Vbl_WaitVbl();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_SetVideoFunc( void (* apFunc)() )
* ACTION   : Vbl_SetVideoFunc
* CREATION : 02.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_SetVideoFunc( void (* apFunc)() )
{
	gVbl.mfVideoFunc = apFunc;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_CallsProcess( void )
* ACTION   : Vbl_CallsProcess
* CREATION : 15.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_CallsProcess( void )
{
	U16	i;

	for( i=0; i<gVbl.mCallCount; i++ )
	{
		gVbl.mfCalls[ i ]();
	}
}

#ifndef	dGODLIB_PLATFORM_ATARI

/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_WaitVbl( )
* ACTION   : Vbl_WaitVbl
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_WaitVbl( )
{
	Vbl_Handler();
	gVblCounter++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_GetCounter( void )
* ACTION   : Vbl_GetCounter
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U32	Vbl_GetCounter( void )
{
	return( gVblCounter );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Vbl_Handler( void )
* ACTION   : Vbl_Handler
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Vbl_Handler( void )
{
	U16	i;

	if( gVbl.mfVideoFunc )
	{
		gVbl.mfVideoFunc();
	}
	if( gVbl.mfTimerBFunc )
	{
		gVbl.mfTimerBFunc();
	}
	for( i=0; i<gVbl.mCallCount; i++ )
	{
		gVbl.mfCalls[ i ]();
	}
}

#endif

/* ################################################################################ */

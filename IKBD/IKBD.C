/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: IKBD.C
::
:: Functions for controlling the IKBD chip - keyboard, joystick, mouse + midi
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"IKBD.H"
#include	"IKBD_DI.H"
#include	"IKBD_SDL.H"

#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/XBIOS/XBIOS.H>


/* ###################################################################################
#  VARIABLES
################################################################################### */

sIKBD	gIKBD;
void 	(*gIKBDOldHandler)(void);


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

extern	void 	(*IKBD_GetHandler( void ))(void);
extern	void	IKBD_SetHandler( void (*apHandler)(void) );

extern	void	IKBD_SendCmd( U8 aCmd );

extern	void	IKBD_MainHandler( void );
extern	void	IKBD_PowerpadHandler( void );

extern	void	IKBD_InitTosLink( U32 apBuffer, U32 aFunction );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_Init()
* ACTION   : gets keyboard tables, saves old kbd handler, 
*			 installs new kbd/midi handler
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_Init()
{
	sXbiosKeyTab *	lpKeyTab;
	U16				i;

	Memory_Clear( sizeof(sIKBD), &gIKBD );
	lpKeyTab = Xbios_Keytbl( (const char *)0xFFFFFFFFL, (const char *)0xFFFFFFFFL, (const char *)0xFFFFFFFFL );
#ifdef	dGODLIB_PLATFORM_ATARI
	gIKBD.mpUnshiftTable = (U8 *)lpKeyTab->mpNormal;
	gIKBD.mpShiftTable   = (U8 *)lpKeyTab->mpShift;
	gIKBD.mpCapsTable    = (U8 *)lpKeyTab->mpCaps;
#endif
	gIKBD.mTeamTapActiveBits = 0;

	gIKBDOldHandler = IKBD_GetHandler();
	IKBD_FlushGemdos();
	IKBD_SetHandler( IKBD_MainHandler );
	IKBD_Flush();

	IKBD_ClearBuffer();

	gIKBD.mPad0Dir = 0;
	gIKBD.mPad0Key = 0;
	gIKBD.mPad1Dir = 0;
	gIKBD.mPad1Key = 0;
	for( i=0; i<8; i++ )
	{
		gIKBD.mTeamTapDirs[ i ] = 0;
		gIKBD.mTeamTapKeys[ i ] = 0;
	}

	switch( System_GetMCH() )
	{
	case	MCH_FALCON:
	case	MCH_STE:
		Vbl_AddCall( IKBD_PowerpadHandler );
		gIKBD.mPadEnabledFlag = 1;
		break;
	default:
		gIKBD.mPadEnabledFlag = 0;
		break;
	}

	gIKBD.mMouseEnabledFlag = 1;
	gIKBD.mJoystickEnabledFlag = 0;

#if defined (dGODLIB_SYSTEM_D3D)
	IKBD_DI_Init();
#elif defined (dGODLIB_SYSTEM_SDL)
	IKBD_SDL_Init();
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DeInit()
* ACTION   : restores old kbd/midi handler
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DeInit()
{
	switch( System_GetMCH() )
	{
	case	MCH_FALCON:
	case	MCH_STE:
		Vbl_RemoveCall( IKBD_PowerpadHandler );
		break;
	default:
		break;
	}

	IKBD_Flush();
	IKBD_SetHandler( gIKBDOldHandler );
	IKBD_FlushGemdos();

#if defined (dGODLIB_SYSTEM_D3D)
	IKBD_DI_DeInit();
#elif defined (dGODLIB_SYSTEM_SDL)
	IKBD_SDL_DeInit();
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_Update( void )
* ACTION   : IKBD_Update
* CREATION : 06.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_Update( void )
{
#if defined (dGODLIB_SYSTEM_D3D)
	IKBD_DI_Update();	
#elif defined (dGODLIB_SYSTEM_SDL)
	IKBD_SDL_Update();
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetKbdOverflowFlag()
* ACTION   : returns a flag indicating keyboard buffer has overflowed
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetKbdOverflowFlag()
{
	return( gIKBD.mKbdOverflowFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMidiOverflowFlag()
* ACTION   : returns a flag indicating midi buffer has overflowed
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetMidiOverflowFlag()
{
	return( gIKBD.mMidiOverflowFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetPad0Dir()
* ACTION   : gets direction packet for jagpad0
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetPad0Dir()
{
	return( gIKBD.mPad0Dir );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetPad0Key()
* ACTION   : gets key data packet for jagpad0
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U16		IKBD_GetPad0Key()
{
	return( gIKBD.mPad0Key );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetPad1Dir()
* ACTION   : gets direction packet for jagpad1
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetPad1Dir()
{
	return( gIKBD.mPad1Dir );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetPad1Key()
* ACTION   : gets key data packet for jagpad1
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U16		IKBD_GetPad1Key()
{
	return( gIKBD.mPad1Key );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetKeyStatus( U8 aScanCode )
* ACTION   : returns keypress value for aScanCode
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

U8			IKBD_GetKeyStatus( U8 aScanCode )
{
	return( gIKBD.mKbdPressTable[ aScanCode ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMouseX()
* ACTION   : returns mouse x position
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

S16		IKBD_GetMouseX()
{
	return( gIKBD.mMouseX );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SetMouseX( S16 aX )
* ACTION   : sets mouse x position
* CREATION : 14.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SetMouseX( S16 aX )
{
	gIKBD.mMouseX  = aX;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMouseY()
* ACTION   : returns mouse y position
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

S16		IKBD_GetMouseY()
{
	return( gIKBD.mMouseY );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SetMouseY( S16 aY )
* ACTION   : sets mouse y position
* CREATION : 14.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SetMouseY( S16 aY )
{
	gIKBD.mMouseY  = aY;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_SetMousePosition( S16 aX, S16 aY )
* ACTION   : sets mouse position to (aX,aY)
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_SetMousePosition( S16 aX, S16 aY )
{
	gIKBD.mMouseX = aX;
	gIKBD.mMouseY = aY;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMouseKeys()
* ACTION   : returns mouse key state
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetMouseKeys()
{
	return( gIKBD.mMouseKeys );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMouseButtonLeft( void )
* ACTION   : IKBD_GetMouseButtonLeft
* CREATION : 02.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_GetMouseButtonLeft( void )
{
	return( (U8)((gIKBD.mMouseKeys >> 1) & 1) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetMouseButtonRight( void )
* ACTION   : IKBD_GetMouseButtonRight
* CREATION : 02.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_GetMouseButtonRight( void )
{
	return( (U8)(gIKBD.mMouseKeys & 1) );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 IKBD_GetLastKeyPress(void)
* ACTION   : IKBD_GetLastKeyPress
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 IKBD_GetLastKeyPress(void)
{
	return( gIKBD.mLastKeypress );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void IKBD_ClearLastKeyPress(void)
* ACTION   : IKBD_ClearLastKeyPress
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void IKBD_ClearLastKeyPress(void)
{
	gIKBD.mLastKeypress = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 IKBD_GetKeyPressedFlag(void)
* ACTION   : IKBD_GetKeyPressedFlag
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 IKBD_GetKeyPressedFlag(void)
{
	return( gIKBD.mKeyPressedFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void IKBD_ClearKeyPressedFlag(void)
* ACTION   : IKBD_ClearKeyPressedFlag
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void IKBD_ClearKeyPressedFlag(void)
{
	gIKBD.mKeyPressedFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 IKBD_GetJoy0Packet(void)
* ACTION   : IKBD_GetJoy0Packet
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 IKBD_GetJoy0Packet(void)
{
	return( gIKBD.mJoy0Packet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U8 IKBD_GetJoy1Packet(void)
* ACTION   : IKBD_GetJoy1Packet
* CREATION : 30.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U8 IKBD_GetJoy1Packet(void)
{
	return( gIKBD.mJoy1Packet );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_EnableDebugging()
* ACTION   : patches my IKBD handler to jump into tos
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_EnableDebugging()
{
	switch( System_GetTosVersion() )
	{
	case	0x104:
		IKBD_InitTosLink( 0xDA0L, 0xFC3C70L );
		break;

	case	0x206:
		IKBD_InitTosLink( 0xED8L, 0xE03044L );
		break;

	case	0x404:
		IKBD_InitTosLink( 0xF96L, 0xE0340AL );
		break;


	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_EnableJoysticks()
* ACTION   : turns on joysticks, turns off mouse
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_EnableJoysticks()
{
	IKBD_SendCmd( 0x12 );
	IKBD_SendCmd( 0x14 );
	gIKBD.mMouseEnabledFlag = 0;
	gIKBD.mJoystickEnabledFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_EnableMouse()
* ACTION   : turns on mouse, turns off joystick
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_EnableMouse()
{
	IKBD_SendCmd( 0x8 );
	gIKBD.mMouseEnabledFlag = 1;
	gIKBD.mJoystickEnabledFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_ConvertScancodeAscii( U8 aScan )
* ACTION   : returns the ascii value of key with scancode aScan
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_ConvertScancodeAscii( U8 aScan )
{
	U8 *	lpKeyTable;

	if( gIKBD.mKbdPressTable[0x2A] || gIKBD.mKbdPressTable[0x36] )
	{
		lpKeyTable = gIKBD.mpShiftTable;
	}
	else
	{
		lpKeyTable = gIKBD.mpUnshiftTable;
	}

	return( lpKeyTable[ aScan ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_ConvertScancodeAsciiCaps( U8 aScan )
* ACTION   : returns the capitalised ascii value of key with scancode aScan
* CREATION : 25.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_ConvertScancodeAsciiCaps( U8 aScan )
{
	U8 *	lpKeyTable;

	if( gIKBD.mKbdPressTable[0x2A] || gIKBD.mKbdPressTable[0x36] )
	{
		lpKeyTable = gIKBD.mpShiftTable;
	}
	else
	{
		lpKeyTable = gIKBD.mpCapsTable;
	}

	return( lpKeyTable[ aScan ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetAsciiNormal( const U8 aScan )
* ACTION   : IKBD_GetAsciiNormal
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_GetAsciiNormal( const U8 aScan )
{
	return( gIKBD.mpUnshiftTable[ aScan ] );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetAsciiCaps( const U8 aScan )
* ACTION   : IKBD_GetAsciiCaps
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_GetAsciiCaps( const U8 aScan )
{
	return( gIKBD.mpCapsTable[ aScan ] );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetAsciiShift( const U8 aScan )
* ACTION   : IKBD_GetAsciiShift
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_GetAsciiShift( const U8 aScan )
{
	return( gIKBD.mpShiftTable[ aScan ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_WaitForKey( const U8 aKey )
* ACTION   : blocks until aKey is pressed
* CREATION : 04.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_WaitForKey( const U8 aKey )
{
	do
	{
		while( !IKBD_GetKbdBytesWaiting() );
	} while( IKBD_PopKbdByte() != aKey );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_WaitForKey( const U8 aKey )
* ACTION   : blocks until aKey is pressed
* CREATION : 04.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_WaitAnyKey()
{
	while( !IKBD_GetKbdBytesWaiting() );
	IKBD_PopKbdByte();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_ClearBuffer( void )
* ACTION   : 
* CREATION : 04.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_ClearBuffer( void )
{
	U16	i;

	IKBD_Flush();
	for( i=0; i<128; i++)
	{
		gIKBD.mKbdPressTable[ i ] = 0;
	}
	IKBD_Flush();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_EnableTeamTap0( void )
* ACTION   : enables team tap 0
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_EnableTeamTap0( void )
{
	gIKBD.mTeamTapActiveBits |= 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_EnableTeamTap1( void )
* ACTION   : enables team tap 1
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_EnableTeamTap1( void )
{
	gIKBD.mTeamTapActiveBits |= 2;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DisableTeamTap0( void )
* ACTION   : disables teamp tap 0
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DisableTeamTap0( void )
{
	gIKBD.mTeamTapActiveBits &= ~1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_DisableTeamTap1( void )
* ACTION   : disables team tap 1
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

void	IKBD_DisableTeamTap1( void )
{
	gIKBD.mTeamTapActiveBits &= ~2;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetTeamTapPadDir( const U8 aTap, const U8 aPad )
* ACTION   : gets direction keys status for pad aPad in team tap aTap
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

U8		IKBD_GetTeamTapPadDir( const U8 aTap, const U8 aPad )
{
	return( gIKBD.mTeamTapDirs[ (aTap<<2)+aPad ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetTeamTapPadKey( const U8 aTap, const U8 aPad )
* ACTION   : gets keys status for pad aPad in team tap aTap
* CREATION : 06.01.01 PNK
*-----------------------------------------------------------------------------------*/

U16		IKBD_GetTeamTapPadKey( const U8 aTap, const U8 aPad )
{
	return( gIKBD.mTeamTapKeys[ (aTap<<2)+aPad ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsPad0Enabled( void )
* ACTION   : IKBD_IsPad0Enabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsPad0Enabled( void )
{
	U8	lRes;

	lRes = 0;
	if( gIKBD.mPadEnabledFlag )
	{
		if( !IKBD_IsTeamTap0Enabled() )
		{
			lRes = 1;
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsPad1Enabled( void )
* ACTION   : IKBD_IsPad1Enabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsPad1Enabled( void )
{
	U8	lRes;

	lRes = 0;
	if( gIKBD.mPadEnabledFlag )
	{
		if( !IKBD_IsTeamTap1Enabled() )
		{
			lRes = 1;
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsMouseEnabled( void )
* ACTION   : IKBD_IsMouseEnabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsMouseEnabled( void )
{
	return( gIKBD.mMouseEnabledFlag );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsJoystickEnabled( void )
* ACTION   : IKBD_IsJoystickEnabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsJoystickEnabled( void )
{
	return( gIKBD.mJoystickEnabledFlag );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsTeamTap0Enabled( void )
* ACTION   : IKBD_IsTeamTap0Enabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsTeamTap0Enabled( void )
{
	return( (U8)(gIKBD.mTeamTapActiveBits & 1) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_IsTeamTap1Enabled( void )
* ACTION   : IKBD_IsTeamTap1Enabled
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_IsTeamTap1Enabled( void )
{
	return( (U8)(gIKBD.mTeamTapActiveBits & 2) );
}


#ifndef	dGODLIB_PLATFORM_ATARI

/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_GetKbdBytesWaiting( void )
* ACTION   : IKBD_GetKbdBytesWaiting
* CREATION : 06.08.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	IKBD_GetKbdBytesWaiting( void )
{
	S16	lRes;
	
	lRes = (S16)(gIKBD.mKbdTail - gIKBD.mKbdHead);
	if( lRes < 0 )
	{
		lRes += 2048;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : IKBD_PopKbdByte( void )
* ACTION   : IKBD_PopKbdByte
* CREATION : 06.08.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	IKBD_PopKbdByte( void )
{
	U8	lRes;
	
	lRes = gIKBD.mKbdBuffer[ gIKBD.mKbdHead ];
	gIKBD.mKbdHead++;
	gIKBD.mKbdHead &= 2047;

	return( lRes );
}


#endif

/* ################################################################################ */

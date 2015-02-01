/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GOD_MAIN.H"

#ifdef	dGODLIB_DEBUGCHANNELS
#include	<GODLIB/DEBUG/DBGCHAN.H>
#endif
#include	<GODLIB/IKBD/IKBD_DI.H>
#include	<GODLIB/KERNEL/KERNEL.H>
#include	<GODLIB/VIDEO/VID_D3D.H>

#ifdef	dGODLIB_SYSTEM_D3D
#include	<d3dx9.h>
#endif
#ifdef	dGODLIB_PLATFORM_WIN
#include	<windows.h>
#endif


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	GodLib_DebugChannels_Init( void );
void	GodLib_DebugChannels_DeInit( void );


/* ###################################################################################
#  CODE
################################################################################### */

#ifdef	dGODLIB_SYSTEM_D3D

HWND	gGodLibMainWindowHandle;

/*-----------------------------------------------------------------------------------*
* FUNCTION : MsgProc( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam )
* ACTION   : MsgProc
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

LRESULT WINAPI	MsgProc( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		Kernel_RequestShutdown();
		return 0;
	case WM_SYSKEYDOWN:
		if( ((lParam>>16)&0xFF) == 0x44 )
		{
			return 0;
		}
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


S16	GodLib_Library_Main( S16 aArgc, char ** apArgv )
{
	S16	i;
	char lCmd;
	GodLib_DebugChannels_Init();

	for( i=1; i<aArgc; i++ )
	{
		char * lpArg = apArgv[ i ];
		while( (*lpArg>0) && (*lpArg <= ' ') )
		{
			lpArg++;
		}
		if( *lpArg == '-' )
		{
			lpArg++;
			lCmd = *lpArg++;

			while( (*lpArg>0) && (*lpArg != '=') )
			{
				lpArg++;
			}
			if( '=' == *lpArg )
			{
				lpArg++;
			}

			switch( lCmd )
			{
			case 'i':
				Kernel_RequestInputLoad( lpArg );
				break;
			}
		}
		else
		{
			/* default unspecified command argument is input */
			while( (*lpArg>0) && (*lpArg <= ' ') )
			{
				lpArg++;
			}
			Kernel_RequestInputLoad( lpArg );
		}
	}
	GodLib_Game_Main( (S16)__argc, __argv);
	GodLib_DebugChannels_DeInit();
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : WinMain( HINSTANCE hInst,HINSTANCE a0,LPSTR a1,INT a2 )
* ACTION   : WinMain
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

INT WINAPI	WinMain( HINSTANCE hInst,HINSTANCE a0,LPSTR a1,INT a2 )
{
	WNDCLASSEX	lWC;

	(void)a0;
	(void)a1;
	(void)a2;
	(void)hInst;
	lWC.cbSize        = sizeof( WNDCLASSEX );
	lWC.style         = CS_CLASSDC;
	lWC.lpfnWndProc   = MsgProc;
	lWC.cbClsExtra    = 0;
	lWC.cbWndExtra    = 0;
	lWC.hInstance     = GetModuleHandle(NULL);
	lWC.hIcon         = NULL;
	lWC.hCursor       = NULL;
	lWC.hbrBackground = NULL;
	lWC.lpszMenuName  = NULL;
	lWC.lpszClassName = "GodLib";
	lWC.hIconSm       = NULL;

	if( RegisterClassEx( &lWC ) )
	{
		gGodLibMainWindowHandle = CreateWindow( "GodLib", "GodLib: Game",
								  WS_OVERLAPPEDWINDOW, 100, 100, 340, 280,
								  GetDesktopWindow(), NULL, lWC.hInstance, NULL );
		VideoD3D_SetWindowHandle( gGodLibMainWindowHandle );
		IKBD_DI_SetWindowHandle( gGodLibMainWindowHandle );
		File_SetWindowHandle( gGodLibMainWindowHandle );

		GodLib_Library_Main( (S16)__argc, __argv );
/*
		GodLib_DebugChannels_Init();
		GodLib_Game_Main( (S16)__argc, __argv);
		GodLib_DebugChannels_DeInit();
		*/
	}

	return( 0 );
}


#elif defined (dGODLIB_SYSTEM_SDL)


/*-----------------------------------------------------------------------------------*
* FUNCTION : SDL_main( int argc,char * argv[] )
* ACTION   : SDL_main
* CREATION : 14.04.2005 PNK
*-----------------------------------------------------------------------------------*/

int	SDL_main( int argc,char * argv[] )
{
	GodLib_DebugChannels_Init();
	GodLib_Game_Main( (S16)argc, argv );
	GodLib_DebugChannels_DeInit();
	return( 0 );
}


#else

/*-----------------------------------------------------------------------------------*
* FUNCTION : main( void )
* ACTION   : it begins  ...
* CREATION : 21.11.01 PNK
*-----------------------------------------------------------------------------------*/

S16	main( S16 argc,char * argv[] )
{
	GodLib_DebugChannels_Init();
	GodLib_Game_Main( argc, argv );
	GodLib_DebugChannels_DeInit();
	return( 0 );
}

#endif


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodLib_DebugChannels_Init( void )
* ACTION   : GodLib_DebugChannels_Init
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	GodLib_DebugChannels_Init( void )
{
#ifdef dGODLIB_DEBUGCHANNELS
	DebugChannel_AppInit();
	DebugChannel_Activate( eDEBUGCHANNEL_ASSET,  (U16)(~eDEBUGCHANNEL_DEST_SCREEN) );
	DebugChannel_Activate( eDEBUGCHANNEL_GAME,   (U16)(~eDEBUGCHANNEL_DEST_SCREEN) );
	DebugChannel_Activate( eDEBUGCHANNEL_GODLIB, (U16)(~eDEBUGCHANNEL_DEST_SCREEN) );
	DebugChannel_Activate( eDEBUGCHANNEL_MEMORY, (U16)(~eDEBUGCHANNEL_DEST_SCREEN) );
	DebugChannel_Activate( eDEBUGCHANNEL_USER,   (U16)(~eDEBUGCHANNEL_DEST_SCREEN) );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GodLib_DebugChannels_DeInit( void )
* ACTION   : GodLib_DebugChannels_DeInit
* CREATION : 23.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	GodLib_DebugChannels_DeInit( void )
{
#ifdef dGODLIB_DEBUGCHANNELS
	DebugChannel_AppDeInit();
#endif
}


/* ################################################################################ */

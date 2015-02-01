/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"PLATFORM.H"

#include	<GODLIB/ASSET/PACKAGE.H>
#include	<GODLIB/ASSET/RELOCATE.H>
#include	<GODLIB/AUDIO/AUDIO.H>
#include	<GODLIB/AUDIO/AMIXER.H>
#include	<GODLIB/AUDIO/REL_SPL.H>
#include	<GODLIB/CLOCK/CLOCK.H>
#include	<GODLIB/CUTSCENE/CUT_SYS.H>
#include	<GODLIB/CUTSCENE/REL_CUT.H>
#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/EXCEPT/EXCEPT.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FE/REL_FED.H>
#include	<GODLIB/FILE/FILE.H>
#include	<GODLIB/FONT/REL_BFB.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/PROFILER/PROFILER.H>
#include	<GODLIB/RANDOM/RANDOM.H>
#include	<GODLIB/SCRNGRAB/SCRNGRAB.H>
#include	<GODLIB/SPRITE/REL_ASB.H>
#include	<GODLIB/SPRITE/REL_BSB.H>
#include	<GODLIB/SPRITE/REL_RSB.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/VIDEO/VIDEO.H>

#ifdef	dGODLIB_SYSTEM_SDL
#pragma pack(push,4)
#include	"SDL.h"
#pragma pack(pop)
#endif

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Platform_Init( void )
* ACTION   : calls all initialisation routines
* CREATION : 21.11.01 PNK
*-----------------------------------------------------------------------------------*/

void	Platform_Init( void )
{
/*	DebugLog_Init( "PLAT.LOG" );*/
	DebugChannel_Printf0( eDEBUGCHANNEL_GODLIB, "Platform_Init()" );

#ifdef	dGODLIB_SYSTEM_SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL GodLib", "GodLib");
#endif

	Platform_Hardware_Init();

#ifdef	dPROFILER
	Profiler_Init( 0, 0 );
#endif

#ifdef	dSCREENGRAB
	ScreenGrab_Init();
	ScreenGrab_SetDirectory( "SCRNGRAB\\" );
	ScreenGrab_Enable();
#endif

	Random_Init();

	CutScene_System_AppInit();

	PackageManager_Init();
	RelocaterManager_Init();
	Relocator_ASB_Init();
	Relocator_BSB_Init();
	Relocator_BFB_Init();
	Relocator_CUT_Init();
	Relocator_FED_Init();
	Relocator_RSB_Init();
	Relocator_SPL_Init();

	PackageManager_SetFilePath( "UNLINK" );
	PackageManager_SetLinkPath( "DATA" );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Platform_DeInit( void )
* ACTION   : calls all de-initialisation routines
* CREATION : 21.11.01 PNK
*-----------------------------------------------------------------------------------*/

void	Platform_DeInit( void )
{
	CutScene_System_AppDeInit();

#ifdef	dSCREENGRAB
	ScreenGrab_Disable();
	ScreenGrab_DeInit();
#endif

#ifdef	dPROFILER
	Profiler_DeInit();
#endif

	Platform_Hardware_DeInit();

	PackageManager_DeInit();
	Relocator_ASB_DeInit();
	Relocator_BSB_DeInit();
	Relocator_BFB_DeInit();
	Relocator_CUT_DeInit();
	Relocator_FED_DeInit();
	Relocator_RSB_DeInit();
	Relocator_SPL_DeInit();
	RelocaterManager_DeInit();

/*	DebugLog_Init( "MEMORY.LOG" );
	Memory_ShowCurrentRecords();
	DebugLog_DeInit();*/
#ifdef	dGODLIB_SYSTEM_SDL
	SDL_Quit();
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Platform_Hardware_Init( void )
* ACTION   : Platform_Hardware_Init
* CREATION : 26.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Platform_Hardware_Init( void )
{
	System_Init();

#ifdef	dEXCEPTION_SCREEN
	Except_Init();
#endif

	Vbl_Init();
	Video_Init();

	IKBD_Init();

	Audio_Init();
#ifdef	dAUDIO_MIXER
	AudioMixer_Init();
	AudioMixer_Enable();
#endif

	System_DataCacheDisable();
	System_InstructionCacheDisable();
	System_SetCPUSpeed( 16 );

	IKBD_EnableJoysticks();

	Graphic_Init();

	Fade_Init();
	Clock_Init();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Platform_Hardware_DeInit( void )
* ACTION   : Platform_Hardware_DeInit
* CREATION : 26.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Platform_Hardware_DeInit( void )
{
	Clock_DeInit();
	Fade_DeInit();
	Graphic_DeInit();
#ifdef	dAUDIO_MIXER
	AudioMixer_DeInit();
#endif
	Audio_DeInit();
	IKBD_EnableMouse();

	IKBD_DeInit();

	Video_DeInit();
	Vbl_DeInit();

#ifdef	dEXCEPTION_SCREEN
	Except_DeInit();
#endif

	System_DeInit();
	Vbl_WaitVbl();
	IKBD_FlushGemdos();

	Audio_SoundChipOff();
}


/* ################################################################################ */

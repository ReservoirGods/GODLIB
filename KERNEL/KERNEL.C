/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"KERNEL.H"

#include	<GODLIB/ACHIEVE/ACH_MAIN.H>
#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/RANDOM/RANDOM.H>
#include	<GODLIB/SCREEN/SCREEN.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/VIDEO/VIDEO.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dKERNEL_MONITOR_LIMIT	200
#define	dKERNEL_MONITOR_CUTOFF	100


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sKernelClass
{
	sGemDosDTA			mDTA;
	U32					mPackageCurrentFlags;
	U32					mPackageGlobalFlags;
	U32					mMonitoredUpdateCount;
	sPackage *			mpPackages;
	sKernelPackageDef *	mpPackageDefs;
	sKernelTask *		mpTasks;
	sKernelTask *		mpTaskCurrent;
	sClock				mClocks[ eKERNEL_CLOCK_LIMIT ];
	sInput				mCombinedInput;
	sInput				mDirInputs[ eINPUTTYPE_LIMIT ];
	sInput				mMonitoredInputs[ eINPUTTYPE_LIMIT ];
	sInput				mFireInputs[ 3 ];
	U16					mDisabledInputs[ eINPUTTYPE_LIMIT ];
	U16					mMonitoredCounts[ eINPUTTYPE_LIMIT ];
	U16					mDirInputCount;
	U16					mFireInputCount;
	U16					mPackageCount;
	U16					mTaskCount;
	U16					mTaskIndex;
	U16					mShutDownFlag;

	U8 *				mpInputBuffer;
	U32					mInputBufferSize;
	U32					mInputBufferOffset;
	sInput				mInputBufferCurrent;
	eKERNEL_INPUT		mInputType;
} sKernelClass;


/* ###################################################################################
#  DATA
################################################################################### */

sKernelClass	gKernelClass;
const char *	gpKernelInputLoadFileName = 0;
U8				gKernelUsesTeamTapsFlag = 0;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Kernel_Clocks_Update( void );
void	Kernel_Input_Update( void );
void	Kernel_PackagesLoad( const U32 aFlags );
void	Kernel_PackagesUnLoad( const U32 aFlags );
sKernelTask *	Kernel_GetpTask( const U16 aIndex );
sPackage *		Kernel_GetpPackage( const U16 aIndex );
void			Kernel_MonitorInputsUpdate( void );
void			Kernel_BuildDisabled( void );

void	Kernel_InputBufferSet( const sInput * apInput );
void	Kernel_InputBufferGet( sInput * apInput );

void	Kernel_CLI_achunlock( const char * apArgs );
void	Kernel_CLI_ass( const char * apArgs );
void	Kernel_CLI_assunused( const char * apArgs );
void	Kernel_CLI_build( const char * apArgs );
void	Kernel_CLI_mem( const char * apArgs );
void	Kernel_CLI_inp( const char * apArgs );
void	Kernel_CLI_quit( const char * apArgs );
void	Kernel_CLI_vid( const char * apArgs );
void	Kernel_CLI_sys( const char * apArgs );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_Init( sKernelTask * apTasks,const U16 aTaskCount,sPackage * apPackages,sKernelPackageDef * apDef,const U16 aPackageLimit )
* ACTION   : Kernel_Init
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_Init( sKernelTask * apTasks,const U16 aTaskCount,sPackage * apPackages,sKernelPackageDef * apDef,const U16 aPackageLimit )
{
	U16	i;

	Memory_Clear( sizeof(sKernelClass), &gKernelClass );

	File_SetDTA( &gKernelClass.mDTA );

	for( i=0; i<eKERNEL_CLOCK_LIMIT; i++ )
	{
		gKernelClass.mClocks[ i ].mState = eCLOCK_STATE_INACTIVE;
	}

	for( i=0; i<eINPUTTYPE_LIMIT; i++ )
	{
		Input_Init( &gKernelClass.mMonitoredInputs[ i ] );
		gKernelClass.mMonitoredInputs[ i ].mInputType = i;
		Input_Update( &gKernelClass.mMonitoredInputs[ i ] );
		Input_Update( &gKernelClass.mMonitoredInputs[ i ] );
		gKernelClass.mDisabledInputs[ i ] = 0;
		gKernelClass.mMonitoredCounts[ i ] = 0;
	}

	gKernelClass.mTaskCount    = aTaskCount;
	gKernelClass.mpTasks       = apTasks;
	gKernelClass.mpPackages    = apPackages;
	gKernelClass.mPackageCount = aPackageLimit;
	gKernelClass.mpPackageDefs = apDef;
	gKernelClass.mMonitoredUpdateCount = 0;
	for( i=0; i<aPackageLimit; i++ )
	{
		Package_Init( &apPackages[ i ], apDef[ i ].mpPackageName, apDef[ i ].mpContextName );
	}

	Cli_Init();
	Cli_CmdInit( "achunlock", Kernel_CLI_achunlock );
	Cli_CmdInit( "ass", Kernel_CLI_ass );
	Cli_CmdInit( "assunused", Kernel_CLI_assunused );
	Cli_CmdInit( "build", Kernel_CLI_build );
	Cli_CmdInit( "inp", Kernel_CLI_inp );
	Cli_CmdInit( "mem", Kernel_CLI_mem );
	Cli_CmdInit( "quit", Kernel_CLI_quit );
	Cli_CmdInit( "sys", Kernel_CLI_sys );
	Cli_CmdInit( "vid", Kernel_CLI_vid );
	Cli_PrintLine( "GodLib CLI" );
	Kernel_CLI_build( 0 );

	gKernelClass.mpTaskCurrent = apTasks;

	Kernel_InputsEnumerate();
	Vbl_AddCall( Kernel_MonitorInputsUpdate );

	gKernelClass.mInputBufferSize = 0;
	gKernelClass.mpInputBuffer = 0;
	gKernelClass.mInputBufferOffset = 0;

	gKernelClass.mInputType = eKERNEL_INPUT_NORMAL;

	if( gpKernelInputLoadFileName )
	{
		Kernel_InputBufferLoad( gpKernelInputLoadFileName );
	}
#ifdef dGODLIB_PLATFORM_WIN
	else
	{
		gKernelClass.mInputType = eKERNEL_INPUT_SAVING;
		gKernelClass.mInputBufferSize = 1024L * 64L;
		gKernelClass.mpInputBuffer = mMEMCALLOC( gKernelClass.mInputBufferSize );
	}
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_DeInit( void )
* ACTION   : Kernel_DeInit
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_DeInit( void )
{
	U16	i;

	for( i=0; i<gKernelClass.mPackageCount; i++ )
	{
		Package_DeInit( &gKernelClass.mpPackages[ i ] );
	}

	Cli_CmdDeInit( "achunlock" );
	Cli_CmdDeInit( "ass" );
	Cli_CmdDeInit( "assunused" );
	Cli_CmdDeInit( "build" );
	Cli_CmdDeInit( "inp" );
	Cli_CmdDeInit( "mem" );
	Cli_CmdDeInit( "quit" );
	Cli_CmdDeInit( "sys" );
	Cli_CmdDeInit( "vid" );

	Cli_DeInit();

	if( gKernelClass.mpInputBuffer )
	{
		mMEMFREE( gKernelClass.mpInputBuffer );
	}
	gKernelClass.mInputBufferOffset = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_Main( void )
* ACTION   : Kernel_Main
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_Main( void )
{
	U32	lPackFlags;
	U32	lMask;
	U16	lIndex;
	U16	lNewTask;
	sKernelTask *	lpTask;

	Screen_Update();
	Cli_FileProcess( "AUTOEXEC.CLI" );
	Cli_PrintLine( "TASK 0 Init()" );
	Kernel_PackagesLoad( gKernelClass.mpTasks[ 0 ].mPackageFlags );
	PackageManager_Update();
	gKernelClass.mpTaskCurrent->mfInit();

	while( !gKernelClass.mShutDownFlag )
	{
		sInput	lInput;
		sInput * lpInput;
		if( gKernelClass.mMonitoredUpdateCount > dKERNEL_MONITOR_LIMIT )
		{
			Vbl_RemoveCall( Kernel_MonitorInputsUpdate );
			Kernel_BuildDisabled();
			Kernel_InputsEnumerate();
			gKernelClass.mMonitoredUpdateCount = 0;
		}
		Screen_Update();
		IKBD_Update();
		Kernel_Clocks_Update();
		Random_Update();
		Cli_Update();
		Kernel_Input_Update();

		lpTask   = 0;

		if( eKERNEL_INPUT_LOADING == gKernelClass.mInputType )
		{
			lpInput = &lInput;
			Kernel_InputBufferGet( lpInput );
		}
		else
		{
			lpInput = &gKernelClass.mCombinedInput;
			if( eKERNEL_INPUT_SAVING == gKernelClass.mInputType )
			{
				Kernel_InputBufferSet( lpInput );
			}
		}

		lNewTask = gKernelClass.mpTaskCurrent->mfUpdate( lpInput );
		if( lNewTask != gKernelClass.mTaskIndex )
		{
			Cli_PrintfLine1( "TASK %d DeInit()", gKernelClass.mTaskIndex );
			gKernelClass.mpTaskCurrent->mfDeInit();

			lpTask     = Kernel_GetpTask( lNewTask );
			lPackFlags = gKernelClass.mPackageCurrentFlags ^ (lpTask->mPackageFlags | gKernelClass.mPackageGlobalFlags);

			lMask  = (U32)(1L<<31L);
			lIndex = 31;
			while( lMask )
			{
				if( lPackFlags & lMask )
				{
					if( gKernelClass.mPackageCurrentFlags & lMask )
					{
						PackageManager_UnLoad( Kernel_GetpPackage( lIndex ) );
						gKernelClass.mPackageCurrentFlags &= ~lMask;
					}
					else
					{
						PackageManager_Load( Kernel_GetpPackage( lIndex ) );
						gKernelClass.mPackageCurrentFlags |= lMask;
					}
				}
				lIndex--;
				lMask >>= 1L;
			}

			gKernelClass.mTaskIndex    = lNewTask;
			gKernelClass.mpTaskCurrent = lpTask;
		}

		PackageManager_Update();

		if( lpTask )
		{
			Kernel_Input_Update();
			Cli_PrintfLine1( "TASK %d Init()", gKernelClass.mTaskIndex );
			lpTask->mfInit();
		}
	}
	Kernel_InputBufferSave();

	lIndex = 0;
	lMask  = 1L;
	for( lIndex =0; lIndex<32; lIndex++ )
	{
		if( lMask & gKernelClass.mPackageCurrentFlags )
		{
			PackageManager_UnLoad( Kernel_GetpPackage( lIndex ) );
			gKernelClass.mPackageCurrentFlags &= ~lMask;
		}
		lMask <<= 1L;
	}
	while( !PackageManager_OpQueueIsEmpty() )
	{
		PackageManager_Update();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_RequestShutdown( void )
* ACTION   : Kernel_RequestShutdown
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_RequestShutdown( void )
{
	gKernelClass.mShutDownFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_Clocks_Update( void )
* ACTION   : Kernel_Clocks_Update
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_Clocks_Update( void )
{
	U16			i;


	for( i=0; i<eKERNEL_CLOCK_LIMIT; i++ )
	{
		Clock_Update( &gKernelClass.mClocks[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_PackagesLoad( const U32 aFlags )
* ACTION   : Kernel_PackagesLoad
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_PackagesLoad( const U32 aFlags )
{
	U32	lMask;
	U32	lIndex;

	lMask  = (U32)(1L<<31L);
	lIndex = 31;
	while( lMask )
	{
		if( aFlags & lMask )
		{
			PackageManager_Load( Kernel_GetpPackage( (U16)lIndex ) );
		}
		lIndex--;
		lMask >>= 1L;
	}

	gKernelClass.mPackageCurrentFlags |= aFlags;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_PackagesUnLoad( const U32 aFlags )
* ACTION   : Kernel_PackagesUnLoad
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_PackagesUnLoad( const U32 aFlags )
{
	U32	lMask;
	U32	lIndex;

	lMask  = (U32)(1L<<31L);
	lIndex = 31;
	while( lMask )
	{
		if( aFlags & lMask )
		{
			PackageManager_UnLoad( Kernel_GetpPackage( (U16)lIndex ) );
		}
		lIndex--;
		lMask >>= 1L;
	}

	gKernelClass.mPackageCurrentFlags &= ~aFlags;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_GlobalPackagesLoad( const U32 aFlags )
* ACTION   : Kernel_GlobalPackagesLoad
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_GlobalPackagesLoad( const U32 aFlags )
{
	Kernel_PackagesLoad( aFlags );
	gKernelClass.mPackageGlobalFlags |= aFlags;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_GlobalPackagesUnLoad( const U32 aFlags )
* ACTION   : Kernel_GlobalPackagesUnLoad
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_GlobalPackagesUnLoad( const U32 aFlags )
{
	Kernel_PackagesUnLoad( aFlags );
	gKernelClass.mPackageGlobalFlags &= ~aFlags;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_InputsEnumerate( void )
* ACTION   : Kernel_InputsEnumerate
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_InputsEnumerate( void )
{
	U16	i;

	for( i=0; i<3; i++ )
	{
		Input_Init( &gKernelClass.mFireInputs[ i ] );
	}
	for( i=0; i<eINPUTTYPE_LIMIT; i++ )
	{
		Input_Init( &gKernelClass.mDirInputs[ i ] );
	}

	/* processing team taps is slow, so lets disable these for now */
	/* if game wants to use team taps, have to change this code */

	if( 0 == gKernelUsesTeamTapsFlag )
	{
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADA ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADB ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADC ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADD ] = 1;

		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADA ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADB ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADC ] = 1;
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADD ] = 1;
	}


	if( gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADA ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADB ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADC ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADD ] )
	{
		IKBD_DisableTeamTap0();
	}
	if( gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADA ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADB ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADC ] ||
		gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADD ] )
	{
		IKBD_DisableTeamTap1();
	}

	gKernelClass.mFireInputCount = 0;
	if( IKBD_IsMouseEnabled() )
	{
		gKernelClass.mFireInputs[ 0 ].mInputType = eINPUTTYPE_MOUSE;
		gKernelClass.mFireInputCount = 1;
	}
	else if( IKBD_IsJoystickEnabled() )
	{
		gKernelClass.mFireInputs[ 0 ].mInputType = eINPUTTYPE_JOY0;
		gKernelClass.mFireInputs[ 1 ].mInputType = eINPUTTYPE_JOY1;
		gKernelClass.mFireInputCount = 2;
	}

	Input_Init( &gKernelClass.mDirInputs[ 0 ] );
	gKernelClass.mDirInputs[ 0 ].mInputType = eINPUTTYPE_IKBD;
	gKernelClass.mDirInputCount = 1;

	if( IKBD_IsPad0Enabled() )
	{
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_PADA ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_PADA;
			gKernelClass.mDirInputCount++;
		}
	}
	else if( IKBD_IsTeamTap0Enabled() )
	{
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADA ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP0_PADA;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADB ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP0_PADB;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADC ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP0_PADC;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP0_PADD ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP0_PADD;
			gKernelClass.mDirInputCount++;
		}
	}

	if( IKBD_IsPad1Enabled() )
	{
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_PADB ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_PADB;
			gKernelClass.mDirInputCount++;
		}
	}
	else if( IKBD_IsTeamTap1Enabled() )
	{
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADA ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP1_PADA;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADB ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP1_PADB;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADC ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP1_PADC;
			gKernelClass.mDirInputCount++;
		}
		if( !gKernelClass.mDisabledInputs[ eINPUTTYPE_TAP1_PADD ] )
		{
			gKernelClass.mDirInputs[ gKernelClass.mDirInputCount ].mInputType = eINPUTTYPE_TAP1_PADD;
			gKernelClass.mDirInputCount++;
		}
	}
	Kernel_Input_Update();
	Kernel_Input_Update();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_Input_Update( void )
* ACTION   : Kernel_Input_Update
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_Input_Update( void )
{
	U16	i;

	Input_CombinedUpdate( &gKernelClass.mCombinedInput, &gKernelClass.mDirInputs[ 0 ], gKernelClass.mDirInputCount );
	for( i=0; i<gKernelClass.mFireInputCount; i++ )
	{
		Input_Update( &gKernelClass.mFireInputs[ i ] );
		Input_CombineFire( &gKernelClass.mCombinedInput, &gKernelClass.mCombinedInput, &gKernelClass.mFireInputs[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_MonitorInputsUpdate( void )
* ACTION   : Kernel_MonitorInputsUpdate
* CREATION : 30.08.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_MonitorInputsUpdate( void )
{
	U16	i,j;
	U8	lBits;

	gKernelClass.mMonitoredUpdateCount++;
	for( i=0; i<eINPUTTYPE_LIMIT; i++ )
	{
		Input_Update( &gKernelClass.mMonitoredInputs[ i ] );

		lBits = 0;
		for( j=0; j<eINPUTKEY_LIMIT; j++ )
		{
			lBits |= gKernelClass.mMonitoredInputs[ i ].mKeyStatus[ j ];
		}
		if( lBits )
		{
			gKernelClass.mMonitoredCounts[ i ]++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_BuildDisabled( void )
* ACTION   : Kernel_BuildDisabled
* CREATION : 31.08.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_BuildDisabled( void )
{
	U16	i;

	for( i=0; i<eINPUTTYPE_LIMIT; i++ )
	{
		if( gKernelClass.mMonitoredCounts[ i ] >= dKERNEL_MONITOR_CUTOFF )
		{
			gKernelClass.mDisabledInputs[ i ] = 1;
		}
		else
		{
			gKernelClass.mDisabledInputs[ i ] = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_GetpTask( const U16 aIndex )
* ACTION   : Kernel_GetpTask
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sKernelTask *	Kernel_GetpTask( const U16 aIndex )
{
	U16	i;

	for( i=0; i<gKernelClass.mTaskCount; i++ )
	{
		if( gKernelClass.mpTasks[ i ].mIndex == aIndex )
		{
			return( &gKernelClass.mpTasks[ i ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_GetpPackage( const U16 aIndex )
* ACTION   : Kernel_GetpPackage
* CREATION : 02.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sPackage *	Kernel_GetpPackage( const U16 aIndex )
{
	U16	i;

	for( i=0; i<gKernelClass.mPackageCount; i++ )
	{
		if( gKernelClass.mpPackageDefs[ i ].mIndex == aIndex )
		{
			return( &gKernelClass.mpPackages[ i ] );
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_GetpClock( const U16 aClockIndex )
* ACTION   : Kernel_GetpClock
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sClock *	Kernel_GetpClock( const U16 aClockIndex )
{
	return( &gKernelClass.mClocks[ aClockIndex ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Kernel_ClockStartCountDown(const U16 aIndex,const U8 aMinutes,const U8 aSeconds)
* ACTION   : Kernel_ClockStartCountDown
* CREATION : 06.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void Kernel_ClockStartCountDown(const U16 aIndex,const U8 aMinutes,const U8 aSeconds)
{
	sClock *	lpClock;

	if( aIndex < eKERNEL_CLOCK_LIMIT )
	{
		lpClock = &gKernelClass.mClocks[ aIndex ];
		lpClock->mCountDownStartTime.mHours = 0;
		lpClock->mCountDownStartTime.mMicroSeconds = 0;
		lpClock->mCountDownStartTime.mMinutes = aMinutes;
		lpClock->mCountDownStartTime.mSeconds = aSeconds;
		Clock_Start( lpClock );
	}
}

/*
	routines for saving input into an input buffer for replay
	the buffer format stores the "sInput" structure
	the first bytes are a raw sInput structure, and what follows is delta packing
	 byte0 : bit7    : 1=frame end, 0=more data
	       bits0-6 : offset into sInput structure
	 byte1 : payload

	 packets are either single byte (0xff, nothing changed this frame) or multi-byte (offset,data)
	 bit 7 marks end of frame
*/


void	Kernel_InputBufferSet( const sInput * apInput )
{
	if( gKernelClass.mpInputBuffer )
	{
		if( 0 == gKernelClass.mInputBufferOffset )
		{
			if( gKernelClass.mInputBufferSize >= sizeof(sInput) )
			{
				Memory_Copy( sizeof(sInput), apInput, gKernelClass.mpInputBuffer );
				gKernelClass.mInputBufferOffset = sizeof(sInput);
				gKernelClass.mInputBufferCurrent = *apInput;
			}
		}
		else
		{
			U16 i;
			U8 * lpOld;
			U8 * lpNew;
			U8 * lpLast;

			lpOld = (U8*)&gKernelClass.mInputBufferCurrent;
			lpNew = (U8*)apInput;
			lpLast = 0;

			for( i=0; i<sizeof(sInput); i++ )
			{
				if( lpOld[ i ] != lpNew[ i ] )
				{
					if( gKernelClass.mInputBufferOffset + 2 < gKernelClass.mInputBufferSize )
					{
						lpLast = &gKernelClass.mpInputBuffer[ gKernelClass.mInputBufferOffset ];
						gKernelClass.mpInputBuffer[ gKernelClass.mInputBufferOffset ] = (U8)i;
						gKernelClass.mInputBufferOffset++;
						gKernelClass.mpInputBuffer[ gKernelClass.mInputBufferOffset ] = lpNew[ i ];
						gKernelClass.mInputBufferOffset++;
					}
				}
			}

			if( lpLast )
			{
				*lpLast |= 0x80;
			}
			else if( gKernelClass.mInputBufferOffset + 1 < gKernelClass.mInputBufferSize )
			{
				gKernelClass.mpInputBuffer[ gKernelClass.mInputBufferOffset ] = 0xFF;
				gKernelClass.mInputBufferOffset++;
			}
		}
	}
}


void	Kernel_InputBufferGet( sInput * apInput )
{
	if( gKernelClass.mpInputBuffer )
	{
		if( 0 == gKernelClass.mInputBufferOffset )
		{
			if( gKernelClass.mInputBufferSize >= sizeof(sInput) )
			{
				Memory_Copy( sizeof(sInput), gKernelClass.mpInputBuffer, &gKernelClass.mInputBufferCurrent );
				gKernelClass.mInputBufferOffset = sizeof(sInput);
			}
		}
		else
		{
			U8 *	lpSrc;
			U8 *	lpDst;
			U8	lByte0;

			lpSrc = &gKernelClass.mpInputBuffer[ gKernelClass.mInputBufferOffset ];
			lpDst = (U8*)&gKernelClass.mInputBufferCurrent;
			lByte0 = 0;
			while( (0 == (0x80 &lByte0)) && (gKernelClass.mInputBufferOffset < gKernelClass.mInputBufferSize) )
			{

				lByte0 = *lpSrc++;
				gKernelClass.mInputBufferOffset++;

				if( 0xFF != lByte0 )
				{
					Assert( (lByte0 & 0x7F) < sizeof(sInput) );

					lpDst[ lByte0 & 0x7F] = *lpSrc++;
					gKernelClass.mInputBufferOffset++;
				}
			}
		}
	}
	*apInput = gKernelClass.mInputBufferCurrent;
}

void	Kernel_InputBufferSave( void )
{
	if( (eKERNEL_INPUT_SAVING==gKernelClass.mInputType) && (gKernelClass.mpInputBuffer) && (gKernelClass.mInputBufferOffset) )
	{
		File_Save( "INPUTS.KIP", gKernelClass.mpInputBuffer, gKernelClass.mInputBufferOffset );
	}
}

void	Kernel_InputBufferLoad( const char * apFileName )
{
	if( apFileName )
	{
		gKernelClass.mInputBufferSize = File_GetSize( apFileName );
		if( gKernelClass.mInputBufferSize )
		{
			gKernelClass.mpInputBuffer = mMEMCALLOC( gKernelClass.mInputBufferSize );
			File_LoadAt( apFileName, gKernelClass.mpInputBuffer );
			gKernelClass.mInputType = eKERNEL_INPUT_LOADING;
		}
	}
}

void	Kernel_RequestInputLoad( const char * apFileName )
{
	gpKernelInputLoadFileName = apFileName;
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_achunlock( const char * apArgs )
* ACTION   : Kernel_CLI_achunlock
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_achunlock( const char * apArgs )
{
	(void)apArgs;
/*
	U16	lTemp;

	lTemp = (U16)atoi( apArgs );

	Achieve_Task_UnLock( lTemp );*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_mem( const char * apArgs )
* ACTION   : Kernel_CLI_mem
* CREATION : 13.11.2002 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_mem( const char * apArgs )
{
	(void)apArgs;
	Cli_PrintLine( "MEMORY DEBUG:" );
	Cli_PrintfLine1( "allocs : %ld", Memory_GetAllocCount() );
	Cli_PrintfLine1( "size   : %ld", Memory_GetAllocatedSize() );
	Cli_PrintfLine1( "hitide : %ld", Memory_GetHighTide() );
	Cli_PrintfLine1( "bigal  : %ld", Memory_GetLargestAlloc() );
	Cli_PrintfLine1( "lilal  : %ld", Memory_GetSmallestAlloc() );
	Cli_PrintfLine1( "fail   : %ld", Memory_GetFailedSize() );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_ass( const char * apArgs )
* ACTION   : Kernel_CLI_ass
* CREATION : 11.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_ass( const char * apArgs )
{
	(void)apArgs;
	Cli_Action( PackageManager_ShowAll( Cli_PrintLine ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_assunused( const char * apArgs )
* ACTION   : Kernel_CLI_assunused
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_assunused( const char * apArgs )
{
	(void)apArgs;
	Cli_Action( PackageManager_ShowUnused( Cli_PrintLine ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_build( const char * apArgs )
* ACTION   : Kernel_CLI_build
* CREATION : 13.02.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_build( const char * apArgs )
{
	(void)apArgs;

	Cli_PrintfLine1( "DATE  : %s", __DATE__ );
	Cli_PrintfLine1( "TIME  : %s", __TIME__ );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_inp( const char * apArgs )
* ACTION   : Kernel_CLI_input
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_inp( const char * apArgs )
{
	U16	i;
	char	lString[ 128 ];
	char	lHitString[ eINPUTKEY_LIMIT+ 1 ];
	char	lHeldString[ eINPUTKEY_LIMIT+ 1 ];
	char	lUnHitString[ eINPUTKEY_LIMIT+ 1 ];

	(void)apArgs;
	Cli_PrintLine( "INPUT   HIT      HELD     UNHIT" );
	for( i=0; i<gKernelClass.mDirInputCount; i++ )
	{
		Input_BuildHeldString(  &gKernelClass.mDirInputs[ i ], lHeldString );
		Input_BuildHitString(   &gKernelClass.mDirInputs[ i ], lHitString );
		Input_BuildUnHitString( &gKernelClass.mDirInputs[ i ], lUnHitString );
		sprintf( lString, "%s %s %s %s", Input_GetTypeName( &gKernelClass.mDirInputs[ i ] ), lHitString, lHeldString, lUnHitString );
		Cli_PrintLine( lString );
	}
	Cli_PrintLine( " " );
	for( i=0; i<gKernelClass.mFireInputCount; i++ )
	{
		Input_BuildHeldString(  &gKernelClass.mFireInputs[ i ], lHeldString );
		Input_BuildHitString(   &gKernelClass.mFireInputs[ i ], lHitString );
		Input_BuildUnHitString( &gKernelClass.mFireInputs[ i ], lUnHitString );
		sprintf( lString, "%s %s %s %s", Input_GetTypeName( &gKernelClass.mFireInputs[ i ] ), lHitString, lHeldString, lUnHitString );
		Cli_PrintLine( lString );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_quit( const char * apArgs )
* ACTION   : Kernel_CLI_quit
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_quit( const char * apArgs )
{
	(void)apArgs;
	Kernel_RequestShutdown();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_sys( const char * apArgs )
* ACTION   : Kernel_CLI_sys
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_sys( const char * apArgs )
{
	(void)apArgs;
	Cli_PrintfLine1( "CPU : %s", System_GetNameCPU() );
	Cli_PrintfLine1( "BLT : %s", System_GetNameBLT() );
	Cli_PrintfLine1( "DSP : %s", System_GetNameDSP() );
	Cli_PrintfLine1( "EMU : %s", System_GetNameEMU() );
	Cli_PrintfLine1( "FPU : %s", System_GetNameFPU() );
	Cli_PrintfLine1( "MCH : %s", System_GetNameMCH() );
	Cli_PrintfLine1( "MON : %s", System_GetNameMON() );
	Cli_PrintfLine1( "MEM : %ld", System_GetMemory() );
	Cli_PrintfLine1( "TOS : %x", System_GetTosVersion() );
	Cli_PrintfLine2( "FRQ : %d.%d", System_GetRefreshRate()->w.w1, System_GetRefreshRate()->w.w0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Kernel_CLI_vid( const char * apArgs )
* ACTION   : Kernel_CLI_vid
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Kernel_CLI_vid( const char * apArgs )
{
	sVideoConfig	lConfig;

	(void)apArgs;
	Video_GetConfig( &lConfig );
	Cli_PrintfLine1( "Width  : %d", lConfig.mWidth  );
	Cli_PrintfLine1( "Height : %d", lConfig.mHeight );
	Cli_PrintfLine1( "Mode   : %d", lConfig.mMode   );
	Cli_PrintfLine1( "NTSC   : %d", lConfig.mNTSCFlag   );
	Cli_PrintfLine1( "Cinema : %d", lConfig.mWideScreenFlag  );
	Cli_PrintfLine1( "H Off  : %d", Video_GetHorizontalOffset() );
	Cli_PrintfLine1( "V Off  : %d", Video_GetVerticalOffset() );
}


/* ################################################################################ */

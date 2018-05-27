/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: VIDEO.C
::
:: Video register manipulation routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"VIDEO.H"
#include	"VID_D3D.H"
#include	"VID_SDL.H"

#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	void *	mpPhysic;
	U16 *	mpPalST;
	U16 *	mpPalTT;
	U32 *	mpPalFalcon;
	U16		mCanvasWidth;
	U16		mWidth;
	U16		mHeight;
	U16		mMode;
	U16		mScrollX;
	U16		mScrollY;
	U16		mScanLineWords0;
	U16		mScanLineWords1;
	S16		mHoff;
	S16		mVoff;
	U16		mHDB;
	U16		mHDE;
	U16		mVDB;
	U16		mVDE;
	U8		mNTSCFlag;
	U8		mWideScreenFlag;
	U8		mUpdatePhysicFlag;
	U8		mUpdatePalSTFlag;
	U8		mUpdatePalTTFlag;
	U8		mUpdatePalFalconFlag;
	U8		mUpdateScrollFlag;
	U8		mEmuColourDepth;
	U8		mEmuResolution;
} sVideo;


typedef	struct
{
	U32	mpPhysic;
	U32	mpLogic;
	U16	mRes;
	U8	mRegMode;
	U8	mRegSync;
} sVideoSaveRegsST;


typedef	struct
{
	U32	mpPhysic;
	U32	mpLogic;
	U16	mRes;
	U8	mRegMode;
	U8	mRegSync;
	U8	mRegLineWidth;
	U8	mRegScroll;
} sVideoSaveRegsSTE;


typedef	struct
{
	U32	mRegs[32];
	U16	mPalST[16];
	U32	mPalFalcon[256];
} sVideoSaveState;


typedef struct
{
	void *	mpPalBase;
	void **	mppLines;
	U32		mPalSizeBytes;
	U16		mSplitCount;
} sVideoPalSplitter;

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Video_SaveRegsST(     void * apBuffer );
void	Video_RestoreRegsST(  void * apBuffer );
void	Video_SaveRegsSTE(    void * apBuffer );
void	Video_RestoreRegsSTE( void * apBuffer );
void	Video_SaveRegsTT(     void * apBuffer );
void	Video_RestoreRegsTT(  void * apBuffer );
void	Video_SaveRegsFalcon(     void * apBuffer );
void	Video_RestoreRegsFalcon(  void * apBuffer );

extern	void	Video_XbiosSetScreen( U16 aMode );
extern	void	Video_XbiosFalconSetLow( void );
extern	void	Video_SetFalconSTLowVGA( void );
extern	void	Video_SetFalconSTLowRGBPAL_W( void );
extern	void	Video_SetFalconSTLowRGBNTSC_W( void );

extern	void	Video_SetFalconSTLowRGBPAL( void );
extern	void	Video_SetFalconSTLowRGBNTSC( void );

extern	void	Video_UpdateRegsST( void );
extern	void	Video_UpdateRegsSTE( void );
extern	void	Video_UpdateRegsTT( void );
extern	void	Video_UpdateRegsFalcon( void );

extern	void	Video_SetFalconTC50RGB( void );
extern	void	Video_SetFalconTC60RGB( void );
extern	void	Video_SetFalconTC50VGA( void );
extern	void	Video_SetFalconTC60VGA( void );


/* ###################################################################################
#  VARIABLES
################################################################################### */

sVideoSaveState		gVideoSaveState;
sVideo				gVideo;
sVideoPalSplitter	gVideoPalSplitter;


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Init( void )
* ACTION   : saves video registers
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Init()
{
	if( System_GetpEmuDesc() )
	{
		gVideo.mEmuColourDepth = System_GetpEmuDesc()->mColourDepth;
		gVideo.mEmuResolution  = System_GetpEmuDesc()->mResolution;
	}

	gVideo.mUpdatePhysicFlag    = 0xFF;
	gVideo.mUpdatePalSTFlag     = 0xFF;
	gVideo.mUpdatePalTTFlag     = 0xFF;
	gVideo.mUpdatePalFalconFlag = 0xFF;
	gVideo.mUpdateScrollFlag    = 0xFF;

	switch( System_GetVDO() )
	{
	case VDO_ST:
		Video_SaveRegsST( &gVideoSaveState.mRegs[0] );
		Video_GetPalST(   &gVideoSaveState.mPalST[0] );
		Vbl_SetVideoFunc( Video_UpdateRegsST );
		break;
	case VDO_STE:
		Video_SaveRegsSTE( &gVideoSaveState.mRegs[0] );
		Video_GetPalST(    &gVideoSaveState.mPalST[0] );
		Vbl_SetVideoFunc( Video_UpdateRegsSTE );
		break;
	case VDO_TT:
		Video_SaveRegsTT( &gVideoSaveState.mRegs[0] );
		Video_GetPalST(   &gVideoSaveState.mPalST[0] );
		Vbl_SetVideoFunc( Video_UpdateRegsTT );
		break;
	case VDO_FALCON:
		Video_SaveRegsFalcon( &gVideoSaveState.mRegs[0] );
		Video_GetPalST(       &gVideoSaveState.mPalST[0] );
		Video_GetPalFalcon(   &gVideoSaveState.mPalFalcon[0] );
		Vbl_SetVideoFunc( Video_UpdateRegsFalcon );
		break;
	default:
		DebugChannel_Printf0( eDEBUGCHANNEL_GODLIB, "Video_Init() WARNING : Unknown hardware type" );
		break;
	}
#if defined	(dGODLIB_SYSTEM_D3D)
	VideoD3D_Init();
#elif defined (dGODLIB_SYSTEM_SDL)
	Video_SDL_Init();
#endif
#ifndef	dGODLIB_PLATFORM_ATARI
	gVideoPalSplitter.mpPalBase = 0;
#endif


	if( System_GetRefreshRate()->w.w1 > 55 )
	{
		gVideo.mNTSCFlag       = 1;
	}
	else
	{
		gVideo.mNTSCFlag       = 0;
	}
	gVideo.mWideScreenFlag = 1;

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_DeInit( void )
* ACTION   : restores video registers
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_DeInit()
{
	if( System_GetpEmuDesc() )
	{
		System_GetpEmuDesc()->mColourDepth = gVideo.mEmuColourDepth;
		System_GetpEmuDesc()->mResolution  = gVideo.mEmuResolution;
	}

	gVideo.mUpdatePhysicFlag    = 0xFF;
	gVideo.mUpdatePalSTFlag     = 0xFF;
	gVideo.mUpdatePalTTFlag     = 0xFF;
	gVideo.mUpdatePalFalconFlag = 0xFF;
	gVideo.mUpdateScrollFlag    = 0xFF;

	switch( System_GetVDO() )
	{
	case VDO_ST:
		Video_RestoreRegsST( &gVideoSaveState.mRegs[0] );
		Video_SetPalST(      &gVideoSaveState.mPalST[0] );
		break;
	case VDO_STE:
		Video_RestoreRegsSTE( &gVideoSaveState.mRegs[0] );
		Video_SetPalST(       &gVideoSaveState.mPalST[0] );
		break;
	case VDO_TT:
		Video_RestoreRegsTT( &gVideoSaveState.mRegs[0] );
		Video_SetPalST(      &gVideoSaveState.mPalST[0] );
		break;
	case VDO_FALCON:
		Video_RestoreRegsFalcon( &gVideoSaveState.mRegs[0] );
		Video_SetPalST(          &gVideoSaveState.mPalST[0] );
		Video_SetPalFalcon(      &gVideoSaveState.mPalFalcon[0] );
		break;
	default:
		break;
	}
#ifndef	dGODLIB_PLATFORM_ATARI
	if( gVideoPalSplitter.mpPalBase )
	{
		mMEMFREE( gVideoPalSplitter.mpPalBase );
		gVideoPalSplitter.mpPalBase = 0;
	}
	if( gVideoPalSplitter.mppLines )
	{
		mMEMFREE( gVideoPalSplitter.mppLines  );
		gVideoPalSplitter.mppLines  = 0;
	}
#endif
#if defined	(dGODLIB_SYSTEM_D3D)
	VideoD3D_DeInit();
#elif defined (dGODLIB_SYSTEM_SDL)
	Video_SDL_DeInit();
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetResolution( const U16 aWidth,const U16 aHeight,const U16 aMode,const U16 aCanvasWidth )
* ACTION   : Video_SetResolution
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	Video_SetResolution( const U16 aWidth,const U16 aHeight,const U16 aMode,const U16 aCanvasWidth )
{
	U8	lRes;

	lRes = Video_SetMode( aWidth, aHeight, aMode, aCanvasWidth, gVideo.mNTSCFlag, gVideo.mWideScreenFlag );

#ifndef	dGODLIB_PLATFORM_ATARI
	if( gVideoPalSplitter.mpPalBase )
	{
		mMEMFREE( gVideoPalSplitter.mpPalBase );
	}
	if( gVideoPalSplitter.mppLines )
	{
		mMEMFREE( gVideoPalSplitter.mppLines );
	}
	gVideoPalSplitter.mPalSizeBytes = 16*2;
	gVideoPalSplitter.mpPalBase = mMEMCALLOC( gVideoPalSplitter.mPalSizeBytes * aHeight );
	gVideoPalSplitter.mppLines  = (void**)mMEMCALLOC( 4 * aHeight );
	Video_PalSplitResetST();
#endif

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetMode( const U16 aWidth, const U16 aHeight, const U16 aMode, const U16 aCanvasWidth, const U8 aNTSCFlag, const U8 aWideScreenFlag )
* ACTION   : sets video mode
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

U8	Video_SetMode( const U16 aWidth, const U16 aHeight, const U16 aMode, const U16 aCanvasWidth, const  U8 aNTSCFlag, const U8 aWideScreenFlag )
{
	U8	lRes;

	lRes = 0;

	gVideo.mWidth          = aWidth;
	gVideo.mHeight         = aHeight;
	gVideo.mCanvasWidth    = aCanvasWidth;
	gVideo.mNTSCFlag       = aNTSCFlag;
	gVideo.mWideScreenFlag = aWideScreenFlag;
	gVideo.mMode           = aMode;

	switch( aMode )
	{
	case eVIDEO_MODE_1PLANE:
		gVideo.mScanLineWords0 = (U16)(aCanvasWidth >> 4);
		gVideo.mScanLineWords1 = (U16)(gVideo.mScanLineWords0 - 1);
		break;
	case eVIDEO_MODE_2PLANE:
		gVideo.mScanLineWords0 = (U16)(aCanvasWidth >> 3);
		gVideo.mScanLineWords1 = (U16)(gVideo.mScanLineWords0 - 1);
		break;
	case eVIDEO_MODE_4PLANE:
		gVideo.mScanLineWords0 = (U16)(aCanvasWidth >> 2);
		gVideo.mScanLineWords1 = (U16)(gVideo.mScanLineWords0 - 1);
		break;
	case eVIDEO_MODE_8PLANE:
		gVideo.mScanLineWords0 = (U16)(aCanvasWidth >> 1);
		gVideo.mScanLineWords1 = (U16)(gVideo.mScanLineWords0 - 1);
		break;
	case eVIDEO_MODE_RGB565:
		gVideo.mScanLineWords0 = aCanvasWidth;
		gVideo.mScanLineWords1 = (U16)(gVideo.mScanLineWords0 - 1);
		break;
	}

#ifdef	dGODLIB_PLATFORM_ATARI
	switch( System_GetVDO() )
	{
	case VDO_ST:
		if( aCanvasWidth != aWidth )
		{
			DebugChannel_Printf2( eDEBUGCHANNEL_GODLIB, "Video_SetMode() : error : width!=canvas width on ST %d %d\n", aWidth, aCanvasWidth );
			return( 0 );
		}

		switch( aMode )
		{

		case eVIDEO_MODE_1PLANE:
			Vbl_WaitVbl();
			if( (aWidth == 640) && (aHeight == 400 ) )
			{
				*(U8 *)0xFFFF8260L = 2;
				lRes = 1;
			}
			break;

		case eVIDEO_MODE_2PLANE:
			Vbl_WaitVbl();
			if( (aWidth == 640) && (aHeight == 200 ) )
			{
				*(U8 *)0xFFFF8260L = 1;
				if( !aNTSCFlag )
				{
					*(U8 *)0xFFFF820AL = 0x2;
				}
				else
				{
					*(U8 *)0xFFFF820AL = 0x0;
				}
				lRes = 1;
			}
			break;

		case eVIDEO_MODE_4PLANE:
			Vbl_WaitVbl();
			if( (aWidth == 320) && (aHeight == 200 ) )
			{
				*(U8 *)0xFFFF8260L = 0;
				if( !aNTSCFlag )
				{
					*(U8 *)0xFFFF820AL = 0x2;
				}
				else
				{
					*(U8 *)0xFFFF820AL = 0x0;
				}
				lRes = 1;
			}
			break;
		case	eVIDEO_MODE_RGB565:
			if( System_GetpEmuDesc() )
			{
				System_GetpEmuDesc()->mColourDepth = 2;
				System_GetpEmuDesc()->mResolution  = 0;
				lRes = 1;
			}
			break;

		}
		break;

	case VDO_STE:
		switch( aMode )
		{

		case eVIDEO_MODE_1PLANE:
			Vbl_WaitVbl();
			*(U8 *)0xFFFF8260L = 2;
			*(U8 *)0xFFFF8265L = 0;
			*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			lRes = 1;
			break;

		case eVIDEO_MODE_2PLANE:
			Vbl_WaitVbl();
			*(U8 *)0xFFFF8260L = 1;
			*(U8 *)0xFFFF8265L = 0;
			*(U8 *)0xFFFF820FL = 0; /*(U8)(gVideo.ScanLineWords1&0xFF);*/
			if( !aNTSCFlag )
			{
				*(U8 *)0xFFFF820AL = 0x2;
			}
			else
			{
				*(U8 *)0xFFFF820AL = 0x0;
			}
			lRes = 1;
			break;

		case eVIDEO_MODE_4PLANE:
			if( System_GetpEmuDesc() )
			{
				System_GetpEmuDesc()->mColourDepth = 0;
				System_GetpEmuDesc()->mResolution  = 0;
			}
			Vbl_WaitVbl();
			*(U8 *)0xFFFF8260L = 0;
			*(U8 *)0xFFFF8265L = 0;
			*(U8 *)0xFFFF820FL = 0; /*(U8)(gVideo.ScanLineWords0&0xFF); */
			if( !aNTSCFlag )
			{
				*(U8 *)0xFFFF820AL = 0x2;
			}
			else
			{
				*(U8 *)0xFFFF820AL = 0x0;
			}
			lRes = 1;
			break;

		case	eVIDEO_MODE_RGB565:
			if( System_GetpEmuDesc() )
			{
				*(U8 *)0xFFFF8260L = 0;
				System_GetpEmuDesc()->mColourDepth = 2;
				System_GetpEmuDesc()->mResolution  = 2;
				*(U8 *)0xFFFF820FL = aCanvasWidth-aWidth;
				lRes = 1;
			}
			break;

		}
		break;

	case VDO_TT:
		switch( aMode )
		{

		case eVIDEO_MODE_1PLANE:
			if( (aWidth <= 640) && (aHeight <= 400) )
			{
				*(U8 *)0xFFFF8260L = 2;
				*(U8 *)0xFFFF8265L = 0;
				*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			}
			else
			{
				*(U16 *)0xFFFF8262L = 0x500;
				*(U8 *)0xFFFF8265L = 0;
				*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			}
			lRes = 1;
			break;

		case eVIDEO_MODE_2PLANE:
			*(U8 *)0xFFFF8260L = 1;
			*(U8 *)0xFFFF8265L = 0;
			*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			lRes = 1;
			break;

		case eVIDEO_MODE_4PLANE:
			if( (aWidth <= 320) && (aHeight <= 200) )
			{
				*(U8 *)0xFFFF8260L = 0;
				*(U8 *)0xFFFF8265L = 0;
				*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			}
			else
			{
				*(U16 *)0xFFFF8262L = 0x400;
				*(U8 *)0xFFFF8265L = 0;
				*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			}
			lRes = 1;
			break;

		case eVIDEO_MODE_8PLANE:
			*(U16 *)0xFFFF8262L = 0x700;
			*(U8 *)0xFFFF8265L = 0;
			*(U8 *)0xFFFF820FL = (U8)(gVideo.mScanLineWords1&0xFF);
			lRes = 1;
			break;

		}
		break;

	case VDO_FALCON:

		switch( aMode )
		{

		case eVIDEO_MODE_1PLANE:
			if( (aWidth == 640) && (aHeight == 400 ) )
			{
				Video_XbiosSetScreen( 2 );
				lRes = 1;
			}
			break;
		case eVIDEO_MODE_2PLANE:
			if( (aWidth == 640) && (aHeight == 200 ) )
			{
				Video_XbiosSetScreen( 1 );
				lRes = 1;
			}
			break;
		case eVIDEO_MODE_4PLANE:
			if( (aWidth == 320) && (aHeight == 200 ) )
			{
/*					Video_XbiosSetScreen( 0 );*/
				if( System_GetMON() == MON_VGA )
				{
					Video_SetFalconSTLowVGA();
				}
				else
				{
					if( aNTSCFlag )
					{
						if( aWideScreenFlag )
						{
							Video_SetFalconSTLowRGBNTSC_W();
						}
						else
						{
							Video_SetFalconSTLowRGBNTSC();
						}
					}
					else
					{
						if( aWideScreenFlag )
						{
							Video_SetFalconSTLowRGBPAL_W();
						}
						else
						{
							Video_SetFalconSTLowRGBPAL();
						}
					}
				}
				lRes = 0;
			}
			break;

		case	eVIDEO_MODE_RGB565:
			if( System_GetMON() == MON_VGA )
			{
				Video_SetFalconTC50VGA();
			}
			else
			{
				if( aNTSCFlag )
				{
					Video_SetFalconTC60RGB();
				}
				else
				{
					Video_SetFalconTC50RGB();
				}
			}
			break;
		}
		break;
	}

	if( System_GetMCH() == MCH_FALCON )
	{
		gVideo.mHDB = *(U16*)0xFFFF8288L;
		gVideo.mHDE = *(U16*)0xFFFF828AL;
		gVideo.mVDB = *(U16*)0xFFFF82A8L;
		gVideo.mVDE = *(U16*)0xFFFF82AAL;
	}
	Video_SetHorizontalOffset( gVideo.mHoff );
	Video_SetVerticalOffset( gVideo.mVoff );
#else
	lRes = 1;
#endif

#ifdef	dGODLIB_SYSTEM_D3D
	VideoD3D_ResolutionUpdate();
#endif
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetMode( void )
* ACTION   : Video_GetMode
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	Video_GetMode( void )
{
	return( gVideo.mMode );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetWidth( void )
* ACTION   : Video_GetWidth
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	Video_GetWidth( void )
{
	return( gVideo.mWidth );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetHeight( void )
* ACTION   : Video_GetHeight
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	Video_GetHeight( void )
{
	return( gVideo.mHeight );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetScreenSize()
* ACTION   : returns the size of the screen
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

U32		Video_GetScreenSize()
{
	U32	lSize;

	lSize  = gVideo.mScanLineWords0 << 1;
	lSize *= gVideo.mHeight;

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetPhysic( const void * apPhysic )
* ACTION   : sets physic to be apPhysic
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPhysic( const void * apPhysic )
{
	gVideo.mpPhysic          = (void *)apPhysic;
	gVideo.mUpdatePhysicFlag = 0;
#if defined (dGODLIB_SYSTEM_D3D)
	VideoD3D_DisplayScreen( (U16*)apPhysic );
#elif defined( dGODLIB_SYSTEM_SDL)
	Video_SDL_DisplayScreen( (U16*)apPhysic );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_pPhysic( void )
* ACTION   : returns pointer to physic
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void *	Video_GetpPhysic( void )
{
	return( gVideo.mpPhysic );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetNextPalST( const U16 * apPal )
* ACTION   : sets ST palette on next VBL interrupt
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetNextPalST( const U16 * apPal )
{
	gVideo.mpPalST          = (U16*)apPal;
	gVideo.mUpdatePalSTFlag = 0;
#if defined (dGODLIB_SYSTEM_D3D)
	Video_SetPalST( apPal );
#elif defined (dGODLIB_SYSTEM_SDL)
	Video_SetPalST( apPal );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetNextPalTT( const U16 * apPal )
* ACTION   : sets TT palette on next VBL interrupt
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetNextPalTT( const U16 * apPal )
{
	gVideo.mpPalTT          = (U16*)apPal;
	gVideo.mUpdatePalTTFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetNextPalFalcon( const U16 * apPal )
* ACTION   : sets Falcon palette on next VBL interrupt
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetNextPalFalcon( const U32 * apPal )
{
	gVideo.mpPalFalcon          = (U32*)apPal;
	gVideo.mUpdatePalFalconFlag = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetConfig( sVideoConfig * apConfig )
* ACTION   : Video_GetConfig
* CREATION : 27.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_GetConfig( sVideoConfig * apConfig )
{
	if( apConfig )
	{
		apConfig->mCanvasWidth    = gVideo.mCanvasWidth;
		apConfig->mHeight         = gVideo.mHeight;
		apConfig->mMode           = gVideo.mMode;
		apConfig->mNTSCFlag       = gVideo.mNTSCFlag;
		apConfig->mWideScreenFlag = gVideo.mWideScreenFlag;
		apConfig->mWidth          = gVideo.mWidth;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetConfig( sVideoConfig * apConfig )
* ACTION   : Video_SetConfig
* CREATION : 27.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetConfig( sVideoConfig * apConfig )
{
	if( apConfig )
	{
		Video_SetMode( apConfig->mWidth, apConfig->mHeight, apConfig->mMode, apConfig->mCanvasWidth, apConfig->mNTSCFlag, apConfig->mWideScreenFlag );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetHorizontalOffset( const S16 aOff )
* ACTION   : Video_SetHorizontalOffset
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetHorizontalOffset( const S16 aOff )
{
	gVideo.mHoff = aOff;
	if( System_GetMCH() == MCH_FALCON )
	{
		*(U16*)0xFFFF8288L = (U16)(gVideo.mHDB + aOff);
		*(U16*)0xFFFF828AL = (U16)(gVideo.mHDE + aOff);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetHorizontalOffset( void )
* ACTION   : Video_GetHorizontalOffset
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S16	Video_GetHorizontalOffset( void )
{
	return( gVideo.mHoff );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetVerticalOffset( const S16 aOff )
* ACTION   : Video_SetVerticalOffset
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetVerticalOffset( const S16 aOff )
{
	gVideo.mVoff = aOff;
	if( System_GetMCH() == MCH_FALCON )
	{
		*(U16*)0xFFFF82A8L = (U16)(gVideo.mVDB + (aOff<<1));
		*(U16*)0xFFFF82AAL = (U16)(gVideo.mVDE + (aOff<<1));
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetVerticalOffset( void )
* ACTION   : Video_GetVerticalOffset
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S16	Video_GetVerticalOffset( void )
{
	return( gVideo.mVoff );
}

#ifndef	dGODLIB_PLATFORM_ATARI


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetPalST( const U16 * apPal )
* ACTION   : Video_SetPalST
* CREATION : 07.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPalST( const U16 * apPal )
{
	Video_PalSplitResetST();
	Video_SetPalSplitST( 0, apPal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetPalST( U16 * apPal )
* ACTION   : Video_GetPalST
* CREATION : 07.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Video_GetPalST( U16 * apPal )
{
	if( apPal && gVideoPalSplitter.mpPalBase )
	{
		Memory_Copy( gVideoPalSplitter.mPalSizeBytes, gVideoPalSplitter.mpPalBase, apPal );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_PalSplitResetST( void )
* ACTION   : Video_PalSplitResetST
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Video_PalSplitResetST( void )
{
	U16	i;
	if( gVideoPalSplitter.mppLines )
	{
		for( i=0; i<gVideo.mHeight; i++ )
		{
			gVideoPalSplitter.mppLines[ i ] = gVideoPalSplitter.mpPalBase;
		}
	}
	gVideoPalSplitter.mSplitCount = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetPalSplitColourST( const U16 aLineY, const U16 aIndex, const U16 aColour )
* ACTION   : Video_SetPalSplitColourST
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPalSplitColourST( const U16 aLineY, const U16 aIndex, const U16 aColour )
{
	U16 *	lpPalDst;

	Video_PalSplitAlloc( aLineY );
	lpPalDst = (U16*)gVideoPalSplitter.mppLines[ aLineY ];
	lpPalDst[ aIndex ] = aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetPalSplitST( const U16 aLineY, const U16 * apPal )
* ACTION   : Video_SetPalSplitST
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPalSplitST( const U16 aLineY, const U16 * apPal )
{
	if( gVideoPalSplitter.mppLines )
	{
		Video_PalSplitAlloc( aLineY );
		Memory_Copy( 16*2, apPal, gVideoPalSplitter.mppLines[ aLineY ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetpPalSplitST( const U16 aLineY )
* ACTION   : Video_GetpPalSplitST
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

U16 *	Video_GetpPalSplitST( const U16 aLineY )
{
	U16 *	lpPal;
	lpPal = 0;
	if( gVideoPalSplitter.mppLines )
	{
		lpPal = (U16*)gVideoPalSplitter.mppLines[ aLineY ];
	}
	return( lpPal );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_PalSplitAlloc( const U16 aLineY )
* ACTION   : Video_PalSplitAlloc
* CREATION : 26.5.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Video_PalSplitAlloc( const U16 aLineY )
{
	U16	i;

	if( aLineY )
	{
		if( gVideoPalSplitter.mppLines[ aLineY ] == gVideoPalSplitter.mppLines[ aLineY-1 ])
		{
			U32	lOffset;
			U8 * lpSrc;

			lOffset  = gVideoPalSplitter.mPalSizeBytes;
			lOffset *= gVideoPalSplitter.mSplitCount;
			lpSrc =(U8*)gVideoPalSplitter.mpPalBase;
			Memory_Copy( gVideoPalSplitter.mPalSizeBytes, gVideoPalSplitter.mppLines[ aLineY-1 ], &lpSrc[ lOffset ] );
			for( i=aLineY; i<gVideo.mHeight; i++ )
			{
				gVideoPalSplitter.mppLines[ i ] = &lpSrc[ lOffset ];
			}
			gVideoPalSplitter.mSplitCount++;
		}
	}
	else
	{
		gVideoPalSplitter.mppLines[ 0 ] = gVideoPalSplitter.mpPalBase;
		if(	!gVideoPalSplitter.mSplitCount )
		{
			gVideoPalSplitter.mSplitCount++;
		}
	}
}

#endif

/* ################################################################################ */

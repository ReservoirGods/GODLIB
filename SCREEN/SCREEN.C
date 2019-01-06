/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"SCREEN.H"

#include	<GODLIB/DEBUG/DBGCHAN.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/VBL/VBL.H>
#include	<GODLIB/VIDEO/VIDEO.H>




/* ###################################################################################
#  DATA
################################################################################### */

sGraphicCanvas	gScreenBackGraphic;
sGraphicCanvas	gScreenLogicGraphic;
sGraphicCanvas	gScreenPhysicGraphic;
sGraphicCanvas	gScreenMiscGraphic;

sScreenClass	gScreenClass;


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Screen_Init( const U16 aWidth,const U16 aHeight,const U16 aBitDepth, const U16 aScrollFlags )
* ACTION   : Screen_Init
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Screen_Init( const U16 aWidth,const U16 aHeight,const U16 aBitDepth, const U16 aScrollFlags )
{
	U32	lSize;
	U32	lTotal;
	U32	lBase;
	U16	lHeight;
	U16	lWidth;

	Memory_Clear( sizeof(sScreenClass), &gScreenClass );

	if( aScrollFlags & eSCREEN_SCROLL_V )
	{
		lHeight = (U16)(aHeight + 32);
		lWidth  = (U16)(aWidth);

		GraphicCanvas_Init( &gScreenLogicGraphic,  aBitDepth, lWidth, lHeight );
		GraphicCanvas_Init( &gScreenPhysicGraphic, aBitDepth, lWidth, lHeight );
		GraphicCanvas_Init( &gScreenBackGraphic,   aBitDepth, lWidth, lHeight );
		GraphicCanvas_Init( &gScreenMiscGraphic,   aBitDepth, lWidth, lHeight );

		lSize  = gScreenLogicGraphic.mLineOffsets[ 1 ];
		lSize *= lHeight;
		lTotal = lSize * 4;

		lBase = (U32)mMEMSCREENCALLOC( lTotal + 255L );
		gScreenClass.mpMemBase                   = (U16*)lBase;
		lBase += 255L;
		lBase &= 0xFFFFFF00L;

		gScreenClass.mpBuffers[ eSCREEN_PHYSIC ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_BACK   ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_LOGIC  ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_MISC   ] = 0;
		Video_SetResolution( aWidth, aHeight, aBitDepth, lWidth );

	}
	else
	{
		GraphicCanvas_Init( &gScreenLogicGraphic,  aBitDepth, aWidth, aHeight );
		GraphicCanvas_Init( &gScreenPhysicGraphic, aBitDepth, aWidth, aHeight );
		GraphicCanvas_Init( &gScreenBackGraphic,   aBitDepth, aWidth, aHeight );
		GraphicCanvas_Init( &gScreenMiscGraphic,   aBitDepth, aWidth, aHeight );

		lSize  = gScreenLogicGraphic.mLineOffsets[ 1 ];
		lSize *= aHeight;

		lTotal = lSize * 3;

		lBase = (U32)mMEMSCREENCALLOC( lTotal + 255L );
		gScreenClass.mpMemBase                   = (U16*)lBase;
		lBase += 255L;
		lBase &= 0xFFFFFF00L;

		gScreenClass.mpBuffers[ eSCREEN_PHYSIC ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_LOGIC  ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_BACK   ] = (U16*)lBase;
		lBase += lSize;
		gScreenClass.mpBuffers[ eSCREEN_MISC   ] = 0;
		Video_SetResolution( aWidth, aHeight, aBitDepth, aWidth );
	}

	gScreenLogicGraphic.mpVRAM  = gScreenClass.mpBuffers[ eSCREEN_LOGIC  ];
	gScreenPhysicGraphic.mpVRAM = gScreenClass.mpBuffers[ eSCREEN_PHYSIC ];
	gScreenBackGraphic.mpVRAM   = gScreenClass.mpBuffers[ eSCREEN_BACK   ];
	gScreenMiscGraphic.mpVRAM   = 0;

	gScreenClass.mFrameRate     = 1;
	gScreenClass.mPhysicIndex   = 0;
	gScreenClass.mFirstTimeFlag = 1;


	DebugChannel_Printf1( eDEBUGCHANNEL_GODLIB, "Screen_Init() base %lx", lBase );

	Screen_Update();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Screen_DeInit( void )
* ACTION   : Screen_DeInit
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Screen_DeInit( void )
{
	mMEMSCREENFREE( gScreenClass.mpMemBase );
	gScreenClass.mpMemBase = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Screen_Update( void )
* ACTION   : Screen_Update
* CREATION : 01.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Screen_Update( void )
{
	uU32	lScrn;
	U32		lOff;
	S32		lVbls;
#ifdef	dGODLIB_PLATFORM_ATARI
	U16		lOldBack;
	U16 *	lpReg;
	U16		lResFlag;
#endif

	if( gScreenClass.mFirstTimeFlag )
	{
		gScreenClass.mLastVbl       = Vbl_GetCounter();
		gScreenClass.mFirstTimeFlag = 0;
	}

	gScreenClass.mPhysicIndex ^= 1;
	lScrn.l = ((U32)gScreenClass.mpBuffers[ gScreenClass.mPhysicIndex ]);

	if( gScreenClass.mScrollY )
	{
		lOff     = gScreenLogicGraphic.mLineOffsets[ 1 ];
		lOff    *= gScreenClass.mScrollY;
		lScrn.l += lOff;
	}

	Video_SetPhysic( (U16*)lScrn.l );

	if( System_GetMCH() == MCH_ST )
	{
		lScrn.w.w0 >>= 8;
#ifdef	dGODLIB_PLATFORM_ATARI
		*(U32*)0xFFFF8200L = lScrn.l;
#endif
	}

#ifdef	dGODLIB_PLATFORM_ATARI
	lpReg = (U16*)0xFFFF8240L;
	lOldBack = lpReg[ 0 ];

	if( IKBD_GetKeyStatus( eIKBDSCAN_TAB ) )
	{
		lpReg[ 0 ] = lOldBack ^ 0xFFF;
		lResFlag = 1;
	}
	else
	{
		lResFlag = 0;
	}
#endif

	do
	{
		Vbl_WaitVbl();
		lVbls = Vbl_GetCounter() - gScreenClass.mLastVbl;
	}
	while( lVbls < (S32)gScreenClass.mFrameRate );


#ifdef	dGODLIB_PLATFORM_ATARI
	if( lResFlag )
	{
		lpReg[ 0 ] = lOldBack;
	}
#endif

	gScreenClass.mLastVbl = Vbl_GetCounter();

	gScreenLogicGraphic.mpVRAM  = gScreenClass.mpBuffers[ gScreenClass.mPhysicIndex ^ 1 ];
	gScreenPhysicGraphic.mpVRAM = gScreenClass.mpBuffers[ gScreenClass.mPhysicIndex     ];
}


/* ################################################################################ */

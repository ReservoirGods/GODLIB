/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_SIGN.H"

#include	"ACH_MAIN.H"
#include	"ACH_SHOW.H"
#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SCREEN/SCREEN.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_UNLK_BUTTON_Y	100


/* ###################################################################################
#  ENUMS
################################################################################### */


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveUnLockedClass
{
	sAssetClient *	mpSmallFontAss;
	sAssetClient *	mpFontAss;
	sAssetClient *	mpBGAss;
	sAssetClient *	mpPalAss;
	sAssetClient *	mpUnLockSymbolAss;
	sAssetClient *	mpButtonAss;
	sAssetClient *	mpLogoAss;
	sFont *			mpFont;
	sFont *			mpSmallFont;
	sSpriteBlock *	mpButtonSpr;
	sSpriteBlock *	mpLogoSprites;
	sSpriteBlock *	mpUnLockSymbolSpr;
	U16 *			mpPal;
	U16				mTaskIndex;
	U16				mTaskUnLockedCount;
	U16				mBGRedrawFlag;
	U16				mWhiteOutFlag;
	uU32			mButtonFrame;
}sAchieveUnLockedClass;


/* ###################################################################################
#  DATA
################################################################################### */


sAchieveUnLockedClass	gAchieveUnLockedClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_UnLocked_BG_Update( void );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_AppInit( void )
* ACTION   : Achieve_UnLocked_AppInit
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UnLocked_AppInit( void )
{
	Memory_Clear( sizeof(sAchieveUnLockedClass), &gAchieveUnLockedClass );
	gAchieveUnLockedClass.mpFontAss         = AssetClient_Register( "FONTY.BFB",    "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpFont            );
	gAchieveUnLockedClass.mpSmallFontAss    = AssetClient_Register( "FONT8X8.BFB",  "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpSmallFont       );
	gAchieveUnLockedClass.mpPalAss          = AssetClient_Register( "ACHNEW.PAL",   "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpPal             );
	gAchieveUnLockedClass.mpUnLockSymbolAss = AssetClient_Register( "ACH_UNLK.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpUnLockSymbolSpr );
	gAchieveUnLockedClass.mpButtonAss       = AssetClient_Register( "ACH_BUTN.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpButtonSpr       );
	gAchieveUnLockedClass.mpLogoAss         = AssetClient_Register( "AWADUNLK.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveUnLockedClass.mpLogoSprites     );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_AppDeInit( void )
* ACTION   : Achieve_UnLocked_AppDeInit
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UnLocked_AppDeInit( void )
{
	AssetClient_UnRegister( gAchieveUnLockedClass.mpFontAss         );
	AssetClient_UnRegister( gAchieveUnLockedClass.mpSmallFontAss    );
	AssetClient_UnRegister( gAchieveUnLockedClass.mpPalAss          );
	AssetClient_UnRegister( gAchieveUnLockedClass.mpUnLockSymbolAss );
	AssetClient_UnRegister( gAchieveUnLockedClass.mpButtonAss       );
	AssetClient_UnRegister( gAchieveUnLockedClass.mpLogoAss         );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_Init( void )
* ACTION   : Achieve_UnLocked_Init
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UnLocked_Init( void )
{
	gAchieveUnLockedClass.mButtonFrame.l = 0;
	gAchieveUnLockedClass.mTaskIndex = 0;
	gAchieveUnLockedClass.mTaskUnLockedCount = Achieve_Tasks_GetNewUnLockedCount();
	gAchieveUnLockedClass.mBGRedrawFlag = 2;
	gAchieveUnLockedClass.mWhiteOutFlag = 0;
	if( gAchieveUnLockedClass.mpPal )
	{
		Fade_StartVblFade( gAchieveUnLockedClass.mpPal, 16 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_DeInit( void )
* ACTION   : Achieve_UnLocked_DeInit
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UnLocked_DeInit( void )
{
	Achieve_Tasks_ClearNewUnLocked();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_Update( sInput * apInput )
* ACTION   : Achieve_UnLocked_Update
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16	Achieve_UnLocked_Update( sInput * apInput )
{
	U16	lRes;

	lRes = 0;
	Achieve_UnLocked_BG_Update();

	if( gAchieveUnLockedClass.mWhiteOutFlag )
	{
		if( Fade_IsVblFadeFinished() )
		{
			gAchieveUnLockedClass.mWhiteOutFlag = 0;
			Fade_StartVblFade( gAchieveUnLockedClass.mpPal, 8 );
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_FIREA ] & eINPUTKEYSTATUS_HIT )
	{
		gAchieveUnLockedClass.mTaskIndex++;
		if( gAchieveUnLockedClass.mTaskIndex >= gAchieveUnLockedClass.mTaskUnLockedCount )
		{
			lRes = 1;
		}
		else
		{
			Fade_StartVblFade( Fade_GetpWhitePal(), 1 );
			gAchieveUnLockedClass.mBGRedrawFlag = 2;
			gAchieveUnLockedClass.mWhiteOutFlag = 1;
		}
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_UnLocked_BG_Update( void )
* ACTION   : Achieve_UnLocked_BG_Update
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_UnLocked_BG_Update( void )
{
	sGraphicCanvas *	lpBack;
	sGraphicCanvas *	lpLogic;
	sGraphicPos			lPos;
	U16					lTaskIndex;

	lpBack  = Screen_GetpBackGraphic();
	lpLogic = Screen_GetpLogicGraphic();

	if( gAchieveUnLockedClass.mBGRedrawFlag )
	{
		if( 2 == gAchieveUnLockedClass.mBGRedrawFlag )
		{
			lpBack->mpFuncs->ClearScreen( lpBack );

			lPos.mX = 0;
			lPos.mY = 0;
			lpBack->mpFuncs->DrawSprite( lpBack, &lPos, gAchieveUnLockedClass.mpLogoSprites->mpSprite[ 0 ] );

			lPos.mX = 0;
			lPos.mY = 199-24;
			lpBack->mpFuncs->DrawSprite( lpBack, &lPos, gAchieveUnLockedClass.mpLogoSprites->mpSprite[ 1 ] );

			lTaskIndex = Achieve_Tasks_GetNewUnLockedIndex( gAchieveUnLockedClass.mTaskIndex );
			lPos.mX = 0;
			lPos.mY = dACH_UNLK_BUTTON_Y;
			Achieve_Show_ButtonRender( lpBack, &lPos, lTaskIndex );
			Achieve_Show_SymbolRender( lpBack, &lPos, &gAchieveUnLockedClass.mButtonFrame, lTaskIndex );
		}
/*		lpLogic->mpFuncs->CopyScreen( lpLogic, lpBack->mpVRAM ); */
		Memory_Copy( 32000, lpBack->mpVRAM, lpLogic->mpVRAM );
		gAchieveUnLockedClass.mBGRedrawFlag--;
	}
}


/* ################################################################################ */

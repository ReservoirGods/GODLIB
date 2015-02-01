/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_SIGN.H"

#include	"ACH_MAIN.H"
#include	"ACH_SHOW.H"
#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SCREEN/SCREEN.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_SHOW_TASKS_PERPAGE	5
#define	dACH_SHOW_TASK_Y		32
#define	dACH_SHOW_TASK_HEIGHT	26
#define	dACH_SHOW_TASK_COUNT_X	2
#define	dACH_SHOW_TASK_COUNT_Y	2
#define	dACH_SHOW_TASK_COUNT_W	50
#define	dACH_SHOW_TASK_COUNT_H	16
#define	dACH_SHOW_TASK_POINTS_X	240
#define	dACH_SHOW_TASK_POINTS_Y	16
#define	dACH_SHOW_TASK_POINTS_W	80
#define	dACH_SHOW_TASK_POINTS_H	16
#define	dACH_SHOW_BUTTON_ANIMSPEED	0x2000


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_SHOW_TASKS_BUTTON_LOCKED,
	eACH_SHOW_TASKS_BUTTON_UNLOCKED,

	eACH_SHOW_TASKS_BUTTON_LIMIT
};

enum
{
	eACH_SHOW_SYMBOL_NONE,
	eACH_SHOW_SYMBOL_LOCKED,
	eACH_SHOW_SYMBOL_UNLOCKED,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveShowClass
{
	sAssetClient *	mpLogoAss;
	sAssetClient *	mpSmallFontAss;
	sAssetClient *	mpFontAss;
	sAssetClient *	mpBGAss;
	sAssetClient *	mpPalAss;
	sAssetClient *	mpButtonAss;
	sAssetClient *	mpLockSymbolAss;
	sAssetClient *	mpUnLockSymbolAss;
	sAssetClient *	mpButtonSausageAss;
	U16 *			mpPal;
	sFont *			mpFont;
	sFont *			mpSmallFont;
	sSpriteBlock *	mpLogoSprites;
	sSpriteBlock *	mpButtonSpr;
	sSpriteBlock *	mpLockSymbolSpr;
	sSpriteBlock *	mpUnLockSymbolSpr;
	sSpriteBlock *	mpButtonSausageSprites;
	sGemDosDTA		mDTA;
	U16				mBGRedrawFlag;
	U16				mMode;
	S16				mTaskTopIndex;
	U16				mButtonIndex;
	U16				mButtonOldIndex;
	S16				mTasksRedrawFlag;
	U16				mSymbolTypes[ dACH_SHOW_TASKS_PERPAGE ];
	U16				mSymbolRedrawFlags[ dACH_SHOW_TASKS_PERPAGE ];
	uU32			mSymbolFrames[ dACH_SHOW_TASKS_PERPAGE ];
	U16				mButtonSelectedFlags[ eACH_SHOW_TASKS_BUTTON_LIMIT ];
	S16				mButtonRedrawFlags[ eACH_SHOW_TASKS_BUTTON_LIMIT ];
}sAchieveShowClass;

sAchieveShowClass	gAchieveShowClass;


/* ###################################################################################
#  DATA
################################################################################### */

sGraphicRect	gAchieveShowTaskButtonRects[ eACH_SHOW_TASKS_BUTTON_LIMIT ] =
{
	{	10,174,140,16		},
	{	170,174,140,16		},
};

char *		gAchieveShowTaskButtonTitles[ eACH_SHOW_TASKS_BUTTON_LIMIT ] =
{
	"LOCKED",
	"UNLOCKED",
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Show_TaskButtonDraw( const U16 aIndex, sGraphicCanvas * apCanvas );
void	Achieve_Show_TaskButtonsDraw( sGraphicCanvas * apCanvas );
void	Achieve_Show_TaskSetButtonIndex( const S16 aIndex );
void	Achieve_Show_LockedTasksDraw( sGraphicCanvas * apCanvas );
void	Achieve_Show_UnLockedTasksDraw( sGraphicCanvas * apCanvas );
void	Achieve_Show_TaskDraw( sGraphicCanvas * apCanvas, U16 aTaskIndex, S16 aY );
void	Achieve_Show_TaskCountDraw( sGraphicCanvas * apCanvas, U16 aCount );
void	Achieve_Show_TaskPointsDraw( sGraphicCanvas * apCanvas, U16 aCount );
void	Achieve_Show_PageInit( void );
void	Achieve_Show_PageUpdate( void );
void	Achieve_Show_SymbolsDraw( sGraphicCanvas * apCanvas );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_AppInit( void )
* ACTION   : Achieve_Show_AppInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_AppInit( void )
{
	Memory_Clear( sizeof(sAchieveShowClass), &gAchieveShowClass );
	gAchieveShowClass.mpFontAss          = AssetClient_Register( "FONTX.BFB",    "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpFont                 );
	gAchieveShowClass.mpSmallFontAss     = AssetClient_Register( "FONT6X6.BFB",  "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpSmallFont            );
	gAchieveShowClass.mpPalAss           = AssetClient_Register( "ACHNEW.PAL",   "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpPal                  );
	gAchieveShowClass.mpButtonAss        = AssetClient_Register( "ACH_BUTN.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpButtonSpr            );
	gAchieveShowClass.mpLockSymbolAss    = AssetClient_Register( "ACH_LOCK.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpLockSymbolSpr        );
	gAchieveShowClass.mpUnLockSymbolAss  = AssetClient_Register( "ACH_UNLK.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpUnLockSymbolSpr      );
	gAchieveShowClass.mpButtonSausageAss = AssetClient_Register( "ACH_BSAS.BSB", "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpButtonSausageSprites );
	gAchieveShowClass.mpLogoAss          = AssetClient_Register( "AWARDZ.BSB",   "ACHIEVE", 0, 0, (void**)&gAchieveShowClass.mpLogoSprites          );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_AppDeInit( void )
* ACTION   : Achieve_Show_AppDeInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_AppDeInit( void )
{
	AssetClient_UnRegister( gAchieveShowClass.mpFontAss          );
	AssetClient_UnRegister( gAchieveShowClass.mpSmallFontAss     );
	AssetClient_UnRegister( gAchieveShowClass.mpPalAss           );
	AssetClient_UnRegister( gAchieveShowClass.mpButtonAss        );
	AssetClient_UnRegister( gAchieveShowClass.mpLockSymbolAss    );
	AssetClient_UnRegister( gAchieveShowClass.mpUnLockSymbolAss  );
	AssetClient_UnRegister( gAchieveShowClass.mpButtonSausageAss );
	AssetClient_UnRegister( gAchieveShowClass.mpLogoAss          );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_Init( void )
* ACTION   : Achieve_Show_Init
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_Init( void )
{
	U16	i;

	gAchieveShowClass.mTaskTopIndex = 0;
	gAchieveShowClass.mTasksRedrawFlag = 2;
	gAchieveShowClass.mBGRedrawFlag = 2;
	for( i=0; i<eACH_SHOW_TASKS_BUTTON_LIMIT; i++ )
	{
		gAchieveShowClass.mButtonRedrawFlags[ i ] = 2;
	}

	if( gAchieveShowClass.mpPal )
	{
		Fade_StartVblFade( gAchieveShowClass.mpPal, 16 );
	}
	Achieve_Show_PageInit();
	Achieve_Show_TaskSetButtonIndex( eACH_SHOW_TASKS_BUTTON_LOCKED );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_DeInit( void )
* ACTION   : Achieve_Show_DeInit
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_DeInit( void )
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_Update( sInput * apInput )
* ACTION   : Achieve_Show_Update
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

U16		Achieve_Show_Update( sInput * apInput )
{
	sGraphicCanvas *	lpLogic;
	sGraphicCanvas *	lpBack;
	sGraphicRect		lRect;
	sGraphicPos			lPos;
	U16					lRes;
	S16					lCount;

	lRes = 0;
	lpBack          = Screen_GetpBackGraphic();
	lpLogic         = Screen_GetpLogicGraphic();

	if( gAchieveShowClass.mBGRedrawFlag )
	{
		if( 2 == gAchieveShowClass.mBGRedrawFlag )
		{
			lpBack->mpFuncs->ClearScreen( lpBack );
			Achieve_Show_TaskButtonsDraw( lpBack );
			lPos.mX = 0;
			lPos.mY = 0;
			lpBack->mpFuncs->DrawSprite( lpBack, &lPos, gAchieveShowClass.mpLogoSprites->mpSprite[0] );
			Achieve_Show_LockedTasksDraw( lpBack );
		}

/*		lpLogic->mpFuncs->CopyScreen( lpLogic, lpBack->mpVRAM ); */
		Memory_Copy( 32000, lpBack->mpVRAM, lpLogic->mpVRAM );
		gAchieveShowClass.mBGRedrawFlag--;
	}

	if( gAchieveShowClass.mTasksRedrawFlag )
	{
		gAchieveShowClass.mTasksRedrawFlag--;

		lRect.mX      = 0;
		lRect.mY      = dACH_SHOW_TASK_Y;
		lRect.mWidth  = 320;
		lRect.mHeight = (dACH_SHOW_TASK_HEIGHT * dACH_SHOW_TASKS_PERPAGE);
		lpLogic->mpFuncs->DrawBox( lpLogic, &lRect, 0 );

		if( gAchieveShowClass.mButtonIndex == eACH_SHOW_TASKS_BUTTON_LOCKED )
		{
			Achieve_Show_LockedTasksDraw( lpLogic );
		}
		else
		{
			Achieve_Show_UnLockedTasksDraw( lpLogic );
		}
	}
	Achieve_Show_PageUpdate();
	Achieve_Show_SymbolsDraw( lpLogic );
	Achieve_Show_TaskButtonsDraw( lpLogic );

	if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HIT )
	{
		Achieve_Show_TaskSetButtonIndex( gAchieveShowClass.mButtonIndex - 1 );
	}
	if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HIT )
	{
		Achieve_Show_TaskSetButtonIndex( gAchieveShowClass.mButtonIndex + 1 );
	}
	if( apInput->mKeyStatus[ eINPUTKEY_UP ] & eINPUTKEYSTATUS_HIT )
	{
		if( gAchieveShowClass.mTaskTopIndex )
		{
			gAchieveShowClass.mTaskTopIndex -= dACH_SHOW_TASKS_PERPAGE;
			gAchieveShowClass.mTasksRedrawFlag = 2;
			Achieve_Show_PageInit();
		}
	}
	if( apInput->mKeyStatus[ eINPUTKEY_DOWN ] & eINPUTKEYSTATUS_HIT )
	{
		lCount = 0;
		switch( gAchieveShowClass.mButtonIndex )
		{
		case eACH_SHOW_TASKS_BUTTON_LOCKED:
			lCount = Achieve_Tasks_GetLockedCount();
			break;
		case eACH_SHOW_TASKS_BUTTON_UNLOCKED:
			lCount = Achieve_Tasks_GetUnLockedCount();
			break;
		}
		if( (gAchieveShowClass.mTaskTopIndex + dACH_SHOW_TASKS_PERPAGE) < lCount )
		{
			gAchieveShowClass.mTaskTopIndex += dACH_SHOW_TASKS_PERPAGE;
			gAchieveShowClass.mTasksRedrawFlag = 2;
			Achieve_Show_PageInit();
		}
	}

	if( apInput->mKeyStatus[ eINPUTKEY_FIREA ] & eINPUTKEYSTATUS_HIT )
	{
		lRes = 1;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskButtonDraw( const U16 aIndex, sGraphicCanvas * apCanvas )
* ACTION   : Achieve_Show_TaskButtonDraw
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskButtonDraw( const U16 aIndex, sGraphicCanvas * apCanvas )
{
	U16	lCol;
	sGraphicPos	lPos;
	sGraphicRect	lRect;

	if( gAchieveShowClass.mButtonSelectedFlags[ aIndex ] )
	{
		lCol = 0;
	}
	else
	{
		lCol = 1;
	}

	lPos.mX = gAchieveShowTaskButtonRects[ aIndex ].mX;
	lPos.mY = gAchieveShowTaskButtonRects[ aIndex ].mY-3;
	apCanvas->mpFuncs->DrawSprite( apCanvas, &lPos, gAchieveShowClass.mpButtonSausageSprites->mpSprite[ lCol ] );


	lRect = gAchieveShowTaskButtonRects[ aIndex ];
	if( gAchieveShowClass.mButtonSelectedFlags[ aIndex ] )
	{
		lRect.mY ++;
	}
	Graphic_FontPrintCentred( apCanvas, gAchieveShowClass.mpFont, &lRect, gAchieveShowTaskButtonTitles[ aIndex ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskButtonsDraw( sGraphicCanvas * apCanvas )
* ACTION   : Achieve_Show_TaskButtonsDraw
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskButtonsDraw( sGraphicCanvas * apCanvas )
{
	U16 i;

	for( i=0; i<eACH_SHOW_TASKS_BUTTON_LIMIT; i++ )
	{
		if( gAchieveShowClass.mButtonRedrawFlags[ i ] )
		{
			gAchieveShowClass.mButtonRedrawFlags[ i ]--;
			Achieve_Show_TaskButtonDraw( i, apCanvas );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskSetButtonIndex( const S16 aIndex )
* ACTION   : Achieve_Show_TaskSetButtonIndex
* CREATION : 15.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskSetButtonIndex( const S16 aIndex )
{
	if( (aIndex>=0) && (aIndex<eACH_SHOW_TASKS_BUTTON_LIMIT) )
	{
		gAchieveShowClass.mButtonSelectedFlags[ gAchieveShowClass.mButtonIndex ] = 0;
		gAchieveShowClass.mButtonRedrawFlags[ gAchieveShowClass.mButtonIndex ]   = 2;

		gAchieveShowClass.mButtonIndex     = aIndex;

		gAchieveShowClass.mButtonSelectedFlags[ gAchieveShowClass.mButtonIndex ] = 1;
		gAchieveShowClass.mButtonRedrawFlags[ gAchieveShowClass.mButtonIndex ]   = 2;
		gAchieveShowClass.mTasksRedrawFlag = 2;
		gAchieveShowClass.mTaskTopIndex    = 0;
		Achieve_Show_PageInit();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_LockedTasksDraw( sGraphicCanvas * apCanvas )
* ACTION   : Achieve_Show_LockedTasksDraw
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_LockedTasksDraw( sGraphicCanvas * apCanvas )
{
	U16	lCount;
	U16	i;
	U16	lIndex;

	lCount = Achieve_Tasks_GetCount();
	lIndex = gAchieveShowClass.mTaskTopIndex;
	i      = 0;

	while( (i<dACH_SHOW_TASKS_PERPAGE) && (lIndex<lCount) )
	{
		if( !Achieve_Task_IsUnLocked(lIndex) )
		{
			gAchieveShowClass.mSymbolTypes[ i ] = eACH_SHOW_SYMBOL_LOCKED;
			Achieve_Show_TaskDraw( apCanvas, lIndex, dACH_SHOW_TASK_Y + (i * dACH_SHOW_TASK_HEIGHT) );
			i++;
		}
		lIndex++;
	}
	Achieve_Show_TaskCountDraw( apCanvas, Achieve_Tasks_GetLockedCount() );
	Achieve_Show_TaskPointsDraw( apCanvas, (U16)(Achieve_Tasks_GetPointsTotal() - Achieve_Tasks_GetPointsUnLocked()) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_UnLockedTasksDraw( sGraphicCanvas * apCanvas )
* ACTION   : Achieve_Show_UnLockedTasksDraw
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_UnLockedTasksDraw( sGraphicCanvas * apCanvas )
{
	U16	lCount;
	U16	i;
	U16	lIndex;

	lCount = Achieve_Tasks_GetCount();
	lIndex = gAchieveShowClass.mTaskTopIndex;
	i      = 0;

	while( (i<dACH_SHOW_TASKS_PERPAGE) && (lIndex<lCount) )
	{
		if( Achieve_Task_IsUnLocked(lIndex) )
		{
			gAchieveShowClass.mSymbolTypes[ i ] = eACH_SHOW_SYMBOL_UNLOCKED;
			Achieve_Show_TaskDraw( apCanvas, lIndex, dACH_SHOW_TASK_Y + (i * dACH_SHOW_TASK_HEIGHT) );
			i++;
		}
		lIndex++;
	}
	Achieve_Show_TaskCountDraw(  apCanvas, Achieve_Tasks_GetUnLockedCount()       );
	Achieve_Show_TaskPointsDraw( apCanvas, (U16)Achieve_Tasks_GetPointsUnLocked() );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskDraw( sGraphicCanvas * apCanvas, U16 aTaskIndex, S16 aY )
* ACTION   : Achieve_Show_TaskDraw
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskDraw( sGraphicCanvas * apCanvas, U16 aTaskIndex, S16 aY )
{
	sGraphicRect	lRect;
	sGraphicPos		lPos;
	sSpriteBlock *	lpSprBlk;

	lRect.mX      = 0;
	lRect.mY      = aY;
	lRect.mWidth  = 320;
	lRect.mHeight = dACH_SHOW_TASK_HEIGHT;

	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, 0 );

	lPos.mX = 0;
	lPos.mY = aY;

	Achieve_Show_ButtonRender( apCanvas, &lPos, aTaskIndex );


	if( Achieve_Task_IsUnLocked(aTaskIndex) )
	{
		lpSprBlk = gAchieveShowClass.mpUnLockSymbolSpr;
	}
	else
	{
		lpSprBlk = gAchieveShowClass.mpLockSymbolSpr;
	}

	lPos.mX = 7;
	lPos.mY = aY + 3;
	apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, lpSprBlk->mpSprite[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskCountDraw( sGraphicCanvas * apCanvas, U16 aCount )
* ACTION   : Achieve_Show_TaskCountDraw
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskCountDraw( sGraphicCanvas * apCanvas, U16 aCount )
{
	sGraphicRect	lRect;
	char	lString[ 32 ];

	sprintf( lString, "(%d/%d)", aCount, Achieve_Tasks_GetCount() );
	lRect.mX      = dACH_SHOW_TASK_COUNT_X;
	lRect.mY      = dACH_SHOW_TASK_COUNT_Y;
	lRect.mWidth  = dACH_SHOW_TASK_COUNT_W;
	lRect.mHeight = dACH_SHOW_TASK_COUNT_H;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, 0 );
	Graphic_FontPrintCentred( apCanvas, gAchieveShowClass.mpSmallFont, &lRect, lString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_PageInit( void )
* ACTION   : Achieve_Show_PageInit
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_PageInit( void )
{
	U16	i;

	for( i=0; i<dACH_SHOW_TASKS_PERPAGE; i++ )
	{
		gAchieveShowClass.mSymbolFrames[ i ].w.w1 = 0;
		gAchieveShowClass.mSymbolFrames[ i ].w.w0 = (U16)((0x10000L / dACH_SHOW_TASKS_PERPAGE) * i);
		gAchieveShowClass.mSymbolRedrawFlags[ i ] = 2;
		gAchieveShowClass.mSymbolTypes[ i ]       = eACH_SHOW_SYMBOL_NONE;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_PageUpdate( void )
* ACTION   : Achieve_Show_PageUpdate
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_PageUpdate( void )
{
	U16	i;
	U16	lOldFrame;

	for( i=0; i<dACH_SHOW_TASKS_PERPAGE; i++ )
	{
		lOldFrame = gAchieveShowClass.mSymbolFrames[ i ].w.w1;
		gAchieveShowClass.mSymbolFrames[ i ].l += dACH_SHOW_BUTTON_ANIMSPEED;
		if( lOldFrame != gAchieveShowClass.mSymbolFrames[ i ].w.w1 )
		{
			gAchieveShowClass.mSymbolRedrawFlags[ i ] = 2;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_SymbolsDraw( sGraphicCanvas * apCanvas )
* ACTION   : Achieve_Show_SymbolsDraw
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_SymbolsDraw( sGraphicCanvas * apCanvas )
{
	U16	lCount;
	U16	i;
	U16	lIndex;
	U16	lFrame;
	sGraphicPos	lPos;
	sGraphicRect	lRect;
	sSpriteBlock *	lpSprBlk;

	lCount  = Achieve_Tasks_GetCount();
	lIndex  = gAchieveShowClass.mTaskTopIndex;
	i       = 0;
	lPos.mX = 7;
	lPos.mY = dACH_SHOW_TASK_Y + 3;

	while( (i<dACH_SHOW_TASKS_PERPAGE) && (lIndex<lCount) )
	{
		if( gAchieveShowClass.mSymbolRedrawFlags[ i ] )
		{
			gAchieveShowClass.mSymbolRedrawFlags[ i ]--;

			if( eACH_SHOW_SYMBOL_NONE == gAchieveShowClass.mSymbolTypes[ i ] )
			{
				lRect.mX = lPos.mX;
				lRect.mY = lPos.mY;
				lRect.mWidth = 18;
				lRect.mHeight = 18;
				apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, 0 );
			}
			else
			{
				if( eACH_SHOW_SYMBOL_UNLOCKED == gAchieveShowClass.mSymbolTypes[ i ] )
				{
					lpSprBlk = gAchieveShowClass.mpUnLockSymbolSpr;
				}
				else
				{
					lpSprBlk = gAchieveShowClass.mpLockSymbolSpr;
				}
				if( gAchieveShowClass.mSymbolFrames[ i ].w.w1 >= lpSprBlk->mHeader.mSpriteCount )
				{
					gAchieveShowClass.mSymbolFrames[ i ].w.w1 = 0;
				}
				lFrame = gAchieveShowClass.mSymbolFrames[ i ].w.w1;

				apCanvas->mpFuncs->DrawSprite( apCanvas, &lPos, lpSprBlk->mpSprite[ lFrame ] );
			}
		}
		lPos.mY += dACH_SHOW_TASK_HEIGHT;
		lIndex++;
		i++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_ButtonRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, const U16 aTaskIndex )
* ACTION   : Achieve_Show_ButtonRender
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_ButtonRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, const U16 aTaskIndex )
{
	sGraphicPos	lPos;
	char		lString[ 32 ];

	lPos = *apPos;

	lPos.mX = 32;
	lPos.mY = apPos->mY;
	apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, gAchieveShowClass.mpButtonSpr->mpSprite[ 0 ] );

	lPos.mX = 33;
	lPos.mY = apPos->mY + 2;
	apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, gAchieveShowClass.mpSmallFont, Achieve_Tasks_GetpTitle(aTaskIndex) );

	sprintf( lString, "%ld", Achieve_Task_GetPoints( aTaskIndex ) );
	lPos.mX = 32 + 283 - 2 - Font_GetStringWidth( gAchieveShowClass.mpSmallFont, lString );
	lPos.mY = apPos->mY + 2;
	apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, gAchieveShowClass.mpSmallFont, lString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_SymbolRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, uU32 * apFrame, const U16 aTaskIndex )
* ACTION   : Achieve_Show_SymbolRender
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_SymbolRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, uU32 * apFrame, const U16 aTaskIndex )
{
	sSpriteBlock *	lpBlock;

	if( aTaskIndex < Achieve_Tasks_GetCount() )
	{
		if( Achieve_Task_IsUnLocked(aTaskIndex) )
		{
			lpBlock = gAchieveShowClass.mpUnLockSymbolSpr;
		}
		else
		{
			lpBlock = gAchieveShowClass.mpLockSymbolSpr;
		}
		if( apFrame->w.w1 > lpBlock->mHeader.mSpriteCount )
		{
			apFrame->w.w1 = 0;
		}
		apCanvas->mpFuncs->DrawSprite( apCanvas, apPos, lpBlock->mpSprite[ apFrame->w.w1 ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Show_TaskPointsDraw( sGraphicCanvas * apCanvas, U16 aCount )
* ACTION   : Achieve_Show_TaskPointsDraw
* CREATION : 19.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Show_TaskPointsDraw( sGraphicCanvas * apCanvas, U16 aCount )
{
	sGraphicRect	lRect;
	char	lString[ 32 ];

	sprintf( lString, "(%d/%ld)", aCount, Achieve_Tasks_GetPointsTotal() );
	lRect.mX      = dACH_SHOW_TASK_POINTS_X;
	lRect.mY      = dACH_SHOW_TASK_POINTS_Y;
	lRect.mWidth  = dACH_SHOW_TASK_POINTS_W;
	lRect.mHeight = dACH_SHOW_TASK_POINTS_H;
	apCanvas->mpFuncs->DrawBox( apCanvas, &lRect, 0 );
	Graphic_FontPrintCentred( apCanvas, gAchieveShowClass.mpSmallFont, &lRect, lString );
}


/* ################################################################################ */

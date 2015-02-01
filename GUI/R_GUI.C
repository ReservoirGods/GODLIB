/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"R_GUI.H"

#include	"GUI.H"
#include	"GUIDATA.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/GUI/GUIEDIT.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SPRITE/SPRITE.H>
#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

#define	dRGUI_MOUSE_HEIGHT		16
#define	dRGUI_CURSOR_RATEON		10
#define	dRGUI_CURSOR_RATEOFF	1
#define	dRGUI_CURSOR_COLOUR		1


typedef	struct	sRGuiMouseBlock
{
	U32 *	mpScreen;
	S16		mHeight;
	U32		mPlanes[ dRGUI_MOUSE_HEIGHT ][ 4 ];
} sRGuiMouseBlock;

typedef	struct	sRGuiCursor
{
	U16		mFlashMode;
	S16		mFlashRateOn;
	S16		mFlashRateOff;
	S16		mFlashCounter;
	U16		mColour;
} sRGuiCursor;

typedef	struct	sRenderGui
{
	U16				mScreenSwapFlag;
	U16				mScreenIndex;
	fRenderGui		mpCustomRenderer;
	sRGuiCursor		mCursor;
	sRGuiMouseBlock	mMouseBlocks[ 2 ];
} sRenderGui;


/* ###################################################################################
#  DATA
################################################################################### */

sRenderGui	gRenderGui;
char		gRGuiString[ 128 ];

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	RenderGui_ButtonDraw( sGuiButton * apButton, sGraphicCanvas * apCanvas, sGuiButtonStyle * apStyle, sGuiFontGroup * apFontGroup, U16 aRedrawFlag );
void	RenderGui_SliderDraw( sGuiSlider * apSlider, sGraphicCanvas * apCanvas, sGuiButtonStyle * apStyle, sGuiFontGroup * apFontGroup, U16 aRedrawFlag );
void	RenderGui_SpriteDraw( sGuiSprite * apSprite, sGraphicCanvas * apCanvas, U32 aFlags, U16 aRedrawFlag );
void	RenderGui_StringDraw( sGuiString * apString, sGraphicCanvas * apCanvas, sGuiFontGroup * apFontGroup, U32 aFlags );
void	RenderGui_WindowDraw( sGuiWindow * apWindow, sGraphicCanvas * apCanvas, U16 aRedrawFlag );
void	RenderGui_FillDraw( sGuiFill * apFill, sGraphicCanvas * apCanvas, sGuiRect * apRect );
void	RenderGui_MouseSave( sRGuiMouseBlock * apBlock, sGraphicCanvas * apCanvas, sGuiMouse * apMouse );
void	RenderGui_MouseRestore( sRGuiMouseBlock * apBlock );
void	RenderGui_MouseDraw( sGuiMouse * apMouse, sGraphicCanvas * apCanvas );
U8		RenderGui_SetClipBox( sGraphicCanvas * apCanvas, const sGuiRect * apRect );
void	RenderGui_CursorDraw( sGraphicCanvas * apCanvas, const S16 aX, const S16 aY, const S16 aHeight );
void	RenderGui_CursorUpdate( sRGuiCursor * apCursor );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_Init( void )
* ACTION   : RenderGui_Init
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_Init( void )
{
	Memory_Clear( sizeof(gRenderGui), &gRenderGui );
	gRenderGui.mScreenIndex          = 0;
	gRenderGui.mScreenSwapFlag       = 1;
	gRenderGui.mCursor.mColour       = dRGUI_CURSOR_COLOUR;
	gRenderGui.mCursor.mFlashRateOn  = dRGUI_CURSOR_RATEON;
	gRenderGui.mCursor.mFlashRateOff = dRGUI_CURSOR_RATEON;
	gRenderGui.mCursor.mFlashCounter = 0;
	gRenderGui.mCursor.mFlashMode    = 0;
	gRenderGui.mMouseBlocks[ 0 ].mpScreen = 0;
	gRenderGui.mMouseBlocks[ 1 ].mpScreen = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_DeInit( void )
* ACTION   : RenderGui_DeInit
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_DeInit( void )
{
	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_Update( sGraphicCanvas * apLogic )
* ACTION   : RenderGui_Update
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_Update( sGraphicCanvas * apLogic )
{
	sGuiMouse *		lpMouse;
	U16				lWindowCount;
	U16				lRedrawFlag;
	U16				i;
	sGuiWindow **	lppWindows;
	sGraphicBox		lClipBox;

	lClipBox.mX0 = 0;
	lClipBox.mY0 = 0;
	lClipBox.mX1 = apLogic->mWidth;
	lClipBox.mY1 = apLogic->mHeight;

	if( gRenderGui.mScreenSwapFlag )
	{
		gRenderGui.mScreenIndex ^= 1;
	}

	RenderGui_MouseRestore( &gRenderGui.mMouseBlocks[ gRenderGui.mScreenIndex ] );

	lWindowCount = Gui_GetWindowStackCount();
	lppWindows   = Gui_GetpWindowStack();

	lRedrawFlag = 0;
	for( i=0; i<lWindowCount; i++ )
	{
		lRedrawFlag |= lppWindows[ i ]->mInfo.mRedrawFlag;
	}

	if( lRedrawFlag )
	{
		apLogic->mpClipFuncs->ClearScreen( apLogic );
	}

	for( i=0; i<lWindowCount; i++ )
	{
		apLogic->mClipBox = lClipBox;
		RenderGui_WindowDraw( lppWindows[ i ], apLogic, lRedrawFlag );
	}

	apLogic->mClipBox = lClipBox;
	lpMouse = Gui_GetpMouse();
	RenderGui_CursorUpdate( &gRenderGui.mCursor );
	RenderGui_MouseSave( &gRenderGui.mMouseBlocks[ gRenderGui.mScreenIndex ], apLogic, lpMouse );
	RenderGui_MouseDraw( lpMouse, apLogic );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_WindowDraw( sGuiWindow * apWindow,sGraphicCanvas * apCanvas,U16 aRedrawFlag )
* ACTION   : RenderGui_WindowDraw
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_WindowDraw( sGuiWindow * apWindow,sGraphicCanvas * apCanvas,U16 aRedrawFlag )
{
	U16			lRedrawFlag;
	U16			i;
	sGraphicBox	lBox;

	if( apWindow )
	{
/*		DebugLog_Printf3( "RenderGui_WindowDraw() %lx %s cc:%d", apWindow, apWindow->mInfo.mpName, apWindow->mControlCount );
		DebugLog_Printf4( "windowcanvas x:%d y:%d w:%d h:%d", apWindow->mCanvas.mX, apWindow->mCanvas.mY, apWindow->mCanvas.mWidth, apWindow->mCanvas.mHeight );*/
		if( RenderGui_SetClipBox( apCanvas, &apWindow->mInfo.mRectPair.mWorld ) )
		{
			lRedrawFlag = (U16)(aRedrawFlag | apWindow->mInfo.mRedrawFlag);
			if( lRedrawFlag )
			{
				RenderGui_FillDraw( apWindow->mpFill, apCanvas, &apWindow->mInfo.mRectPair.mWorld );
			}

			lBox = apCanvas->mClipBox;
			lBox.mX0 += 2;
			lBox.mX1 -= 2;
			lBox.mY0 += 2;
			lBox.mY1 -= 2;

			if( (lBox.mX0 < lBox.mX1) && (lBox.mY0 < lBox.mY1) )
			{
				for( i=0; i<apWindow->mControlCount; i++ )
				{
					apCanvas->mClipBox = lBox;
					switch( apWindow->mppControls[ i ]->mType )
					{
					case	eGUI_TYPE_BUTTON:
						RenderGui_ButtonDraw( (sGuiButton*)apWindow->mppControls[ i ], apCanvas, apWindow->mpButtonStyle, apWindow->mpFontGroup, lRedrawFlag );
						break;
					case	eGUI_TYPE_SLIDER:
						RenderGui_SliderDraw( (sGuiSlider*)apWindow->mppControls[ i ], apCanvas, apWindow->mpButtonStyle, apWindow->mpFontGroup, lRedrawFlag );
						break;
					case	eGUI_TYPE_WINDOW:
						RenderGui_WindowDraw( (sGuiWindow*)apWindow->mppControls[ i ], apCanvas, lRedrawFlag );
						break;
					}
				}
			}
			if( apWindow->mInfo.mRedrawFlag )
			{
				apWindow->mInfo.mRedrawFlag--;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_ButtonDraw( sGuiButton * apButton,sGraphicCanvas * apCanvas,sGuiButtonStyle * apStyle,sGuiFontGroup * apFontGroup,U16 aRedrawFlag )
* ACTION   : RenderGui_ButtonDraw
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_ButtonDraw( sGuiButton * apButton,sGraphicCanvas * apCanvas,sGuiButtonStyle * apStyle,sGuiFontGroup * apFontGroup,U16 aRedrawFlag )
{
	sGuiButtonStyle *	lpStyle;
	U16					lRedrawFlag;

	if( apButton )
	{
/*		DebugLog_Printf2( "RenderGui_ButtonDraw() x:%d y:%d", apButton->mInfo.mRectPair.mWorld.mX,  apButton->mInfo.mRectPair.mWorld.mY );*/
		if( RenderGui_SetClipBox( apCanvas, &apButton->mInfo.mRectPair.mWorld ) )
		{
			if( !(apButton->mInfo.mFlags & eGUIINFO_FLAG_INVISIBLE) )
			{
				lRedrawFlag = (U16)(apButton->mInfo.mRedrawFlag | aRedrawFlag);

				if( apButton->mpStyle )
				{
					lpStyle = apButton->mpStyle;
				}
				else
				{
					lpStyle = apStyle;
				}

				if( eGUI_BUTTON_CUSTOM == apButton->mButtonType )
				{
					if( gRenderGui.mpCustomRenderer )
					{
						gRenderGui.mpCustomRenderer( apCanvas, apButton, lRedrawFlag );
					}
				}
				else if( lRedrawFlag )
				{
					{
						if( apButton->mInfo.mFlags & eGUIINFO_FLAG_LOCKED ) 
						{
							RenderGui_FillDraw( lpStyle->mpFillLocked, apCanvas, &apButton->mInfo.mRectPair.mWorld );
						}
						else if( apButton->mInfo.mFlags & eGUIINFO_FLAG_SELECTED ) 
						{
							RenderGui_FillDraw( lpStyle->mpFillSelected, apCanvas, &apButton->mInfo.mRectPair.mWorld );
						}
						else
						{
							RenderGui_FillDraw( lpStyle->mpFillNormal, apCanvas, &apButton->mInfo.mRectPair.mWorld );
						}
						RenderGui_SpriteDraw( &apButton->mSprite, apCanvas, apButton->mInfo.mFlags, lRedrawFlag );
						RenderGui_StringDraw( &apButton->mString, apCanvas, apFontGroup, apButton->mInfo.mFlags );

						if( apButton->mString.mVar.mpName )
						{
							DebugLog_Printf2( "RenderGui_ButtonDraw() %s %s", apButton->mInfo.mpName, apButton->mString.mVar.mpName );
						}
					}
				}
			}
			if( apButton->mInfo.mRedrawFlag )
			{
				apButton->mInfo.mRedrawFlag--;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_FillDraw( sGuiFill * apFill,sGraphicCanvas * apCanvas,sGuiRect * apRect )
* ACTION   : RenderGui_FillDraw
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_FillDraw( sGuiFill * apFill,sGraphicCanvas * apCanvas,sGuiRect * apRect )
{
	sGraphicRect	lRect;
	sGuiColour *	lpColour;


	if( (apFill) && (apRect) )
	{

		lpColour = apFill->mpColours[ eGUI_FILLCOLOUR_MAIN ];
		if( lpColour )
		{
			lRect.mX      = (S16)(apRect->mX + 2);
			lRect.mY      = (S16)(apRect->mY + 2);
			lRect.mWidth  = (S16)(apRect->mWidth - 2);
			lRect.mHeight = (S16)(apRect->mHeight - 2);

			if( (lRect.mWidth > 0) && (lRect.mHeight>0) )
			{
				apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );
			}
		}

		lpColour = apFill->mpColours[ eGUI_FILLCOLOUR_TL0 ];
		if( lpColour )
		{
			lRect.mX      = apRect->mX;
			lRect.mY      = apRect->mY;
			lRect.mWidth  = apRect->mWidth;
			lRect.mHeight = 1;

			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );

			lRect.mWidth  = 1;
			lRect.mHeight = apRect->mHeight;
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );
		}

		lpColour = apFill->mpColours[ eGUI_FILLCOLOUR_TL1 ];
		if( lpColour )
		{
			lRect.mX      = (S16)(apRect->mX + 1);
			lRect.mY      = (S16)(apRect->mY + 1);
			lRect.mWidth  = (S16)(apRect->mWidth - 2);
			lRect.mHeight = 1;
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );

			lRect.mWidth  = 1;
			lRect.mHeight = (S16)(apRect->mHeight - 2);
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );
		}

		lpColour = apFill->mpColours[ eGUI_FILLCOLOUR_BR0 ];
		if( lpColour )
		{
			lRect.mX      = (S16)(apRect->mX + apRect->mWidth -1);
			lRect.mY      = (S16)(apRect->mY + 1);
			lRect.mWidth  = 1;
			lRect.mHeight = (S16)(apRect->mHeight - 1);
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );

			lRect.mX      = (S16)(apRect->mX + 1);
			lRect.mY      = (S16)(apRect->mY + apRect->mHeight - 1);
			lRect.mWidth  = (S16)(apRect->mWidth - 1);
			lRect.mHeight = 1;
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );
		}

		lpColour = apFill->mpColours[ eGUI_FILLCOLOUR_BR1 ];
		if( lpColour )
		{
			lRect.mX      = (S16)(apRect->mX + apRect->mWidth -2);
			lRect.mY      = (S16)(apRect->mY + 2);
			lRect.mWidth  = 1;
			lRect.mHeight = (S16)(apRect->mHeight - 2);
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );

			lRect.mX      = (S16)(apRect->mX + 2);
			lRect.mY      = (S16)(apRect->mY + apRect->mHeight - 2);
			lRect.mWidth  = (S16)(apRect->mWidth - 2);
			lRect.mHeight = 1;
			apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, lpColour->mPalIndex );
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_SliderDraw( sGuiSlider * apSlider,sGraphicCanvas * apCanvas,sGuiButtonStyle * apStyle,sGuiFontGroup * apFontGroup,U16 aRedrawFlag )
* ACTION   : RenderGui_SliderDraw
* CREATION : 21.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_SliderDraw( sGuiSlider * apSlider,sGraphicCanvas * apCanvas,sGuiButtonStyle * apStyle,sGuiFontGroup * apFontGroup,U16 aRedrawFlag )
{
	U16			i;
	U16			lRedrawFlag;
	sGraphicBox	lBox;

	if( apSlider )
	{
		lBox = apCanvas->mClipBox;
		lRedrawFlag = (S16)(aRedrawFlag | apSlider->mInfo.mRedrawFlag);
		for( i=0; i<eGUI_SLIDERBUT_LIMIT; i++ )
		{
			apCanvas->mClipBox = lBox;
			RenderGui_ButtonDraw( apSlider->mpButtons[ i ], apCanvas, apStyle, apFontGroup, lRedrawFlag );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_SpriteDraw( sGuiSprite * apSprite,sGraphicCanvas * apCanvas,U16 aFlags,U16 aRedrawFlag )
* ACTION   : RenderGui_SpriteDraw
* CREATION : 14.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_SpriteDraw( sGuiSprite * apSprite,sGraphicCanvas * apCanvas,U32 aFlags,U16 aRedrawFlag )
{
	sSpriteBlock *	lpBlock;
	sGraphicPos		lPos;
	U16				lIndex;

	if( apSprite )
	{
		if( apSprite->mRedrawFlag || aRedrawFlag )
		{
			if( apSprite->mpAsset )
			{
				lpBlock = (sSpriteBlock*)apSprite->mpAsset->mpData;
				if( lpBlock )
				{
					lPos.mX = apSprite->mRectPair.mWorld.mX;
					lPos.mY = apSprite->mRectPair.mWorld.mY;

					lIndex = 0;
					if( aFlags & eGUIINFO_FLAG_SELECTED )
					{
						lPos.mX += 1;
						lPos.mY += 1;
						lIndex  = 1;
					}
					if( aFlags & eGUIINFO_FLAG_LOCKED )
					{
						lIndex = 2;
					}
					if( lIndex >= lpBlock->mHeader.mSpriteCount )
					{
						lIndex = 0;
					}
					apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, lpBlock->mpSprite[ lIndex ] );
				}
			}
			if( apSprite->mRedrawFlag )
			{
				apSprite->mRedrawFlag--;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_StringDraw( sGuiString * apString,sGraphicCanvas * apCanvas,sGuiFontGroup * apFontGroup,U32 aFlags )
* ACTION   : RenderGui_StringDraw
* CREATION : 17.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_StringDraw( sGuiString * apString,sGraphicCanvas * apCanvas,sGuiFontGroup * apFontGroup,U32 aFlags )
{
	sGuiAsset *		lpAsset;
	sGuiVar *		lpVar;
	sGuiFontGroup *	lpFontGroup;
	sFont *			lpFont;
	sGraphicPos		lPos;
	sString *		lpString;
	sGuiTextLine *	lpTextLine;
	char *			lpChars;
	char			lChars[ 128 ];
	S8				lS8;
	U8				lU8;
	S16				lS16;
	U16				lU16;
	S32				lS32;
	U32				lU32;
	FP32			lFP32;
	U16				lWidth;


	if( apString )
	{
		if( apString->mpFontGroup )
		{
			lpFontGroup = apString->mpFontGroup;
		}
		else
		{
			lpFontGroup = apFontGroup;
		}

		lpChars  = 0;
		lpString = 0;

		lpVar = &apString->mVar;
		if( lpVar->mpVar )
		{
			if( apString->mVar.mpName )
			{
				DebugLog_Printf2( "RenderGui_StringDraw() %s type:%d", apString->mVar.mpName, lpVar->mType );
			}
			switch( lpVar->mType )
			{

			case	eGUI_VAR_S8:
				HashTree_VarRead( lpVar->mpVar, &lS8, sizeof(lS8) );
				sprintf( lChars, "%d", lS8 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_S16:
				HashTree_VarRead( lpVar->mpVar, &lS16, sizeof(lS16) );
				sprintf( lChars, "%d", lS16 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_S32:
				HashTree_VarRead( lpVar->mpVar, &lS32, sizeof(lS32) );
				sprintf( lChars, "%ld", lS32 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_U8:
				HashTree_VarRead( lpVar->mpVar, &lU8, sizeof(lU8) );
				sprintf( lChars, "%d", lU8 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_U16:
				HashTree_VarRead( lpVar->mpVar, &lU16, sizeof(lU16) );
				sprintf( lChars, "%d", lU16 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_U32:
				HashTree_VarRead( lpVar->mpVar, &lU32, sizeof(lU32) );
				sprintf( lChars, "%ld", lU32 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_FP32:
				HashTree_VarRead( lpVar->mpVar, &lFP32, sizeof(lFP32) );
				sprintf( lChars, "%f", lFP32 );
				lpChars = &lChars[ 0 ];
				break;

			case	eGUI_VAR_STRING:
				HashTree_VarRead( lpVar->mpVar, &lpString, sizeof(lpString) );
				if( lpString )
				{
					lpChars = lpString->mpChars;
				}
				break;
			}
		}
		else
		{
			lpChars = apString->mpTitle;
		}


		if( lpChars && lpFontGroup )
		{

			if( aFlags & eGUIINFO_FLAG_SELECTED )
			{
				lpAsset = lpFontGroup->mpSelected;
			}
			else if( aFlags & eGUIINFO_FLAG_LOCKED )
			{
				lpAsset = lpFontGroup->mpLocked;
			}
			else
			{
				lpAsset = lpFontGroup->mpNormal;
			}
			if( lpAsset )
			{
				lpFont = (sFont*)lpAsset->mpData;
				if( lpAsset->mpData )
				{

					switch( (U16)apString->mAlign.mH )
					{
					case	eGUI_ALIGNH_CENTRE:
						lWidth  = Font_GetStringWidth( lpFont, lpChars );
						lPos.mX = (S16)(apString->mRects.mWorld.mX + (apString->mRects.mWorld.mWidth>>1));
						lPos.mX = (S16)(lPos.mX - (lWidth>>1));
						break;
					case	eGUI_ALIGNH_RIGHT:
						lWidth  = Font_GetStringWidth( lpFont, lpChars );
						lPos.mX = (S16)(apString->mRects.mWorld.mX + apString->mRects.mWorld.mWidth - 2);
						lPos.mX = (S16)(lPos.mX - lWidth);
						break;
					case	eGUI_ALIGNH_LEFT:
					default:
						lPos.mX = (S16)(apString->mRects.mWorld.mX + 2);
						break;
					}

					switch( (U16)apString->mAlign.mV )
					{
					case	eGUI_ALIGNV_CENTRE:
						lPos.mY = (S16)(apString->mRects.mWorld.mY + (apString->mRects.mWorld.mHeight>>1));
						lPos.mY = (S16)(lPos.mY - (lpFont->mHeightMax>>1));
						break;
					case	eGUI_ALIGNV_BOTTOM:
						lPos.mY = (S16)(apString->mRects.mWorld.mY + apString->mRects.mWorld.mHeight);
						lPos.mY = (S16)(lPos.mY - (lpFont->mHeightMax+2));
						break;
					case	eGUI_ALIGNV_TOP:
						lPos.mY = (S16)(apString->mRects.mWorld.mY + 2);
						break;
					}
					
					apCanvas->mpClipFuncs->FontPrint( apCanvas, &lPos, lpFont, lpChars );

					if( lpString )
					{
						lpTextLine = Gui_GetpTextLine();
						if( (lpString == lpTextLine->mpString) && (lpTextLine->mEditFlag) )
						{
							RenderGui_CursorDraw( 
								apCanvas,
								(S16)(lPos.mX + Font_GetStringCharX( lpFont, lpChars, lpTextLine->mCursorX )),
								lPos.mY,
								lpFont->mHeightMax );
						}
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_MouseSave( sRGuiMouseBlock * apBlock,sGraphicCanvas * apCanvas,sGuiMouse * apMouse )
* ACTION   : RenderGui_MouseSave
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_MouseSave( sRGuiMouseBlock * apBlock,sGraphicCanvas * apCanvas,sGuiMouse * apMouse )
{
	U32 *	lpSrc;
	U32 *	lpDst;
	U16		lOff;
	S16		i;

	lOff = apMouse->mX;
	if( lOff > (apCanvas->mWidth - 32) )
	{
		lOff = (S16)(apCanvas->mWidth - 32);
	}

	lpSrc  =  (U32*)apCanvas->mpVRAM;
	lpSrc += ((lOff>>4)<<1);
	lpSrc += (apMouse->mY * 40);

	lpDst = (U32*)&apBlock->mPlanes[ 0 ][ 0 ];

	apBlock->mHeight  = (S16)(apCanvas->mHeight - apMouse->mY);
	apBlock->mpScreen = lpSrc;

	if( apBlock->mHeight > dRGUI_MOUSE_HEIGHT )
	{
		apBlock->mHeight = dRGUI_MOUSE_HEIGHT;
	}

	
	if( apBlock->mHeight > 0 )
	{
		for( i=0; i<apBlock->mHeight; i++ )
		{
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			lpSrc += (40-4);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_MouseRestore( sRGuiMouseBlock * apBlock )
* ACTION   : RenderGui_MouseRestore
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_MouseRestore( sRGuiMouseBlock * apBlock )
{
	S16		i;
	U32 *	lpSrc;
	U32 *	lpDst;

	if( (apBlock->mpScreen)	&& (apBlock->mHeight > 0) )
	{
		lpSrc = &apBlock->mPlanes[ 0 ][ 0 ];
		lpDst = apBlock->mpScreen;
		for( i=0; i<apBlock->mHeight; i++ )
		{
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			lpDst += (40-4);
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_MouseDraw( sGuiMouse * apMouse,sGraphicCanvas * apCanvas )
* ACTION   : RenderGui_MouseDraw
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_MouseDraw( sGuiMouse * apMouse,sGraphicCanvas * apCanvas )
{
	sGraphicPos		lPos;
	sSpriteBlock *	lpBlock;


	apCanvas->mClipBox.mX0 = 0;
	apCanvas->mClipBox.mY0 = 0;
	apCanvas->mClipBox.mX1 = apCanvas->mWidth;
	apCanvas->mClipBox.mY1 = apCanvas->mHeight;

/*	apMouse->mX = 3;
	apMouse->mY = 100;

	if( apMouse->mX < 0 )
	{
		apCanvas->mClipBox.mX0 = 0;
	}
	else if( apMouse->mX > (320-16) )
	{
		apCanvas->mClipBox.mX0 = 320-16;
	}
	else
	{
		apCanvas->mClipBox.mX0=apMouse->mX+4;
	}
	if( apMouse->mY < 0 )
	{
		apCanvas->mClipBox.mY0 = 0;
	}
	else if( apMouse->mY > (200-16) )
	{
		apCanvas->mClipBox.mY0 = 200-16;
	}
	else
	{
		apCanvas->mClipBox.mY0=apMouse->mY;
	}
	apCanvas->mClipBox.mX1=apCanvas->mClipBox.mX0+16;
	apCanvas->mClipBox.mY1=apCanvas->mClipBox.mY0+16;
*/

	if( apMouse )
	{
		if( apMouse->mpSprite )
		{
			lpBlock = (sSpriteBlock*)apMouse->mpSprite->mpData;
			if( lpBlock )
			{
				lPos.mX = apMouse->mX;
				lPos.mY = apMouse->mY;
				apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, lpBlock->mpSprite[ 0 ] );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_CursorDraw( sGraphicCanvas * apCanvas,const S16 aX,const S16 aY,const S16 aHeight )
* ACTION   : RenderGui_CursorDraw
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_CursorDraw( sGraphicCanvas * apCanvas,const S16 aX,const S16 aY,const S16 aHeight )
{
	sGraphicRect	lRect;

	if( gRenderGui.mCursor.mFlashMode )
	{
		lRect.mX      = aX;
		lRect.mY      = aY;
		lRect.mHeight = aHeight;
		lRect.mWidth  = 2;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, gRenderGui.mCursor.mColour );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_CursorUpdate( sRGuiCursor * apCursor )
* ACTION   : RenderGui_CursorUpdate
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_CursorUpdate( sRGuiCursor * apCursor )
{
	apCursor->mFlashCounter--;
	if( apCursor->mFlashCounter < 0 )
	{
		if( apCursor->mFlashMode )
		{
			apCursor->mFlashMode    = 0;
			apCursor->mFlashCounter = apCursor->mFlashRateOff;
		}
		else
		{
			apCursor->mFlashMode    = 1;
			apCursor->mFlashCounter = apCursor->mFlashRateOn;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_SetClipBox( sGraphicCanvas * apCanvas,const sGuiRect * apRect )
* ACTION   : RenderGui_SetClipBox
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	RenderGui_SetClipBox( sGraphicCanvas * apCanvas,const sGuiRect * apRect )
{
	U8			lRes;
	sGraphicBox	lBox;

	lRes = 1;

	lBox.mX0 = apRect->mX;
	lBox.mY0 = apRect->mY;
	lBox.mX1 = (S16)(apRect->mX + apRect->mWidth);
	lBox.mY1 = (S16)(apRect->mY + apRect->mHeight);

	if( lBox.mX0 < apCanvas->mClipBox.mX0 )
	{
		lBox.mX0 = apCanvas->mClipBox.mX0;
	}
	if( lBox.mX1 > apCanvas->mClipBox.mX1 )
	{
		lBox.mX1 = apCanvas->mClipBox.mX1;
	}
	if( lBox.mY0 < apCanvas->mClipBox.mY0 )
	{
		lBox.mY0 = apCanvas->mClipBox.mY0;
	}
	if( lBox.mY1 > apCanvas->mClipBox.mY1 )
	{
		lBox.mY1 = apCanvas->mClipBox.mY1;
	}

	apCanvas->mClipBox = lBox;

	if( apCanvas->mClipBox.mX0 < 0 )
	{
		apCanvas->mClipBox.mX0 = 0;
	}
	if( apCanvas->mClipBox.mX1 > (S16)apCanvas->mWidth )
	{
		apCanvas->mClipBox.mX1 = apCanvas->mWidth;
	}
	if( apCanvas->mClipBox.mY0 < 0 )
	{
		apCanvas->mClipBox.mY0 = 0;
	}
	if( apCanvas->mClipBox.mY1 > (S16)apCanvas->mHeight )
	{
		apCanvas->mClipBox.mY1 = apCanvas->mHeight;
	}

	if( apCanvas->mClipBox.mX0 >= (S16)apCanvas->mWidth )
	{
		lRes = 0;
	}
	else if( apCanvas->mClipBox.mX1 <= 0 )
	{
		lRes = 0;
	}
	else if( apCanvas->mClipBox.mY0 >= (S16)apCanvas->mHeight )
	{
		lRes = 0;
	}
	else if( apCanvas->mClipBox.mY1 <= 0 )
	{
		lRes = 0;
	}
	else if( apCanvas->mClipBox.mX0 >= apCanvas->mClipBox.mX1 )
	{
		lRes = 0;
	}
	else if( apCanvas->mClipBox.mY0 >= apCanvas->mClipBox.mY1 )
	{
		lRes = 0;
	}
	else
	{
		lRes = 1;
	}


	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_SetCustomRender( fRenderGui aCustomRender )
* ACTION   : RenderGui_SetCustomRender
* CREATION : 01.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderGui_SetCustomRender( fRenderGui aCustomRender )
{
	gRenderGui.mpCustomRenderer = aCustomRender;	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderGui_GetScreenIndex( void )
* ACTION   : RenderGui_GetScreenIndex
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	RenderGui_GetScreenIndex( void )
{
	return( (U16)(gRenderGui.mScreenIndex & 1) );	
}


/* ################################################################################ */

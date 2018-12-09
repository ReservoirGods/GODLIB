/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"R_FED.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FE/FED.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/SPRITE/SPRITE.H>
#include	<GODLIB/VIDEO/VIDEO.H>
#include	<GODLIB/WIPE/WIPE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eRFED_TRANS_NONE,
	eRFED_TRANS_INTRO,
	eRFED_TRANS_OUTRO
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sRenderFed
{
	sGraphicCanvas *	mpCanvas;
	sGraphicCanvas *	mpBackCanvas;
	sFedPage *			mpPage;
	sFedSprite *		mpCursor;
	sFedBox				mRedrawBoxes[ 2 ];
	sFedBox				mBox;
	sFedTransition		mTransition;
	U32					mTicks;
	U16					mCursorRedrawFlags[ 2 ];
	U16					mPal[ 16 ];
	U16					mRedrawIndex;
	U16					mTransMode;
	U16					mTransStartedFlag;
} sRenderFed;

typedef struct	sRenderFedLock
{
	U8	mLockedFlag;
	U8	mVisFlag;
} sRenderFedLock;


/* ###################################################################################
#  DATA
################################################################################### */

sRenderFed	gRenderFed;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	RenderFed_PageDraw( sFedPage * apPage, sGraphicCanvas * apCanvas, U16 aRedrawFlag );
void	RenderFed_BGDraw( sFedPage * apPage, sGraphicCanvas * apCanvas );

void	RenderFed_LockEvaluate( sFedLock * apLock, sRenderFedLock * apRFL );

void	RenderFed_ControlDraw( sFedControl * apControl,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag, sFedFontGroup * apFontGroup, const U8 aSelectedFlag );
void	RenderFed_ControlListDraw( sFedControlList * apControlList,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag, sFedFontGroup * apFontGroup );
void	RenderFed_ImageDraw( sFedAsset * apImage,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag );
void	RenderFed_ListDraw( sFedList * apList, sGraphicCanvas * apCanvas, const sFedBox * apBox, const U16 aRedrawFlag, sFedFontGroup * apFontGroup, const U8 aSelectedFlag, const U8 aLockedFlag );
void	RenderFed_ListItemDraw( sFedListItem * apListItem, sGraphicCanvas * apCanvas, const sFedBox * apBox, const U16 aRedrawFlag, sFedFontGroup * apFontGroup, const U8 aSelectedFlag, const U8 aLockedFlag );
void	RenderFed_SpriteDraw( sFedSprite * apImage,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag );
void	RenderFed_SliderDraw( sFedSlider * apSlider,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag );
void	RenderFed_TextDraw( sFedText * apText, sGraphicCanvas * apCanvas, const sFedBox * apBox, const U16 aRedrawFlag, sFedFontGroup * apFontGroup, const U8 aSelectedFlag, const U8 aLockedFlag );
void	RenderFed_CursorSave( sFedSprite * apCursor, const sFedBox * apBox );
void	RenderFed_CursorRestore( sGraphicCanvas * apCanvas );
void	RenderFed_TransitionStart( void );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_Init( void )
* ACTION   : RenderFed_Init
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_Init( void )
{
	U16	i;
	gRenderFed.mRedrawIndex = 0;

	for( i=0; i<2; i++ )
	{
		gRenderFed.mRedrawBoxes[ i ].mPos.mX = 0;
		gRenderFed.mRedrawBoxes[ i ].mPos.mY = 0;
		gRenderFed.mRedrawBoxes[ i ].mSize.mWidth = 0;
		gRenderFed.mRedrawBoxes[ i ].mSize.mHeight = 0;

		gRenderFed.mCursorRedrawFlags[ i ] = 0;
	}

	gRenderFed.mTransMode = eRFED_TRANS_NONE;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_DeInit( void )
* ACTION   : RenderFed_DeInit
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_DeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_Update( sGraphicCanvas * apLogic,sGraphicCanvas * apBack )
* ACTION   : RenderFed_Update
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_Update( sGraphicCanvas * apLogic,sGraphicCanvas * apBack )
{
	U8	lDrawPageFlag;

	gRenderFed.mpCanvas     = apLogic;
	gRenderFed.mpBackCanvas = apBack;

	Wipe_Update( (U16*)apLogic->mpVRAM );

	if( eRFED_TRANS_OUTRO == gRenderFed.mTransMode )
	{
		if( RenderFed_IsTransitionComplete() )
		{
			gRenderFed.mTransMode = eRFED_TRANS_NONE;
		}
	}
	else
	{
		if( Fed_GetpActivePage() )
		{
			lDrawPageFlag = 1;

			if( eRFED_TRANS_INTRO == gRenderFed.mTransMode )
			{
				if( gRenderFed.mTransition.mWipeType )
				{
					lDrawPageFlag = 0;
				}

				if( !gRenderFed.mTransStartedFlag )
				{
					if( !Fed_GetpActivePage()->mRedrawFlag )
					{
						if( gRenderFed.mTransition.mWipeType )
						{
							RenderFed_BGDraw( Fed_GetpActivePage(), apBack );
							RenderFed_PageDraw( Fed_GetpActivePage(), apBack, 1 );
						}
						RenderFed_TransitionStart();
					}
				}
				else if( RenderFed_IsTransitionComplete() )
				{
					if( gRenderFed.mTransition.mWipeType )
					{
						Fed_GetpActivePage()->mRedrawFlag = 2;
						lDrawPageFlag = 1;
					}
					gRenderFed.mTransMode = eRFED_TRANS_NONE;
				}
			}

			if( lDrawPageFlag )
			{
				if( 2 == Fed_GetpActivePage()->mRedrawFlag )
				{
					RenderFed_BGDraw( Fed_GetpActivePage(), apBack );
				}
				if( Fed_GetpActivePage()->mRedrawFlag )
				{
					RenderFed_BGDraw( Fed_GetpActivePage(), apLogic );
				}
				RenderFed_PageDraw( Fed_GetpActivePage(), apLogic, 0 );
			}
		}
	}
	gRenderFed.mRedrawIndex ^= 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_BGDraw( sFedPage * apPage,sGraphicCanvas * apCanvas )
* ACTION   : RenderFed_BGDraw
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_BGDraw( sFedPage * apPage,sGraphicCanvas * apCanvas )
{
	sFedAsset *			lpImage;
	sFedBox				lBox;

	lBox.mPos.mX = 0;
	lBox.mPos.mY = 0;
	lBox.mSize.mWidth = 320;
	lBox.mSize.mHeight = 200;

	lpImage = 0;

	if( apPage->mpPageStyle )
	{
		lpImage = apPage->mpPageStyle->mpPageBG;
	}
	if( apPage->mpBG )
	{
		lpImage = apPage->mpBG;
	}

	if( lpImage )
	{
		RenderFed_ImageDraw( lpImage, apCanvas, &lBox, 1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_PageDraw( sFedPage * apPage,sGraphicCanvs * apCanvas,U16 aRedrawFlag )
* ACTION   : RenderFed_PageDraw
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_PageDraw( sFedPage * apPage,sGraphicCanvas * apCanvas,U16 aRedrawFlag )
{
	U16					lRedrawFlag;
	sFedPageStyle *		lpPageStyle;
	sFedFontGroup *		lpControlFonts;
	sFedBox				lBox;

	lBox.mPos.mX = 0;
	lBox.mPos.mY = 0;
	lBox.mSize.mWidth = 320;
	lBox.mSize.mHeight = 200;

	RenderFed_CursorRestore( apCanvas );

	lRedrawFlag = (U16)(apPage->mRedrawFlag | aRedrawFlag);
	if( apPage->mRedrawFlag )
	{
		apPage->mRedrawFlag--;
	}

	lpPageStyle = apPage->mpPageStyle;
	if( lpPageStyle )
	{
		lpControlFonts = lpPageStyle->mpControlFonts;
		gRenderFed.mpCursor = lpPageStyle->mpCursor;
	}
	else
	{
		lpControlFonts = 0;
	}

	if( apPage->mpCursor )
	{
		gRenderFed.mpCursor = 0;
	}

	RenderFed_TextDraw( apPage->mpTitle, apCanvas, &lBox, lRedrawFlag, lpControlFonts, 0, 0 );
	RenderFed_ControlListDraw( apPage->mpControlList, apCanvas, &lBox, lRedrawFlag, lpControlFonts );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_ImageDraw( sFedImage * apImage,U16 * apScreen,const sFedBox * apBox,const U16 aRedrawFlag )
* ACTION   : RenderFed_ImageDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_ImageDraw( sFedAsset * apImage,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag )
{
	sDegas *	lpDegas;

	(void)apBox;
	if( aRedrawFlag )
	{
		if( apImage )
		{
			lpDegas = (sDegas*)apImage->mpData;
		}
		else
		{
			lpDegas = 0;
		}

		if( lpDegas)
		{
/*			apCanvas->mpFuncs->CopyScreen( apCanvas, &lpDegas->mPixels[0] );*/
			Memory_Copy( apCanvas->mLineOffsets[ Video_GetHeight() ], &lpDegas->mPixels[0], apCanvas->mpVRAM );
		}
		else
		{
			apCanvas->mpFuncs->ClearScreen( apCanvas );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_ControlDraw( sFedControl * apControl,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFontGroup * apFontGroup,const U8 aSelectedFlag )
* ACTION   : RenderFed_ControlDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_ControlDraw( sFedControl * apControl,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag )
{
	sRenderFedLock	lLock;
	sFedBox			lBox;
	U16				lRedrawFlag;


	if( apControl )
	{
		lBox.mPos.mX = (S16)(apBox->mPos.mX + apControl->mPos.mX);
		lBox.mPos.mY = (S16)(apBox->mPos.mY + apControl->mPos.mY);
		lBox.mSize   = apBox->mSize;

		lRedrawFlag = (U16)(aRedrawFlag | apControl->mRedrawFlag);


		RenderFed_LockEvaluate( apControl->mpLock, &lLock );

		if( lLock.mVisFlag )
		{
			if( aSelectedFlag )
			{
				RenderFed_CursorSave( gRenderFed.mpCursor, &lBox );
				RenderFed_SpriteDraw( gRenderFed.mpCursor, apCanvas, &lBox, 1 );
			}

			RenderFed_SpriteDraw( apControl->mpSprite, apCanvas, &lBox, lRedrawFlag );
			RenderFed_TextDraw( apControl->mpTitle, apCanvas, &lBox, lRedrawFlag, apFontGroup, aSelectedFlag, lLock.mLockedFlag );


			switch( apControl->mControlType )
			{
			case	eFED_CONTROL_LIST:
				RenderFed_ListDraw( apControl->mpList, apCanvas, &lBox, lRedrawFlag, apFontGroup, aSelectedFlag, lLock.mLockedFlag );
				break;
			case	eFED_CONTROL_SLIDER:
				RenderFed_SliderDraw( apControl->mpSlider, apCanvas, &lBox, lRedrawFlag );
				break;
			}
		}

		if( apControl->mRedrawFlag )
		{
			apControl->mRedrawFlag--;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_ControlListDraw( sFedControlList * apControlList,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup )
* ACTION   : RenderFed_ControlListDraw
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_ControlListDraw( sFedControlList * apControlList,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup )
{
	U16	i;
	U8	lSelFlag;

	if( apControlList )
	{
		for( i=0; i<apControlList->mControlCount; i++ )
		{
			if( i == apControlList->mControlIndex )
			{
				lSelFlag = 1;
			}
			else
			{
				lSelFlag = 0;
			}

			RenderFed_ControlDraw( apControlList->mppControls[ i ], apCanvas, apBox, aRedrawFlag, apFontGroup, lSelFlag );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_ListDraw( sFedList * apList,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
* ACTION   : RenderFed_ListDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_ListDraw( sFedList * apList,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
{
	sFedBox	lBox;
	S32 *	lpData;
	S32		lVal;


	if( (apList) && (aRedrawFlag) )
	{
		lBox.mPos.mX = (S16)(apBox->mPos.mX + apList->mPos.mX);
		lBox.mPos.mY = (S16)(apBox->mPos.mY + apList->mPos.mY);
		lBox.mSize   = apBox->mSize;

		if( apList->mpVar )
		{
			lVal = 0;
			if( apList->mpVar->mVarClient.mpVar )
			{
				lpData = (S32*)apList->mpVar->mVarClient.mpVar->mpData;
				if( lpData )
				{
					lVal   = *lpData;
				}
			}
			if( lVal >= (S32)apList->mItemCount )
			{
				lVal = apList->mItemCount-1;
			}
			RenderFed_ListItemDraw( apList->mppItems[ lVal ], apCanvas, &lBox, aRedrawFlag, apFontGroup, aSelectedFlag, aLockedFlag );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_ListItemDraw( sFedListItem * apListItem,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
* ACTION   : RenderFed_ListItemDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_ListItemDraw( sFedListItem * apListItem,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
{
	sRenderFedLock	lLock;
	sFedBox			lBox;
	sFedSprite *	lpSpr;
	U8				lLockedFlag;

	if( (apListItem) && (aRedrawFlag) )
	{
		lBox.mPos.mX = (S16)(apBox->mPos.mX + apListItem->mPos.mX);
		lBox.mPos.mY = (S16)(apBox->mPos.mY + apListItem->mPos.mY);
		lBox.mSize   = apBox->mSize;

		RenderFed_LockEvaluate( apListItem->mpLock, &lLock );

		if( lLock.mVisFlag )
		{
			lLockedFlag = (U8)(aLockedFlag | lLock.mLockedFlag);

			RenderFed_TextDraw( apListItem->mpText, apCanvas, &lBox, aRedrawFlag, apFontGroup, aSelectedFlag, lLockedFlag );

			if( apListItem->mpSpriteGroup )
			{
				if( aSelectedFlag )
				{
					if( lLockedFlag )
					{
						lpSpr = apListItem->mpSpriteGroup->mpSpriteLockedSelected;
					}
					else
					{
						lpSpr = apListItem->mpSpriteGroup->mpSpriteSelected;
					}
				}
				else
				{
					if( lLockedFlag )
					{
						lpSpr = apListItem->mpSpriteGroup->mpSpriteLocked;
					}
					else
					{
						lpSpr = apListItem->mpSpriteGroup->mpSpriteNormal;
					}
				}
				RenderFed_SpriteDraw( lpSpr, apCanvas, &lBox, aRedrawFlag );
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_SpriteDraw( sFedSprite * apImage,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag )
* ACTION   : RenderFed_SpriteDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_SpriteDraw( sFedSprite * apSprite,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag )
{
	sSpriteBlock *	lpBlock;
	sGraphicPos		lPos;
	S16				lFrameOld;

	if( apSprite )
	{
		if( apSprite->mAnimSpeed.l )
		{
			lFrameOld           = apSprite->mFrame.w.w1;
			apSprite->mFrame.l += apSprite->mAnimSpeed.l;
			if( lFrameOld != apSprite->mFrame.w.w1 )
			{
				apSprite->mRedrawFlag = 2;
			}
		}

		if( aRedrawFlag || apSprite->mRedrawFlag )
		{
			if( apSprite->mRedrawFlag )
			{
				apSprite->mRedrawFlag--;
			}

			if( apSprite->mpAsset && apSprite->mpAsset->mpData )
			{
				lpBlock = (sSpriteBlock*)apSprite->mpAsset->mpData;

				if( apSprite->mFrame.w.w1 >= (S16)lpBlock->mHeader.mSpriteCount )
				{
					apSprite->mFrame.w.w1 = 0;
				}

				if( lpBlock )
				{
					if( apSprite->mFrame.w.w1 < (S16)lpBlock->mHeader.mSpriteCount )
					{
						lPos.mX = (S16)(apBox->mPos.mX + apSprite->mPos.mX);
						lPos.mY = (S16)(apBox->mPos.mY + apSprite->mPos.mY);

						apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, lpBlock->mpSprite[ apSprite->mFrame.w.w1 ] );
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_SliderDraw( sFedSlider * apSlider,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag )
* ACTION   : RenderFed_SliderDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_SliderDraw( sFedSlider * apSlider,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag )
{
	sFedBox	lBox;
	sGraphicRect	lRect;
	S32		lWidth;
	S32 *	lpData;
	S32		lDiff;
	S32		lVal;
	S32		lValMin;
	S32		lValMax;

	if( (apSlider) && (aRedrawFlag) )
	{
		lBox.mPos.mX       = (S16)(apBox->mPos.mX + apSlider->mBox.mPos.mX);
		lBox.mPos.mY       = (S16)(apBox->mPos.mY + apSlider->mBox.mPos.mY);
		lBox.mSize.mHeight = (S16)(apBox->mSize.mHeight + apSlider->mBox.mSize.mHeight);
		lBox.mSize.mWidth  = (S16)(apBox->mSize.mWidth  + apSlider->mBox.mSize.mWidth);

		lVal    = 0;
		lValMin = apSlider->mValueMin;
		lValMax = apSlider->mValueMax;


		if( apSlider->mpVar )
		{
			if( apSlider->mpVar->mVarClient.mpVar )
			{
				lpData = (S32*)apSlider->mpVar->mVarClient.mpVar->mpData;
				if( lpData )
				{
					lVal = *lpData;
				}
			}
		}


		if( lVal < lValMin )
		{
			lVal = lValMin;
		}
		if( lVal > lValMax )
		{
			lVal = lValMax;
		}

		lVal -= lValMin;
		lDiff = lValMax - lValMin;
		if( lDiff <= 0 )
		{
			lDiff = 1;
		}

		lWidth  = apSlider->mBox.mSize.mWidth;
		lWidth *= lVal;
		lWidth /= lDiff;


		lRect.mX = lBox.mPos.mX;
		lRect.mY = lBox.mPos.mY;
		lRect.mWidth = apSlider->mBox.mSize.mWidth;
		lRect.mHeight = apSlider->mBox.mSize.mHeight;

		lRect.mX--;
		lRect.mY--;
		lRect.mWidth  += 2;
		lRect.mHeight += 2;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, 15 );

		lRect.mX++;
		lRect.mY++;
		lRect.mHeight -= 2;
		lRect.mWidth  -= 2;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, 0 );

		lRect.mWidth = (S16)lWidth;

		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, 1 );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_TextDraw( sFedText * apText,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
* ACTION   : RenderFed_TextDraw
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_TextDraw( sFedText * apText,sGraphicCanvas * apCanvas,const sFedBox * apBox,const U16 aRedrawFlag,sFedFontGroup * apFontGroup,const U8 aSelectedFlag,const U8 aLockedFlag )
{
	sFedBox		lBox;
	sGraphicRect	lRect;
	sGraphicPos	lPos;
	sFont *		lpFont;
	U16			lStrWidth;


	if( (apText) && (aRedrawFlag) )
	{
		lBox.mPos.mX       = (S16)(apBox->mPos.mX + apText->mBox.mPos.mX);
		lBox.mPos.mY       = (S16)(apBox->mPos.mY + apText->mBox.mPos.mY);
		if( apText->mBox.mSize.mHeight <= 0 )
		{
			lBox.mSize.mHeight = (S16)(apBox->mSize.mHeight + apText->mBox.mSize.mHeight);
		}
		else
		{
			lBox.mSize.mHeight = apText->mBox.mSize.mHeight;
		}

		if( apText->mBox.mSize.mWidth <= 0 )
		{
			lBox.mSize.mWidth  = (S16)(apBox->mSize.mWidth  + apText->mBox.mSize.mWidth);
		}
		else
		{
			lBox.mSize.mWidth = apText->mBox.mSize.mWidth;
		}

		lpFont = 0;

		if( aSelectedFlag )
		{
			if( aLockedFlag )
			{
				if( apFontGroup )
				{
					if( apFontGroup->mpFontLockedSelected )
					{
						lpFont = (sFont*)apFontGroup->mpFontLockedSelected->mpData;
					}
				}

				if( apText->mpFontGroup )
				{
					if( apText->mpFontGroup->mpFontLockedSelected )
					{
						lpFont = (sFont*)apText->mpFontGroup->mpFontLockedSelected->mpData;
					}
				}
			}
			else
			{
				if( apFontGroup )
				{
					if( apFontGroup->mpFontSelected )
					{
						lpFont = (sFont*)apFontGroup->mpFontSelected->mpData;
					}
				}

				if( apText->mpFontGroup )
				{
					if( apText->mpFontGroup->mpFontSelected )
					{
						lpFont = (sFont*)apText->mpFontGroup->mpFontSelected->mpData;
					}
				}
			}
		}
		else if( aLockedFlag )
		{
			if( apFontGroup )
			{
				if( apFontGroup->mpFontLocked )
				{
					lpFont = (sFont*)apFontGroup->mpFontLocked->mpData;
				}
			}

			if( apText->mpFontGroup )
			{
				if( apText->mpFontGroup->mpFontLocked )
				{
					lpFont = (sFont*)apText->mpFontGroup->mpFontLocked->mpData;
				}
			}
		}
		else
		{
			if( apFontGroup )
			{
				if( apFontGroup->mpFontNormal )
				{
					lpFont = (sFont*)apFontGroup->mpFontNormal->mpData;
				}
			}

			if( apText->mpFontGroup )
			{
				if( apText->mpFontGroup->mpFontNormal )
				{
					lpFont = (sFont*)apText->mpFontGroup->mpFontNormal->mpData;
				}
			}

		}


		if( (lpFont) && (apText->mpString) )
		{
			lRect.mX = lBox.mPos.mX;
			lRect.mY = lBox.mPos.mY;
			lRect.mWidth = lBox.mSize.mWidth;
			lRect.mHeight = lBox.mSize.mHeight;

			lPos.mX = lBox.mPos.mX;
			lPos.mY = lBox.mPos.mY;

			if( lRect.mWidth && lRect.mHeight )
			{
				apCanvas->mpClipFuncs->Blit( apCanvas, &lPos, &lRect, gRenderFed.mpBackCanvas );
			}


			lStrWidth = Font_GetStringWidth( lpFont, apText->mpString );

			switch( apText->mAlign )
			{
			case	eFED_ALIGN_RIGHT:
				lPos.mX = (S16)(lBox.mPos.mX + lBox.mSize.mWidth - lStrWidth);
				break;

			case	eFED_ALIGN_CENTRE:
				lPos.mX = lBox.mPos.mX;
				lPos.mX = (S16)(lPos.mX + (lBox.mSize.mWidth>>1));
				lPos.mX = (S16)(lPos.mX - (lStrWidth>>1));
				break;

			case	eFED_ALIGN_LEFT:
			default:
				lPos.mX = lBox.mPos.mX;
				break;
			}
			lPos.mY = lBox.mPos.mY;

			apCanvas->mpClipFuncs->FontPrint( apCanvas, &lPos, lpFont, apText->mpString );
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_LockEvaluate( sFedLock * apLock,sRenderFedLock * apRFL )
* ACTION   : RenderFed_LockEvaluate
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_LockEvaluate( sFedLock * apLock,sRenderFedLock * apRFL )
{
#if	0
	S32		lVal;
	S32 *	lpData;


	apRFL->mLockedFlag = 0;
	apRFL->mVisFlag    = 1;

	if( apLock )
	{
		if( apLock->mpLockVar )
		{
			if( apLock->mpLockVar->mpVar )
			{
				lpData = (S32*)apLock->mpLockVar->mpVar->mpData;
				lVal   = *lpData;

				if( lVal < apLock->mLockValue )
				{
					apRFL->mLockedFlag = 1;
				}
			}
		}

		if( apLock->mpVisVar )
		{
			if( apLock->mpVisVar->mpVar )
			{
				lpData = (S32*)apLock->mpVisVar->mpVar->mpData;
				lVal   = *lpData;

				if( lVal < apLock->mVisValue )
				{
					apRFL->mVisFlag = 0;
				}
			}
		}
	}
#else
	S32		lVal;
	S32 *	lpData;


	apRFL->mLockedFlag = 0;
	apRFL->mVisFlag    = 1;

	if( apLock )
	{
		if( apLock->mpLockVar )
		{
			if( apLock->mpLockVar->mVarClient.mpVar )
			{
				lpData = (S32*)apLock->mpLockVar->mVarClient.mpVar->mpData;
				lVal   = *lpData;

				switch( apLock->mLockCompare )
				{
				case	eFED_COMPARE_EQUAL:
					if( lVal != apLock->mLockValue )
					{
						apRFL->mLockedFlag = 1;
					}
					break;

				case	eFED_COMPARE_LESS:
					if( lVal > apLock->mLockValue )
					{
						apRFL->mLockedFlag = 1;
					}
					break;

				case	eFED_COMPARE_GREATER:
				default:
					if( lVal < apLock->mLockValue )
					{
						apRFL->mLockedFlag = 1;
					}
					break;
				}
			}
		}

		if( apLock->mpVisVar )
		{
			if( apLock->mpVisVar->mVarClient.mpVar )
			{
				lpData = (S32*)apLock->mpVisVar->mVarClient.mpVar->mpData;
				lVal   = *lpData;

				switch( apLock->mVisCompare )
				{
				case	eFED_COMPARE_EQUAL:
					if( lVal != apLock->mVisValue )
					{
						apRFL->mVisFlag = 0;
					}
					break;
				case	eFED_COMPARE_LESS:
					if( lVal > apLock->mVisValue )
					{
						apRFL->mVisFlag = 0;
					}
					break;
				case	eFED_COMPARE_GREATER:
				default:
					if( lVal < apLock->mVisValue )
					{
						apRFL->mVisFlag = 0;
					}
					break;
				}
			}
		}
	}
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_CursorSave( sFedSprite * apCursor,const sFedBox * apBox )
* ACTION   : RenderFed_CursorSave
* CREATION : 09.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_CursorSave( sFedSprite * apCursor,const sFedBox * apBox )
{
	sFedBox *		lpBox;
	sSpriteBlock *	lpBlock;

	if( apCursor->mpAsset )
	{
		lpBlock = (sSpriteBlock*)apCursor->mpAsset->mpData;

		if( lpBlock )
		{
			lpBox  = &gRenderFed.mRedrawBoxes[ gRenderFed.mRedrawIndex ];

			lpBox->mPos.mX       = (S16)(apBox->mPos.mX + apCursor->mPos.mX);
			lpBox->mPos.mY       = (S16)(apBox->mPos.mY + apCursor->mPos.mY);
			lpBox->mSize.mWidth  = (S16)(lpBlock->mpSprite[ 0 ]->mWidth);
			lpBox->mSize.mHeight = (S16)(lpBlock->mpSprite[ 0 ]->mHeight);

			gRenderFed.mCursorRedrawFlags[ gRenderFed.mRedrawIndex ] = 1;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_CursorRestore( sGraphicCanvas * apCanvas )
* ACTION   : RenderFed_CursorRestore
* CREATION : 09.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_CursorRestore( sGraphicCanvas * apCanvas )
{
	sFedBox *		lpBox;
	sGraphicPos		lPos;
	sGraphicRect	lRect;

	if( gRenderFed.mCursorRedrawFlags[ gRenderFed.mRedrawIndex ] )
	{
		lpBox  = &gRenderFed.mRedrawBoxes[ gRenderFed.mRedrawIndex ];

		lPos.mX = lpBox->mPos.mX;
		lPos.mY = lpBox->mPos.mY;

		lRect.mX      = lPos.mX;
		lRect.mY      = lPos.mY;
		lRect.mWidth  = lpBox->mSize.mWidth;
		lRect.mHeight = lpBox->mSize.mHeight;

		apCanvas->mpClipFuncs->Blit( apCanvas, &lPos, &lRect, gRenderFed.mpBackCanvas );

		gRenderFed.mCursorRedrawFlags[ gRenderFed.mRedrawIndex ] = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_IntroInit( sFedTransition * apTrans )
* ACTION   : RenderFed_IntroInit
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_IntroInit( sFedTransition * apTrans )
{
	if( apTrans )
	{
		DebugLog_Printf0( "rfed intro init" );
		gRenderFed.mTransition       = *apTrans;
		gRenderFed.mTransMode        = eRFED_TRANS_INTRO;
		gRenderFed.mTransStartedFlag = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_OutroInit( sFedTransition * apTrans )
* ACTION   : RenderFed_OutroInit
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_OutroInit( sFedTransition * apTrans )
{
	if( apTrans )
	{
		DebugLog_Printf0( "rfed outro init" );
		gRenderFed.mTransition       = *apTrans;
		gRenderFed.mTransMode        = eRFED_TRANS_OUTRO;
		gRenderFed.mTransStartedFlag = 0;
		RenderFed_TransitionStart();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_IsTransitionComplete( void )
* ACTION   : RenderFed_IsTransitionComplete
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	RenderFed_IsTransitionComplete( void )
{
	U8	lComp;

	lComp = 1;

	if( gRenderFed.mTransMode )
	{
		if( !Fade_IsVblFadeFinished() )
		{
			lComp = 0;
		}
		if( !Wipe_IsFinished() )
		{
			lComp = 0;
		}
	}

	return( lComp );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RenderFed_TransitionStart( void )
* ACTION   : RenderFed_TransitionStart
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	RenderFed_TransitionStart( void )
{
	U16			i;
	sFedPage *	lpPage;
	sFedAsset *	lpAss;
	sDegas *	lpDegas;

	gRenderFed.mTransStartedFlag = 1;


	lpPage  = Fed_GetpActivePage();
	lpDegas = 0;
	if( lpPage )
	{
		lpAss = 0;
		if( lpPage->mpPageStyle )
		{
			lpAss = lpPage->mpPageStyle->mpPageBG;
		}
		if( lpPage->mpBG )
		{
			lpAss = lpPage->mpBG;
		}
		if( lpAss )
		{
			lpDegas = (sDegas*)lpAss->mpData;
		}
	}

	if( gRenderFed.mTransition.mWipeType )
	{
		switch( gRenderFed.mTransMode )
		{
		case	eRFED_TRANS_INTRO:
			if( lpDegas )
			{
				DebugLog_Printf0( "wipe in init" );
				Wipe_In_Init( gRenderFed.mTransition.mWipeIndex, (U16*)gRenderFed.mpBackCanvas->mpVRAM );
			}
			break;
		case	eRFED_TRANS_OUTRO:
			DebugLog_Printf0( "wipe out init" );
			Wipe_Out_Init( gRenderFed.mTransition.mWipeIndex );
			break;
		}
	}

	switch( gRenderFed.mTransition.mFadeType )
	{
	case	eFED_FADE_NONE:
		DebugLog_Printf0( "fade none" );
		break;
	case	eFED_FADE_BG:
		DebugLog_Printf0( "fade bg" );
		Video_GetPalST( &gRenderFed.mPal[ 0 ] );
		for( i=0; i<16; i++ )
		{
			gRenderFed.mPal[ i ] = gRenderFed.mPal[ gRenderFed.mTransition.mFadeColour ];
		}
		Fade_StartVblFade( &gRenderFed.mPal[ 0 ], gRenderFed.mTransition.mFadeFrameCount );
		break;
	case	eFED_FADE_PAL:
		DebugLog_Printf1( "fade pal : frames %d", gRenderFed.mTransition.mFadeFrameCount );
		if( lpDegas )
		{
			Fade_StartVblFade( &lpDegas->mHeader.mPalette[ 0 ], gRenderFed.mTransition.mFadeFrameCount );
		}
		else
		{
			DebugLog_Printf0( "ERROR - no degas file found" );
		}
		break;
	case	eFED_FADE_RGB:
		DebugLog_Printf0( "fade rgb" );
		for( i=0; i<16; i++ )
		{
			gRenderFed.mPal[ i ] = gRenderFed.mTransition.mFadeColour;
		}
		Fade_StartVblFade( &gRenderFed.mPal[ 0 ], gRenderFed.mTransition.mFadeFrameCount );
		break;
	}
}


/* ################################################################################ */

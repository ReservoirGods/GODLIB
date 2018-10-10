/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CUT_SYS.H"

#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/SCREEN/SCREEN.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/SPRITE/SPRITE.H>
#include	<GODLIB/STRING/STRING.H>

#ifndef dGODLIB_COMPILER_AHCC
#include	<MATH.H>
#endif

/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eCUTTHREAD_WAIT_NONE,

	eCUTTHREAD_WAIT_ANIM,
	eCUTTHREAD_WAIT_MOVE,
	eCUTTHREAD_WAIT_FADE,
	eCUTTHREAD_WAIT_VBL,

	eCUTTHREAD_WAIT_LIMIT
};


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dCUTSCENE_STACK_LIMIT	16


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sCutScenePC
{
	sCutScript *	mpScript;
	U16				mLine;
} sCutScenePC;

typedef	struct	sCutSceneThread
{
	sCutScenePC	mStack[ dCUTSCENE_STACK_LIMIT ];
	sCutScenePC	mPC;
	U16			mStackIndex;
	U16			mWaitMode;
	U32			mWaitData;
} sCutSceneThread;

typedef	struct	sCutSceneSysClass
{
	sCutScene *		mpScene;
	sCutAsset *		mpPal;
	sCutAsset *		mpBGAsset;
	sHashTree *		mpTree;
	sCutSceneThread	mThread;
	U16				mBGRedrawFlag;
	U16				mBGEnableFlag;
} sCutSceneSysClass;


/* ###################################################################################
#  DATA
################################################################################### */

sCutSceneSysClass	gCutSceneSysClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	CutScene_System_SpriteMoveUpdate( sCutSprite * apSprite );
void	CutScene_System_SpriteAnimUpdate( sCutSprite * apSprite );
void	CutScene_System_SpriteMoveInit( sCutSprite * apSprite, S16 aTargetX, S16 aTargetY );
S32		CutSceneSys_Sqrt( S32 aR );

U32		CutSceneThread_Update( sCutSceneThread * apThread );
U16		CutSceneThread_IsWaiting( sCutSceneThread * apThread );

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_AppInit( void )
* ACTION   : CutScene_System_AppInit
* CREATION : 22.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	CutScene_System_AppInit( void )
{
	Memory_Clear( sizeof(sCutSceneSysClass), &gCutSceneSysClass );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_AppDeInit( void )
* ACTION   : CutScene_System_AppDeInit
* CREATION : 22.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	CutScene_System_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_Update( void )
* ACTION   : CutScene_System_Update
* CREATION : 22.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	CutScene_System_Update( void )
{
	return( (U8)CutSceneThread_Update( &gCutSceneSysClass.mThread ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutSceneThread_IsWaiting( sCutSceneThread * apThread )
* ACTION   : CutSceneThread_IsWaiting
* CREATION : 30.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	CutSceneThread_IsWaiting( sCutSceneThread * apThread )
{
	sCutSprite * lpSprite;
	U16			lRes;

	lRes = 0;

	switch( apThread->mWaitMode )
	{
	case	eCUTTHREAD_WAIT_ANIM:
		lpSprite = (sCutSprite*)apThread->mWaitData;
		if( lpSprite->mAnimFrame.w.w1 != lpSprite->mCurrentDef.mpAnimation->mFrame1 )
		{
			lRes = 1;
		}
		break;
	case	eCUTTHREAD_WAIT_MOVE:
		lpSprite = (sCutSprite*)apThread->mWaitData;
		if( lpSprite->mPos.w.w1 < lpSprite->mDist )
		{
			lRes = 1;
		}
		break;
	case	eCUTTHREAD_WAIT_FADE:
		if( !Fade_IsVblFadeFinished() )
		{
			lRes = 1;
		}
		break;
	case	eCUTTHREAD_WAIT_VBL:
		if( apThread->mWaitData )
		{
			apThread->mWaitData--;
			lRes = 1;
		}
		break;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutSceneThread_Update( sCutSceneThread * apThread )
* ACTION   : CutSceneThread_Update
* CREATION : 30.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	CutSceneThread_Update( sCutSceneThread * apThread )
{
	U8				lRes;
	U8				lDone;
	U16				i;
	sCutAsset *		lpAss;
	sCutCommand *	lpCmd;
	sCutText *		lpText;
	sCutSprite *	lpSprite;
	sCutVar *		lpVar;

	if( 0 == gCutSceneSysClass.mpScene )
	{
		return( 0 );
	}

	lRes  = 0;
	lDone = 0;

	for( i=0; i<gCutSceneSysClass.mpScene->mSpriteCount; i++ )
	{
		CutScene_System_SpriteMoveUpdate( &gCutSceneSysClass.mpScene->mpSprites[ i ] );
		CutScene_System_SpriteAnimUpdate( &gCutSceneSysClass.mpScene->mpSprites[ i ] );
	}


	while( !lDone )
	{
		if( CutSceneThread_IsWaiting( apThread ) )
		{
			lDone = 1;
		}
		else
		{
			apThread->mWaitMode = eCUTTHREAD_WAIT_NONE;
			if(  apThread->mPC.mLine < apThread->mPC.mpScript->mCommandCount )
			{
				lpCmd = &apThread->mPC.mpScript->mpCommands[ apThread->mPC.mLine ];
				switch( lpCmd->mOp )
				{
				case	eCUT_CMD_CALL:

					apThread->mStack[ apThread->mStackIndex ] = apThread->mPC;
					apThread->mStackIndex++;

					apThread->mPC.mpScript = (sCutScript*)lpCmd->mArg0;
					apThread->mPC.mLine    = 0;
					break;

				case	eCUT_CMD_CALLBACK:
					lRes  = 2;
					lDone = 1;
					break;

				case	eCUT_CMD_BG_GFX_CLEAR:
					gCutSceneSysClass.mpBGAsset = 0;
					gCutSceneSysClass.mBGRedrawFlag = 2;
					break;

				case	eCUT_CMD_BG_GFX_SET:
					gCutSceneSysClass.mpBGAsset = (sCutAsset*)lpCmd->mArg0;
					gCutSceneSysClass.mBGRedrawFlag = 2;
					break;

				case	eCUT_CMD_BG_GFX_ENABLE:
					gCutSceneSysClass.mBGEnableFlag = 1;
					break;
				case	eCUT_CMD_BG_GFX_DISABLE:
					gCutSceneSysClass.mBGEnableFlag = 0;
					break;

				case	eCUT_CMD_FADE_SET:
					lpAss = (sCutAsset*)lpCmd->mArg0;
					if( lpAss )
					{
						if( lpAss->mpData )
						{
							Fade_StartVblFade( (U16*)lpAss->mpData, (U16)lpCmd->mArg1 );
						}
					}
					break;
				case	eCUT_CMD_FADE_TOBLACK:
					Fade_StartVblFade( Fade_GetpBlackPal(), (U16)lpCmd->mArg0 );
					break;
				case	eCUT_CMD_FADE_TOPI1:
					lpAss = (sCutAsset*)lpCmd->mArg0;
					if( lpAss )
					{
						sDegas *	lpDegas;

						lpDegas = (sDegas*)lpAss->mpData;
						if( lpDegas )
						{
							Fade_StartVblFade( lpDegas->mHeader.mPalette, (U16)lpCmd->mArg1 );
						}
					}
					break;
				case	eCUT_CMD_FADE_TOWHITE:
					Fade_StartVblFade( Fade_GetpWhitePal(), (U16)lpCmd->mArg0 );
					break;
				case	eCUT_CMD_FADE_WAIT:
					apThread->mWaitMode = eCUTTHREAD_WAIT_FADE;
					apThread->mWaitData = 0;
					break;

				case	eCUT_CMD_RETURN:
					if( apThread->mStackIndex )
					{
						apThread->mStackIndex--;
						apThread->mPC = apThread->mStack[ apThread->mStackIndex ];
					}
					else
					{
						lDone = 1;
						lRes  = 1;
					}
					break;

				case	eCUT_CMD_SAMPLE_ENABLE:
				case	eCUT_CMD_SAMPLE_DISABLE:
				case	eCUT_CMD_SAMPLE_WAIT:
					break;

				case	eCUT_CMD_SPRITE_ANIM_SET:
					lpSprite                          = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mCurrentDef.mpAnimation = (sCutAnimation*)lpCmd->mArg1;
					lpSprite->mAnimFrame.w.w0         = 0;
					lpSprite->mAnimFrame.w.w1         = lpSprite->mCurrentDef.mpAnimation->mFrame0;
					lpSprite->mAnimDir                = 1;
					break;

				case	eCUT_CMD_SPRITE_ANIM_WAIT:
					apThread->mWaitMode = eCUTTHREAD_WAIT_ANIM;
					apThread->mWaitData = lpCmd->mArg0;
					break;

				case	eCUT_CMD_SPRITE_DISABLE:
					lpSprite                          = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mEnabledFlag = 0;
					break;

				case	eCUT_CMD_SPRITE_ENABLE:
					lpSprite                          = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mEnabledFlag = 1;
					break;

				case	eCUT_CMD_SPRITE_GFX_SET:
					lpSprite                      = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mCurrentDef.mpAsset = (sCutAsset*)lpCmd->mArg1;
					break;

				case	eCUT_CMD_SPRITE_SPEED_SET:
					lpSprite                     = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mCurrentDef.mSpeed = lpCmd->mArg1;
					break;

				case	eCUT_CMD_SPRITE_MOVE_TO:
					CutScene_System_SpriteMoveInit( (sCutSprite*)lpCmd->mArg0, (S16)lpCmd->mArg1, (S16)lpCmd->mArg2 );
					break;

				case	eCUT_CMD_SPRITE_MOVE_WAIT:
					apThread->mWaitMode = eCUTTHREAD_WAIT_MOVE;
					apThread->mWaitData = lpCmd->mArg0;
					break;

				case	eCUT_CMD_SPRITE_POS_SET:
					lpSprite                    = (sCutSprite*)lpCmd->mArg0;
					lpSprite->mCurrentDef.mX    = (S16)lpCmd->mArg1;
					lpSprite->mCurrentDef.mY    = (S16)lpCmd->mArg2;
					lpSprite->mSrcX = lpSprite->mDstX = lpSprite->mCurrentDef.mX;
					lpSprite->mDstX = lpSprite->mDstX = lpSprite->mCurrentDef.mX;
					lpSprite->mPos.l = 0;
					lpSprite->mDist  = 0;
					break;

				case	eCUT_CMD_TEXT_ANIM_SET:
					lpText                     = (sCutText*)lpCmd->mArg0;
					break;

				case	eCUT_CMD_TEXT_ANIM_WAIT:
					break;

				case	eCUT_CMD_TEXT_DISABLE:
					lpText                     = (sCutText*)lpCmd->mArg0;
					lpText->mEnabledFlag       = 0;
					lpText->mRedrawFlag        = 0;
					lpText->mUndrawFlag        = 2;
					break;

				case	eCUT_CMD_TEXT_ENABLE:
					lpText                     = (sCutText*)lpCmd->mArg0;
					lpText->mEnabledFlag       = 1;
					lpText->mRedrawFlag        = 2;
					lpText->mUndrawFlag        = 2;
					break;

				case	eCUT_CMD_TEXT_FONT_SET:
					lpText                      = (sCutText*)lpCmd->mArg0;
					lpText->mCurrentDef.mpAsset = (sCutAsset*)lpCmd->mArg1;
					break;

				case	eCUT_CMD_TEXT_PAGE_SET:
					lpText                     = (sCutText*)lpCmd->mArg0;
					lpText->mCurrentDef.mpPage = (sCutPage*)lpCmd->mArg1;
					lpText->mOffset            = 0;
					break;

				case	eCUT_CMD_VAR_SET:
					lpVar                     = (sCutVar*)lpCmd->mArg0;
					HashTree_VarWrite( lpVar->mpVar, &lpCmd->mArg1 );
					break;

				case	eCUT_CMD_WAIT:
					apThread->mWaitMode = eCUTTHREAD_WAIT_VBL;
					apThread->mWaitData = lpCmd->mArg0;
					break;
				}

				if( eCUT_CMD_CALL != lpCmd->mOp )
				{
					apThread->mPC.mLine++;
				}
			}
		}
	}
	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_ScriptInit( sCutScene * apCut,char * apScriptName )
* ACTION   : CutScene_System_ScriptInit
* CREATION : 24.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	CutScene_System_ScriptInit( sCutScene * apCut,char * apScriptName )
{
	
	U16	i;

	if( apCut )
	{
		gCutSceneSysClass.mpScene = apCut;
		gCutSceneSysClass.mThread.mWaitMode = eCUTTHREAD_WAIT_NONE;

		for( i=0; i<apCut->mSpriteCount; i++ )
		{
			sCutSprite * lpSprite;

			lpSprite = &apCut->mpSprites[ i ];

			lpSprite->mCurrentDef = lpSprite->mInitialDef;
			lpSprite->mSrcX  = lpSprite->mDstX = lpSprite->mInitialDef.mX;
			lpSprite->mSrcY  = lpSprite->mDstY = lpSprite->mInitialDef.mY;
			lpSprite->mPos.l = 0;
			lpSprite->mDist  = 0;
			lpSprite->mAnimFrame.l = 0;
			lpSprite->mAnimDir     = 1;
		}

		for( i=0; i<apCut->mTextCount; i++ )
		{
			apCut->mpTexts[ i ].mCurrentDef = apCut->mpTexts[ i ].mInitialDef;
		}

		i = 0;
		while( (i<apCut->mScriptCount) && (String_StrCmpi( apScriptName, apCut->mpScripts[ i ].mpName ) ) )
		{
			i++;
		}
		if( i < apCut->mScriptCount )
		{
			gCutSceneSysClass.mThread.mPC.mLine    = 0;
			gCutSceneSysClass.mThread.mPC.mpScript = &apCut->mpScripts[ i ];
			gCutSceneSysClass.mThread.mStackIndex  = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutScene_System_SpriteMoveUpdate(sCutSprite * apSprite)
* ACTION   : CutScene_System_SpriteMoveUpdate
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutScene_System_SpriteMoveUpdate(sCutSprite * apSprite)
{
	S32	lDiff;

	if( apSprite->mPos.w.w1 < apSprite->mDist )
	{
		apSprite->mPos.l += apSprite->mCurrentDef.mSpeed;
		if( apSprite->mPos.w.w1 >= apSprite->mDist )
		{
			apSprite->mPos.l = 0;
			apSprite->mDist  = 0;
			apSprite->mCurrentDef.mX = apSprite->mDstX;
			apSprite->mCurrentDef.mY = apSprite->mDstY;
			apSprite->mSrcX = apSprite->mDstX;
			apSprite->mSrcY = apSprite->mDstY;
		}
		else
		{
			lDiff = apSprite->mDstX - apSprite->mSrcX;
			lDiff = lDiff * apSprite->mPos.w.w1;
			lDiff = lDiff / apSprite->mDist;
			lDiff = lDiff + apSprite->mSrcX;
			apSprite->mCurrentDef.mX = (S16)lDiff;

			lDiff = apSprite->mDstY - apSprite->mSrcY;
			lDiff = lDiff * apSprite->mPos.w.w1;
			lDiff = lDiff / apSprite->mDist;
			lDiff = lDiff + apSprite->mSrcY;
			apSprite->mCurrentDef.mY = (S16)lDiff;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutScene_System_SpriteAnimUpdate(sCutSprite * apSprite)
* ACTION   : CutScene_System_SpriteAnimUpdate
* CREATION : 27.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutScene_System_SpriteAnimUpdate(sCutSprite * apSprite)
{
	sCutAnimation *	lpAnim;

	lpAnim = apSprite->mCurrentDef.mpAnimation;

	if( lpAnim )
	{
		if( apSprite->mAnimDir )
		{
			apSprite->mAnimFrame.l += lpAnim->mSpeed;
		}
		else
		{
			apSprite->mAnimFrame.l -= lpAnim->mSpeed;
		}

		if( apSprite->mAnimFrame.w.w1 < 0 )
		{
			if( lpAnim->mFlagPingPong )
			{
				if( lpAnim->mFrame1 )
				{
					apSprite->mAnimFrame.w.w1 = 1;
				}
				else
				{
					apSprite->mAnimFrame.w.w1 = 0;
				}
			}
			else
			{
				apSprite->mAnimFrame.w.w1 = 0;
			}
			apSprite->mAnimDir = 1;
		}

		if( apSprite->mAnimFrame.w.w1 > lpAnim->mFrame1 )
		{
			if( lpAnim->mFlagLoop )
			{
				apSprite->mAnimFrame.w.w1 = 0;
			}
			else if( lpAnim->mFlagPingPong )
			{
				apSprite->mAnimDir = 0;
				apSprite->mAnimFrame.w.w1 = lpAnim->mFrame1;
				if( apSprite->mAnimFrame.w.w1 )
				{
					apSprite->mAnimFrame.w.w1--;
				}
			}
			else
			{
				apSprite->mAnimFrame.w.w1 = lpAnim->mFrame1;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutScene_System_SpriteMoveInit(sCutSprite * apSprite,S16 aTargetX,S16 aTargetY)
* ACTION   : CutScene_System_SpriteMoveInit
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutScene_System_SpriteMoveInit(sCutSprite * apSprite,S16 aTargetX,S16 aTargetY)
{
	S32	lDistX;
	S32	lDistY;

	apSprite->mSrcX = apSprite->mCurrentDef.mX;
	apSprite->mSrcY = apSprite->mCurrentDef.mY;
	apSprite->mDstX = aTargetX;
	apSprite->mDstY = aTargetY;

	lDistX = apSprite->mDstX - apSprite->mSrcX;
	lDistX = lDistX * lDistX;

	lDistY = apSprite->mDstY - apSprite->mSrcY;
	lDistY = lDistY * lDistY;

	lDistX = lDistX + lDistY;

	lDistX = CutSceneSys_Sqrt( lDistX );

	apSprite->mPos.l = 0;
	apSprite->mDist  = (S16)lDistX;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : S32 CutSceneSys_Sqrt(S32 aR)
* ACTION   : CutSceneSys_Sqrt
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S32 CutSceneSys_Sqrt(S32 aR)
{
	S32	lT,lB,lC;

	lC = 0;

	for( lB=0x10000000L; lB!=0; lB>>=2 )
	{
		lT   = lC + lB;
		lC >>= 1L;
		if( lT <= aR)
		{
			aR -= lT;
			lC += lB;
		}
	}

	return( lC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_Render( void )
* ACTION   : CutScene_System_Render
* CREATION : 22.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	CutScene_System_Render( void )
{
	sCutScene *			lpCut;
	sCutSprite *		lpSprite;
	sCutText *			lpText;
	sFont *				lpFont;
	sSpriteBlock *		lpSprBlock;
	sGraphicCanvas *	lpLogic;
	sGraphicCanvas *	lpBack;
	sGraphicPos			lPos;
	sGraphicRect		lRect;
	U16					lLogicIndex;
	U16					i;


	lpCut   = gCutSceneSysClass.mpScene;
	if( lpCut )
	{
		lpBack  = Screen_GetpBackGraphic();
		lpLogic = Screen_GetpLogicGraphic();
		lLogicIndex = Screen_GetLogicIndex();

		if( gCutSceneSysClass.mBGRedrawFlag )
		{
			if( gCutSceneSysClass.mpBGAsset )
			{
				sDegas *	lpDegas;

				lpDegas = (sDegas*)gCutSceneSysClass.mpBGAsset->mpData;
				if( 2 == gCutSceneSysClass.mBGRedrawFlag )
				{
					Memory_Copy( 32000, lpDegas->mPixels, lpBack->mpVRAM );
/*					lpBack->mpFuncs->CopyScreen( lpBack, lpDegas->mPixels );*/
				}
				Memory_Copy( 32000, lpDegas->mPixels, lpLogic->mpVRAM );
/*				lpLogic->mpFuncs->CopyScreen( lpLogic, lpDegas->mPixels );*/
			}
			else
			{
				if( 2 == gCutSceneSysClass.mBGRedrawFlag )
				{
					Memory_Clear( 32000, lpBack->mpVRAM );
				}
				Memory_Clear( 32000, lpLogic->mpVRAM );
			}
			gCutSceneSysClass.mBGRedrawFlag--;
		}

		for( i=0; i<lpCut->mSpriteCount; i++ )
		{
			lpSprite    = &lpCut->mpSprites[ i ];

			lPos.mX       = lpSprite->mDirtyRects[ lLogicIndex ].mX;
			lPos.mY       = lpSprite->mDirtyRects[ lLogicIndex ].mY;
			lRect.mX      = lPos.mX;
			lRect.mY      = lPos.mY;
			lRect.mWidth  = lpSprite->mDirtyRects[ lLogicIndex ].mWidth;
			lRect.mHeight = lpSprite->mDirtyRects[ lLogicIndex ].mHeight;
			if( lRect.mWidth && lRect.mHeight )
			{
				lpLogic->mpClipFuncs->Blit( lpLogic, &lPos, &lRect, lpBack );
				lpSprite->mDirtyRects[ lLogicIndex ].mWidth = 0;
			}
		}

		for( i=0; i<lpCut->mTextCount; i++ )
		{
			lpText    = &lpCut->mpTexts[ i ];

			if( lpText->mUndrawFlag )
			{
				lPos.mX       = lpText->mDirtyRects[ lLogicIndex ].mX;
				lPos.mY       = lpText->mDirtyRects[ lLogicIndex ].mY;
				lRect.mX      = lPos.mX;
				lRect.mY      = lPos.mY;
				lRect.mWidth  = lpText->mDirtyRects[ lLogicIndex ].mWidth;
				lRect.mHeight = lpText->mDirtyRects[ lLogicIndex ].mHeight;
				if( lRect.mWidth && lRect.mHeight )
				{
					lpLogic->mpClipFuncs->Blit( lpLogic, &lPos, &lRect, lpBack );
					lpText->mDirtyRects[ lLogicIndex ].mWidth = 0;
				}
				lpText->mUndrawFlag--;
			}
		}

		for( i=0; i<lpCut->mSpriteCount; i++ )
		{
			lpSprite = &lpCut->mpSprites[ i ];
			if( lpSprite->mEnabledFlag )
			{
				if( lpSprite->mCurrentDef.mpAsset )
				{
					lpSprBlock = (sSpriteBlock*)lpSprite->mCurrentDef.mpAsset->mpData;
					if( lpSprBlock )
					{
						S16	lSprIndex;

						lSprIndex = lpSprite->mAnimFrame.w.w1;

						if( (lSprIndex<0) || (lSprIndex>=(S16)lpSprBlock->mHeader.mSpriteCount) )
						{
							lSprIndex = 0;
						}

						lPos.mX = lpSprite->mCurrentDef.mX;
						lPos.mY = lpSprite->mCurrentDef.mY;
						lpLogic->mpClipFuncs->DrawSprite( lpLogic, &lPos, lpSprBlock->mpSprite[ lSprIndex ] );

						lpSprite->mDirtyRects[ lLogicIndex ].mX = lPos.mX;
						lpSprite->mDirtyRects[ lLogicIndex ].mY = lPos.mY;
						lpSprite->mDirtyRects[ lLogicIndex ].mWidth = lpSprBlock->mpSprite[ lSprIndex ]->mWidth;
						lpSprite->mDirtyRects[ lLogicIndex ].mHeight = lpSprBlock->mpSprite[ lSprIndex ]->mHeight;
					}
				}
			}
		}

		for( i=0; i<lpCut->mTextCount; i++ )
		{
			lpText = &lpCut->mpTexts[ i ];
			if( lpText->mEnabledFlag )
			{
				if( lpText->mCurrentDef.mpAsset )
				{
					lpFont = (sFont*)lpText->mCurrentDef.mpAsset->mpData;
					if( lpFont )
					{
						if( lpText->mRedrawFlag )
						{
							U16		lWordOffset;
							char	lWordString[ 128 ];
							char *	lpTextSrc = lpText->mCurrentDef.mpPage->mpText;

							lPos.mX = lpText->mCurrentDef.mRect.mX;
							lPos.mY = lpText->mCurrentDef.mRect.mY;

							while( *lpTextSrc )
							{
								lWordOffset = 0;
								while( *lpTextSrc > ' ' )
								{
									lWordString[ lWordOffset ] = *lpTextSrc;
									lWordOffset++;
									lpTextSrc++;
								}
								if( lWordOffset )
								{
									S16	lWordWidth;
									lWordString[ lWordOffset ] = 0;
									lWordWidth = Font_GetStringWidth( lpFont, lWordString );

									if( (lPos.mX + lWordWidth) > lpText->mCurrentDef.mRect.mWidth )
									{
										lPos.mY += lpFont->mHeightMax + 2;
										lPos.mX  = lpText->mCurrentDef.mRect.mX;
									}
									lpLogic->mpClipFuncs->FontPrint( lpLogic, &lPos, lpFont, lWordString );

									lPos.mX += lWordWidth + lpFont->mWidthMax;
								}
								while( *lpTextSrc == ' ' )
								{
									lpTextSrc++;
								}
								if( (*lpTextSrc==10) || (*lpTextSrc==13) )
								{
									lPos.mY += lpFont->mHeightMax + lpFont->mKerning;
									lPos.mX  = lpText->mCurrentDef.mRect.mX;

									while( (*lpTextSrc) && ((*lpTextSrc!=10) && (*lpTextSrc!=13)) )
									{
										lpTextSrc++;
									}
								}
							}

							lpText->mDirtyRects[ lLogicIndex ].mX      = lpText->mCurrentDef.mRect.mX;
							lpText->mDirtyRects[ lLogicIndex ].mY      = lpText->mCurrentDef.mRect.mY;
							lpText->mDirtyRects[ lLogicIndex ].mWidth  = lpText->mCurrentDef.mRect.mWidth;
							lpText->mDirtyRects[ lLogicIndex ].mHeight = lpText->mCurrentDef.mRect.mHeight;

							lpText->mRedrawFlag--;
						}
					}
				}
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_SetpHashTree( sHashTree * apTree )
* ACTION   : CutScene_System_SetpHashTree
* CREATION : 27.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	CutScene_System_SetpHashTree( sHashTree * apTree )
{
	gCutSceneSysClass.mpTree = apTree;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_GetpHashTree( void )
* ACTION   : CutScene_System_GetpHashTree
* CREATION : 27.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sHashTree *	CutScene_System_GetpHashTree( void )
{
	return( gCutSceneSysClass.mpTree );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_System_IsScriptFinished( void )
* ACTION   : CutScene_System_IsScriptFinished
* CREATION : 30.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U8	CutScene_System_IsScriptFinished( void )
{
	U8	lRes;

	lRes = 0;

	if( !gCutSceneSysClass.mThread.mStackIndex )
	{
		if( gCutSceneSysClass.mThread.mPC.mLine >= gCutSceneSysClass.mThread.mPC.mpScript->mCommandCount )
		{
			lRes = 1;
		}
	}

	return( lRes );
}


/* ################################################################################ */

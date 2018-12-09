#ifndef	INCLUDED_CUTSCENE_H
#define	INCLUDED_CUTSCENE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/HASHTREE/HASHTREE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

#define	dCUTSCENE_VERSION	3

enum
{
	eCUT_CMD_CALL,
	eCUT_CMD_CALLBACK,

	eCUT_CMD_BG_GFX_CLEAR,
	eCUT_CMD_BG_GFX_SET,
	eCUT_CMD_BG_GFX_ENABLE,
	eCUT_CMD_BG_GFX_DISABLE,

	eCUT_CMD_FADE_SET,
	eCUT_CMD_FADE_TOBLACK,
	eCUT_CMD_FADE_TOPI1,
	eCUT_CMD_FADE_TOWHITE,
	eCUT_CMD_FADE_WAIT,

	eCUT_CMD_RETURN,

	eCUT_CMD_SAMPLE_ENABLE,
	eCUT_CMD_SAMPLE_DISABLE,
	eCUT_CMD_SAMPLE_WAIT,

	eCUT_CMD_SPRITE_ANIM_SET,
	eCUT_CMD_SPRITE_ANIM_WAIT,
	eCUT_CMD_SPRITE_DISABLE,
	eCUT_CMD_SPRITE_ENABLE,
	eCUT_CMD_SPRITE_GFX_SET,
	eCUT_CMD_SPRITE_MOVE_TO,
	eCUT_CMD_SPRITE_MOVE_WAIT,
	eCUT_CMD_SPRITE_POS_SET,
	eCUT_CMD_SPRITE_SPEED_SET,

	eCUT_CMD_TEXT_ANIM_SET,
	eCUT_CMD_TEXT_ANIM_WAIT,
	eCUT_CMD_TEXT_ENABLE,
	eCUT_CMD_TEXT_FONT_SET,
	eCUT_CMD_TEXT_DISABLE,
	eCUT_CMD_TEXT_PAGE_SET,

	eCUT_CMD_VAR_SET,

	eCUT_CMD_WAIT,

	eCUT_CMD_LIMIT,
};

enum
{
	eCUT_CMD_ARG_NONE,

	eCUT_CMD_ARG_FIX16_16,
	eCUT_CMD_ARG_FLOAT,
	eCUT_CMD_ARG_INT,

	eCUT_CMD_ARG_ASSET,
	eCUT_CMD_ARG_ANIMATION,
	eCUT_CMD_ARG_MOVEMENT,
	eCUT_CMD_ARG_PAGE,
	eCUT_CMD_ARG_SAMPLE,
	eCUT_CMD_ARG_SCRIPT,
	eCUT_CMD_ARG_SPRITE,
	eCUT_CMD_ARG_TEXT,
	eCUT_CMD_ARG_VAR,

	eCUT_CMD_ARG_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef struct	sCutCmdDef
{
	U16				mIndex;
	char *			mpString;
	U16				mArg0;
	U16				mArg1;
	U16				mArg2;
} sCutCmdDef;

typedef struct	sCutAsset
{
	char *			mpName;
	char *			mpFileName;
	char *			mpContext;
	void *			mpData;
	sAssetClient 	mClient;
} sCutAsset;

typedef struct	sCutAnimation
{
	char *	mpName;
	S32		mSpeed;
	S16		mFrame0;
	S16		mFrame1;
	U8		mFlagPingPong;
	U8		mFlagLoop;
} sCutAnimation;

typedef struct	sCutCommand
{
	U16		mOp;
	U32		mArg0;
	U32		mArg1;
	U32		mArg2;
} sCutCommand;


typedef struct sCutRect
{
	S16		mX;
	S16		mY;
	S16		mWidth;
	S16		mHeight;
} sCutRect;

typedef struct	sCutSample
{
	char *			mpName;
	sCutAsset *		mpAsset;
	U8				mVolume;
	U8				mStereoPos;
	U8				mFlagLoop;
	U8				mPad;
} sCutSample;

typedef struct	sCutSpriteDef
{
	sCutAsset *		mpAsset;
	sCutAnimation *	mpAnimation;
	S32				mSpeed;
	S16				mX;
	S16				mY;
} sCutSpriteDef;

typedef struct	sCutSprite
{
	char *			mpName;
	sCutSpriteDef	mInitialDef;
	sCutSpriteDef	mCurrentDef;
	sCutRect		mDirtyRects[ 2 ];
	S16				mSrcX;
	S16				mSrcY;
	S16				mDstX;
	S16				mDstY;
	S16				mDist;
	uS32			mPos;
	uS32			mAnimFrame;
	S16				mAnimDir;
	U16				mMoveFrame;
	U16				mEnabledFlag;
} sCutSprite;

typedef struct	sCutScript
{
	char *			mpName;
	U16				mCommandCount;
	sCutCommand *	mpCommands;
} sCutScript;

typedef struct	sCutPage
{
	char *	mpName;
	char *	mpText;
} sCutPage;

typedef struct	sCutTextDef
{
	sCutAsset *	mpAsset;
	sCutRect 	mRect;
	sCutPage *	mpPage;
} sCutTextDef;

typedef struct	sCutText
{
	char *		mpName;
	sCutTextDef	mInitialDef;
	sCutTextDef	mCurrentDef;
	sCutRect	mDirtyRects[ 2 ];
	U16			mEnabledFlag;
	U16			mRedrawFlag;
	U16			mUndrawFlag;
	U16			mWordPauseConstant;
	U16			mWordPauseCounter;
	U32			mOffset;
} sCutText;

typedef	struct	sCutVar
{
	char *			mpName;
	char *			mpVarName;
	sHashTreeVarClient	mVarClient;
} sCutVar;

typedef	struct	sCutScene
{
	U32				mID;
	U32				mVersion;

	sCutAnimation *	mpAnimations;
	sCutAsset *		mpAssets;
	sCutPage *		mpPages;
	sCutSample *	mpSamples;
	sCutScript *	mpScripts;
	sCutSprite *	mpSprites;
	sCutText *		mpTexts;
	sCutVar *		mpVars;

	U16				mAnimationCount;
	U16				mAssetCount;
	U16				mSampleCount;
	U16				mScriptCount;
	U16				mSpriteCount;
	U16				mPageCount;
	U16				mTextCount;
	U16				mVarCount;

} sCutScene;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			CutScene_Delocate( sCutScene * apCutScene );
void			CutScene_Relocate( sCutScene * apCutScene );

U32				CutScene_GetSize( sCutScene * apCutScene );

sCutCmdDef *	CutScene_CmdDefFind( const char * apString );
sCutCmdDef *	CutScene_CmdDefFindByID( U16 aID );


/* ################################################################################ */

#endif	/* INCLUDED_CUTSCENE_H */

#ifndef	INCLUDED_FED_H
#define	INCLUDED_FED_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/HASHTREE/HASHTREE.H>
#include	<GODLIB/INPUT/INPUT.H>
#include	<GODLIB/PICTYPES/DEGAS.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dFED_ID			mSTRING_TO_U32( 'F','E', 'D', 'S' )
#define	dFED_VERSION_OLD	0
#define	dFED_VERSION_NEW	1

enum
{
	eFED_CONTROL_NONE,
	eFED_CONTROL_LINK,
	eFED_CONTROL_CALL,
	eFED_CONTROL_LIST,
	eFED_CONTROL_SLIDER,
	eFED_CONTROL_LIMIT
};

enum
{
	eFED_TYPE_U8,
	eFED_TYPE_S8,
	eFED_TYPE_U16,
	eFED_TYPE_S16,
	eFED_TYPE_U32,
	eFED_TYPE_S32,
	eFED_TYPE_FP32,
	eFED_TYPE_STRING,

	eFED_TYPE_LIMIT
};

enum
{
	eFED_ALIGN_LEFT,
	eFED_ALIGN_RIGHT,
	eFED_ALIGN_CENTRE,

	eFED_ALIGN_LIMIT
};

enum
{
	eFED_COMPARE_GREATER,
	eFED_COMPARE_EQUAL,
	eFED_COMPARE_LESS,

	eFED_COMPARE_LIMIT
};

enum
{
	eFED_FADE_NONE,
	eFED_FADE_BG,
	eFED_FADE_PAL,
	eFED_FADE_RGB,

	eFED_FADE_LIMIT
};

enum
{
	eFED_WIPE_NONE,
	eFED_WIPE_RANDOM,
	eFED_WIPE_SPECIFIC,

	eFED_WIPE_LIMIT
};

enum
{
	eFED_LOCKCOMPARE_GREATER,
	eFED_LOCKCOMPARE_EQUAL,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sFedAsset
{
	sAssetClient *	mpAsset;
	char *			mpFileName;
	char *			mpContext;
	void *			mpData;
} sFedAsset;

typedef	struct	sFedPos
{
	S16	mX;
	S16	mY;
} sFedPos;

typedef	struct	sFedSize
{
	S16	mWidth;
	S16	mHeight;
} sFedSize;

typedef	struct	sFedVar
{
	char *			mpName;
	sHashTreeVar *	mpVar;
} sFedVar;

typedef	struct	sFedBox
{
	sFedPos		mPos;
	sFedSize	mSize;
} sFedBox;

typedef	struct	sFedSprite
{
	sFedPos			mPos;
	sFedAsset *		mpAsset;
	uS32			mAnimSpeed;
	uS32			mFrame;
	U16				mFrameBase;
	U16				mRedrawFlag;
} sFedSprite;

typedef	struct	sFedSpriteList
{
	U16				mSpriteCount;
	sFedSprite **	mppSprites;
} sFedSpriteList;

typedef struct	sFedSpriteGroup
{
	sFedSprite *	mpSpriteNormal;
	sFedSprite *	mpSpriteSelected;
	sFedSprite *	mpSpriteLocked;
	sFedSprite *	mpSpriteLockedSelected;
} sFedSpriteGroup;

typedef	struct	sFedFontGroup
{
	sFedAsset *		mpFontNormal;
	sFedAsset *		mpFontSelected;
	sFedAsset *		mpFontLocked;
	sFedAsset *		mpFontLockedSelected;
} sFedFontGroup;

typedef struct	sFedText
{
	sFedBox			mBox;
	sFedFontGroup *	mpFontGroup;
	char *			mpString;
	U16				mAlign;
	U16				mPad;
} sFedText;

typedef	struct	sFedTransition
{
	U16		mFadeFrameCount;
	U16		mFadeColour;
	U16		mFadeType;
	U16		mWipeIndex;
	U16		mWipeType;
} sFedTransition;

typedef	struct	sFedLock
{
	sFedVar *		mpLockVar;
	S32				mLockValue;
	sFedVar *		mpVisVar;
	S32				mVisValue;
	U16				mLockCompare;
	U16				mVisCompare;
} sFedLock;



typedef	struct	sFedSlider
{
	sFedBox		mBox;
	sFedVar *	mpVar;
	S32			mValueMin;
	S32			mValueMax;
	S32			mValueAdd;
} sFedSlider;

typedef	struct	sFedListItem
{
	sFedPos				mPos;
	sFedSpriteGroup *	mpSpriteGroup;
	sFedText *			mpText;
	sFedLock *			mpLock;
} sFedListItem;

typedef	struct	sFedList
{
	sFedPos			mPos;
	sFedListItem **	mppItems;
	sFedVar *		mpVar;
	U16				mItemCount;
	U16				mPad;
} sFedList;

typedef	struct	sFedControl
{
	sFedPos				mPos;
	sFedLock *			mpLock;
	sFedText *			mpTitle;
	sFedSprite *		mpSprite;
	sFedSlider *		mpSlider;
	struct sFedPage *	mpLink;
	sFedList *			mpList;
	struct sFedCall *	mpCall;
	struct sFedVar *	mpSetVar;
	S32					mSetValue;
	U16					mControlType;
	U16					mRedrawFlag;
} sFedControl;

typedef	struct	sFedControlList
{
	U16				mControlCount;
	U16				mControlIndex;
	sFedControl **	mppControls;
} sFedControlList;

typedef	struct	sFedSample
{
	sFedAsset *	mpAsset;
	U8			mPan;
	U8			mVolume;
	U16			mPad;
} sFedSample;

typedef	struct	sFedPageStyle
{
	sFedAsset *			mpPageTitleFont;
	sFedAsset *			mpPageBG;
	sFedFontGroup *		mpControlFonts;
	sFedSprite *		mpCursor;
	sFedTransition *	mpIntroTrans;
	sFedTransition *	mpOutroTrans;
	sFedSample *		mpSampleMove;
	sFedSample *		mpSampleSelect;
} sFedPageStyle;


typedef	struct	sFedPage
{
	U32					mHash;
	sFedText *			mpTitle;
	sFedAsset *			mpBG;
	sFedSprite *		mpCursor;
	sFedPageStyle *		mpPageStyle;
	sFedControlList *	mpControlList;
	sFedSample *		mpSampleMove;
	sFedSample *		mpSampleSelect;
	sFedSpriteList *	mpSpriteList;
	struct sFedPage *	mpPageParent;
	U16					mControlIndex;
	U16					mRedrawFlag;
} sFedPage;


typedef	struct	sFedCall
{
	sFedPage *	mpPageReturn;
	sFedVar *	mpCallVar;
	S32			mCallValue;
} sFedCall;

/*
	asset - file with context
	call - details variable to write to which can invoke hashtree callback function
	control - a single control
	controllist - a list of controls on a page
	fontgroup - normal/selected fonts
	list - a combobox like sliding selection of values
	listitem - a single item in the selection
	lock - a variable and value to lock against
	page - collection of controls and style
	pagestyle - fonts, bg images and transitions
	sample - sound effects for different situations
	sprite - spries
	spritelist - sprites to show on a page
	spritegroup - collection of sprites (e.g. controller icons)
	text - raw text
	transition - fades/wipes
	var - hashtree variables

*/

typedef struct	sFedHeader
{
	U32					mID;
	U32					mVersion;


	sFedAsset *			mpAssets;
	sFedCall *			mpCalls;
	sFedControl *		mpControls;
	sFedControlList *	mpControlLists;
	sFedFontGroup *		mpFontGroups;
	sFedList *			mpLists;
	sFedListItem *		mpListItems;
	sFedLock *			mpLocks;
	sFedPage *			mpPages;
	sFedPageStyle *		mpPageStyles;
	sFedSample *		mpSamples;
	sFedSlider *		mpSliders;
	sFedSprite *		mpSprites;
	sFedSpriteGroup *	mpSpriteGroups;
	sFedSpriteList *	mpSpriteLists;
	sFedText *			mpTexts;
	sFedTransition *	mpTransitions;
	sFedVar *			mpVars;

	U16					mAssetCount;
	U16					mCallCount;

	U16					mControlCount;
	U16					mControlListCount;

	U16					mFontGroupCount;
	U16					mListCount;

	U16					mListItemCount;
	U16					mLockCount;

	U16					mPageCount;
	U16					mPageStyleCount;

	U16					mSampleCount;
	U16					mSliderCount;

	U16					mSpriteCount;
	U16					mSpriteGroupCount;
	U16					mSpriteListCount;

	U16					mTextCount;
	U16					mTransitionCount;
	U16					mVarCount;

} sFedHeader;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		Fed_Delocate( sFedHeader * apHeader );
void		Fed_Relocate( sFedHeader * apHeader );
void		Fed_Init( sFedHeader * apHeader );
void		Fed_DeInit( sFedHeader * apHeader );
void		Fed_Begin( sFedHeader * apHeader, const char * apPageName );
U8			Fed_Update( const sInput * apInput );
sFedPage *	Fed_GetpActivePage( void );
void		Fed_SetPage( const char * apPageName );
void		Fed_ForceRedraw( void );
void		Fed_SetpHashTree( sHashTree * apTree );


/* ################################################################################ */

#endif	/* INCLUDED_FED_H */

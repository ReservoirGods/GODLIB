/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FED.H"

#include	<GODLIB/AUDIO/AMIXER.H>
#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/FE/R_FED.H>
#include	<GODLIB/HASHLIST/HASHLIST.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

enum
{
	eFED_MODE_NORMAL,
	eFED_MODE_INTRO,
	eFED_MODE_OUTRO,
};

#define	dFED_KEYWAIT0	15
#define	dFED_KEYWAIT1	0


/* ###################################################################################
#  STRUCT
################################################################################### */

typedef struct	sFedLockStatus
{
	U8	mLockedFlag;
	U8	mVisFlag;
} sFedLockStatus;


/* ###################################################################################
#  DATA
################################################################################### */

sFedPage *		gpFedPage = 0;
sFedHeader *	gpFedHeader = 0;
sHashTree *		gpFedHashTree = 0;
U16				gFedKeyWait;
U16				gFedMode;
U16				gFedPageHash;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Fed_ListMove( sFedList * apList, const S32 aDir );
void	Fed_SliderMove( sFedSlider * apSlider, const S32 aDir );
void	Fed_ControlListMove( sFedControlList * apList, const S16 aDir );
void	Fed_InputProcess( void );
void	Fed_SetOutro( void );
void	Fed_SetIntro( void );
void	Fed_LockEvaluate( sFedLock * apLock,sFedLockStatus * apRFL );
void	Fed_SampleMovePlay( void );
void	Fed_SampleSelectPlay( void );

void		Fed_DelocateOld( sFedHeader * apHeader );
void		Fed_RelocateOld( sFedHeader * apHeader );

/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_Delocate( sFedHeader * apHeader )
* ACTION   : Fed_Delocate
* CREATION : 07.02.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	mFED_DELOC( _a )			if( apHeader->_a ) { (*(U32*)(&apHeader->_a) -= (U32)apHeader); Endian_FromBigU32( &apHeader->_a ); }
#define	mFED_RELOC( _a )			if( apHeader->_a ) { Endian_FromBigU32( &apHeader->_a ); *(U32*)&apHeader->_a += (U32)apHeader;	}
#define	mFED_LIST_RELOC( _a, _l )	if( apHeader->_a ) { Endian_FromBigU32( &apHeader->_a ); apHeader->_a = &apHeader->_l[ (U32)(apHeader->_a) - 1]; }

void	Fed_Delocate( sFedHeader * apHeader )
{
	U16	i,j;

	Endian_FromBigU32( &apHeader->mID );
	Endian_FromBigU32( &apHeader->mVersion );

	if( apHeader->mVersion < dFED_VERSION_NEW )
	{
		Fed_DelocateOld( apHeader );
		return;
	}

	for( i=0; i<apHeader->mAssetCount; i++ )
	{
		mFED_DELOC( mpAssets[ i ].mpFileName );
		mFED_DELOC( mpAssets[ i ].mpContext );
	}

	for( i=0; i<apHeader->mCallCount; i++ )
	{
		mFED_DELOC( mpCalls[ i ].mpPageReturn );
		mFED_DELOC( mpCalls[ i ].mpCallVar    );
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mCallValue   );
	}

	for( i=0; i<apHeader->mControlCount; i++ )
	{
		mFED_DELOC( mpControls[ i ].mpLock );
		mFED_DELOC( mpControls[ i ].mpSprite );
		mFED_DELOC( mpControls[ i ].mpTitle );

		mFED_DELOC( mpControls[ i ].mpCall );
		mFED_DELOC( mpControls[ i ].mpLink );
		mFED_DELOC( mpControls[ i ].mpList );
		mFED_DELOC( mpControls[ i ].mpSlider );
		mFED_DELOC( mpControls[ i ].mpSetVar );

		Endian_FromBigU32( &apHeader->mpControls[ i ].mSetValue );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mControlType );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mY  );
	}

	for( i=0; i<apHeader->mControlListCount; i++ )
	{
		for( j=0; j<apHeader->mpControlLists[ i ].mControlCount; j++ )
		{
			mFED_DELOC( mpControlLists[ i ].mppControls[ j ] );
		}
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlCount );
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlIndex );
		mFED_DELOC( mpControlLists[ i ].mppControls );
	}

	for( i=0; i<apHeader->mFontGroupCount; i++ )
	{
		mFED_DELOC( mpFontGroups[ i ].mpFontLocked );
		mFED_DELOC( mpFontGroups[ i ].mpFontNormal );
		mFED_DELOC( mpFontGroups[ i ].mpFontSelected );
		mFED_DELOC( mpFontGroups[ i ].mpFontLockedSelected );
	}

	for( i=0; i<apHeader->mListCount; i++ )
	{
		for( j=0; j<apHeader->mpLists[ i ].mItemCount; j++ )
		{
			mFED_DELOC( mpLists[ i ].mppItems[ j ] );
		}
		Endian_FromBigU16( &apHeader->mpLists[ i ].mItemCount );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mY  );

		mFED_DELOC( mpLists[ i ].mpVar );

		mFED_DELOC( mpLists[ i ].mppItems );
	}

	for( i=0; i<apHeader->mListItemCount; i++ )
	{
		mFED_DELOC( mpListItems[ i ].mpSpriteGroup );
		mFED_DELOC( mpListItems[ i ].mpLock   );
		mFED_DELOC( mpListItems[ i ].mpText   );

		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mY  );
	}

	for( i=0; i<apHeader->mLockCount; i++ )
	{
		mFED_DELOC( mpLocks[ i ].mpLockVar );
		mFED_DELOC( mpLocks[ i ].mpVisVar  );

		Endian_FromBigU32( &apHeader->mpLocks[ i ].mLockValue );
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mVisValue  );

		Endian_FromBigU16( &apHeader->mpLocks[ i ].mLockCompare );
		Endian_FromBigU16( &apHeader->mpLocks[ i ].mVisCompare  );
	}

	for( i=0; i<apHeader->mPageCount; i++ )
	{
		mFED_DELOC( mpPages[ i ].mpBG          );
		mFED_DELOC( mpPages[ i ].mpCursor      );
		mFED_DELOC( mpPages[ i ].mpPageStyle   );
		mFED_DELOC( mpPages[ i ].mpPageParent  );
		mFED_DELOC( mpPages[ i ].mpSampleMove  );
		mFED_DELOC( mpPages[ i ].mpSampleSelect);
		mFED_DELOC( mpPages[ i ].mpTitle       );

		mFED_DELOC( mpPages[ i ].mpControlList );
		mFED_DELOC( mpPages[ i ].mpSpriteList  );

		Endian_FromBigU32( &apHeader->mpPages[ i ].mHash         );
		Endian_FromBigU16( &apHeader->mpPages[ i ].mControlIndex );
	}

	for( i=0; i<apHeader->mPageStyleCount; i++ )
	{
		mFED_DELOC( mpPageStyles[ i ].mpPageTitleFont );
		mFED_DELOC( mpPageStyles[ i ].mpPageBG );
		mFED_DELOC( mpPageStyles[ i ].mpCursor );
		mFED_DELOC( mpPageStyles[ i ].mpControlFonts );
		mFED_DELOC( mpPageStyles[ i ].mpIntroTrans );
		mFED_DELOC( mpPageStyles[ i ].mpOutroTrans );
		mFED_DELOC( mpPageStyles[ i ].mpSampleMove  );
		mFED_DELOC( mpPageStyles[ i ].mpSampleSelect);
	}

	for( i=0; i<apHeader->mSampleCount; i++ )
	{
		mFED_DELOC( mpSamples[ i ].mpAsset );
	}

	for( i=0; i<apHeader->mSliderCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mpVar     );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMax );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMin );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueAdd );

		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mX       );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mY       );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mWidth  );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mHeight );
	}

	for( i=0; i<apHeader->mSpriteCount; i++ )
	{
		mFED_DELOC( mpSprites[ i ].mpAsset );

		Endian_FromBigU32( &apHeader->mpSprites[ i ].mAnimSpeed );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mFrameBase  );

		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mY  );
	}

	for( i=0; i<apHeader->mSpriteListCount; i++ )
	{
		for( j=0; j<apHeader->mpSpriteLists[ i ].mSpriteCount; j++ )
		{
			mFED_DELOC( mpSpriteLists[ i ].mppSprites[ j ] );
		}
		mFED_DELOC( mpSpriteLists[ i ].mppSprites );

		Endian_FromBigU16( &apHeader->mpSpriteLists[ i ].mSpriteCount );

		mFED_DELOC( mpSpriteLists[ i ].mppSprites );
	}

	for( i=0; i<apHeader->mSpriteGroupCount; i++ )
	{
		mFED_DELOC( mpSpriteGroups[ i ].mpSpriteLocked );
		mFED_DELOC( mpSpriteGroups[ i ].mpSpriteNormal );
		mFED_DELOC( mpSpriteGroups[ i ].mpSpriteSelected );
		mFED_DELOC( mpSpriteGroups[ i ].mpSpriteLockedSelected );
	}


	for( i=0; i<apHeader->mTextCount; i++ )
	{
		mFED_DELOC( mpTexts[ i ].mpFontGroup );
		mFED_DELOC( mpTexts[ i ].mpString );

		Endian_FromBigU16( &apHeader->mpTexts[ i ].mAlign );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mX       );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mY       );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mWidth  );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mHeight );
	}

	for( i=0; i<apHeader->mTransitionCount; i++ )
	{
		if( eFED_FADE_RGB != apHeader->mpTransitions[ i ].mFadeType )
		{
			mFED_DELOC( mpTransitions[ i ].mFadeColour );
		}
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeFrameCount );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeType );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeIndex );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeType );
	}

	for( i=0; i<apHeader->mVarCount; i++ )
	{
		mFED_DELOC( mpVars[ i ].mpName );
	}

	mFED_DELOC( mpAssets );
	mFED_DELOC( mpCalls );
	mFED_DELOC( mpControls );
	mFED_DELOC( mpControlLists );
	mFED_DELOC( mpFontGroups );
	mFED_DELOC( mpLists );
	mFED_DELOC( mpListItems );
	mFED_DELOC( mpLocks );
	mFED_DELOC( mpPages );
	mFED_DELOC( mpPageStyles );
	mFED_DELOC( mpSamples );
	mFED_DELOC( mpSliders );
	mFED_DELOC( mpSprites );
	mFED_DELOC( mpSpriteGroups );
	mFED_DELOC( mpSpriteLists );
	mFED_DELOC( mpTexts );
	mFED_DELOC( mpTransitions );
	mFED_DELOC( mpVars );

	Endian_FromBigU16( &apHeader->mAssetCount );
	Endian_FromBigU16( &apHeader->mCallCount );
	Endian_FromBigU16( &apHeader->mControlCount );
	Endian_FromBigU16( &apHeader->mControlListCount );
	Endian_FromBigU16( &apHeader->mFontGroupCount );
	Endian_FromBigU16( &apHeader->mListCount );
	Endian_FromBigU16( &apHeader->mListItemCount );
	Endian_FromBigU16( &apHeader->mLockCount );
	Endian_FromBigU16( &apHeader->mPageCount );
	Endian_FromBigU16( &apHeader->mPageStyleCount );
	Endian_FromBigU16( &apHeader->mSampleCount );
	Endian_FromBigU16( &apHeader->mSliderCount );
	Endian_FromBigU16( &apHeader->mSpriteCount );
	Endian_FromBigU16( &apHeader->mSpriteGroupCount );
	Endian_FromBigU16( &apHeader->mSpriteListCount );
	Endian_FromBigU16( &apHeader->mTextCount );
	Endian_FromBigU16( &apHeader->mTransitionCount );
	Endian_FromBigU16( &apHeader->mVarCount );
}

void	Fed_DelocateOld( sFedHeader * apHeader )
{
	U16	i, j;

	for( i = 0; i<apHeader->mAssetCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpAssets[ i ].mpFileName );
		Endian_FromBigU32( &apHeader->mpAssets[ i ].mpContext );
	}

	for( i = 0; i<apHeader->mCallCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mpPageReturn );
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mpCallVar );
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mCallValue );
	}

	for( i = 0; i<apHeader->mControlCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpLock );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpSprite );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpTitle );

		Endian_FromBigU32( &apHeader->mpControls[ i ].mpCall );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpLink );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpList );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpSlider );
		Endian_FromBigU32( &apHeader->mpControls[ i ].mpSetVar );

		Endian_FromBigU32( &apHeader->mpControls[ i ].mSetValue );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mControlType );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mY );
	}

	for( i = 0; i<apHeader->mControlListCount; i++ )
	{
		*(U32*)( &apHeader->mpControlLists[ i ].mppControls ) += (U32)apHeader;

		for( j = 0; j<apHeader->mpControlLists[ i ].mControlCount; j++ )
		{
			Endian_FromBigU32( &apHeader->mpControlLists[ i ].mppControls[ j ] );
		}
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlCount );
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlIndex );
		mFED_DELOC( mpControlLists[ i ].mppControls );
	}

	for( i = 0; i<apHeader->mFontGroupCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpFontGroups[ i ].mpFontLocked );
		Endian_FromBigU32( &apHeader->mpFontGroups[ i ].mpFontNormal );
		Endian_FromBigU32( &apHeader->mpFontGroups[ i ].mpFontSelected );
		Endian_FromBigU32( &apHeader->mpFontGroups[ i ].mpFontLockedSelected );
	}

	for( i = 0; i<apHeader->mListCount; i++ )
	{
		*(U32*)( &apHeader->mpLists[ i ].mppItems ) += (U32)apHeader;

		for( j = 0; j<apHeader->mpLists[ i ].mItemCount; j++ )
		{
			Endian_FromBigU32( &apHeader->mpLists[ i ].mppItems[ j ] );
		}
		Endian_FromBigU16( &apHeader->mpLists[ i ].mItemCount );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mY );

		Endian_FromBigU32( &apHeader->mpLists[ i ].mpVar );

		mFED_DELOC( mpLists[ i ].mppItems );
	}

	for( i = 0; i<apHeader->mListItemCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpListItems[ i ].mpSpriteGroup );
		Endian_FromBigU32( &apHeader->mpListItems[ i ].mpLock );
		Endian_FromBigU32( &apHeader->mpListItems[ i ].mpText );

		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mY );
	}

	for( i = 0; i<apHeader->mLockCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mpLockVar );
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mpVisVar );

		Endian_FromBigU32( &apHeader->mpLocks[ i ].mLockValue );
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mVisValue );

		Endian_FromBigU16( &apHeader->mpLocks[ i ].mLockCompare );
		Endian_FromBigU16( &apHeader->mpLocks[ i ].mVisCompare );
	}

	for( i = 0; i<apHeader->mPageCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpBG );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpCursor );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpPageStyle );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpPageParent );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpSampleMove );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpSampleSelect );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpTitle );

		Endian_FromBigU32( &apHeader->mpPages[ i ].mpControlList );
		Endian_FromBigU32( &apHeader->mpPages[ i ].mpSpriteList );

		Endian_FromBigU32( &apHeader->mpPages[ i ].mHash );
		Endian_FromBigU16( &apHeader->mpPages[ i ].mControlIndex );
	}

	for( i = 0; i<apHeader->mPageStyleCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpPageTitleFont );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpPageBG );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpCursor );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpControlFonts );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpIntroTrans );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpOutroTrans );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpSampleMove );
		Endian_FromBigU32( &apHeader->mpPageStyles[ i ].mpSampleSelect );
	}

	for( i = 0; i<apHeader->mSampleCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpSamples[ i ].mpAsset );
	}

	for( i = 0; i<apHeader->mSliderCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mpVar );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMax );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMin );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueAdd );

		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mX );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mY );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mWidth );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mHeight );
	}

	for( i = 0; i<apHeader->mSpriteCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpSprites[ i ].mpAsset );

		Endian_FromBigU32( &apHeader->mpSprites[ i ].mAnimSpeed );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mFrameBase );

		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mY );
	}

	for( i = 0; i<apHeader->mSpriteListCount; i++ )
	{
		*(U32*)( &apHeader->mpSpriteLists[ i ].mppSprites ) += (U32)apHeader;
		for( j = 0; j<apHeader->mpSpriteLists[ i ].mSpriteCount; j++ )
		{
			Endian_FromBigU32( &apHeader->mpSpriteLists[ i ].mppSprites[ j ] );
		}
		Endian_FromBigU32( &apHeader->mpSpriteLists[ i ].mppSprites );
		Endian_FromBigU16( &apHeader->mpSpriteLists[ i ].mSpriteCount );

		mFED_DELOC( mpSpriteLists[ i ].mppSprites );
	}

	for( i = 0; i<apHeader->mSpriteGroupCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpSpriteGroups[ i ].mpSpriteLocked );
		Endian_FromBigU32( &apHeader->mpSpriteGroups[ i ].mpSpriteNormal );
		Endian_FromBigU32( &apHeader->mpSpriteGroups[ i ].mpSpriteSelected );
		Endian_FromBigU32( &apHeader->mpSpriteGroups[ i ].mpSpriteLockedSelected );
	}


	for( i = 0; i<apHeader->mTextCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpTexts[ i ].mpFontGroup );
		Endian_FromBigU32( &apHeader->mpTexts[ i ].mpString );

		Endian_FromBigU16( &apHeader->mpTexts[ i ].mAlign );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mX );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mY );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mWidth );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mHeight );
	}

	for( i = 0; i<apHeader->mTransitionCount; i++ )
	{
		if( eFED_FADE_RGB != apHeader->mpTransitions[ i ].mFadeType )
		{
			Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeColour );
		}
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeFrameCount );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeType );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeIndex );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeType );
	}

	for( i = 0; i<apHeader->mVarCount; i++ )
	{
		Endian_FromBigU32( &apHeader->mpVars[ i ].mpName );
	}

	mFED_DELOC( mpAssets );
	mFED_DELOC( mpCalls );
	mFED_DELOC( mpControls );
	mFED_DELOC( mpControlLists );
	mFED_DELOC( mpFontGroups );
	mFED_DELOC( mpLists );
	mFED_DELOC( mpListItems );
	mFED_DELOC( mpLocks );
	mFED_DELOC( mpPages );
	mFED_DELOC( mpPageStyles );
	mFED_DELOC( mpSamples );
	mFED_DELOC( mpSliders );
	mFED_DELOC( mpSprites );
	mFED_DELOC( mpSpriteGroups );
	mFED_DELOC( mpSpriteLists );
	mFED_DELOC( mpTexts );
	mFED_DELOC( mpTransitions );
	mFED_DELOC( mpVars );

	Endian_FromBigU16( &apHeader->mAssetCount );
	Endian_FromBigU16( &apHeader->mCallCount );
	Endian_FromBigU16( &apHeader->mControlCount );
	Endian_FromBigU16( &apHeader->mControlListCount );
	Endian_FromBigU16( &apHeader->mFontGroupCount );
	Endian_FromBigU16( &apHeader->mListCount );
	Endian_FromBigU16( &apHeader->mListItemCount );
	Endian_FromBigU16( &apHeader->mLockCount );
	Endian_FromBigU16( &apHeader->mPageCount );
	Endian_FromBigU16( &apHeader->mPageStyleCount );
	Endian_FromBigU16( &apHeader->mSampleCount );
	Endian_FromBigU16( &apHeader->mSliderCount );
	Endian_FromBigU16( &apHeader->mSpriteCount );
	Endian_FromBigU16( &apHeader->mSpriteGroupCount );
	Endian_FromBigU16( &apHeader->mSpriteListCount );
	Endian_FromBigU16( &apHeader->mTextCount );
	Endian_FromBigU16( &apHeader->mTransitionCount );
	Endian_FromBigU16( &apHeader->mVarCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_Relocate( sFedHeader * apHeader )
* ACTION   : Fed_Relocate
* CREATION : 04.02.2004 PNK
*-----------------------------------------------------------------------------------*/


void	Fed_Relocate( sFedHeader * apHeader )
{
	U16	i,j;

	Endian_FromBigU32( &apHeader->mID );
	Endian_FromBigU32( &apHeader->mVersion );

	if( apHeader->mVersion < dFED_VERSION_NEW )
	{
		Fed_RelocateOld( apHeader );
		return;
	}


	mFED_RELOC( mpAssets );
	mFED_RELOC( mpCalls );
	mFED_RELOC( mpControls );
	mFED_RELOC( mpControlLists );
	mFED_RELOC( mpFontGroups );
	mFED_RELOC( mpListItems );
	mFED_RELOC( mpLists );
	mFED_RELOC( mpLocks );
	mFED_RELOC( mpPages );
	mFED_RELOC( mpPageStyles );
	mFED_RELOC( mpSamples );
	mFED_RELOC( mpSliders );
	mFED_RELOC( mpSprites );
	mFED_RELOC( mpSpriteGroups );
	mFED_RELOC( mpSpriteLists );
	mFED_RELOC( mpTexts );
	mFED_RELOC( mpTransitions );
	mFED_RELOC( mpVars );

	Endian_FromBigU16( &apHeader->mAssetCount );
	Endian_FromBigU16( &apHeader->mCallCount );
	Endian_FromBigU16( &apHeader->mControlCount );
	Endian_FromBigU16( &apHeader->mControlListCount );
	Endian_FromBigU16( &apHeader->mFontGroupCount );
	Endian_FromBigU16( &apHeader->mListCount );
	Endian_FromBigU16( &apHeader->mListItemCount );
	Endian_FromBigU16( &apHeader->mLockCount );
	Endian_FromBigU16( &apHeader->mPageCount );
	Endian_FromBigU16( &apHeader->mPageStyleCount );
	Endian_FromBigU16( &apHeader->mSampleCount );
	Endian_FromBigU16( &apHeader->mSliderCount );
	Endian_FromBigU16( &apHeader->mSpriteCount );
	Endian_FromBigU16( &apHeader->mSpriteGroupCount );
	Endian_FromBigU16( &apHeader->mSpriteListCount );
	Endian_FromBigU16( &apHeader->mTextCount );
	Endian_FromBigU16( &apHeader->mTransitionCount );
	Endian_FromBigU16( &apHeader->mVarCount );

	for( i=0; i<apHeader->mAssetCount; i++ )
	{
		mFED_RELOC( mpAssets[ i ].mpFileName );
		mFED_RELOC( mpAssets[ i ].mpContext );
	}

	for( i=0; i<apHeader->mCallCount; i++ )
	{
		mFED_RELOC( mpCalls[ i ].mpPageReturn );
		mFED_RELOC( mpCalls[ i ].mpCallVar    );
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mCallValue );
	}

	for( i=0; i<apHeader->mControlCount; i++ )
	{
		mFED_RELOC( mpControls[ i ].mpLock   );
		mFED_RELOC( mpControls[ i ].mpSprite );
		mFED_RELOC( mpControls[ i ].mpTitle  );

		mFED_RELOC( mpControls[ i ].mpCall   );
		mFED_RELOC( mpControls[ i ].mpLink   );
		mFED_RELOC( mpControls[ i ].mpList   );
		mFED_RELOC( mpControls[ i ].mpSlider );
		mFED_RELOC( mpControls[ i ].mpSetVar );

		Endian_FromBigU32( &apHeader->mpControls[ i ].mSetValue );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mControlType );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mY  );
	}


	for( i=0; i<apHeader->mControlListCount; i++ )
	{
		mFED_RELOC( mpControlLists[ i ].mppControls );

		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlCount );
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlIndex );
		for( j=0; j<apHeader->mpControlLists[ i ].mControlCount; j++ )
		{
			mFED_RELOC( mpControlLists[ i ].mppControls[ j ] );
		}
	}

	for( i=0; i<apHeader->mFontGroupCount; i++ )
	{
		mFED_RELOC( mpFontGroups[ i ].mpFontLocked );
		mFED_RELOC( mpFontGroups[ i ].mpFontNormal );
		mFED_RELOC( mpFontGroups[ i ].mpFontSelected );
		mFED_RELOC( mpFontGroups[ i ].mpFontLockedSelected );
	}

	for( i=0; i<apHeader->mListCount; i++ )
	{
		mFED_RELOC( mpLists[ i ].mppItems );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mItemCount );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mY  );
		for( j=0; j<apHeader->mpLists[ i ].mItemCount; j++ )
		{
			mFED_RELOC( mpLists[ i ].mppItems[ j ] );
		}

		mFED_RELOC( mpLists[ i ].mpVar );
	}

	for( i=0; i<apHeader->mListItemCount; i++ )
	{
		mFED_RELOC( mpListItems[ i ].mpSpriteGroup );
		mFED_RELOC( mpListItems[ i ].mpLock );
		mFED_RELOC( mpListItems[ i ].mpText );

		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mY  );
	}

	for( i=0; i<apHeader->mLockCount; i++ )
	{
		mFED_RELOC( mpLocks[ i ].mpLockVar );
		mFED_RELOC( mpLocks[ i ].mpVisVar );

		Endian_FromBigU32( &apHeader->mpLocks[ i ].mLockValue );
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mVisValue  );

		Endian_FromBigU16( &apHeader->mpLocks[ i ].mLockCompare );
		Endian_FromBigU16( &apHeader->mpLocks[ i ].mVisCompare  );
	}

	for( i=0; i<apHeader->mPageCount; i++ )
	{
		mFED_RELOC( mpPages[ i ].mpBG );
		mFED_RELOC( mpPages[ i ].mpCursor );
		mFED_RELOC( mpPages[ i ].mpPageStyle );

		mFED_RELOC( mpPages[ i ].mpSampleMove );
		mFED_RELOC( mpPages[ i ].mpSampleSelect );

		mFED_RELOC( mpPages[ i ].mpPageParent );
		mFED_RELOC( mpPages[ i ].mpTitle );

		mFED_RELOC( mpPages[ i ].mpControlList );
		mFED_RELOC( mpPages[ i ].mpSpriteList );

		Endian_FromBigU32( &apHeader->mpPages[ i ].mHash         );
		Endian_FromBigU16( &apHeader->mpPages[ i ].mControlIndex );
	}

	for( i=0; i<apHeader->mPageStyleCount; i++ )
	{
		mFED_RELOC( mpPageStyles[ i ].mpPageTitleFont );
		mFED_RELOC( mpPageStyles[ i ].mpPageBG );
		mFED_RELOC( mpPageStyles[ i ].mpCursor );
		mFED_RELOC( mpPageStyles[ i ].mpControlFonts );
		mFED_RELOC( mpPageStyles[ i ].mpIntroTrans );
		mFED_RELOC( mpPageStyles[ i ].mpOutroTrans );
		mFED_RELOC( mpPageStyles[ i ].mpSampleMove );
		mFED_RELOC( mpPageStyles[ i ].mpSampleSelect );
	}

	for( i=0; i<apHeader->mSampleCount; i++ )
	{
		mFED_RELOC( mpSamples[ i ].mpAsset );
	}

	for( i=0; i<apHeader->mSliderCount; i++ )
	{
		mFED_RELOC( mpSliders[ i ].mpVar );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMax );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMin );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueAdd );

		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mX       );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mY       );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mWidth  );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mHeight );
	}

	for( i=0; i<apHeader->mSpriteCount; i++ )
	{
		mFED_RELOC( mpSprites[ i ].mpAsset );

		Endian_FromBigU32( &apHeader->mpSprites[ i ].mAnimSpeed );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mFrameBase  );

		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mX  );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mY  );

		apHeader->mpSprites[ i ].mFrame.w.w1 = apHeader->mpSprites[ i ].mFrameBase;
		apHeader->mpSprites[ i ].mFrame.w.w0 = 0;
	}

	for( i=0; i<apHeader->mSpriteGroupCount; i++ )
	{
		mFED_RELOC( mpSpriteGroups[ i ].mpSpriteLocked );
		mFED_RELOC( mpSpriteGroups[ i ].mpSpriteNormal );
		mFED_RELOC( mpSpriteGroups[ i ].mpSpriteSelected );
		mFED_RELOC( mpSpriteGroups[ i ].mpSpriteLockedSelected );
	}

	for( i=0; i<apHeader->mSpriteListCount; i++ )
	{
		mFED_RELOC( mpSpriteLists[ i ].mppSprites );
		Endian_FromBigU16( &apHeader->mpSpriteLists[ i ].mSpriteCount );

		for( j=0; j<apHeader->mpSpriteLists[ i ].mSpriteCount; j++ )
		{
			mFED_RELOC( mpSpriteLists[ i ].mppSprites[ j ] );
		}
	}

	for( i=0; i<apHeader->mTextCount; i++ )
	{
		mFED_RELOC( mpTexts[ i ].mpFontGroup );
		mFED_RELOC( mpTexts[ i ].mpString );

		Endian_FromBigU16( &apHeader->mpTexts[ i ].mAlign );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mX       );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mY       );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mWidth  );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mHeight );
	}

	for( i=0; i<apHeader->mTransitionCount; i++ )
	{
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeFrameCount );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeType );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeIndex );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeType );
		if( eFED_FADE_RGB != apHeader->mpTransitions[ i ].mFadeType )
		{
			Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeColour );
		}
	}

	for( i=0; i<apHeader->mVarCount; i++ )
	{
		mFED_RELOC( mpVars[ i ].mpName );
	}
}


void	Fed_RelocateOld( sFedHeader * apHeader )
{
	U16	i, j;

	mFED_RELOC( mpAssets );
	mFED_RELOC( mpCalls );
	mFED_RELOC( mpControls );
	mFED_RELOC( mpControlLists );
	mFED_RELOC( mpFontGroups );
	mFED_RELOC( mpListItems );
	mFED_RELOC( mpLists );
	mFED_RELOC( mpLocks );
	mFED_RELOC( mpPages );
	mFED_RELOC( mpPageStyles );
	mFED_RELOC( mpSamples );
	mFED_RELOC( mpSliders );
	mFED_RELOC( mpSprites );
	mFED_RELOC( mpSpriteGroups );
	mFED_RELOC( mpSpriteLists );
	mFED_RELOC( mpTexts );
	mFED_RELOC( mpTransitions );
	mFED_RELOC( mpVars );

	Endian_FromBigU16( &apHeader->mAssetCount );
	Endian_FromBigU16( &apHeader->mCallCount );
	Endian_FromBigU16( &apHeader->mControlCount );
	Endian_FromBigU16( &apHeader->mControlListCount );
	Endian_FromBigU16( &apHeader->mFontGroupCount );
	Endian_FromBigU16( &apHeader->mListCount );
	Endian_FromBigU16( &apHeader->mListItemCount );
	Endian_FromBigU16( &apHeader->mLockCount );
	Endian_FromBigU16( &apHeader->mPageCount );
	Endian_FromBigU16( &apHeader->mPageStyleCount );
	Endian_FromBigU16( &apHeader->mSampleCount );
	Endian_FromBigU16( &apHeader->mSliderCount );
	Endian_FromBigU16( &apHeader->mSpriteCount );
	Endian_FromBigU16( &apHeader->mSpriteGroupCount );
	Endian_FromBigU16( &apHeader->mSpriteListCount );
	Endian_FromBigU16( &apHeader->mTextCount );
	Endian_FromBigU16( &apHeader->mTransitionCount );
	Endian_FromBigU16( &apHeader->mVarCount );

	for( i = 0; i<apHeader->mAssetCount; i++ )
	{
		mFED_RELOC( mpAssets[ i ].mpFileName );
		mFED_RELOC( mpAssets[ i ].mpContext );
	}

	for( i = 0; i<apHeader->mCallCount; i++ )
	{
		mFED_LIST_RELOC( mpCalls[ i ].mpPageReturn, mpPages );
		mFED_LIST_RELOC( mpCalls[ i ].mpCallVar, mpVars );
		Endian_FromBigU32( &apHeader->mpCalls[ i ].mCallValue );
	}

	for( i = 0; i<apHeader->mControlCount; i++ )
	{
		mFED_LIST_RELOC( mpControls[ i ].mpLock, mpLocks );
		mFED_LIST_RELOC( mpControls[ i ].mpSprite, mpSprites );
		mFED_LIST_RELOC( mpControls[ i ].mpTitle, mpTexts );

		mFED_LIST_RELOC( mpControls[ i ].mpCall, mpCalls );
		mFED_LIST_RELOC( mpControls[ i ].mpLink, mpPages );
		mFED_LIST_RELOC( mpControls[ i ].mpList, mpLists );
		mFED_LIST_RELOC( mpControls[ i ].mpSlider, mpSliders );
		mFED_LIST_RELOC( mpControls[ i ].mpSetVar, mpVars );

		Endian_FromBigU32( &apHeader->mpControls[ i ].mSetValue );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mControlType );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpControls[ i ].mPos.mY );
	}


	for( i = 0; i<apHeader->mControlListCount; i++ )
	{
		mFED_RELOC( mpControlLists[ i ].mppControls );

		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlCount );
		Endian_FromBigU16( &apHeader->mpControlLists[ i ].mControlIndex );
		for( j = 0; j<apHeader->mpControlLists[ i ].mControlCount; j++ )
		{
			mFED_LIST_RELOC( mpControlLists[ i ].mppControls[ j ], mpControls );
		}
	}

	for( i = 0; i<apHeader->mFontGroupCount; i++ )
	{
		mFED_LIST_RELOC( mpFontGroups[ i ].mpFontLocked, mpAssets );
		mFED_LIST_RELOC( mpFontGroups[ i ].mpFontNormal, mpAssets );
		mFED_LIST_RELOC( mpFontGroups[ i ].mpFontSelected, mpAssets );
		mFED_LIST_RELOC( mpFontGroups[ i ].mpFontLockedSelected, mpAssets );
	}

	for( i = 0; i<apHeader->mListCount; i++ )
	{
		mFED_RELOC( mpLists[ i ].mppItems );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mItemCount );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpLists[ i ].mPos.mY );
		for( j = 0; j<apHeader->mpLists[ i ].mItemCount; j++ )
		{
			mFED_LIST_RELOC( mpLists[ i ].mppItems[ j ], mpListItems );
		}

		mFED_LIST_RELOC( mpLists[ i ].mpVar, mpVars );
	}

	for( i = 0; i<apHeader->mListItemCount; i++ )
	{
		mFED_LIST_RELOC( mpListItems[ i ].mpSpriteGroup, mpSpriteGroups );
		mFED_LIST_RELOC( mpListItems[ i ].mpLock, mpLocks );
		mFED_LIST_RELOC( mpListItems[ i ].mpText, mpTexts );

		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpListItems[ i ].mPos.mY );
	}

	for( i = 0; i<apHeader->mLockCount; i++ )
	{
		mFED_LIST_RELOC( mpLocks[ i ].mpLockVar, mpVars );
		mFED_LIST_RELOC( mpLocks[ i ].mpVisVar, mpVars );

		Endian_FromBigU32( &apHeader->mpLocks[ i ].mLockValue );
		Endian_FromBigU32( &apHeader->mpLocks[ i ].mVisValue );

		Endian_FromBigU16( &apHeader->mpLocks[ i ].mLockCompare );
		Endian_FromBigU16( &apHeader->mpLocks[ i ].mVisCompare );
	}

	for( i = 0; i<apHeader->mPageCount; i++ )
	{
		mFED_LIST_RELOC( mpPages[ i ].mpBG, mpAssets );
		mFED_LIST_RELOC( mpPages[ i ].mpCursor, mpSprites );
		mFED_LIST_RELOC( mpPages[ i ].mpPageStyle, mpPageStyles );

		mFED_LIST_RELOC( mpPages[ i ].mpSampleMove, mpSamples );
		mFED_LIST_RELOC( mpPages[ i ].mpSampleSelect, mpSamples );

		mFED_LIST_RELOC( mpPages[ i ].mpPageParent, mpPages );
		mFED_LIST_RELOC( mpPages[ i ].mpTitle, mpTexts );

		mFED_LIST_RELOC( mpPages[ i ].mpControlList, mpControlLists );
		mFED_LIST_RELOC( mpPages[ i ].mpSpriteList, mpSpriteLists );

		Endian_FromBigU32( &apHeader->mpPages[ i ].mHash );
		Endian_FromBigU16( &apHeader->mpPages[ i ].mControlIndex );
	}

	for( i = 0; i<apHeader->mPageStyleCount; i++ )
	{
		mFED_LIST_RELOC( mpPageStyles[ i ].mpPageTitleFont, mpAssets );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpPageBG, mpAssets );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpCursor, mpSprites );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpControlFonts, mpFontGroups );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpIntroTrans, mpTransitions );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpOutroTrans, mpTransitions );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpSampleMove, mpSamples );
		mFED_LIST_RELOC( mpPageStyles[ i ].mpSampleSelect, mpSamples );
	}

	for( i = 0; i<apHeader->mSampleCount; i++ )
	{
		mFED_LIST_RELOC( mpSamples[ i ].mpAsset, mpAssets );
	}

	for( i = 0; i<apHeader->mSliderCount; i++ )
	{
		mFED_LIST_RELOC( mpSliders[ i ].mpVar, mpVars );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMax );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueMin );
		Endian_FromBigU32( &apHeader->mpSliders[ i ].mValueAdd );

		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mX );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mPos.mY );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mWidth );
		Endian_FromBigU16( &apHeader->mpSliders[ i ].mBox.mSize.mHeight );
	}

	for( i = 0; i<apHeader->mSpriteCount; i++ )
	{
		mFED_LIST_RELOC( mpSprites[ i ].mpAsset, mpAssets );

		Endian_FromBigU32( &apHeader->mpSprites[ i ].mAnimSpeed );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mFrameBase );

		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mX );
		Endian_FromBigU16( &apHeader->mpSprites[ i ].mPos.mY );

		apHeader->mpSprites[ i ].mFrame.w.w1 = apHeader->mpSprites[ i ].mFrameBase;
		apHeader->mpSprites[ i ].mFrame.w.w0 = 0;
	}

	for( i = 0; i<apHeader->mSpriteGroupCount; i++ )
	{
		mFED_LIST_RELOC( mpSpriteGroups[ i ].mpSpriteLocked, mpSprites );
		mFED_LIST_RELOC( mpSpriteGroups[ i ].mpSpriteNormal, mpSprites );
		mFED_LIST_RELOC( mpSpriteGroups[ i ].mpSpriteSelected, mpSprites );
		mFED_LIST_RELOC( mpSpriteGroups[ i ].mpSpriteLockedSelected, mpSprites );
	}

	for( i = 0; i<apHeader->mSpriteListCount; i++ )
	{
		mFED_RELOC( mpSpriteLists[ i ].mppSprites );
		Endian_FromBigU16( &apHeader->mpSpriteLists[ i ].mSpriteCount );

		for( j = 0; j<apHeader->mpSpriteLists[ i ].mSpriteCount; j++ )
		{
			mFED_LIST_RELOC( mpSpriteLists[ i ].mppSprites[ j ], mpSprites );
		}
	}

	for( i = 0; i<apHeader->mTextCount; i++ )
	{
		mFED_LIST_RELOC( mpTexts[ i ].mpFontGroup, mpFontGroups );
		mFED_RELOC( mpTexts[ i ].mpString );

		Endian_FromBigU16( &apHeader->mpTexts[ i ].mAlign );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mX );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mPos.mY );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mWidth );
		Endian_FromBigU16( &apHeader->mpTexts[ i ].mBox.mSize.mHeight );
	}

	for( i = 0; i<apHeader->mTransitionCount; i++ )
	{
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeFrameCount );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeType );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeIndex );
		Endian_FromBigU16( &apHeader->mpTransitions[ i ].mWipeType );
		if( eFED_FADE_RGB != apHeader->mpTransitions[ i ].mFadeType )
		{
			Endian_FromBigU16( &apHeader->mpTransitions[ i ].mFadeColour );
		}
	}

	for( i = 0; i<apHeader->mVarCount; i++ )
	{
		mFED_RELOC( mpVars[ i ].mpName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Fed_Init(sFedHeader * apHeader)
* ACTION   : Fed_Init
* CREATION : 18.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void Fed_Init(sFedHeader * apHeader)
{
	U16	i;

	for( i=0; i<apHeader->mAssetCount; i++ )
	{
		AssetClient_Init(  &apHeader->mpAssets[ i ].mAsset, apHeader->mpAssets[ i ].mpFileName, apHeader->mpAssets[ i ].mpContext, (void**)&apHeader->mpAssets[ i ].mpData );
	}


	gpFedHeader = apHeader;
	gpFedPage   = &apHeader->mpPages[ 0 ];
	gpFedPage->mRedrawFlag = 2;

	for( i=0; i<apHeader->mVarCount; i++ )
	{
		apHeader->mpVars[ i ].mpVar = HashTree_VarRegister( gpFedHashTree, apHeader->mpVars[ i ].mpName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_DeInit( sFedHeader * apHeader,sHashTree * apTree )
* ACTION   : Fed_DeInit
* CREATION : 04.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_DeInit( sFedHeader * apHeader )
{
	U16	i;

	for( i=0; i<apHeader->mAssetCount; i++ )
	{
		AssetClient_DeInit( &apHeader->mpAssets[ i ].mAsset );
	}

	for( i=0; i<apHeader->mVarCount; i++ )
	{
		HashTree_VarUnRegister( gpFedHashTree, apHeader->mpVars[ i ].mpVar );
	}

	gpFedPage = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_Update( const sInput * apInput )
* ACTION   : Fed_Update
* CREATION : 04.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U8	Fed_Update( const sInput * apInput )
{
	U8					lExitFlag;
	sFedCall *			lpCall;
	sFedControl *		lpControl;
	sFedControlList *	lpControlList;

	lExitFlag = 0;

	if( gpFedPage )
	{

		switch( gFedMode )
		{
		case	eFED_MODE_INTRO:
			if( RenderFed_IsTransitionComplete() )
			{
				gFedMode = eFED_MODE_NORMAL;
			}
			break;

		case	eFED_MODE_OUTRO:
			if( RenderFed_IsTransitionComplete() )
			{
				Fed_SetIntro();
			}
			break;

		case	eFED_MODE_NORMAL:
			lpControlList = gpFedPage->mpControlList;
			if( lpControlList )
			{
				if( lpControlList->mControlIndex > lpControlList->mControlCount )
				{
					lpControlList->mControlIndex = 0;
				}
			}

			if( apInput->mKeyStatus[ eINPUTKEY_UP ] & eINPUTKEYSTATUS_HIT )
			{
				if( lpControlList )
				{
					if( lpControlList->mControlIndex )
					{
						Fed_ControlListMove( lpControlList, -1 );
					}
				}
			}

			if( apInput->mKeyStatus[ eINPUTKEY_DOWN ] & eINPUTKEYSTATUS_HIT )
			{
				if( lpControlList )
				{
					if( (lpControlList->mControlIndex+1) < lpControlList->mControlCount )
					{
						Fed_ControlListMove( lpControlList, 1 );
					}
				}
			}

			if( lpControlList )
			{
				lpControl = lpControlList->mppControls[ lpControlList->mControlIndex ];
			}
			else
			{
				lpControl = 0;
			}

			if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HIT )
			{
				gFedKeyWait = dFED_KEYWAIT0;
				if( lpControl )
				{
					switch( lpControl->mControlType )
					{
					case	eFED_CONTROL_LIST:
						lpControl->mRedrawFlag = 2;
						Fed_ListMove( lpControl->mpList, -1 );
						break;

					case	eFED_CONTROL_SLIDER:
						lpControl->mRedrawFlag = 2;
						Fed_SliderMove( lpControl->mpSlider, -1 );
						break;
					}
				}
			}
			else if( apInput->mKeyStatus[ eINPUTKEY_LEFT ] & eINPUTKEYSTATUS_HELD )
			{
				if( gFedKeyWait )
				{
					gFedKeyWait--;
				}
				else
				{
					gFedKeyWait = dFED_KEYWAIT1;
					if( lpControl )
					{
						switch( lpControl->mControlType )
						{
						case	eFED_CONTROL_SLIDER:
							lpControl->mRedrawFlag = 2;
							Fed_SliderMove( lpControl->mpSlider, -1 );
							break;
						}
					}
				}
			}

			if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HIT )
			{
				gFedKeyWait = dFED_KEYWAIT0;
				if( lpControl )
				{
					switch( lpControl->mControlType )
					{
					case	eFED_CONTROL_LIST:
						lpControl->mRedrawFlag = 2;
						Fed_ListMove( lpControl->mpList, 1 );
						break;

					case	eFED_CONTROL_SLIDER:
						lpControl->mRedrawFlag = 2;
						Fed_SliderMove( lpControl->mpSlider, 1 );
						break;
					}
				}
			}
			else if( apInput->mKeyStatus[ eINPUTKEY_RIGHT ] & eINPUTKEYSTATUS_HELD )
			{
				if( gFedKeyWait )
				{
					gFedKeyWait--;
				}
				else
				{
					gFedKeyWait = dFED_KEYWAIT1;
					if( lpControl )
					{
						switch( lpControl->mControlType )
						{
						case	eFED_CONTROL_SLIDER:
							lpControl->mRedrawFlag = 2;
							Fed_SliderMove( lpControl->mpSlider, 1 );
							break;
						}
					}
				}
			}

			if( apInput->mKeyStatus[ eINPUTKEY_FIREA ] & eINPUTKEYSTATUS_HIT )
			{
				if( lpControl )
				{
					if( lpControl->mpSetVar )
					{
						HashTree_VarWrite( lpControl->mpSetVar->mpVar, &lpControl->mSetValue );
					}

					switch( lpControl->mControlType )
					{
					case	eFED_CONTROL_LINK:
						Fed_SampleSelectPlay();
						if( lpControl->mpLink )
						{
							Fed_SetOutro();
							gpFedPage = lpControl->mpLink;
							gpFedPage->mRedrawFlag = 2;
						}
						break;

					case	eFED_CONTROL_CALL:
						Fed_SampleSelectPlay();
						lpCall = lpControl->mpCall;
						if( lpCall )
						{
							HashTree_VarWrite( lpCall->mpCallVar->mpVar, &lpCall->mCallValue );
							lExitFlag = 1;
						}
						break;

					case	eFED_CONTROL_LIST:
						lpControl->mRedrawFlag = 2;
						Fed_ListMove( lpControl->mpList, 1 );
						break;

					case	eFED_CONTROL_SLIDER:
						lpControl->mRedrawFlag = 2;
						Fed_SliderMove( lpControl->mpSlider, 1 );
						break;
					}
				}
			}
			break;
		}
	}
	return( lExitFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_ListMove( sFedList * apList,const S32 aDir )
* ACTION   : Fed_ListMove
* CREATION : 04.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_ListMove( sFedList * apList,const S32 aDir )
{
	S32		lIndex;
	S32		lOld;
	S32 *	lpData;
	sFedLockStatus	lLockStatus;

	DebugLog_Printf1( "Fed_ListMove() dir:%ld", aDir );

	if( (apList->mpVar) && (apList->mpVar->mpVar) )
	{
		lpData = (S32*)apList->mpVar->mpVar->mpData;
		if( lpData )
		{
			lIndex  =  *lpData;
			lOld    = lIndex;
			do
			{
				lIndex  += aDir;

				DebugLog_Printf2( "Fed_ListMove() old:%ld new:%ld", *lpData, lIndex );

				if( lIndex >= (S32)apList->mItemCount )
				{
					lIndex = 0;
				}

				if( lIndex < 0 )
				{
					lIndex = apList->mItemCount - 1;
				}
				Fed_LockEvaluate( apList->mppItems[ lIndex ]->mpLock, &lLockStatus );
			} while( (lLockStatus.mLockedFlag || (!lLockStatus.mVisFlag)) && (lOld != lIndex) );
			HashTree_VarWrite( apList->mpVar->mpVar, &lIndex );
			Fed_SampleMovePlay();
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SliderMove( sFedSlider * apSlider,const S32 aDir )
* ACTION   : Fed_SliderMove
* CREATION : 04.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SliderMove( sFedSlider * apSlider,const S32 aDir )
{
	S32	lVal;
	S32	lValMin,lValMax;
	S32	lValAdd;
	S32 *	lpData;


	lValMax = apSlider->mValueMax;
	lValMin = apSlider->mValueMin;
	if( apSlider->mValueAdd )
	{
		lValAdd = aDir * apSlider->mValueAdd;
	}
	else
	{
		lValAdd = aDir;
	}

	DebugLog_Printf2( "Fed_SliderMove() min:%ld max:%ld", lValMin, lValMax );

	if( apSlider->mpVar )
	{
		if( apSlider->mpVar->mpVar )
		{
			lpData  = (S32*)apSlider->mpVar->mpVar->mpData;
			lVal    = *lpData;

			lVal   += lValAdd;
			if( lVal > lValMax )
			{
				lVal = lValMax;
			}
			if( lVal < lValMin )
			{
				lVal = lValMin;
			}
			HashTree_VarWrite( apSlider->mpVar->mpVar, &lVal );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_GetpActivePage( void )
* ACTION   : Fed_GetpActivePage
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sFedPage *	Fed_GetpActivePage( void )
{
	return( gpFedPage );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_Begin( sFedHeader * apHeader,const char * apPageName )
* ACTION   : Fed_Begin
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_Begin( sFedHeader * apHeader,const char * apPageName )
{
	U16	i;
	U32	lHash;

	lHash = HashTree_BuildHash( apPageName );

	gpFedPage = &apHeader->mpPages[ 0 ];
	for( i=0; i<apHeader->mPageCount; i++ )
	{
		if( apHeader->mpPages[ i ].mHash == lHash )
		{
			gpFedPage = &apHeader->mpPages[ i ];
		}
	}
	gpFedPage->mRedrawFlag = 2;
	Fed_SetIntro();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SetOutro( void )
* ACTION   : Fed_SetOutro
* CREATION : 11.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SetOutro( void )
{
	sFedTransition *	lpTrans;

	gFedMode = eFED_MODE_NORMAL;
	if( gpFedPage )
	{
		if( gpFedPage->mpPageStyle )
		{
			lpTrans = gpFedPage->mpPageStyle->mpOutroTrans;
		}
		else
		{
			lpTrans = 0;
		}
		if( lpTrans )
		{
			RenderFed_OutroInit( lpTrans );
			gFedMode = eFED_MODE_OUTRO;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SetIntro( void )
* ACTION   : Fed_SetIntro
* CREATION : 11.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SetIntro( void )
{
	sFedTransition *	lpTrans;

	gFedMode = eFED_MODE_NORMAL;
	if( gpFedPage )
	{
		if( gpFedPage->mpPageStyle )
		{
			lpTrans = gpFedPage->mpPageStyle->mpIntroTrans;
		}
		else
		{
			lpTrans = 0;
		}
		if( lpTrans )
		{
			RenderFed_IntroInit( lpTrans );
			gFedMode = eFED_MODE_INTRO;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_LockEvaluate( sFedLock * apLock,sFedLockStatus * apRFL )
* ACTION   : Fed_LockEvaluate
* CREATION : 05.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_LockEvaluate( sFedLock * apLock,sFedLockStatus * apRFL )
{
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
			if( apLock->mpVisVar->mpVar )
			{
				lpData = (S32*)apLock->mpVisVar->mpVar->mpData;
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
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_ControlListMove( sFedControlList * apList,const S16 aDir )
* ACTION   : Fed_ControlListMove
* CREATION : 14.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_ControlListMove( sFedControlList * apList,const S16 aDir )
{
	sFedLockStatus		lStatus;
	U16					lClipFlag;
	S16					lIndexNew;

	lClipFlag = 0;
	lIndexNew = apList->mControlIndex;

	apList->mppControls[ apList->mControlIndex ]->mRedrawFlag = 2;

	do
	{

		lIndexNew = (S16)(lIndexNew + aDir);
		if( lIndexNew >= (S16)apList->mControlCount )
		{
			lIndexNew = (S16)(apList->mControlCount - 1);
			lClipFlag = 1;
		}
		if( lIndexNew < 0 )
		{
			lIndexNew = 0;
			lClipFlag = 1;
		}

		Fed_LockEvaluate( apList->mppControls[ lIndexNew ]->mpLock, &lStatus );
	} while( (!lStatus.mVisFlag) && (!lClipFlag) );

	if( (!lClipFlag) && (lStatus.mVisFlag) )
	{
		apList->mppControls[ lIndexNew ]->mRedrawFlag = 2;
		apList->mControlIndex = lIndexNew;
	}

	Fed_SampleMovePlay();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SetPage( const char * apPageName )
* ACTION   : Fed_SetPage
* CREATION : 21.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SetPage( const char * apPageName )
{
	U32	lHash;
	U16	i;

	lHash = HashList_BuildHash( apPageName );
	if( gpFedHeader )
	{
		for( i=0; i<gpFedHeader->mPageCount; i++ )
		{
			if( lHash == gpFedHeader->mpPages[ i ].mHash )
			{
				gpFedPage = &gpFedHeader->mpPages[ i ];
				gpFedPage->mRedrawFlag = 2;
				return;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_ForceRedraw( void )
* ACTION   : Fed_ForceRedraw
* CREATION : 21.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_ForceRedraw( void )
{
	if( gpFedPage )
	{
		gpFedPage->mRedrawFlag = 2;
		Fed_SetIntro();
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void Fed_SetpHashTree(sHashTree * apTree)
* ACTION   : Fed_SetpHashTree
* CREATION : 18.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void Fed_SetpHashTree(sHashTree * apTree)
{
	gpFedHashTree = apTree;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SampleMovePlay( void )
* ACTION   : Fed_SampleMovePlay
* CREATION : 24.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SampleMovePlay( void )
{
	sAudioDmaSound *	lpSpl;

	lpSpl = 0;

	if( Fed_GetpActivePage()->mpPageStyle->mpSampleMove )
	{
		if( Fed_GetpActivePage()->mpPageStyle->mpSampleMove->mpAsset )
		{
			lpSpl = (sAudioDmaSound*)Fed_GetpActivePage()->mpPageStyle->mpSampleMove->mpAsset->mpData;
		}
	}

	if( Fed_GetpActivePage()->mpSampleMove )
	{
		if( Fed_GetpActivePage()->mpSampleMove->mpAsset )
		{
			if( Fed_GetpActivePage()->mpSampleMove->mpAsset->mpData )
			{
				lpSpl = (sAudioDmaSound*)Fed_GetpActivePage()->mpSampleMove->mpAsset->mpData;
			}
		}
	}

	AudioMixer_PlaySample( lpSpl, eAMIXER_PAN_CENTRE );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fed_SampleSelectPlay( void )
* ACTION   : Fed_SampleSelectPlay
* CREATION : 23.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Fed_SampleSelectPlay( void )
{
	sAudioDmaSound *	lpSpl;

	lpSpl = 0;

	if( Fed_GetpActivePage()->mpPageStyle->mpSampleSelect )
	{
		if( Fed_GetpActivePage()->mpPageStyle->mpSampleSelect->mpAsset )
		{
			lpSpl = (sAudioDmaSound*)Fed_GetpActivePage()->mpPageStyle->mpSampleSelect->mpAsset->mpData;
		}
	}

	if( Fed_GetpActivePage()->mpSampleSelect )
	{
		if( Fed_GetpActivePage()->mpSampleSelect->mpAsset )
		{
			if( Fed_GetpActivePage()->mpSampleSelect->mpAsset->mpData )
			{
				lpSpl = (sAudioDmaSound*)Fed_GetpActivePage()->mpSampleSelect->mpAsset->mpData;
			}
		}
	}

	AudioMixer_PlaySample( lpSpl, eAMIXER_PAN_CENTRE );
}


/* ################################################################################ */

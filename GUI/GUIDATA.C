/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GUIDATA.H"

#include	"GUIEDIT.H"

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/CLI/CLI.H>
#include	<GODLIB/MEMORY/MEMORY.H>

#include	<STRING.H>


/* ###################################################################################
#  DATA
################################################################################### */

char	gGuiDataString[ 128 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			GuiInfo_Relocate( sGuiInfo * apInfo, const sGuiData * apData );
void			GuiInfo_Delocate( sGuiInfo * apInfo, const sGuiData * apData );
char *			GuiData_StringSerialise( const char * apString, char * apDest );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_Init( sGuiData * apData,sHashTree * apTree )
* ACTION   : GuiData_Init
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiData_Init( sGuiData * apData,sHashTree * apTree )
{
	U16	i;


	for( i=0; i<apData->mAssetCount; i++ )
	{
		AssetClient_Init( &apData->mpAssets[ i ].mAsset, apData->mpAssets[ i ].mpFileName, apData->mpAssets[ i ].mpContext, (void**)&apData->mpAssets[ i ].mpData );
	}

	for( i=0; i<apData->mButtonCount; i++ )
	{
		if( apData->mpButtons[ i ].mString.mVar.mpName )
		{
			apData->mpButtons[ i ].mString.mVar.mpVar = HashTree_VarRegister( apTree, apData->mpButtons[ i ].mString.mVar.mpName );
		}
	}

	for( i=0; i<apData->mVarCount; i++ )
	{
		apData->mpVars[ i ].mpVar = HashTree_VarRegister( apTree, apData->mpVars[ i ].mpName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_DeInit( sGuiData * apData, sHashTree * apTree )
* ACTION   : GuiData_DeInit
* CREATION : 16.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiData_DeInit( sGuiData * apData, sHashTree * apTree )
{
	U16	i;


	for( i=0; i<apData->mAssetCount; i++ )
	{
		AssetClient_DeInit( &apData->mpAssets[ i ].mAsset );
	}

	for( i=0; i<apData->mButtonCount; i++ )
	{
		if( apData->mpButtons[ i ].mString.mVar.mpVar )
		{
			HashTree_VarUnRegister( apTree, apData->mpButtons[ i ].mString.mVar.mpVar );
			apData->mpButtons[ i ].mString.mVar.mpVar = 0;
		}
	}

	for( i=0; i<apData->mVarCount; i++ )
	{
		HashTree_VarUnRegister( apTree, apData->mpVars[ i ].mpVar );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_RectPairConvert( sGuiRectPair * apRectPair )
* ACTION   : GuiData_RectPairConvert
* CREATION : 09.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiData_RectPairConvert( sGuiRectPair * apRectPair )
{
	Endian_FromBigU16( &apRectPair->mLocal.mHeight );
	Endian_FromBigU16( &apRectPair->mLocal.mWidth );
	Endian_FromBigU16( &apRectPair->mLocal.mX );
	Endian_FromBigU16( &apRectPair->mLocal.mY );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_Relocate( const sGuiData * apData )
* ACTION   : GuiData_Relocate
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

/* #define	mGUI_RELOC( _a )	if( apData->_a ) { *(U32*)&apData->_a += (U32)apData; } */
#define	mGUI_RELOC( _a )	{ Endian_FromBigU32( &apData->_a ); if( apData->_a ) { *(U32*)&apData->_a += (U32)apData; } }

void			GuiData_Relocate( sGuiData * apData )
{
	U16	i,j;

	Endian_FromBigU32( &apData->mID );
	Endian_FromBigU32( &apData->mVersion );

	GODLIB_ASSERT( apData->mVersion == dGUIDATA_VERSION );

	mGUI_RELOC( mpActions );
	mGUI_RELOC( mpAssets );
	mGUI_RELOC( mpButtons );
	mGUI_RELOC( mpButtonStyles );
	mGUI_RELOC( mpColours );
	mGUI_RELOC( mpCursors );
	mGUI_RELOC( mpFills );
	mGUI_RELOC( mpFontGroups );
	mGUI_RELOC( mpKeyActions );
	mGUI_RELOC( mpLists );
	mGUI_RELOC( mpLocks );
	mGUI_RELOC( mpSliders );
	mGUI_RELOC( mpValues );
	mGUI_RELOC( mpVars );
	mGUI_RELOC( mpWindows );

	Endian_FromBigU16( &apData->mAssetCount );
	Endian_FromBigU16( &apData->mActionCount );
	Endian_FromBigU16( &apData->mButtonCount );
	Endian_FromBigU16( &apData->mButtonStyleCount );
	Endian_FromBigU16( &apData->mColourCount );
	Endian_FromBigU16( &apData->mCursorCount );
	Endian_FromBigU16( &apData->mFillCount );
	Endian_FromBigU16( &apData->mFontGroupCount );
	Endian_FromBigU16( &apData->mKeyActionCount );
	Endian_FromBigU16( &apData->mListCount );
	Endian_FromBigU16( &apData->mLockCount );
	Endian_FromBigU16( &apData->mSliderCount );
	Endian_FromBigU16( &apData->mValueCount );
	Endian_FromBigU16( &apData->mVarCount );
	Endian_FromBigU16( &apData->mWindowCount );

	for( i=0; i<apData->mActionCount; i++ )
	{
		mGUI_RELOC( mpActions[ i ].mpValue  );
		mGUI_RELOC( mpActions[ i ].mpWindowClose );
		mGUI_RELOC( mpActions[ i ].mpWindowOpen );

		Endian_FromBigU16( &apData->mpActions[ i ].mAction );
		Endian_FromBigU32( &apData->mpActions[ i ].mConstant );
	}

	for( i=0; i<apData->mAssetCount; i++ )
	{
		mGUI_RELOC( mpAssets[ i ].mpContext  );
		mGUI_RELOC( mpAssets[ i ].mpFileName );
	}

	for( i=0; i<apData->mButtonCount; i++ )
	{
		GuiInfo_Relocate( &apData->mpButtons[ i ].mInfo, apData );

		mGUI_RELOC( mpButtons[ i ].mpOnLeftClick );
		mGUI_RELOC( mpButtons[ i ].mpOnLeftHeld );
		mGUI_RELOC( mpButtons[ i ].mpOnLeftRelease );
		mGUI_RELOC( mpButtons[ i ].mpOnRightClick );
		mGUI_RELOC( mpButtons[ i ].mpOnRightHeld );
		mGUI_RELOC( mpButtons[ i ].mpOnRightRelease );
		mGUI_RELOC( mpButtons[ i ].mpOnFocus );
		mGUI_RELOC( mpButtons[ i ].mpOnDeFocus );
		mGUI_RELOC( mpButtons[ i ].mpOnIKBD );

		mGUI_RELOC( mpButtons[ i ].mpStyle );

		mGUI_RELOC( mpButtons[ i ].mSprite.mpAsset );

		mGUI_RELOC( mpButtons[ i ].mString.mpFontGroup );
		mGUI_RELOC( mpButtons[ i ].mString.mpTitle );
		mGUI_RELOC( mpButtons[ i ].mString.mVar.mpName );

		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mStructOffset );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mStructSize   );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mSize         );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mType         );

		Endian_FromBigU16( &apData->mpButtons[ i ].mButtonType );
		Endian_FromBigU16( &apData->mpButtons[ i ].mSubType );

		GuiData_RectPairConvert( &apData->mpButtons[ i ].mSprite.mRectPair );
		GuiData_RectPairConvert( &apData->mpButtons[ i ].mString.mRects );
	}

	for( i=0; i<apData->mButtonStyleCount; i++ )
	{
		mGUI_RELOC( mpButtonStyles[ i ].mpFillLocked  );
		mGUI_RELOC( mpButtonStyles[ i ].mpFillNormal  );
		mGUI_RELOC( mpButtonStyles[ i ].mpFillSelected  );

	}

	for( i=0; i<apData->mCursorCount; i++ )
	{
		mGUI_RELOC( mpCursors[ i ].mpAsset  );
	}


	for( i=0; i<apData->mFillCount; i++ )
	{
		Endian_FromBigU16( &apData->mpFills[ i ].mFillPattern );
		Endian_FromBigU16( &apData->mpFills[ i ].mFillType );
		for( j=0; j<eGUI_FILLCOLOUR_LIMIT; j++ )
		{
			mGUI_RELOC( mpFills[ i ].mpColours[ j ] );
		}
	}

	for( i=0; i<apData->mFontGroupCount; i++ )
	{
		mGUI_RELOC( mpFontGroups[ i ].mpLocked   );
		mGUI_RELOC( mpFontGroups[ i ].mpNormal   );
		mGUI_RELOC( mpFontGroups[ i ].mpSelected );
	}

	for( i=0; i<apData->mKeyActionCount; i++ )
	{
		mGUI_RELOC( mpKeyActions[ i ].mpAction );
	}

	for( i=0; i<apData->mListCount; i++ )
	{
		GuiInfo_Relocate( &apData->mpLists[ i ].mInfo, apData );
		mGUI_RELOC( mpLists[ i ].mpButton );
		mGUI_RELOC( mpLists[ i ].mpSlider );
		mGUI_RELOC( mpLists[ i ].mpWindow );
	}

	for( i=0; i<apData->mLockCount; i++ )
	{
		mGUI_RELOC( mpLocks[ i ].mpLockValue );
		mGUI_RELOC( mpLocks[ i ].mpLockVar );
		mGUI_RELOC( mpLocks[ i ].mpVisVar );
		mGUI_RELOC( mpLocks[ i ].mpVisValue );
	}

	for( i=0; i<apData->mSliderCount; i++ )
	{
		GuiInfo_Relocate( &apData->mpSliders[ i ].mInfo, apData );
		mGUI_RELOC( mpSliders[ i ].mpLineSize );
		mGUI_RELOC( mpSliders[ i ].mpPageSize );
		mGUI_RELOC( mpSliders[ i ].mpWindow );
		for( j=0; j<eGUI_SLIDERBUT_LIMIT; j++ )
		{
			mGUI_RELOC( mpSliders[ i ].mpButtons[ j ] );
		}

		Endian_FromBigU16( &apData->mpSliders[ i ].mButtonSize );
		Endian_FromBigU16( &apData->mpSliders[ i ].mSizeMin );
		Endian_FromBigU16( &apData->mpSliders[ i ].mSliderType );
	}

	for( i=0; i<apData->mValueCount; i++ )
	{
		mGUI_RELOC( mpValues[ i ].mpVar );
		mGUI_RELOC( mpValues[ i ].mpValueMin );
		mGUI_RELOC( mpValues[ i ].mpValueMax );
	}

	for( i=0; i<apData->mVarCount; i++ )
	{
		Endian_FromBigU16( &apData->mpVars[ i ].mStructOffset );
		Endian_FromBigU16( &apData->mpVars[ i ].mStructSize   );
		Endian_FromBigU16( &apData->mpVars[ i ].mSize         );
		Endian_FromBigU16( &apData->mpVars[ i ].mType         );
		mGUI_RELOC( mpVars[ i ].mpName );
	}

	for( i=0; i<apData->mValueCount; i++ )
	{
		Endian_FromBigU32( &apData->mpValues[ i ].mMax );
		Endian_FromBigU32( &apData->mpValues[ i ].mMin );
	}

	for( i=0; i<apData->mWindowCount; i++ )
	{
		GuiInfo_Relocate( &apData->mpWindows[ i ].mInfo, apData );

		mGUI_RELOC( mpWindows[ i ].mpButtonStyle );
		mGUI_RELOC( mpWindows[ i ].mpFill );
		mGUI_RELOC( mpWindows[ i ].mpFontGroup );
		mGUI_RELOC( mpWindows[ i ].mpOnIKBD );
		mGUI_RELOC( mpWindows[ i ].mppControls );
		mGUI_RELOC( mpWindows[ i ].mppKeyActions );

		Endian_FromBigU16( &apData->mpWindows[ i ].mControlCount );
		Endian_FromBigU16( &apData->mpWindows[ i ].mKeyActionCount );

		for( j=0; j<apData->mpWindows[ i ].mControlCount; j++ )
		{
			mGUI_RELOC( mpWindows[ i ].mppControls[ j ] );
		}

		for( j=0; j<apData->mpWindows[ i ].mKeyActionCount; j++ )
		{
			mGUI_RELOC( mpWindows[ i ].mppKeyActions[ j ] );
		}

	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_Delocate( const sGuiData * apData )
* ACTION   : GuiData_Delocate
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

#define	mGUI_DELOC( _a )	if( apData->_a ) { *(U32*)&apData->_a -= (U32)apData; Endian_FromBigU32( &apData->_a ); }

void			GuiData_Delocate( sGuiData * apData )
{
	U16	i,j;

	apData->mID = mSTRING_TO_U32( 'G', 'U', 'D', 'A' );
	apData->mVersion = dGUIDATA_VERSION;
	Endian_FromBigU32( &apData->mID );
	Endian_FromBigU32( &apData->mVersion );

	for( i=0; i<apData->mActionCount; i++ )
	{
		Endian_FromBigU16( &apData->mpActions[ i ].mAction );
		Endian_FromBigU32( &apData->mpActions[ i ].mConstant );

		mGUI_DELOC( mpActions[ i ].mpValue  );
		mGUI_DELOC( mpActions[ i ].mpWindowClose );
		mGUI_DELOC( mpActions[ i ].mpWindowOpen );
	}

	for( i=0; i<apData->mAssetCount; i++ )
	{
		mGUI_DELOC( mpAssets[ i ].mpContext  );
		mGUI_DELOC( mpAssets[ i ].mpFileName );
	}

	for( i=0; i<apData->mButtonCount; i++ )
	{
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mStructOffset );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mStructSize   );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mSize         );
		Endian_FromBigU16( &apData->mpButtons[ i ].mString.mVar.mType         );

		Endian_FromBigU16( &apData->mpButtons[ i ].mButtonType );
		Endian_FromBigU16( &apData->mpButtons[ i ].mSubType );

		mGUI_DELOC( mpButtons[ i ].mpOnLeftClick );
		mGUI_DELOC( mpButtons[ i ].mpOnLeftHeld );
		mGUI_DELOC( mpButtons[ i ].mpOnLeftRelease );
		mGUI_DELOC( mpButtons[ i ].mpOnRightClick );
		mGUI_DELOC( mpButtons[ i ].mpOnRightHeld );
		mGUI_DELOC( mpButtons[ i ].mpOnRightRelease );
		mGUI_DELOC( mpButtons[ i ].mpOnFocus );
		mGUI_DELOC( mpButtons[ i ].mpOnDeFocus );

		mGUI_DELOC( mpButtons[ i ].mpStyle );

		mGUI_DELOC( mpButtons[ i ].mSprite.mpAsset );

		mGUI_DELOC( mpButtons[ i ].mString.mpFontGroup );
		mGUI_DELOC( mpButtons[ i ].mString.mVar.mpName );
		mGUI_DELOC( mpButtons[ i ].mString.mpTitle );

		GuiData_RectPairConvert( &apData->mpButtons[ i ].mSprite.mRectPair );
		GuiData_RectPairConvert( &apData->mpButtons[ i ].mString.mRects );

		GuiInfo_Delocate( &apData->mpButtons[ i ].mInfo, apData );		
	}

	for( i=0; i<apData->mButtonStyleCount; i++ )
	{
		mGUI_DELOC( mpButtonStyles[ i ].mpFillLocked  );
		mGUI_DELOC( mpButtonStyles[ i ].mpFillNormal  );
		mGUI_DELOC( mpButtonStyles[ i ].mpFillSelected  );
	}

	for( i=0; i<apData->mCursorCount; i++ )
	{
		mGUI_DELOC( mpCursors[ i ].mpAsset  );
	}

	for( i=0; i<apData->mFontGroupCount; i++ )
	{
		mGUI_DELOC( mpFontGroups[ i ].mpLocked   );
		mGUI_DELOC( mpFontGroups[ i ].mpNormal   );
		mGUI_DELOC( mpFontGroups[ i ].mpSelected );
	}

	for( i=0; i<apData->mFillCount; i++ )
	{
		Endian_FromBigU16( &apData->mpFills[ i ].mFillPattern );
		Endian_FromBigU16( &apData->mpFills[ i ].mFillType );

		for( j=0; j<eGUI_FILLCOLOUR_LIMIT; j++ )
		{
			mGUI_DELOC( mpFills[ i ].mpColours[ j ] );
		}
	}

	for( i=0; i<apData->mKeyActionCount; i++ )
	{
		mGUI_DELOC( mpKeyActions[ i ].mpAction );
	}

	for( i=0; i<apData->mListCount; i++ )
	{
		GuiInfo_Delocate( &apData->mpLists[ i ].mInfo, apData );
		mGUI_DELOC( mpLists[ i ].mpButton );
		mGUI_DELOC( mpLists[ i ].mpSlider );
		mGUI_DELOC( mpLists[ i ].mpWindow );
	}

	for( i=0; i<apData->mLockCount; i++ )
	{
		mGUI_DELOC( mpLocks[ i ].mpLockValue );
		mGUI_DELOC( mpLocks[ i ].mpLockVar );
		mGUI_DELOC( mpLocks[ i ].mpVisVar );
		mGUI_DELOC( mpLocks[ i ].mpVisValue );
	}

	for( i=0; i<apData->mSliderCount; i++ )
	{
		Endian_FromBigU16( &apData->mpSliders[ i ].mButtonSize );
		Endian_FromBigU16( &apData->mpSliders[ i ].mSizeMin );
		Endian_FromBigU16( &apData->mpSliders[ i ].mSliderType );

		GuiInfo_Delocate( &apData->mpSliders[ i ].mInfo, apData );
		for( j=0; j<eGUI_SLIDERBUT_LIMIT; j++ )
		{
			mGUI_DELOC( mpSliders[ i ].mpButtons[ j ] );
		}
		mGUI_DELOC( mpSliders[ i ].mpLineSize );
		mGUI_DELOC( mpSliders[ i ].mpPageSize );
		mGUI_DELOC( mpSliders[ i ].mpWindow );
	}

	for( i=0; i<apData->mValueCount; i++ )
	{
		Endian_FromBigU32( &apData->mpValues[ i ].mMax );
		Endian_FromBigU32( &apData->mpValues[ i ].mMin );

		mGUI_DELOC( mpValues[ i ].mpVar );
		mGUI_DELOC( mpValues[ i ].mpValueMin );
		mGUI_DELOC( mpValues[ i ].mpValueMax );
	}

	for( i=0; i<apData->mVarCount; i++ )
	{
		Endian_FromBigU16( &apData->mpVars[ i ].mStructOffset );
		Endian_FromBigU16( &apData->mpVars[ i ].mStructSize   );
		Endian_FromBigU16( &apData->mpVars[ i ].mSize         );
		Endian_FromBigU16( &apData->mpVars[ i ].mType         );

		mGUI_DELOC( mpVars[ i ].mpName );
	}

	for( i=0; i<apData->mWindowCount; i++ )
	{
		for( j=0; j<apData->mpWindows[ i ].mControlCount; j++ )
		{
			mGUI_DELOC( mpWindows[ i ].mppControls[ j ] );
		}
		for( j=0; j<apData->mpWindows[ i ].mKeyActionCount; j++ )
		{
			mGUI_DELOC( mpWindows[ i ].mppKeyActions[ j ] );
		}
		mGUI_DELOC( mpWindows[ i ].mpButtonStyle );
		mGUI_DELOC( mpWindows[ i ].mpFill );
		mGUI_DELOC( mpWindows[ i ].mpFontGroup );
		mGUI_DELOC( mpWindows[ i ].mpOnIKBD );
		mGUI_DELOC( mpWindows[ i ].mppControls );
		mGUI_DELOC( mpWindows[ i ].mppKeyActions );

		GuiInfo_Delocate( &apData->mpWindows[ i ].mInfo, apData );

		Endian_FromBigU16( &apData->mpWindows[ i ].mControlCount );
		Endian_FromBigU16( &apData->mpWindows[ i ].mKeyActionCount );
	}

	Endian_FromBigU16( &apData->mAssetCount );
	Endian_FromBigU16( &apData->mActionCount );
	Endian_FromBigU16( &apData->mButtonCount );
	Endian_FromBigU16( &apData->mButtonStyleCount );
	Endian_FromBigU16( &apData->mColourCount );
	Endian_FromBigU16( &apData->mCursorCount );
	Endian_FromBigU16( &apData->mFillCount );
	Endian_FromBigU16( &apData->mFontGroupCount );
	Endian_FromBigU16( &apData->mKeyActionCount );
	Endian_FromBigU16( &apData->mListCount );
	Endian_FromBigU16( &apData->mLockCount );
	Endian_FromBigU16( &apData->mSliderCount );
	Endian_FromBigU16( &apData->mValueCount );
	Endian_FromBigU16( &apData->mVarCount );
	Endian_FromBigU16( &apData->mWindowCount );

	mGUI_DELOC( mpActions );
	mGUI_DELOC( mpAssets );
	mGUI_DELOC( mpButtons );
	mGUI_DELOC( mpButtonStyles );
	mGUI_DELOC( mpColours );
	mGUI_DELOC( mpCursors );
	mGUI_DELOC( mpFills );
	mGUI_DELOC( mpFontGroups );
	mGUI_DELOC( mpKeyActions );
	mGUI_DELOC( mpLists );
	mGUI_DELOC( mpLocks );
	mGUI_DELOC( mpSliders );
	mGUI_DELOC( mpValues );
	mGUI_DELOC( mpVars );
	mGUI_DELOC( mpWindows );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiInfo_Relocate( sGuiInfo * apInfo, const sGuiData * apData )
* ACTION   : GuiInfo_Relocate
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

/*#define	mGUIINFO_RELOC( _a )	if( apInfo->_a) { *(U32*)&apInfo->_a += (U32)apData; }*/
#define	mGUIINFO_RELOC( _a )	{ Endian_FromBigU32( &apInfo->_a ); if( apInfo->_a ) { *(U32*)&apInfo->_a += (U32)apData; } }

void	GuiInfo_Relocate( sGuiInfo * apInfo, const sGuiData * apData )
{
	mGUIINFO_RELOC( mpValue );
	mGUIINFO_RELOC( mpLock );
	mGUIINFO_RELOC( mpName );

	Endian_FromBigU16( &apInfo->mEvent.mEvent );
	Endian_FromBigU32( &apInfo->mFlags );
	Endian_FromBigU16( &apInfo->mHash );
	Endian_FromBigU16( &apInfo->mType );

	GuiData_RectPairConvert( &apInfo->mRectPair );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiInfo_Delocate( sGuiInfo * apInfo, const sGuiData * apData )
* ACTION   : GuiInfo_Delocate
* CREATION : 13.02.2004 PNK
*-----------------------------------------------------------------------------------*/

#define	mGUIINFO_DELOC( _a )	if( apInfo->_a ) { *(U32*)&apInfo->_a -= (U32)apData; Endian_FromBigU32( &apInfo->_a ); }

void	GuiInfo_Delocate( sGuiInfo * apInfo, const sGuiData * apData )
{
	mGUIINFO_DELOC( mpValue );
	mGUIINFO_DELOC( mpLock );
	mGUIINFO_DELOC( mpName );

	Endian_FromBigU16( &apInfo->mEvent.mEvent );
	Endian_FromBigU32( &apInfo->mFlags );
	Endian_FromBigU16( &apInfo->mHash );
	Endian_FromBigU16( &apInfo->mType );

	GuiData_RectPairConvert( &apInfo->mRectPair );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_GetSize( sGuiData * apData )
* ACTION   : GuiData_GetSize
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	GuiData_GetSize( sGuiData * apData )
{
	U32	lSize;

	lSize  = sizeof( sGuiData );
	lSize += apData->mActionCount      * (sizeof( sGuiAction )      );
	lSize += apData->mAssetCount       * (sizeof( sGuiAsset )       );
	lSize += apData->mButtonCount      * (sizeof( sGuiButton )      );
	lSize += apData->mButtonStyleCount * (sizeof( sGuiButtonStyle ) );
	lSize += apData->mColourCount      * (sizeof( sGuiColour )      );
	lSize += apData->mCursorCount      * (sizeof( sGuiCursor )      );
	lSize += apData->mFillCount        * (sizeof( sGuiFill )        );
	lSize += apData->mFontGroupCount   * (sizeof( sGuiFontGroup )   );
	lSize += apData->mKeyActionCount   * (sizeof( sGuiKeyAction )   );
	lSize += apData->mListCount        * (sizeof( sGuiList )        );
	lSize += apData->mLockCount        * (sizeof( sGuiLock )        );
	lSize += apData->mSliderCount      * (sizeof( sGuiSlider )      );
	lSize += apData->mValueCount       * (sizeof( sGuiValue )       );
	lSize += apData->mVarCount         * (sizeof( sGuiVar )         );
	lSize += apData->mWindowCount      * (sizeof( sGuiWindow )      );
	lSize += GuiData_GetArraysSize( apData );
	lSize += GuiData_GetStringsSize( apData );

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_GetArraysSize( sGuiData * apData )
* ACTION   : GuiData_GetArraysSize
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	GuiData_GetArraysSize( sGuiData * apData )
{
	U32	lSize;
	U16	i;

	lSize = 0;

	for( i=0; i<apData->mWindowCount; i++ )
	{
		lSize += (apData->mpWindows[ i ].mControlCount * 4);
		lSize += (apData->mpWindows[ i ].mKeyActionCount * 4);
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_GetStringsSize( sGuiData * apData )
* ACTION   : GuiData_GetStringsSize
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	GuiData_GetStringsSize( sGuiData * apData )
{
	U32	lSize;
	U16	i;

	lSize = 0;


	for( i=0; i<apData->mAssetCount; i++ )
	{
		if( apData->mpAssets[ i ].mpContext )
		{
			lSize += strlen( apData->mpAssets[ i ].mpContext ) + 1;
		}
		if( apData->mpAssets[ i ].mpFileName )
		{
			lSize += strlen( apData->mpAssets[ i ].mpFileName ) + 1;
		}
	}
	for( i=0; i<apData->mButtonCount; i++ )
	{
		if( apData->mpButtons[ i ].mString.mpTitle )
		{
			lSize += strlen( apData->mpButtons[ i ].mString.mpTitle ) + 1;
		}
		if( apData->mpButtons[ i ].mInfo.mpName )
		{
			lSize += strlen( apData->mpButtons[ i ].mInfo.mpName ) + 1;
		}
		if( apData->mpButtons[ i ].mString.mVar.mpName )
		{
			lSize += strlen( apData->mpButtons[ i ].mString.mVar.mpName ) + 1;
		}
	}
	for( i=0; i<apData->mListCount; i++ )
	{
		if( apData->mpLists[ i ].mInfo.mpName )
		{
			lSize += strlen( apData->mpLists[ i ].mInfo.mpName ) + 1;
		}
	}
	for( i=0; i<apData->mSliderCount; i++ )
	{
		if( apData->mpSliders[ i ].mInfo.mpName )
		{
			lSize += strlen( apData->mpSliders[ i ].mInfo.mpName ) + 1;
		}
	}
	for( i=0; i<apData->mVarCount; i++ )
	{
		if( apData->mpVars[ i ].mpName )
		{
			lSize += strlen( apData->mpVars[ i ].mpName ) + 1;
		}
	}
	for( i=0; i<apData->mWindowCount; i++ )
	{
		if( apData->mpWindows[ i ].mInfo.mpName )
		{
			lSize += strlen( apData->mpWindows[ i ].mInfo.mpName ) + 1;
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_Serialise( sGuiData * apData )
* ACTION   : GuiData_Serialise
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

#define	mGUI_SERIALISE( _aPtr, _aMember, _aGroup )	if(apData->_aPtr[ i ]._aMember) { *(U32*)&lpData->_aPtr[ i ]._aMember = (U32)lpData->_aGroup + ( (U32)apData->_aPtr[ i ]._aMember - (U32)apData->_aGroup ); }
#define	mGUI_STR_SERIALISE( _aMember )	if( apData->_aMember ) { lpData->_aMember = lpStrings; lpStrings = GuiData_StringSerialise( apData->_aMember, lpStrings ); }

sGuiData *	GuiData_Serialise( sGuiData * apData )
{
	sGuiData *	lpData;
	U32				lSize;
	U32				lOff;
	char *			lpStrings = 0;
	U16				i,j;
	U8 *			lpMem;

	lSize    = GuiData_GetSize( apData );
	lpMem    = (U8*)mMEMCALLOC( lSize );
	lpData = (sGuiData*)lpMem;

	if( lpData )
	{
		Memory_Copy( sizeof(sGuiData), apData, lpData );

		lOff                     = sizeof(sGuiData);

		lpData->mpActions       = (sGuiAction*)&lpMem[ lOff ];
		lOff                    += (apData->mActionCount * sizeof(sGuiAction) );

		lpData->mpAssets       = (sGuiAsset*)&lpMem[ lOff ];
		lOff                    += (apData->mAssetCount * sizeof(sGuiAsset) );

		lpData->mpButtons      = (sGuiButton*)&lpMem[ lOff ];
		lOff                    += (apData->mButtonCount * sizeof(sGuiButton));

		lpData->mpButtonStyles = (sGuiButtonStyle*)&lpMem[ lOff ];
		lOff                    += (apData->mButtonStyleCount * sizeof(sGuiButtonStyle));

		lpData->mpColours      = (sGuiColour*)&lpMem[ lOff ];
		lOff                    += (apData->mColourCount * sizeof(sGuiColour));

		lpData->mpCursors      = (sGuiCursor*)&lpMem[ lOff ];
		lOff                    += (apData->mCursorCount * sizeof(sGuiCursor));

		lpData->mpFills        = (sGuiFill*)&lpMem[ lOff ];
		lOff                    += (apData->mFillCount * sizeof(sGuiFill));

		lpData->mpFontGroups   = (sGuiFontGroup*)&lpMem[ lOff ];
		lOff                    += (apData->mFontGroupCount * sizeof(sGuiFontGroup));

		lpData->mpKeyActions     = (sGuiKeyAction*)&lpMem[ lOff ];
		lOff                    += (apData->mKeyActionCount * sizeof(sGuiKeyAction));

		lpData->mpLists          = (sGuiList*)&lpMem[ lOff ];
		lOff                    += (apData->mListCount * sizeof(sGuiList));

		lpData->mpLocks        = (sGuiLock*)&lpMem[ lOff ];
		lOff                    += (apData->mLockCount * sizeof(sGuiLock));

		lpData->mpSliders      = (sGuiSlider*)&lpMem[ lOff ];
		lOff                    += (apData->mSliderCount * sizeof(sGuiSlider));

		lpData->mpValues       = (sGuiValue*)&lpMem[ lOff ];
		lOff                    += (apData->mValueCount * sizeof(sGuiValue));

		lpData->mpVars         = (sGuiVar*)&lpMem[ lOff ];
		lOff                    += (apData->mVarCount * sizeof(sGuiVar));

		lpData->mpWindows      = (sGuiWindow*)&lpMem[ lOff ];
		lOff                    += (apData->mWindowCount * sizeof(sGuiWindow));

		lpStrings                = (char*)&lpMem[ lOff + GuiData_GetArraysSize( apData ) ];

		for( i=0; i<apData->mActionCount; i++ )
		{
			Memory_Copy( sizeof(sGuiAction), &apData->mpActions[ i ], &lpData->mpActions[ i ] );
			mGUI_SERIALISE( mpActions, mpValue,  mpValues );
			mGUI_SERIALISE( mpActions, mpWindowClose, mpWindows );
			mGUI_SERIALISE( mpActions, mpWindowOpen, mpWindows );
		}

		for( i=0; i<apData->mAssetCount; i++ )
		{
			Memory_Copy( sizeof(sGuiAsset), &apData->mpAssets[ i ], &lpData->mpAssets[ i ] );
			mGUI_STR_SERIALISE( mpAssets[ i ].mpContext );
			mGUI_STR_SERIALISE( mpAssets[ i ].mpFileName );
		}

		for( i=0; i<apData->mButtonCount; i++ )
		{
			Memory_Copy( sizeof(sGuiButton), &apData->mpButtons[ i ], &lpData->mpButtons[ i ] );
			mGUI_SERIALISE( mpButtons, mpOnLeftClick,    mpActions );
			mGUI_SERIALISE( mpButtons, mpOnLeftHeld,     mpActions );
			mGUI_SERIALISE( mpButtons, mpOnLeftRelease,  mpActions );
			mGUI_SERIALISE( mpButtons, mpOnRightClick,   mpActions );
			mGUI_SERIALISE( mpButtons, mpOnRightHeld,    mpActions );
			mGUI_SERIALISE( mpButtons, mpOnRightRelease, mpActions );
			mGUI_SERIALISE( mpButtons, mpOnFocus,        mpActions );
			mGUI_SERIALISE( mpButtons, mpOnDeFocus,      mpActions );
			mGUI_SERIALISE( mpButtons, mpOnIKBD,         mpVars );
			mGUI_SERIALISE( mpButtons, mpStyle,          mpButtonStyles );

			mGUI_SERIALISE( mpButtons, mSprite.mpAsset,     mpAssets );
			mGUI_SERIALISE( mpButtons, mString.mpFontGroup, mpFontGroups );
			mGUI_SERIALISE( mpButtons, mInfo.mpLock,        mpLocks );
			mGUI_SERIALISE( mpButtons, mInfo.mpValue,       mpValues );

			mGUI_STR_SERIALISE( mpButtons[ i ].mInfo.mpName );

			mGUI_STR_SERIALISE( mpButtons[ i ].mString.mVar.mpName );
			mGUI_STR_SERIALISE( mpButtons[ i ].mString.mpTitle );
		}

		for( i=0; i<apData->mButtonStyleCount; i++ )
		{
			Memory_Copy( sizeof(sGuiButtonStyle), &apData->mpButtonStyles[ i ], &lpData->mpButtonStyles[ i ] );
			mGUI_SERIALISE( mpButtonStyles, mpFillLocked,   mpFills );
			mGUI_SERIALISE( mpButtonStyles, mpFillNormal,   mpFills );
			mGUI_SERIALISE( mpButtonStyles, mpFillSelected, mpFills );
		}

		for( i=0; i<apData->mColourCount; i++ )
		{
			Memory_Copy( sizeof(sGuiColour), &apData->mpColours[ i ], &lpData->mpColours[ i ] );
		}

		for( i=0; i<apData->mCursorCount; i++ )
		{
			Memory_Copy( sizeof(sGuiCursor), &apData->mpCursors[ i ], &lpData->mpCursors[ i ] );
			mGUI_SERIALISE( mpCursors, mpAsset, mpAssets );
		}

		for( i=0; i<apData->mFillCount; i++ )
		{
			Memory_Copy( sizeof(sGuiFill), &apData->mpFills[ i ], &lpData->mpFills[ i ] );
			for( j=0; j<eGUI_FILLCOLOUR_LIMIT; j++ )
			{
				mGUI_SERIALISE( mpFills, mpColours[ j ], mpColours );
			}
		}

		for( i=0; i<apData->mFontGroupCount; i++ )
		{
			Memory_Copy( sizeof(sGuiFontGroup), &apData->mpFontGroups[ i ], &lpData->mpFontGroups[ i ] );
			mGUI_SERIALISE( mpFontGroups, mpNormal, mpAssets );
			mGUI_SERIALISE( mpFontGroups, mpLocked, mpAssets );
			mGUI_SERIALISE( mpFontGroups, mpSelected, mpAssets );
		}

		for( i = 0; i < apData->mKeyActionCount; i++ )
		{
			Memory_Copy( sizeof( sGuiKeyAction ), &apData->mpKeyActions[ i ], &lpData->mpKeyActions[ i ] );
			mGUI_SERIALISE( mpKeyActions, mpAction, mpActions );
		}

		for( i = 0; i < apData->mListCount; i++ )
		{
			Memory_Copy( sizeof( sGuiList ), &apData->mpLists[ i ], &lpData->mpLists[ i ] );
			mGUI_SERIALISE( mpLists, mInfo.mpLock, mpLocks );
			mGUI_SERIALISE( mpLists, mInfo.mpValue, mpValues );
			mGUI_SERIALISE( mpLists, mpButton, mpButtons );
			mGUI_SERIALISE( mpLists, mpSlider, mpSliders );
			mGUI_SERIALISE( mpLists, mpWindow, mpWindows );

			mGUI_STR_SERIALISE( mpLists[ i ].mInfo.mpName );
		}


		for( i = 0; i < apData->mLockCount; i++ )
		{
			Memory_Copy( sizeof( sGuiLock ), &apData->mpLocks[ i ], &lpData->mpLocks[ i ] );
			mGUI_SERIALISE( mpLocks, mpLockVar, mpVars );
			mGUI_SERIALISE( mpLocks, mpLockValue, mpVars );
			mGUI_SERIALISE( mpLocks, mpVisVar, mpVars );
			mGUI_SERIALISE( mpLocks, mpVisValue, mpVars );
		}

		for( i = 0; i < apData->mSliderCount; i++ )
		{
			Memory_Copy( sizeof( sGuiSlider ), &apData->mpSliders[ i ], &lpData->mpSliders[ i ] );
			for( j = 0; j < eGUI_SLIDERBUT_LIMIT; j++ )
			{
				mGUI_SERIALISE( mpSliders, mpButtons[ j ], mpButtons );
			}
			mGUI_SERIALISE( mpSliders, mInfo.mpLock, mpLocks );
			mGUI_SERIALISE( mpSliders, mInfo.mpValue, mpValues );
			mGUI_SERIALISE( mpSliders, mpLineSize, mpVars );
			mGUI_SERIALISE( mpSliders, mpPageSize, mpVars );
			mGUI_SERIALISE( mpSliders, mpWindow, mpWindows );

			mGUI_STR_SERIALISE( mpSliders[ i ].mInfo.mpName );
		}

		for( i = 0; i < apData->mValueCount; i++ )
		{
			Memory_Copy( sizeof( sGuiValue ), &apData->mpValues[ i ], &lpData->mpValues[ i ] );
			mGUI_SERIALISE( mpValues, mpVar, mpVars );
			mGUI_SERIALISE( mpValues, mpValueMin, mpVars );
			mGUI_SERIALISE( mpValues, mpValueMax, mpVars );
		}

		for( i = 0; i < apData->mVarCount; i++ )
		{
			Memory_Copy( sizeof( sGuiVar ), &apData->mpVars[ i ], &lpData->mpVars[ i ] );
			mGUI_STR_SERIALISE( mpVars[ i ].mpName );
		}

		for( i = 0; i < apData->mWindowCount; i++ )
		{
			Memory_Copy( sizeof( sGuiWindow ), &apData->mpWindows[ i ], &lpData->mpWindows[ i ] );
			mGUI_SERIALISE( mpWindows, mInfo.mpLock, mpLocks );
			mGUI_SERIALISE( mpWindows, mInfo.mpValue, mpValues );

			mGUI_SERIALISE( mpWindows, mpFontGroup, mpFontGroups );
			mGUI_SERIALISE( mpWindows, mpButtonStyle, mpButtonStyles );
			mGUI_SERIALISE( mpWindows, mpFill, mpFills );
			mGUI_SERIALISE( mpWindows, mpOnIKBD, mpVars );

			lpData->mpWindows[ i ].mppControls = (sGuiInfo**)&lpMem[ lOff ];
			lOff += ( lpData->mpWindows[ i ].mControlCount * 4 );

			for( j = 0; j < lpData->mpWindows[ i ].mControlCount; j++ )
			{
				switch( apData->mpWindows[ i ].mppControls[ j ]->mType )
				{
				case	eGUI_TYPE_SLIDER:
					mGUI_SERIALISE( mpWindows, mppControls[ j ], mpSliders );
					break;
				case	eGUI_TYPE_WINDOW:
					mGUI_SERIALISE( mpWindows, mppControls[ j ], mpWindows );
					break;
				case	eGUI_TYPE_BUTTON:
					mGUI_SERIALISE( mpWindows, mppControls[ j ], mpButtons );
					break;
				}
			}

			lpData->mpWindows[ i ].mppKeyActions = (sGuiKeyAction**)&lpMem[ lOff ];
			lOff += ( lpData->mpWindows[ i ].mKeyActionCount * 4 );

			for( j = 0; j < lpData->mpWindows[ i ].mKeyActionCount; j++ )
			{
				mGUI_SERIALISE( mpWindows, mppKeyActions[ j ], mpKeyActions );
			}
			mGUI_STR_SERIALISE( mpWindows[ i ].mInfo.mpName );

		}
	}
	{
		char * begin = (char*)lpData;
		char * end = begin + lSize;
		GODLIB_ASSERT( lpStrings == end );
	}

	return( lpData );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_StringSerialise( const char * apString,char * apDest )
* ACTION   : GuiData_StringSerialise
* CREATION : 15.02.2004 PNK
*-----------------------------------------------------------------------------------*/

char *	GuiData_StringSerialise( const char * apString,char * apDest )
{
	if( apString && apDest )
	{
		strcpy( apDest, apString );
		apDest += ( strlen( apString ) + 1 );;
	}

	return( apDest );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_EventsRegister( sGuiData * apData,sHashTree * apTree )
* ACTION   : GuiData_EventsRegister
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiData_EventsRegister( sGuiData * apData,sHashTree * apTree )
{
	U16	i;
	char	lString[ 128 ];

	for( i=0; i<apData->mButtonCount; i++ )
	{
		sprintf( lString, "GUI\\BUTTONS\\%s", apData->mpButtons[ i ].mInfo.mpName );
		apData->mpButtons[ i ].mInfo.mEvent.mpInfo = &apData->mpButtons[ i ];
		apData->mpButtons[ i ].mInfo.mEvent.mEvent = 0;
		apData->mpButtons[ i ].mInfo.mpEventVar    = HashTree_Var_Create( apTree, lString, sizeof(sGuiEvent), &apData->mpButtons[ i ].mInfo.mEvent );
	}
	for( i=0; i<apData->mListCount; i++ )
	{
		sprintf( lString, "GUI\\LISTS\\%s", apData->mpLists[ i ].mInfo.mpName );
		apData->mpLists[ i ].mInfo.mEvent.mpInfo = &apData->mpLists[ i ];
		apData->mpLists[ i ].mInfo.mEvent.mEvent = 0;
		apData->mpLists[ i ].mInfo.mpEventVar    = HashTree_Var_Create( apTree, lString, sizeof(sGuiEvent), &apData->mpLists[ i ].mInfo.mEvent );
	}
	for( i=0; i<apData->mSliderCount; i++ )
	{
		sprintf( lString, "GUI\\SLIDERS\\%s", apData->mpSliders[ i ].mInfo.mpName );
		apData->mpSliders[ i ].mInfo.mEvent.mpInfo = &apData->mpSliders[ i ];
		apData->mpSliders[ i ].mInfo.mEvent.mEvent = 0;
		apData->mpSliders[ i ].mInfo.mpEventVar    = HashTree_Var_Create( apTree, lString, sizeof(sGuiEvent), &apData->mpSliders[ i ].mInfo.mEvent );
	}
	for( i=0; i<apData->mWindowCount; i++ )
	{
		sprintf( lString, "GUI\\WINDOWS\\%s", apData->mpWindows[ i ].mInfo.mpName );
		apData->mpWindows[ i ].mInfo.mEvent.mpInfo = &apData->mpWindows[ i ];
		apData->mpWindows[ i ].mInfo.mEvent.mEvent = 0;
		apData->mpWindows[ i ].mInfo.mpEventVar    = HashTree_Var_Create( apTree, lString, sizeof(sGuiEvent), &apData->mpWindows[ i ].mInfo.mEvent );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiData_EventsUnRegister( sGuiData * apData, sHashTree * apTree )
* ACTION   : GuiData_EventsUnRegister
* CREATION : 28.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiData_EventsUnRegister( sGuiData * apData, sHashTree * apTree )
{
	U16	i;

	for( i=0; i<apData->mButtonCount; i++ )
	{
		HashTree_VarUnRegister( apTree, apData->mpButtons[ i ].mInfo.mpEventVar );
	}
	for( i=0; i<apData->mListCount; i++ )
	{
		HashTree_VarUnRegister( apTree, apData->mpLists[ i ].mInfo.mpEventVar );
	}
	for( i=0; i<apData->mSliderCount; i++ )
	{
		HashTree_VarUnRegister( apTree, apData->mpSliders[ i ].mInfo.mpEventVar );
	}
	for( i=0; i<apData->mWindowCount; i++ )
	{
		HashTree_VarUnRegister( apTree, apData->mpWindows[ i ].mInfo.mpEventVar );
	}
}


/* ################################################################################ */

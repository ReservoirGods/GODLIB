/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CUTSCENE.H"

#include	<GODLIB/STRING/STRING.H>


/* ###################################################################################
#  DATA
################################################################################### */

sCutCmdDef	gCutSceneCmdDefs[] =
{
	{	eCUT_CMD_CALL,				"CALL",				eCUT_CMD_ARG_SCRIPT,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_CALLBACK,			"CALLBACK",			eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	
	{	eCUT_CMD_BG_GFX_CLEAR,		"BG_GFX_CLEAR",		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_BG_GFX_SET,		"BG_GFX_SET",		eCUT_CMD_ARG_ASSET,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_BG_GFX_ENABLE,		"BG_GFX_ENABLE",	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_BG_GFX_DISABLE,	"BG_GFX_DISABLE",	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_FADE_SET,			"FADE_SET",			eCUT_CMD_ARG_ASSET,		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_FADE_TOBLACK,		"FADE_TOBLACK",		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_FADE_TOPI1,		"FADE_TOPI1",		eCUT_CMD_ARG_ASSET,		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_FADE_TOWHITE,		"FADE_TOWHITE",		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_FADE_WAIT,			"FADE_WAIT",		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_RETURN,			"RETURN",			eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_SAMPLE_ENABLE,		"SAMPLE_ENABLE",	eCUT_CMD_ARG_SAMPLE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SAMPLE_DISABLE,	"SAMPLE_DISABLE",	eCUT_CMD_ARG_SAMPLE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SAMPLE_WAIT,		"SAMPLE_WAIT",		eCUT_CMD_ARG_SAMPLE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_SPRITE_ANIM_SET,	"SPRITE_ANIM_SET",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_ANIMATION,	eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_ANIM_WAIT,	"SPRITE_ANIM_WAIT",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_DISABLE,	"SPRITE_DISABLE",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_ENABLE,		"SPRITE_ENABLE",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_GFX_SET,	"SPRITE_GFX_SET",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_ASSET,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_MOVE_TO,	"SPRITE_MOVE_TO",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_INT		},
	{	eCUT_CMD_SPRITE_MOVE_WAIT,	"SPRITE_MOVE_WAIT",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_SPRITE_POS_SET,	"SPRITE_POS_SET",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_INT		},
	{	eCUT_CMD_SPRITE_SPEED_SET,	"SPRITE_SPEED_SET",	eCUT_CMD_ARG_SPRITE,	eCUT_CMD_ARG_FIX16_16,	eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_TEXT_ANIM_SET,		"TEXT_ANIM_SET",	eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_TEXT_ANIM_WAIT,	"TEXT_ANIM_WAIT",	eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_TEXT_ENABLE,		"TEXT_ENABLE",		eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_TEXT_FONT_SET,		"TEXT_FONT_SET",	eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_ASSET,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_TEXT_DISABLE,		"TEXT_DISABLE",		eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},
	{	eCUT_CMD_TEXT_PAGE_SET,		"TEXT_PAGE_SET",	eCUT_CMD_ARG_TEXT,		eCUT_CMD_ARG_PAGE,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_VAR_SET,			"VAR_SET",			eCUT_CMD_ARG_VAR,		eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE		},

	{	eCUT_CMD_WAIT,				"WAIT",				eCUT_CMD_ARG_INT,		eCUT_CMD_ARG_NONE,		eCUT_CMD_ARG_NONE		},

	{	0, 0, 0, 0 },
};


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_Delocate( sCutScene * apCutScene )
* ACTION   : CutScene_Delocate
* CREATION : 20.04.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	dCUT_PTR_DELOC( _aP )	{	if(apCutScene->_aP) {*(U32*)&apCutScene->_aP -= (U32)apCutScene; Endian_FromBigU32( &apCutScene->_aP );}	}

void	CutScene_Delocate( sCutScene * apCutScene )
{
	U16	i,j;

	Endian_FromBigU32( &apCutScene->mID      );
	Endian_FromBigU32( &apCutScene->mVersion );

	for( i=0; i<apCutScene->mAnimationCount; i++ )
	{
		dCUT_PTR_DELOC( mpAnimations[ i ].mpName );
		Endian_FromBigU16( &apCutScene->mpAnimations[ i ].mFrame0 );
		Endian_FromBigU16( &apCutScene->mpAnimations[ i ].mFrame1 );
		Endian_FromBigU32( &apCutScene->mpAnimations[ i ].mSpeed  );
	}

	for( i=0; i<apCutScene->mAssetCount; i++ )
	{
		dCUT_PTR_DELOC( mpAssets[ i ].mpName     );
		dCUT_PTR_DELOC( mpAssets[ i ].mpContext  );
		dCUT_PTR_DELOC( mpAssets[ i ].mpFileName );
	}

	for( i=0; i<apCutScene->mPageCount; i++ )
	{
		dCUT_PTR_DELOC( mpPages[ i ].mpName );
		dCUT_PTR_DELOC( mpPages[ i ].mpText );
	}

	for( i=0; i<apCutScene->mSampleCount; i++ )
	{
		dCUT_PTR_DELOC( mpSamples[ i ].mpName  );
		dCUT_PTR_DELOC( mpSamples[ i ].mpAsset );
	}

	for( i=0; i<apCutScene->mScriptCount; i++ )
	{
		sCutCmdDef *	lpDef;


		dCUT_PTR_DELOC( mpScripts[ i ].mpName  );
		for( j=0; j<apCutScene->mpScripts[ i ].mCommandCount; j++ )
		{
			lpDef = CutScene_CmdDefFindByID( apCutScene->mpScripts[ i ].mpCommands[ j ].mOp );

			switch( lpDef->mArg0 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_DELOC( mpScripts[ i ].mpCommands[ j ].mArg0 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg0 );
				break;
			}

			switch( lpDef->mArg1 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_DELOC( mpScripts[ i ].mpCommands[ j ].mArg1 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg1 );
				break;
			}

			switch( lpDef->mArg2 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_DELOC( mpScripts[ i ].mpCommands[ j ].mArg2 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg2 );
				break;
			}

			Endian_FromBigU16( &apCutScene->mpScripts[ i ].mpCommands[ j ].mOp );
		}
		dCUT_PTR_DELOC( mpScripts[ i ].mpCommands );
		Endian_FromBigU16( &apCutScene->mpScripts[ i ].mCommandCount );
	}

	for( i=0; i<apCutScene->mSpriteCount; i++ )
	{
		dCUT_PTR_DELOC( mpSprites[ i ].mpName );
		dCUT_PTR_DELOC( mpSprites[ i ].mInitialDef.mpAsset     );
		dCUT_PTR_DELOC( mpSprites[ i ].mInitialDef.mpAnimation );
		Endian_FromBigU16( &apCutScene->mpSprites[ i ].mInitialDef.mX      );
		Endian_FromBigU16( &apCutScene->mpSprites[ i ].mInitialDef.mY      );
		Endian_FromBigU32( &apCutScene->mpSprites[ i ].mInitialDef.mSpeed );
	}

	for( i=0; i<apCutScene->mTextCount; i++ )
	{
		dCUT_PTR_DELOC( mpTexts[ i ].mpName );
		dCUT_PTR_DELOC( mpTexts[ i ].mInitialDef.mpAsset );
		dCUT_PTR_DELOC( mpTexts[ i ].mInitialDef.mpPage  );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mX );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mY );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mWidth );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mHeight );
	}

	for( i=0; i<apCutScene->mVarCount; i++ )
	{
		dCUT_PTR_DELOC( mpVars[ i ].mpName );
		dCUT_PTR_DELOC( mpVars[ i ].mpVarName );
	}

	dCUT_PTR_DELOC( mpAnimations );
	dCUT_PTR_DELOC( mpAssets     );
	dCUT_PTR_DELOC( mpPages      );
	dCUT_PTR_DELOC( mpSamples    );
	dCUT_PTR_DELOC( mpScripts    );
	dCUT_PTR_DELOC( mpSprites    );
	dCUT_PTR_DELOC( mpTexts      );
	dCUT_PTR_DELOC( mpVars      );

	Endian_FromBigU16( &apCutScene->mAnimationCount );
	Endian_FromBigU16( &apCutScene->mAssetCount     );
	Endian_FromBigU16( &apCutScene->mPageCount      );
	Endian_FromBigU16( &apCutScene->mSampleCount    );
	Endian_FromBigU16( &apCutScene->mScriptCount    );
	Endian_FromBigU16( &apCutScene->mSpriteCount    );
	Endian_FromBigU16( &apCutScene->mTextCount      );
	Endian_FromBigU16( &apCutScene->mVarCount       );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_Relocate( sCutScene * apCutScene )
* ACTION   : CutScene_Relocate
* CREATION : 20.04.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	dCUT_PTR_RELOC( _aP )	{	if( apCutScene->_aP) { Endian_FromBigU32( &apCutScene->_aP ); *(U32*)&apCutScene->_aP += (U32)apCutScene; }	}

void	CutScene_Relocate( sCutScene * apCutScene )
{
	U16	i,j;

	Endian_FromBigU32( &apCutScene->mID      );	
	Endian_FromBigU32( &apCutScene->mVersion );

	Endian_FromBigU16( &apCutScene->mAnimationCount );
	Endian_FromBigU16( &apCutScene->mAssetCount     );
	Endian_FromBigU16( &apCutScene->mPageCount      );
	Endian_FromBigU16( &apCutScene->mSampleCount    );
	Endian_FromBigU16( &apCutScene->mScriptCount    );
	Endian_FromBigU16( &apCutScene->mSpriteCount    );
	Endian_FromBigU16( &apCutScene->mTextCount      );
	Endian_FromBigU16( &apCutScene->mVarCount       );

	dCUT_PTR_RELOC( mpAnimations );
	dCUT_PTR_RELOC( mpAssets     );
	dCUT_PTR_RELOC( mpPages      );
	dCUT_PTR_RELOC( mpSamples    );
	dCUT_PTR_RELOC( mpScripts    );
	dCUT_PTR_RELOC( mpSprites    );
	dCUT_PTR_RELOC( mpTexts      );
	dCUT_PTR_RELOC( mpVars       );

	for( i=0; i<apCutScene->mAnimationCount; i++ )
	{
		dCUT_PTR_RELOC( mpAnimations[ i ].mpName );
		Endian_FromBigU16( &apCutScene->mpAnimations[ i ].mFrame0 );
		Endian_FromBigU16( &apCutScene->mpAnimations[ i ].mFrame1 );
		Endian_FromBigU32( &apCutScene->mpAnimations[ i ].mSpeed  );
	}

	for( i=0; i<apCutScene->mAssetCount; i++ )
	{
		dCUT_PTR_RELOC( mpAssets[ i ].mpName     );
		dCUT_PTR_RELOC( mpAssets[ i ].mpContext  );
		dCUT_PTR_RELOC( mpAssets[ i ].mpFileName );
	}

	for( i=0; i<apCutScene->mPageCount; i++ )
	{
		dCUT_PTR_RELOC( mpPages[ i ].mpName );
		dCUT_PTR_RELOC( mpPages[ i ].mpText );
	}

	for( i=0; i<apCutScene->mSampleCount; i++ )
	{
		dCUT_PTR_RELOC( mpSamples[ i ].mpName  );
		dCUT_PTR_RELOC( mpSamples[ i ].mpAsset );
	}

	for( i=0; i<apCutScene->mScriptCount; i++ )
	{
		sCutCmdDef *	lpDef;


		dCUT_PTR_RELOC( mpScripts[ i ].mpName  );
		dCUT_PTR_RELOC( mpScripts[ i ].mpCommands );
		Endian_FromBigU16( &apCutScene->mpScripts[ i ].mCommandCount );

		for( j=0; j<apCutScene->mpScripts[ i ].mCommandCount; j++ )
		{
			Endian_FromBigU16( &apCutScene->mpScripts[ i ].mpCommands[ j ].mOp );

			lpDef = CutScene_CmdDefFindByID( apCutScene->mpScripts[ i ].mpCommands[ j ].mOp );

			switch( lpDef->mArg0 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_RELOC( mpScripts[ i ].mpCommands[ j ].mArg0 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg0 );
				break;
			}

			switch( lpDef->mArg1 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_RELOC( mpScripts[ i ].mpCommands[ j ].mArg1 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg1 );
				break;
			}

			switch( lpDef->mArg2 )
			{
			case	eCUT_CMD_ARG_ASSET:
			case	eCUT_CMD_ARG_ANIMATION:
			case	eCUT_CMD_ARG_MOVEMENT:
			case	eCUT_CMD_ARG_PAGE:
			case	eCUT_CMD_ARG_SAMPLE:
			case	eCUT_CMD_ARG_SCRIPT:
			case	eCUT_CMD_ARG_SPRITE:
			case	eCUT_CMD_ARG_TEXT:
			case	eCUT_CMD_ARG_VAR:
				dCUT_PTR_RELOC( mpScripts[ i ].mpCommands[ j ].mArg2 );
				break;
			default:
				Endian_FromBigU32( &apCutScene->mpScripts[ i ].mpCommands[ j ].mArg2 );
				break;
			}
		}
	}

	for( i=0; i<apCutScene->mSpriteCount; i++ )
	{
		dCUT_PTR_RELOC( mpSprites[ i ].mpName );
		dCUT_PTR_RELOC( mpSprites[ i ].mInitialDef.mpAsset     );
		dCUT_PTR_RELOC( mpSprites[ i ].mInitialDef.mpAnimation );
		Endian_FromBigU16( &apCutScene->mpSprites[ i ].mInitialDef.mX     );
		Endian_FromBigU16( &apCutScene->mpSprites[ i ].mInitialDef.mY     );
		Endian_FromBigU32( &apCutScene->mpSprites[ i ].mInitialDef.mSpeed );
	}

	for( i=0; i<apCutScene->mTextCount; i++ )
	{
		dCUT_PTR_RELOC( mpTexts[ i ].mpName );
		dCUT_PTR_RELOC( mpTexts[ i ].mInitialDef.mpAsset );
		dCUT_PTR_RELOC( mpTexts[ i ].mInitialDef.mpPage  );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mX );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mY );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mWidth );
		Endian_FromBigU16( &apCutScene->mpTexts[ i ].mInitialDef.mRect.mHeight );
	}

	for( i=0; i<apCutScene->mVarCount; i++ )
	{
		dCUT_PTR_RELOC( mpVars[ i ].mpName );
		dCUT_PTR_RELOC( mpVars[ i ].mpVarName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_GetSize( sCutScene * apCutScene )
* ACTION   : CutScene_GetSize
* CREATION : 20.04.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	mCUT_STRSIZE( _a )								\
	if( apCutScene->_a )								\
	{													\
		lSize += String_StrLen( apCutScene->_a );		\
		lSize++;										\
	}

U32	CutScene_GetSize( sCutScene * apCutScene )
{
	U32	lSize;
	U16	i;
	
	lSize = 0;
	if( apCutScene )
	{
		lSize = sizeof(sCutScene);

		for( i=0; i<apCutScene->mAnimationCount; i++ )
		{
			mCUT_STRSIZE( mpAnimations[ i ].mpName );
			lSize += sizeof(sCutAnimation);
		}

		for( i=0; i<apCutScene->mAssetCount; i++ )
		{
			mCUT_STRSIZE( mpAssets[ i ].mpName );
			mCUT_STRSIZE( mpAssets[ i ].mpFileName );
			mCUT_STRSIZE( mpAssets[ i ].mpContext );
			lSize += sizeof(sCutAsset);
		}

		for( i=0; i<apCutScene->mPageCount; i++ )
		{
			mCUT_STRSIZE( mpPages[ i ].mpName );
			mCUT_STRSIZE( mpPages[ i ].mpText );
			lSize += sizeof(sCutPage);
		}

		for( i=0; i<apCutScene->mSampleCount; i++ )
		{
			mCUT_STRSIZE( mpSamples[ i ].mpName );
			lSize += sizeof(sCutSample);
		}

		for( i=0; i<apCutScene->mScriptCount; i++ )
		{
			mCUT_STRSIZE( mpScripts[ i ].mpName );
			lSize +=(sizeof(sCutCommand) * apCutScene->mpScripts[i].mCommandCount);
			lSize += sizeof(sCutScript);
		}

		for( i=0; i<apCutScene->mSpriteCount; i++ )
		{
			mCUT_STRSIZE( mpSprites[ i ].mpName );
			lSize += sizeof(sCutSprite);
		}

		for( i=0; i<apCutScene->mTextCount; i++ )
		{
			mCUT_STRSIZE( mpTexts[ i ].mpName );
			lSize += sizeof(sCutText);
		}

		for( i=0; i<apCutScene->mVarCount; i++ )
		{
			mCUT_STRSIZE( mpVars[ i ].mpName );
			mCUT_STRSIZE( mpVars[ i ].mpVarName );
			lSize += sizeof(sCutVar);
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_CmdDefFind( const char * apString )
* ACTION   : CutScene_CmdDefFind
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sCutCmdDef *	CutScene_CmdDefFind( const char * apString )
{
	sCutCmdDef *	lpDef;
	
	lpDef = 0;

	if( apString )
	{
		lpDef =	gCutSceneCmdDefs;

		while( (lpDef->mpString) && (String_StrCmpi( apString, lpDef->mpString)) )
		{
			lpDef++;
		}
		if( !lpDef->mpString )
		{
			lpDef = 0;
		}
	}

	return( lpDef );
	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_CmdDefFindByID( U16 aID )
* ACTION   : CutScene_CmdDefFindByID
* CREATION : 26.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sCutCmdDef *	CutScene_CmdDefFindByID( U16 aID )
{
	sCutCmdDef *	lpDef;
	
	lpDef =	gCutSceneCmdDefs;

	while( (lpDef->mpString) && (lpDef->mIndex != aID) )
	{
		lpDef++;
	}
	if( !lpDef->mpString )
	{
		lpDef = 0;
	}

	return( lpDef );	
}


/* ################################################################################ */

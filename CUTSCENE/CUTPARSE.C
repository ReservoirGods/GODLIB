/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CUTPARSE.H"
#include	"CUTSCENE.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/LINKLIST/GOD_LL.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/STRING/STRING.H>
#include	<GODLIB/TOKENISE/TOKENISE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eCUTPARSE_CHUNK_NONE,

	eCUTPARSE_CHUNK_ANIMATION,
	eCUTPARSE_CHUNK_ASSET,
	eCUTPARSE_CHUNK_PAGE,
	eCUTPARSE_CHUNK_RECT,
	eCUTPARSE_CHUNK_SAMPLE,
	eCUTPARSE_CHUNK_SCRIPT,
	eCUTPARSE_CHUNK_SPRITE,
	eCUTPARSE_CHUNK_TEXT,
	eCUTPARSE_CHUNK_VAR,

	eCUTPARSE_CHUNK_LIMIT
};

enum
{
	eCUTPARSE_MODE_FIND,

	eCUTPARSE_MODE_PAGEFIND,
	eCUTPARSE_MODE_PAGEREAD,
	eCUTPARSE_MODE_STRUCTREAD,
	eCUTPARSE_MODE_SCRIPTREAD,

	eCUTPARSE_MODE_LIMIT,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sCutChunkItem
{
	U16						mType;
	void *					mpStruct;
	struct sCutChunkItem *	mpNext;
} sCutChunkItem;

typedef struct	sCutScriptItem
{
	sCutCommand				mCommand;
	struct sCutScriptItem *	mpNext;
} sCutScriptItem;


/* ###################################################################################
#  DATA
################################################################################### */


sTagString	gCutSceneParseChunkTags[ eCUTPARSE_CHUNK_LIMIT ] =
{
	{	eCUTPARSE_CHUNK_NONE,		"null"		},

	{	eCUTPARSE_CHUNK_ANIMATION,	"ANIMATION"	},
	{	eCUTPARSE_CHUNK_ASSET,		"ASSET"		},
	{	eCUTPARSE_CHUNK_PAGE,		"PAGE"		},
	{	eCUTPARSE_CHUNK_RECT,		"RECT"		},
	{	eCUTPARSE_CHUNK_SAMPLE,		"SAMPLE"	},
	{	eCUTPARSE_CHUNK_SCRIPT,		"SCRIPT"	},
	{	eCUTPARSE_CHUNK_SPRITE,		"SPRITE"	},
	{	eCUTPARSE_CHUNK_TEXT,		"TEXT"		},
	{	eCUTPARSE_CHUNK_VAR,		"VAR"		},
};

sTokeniserMember	gCutSceneAnimationMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,	"NAME",				mOFFSET( sCutAnimation, mpName )		},
	{	eTOKENISER_TYPE_FIX16_16,	0,	"SPEED",			mOFFSET( sCutAnimation, mSpeed )		},
	{	eTOKENISER_TYPE_S16,		0,	"FRAME0",			mOFFSET( sCutAnimation, mFrame0 )		},
	{	eTOKENISER_TYPE_S16,		0,	"FRAME1",			mOFFSET( sCutAnimation, mFrame1 )		},
	{	eTOKENISER_TYPE_U8,			0,	"FLAG_PINGPONG",	mOFFSET( sCutAnimation, mFlagPingPong )	},
	{	eTOKENISER_TYPE_U8,			0,	"FLAG_LOOP",		mOFFSET( sCutAnimation, mFlagLoop )		},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneAssetMembers[] =
{
	{	eTOKENISER_TYPE_STRING,	0,	"NAME",		mOFFSET( sCutAsset, mpName )		},
	{	eTOKENISER_TYPE_STRING,	0,	"CONTEXT",	mOFFSET( sCutAsset, mpContext )		},
	{	eTOKENISER_TYPE_STRING,	0,	"FILENAME",	mOFFSET( sCutAsset, mpFileName )	},
	{	0,	0,	0,	0	},
};


sTokeniserMember	gCutScenePageMembers[] =
{
	{	eTOKENISER_TYPE_STRING,	0,	"NAME",		mOFFSET( sCutPage, mpName )					},
	{	eTOKENISER_TYPE_STRING,	0,	"TEXT",		mOFFSET( sCutPage, mpText )					},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneSpriteMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,	"NAME",			mOFFSET( sCutSprite, mpName )					},
	{	eTOKENISER_TYPE_REFERENCE,	0,	"ASSET",		mOFFSET( sCutSprite, mInitialDef.mpAsset )		},
	{	eTOKENISER_TYPE_REFERENCE,	0,	"ANIMATION",	mOFFSET( sCutSprite, mInitialDef.mpAnimation )	},
	{	eTOKENISER_TYPE_S16,		0,	"X",			mOFFSET( sCutSprite, mInitialDef.mX )			},
	{	eTOKENISER_TYPE_S16,		0,	"Y",			mOFFSET( sCutSprite, mInitialDef.mY )			},
	{	eTOKENISER_TYPE_FIX16_16,	0,	"SPEED",		mOFFSET( sCutSprite, mInitialDef.mSpeed )		},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneSampleMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,	"NAME",			mOFFSET( sCutSample, mpName )		},
	{	eTOKENISER_TYPE_REFERENCE,	0,	"ASSET",		mOFFSET( sCutSample, mpAsset )		},
	{	eTOKENISER_TYPE_U8,			0,	"VOLUME",		mOFFSET( sCutSample, mVolume )		},
	{	eTOKENISER_TYPE_U8,			0,	"STEROPOS",		mOFFSET( sCutSample, mStereoPos )	},
	{	eTOKENISER_TYPE_U8,			0,	"FLAG_LOOP",	mOFFSET( sCutSample, mFlagLoop )	},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneScriptMembers[] =
{
	{	eTOKENISER_TYPE_STRING,	0,	"NAME",			mOFFSET( sCutScript, mpName )		},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneTextMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,	"NAME",		mOFFSET( sCutText, mpName )					},
	{	eTOKENISER_TYPE_REFERENCE,	0,	"ASSET",	mOFFSET( sCutText, mInitialDef.mpAsset )	},
	{	eTOKENISER_TYPE_REFERENCE,	0,	"PAGE",		mOFFSET( sCutText, mInitialDef.mpPage )		},
	{	eTOKENISER_TYPE_S16,		0,	"X",		mOFFSET( sCutText, mInitialDef.mRect.mX )	},
	{	eTOKENISER_TYPE_S16,		0,	"Y",		mOFFSET( sCutText, mInitialDef.mRect.mY )	},
	{	eTOKENISER_TYPE_S16,		0,	"WIDTH",	mOFFSET( sCutText, mInitialDef.mRect.mWidth )	},
	{	eTOKENISER_TYPE_S16,		0,	"HEIGHT",	mOFFSET( sCutText, mInitialDef.mRect.mHeight )	},
	{	0,	0,	0,	0	},
};

sTokeniserMember	gCutSceneVarMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,	"NAME",		mOFFSET( sCutVar, mpName )					},
	{	eTOKENISER_TYPE_STRING,		0,	"VARNAME",	mOFFSET( sCutVar, mpVarName )				},
};

sTagValue	gCutSceneStructDefs[] =
{
	{	eCUTPARSE_CHUNK_NONE,		0								},
	{	eCUTPARSE_CHUNK_ANIMATION,	(U32)gCutSceneAnimationMembers	},
	{	eCUTPARSE_CHUNK_ASSET,		(U32)gCutSceneAssetMembers		},
	{	eCUTPARSE_CHUNK_PAGE,		(U32)gCutScenePageMembers		},
	{	eCUTPARSE_CHUNK_SAMPLE,		(U32)gCutSceneSampleMembers		},
	{	eCUTPARSE_CHUNK_SCRIPT,		(U32)gCutSceneScriptMembers		},
	{	eCUTPARSE_CHUNK_SPRITE,		(U32)gCutSceneSpriteMembers		},
	{	eCUTPARSE_CHUNK_TEXT,		(U32)gCutSceneTextMembers		},
	{	eCUTPARSE_CHUNK_VAR,		(U32)gCutSceneVarMembers		},
};

sTagValue	gCutSceneStructSizes[] =
{
	{	eCUTPARSE_CHUNK_NONE,		0						},

	{	eCUTPARSE_CHUNK_ANIMATION,	sizeof(sCutAnimation)	},
	{	eCUTPARSE_CHUNK_ASSET,		sizeof(sCutAsset)		},
	{	eCUTPARSE_CHUNK_PAGE,		sizeof(sCutPage)		},
	{	eCUTPARSE_CHUNK_RECT,		sizeof(sCutRect)		},
	{	eCUTPARSE_CHUNK_SAMPLE,		sizeof(sCutSample)		},
	{	eCUTPARSE_CHUNK_SCRIPT,		sizeof(sCutScript)		},
	{	eCUTPARSE_CHUNK_SPRITE,		sizeof(sCutSprite)		},
	{	eCUTPARSE_CHUNK_TEXT,		sizeof(sCutText)		},
	{	eCUTPARSE_CHUNK_VAR,		sizeof(sCutVar)			},

	{	eCUTPARSE_CHUNK_LIMIT,		0						}
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			CutSceneParse_ScriptItemsSerialise( sCutScriptItem * apItems, sCutScript * apScript );
void			CutSceneParse_ScriptItemAdd( sCutScriptItem ** appItems, sTokeniserArgs * apArgs );
sCutScene *		CutSceneParse_ChunkItemsSerialise( sCutChunkItem * apItems );
void			CutSceneParse_ChunkItemsStrListBuild( sCutChunkItem * apItems, sStringList * apList );
void			CutSceneParse_SceneStringsFix( sCutScene * apScene );
void			CutSceneParse_StructStringsFix( sCutScene * apScene, sTokeniserMember * lpMembers, void * apStruct );
void *			CutSceneParse_StructFind( void * apStructs, U16 aStructCount, U32 aStructSize, U32 aOffset, const char * apName );
void			CutSceneParse_CommandsFix( sCutScene * apScene );
S32				CutSceneParse_ArgFix( sCutScene * apScene, U16 aArgType, char * apString );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : CutScene_Parse( const char * apText,const U32 aSize )
* ACTION   : CutScene_Parse
* CREATION : 22.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sCutScene *	CutScene_Parse( const char * apText,const U32 aSize )
{
	sTokeniserArgs		lArgs;
	U32					lOffset;
	U32					lSize;
	U32					lPageStart;
	U32					lPageEnd;
	U16					lMode;
	sTokeniserMember *	lpStructTable;
	sTokeniserMember *	lpMember;
	sCutScriptItem *	lpScriptItems;
	sCutChunkItem *		lpItems;
	sCutChunkItem *		lpItemNew;
	sCutScene *			lpScene;
	void *				lpStruct;

	lMode         = eCUTPARSE_MODE_FIND;
	lOffset       = 0;
	lpItems       = 0;
	lpStruct      = 0;
	lpStructTable = 0;
	lPageStart    = 0;
	lPageEnd      = 0;
	lpScriptItems = 0;

	while( lOffset < aSize )
	{
		switch( lMode )
		{
		case	eCUTPARSE_MODE_FIND:
			lOffset = TokeniserArgs_Build( &lArgs, apText, lOffset, aSize );
			if( TokeniserArgs_IsChunkHeader( &lArgs ) )
			{
				lMode  = (U16)sTagString_GetID( lArgs.mpStrings[ 1 ], gCutSceneParseChunkTags, eCUTPARSE_CHUNK_LIMIT );

				lpStructTable = (sTokeniserMember*)sTagValue_GetValue( lMode, gCutSceneStructDefs, eCUTPARSE_CHUNK_LIMIT );
				if( lpStructTable )
				{
					lSize = sTagValue_GetValue( lMode, gCutSceneStructSizes, eCUTPARSE_CHUNK_LIMIT );
					if( lSize )
					{
						lpStruct  = mMEMCALLOC( lSize );
						lpItemNew = (sCutChunkItem*)mMEMCALLOC( sizeof(sCutChunkItem) );
						GOD_LL_INSERT( lpItems, mpNext, lpItemNew );
						lpItemNew->mpStruct = lpStruct;
						lpItemNew->mType    = lMode;

						switch( lMode )
						{
						case	eCUTPARSE_CHUNK_PAGE:
							lMode = eCUTPARSE_MODE_PAGEFIND;
							break;
						case	eCUTPARSE_CHUNK_SCRIPT:
							lpScriptItems = 0;
							lMode = eCUTPARSE_MODE_SCRIPTREAD;
							break;
						default:
							lMode = eCUTPARSE_MODE_STRUCTREAD;
							break;
						}

						lpMember = lpStructTable;
						while( (lpMember->mpString) && (String_StrCmpi( lpMember->mpString, "NAME")) )
						{
							lpMember++;
						}
						if( lpMember->mpString )
						{
							U8 *	lpDst;
							char *	lpStr;
							lpDst  = (U8*)lpStruct;
							lpDst += lpMember->mOffset;

							lpStr = (char*)mMEMCALLOC( String_StrLen( lArgs.mpStrings[ 4 ] ) + 1 );
							String_StrCpy( lpStr, lArgs.mpStrings[ 4 ] );
							*(char**)lpDst = lpStr;
						}
					}
				}
			}
			break;

		case	eCUTPARSE_MODE_STRUCTREAD:
			lOffset = TokeniserArgs_Build( &lArgs, apText, lOffset, aSize );
			if( lArgs.mArgCount )
			{
				if( lArgs.mpStrings[0][0] == '}' )
				{
					lMode = eCUTPARSE_MODE_FIND;
				}
				else
				{
					TokeniserArgs_MemberBuild( &lArgs, lpStructTable, lpStruct );
				}
			}
			break;

		case	eCUTPARSE_MODE_PAGEFIND:
			lOffset = TokeniserArgs_Build( &lArgs, apText, lOffset, aSize );
			if( lArgs.mArgCount )
			{
				if( '{' == lArgs.mpStrings[0][0] )
				{
					while( (10==apText[lOffset]) || (13==apText[lOffset]) )
					{
						lOffset++;
					}
					lPageStart = lOffset;
					lMode      = eCUTPARSE_MODE_PAGEREAD;
				}
			}
			break;

		case	eCUTPARSE_MODE_PAGEREAD:
			lPageEnd = lOffset;
			lOffset  = TokeniserArgs_Build( &lArgs, apText, lOffset, aSize );
			if( lArgs.mArgCount )
			{
				if( '}' == lArgs.mpStrings[0][0] )
				{
					sCutPage *	lpPage;

					lSize          = (lPageEnd-lPageStart)+1;
					lpPage         = (sCutPage*)lpStruct;
					lpPage->mpText = (char*)mMEMCALLOC( (lPageEnd-lPageStart)+1 );
					Memory_Copy( lSize-1, &apText[ lPageStart ], lpPage->mpText );
					lMode       = eCUTPARSE_MODE_FIND;
				}
			}
			break;

		case	eCUTPARSE_MODE_SCRIPTREAD:
			lOffset  = TokeniserArgs_Build( &lArgs, apText, lOffset, aSize );
			if( lArgs.mArgCount )
			{
				if( '}' == lArgs.mpStrings[0][0] )
				{
					sCutScriptItem *	lpCmdNew;

					lpCmdNew = (sCutScriptItem*)mMEMCALLOC( sizeof(sCutScriptItem) );
					lpCmdNew->mCommand.mOp = eCUT_CMD_RETURN;

					GOD_LL_INSERT_TAIL( sCutScriptItem, lpScriptItems, mpNext, lpCmdNew );


					CutSceneParse_ScriptItemsSerialise( lpScriptItems, (sCutScript*)lpStruct );
					lMode         = eCUTPARSE_MODE_FIND;
					lpScriptItems = 0;
				}
				else
				{
					CutSceneParse_ScriptItemAdd( &lpScriptItems, &lArgs );
				}
			}
			break;

		}
	}

	lpScene = CutSceneParse_ChunkItemsSerialise( lpItems );

	return( lpScene );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_ScriptItemsSerialise(sCutScriptItem * apItems,sCutScript * apScript)
* ACTION   : CutSceneParse_ScriptItemsSerialise
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_ScriptItemsSerialise(sCutScriptItem * apItems,sCutScript * apScript)
{
	sCutScriptItem *	lpItem;
	sCutScriptItem *	lpNext;
	U32					lSize;

	if( apItems && apScript )
	{
		apScript->mCommandCount = 0;

		lpItem = apItems;
		while( lpItem )
		{
			apScript->mCommandCount++;
			lpItem = lpItem->mpNext;
		}

		if( apScript->mCommandCount )
		{
			lSize  = apScript->mCommandCount;
			lSize *= sizeof(sCutCommand);

			apScript->mpCommands = (sCutCommand*)mMEMCALLOC( lSize );

			lpItem = apItems;
			lSize  = 0;
			while( lpItem )
			{
				lpNext = lpItem->mpNext;
				apScript->mpCommands[ lSize ] = lpItem->mCommand;
				lSize++;
				mMEMFREE( lpItem );
				lpItem = lpNext;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_ScriptItemAdd(sCutScriptItem * apItems,sTokeniserArgs * apArgs)
* ACTION   : CutSceneParse_ScriptItemAdd
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_ScriptItemAdd(sCutScriptItem ** appItems,sTokeniserArgs * apArgs)
{
	sCutCmdDef *		lpDef;
	sCutScriptItem *	lpCmdNew;
	char *				lpStr;

	if( apArgs->mArgCount )
	{
		lpDef = CutScene_CmdDefFind( apArgs->mpStrings[0] );
		if( lpDef )
		{
			lpCmdNew = (sCutScriptItem*)mMEMCALLOC( sizeof(sCutScriptItem) );

			GOD_LL_INSERT_TAIL( sCutScriptItem, *appItems, mpNext, lpCmdNew );

			lpCmdNew->mCommand.mOp = lpDef->mIndex;
			if( apArgs->mArgCount >=2 )
			{
				lpStr = (char*)mMEMCALLOC( String_StrLen(apArgs->mpStrings[1]) + 1 );
				String_StrCpy( lpStr, apArgs->mpStrings[1] );
				lpCmdNew->mCommand.mArg0 = (U32)lpStr;
			}
			if( apArgs->mArgCount >=3 )
			{
				lpStr = (char*)mMEMCALLOC( String_StrLen(apArgs->mpStrings[2]) + 1 );
				String_StrCpy( lpStr, apArgs->mpStrings[2] );
				lpCmdNew->mCommand.mArg1 = (U32)lpStr;
			}
			if( apArgs->mArgCount >=4 )
			{
				lpStr = (char*)mMEMCALLOC( String_StrLen(apArgs->mpStrings[3]) + 1 );
				String_StrCpy( lpStr, apArgs->mpStrings[3] );
				lpCmdNew->mCommand.mArg2 = (U32)lpStr;
			}
		}
		else
		{
			DebugLog_Printf1( "CutScene CMD not found: %s\n", apArgs->mpStrings[0] );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sCutScene * CutSceneParse_ChunkItemsSerialise(sCutChunkItem * apItems)
* ACTION   : CutSceneParse_ChunkItemsSerialise
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sCutScene * CutSceneParse_ChunkItemsSerialise(sCutChunkItem * apItems)
{
	sCutChunkItem *	lpItem;
	sCutChunkItem *	lpNext;
	sStringList		lStrList;
	U32				lSize;
	U8 *			lpMem;
	U32				lScriptOff;
	U32				lOff;
	sCutScene *		lpScene;
	U16				i;
	U32				lCounts[ eCUTPARSE_CHUNK_LIMIT ];
	U32				lSizes[ eCUTPARSE_CHUNK_LIMIT ];

	lpScene = 0;

	if( apItems )
	{
		StringList_Init( &lStrList );
		CutSceneParse_ChunkItemsStrListBuild( apItems, &lStrList );

		for( i=0; i<eCUTPARSE_CHUNK_LIMIT; i++ )
		{
			lCounts[ i ] = 0;
			lSizes[ i ]  = 0;
		}

		lpItem = apItems;
		lSize  = sizeof(sCutScene);
		while( lpItem )
		{
			if( lpItem->mType < eCUTPARSE_CHUNK_LIMIT )
			{
				lSize                   += sTagValue_GetValue( lpItem->mType, gCutSceneStructSizes, eCUTPARSE_CHUNK_LIMIT );
				lSizes[ lpItem->mType ] += sTagValue_GetValue( lpItem->mType, gCutSceneStructSizes, eCUTPARSE_CHUNK_LIMIT );
				lCounts[ lpItem->mType ]++;
				if( eCUTPARSE_CHUNK_SCRIPT == lpItem->mType )
				{
					sCutScript *	lpScr;

					lpScr = (sCutScript*)lpItem->mpStruct;
					lSizes[ lpItem->mType ] += (lpScr->mCommandCount * sizeof(sCutCommand));
					lSize += (lpScr->mCommandCount * sizeof(sCutCommand));
				}
			}
			lpItem = lpItem->mpNext;
		}

		lSize += StringList_GetStringsSize( &lStrList );

		lpMem   = (U8*)mMEMCALLOC( lSize );
		lpScene = (sCutScene*)lpMem;
		lpScene->mVersion = dCUTSCENE_VERSION;
		lOff    = sizeof(sCutScene);

		lpScene->mpAnimations = (sCutAnimation*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_ANIMATION ];

		lpScene->mpAssets     = (sCutAsset*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_ASSET ];

		lpScene->mpPages      = (sCutPage*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_PAGE ];

		lpScene->mpSamples    = (sCutSample*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_SAMPLE ];

		lpScene->mpScripts    = (sCutScript*)&lpMem[ lOff ];
		lScriptOff            = (lOff + (sizeof(sCutScript) * lCounts[ eCUTPARSE_CHUNK_SCRIPT ]));
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_SCRIPT ];

		lpScene->mpSprites    = (sCutSprite*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_SPRITE ];

		lpScene->mpTexts      = (sCutText*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_TEXT ];

		lpScene->mpVars       = (sCutVar*)&lpMem[ lOff ];
		lOff                  = lOff + lSizes[ eCUTPARSE_CHUNK_VAR ];

		StringList_StringsSerialiseTo( &lStrList, &lpMem[ lOff ] );


		lpItem = apItems;
		while( lpItem )
		{
			lpNext = lpItem->mpNext;

			switch( lpItem->mType )
			{
			case	eCUTPARSE_CHUNK_ANIMATION:
				lpScene->mpAnimations[ lpScene->mAnimationCount ] = *(sCutAnimation*)lpItem->mpStruct;
				lpScene->mAnimationCount++;
				break;
			case	eCUTPARSE_CHUNK_ASSET:
				lpScene->mpAssets[ lpScene->mAssetCount ] = *(sCutAsset*)lpItem->mpStruct;
				lpScene->mAssetCount++;
				break;
			case	eCUTPARSE_CHUNK_PAGE:
				lpScene->mpPages[ lpScene->mPageCount ] = *(sCutPage*)lpItem->mpStruct;
				lpScene->mPageCount++;
				break;
			case	eCUTPARSE_CHUNK_SAMPLE:
				lpScene->mpSamples[ lpScene->mSampleCount ] = *(sCutSample*)lpItem->mpStruct;
				lpScene->mSampleCount++;
				break;
			case	eCUTPARSE_CHUNK_SCRIPT:
				{
					sCutScript *	lpScr;
					U32				lComSize;

					lpScr  = &lpScene->mpScripts[ lpScene->mScriptCount ];
					*lpScr = *(sCutScript*)lpItem->mpStruct;
					if( lpScr->mCommandCount )
					{
						lComSize = lpScr->mCommandCount * sizeof(sCutCommand);
						Memory_Copy( lComSize, lpScr->mpCommands, &lpMem[ lScriptOff ] );
						mMEMFREE( lpScr->mpCommands );
						lpScr->mpCommands = (sCutCommand*)&lpMem[ lScriptOff ];
						lScriptOff += lComSize;
					}
					lpScene->mScriptCount++;
				}
				break;
			case	eCUTPARSE_CHUNK_SPRITE:
				lpScene->mpSprites[ lpScene->mSpriteCount ] = *(sCutSprite*)lpItem->mpStruct;
				lpScene->mSpriteCount++;
				break;
			case	eCUTPARSE_CHUNK_TEXT:
				lpScene->mpTexts[ lpScene->mTextCount ] = *(sCutText*)lpItem->mpStruct;
				lpScene->mTextCount++;
				break;
			case	eCUTPARSE_CHUNK_VAR:
				lpScene->mpVars[ lpScene->mVarCount ] = *(sCutVar*)lpItem->mpStruct;
				lpScene->mVarCount++;
				break;
			}

			mMEMFREE( lpItem->mpStruct );
			mMEMFREE( lpItem );

			lpItem = lpNext;
		}

		CutSceneParse_SceneStringsFix( lpScene );
		CutSceneParse_CommandsFix( lpScene );
	}

	return( lpScene );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_ChunkItemsStrListBuild(sCutChunkItem * apItems,sStrList * apList)
* ACTION   : CutSceneParse_ChunkItemsStrListBuild
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_ChunkItemsStrListBuild(sCutChunkItem * apItems,sStringList * apList)
{
	sCutChunkItem *		lpItem;
	sTokeniserMember *	lpMembers;

	if( apItems && apList )
	{
		lpItem = apItems;
		while( lpItem )
		{
			lpMembers = (sTokeniserMember*)sTagValue_GetValue( lpItem->mType, gCutSceneStructDefs, eCUTPARSE_CHUNK_LIMIT );
			if( lpMembers )
			{
				while( lpMembers->mpString )
				{
					if( eTOKENISER_TYPE_STRING == lpMembers->mType )
					{
						sStringListItem *	lpStrIt;
						U8 *				lpStruct;
						char *				lpString;


						lpStruct = (U8*)lpItem->mpStruct;
						lpString =*(char**)&lpStruct[ lpMembers->mOffset ];
						lpStrIt  = StringList_ItemCreate( apList, lpString );
						mMEMFREE( lpString );
						*(char**)&lpStruct[ lpMembers->mOffset ] = (char*)lpStrIt;
					}
					lpMembers++;
				}
			}
			lpItem = lpItem->mpNext;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_SceneStringsFix(sCutScene * apScene)
* ACTION   : CutSceneParse_StringsFix
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_SceneStringsFix(sCutScene * apScene)
{
	U16	i;

	for( i=0; i<apScene->mAnimationCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneAnimationMembers, &apScene->mpAnimations[ i ] );
	}

	for( i=0; i<apScene->mAssetCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneAssetMembers, &apScene->mpAssets[ i ] );
	}

	for( i=0; i<apScene->mPageCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutScenePageMembers, &apScene->mpPages[ i ] );
	}

	for( i=0; i<apScene->mSampleCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneSampleMembers, &apScene->mpSamples[ i ] );
	}

	for( i=0; i<apScene->mScriptCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneScriptMembers, &apScene->mpScripts[ i ] );
	}

	for( i=0; i<apScene->mSpriteCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneSpriteMembers, &apScene->mpSprites[ i ] );
	}

	for( i=0; i<apScene->mTextCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneTextMembers, &apScene->mpTexts[ i ] );
	}

	for( i=0; i<apScene->mVarCount; i++ )
	{
		CutSceneParse_StructStringsFix( apScene, gCutSceneVarMembers, &apScene->mpVars[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_StructStringsFix(sCutScene * apScene,sTokeniserMember * lpMembers,void * apStruct)
* ACTION   : CutSceneParse_StructStringsFix
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_StructStringsFix(sCutScene * apScene,sTokeniserMember * lpMembers,void * apStruct)
{
	sStringListItem *	lpStrIt;
	U16					lChunk;
	char *				lpString;
	U8 *				lpStruct;
	void *				lpRef;

	if( apScene && lpMembers && apStruct )
	{
		lpStruct = (U8*)apStruct;
		while( lpMembers->mpString )
		{
			switch( lpMembers->mType )
			{
			case	eTOKENISER_TYPE_STRING:
				lpStrIt = *(sStringListItem**)&lpStruct[ lpMembers->mOffset ];
				*(char**)&lpStruct[ lpMembers->mOffset ] = lpStrIt->mpString->mpChars;
				break;
			case	eTOKENISER_TYPE_REFERENCE:
				lpString = *(char**)&lpStruct[ lpMembers->mOffset ];
				if( lpString )
				{
					lChunk = (U16)sTagString_GetID( lpMembers->mpString, gCutSceneParseChunkTags, eCUTPARSE_CHUNK_LIMIT );
					lpRef  = 0;
					switch( lChunk )
					{
					case	eCUTPARSE_CHUNK_ANIMATION:
						lpRef = CutSceneParse_StructFind( apScene->mpAnimations, apScene->mAnimationCount, sizeof(sCutAnimation), mOFFSET( sCutAnimation, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_ASSET:
						lpRef = CutSceneParse_StructFind( apScene->mpAssets, apScene->mAssetCount, sizeof(sCutAsset), mOFFSET( sCutAsset, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_PAGE:
						lpRef = CutSceneParse_StructFind( apScene->mpPages, apScene->mPageCount, sizeof(sCutPage), mOFFSET( sCutPage, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_SAMPLE:
						lpRef = CutSceneParse_StructFind( apScene->mpSamples, apScene->mSampleCount, sizeof(sCutSample), mOFFSET( sCutSample, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_SCRIPT:
						lpRef = CutSceneParse_StructFind( apScene->mpScripts, apScene->mScriptCount, sizeof(sCutScript), mOFFSET( sCutScript, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_SPRITE:
						lpRef = CutSceneParse_StructFind( apScene->mpSprites, apScene->mSpriteCount, sizeof(sCutSprite), mOFFSET( sCutSprite, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_TEXT:
						lpRef = CutSceneParse_StructFind( apScene->mpTexts, apScene->mTextCount, sizeof(sCutText), mOFFSET( sCutText, mpName ), lpString );
						break;
					case	eCUTPARSE_CHUNK_VAR:
						lpRef = CutSceneParse_StructFind( apScene->mpVars, apScene->mVarCount, sizeof(sCutVar), mOFFSET( sCutVar, mpName ), lpString );
						break;
					}
					mMEMFREE( lpString );
					*(void**)&lpStruct[ lpMembers->mOffset ] = lpRef;
				}
				break;
			}
			lpMembers++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void * CutSceneParse_StructFind(void * apStructs,U16 aStructCount,U32 aStructSize,U32 aOffset,const char * apName)
* ACTION   : CutSceneParse_StructFind
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void * CutSceneParse_StructFind(void * apStructs,U16 aStructCount,U32 aStructSize,U32 aOffset,const char * apName)
{
	U8 *	lpStruct;
	char *	lpString;
	U16		i;

	if( apStructs && apName )
	{
		lpStruct = (U8*)apStructs;
		for( i=0; i<aStructCount; i++ )
		{
			lpString  = *(char**)&lpStruct[ aOffset ];
			if( lpString )
			{
				if( 0 == String_StrCmpi( apName, lpString ) )
				{
					return( lpStruct );
				}
			}
			lpStruct += aStructSize;
		}
	}

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void CutSceneParse_CommandsFix(sCutScene * apScene)
* ACTION   : CutSceneParse_CommandsFix
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void CutSceneParse_CommandsFix(sCutScene * apScene)
{
	U16	i,j;
	sCutScript *	lpScript;
	sCutCmdDef *	lpDef;
	sCutCommand *	lpCmd;
	U32				lArg0;
	U32				lArg1;
	U32				lArg2;

	for( i=0; i<apScene->mScriptCount; i++ )
	{
		lpScript = &apScene->mpScripts[ i ];
		for( j=0; j<lpScript->mCommandCount; j++ )
		{
			lpCmd =&lpScript->mpCommands[ j ];
			lpDef = CutScene_CmdDefFindByID( lpCmd->mOp );
			if( lpDef )
			{
				lArg0 = CutSceneParse_ArgFix( apScene, lpDef->mArg0, (char*)lpCmd->mArg0 );
				lArg1 = CutSceneParse_ArgFix( apScene, lpDef->mArg1, (char*)lpCmd->mArg1 );
				lArg2 = CutSceneParse_ArgFix( apScene, lpDef->mArg2, (char*)lpCmd->mArg2 );
			}
			else
			{
				lArg0 = 0;
				lArg1 = 0;
				lArg2 = 0;
			}
			if( lpCmd->mArg0 )
			{
				mMEMFREE( (void*)lpCmd->mArg0 )
			}
			if( lpCmd->mArg1 )
			{
				mMEMFREE( (void*)lpCmd->mArg1 );
			}
			if( lpCmd->mArg2 )
			{
				mMEMFREE( (void*)lpCmd->mArg2 );
			}
			lpCmd->mArg0 = lArg0;
			lpCmd->mArg1 = lArg1;
			lpCmd->mArg2 = lArg2;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : S32 CutSceneParse_ArgFix(sCutScene * apScene,U16 aArgType,char * apString)
* ACTION   : CutSceneParse_ArgFix
* CREATION : 25.04.2005 PNK
*-----------------------------------------------------------------------------------*/

S32 CutSceneParse_ArgFix(sCutScene * apScene,U16 aArgType,char * apString)
{
	S32		lRes;
	FP32	lFloat;

	lRes = 0;

	switch( aArgType )
	{
	case	eCUT_CMD_ARG_NONE:
		break;
	case	eCUT_CMD_ARG_FIX16_16:
#ifndef dGODLIB_COMPILER_AHCC
		if( apString )
		{
			S32	lFrac,lInt;

			lFloat = (FP32)atof( apString );
			lInt   = (S32)lFloat;
			lFloat = lFloat - (FP32)lInt;
			lFloat = lFloat * 65536.f;
			lFrac  = (S32)lFloat;
			lInt <<= 16L;
			lInt  |= lFrac;

			lRes = lInt;
		}
#endif
		break;
	case	eCUT_CMD_ARG_FLOAT:
#ifndef dGODLIB_COMPILER_AHCC
		if( apString )
		{
			lFloat = (FP32)atof( apString );
/*			Memory_Copy( 4, &lFloat, &lRes );*/
			lRes =*( S32* )&lFloat;
		}
#endif
		break;
	case	eCUT_CMD_ARG_INT:
		if( apString )
		{
			lRes = String_ToValue( apString );
		}
		break;

	case	eCUT_CMD_ARG_ASSET:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpAssets, apScene->mAssetCount, sizeof(sCutAsset), mOFFSET( sCutAsset, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find asset %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_ANIMATION:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpAnimations, apScene->mAnimationCount, sizeof(sCutAnimation), mOFFSET( sCutAnimation, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find animation %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_PAGE:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpPages, apScene->mPageCount, sizeof(sCutPage), mOFFSET( sCutPage, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find page %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_SAMPLE:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpSamples, apScene->mSampleCount, sizeof(sCutSample), mOFFSET( sCutSample, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find sample %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_SCRIPT:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpScripts, apScene->mScriptCount, sizeof(sCutScript), mOFFSET( sCutScript, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find script %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_SPRITE:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpSprites, apScene->mSpriteCount, sizeof(sCutSprite), mOFFSET( sCutSprite, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find sprite %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_TEXT:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpTexts, apScene->mTextCount, sizeof(sCutText), mOFFSET( sCutText, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find text %s\n", apString );
		}
		break;
	case	eCUT_CMD_ARG_VAR:
		lRes = (U32)CutSceneParse_StructFind( apScene->mpVars, apScene->mVarCount, sizeof(sCutVar), mOFFSET( sCutVar, mpName ), apString );
		if( !lRes )
		{
			DebugLog_Printf1( "error: Can't find Var %s\n", apString );
		}
		break;
	}

	return( lRes );
}


/* ################################################################################ */

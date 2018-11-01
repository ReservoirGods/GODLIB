/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FEDPARSE.H"

#include	"FED.H"

#include	<STRING.H>

#include	<GODLIB/GEMDOS/GEMDOS.H>
#include	<GODLIB/HASHLIST/HASHLIST.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/TOKENISE/TOKENISE.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eFEDPARSE_TYPE_NONE,
	eFEDPARSE_TYPE_U8,
	eFEDPARSE_TYPE_S8,
	eFEDPARSE_TYPE_U16,
	eFEDPARSE_TYPE_S16,
	eFEDPARSE_TYPE_U32,
	eFEDPARSE_TYPE_S32,
	eFEDPARSE_TYPE_FP32,
	eFEDPARSE_TYPE_STRING,

	eFEDPARSE_TYPE_CONTROL_LIST,
	eFEDPARSE_TYPE_ITEM_LIST,
	eFEDPARSE_TYPE_SPRITE_LIST,

	eFEDPARSE_TYPE_ALIGN,
	eFEDPARSE_TYPE_COMPARE,
	eFEDPARSE_TYPE_CONTROL,
	eFEDPARSE_TYPE_FADE,
	eFEDPARSE_TYPE_WIPE,

	eFEDPARSE_TYPE_pASSET,
	eFEDPARSE_TYPE_pCALL,
	eFEDPARSE_TYPE_pCONTROLLIST,
	eFEDPARSE_TYPE_pFONTGROUP,
	eFEDPARSE_TYPE_pLIST,
	eFEDPARSE_TYPE_pLOCK,
	eFEDPARSE_TYPE_pPAGE,
	eFEDPARSE_TYPE_pPAGESTYLE,
	eFEDPARSE_TYPE_pSAMPLE,
	eFEDPARSE_TYPE_pSLIDER,
	eFEDPARSE_TYPE_pSPRITE,
	eFEDPARSE_TYPE_pSPRITEGROUP,
	eFEDPARSE_TYPE_pSPRITELIST,
	eFEDPARSE_TYPE_pTEXT,
	eFEDPARSE_TYPE_pTRANS,
	eFEDPARSE_TYPE_pVAR,

	eFEDPARSE_TYPE_LIMIT
};

enum
{
	eFEDPARSE_CHUNK_ASSET,
	eFEDPARSE_CHUNK_CALL,
	eFEDPARSE_CHUNK_CONTROL,
	eFEDPARSE_CHUNK_CONTROLLIST,
	eFEDPARSE_CHUNK_FONT,
	eFEDPARSE_CHUNK_FONTGROUP,
	eFEDPARSE_CHUNK_IMAGE,
	eFEDPARSE_CHUNK_LINK,
	eFEDPARSE_CHUNK_LIST,
	eFEDPARSE_CHUNK_LISTITEM,
	eFEDPARSE_CHUNK_LOCK,
	eFEDPARSE_CHUNK_PAGE,
	eFEDPARSE_CHUNK_PAGESTYLE,
	eFEDPARSE_CHUNK_SAMPLE,
	eFEDPARSE_CHUNK_SLIDER,
	eFEDPARSE_CHUNK_SPRITE,
	eFEDPARSE_CHUNK_SPRITEGROUP,
	eFEDPARSE_CHUNK_SPRITELIST,
	eFEDPARSE_CHUNK_TEXT,
	eFEDPARSE_CHUNK_TRANSITION,
	eFEDPARSE_CHUNK_VAR,

	eFEDPARSE_CHUNK_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct	sFedParserMember
{
	U16		mType;
	char *	mpString;
	U32		mOffset;
} sFedParserMember;

typedef	struct	sFedParserMemberTag
{
	char *				mpName;
	sFedParserMember *	mpMember;
	U32					mIndex;
} sFedParserMemberTag;

typedef	struct	sFedParserList
{
	char *				mpChunkTitle;
	sFedParserMember *	mpMembers;
	U32					mItemSize;
	U16					mItemIndex;
	U16 				mItemCount;
	U32 *				mpHashes;
	U8 *				mpData;
} sFedParserList;

typedef	struct	sFedParser
{
	U8 *				mpData;
	U16					mChunkIndex;
	U32					mDataSize;

	U32					mStringTempSize;
	U32					mStringGlobalSize;
	char *				mpSerialisedStrings;
	char *				mpCurrentChunkTitle;
	char *				mpCurrentChunkName;
	U32					mSerialisedStringsOffset;
	U32					mSerialisedStringsSize;

	U32 *				mpArrayBase;
	U32					mArrayOffset;
	U32					mArraySize;

	char *				mpStringBase;
	U32					mStringSize;
	U32					mStringOffset;

	U16					mItemArrayCount;

	sFedHeader *		mpHeader;
	sFedParserMember *	mpMembers;
	sHashList			mHashLists[ eFEDPARSE_CHUNK_LIMIT ];
	sFedParserList		mLists[ eFEDPARSE_CHUNK_LIMIT ];
} sFedParser;


/* ###################################################################################
#  DATA
################################################################################### */

void	FedParse_Pass1_Init( const char * apTitle, const U16 aIndex, const char * apName );
void	FedParse_Pass1_DeInit( void );
void	FedParse_Pass1_Var( sTokeniserArgs * apArgs );

void	FedParse_Pass2_Init( const char * apTitle, const U16 aIndex, const char * apName );

void	FedParse_Pass3_Init( const char * apTitle, const U16 aIndex, const char * apName );
void	FedParse_Pass3_DeInit( void );
void	FedParse_Pass3_Var( sTokeniserArgs * apArgs );

sTagString	gFedParseChunkNames[ eFEDPARSE_CHUNK_LIMIT ] =
{
	{	eFEDPARSE_CHUNK_ASSET,			"ASSET"			},
	{	eFEDPARSE_CHUNK_CALL,			"CALL"			},
	{	eFEDPARSE_CHUNK_CONTROL,		"CONTROL"		},
	{	eFEDPARSE_CHUNK_CONTROLLIST,	"CONTROLLIST"	},
	{	eFEDPARSE_CHUNK_FONT,			"FONT"			},
	{	eFEDPARSE_CHUNK_FONTGROUP,		"FONTGROUP"		},
	{	eFEDPARSE_CHUNK_IMAGE,			"IMAGE"			},
	{	eFEDPARSE_CHUNK_LINK,			"LINK"			},
	{	eFEDPARSE_CHUNK_LIST,			"LIST"			},
	{	eFEDPARSE_CHUNK_LISTITEM,		"LISTITEM"		},
	{	eFEDPARSE_CHUNK_LOCK,			"LOCK"			},
	{	eFEDPARSE_CHUNK_PAGE,			"PAGE"			},
	{	eFEDPARSE_CHUNK_PAGESTYLE,		"PAGESTYLE"		},
	{	eFEDPARSE_CHUNK_SAMPLE,			"SAMPLE"		},
	{	eFEDPARSE_CHUNK_SLIDER,			"SLIDER"		},
	{	eFEDPARSE_CHUNK_SPRITE,			"SPRITE"		},
	{	eFEDPARSE_CHUNK_SPRITEGROUP,	"SPRITEGROUP"	},
	{	eFEDPARSE_CHUNK_SPRITELIST,		"SPRITELIST"	},
	{	eFEDPARSE_CHUNK_TEXT,			"TEXT"			},
	{	eFEDPARSE_CHUNK_TRANSITION,		"TRANSITION"	},
	{	eFEDPARSE_CHUNK_VAR,			"VAR"			},
};

sTagString	gFedParseAlignNames[ eFED_ALIGN_LIMIT ] =
{
	{	eFED_ALIGN_LEFT,	"LEFT"		},
	{	eFED_ALIGN_RIGHT,	"RIGHT"		},
	{	eFED_ALIGN_CENTRE,	"CENTRE"	},
};

sTagString	gFedParseCompareNames[ eFED_COMPARE_LIMIT ] =
{
	{	eFED_COMPARE_GREATER,	"GREATER"	},
	{	eFED_COMPARE_EQUAL,		"EQUAL"		},
	{	eFED_COMPARE_LESS,		"LESS"		},
};

sTagString	gFedParseControlTypeNames[ eFED_CONTROL_LIMIT ] =
{
	{	eFED_CONTROL_NONE,		"NONE"		},
	{	eFED_CONTROL_LINK,		"LINK"		},
	{	eFED_CONTROL_CALL,		"CALL"		},
	{	eFED_CONTROL_LIST,		"LIST"		},
	{	eFED_CONTROL_SLIDER,	"SLIDER"	},
};

sTagString	gFedParseFadeNames[ eFED_FADE_LIMIT ] =
{
	{	eFED_FADE_NONE,		"NONE"		},
	{	eFED_FADE_BG,		"BG"		},
	{	eFED_FADE_PAL,		"PAL"		},
	{	eFED_FADE_RGB,		"RGB"		},
};

sTagString	gFedParseWipeNames[ eFED_WIPE_LIMIT ] =
{
	{	eFED_WIPE_NONE,		"NONE"		},
	{	eFED_WIPE_RANDOM,	"RANDOM"	},
	{	eFED_WIPE_SPECIFIC,	"SPECIFIC"	},
};

sTagValue	gFedParseStructSizes[ eFEDPARSE_CHUNK_LIMIT ] =
{
	{	eFEDPARSE_CHUNK_ASSET,			sizeof( sFedAsset )			},
	{	eFEDPARSE_CHUNK_CALL,			sizeof( sFedCall )			},
	{	eFEDPARSE_CHUNK_CONTROL,		sizeof( sFedControl)		},
	{	eFEDPARSE_CHUNK_CONTROLLIST,	sizeof( sFedControlList)	},
	{	eFEDPARSE_CHUNK_FONTGROUP,		sizeof( sFedFontGroup )		},
	{	eFEDPARSE_CHUNK_LOCK,			sizeof( sFedLock )			},
	{	eFEDPARSE_CHUNK_LIST,			sizeof( sFedList )			},
	{	eFEDPARSE_CHUNK_LISTITEM,		sizeof( sFedListItem )		},
	{	eFEDPARSE_CHUNK_PAGE,			sizeof( sFedPage )			},
	{	eFEDPARSE_CHUNK_PAGESTYLE,		sizeof( sFedPageStyle )		},
	{	eFEDPARSE_CHUNK_SAMPLE,			sizeof( sFedSample )		},
	{	eFEDPARSE_CHUNK_SLIDER,			sizeof( sFedSlider )		},
	{	eFEDPARSE_CHUNK_SPRITE,			sizeof( sFedSprite )		},
	{	eFEDPARSE_CHUNK_SPRITEGROUP,	sizeof( sFedSpriteGroup )	},
	{	eFEDPARSE_CHUNK_SPRITELIST,		sizeof( sFedSpriteList )	},
	{	eFEDPARSE_CHUNK_TEXT,			sizeof( sFedText )			},
	{	eFEDPARSE_CHUNK_TRANSITION,		sizeof( sFedTransition )	},
	{	eFEDPARSE_CHUNK_VAR,			sizeof( sFedVar )			},
};

sFedParserMember	gFedParseAssetMembers[] =
{
	{	eFEDPARSE_TYPE_STRING,	"FILENAME",	mOFFSET( sFedAsset, mpFileName)	},
	{	eFEDPARSE_TYPE_STRING,	"CONTEXT",	mOFFSET( sFedAsset, mpContext)	},
	{	eFEDPARSE_TYPE_NONE,	0,			0								},
};

sFedParserMember	gFedParseCallMembers[] =
{
	{	eFEDPARSE_TYPE_pVAR,	"CALL_VAR",		mOFFSET( sFedCall, mpCallVar)		},
	{	eFEDPARSE_TYPE_S32,		"CALL_VALUE",	mOFFSET( sFedCall, mCallValue)		},
	{	eFEDPARSE_TYPE_pPAGE,	"PAGE_RETURN",	mOFFSET( sFedCall, mpPageReturn)	},
	{	eFEDPARSE_TYPE_NONE,	0,				0									},
};

sFedParserMember	gFedParseControlMembers[] =
{
	{	eFEDPARSE_TYPE_S16,			"X",			mOFFSET( sFedControl, mPos.mX)			},
	{	eFEDPARSE_TYPE_S16,			"Y",			mOFFSET( sFedControl, mPos.mY)			},
	{	eFEDPARSE_TYPE_CONTROL,		"CONTROL_TYPE",	mOFFSET( sFedControl, mControlType)		},
	{	eFEDPARSE_TYPE_pCALL,		"CALL",			mOFFSET( sFedControl, mpCall)			},
	{	eFEDPARSE_TYPE_pLIST,		"LIST",			mOFFSET( sFedControl, mpList)			},
	{	eFEDPARSE_TYPE_pLOCK,		"LOCK",			mOFFSET( sFedControl, mpLock)			},
	{	eFEDPARSE_TYPE_pPAGE,		"LINK",			mOFFSET( sFedControl, mpLink)			},
	{	eFEDPARSE_TYPE_pSLIDER,		"SLIDER",		mOFFSET( sFedControl, mpSlider)			},
	{	eFEDPARSE_TYPE_pSPRITE,		"SPRITE",		mOFFSET( sFedControl, mpSprite)			},
	{	eFEDPARSE_TYPE_pTEXT,		"TITLE",		mOFFSET( sFedControl, mpTitle)			},
	{	eFEDPARSE_TYPE_pVAR,		"SET_VAR",		mOFFSET( sFedControl, mpSetVar)			},
	{	eFEDPARSE_TYPE_S32,			"SET_VALUE",	mOFFSET( sFedControl, mSetValue)		},
	{	eFEDPARSE_TYPE_NONE,		0,				0										},
};

sFedParserMember	gFedParseControlListMembers[] =
{
	{	eFEDPARSE_TYPE_CONTROL_LIST,	"CONTROL",	mOFFSET( sFedControlList, mppControls)	},
	{	eFEDPARSE_TYPE_U16,				"INDEX",	mOFFSET( sFedControlList, mControlIndex)	},
	{	eFEDPARSE_TYPE_NONE,	0,			0							},
};

sFedParserMember	gFedParseFontGroupMembers[] =
{
	{	eFEDPARSE_TYPE_pASSET,	"FONT_LOCKED",			mOFFSET( sFedFontGroup, mpFontLocked)			},
	{	eFEDPARSE_TYPE_pASSET,	"FONT_LOCKEDSELECTED",	mOFFSET( sFedFontGroup, mpFontLockedSelected)	},
	{	eFEDPARSE_TYPE_pASSET,	"FONT_NORMAL",			mOFFSET( sFedFontGroup, mpFontNormal)			},
	{	eFEDPARSE_TYPE_pASSET,	"FONT_SELECTED",		mOFFSET( sFedFontGroup, mpFontSelected)			},
	{	eFEDPARSE_TYPE_NONE,	0,			0							},
};


sFedParserMember	gFedParseListMembers[] =
{
	{	eFEDPARSE_TYPE_S16,			"X",	mOFFSET( sFedList, mPos.mX)		},
	{	eFEDPARSE_TYPE_S16,			"Y",	mOFFSET( sFedList, mPos.mY)		},
	{	eFEDPARSE_TYPE_ITEM_LIST,	"ITEM",	mOFFSET( sFedList, mppItems)	},
	{	eFEDPARSE_TYPE_pVAR,		"VAR",	mOFFSET( sFedList, mpVar)		},
	{	eFEDPARSE_TYPE_NONE,		0,		0								},
	{	eFEDPARSE_TYPE_NONE,		0,		0								},
};

sFedParserMember	gFedParseListItemMembers[] =
{
	{	eFEDPARSE_TYPE_S16,				"X",			mOFFSET( sFedListItem, mPos.mX)			},
	{	eFEDPARSE_TYPE_S16,				"Y",			mOFFSET( sFedListItem, mPos.mY)			},
	{	eFEDPARSE_TYPE_pSPRITEGROUP,	"SPRITEGROUP",	mOFFSET( sFedListItem, mpSpriteGroup)	},
	{	eFEDPARSE_TYPE_pTEXT,			"TEXT",			mOFFSET( sFedListItem, mpText)			},
	{	eFEDPARSE_TYPE_pLOCK,			"LOCK",			mOFFSET( sFedListItem, mpLock)			},
	{	eFEDPARSE_TYPE_NONE,			0,				0										},
};

sFedParserMember	gFedParseLockMembers[] =
{
	{	eFEDPARSE_TYPE_pVAR,	"LOCK_VAR",		mOFFSET( sFedLock, mpLockVar)		},
	{	eFEDPARSE_TYPE_S32,		"LOCK_VALUE",	mOFFSET( sFedLock, mLockValue)		},
	{	eFEDPARSE_TYPE_COMPARE,	"LOCK_COMPARE",	mOFFSET( sFedLock, mLockCompare)	},
	{	eFEDPARSE_TYPE_pVAR,	"VIS_VAR",		mOFFSET( sFedLock, mpVisVar)		},
	{	eFEDPARSE_TYPE_S32,		"VIS_VALUE",	mOFFSET( sFedLock, mVisValue)		},
	{	eFEDPARSE_TYPE_COMPARE,	"VIS_COMPARE",	mOFFSET( sFedLock, mVisCompare)		},
	{	eFEDPARSE_TYPE_NONE,	0,			0										},
};

sFedParserMember	gFedParsePageMembers[] =
{
	{	eFEDPARSE_TYPE_pTEXT,		"TITLE",			mOFFSET( sFedPage, mpTitle)			},
	{	eFEDPARSE_TYPE_pASSET,		"BG",				mOFFSET( sFedPage, mpBG)			},
	{	eFEDPARSE_TYPE_pSPRITE,		"CURSOR",			mOFFSET( sFedPage, mpCursor)		},
	{	eFEDPARSE_TYPE_pCONTROLLIST,"CONTROL_LIST",		mOFFSET( sFedPage, mpControlList)	},
	{	eFEDPARSE_TYPE_pSAMPLE,		"SAMPLE_MOVE",		mOFFSET( sFedPage, mpSampleMove)	},
	{	eFEDPARSE_TYPE_pSAMPLE,		"SAMPLE_SELECT",	mOFFSET( sFedPage, mpSampleSelect)	},
	{	eFEDPARSE_TYPE_pPAGESTYLE,	"PAGE_STYLE",		mOFFSET( sFedPage, mpPageStyle)		},
	{	eFEDPARSE_TYPE_pSPRITELIST,	"SPRITE_LIST",		mOFFSET( sFedPage, mpSpriteList)	},
	{	eFEDPARSE_TYPE_pPAGE,		"PAGE_PARENT",		mOFFSET( sFedPage, mpPageParent)	},
	{	eFEDPARSE_TYPE_NONE,		0,					0									},
};

sFedParserMember	gFedParsePageStyleMembers[] =
{
	{	eFEDPARSE_TYPE_pASSET,		"PAGE_TITLE_FONT",	mOFFSET( sFedPageStyle, mpPageTitleFont)	},
	{	eFEDPARSE_TYPE_pASSET,		"PAGE_BG",			mOFFSET( sFedPageStyle, mpPageBG)		},
	{	eFEDPARSE_TYPE_pFONTGROUP,	"CONTROL_FONTS",	mOFFSET( sFedPageStyle, mpControlFonts)	},
	{	eFEDPARSE_TYPE_pSAMPLE,		"SAMPLE_MOVE",		mOFFSET( sFedPageStyle, mpSampleMove)	},
	{	eFEDPARSE_TYPE_pSAMPLE,		"SAMPLE_SELECT",	mOFFSET( sFedPageStyle, mpSampleSelect)	},
	{	eFEDPARSE_TYPE_pSPRITE,		"CURSOR",			mOFFSET( sFedPageStyle, mpCursor)		},
	{	eFEDPARSE_TYPE_pTRANS,		"INTRO_TRANS",		mOFFSET( sFedPageStyle, mpIntroTrans)	},
	{	eFEDPARSE_TYPE_pTRANS,		"OUTRO_TRANS",		mOFFSET( sFedPageStyle, mpOutroTrans)	},
	{	eFEDPARSE_TYPE_NONE,		0,				0										},
};

sFedParserMember	gFedParseSampleMembers[] =
{
	{	eFEDPARSE_TYPE_pASSET,	"ASSET",		mOFFSET( sFedSample, mpAsset)		},
	{	eFEDPARSE_TYPE_U8,		"PAN",			mOFFSET( sFedSample, mPan)			},
	{	eFEDPARSE_TYPE_U8,		"VOLUME",		mOFFSET( sFedSample, mVolume)		},
	{	eFEDPARSE_TYPE_NONE,	0,			0										},
};

sFedParserMember	gFedParseSliderMembers[] =
{
	{	eFEDPARSE_TYPE_S16,		"X",			mOFFSET( sFedSlider, mBox.mPos.mX)		},
	{	eFEDPARSE_TYPE_S16,		"Y",			mOFFSET( sFedSlider, mBox.mPos.mY)		},
	{	eFEDPARSE_TYPE_S16,		"HEIGHT",		mOFFSET( sFedSlider, mBox.mSize.mHeight)	},
	{	eFEDPARSE_TYPE_S16,		"WIDTH",		mOFFSET( sFedSlider, mBox.mSize.mWidth)	},
	{	eFEDPARSE_TYPE_pVAR,	"VAR",			mOFFSET( sFedSlider, mpVar)				},
	{	eFEDPARSE_TYPE_S32,		"VALUE_MIN",	mOFFSET( sFedSlider, mValueMin)			},
	{	eFEDPARSE_TYPE_S32,		"VALUE_MAX",	mOFFSET( sFedSlider, mValueMax)			},
	{	eFEDPARSE_TYPE_S32,		"VALUE_ADD",	mOFFSET( sFedSlider, mValueAdd)			},
	{	eFEDPARSE_TYPE_NONE,	0,			0											},
};

sFedParserMember	gFedParseSpriteMembers[] =
{
	{	eFEDPARSE_TYPE_S16,		"X",			mOFFSET( sFedSprite, mPos.mX)		},
	{	eFEDPARSE_TYPE_S16,		"Y",			mOFFSET( sFedSprite, mPos.mY)		},
	{	eFEDPARSE_TYPE_pASSET,	"ASSET",		mOFFSET( sFedSprite, mpAsset)		},
	{	eFEDPARSE_TYPE_S32,		"ANIM_SPEED",	mOFFSET( sFedSprite, mAnimSpeed)	},
	{	eFEDPARSE_TYPE_U16,		"FRAME",		mOFFSET( sFedSprite, mFrameBase)	},
	{	eFEDPARSE_TYPE_NONE,	0,			0										},
};

sFedParserMember	gFedParseSpriteGroupMembers[] =
{
	{	eFEDPARSE_TYPE_pSPRITE,	"SPRITE_LOCKED",			mOFFSET( sFedSpriteGroup, mpSpriteLocked)			},
	{	eFEDPARSE_TYPE_pSPRITE,	"SPRITE_LOCKEDSELECTED",	mOFFSET( sFedSpriteGroup, mpSpriteLockedSelected)	},
	{	eFEDPARSE_TYPE_pSPRITE,	"SPRITE_NORMAL",			mOFFSET( sFedSpriteGroup, mpSpriteNormal)			},
	{	eFEDPARSE_TYPE_pSPRITE,	"SPRITE_SELECTED",			mOFFSET( sFedSpriteGroup, mpSpriteSelected)			},
	{	eFEDPARSE_TYPE_NONE,	0,			0							},
};

sFedParserMember	gFedParseSpriteListMembers[] =
{
	{	eFEDPARSE_TYPE_SPRITE_LIST,	"SPRITE",	mOFFSET( sFedSpriteList, mppSprites)	},
	{	eFEDPARSE_TYPE_NONE,	0,			0							},
};

sFedParserMember	gFedParseTextMembers[] =
{
	{	eFEDPARSE_TYPE_S16,			"X",			mOFFSET( sFedText, mBox.mPos.mX )		},
	{	eFEDPARSE_TYPE_S16,			"Y",			mOFFSET( sFedText, mBox.mPos.mY )		},
	{	eFEDPARSE_TYPE_S16,			"WIDTH",		mOFFSET( sFedText, mBox.mSize.mWidth )	},
	{	eFEDPARSE_TYPE_S16,			"HEIGHT",		mOFFSET( sFedText, mBox.mSize.mHeight )	},
	{	eFEDPARSE_TYPE_pFONTGROUP,	"FONT_GROUP",	mOFFSET( sFedText, mpFontGroup )		},
	{	eFEDPARSE_TYPE_STRING,		"STRING",		mOFFSET( sFedText, mpString )			},
	{	eFEDPARSE_TYPE_ALIGN,		"ALIGN",		mOFFSET( sFedText, mAlign )				},
	{	eFEDPARSE_TYPE_NONE,		0,				0										},
};

sFedParserMember	gFedParseTransitionMembers[] =
{
	{	eFEDPARSE_TYPE_U16,		"FADE_COLOUR",		mOFFSET( sFedTransition, mFadeColour)		},
	{	eFEDPARSE_TYPE_FADE,	"FADE_TYPE",		mOFFSET( sFedTransition, mFadeType)			},
	{	eFEDPARSE_TYPE_WIPE,	"WIPE_TYPE",		mOFFSET( sFedTransition, mWipeType)			},
	{	eFEDPARSE_TYPE_U16,		"WIPE_INDEX",		mOFFSET( sFedTransition, mWipeIndex)		},
	{	eFEDPARSE_TYPE_U16,		"FADE_FRAME_COUNT",	mOFFSET( sFedTransition, mFadeFrameCount)	},
	{	eFEDPARSE_TYPE_NONE,	0,					0											},
};

sFedParserMember	gFedParseVarMembers[] =
{
	{	eFEDPARSE_TYPE_STRING,	"NAME",			mOFFSET( sFedVar, mpName )	},
	{	eFEDPARSE_TYPE_NONE,	0,					0						},
};

sFedParserMemberTag	gFedParserMemberTags[] =
{
	{	"ASSET",		gFedParseAssetMembers,			eFEDPARSE_CHUNK_ASSET		},
	{	"CALL",			gFedParseCallMembers,			eFEDPARSE_CHUNK_CALL		},
	{	"CONTROL",		gFedParseControlMembers,		eFEDPARSE_CHUNK_CONTROL		},
	{	"CONTROLLIST",	gFedParseControlListMembers,	eFEDPARSE_CHUNK_CONTROLLIST	},
	{	"FONTGROUP",	gFedParseFontGroupMembers,		eFEDPARSE_CHUNK_FONTGROUP	},
	{	"LIST",			gFedParseListMembers,			eFEDPARSE_CHUNK_LIST		},
	{	"LISTITEM",		gFedParseListItemMembers,		eFEDPARSE_CHUNK_LISTITEM	},
	{	"LOCK",			gFedParseLockMembers,			eFEDPARSE_CHUNK_LOCK		},
	{	"PAGE",			gFedParsePageMembers,			eFEDPARSE_CHUNK_PAGE		},
	{	"PAGESTYLE",	gFedParsePageStyleMembers,		eFEDPARSE_CHUNK_PAGESTYLE	},
	{	"SAMPLE",		gFedParseSampleMembers,			eFEDPARSE_CHUNK_SAMPLE		},
	{	"SLIDER",		gFedParseSliderMembers,			eFEDPARSE_CHUNK_SLIDER		},
	{	"SPRITE",		gFedParseSpriteMembers,			eFEDPARSE_CHUNK_SPRITE		},
	{	"SPRITEGROUP",	gFedParseSpriteGroupMembers,	eFEDPARSE_CHUNK_SPRITEGROUP	},
	{	"SPRITELIST",	gFedParseSpriteListMembers,		eFEDPARSE_CHUNK_SPRITELIST	},
	{	"TEXT",			gFedParseTextMembers,			eFEDPARSE_CHUNK_TEXT		},
	{	"TRANSITION",	gFedParseTransitionMembers,		eFEDPARSE_CHUNK_TRANSITION	},
	{	"VAR",			gFedParseVarMembers	,			eFEDPARSE_CHUNK_VAR			},
	{	0,	0, 0	},
};


sTokeniserHandler	gFedParserPass1Handler =
{
	FedParse_Pass1_Init,
	FedParse_Pass1_DeInit,
	FedParse_Pass1_Var,
	0,
	"*"
};

sTokeniserHandler	gFedParserPass2Handler =
{
	FedParse_Pass2_Init,
	0,
	0,
	0,
	"*"
};

sTokeniserHandler	gFedParserPass3Handler =
{
	FedParse_Pass3_Init,
	FedParse_Pass3_DeInit,
	FedParse_Pass3_Var,
	0,
	"*"
};

sFedParser	gFedParser;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void *	FedParse_StringTempAlloc( const char * apSrcString );
void	FedParse_StringTempFree( void * apMem );

void	FedParse_ShowItem( U8 * apItem, sFedParserMember * apMembers );

void	FedParse_HashListsInit( void );
void	FedParse_HeaderInit(void);

U32		FedParse_GetListIndex( const U16 aListIndex, const char * apName );
void	FedParse_SetItemHash( void );
void	FedParse_SetItemArrayCount( void );
void	FedParse_SetItemArrayPtrs( void );
void	FedParse_ArrayItemAdd( const U32 aValue );
char *	FedParse_StringAdd( const char * apString );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Init( void )
* ACTION   : FedParse_Init
* CREATION : 31.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Init( void )
{
	U16	i;
	sFedParserMemberTag *	lpTags;

	lpTags = gFedParserMemberTags;

	for( i=0; i<eFEDPARSE_CHUNK_LIMIT; i++ )
	{
		lpTags = gFedParserMemberTags;
		while( (lpTags->mpName) && (lpTags->mIndex != i) )
		{
			lpTags++;
		}
		if( (lpTags->mpName) && (i == lpTags->mIndex) )
		{
			gFedParser.mLists[ i ].mpMembers    = lpTags->mpMember;
		}
		gFedParser.mLists[ i ].mpChunkTitle = sTagString_GetpString( i, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT );
		gFedParser.mLists[ i ].mItemCount   = 0;
		gFedParser.mLists[ i ].mItemIndex   = 0;
		gFedParser.mLists[ i ].mItemSize    = sTagValue_GetValue( i, gFedParseStructSizes, eFEDPARSE_CHUNK_LIMIT );
		gFedParser.mLists[ i ].mpHashes     = 0;
		gFedParser.mLists[ i ].mpData       = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_DeInit( void )
* ACTION   : FedParse_DeInit
* CREATION : 06.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_DeInit( void )
{
	U16	i;

	for( i=0; i<eFEDPARSE_CHUNK_LIMIT; i++ )
	{
		if( gFedParser.mLists[ i ].mpHashes )
		{
			mMEMFREE( gFedParser.mLists[ i ].mpHashes );
			gFedParser.mLists[ i ].mpHashes = 0;
		}
	}
	if( gFedParser.mpHeader )
	{
		mMEMFREE( gFedParser.mpHeader );
		gFedParser.mpHeader = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Text( const char * apText,const U32 aSize )
* ACTION   : FedParse_Text
* CREATION : 31.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Text( const char * apText,const U32 aSize )
{
	gFedParser.mChunkIndex       = 0;
	gFedParser.mDataSize         = 0;
	gFedParser.mStringGlobalSize = 0;
	gFedParser.mStringTempSize   = 0;

	gFedParser.mStringSize = 0;
	gFedParser.mArraySize = 0;
	gFedParser.mpHeader = 0;

	Tokeniser_Parse( apText, aSize, &gFedParserPass1Handler, 1 );

	FedParse_HashListsInit();
	FedParse_HeaderInit();

	Tokeniser_Parse( apText, aSize, &gFedParserPass2Handler, 1 );
	Tokeniser_Parse( apText, aSize, &gFedParserPass3Handler, 1 );
/*
	FedParse_ShowIncomplete();
	FedParse_ShowInfo();*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass1_Init( const char * apTitle,const U16 aIndex,const char * apName )
* ACTION   : FedParse_Pass1_Init
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass1_Init( const char * apTitle,const U16 aIndex,const char * apName )
{
	(void)aIndex;
	(void)apName;


	if( sTagString_StringExists( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT ) )
	{
		gFedParser.mChunkIndex = (U16)sTagString_GetID( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT );

		gFedParser.mLists[ gFedParser.mChunkIndex ].mItemCount++;
		gFedParser.mpMembers = gFedParser.mLists[ gFedParser.mChunkIndex ].mpMembers;
	}
	else
	{
		gFedParser.mpMembers = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass1_DeInit( void )
* ACTION   : FedParse_Pass1_DeInit
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass1_DeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass1_Var( sTokeniserArgs * apArgs )
* ACTION   : FedParse_Pass1_Var
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass1_Var( sTokeniserArgs * apArgs )
{
	sFedParserMember *	lpMember;

	lpMember = gFedParser.mpMembers;
	if( lpMember )
	{
		while( eFEDPARSE_TYPE_NONE != lpMember->mType )
		{
			if( !String_StrCmpi( lpMember->mpString, apArgs->mpStrings[ 0 ] ) )
			{
				switch( lpMember->mType )
				{
				case	eFEDPARSE_TYPE_STRING:
					gFedParser.mStringSize += (strlen(apArgs->mpStrings[2])+1);
					break;
				case	eFEDPARSE_TYPE_CONTROL_LIST:
				case	eFEDPARSE_TYPE_ITEM_LIST:
				case	eFEDPARSE_TYPE_SPRITE_LIST:
					gFedParser.mArraySize++;
					break;
				}
			}
			lpMember++;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass2_Init( const char * apTitle,const U16 aIndex,const char * apName )
* ACTION   : FedParse_Pass2_Init
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass2_Init( const char * apTitle,const U16 aIndex,const char * apName )
{
	sFedParserList *	lpList;


	(void)aIndex;
	if( sTagString_StringExists( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT ) )
	{
		gFedParser.mChunkIndex = (U16)sTagString_GetID( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT );

		lpList = &gFedParser.mLists[ gFedParser.mChunkIndex ];
		if( lpList->mItemIndex < lpList->mItemCount )
		{
			lpList->mpHashes[ lpList->mItemIndex ] = HashList_BuildHash( apName );
			lpList->mItemIndex++;
		}
	}
	else
	{
		gFedParser.mpMembers = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass3_Init( const char * apTitle,const U16 aIndex,const char * apName )
* ACTION   : FedParse_Pass3_Init
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass3_Init( const char * apTitle,const U16 aIndex,const char * apName )
{
	sFedParserList *	lpList;

	(void)aIndex;
	(void)apName;

	gFedParser.mpCurrentChunkName  = (char*)FedParse_StringTempAlloc( apName );
	gFedParser.mpCurrentChunkTitle = (char*)FedParse_StringTempAlloc( apTitle );

	if( sTagString_StringExists( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT ) )
	{
		gFedParser.mChunkIndex = (U16)sTagString_GetID( apTitle, gFedParseChunkNames, eFEDPARSE_CHUNK_LIMIT );

		lpList = &gFedParser.mLists[ gFedParser.mChunkIndex ];

		gFedParser.mpMembers = lpList->mpMembers;
		gFedParser.mpData    = lpList->mpData;
		FedParse_SetItemArrayPtrs();
	}
	else
	{
		printf( "WARNING: Unknown chunk type %s::%s\n", apTitle, apName );
		gFedParser.mpMembers = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass3_DeInit( void )
* ACTION   : FedParse_Pass3_DeInit
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass3_DeInit( void )
{
	sFedParserList *	lpList;

	FedParse_SetItemArrayCount();
	FedParse_SetItemHash();

	lpList = &gFedParser.mLists[ gFedParser.mChunkIndex ];
	lpList->mpData += lpList->mItemSize;

	FedParse_StringTempFree( gFedParser.mpCurrentChunkName );
	FedParse_StringTempFree( gFedParser.mpCurrentChunkTitle );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_Pass3_Var( sTokeniserArgs * apArgs )
* ACTION   : FedParse_Pass3_Var
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_Pass3_Var( sTokeniserArgs * apArgs )
{
	sFedParserMember *		lpMember;
	S8 *					lpS8;
	U8 *					lpU8;
	U16 *					lpU16;
	S16 *					lpS16;
	S32 *					lpS32;
	U32 *					lpU32;
	FP32 *					lpFP32;
	char **					lppString;

	if( (gFedParser.mpData) && (gFedParser.mpMembers) )
	{
		lpMember = gFedParser.mpMembers;
		while( eFEDPARSE_TYPE_NONE != lpMember->mType )
		{
			if( !String_StrCmpi( lpMember->mpString, apArgs->mpStrings[ 0 ] ) )
			{
				switch( lpMember->mType )
				{
				case	eFEDPARSE_TYPE_S8:
					lpS8  = (S8*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpS8 = (S8)atoi( apArgs->mpStrings[ 2 ] );
					break;
				case	eFEDPARSE_TYPE_U8:
					lpU8  = (U8*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU8 = (U8)atoi( apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_S16:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)atoi( apArgs->mpStrings[ 2 ] );
					break;
				case	eFEDPARSE_TYPE_U16:
					lpS16  = (S16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpS16 = (S16)atoi( apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_S32:
					lpS32  = (S32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpS32 = (S32)atoi( apArgs->mpStrings[ 2 ] );
					break;
				case	eFEDPARSE_TYPE_U32:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = (U32)atoi( apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_FP32:
					lpFP32  = (FP32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpFP32 = (FP32)atof( apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_STRING:
					lppString  = (char**)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lppString = FedParse_StringAdd( apArgs->mpStrings[ 2 ] );
					break;


				case	eFEDPARSE_TYPE_ALIGN:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)sTagString_GetID( apArgs->mpStrings[2], gFedParseAlignNames, eFED_ALIGN_LIMIT );
					break;

				case	eFEDPARSE_TYPE_COMPARE:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)sTagString_GetID( apArgs->mpStrings[2], gFedParseCompareNames, eFED_COMPARE_LIMIT );
					break;

				case	eFEDPARSE_TYPE_CONTROL:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)sTagString_GetID( apArgs->mpStrings[2], gFedParseControlTypeNames, eFED_CONTROL_LIMIT );
					break;

				case	eFEDPARSE_TYPE_FADE:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)sTagString_GetID( apArgs->mpStrings[2], gFedParseFadeNames, eFED_FADE_LIMIT );
					break;

				case	eFEDPARSE_TYPE_WIPE:
					lpU16  = (U16*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU16 = (U16)sTagString_GetID( apArgs->mpStrings[2], gFedParseWipeNames, eFED_WIPE_LIMIT );
					break;


				case	eFEDPARSE_TYPE_CONTROL_LIST:
					FedParse_ArrayItemAdd( FedParse_GetListIndex( eFEDPARSE_CHUNK_CONTROL, apArgs->mpStrings[ 2 ] ) );
					break;

				case	eFEDPARSE_TYPE_ITEM_LIST:
					FedParse_ArrayItemAdd( FedParse_GetListIndex( eFEDPARSE_CHUNK_LISTITEM, apArgs->mpStrings[ 2 ] ) );
					break;

				case	eFEDPARSE_TYPE_SPRITE_LIST:
					FedParse_ArrayItemAdd( FedParse_GetListIndex( eFEDPARSE_CHUNK_SPRITE, apArgs->mpStrings[ 2 ] ) );
					break;


				case	eFEDPARSE_TYPE_pASSET:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_ASSET, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pCALL:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_CALL, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pCONTROLLIST:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_CONTROLLIST, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pFONTGROUP:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_FONTGROUP, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pLIST:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_LIST, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pLOCK:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_LOCK, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pPAGE:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_PAGE, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pPAGESTYLE:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_PAGESTYLE, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pSAMPLE:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_SAMPLE, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pSLIDER:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_SLIDER, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pSPRITE:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_SPRITE, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pSPRITEGROUP:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_SPRITEGROUP, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pSPRITELIST:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_SPRITELIST, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pTEXT:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_TEXT, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pTRANS:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_TRANSITION, apArgs->mpStrings[ 2 ] );
					break;

				case	eFEDPARSE_TYPE_pVAR:
					lpU32  = (U32*)(&gFedParser.mpData[ lpMember->mOffset ]);
					*lpU32 = FedParse_GetListIndex( eFEDPARSE_CHUNK_VAR, apArgs->mpStrings[ 2 ] );
					break;

				default:
					printf( "WARNING: unhandled member type %d\n", lpMember->mType );
					break;
				}
				return;
			}
			lpMember++;
		}
		printf( "FedParse : Unknown Var %s in Chunk %s::%s\n", apArgs->mpStrings[0], gFedParser.mpCurrentChunkTitle, gFedParser.mpCurrentChunkName );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_StringAdd( const char * apString )
* ACTION   : FedParse_StringAdd
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

char *	FedParse_StringAdd( const char * apString )
{
	char *	lpString;

	lpString  = &gFedParser.mpStringBase[ gFedParser.mStringOffset ];
	lpString -= (U32)(gFedParser.mpHeader);
	if( apString )
	{
		strcpy( &gFedParser.mpStringBase[ gFedParser.mStringOffset ], apString );
		gFedParser.mStringOffset += (strlen(apString)) +1 ;
	}

	return( lpString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_GetListIndex( const U16 aListIndex,const char * apName )
* ACTION   : FedParse_GetListIndex
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	FedParse_GetListIndex( const U16 aListIndex,const char * apName )
{
	sFedParserList *	lpList;
	U32					lHash;
	U16					i;

	lpList = &gFedParser.mLists[ aListIndex ];
	lHash  = HashList_BuildHash( apName );

	for( i=0; i<lpList->mItemCount; i++ )
	{
		if( lpList->mpHashes[ i ] == lHash )
		{
			return( i+1 );
		}
	}

	printf( "WARNING : Unfound %s::%s in %s::%s\n", lpList->mpChunkTitle, apName, gFedParser.mpCurrentChunkTitle, gFedParser.mpCurrentChunkName );

	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_SetItemArrayPtrs( void )
* ACTION   : FedParse_SetItemArrayPtrs
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_SetItemArrayPtrs( void )
{
	U32	lBase;

	lBase  = (U32)&gFedParser.mpArrayBase[ gFedParser.mArrayOffset ];
	lBase -= (U32)gFedParser.mpHeader;

	switch( gFedParser.mChunkIndex )
	{
	case	eFEDPARSE_CHUNK_LIST:
		((sFedList*)gFedParser.mpData)->mppItems = (sFedListItem**)lBase;
		break;
	case	eFEDPARSE_CHUNK_CONTROLLIST:
		((sFedControlList*)gFedParser.mpData)->mppControls = (sFedControl**)lBase;
		break;
	case	eFEDPARSE_CHUNK_SPRITELIST:
		((sFedSpriteList*)gFedParser.mpData)->mppSprites = (sFedSprite**)lBase;
		break;
	}

	gFedParser.mItemArrayCount = 0;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_SetItemArrayCount( void )
* ACTION   : FedParse_SetItemArrayCount
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_SetItemArrayCount( void )
{
	switch( gFedParser.mChunkIndex )
	{
	case	eFEDPARSE_CHUNK_LIST:
		((sFedList*)gFedParser.mpData)->mItemCount = gFedParser.mItemArrayCount;
		break;
	case	eFEDPARSE_CHUNK_CONTROLLIST:
		((sFedControlList*)gFedParser.mpData)->mControlCount = gFedParser.mItemArrayCount;
		break;
	case	eFEDPARSE_CHUNK_SPRITELIST:
		((sFedSpriteList*)gFedParser.mpData)->mSpriteCount = gFedParser.mItemArrayCount;
		break;
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_ArrayItemAdd( const U32 aValue )
* ACTION   : FedParse_ArrayItemAdd
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_ArrayItemAdd( const U32 aValue )
{
	if( gFedParser.mArrayOffset < gFedParser.mArraySize )
	{
		gFedParser.mpArrayBase[ gFedParser.mArrayOffset ] = aValue;
		gFedParser.mArrayOffset++;
		gFedParser.mItemArrayCount++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_SetItemHash( void )
* ACTION   : FedParse_SetItemHash
* CREATION : 10.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_SetItemHash( void )
{
	switch( gFedParser.mChunkIndex )
	{
	case	eFEDPARSE_CHUNK_PAGE:
		((sFedPage*)gFedParser.mpData)->mHash = HashList_BuildHash( gFedParser.mpCurrentChunkName );
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_HashListsInit( void )
* ACTION   : FedParse_HashListsInit
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_HashListsInit( void )
{
	U16	i;

	for( i=0; i<eFEDPARSE_CHUNK_LIMIT; i++ )
	{
		if( gFedParser.mLists[ i ].mItemCount )
		{
			gFedParser.mLists[ i ].mpHashes = (U32*)mMEMCALLOC( gFedParser.mLists[ i ].mItemCount * 4 );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_HeaderInit( void )
* ACTION   : FedParse_HeaderInit
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

#define	mFEDPARSE_DELOC( _a )	{ (*(U32*)(&gFedParser.mpHeader->_a) -= (U32)gFedParser.mpHeader); Endian_FromBigU32( &gFedParser.mpHeader->_a ); }

void	FedParse_HeaderInit( void )
{
	U16	i;
	U32	lSize;
	U32	lItemsSize;
	U32	lOff;
	U8 *	lpMem;

	lSize  = sizeof( sFedHeader);
	lSize += (gFedParser.mArraySize * 4);
	lSize += gFedParser.mStringSize;

	lItemsSize = 0;
	for( i=0; i<eFEDPARSE_CHUNK_LIMIT; i++ )
	{
		lItemsSize += gFedParser.mLists[ i ].mItemSize * gFedParser.mLists[ i ].mItemCount;
	}

	lSize += lItemsSize;

	gFedParser.mDataSize = lSize;

	lpMem = (U8*)mMEMCALLOC( lSize );
	gFedParser.mpHeader = (sFedHeader*)lpMem;

	lOff = sizeof( sFedHeader );
	gFedParser.mpArrayBase = (U32*)&lpMem[ lOff ];
	lOff += (gFedParser.mArraySize *4);

	for( i=0; i<eFEDPARSE_CHUNK_LIMIT; i++ )
	{
		gFedParser.mLists[ i ].mpData = &lpMem[ lOff ];
		lOff += gFedParser.mLists[ i ].mItemSize * gFedParser.mLists[ i ].mItemCount;
	}


	gFedParser.mpStringBase = (char*)&lpMem[ lOff ];

	gFedParser.mArrayOffset = 0;
	gFedParser.mStringOffset = 0;


	gFedParser.mpHeader->mID      = dFED_ID;
	gFedParser.mpHeader->mVersion = dFED_VERSION_OLD;

	gFedParser.mpHeader->mAssetCount       = gFedParser.mLists[ eFEDPARSE_CHUNK_ASSET ].mItemCount;
	gFedParser.mpHeader->mCallCount        = gFedParser.mLists[ eFEDPARSE_CHUNK_CALL ].mItemCount;
	gFedParser.mpHeader->mControlCount     = gFedParser.mLists[ eFEDPARSE_CHUNK_CONTROL ].mItemCount;
	gFedParser.mpHeader->mControlListCount = gFedParser.mLists[ eFEDPARSE_CHUNK_CONTROLLIST ].mItemCount;
	gFedParser.mpHeader->mFontGroupCount   = gFedParser.mLists[ eFEDPARSE_CHUNK_FONTGROUP ].mItemCount;
	gFedParser.mpHeader->mListCount        = gFedParser.mLists[ eFEDPARSE_CHUNK_LIST ].mItemCount;
	gFedParser.mpHeader->mListItemCount    = gFedParser.mLists[ eFEDPARSE_CHUNK_LISTITEM ].mItemCount;
	gFedParser.mpHeader->mLockCount        = gFedParser.mLists[ eFEDPARSE_CHUNK_LOCK ].mItemCount;
	gFedParser.mpHeader->mPageCount        = gFedParser.mLists[ eFEDPARSE_CHUNK_PAGE ].mItemCount;
	gFedParser.mpHeader->mPageStyleCount   = gFedParser.mLists[ eFEDPARSE_CHUNK_PAGESTYLE ].mItemCount;
	gFedParser.mpHeader->mSampleCount      = gFedParser.mLists[ eFEDPARSE_CHUNK_SAMPLE ].mItemCount;
	gFedParser.mpHeader->mSliderCount      = gFedParser.mLists[ eFEDPARSE_CHUNK_SLIDER ].mItemCount;
	gFedParser.mpHeader->mSpriteCount      = gFedParser.mLists[ eFEDPARSE_CHUNK_SPRITE ].mItemCount;
	gFedParser.mpHeader->mSpriteGroupCount = gFedParser.mLists[ eFEDPARSE_CHUNK_SPRITEGROUP ].mItemCount;
	gFedParser.mpHeader->mSpriteListCount  = gFedParser.mLists[ eFEDPARSE_CHUNK_SPRITELIST ].mItemCount;
	gFedParser.mpHeader->mTextCount        = gFedParser.mLists[ eFEDPARSE_CHUNK_TEXT ].mItemCount;
	gFedParser.mpHeader->mTransitionCount  = gFedParser.mLists[ eFEDPARSE_CHUNK_TRANSITION ].mItemCount;
	gFedParser.mpHeader->mVarCount         = gFedParser.mLists[ eFEDPARSE_CHUNK_VAR ].mItemCount;

	gFedParser.mpHeader->mpAssets       = (sFedAsset*)gFedParser.mLists[       eFEDPARSE_CHUNK_ASSET ].mpData;
	gFedParser.mpHeader->mpCalls        = (sFedCall*)gFedParser.mLists[        eFEDPARSE_CHUNK_CALL ].mpData;
	gFedParser.mpHeader->mpControls     = (sFedControl*)gFedParser.mLists[     eFEDPARSE_CHUNK_CONTROL ].mpData;
	gFedParser.mpHeader->mpControlLists = (sFedControlList*)gFedParser.mLists[ eFEDPARSE_CHUNK_CONTROLLIST ].mpData;
	gFedParser.mpHeader->mpFontGroups   = (sFedFontGroup*)gFedParser.mLists[   eFEDPARSE_CHUNK_FONTGROUP ].mpData;
	gFedParser.mpHeader->mpLists        = (sFedList*)gFedParser.mLists[        eFEDPARSE_CHUNK_LIST ].mpData;
	gFedParser.mpHeader->mpListItems    = (sFedListItem*)gFedParser.mLists[    eFEDPARSE_CHUNK_LISTITEM ].mpData;
	gFedParser.mpHeader->mpLocks        = (sFedLock*)gFedParser.mLists[        eFEDPARSE_CHUNK_LOCK ].mpData;
	gFedParser.mpHeader->mpPages        = (sFedPage*)gFedParser.mLists[        eFEDPARSE_CHUNK_PAGE ].mpData;
	gFedParser.mpHeader->mpPageStyles   = (sFedPageStyle*)gFedParser.mLists[   eFEDPARSE_CHUNK_PAGESTYLE ].mpData;
	gFedParser.mpHeader->mpSamples      = (sFedSample*)gFedParser.mLists[      eFEDPARSE_CHUNK_SAMPLE ].mpData;
	gFedParser.mpHeader->mpSliders      = (sFedSlider*)gFedParser.mLists[      eFEDPARSE_CHUNK_SLIDER ].mpData;
	gFedParser.mpHeader->mpSprites      = (sFedSprite*)gFedParser.mLists[      eFEDPARSE_CHUNK_SPRITE ].mpData;
	gFedParser.mpHeader->mpSpriteGroups = (sFedSpriteGroup*)gFedParser.mLists[ eFEDPARSE_CHUNK_SPRITEGROUP ].mpData;
	gFedParser.mpHeader->mpSpriteLists  = (sFedSpriteList*)gFedParser.mLists[  eFEDPARSE_CHUNK_SPRITELIST ].mpData;
	gFedParser.mpHeader->mpTexts        = (sFedText*)gFedParser.mLists[        eFEDPARSE_CHUNK_TEXT ].mpData;
	gFedParser.mpHeader->mpTransitions  = (sFedTransition*)gFedParser.mLists[  eFEDPARSE_CHUNK_TRANSITION ].mpData;
	gFedParser.mpHeader->mpVars         = (sFedVar*)gFedParser.mLists[         eFEDPARSE_CHUNK_VAR ].mpData;

/*
	for( i=0; i<gFedParser.mpHeader->mControlListCount; i++ )
	{
		mFEDPARSE_DELOC( mpControlLists[ i ].mppControls );
	}

	for( i=0; i<gFedParser.mpHeader->mListCount; i++ )
	{
		mFEDPARSE_DELOC( mpLists[ i ].mppItems );
	}

	for( i=0; i<gFedParser.mpHeader->mSpriteListCount; i++ )
	{
		mFEDPARSE_DELOC( mpSpriteLists[ i ].mppSprites );
	}
*/

/*	for( i=0; i<gFedParser.mpHeader->mControlListCount; i++ )
	{
		Endian_FromBigU16( &gFedParser.mpHeader->mpControlLists[ i ].mControlCount );
	}*/

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_ShowFedInfo( void )
* ACTION   : FedParse_ShowFedInfo
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_ShowFedInfo( void )
{
	sFedHeader *	lpHeader;
	U16				i,j;

	lpHeader = gFedParser.mpHeader;

	printf( "ID : %s\n", (char*)&lpHeader->mID );
	printf( "Version : %ld", lpHeader->mVersion );

	printf( "AssetCount; %d\n",      lpHeader->mAssetCount);
	printf( "CallCount; %d\n",       lpHeader->mCallCount);
	printf( "ControlCount; %d\n",    lpHeader->mControlCount);
	printf( "ControlListCount; %d\n",lpHeader->mControlListCount);
	printf( "FontGroupCount; %d\n",  lpHeader->mFontGroupCount);
	printf( "ListCount; %d\n",       lpHeader->mListCount);
	printf( "ListItemCount; %d\n",   lpHeader->mListItemCount);
	printf( "LockCount; %d\n",       lpHeader->mLockCount);
	printf( "PageCount; %d\n",       lpHeader->mPageCount);
	printf( "PageStyleCount; %d\n",  lpHeader->mPageStyleCount);
	printf( "SampleCount; %d\n",     lpHeader->mSampleCount);
	printf( "SliderCount; %d\n",     lpHeader->mSliderCount);
	printf( "SpriteCount; %d\n",     lpHeader->mSpriteCount);
	printf( "SpriteListCount; %d\n", lpHeader->mSpriteListCount);
	printf( "TextCount; %d\n",       lpHeader->mTextCount);
	printf( "TransitionCount; %d\n", lpHeader->mTransitionCount);
	printf( "VarCount; %d\n",        lpHeader->mVarCount);

	printf( "ASSETS:\n" );
	for( i=0; i<lpHeader->mAssetCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpAssets[ i ] );
		FedParse_ShowItem( (U8*)&lpHeader->mpAssets[ i ], gFedParseAssetMembers );
	}

	printf( "\nCALLS:\n" );
	for( i=0; i<lpHeader->mCallCount; i++ )
	{
		FedParse_ShowItem( (U8*)&lpHeader->mpCalls[ i ], gFedParseCallMembers );
	}

	printf( "\nCONTROLS:\n" );
	for( i=0; i<lpHeader->mControlCount; i++ )
	{
		FedParse_ShowItem( (U8*)&lpHeader->mpControls[ i ], gFedParseControlMembers );
	}

	printf( "\nCONTROLLISTS:\n" );
	for( i=0; i<lpHeader->mControlListCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpControlLists[ i ] );
		printf( "Control Count %d\n",  lpHeader->mpControlLists[ i ].mControlCount );
		for( j=0; j<lpHeader->mpControlLists[ i ].mControlCount; j++ )
		{
			printf( "%lx\n", (U32)lpHeader->mpControlLists[ i ].mppControls[ j ] );
		}
/*		FedParse_ShowItem( (U8*)&lpHeader->mpControlLists[ i ], gFedParseControlListMembers );*/
	}

	printf( "\nFONTGROUPS:\n" );
	for( i=0; i<lpHeader->mFontGroupCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpFontGroups[ i ] );
		FedParse_ShowItem( (U8*)&lpHeader->mpFontGroups[ i ], gFedParseFontGroupMembers );
	}
	printf( "\nPAGES:\n" );
	for( i=0; i<lpHeader->mPageCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpPages[ i ] );
		FedParse_ShowItem( (U8*)&lpHeader->mpPages[ i ], gFedParsePageMembers );
	}

	printf( "\nPAGESTYLE:\n" );
	for( i=0; i<lpHeader->mPageStyleCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpPageStyles[ i ] );
		FedParse_ShowItem( (U8*)&lpHeader->mpPageStyles[ i ], gFedParsePageStyleMembers );
	}

	printf( "\nTEXTS:\n" );
	for( i=0; i<lpHeader->mTextCount; i++ )
	{
		printf( "\n%d: %lx\n", i, (U32)&lpHeader->mpTexts[ i ] );
		FedParse_ShowItem( (U8*)&lpHeader->mpTexts[ i ], gFedParseTextMembers );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_ShowItem( U8 * apItem,sFedParserMember * apMembers )
* ACTION   : FedParse_ShowItem
* CREATION : 08.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_ShowItem( U8 * apItem,sFedParserMember * apMember )
{
	S32						lS32;
	U32						lU32;
	FP32					lFP32;
	char *					lpString;

	while( apMember->mpString )
	{
		printf( "%s : ", apMember->mpString );
		switch( apMember->mType )
		{

		case	eFEDPARSE_TYPE_U8:
			lU32 = *(U8*)&apItem[ apMember->mOffset ];
			printf( "%ld", lU32 );
			break;
		case	eFEDPARSE_TYPE_U16:
			lU32 = *(U16*)&apItem[ apMember->mOffset ];
			printf( "%ld", lU32 );
			break;
		case	eFEDPARSE_TYPE_U32:
			lU32 = *(U32*)&apItem[ apMember->mOffset ];
			printf( "%ld", lU32 );
			break;

		case	eFEDPARSE_TYPE_S8:
			lS32 = *(S8*)&apItem[ apMember->mOffset ];
			printf( "%ld", lS32 );
			break;
		case	eFEDPARSE_TYPE_S16:
			lS32 = *(S16*)&apItem[ apMember->mOffset ];
			printf( "%ld", lS32 );
			break;
		case	eFEDPARSE_TYPE_S32:
			lS32 = *(S32*)&apItem[ apMember->mOffset ];
			printf( "%ld", lS32 );
			break;

		case	eFEDPARSE_TYPE_FP32:
			lFP32 = *(FP32*)&apItem[ apMember->mOffset ];
			printf( "%f", lFP32 );
			break;

		case	eFEDPARSE_TYPE_STRING:
			lU32 = *(U32*)&apItem[ apMember->mOffset ];
			lpString = (char*)lU32;
			printf( "%s", lpString );
			break;

		case	eFEDPARSE_TYPE_CONTROL_LIST:
		case	eFEDPARSE_TYPE_ITEM_LIST:
		case	eFEDPARSE_TYPE_SPRITE_LIST:
			lU32 = *(U32*)&apItem[ apMember->mOffset ];
			printf( "%lx", lU32 );
			break;

		case	eFEDPARSE_TYPE_ALIGN:
		case	eFEDPARSE_TYPE_CONTROL:
		case	eFEDPARSE_TYPE_FADE:
		case	eFEDPARSE_TYPE_WIPE:
			lU32 = *(U16*)&apItem[ apMember->mOffset ];
			printf( "%ld", lU32 );
			break;

		case	eFEDPARSE_TYPE_pASSET:
		case	eFEDPARSE_TYPE_pCALL:
		case	eFEDPARSE_TYPE_pCONTROLLIST:
		case	eFEDPARSE_TYPE_pFONTGROUP:
		case	eFEDPARSE_TYPE_pLOCK:
		case	eFEDPARSE_TYPE_pPAGE:
		case	eFEDPARSE_TYPE_pPAGESTYLE:
		case	eFEDPARSE_TYPE_pSAMPLE:
		case	eFEDPARSE_TYPE_pSLIDER:
		case	eFEDPARSE_TYPE_pSPRITE:
		case	eFEDPARSE_TYPE_pSPRITEGROUP:
		case	eFEDPARSE_TYPE_pSPRITELIST:
		case	eFEDPARSE_TYPE_pTEXT:
		case	eFEDPARSE_TYPE_pTRANS:
		case	eFEDPARSE_TYPE_pVAR:
			lU32 = *(U32*)&apItem[ apMember->mOffset ];
			printf( "%lx", lU32 );
			break;
		}
		printf( "\n" );
		apMember++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_GetpFed( void )
* ACTION   : FedParse_GetpFed
* CREATION : 07.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sFedHeader *	FedParse_GetpFed( void )
{
	return( gFedParser.mpHeader );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_GetSize( void )
* ACTION   : FedParse_GetSize
* CREATION : 07.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	FedParse_GetSize( void )
{
	return( gFedParser.mDataSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_StringTempAlloc( const char * apSrcString )
* ACTION   : FedParse_StringTempAlloc
* CREATION : 03.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void *	FedParse_StringTempAlloc( const char * apSrcString )
{
	S32		lLen;
	char *	lpString;

	if( apSrcString )
	{
		lLen  = strlen( apSrcString );
		lLen += 1;
		lpString  = (char*)mMEMCALLOC( lLen );
		if( lpString )
		{
			strcpy( lpString, apSrcString );
		}
		gFedParser.mStringTempSize += lLen;
	}
	else
	{
		lpString = 0;
	}

	return( lpString );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : FedParse_StringTempFree( void * apMem )
* ACTION   : FedParse_StringTempFree
* CREATION : 03.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	FedParse_StringTempFree( void * apMem )
{
	mMEMFREE( apMem );
}


/* ################################################################################ */

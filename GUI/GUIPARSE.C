/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GUIPARSE.H"

#include	"GUI.H"
#include	"GUIEDIT.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/HASHLIST/HASHLIST.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/TOKENISE/TOKENISE.H>

#include	<STRING.H>


/* ###################################################################################
#  ENUMS
################################################################################### */


enum
{
	eGUIPARSE_CHUNK_ACTION,
	eGUIPARSE_CHUNK_ASSET,
	eGUIPARSE_CHUNK_BUTTON,
	eGUIPARSE_CHUNK_BUTTONSTYLE,
	eGUIPARSE_CHUNK_COLOUR,
	eGUIPARSE_CHUNK_CURSOR,
	eGUIPARSE_CHUNK_FILL,
	eGUIPARSE_CHUNK_FONTGROUP,
	eGUIPARSE_CHUNK_KEYACTION,
	eGUIPARSE_CHUNK_LIST,
	eGUIPARSE_CHUNK_LOCK,
	eGUIPARSE_CHUNK_SLIDER,
	eGUIPARSE_CHUNK_VALUE,
	eGUIPARSE_CHUNK_VAR,
	eGUIPARSE_CHUNK_WINDOW,

	eGUIPARSE_CHUNK_LIMIT
};

enum
{
	eGUIPARSE_TAGVALUE_ACTION,
	eGUIPARSE_TAGVALUE_ALIGNH,
	eGUIPARSE_TAGVALUE_ALIGNV,
	eGUIPARSE_TAGVALUE_BUTTON,
	eGUIPARSE_TAGVALUE_FILLPATTERN,
	eGUIPARSE_TAGVALUE_FILLTYPE,
	eGUIPARSE_TAGVALUE_KEYSPECIAL,
	eGUIPARSE_TAGVALUE_KEYSTATUS,
	eGUIPARSE_TAGVALUE_SCANCODE,
	eGUIPARSE_TAGVALUE_SLIDER,
	eGUIPARSE_TAGVALUE_VARTYPE,

	eGUIPARSE_TAGVALUE_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */



/* ###################################################################################
#  DATA
################################################################################### */

sTokeniserTagString	gGuiParseChunkTags[] =
{
	{	eGUIPARSE_CHUNK_ACTION,			"ACTION"		},
	{	eGUIPARSE_CHUNK_ASSET,			"ASSET"			},
	{	eGUIPARSE_CHUNK_BUTTON,			"BUTTON"		},
	{	eGUIPARSE_CHUNK_BUTTONSTYLE,	"BUTTONSTYLE"	},
	{	eGUIPARSE_CHUNK_COLOUR,			"COLOUR"		},
	{	eGUIPARSE_CHUNK_CURSOR,			"CURSOR"		},
	{	eGUIPARSE_CHUNK_FILL,			"FILL"			},
	{	eGUIPARSE_CHUNK_FONTGROUP,		"FONTGROUP"		},
	{	eGUIPARSE_CHUNK_KEYACTION,		"KEYACTION"		},
	{	eGUIPARSE_CHUNK_LIST,			"LIST"			},
	{	eGUIPARSE_CHUNK_LOCK,			"LOCK"			},
	{	eGUIPARSE_CHUNK_SLIDER,			"SLIDER"		},
	{	eGUIPARSE_CHUNK_VALUE,			"VALUE"			},
	{	eGUIPARSE_CHUNK_VAR,			"VAR"			},
	{	eGUIPARSE_CHUNK_WINDOW,			"WINDOW"		},
	{	0,	0	}
};


sTokeniserTagString	gGuiParseActionTags[] =
{
	{	eGUI_ACTION_NONE,			"NONE"			},
	{	eGUI_ACTION_APP_EXIT,		"APP_EXIT"		},
	{	eGUI_ACTION_CALL,			"CALL"			},
	{	eGUI_ACTION_ADD,			"ADD"			},
	{	eGUI_ACTION_SET_CONSTANT,	"SET_CONSTANT"	},
	{	eGUI_ACTION_SET_MOUSE,		"SET_MOUSE"		},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseAlignhTags[] =
{
	{	eGUI_ALIGNH_LEFT,			"LEFT"		},
	{	eGUI_ALIGNH_CENTRE,			"CENTRE"	},
	{	eGUI_ALIGNH_RIGHT,			"RIGHT"		},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseAlignvTags[] =
{
	{	eGUI_ALIGNV_TOP,			"TOP"		},
	{	eGUI_ALIGNV_CENTRE,			"CENTRE"	},
	{	eGUI_ALIGNV_BOTTOM,			"BOTTOM"	},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseButtonTags[] =
{
	{	eGUI_BUTTON_NONE,			"NONE"			},
	{	eGUI_BUTTON_CUSTOM,			"CUSTOM"		},
	{	eGUI_BUTTON_HOVER,			"HOVER"			},
	{	eGUI_BUTTON_SLIDERLINEADD,	"SLIDERLINEADD"	},
	{	eGUI_BUTTON_SLIDERLINESUB,	"SLIDERLINESUB"	},
	{	eGUI_BUTTON_SLIDERPAGEADD,	"SLIDERPAGEADD"	},
	{	eGUI_BUTTON_SLIDERPAGESUB,	"SLIDERPAGESUB"	},
	{	eGUI_BUTTON_SLIDERX,		"SLIDERX"		},
	{	eGUI_BUTTON_SLIDERY,		"SLIDERY"		},
	{	eGUI_BUTTON_SPRING,			"SPRING"		},
	{	eGUI_BUTTON_SWITCH,			"SWITCH"		},
	{	eGUI_BUTTON_TEXTBOX,		"TEXTBOX"		},
	{	eGUI_BUTTON_TEXTLINE,		"TEXTLINE"		},
	{	eGUI_BUTTON_VALUE,			"VALUE"			},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseControlTypeTags[] =
{
	{	eGUI_TYPE_BUTTON,	"BUTTON"	},
	{	eGUI_TYPE_LIST,		"LIST"		},
	{	eGUI_TYPE_SLIDER,	"SLIDER"	},
	{	eGUI_TYPE_WINDOW,	"WINDOW"	},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseKeySpecialTags[] =
{
	{	eGUIKEYSPECIAL_NONE,			"none"				},
	{	eGUIKEYSPECIAL_ALT,				"ALT"				},
	{	eGUIKEYSPECIAL_CTRL,			"CTRL"				},
	{	eGUIKEYSPECIAL_ALT_CTRL,		"ALT_CTRL"			},
	{	eGUIKEYSPECIAL_SHIFT,			"SHIFT"				},
	{	eGUIKEYSPECIAL_ALT_SHIFT,		"ALT_SHIT"			},
	{	eGUIKEYSPECIAL_CTRL_SHIFT,		"CTRL_SHIFT"		},
	{	eGUIKEYSPECIAL_ALT_CTRL_SHIFT,	"ALT_CTRL_SHIFT"	},
	{	0, 0 }
};

sTokeniserTagString	gGuiParseKeyStatusTags[] =
{
	{	eGUIKEYSTATUS_NONE,			"none"		},
	{	eGUIKEYSTATUS_HELD,			"HELD"		},
	{	eGUIKEYSTATUS_PRESSED,		"PRESSED"	},
	{	eGUIKEYSTATUS_RELEASED,		"RELEASED"	},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseScanCodeTags[] =
{
	{	eIKBDSCAN_A,					"A"			},
	{	eIKBDSCAN_B,					"B"			},
	{	eIKBDSCAN_C,					"C"			},
	{	eIKBDSCAN_D,					"D"			},
	{	eIKBDSCAN_E,					"E"			},
	{	eIKBDSCAN_F,					"F"			},
	{	eIKBDSCAN_G,					"G"			},
	{	eIKBDSCAN_H,					"H"			},
	{	eIKBDSCAN_I,					"I"			},
	{	eIKBDSCAN_J,					"J"			},
	{	eIKBDSCAN_K,					"K"			},
	{	eIKBDSCAN_L,					"L"			},
	{	eIKBDSCAN_M,					"M"			},
	{	eIKBDSCAN_N,					"N"			},
	{	eIKBDSCAN_O,					"O"			},
	{	eIKBDSCAN_P,					"P"			},
	{	eIKBDSCAN_Q,					"Q"			},
	{	eIKBDSCAN_R,					"R"			},
	{	eIKBDSCAN_S,					"S"			},
	{	eIKBDSCAN_T,					"T"			},
	{	eIKBDSCAN_U,					"U"			},
	{	eIKBDSCAN_V,					"V"			},
	{	eIKBDSCAN_W,					"W"			},
	{	eIKBDSCAN_X,					"X"			},
	{	eIKBDSCAN_Y,					"Y"			},
	{	eIKBDSCAN_Z,					"Z"			},

	{	eIKBDSCAN_0,					"0"			},
	{	eIKBDSCAN_1,					"1"			},
	{	eIKBDSCAN_2,					"2"			},
	{	eIKBDSCAN_3,					"3"			},
	{	eIKBDSCAN_4,					"4"			},
	{	eIKBDSCAN_5,					"5"			},
	{	eIKBDSCAN_6,					"6"			},
	{	eIKBDSCAN_7,					"7"			},
	{	eIKBDSCAN_8,					"8"			},
	{	eIKBDSCAN_9,					"9"			},

	{	eIKBDSCAN_F1,					"F1"		},
	{	eIKBDSCAN_F2,					"F2"		},
	{	eIKBDSCAN_F3,					"F3"		},
	{	eIKBDSCAN_F4,					"F4"		},
	{	eIKBDSCAN_F5,					"F5"		},
	{	eIKBDSCAN_F6,					"F6"		},
	{	eIKBDSCAN_F7,					"F7"		},
	{	eIKBDSCAN_F8,					"F8"		},
	{	eIKBDSCAN_F9,					"F9"		},
	{	eIKBDSCAN_F10,					"F10"		},

	{	eIKBDSCAN_AT,					"@"			},
	{	eIKBDSCAN_AMPERSAND,			"&"			},
	{	eIKBDSCAN_BAR,					"|"			},
	{	eIKBDSCAN_BACKSLASH,			"\\"		},
	{	eIKBDSCAN_CLOSEAPOSTROPHE,		"K"			},
	{	eIKBDSCAN_CLOSEBRACKET,			")"			},
	{	eIKBDSCAN_CLOSECURLYBRACKET,	"}"			},
	{	eIKBDSCAN_CLOSESQUAREBRACKET,	"]"			},
	{	eIKBDSCAN_COMMA,				","			},
	{	eIKBDSCAN_COLON,				":"			},
	{	eIKBDSCAN_DOLLARSIGN,			"$"			},
	{	eIKBDSCAN_DOT,					"."			},
	{	eIKBDSCAN_EQUALS,				"="			},
	{	eIKBDSCAN_EXCLAMATIONMARK,		"!"			},
	{	eIKBDSCAN_FORWARDSLASH,			"/"			},
	{	eIKBDSCAN_GREATERTHAN,			">"			},
	{	eIKBDSCAN_HASH,					"#"			},
	{	eIKBDSCAN_LESSTHAN,				"<"			},
	{	eIKBDSCAN_MINUS,				"-"			},
	{	eIKBDSCAN_OPENAPOSTROPHE,		"K"			},
	{	eIKBDSCAN_OPENBRACKET,			"("			},
	{	eIKBDSCAN_OPENCURLYBRACKET,		"{"			},
	{	eIKBDSCAN_OPENSQUAREBRACKET,	"["			},
	{	eIKBDSCAN_OVERSCORE,			"-"			},
	{	eIKBDSCAN_PERCENTAGE,			"%"			},
	{	eIKBDSCAN_PLUS,					"+"			},
	{	eIKBDSCAN_POUNDSIGN,			"£"			},
	{	eIKBDSCAN_QUOTEMARK,			"\""		},
	{	eIKBDSCAN_QUESTTIONMARK,		"?"			},
	{	eIKBDSCAN_SEMICOLON,			";"			},
	{	eIKBDSCAN_STAR,					"*"			},
	{	eIKBDSCAN_TILDE,				"~"			},
	{	eIKBDSCAN_UNDERSCORE,			"_"			},

	{	eIKBDSCAN_NUMPAD0,				"N0"		},
	{	eIKBDSCAN_NUMPAD1,				"N1"		},
	{	eIKBDSCAN_NUMPAD2,				"N2"		},
	{	eIKBDSCAN_NUMPAD3,				"N3"		},
	{	eIKBDSCAN_NUMPAD4,				"N4"		},
	{	eIKBDSCAN_NUMPAD5,				"N5"		},
	{	eIKBDSCAN_NUMPAD6,				"N6"		},
	{	eIKBDSCAN_NUMPAD7,				"N7"		},
	{	eIKBDSCAN_NUMPAD8,				"N8"		},
	{	eIKBDSCAN_NUMPAD9,				"N9"		},
	{	eIKBDSCAN_NUMPADOPENBRACKET,	"N("		},
	{	eIKBDSCAN_NUMPADCLOSEBRACKET,	"N)"		},
	{	eIKBDSCAN_NUMPADDOT,			"N."		},
	{	eIKBDSCAN_NUMPADENTER,			"ENTER"		},
	{	eIKBDSCAN_NUMPADMINUS,			"N-"		},
	{	eIKBDSCAN_NUMPADPLUS,			"N+"		},
	{	eIKBDSCAN_NUMPADSLASH,			"N/"		},
	{	eIKBDSCAN_NUMPADSTAR,			"N*"		},

	{	eIKBDSCAN_DOWNARROW,			"DOWN"		},
	{	eIKBDSCAN_LEFTARROW,			"LEFT"		},
	{	eIKBDSCAN_RIGHTARROW,			"RIGHT"		},
	{	eIKBDSCAN_UPARROW,				"UP"		},

	{	eIKBDSCAN_ALTERNATE,			"ALT"		},
	{	eIKBDSCAN_BACKSPACE,			"BACKSPACE"	},
	{	eIKBDSCAN_CAPSLOCK,				"CAPSLOCK"	},
	{	eIKBDSCAN_CLRHOME,				"CLRHOME"	},
	{	eIKBDSCAN_CONTROL,				"CTRL"		},
	{	eIKBDSCAN_DELETE,				"DEL"		},
	{	eIKBDSCAN_ESC,					"ESC"		},
	{	eIKBDSCAN_HELP,					"HELP"		},
	{	eIKBDSCAN_INSERT,				"INSERT"	},
	{	eIKBDSCAN_LEFTSHIFT,			"LSHIFT"	},
	{	eIKBDSCAN_RETURN,				"RETURN"	},
	{	eIKBDSCAN_RIGHTSHIFT,			"RSHIFT"	},
	{	eIKBDSCAN_SPACE,				"SPACE"		},
	{	eIKBDSCAN_TAB,					"TAB"		},
	{	eIKBDSCAN_UNDO,					"UNDO"		},
	{	0, 0 }
};

sTokeniserTagString	gGuiParseSliderTags[] =
{
	{	eGUI_SLIDER_VERTICAL,	"VERTICAL"		},
	{	eGUI_SLIDER_HORIZONTAL,	"HORIZONTAL"	},
	{	0,	0	},
};

sTokeniserTagString	gGuiParseVarTypeTags[] =
{
	{	eGUI_VAR_S8,		"S8"		},
	{	eGUI_VAR_S16,		"S16"		},
	{	eGUI_VAR_S32,		"S32"		},
	{	eGUI_VAR_U8,		"U8"		},
	{	eGUI_VAR_U16,		"U16"		},
	{	eGUI_VAR_U32,		"U32"		},
	{	eGUI_VAR_FP32,		"FP32"		},
	{	eGUI_VAR_STRING,	"STRING"	},
	{	0,	0	},
};




sTokeniserStructMember	gGuiParseActionMembers[] =
{
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,	eGUIPARSE_TAGVALUE_ACTION,	0,	"ACTION",		mOFFSET( sGuiAction, mAction )		},
	{	eTOKENISER_TYPE_S32,		0,						0,							0,	"CONSTANT",		mOFFSET( sGuiAction, mConstant )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VALUE,	0,							0,	"VALUE",		mOFFSET( sGuiAction, mpValue )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_WINDOW,	0,							0,	"WINDOW_CLOSE",	mOFFSET( sGuiAction, mpWindowClose )},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_WINDOW,	0,							0,	"WINDOW_OPEN",	mOFFSET( sGuiAction, mpWindowOpen )	},
	{	0, 0, 0, 0, 0, 0 }
};



sTokeniserStructMember	gGuiParseAssetMembers[] =
{
	{	eTOKENISER_TYPE_STRING,	0,	0,	0,	"FILENAME",	mOFFSET( sGuiAsset, mpFileName )	},
	{	eTOKENISER_TYPE_STRING,	0,	0,	0,	"CONTEXT",	mOFFSET( sGuiAsset, mpContext )		},
	{	0, 0, 0, 0, 0, 0 }
};


sTokeniserStructMember	gGuiParseButtonMembers[] =
{
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,			eGUIPARSE_TAGVALUE_BUTTON,	0,	"BUTTONTYPE",		mOFFSET( sGuiButton, mButtonType )						},
	{	eTOKENISER_TYPE_U16,		0,								0,							0,	"SUBTYPE",			mOFFSET( sGuiButton, mSubType )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTONSTYLE,	0,							0,	"BUTTONSTYLE",		mOFFSET( sGuiButton, mpStyle )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONLEFTCLICK",		mOFFSET( sGuiButton, mpOnLeftClick )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONLEFTHELD",		mOFFSET( sGuiButton, mpOnLeftHeld )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONLEFTRELEASE",	mOFFSET( sGuiButton, mpOnLeftRelease )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONRIGHTCLICK",		mOFFSET( sGuiButton, mpOnRightClick )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONRIGHTHELD",		mOFFSET( sGuiButton, mpOnRightHeld )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,			0,							0,	"ONRIGHTRELEASE",	mOFFSET( sGuiButton, mpOnRightRelease )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,			0,							0,	"ONIKBD",			mOFFSET( sGuiButton, mpOnIKBD )							},

	{	eTOKENISER_TYPE_CHUNKHASH,	0,								0,							0,	"HASH",				mOFFSET( sGuiButton, mInfo.mHash )						},
	{	eTOKENISER_TYPE_CHUNKNAME,	0,								0,							0,	"NAME",				mOFFSET( sGuiButton, mInfo.mpName )						},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"X",				mOFFSET( sGuiButton, mInfo.mRectPair.mLocal.mX )		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"Y",				mOFFSET( sGuiButton, mInfo.mRectPair.mLocal.mY )		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"WIDTH",			mOFFSET( sGuiButton, mInfo.mRectPair.mLocal.mWidth)		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"HEIGHT",			mOFFSET( sGuiButton, mInfo.mRectPair.mLocal.mHeight )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_LOCK,			0,							0,	"LOCK",				mOFFSET( sGuiButton, mInfo.mpLock )						},
	{	eTOKENISER_TYPE_CONSTANT,	eTOKENISER_TYPE_U16,			eGUI_TYPE_BUTTON,			0,	"TYPE",				mOFFSET( sGuiButton, mInfo.mType )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VALUE,			0,							0,	"VALUE",			mOFFSET( sGuiButton, mInfo.mpValue )					},

	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"SPRITE_X",			mOFFSET( sGuiButton, mSprite.mRectPair.mLocal.mX )		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"SPRITE_Y",			mOFFSET( sGuiButton, mSprite.mRectPair.mLocal.mY )		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"SPRITE_WIDTH",		mOFFSET( sGuiButton, mSprite.mRectPair.mLocal.mWidth )	},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"SPRITE_HEIGHT",	mOFFSET( sGuiButton, mSprite.mRectPair.mLocal.mHeight )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ASSET,			0,							0,	"SPRITE_ASSET",		mOFFSET( sGuiButton, mSprite.mpAsset )					},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,				eGUIPARSE_TAGVALUE_ALIGNH,	0,	"SPRITE_ALIGNH",	mOFFSET( sGuiButton, mSprite.mAlign.mH )				},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,				eGUIPARSE_TAGVALUE_ALIGNV,	0,	"SPRITE_ALIGNV",	mOFFSET( sGuiButton, mSprite.mAlign.mV )				},

	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"STRING_X",			mOFFSET( sGuiButton, mString.mRects.mLocal.mX )			},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"STRING_Y",			mOFFSET( sGuiButton, mString.mRects.mLocal.mY )			},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"STRING_WIDTH",		mOFFSET( sGuiButton, mString.mRects.mLocal.mWidth )		},
	{	eTOKENISER_TYPE_S16,		0,								0,							0,	"STRING_HEIGHT",	mOFFSET( sGuiButton, mString.mRects.mLocal.mHeight )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FONTGROUP,		0,							0,	"STRING_FONTGROUP",	mOFFSET( sGuiButton, mString.mpFontGroup )				},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,				eGUIPARSE_TAGVALUE_ALIGNH,	0,	"STRING_ALIGNH",	mOFFSET( sGuiButton, mString.mAlign.mH )				},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,				eGUIPARSE_TAGVALUE_ALIGNV,	0,	"STRING_ALIGNV",	mOFFSET( sGuiButton, mString.mAlign.mV )				},
	{	eTOKENISER_TYPE_STRING,		0,								0,							0,	"STRING_TITLE",		mOFFSET( sGuiButton, mString.mpTitle )					},
	{	eTOKENISER_TYPE_STRING,		0,								0,							0,	"STRING_VAR_NAME",	mOFFSET( sGuiButton, mString.mVar.mpName )				},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,			eGUIPARSE_TAGVALUE_VARTYPE,	0,	"STRING_VAR_TYPE",	mOFFSET( sGuiButton, mString.mVar.mType )				},
	{	eTOKENISER_TYPE_U16,		0,								0,							0,	"STRING_VAR_SIZE",	mOFFSET( sGuiButton, mString.mVar.mSize )				},
	{	0, 0, 0, 0, 0, 0 }
};


sTokeniserStructMember	gGuiParseButtonStyleMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FILL,	0,	0,	"FILL_LOCKED",		mOFFSET( sGuiButtonStyle, mpFillLocked )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FILL,	0,	0,	"FILL_NORMAL",		mOFFSET( sGuiButtonStyle, mpFillNormal )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FILL,	0,	0,	"FILL_SELECTED",	mOFFSET( sGuiButtonStyle, mpFillSelected )		},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseColourMembers[] =
{
	{	eTOKENISER_TYPE_U8,	0,	0,	0,	"R",		mOFFSET( sGuiColour, mR )			},
	{	eTOKENISER_TYPE_U8,	0,	0,	0,	"G",		mOFFSET( sGuiColour, mR )			},
	{	eTOKENISER_TYPE_U8,	0,	0,	0,	"B",		mOFFSET( sGuiColour, mR )			},
	{	eTOKENISER_TYPE_U8,	0,	0,	0,	"A",		mOFFSET( sGuiColour, mR )			},
	{	eTOKENISER_TYPE_U8,	0,	0,	0,	"PALINDEX",	mOFFSET( sGuiColour, mPalIndex )	},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseCursorMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ASSET,	0,	0,	"ASSET",	mOFFSET( sGuiCursor, mpAsset )	},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseFillMembers[] =
{
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,		eGUIPARSE_TAGVALUE_FILLTYPE,	0,	"TYPE",			mOFFSET( sGuiFill, mFillType )							},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,		eGUIPARSE_TAGVALUE_FILLPATTERN,	0,	"PATTERN",		mOFFSET( sGuiFill, mFillPattern )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_COLOUR,		0,								0,	"COLOUR_MAIN",	mOFFSET( sGuiFill, mpColours[ eGUI_FILLCOLOUR_MAIN ] )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_COLOUR,		0,								0,	"COLOUR_TL0",	mOFFSET( sGuiFill, mpColours[ eGUI_FILLCOLOUR_TL0 ] )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_COLOUR,		0,								0,	"COLOUR_TL1",	mOFFSET( sGuiFill, mpColours[ eGUI_FILLCOLOUR_TL1 ] )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_COLOUR,		0,								0,	"COLOUR_BR0",	mOFFSET( sGuiFill, mpColours[ eGUI_FILLCOLOUR_BR0 ] )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_COLOUR,		0,								0,	"COLOUR_BR1",	mOFFSET( sGuiFill, mpColours[ eGUI_FILLCOLOUR_BR1 ] )	},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseFontGroupMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ASSET,		0,		0,	"LOCKED",		mOFFSET( sGuiFontGroup, mpLocked )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ASSET,		0,		0,	"NORMAL",		mOFFSET( sGuiFontGroup, mpNormal )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ASSET,		0,		0,	"SELECTED",		mOFFSET( sGuiFontGroup, mpSelected )	},
	{	0,	0,	0	},
};

sTokeniserStructMember	gGuiParseKeyActionMembers[] =
{
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,		eGUIPARSE_TAGVALUE_KEYSPECIAL,	0,	"SPECIAL",		mOFFSET( sGuiKeyAction, mSpecialStatus )	},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,		eGUIPARSE_TAGVALUE_SCANCODE,	0,	"SCANCODE",		mOFFSET( sGuiKeyAction, mScanCode )			},
	{	eTOKENISER_TYPE_CHAR,		0,						0,								0,	"ASCII",		mOFFSET( sGuiKeyAction,	mAscii )			},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U8,		eGUIPARSE_TAGVALUE_KEYSTATUS,	0,	"CONDITION",	mOFFSET( sGuiKeyAction, mKeyCondition )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_ACTION,	0,								0,	"ACTION",		mOFFSET( sGuiKeyAction, mpAction )			},
	{	0, 0, 0, 0, 0, 0 }
};

sTokeniserStructMember	gGuiParseListMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,	0,				0,	"BUTTON",	mOFFSET( sGuiList, mpButton )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_SLIDER,	0,				0,	"SLIDER",	mOFFSET( sGuiList, mpSlider )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_WINDOW,	0,				0,	"WINDOW",	mOFFSET( sGuiList, mpWindow )						},

	{	eTOKENISER_TYPE_CHUNKHASH,	0,						0,				0,	"HASH",		mOFFSET( sGuiList, mInfo.mHash )					},
	{	eTOKENISER_TYPE_S16,		0,						0,				0,	"X",		mOFFSET( sGuiList, mInfo.mRectPair.mLocal.mX )		},
	{	eTOKENISER_TYPE_S16,		0,						0,				0,	"Y",		mOFFSET( sGuiList, mInfo.mRectPair.mLocal.mY )		},
	{	eTOKENISER_TYPE_S16,		0,						0,				0,	"WIDTH",	mOFFSET( sGuiList, mInfo.mRectPair.mLocal.mWidth)	},
	{	eTOKENISER_TYPE_S16,		0,						0,				0,	"HEIGHT",	mOFFSET( sGuiList, mInfo.mRectPair.mLocal.mHeight )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_LOCK,	0,				0,	"LOCK",		mOFFSET( sGuiList, mInfo.mpLock )					},
	{	eTOKENISER_TYPE_CHUNKNAME,	0,						0,				0,	"NAME",		mOFFSET( sGuiList, mInfo.mpName )					},
	{	eTOKENISER_TYPE_CONSTANT,	eTOKENISER_TYPE_U16,	eGUI_TYPE_LIST,	0,	"TYPE",		mOFFSET( sGuiList, mInfo.mType )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VALUE,	0,				0,	"VALUE",	mOFFSET( sGuiList, mInfo.mpValue )					},

	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseLockMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,		eGUIPARSE_CHUNK_VAR,		0,		0,	"LOCKVAR",		mOFFSET( sGuiLock, mpLockVar )			},
	{	eTOKENISER_TYPE_REFERENCE,		eGUIPARSE_CHUNK_VAR,		0,		0,	"LOCKVALUE",	mOFFSET( sGuiLock, mpLockValue )		},
	{	eTOKENISER_TYPE_REFERENCE,		eGUIPARSE_CHUNK_VAR,		0,		0,	"VISVAR",		mOFFSET( sGuiLock, mpVisVar )			},
	{	eTOKENISER_TYPE_REFERENCE,		eGUIPARSE_CHUNK_VAR,		0,		0,	"VISVALUE",		mOFFSET( sGuiLock, mpVisValue )			},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseSliderMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,		0,							0,	"PAGESIZE",			mOFFSET( sGuiSlider, mpPageSize )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,		0,							0,	"LINESIZE",			mOFFSET( sGuiSlider, mpLineSize )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_WINDOW,		0,							0,	"WINDOW",			mOFFSET( sGuiSlider, mpWindow )								},
	{	eTOKENISER_TYPE_U16,		0,							0,							0,	"SIZEMIN",			mOFFSET( sGuiSlider, mSizeMin )								},
	{	eTOKENISER_TYPE_U16,		0,							0,							0,	"BUTTONSIZE",		mOFFSET( sGuiSlider, mButtonSize )							},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,		eGUIPARSE_TAGVALUE_SLIDER,	0,	"SLIDERTYPE",		mOFFSET( sGuiSlider, mSliderType )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,		0,							0,	"BUTTONADD",		mOFFSET( sGuiSlider, mpButtons[ eGUI_SLIDERBUT_ADD ] )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,		0,							0,	"BUTTONSUB",		mOFFSET( sGuiSlider, mpButtons[ eGUI_SLIDERBUT_SUB ] )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,		0,							0,	"BUTTONMAIN",		mOFFSET( sGuiSlider, mpButtons[ eGUI_SLIDERBUT_MAIN ] )		},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,		0,							0,	"BUTTONBACKADD",	mOFFSET( sGuiSlider, mpButtons[ eGUI_SLIDERBUT_BACKADD ] )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,		0,							0,	"BUTTONBACKSUB",	mOFFSET( sGuiSlider, mpButtons[ eGUI_SLIDERBUT_BACKSUB ] )	},

	{	eTOKENISER_TYPE_CHUNKHASH,	0,							0,							0,	"HASH",				mOFFSET( sGuiSlider, mInfo.mHash )							},
	{	eTOKENISER_TYPE_CHUNKNAME,	0,							0,							0,	"NAME",				mOFFSET( sGuiWindow, mInfo.mpName )							},
	{	eTOKENISER_TYPE_S16,		0,							0,							0,	"X",				mOFFSET( sGuiSlider, mInfo.mRectPair.mLocal.mX )			},
	{	eTOKENISER_TYPE_S16,		0,							0,							0,	"Y",				mOFFSET( sGuiSlider, mInfo.mRectPair.mLocal.mY )			},
	{	eTOKENISER_TYPE_S16,		0,							0,							0,	"WIDTH",			mOFFSET( sGuiSlider, mInfo.mRectPair.mLocal.mWidth)			},
	{	eTOKENISER_TYPE_S16,		0,							0,							0,	"HEIGHT",			mOFFSET( sGuiSlider, mInfo.mRectPair.mLocal.mHeight )		},
	{	eTOKENISER_TYPE_CONSTANT,	eTOKENISER_TYPE_U16,		eGUI_TYPE_SLIDER,			0,	"TYPE",				mOFFSET( sGuiSlider, mInfo.mType )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_LOCK,		0,							0,	"LOCK",				mOFFSET( sGuiSlider, mInfo.mpLock )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VALUE,		0,							0,	"VALUE",			mOFFSET( sGuiSlider, mInfo.mpValue )						},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseValueMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,	0,	0,	"VAR",			mOFFSET( sGuiValue, mpVar )			},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,	0,	0,	"VAR_MIN",		mOFFSET( sGuiValue, mpValueMin )	},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,	0,	0,	"VAR_MAX",		mOFFSET( sGuiValue, mpValueMax )	},
	{	eTOKENISER_TYPE_S32,		0,						0,	0,	"MIN",			mOFFSET( sGuiValue, mMin )			},
	{	eTOKENISER_TYPE_S32,		0,						0,	0,	"MAX",			mOFFSET( sGuiValue, mMax )			},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseVarMembers[] =
{
	{	eTOKENISER_TYPE_STRING,		0,						0,							0,	"NAME",	mOFFSET( sGuiVar, mpName )	},
	{	eTOKENISER_TYPE_TAGVALUE,	eTOKENISER_TYPE_U16,	eGUIPARSE_TAGVALUE_VARTYPE,	0,	"TYPE",	mOFFSET( sGuiVar, mType )	},
	{	eTOKENISER_TYPE_U16,		0,						0,							0,	"SIZE",	mOFFSET( sGuiVar, mSize )	},
	{	0,	0,	0	},
};


sTokeniserStructMember	gGuiParseWindowMembers[] =
{
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FONTGROUP,		0,										0,	"FONTGROUP",		mOFFSET( sGuiWindow, mpFontGroup )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTONSTYLE,	0,										0,	"BUTTONSTYLE",		mOFFSET( sGuiWindow, mpButtonStyle )					},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_FILL,			0,										0,	"FILL",				mOFFSET( sGuiWindow, mpFill )							},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VAR,			0,										0,	"ONIKBD",			mOFFSET( sGuiWindow, mpOnIKBD )							},

	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_BUTTON,			mOFFSET( sGuiWindow, mControlCount ),	1,	"BUTTON",			mOFFSET( sGuiWindow, mppControls )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_SLIDER,			mOFFSET( sGuiWindow, mControlCount ),	2,	"SLIDER",			mOFFSET( sGuiWindow, mppControls )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_WINDOW,			mOFFSET( sGuiWindow, mControlCount ),	2,	"WINDOW",			mOFFSET( sGuiWindow, mppControls )						},
	{	eTOKENISER_TYPE_U16,		0,								0,										0,	"CONTROLCOUNT",		mOFFSET( sGuiWindow, mControlCount )					},

	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_KEYACTION,		mOFFSET( sGuiWindow, mKeyActionCount ),	1,	"KEYACTION",		mOFFSET( sGuiWindow, mppKeyActions )					},
	{	eTOKENISER_TYPE_U16,		0,								0,										0,	"KEYACTIONCOUNT",	mOFFSET( sGuiWindow, mKeyActionCount )					},

	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"CANVASX",			mOFFSET( sGuiWindow, mCanvas.mX )						},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"CANVASY",			mOFFSET( sGuiWindow, mCanvas.mY )						},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"CANVASWIDTH",		mOFFSET( sGuiWindow, mCanvas.mWidth )					},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"CANVASHEIGHT",		mOFFSET( sGuiWindow, mCanvas.mHeight )					},

	{	eTOKENISER_TYPE_CHUNKHASH,	0,								0,										0,	"HASH",				mOFFSET( sGuiWindow, mInfo.mHash )						},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"X",				mOFFSET( sGuiWindow, mInfo.mRectPair.mLocal.mX )		},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"Y",				mOFFSET( sGuiWindow, mInfo.mRectPair.mLocal.mY )		},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"WIDTH",			mOFFSET( sGuiWindow, mInfo.mRectPair.mLocal.mWidth)		},
	{	eTOKENISER_TYPE_S16,		0,								0,										0,	"HEIGHT",			mOFFSET( sGuiWindow, mInfo.mRectPair.mLocal.mHeight )	},
	{	eTOKENISER_TYPE_CHUNKNAME,	0,								0,										0,	"NAME",				mOFFSET( sGuiWindow, mInfo.mpName )						},
	{	eTOKENISER_TYPE_CONSTANT,	eTOKENISER_TYPE_U16,			eGUI_TYPE_WINDOW,						0,	"TYPE",				mOFFSET( sGuiWindow, mInfo.mType )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_LOCK,			0,										0,	"LOCK",				mOFFSET( sGuiWindow, mInfo.mpLock )						},
	{	eTOKENISER_TYPE_REFERENCE,	eGUIPARSE_CHUNK_VALUE,			0,										0,	"VALUE",			mOFFSET( sGuiWindow, mInfo.mpValue )					},

	{	0,	0,	0	},
};

sTokeniserChunkDef	gGuiParseChunkDefs[] =
{
	{	eGUIPARSE_CHUNK_ACTION,			"ACTION",		gGuiParseActionMembers,			sizeof(sGuiAction),			mOFFSET( sGuiData, mActionCount ),		mOFFSET( sGuiData, mpActions )		},
	{	eGUIPARSE_CHUNK_ASSET,			"ASSET",		gGuiParseAssetMembers,			sizeof(sGuiAsset),			mOFFSET( sGuiData, mAssetCount ),		mOFFSET( sGuiData, mpAssets )		},
	{	eGUIPARSE_CHUNK_BUTTON,			"BUTTON",		gGuiParseButtonMembers,			sizeof(sGuiButton),			mOFFSET( sGuiData, mButtonCount ),		mOFFSET( sGuiData, mpButtons )		},
	{	eGUIPARSE_CHUNK_BUTTONSTYLE,	"BUTTONSTYLE",	gGuiParseButtonStyleMembers,	sizeof(sGuiButtonStyle),	mOFFSET( sGuiData, mButtonStyleCount ),	mOFFSET( sGuiData, mpButtonStyles )	},
	{	eGUIPARSE_CHUNK_COLOUR,			"COLOUR",		gGuiParseColourMembers,			sizeof(sGuiColour),			mOFFSET( sGuiData, mColourCount ),		mOFFSET( sGuiData, mpColours )		},
	{	eGUIPARSE_CHUNK_CURSOR,			"CURSOR",		gGuiParseCursorMembers,			sizeof(sGuiCursor),			mOFFSET( sGuiData, mCursorCount ),		mOFFSET( sGuiData, mpCursors )		},
	{	eGUIPARSE_CHUNK_FILL,			"FILL",			gGuiParseFillMembers,			sizeof(sGuiFill),			mOFFSET( sGuiData, mFillCount ),		mOFFSET( sGuiData, mpFills )		},
	{	eGUIPARSE_CHUNK_FONTGROUP,		"FONTGROUP",	gGuiParseFontGroupMembers,		sizeof(sGuiFontGroup),		mOFFSET( sGuiData, mFontGroupCount ),	mOFFSET( sGuiData, mpFontGroups )	},
	{	eGUIPARSE_CHUNK_KEYACTION,		"KEYACTION",	gGuiParseKeyActionMembers,		sizeof(sGuiKeyAction),		mOFFSET( sGuiData, mKeyActionCount ),	mOFFSET( sGuiData, mpKeyActions )	},
	{	eGUIPARSE_CHUNK_LIST,			"LIST",			gGuiParseListMembers,			sizeof(sGuiList),			mOFFSET( sGuiData, mListCount ),		mOFFSET( sGuiData, mpLists )		},
	{	eGUIPARSE_CHUNK_LOCK,			"LOCK",			gGuiParseLockMembers,			sizeof(sGuiLock),			mOFFSET( sGuiData, mLockCount ),		mOFFSET( sGuiData, mpLocks )		},
	{	eGUIPARSE_CHUNK_SLIDER,			"SLIDER",		gGuiParseSliderMembers,			sizeof(sGuiSlider),			mOFFSET( sGuiData, mSliderCount ),		mOFFSET( sGuiData, mpSliders )		},
	{	eGUIPARSE_CHUNK_VALUE,			"VALUE",		gGuiParseValueMembers,			sizeof(sGuiValue),			mOFFSET( sGuiData, mValueCount ),		mOFFSET( sGuiData, mpValues )		},
	{	eGUIPARSE_CHUNK_VAR,			"VAR",			gGuiParseVarMembers,			sizeof(sGuiVar),			mOFFSET( sGuiData, mVarCount ),			mOFFSET( sGuiData, mpVars )			},
	{	eGUIPARSE_CHUNK_WINDOW,			"WINDOW",		gGuiParseWindowMembers,			sizeof(sGuiWindow),			mOFFSET( sGuiData, mWindowCount ),		mOFFSET( sGuiData, mpWindows )		},
	{	0,	0,	0,	0,	0,	0	},
};

sTokeniserTagStringList	gGuiParseTagLists[] =
{
	{	eGUIPARSE_TAGVALUE_ACTION,			eGUI_ACTION_LIMIT,		gGuiParseActionTags		},
	{	eGUIPARSE_TAGVALUE_ALIGNH,			eGUI_ALIGNH_LIMIT,		gGuiParseAlignhTags		},
	{	eGUIPARSE_TAGVALUE_ALIGNV,			eGUI_ALIGNV_LIMIT,		gGuiParseAlignvTags		},
	{	eGUIPARSE_TAGVALUE_BUTTON,			eGUI_BUTTON_LIMIT,		gGuiParseButtonTags		},
	{	eGUIPARSE_TAGVALUE_KEYSPECIAL,		eGUIKEYSPECIAL_LIMIT,	gGuiParseKeySpecialTags	},
	{	eGUIPARSE_TAGVALUE_KEYSTATUS,		eGUIKEYSTATUS_LIMIT,	gGuiParseKeyStatusTags	},
	{	eGUIPARSE_TAGVALUE_SCANCODE,		128,					gGuiParseScanCodeTags	},
	{	eGUIPARSE_TAGVALUE_SLIDER,			eGUI_SLIDER_LIMIT,		gGuiParseSliderTags		},
	{	eGUIPARSE_TAGVALUE_VARTYPE,			eGUI_VAR_LIMIT,			gGuiParseVarTypeTags	},
	{	eGUIPARSE_TAGVALUE_FILLPATTERN,		0,						0						},
	{	eGUIPARSE_TAGVALUE_FILLTYPE,		0,						0						},
	{	0,	0,	0	}
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	GuiParse_OnWarning( const char * apName );
void	GuiParse_InfoInit( sTokeniserInfo * apInfo );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_Init(void)
* ACTION   : GuiParse_Init
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_Init(void)
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_DeInit(void)
* ACTION   : GuiParse_DeInit
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_DeInit(void)
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiParse_Text2( const char * apText,const U32 aSize )
* ACTION   : GuiParse_Text2
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

sGuiData *	GuiParse_Text( const char * apText,const U32 aSize )
{
	sTokeniserInfo		lInfo;
	sTokeniserBuilder *	lpBuilder;
	sGuiData *			lpData;

	GuiParse_InfoInit( &lInfo );

	lpBuilder             = Tokeniser_Init( apText, aSize, &lInfo );
	lpData                = ( (sGuiData*)Tokeniser_Serialise( lpBuilder ) );
	Tokeniser_DeInit( lpBuilder );
	return( lpData );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_Delocate(sGuiData * apHeader)
* ACTION   : GuiParse_Delocate
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_Delocate(sGuiData * apHeader)
{
	sTokeniserInfo		lInfo;

	GuiParse_InfoInit( &lInfo );

	Tokeniser_Delocate( (U8*)apHeader, &lInfo );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_Relocate(sGuiData * apHeader)
* ACTION   : GuiParse_Relocate
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_Relocate(sGuiData * apHeader)
{
	sTokeniserInfo		lInfo;

	GuiParse_InfoInit( &lInfo );

	Tokeniser_Delocate( (U8*)apHeader, &lInfo );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_ShowInfo(sGuiData * apHeader)
* ACTION   : GuiParse_ShowInfo
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_ShowInfo(sGuiData * apHeader)
{
	sTokeniserInfo		lInfo;

	GuiParse_InfoInit( &lInfo );

	Tokeniser_ShowInfo( (U8*)apHeader, &lInfo );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void GuiParse_InfoInit(sTokeniserInfo * apInfo)
* ACTION   : GuiParse_InfoInit
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void GuiParse_InfoInit(sTokeniserInfo * apInfo)
{
	if( apInfo )
	{
		apInfo->mChunkCount     = eGUIPARSE_CHUNK_LIMIT;
		apInfo->mContainerSize  = sizeof( sGuiData );
		apInfo->mOnWarning      = GuiParse_OnWarning;
		apInfo->mpChunkDefs     = gGuiParseChunkDefs;
		apInfo->mpTagStringList = gGuiParseTagLists;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GuiParse_OnWarning( const char * apName )
* ACTION   : GuiParse_OnWarning
* CREATION : 26.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	GuiParse_OnWarning( const char * apName )
{
	DebugLog_Printf0( apName );
	printf( "%s\n", apName );
}


/* ################################################################################ */

#ifndef	INCLUDED_GUI_H
#define	INCLUDED_GUI_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/ASSET.H>
#include	<GODLIB/HASHTREE/HASHTREE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGUI_IKBD_LIMIT			256

enum
{
	eGUI_TYPE_BUTTON,
	eGUI_TYPE_LIST,
	eGUI_TYPE_SLIDER,
	eGUI_TYPE_WINDOW,

	eGUI_TYPE_LIMIT,
};

enum
{
	eGUI_EDITTYPE_DECIMAL,
	eGUI_EDITTYPE_HEX,
	eGUI_EDITTYPE_FILENAME,
	eGUI_EDITTYPE_FLOAT,
	eGUI_EDITTYPE_PAGE,
	eGUI_EDITTYPE_STRING,

	eGUI_EDITTYPE_LIMIT
};

enum
{
	eGUI_FILLCOLOUR_MAIN,
	eGUI_FILLCOLOUR_TL0,
	eGUI_FILLCOLOUR_TL1,
	eGUI_FILLCOLOUR_BR0,
	eGUI_FILLCOLOUR_BR1,

	eGUI_FILLCOLOUR_LIMIT
};

enum
{
	eGUI_BUTTON_NONE,
	eGUI_BUTTON_CUSTOM,
	eGUI_BUTTON_HOVER,
	eGUI_BUTTON_SLIDERLINEADD,
	eGUI_BUTTON_SLIDERLINESUB,
	eGUI_BUTTON_SLIDERPAGEADD,
	eGUI_BUTTON_SLIDERPAGESUB,
	eGUI_BUTTON_SLIDERX,
	eGUI_BUTTON_SLIDERY,
	eGUI_BUTTON_SPRING,
	eGUI_BUTTON_SWITCH,
	eGUI_BUTTON_TEXTBOX,
	eGUI_BUTTON_TEXTLINE,
	eGUI_BUTTON_VALUE,

	eGUI_BUTTON_LIMIT
};

enum
{
	eGUI_MOUSEBUTTON_NONE,
	eGUI_MOUSEBUTTON_CLICKED,
	eGUI_MOUSEBUTTON_HELD,
	eGUI_MOUSEBUTTON_RELEASED
};

enum
{
	eGUI_ALIGNH_LEFT,
	eGUI_ALIGNH_CENTRE,
	eGUI_ALIGNH_RIGHT,

	eGUI_ALIGNH_LIMIT
};

enum
{
	eGUI_ALIGNV_TOP,
	eGUI_ALIGNV_CENTRE,
	eGUI_ALIGNV_BOTTOM,

	eGUI_ALIGNV_LIMIT
};

enum
{
	eGUI_ACTION_NONE,
	eGUI_ACTION_APP_EXIT,
	eGUI_ACTION_CALL,
	eGUI_ACTION_ADD,
	eGUI_ACTION_SET_CONSTANT,
	eGUI_ACTION_SET_MOUSE,

	eGUI_ACTION_LIMIT
};

enum
{
	eGUI_SLIDERBUT_ADD,
	eGUI_SLIDERBUT_SUB,
	eGUI_SLIDERBUT_MAIN,
	eGUI_SLIDERBUT_BACKADD,
	eGUI_SLIDERBUT_BACKSUB,

	eGUI_SLIDERBUT_LIMIT
};

enum
{
	eGUI_SLIDER_VERTICAL,
	eGUI_SLIDER_HORIZONTAL,

	eGUI_SLIDER_LIMIT
};

enum
{
	eGUI_VAR_S8,
	eGUI_VAR_S16,
	eGUI_VAR_S32,
	eGUI_VAR_U8,
	eGUI_VAR_U16,
	eGUI_VAR_U32,
	eGUI_VAR_FP32,

	eGUI_VAR_STRING,

	eGUI_VAR_LIMIT,
};

enum
{
	eGUIEVENT_BUTTON_LEFTCLICK,
	eGUIEVENT_BUTTON_LEFTHELD,
	eGUIEVENT_BUTTON_LEFTRELEASE,
	eGUIEVENT_BUTTON_RIGHTCLICK,
	eGUIEVENT_BUTTON_RIGHTHELD,
	eGUIEVENT_BUTTON_RIGHTRELEASE,
	eGUIEVENT_BUTTON_UPDATE,

	eGUIEVENT_WINDOW_CLOSE,
	eGUIEVENT_WINDOW_OPEN,
	eGUIEVENT_WINDOW_REFRESH,

	eGUIEVENT_LIMIT
};

enum
{
	eGUIKEYSPECIAL_NONE           = 0,
	eGUIKEYSPECIAL_ALT            = 1,
	eGUIKEYSPECIAL_CTRL           = 2,
	eGUIKEYSPECIAL_ALT_CTRL       = ( eGUIKEYSPECIAL_ALT  + eGUIKEYSPECIAL_CTRL ),
	eGUIKEYSPECIAL_SHIFT          = 4,
	eGUIKEYSPECIAL_ALT_SHIFT      = ( eGUIKEYSPECIAL_ALT  + eGUIKEYSPECIAL_SHIFT ),
	eGUIKEYSPECIAL_CTRL_SHIFT     = ( eGUIKEYSPECIAL_CTRL + eGUIKEYSPECIAL_SHIFT ),
	eGUIKEYSPECIAL_ALT_CTRL_SHIFT = ( eGUIKEYSPECIAL_ALT  + eGUIKEYSPECIAL_CTRL + eGUIKEYSPECIAL_SHIFT ),

	eGUIKEYSPECIAL_LIMIT

};

enum
{
	eGUIKEYSTATUS_NONE,
	eGUIKEYSTATUS_HELD,
	eGUIKEYSTATUS_PRESSED,
	eGUIKEYSTATUS_RELEASED,

	eGUIKEYSTATUS_LIMIT
};

enum
{
	eGUIINFO_FLAG_SELECTED  = (1<<0),
	eGUIINFO_FLAG_LOCKED    = (1<<1),
	eGUIINFO_FLAG_INVISIBLE = (1<<2),
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sGuiEvent
{
	U16		mEvent;
	U16		mPad;
	void *	mpInfo;
} sGuiEvent;

typedef	struct	sGuiAlign
{
	U8	mH;
	U8	mV;
} sGuiAlign;

typedef	struct	sGuiPos
{
	S16	mX;
	S16	mY;
} sGuiPos;

typedef	struct	sGuiRect
{
	S16	mX;
	S16	mY;
	S16	mWidth;
	S16	mHeight;
} sGuiRect;

typedef	struct	sGuiRectPair
{
	sGuiRect	mLocal;
	sGuiRect	mWorld;
} sGuiRectPair;

typedef	struct	sGuiAsset
{
	sAssetClient 	mAsset;
	char *			mpFileName;
	char *			mpContext;
	void *			mpData;
} sGuiAsset;

typedef	struct sGuiColour
{
	U8	mR;
	U8	mG;
	U8	mB;
	U8	mA;
	U8	mPalIndex;
	U8	mFlags;
} sGuiColour;

typedef	struct	sGuiCursor
{
	sGuiAsset *	mpAsset;
} sGuiCursor;

typedef	struct	sGuiFontGroup
{
	sGuiAsset *		mpNormal;
	sGuiAsset *		mpSelected;
	sGuiAsset *		mpLocked;
} sGuiFontGroup;

typedef	struct	sGuiMouse
{
	S16	mX;
	S16	mY;
	U8	mButtonLeft;
	U8	mButtonRight;
	U16	mPad;
	sGuiAsset *	mpSprite;
} sGuiMouse;

typedef	struct	sGuiIKBD
{
	U16	mCount;
	U8	mData[ dGUI_IKBD_LIMIT ];
} sGuiIKBD;

typedef	struct	sGuiVar
{
	char *					mpName;
	sHashTreeVar *			mpVar;
	sHashTreeVarClient *	mpVarClient;
	U16						mStructOffset;
	U16						mStructSize;
	U16						mSize;
	U16						mType;
} sGuiVar;

typedef struct	sGuiValue
{
	sGuiVar *		mpVar;
	sGuiVar *		mpValueMin;
	sGuiVar *		mpValueMax;
	S32				mMin;
	S32				mMax;
} sGuiValue;

typedef	struct	sGuiLock
{
	sGuiVar *		mpLockVar;
	sGuiVar *		mpVisVar;
	sGuiVar *		mpLockValue;
	sGuiVar *		mpVisValue;
} sGuiLock;

typedef	struct sGuiAction
{
	U16						mAction;
	U16						mPad;
	S32						mConstant;
	sGuiValue *				mpValue;
	struct	sGuiWindow *	mpWindowClose;
	struct	sGuiWindow *	mpWindowOpen;
} sGuiAction;

typedef	struct	sGuiKeyAction
{
	U8				mSpecialStatus;
	U8				mScanCode;
	U8				mAscii;
	U8				mKeyCondition;
	sGuiAction *	mpAction;
} sGuiKeyAction;

typedef	struct	sGuiInfo
{
	sGuiRectPair	mRectPair;
	U32				mHash;
	U32				mFlags;
	U16				mRedrawFlag;
	U16				mType;
	sGuiValue *		mpValue;
	sGuiLock *		mpLock;
	char *			mpName;
	sHashTreeVar *	mpEventVar;
	sGuiEvent		mEvent;
} sGuiInfo;


typedef struct	sGuiSprite
{
	sGuiRectPair	mRectPair;
	sGuiAsset *		mpAsset;
	U16				mRedrawFlag;
	sGuiAlign		mAlign;
} sGuiSprite;


typedef	struct	sGuiFill
{
	U16				mFillType;
	U16				mFillPattern;
	sGuiColour *	mpColours[ eGUI_FILLCOLOUR_LIMIT ];
} sGuiFill;

typedef	struct	sGuiString
{
	sGuiRectPair	mRects;
	sGuiFontGroup *	mpFontGroup;
	char *			mpTitle;
	sGuiVar			mVar;
	sGuiAlign		mAlign;
	U16				mPad;
} sGuiString;

typedef struct	sGuiButtonStyle
{
	sGuiFill *	mpFillNormal;
	sGuiFill *	mpFillSelected;
	sGuiFill *	mpFillLocked;
} sGuiButtonStyle;

typedef	struct	sGuiButton
{
	sGuiInfo			mInfo;
	U16					mButtonType;
	U16					mSubType;

	struct sGuiSlider *	mpSlider;
	sGuiButtonStyle *	mpStyle;
	sGuiSprite			mSprite;
	sGuiString			mString;

	sGuiAction *		mpOnLeftClick;
	sGuiAction *		mpOnLeftHeld;
	sGuiAction *		mpOnLeftRelease;
	sGuiAction *		mpOnRightClick;
	sGuiAction *		mpOnRightHeld;
	sGuiAction *		mpOnRightRelease;
	sGuiAction *		mpOnFocus;
	sGuiAction *		mpOnDeFocus;
	sGuiVar *			mpOnIKBD;
} sGuiButton;


typedef	struct	sGuiSlider
{
	sGuiInfo			mInfo;
	sGuiVar *			mpPageSize;
	sGuiVar *			mpLineSize;
	sGuiButton *		mpButtons[ eGUI_SLIDERBUT_LIMIT ];
	struct sGuiWindow *		mpWindow;
	U16					mSliderType;
	S16					mSizeMin;
	S16					mButtonSize;
	U16					mPad;
} sGuiSlider;

typedef	struct	sGuiTable
{
	U16				mColCount;
	U16				mPad;
	sGuiVar**		mppVars;
	sGuiButton**	mppButtons;
} sGuiTable;

typedef	struct	sGuiWindow
{
	sGuiInfo			mInfo;
	sGuiRect			mCanvas;
	sGuiFontGroup *		mpFontGroup;
	sGuiButtonStyle *	mpButtonStyle;
	sGuiFill *			mpFill;
	sGuiVar *			mpOnIKBD;
	U16					mKeyActionCount;
	U16					mControlCount;
	sGuiKeyAction **	mppKeyActions;
	sGuiInfo **			mppControls;
} sGuiWindow;

typedef	struct	sGuiList
{
	sGuiInfo			mInfo;
	U16					mOpenFlag;
	U16					mPad;
	sGuiButton *		mpButton;
	sGuiSlider *		mpSlider;
	sGuiWindow *		mpWindow;
} sGuiList;



struct	sGuiData;
struct	sGuiTextLine;

typedef	void	(*fGuiFocus)( sGuiButton * apButton, const sGuiMouse * apMouse );
typedef	void	(*fGuiDeFocus)( sGuiButton * apButton );

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Gui_Init( sHashTree * apTree );
void			Gui_DeInit( void );
U8				Gui_Update( void );
void			Gui_DeInit( void );

void			Gui_DataAdd( struct sGuiData * apHeader );
void			Gui_DataRemove( struct sGuiData * apHeader );

void			Gui_WorldPosUpdate( void );
void			Gui_RequestWindowRefresh( sGuiWindow * apWindow );

void			Gui_SetCustomFocus( fGuiFocus apFocus );
void			Gui_SetCustomDeFocus( fGuiDeFocus apDeFocus );

void			GuiEventQueue_EventAdd( sGuiEvent * apEvent );

void			GuiButton_Select( sGuiButton * apButton );
void			GuiButton_DeSelect( sGuiButton * apButton );

U8				GuiLock_IsLocked( sGuiLock * apLock );
U8				GuiLock_IsVisible( sGuiLock * apLock );

sGuiWindow *	Gui_GetpWindowFromName( const char * apName );
sGuiMouse *		Gui_GetpMouse( void );
U16				Gui_GetWindowStackCount( void );
sGuiWindow **	Gui_GetpWindowStack( void );
sHashTree *		Gui_GetpHashTree( void );
struct sGuiTextLine *	Gui_GetpTextLine( void );


/* ################################################################################ */

#endif	/* INCLUDED_GUI_H */

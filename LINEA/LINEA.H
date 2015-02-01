#ifndef	INCLUDED_LINEA_H
#define	INCLUDED_LINEA_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

enum
{
	eLINEA_MWRITE_VDI = -1,
	eLINEA_MWRITE_XOR = 1
};

enum
{
	eLINEA_MOUSEINT_DISABLE = 0,
	eLINEA_MOUSEINT_ENABLE = 1,
};


/* ###################################################################################
#  STRUCTS
################################################################################### */


typedef	struct sLineAFont
{	
	S16						mID;
	S16						mPointSize;
	char					mName[ 32 ];
	U16						mCharFirst;
	U16						mCharLast;
	U16						mTop;
	U16						mAscent;
	U16						mHalf;
	U16						mDescent;
	U16						mBottom;
	U16						mCharWidthMax;
	U16						mCellWidthMax;
	U16						mOffsetLeft;
	U16						mOffsetRight;
	U16						mThicken;
	U16						mUnderLineSize;
	U16						mLighten;
	U16						mSkew;
	U16						mFlags;
	U8 *					mpHorOffsetTable;
	U16	*					mpCharOffsetTable;
	U8 *					mpData;
	U16						mFormWidth;
	U16						mFormHeight;
	struct	sLineAFont *	mpNext;
	U16						mReserved;
} sLineAFont;

typedef	struct sLineA
{
	S32				mReserved0;
	sLineAFont *	mpFontCurrent;
	U8				mReserved1[ 46 ];
	S16				mMouseHotSpotX;
	S16				mMouseHotSpotY;
	S16				mMouseWriteMode;
	S16				mMouseColourBG;
	S16				mMouseColourFG;
	U16				mMouseGfx[ 32 ];
	S16				mInqTable[ 45 ];
	S16				mDevTable[ 45 ];
	S16				mMouseX;
	S16				mMouseY;
	S16				mMouseHideCount;
	U16				mMouseButtonStatus;
	U16				mVdiPalette[ 48 ];
	S16				mSizeTable[ 15 ];
	U16				mReserved2;
	U16				mReserved3;
	U8 *			mpWork;
	sLineAFont *	mpFontDefault;
	sLineAFont *	mpFontRing[ 4 ];
	U16				mFontCount;
	U8				mReserved4[ 90 ];
	U8				mMouseStatusCurrent;
	U8				mReserved5;
	S16				mCursorHideCount;
	S16				mCursorX;
	S16				mCursorY;
	U8				mMouseRedrawFlag;
	U8				mMouseInterruptFlag;
	U32				mReserved6;
	S16				mCursorSavedX;
	S16				mCursorSavedY;
	S16				mHeightSaved;
	U16 *			mpScreenSaved;
	U16				mStatusSaved;
	U8				mMouseBufferSaved[ 256 ];
	void *			mpfOnTick;
	void *			mpfOnTickEnd;
	void *			mpfOnMouseButton;
	void *			mpfOnMouseRender;
	void *			mpfOnMouseMoved;
	S16				mTextCellHeight;
	S16				mTextColCount;
	S16				mTextRowCount;
	S16				mCellGap;
	S16				mTextCellColourBG;
	S16				mTextCellColourFG;
	U8 *			mpTextCursor;
	S16				mTextOffset;
	S16				mTextCursorX;
	S16				mTextCursorY;
	S8				mCursorBlinkRate;
	S8				mCursorBlinkCountDown;
	sLineAFont *	mpFontSystem;
	S16				mSystemFontCharLast;
	S16				mSystemFontCharFirst;
	S16				mSystemFontWidth;
	S16				mHorizontalRes;
	U16 *			mpFontOffsets;
	U16				mReserved7;
	S16				mVerticalRes;
	S16				mScreenLineSize;
	U16				mPlaneCount;
	U16				mWidth;
	S16 *			mpControl;
	S16 *			mpIntin;
	S16 *			mpPtsin;
	S16 *			mpIntout;
	S16 *			mpPtsout;
	S16				mColBit0;
	S16				mColBit1;
	S16				mColBit2;
	S16				mColBit3;
	S16				mLastPixelDrawFlag;
	S16				mLinePattern;
	S16				mVDIWriteMode;
	S16				mX1;
	S16				mY1;
	S16				mX2;
	S16				mY2;
	U16 *			mpFillPattern;
	U16				mPatternMask;
	U16				mMultiPlaneFillFlag;
	U16				mClippingFlag;
	S16				mClipLeft;
	S16				mClipRight;
	S16				mClipTop;
	S16				mClipBot;
	S16				mTextScaleDDA;
	S16				mTextScaleInc;
	U16				mTextScaleDir;
	U16				mMonoSpaceFontFlag;
	S16				mSourceX;
	S16				mSourceY;
	S16				mDestX;
	S16				mDestY;
	S16				mOutputCharWidth;
	S16				mOutputCharHeight;
	U16 *			mpFontImage;
	U16				mFontFormWidth;
	U16				mFontStyle;
	U16				mLightenMask;
	U16				mSkewMsk;
	U16				mThickenWidth;
	U16				mItalicRightOff;
	U16				mItalicLeftOff;
	U16				mTextScalingFlag;
	S16				mCharRotAngle;
	U16				mTextColourFG;
	U16 *			mpScratchBuffer;
	U16				mSratchBuffer2Offset;
	U16				mTextColourBG;
	U16				mRasterCopyMode;
	void *			mpfSeedFill;
} sLineA;

typedef	struct
{
	S16	mX0;
	S16	mY0;
	S16	mX1;
	S16	mY1;
	U16	mColour;
	U16	mLastLineFlag;
	U16	mLineMask;
	U16	mWriteMode;
} sLineALine;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sLineA *	LineA_Init( void );
void		LineA_PlotPixel( const S16 aX, const S16 aY, const U16 aColour);
U16			LineA_GetPixel(  const S16 aX, const S16 aY );
U16			LineA_ArbitaryLine( const sLineALine * apLine );
U16			LineA_HorizontalLine( const sLineALine * apLine );


/* ################################################################################ */

#endif	/* INCLUDED_LINEA_H */

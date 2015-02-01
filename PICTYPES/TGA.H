#ifndef	INCLUDED_TGA_H
#define	INCLUDED_TGA_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/PICTYPES/CANVAS.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8	mImageIDLength;
	U8	mColourMapType;
	U8	mImageType;
	U8	mColourMapFirstIndex[ 2 ];
	U8	mColourMapCount[ 2 ];
	U8	mColourMapBitDepth;
	U8	mOriginX[ 2 ];
	U8	mOriginY[ 2 ];
	U8	mWidth[ 2 ];
	U8	mHeight[ 2 ];
	U8	mPixelDepth;
	U8	mDescriptor;
} sTgaHeader;


typedef	struct
{
	U16	mTag;
	U32	mOffset;
	U32	mSize;
} sTgaDirectoryEntry;


typedef	struct
{
	U16	mTagCount;
} sTgaDirectoryHeader;


typedef	struct
{
	sTgaDirectoryHeader	mHeader;
	sTgaDirectoryEntry	mEntry[ 1 ];
} sTgaDirectory;


typedef	struct
{
	U16	mExtensionSize;
	U8	mAuthorName[ 41 ];
	U8	mAuthorComments[ 324 ];
	U16	mMonth;
	U16	mDay;
	U16	mYear;
	U16	mHour;
	U16	mMinute;
	U16	mSecond;
	U8	mJobName[ 41 ];
	U16	mJobHours;
	U16	mJobMinutes;
	U16	mJobSeconds;
	U8	mSoftwareID[ 41 ];
	U16	mVersionNumber;
	U8	mVersionLetter;
	U32	mKeyColour;
	U16	mPixelWidth;
	U16	mPixelHeight;
	U16	mGammaNumerator;
	U16	mGammaDenominator;
	U32	mColourCorectionTableOffset;
	U32	mPostageStampOffset;
	U32	mScanLineOffset;
	U8	mAlphaAttributes;
} sTgaExtension;


typedef	struct
{
	U32	mExtensionAreaOffset;
	U32	mDevloperDirectoryOffset;
	U8	mSignature[ 16 ];
	U8	mPeriod;
	U8	mZero;
} sTgaFooter;


typedef	struct
{
	U16	mA;
	U16	mR;
	U16	mG;
	U16	mB;
} sTgaColourCorrectionEntry;


typedef	struct
{
	sTgaColourCorrectionEntry	mColour[ 256 ];
} sTgaColourCorrectionTable;


typedef	struct
{
	sTgaHeader	mHeader;
	U8			mData[1];
} sTga;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sTga *		Tga_FromCanvas( sCanvas * apCanvas );
sCanvas *	Tga_ToCanvas( sTga * apTga );


/* ################################################################################ */

#endif	/*	INCLUDED_TGA_H */

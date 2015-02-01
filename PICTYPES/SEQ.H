#ifndef	INCLUDED_SEQ_H
#define	INCLUDED_SEQ_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16	mMagic;
	U16	mVersion;
	U16	mFrameCount;
	U16	mSpeed;
	U8	mReserved[ 118 ];
} sSeqHeader;


typedef	struct
{
	U16	mType;
	U16	mResolution;
	U16	mPalette[ 16 ];
	U8	mFileName[ 12 ];
	U16	mColourAnimLimits;
	U16	mColourAnimSpeedDir;
	U16	mColourSteps;
	U16	mX;
	U16	mY;
	U16	mWidth;
	U16	mHeight;
	U8	mOperation;
	U8	mCompressionFlag;
	U32	mDataLength;
	U8	mReserved[ 60 ];
} sSeqFrame;


/* ################################################################################ */

#endif	/*	INCLUDED_SEQ_H */
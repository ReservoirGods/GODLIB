#ifndef	INCLUDED_IFF_H
#define	INCLUDED_IFF_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dIFF_BMHD_ID		mSTRING_TO_U32( 'B', 'M', 'H', 'D' )
#define	dIFF_BODY_ID		mSTRING_TO_U32( 'B', 'O', 'D', 'Y' )
#define	dIFF_CAMG_ID		mSTRING_TO_U32( 'C', 'A', 'M', 'G' )
#define	dIFF_CMAP_ID		mSTRING_TO_U32( 'C', 'M', 'A', 'P' )
#define	dIFF_CRNG_ID		mSTRING_TO_U32( 'C', 'R', 'N', 'G' )
#define	dIFF_FORM_ID		mSTRING_TO_U32( 'F', 'O', 'R', 'M' )
#define	dIFF_ILBM_ID		mSTRING_TO_U32( 'I', 'L', 'B', 'M' )


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U32	mID;
	U32	mLength;
} sIffChunk;


typedef	struct
{
	U32	mID;
	U32	mLength;
	U32	mILBM;
} sIffFormChunk;


typedef	struct
{
	U32	mID;
	U32	mLength;
	U16	mWidth;
	U16	mHeight;
	U16	mX;
	U16	mY;
	U8	mPlaneCount;
	U8	mMask;
	U8	mCompressedFlag;
	U8	mReserved;
	U8	mTransparentColourIndex;
	U8	mAspectX;
	U8	mAspectY;
	U16	mPageWidth;
	U16	mPageHeight;
} sIffBmhdChunk;


typedef	struct
{
	U8	mR;
	U8	mG;
	U8	mB;
} sIffColour;


typedef	struct
{
	U32			mID;
	U32			mLength;
	sIffColour	mColour[ 1 ];
} sIffCmapChunk;


typedef	struct
{
	U32	mID;
	U32	mLength;
	U16	mReserved;
	U16	mAnimSpeed;
	U16	mActiveFlag;
	U8	mLeftBotColourAnimLimit;
	U8	mRightTopColourAnimLimit;
} sIffCrngChunk;


typedef	struct
{
	U32	mID;
	U32	mLength;
	U32	mViewportMode;
} sIffCamgChunk;


typedef	struct
{
	U32	mID;
	U32	mLength;
	U8	mPixels[ 1 ];
} sIffBodyChunk;


/* ################################################################################ */

#endif	/*	INCLUDED_IFF_H */
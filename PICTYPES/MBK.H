#ifndef	INCLUDED_MBK_H
#define	INCLUDED_MBK_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dMBK_HEADER_MAGIC	0x19861987 
#define	dMBK_PAL_MAGIC		0x50414C54


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16	mReserved[ 9 ];
	U32	mMagic;
	U32	mLowResOffset;
	U32	mMedResOffset;
	U32	mHighResOffset;
	U16	mLowResFrameCount;
	U16	mMedResFrameCount;
	U16	mHighResFrameCount;
} sMbkHeader;


typedef	struct
{
	U32	mDataOffset;
	U16	mWidth;
	U16	mHeight
	U16	mHotspotX;
	U16	mHotspotY;
} sMbkFrame;


typedef	struct
{
	U32	mMagic;
	U16	mPalette[ 16 ];
} sMbkPal;


/* ################################################################################ */

#endif	/*	INCLUDED_MBK_H */
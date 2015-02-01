#ifndef	INCLUDED_IMG_H
#define	INCLUDED_IMG_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U16	mVersion;
	U16	mHeaderLength;
	U16	mPlaneCount;
	U16	mPatternLength;
	U16	mPixelWidth;
	U16	mPixelHeight;
	U16	mWidth;
	U16	mHeight;
} sImgHeader;


/* ################################################################################ */

#endif	/*	INCLUDED_IMG_H */
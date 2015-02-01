#ifndef	INCLUDED_GIF_H
#define	INCLUDED_GIF_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/PICTYPES/CANVAS.H>
#include	<GODLIB/PICTYPES/CANVASIC.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	U8	mSig[ 3 ];
	U8	mVersion[ 3 ];
	U8	mWidth[ 2 ];
	U8	mHeight[ 2 ];
	U8	mFlags;
	U8	mBackgroundColourIndex;
	U8	mPixelAspectRatio;
} sGifHeader;

typedef	struct
{
	U8	mR;
	U8	mG;
	U8	mB;
} sGifColour;

typedef	struct
{
	U8	mSeperator;
	U8	mX[ 2 ];
	U8	mY[ 2 ];
	U8	mWidth[ 2 ];
	U8	mHeight[ 2 ];
	U8	mFlags;
} sGifImageDescriptor;

typedef	struct
{
	sGifHeader	mHeader;
	U8			mData[1];
} sGif;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sGif *		Gif_FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );
sCanvas *	Gif_ToCanvas( sGif * apGif );


/* ################################################################################ */

#endif	/*	INCLUDED_GIF_H */

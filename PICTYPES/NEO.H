#ifndef	INCLUDED_NEO_H
#define	INCLUDED_NEO_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>

#include	<GODLIB/PICTYPES/CANVAS.H>
#include	<GODLIB/PICTYPES/CANVASIC.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	U16	mFlag;
	U16	mResolution;
	U16	mPalette[ 16 ];
	U8	mFileName[ 12 ];
	U16	mColourAnimLimit;
	U16	mColourAnimSpeedDir;
	U16	mColourSteps;
	U16	mOffsetX;
	U16	mOffsetY;
	U16	mWidth;
	U16	mHeight;
	U16	mReserved[ 33 ];
} sNeoHeader;


typedef	struct
{
	sNeoHeader	mHeader;
	U16			mPixels[ 16000 ];
} sNeo;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sNeo * 		Neo_FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );
sCanvas *	Neo_ToCanvas(   sNeo * apNeo );


/* ################################################################################ */

#endif	/*	INCLUDED_NEO_H */

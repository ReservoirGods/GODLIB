#ifndef	INCLUDED_DEGAS_H
#define	INCLUDED_DEGAS_H

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
	U16	mMode;
	U16	mPalette[ 16 ];
} sDegasHeader;


typedef struct
{
	U16	mLeftColourAnimLimit[ 4 ];
	U16	mRightColourAnimLimit[ 4 ];
	U16	mAnimChannelDir[ 4 ];
	U16	mAnimChannelDelay[ 4 ];
} sDegasAnim;


typedef struct
{
	sDegasHeader	mHeader;
	U16				mPixels[ 16000 ];
} sDegas;


typedef struct
{
	sDegasHeader	mHeader;
	U16				mPixels[ 16000 ];
	sDegasAnim		mAnim;
} sDegasElite;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sCanvas *		Degas_ToCanvas(   sDegas * apDegas );
sDegas *		Degas_FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );

sCanvas *		Degas_Pi1ToCanvas( sDegas * apDegas );
sCanvas *		Degas_Pi2ToCanvas( sDegas * apDegas );
sCanvas *		Degas_Pi3ToCanvas( sDegas * apDegas );

sDegas *		Degas_Pi1FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );
sDegas *		Degas_Pi2FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );
sDegas *		Degas_Pi3FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette );

U8				Degas_GetPixel( const sDegas * apDegas, const S16 aX, const S16 aY );
void			Degas_SetPixel( sDegas * apDegas, const S16 aX, const S16 aY, const U8 aPixel );


/* ################################################################################ */

#endif	/*	INCLUDED_DEGAS_H */

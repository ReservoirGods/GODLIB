/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: NEO.C
::
:: Routines for manipulating neochrome neo images
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"NEO.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION    : Neo_ToCanvas( sNeo * apNeo )
* DESCRIPTION : converts a pi1 image to a canvas
* AUTHOR      : 24.12.00 PNK
*-----------------------------------------------------------------------------------*/

sCanvas *		Neo_ToCanvas( sNeo * apNeo )
{
	uCanvasPixel	lPal[ 16 ];
	sCanvas *		lpCanvas;

	lpCanvas = Canvas_Create();

	Canvas_PaletteFromST(   &lPal[ 0 ],  16, apNeo->mHeader.mPalette );
	lpCanvas = Canvas_ImageFrom4Plane( 320, 200, apNeo->mPixels, &lPal[ 0 ] );

	return( lpCanvas );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Neo_FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette )
* DESCRIPTION : converts a neo from canvas
* AUTHOR      : 27.12.00 PNK
*-----------------------------------------------------------------------------------*/

sNeo *		Neo_FromCanvas( sCanvas * apCanvas, uCanvasPixel * apPalette )
{
	sCanvasIC *	lpCanvasIC;
	sNeo *		lpNeo;

	lpNeo      = 0;
	lpCanvasIC = CanvasIC_FromCanvas( apCanvas, 16, apPalette );

	if( lpCanvasIC )
	{

		lpNeo = (sNeo*)mMEMCALLOC( sizeof(sNeo) );

		if( lpNeo )
		{
			lpNeo->mHeader.mWidth      = 320;
			lpNeo->mHeader.mHeight     = 200;
			lpNeo->mHeader.mResolution = 0;
			lpNeo->mHeader.mOffsetX    = 0;
			lpNeo->mHeader.mOffsetY    = 0;

			CanvasIC_PaletteToST( lpCanvasIC, 16, lpNeo->mHeader.mPalette );
			CanvasIC_To4Plane(    lpCanvasIC, 320, 200, lpNeo->mPixels );
		}
		CanvasIC_Destroy( lpCanvasIC );
	}
	return( lpNeo );
}


/* ################################################################################ */

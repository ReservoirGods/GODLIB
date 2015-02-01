/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: GOD.C
::
:: Routines for manipulating godpaint GOD images
::
:: [c] 2000 Reservoir Tgas
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"TGA.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  FUNCTIONS
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Tga_ToCanvas( sTga * apTga )
* DESCRIPTION : converts a tga image to a canvas
* AUTHOR      : 28.12.00 PNK
*-----------------------------------------------------------------------------------*/

void	Tga_DePack( U8 * apSrc, U8 * apDest, U16 aPacketSize, S32 aPixelCount )
{
	U32		lCode;
	U8 *	lpPacket;
	U16		i;

	while( aPixelCount > 0 )
	{
		lCode = *apSrc++;

		if( lCode & 0x80 )
		{
			lCode       &= 0x7F;
			lCode       += 1;
			lpPacket     = apSrc;
			apSrc       += lCode;
			aPixelCount -= lCode;

			while( lCode )
			{
				for( i=0; i<aPacketSize; i++ )
					*apDest++ = lpPacket[ i ];
				lCode--;
			}
		}
		else
		{
			lCode       &= 0x7F;
			lCode       += 1;
			lCode       *= aPacketSize;

			while( lCode )
			{
				*apDest++ = *apSrc++;
				lCode--;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Tga_ToCanvas( sTga * apTga )
* DESCRIPTION : converts a tga image to a canvas
* AUTHOR      : 28.12.00 PNK
*-----------------------------------------------------------------------------------*/

sCanvas *		Tga_ToCanvas( sTga * apTga )
{
	sCanvas *		lpCanvas;
	uCanvasPixel	lPixel;
	U32				lColourMapSize;
	U8 *			lpColourMap;
	U8 *			lpImage;
	uCanvasPixel *	lpPal;
	U16				lWidth,lHeight;
	U32				lIndex;
	S16				lStartX,lEndX,lStepX;
	S16				lStartY,lEndY,lStepY;
	S16				x,y;
	U8				lBPP;
	U8 *			lpImageDepack;

	lpCanvas = Canvas_Create();

	lpPal = 0;

	if( !lpCanvas )
	{
		return( 0 );
	}

	lpImageDepack = 0;

	lWidth  = EndianSwap_U16( apTga->mHeader.mWidth  );
	lHeight = EndianSwap_U16( apTga->mHeader.mHeight );

	if( !Canvas_CreateImage( lpCanvas, lWidth, lHeight ) )
	{
		return( 0 );
	}

	lpColourMap  = (U8*)apTga;
	lpColourMap += sizeof(sTgaHeader);
	lpColourMap += apTga->mHeader.mImageIDLength;
	lpImage      = lpColourMap;

/*-----------------------------------------------------------------------------------*
 *	create palette
 *---------------------------------------------------------------------------=-------*/

	if( apTga->mHeader.mColourMapType )
	{
		lColourMapSize  = (apTga->mHeader.mColourMapBitDepth+7)>>3;
		lColourMapSize *= EndianSwap_U16( apTga->mHeader.mColourMapCount );
		lpImage        += lColourMapSize;

		lpPal = (uCanvasPixel*)mMEMCALLOC( EndianSwap_U16( apTga->mHeader.mColourMapCount ) * sizeof(uCanvasPixel) );
		if( !lpPal )
		{
			return( 0 );
		}

		lStartX = EndianSwap_U16( apTga->mHeader.mColourMapFirstIndex );
		lEndX   = EndianSwap_U16( apTga->mHeader.mColourMapCount );

		switch( apTga->mHeader.mColourMapBitDepth )
		{

		case 15:
			for( x=lStartX; x<lEndX; x++ )
			{
				lPixel.b.a  = 0;
				lPixel.b.b  = (U8)((lpColourMap[ 0 ]<<3) & 0xF8);
				lPixel.b.g  = (U8)((lpColourMap[ 0 ]>>2) & 0x38);
				lPixel.b.g |= (U8)((lpColourMap[ 1 ]>>2) & 0xC0);
				lPixel.b.r  = (U8)((lpColourMap[ 1 ]<<1) & 0xF8);
				lpColourMap += 2;
				lpPal[ x ] = lPixel;
			}
			break;

		case 16:
			for( x=lStartX; x<lEndX; x++ )
			{
				lPixel.b.b  = (U8)((lpColourMap[ 0 ]<<3) & 0xF8);
				lPixel.b.g  = (U8)((lpColourMap[ 0 ]>>2) & 0x38);
				lPixel.b.g |= (U8)((lpColourMap[ 1 ]>>2) & 0xC0);
				lPixel.b.r  = (U8)((lpColourMap[ 1 ]<<1) & 0xF8);
				lPixel.b.a  = (U8)((lpColourMap[ 1 ]>>7) & 1);
				lpPal[ x ] = lPixel;
				lpColourMap += 2;
			}
			break;

		case 24:
			for( x=lStartX; x<lEndX; x++ )
			{
				lPixel.b.b  = *lpColourMap++;
				lPixel.b.g  = *lpColourMap++;
				lPixel.b.r  = *lpColourMap++;
				lpPal[ x ] = lPixel;
			}
			break;

		case 32:
			for( x=lStartX; x<lEndX; x++ )
			{
				lPixel.b.b  = *lpColourMap++;
				lPixel.b.g  = *lpColourMap++;
				lPixel.b.r  = *lpColourMap++;
				lPixel.b.a  = *lpColourMap++;
				lpPal[ x ] = lPixel;
			}
			break;
		}
	}


/*-----------------------------------------------------------------------------------*
 *	determine orientation of image
 *---------------------------------------------------------------------------=-------*/

	if( apTga->mHeader.mDescriptor & 0x10 )
	{
		lStartX = (S16)(lWidth-1);
		lEndX   = -1;
		lStepX  = -1;
	}
	else
	{
		lStartX = 0;
		lEndX   = lWidth;
		lStepX  = 1;
	}

	if( apTga->mHeader.mDescriptor & 0x20 )
	{
		lStartY = 0;
		lEndY   = lHeight;
		lStepY  = 1;
	}
	else
	{
		lStartY = (S16)(lHeight-1);
		lEndY   = -1;
		lStepY  = -1;
	}

	lIndex = 0;
	lBPP   = apTga->mHeader.mPixelDepth;
	lBPP  += 7;
	lBPP >>= 3;

	switch( apTga->mHeader.mImageType )
	{

/*-----------------------------------------------------------------------------------*
 *	uncompressed indexed colour image
 *---------------------------------------------------------------------------=-------*/

	case	1:
		if( !lpPal )
		{
			return( 0 );
		}
		switch( lBPP )
		{
		case 1:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lIndex = *lpImage++;
					Canvas_SetPixel( lpCanvas, x, y, lpPal[ lIndex ].l );
				}
			}
			break;

		case 2:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lIndex = EndianSwap_U16( lpImage );
					lpImage += 2;
					Canvas_SetPixel( lpCanvas, x, y, lpPal[ lIndex ].l );
				}
			}
			break;

		case 3:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lIndex = EndianSwap_U24( lpImage );
					lpImage += 3;
					Canvas_SetPixel( lpCanvas, x, y, lpPal[ lIndex ].l );
				}
			}
			break;

		case 4:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lIndex = EndianSwap_U32( lpImage );
					lpImage += 4;
					Canvas_SetPixel( lpCanvas, x, y, lpPal[ lIndex ].l );
				}
			}
			break;

		}
		break;

/*-----------------------------------------------------------------------------------*
 *	uncompressed rgba image
 *---------------------------------------------------------------------------=-------*/

	case	2:

		switch( apTga->mHeader.mPixelDepth )
		{
		case 15:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lPixel.b.a  = 0;
					lPixel.b.b  = (U8)((lpImage[ 0 ]<<3) & 0xF8);
					lPixel.b.g  = (U8)((lpImage[ 0 ]>>2) & 0x38);
					lPixel.b.g |= (U8)((lpImage[ 1 ]>>2) & 0xC0);
					lPixel.b.r  = (U8)((lpImage[ 1 ]<<1) & 0xF8);
					lpImage += 2;
					Canvas_SetPixel( lpCanvas, x, y, lPixel.l );
				}
			}
			break;

		case 16:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lPixel.b.b  = (U8)((lpImage[ 0 ]<<3) & 0xF8);
					lPixel.b.g  = (U8)((lpImage[ 0 ]>>2) & 0x38);
					lPixel.b.g |= (U8)((lpImage[ 1 ]>>2) & 0xC0);
					lPixel.b.r  = (U8)((lpImage[ 1 ]<<1) & 0xF8);
					lPixel.b.a  = (U8)((lpImage[ 1 ]>>7) & 1);
					lpImage += 2;
					Canvas_SetPixel( lpCanvas, x, y, lPixel.l );
				}
			}
			break;

		case 24:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lPixel.b.b  = *lpImage++;
					lPixel.b.g  = *lpImage++;
					lPixel.b.r  = *lpImage++;
					Canvas_SetPixel( lpCanvas, x, y, lPixel.l );
				}
			}
			break;

		case 32:
			for( y=lStartY; y!=lEndY; y=(S16)(y+lStepY) )
			{
				for( x=lStartX; x!=lEndX; x=(S16)(x+lStepX) )
				{
					lPixel.b.b  = *lpImage++;
					lPixel.b.g  = *lpImage++;
					lPixel.b.r  = *lpImage++;
					lPixel.b.a  = *lpImage++;
					Canvas_SetPixel( lpCanvas, x, y, lPixel.l );
				}
			}
			break;

		break;

		}

	}

	if( lpImageDepack )
	{
		mMEMFREE( lpImageDepack );
	}

	return( lpCanvas );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Tga_FromCanvas( sCanvas * apCanvas )
* DESCRIPTION : converts to a tga file from canvas
* AUTHOR      : 28.12.00 PNK
*-----------------------------------------------------------------------------------*/

sTga *		Tga_FromCanvas( sCanvas * apCanvas )
{
	sTga *			lpTga;
	U32				lSize;
	uCanvasPixel *	lpSrc;
	U8 *			lpDst;

	lSize   = apCanvas->mWidth;
	lSize  *= apCanvas->mHeight;
	lSize <<= 2;
	lSize  += sizeof(sTgaHeader);

	lpTga = (sTga*)mMEMCALLOC( lSize );

	if( lpTga )
	{
		lpTga->mHeader.mColourMapBitDepth        = 0;
		lpTga->mHeader.mColourMapCount[ 0 ]      = 0;
		lpTga->mHeader.mColourMapCount[ 1 ]      = 0;
		lpTga->mHeader.mColourMapFirstIndex[ 0 ] = 0;
		lpTga->mHeader.mColourMapFirstIndex[ 1 ] = 0;
		lpTga->mHeader.mDescriptor               = 32;
		lpTga->mHeader.mHeight[ 0 ]              = (U8)(apCanvas->mHeight & 0xFF);
		lpTga->mHeader.mHeight[ 1 ]              = (U8)((apCanvas->mHeight>>8) & 0xFF);
		lpTga->mHeader.mImageIDLength            = 0;
		lpTga->mHeader.mImageType                = 2;
		lpTga->mHeader.mOriginX[ 0 ]             = 0;
		lpTga->mHeader.mOriginX[ 1 ]             = 0;
		lpTga->mHeader.mOriginY[ 0 ]             = 0;
		lpTga->mHeader.mOriginY[ 1 ]             = 0;
		lpTga->mHeader.mPixelDepth               = 32;
		lpTga->mHeader.mWidth[ 0 ]               = (U8)(apCanvas->mWidth & 0xFF);
		lpTga->mHeader.mWidth[ 1 ]               = (U8)((apCanvas->mWidth>>8) & 0xFF);

		lSize   = apCanvas->mWidth;
		lSize  *= apCanvas->mHeight;

		if( lSize )
		{
			lSize--;
			lpDst = &lpTga->mData[ 0 ];
			lpSrc = apCanvas->mpPixels;
			while( lSize )
			{
				*lpDst++ = lpSrc->b.b;
				*lpDst++ = lpSrc->b.g;
				*lpDst++ = lpSrc->b.r;
				*lpDst++ = lpSrc->b.a;
				lSize--;
				lpSrc++;
			}
		}
	}

	return( lpTga );
}


U32		Tga_Pack( U8 * apSrc, U8 * apDest )
{
	(void)apSrc;
	(void)apDest;
	return(0);
}


/* ################################################################################ */

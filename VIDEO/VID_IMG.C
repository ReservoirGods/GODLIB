#ifndef	dGODLIB_SYSTEM_TOS

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"VID_IMG.H"
#include	"VIDEO.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sVideoImageClass
{
	U16	mPalOriginalST[ 16 ];
	U32	mPalOriginalFalcon[ 256 ];
	U32	mPalConvST[ 16 ];
	U32	mPalConvFalcon[ 256 ];
} sVideoImageClass;


/* ###################################################################################
#  DATA
################################################################################### */

sVideoImageClass	gVideoImageClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_Init( void )
* ACTION   : Video_Image_Init
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_Init( void )
{
	Memory_Clear( sizeof(sVideoImageClass), &gVideoImageClass );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_DeInit( void )
* ACTION   : Video_Image_DeInit
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_DeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_ConvPalST( const U16 aFormat )
* ACTION   : Video_Image_ConvPalST
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_ConvPalST( const U16 aFormat )
{
	U16	i;
	U32	lCol;
	U32	lR,lG,lB;

	for( i=0; i<16; i++ )
	{
		lCol = gVideoImageClass.mPalOriginalST[ i ];
		lR   = (lCol>>0) & 7;
		lG   = (lCol>>12) & 7;
		lB   = (lCol>>8) & 7;
		lR <<= 1;
		lG <<= 1;
		lB <<= 1;
		lR  |= (lCol>>3)&1;
		lG  |= (lCol>>15)&1;
		lB  |= (lCol>>11)&1;
		switch( aFormat )
		{
		case	eVIDEO_IMAGE_FMT_R8G8B8:
		case	eVIDEO_IMAGE_FMT_A8R8G8B8:
		case	eVIDEO_IMAGE_FMT_X8R8G8B8:
				gVideoImageClass.mPalConvST[ i ] = (lR<<20) | (lG<<12) | (lB<<4);
				break;
		case	eVIDEO_IMAGE_FMT_R5G6B5:
				gVideoImageClass.mPalConvST[ i ] = (lR<<12) | (lG<<7) | (lB<<1);
				break;
		case	eVIDEO_IMAGE_FMT_X1R5G5B5:
		case	eVIDEO_IMAGE_FMT_A1R5G5B5:
				gVideoImageClass.mPalConvST[ i ] = (lR<<12) | (lG<<6) | (lB<<1);
				break;
		case	eVIDEO_IMAGE_FMT_A4R4G4B4:
		case	eVIDEO_IMAGE_FMT_X4R4G4B4:
				gVideoImageClass.mPalConvST[ i ] = (lR<<8) | (lG<<4) | (lB);
				break;
		case	eVIDEO_IMAGE_FMT_R3G3B2:
		case	eVIDEO_IMAGE_FMT_A8R3G3B2:
				gVideoImageClass.mPalConvST[ i ] = ((lR<<1)<<5) | ((lG>>1)<<2) | (lB>>2);
				break;
		case	eVIDEO_IMAGE_FMT_A2B10G10R10:
				gVideoImageClass.mPalConvST[ i ] = (lR<<26) | (lG<<16) | (lB<<6);
		case	eVIDEO_IMAGE_FMT_G16R16:
				gVideoImageClass.mPalConvST[ i ] = (lR<<12) | (lG<<28);
				break;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_ConvPalFalcon( const U16 aFormat )
* ACTION   : Video_Image_ConvPalFalcon
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_ConvPalFalcon( const U16 aFormat )
{
	(void)aFormat;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_SetPalFalcon( const U32 * apPal )
* ACTION   : Video_SetPalFalcon
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_SetPalFalcon( const U32 * apPal )
{
	U16	i;

	if( apPal )
	{
		for( i=0; i<256; i++ )
		{
			gVideoImageClass.mPalOriginalFalcon[ i ] = apPal[ i ];
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_GetPalFalcon( U32 * apPal )
* ACTION   : Video_GetPalFalcon
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_GetPalFalcon( U32 * apPal )
{
	U16	i;

	if( apPal )
	{
		for( i=0; i<256; i++ )
		{
			apPal[ i ] = gVideoImageClass.mPalOriginalFalcon[ i ];
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_4BP_ToU8( const U8 * apSrc,U8 *  apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
* ACTION   : Video_Image_4BP_ToU8
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_4BP_ToU8( const U8 * apSrc,U8 *  apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U8)( apPal[ lColour ] & 0xFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += aDstPitch;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_4BP_ToU16( const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
* ACTION   : Video_Image_4BP_ToU16
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_4BP_ToU16( const U8 * apSrc,U16 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
{
	U16 *		lpDst0;
	U16 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U16)( apPal[ lColour ] & 0xFFFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += (aDstPitch>>1);
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_4BP_ToU24( const U8 * apSrc,U8 *  apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
* ACTION   : Video_Image_4BP_ToU24
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_4BP_ToU24( const U8 * apSrc,U8 *  apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
{
	U8 *		lpDst0;
	U8 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = (U8)( (apPal[ lColour ]>>16) & 0xFF );
			*lpDst1++ = (U8)( (apPal[ lColour ]>>8) & 0xFF );
			*lpDst1++ = (U8)( (apPal[ lColour ]>>0) & 0xFF );
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += aDstPitch;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Video_Image_4BP_ToU32( const U8 * apSrc,U32 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
* ACTION   : Video_Image_4BP_ToU32
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Video_Image_4BP_ToU32( const U8 * apSrc,U32 * apDst,const U16 aWidth,const U16 aHeight,const U32 aSrcPitch,const U32 aDstPitch,U32 * apPal )
{
	U32 *		lpDst0;
	U32 *		lpDst1;
	const U8 *	lpSrc0;
	const U8 *	lpSrc1;
	U16			lColour;
	U16			lH,lW;
	U8 *		lpArray;
	U16			lPlanes[ 4 ];
	U16			lMask;

	lpSrc0 = apSrc;
	lpDst0 = apDst;
	lH     = aHeight;

	while( lH-- )
	{
		lMask     = 0;
		lpSrc1    = lpSrc0;
		lpDst1    = lpDst0;
		lW        = aWidth;

		while( lW-- )
		{
			if( !lMask )
			{
				lpArray      = (U8*)&lPlanes[ 0 ];

				lpArray[ 1 ] = *lpSrc1++;
				lpArray[ 0 ] = *lpSrc1++;

				lpArray[ 3 ] = *lpSrc1++;
				lpArray[ 2 ] = *lpSrc1++;

				lpArray[ 5 ] = *lpSrc1++;
				lpArray[ 4 ] = *lpSrc1++;

				lpArray[ 7 ] = *lpSrc1++;
				lpArray[ 6 ] = *lpSrc1++;

				lMask        = 0x8000;
			}

			lColour = 0;
			if( lPlanes[ 3 ] & lMask )
			{
				lColour |= 8;
			}
			if( lPlanes[ 2 ] & lMask )
			{
				lColour |= 4;
			}
			if( lPlanes[ 1 ] & lMask )
			{
				lColour |= 2;
			}
			if( lPlanes[ 0 ] & lMask )
			{
				lColour |= 1;
			}

			lMask >>= 1;

			*lpDst1++ = apPal[ lColour ];
		}
		lpSrc0 += aSrcPitch;
		lpDst0 += (aDstPitch>>2);
	}
}


/* ################################################################################ */

#endif

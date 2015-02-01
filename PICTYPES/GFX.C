/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: GFX.C
::
:: Routines for manipulating art director GFX images
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GFX.H"

#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/FILE/FILE.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGFX_VERSION	1
#define	dGFX_ID			mSTRING_TO_U32( 'G', 'F', 'X', ' ' )


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Gfx_Create( U16 aWidth, U16 aHeight, U16 aPlaneCount, U32 aLineSize, U16 * apSrc )
* DESCRIPTION : creates a GFX image
* AUTHOR      : 31.12.00 PNK
*-----------------------------------------------------------------------------------*/

sGfx *		Gfx_Create( U16 aWidth, U16 aHeight, U8 aPlaneCount, U32 aLineSize, U16 * apSrc )
{
	U32		lSize;
	U16 *	lpSrc;
	U16 *	lpPixels;
	sGfx *	lpGfx;
	U16		x,y,i;
	U16		lMask;

	lSize  = ((aWidth+15)>>4) << 1;
	lSize *= aPlaneCount + 1;
	lSize *= aHeight;
	lSize += sizeof(sGfxHeader);

	lpGfx = (sGfx*)mMEMALLOC( lSize );

	if( lpGfx )
	{
		lpGfx->mHeader.mHeight      = aHeight;
		lpGfx->mHeader.mWidth       = aWidth;
		lpGfx->mHeader.mPlaneCount  = aPlaneCount;
		lpGfx->mHeader.mVersion     = dGFX_VERSION;
		lpGfx->mHeader.mID          = dGFX_ID;
		lpGfx->mHeader.mHasMaskFlag = 1;

		lpPixels = &lpGfx->mPixels[ 0 ];

		for( y=0; y<aHeight; y++ )
		{

			lpSrc = &apSrc[ (aLineSize*y)>>1 ];

			for( x=0; x<((aWidth+15)>>4); x++ )
			{
				lMask  = (U16)(~lpSrc[ 0 ]);
				for( i=1; i<aPlaneCount; i++ )
				{
					lMask &= ~lpSrc[ i ];
				}

				*lpPixels++ = lMask;
				for( i=0; i<aPlaneCount; i++ )
					*lpPixels++ = *lpSrc++;

			}
		}
	}

	return( lpGfx );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Gfx_Delete( sGfx * apGfx )
* DESCRIPTION : deletes a GFX image
* AUTHOR      : 31.12.00 PNK
*-----------------------------------------------------------------------------------*/

void		Gfx_Delete( sGfx * apGfx )
{
	mMEMFREE( apGfx );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Gfx_Save( sGfx * apGfx, char * apFileName )
* DESCRIPTION : saves a GFX image
* AUTHOR      : 31.12.00 PNK
*-----------------------------------------------------------------------------------*/

U8			Gfx_Save( sGfx * apGfx, char * apFileName )
{
	U32	lSize;

	lSize  = ((apGfx->mHeader.mWidth+15)>>4) << 1;
	lSize *= apGfx->mHeader.mPlaneCount + 1;
	lSize *= apGfx->mHeader.mHeight;
	lSize += sizeof(sGfxHeader);

	return( File_Save( apFileName, apGfx, lSize ) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : Gfx_UnMaskInteriorBlack( sGfx * apGfx )
* DESCRIPTION : masks out interior points
* AUTHOR      : 02.01.00 PNK
*-----------------------------------------------------------------------------------*/

void		Gfx_UnMaskInteriorBlack( sGfx * apGfx )
{
	U16		x,y;
	U16 *	lpMask;

	if( apGfx )
	{
		lpMask = apGfx->mPixels;
		for( y=0; y < apGfx->mHeader.mHeight; y++ )
		{
			for( x=0; x < ((apGfx->mHeader.mWidth+15)>>4); x++ )
			{
				*lpMask &= 0;
				lpMask += (apGfx->mHeader.mPlaneCount+1);
			}
		}
	}
}

/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: COLQUANT.C
::
:: Routines for colour quantizing images
::
:: [c] 2002 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"COLQUANT.H"

#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/PICTYPES/OCTTREE.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct
{
	uCanvasPixel	mPixel;
	U32				mCount;
} sCQCounter;

typedef	struct
{
	U32					mTotalCount;
	U32					mActiveCount;
} sCQHeader;

typedef	struct
{
	sCQHeader	mHeader;
	sCQCounter	mCounter[1];
} sCQContainer;


typedef	struct
{
	U32				mColourCount;
	U32				mMaxDepth;
	sCQContainer *	mpOldPal;
	U32				mRGBCounters[ 3 ][ 256 ];
	uCanvasPixel	mPalette[ 256 ];
} sCQMedianContainer;


typedef	struct
{
	U32				mRedSum;
	U32				mGreenSum;
	U32				mBlueSum;
	U32				mCount;
} sCQLKMColour;


/* ###################################################################################
#  DATA
################################################################################### */

U16	gColourQuantizeMethod     = eCOLOURQUANTIZE_MEDIANCUT;
U32	gColourQuantizeVariance   = 1024;
U32	gColourQuantizeIterations = 1024;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sCQContainer *	ColourQuantize_ContainerInit( const U32 aCount );
void			ColourQuantize_ContainerAdd( sCQContainer * apContainer, const U32 aPixel );
void			ColourQuantize_ContainerSort( sCQContainer * apContainer );
U16				ColourQuantize_GetIndex( const U32 aPixel, uCanvasPixel * apPal, const U16 aColourCount );

void			ColourQuantize_MedianSubDivide( sCQMedianContainer * apContainer, U32 aRGBmin, U32 aRGBmax, U32 aDepth );


/* ###################################################################################
#  FUNCTIONS
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_FromCanvas( sCanvas * apCanvas, const U16 aColourCount )
* DESCRIPTION : quantizes canvas image
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_FromCanvas( sCanvas * apCanvas, const U16 aColourCount )
{
	sCanvasIC *	lpCanvasIC;

	switch( gColourQuantizeMethod )
	{

	case	eCOLOURQUANTIZE_BITSHIFT:
		lpCanvasIC = ColourQuantize_BitShift( apCanvas, aColourCount );
		break;

	case	eCOLOURQUANTIZE_POPULAR:
		lpCanvasIC = ColourQuantize_Popular( apCanvas, aColourCount );
		break;

	case	eCOLOURQUANTIZE_MEDIANCUT:
		lpCanvasIC = ColourQuantize_MedianCut( apCanvas, aColourCount );
		break;

	case	eCOLOURQUANTIZE_LKM:
		lpCanvasIC = ColourQuantize_LKM( apCanvas, aColourCount );
		break;

	case	eCOLOURQUANTIZE_OCTREE:
		lpCanvasIC = ColourQuantize_Octree( apCanvas, aColourCount );
		break;

	default:
		lpCanvasIC = 0;
		break;
	}

	return( lpCanvasIC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_BitShift(  sCanvas * apCanvas, const U16 aColourCount )
* DESCRIPTION : quantizes images
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_BitShift(  sCanvas * apCanvas, const U16 aColourCount )
{
	sCanvasIC *		lpCanvasIC;
	uCanvasPixel *	lpSrc;
	U8 *			lpDst;
	U16				x,y;

	lpCanvasIC = CanvasIC_Create();

	if( lpCanvasIC )
	{
		if( CanvasIC_CreateImage( lpCanvasIC, apCanvas->mWidth, apCanvas->mHeight ) )
		{

			lpSrc = apCanvas->mpPixels;
			lpDst = lpCanvasIC->mpPixels;

			switch( aColourCount )
			{

			case	2:
				lpCanvasIC->mPalette[ 0 ].l = 0x00000000;
				lpCanvasIC->mPalette[ 1 ].l = 0xFFFFFFFF;

				for( y=0; y<apCanvas->mHeight; y++ )
				{
					for( x=0; x<apCanvas->mWidth; x++ )
					{
						*lpDst = (U8)(((lpSrc->b.r>>7) | (lpSrc->b.g>>7) | (lpSrc->b.b>>7))&1);
						lpDst++;
						lpSrc++;
					}
				}

				break;

			case	4:
				lpCanvasIC->mPalette[ 0 ].l = 0x00000000;
				lpCanvasIC->mPalette[ 1 ].l = 0x55555555;
				lpCanvasIC->mPalette[ 2 ].l = 0xAAAAAAAA;
				lpCanvasIC->mPalette[ 1 ].l = 0xFFFFFFFF;

				for( y=0; y<apCanvas->mHeight; y++ )
				{
					for( x=0; x<apCanvas->mWidth; x++ )
					{
						*lpDst = (U8)(((lpSrc->b.r>>7)&1) + ((lpSrc->b.g>>7)&1) + ((lpSrc->b.b>>7)&1));
						lpDst++;
						lpSrc++;
					}
				}
				break;

			case	16:

				for( x=0; x<256; x++ )
				{
					lpCanvasIC->mPalette[ x ].b.r = (U8)((((x>>3)&1)<<7));
					lpCanvasIC->mPalette[ x ].b.g = (U8)((((x>>1)&3)<<6));
					lpCanvasIC->mPalette[ x ].b.b = (U8)(((x&1)<<7));
				}

				for( y=0; y<apCanvas->mHeight; y++ )
				{
					for( x=0; x<apCanvas->mWidth; x++ )
					{
						*lpDst = (U8)((((lpSrc->b.r>>7)&1)<<3) | (((lpSrc->b.g>>6)&3)<<1) | ((lpSrc->b.b>>7)&1));
						lpDst++;
						lpSrc++;
					}
				}
				break;

			case	256:

				for( x=0; x<256; x++ )
				{
					lpCanvasIC->mPalette[ x ].b.r = (U8)((((x>>5)&7)<<5));
					lpCanvasIC->mPalette[ x ].b.g = (U8)((((x>>2)&7)<<5));
					lpCanvasIC->mPalette[ x ].b.b = (U8)(((x&3)<<6));
				}

				for( y=0; y<apCanvas->mHeight; y++ )
				{
					for( x=0; x<apCanvas->mWidth; x++ )
					{
						*lpDst = (U8)((((lpSrc->b.r>>5)&3)<<5) | (((lpSrc->b.g>>5)&3)<<2) | ((lpSrc->b.b>>6)&3));
						lpDst++;
						lpSrc++;
					}
				}
				break;
			}
		}
		else
		{
			CanvasIC_Destroy( lpCanvasIC );
			lpCanvasIC = 0;
		}
	}

	return( lpCanvasIC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_Popular(   sCanvas * apCanvas, const U16 aColourCount );
* DESCRIPTION : quantizes images
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_Popular(   sCanvas * apCanvas, const U16 aColourCount )
{
	sCQContainer *	lpContainer;
	sCanvasIC *		lpCanvasIC;
	uCanvasPixel *	lpSrc;
	U8 *			lpDst;
	U32				lSize;
	U32				i;

	lSize  = apCanvas->mWidth;
	lSize *= apCanvas->mHeight;

	lpCanvasIC  = 0;
	lpContainer = ColourQuantize_ContainerInit( lSize );
	
	if( lpContainer )
	{
		lpCanvasIC = CanvasIC_Create();

		if( lpCanvasIC )
		{
			if( CanvasIC_CreateImage( lpCanvasIC, apCanvas->mWidth, apCanvas->mHeight ) )
			{
				lpSrc = apCanvas->mpPixels;
				for( i=0; i<lSize; i++ )
				{
					ColourQuantize_ContainerAdd( lpContainer, lpSrc->l );
					lpSrc++;
				}
				ColourQuantize_ContainerSort( lpContainer );

				for( i=0; i<aColourCount; i++ )
				{
					lpCanvasIC->mPalette[ i ] = lpContainer->mCounter[ i ].mPixel;
				}

				lpSrc = apCanvas->mpPixels;
				lpDst = lpCanvasIC->mpPixels;
				for( i=0; i<lSize; i++ )
				{
					*lpDst = (U8)ColourQuantize_GetIndex( lpSrc->l, &lpCanvasIC->mPalette[0], aColourCount );
					lpSrc++;
					lpDst++;
				}
				ColourQuantize_ContainerSort( lpContainer );

			}
		}

		mMEMFREE( lpContainer );
	}

	return( lpCanvasIC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_MedianSubDivide( sCQMedianContainer * apContainer, U32 aRGBmin, U32 aRGBmax, U32 aDepth )
* DESCRIPTION : performs median cut
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

void			ColourQuantize_MedianSubDivide( sCQMedianContainer * apContainer, U32 aRGBmin, U32 aRGBmax, U32 aDepth )
{
	uCanvasPixel	lStart0;
	uCanvasPixel	lEnd0;
	uCanvasPixel	lStart1;
	uCanvasPixel	lEnd1;
	sCQCounter *	lpEntry;
	S16				lDiff[ 3 ];
	U16				lDiffIndex;
	U32				lActiveColours;
	U32				lMedian;
	U32				lPos;
	S32				i;

	lStart0.l = aRGBmin;
	lEnd0.l   = aRGBmax;

	lStart1.l = aRGBmin;
	lEnd1.l   = aRGBmax;

	if( aDepth == apContainer->mMaxDepth )
	{
		lStart1.b.r = (U8)((lStart0.b.r + lEnd0.b.r)>>1);
		lStart1.b.g = (U8)((lStart0.b.g + lEnd0.b.g)>>1);
		lStart1.b.b = (U8)((lStart0.b.b + lEnd0.b.b)>>1);

		if( apContainer->mColourCount < 256 )
		{
			apContainer->mPalette[ apContainer->mColourCount ] = lStart1;
			apContainer->mColourCount++;
		}
	}
	else
	{
		for( i=0; i<256; i++ )
		{
			apContainer->mRGBCounters[ 0 ][ i ] = 0L;
			apContainer->mRGBCounters[ 1 ][ i ] = 0L;
			apContainer->mRGBCounters[ 2 ][ i ] = 0L;
		}

		lpEntry = &apContainer->mpOldPal->mCounter[ 0 ];
		i = apContainer->mpOldPal->mHeader.mActiveCount;


		lActiveColours = 0;
		while( i>0 )
		{
			if( (lpEntry->mPixel.b.r >= lStart0.b.r) && (lpEntry->mPixel.b.r <= lEnd0.b.r) )
			{
				if( (lpEntry->mPixel.b.g >= lStart0.b.g) && (lpEntry->mPixel.b.g <= lEnd0.b.g) )
				{
					if( (lpEntry->mPixel.b.b >= lStart0.b.b) && (lpEntry->mPixel.b.b <= lEnd0.b.b) )
					{
						apContainer->mRGBCounters[ 0 ][ lpEntry->mPixel.b.r ] += lpEntry->mCount;
						apContainer->mRGBCounters[ 1 ][ lpEntry->mPixel.b.g ] += lpEntry->mCount;
						apContainer->mRGBCounters[ 2 ][ lpEntry->mPixel.b.b ] += lpEntry->mCount;
						lActiveColours++;
					}
				}
			}
			lpEntry++;
			i--;
		}

		if( lActiveColours <= 1 )
		{
			lStart1.b.r = (U8)((lStart0.b.r + lEnd0.b.r)>>1);
			lStart1.b.g = (U8)((lStart0.b.g + lEnd0.b.g)>>1);
			lStart1.b.b = (U8)((lStart0.b.b + lEnd0.b.b)>>1);

			if( apContainer->mColourCount < 256 )
			{
				apContainer->mPalette[ apContainer->mColourCount ] = lStart1;
				apContainer->mColourCount++;
			}
		}
		else
		{
			lDiff[ 0 ] = (S16)(lEnd0.b.r - lStart0.b.r);
			lDiff[ 1 ] = (S16)(lEnd0.b.g - lStart0.b.g);
			lDiff[ 2 ] = (S16)(lEnd0.b.b - lStart0.b.b);

			for( i=0; i<3; i++ )
			{
				if( lDiff[ i ] < 0 )
				{
					lDiff[ i ] = (S16)(0 - lDiff[ i ]);
				}
			}

			lDiffIndex = 0;
			if( lDiff[ 1 ] > lDiff[ 0 ] )
			{
				lDiffIndex = 1;
			}
			if( lDiff[ 2 ] > lDiff[ lDiffIndex ] )
			{
				lDiffIndex = 2;
			}


			lPos = 0;
			lMedian = lActiveColours>>1;
			if( !lMedian )
			{
				lMedian = 1;
			}
			i = 256;
			do
			{
				i--;
				lPos += apContainer->mRGBCounters[ lDiffIndex ][ i ];
			} while( (i>=0) && (lPos < lMedian ) );



			switch( lDiffIndex )
			{
				case 0:
					lEnd0.b.r   = (U8)i;
					lStart1.b.r = (U8)i;
					break;
				case 1:
					lEnd0.b.g   = (U8)i;
					lStart1.b.g = (U8)i;
					break;
				case 2:
					lEnd0.b.b   = (U8)i;
					lStart1.b.b = (U8)i;
					break;
			}

			ColourQuantize_MedianSubDivide( apContainer, lStart0.l, lEnd0.l, aDepth+1 );
			ColourQuantize_MedianSubDivide( apContainer, lStart1.l, lEnd1.l, aDepth+1 );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_MedianCut( sCanvas * apCanvas, const U16 aColourCount )
* DESCRIPTION : quantizes images
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_MedianCut( sCanvas * apCanvas, const U16 aColourCount )
{
	sCanvasIC *			lpCanvasIC;
	sCQMedianContainer	lContainer;
	uCanvasPixel *		lpSrc;
	U32					lSize;
	U32					i;


	lSize  = apCanvas->mWidth;
	lSize *= apCanvas->mHeight;

	lpCanvasIC = 0;

	lContainer.mpOldPal =  ColourQuantize_ContainerInit( lSize );

	if( lContainer.mpOldPal )
	{

		lpSrc = apCanvas->mpPixels;
		for( i=0; i<lSize; i++ )
		{
			ColourQuantize_ContainerAdd( lContainer.mpOldPal, lpSrc->l );
			lpSrc++;
		}

		ColourQuantize_MedianSubDivide( &lContainer, 0L, 0xFFFFFFFFL, 0 );

		mMEMFREE( lContainer.mpOldPal );

		lpCanvasIC = CanvasIC_Create();

		if( lpCanvasIC )
		{
			if( CanvasIC_CreateImage( lpCanvasIC, apCanvas->mWidth, apCanvas->mHeight ) )
			{
				for( i=0; i<aColourCount; i++ )
				{
					lpCanvasIC->mPalette[ i ] = lContainer.mPalette[ i ];
				}
			}
		}
		
	}


	return( lpCanvasIC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_BitShift(  sCanvas * apCanvas, const U16 aColourCount )
* DESCRIPTION : quantizes images
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_LKM(       sCanvas * apCanvas, const U16 aColourCount )
{
	sCQContainer *	lpContainer;
	sCanvasIC *		lpCanvasIC;
	uCanvasPixel *	lpSrc;
	uCanvasPixel *	lpPal;
	sCQLKMColour *	lpColours;
	U8 *			lpDst;
	U32				lVariance;
	U32				lIterations;
	U32				lSize;
	U32				i;
	U32				lRandomIndex;
	S32				lDiff;
	U16				lIndex;

	lSize  = apCanvas->mWidth;
	lSize *= apCanvas->mHeight;

	lpCanvasIC  = 0;
	lpContainer = ColourQuantize_ContainerInit( lSize );
	
	if( lpContainer )
	{
		lpCanvasIC = CanvasIC_Create();

		if( lpCanvasIC )
		{
			if( CanvasIC_CreateImage( lpCanvasIC, apCanvas->mWidth, apCanvas->mHeight ) )
			{
				lpSrc = apCanvas->mpPixels;
				for( i=0; i<lSize; i++ )
				{
					ColourQuantize_ContainerAdd( lpContainer, lpSrc->l );
					lpSrc++;
				}

				lSize  = aColourCount;
				lSize *= sizeof( sCQLKMColour );

				lpColours = (sCQLKMColour*)mMEMCALLOC( lSize );

				if( lpColours )
				{
					lSize  = aColourCount;
					lSize *= sizeof( uCanvasPixel );

					lpPal = (uCanvasPixel*)mMEMCALLOC( lSize );
					
					if( lpPal )
					{
						for( i=0; i<aColourCount; i++ )
						{
							lpPal[ i ] = lpContainer->mCounter[ i ].mPixel;
						}

						lSize  = apCanvas->mWidth;
						lSize *= apCanvas->mHeight;

						lIterations  = 0;
						lRandomIndex = 0;
						do
						{
							for( i=0; i<aColourCount; i++ )
							{
								lpColours[ i ].mCount    = 0;
								lpColours[ i ].mBlueSum  = 0;
								lpColours[ i ].mGreenSum = 0;
								lpColours[ i ].mRedSum   = 0;
							}

							for( i=0; i<lpContainer->mHeader.mActiveCount; i++ )
							{
								lIndex = ColourQuantize_GetIndex( lpContainer->mCounter[ i ].mPixel.l, lpPal, aColourCount );
								lpColours[ lIndex ].mCount++;
								lpColours[ lIndex ].mRedSum   += lpContainer->mCounter[ i ].mPixel.b.r;
								lpColours[ lIndex ].mGreenSum += lpContainer->mCounter[ i ].mPixel.b.g;
								lpColours[ lIndex ].mBlueSum  += lpContainer->mCounter[ i ].mPixel.b.b;
							}

							lVariance = 0;
							for( i=0; i<aColourCount; i++ )
							{
								if( !lpColours[ i ].mCount )
								{
									lpColours[ i ].mRedSum   = lpContainer->mCounter[ lRandomIndex ].mPixel.b.r;
									lpColours[ i ].mGreenSum = lpContainer->mCounter[ lRandomIndex ].mPixel.b.g;
									lpColours[ i ].mBlueSum  = lpContainer->mCounter[ lRandomIndex ].mPixel.b.b;
									lRandomIndex++;
									if( lRandomIndex > lpContainer->mHeader.mActiveCount )
									{
										lRandomIndex = 0;
									}
								}
								else
								{
									lpColours[ i ].mRedSum   /= lpColours[ i ].mCount;
									lpColours[ i ].mGreenSum /= lpColours[ i ].mCount;
									lpColours[ i ].mBlueSum  /= lpColours[ i ].mCount;
								}

								lDiff= (lpColours[ i ].mRedSum - lpPal[ i ].b.r);
								if( lDiff < 0 )
								{
									lDiff = -lDiff;
								}
								lVariance += lDiff;

								lDiff= (lpColours[ i ].mGreenSum - lpPal[ i ].b.g);
								if( lDiff < 0 )
								{
									lDiff = -lDiff;
								}
								lVariance += lDiff;
								lDiff= (lpColours[ i ].mBlueSum - lpPal[ i ].b.b);
								if( lDiff < 0 )
								{
									lDiff = -lDiff;
								}
								lVariance += lDiff;

								lpPal[ i ].b.r = (U8)lpColours[ i ].mRedSum;
								lpPal[ i ].b.g = (U8)lpColours[ i ].mGreenSum;
								lpPal[ i ].b.b = (U8)lpColours[ i ].mBlueSum;

							}

							lIterations++;
						} while( (lVariance < gColourQuantizeVariance) && (lIterations < gColourQuantizeIterations) );


						lpSrc = apCanvas->mpPixels;
						lpDst = lpCanvasIC->mpPixels;
						for( i=0; i<lSize; i++ )
						{
							*lpDst = (U8)ColourQuantize_GetIndex( lpSrc->l, &lpCanvasIC->mPalette[0], aColourCount );
							lpSrc++;
							lpDst++;
						}
						ColourQuantize_ContainerSort( lpContainer );

						mMEMFREE( lpPal );
					}
					mMEMFREE( lpColours );
				}

			}
		}

		mMEMFREE( lpContainer );
	}

	return( lpCanvasIC );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_BitShift(  sCanvas * apCanvas, const U16 aColourCount )
* DESCRIPTION : quantizes images
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCanvasIC *		ColourQuantize_Octree(    sCanvas * apCanvas, const U16 aColourCount )
{
	(void)apCanvas;
	(void)aColourCount;
	return( 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_ContainerInit( const U32 aCount )
* DESCRIPTION : inits a container
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

sCQContainer * ColourQuantize_ContainerInit( const U32 aCount )
{
	sCQContainer *	lpContainer;
	U32				lSize;

	lSize  = aCount;
	lSize *= sizeof( sCQCounter );
	lSize += sizeof( sCQHeader  );
	
	lpContainer = (sCQContainer*)mMEMCALLOC( lSize );

	if( lpContainer )
	{
		lpContainer->mHeader.mActiveCount = 0;
		lpContainer->mHeader.mTotalCount  = 0;
	}

	return( lpContainer );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_ContainerAdd( sCQContainer * apContainer, const U32 aPixel )
* DESCRIPTION : adds a colour to a container
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

void	 ColourQuantize_ContainerAdd( sCQContainer * apContainer, const U32 aPixel )
{
	U32	i;

	for( i=0; i<apContainer->mHeader.mActiveCount; i++ )
	{
		if( apContainer->mCounter[ i ].mPixel.l == aPixel )
		{
			apContainer->mCounter[ i ].mCount++;
			return;
		}
	}
	apContainer->mCounter[ apContainer->mHeader.mActiveCount ].mPixel.l = aPixel;
	apContainer->mCounter[ apContainer->mHeader.mActiveCount ].mCount   = 1;
	apContainer->mHeader.mActiveCount++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_ContainerSort( sCQContainer * apContainer )
* DESCRIPTION : sorts a container
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

void			ColourQuantize_ContainerSort( sCQContainer * apContainer )
{
	U8	lSwitchFlag;
	U32			lGap;
	S32			i;
	sCQCounter	lTemp;

	lSwitchFlag = 0;
	lGap        = apContainer->mHeader.mActiveCount;

	while( (lGap>1) || (lSwitchFlag) )
	{
		lGap *= 10L;
		lGap /= 13L;

		if( lGap < 1 )
		{
			lGap = 1;
		}

		if( (lGap==9) || (lGap==10) )
		{
			lGap = 11;
		}

		lSwitchFlag = 0;

		for( i=1; i<(S32)(apContainer->mHeader.mActiveCount-lGap); i++ )
		{
			if( apContainer->mCounter[ i ].mCount > apContainer->mCounter[ i+lGap ].mCount )
			{
				lTemp = apContainer->mCounter[ i ];
				apContainer->mCounter[ i      ] = apContainer->mCounter[ i+lGap ];
				apContainer->mCounter[ i+lGap ] = lTemp;
				lSwitchFlag = 1;
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_GetIndex( const U32 aPixel, uCanvasPixel * apPal, const U16 aColourCount )
* DESCRIPTION : returns index of colour in a palette
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

U16				ColourQuantize_GetIndex( const U32 aPixel, uCanvasPixel * apPal, const U16 aColourCount )
{
	U32				lIndex;
	S16				lDiffR;
	S16				lDiffG;
	S16				lDiffB;
	S16				lBestDiff;
	U16				i;
	uCanvasPixel *	lpPal;
	uCanvasPixel	lPixel;

	lBestDiff = 0x7FFF;
	lPixel.l  = aPixel;
	lIndex    = 0;
	lpPal     = apPal;

	for( i=0; i<aColourCount; i++ )
	{
		lDiffR = (S16)(lpPal->b.r - lPixel.b.r);
		lDiffG = (S16)(lpPal->b.g - lPixel.b.g);
		lDiffB = (S16)(lpPal->b.b - lPixel.b.b);

		if( lDiffR < 0 )
		{
			lDiffR = (S16)(-lDiffR);
		}
		if( lDiffG < 0 )
		{
			lDiffG = (S16)(-lDiffG);
		}
		if( lDiffB < 0 )
		{
			lDiffB = (S16)(-lDiffB);
		}

		lDiffR = (S16)(lDiffR+lDiffG);
		lDiffR = (S16)(lDiffR+lDiffB);

		if( 0 == lDiffR )
		{
			return( i );
		}
		else if( lDiffR < lBestDiff )
		{
			lIndex    = i;
			lBestDiff = lDiffR;
		}

		lpPal++;
	}


	return( (U16)(lIndex) );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_SetMethod( const U16 aMethod )
* DESCRIPTION : sets quantization method
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

void			ColourQuantize_SetMethod( const U16 aMethod )
{
	gColourQuantizeMethod = aMethod;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION    : ColourQuantize_GetMethod( void )
* DESCRIPTION : returns quantization method
* AUTHOR      : 12.05.02 PNK
*-----------------------------------------------------------------------------------*/

U16				ColourQuantize_GetMethod( void )
{
	return( gColourQuantizeMethod );
}

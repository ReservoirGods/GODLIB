/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"RSPRITE.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eRLESPRITE_OUTPUT_NONE,
	eRLESPRITE_OUTPUT_SKIP,
	eRLESPRITE_OUTPUT_DRAW,
};


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dRLESPRITEBLOCK_ID		mSTRING_TO_U32( 'R', 'S', 'B', 'K' )
#define	dRLESPRITEBLOCK_VERSION	0


/* ###################################################################################
#  DATA
################################################################################### */


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16 *	RleSprite_SkipOutput( U16 * apRle, U16 aSkipCount );
U16 *	RleSprite_PixelsOutput( U16 * apRle, const U16 * apPixels, U16 aPixelCount );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_SkipOutput( U16 * apRle,U16 aSkipCount )
* ACTION   : RleSprite_SkipOutput
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	RleSprite_SkipOutput( U16 * apRle,U16 aSkipCount )
{
	if( apRle )
	{
		Endian_WriteBigU16( apRle, aSkipCount );
		apRle++;
	}
	return( apRle );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_PixelsOutput( U16 * apRle,U16 * apPixels,U16 aPixelCount )
* ACTION   : RleSprite_PixelsOutput
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	RleSprite_PixelsOutput( U16 * apRle,const U16 * apPixels,U16 aPixelCount )
{
	if( apRle )
	{
		Endian_WriteBigU16( apRle, aPixelCount );
		apRle++;
		while( aPixelCount )
		{
			*apRle++ = *apPixels++;
			aPixelCount--;
		}
	}
	return( apRle );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Create( const U16 * apGfx,const sSpriteRect * apRect,const U16 aGfxWidth,const U16 aOpaqueColour,const U8 aOpaqueFlag )
* ACTION   : RleSprite_Create
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sRleSprite *	RleSprite_Create( const U16 * apGfx,const sSpriteRect * apRect,const U16 aGfxWidth,const U16 aOpaqueColour,const U8 aOpaqueFlag )
{
	U32				lSize;
	sRleSprite *	lpSprite;
	const U16 *		lpGfx;
	U16 *			lpRle;
	const U16 *		lpPixels;
	U16				lSkipCount;
	U16				lDrawCount;
	U16				lPixel;
	S16				lX,lY;

	lpSprite = 0;

	if( apGfx && apRect->mWidth && apRect->mHeight )
	{
		lpGfx  = apGfx;
		lpGfx += (aGfxWidth * apRect->mY);
		lpGfx += (apRect->mX);

		lSize  = apRect->mWidth;
		lSize += 3;
		lSize *= 4;
		lSize *= apRect->mHeight;
		lSize += sizeof(sRleSpriteHeader);

		lpSprite = (sRleSprite*)mMEMCALLOC( lSize );

		if( lpSprite )
		{
			lpSprite->mHeader.mHeight = apRect->mHeight;
			lpSprite->mHeader.mWidth  = apRect->mWidth;

			lpRle  = (U16*)lpSprite;
			lpRle +=(sizeof(sRleSpriteHeader)>>1);
			lpRle +=((apRect->mHeight+1)<<1);

			for( lY=0; lY<apRect->mHeight; lY++ )
			{
				lpPixels   = lpGfx;
				lSkipCount = 0;
				lDrawCount = 0;

				lpSprite->mpLines[ lY ] = lpRle;

				if( aOpaqueFlag )
				{
					lpRle = RleSprite_SkipOutput(   lpRle, 0                     );
					lpRle = RleSprite_PixelsOutput( lpRle, lpGfx, apRect->mWidth );
				}
				else
				{
					for( lX=0; lX<apRect->mWidth; lX++ )
					{
						lSize  = lY;
						lSize *= aGfxWidth;
						lSize += lX;
						Endian_ReadBigU16( &lpGfx[ lSize ], lPixel );

						if( lPixel == aOpaqueColour )
						{
							if( lDrawCount )
							{
								lpRle = RleSprite_SkipOutput(   lpRle, lSkipCount            );
								lpRle = RleSprite_PixelsOutput( lpRle, lpPixels,  lDrawCount );
								lDrawCount  = 0;
								lSkipCount  = 0;
							}
							lSkipCount++;
						}
						else
						{
							if( !lDrawCount )
							{
								lpPixels   =&lpGfx[ lSize ];
							}
							lDrawCount++;
						}
					}
				}

				lpRle = RleSprite_SkipOutput(   lpRle, lSkipCount            );
				lpRle = RleSprite_PixelsOutput( lpRle, lpPixels,  lDrawCount );
				if( lDrawCount )
				{
					lpRle = RleSprite_SkipOutput(   lpRle, 0 );
					lpRle = RleSprite_PixelsOutput( lpRle, lpPixels, 0 );
				}
			}
			lpSprite->mpLines[ apRect->mHeight ] = lpRle;
		}
	}

	return( lpSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Destroy( sRleSprite * apSprite )
* ACTION   : RleSprite_Destroy
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_Destroy( sRleSprite * apSprite )
{
	if( apSprite )
	{
		mMEMFREE( apSprite );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_GetSize( const sRleSprite * apSprite )
* ACTION   : RleSprite_GetSize
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	RleSprite_GetSize( const sRleSprite * apSprite )
{
	U32	lSize;
	
	lSize = 0;
	if( apSprite )
	{
		lSize  = sizeof(sRleSpriteHeader);
		lSize += (U32)((apSprite->mHeader.mHeight+1) *4L );
		lSize += ( (U32)(apSprite->mpLines[apSprite->mHeader.mHeight]) - (U32)(apSprite->mpLines[0]) );
	}
	return( lSize );
}	


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Delocate( sRleSprite * apSprite )
* ACTION   : RleSprite_Delocate
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_Delocate( sRleSprite * apSprite )
{
	U16	i;

	if( apSprite )
	{
		for( i=0; i<=apSprite->mHeader.mHeight; i++ )
		{
			*(U32*)&apSprite->mpLines[ i ] -= (U32)apSprite;
			Endian_FromBigU32( &apSprite->mpLines[ i ] );
		}
		Endian_FromBigU16( &apSprite->mHeader.mHeight );
		Endian_FromBigU16( &apSprite->mHeader.mWidth  );
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Relocate( sRleSprite * apSprite )
* ACTION   : RleSprite_Relocate
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_Relocate( sRleSprite * apSprite )
{
	U16	i;

	if( apSprite )
	{
		Endian_FromBigU16( &apSprite->mHeader.mHeight );
		Endian_FromBigU16( &apSprite->mHeader.mWidth  );
		for( i=0; i<=apSprite->mHeader.mHeight; i++ )
		{
			Endian_FromBigU32( &apSprite->mpLines[ i ] );
			*(U32*)&apSprite->mpLines[ i ] += (U32)apSprite;
		}
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_Create( const U16 aSpriteCount )
* ACTION   : RleSpriteBlock_Create
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sRleSpriteBlock *	RleSpriteBlock_Create( const U16 aSpriteCount )
{
	sRleSpriteBlock *	lpBlock;
	U32					lSize;

	lSize = aSpriteCount;
	lSize *= 4;
	lSize += sizeof(sRleSpriteBlockHeader);

	lpBlock = (sRleSpriteBlock*)mMEMCALLOC( lSize );

	if( lpBlock )
	{
		lpBlock->mHeader.mID          = dRLESPRITEBLOCK_ID;
		lpBlock->mHeader.mVersion     = dRLESPRITEBLOCK_VERSION;
		lpBlock->mHeader.mSpriteCount = aSpriteCount;
	}
	return( lpBlock );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_Destroy( sRleSpriteBlock * apBlock )
* ACTION   : RleSpriteBlock_Destroy
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSpriteBlock_Destroy( sRleSpriteBlock * apBlock )
{
	mMEMFREE( apBlock );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_Serialise( const sRleSpriteBlock * apBlock )
* ACTION   : RleSpriteBlock_Serialise
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sRleSpriteBlock *	RleSpriteBlock_Serialise( const sRleSpriteBlock * apBlock )
{
	U32					lSize;
	U32					lOff;
	sRleSpriteBlock *	lpBlock;
	sRleSprite *		lpSprite;
	U8 *				lpMem;
	U16					i;

	lpBlock = 0;
	if( apBlock )
	{
		lSize   = RleSpriteBlock_GetSize( apBlock );
		lpMem   = (U8*)mMEMCALLOC( lSize );
		if( lpMem )
		{
			lpBlock          = (sRleSpriteBlock*)lpMem;
			lOff             = 0;
			lpBlock->mHeader = apBlock->mHeader;
			lOff             = apBlock->mHeader.mSpriteCount;
			lOff            *= 4L;
			lOff            += sizeof(sRleSpriteBlockHeader);
			for( i=0; i<lpBlock->mHeader.mSpriteCount; i++ )
			{
				lpSprite                = (sRleSprite*)&lpMem[ lOff ];
				lpBlock->mpSprites[ i ] = lpSprite;

				lSize = RleSprite_GetSize( apBlock->mpSprites[ i ] );
				RleSprite_Delocate( apBlock->mpSprites[ i ] );
				Memory_Copy( lSize, apBlock->mpSprites[ i ], lpBlock->mpSprites[ i ] );
				RleSprite_Relocate( apBlock->mpSprites[ i ] );
				RleSprite_Relocate( lpBlock->mpSprites[ i ] );
				lOff += lSize;
			}
		}
	}

	return( lpBlock );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_Delocate( sRleSpriteBlock * apBlock )
* ACTION   : RleSpriteBlock_Delocate
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSpriteBlock_Delocate( sRleSpriteBlock * apBlock )
{
	U16	i;

	if( apBlock )
	{
		for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
		{
			RleSprite_Delocate( apBlock->mpSprites[ i ] );
			*(U32*)&apBlock->mpSprites[ i ] -= (U32)apBlock;
			Endian_FromBigU32( &apBlock->mpSprites[ i ] );
		}
		Endian_FromBigU32( &apBlock->mHeader.mID          );
		Endian_FromBigU16( &apBlock->mHeader.mVersion     );
		Endian_FromBigU16( &apBlock->mHeader.mSpriteCount );
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_Relocate( sRleSpriteBlock * apBlock )
* ACTION   : RleSpriteBlock_Relocate
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSpriteBlock_Relocate( sRleSpriteBlock * apBlock )
{
	U16	i;

	if( apBlock )
	{
		Endian_FromBigU32( &apBlock->mHeader.mID          );
		Endian_FromBigU16( &apBlock->mHeader.mVersion     );
		Endian_FromBigU16( &apBlock->mHeader.mSpriteCount );
		for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
		{
			Endian_FromBigU32( &apBlock->mpSprites[ i ] );
			*(U32*)&apBlock->mpSprites[ i ] += (U32)apBlock;
			RleSprite_Relocate( apBlock->mpSprites[ i ] );
		}
	}		
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSpriteBlock_GetSize( const sRleSpriteBlock * apBlock )
* ACTION   : RleSpriteBlock_GetSize
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	RleSpriteBlock_GetSize( const sRleSpriteBlock * apBlock )
{
	U32	lSize;
	U16	i;
	
	lSize = 0;
	if( apBlock )
	{
		lSize  = apBlock->mHeader.mSpriteCount;
		lSize *= 4L;
		lSize += sizeof(sRleSpriteBlockHeader);
		for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
		{
			lSize += RleSprite_GetSize( apBlock->mpSprites[ i ] );
		}
	}
	return( lSize );
}

#ifndef	dGODLIB_PLATFORM_ATARI

/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Draw( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
* ACTION   : RleSprite_Draw
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_Draw( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
{
	U16 *	lpDst0;	
	U16 *	lpDst1;	
	U16 *	lpRle;
	U16		lH;
	U16		lSkip;
	U16		lDraw;

	lpDst0 = apScreen;
	lH     = apSprite->mHeader.mHeight;
	lpRle  = apSprite->mpLines[ 0 ];
	while( lH )
	{
		lpDst1 = lpDst0;

		do
		{
			Endian_ReadBigU16( lpRle, lSkip );
			lpRle++;
			lpDst1 += lSkip;

			Endian_ReadBigU16( lpRle, lDraw );
			lpRle++;
			lSkip   = lDraw;
			while( lSkip )
			{
				*lpDst1++ = *lpRle++;
				lSkip--;
			}
		} while( lDraw );

		lpDst0 += aScreenWidth;
		lH--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_DrawClip( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth,const sSpriteRect * apRect )
* ACTION   : RleSprite_DrawClip
* CREATION : 16.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_DrawClip( const sRleSprite * apSprite,U16 * apScreen,U32 aScreenWidth,const sSpriteRect * apRect )
{
	U16 *	lpDst0;	
	U16 *	lpDst1;	
	U16 *	lpRle;
	U16		lH;
	U16		lX;
	U16		lX1;
	U16		lPixel;
	U16		lSkip;
	U16		lDraw;

	lpDst0 = apScreen;
	lH     = apRect->mHeight;
	lX1    = (U16)(apRect->mX + apRect->mWidth);
	lpRle  = apSprite->mpLines[ apRect->mY ];
	while( lH )
	{
		lpDst1 = lpDst0;
		lX     = 0;
		do
		{
			Endian_ReadBigU16( lpRle, lSkip );
			lpRle++;
			lX      = (U16)(lX + lSkip);

			Endian_ReadBigU16( lpRle, lDraw );
			lpRle++;
			lSkip   = lDraw;
			while( lSkip )
			{
				lPixel = *lpRle++;
				if( ((S16)lX >= apRect->mX) && (lX<lX1) )
				{
					lpDst1[ lX - apRect->mX ] = lPixel;
				}
				lX++;
				lSkip--;
			}
		} while( lDraw );

		lpDst0 += aScreenWidth;
		lH--;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : RleSprite_Clear( const sRleSprite * apSprite,const U16 * apSrc,U16 * apDst,U32 aScreenWidth )
* ACTION   : RleSprite_Clear
* CREATION : 17.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	RleSprite_Clear( const sRleSprite * apSprite,const U16 * apSrc,U16 * apDst,U32 aScreenWidth )
{
	U16 *		lpDst0;	
	U16 *		lpDst1;	
	const U16 *	lpSrc0;	
	const U16 *	lpSrc1;	
	U16 *		lpRle;
	U16			lH;
	U16			lSkip;
	U16			lDraw;

	lpDst0 = apDst;
	lpSrc0 = apSrc;
	lH     = apSprite->mHeader.mHeight;
	lpRle  = apSprite->mpLines[ 0 ];
	while( lH )
	{
		lpDst1 = lpDst0;
		lpSrc1 = lpSrc0;

		do
		{
			Endian_ReadBigU16( lpRle, lSkip );
			lpRle++;
			lpDst1 += lSkip;
			lpSrc1 += lSkip;

			Endian_ReadBigU16( lpRle, lDraw );
			lpRle++;
			lSkip  = lDraw;
			lpRle += lDraw;
			while( lSkip )
			{
				*lpDst1++ = *lpSrc1++;
				lSkip--;
			}
		} while( lDraw );

		lpDst0 += aScreenWidth;
		lpSrc0 += aScreenWidth;
		lH--;
	}
	
}


#endif

/* ################################################################################ */

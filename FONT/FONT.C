/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FONT.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dFONT_ID			mSTRING_TO_U32( 'F', 'O', 'N', 'T' )
#define	dFONT_VERSION	0
#define	dFONT_KERNING	2


/* ###################################################################################
#  PROTOTYPES
################################################################################### */


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_Create( const sSpriteBlock * apBlock,const U8 * apCharMap,const U16 aFixedWidth )
* ACTION   : Font_Create
* CREATION : 17.03.2005 PNK
*-----------------------------------------------------------------------------------*/

sFont *	Font_Create( const sSpriteBlock * apBlock,const U8 * apCharMapString,const U16 aFixedWidth )
{
	U8			lSpriteUsedFlags[ 256 ];
	U8			lCharMap[ 256 ];
	sFont *		lpFont;
	sSprite *	lpSprite;
	U8 *		lpMem;
	U32			lBytes;
	U32			lOff;
	U32			lSize;
	S32			lCharCount;
	U16			lSpriteCount;
	U16			i,j;
	U8			lCharFirst;
	U8			lCharLast;

	lpFont       = 0;
	if( apBlock )
	{
		lSpriteCount = 0;
		lCharFirst   = 0xFF;
		lCharLast    = 0;

		for( i=0; i<256; i++ )
		{
			lSpriteUsedFlags[ i ] = 0;
			lCharMap[ i ] = 0xFF;
		}

		i = 0;
		while( ((!i) || (apCharMapString[ i ])) && (i<256) )
		{
			lCharMap[ apCharMapString[ i ] ] = (U8)i;
			i++;
		}

		for( i=0; i<256; i++ )
		{
			if( lCharMap[ i ] < apBlock->mHeader.mSpriteCount )
			{
				if( !lSpriteUsedFlags[ lCharMap[i] ] )
				{
					lSpriteUsedFlags[ lCharMap[i] ] = 1;
					lSpriteCount++;
				}
				if( i < lCharFirst )
				{
					lCharFirst = (U8)i;
				}
				if( i > lCharLast )
				{
					lCharLast = (U8)i;
				}
			}
		}


		lCharCount = (lCharLast - lCharFirst) + 1;
		if( lCharCount > 0 )
		{
			lSize  = lCharCount;
			lSize += lSpriteCount * Sprite_GetSize( apBlock->mpSprite[ 0 ] );
			lSize += lSpriteCount * sizeof(sSpriteRegion);
			lSize += sizeof( sFont );


			lpFont = (sFont*)mMEMCALLOC( lSize );
			if( lpFont )
			{
				lpFont->mID          = dFONT_ID;
				lpFont->mVersion     = dFONT_VERSION;
				lpFont->mCharFirst   = lCharFirst;
				lpFont->mCharLast    = lCharLast;
				lpFont->mSpriteCount = 0;
				lpFont->mKerning     = dFONT_KERNING;
				lpFont->mSpaceWidth  = 0;

				lpMem              = (U8*)lpFont;
				lOff               = sizeof( sFont );

				for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
				{
					if( lSpriteUsedFlags[ i ] )
					{
						lpFont->mSpriteCount++;
					}
				}

				lpFont->mpSprites  = (sSprite*)&lpMem[ lOff ];
				lOff              += lpFont->mSpriteCount * sizeof(sSprite);

				lpFont->mpRegions  = (sSpriteRegion*)&lpMem[ lOff ];
				lOff              += lpFont->mSpriteCount * sizeof(sSpriteRegion);

				j = 0;
				for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
				{
					if( lSpriteUsedFlags[ i ] )
					{
						lpSprite  = &lpFont->mpSprites[ j ];

						lpSprite->mGfxPlaneCount  = apBlock->mpSprite[ i ]->mGfxPlaneCount;
						lpSprite->mHeight         = apBlock->mpSprite[ i ]->mHeight;
						lpSprite->mMaskPlaneCount = apBlock->mpSprite[ i ]->mMaskPlaneCount;
						lpSprite->mWidth          = apBlock->mpSprite[ i ]->mWidth;

						lpSprite->mpMask          = (U16*)&lpMem[ lOff ];
						lBytes                    = Sprite_GetMskSize( apBlock->mpSprite[ i ] );
						Memory_Copy( lBytes, apBlock->mpSprite[ i ]->mpMask, lpSprite->mpMask );
						lOff                     += lBytes;

						lpSprite->mpGfx           = (U16*)&lpMem[ lOff ];
						lBytes                    = Sprite_GetGfxSize( apBlock->mpSprite[ i ] );
						Memory_Copy( lBytes, apBlock->mpSprite[ i ]->mpGfx, lpSprite->mpGfx );
						lOff                     += lBytes;

						if( aFixedWidth )
						{
							lpFont->mpRegions[ j ].mX0 = 0;
							lpFont->mpRegions[ j ].mY0 = 0;
							lpFont->mpRegions[ j ].mX1 = (U16)(aFixedWidth - 1);
							lpFont->mpRegions[ j ].mY1 = (U16)(apBlock->mpSprite[ 0 ]->mHeight - 1);
						}
						else
						{
							Sprite_GetRegion( lpSprite, &lpFont->mpRegions[ j ] );
						}
						j++;

					}
				}

				lpFont->mWidthMax = 0;
				lpFont->mHeightMax = 0;
				for( i=0; i<lpFont->mSpriteCount; i++ )
				{
					j = (U16)(lpFont->mpRegions[ i ].mX1 - lpFont->mpRegions[ i ].mX0);
					if( j > lpFont->mWidthMax )
					{
						lpFont->mWidthMax = j;
					}

					j = (U16)(lpFont->mpRegions[ i ].mY1 - lpFont->mpRegions[ i ].mY0);
					if( j > lpFont->mHeightMax )
					{
						lpFont->mHeightMax = j;
					}

				}

				lpFont->mpCharMap = &lpMem[ lOff ];
				i = lCharFirst;
				while( i <= lCharLast )
				{
					lpMem[ lOff ] = lCharMap[ i ];
					lOff++;
					i++;
				}
			}
		}
	}
	return( lpFont );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_Destroy( sFont * apFont )
* ACTION   : Font_Destroy
* CREATION : 20.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Font_Destroy( sFont * apFont )
{
	mMEMFREE( apFont );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetStringWidth( const sFont * apFont,const char * apString )
* ACTION   : Font_GetStringWidth
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Font_GetStringWidth( const sFont * apFont,const char * apString )
{
	U16	lWidth;

	lWidth = 0;
	while( *apString )
	{
		if( lWidth )
		{
			lWidth = (U16)(lWidth + apFont->mKerning);
		}
		lWidth = (U16)(lWidth + Font_GetCharWidth( apFont, *apString ));
		apString++;
	}
	return( lWidth );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetStringCharX( const sFont * apFont,const char * apString,const U16 aIndex )
* ACTION   : Font_GetStringCharX
* CREATION : 19.02.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Font_GetStringCharX( const sFont * apFont,const char * apString,const U16 aIndex )
{
	U16	lWidth;
	U16	i;

	lWidth = 0;
	i      = 0;
	while( (*apString) && (i<aIndex) )
	{
		if( lWidth )
		{
			lWidth = (U16)(lWidth +apFont->mKerning);
		}
		lWidth = (U16)(lWidth + Font_GetCharWidth( apFont, *apString ));
		apString++;
		i++;
	}
	return( lWidth );

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetCharWidth( const sFont * apFont,const char aChar )
* ACTION   : Font_GetCharWidth
* CREATION : 20.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Font_GetCharWidth( const sFont * apFont,const U8 aChar )
{
	U16	lWidth;
	U16	lIndex;

	lWidth = 0;
	if( apFont )
	{
		if( ' ' == aChar )
		{
			lWidth = apFont->mSpaceWidth;
		}
		else if( (aChar>=apFont->mCharFirst) && (aChar<=apFont->mCharLast) )
		{
			lIndex  = apFont->mpCharMap[ aChar - apFont->mCharFirst ];
			lWidth  = (U16)(apFont->mpRegions[ lIndex ].mX1 - apFont->mpRegions[ lIndex ].mX0);
			lWidth += 1;
		}
		else
		{
			lWidth = apFont->mSpaceWidth;
		}
	}
	return( lWidth );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetCharX0( const sFont * apFont,const U8 aChar )
* ACTION   : Font_GetCharX0
* CREATION : 26.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Font_GetCharX0( const sFont * apFont,const U8 aChar )
{
	U16	lX0;
	U16	lIndex;

	lX0 = 0;
	if( apFont )
	{
		if( (aChar>=apFont->mCharFirst) && (aChar<=apFont->mCharLast) )
		{
			lIndex = apFont->mpCharMap[ aChar - apFont->mCharFirst ];
			lX0    = (apFont->mpRegions[ lIndex ].mX0);
		}
	}
	return( lX0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetCharX1( const sFont * apFont,const U8 aChar )
* ACTION   : Font_GetCharX1
* CREATION : 27.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U16	Font_GetCharX1( const sFont * apFont,const U8 aChar )
{
	U16	lX1;
	U16	lIndex;

	lX1 = 0;
	if( apFont )
	{
		if( (aChar>=apFont->mCharFirst) && (aChar<=apFont->mCharLast) )
		{
			lIndex = apFont->mpCharMap[ aChar - apFont->mCharFirst ];
			if( lIndex < apFont->mSpriteCount )
			{
				lX1    = (apFont->mpRegions[ lIndex ].mX1);
			}
		}
		else if( ' ' == aChar )
		{
			lX1 = apFont->mSpaceWidth;
		}
	}
	return( lX1 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetpSpriteRegion( const sFont * apFont,const char aChar )
* ACTION   : Font_GetpSpriteRegion
* CREATION : 20.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sSpriteRegion*	Font_GetpSpriteRegion( const sFont * apFont,const char aChar )
{
	sSpriteRegion *	lpRegion;
	U16				lIndex;

	lpRegion = 0;

	if( apFont )
	{
		if( (aChar>=apFont->mCharFirst) && (aChar<=apFont->mCharLast) )
		{
			lIndex   =  apFont->mpCharMap[ aChar - apFont->mCharFirst ];
			lpRegion = &apFont->mpRegions[ lIndex ];
		}
	}
	return( lpRegion );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetpSprite( const sFont * apFont,const char aChar )
* ACTION   : Font_GetpSprite
* CREATION : 20.01.2004 PNK
*-----------------------------------------------------------------------------------*/

sSprite *	Font_GetpSprite( const sFont * apFont,const char aChar )
{
	sSprite *	lpSprite;
	U16			lIndex;

	lpSprite = 0;

	if( apFont )
	{
		if( (aChar>=apFont->mCharFirst) && (aChar<=apFont->mCharLast) )
		{
			lIndex   =  apFont->mpCharMap[ aChar - apFont->mCharFirst ];
			if( lIndex < apFont->mSpriteCount )
			{
				lpSprite = &apFont->mpSprites[ lIndex ];
			}
			else
			{
				lpSprite = 0;
			}
		}
	}
	return( lpSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_Delocate( sFont * apFont )
* ACTION   : Font_Delocate
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Font_Delocate( sFont * apFont )
{
	U16	i;

	if( apFont )
	{
		for( i=0; i<apFont->mSpriteCount; i++ )
		{
			Sprite_Delocate( &apFont->mpSprites[ i ] );

			Endian_FromBigU16( &apFont->mpRegions[ i ].mX0 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mX1 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mY0 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mY1 );

/*			*(U32*)&apFont->mpSprites[ i ].mpGfx  -= (U32)apFont;
			*(U32*)&apFont->mpSprites[ i ].mpMask -= (U32)apFont;*/
		}

		*(U32*)&apFont->mpCharMap  -= (U32)apFont;
		*(U32*)&apFont->mpRegions  -= (U32)apFont;
		*(U32*)&apFont->mpSprites  -= (U32)apFont;

		Endian_FromBigU16( &apFont->mWidthMax );
		Endian_FromBigU16( &apFont->mHeightMax );

		Endian_FromBigU32( &apFont->mpCharMap );
		Endian_FromBigU32( &apFont->mpRegions );
		Endian_FromBigU32( &apFont->mpSprites );

		Endian_FromBigU16( &apFont->mSpriteCount );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_Relocate( sFont * apFont )
* ACTION   : Font_Relocate
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Font_Relocate( sFont * apFont )
{
	U16	i;

	if( apFont )
	{
		Endian_FromBigU16( &apFont->mWidthMax );
		Endian_FromBigU16( &apFont->mHeightMax );

		Endian_FromBigU32( &apFont->mpCharMap );
		Endian_FromBigU32( &apFont->mpRegions );
		Endian_FromBigU32( &apFont->mpSprites );

		*(U32*)&apFont->mpCharMap  += (U32)apFont;
		*(U32*)&apFont->mpRegions  += (U32)apFont;
		*(U32*)&apFont->mpSprites  += (U32)apFont;

		Endian_FromBigU16( &apFont->mSpriteCount );
		for( i=0; i<apFont->mSpriteCount; i++ )
		{
			Sprite_Relocate( &apFont->mpSprites[ i ] );
/*
			Endian_FromBigU32( &apFont->mpSprites[ i ].mpGfx  );
			Endian_FromBigU32( &apFont->mpSprites[ i ].mpMask );
			*(U32*)&apFont->mpSprites[ i ].mpGfx  += (U32)apFont;
			*(U32*)&apFont->mpSprites[ i ].mpMask += (U32)apFont;
*/
			Endian_FromBigU16( &apFont->mpRegions[ i ].mX0 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mX1 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mY0 );
			Endian_FromBigU16( &apFont->mpRegions[ i ].mY1 );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_GetSize( sFont * apFont )
* ACTION   : Font_GetSize
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

U32	Font_GetSize( sFont * apFont )
{
	U32	lSize;
	U16	i;

	if( apFont )
	{
		lSize  = sizeof(sFont);
		lSize += apFont->mSpriteCount * sizeof(sSpriteRegion);
		for( i=0; i<apFont->mSpriteCount; i++ )
		{
			lSize += Sprite_GetSize( &apFont->mpSprites[ i ] );
		}
		if( apFont->mCharLast > apFont->mCharFirst )
		{
			lSize += (apFont->mCharLast - apFont->mCharFirst)+1;
		}
	}
	else
	{
		lSize = 0;
	}
	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Font_ShowInfo( sFont * apFont )
* ACTION   : Font_ShowInfo
* CREATION : 18.02.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Font_ShowInfo( sFont * apFont )
{
	U16	i;
	sSprite *	lpSprite;

	printf( "Font_ShowInfo() %p\n", apFont );
	printf( "CharFirst %d\n", apFont->mCharFirst );
	printf( "CharLast %d\n", apFont->mCharLast );
	printf( "HeightMax %d\n", apFont->mHeightMax );
	printf( "ID %ld\n", apFont->mID );
	printf( "Kerning %d\n", apFont->mKerning );
	printf( "SpaceWidth %d\n", apFont->mSpaceWidth );
	printf( "SpriteCount %d\n", apFont->mSpriteCount );
	printf( "Version %ld\n", apFont->mVersion );
	printf( "WidthMax %d\n", apFont->mWidthMax );

	for( i=0; i<apFont->mSpriteCount; i++ )
	{
		lpSprite = &apFont->mpSprites[ i ];
		printf( "Sprite %d w:%d h:%d\n", i, lpSprite->mWidth, lpSprite->mHeight );
	}
}


/* ################################################################################ */

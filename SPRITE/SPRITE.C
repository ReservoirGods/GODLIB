/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: SPRITE.C
::
:: Sprite routines
::
:: [c] 2001 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"SPRITE.H"

#include	<GODLIB/DEBUGLOG/DEBUGLOG.H>
#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  DEFINES
################################################################################### */


#define	dSPRITEBLOCK_ID			mSTRING_TO_U32( 'B', 'S', 'B', 'K' )
#define	dSPRITEBLOCK_VERSION	0


/* ###################################################################################
#  DATA
################################################################################### */

U8	gSpriteFlipTable[ 256 ];


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		Sprite_BitRead( U16 * apBase, U16 aBitPlaneCount, U16 aBitPlane, U16 aX );
void	Sprite_BitWrite( U16 * apBase, U16 aBitPlaneCount,  U16 aBitPlane, U16 aX, U16 aBit );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Init( void )
* ACTION   : Sprite_Init
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_Init( void )
{
	U16	i,j;
	U16	lMask;
	U8	lByte;
	U8	lOr;

	for( i=0; i<256; i++ )
	{
		lByte = 0;
		lOr   = 0x80;
		lMask = 1;

		for( j=0; j<8; j++ )
		{
			if( i & lMask )
			{
				lByte |= lOr;
			}
			lOr   >>= 1;
			lMask <<= 1;
		}

		gSpriteFlipTable[ i ] = lByte;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_DeInit( void )
* ACTION   : Sprite_DeInit
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_DeInit( void )
{
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Create( U16 * apGfx, U16 * apMsk, U16 aWidth, U16 aHeight, U16 aGfxPlaneCount, U16 aMskPlaneCount )
* ACTION   : creates a sprite
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

sSprite *		Sprite_Create( U16 * apGfx, U16 * apMsk, U16 aWidth, U16 aHeight, U16 aGfxPlaneCount, U16 aMskPlaneCount, const U8 aOpaqueFlag )
{
	sSprite *	lpSprite;
	U8 *				lpMem;
	U16 *				lpSrc;
	U16 *				lpDst;
	U16 *				lpSrc2;
	U32					lSize;
	U32					lGfxSize;
	U32					lMskSize;
	U32					lXwords;
	U16					i,y,x;

	lXwords   = ((aWidth+15)>>4);

	lGfxSize  = (lXwords<<1);
	lGfxSize *= aGfxPlaneCount;
	lGfxSize *= aHeight;

	lMskSize  = (lXwords<<1);
	lMskSize *= aHeight;

	lSize  = lGfxSize + lMskSize;
	lSize += sizeof( sSprite );

	lpSprite = (sSprite*)mMEMCALLOC( lSize );

	lpMem           = (U8*)lpSprite;
	lpMem          += sizeof( sSprite );
	lpSprite->mpMask = (U16*)lpMem;
	lpMem          += lMskSize;
	lpSprite->mpGfx  = (U16*)lpMem;

	lpSprite->mGfxPlaneCount  = aGfxPlaneCount;
	lpSprite->mHeight         = aHeight;
	lpSprite->mMaskPlaneCount = aMskPlaneCount;
	lpSprite->mWidth          = aWidth;

	lpDst = lpSprite->mpMask;
	lpSrc = apMsk;
	if( lpSprite->mMaskPlaneCount )
	{
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			lpSrc2 = lpSrc;
			for( x=0; x<lXwords; x++ )
			{
				if( aOpaqueFlag )
				{
					*lpDst = 0x0;
				}
				else
				{
					*lpDst  = *lpSrc2++;
					*lpDst |= *lpSrc2++;
					*lpDst |= *lpSrc2++;
					*lpDst |= *lpSrc2++;
				}
				lpDst++;
			}
			lpSrc += 80;
		}
	}


	lpSrc = apGfx;
	lpDst = lpSprite->mpGfx;
	if( lpSprite->mGfxPlaneCount )
	{
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			lpSrc2 = lpSrc;
			for( x=0; x<lXwords; x++ )
			{
				for( i=0; i<lpSprite->mGfxPlaneCount; i++ )
				{
					*lpDst++ = lpSrc2[ i ];
				}
				lpSrc2 += 4;

			}
			lpSrc += 80;
		}
	}


	return( lpSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_CreateUnaligned( U16 * apGfx,U16 * apMsk,U16 aWidth,U16 aHeight,U16 aGfxPlaneCount,U16 aMskPlaneCount,const U8 aOpaqueFlag,U16 aX )
* ACTION   : Sprite_CreateUnaligned
* CREATION : 01.03.2005 PNK
*-----------------------------------------------------------------------------------*/

sSprite *	Sprite_CreateUnaligned( U16 * apGfx,U16 * apMsk,U16 aWidth,U16 aHeight,U16 aGfxPlaneCount,U16 aMskPlaneCount,const U8 aOpaqueFlag,U16 aX )
{
	sSprite *	lpSprite;
	U8 *				lpMem;
	U16 *				lpSrc;
	U16 *				lpDst;
	U16 *				lpSrc2;
	U32					lSize;
	U32					lGfxSize;
	U32					lMskSize;
	U32					lXwords;
	U16					lWidth;
	U16					i,y,x;

	lXwords   = ((aWidth+15)>>4);
	lWidth    = (U16)(lXwords << 4);

	lGfxSize  = (lXwords<<1);
	lGfxSize *= aGfxPlaneCount;
	lGfxSize *= aHeight;

	lMskSize  = (lXwords<<1);
	lMskSize *= aHeight;

	lSize  = lGfxSize + lMskSize;
	lSize += sizeof( sSprite );

	lpSprite = (sSprite*)mMEMCALLOC( lSize );

	lpMem           = (U8*)lpSprite;
	lpMem          += sizeof( sSprite );
	lpSprite->mpMask = (U16*)lpMem;
	lpMem          += lMskSize;
	lpSprite->mpGfx  = (U16*)lpMem;

	lpSprite->mGfxPlaneCount  = aGfxPlaneCount;
	lpSprite->mHeight         = aHeight;
	lpSprite->mMaskPlaneCount = aMskPlaneCount;
	lpSprite->mWidth          = lWidth;

	lpDst = lpSprite->mpMask;
	lpSrc = apMsk;
	if( lpSprite->mMaskPlaneCount )
	{
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			lpSrc2 = lpSrc;
			for( x=0; x<lXwords; x++ )
			{
				lpDst[ x ] = 0;
			}

			for( x=0; x<aWidth; x++ )
			{
				if( aOpaqueFlag )
				{
					Sprite_BitWrite( lpDst, 1, 0, x, 0 );
				}
				else
				{
					i  = Sprite_BitRead( lpSrc2, 4, 0, (U16)(aX + x) );
					i |= Sprite_BitRead( lpSrc2, 4, 1, (U16)(aX + x) );
					i |= Sprite_BitRead( lpSrc2, 4, 2, (U16)(aX + x) );
					i |= Sprite_BitRead( lpSrc2, 4, 3, (U16)(aX + x) );
					i &= 1;
					Sprite_BitWrite( lpDst, 1, 0, x, i );
				}
			}
			for( x=aWidth; x<lWidth; x++ )
			{
				Sprite_BitWrite( lpDst, 1, 0, x, 1 );
			}
			lpDst += lXwords;
			lpSrc += 80;
		}
	}



	lpSrc = apGfx;
	lpDst = lpSprite->mpGfx;
	if( lpSprite->mGfxPlaneCount )
	{
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			for( i=0; i<lpSprite->mGfxPlaneCount; i++ )
			{
				for( x=0; x<aWidth; x++ )
				{
					Sprite_BitWrite( lpDst, lpSprite->mGfxPlaneCount, i, x,
						Sprite_BitRead( lpSrc, 4, i, (U16)(aX + x) ) );
				}
			}
			lpDst +=(lXwords * lpSprite->mGfxPlaneCount);
			lpSrc += 80;
		}
	}

	return( lpSprite );
}


sSprite	*	Sprite_CreatePreShifted( sSprite * apSprite, const U16 aShiftAmount )
{
	sSprite *	lpSpriteNew;
	U8 *		lpMem;
	U16 *		lpSrc;
	U16 *		lpDst;
	U32			lSize;
	U32			lGfxSize;
	U32			lMskSize;
	U32			lXwords;
	U16			lWidth;
	U16			i,y,x;

	lXwords   = ((apSprite->mWidth+31)>>4);
	lWidth    = (U16)(lXwords << 4);

	lGfxSize  = (lXwords<<1);
	lGfxSize *= apSprite->mGfxPlaneCount;
	lGfxSize *= apSprite->mHeight;

	lMskSize  = (lXwords<<1);
	lMskSize *= apSprite->mHeight;

	lSize  = lGfxSize + lMskSize;
	lSize += sizeof( sSprite );

	lpSpriteNew = (sSprite*)mMEMCALLOC( lSize );

	lpMem           = (U8*)lpSpriteNew;
	lpMem          += sizeof( sSprite );
	lpSpriteNew->mpMask = (U16*)lpMem;
	lpMem          += lMskSize;
	lpSpriteNew->mpGfx  = (U16*)lpMem;

	lpSpriteNew->mGfxPlaneCount  = apSprite->mGfxPlaneCount;
	lpSpriteNew->mHeight         = apSprite->mHeight;
	lpSpriteNew->mMaskPlaneCount = apSprite->mMaskPlaneCount;
	lpSpriteNew->mWidth          = (U16)(lXwords << 4L);

	lpDst = lpSpriteNew->mpMask;
	lpSrc = apSprite->mpMask;

	if( lpSpriteNew->mMaskPlaneCount )
	{
		for( y=0; y<lpSpriteNew->mHeight; y++ )
		{
			for( x=0; x<lXwords; x++ )
			{
				lpDst[ x ] = 0xFFFF;
			}

			for( x=0; x<apSprite->mWidth; x++ )
			{
				U16	lPixel;

				lPixel = Sprite_BitRead( lpSrc, 1, 0, x );
				Sprite_BitWrite( lpDst, 1, 0, x + aShiftAmount, lPixel );
			}
			lpSrc += ((apSprite->mWidth+15)>>4);
			lpDst += lXwords;
		}
	}


	lpSrc = apSprite->mpGfx;
	lpDst = lpSpriteNew->mpGfx;
	for( y=0; y<lpSpriteNew->mHeight; y++ )
	{
		for( x=0; x<lXwords; x++ )
		{
			U16 * lpClr;
			lpClr = lpDst;
			for( i=0; i<apSprite->mGfxPlaneCount; i++ )
			{
				*lpClr++ = 0;
			}
		}

		for( x=0; x<apSprite->mWidth; x++ )
		{
			for( i=0; i<apSprite->mGfxPlaneCount; i++ )
			{
				U16	lPixel;
				lPixel = Sprite_BitRead( lpSrc, apSprite->mGfxPlaneCount, i, x );
				Sprite_BitWrite( lpDst, apSprite->mGfxPlaneCount, i, x + aShiftAmount, lPixel );
			}
		}

		lpSrc += ((apSprite->mWidth+15)>>4) * apSprite->mGfxPlaneCount;
		lpDst += lXwords * apSprite->mGfxPlaneCount;
	}

	return( lpSpriteNew );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_CreateTC( U16 * apGfxBase,U16 aMskColour,sSpriteRect * apDstRect,U16 aSrcWidth,const U8 aOpaqueFlag )
* ACTION   : Sprite_CreateTC
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

sSprite *	Sprite_CreateTC( U16 * apGfxBase,U16 aMskColour,sSpriteRect * apDstRect,U16 aSrcWidth,const U8 aOpaqueFlag )
{
	sSprite *	lpSprite;
	U8 *				lpMem;
	U16 *				lpSrc;
	U16 *				lpDst;
	U16 *				lpSrc2;
	U32					lSize;
	U32					lGfxSize;
	U32					lMskSize;
	U32					lXwords;
	U16					lWidth;
	U16					y,x;

	lXwords   = ((apDstRect->mWidth+15)>>4);
	lWidth    = (U16)(lXwords << 4);

	lGfxSize   = apDstRect->mWidth;
	lGfxSize <<= 1;
	lGfxSize  *= apDstRect->mHeight;

	lMskSize  = (lXwords<<1);
	lMskSize *= apDstRect->mHeight;

	lSize  = lGfxSize + lMskSize;
	lSize += sizeof( sSprite );

	lpSprite = (sSprite*)mMEMCALLOC( lSize );

	lpMem           = (U8*)lpSprite;
	lpMem          += sizeof( sSprite );
	lpSprite->mpMask = (U16*)lpMem;
	lpMem          += lMskSize;
	lpSprite->mpGfx  = (U16*)lpMem;

	lpSprite->mGfxPlaneCount  = 16;
	lpSprite->mHeight         = apDstRect->mHeight;
	if( aOpaqueFlag )
	{
		lpSprite->mMaskPlaneCount = 0;
	}
	else
	{
		lpSprite->mMaskPlaneCount = 16;
	}

	lpSprite->mWidth          = apDstRect->mWidth;

	lpDst = lpSprite->mpMask;
	if( lpSprite->mMaskPlaneCount )
	{
		lMskSize  = apDstRect->mY;
		lMskSize *= aSrcWidth;
		lMskSize += apDstRect->mX;
		lpSrc     = apGfxBase;
		lpSrc    += lMskSize;
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			lpSrc2 = lpSrc;
			for( x=0; x<lXwords; x++ )
			{
				lpDst[ x ] = 0;
			}

			for( x=0; (S16)x<apDstRect->mWidth; x++ )
			{
				if( (lpSrc[ x ] != aMskColour) || (aOpaqueFlag) )
				{
					Sprite_BitWrite( lpDst, 1, 0, x, 0 );
				}
				else
				{
					Sprite_BitWrite( lpDst, 1, 0, x, 1 );
				}
			}
			for( x=apDstRect->mWidth; x<lWidth; x++ )
			{
				Sprite_BitWrite( lpDst, 1, 0, x, 1 );
			}
			lpDst += lXwords;
			lpSrc += aSrcWidth;
		}
	}



	lpSrc = apGfxBase;
	lpDst = lpSprite->mpGfx;
	if( lpSprite->mGfxPlaneCount )
	{
		lMskSize  = apDstRect->mY;
		lMskSize *= aSrcWidth;
		lMskSize += apDstRect->mX;
		lpSrc     = apGfxBase;
		lpSrc    += lMskSize;
		for( y=0; y<lpSprite->mHeight; y++ )
		{
			lpSrc2 = lpSrc;
			for( x=0; (S16)x<apDstRect->mWidth; x++ )
			{
				*lpDst++ = *lpSrc2++;
			}
			lpSrc += aSrcWidth;
		}
	}

	return( lpSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Destroy( sSprite * apSprite )
* ACTION   : releases memory allocate by a sprite
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_Destroy( sSprite * apSprite )
{
	if( apSprite )
	{
/*		mMEMFREE( apSprite->pGfx );
		mMEMFREE( apSprite->pMask ); */
		mMEMFREE( apSprite );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Delocate( sSprite * apSprite )
* ACTION   : delocates a sprite
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_Delocate( sSprite * apSprite )
{
	*(U32*)&apSprite->mpGfx  -= (U32) apSprite;
	*(U32*)&apSprite->mpMask -= (U32) apSprite;
	Endian_FromBigU16( &apSprite->mGfxPlaneCount  );
	Endian_FromBigU16( &apSprite->mMaskPlaneCount  );
	Endian_FromBigU16( &apSprite->mHeight  );
	Endian_FromBigU16( &apSprite->mWidth  );

	Endian_FromBigU32( &apSprite->mpGfx  );
	Endian_FromBigU32( &apSprite->mpMask );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Relocate( sSprite * apSprite )
* ACTION   : relocates a sprite
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_Relocate( sSprite * apSprite )
{
	Endian_FromBigU16( &apSprite->mGfxPlaneCount  );
	Endian_FromBigU16( &apSprite->mMaskPlaneCount  );
	Endian_FromBigU16( &apSprite->mHeight  );
	Endian_FromBigU16( &apSprite->mWidth  );

	Endian_FromBigU32( &apSprite->mpGfx  );
	Endian_FromBigU32( &apSprite->mpMask );
	*(U32*)&apSprite->mpGfx  += (U32) apSprite;
	*(U32*)&apSprite->mpMask += (U32) apSprite;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_GetSize( sSprite * apSprite )
* ACTION   : returns the size of a sprite
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

U32						Sprite_GetSize( sSprite * apSprite )
{
	U32	lSize;

	lSize = 0;
	if( apSprite )
	{
		lSize  = Sprite_GetGfxSize( apSprite );
		lSize += Sprite_GetMskSize( apSprite );
		lSize += sizeof( sSprite );
	}
	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_GetMskSize( sSprite * apSprite )
* ACTION   : Sprite_GetMskSize
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Sprite_GetMskSize( sSprite * apSprite )
{
	U32	lSize;
	U32	lXwords;

	lSize = 0;
	if( apSprite )
	{
		if( apSprite->mMaskPlaneCount )
		{
			lXwords  = ((apSprite->mWidth+15)>>4);
			lSize    = (lXwords<<1);
			lSize   *= apSprite->mHeight;
		}
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_GetGfxSize( sSprite * apSprite )
* ACTION   : Sprite_GetGfxSize
* CREATION : 26.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	Sprite_GetGfxSize( sSprite * apSprite )
{
	U32	lSize;
	U32	lXwords;

	lSize   = 0;
	lXwords = ((apSprite->mWidth+15)>>4);

	if( apSprite->mGfxPlaneCount >= 16 )
	{
		lSize = apSprite->mWidth;
	}
	else
	{
		lSize = lXwords << 4;
	}
	lSize *= apSprite->mGfxPlaneCount;
	lSize>>= 3;
	lSize *= apSprite->mHeight;

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockClone( sSpriteBlock * apSrc )
* ACTION   : Sprite_BlockClone
* CREATION : 12.08.2004 PNK
*-----------------------------------------------------------------------------------*/

sSpriteBlock *	Sprite_BlockClone( sSpriteBlock * apSrc )
{
	U32				lSize;
	sSpriteBlock *	lpBlock;

	lSize   = Sprite_BlockGetSize( apSrc );
	lpBlock = (sSpriteBlock*)mMEMCALLOC( lSize );
	if( lpBlock )
	{
		Sprite_BlockDelocate( apSrc );
		Memory_Copy( lSize, apSrc, lpBlock );
		Sprite_BlockRelocate( lpBlock );
		Sprite_BlockRelocate( apSrc );
	}
	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockCreate( U16 aSpriteCount )
* ACTION   : creates a sprite block
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

sSpriteBlock *	Sprite_BlockCreate( U16 aSpriteCount )
{
	sSpriteBlock *	lpBlock;
	U32						lSize;

	lSize  = sizeof( sSpriteBlockHeader );
	lSize += aSpriteCount << 2;

	lpBlock = (sSpriteBlock*)mMEMCALLOC( lSize );

	lpBlock->mHeader.mID          = dSPRITEBLOCK_ID;
	lpBlock->mHeader.mVersion     = dSPRITEBLOCK_VERSION;
	lpBlock->mHeader.mSpriteCount = aSpriteCount;

	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockDestroy( sSpriteBlock * apBlock )
* ACTION   : releases memory allocated by a sprite block
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_BlockDestroy( sSpriteBlock * apBlock )
{
	U16	i;

	for( i=0 ;i< apBlock->mHeader.mSpriteCount; i++ )
	{
		Sprite_Destroy( apBlock->mpSprite[ i ] );
	}
	mMEMFREE( apBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockDelocate( sSpriteBlock * apBlock )
* ACTION   : delocates all elements in a sprite block
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_BlockDelocate( sSpriteBlock * apBlock )
{
	U16	i;

	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		if( apBlock->mpSprite[ i ] )
		{
			Sprite_Delocate( apBlock->mpSprite[ i ] );
		}
	}

	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		*(U32*)&apBlock->mpSprite[ i ] -= (U32)apBlock;
		Endian_FromBigU32( &apBlock->mpSprite[ i ] );
	}
	Endian_FromBigU16( &apBlock->mHeader.mSpriteCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockRelocate( sSpriteBlock * apBlock )
* ACTION   : relocates all elements in a sprite block
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

void					Sprite_BlockRelocate( sSpriteBlock * apBlock )
{
	U16	i;

	if( !apBlock )
	{
		return;
	}

	Endian_FromBigU16( &apBlock->mHeader.mSpriteCount );
	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		Endian_FromBigU32( &apBlock->mpSprite[ i ] );
		*(U32*)&apBlock->mpSprite[ i ] += (U32)apBlock;
	}

	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		if( apBlock->mpSprite[ i ] )
		{
			Sprite_Relocate( apBlock->mpSprite[ i ] );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockGetSize( sSpriteBlock * apBlock )
* ACTION   : returns the size of a sprite block
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

U32						Sprite_BlockGetSize( sSpriteBlock * apBlock )
{
	U32						lSize;
	U16						i;

	if( !apBlock )
		return( 0 );

	lSize  = sizeof( sSpriteBlockHeader );
	lSize += apBlock->mHeader.mSpriteCount << 2;
	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		lSize += Sprite_GetSize( apBlock->mpSprite[ i ] );
	}

	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockSerialise( sSpriteBlock * apBlock )
* ACTION   : creates a serialised copy of apBlock
* CREATION : 17.02.01 PNK
*-----------------------------------------------------------------------------------*/

sSpriteBlock	*	Sprite_BlockSerialise( sSpriteBlock * apBlock )
{
	sSpriteBlock *	lpBlock;
	sSprite *		lpSprite;
	U8 *					lpMem;
	U32						lSize;
	U16						i;

	lSize   = Sprite_BlockGetSize( apBlock );
	lpBlock = (sSpriteBlock*)mMEMCALLOC( lSize );
	if( !lpBlock )
		return( 0 );

	lpBlock->mHeader.mID          = apBlock->mHeader.mID;
	lpBlock->mHeader.mSpriteCount = apBlock->mHeader.mSpriteCount;
	lpBlock->mHeader.mVersion     = apBlock->mHeader.mVersion;

	lpMem  = (U8*)lpBlock;
	lpMem += sizeof( sSpriteBlockHeader );
	lpMem += apBlock->mHeader.mSpriteCount << 2;


	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		lpSprite = (sSprite*)lpMem;
		lpBlock->mpSprite[ i ] = lpSprite;
		lpMem += sizeof( sSprite );

		lpSprite->mGfxPlaneCount  = apBlock->mpSprite[ i ]->mGfxPlaneCount;
		lpSprite->mHeight         = apBlock->mpSprite[ i ]->mHeight;
		lpSprite->mMaskPlaneCount = apBlock->mpSprite[ i ]->mMaskPlaneCount;
		lpSprite->mWidth          = apBlock->mpSprite[ i ]->mWidth;

		lpSprite->mpMask = (U16*)lpMem;
		if( apBlock->mpSprite[ i ]->mMaskPlaneCount )
		{
			lSize  = Sprite_GetMskSize( apBlock->mpSprite[ i ] );
			Memory_Copy( lSize, apBlock->mpSprite[ i ]->mpMask, lpSprite->mpMask );
			lpMem += lSize;
		}
		lpSprite->mpGfx = (U16*)lpMem;
		if( apBlock->mpSprite[ i ]->mGfxPlaneCount )
		{
			lSize  = Sprite_GetGfxSize( apBlock->mpSprite[ i ] );
			Memory_Copy( lSize, apBlock->mpSprite[ i ]->mpGfx, lpSprite->mpGfx );
			lpMem += lSize;
		}
	}

	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_FlipH( sSprite * apBlock )
* ACTION   : Sprite_FlipH
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_FlipH( sSprite * apBlock )
{
	U16		i,j,k;
	U16		lWidth;
	U8 *	lpGfx;
	U8	*	lpMsk;
	U16 *	lpGfxDst;
	U16 *	lpMskDst;
	U16		lByte0;
	U16		lByte1;
	U16		lStack[ 256 ];
	U16		lStackIndex;

	lpGfx = (U8*)apBlock->mpGfx;
	lpMsk = (U8*)apBlock->mpMask;

	lWidth   = (U16)(apBlock->mWidth+15);
	lWidth >>= 4;

	for( i=0; i<apBlock->mHeight; i++ )
	{
		lpGfxDst = (U16*)lpGfx;
		lpMskDst = (U16*)lpMsk;

		lStackIndex = 0;
		for( j=0; j<lWidth; j++ )
		{
			for( k=0; k<apBlock->mGfxPlaneCount; k++ )
			{
				lByte0 = lpGfx[ 0 ];
				lByte1 = lpGfx[ 1 ];

				lByte0 = gSpriteFlipTable[ lByte0 ];
				lByte1 = gSpriteFlipTable[ lByte1 ];

				lByte1 <<= 8;
				lByte0  &= 0xFF;

				lByte0  |= lByte1;

				lStack[ lStackIndex ] = lByte0;

				lStackIndex++;
				lpGfx += 2;
			}
		}

		for( j=0; j<lWidth; j++ )
		{
			lStackIndex = (U16)(lStackIndex - apBlock->mGfxPlaneCount);
			for( k=0; k<apBlock->mGfxPlaneCount; k++ )
			{
				*lpGfxDst++ = lStack[ lStackIndex + k ];
			}
		}


		if( apBlock->mMaskPlaneCount )
		{
			lStackIndex = 0;
			for( j=0; j<lWidth; j++ )
			{
				lByte0 = lpMsk[ 0 ];
				lByte1 = lpMsk[ 1 ];

				lByte0 = gSpriteFlipTable[ lByte0 ];
				lByte1 = gSpriteFlipTable[ lByte1 ];

				lByte1 <<= 8;
				lByte0  &= 0xFF;

				lByte0  |= lByte1;

				lStack[ lStackIndex ] = lByte0;

				lStackIndex++;
				lpMsk += 2;
			}
			for( j=0; j<lWidth; j++ )
			{
				lStackIndex--;
				*lpMskDst++ = lStack[ lStackIndex ];
			}
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockFlipH( sSpriteBlock * apBlock )
* ACTION   : Sprite_BlockFlipH
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_BlockFlipH( sSpriteBlock * apBlock )
{
	U16	i;

	for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
	{
		Sprite_FlipH( apBlock->mpSprite[ i ] );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_GetRegion( sSprite * apBlock,sSpriteRegion * apRegion )
* ACTION   : Sprite_GetRegion
* CREATION : 07.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_GetRegion( sSprite * apSprite,sSpriteRegion * apRegion )
{
	U16		i;
	U16		x,y;
	U16		lChunks;
	U16		lX0,lX1,lY0,lY1;
	U16		lBits;
	U16 *	lpMask;

	if( apSprite )
	{
		lpMask        = apSprite->mpMask;

		lX0           = apSprite->mWidth;
		lX1           = 0;
		lY0           = apSprite->mHeight;
		lY1           = 0;
		apRegion->mX0 = 0;
		apRegion->mX1 = 0;
		apRegion->mY0 = 0;
		apRegion->mY1 = 0;

		lChunks = (U16)((apSprite->mWidth + 15)>>4);

		for( y=0; y<apSprite->mHeight; y++ )
		{
			lBits = 0;

			for( i=0; i<lChunks; i++ )
			{
				Endian_ReadBigU16( lpMask, lBits );
				lBits = (U16)(~ lBits);
				lpMask++;
/*				lBits = ~*lpMask++;*/

				if( lBits )
				{
					if( y < lY0 )
					{
						lY0           = y;
						apRegion->mY0 = y;
					}
					if( y > lY1 )
					{
						lY1           = y;
						apRegion->mY1 = y;
					}

					x=0;
					while( !(lBits & (0x8000>>x)) )
					{
						x++;
					}
					x = (U16)(x +(i<<4));

					if( x < lX0 )
					{
						lX0           = x;
						apRegion->mX0 = x;
					}

					x = 15;
					while( !(lBits & (0x8000>>x)) )
					{
						x--;
					}

					x = (U16)(x+(i<<4));

					if( x > lX1 )
					{
						lX1           = x;
						apRegion->mX1 = x;
					}
				}
			}
		}
	}
}


U16	Sprite_BitPlane_GetPixel( U16 * apPlanes, const U16 aX, const U16 aY, const U16 aWidth, const U16 aSkipWords )
{
	U32	lOff;
	U16	lPixel;

	lOff  = (aWidth+15)>>4;
	lOff *= aSkipWords;
	lOff *= aY;
	lOff += ((aX>>4)*aSkipWords);

/*	lPixel   = apPlanes[ lOff ];*/
	Endian_ReadBigU16( &apPlanes[ lOff ], lPixel );
	lPixel >>= (15-(aX&15));
	lPixel  &= 1;

	return( lPixel );
}

void	Sprite_BitPlane_SetPixel( U16 * apPlanes, const U16 aX, const U16 aY, const U16 aWidth, const U16 aSkipWords, const U16 aVal )
{
	U32	lOff;
	U16	lPixel;
	U16	lTemp;

	lOff  = (aWidth+15)>>4;
	lOff *= aSkipWords;
	lOff *= aY;
	lOff += ((aX>>4)*aSkipWords);

	lPixel   = aVal;
	lPixel  &= 1;
	lPixel <<= (15-(aX&15));

/*	apPlanes[ lOff ] &= ~lPixel;
	apPlanes[ lOff ] |= lPixel;*/

	Endian_ReadBigU16( &apPlanes[ lOff ], lTemp );
	lTemp &= ~lPixel;
	lTemp |= lPixel;
	Endian_WriteBigU16( &apPlanes[ lOff ], lTemp );


}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BitPlaneRot90( U16 * apSrc,U16 * apDst,U16 aWidth,U16 aHeight,U16 aSkipWords )
* ACTION   : Sprite_BitPlaneRot90
* CREATION : 15.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_BitPlaneRot90( U16 * apSrc,U16 * apDst,U16 aWidth,U16 aHeight,U16 aSkipWords )
{
	U16	i,j;
	U32	lSrcOff;
	U32	lDstOff;
	U16	lSrcWordsX;
	U16	lSrcWordsY;
	U16	lDstWordsX;
	U16	lDstWordsY;
	U16	lBits;

	lSrcOff = 0;
	lDstOff = 0;

	lSrcWordsX = (aSkipWords);
	lSrcWordsY = (U16)(((aWidth+15)>>4) * (aSkipWords));
	lDstWordsX = (aSkipWords);
	lDstWordsY = (U16)(((aHeight+15)>>4) * (aSkipWords));

	Memory_Clear( ((aHeight+15)>>4) * aWidth * 2, apDst );

	for( i=0; i<aWidth; i++ )
	{
		for( j=0; j<aHeight; j++ )
		{
			lSrcOff  = (j*lSrcWordsY);
			lSrcOff += (i>>4) * lSrcWordsX;

			lDstOff  = (i*lDstWordsY);
			lDstOff += (j>>4) * lDstWordsX;

			lBits   = apSrc[ lSrcOff ];
			lBits >>= (15-(i&15));
			lBits  &= 1;
			lBits <<= (15-(j&15));
			apDst[ lDstOff ] |= lBits;
		}
	}

/*	for( i=0; i<((aWidth*aHeight)>>4); i++ )
	{
		DebugLog_Printf3( "planerot0 %d src:%04X dst:%04X", i, apSrc[i], apDst[i] );
	}*/

	Memory_Clear( ((aHeight+15)>>4) * aWidth * 2, apDst );

	for( i=0; i<aWidth; i++ )
	{
		for( j=0; j<aHeight; j++ )
		{
			Sprite_BitPlane_SetPixel( apDst, j, i, aHeight, aSkipWords,
				Sprite_BitPlane_GetPixel( apSrc, i, j, aWidth, aSkipWords ) );
		}
	}

/*	for( i=0; i<((aWidth*aHeight)>>4); i++ )
	{
		DebugLog_Printf3( "planerot1 %d src:%04X dst:%04X", i, apSrc[i], apDst[i] );
	}*/
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_Rotate90( const sSprite * apSrc,sSprite * apDst )
* ACTION   : Sprite_Rotate90
* CREATION : 15.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_Rotate90( const sSprite * apSrc,sSprite * apDst )
{
	U16	i;

	apDst->mHeight         = apSrc->mWidth;
	apDst->mWidth          = apSrc->mHeight;
	apDst->mGfxPlaneCount  = apSrc->mGfxPlaneCount;
	apDst->mMaskPlaneCount = apSrc->mMaskPlaneCount;

	Sprite_BitPlaneRot90(  apSrc->mpMask, apDst->mpMask, apSrc->mWidth, apSrc->mHeight, 1 );
	for( i=0; i<apSrc->mGfxPlaneCount; i++ )
	{
		Sprite_BitPlaneRot90(  &apSrc->mpGfx[ i ], &apDst->mpGfx[ i ], apSrc->mWidth, apSrc->mHeight, apSrc->mGfxPlaneCount );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : SpriteBlock_Rot90( const sSpriteBlock * apSrc )
* ACTION   : SpriteBlock_Rot90
* CREATION : 15.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sSpriteBlock *	SpriteBlock_Rot90( const sSpriteBlock * apSrc )
{
	sSprite			lSprite;
	sSpriteBlock *	lpBlock;
	U8 *			lpMem;
	U32				lOff;
	U32				lSize;
	U16				lChunks;
	U16				i;

	lSize = 0;
	for( i=0; i<apSrc->mHeader.mSpriteCount; i++ )
	{
		lSprite         = *apSrc->mpSprite[ i ];
		lSprite.mHeight = apSrc->mpSprite[ i ]->mWidth;
		lSprite.mWidth  = apSrc->mpSprite[ i ]->mHeight;
		lSize          += Sprite_GetSize( &lSprite );
	}
	lSize += sizeof( sSpriteBlockHeader );
	lSize += apSrc->mHeader.mSpriteCount << 2;

	lpMem    = (U8*)mMEMCALLOC( lSize );
	lpBlock  = (sSpriteBlock*)lpMem;
	*lpBlock =*apSrc;
	lOff     = sizeof( sSpriteBlockHeader );
	lOff    += apSrc->mHeader.mSpriteCount << 2;

	for( i=0; i<apSrc->mHeader.mSpriteCount; i++ )
	{
		lpBlock->mpSprite[ i ]         = (sSprite*)&lpMem[ lOff ];
		*lpBlock->mpSprite[ i ]        = *apSrc->mpSprite[ i ];
		lpBlock->mpSprite[ i ]->mHeight = apSrc->mpSprite[ i ]->mWidth;
		lpBlock->mpSprite[ i ]->mWidth  = apSrc->mpSprite[ i ]->mHeight;

		lOff                         += sizeof( sSprite );
		lpBlock->mpSprite[ i ]->mpMask  = (U16*)&lpMem[ lOff ];

		lChunks  = (U16)(((lpBlock->mpSprite[i]->mWidth+15)>>4));
		lOff    += (lChunks * lpBlock->mpSprite[i]->mHeight);

		lpBlock->mpSprite[ i ]->mpGfx  = (U16*)&lpMem[ lOff ];
		lOff    += (lChunks * lpBlock->mpSprite[i]->mHeight) * lpBlock->mpSprite[ i ]->mGfxPlaneCount;

		Sprite_Rotate90( apSrc->mpSprite[ i ], lpBlock->mpSprite[ i ] );
		Sprite_Rotate90( lpBlock->mpSprite[ i ],&lSprite );
/*
		for( j=0; j<((apSrc->pSprite[ i ]->Width>>4)*(apSrc->pSprite[ i ]->Height)); j++ )
		{
			lBits0 = lSprite.pMask[ j ];
			lBits1 = apSrc->pSprite[ i ]->pMask[ j ];
		}

		for( j=0; j<((apSrc->pSprite[ i ]->Width>>4)*(apSrc->pSprite[ i ]->Height)*apSrc->pSprite[i]->GfxPlaneCount); j++ )
		{
			lBits0 = lSprite.pGfx[ j ];
			lBits1 = apSrc->pSprite[ i ]->pGfx[ j ];
		}
*/
	}

	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BitRead( U16 * apBase,U16 aBitPlaneCount,U16 aBitPlane,U16 aX )
* ACTION   : Sprite_BitRead
* CREATION : 01.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	Sprite_BitRead( U16 * apBase,U16 aBitPlaneCount,U16 aBitPlane,U16 aX )
{
	U32	lOff;
	U16	lBit;

	lOff   = (aX>>4);
	lOff  *= aBitPlaneCount;
	lOff  += aBitPlane;

	Endian_ReadBigU16( &apBase[ lOff ], lBit );
	lBit >>= (15 - (aX&15));
	lBit  &= 1;

	return( lBit );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BitWrite( U16 * apBase,U16 aBitPlaneCount,U16 aBitPlane,U16 aX,U16 aBit )
* ACTION   : Sprite_BitWrite
* CREATION : 01.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_BitWrite( U16 * apBase,U16 aBitPlaneCount,U16 aBitPlane,U16 aX,U16 aBit )
{
	U32	lOff;
	U16	lBit;
	U16	lPix;
	U16 lMask;

	lOff   = (aX>>4);
	lOff  *= aBitPlaneCount;
	lOff  += aBitPlane;

	lBit   = (U16)(aBit & 1);
	lBit <<= (15 - (aX&15));
	lMask = 1 << (15 - (aX&15));

	Endian_ReadBigU16( &apBase[ lOff ], lPix );
	lPix &= ~lMask;
	lPix |= lBit;
	Endian_WriteBigU16( &apBase[ lOff ], lPix );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Sprite_BlockGetRegion( sSpriteBlock * apBlock,sSpriteRegion * apRegion )
* ACTION   : Sprite_BlockGetRegion
* CREATION : 16.07.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Sprite_BlockGetRegion( sSpriteBlock * apBlock,sSpriteRegion * apRegion )
{
	U16	lBlocks;
	U16	lY;
	U16	lX;
	U16	i;
	U32	lGfxOff;
	U32	lMskOff;
	U16	lBits[ 4 ];
	U16	lNew[ 4 ];
	U16 *	lpGfx;
	U16 *	lpMsk;
	U16		lMask;

	apRegion->mX0 = apBlock->mpSprite[ 0 ]->mWidth;
	apRegion->mY0 = apBlock->mpSprite[ 0 ]->mHeight;
	apRegion->mX1 = 0;
	apRegion->mY1 = 0;

	lBlocks = (U16)((apBlock->mpSprite[ 0 ]->mWidth+15)>>4);
	lGfxOff = 0;
	lMskOff = 0;

	for( lY=0; lY<apBlock->mpSprite[0]->mHeight; lY++ )
	{
		for( lX=0; lX<lBlocks; lX++ )
		{
			lpGfx  = apBlock->mpSprite[ 0 ]->mpGfx;
			lpMsk  = apBlock->mpSprite[ 0 ]->mpMask;
			lpGfx += lGfxOff;
			lpMsk += lMskOff;

			lMask      = (U16)~lpMsk[ 0 ];
			lBits[ 0 ] = (U16)(lpGfx[ 0 ] & lMask);
			lBits[ 1 ] = (U16)(lpGfx[ 1 ] & lMask);
			lBits[ 2 ] = (U16)(lpGfx[ 2 ] & lMask);
			lBits[ 3 ] = (U16)(lpGfx[ 3 ] & lMask);
			for( i=1; i<apBlock->mHeader.mSpriteCount; i++ )
			{
				lpGfx  = apBlock->mpSprite[ i ]->mpGfx;
				lpMsk  = apBlock->mpSprite[ i ]->mpMask;
				lpGfx += lGfxOff;
				lpMsk += lMskOff;
				lMask     = (U16)~lpMsk[ 0 ];
				lNew[ 0 ] = (U16)(lpGfx[ 0 ] & lMask);
				lNew[ 1 ] = (U16)(lpGfx[ 1 ] & lMask);
				lNew[ 2 ] = (U16)(lpGfx[ 2 ] & lMask);
				lNew[ 3 ] = (U16)(lpGfx[ 3 ] & lMask);

				if( (lNew[0] != lBits[0]) ||
					(lNew[1] != lBits[1]) ||
					(lNew[2] != lBits[2]) ||
					(lNew[3] != lBits[3]) )
				{
					if( lY < apRegion->mY0 )
					{
						apRegion->mY0 = lY;
					}
					if( lY > apRegion->mY1 )
					{
						apRegion->mY1 = lY;
					}
					if( lX < apRegion->mX0 )
					{
						apRegion->mX0 = lX;
					}
					if( lX > apRegion->mX1 )
					{
						apRegion->mX1 = lX;
					}
				}
			}
			lGfxOff += 4;
			lMskOff += 1;
		}
	}
	apRegion->mX0 <<= 4;
	apRegion->mX1 <<= 4;
}


/* ################################################################################ */

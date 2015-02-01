/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ASPRITE.H"

#include	<GODLIB/MEMORY/MEMORY.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct	sAsmColourCounter
{
	U16	mColour;
	U16	mCount;
} sAsmColourCounter;

typedef struct	sAsmPairCounter
{
	U16	mColour0;
	U16	mColour1;
	U16	mCount;
} sAsmPairCounter;


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dASPRITEBLOCK_ID		mSTRING_TO_U32( 'A', 'S', 'B', 'K' )
#define	dASPRITEBLOCK_VERSION	0

#define	dINS_ADDQL_2_A1		0x5489
#define	dINS_ADDQL_4_A1		0x5889
#define	dINS_ADDQL_6_A1		0x5C89
#define	dINS_ADDQL_8_A1		0x5089

#define	dINS_ADDL_D0_A1		0xD3C0

#define	dINS_LEA_I_A1		0x43E9

#define	dINS_MOVEL_A0_A1_P	0x22C8
#define	dINS_MOVEL_A2_A1_P	0x22CA
#define	dINS_MOVEL_A3_A1_P	0x22CB
#define	dINS_MOVEL_A4_A1_P	0x22CC
#define	dINS_MOVEL_A5_A1_P	0x22CD
#define	dINS_MOVEL_A6_A1_P	0x22CE

#define	dINS_MOVEL_D1_A1_P	0x22C1
#define	dINS_MOVEL_D2_A1_P	0x22C2
#define	dINS_MOVEL_D3_A1_P	0x22C3
#define	dINS_MOVEL_D4_A1_P	0x22C4
#define	dINS_MOVEL_D5_A1_P	0x22C5
#define	dINS_MOVEL_D6_A1_P	0x22C6
#define	dINS_MOVEL_D7_A1_P	0x22C7

#define	dINS_MOVEL_I_A1_P	0x22FC

#define	dINS_MOVEW_A0_A1_P	0x32C8
#define	dINS_MOVEW_A2_A1_P	0x32CA
#define	dINS_MOVEW_A3_A1_P	0x32CB
#define	dINS_MOVEW_A4_A1_P	0x32CC
#define	dINS_MOVEW_A5_A1_P	0x32CD
#define	dINS_MOVEW_A6_A1_P	0x32CE

#define	dINS_MOVEW_D1_A1_P	0x32C1
#define	dINS_MOVEW_D2_A1_P	0x32C2
#define	dINS_MOVEW_D3_A1_P	0x32C3
#define	dINS_MOVEW_D4_A1_P	0x32C4
#define	dINS_MOVEW_D5_A1_P	0x32C5
#define	dINS_MOVEW_D6_A1_P	0x32C6
#define	dINS_MOVEW_D7_A1_P	0x32C7

#define	dINS_MOVEW_I_A1_P	0x32FC

#define	dINS_RTS			0x4E75


/* ###################################################################################
#  DATA
################################################################################### */

U16	gAsmAddql[] =
{
	dINS_ADDQL_2_A1,
	dINS_ADDQL_4_A1,
	dINS_ADDQL_6_A1,
	dINS_ADDQL_8_A1,
};

U16	gAsmInsMoveW[] =
{
	dINS_MOVEW_D1_A1_P,
	dINS_MOVEW_D2_A1_P,
	dINS_MOVEW_D3_A1_P,
	dINS_MOVEW_D4_A1_P,
	dINS_MOVEW_D5_A1_P,
	dINS_MOVEW_D6_A1_P,
	dINS_MOVEW_D7_A1_P,

	dINS_MOVEW_A0_A1_P,
	dINS_MOVEW_A2_A1_P,
	dINS_MOVEW_A3_A1_P,
	dINS_MOVEW_A4_A1_P,
	dINS_MOVEW_A5_A1_P,
	dINS_MOVEW_A6_A1_P,
};

U16	gAsmInsMoveL[] =
{
	dINS_MOVEL_D1_A1_P,
	dINS_MOVEL_D2_A1_P,
	dINS_MOVEL_D3_A1_P,
	dINS_MOVEL_D4_A1_P,
	dINS_MOVEL_D5_A1_P,
	dINS_MOVEL_D6_A1_P,
	dINS_MOVEL_D7_A1_P,

	dINS_MOVEL_A0_A1_P,
	dINS_MOVEL_A2_A1_P,
	dINS_MOVEL_A3_A1_P,
	dINS_MOVEL_A4_A1_P,
	dINS_MOVEL_A5_A1_P,
	dINS_MOVEL_A6_A1_P,
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	AsmColourCounter_Add( sAsmColourCounter * apCounter, const U16 aColour );
U32		AsmColourCounter_GetCount( sAsmColourCounter * apCounter );
int		AsmColourCounter_Compare( const void * apCounter0, const void * apCounter1 );

void	AsmPairCounter_Add( sAsmPairCounter * apCounter, const U16 aColour0, U16 aColour1 );
U32		AsmPairCounter_GetCount( sAsmPairCounter * apCounter );
int		AsmPairCounter_Compare( const void * apCounter0, const void * apCounter1 );

void	AsmSprite_DrawLines( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth, U16 aFirstLine );

U16 *	AsmSprite_SkipOutput( U16 * apCod, U16 aSkipCount );
U16 *	AsmSprite_SingleOutput( U16 * apCod, const sAsmSprite * apSprite, U16 aPixel );
U16 *	AsmSprite_DoubleOutput( U16 * apCod, const sAsmSprite * apSprite, U16 aPixel0, U16 aPixel1 );
U16 *	AsmSprite_PixelsOutput( U16 * apCod, const sAsmSprite * apSprite, const U16 * apGfx, const U16 aPixelCount );

U16		AsmSprite_GetWordIndex( const sAsmSprite * apSprite, const U16 aPixel );
U16		AsmSprite_GetLongIndex( const sAsmSprite * apSprite, const U16 aPixel0, const U16 aPixel1 );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmColourCounter_Add( sAsmColourCounter * apCounter,const U16 aColour )
* ACTION   : AsmColourCounter_Add
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmColourCounter_Add( sAsmColourCounter * apCounter,const U16 aColour )
{
	while( (apCounter->mColour != aColour) && (apCounter->mCount) )
	{
		apCounter++;
	}
	apCounter->mColour = aColour;
	apCounter->mCount++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmColourCounter_GetCount( sAsmColourCounter * apCounter )
* ACTION   : AsmColourCounter_GetCount
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	AsmColourCounter_GetCount( sAsmColourCounter * apCounter )
{
	U32	lCount;

	lCount = 0;

	while( apCounter->mCount )
	{
		lCount++;
		apCounter++;
	}
	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmColourCounter_Compare( const void * apCounter0,const void * apCounter1 )
* ACTION   : AsmColourCounter_Compare
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

int	AsmColourCounter_Compare( const void * apCounter0,const void * apCounter1 )
{
	sAsmColourCounter *	lpCounter0;
	sAsmColourCounter *	lpCounter1;
	int					lRes;

	lpCounter0 = (sAsmColourCounter*)apCounter0;
	lpCounter1 = (sAsmColourCounter*)apCounter1;

	if( lpCounter0->mCount < lpCounter1->mCount )
	{
		lRes = 1;
	}
	else if( lpCounter0->mCount > lpCounter1->mCount )
	{
		lRes = -1;
	}
	else
	{
		lRes = 0;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmPairCounter_GetCount( sAsmPairCounter * apCounter )
* ACTION   : AsmPairCounter_GetCount
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	AsmPairCounter_GetCount( sAsmPairCounter * apCounter )
{
	U32	lCount;

	lCount = 0;

	while( apCounter->mCount )
	{
		lCount++;
		apCounter++;
	}
	return( lCount );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmPairCounter_Add( sAsmPairCounter * apCounter,const U16 aColour0,U16 aColour1 )
* ACTION   : AsmPairCounter_Add
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmPairCounter_Add( sAsmPairCounter * apCounter,const U16 aColour0,U16 aColour1 )
{
	while(
		(( apCounter->mColour0 != aColour0 ) ||	( apCounter->mColour1 != aColour1 ))  &&
		( apCounter->mCount ) )
	{
		apCounter++;
	}
	apCounter->mColour0 = aColour0;
	apCounter->mColour1 = aColour1;
	apCounter->mCount++;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmPairCounter_Compare( const void * apCounter0,const void * apCounter1 )
* ACTION   : AsmPairCounter_Compare
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

int	AsmPairCounter_Compare( const void * apCounter0,const void * apCounter1 )
{
	sAsmPairCounter *	lpCounter0;
	sAsmPairCounter *	lpCounter1;
	int					lRes;

	lpCounter0 = (sAsmPairCounter*)apCounter0;
	lpCounter1 = (sAsmPairCounter*)apCounter1;

	if( lpCounter0->mCount < lpCounter1->mCount )
	{
		lRes = 1;
	}
	else if( lpCounter0->mCount > lpCounter1->mCount )
	{
		lRes = -1;
	}
	else
	{
		lRes = 0;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_SkipOutput( U16 * apCod,U16 aSkipCount )
* ACTION   : AsmSprite_SkipOutput
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	AsmSprite_SkipOutput( U16 * apCod,U16 aSkipCount )
{
	U16	lIns;

	switch( aSkipCount )
	{
	case 0:
		break;
	case 1:
		lIns = dINS_ADDQL_2_A1;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		break;
	case 2:
		lIns = dINS_ADDQL_4_A1;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		break;
	case 3:
		lIns = dINS_ADDQL_6_A1;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		break;
	case 4:
		lIns = dINS_ADDQL_8_A1;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		break;
	default:
		lIns = dINS_LEA_I_A1;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		lIns = (U16)(aSkipCount * 2);
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		break;
	}

	return( apCod );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_GetWordIndex( const sAsmSprite * apSprite,const U16 aPixel )
* ACTION   : AsmSprite_GetWordIndex
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	AsmSprite_GetWordIndex( const sAsmSprite * apSprite,const U16 aPixel )
{
	U16	lIndex;

	lIndex = 0;

	while( (lIndex<eASPRITE_COLOUR_LIMIT) && (apSprite->mHeader.mColours[lIndex][1] != aPixel) )
	{
		lIndex++;
	}

	return( lIndex );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_GetLongIndex( const sAsmSprite * apSprite,const U16 aPixel0,const U16 aPixel1 )
* ACTION   : AsmSprite_GetLongIndex
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	AsmSprite_GetLongIndex( const sAsmSprite * apSprite,const U16 aPixel0,const U16 aPixel1 )
{
	U16	lIndex;

	lIndex = 0;

	while(  (lIndex<eASPRITE_COLOUR_LIMIT) &&
			((apSprite->mHeader.mColours[lIndex][0] != aPixel0) ||
			(apSprite->mHeader.mColours[lIndex][1] != aPixel1)) )
	{
		lIndex++;
	}

	return( lIndex );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_SingleOutput( U16 * apCod,const sAsmSprite * apSprite,U16 aPixel )
* ACTION   : AsmSprite_SingleOutput
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	AsmSprite_SingleOutput( U16 * apCod,const sAsmSprite * apSprite,U16 aPixel )
{
	U16	lIns;
	U16	lIndex;


	lIndex = AsmSprite_GetWordIndex( apSprite, aPixel );

	if( lIndex < eASPRITE_COLOUR_LIMIT )
	{
		Endian_WriteBigU16( apCod, gAsmInsMoveW[ lIndex ] );
		apCod++;
	}
	else
	{
		lIns = dINS_MOVEW_I_A1_P;
		Endian_WriteBigU16( apCod, lIns );
		apCod++;
		Endian_WriteBigU16( apCod, aPixel );
		apCod++;
	}

	return( apCod );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_DoubleOutput( U16 * apCod,const sAsmSprite * apSprite,U16 aPixel0,U16 aPixel1 )
* ACTION   : AsmSprite_DoubleOutput
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	AsmSprite_DoubleOutput( U16 * apCod,const sAsmSprite * apSprite,U16 aPixel0,U16 aPixel1 )
{
	U16	lIndex0;
	U16	lIndex1;
	U16	lIns;

	lIndex0 = AsmSprite_GetLongIndex( apSprite, aPixel0, aPixel1 );

	if( lIndex0 < eASPRITE_COLOUR_LIMIT )
	{
		Endian_WriteBigU16( apCod, gAsmInsMoveL[ lIndex0 ] );
		apCod++;
	}
	else
	{
		lIndex0 = AsmSprite_GetWordIndex( apSprite, aPixel0 );
		lIndex1 = AsmSprite_GetWordIndex( apSprite, aPixel1 );

		if( (lIndex0<eASPRITE_COLOUR_LIMIT) || (lIndex1<eASPRITE_COLOUR_LIMIT) )
		{
			apCod = AsmSprite_SingleOutput( apCod, apSprite, aPixel0 );
			apCod = AsmSprite_SingleOutput( apCod, apSprite, aPixel1 );
		}
		else
		{
			lIns = dINS_MOVEL_I_A1_P;
			Endian_WriteBigU16( apCod, lIns );
			apCod++;
			Endian_WriteBigU16( apCod, aPixel0 );
			apCod++;
			Endian_WriteBigU16( apCod, aPixel1 );
			apCod++;
		}
	}

	return( apCod );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_PixelsOutput( U16 * apCod,const sAsmSprite * apSprite,const U16 * apGfx,const U16 aPixelCount )
* ACTION   : AsmSprite_PixelsOutput
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U16 *	AsmSprite_PixelsOutput( U16 * apCod,const sAsmSprite * apSprite,const U16 * apGfx,const U16 aPixelCount )
{
	const U16 * lpGfx;
	U16			lIndex;
	U16			lPixelCount;
	U16			lPixel0;
	U16			lPixel1;
	U16			lPixel2;

	lpGfx       = apGfx;
	lPixelCount = aPixelCount;

	while( lPixelCount )
	{
		Endian_ReadBigU16( lpGfx, lPixel0 );
		lpGfx++;

		if( 1 == lPixelCount )
		{
			apCod = AsmSprite_SingleOutput( apCod, apSprite, lPixel0 );
			lPixelCount--;
		}
		else if( 2 == lPixelCount )
		{

			Endian_ReadBigU16( lpGfx, lPixel1 );
			apCod = AsmSprite_DoubleOutput( apCod, apSprite, lPixel0, lPixel1 );
			lPixelCount = 0;
		}
		else
		{
			Endian_ReadBigU16( lpGfx, lPixel1 );
			lIndex = AsmSprite_GetLongIndex( apSprite, lPixel0, lPixel1 );

			if( lIndex < eASPRITE_COLOUR_LIMIT )
			{
				apCod = AsmSprite_DoubleOutput( apCod, apSprite, lPixel0, lPixel1 );
				lPixelCount -= 2;
				lpGfx++;
			}
			else
			{
				Endian_ReadBigU16( &lpGfx[ 1 ], lPixel2 );
				lIndex = AsmSprite_GetLongIndex( apSprite, lPixel1, lPixel2 );
				if( lIndex < eASPRITE_COLOUR_LIMIT )
				{
					apCod = AsmSprite_SingleOutput( apCod, apSprite, lPixel0 );
					apCod = AsmSprite_DoubleOutput( apCod, apSprite, lPixel1, lPixel2 );
					lPixelCount -= 3;
					lpGfx += 2;
				}
				else
				{
					apCod = AsmSprite_DoubleOutput( apCod, apSprite, lPixel0, lPixel1 );
					lPixelCount -= 2;
					lpGfx ++;
				}
			}
		}
	}


	return( apCod );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_Create( const U16 * apGfx,const sSpriteRect * apRect,const U16 aGfxWidth,const U16 aOpaqueColour,const U8 aOpaqueFlag )
* ACTION   : AsmSprite_Create
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sAsmSprite *	AsmSprite_Create( const U16 * apGfx,const sSpriteRect * apRect,const U16 aGfxWidth,const U16 aOpaqueColour,const U8 aOpaqueFlag )
{
	sAsmSprite *		lpSprite;
	sAsmColourCounter *	lpSingles;
	sAsmPairCounter *	lpPairs;
	U16 *				lpCod;
	U16					lIns;
	U16					lPixel0;
	U16					lPixel1;
	U32					lSingleCount;
	U32					lPairCount;
	U32					lSize;
	U16					lX,lY;
	U16					lSkip;
	U16					lPixelCount;
	const U16 *			lpPixels;
	const U16 *			lpGfx;
	U16					aSprWidth,aSprHeight;

	lpSprite = 0;

	aSprWidth = apRect->mWidth;
	aSprHeight = apRect->mHeight;
	lpGfx      = apGfx;

	if( apGfx && aSprWidth && aSprHeight )
	{
		lpGfx += (aGfxWidth * apRect->mY);
		lpGfx += (apRect->mX);

		lSize  = aSprWidth;
		lSize += 3;
		lSize *= 4;
		lSize *= aSprHeight;
		lSize += sizeof(sAsmSpriteHeader);

		lpSprite = (sAsmSprite*)mMEMCALLOC( lSize );

		lSize    = aSprHeight;
		lSize   *= aSprWidth;
		lSize   *= sizeof( sAsmColourCounter );
		lpSingles = (sAsmColourCounter*)mMEMCALLOC( lSize );

		lSize    = aSprHeight;
		lSize   *= aSprWidth;
		lSize   *= sizeof( sAsmPairCounter );
		lpPairs  = (sAsmPairCounter*)mMEMCALLOC( lSize );

		if( lpSprite && lpSingles )
		{
			lpSprite->mHeader.mWidth  = aSprWidth;
			lpSprite->mHeader.mHeight = aSprHeight;


			for( lY=0; lY<aSprHeight; lY++ )
			{
				for( lX=0; lX<aSprWidth; lX++ )
				{
					lSize  = lY;
					lSize *= aGfxWidth;
					lSize += lX;
					Endian_ReadBigU16( &lpGfx[ lSize ], lPixel0 );
					if( aOpaqueFlag )
					{
						AsmColourCounter_Add( lpSingles, lPixel0 );
					}
					else
					{
						if( lpGfx[ lSize ] != aOpaqueColour )
						{
							AsmColourCounter_Add( lpSingles, lPixel0 );
						}
					}
					if( lX )
					{
						Endian_ReadBigU16( &lpGfx[ lSize-1 ], lPixel1 );
						if( aOpaqueFlag )
						{
							AsmPairCounter_Add( lpPairs, lPixel1, lPixel0 );
						}
						else
						{
							if( (lpGfx[ lSize-1 ] != aOpaqueColour) && (lpGfx[ lSize ] != aOpaqueColour) )
							{
								AsmPairCounter_Add( lpPairs, lPixel1, lPixel0 );
							}
						}
					}
				}
			}

			lSingleCount = AsmColourCounter_GetCount( lpSingles );
			lPairCount   = AsmPairCounter_GetCount( lpPairs );

			qsort( lpSingles, lSingleCount, sizeof(sAsmColourCounter), AsmColourCounter_Compare );
			qsort( lpPairs,   lPairCount,   sizeof(sAsmPairCounter),   AsmPairCounter_Compare   );

			lpCod  = (U16*)lpSprite;
			lpCod += (sizeof(sAsmSpriteHeader)>>1);
			lpCod += ((aSprHeight+1)<<1);


			lX = 0;
			while( (lX<lSingleCount) && (lX<eASPRITE_COLOUR_LIMIT) )
			{
				lpSprite->mHeader.mColours[ lX ][1] = lpSingles[ lX ].mColour;

				lY = 0;

				while( (lY<lPairCount) && (lpPairs->mColour1 == lpSingles[ lX ].mColour) )
				{
					lY++;
				}
				if( lY < lPairCount )
				{
					lpSprite->mHeader.mColours[ lX ][0] = lpPairs[ lX ].mColour0;
				}

				lX++;
			}

			lpPixels = lpGfx;
			for( lY=0; lY<aSprHeight; lY++ )
			{
				lpSprite->mfLines[ lY ] = lpCod;
				lSkip = 0;
				lPixelCount = 0;

				for( lX=0; lX<aSprWidth; lX++ )
				{
					lSize  = lY;
					lSize *= aGfxWidth;
					lSize += lX;

					Endian_ReadBigU16( &lpGfx[ lSize ], lPixel0 );
					if( aOpaqueFlag )
					{
						if( !lPixelCount )
						{
							lpPixels = &lpGfx[ lSize ];
						}
						lPixelCount++;
					}
					else
					{
						if( lPixel0 == aOpaqueColour )
						{
							lpCod = AsmSprite_PixelsOutput( lpCod, lpSprite, lpPixels, lPixelCount );
							lPixelCount = 0;
							lSkip++;
						}
						else
						{
							lpCod = AsmSprite_SkipOutput( lpCod, lSkip );
							lSkip = 0;
							if( !lPixelCount )
							{
								lpPixels = &lpGfx[ lSize ];
							}
							lPixelCount++;
						}
					}
				}
				lpCod = AsmSprite_PixelsOutput( lpCod, lpSprite, lpPixels, lPixelCount );
				lPixelCount = 0;
				lpCod = AsmSprite_SkipOutput( lpCod, lSkip );
				lSkip = 0;
				lIns = dINS_ADDL_D0_A1;
				Endian_WriteBigU16( lpCod, lIns );
				lpCod++;
			}
			lIns = dINS_RTS;
			Endian_WriteBigU16( lpCod, lIns );
			lpCod++;
			lpSprite->mfLines[ lY ] = lpCod;

			for( lX=0; lX<eASPRITE_COLOUR_LIMIT; lX++ )
			{
				Endian_FromBigU16( &lpSprite->mHeader.mColours[ lX ][ 0 ] );
				Endian_FromBigU16( &lpSprite->mHeader.mColours[ lX ][ 1 ] );
			}
		}

		mMEMFREE( lpPairs   );
		mMEMFREE( lpSingles );
	}

	return( lpSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void AsmSprite_Destroy(sAsmSprite * apSprite)
* ACTION   : AsmSprite_Destroy
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void AsmSprite_Destroy(sAsmSprite * apSprite)
{
	mMEMFREE( apSprite );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 AsmSprite_GetSize(const sAsmSprite * apSprite)
* ACTION   : AsmSprite_GetSize
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32 AsmSprite_GetSize(const sAsmSprite * apSprite)
{
	U32	lSize;

	lSize = 0;
	if( apSprite )
	{
		lSize  = sizeof(sAsmSpriteHeader);
		lSize += (U32)((apSprite->mHeader.mHeight+1) *4L );
		lSize += ( (U32)(apSprite->mfLines[apSprite->mHeader.mHeight]) - (U32)(apSprite->mfLines[0]) );
		lSize += 4;
	}
	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_Delocate( sAsmSprite * apSprite )
* ACTION   : AsmSprite_Delocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmSprite_Delocate( sAsmSprite * apSprite )
{
	U16	i;

	if( apSprite )
	{
		for( i=0; i<=apSprite->mHeader.mHeight; i++ )
		{
			*(U32*)&apSprite->mfLines[ i ] -= (U32)apSprite;
			Endian_FromBigU32( &apSprite->mfLines[ i ] );
		}
		Endian_FromBigU16( &apSprite->mHeader.mHeight );
		Endian_FromBigU16( &apSprite->mHeader.mWidth  );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_Relocate( sAsmSprite * apSprite )
* ACTION   : AsmSprite_Relocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmSprite_Relocate( sAsmSprite * apSprite )
{
	U16	i;

	if( apSprite )
	{
		Endian_FromBigU16( &apSprite->mHeader.mHeight );
		Endian_FromBigU16( &apSprite->mHeader.mWidth  );
		for( i=0; i<=apSprite->mHeader.mHeight; i++ )
		{
			Endian_FromBigU32( &apSprite->mfLines[ i ] );
			*(U32*)&apSprite->mfLines[ i ] += (U32)apSprite;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sAsmSpriteBlock * AsmSpriteBlock_Create(const U16 aSpriteCount)
* ACTION   : AsmSpriteBlock_Create
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sAsmSpriteBlock * AsmSpriteBlock_Create(const U16 aSpriteCount)
{
	sAsmSpriteBlock *	lpBlock;
	U32					lSize;

	lSize = aSpriteCount;
	lSize *= 4;
	lSize += sizeof(sAsmSpriteBlockHeader);

	lpBlock = (sAsmSpriteBlock*)mMEMCALLOC( lSize );

	if( lpBlock )
	{
		lpBlock->mHeader.mID          = dASPRITEBLOCK_ID;
		lpBlock->mHeader.mVersion     = dASPRITEBLOCK_VERSION;
		lpBlock->mHeader.mSpriteCount = aSpriteCount;
	}
	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void AsmSpriteBlock_Destroy(sAsmSpriteBlock * apBlock)
* ACTION   : AsmSpriteBlock_Destroy
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void AsmSpriteBlock_Destroy(sAsmSpriteBlock * apBlock)
{
	mMEMFREE( apBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSpriteBlock_GetSize( const sAsmSpriteBlock * apBlock )
* ACTION   : AsmSpriteBlock_GetSize
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

U32	AsmSpriteBlock_GetSize( const sAsmSpriteBlock * apBlock )
{
	U32	lSize;
	U16	i;

	lSize = 0;
	if( apBlock )
	{
		lSize  = apBlock->mHeader.mSpriteCount;
		lSize *= 4L;
		lSize += sizeof(sAsmSpriteBlockHeader);
		for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
		{
			lSize += AsmSprite_GetSize( apBlock->mpSprites[ i ] );
		}
	}
	return( lSize );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : sAsmSpriteBlock * AsmSpriteBlock_Serialise(const sAsmSpriteBlock * apBlock)
* ACTION   : AsmSpriteBlock_Serialise
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

sAsmSpriteBlock * AsmSpriteBlock_Serialise(const sAsmSpriteBlock * apBlock)
{
	U32					lSize;
	U32					lOff;
	sAsmSpriteBlock *	lpBlock;
	sAsmSprite *		lpSprite;
	U8 *				lpMem;
	U16					i;

	lpBlock = 0;
	if( apBlock )
	{
		lSize   = AsmSpriteBlock_GetSize( apBlock );
		lpMem   = (U8*)mMEMCALLOC( lSize );
		if( lpMem )
		{
			lpBlock          = (sAsmSpriteBlock*)lpMem;
			lOff             = 0;
			lpBlock->mHeader = apBlock->mHeader;
			lOff             = apBlock->mHeader.mSpriteCount;
			lOff            *= 4L;
			lOff            += sizeof(sAsmSpriteBlockHeader);
			for( i=0; i<lpBlock->mHeader.mSpriteCount; i++ )
			{
				lpSprite                = (sAsmSprite*)&lpMem[ lOff ];
				lpBlock->mpSprites[ i ] = lpSprite;

				lSize = AsmSprite_GetSize( apBlock->mpSprites[ i ] );
				AsmSprite_Delocate( apBlock->mpSprites[ i ] );
				Memory_Copy( lSize, apBlock->mpSprites[ i ], lpBlock->mpSprites[ i ] );
				AsmSprite_Relocate( apBlock->mpSprites[ i ] );
				AsmSprite_Relocate( lpBlock->mpSprites[ i ] );
				lOff += lSize;
			}
		}
	}

	return( lpBlock );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void AsmSpriteBlock_Delocate(sAsmSpriteBlock * apBlock)
* ACTION   : AsmSpriteBlock_Delocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void AsmSpriteBlock_Delocate(sAsmSpriteBlock * apBlock)
{
	U16	i;

	if( apBlock )
	{
		for( i=0; i<apBlock->mHeader.mSpriteCount; i++ )
		{
			AsmSprite_Delocate( apBlock->mpSprites[ i ] );
			*(U32*)&apBlock->mpSprites[ i ] -= (U32)apBlock;
			Endian_FromBigU32( &apBlock->mpSprites[ i ] );
		}
		Endian_FromBigU32( &apBlock->mHeader.mID          );
		Endian_FromBigU16( &apBlock->mHeader.mVersion     );
		Endian_FromBigU16( &apBlock->mHeader.mSpriteCount );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void AsmSpriteBlock_Relocate(sAsmSpriteBlock * apBlock)
* ACTION   : AsmSpriteBlock_Relocate
* CREATION : 08.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void AsmSpriteBlock_Relocate(sAsmSpriteBlock * apBlock)
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
			AsmSprite_Relocate( apBlock->mpSprites[ i ] );
		}
	}
}


#ifndef	dGODLIB_PLATFORM_ATARI

/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_Draw( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
* ACTION   : AsmSprite_Draw
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmSprite_Draw( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth )
{
	AsmSprite_DrawLines( apSprite, apScreen, aScreenWidth, 0 );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_DrawClip( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth,S16 aTopLine,S16 aBotLine )
* ACTION   : AsmSprite_DrawClip
* CREATION : 09.04.2005 PNK
*-----------------------------------------------------------------------------------*/

void	AsmSprite_DrawClip( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth,S16 aTopLine,S16 aBotLine )
{
	U16 *	lpRts;

	lpRts = apSprite->mfLines[ aBotLine ];
	lpRts--;
	*lpRts = dINS_RTS;
	AsmSprite_DrawLines( apSprite, apScreen, aScreenWidth, aTopLine );
	*lpRts = dINS_ADDL_D0_A1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AsmSprite_DrawLines( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth,U16 aFirstLine )
* ACTION   : AsmSprite_DrawLines
* CREATION : 10.04.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	mASMSPR_DRAW_REG_W( _aIndex )	{ *lpDst++ = apSprite->mHeader.mColours[ _aIndex ][ 1 ]; }
#define	mASMSPR_DRAW_REG_L( _aIndex )	{ *lpDst++ = apSprite->mHeader.mColours[ _aIndex ][ 0 ]; *lpDst++ = apSprite->mHeader.mColours[ _aIndex ][ 1 ]; }

void	AsmSprite_DrawLines( const sAsmSprite * apSprite,U16 * apScreen,U32 aScreenWidth,U16 aFirstLine )
{
	U16 *	lpDst;
	U16 *	lpCod;
	U16		lIns;
	U32		lD0;



	lpDst = apScreen;
	lpCod = (U16*)apSprite->mfLines[aFirstLine];
	lD0   = aScreenWidth;
	lD0  -= apSprite->mHeader.mWidth;


	Endian_ReadBigU16( lpCod, lIns );
	lpCod++;
	while( lIns != dINS_RTS )
	{
		switch( lIns )
		{
		case	dINS_ADDL_D0_A1:
			lpDst += lD0;
			break;

		case	dINS_ADDQL_2_A1:
			lpDst++;
			break;
		case	dINS_ADDQL_4_A1:
			lpDst += 2;
			break;
		case	dINS_ADDQL_6_A1:
			lpDst += 3;
			break;
		case	dINS_ADDQL_8_A1:
			lpDst += 4;
			break;
		case	dINS_LEA_I_A1:
			Endian_ReadBigU16( lpCod, lIns );
			lpCod++;
			lpDst += (lIns>>1);
			break;

		case	dINS_MOVEL_A0_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A0 );
			break;
		case	dINS_MOVEL_A2_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A2 );
			break;
		case	dINS_MOVEL_A3_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A3 );
			break;
		case	dINS_MOVEL_A4_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A4 );
			break;
		case	dINS_MOVEL_A5_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A5 );
			break;
		case	dINS_MOVEL_A6_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_A6 );
			break;
		case	dINS_MOVEL_D1_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D1 );
			break;
		case	dINS_MOVEL_D2_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D2 );
			break;
		case	dINS_MOVEL_D3_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D3 );
			break;
		case	dINS_MOVEL_D4_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D4 );
			break;
		case	dINS_MOVEL_D5_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D5 );
			break;
		case	dINS_MOVEL_D6_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D6 );
			break;
		case	dINS_MOVEL_D7_A1_P:
			mASMSPR_DRAW_REG_L( eASPRITE_COLOUR_D7 );
			break;

		case	dINS_MOVEL_I_A1_P:
			*lpDst++ = *lpCod++;
			*lpDst++ = *lpCod++;
			break;

		case	dINS_MOVEW_A0_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A0 );
			break;
		case	dINS_MOVEW_A2_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A2 );
			break;
		case	dINS_MOVEW_A3_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A3 );
			break;
		case	dINS_MOVEW_A4_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A4 );
			break;
		case	dINS_MOVEW_A5_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A5 );
			break;
		case	dINS_MOVEW_A6_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_A6 );
			break;
		case	dINS_MOVEW_D1_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D1 );
			break;
		case	dINS_MOVEW_D2_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D2 );
			break;
		case	dINS_MOVEW_D3_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D3 );
			break;
		case	dINS_MOVEW_D4_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D4 );
			break;
		case	dINS_MOVEW_D5_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D5 );
			break;
		case	dINS_MOVEW_D6_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D6 );
			break;
		case	dINS_MOVEW_D7_A1_P:
			mASMSPR_DRAW_REG_W( eASPRITE_COLOUR_D7 );
			break;

		case	dINS_MOVEW_I_A1_P:
			*lpDst++ = *lpCod++;
			break;

		}
		Endian_ReadBigU16( lpCod, lIns );
		lpCod++;
	}

}


#endif

/* ################################################################################ */

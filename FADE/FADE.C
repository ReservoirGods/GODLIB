/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: FADE.C
::
:: Fade routines
::
:: [c] 2000 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"FADE.H"


/* ###################################################################################
#  STRUCTS
################################################################################### */



/* ###################################################################################
#  DATA
################################################################################### */

U16	gFadeTableFromSTE[ 16 ] =
{
	0x0,
	0x2,
	0x4,
	0x6,
	0x8,
	0xA,
	0xC,
	0xE,
	0x1,
	0x3,
	0x5,
	0x7,
	0x9,
	0xB,
	0xD,
	0xF
};

U16	gFadeTableToSTE[ 16 ] =
{
	0x0,
	0x8,
	0x1,
	0x9,
	0x2,
	0xA,
	0x3,
	0xB,
	0x4,
	0xC,
	0x5,
	0xD,
	0x6,
	0xE,
	0x7,
	0xF
};

U16	gFadeBlackPal[ 16 ] =
{
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
};

U16	gFadeWhitePal[ 16 ] =
{
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF
};


U16	gFadeGamma = 16;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */



/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_PalST( U16 * apDest, U16 * apSrc, U16 * apTarget, U16 aColourCount U16 aPos )
* ACTION   : fades aColourCount colour between pal apSrc and apTarget and stores result in apDest
*			 aPos is in 8.8 fixed point format where 0x0100 = 1.0
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_PalST( U16 * apDest, U16 * apSrc, U16 * apTarget, U16 aColourCount, U16 aPos )
{
	U16	i;
	S16	lR0,lG0,lB0;
	S16	lDiffR,lDiffG,lDiffB;

	for( i=0; i<aColourCount; i++ )
	{
		lDiffR  = (S16)(( apTarget[ i ] >> 8) & 7);
		lDiffG  = (S16)(( apTarget[ i ] >> 4) & 7);
		lDiffB  = (S16)(( apTarget[ i ]     ) & 7);

		lR0     = (S16)(( apSrc[ i ] >> 8) & 7);
		lG0     = (S16)(( apSrc[ i ] >> 4) & 7);
		lB0     = (S16)(( apSrc[ i ]     ) & 7);

		lDiffR = (S16)(lDiffR-lR0);
		lDiffG = (S16)(lDiffG-lG0);
		lDiffB = (S16)(lDiffB-lB0);

		lDiffR = (S16)(lDiffR*aPos);
		lDiffG = (S16)(lDiffG*aPos);
		lDiffB = (S16)(lDiffB*aPos);

		lDiffR >>= 8;
		lDiffG >>= 8;
		lDiffB >>= 8;

		lR0 = (S16)(lR0+lDiffR);
		lG0 = (S16)(lG0+lDiffG);
		lB0 = (S16)(lB0+lDiffB);

		lR0 = (S16)(lR0*gFadeGamma);
		lG0 = (S16)(lG0*gFadeGamma);
		lB0 = (S16)(lB0*gFadeGamma);

		lR0 >>= 4;
		lG0 >>= 4;
		lB0 >>= 4;

		if( lR0 < 0 )
		{
			lR0 = 0;
		}
		if( lG0 < 0 )
		{
			lG0 = 0;
		}
		if( lB0 < 0 )
		{
			lB0 = 0;
		}
		if( lR0 > 0x7 )
		{
			lR0 = 0x7;
		}
		if( lG0 > 0x7 )
		{
			lG0 = 0x7;
		}
		if( lB0 > 0x7 )
		{
			lB0 = 0x7;
		}

		lR0 <<= 8;
		lG0 <<= 4;

		apDest[ i ] = (U16)(lR0 | lG0 | lB0);
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_PalSTE( U16 * apDest, U16 * apSrc, U16 * apTarget, U16 aColourCount U16 aPos )
* ACTION   : fades aColourCount colour between pal apSrc and apTarget and stores result in apDest
*			 aPos is in 8.8 fixed point format where 0x0100 = 1.0
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_PalSTE( U16 * apDest, U16 * apSrc, U16 * apTarget, U16 aColourCount, U16 aPos )
{
	U16	i;
	S16	lR0,lG0,lB0;
	S16	lDiffR,lDiffG,lDiffB;

	for( i=0; i<aColourCount; i++ )
	{
		Endian_ReadBigU16( &apTarget[ i ], lDiffB );

		lDiffR  = (S16)(( lDiffB >> 8) & 0xF);
		lDiffG  = (S16)(( lDiffB >> 4) & 0xF);
		lDiffB  = (S16)(( lDiffB     ) & 0xF);

		Endian_ReadBigU16( &apSrc[ i ], lB0 );

		lR0     = (S16)(( lB0 >> 8) & 0xF);
		lG0     = (S16)(( lB0 >> 4) & 0xF);
		lB0     = (S16)(( lB0     ) & 0xF);

/*		lDiffR  = ( apTarget[ i ] >> 8) & 0xF;
		lDiffG  = ( apTarget[ i ] >> 4) & 0xF;
		lDiffB  = ( apTarget[ i ]     ) & 0xF;

		lR0     = ( apSrc[ i ] >> 8) & 0xF;
		lG0     = ( apSrc[ i ] >> 4) & 0xF;
		lB0     = ( apSrc[ i ]     ) & 0xF;*/

		lDiffR  = gFadeTableFromSTE[ lDiffR ];
		lDiffG  = gFadeTableFromSTE[ lDiffG ];
		lDiffB  = gFadeTableFromSTE[ lDiffB ];

		lR0  = gFadeTableFromSTE[ lR0 ];
		lG0  = gFadeTableFromSTE[ lG0 ];
		lB0  = gFadeTableFromSTE[ lB0 ];

		lDiffR = (S16)(lDiffR - lR0);
		lDiffG = (S16)(lDiffG - lG0);
		lDiffB = (S16)(lDiffB - lB0);

		lDiffR = (S16)(lDiffR*aPos);
		lDiffG = (S16)(lDiffG*aPos);
		lDiffB = (S16)(lDiffB*aPos);

		lDiffR >>= 8;
		lDiffG >>= 8;
		lDiffB >>= 8;

		lR0 = (S16)(lR0 + lDiffR);
		lG0 = (S16)(lG0 + lDiffG);
		lB0 = (S16)(lB0 + lDiffB);

		lR0 = (S16)(lR0 * gFadeGamma);
		lG0 = (S16)(lG0 * gFadeGamma);
		lB0 = (S16)(lB0 * gFadeGamma);

		lR0 >>= 4;
		lG0 >>= 4;
		lB0 >>= 4;

		if( lR0 < 0 )
		{
			lR0 = 0;
		}
		if( lG0 < 0 )
		{
			lG0 = 0;
		}
		if( lB0 < 0 )
		{
			lB0 = 0;
		}
		if( lR0 > 0xF )
		{
			lR0 = 0xF;
		}
		if( lG0 > 0xF )
		{
			lG0 = 0xF;
		}
		if( lB0 > 0xF )
		{
			lB0 = 0xF;
		}


		lR0  = gFadeTableToSTE[ lR0 ];
		lG0  = gFadeTableToSTE[ lG0 ];
		lB0  = gFadeTableToSTE[ lB0 ];

		lR0 <<= 8;
		lG0 <<= 4;

		lR0 |= lG0;
		lR0 |= lB0;

/*		apDest[ i ] = lR0 | lG0 | lB0;*/
		Endian_WriteBigU16( &apDest[ i ], lR0 );
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_PalFalcon( U32 * apDest, U32 * apSrc, U32 * apTarget, U16 aColourCount U16 aPos )
* ACTION   : fades aColourCount colour between pal apSrc and apTarget and stores result in apDest
*			 aPos is in 8.8 fixed point format where 0x0100 = 1.0
* CREATION : 24.03.00 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_PalFalcon( U32 * apDest, U32 * apSrc, U32 * apTarget, U16 aColourCount, U16 aPos )
{
	U16	i;
	S16	lR0,lG0,lB0;
	S16	lDiffR,lDiffG,lDiffB;

	for( i=0; i<aColourCount; i++ )
	{
		lDiffR  = (S16)(( apTarget[ i ] >> 24) & 0xFF );
		lDiffG  = (S16)(( apTarget[ i ] >> 16) & 0xFF );
		lDiffB  = (S16)(( apTarget[ i ]      ) & 0xFF );

		lR0     = (S16)(( apSrc[ i ] >> 24) & 0xFF );
		lG0     = (S16)(( apSrc[ i ] >> 16) & 0xFF );
		lB0     = (S16)(( apSrc[ i ]      ) & 0xFF );

		lDiffR = (S16)(lDiffR-lR0);
		lDiffG = (S16)(lDiffG-lG0);
		lDiffB = (S16)(lDiffB-lB0);

		lDiffR = (S16)(lDiffR*aPos);
		lDiffG = (S16)(lDiffG*aPos);
		lDiffB = (S16)(lDiffB*aPos);

		lDiffR >>= 8;
		lDiffG >>= 8;
		lDiffB >>= 8;

		lR0 = (S16)(lR0+lDiffR);
		lG0 = (S16)(lG0+lDiffG);
		lB0 = (S16)(lB0+lDiffB);

		lR0 &=0xFF;
		lG0 &=0xFF;
		lB0 &=0xFF;

		lR0 <<= 24;
		lG0 <<= 16;

		apDest[ i ] = lR0 | lG0 | lB0;
	}
}




/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_GetpBlackPal( void )
* ACTION   : Fade_GetpBlackPal
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U16 *	Fade_GetpBlackPal( void )
{
	return( &gFadeBlackPal[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_GetpWhitePal( void )
* ACTION   : Fade_GetpWhitePal
* CREATION : 07.01.2003 PNK
*-----------------------------------------------------------------------------------*/

U16 *	Fade_GetpWhitePal( void )
{
	return( &gFadeWhitePal[ 0 ] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_SetGamma( const U16 aGamma )
* ACTION   : Fade_SetGamma
* CREATION : 14.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_SetGamma( const U16 aGamma )
{
	gFadeGamma = aGamma;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Fade_BuildGammaCorrectedPal( const U16 * apSrc, U16 * apDst )
* ACTION   : Fade_BuildGammaCorrectedPal
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Fade_BuildGammaCorrectedPal( const U16 * apPal, U16 * apDst )
{
	U16	i;
	U16	lCol;
	S16	lR0,lG0,lB0;

	for( i=0; i<16; i++ )
	{
		Endian_ReadBigU16( &apPal[ i ], lCol );
		lR0     = (S16)(( lCol >> 8) & 0xF);
		lG0     = (S16)(( lCol >> 4) & 0xF);
		lB0     = (S16)(( lCol     ) & 0xF);

		lR0  = gFadeTableFromSTE[ lR0 ];
		lG0  = gFadeTableFromSTE[ lG0 ];
		lB0  = gFadeTableFromSTE[ lB0 ];

		lR0 = (S16)(lR0 * gFadeGamma);
		lG0 = (S16)(lG0 * gFadeGamma);
		lB0 = (S16)(lB0 * gFadeGamma);

		lR0 >>= 4;
		lG0 >>= 4;
		lB0 >>= 4;

		if( lR0 > 0xF )
		{
			lR0 = 0xF;
		}
		if( lG0 > 0xF )
		{
			lG0 = 0xF;
		}
		if( lB0 > 0xF )
		{
			lB0 = 0xF;
		}

		lR0  = gFadeTableToSTE[ lR0 ];
		lG0  = gFadeTableToSTE[ lG0 ];
		lB0  = gFadeTableToSTE[ lB0 ];

		lR0 <<= 8;
		lG0 <<= 4;

		lCol = (U16)(lR0 | lG0 | lB0);

		Endian_WriteBigU16( &apDst[ i ], lCol );
	}
}


/* ################################################################################ */

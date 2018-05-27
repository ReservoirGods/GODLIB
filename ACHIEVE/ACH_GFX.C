/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"ACH_GFX.H"

#include	"ACH_MAIN.H"
#include	"ACH_LOGN.H"
#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/FADE/FADE.H>
#include	<GODLIB/FONT8X8/FONT8X8.H>
#include	<GODLIB/IKBD/IKBD.H>
#include	<GODLIB/THREAD/THREAD.H>
#ifndef	dGODLIB_PLATFORM_ATARI
#include	<GODLIB/VIDEO/VIDEO.H>
#endif


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dACH_BEAM_SPEED0	4
#define	dACH_BEAM_SPEED1	8


/* ###################################################################################
#  ENUMS
################################################################################### */


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct sAchieveGfx
{
	sThread	mBeamThread;
	sAchieveGfxQuad	mBeams[ 2 ];
	U16		mBeamsActiveFlag;
	U16		mFadePos;
	U16		mFadeStep;
	U16		mFadeActiveFlag;
	U16		mPalOld[ 16 ];
	U16		mPalTmp[ 16 ];
	U16		mPalTarget[ 16 ];
	U16		mPalOriginal[ 16 ];
	U16 *	mpLogic;
	U16 *	mpPhysic;
	U8 *	mpFont;
	U8		mInputPacket;
	U8		mInputKey;
	U8		mInputHit;
	U8		mInputUnHit;
} sAchieveGfx;

extern	U8 	gFont8x8[12544];


/* ###################################################################################
#  DATA
################################################################################### */

sAchieveGfx	gAchieveGfxClass;

sAchieveGfxQuad	gAchieveGfxBeamTopPos0	=	{	  0,  0,  10,  10 };
sAchieveGfxQuad	gAchieveGfxBeamTopPos1	=	{	319,319,  10,  10 };
sAchieveGfxQuad	gAchieveGfxBeamBotPos0	=	{	319,319, 190, 190 };
sAchieveGfxQuad	gAchieveGfxBeamBotPos1	=	{	  0,  0, 190, 190 };

/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		AchieveGfx_Fade_Update( void );
U8		Achieve_Gfx_Beam_Thread( sThread * apThread );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_AppInit( void )
* ACTION   : Achieve_Gfx_AppInit
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_AppInit( void )
{
	memset( &gAchieveGfxClass, 0, sizeof(sAchieveGfx) );
	gAchieveGfxClass.mpFont = &gFont8x8[0];
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_AppDeInit( void )
* ACTION   : Achieve_Gfx_AppDeInit
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_AppDeInit( void )
{

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Init( void )
* ACTION   : Achieve_Gfx_Init
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_Init( void )
{
	mTHREAD_INIT( &gAchieveGfxClass.mBeamThread );
	Achieve_Gfx_GetPal( &gAchieveGfxClass.mPalOld[0] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_DeInit( void )
* ACTION   : Achieve_Gfx_DeInit
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_DeInit( void )
{
	Achieve_Gfx_SetPal( &gAchieveGfxClass.mPalOld[0] );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Update( void * apPhysic, void * apLogic )
* ACTION   : Achieve_Gfx_Update
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_Update( void * apPhysic, void * apLogic )
{
	gAchieveGfxClass.mpPhysic = (U16*)apPhysic;
	gAchieveGfxClass.mpLogic  = (U16*)apLogic;

	AchieveGfx_Fade_Update();
	Achieve_Gfx_Beam_Thread( &gAchieveGfxClass.mBeamThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Fade_Begin( const U16 * apPal, const U16 aFrameCount )
* ACTION   : Achieve_Gfx_Fade_Begin
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_Fade_Begin( const U16 * apPal, const U16 aFrameCount )
{
	U16	i;

	for( i=0; i<16; i++ )
	{
		gAchieveGfxClass.mPalTarget[ i ] = apPal[ i ];
	}
	Achieve_Gfx_GetPal( &gAchieveGfxClass.mPalOriginal[ 0 ] );

	gAchieveGfxClass.mFadePos = 0;
	if( aFrameCount )
	{
		gAchieveGfxClass.mFadeStep = 0x100 / aFrameCount;
	}
	else
	{
		gAchieveGfxClass.mFadeStep = 0x100;
	}
	if( !gAchieveGfxClass.mFadeStep )
	{
		gAchieveGfxClass.mFadeStep = 1;
	}
	gAchieveGfxClass.mFadeActiveFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : AchieveGfx_Fade_Update( void )
* ACTION   : AchieveGfx_Fade_Update
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U16	AchieveGfx_Fade_Update( void )
{
	if( gAchieveGfxClass.mFadeActiveFlag )
	{
		Fade_PalSTE( &gAchieveGfxClass.mPalTmp[0], &gAchieveGfxClass.mPalOriginal[0], &gAchieveGfxClass.mPalTarget[0], 16, gAchieveGfxClass.mFadePos );
		if( gAchieveGfxClass.mFadePos < 0x100 )
		{
			gAchieveGfxClass.mFadePos += gAchieveGfxClass.mFadeStep;
		}
		if( gAchieveGfxClass.mFadePos >= 0x100 )
		{
			gAchieveGfxClass.mFadeActiveFlag = 0;
		}
#ifdef	dGODLIB_PLATFORM_ATARI
		memcpy( (void*)0xFFFF8240L, &gAchieveGfxClass.mPalTmp[0], 32 );
#else
		Video_SetNextPalST( &gAchieveGfxClass.mPalTmp[0] );
#endif
	}

	return( !gAchieveGfxClass.mFadeActiveFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_HLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour )
* ACTION   : Achieve_Gfx_HLine_Render
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_HLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour )
{
	U16 *	lpDst;
	U16 *	lpDst2;
	U16		lPixels;
	S16		lMidCount;
	S16		lX0,lX1;

	lX0 = apLine->mX0;
	lX1 = apLine->mX1;

	if( lX0 > 319 )		lX0 = 319;
	if( lX0 < 0 )		lX0 = 0;
	if( lX1 > 319 )		lX1 = 319;
	if( lX1 < 0 )		lX1 = 0;
	if( lX0 > lX1 )
	{
		S16	lTX = lX0;
		lX0 = lX1;
		lX1 = lTX;
	}

/*	if( apLine->mY0 > dACH_LOGIN_BOX_MAXY ) return;
	if( apLine->mY1 < dACH_LOGIN_BOX_MINY ) return;*/

	lpDst   = &apScreen[ dACH_GFX_YOFF(apLine->mY0) + dACH_GFX_XOFF(lX0) ];
	lpDst2  = &apScreen[ dACH_GFX_YOFF(apLine->mY1) + dACH_GFX_XOFF(lX0) ];
	lPixels = 0xFFFF;
	if( lX0 & 15 )
	{
		lPixels = (0xFFFF) >> (lX0&15);

		if( (lX1>>4) == (lX0>>4) )
		{
			lPixels &= (0xFFFF)<<(15-(lX1&15));
		}

		Endian_FromBigU16( &lPixels );
		if( aColour )
		{
			*lpDst  |= lPixels;
			*lpDst2 |= lPixels;
		}
		else
		{
			lPixels = ~lPixels;
			*lpDst  &= lPixels;
			*lpDst2 &= lPixels;
		}
		lX0 += 15;
		lX0 &= 0xFFF0;

		lpDst  += 4;
		lpDst2 += 4;
	}

	lMidCount = (lX1>>4) - (lX0>>4);
	if( lMidCount > 0 )
	{
		if( aColour )
		{
			lPixels = 0xFFFF;
		}
		else
		{
			lPixels = 0;
		}

		while( lMidCount )
		{
			*lpDst   = lPixels;
			*lpDst2  = lPixels;
			lpDst  += 4;
			lpDst2 += 4;
			lMidCount--;
			lX0 += 16;
		}
	}

	if( lX1 >= lX0 )
	{
		lPixels = (0xFFFF)<<(15-(lX1&15));

		Endian_FromBigU16( &lPixels );
		if( aColour )
		{
			*lpDst  |= lPixels;
			*lpDst2 |= lPixels;
		}
		else
		{
			lPixels = ~lPixels;
			*lpDst  &= lPixels;
			*lpDst2 &= lPixels;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_VLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour )
* ACTION   : Achieve_Gfx_VLine_Render
* CREATION : 6.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_VLine_Render( U16 * apScreen, const sAchieveGfxQuad * apLine, const U16 aColour )
{
	U16	lPixels;
	U16	lPixels2;
	U16 *	lpDst;
	U16 *	lpDst2;
	S16	lY0,lY1;
	S16	lLineCount;

	lY0 = apLine->mY0;
	lY1 = apLine->mY1;

	if( lY0 > lY1 )
	{
		S16	lTY = lY0;
		lY0 = lY1;
		lY1 = lTY;
	}

	lpDst   = &apScreen[ dACH_GFX_YOFF(lY0) + dACH_GFX_XOFF(apLine->mX0) ];
	lpDst2  = &apScreen[ dACH_GFX_YOFF(lY0) + dACH_GFX_XOFF(apLine->mX1) ];
	lPixels = 1<<(15-(apLine->mX0&15));
	lPixels2 = 1<<(15-(apLine->mX1&15));
	Endian_FromBigU16( &lPixels );
	Endian_FromBigU16( &lPixels2 );

	lLineCount = (lY1 - lY0) + 1;

	if( aColour )
	{
		while( lLineCount )
		{
			*lpDst  |= lPixels;
			*lpDst2 |= lPixels2;
			lpDst  += 80;
			lpDst2 += 80;
			lLineCount--;
		}
	}
	else
	{
		lPixels = ~lPixels;
		lPixels2 = ~lPixels2;
		while( lLineCount )
		{
			*lpDst  &= lPixels;
			*lpDst2 &= lPixels2;
			lpDst   += 80;
			lpDst2  += 80;
			lLineCount--;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Box_Render( const sAchieveGfxQuad * apQuad, const U16 aColour, const sAchieveGfxQuad * apClipRect )
* ACTION   : Achieve_Gfx_Box_Render
* CREATION : 10.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_Box_Render( const sAchieveGfxQuad * apQuad, const U16 aColour, const sAchieveGfxQuad * apClipRect )
{
	sAchieveGfxQuad	lQuad;

	if( Achieve_Gfx_QuadClip( &lQuad, apQuad, apClipRect ) )
	{
		Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &lQuad, aColour );
		Achieve_Gfx_VLine_Render( gAchieveGfxClass.mpLogic, &lQuad, aColour );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_QuadFromSizePos( sAchieveGfxQuad * apQuad, const sAchieveGfxSizePos * apSizePos )
* ACTION   : Achieve_Gfx_QuadFromSizePos
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_QuadFromSizePos( sAchieveGfxQuad * apQuad, const sAchieveGfxSizePos * apSizePos )
{
	apQuad->mY0 = apSizePos->mY - (apSizePos->mHeight>>1);
	apQuad->mY1 = apSizePos->mY + (apSizePos->mHeight>>1);
	apQuad->mX0 = apSizePos->mX - (apSizePos->mWidth>>1);
	apQuad->mX1 = apSizePos->mX + (apSizePos->mWidth>>1);
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Fade_IsFinished( void )
* ACTION   : Achieve_Gfx_Fade_IsFinished
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Gfx_Fade_IsFinished( void )
{
	return( 0 == gAchieveGfxClass.mFadeActiveFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_QuadClip( sAchieveGfxQuad * apTarget, const sAchieveGfxQuad * apSrc, const sAchieveGfxQuad * apClipRect )
* ACTION   : Achieve_Gfx_QuadClip
* CREATION : 7.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Gfx_QuadClip( sAchieveGfxQuad * apTarget, const sAchieveGfxQuad * apSrc, const sAchieveGfxQuad * apClipRect )
{
	U8	lRes = 0;

	*apTarget = *apSrc;

	if( apTarget->mX0 > apTarget->mX1 )
	{
		S16 lTmp = apTarget->mX0;
		apTarget->mX0 = apTarget->mX1;
		apTarget->mX1 = lTmp;
	}

	if( apTarget->mY0 > apTarget->mY1 )
	{
		S16 lTmp = apTarget->mY0;
		apTarget->mY0 = apTarget->mY1;
		apTarget->mY1 = lTmp;
	}

	if( (apTarget->mX0 < apClipRect->mX1) &&
		(apTarget->mX1 > apClipRect->mX0) &&
		(apTarget->mY0 < apClipRect->mY1) &&
		(apTarget->mY1 > apClipRect->mY0) &&
		(apTarget->mX0 != apTarget->mX1) &&
		(apTarget->mY0 != apTarget->mY1)		)
	{
		if( apTarget->mX0 < apClipRect->mX0 )
		{
			apTarget->mX0 = apClipRect->mX0;
		}
		if( apTarget->mX1 > apClipRect->mX1 )
		{
			apTarget->mX1 = apClipRect->mX1;
		}
		if( apTarget->mY0 < apClipRect->mY0 )
		{
			apTarget->mY0 = apClipRect->mY0;
		}
		if( apTarget->mY1 > apClipRect->mY1 )
		{
			apTarget->mY1 = apClipRect->mY1;
		}

		lRes = 1;
	}

	return( lRes );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_FontDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect )
* ACTION   : Achieve_Gfx_FontDraw
* CREATION : 9.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_FontDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect )
{
	if( apText && apPos )
	{
		S16		lLen;
		U8 *	lpDst;
		sAchieveGfxPos	lPos;

		lLen = 0;
		while( apText[ lLen ] )
		{
			lLen++;
		}

		lPos.mX = apPos->mX;
		switch( aAlign )
		{
		case eACH_GFX_ALIGN_CENTRE:
			lPos.mX = apPos->mX - (lLen<<2);
			break;
		case eACH_GFX_ALIGN_LEFT:
			lPos.mX = apPos->mX;
			break;
		case eACH_GFX_ALIGN_RIGHT:
			lPos.mX = apPos->mX - (lLen<<3);
			break;
		}
		lPos.mY = apPos->mY;

		if( !aColour )
		{
			lLen += 3;
			lPos.mX -= 8;
		}

		if( ( lPos.mX < apClipRect->mX1 ) &&
			((lPos.mX + (lLen<<3)) > apClipRect->mX0 )&&
			( lPos.mY < apClipRect->mY1) &&
			((lPos.mY + 8) > apClipRect->mY0) )
		{
			const char * lpText = apText;
			S16	lClipY;
			S16	lH;
			lpDst   = (U8*)&apScreen[ dACH_GFX_YOFF(lPos.mY) + dACH_GFX_XOFF(lPos.mX) ];

			if( lPos.mX < apClipRect->mX0 )
			{
				S16	lClipX = (apClipRect->mX0-lPos.mX);

				lClipX >>= 3;
				if( lClipX > lLen )
				{
					return;
				}
				lpText = &apText[ lClipX ];
			}

			if( (lPos.mX + (lLen<<3)) > apClipRect->mX1 )
			{
				S16	lClipX = ( (lPos.mX + (lLen<<3)) - apClipRect->mX1 );

				lClipX >>= 3;
				lLen    -= lClipX;

				if( lLen <= 0 )
				{
					return;
				}
			}

			if( (lPos.mY + 8) > apClipRect->mY1 )
			{
				lH = apClipRect->mY1 - lPos.mY;
			}
			else
			{
				lH = 8;
			}

			if( lPos.mY < apClipRect->mY0 )
			{
				lClipY = apClipRect->mY0-lPos.mY;
				lH    -= lClipY;
				if( lH <= 0 )
				{
					return;
				}
				lPos.mY += lClipY;
			}
			else
			{
				lClipY = 0;
			}
			if( lClipY >= 8 )
			{
				return;
			}


			lpDst = (U8*)(&apScreen[ dACH_GFX_YOFF( lPos.mY ) + dACH_GFX_XOFF( lPos.mX ) ] );
			if( lPos.mX & 8 )
			{
				lpDst++;
			}

			while( lLen )
			{
				U16		lChar;
				S16		lLoopH;
				U8 *	lpSrc;
				U8 *	lpDst2;

				lChar   = (U16)((*lpText++ - 32) & 0xFF);
				lChar <<=3;
				lpSrc   = &gFont8x8[ lChar ];

				lLoopH = lH;
				lpSrc += lClipY;
				lpDst2 = lpDst;

				if( aColour )
				{
					while( lLoopH )
					{
						*lpDst2 = *lpSrc++;
						lpDst2 += 160L;
						lLoopH--;
					}
				}
				else
				{
					while( lLoopH )
					{
						*lpDst2 = 0;
						lpDst2 += 160L;
						lLoopH--;
					}
				}

				if( lPos.mX & 8 )
				{
					lpDst += 7;
				}
				else
				{
					lpDst++;
				}

				lPos.mX += 8;
				lLen--;
			}
		}
	}
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_CursorDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect )
* ACTION   : Achieve_Gfx_CursorDraw
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_CursorDraw( const char * apText, U16 * apScreen, const U16 aColour, const U8 aAlign, const sAchieveGfxPos * apPos, const sAchieveGfxQuad * apClipRect )
{
	(void)aColour;
	if( apText && apPos )
	{
		S16		lLen;
		U8 *	lpDst;
		sAchieveGfxPos	lPos;

		lLen = 0;
		while( apText[ lLen ] )
		{
			lLen++;
		}

		lPos.mX = apPos->mX;
		switch( aAlign )
		{
		case eACH_GFX_ALIGN_CENTRE:
			lPos.mX = apPos->mX - (lLen<<2);
			break;
		case eACH_GFX_ALIGN_LEFT:
			lPos.mX = apPos->mX;
			break;
		case eACH_GFX_ALIGN_RIGHT:
			lPos.mX = apPos->mX - (lLen<<3);
			break;
		}
		lPos.mY = apPos->mY;

		lPos.mX += (lLen<<3);
		if( lLen >= 16 )
		{
			lPos.mX -= 8;
		}

		if( ( lPos.mX < apClipRect->mX1 ) &&
			((lPos.mX + 8) > apClipRect->mX0 )&&
			( lPos.mY < apClipRect->mY1) &&
			((lPos.mY + 8) > apClipRect->mY0) )
		{
			S16	lH;

			lpDst = (U8*)(&apScreen[ dACH_GFX_YOFF( lPos.mY ) + dACH_GFX_XOFF( lPos.mX ) ] );
			if( lPos.mX & 8 )
			{
				lpDst++;
			}
			if( (lPos.mY + 8) > apClipRect->mY1 )
			{
				lH = apClipRect->mY1 - lPos.mY;
			}
			else
			{
				lH = 8;
			}

			while( lH )
			{
				*lpDst = ~*lpDst;
				lpDst += 160L;
				lH--;
			}
		}
	}
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_GetPal( U16 * apPal )
* ACTION   : Achieve_Gfx_GetPal
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_GetPal( U16 * apPal )
{
	(void)apPal;
#ifdef	dGODLIB_PLATFORM_ATARI
	memcpy( apPal, (void*)0xFFFF8240L, 32 );
#endif
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_SetPal( const U16 * apPal )
* ACTION   : Achieve_Gfx_SetPal
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_SetPal( const U16 * apPal )
{
	(void)apPal;
#ifdef	dGODLIB_PLATFORM_ATARI
	memcpy( (void*)0xFFFF8240L, apPal, 32 );
#endif
}

/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Beam_Thread( sThread * apThread )
* ACTION   : Achieve_Gfx_Beam_Thread
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Gfx_Beam_Thread( sThread * apThread )
{
	mTHREAD_BEGIN( apThread );

	mTHREAD_WAIT_UNTIL( apThread, gAchieveGfxClass.mBeamsActiveFlag );
	gAchieveGfxClass.mBeamsActiveFlag = 0;

	gAchieveGfxClass.mBeams[ 0 ] = gAchieveGfxBeamTopPos0;
	gAchieveGfxClass.mBeams[ 1 ] = gAchieveGfxBeamBotPos0;

	do
	{
		gAchieveGfxClass.mBeams[ 0 ].mX1 += dACH_BEAM_SPEED0;
		gAchieveGfxClass.mBeams[ 1 ].mX0 -= dACH_BEAM_SPEED0;
		Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 0 ], 1 );
		Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 1 ], 1 );
		mTHREAD_YIELD( apThread);
	} while( gAchieveGfxClass.mBeams[ 1 ].mX0 > 0 );
	Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 0 ], 1 );
	Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 1 ], 1 );

	mTHREAD_WAIT_UNTIL( apThread, gAchieveGfxClass.mBeamsActiveFlag );
	gAchieveGfxClass.mBeamsActiveFlag = 0;

	gAchieveGfxClass.mBeams[ 0 ] = gAchieveGfxBeamTopPos1;
	gAchieveGfxClass.mBeams[ 1 ] = gAchieveGfxBeamBotPos1;

	do
	{
		gAchieveGfxClass.mBeams[ 0 ].mX0 -= dACH_BEAM_SPEED1;
		gAchieveGfxClass.mBeams[ 1 ].mX1 += dACH_BEAM_SPEED1;
		Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 0 ], 0 );
		Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 1 ], 0 );
		mTHREAD_YIELD( apThread);
	} while( gAchieveGfxClass.mBeams[ 0 ].mX0 > 0 );
	Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 0 ], 0 );
	Achieve_Gfx_HLine_Render( gAchieveGfxClass.mpLogic, &gAchieveGfxClass.mBeams[ 1 ], 0 );

	mTHREAD_SUSPEND( apThread );

	mTHREAD_END( apThread );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Gfx_Beam_Begin( void )
* ACTION   : Achieve_Gfx_Beam_Begin
* CREATION : 13.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Gfx_Beam_Begin( void )
{
	gAchieveGfxClass.mBeamsActiveFlag = 1;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Input_Update( const U8 aPacket, const U8 aKey )
* ACTION   : Achieve_Input_Update
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

void	Achieve_Input_Update( const U8 aPacket, const U8 aKey )
{
	U8	lPacket = aPacket;
	U8	lKey = aKey;


	if( aKey & 128 )
	{
		lKey = 0;
	}

	if( eIKBDSCAN_DOWNARROW == lKey )
	{
		lPacket |= eACH_INPUT_PACKET_DOWN;
	}
	else if( eIKBDSCAN_UPARROW == lKey )
	{
		lPacket |= eACH_INPUT_PACKET_UP;
	}
	else if( eIKBDSCAN_LEFTARROW == lKey )
	{
		lPacket |= eACH_INPUT_PACKET_LEFT;
	}
	else if( eIKBDSCAN_RIGHTARROW == lKey )
	{
		lPacket |= eACH_INPUT_PACKET_RIGHT;
	}
	else if( ( eIKBDSCAN_RETURN == lKey ) || ( eIKBDSCAN_NUMPADENTER == lKey ) )
	{
		lPacket |= eACH_INPUT_PACKET_FIRE;
	}

	gAchieveGfxClass.mInputHit    = (~gAchieveGfxClass.mInputPacket) & lPacket;
	gAchieveGfxClass.mInputUnHit  = (gAchieveGfxClass.mInputPacket)  & (~lPacket);
	gAchieveGfxClass.mInputPacket = aPacket;
	gAchieveGfxClass.mInputKey    = lKey;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Input_GetPacket( void )
* ACTION   : Achieve_Input_GetPacket
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Input_GetPacket( void )
{
	return( gAchieveGfxClass.mInputPacket );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Input_GetKey( void )
* ACTION   : Achieve_Input_GetKey
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Input_GetKey( void )
{
	return( gAchieveGfxClass.mInputKey );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Input_GetHit( void )
* ACTION   : Achieve_Input_GetHit
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Input_GetHit( void )
{
	return( gAchieveGfxClass.mInputHit );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Achieve_Input_GetUnHit( void )
* ACTION   : Achieve_Input_GetUnHit
* CREATION : 14.6.2009 PNK
*-----------------------------------------------------------------------------------*/

U8	Achieve_Input_GetUnHit( void )
{
	return( gAchieveGfxClass.mInputUnHit );
}



/* ################################################################################ */

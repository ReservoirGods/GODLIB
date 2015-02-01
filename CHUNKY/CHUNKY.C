/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"CHUNKY.H"

#include	<GODLIB/FONT/FONT.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_Blit( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_Blit
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_Blit( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	S16		lWidth,lHeight;
	U8 *	lpSrc0;
	U8 *	lpSrc1;
	U8 *	lpDst0;
	U8 *	lpDst1;
	
	lHeight = apRect->mHeight;

	lpSrc0  = (U8*)apSrc->mpVRAM;
	lpSrc0 += apRect->mY * apSrc->mWidth;
	lpSrc0 += apRect->mX;

	lpDst0  = (U8*)apCanvas->mpVRAM;
	lpDst0 += apCoords->mY * apCanvas->mWidth;
	lpDst0 += apCoords->mX;

	while( lHeight-- )
	{
		lpSrc1 = lpSrc0;
		lpDst1 = lpDst0;
		lWidth = apRect->mWidth;
		while( lWidth-- )
		{
			*lpDst1++ = *lpSrc1++;
		}
		lpSrc0 += apSrc->mWidth;
		lpDst0 += apCanvas->mWidth;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_ClearScreen( sGraphicCanvas * apCanvas )
* ACTION   : ChunkySurface_ClearScreen
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_ClearScreen( sGraphicCanvas * apCanvas )
{
	S16		lWidth,lHeight;
	U8 *	lpDst;

	lHeight = apCanvas->mHeight;
	lpDst   = (U8*)apCanvas->mpVRAM;
	while( lHeight-- )
	{
		lWidth  = apCanvas->mWidth;
		while( lWidth-- )
		{
			*lpDst++ = 0;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_ConvertBlit( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_ConvertBlit
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_ConvertBlit( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	switch( apSrc->mColourMode )
	{
	case	eGRAPHIC_COLOURMODE_1PLANE:
		break;
	case	eGRAPHIC_COLOURMODE_2PLANE:
		break;
	case	eGRAPHIC_COLOURMODE_4PLANE:
		ChunkySurface_From4Plane( apCanvas,apCoords, apRect, apSrc );
		break;
	case	eGRAPHIC_COLOURMODE_8PLANE:
		break;
	case	eGRAPHIC_COLOURMODE_8BPP:
		ChunkySurface_Blit( apCanvas, apCoords, apRect, apSrc );
		break;
	case	eGRAPHIC_COLOURMODE_16BPP:
	case	eGRAPHIC_COLOURMODE_24BPP:
	case	eGRAPHIC_COLOURMODE_32BPP:
		break;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_CopyScreen( sGraphicCanvas * apCanvas,void * apSrc )
* ACTION   : ChunkySurface_CopyScreen
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_CopyScreen( sGraphicCanvas * apCanvas,void * apSrc )
{
	S16		lWidth,lHeight;
	U8 *	lpSrc0;
	U8 *	lpSrc1;
	U8 *	lpDst0;
	U8 *	lpDst1;
	
	lHeight = apCanvas->mHeight;

	lpSrc0  = (U8*)apSrc;
	lpDst0  = (U8*)apCanvas->mpVRAM;

	while( lHeight-- )
	{
		lpSrc1 = lpSrc0;
		lpDst1 = lpDst0;
		lWidth = apCanvas->mWidth;
		while( lWidth-- )
		{
			*lpDst1++ = *lpSrc1++;
		}
		lpSrc0 += apCanvas->mWidth;
		lpDst0 += apCanvas->mWidth;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawBox( sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawBox
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawBox( sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
{
	S16		lWidth,lHeight;
	U8 *	lpDst0;
	U8 *	lpDst1;
	
	lHeight = apCoords->mHeight;

	lpDst0  = (U8*)apCanvas->mpVRAM;
	lpDst0 += apCoords->mY * apCanvas->mWidth;
	lpDst0 += apCoords->mX;

	while( lHeight-- )
	{
		lpDst1 = lpDst0;
		lWidth = apCoords->mWidth;
		while( lWidth-- )
		{
			*lpDst1++ = (U8)aColour;
		}
		lpDst0 += apCanvas->mWidth;
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawLine( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawLine
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawLine( sGraphicCanvas * apCanvas,sGraphicBox * apCoords,S16 aColour )
{
	S16		lX0,lY0,lX1,lY1;
	S16		lDy,lDx;
	S16		lStepX,lStepY;
	S16		lFrac;
	U8 *	lpDst;

	lX0   = apCoords->mX0;
	lY0   = apCoords->mY0;
	lX1   = apCoords->mX1;
	lY1   = apCoords->mY1;
	lDx   = (S16)(lX1 - lX0);
	lDy   = (S16)(lY1 - lY0);
	lpDst = (U8*)apCanvas->mpVRAM;

	if (lDy < 0) 
	{ 
		lDy    = (S16)(-lDy);
		lStepY = (S16)-apCanvas->mWidth;
	} 
	else 
	{ 
		lStepY = apCanvas->mWidth;
	}

	if (lDx < 0) 
	{ 
		lDx    = (S16)(-lDx); 
		lStepX = -1; 
	} 
	else 
	{ 
		lStepX = 1; 
	}

	lDy <<= 1;
	lDx <<= 1;

	lY0 = (S16)(lY0 * apCanvas->mWidth);
	lY1 = (S16)(lY1 * apCanvas->mWidth);

	lpDst[ lX0 + lY0 ] = (U8)aColour;

	if( lDx > lDy ) 
	{
		lFrac = (S16)(lDy - (lDx >> 1));
		while( lX0 != lX1 ) 
		{
			if( lFrac >= 0 ) 
			{
				lY0   = (S16)(lY0 + lStepY);
				lFrac = (S16)(lFrac - lDx);
			}
			lX0   = (S16)(lX0 + lStepX);
			lFrac = (S16)(lFrac + lDy);
			lpDst[ lX0 + lY0 ] = (U8)aColour;
		}
	} 
	else 
	{
        lFrac = (S16)(lDx - (lDy >> 1));
        while (lY0 != lY1) 
		{
            if (lFrac >= 0) 
			{
                lX0   = (S16)(lX0   + lStepX);
                lFrac = (S16)(lFrac - lDy);
            }
            lY0   = (S16)(lY0 + lStepY);
            lFrac = (S16)(lFrac + lDx);
            lpDst[ lX0 + lY0 ] = (U8)aColour;
        }
    }	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawPixel( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawPixel
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawPixel( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	U8 *	lpDst;	

	lpDst  = (U8*)apCanvas->mpVRAM;
	lpDst += apCoords->mY * apCanvas->mWidth;
	lpDst += apCoords->mX;

	*lpDst = (U8)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawSprite( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
* ACTION   : ChunkySurface_DrawSprite
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawSprite( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
{
	U8 *		lpGfx;
	U8 *		lpDst0;
	U8 *		lpDst1;
	U8			lPixel;
	sSprite *	lpSprite;
	U16			lWidth,lHeight;

	lpSprite = (sSprite*)apSprite;
	lpGfx    = (U8*)lpSprite->mpGfx;
	lHeight  = lpSprite->mHeight;
	lpDst0   = (U8*)apCanvas->mpVRAM;
	lpDst0  += (apCoords->mY * apCanvas->mWidth);
	lpDst0  += apCoords->mX;

	while( lHeight-- )
	{
		lWidth  = lpSprite->mWidth;
		lpDst1  = lpDst0;
		while( lWidth-- )
		{
			lPixel = *lpGfx++;
			if( lPixel != 0xFF )
			{
				*lpDst1 = lPixel;
			}
			lpDst1++;
		}
		lpDst0 += apCanvas->mWidth;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawTri( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawTri
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawTri( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawQuad( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawQuad
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawQuad( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_FontPrint( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apFont,const char * apString )
* ACTION   : ChunkySurface_FontPrint
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_FontPrint( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apFont,const char * apString )
{
	(void)apCanvas;
	(void)apCoords;
	(void)apFont;
	(void)apString;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_Blit_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_Blit_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_Blit_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	sGraphicPos		lCoords;
	sGraphicRect	lRect;
	S16				lDiff;

	lCoords = *apCoords;
	lRect   = *apRect;

	if( ( lCoords.mX < (S16)apCanvas->mWidth  ) && 
		( lCoords.mY < (S16)apCanvas->mHeight ) &&
		( lRect.mX   < (S16)apSrc->mWidth     ) &&
		( lRect.mY   < (S16)apSrc->mHeight    ) )
	{

		if( lCoords.mX < 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth + lCoords.mX);
			lCoords.mX   = 0;
		}
		if( lCoords.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight + lCoords.mY);
			lCoords.mY     = 0;
		}

		if( lRect.mX < 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth + lRect.mX);
			lCoords.mX   = (S16)(lCoords.mX   - lRect.mX);
			lRect.mX     = 0;
		}

		if( lRect.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight + lRect.mY);
			lCoords.mY    = (S16)(lCoords.mY    - lRect.mY);
			lRect.mY      = 0;
		}

		lDiff = (S16)((lRect.mX + lRect.mWidth) - apSrc->mWidth);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lRect.mY + lRect.mHeight) - apSrc->mHeight); 
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		lDiff = (S16)((lCoords.mX + lRect.mWidth) - apCanvas->mWidth);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lCoords.mY + lRect.mHeight) - apCanvas->mHeight);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		if( ( lRect.mHeight > 0 ) && (lRect.mWidth > 0 ) )
		{
			ChunkySurface_Blit( apCanvas, &lCoords, &lRect, apSrc );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_ConvertBlit_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_ConvertBlit_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_ConvertBlit_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	sGraphicPos		lCoords;
	sGraphicRect	lRect;
	S16				lDiff;

	lCoords = *apCoords;
	lRect   = *apRect;

	if( ( lCoords.mX < (S16)apCanvas->mWidth  ) && 
		( lCoords.mY < (S16)apCanvas->mHeight ) &&
		( lRect.mX   < (S16)apSrc->mWidth     ) &&
		( lRect.mY   < (S16)apSrc->mHeight    ) )
	{

		if( lCoords.mX < 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth + lCoords.mX);
			lCoords.mX   = 0;
		}
		if( lCoords.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight + lCoords.mY);
			lCoords.mY    = 0;
		}

		if( lRect.mX < 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth + lRect.mX);
			lCoords.mX   = (S16)(lCoords.mX - lRect.mX);
			lRect.mX     = 0;
		}

		if( lRect.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight + lRect.mY);
			lCoords.mY    = (S16)(lCoords.mY - lRect.mY);
			lRect.mY      = 0;
		}

		lDiff = (S16)((lRect.mX + lRect.mWidth) - apSrc->mWidth);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lRect.mY + lRect.mHeight) - apSrc->mHeight);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		lDiff = (S16)((lCoords.mX + lRect.mWidth) - apCanvas->mWidth);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lCoords.mY + lRect.mHeight) - apCanvas->mHeight);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		if( ( lRect.mHeight > 0 ) && (lRect.mWidth > 0 ) )
		{
			ChunkySurface_ConvertBlit( apCanvas, &lCoords, &lRect, apSrc );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawBox_Clip( sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawBox_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawBox_Clip( sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
{
	sGraphicRect	lRect;
	S16				lDiff;

	lRect   = *apCoords;

	if( ( lRect.mX < apCanvas->mClipBox.mX1  ) && 
		( lRect.mY < apCanvas->mClipBox.mY1 ) )
	{

		lDiff = lRect.mX = apCanvas->mClipBox.mX0;
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
			lRect.mX     = apCanvas->mClipBox.mX0;
		}

		lDiff = lRect.mY = apCanvas->mClipBox.mY0;
		if( lRect.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
			lRect.mY      = apCanvas->mClipBox.mY0;
		}

		lDiff = (S16)((lRect.mX + lRect.mWidth) - apCanvas->mClipBox.mX1);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lRect.mY + lRect.mHeight) - apCanvas->mClipBox.mY1); 
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		if( ( lRect.mHeight > 0 ) && (lRect.mWidth > 0 ) )
		{
			ChunkySurface_DrawBox( apCanvas, &lRect, aColour );
		}
	}
	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawLine_Clip( sGraphicCanvas * apCanvas,sGraphicBox * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawLine_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawLine_Clip( sGraphicCanvas * apCanvas,sGraphicBox * apCoords,S16 aColour )
{
	sGraphicBox	lBox;
	S16			lDx,lDy;
	S16			lStep;

	if( apCoords->mX0 < apCoords->mX1 )
	{
		lBox = *apCoords;
	}
	else
	{
		lBox.mX0 = apCoords->mX1;
		lBox.mY0 = apCoords->mY1;
		lBox.mX1 = apCoords->mX0;
		lBox.mY1 = apCoords->mY0;
	}


	if( (lBox.mX0 < (S16)apCanvas->mWidth) && (lBox.mX1 >= 0) )
	{

		lDx  = (S16)(lBox.mX1 - lBox.mX0);
		lDy  = (S16)(lBox.mY1 - lBox.mY0);


		if( lBox.mX0 < 0 )
		{
			lStep    = (S16)(lBox.mX0 * lDy);
			lStep    = (S16)(lStep/lDx);
			lBox.mY0 = (S16)(lBox.mY0-lStep);
			lBox.mX0 = 0;
		}

		lStep = (S16)(lBox.mX1 - (apCanvas->mWidth-1));
		if( lStep > 0 )
		{
			lStep    = (S16)(lStep * lDy);
			lStep    = (S16)(lStep / lDx);
			lBox.mX0 = (S16)(apCanvas->mWidth-1);
			lBox.mY0 = (S16)(lBox.mY0 -lStep);
		}

		if( lBox.mY0 > lBox.mY1 )
		{
			lStep    = lBox.mX0;
			lBox.mX0 = lBox.mX1;
			lBox.mX1 = lStep;

			lStep    = lBox.mY0;
			lBox.mY0 = lBox.mY1;
			lBox.mY1 = lStep;
		}

		if( (lBox.mY0 < (S16)apCanvas->mHeight) && (lBox.mY1 >= 0) )
		{
			if( lBox.mY0 < 0 )
			{
				lStep     = (S16)(lBox.mY0 * lDx);
				lStep     = (S16)(lStep/lDy);
				lBox.mX0  = (S16)(lBox.mX0-lStep);
				lBox.mY0  = 0;
			}

			lStep = (S16)(lBox.mY1 - (apCanvas->mHeight-1));
			if( lStep > 0 )
			{
				lStep    = (S16)(lStep*lDx);
				lStep    = (S16)(lStep/lDy);
				lBox.mX1 = (S16)(lBox.mX1-lStep);
				lBox.mY1 = (S16)(apCanvas->mHeight-1);
			}
			ChunkySurface_DrawLine( apCanvas, &lBox, aColour );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawPixel_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawPixel_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawPixel_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	if( ( apCoords->mX >= apCanvas->mClipBox.mX0 ) &&
		( apCoords->mY >= apCanvas->mClipBox.mY0 ) &&
		( apCoords->mX  < apCanvas->mClipBox.mX1 ) &&
		( apCoords->mY  < apCanvas->mClipBox.mY1 ) )
	{
		ChunkySurface_DrawPixel( apCanvas, apCoords, aColour );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawSprite_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
* ACTION   : ChunkySurface_DrawSprite_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawSprite_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
{
	U8 *		lpGfx;
	U8 *		lpDst0;
	U8 *		lpDst1;
	sSprite *	lpSprite;
	U16			lWidth,lHeight;
	S16			lX,lY;
	S16			lDiff;
	U8			lPixel;

	lX       = apCoords->mX;
	lY       = apCoords->mY;

	if( (lX < (S16)apCanvas->mWidth) && ( lY < (S16)apCanvas->mHeight ) )
	{
		lpSprite = (sSprite*)apSprite;
		lWidth   = lpSprite->mWidth;
		lHeight  = lpSprite->mHeight;
		lpGfx    = (U8*)lpSprite->mpGfx;

		if( lX < 0 )
		{
			lWidth = (S16)(lWidth + lX);
			lpGfx  -= lX;
			lX      = 0;
		}
		if( lY < 0 )
		{
			lHeight = (S16)(lHeight+lY);
			lpGfx   += (-lY * lpSprite->mWidth);
			lY       = 0;
		}

		lDiff = (S16)((lX + lWidth) - apCanvas->mWidth);
		if( lDiff )
		{
			lWidth = (S16)(lWidth-lDiff);
		}

		lDiff = (S16)((lY + lHeight) - apCanvas->mHeight);
		if( lDiff )
		{
			lHeight = (S16)(lHeight-lDiff);
		}

		if( (lWidth>0) && (lHeight>0) )
		{


			lpDst0   = (U8*)apCanvas->mpVRAM;
			lpDst0  += (lY * apCanvas->mWidth);
			lpDst0  += lX;

			while( lHeight-- )
			{
				lX      = lWidth;
				lpDst1  = lpDst0;
				while( lX-- )
				{
					lPixel = *lpGfx++;
					if( lPixel != 0xFF )
					{
						*lpDst1 = lPixel;
					}
					lpDst1++;
				}
				lpDst0 += apCanvas->mWidth;
			}
		}
	}	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawTri_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawTri_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawTri_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_DrawQuad_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : ChunkySurface_DrawQuad_Clip
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_DrawQuad_Clip( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_From4Plane( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_From4Plane
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_From4Plane( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	S16		lWidth,lHeight;
	U8 *	lpDst;
	U16 *	lpSrc;
	
	(void)apSrc;

	lHeight = apRect->mHeight;
	lpDst   = (U8*)apCanvas->mpVRAM;
	lpDst  += (apCoords->mY * apCanvas->mWidth);
	lpDst  += apCoords->mX;

	lpSrc   = (U16*)apCanvas->mpVRAM;
	lpSrc  += (apCanvas->mLineOffsets[apRect->mY]>>1);
	lpSrc  += (apRect->mX >> 4);
	while( lHeight-- )
	{
		lWidth = apRect->mWidth;
		while( lWidth-- )
		{
		}
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : ChunkySurface_To4Plane( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
* ACTION   : ChunkySurface_To4Plane
* CREATION : 04.04.2004 PNK
*-----------------------------------------------------------------------------------*/

void	ChunkySurface_To4Plane( sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,sGraphicCanvas * apSrc )
{
	(void)apCanvas;
	(void)apCoords;
	(void)apRect;
	(void)apSrc;
}


/* ################################################################################ */

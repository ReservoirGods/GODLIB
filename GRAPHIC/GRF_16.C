/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GRF_16.H"

#include	<GODLIB/MEMORY/MEMORY.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/SPRITE/SPRITE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

U16		Graphic_16BPP_GetPixel( const sGraphicCanvas * apCanvas, const S16 aX, const S16 aY );
void	Graphic_16BPP_SetPixel( const sGraphicCanvas * apCanvas, const S16 aX, const S16 aY, const U16 aPixel );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_Blit( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,struct sGraphicCanvas * apSrc )
* ACTION   : Graphic_16BPP_Blit
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_Blit( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,struct sGraphicCanvas * apSrc )
{
	S16	lSX,lSY;
	S16	lDX,lDY;
	S16	lW,lH;


	lSY = apRect->mY;
	lDY = apCoords->mY;
	lH  = apRect->mHeight;

	while( lH > 0 )
	{
		lW  = apRect->mWidth;
		lSX = apRect->mX;
		lDX = apCoords->mX;
		while( lW > 0 )
		{
			Graphic_16BPP_SetPixel( apCanvas, lDX, lDY, Graphic_16BPP_GetPixel( apSrc, lSX, lSY ) );
			lSX++;
			lDX++;
			lW--;
		}
		lSY++;
		lDY++;
		lH--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_ClearScreen( struct sGraphicCanvas * apCanvas )
* ACTION   : Graphic_16BPP_ClearScreen
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_ClearScreen( struct sGraphicCanvas * apCanvas )
{
	U32	lSize;

	lSize  = apCanvas->mWidth;
	lSize *= apCanvas->mHeight;
	lSize += lSize;

	Memory_Clear( lSize, apCanvas->mpVRAM );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_CopyScreen( struct sGraphicCanvas * apCanvas,void * apSrc )
* ACTION   : Graphic_16BPP_CopyScreen
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_CopyScreen( struct sGraphicCanvas * apCanvas,void * apSrc )
{
	U32	lSize;

	lSize  = apCanvas->mWidth;
	lSize *= apCanvas->mHeight;
	lSize += lSize;

	Memory_Copy( lSize, apSrc, apCanvas->mpVRAM );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawBox( struct sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawBox
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawBox( struct sGraphicCanvas * apCanvas,sGraphicRect * apRect,S16 aColour )
{
	S16	lX,lY;
	S16	lW,lH;


	lY = apRect->mY;
	lH = apRect->mHeight;

	while( lH > 0 )
	{
		lW = apRect->mWidth;
		lX = apRect->mX;
		while( lW > 0 )
		{
			Graphic_16BPP_SetPixel( apCanvas, lX, lY, aColour );
			lX++;
			lW--;
		}
		lY++;
		lH--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawLine( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawLine
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawLine( struct sGraphicCanvas * apCanvas,sGraphicBox * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawPixel( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawPixel
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawPixel( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	Graphic_16BPP_SetPixel( apCanvas, apCoords->mX, apCoords->mY, aColour );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawSprite( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
* ACTION   : Graphic_16BPP_DrawSprite
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawSprite( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
{
	U16 *		lpDst;
	U16 *		lpSrc;
	U16 *		lpMsk;
	U16			lOff;
	U16			lBits;
	U16			lMskCount;
	U16			lW,lH;
	sSprite *	lpSprite;

	lpSprite = (sSprite*)apSprite;
	lpDst    = (U16*)apCanvas->mpVRAM;
	lpDst   +=(apCoords->mY * apCanvas->mWidth);
	lpDst   += apCoords->mX;
	lpSrc    = lpSprite->mpGfx;
	lpMsk    = lpSprite->mpMask;
	lH       = lpSprite->mHeight;
	lOff     = (U16)(apCanvas->mWidth - lpSprite->mWidth);
	lBits    = 0;
	while( lH )
	{
		lW = lpSprite->mWidth;
		lMskCount = 0;
		while( lW )
		{
			if( lpSprite->mMaskPlaneCount )
			{
				if( !lMskCount )
				{
					Endian_ReadBigU16( lpMsk, lBits );
					lpMsk++;
					lMskCount = 16;
				}
				lMskCount--;
				if( 0x8000 & lBits )
				{
					lpDst++;
					lpSrc++;
				}
				else
				{
					*lpDst++ = *lpSrc++;
				}
				lBits <<= 1;
			}
			else
			{
				*lpDst++ = *lpSrc++;
			}
			lW--;
		}
		lpDst += lOff;
		lH--;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawTri( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawTri
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawTri( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawQuad( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawQuad
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawQuad( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_Blit_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,struct sGraphicCanvas * apSrc )
* ACTION   : Graphic_16BPP_Blit_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_Blit_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,sGraphicRect * apRect,struct sGraphicCanvas * apSrc )
{
	sGraphicPos		lCoords;
	sGraphicRect	lRect;
	S16				lDiff;

	lCoords = *apCoords;
	lRect   = *apRect;

	if( ( lCoords.mX                   < apCanvas->mClipBox.mX1 ) &&
		( lCoords.mY                   < apCanvas->mClipBox.mY1 ) &&
		( lCoords.mX + apRect->mWidth  > apCanvas->mClipBox.mX0 ) &&
		( lCoords.mY + apRect->mHeight > apCanvas->mClipBox.mY0 ) &&
		( lRect.mX                     < apSrc->mClipBox.mX1    ) &&
		( lRect.mY                     < apSrc->mClipBox.mY1    ) &&
		( lRect.mX + apRect->mWidth    > apSrc->mClipBox.mX0    ) &&
		( lRect.mY + apRect->mHeight   > apSrc->mClipBox.mY0    )
		)
	{

		lDiff = (S16)(apCanvas->mClipBox.mX0 - lCoords.mX);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
			lRect.mX     = (S16)(lRect.mX + lDiff);
			lCoords.mX   = apCanvas->mClipBox.mX0;
		}

		lDiff = (S16)(apSrc->mClipBox.mX0 - lRect.mX);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
			lCoords.mX   = (S16)(lCoords.mX + lDiff);
			lRect.mX     = 0;
		}

		lDiff = (S16)(apCanvas->mClipBox.mY0 - lCoords.mY);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
			lRect.mY      = (S16)(lRect.mY + lDiff);
			lCoords.mY    = apCanvas->mClipBox.mY0;
		}

		lDiff = (S16)(apSrc->mClipBox.mY0 - lRect.mY);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
			lCoords.mY    = (S16)(lCoords.mY + lDiff);
			lRect.mY      = apSrc->mClipBox.mY0;
		}

		lDiff = (S16)((lRect.mX + lRect.mWidth) - apSrc->mClipBox.mX1);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth - lDiff);
		}

		lDiff = (S16)((lRect.mY + lRect.mHeight) - apSrc->mClipBox.mY1);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight - lDiff);
		}

		lDiff = (S16)((lCoords.mX + lRect.mWidth) - apCanvas->mClipBox.mX1);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth-lDiff);
		}

		lDiff = (S16)((lCoords.mY + lRect.mHeight) - apCanvas->mClipBox.mY1);
		if( lDiff > 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight-lDiff);
		}

		if( ( lRect.mHeight > 0 ) && (lRect.mWidth > 0 ) )
		{
			Graphic_16BPP_Blit( apCanvas, &lCoords, &lRect, apSrc );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawBox_Clip( struct sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawBox_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawBox_Clip( struct sGraphicCanvas * apCanvas,sGraphicRect * apCoords,S16 aColour )
{
	sGraphicRect	lRect;
	S16				lDiff;

	lRect   = *apCoords;

	if( ( lRect.mX < apCanvas->mClipBox.mX1  ) &&
		( lRect.mY < apCanvas->mClipBox.mY1 ) )
	{

		lDiff = (S16)(apCanvas->mClipBox.mX0 - lRect.mX);
		if( lDiff > 0 )
		{
			lRect.mWidth = (S16)(lRect.mWidth-lDiff);
			lRect.mX     = apCanvas->mClipBox.mX0;
		}

		lDiff = (S16)(apCanvas->mClipBox.mY0 - lRect.mY);
		if( lRect.mY < 0 )
		{
			lRect.mHeight = (S16)(lRect.mHeight-lDiff);
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
			lRect.mHeight = (S16)(lRect.mHeight-lDiff);
		}

		if( ( lRect.mHeight > 0 ) && (lRect.mWidth > 0 ) )
		{
			Graphic_16BPP_DrawBox( apCanvas, &lRect, aColour );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawLine_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawLine_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawLine_Clip( struct sGraphicCanvas * apCanvas,sGraphicBox * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawPixel_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawPixel_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawPixel_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	if( ( apCoords->mX >= apCanvas->mClipBox.mX0 ) &&
		( apCoords->mY >= apCanvas->mClipBox.mY0 ) &&
		( apCoords->mX  < apCanvas->mClipBox.mX1 ) &&
		( apCoords->mY  < apCanvas->mClipBox.mY1 ) )
	{
		Graphic_16BPP_DrawPixel( apCanvas, apCoords, aColour );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawSprite_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
* ACTION   : Graphic_16BPP_DrawSprite_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawSprite_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apSprite )
{
	U16 *		lpDst;
	U16 *		lpSrc;
	U16 *		lpMsk;
	U16			lOff;
	U16			lBits;
	U16			lMskCount;
	U16			lW,lH;
	sGraphicPos	lPos;
	S16			lSprW,lSprH;
	sSprite *	lpSprite;

	lpSprite = (sSprite*)apSprite;
	lSprW = lpSprite->mWidth;
	lSprH = lpSprite->mHeight;
	lPos  = *apCoords;

	if( lPos.mX < 0 )
	{
		lSprW = (S16)(lSprW + lPos.mX);
		lPos.mX = 0;
	}
	if( lPos.mY < 0 )
	{
		lSprH = (S16)(lSprH + lPos.mY);
		lPos.mY = 0;
	}
	lW  = apCanvas->mWidth;
	lW  = (S16)(lW - (lPos.mX + lSprW));
	if( lW < 0 )
	{
		lSprW = (S16)(lSprW + lW);
	}
	lH  = apCanvas->mHeight;
	lH  = (S16)(lH - (lPos.mY + lSprH));
	if( lH < 0 )
	{
		lSprH = (S16)(lSprH + lH);
	}

	if( (lSprW>0) && (lSprH>0) &&
		(lPos.mX < (S16)apCanvas->mWidth) &&
		(lPos.mY < (S16)apCanvas->mHeight) )
	{
		lpDst    = (U16*)apCanvas->mpVRAM;
		lpDst   +=(lPos.mY * apCanvas->mWidth);
		lpDst   += lPos.mX;

		lpSrc    = lpSprite->mpGfx;
		lpMsk    = lpSprite->mpMask;
		lH       = lSprH;
		lOff     = (U16)(apCanvas->mWidth - lpSprite->mWidth);
		lBits    = 0;
		while( lH )
		{
			lW = lSprW;
			lMskCount = 0;
			while( lW )
			{
				if( !lMskCount )
				{
					lBits = *lpMsk++;
					lMskCount = 16;
				}
				lMskCount--;
				if( 0x8000 & lBits )
				{
					lpDst++;
					lpSrc++;
				}
				else
				{
					*lpDst++ = *lpSrc++;
				}
				lW--;
			}
			lpDst += lOff;
			lH--;
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawTri_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawTri_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawTri_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_DrawQuad_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
* ACTION   : Graphic_16BPP_DrawQuad_Clip
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_DrawQuad_Clip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,S16 aColour )
{
	(void)apCanvas;
	(void)apCoords;
	(void)aColour;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_GetPixel( const sGraphicCanvas * apCanvas,const S16 aX,const S16 aY )
* ACTION   : Graphic_16BPP_GetPixel
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

U16	Graphic_16BPP_GetPixel( const sGraphicCanvas * apCanvas,const S16 aX,const S16 aY )
{
	U32		lOff;
	U16 *	lpPixel;
	U16		lPixel;

	lOff  = aY;
	lOff *=(apCanvas->mWidth);
	lOff += aX;

	lpPixel  = (U16*)apCanvas->mpVRAM;
	lpPixel += lOff;


	Endian_ReadBigU16( lpPixel, lPixel );
	return( lPixel );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_16BPP_SetPixel( const sGraphicCanvas * apCanvas,const S16 aX,const S16 aY,const U16 aPixel )
* ACTION   : Graphic_16BPP_SetPixel
* CREATION : 25.03.2005 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_16BPP_SetPixel( const sGraphicCanvas * apCanvas,const S16 aX,const S16 aY,const U16 aPixel )
{
	U32		lOff;
	U16 *	lpPixel;

	lOff  = aY;
	lOff *=(apCanvas->mWidth);
	lOff += aX;

	lpPixel  = (U16*)apCanvas->mpVRAM;
	lpPixel += lOff;

	Endian_WriteBigU16( lpPixel, aPixel );
}


/* ################################################################################ */

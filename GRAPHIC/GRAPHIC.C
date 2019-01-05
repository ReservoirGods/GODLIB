/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: GRAPHIC.C
::
:: Display Independent Graphic Routs
::
:: [c] 2002 Reservoir Gods
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"GRAPHIC.H"
#ifdef dGODLIB_16BPP
#include	"GRF_16.H"
#endif

#include	<GODLIB/ASSERT/ASSERT.H>
#include	<GODLIB/BLITTER/BLITTER.H>
#include	<GODLIB/SYSTEM/SYSTEM.H>
#include	<GODLIB/FONT/FONT.H>

#ifdef dGODLIB_CHUNKY
#include	<GODLIB/CHUNKY/CHUNKY.H>
#endif

/* ###################################################################################
#  DATA
################################################################################### */

sGraphicFuncs	gGraphicFuncs[ eGRAPHIC_COLOURMODE_LIMIT ];
sGraphicFuncs	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_LIMIT ];
U8				gGraphicBlitterEnableFlag = 0;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Graphic_FontPrint(  sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,    void * apFont, const char * apString );
void	Graphic_FontPrintClip(  sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,    void * apFont, const char * apString );

extern	void Graphic_4BP_Blit(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
extern	void Graphic_4BP_ClearScreen(  struct sGraphicCanvas * apCanvas );
extern	void Graphic_4BP_CopyScreen(   struct sGraphicCanvas * apCanvas, void * apSrc );
extern	void Graphic_4BP_DrawBox(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
extern	void Graphic_4BP_DrawLine(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawPixel(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawSprite(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
extern	void Graphic_4BP_DrawTri(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawQuad(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

extern	void Graphic_4BP_Blit_Clip(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
extern	void Graphic_4BP_DrawBox_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
extern	void Graphic_4BP_DrawLine_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawPixel_Clip(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawSprite_Clip(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
extern	void Graphic_4BP_DrawTri_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawQuad_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

extern	void Graphic_4BP_Blit_BLT(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
extern	void Graphic_4BP_ClearScreen_BLT(  struct sGraphicCanvas * apCanvas );
extern	void Graphic_4BP_CopyScreen_BLT(   struct sGraphicCanvas * apCanvas, void * apSrc );
extern	void Graphic_4BP_DrawBox_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
extern	void Graphic_4BP_DrawLine_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawPixel_BLT(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawSprite_BLT(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
extern	void Graphic_4BP_DrawTri_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawQuad_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

extern	void Graphic_4BP_Blit_Clip_BLT(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
extern	void Graphic_4BP_DrawBox_Clip_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
extern	void Graphic_4BP_DrawLine_Clip_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawPixel_Clip_BLT(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawSprite_Clip_BLT(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
extern	void Graphic_4BP_DrawTri_Clip_BLT(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
extern	void Graphic_4BP_DrawQuad_Clip_BLT(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );


/* ###################################################################################
#  CODE
################################################################################### */


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_Init
* ACTION   :
* CREATION : 01.02.02 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_Init( void )
{
	Blitter_Init();

	Graphic_SetBlitterEnable( gGraphicBlitterEnableFlag );
/*
	if( BLT_BLITTER == System_GetBLT() )
	{
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite_BLT;

		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit_Clip;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox_Clip_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel_Clip_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite_Clip_BLT;
	}
	else
*/

#ifdef dGODLIB_CHUNKY
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].Blit        = ChunkySurface_Blit;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].ClearScreen = ChunkySurface_ClearScreen;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].CopyScreen  = ChunkySurface_CopyScreen;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].DrawBox     = ChunkySurface_DrawBox;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].DrawLine    = ChunkySurface_DrawLine;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].DrawPixel   = ChunkySurface_DrawPixel;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_8BPP ].DrawSprite  = ChunkySurface_DrawSprite;

	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].Blit        = ChunkySurface_Blit_Clip;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].ClearScreen = ChunkySurface_ClearScreen;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].CopyScreen  = ChunkySurface_CopyScreen;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].DrawBox     = ChunkySurface_DrawBox_Clip;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].DrawLine    = ChunkySurface_DrawLine_Clip;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].DrawPixel   = ChunkySurface_DrawPixel_Clip;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_8BPP ].DrawSprite  = ChunkySurface_DrawSprite_Clip;
#endif

#ifdef dGODLIB_16BPP
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].Blit        = Graphic_16BPP_Blit;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].ClearScreen = Graphic_16BPP_ClearScreen;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].CopyScreen  = Graphic_16BPP_CopyScreen;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].DrawBox     = Graphic_16BPP_DrawBox;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].DrawLine    = Graphic_16BPP_DrawLine;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].DrawPixel   = Graphic_16BPP_DrawPixel;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].DrawSprite  = Graphic_16BPP_DrawSprite;
	gGraphicFuncs[ eGRAPHIC_COLOURMODE_16BPP ].FontPrint   = Graphic_FontPrint;
#endif

	gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].FontPrint = Graphic_FontPrint;
	gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].FontPrint = Graphic_FontPrintClip;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_DeInit( void )
* ACTION   : Graphic_DeInit
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_DeInit( void )
{
	Blitter_DeInit();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_SetBlitterEnable( U8 aFlag )
* ACTION   : sets graphics routines to either use blitter or CPU
* CREATION : 05.01.2018 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_SetBlitterEnable( U8 aFlag )
{
	gGraphicBlitterEnableFlag = aFlag;

	if(  aFlag && (BLT_BLITTER == System_GetBLT()) )
	{
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel_BLT;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite_BLT;

		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit_Clip;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox_Clip_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel_Clip_BLT;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite_Clip_BLT;
	}
	else
	{
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel;
		gGraphicFuncs[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite;

		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].Blit        = Graphic_4BP_Blit_Clip;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].ClearScreen = Graphic_4BP_ClearScreen;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].CopyScreen  = Graphic_4BP_CopyScreen;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawBox     = Graphic_4BP_DrawBox_Clip;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawPixel   = Graphic_4BP_DrawPixel_Clip;
		gGraphicFuncsClip[ eGRAPHIC_COLOURMODE_4PLANE ].DrawSprite  = Graphic_4BP_DrawSprite_Clip;

		gGraphicBlitterEnableFlag = 0;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_GetBlitterEnable( void )
* ACTION   : returns if blitter is enabled or not for graphic rendering
* CREATION : 05.01.2018 PNK
*-----------------------------------------------------------------------------------*/

U8		Graphic_GetBlitterEnable( void )
{
	return( gGraphicBlitterEnableFlag );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GraphicCanvas_Init( sGraphicCanvas * apCanvas,const U16 aColourMode,const U16 aWidth,const U16 aHeight )
* ACTION   : GraphicCanvas_Init
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	GraphicCanvas_Init( sGraphicCanvas * apCanvas,const U16 aColourMode,const U16 aWidth,const U16 aHeight )
{
	U16		i;
	U32		lLineWidth;
	U32		lOffset;

	apCanvas->mColourMode = aColourMode;
	apCanvas->mWidth      = aWidth;
	apCanvas->mHeight     = aHeight;

	switch( aColourMode )
	{
	case	eGRAPHIC_COLOURMODE_1PLANE:
		lLineWidth   = (aWidth+15);
		lLineWidth  &= 0x0000FFF0L;
		lLineWidth >>= 3L;
		break;

	case	eGRAPHIC_COLOURMODE_2PLANE:
		lLineWidth   = (aWidth+15);
		lLineWidth  &= 0x0000FFF0L;
		lLineWidth >>= 2L;
		break;

	case	eGRAPHIC_COLOURMODE_4PLANE:
		lLineWidth   = (aWidth+15);
		lLineWidth  &= 0x0000FFF0L;
		lLineWidth >>= 1L;
		break;

	case	eGRAPHIC_COLOURMODE_8BPP:
		lLineWidth = aWidth;
		break;

	case	eGRAPHIC_COLOURMODE_16BPP:
		lLineWidth   = aWidth;
		lLineWidth <<= 1L;
		break;

	case	eGRAPHIC_COLOURMODE_24BPP:
		lLineWidth  = aWidth;
		lLineWidth *= 3;
		break;

	case	eGRAPHIC_COLOURMODE_32BPP:
		lLineWidth   = aWidth;
		lLineWidth <<= 2L;
		break;
	default:
		lLineWidth = aWidth;
		break;
	}

	lOffset = 0;

	if( apCanvas->mHeight >= (dGRAPHIC_HEIGHT_LIMIT-1) )
	{
		apCanvas->mHeight = dGRAPHIC_HEIGHT_LIMIT-1;
	}

	for( i=0; i<dGRAPHIC_HEIGHT_LIMIT; i++ )
	{
		apCanvas->mLineOffsets[ i ] = lOffset;
		lOffset += lLineWidth;
	}

	apCanvas->mpFuncs     = &gGraphicFuncs[ aColourMode ];
	apCanvas->mpClipFuncs = &gGraphicFuncsClip[ aColourMode ];
	apCanvas->mClipBox.mX0 = 0;
	apCanvas->mClipBox.mY0 = 0;
	apCanvas->mClipBox.mX1 = apCanvas->mWidth;
	apCanvas->mClipBox.mY1 = apCanvas->mHeight;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GraphicCanvas_SetpVRAM( sGraphicCanvas * apCanvas,void * apVRAM )
* ACTION   : GraphicCanvas_SetpVRAM
* CREATION : 04.01.2003 PNK
*-----------------------------------------------------------------------------------*/

void	GraphicCanvas_SetpVRAM( sGraphicCanvas * apCanvas,void * apVRAM )
{
	apCanvas->mpVRAM = apVRAM;
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_FontPrint( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,struct sFont * apFont, const char * apString )
* ACTION   : Graphic_FontPrint
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_FontPrint( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apFont, const char * apString )
{
	sGraphicPos	lPos;
	sFont *		lpFont;
	sSprite *	lpSprite;

	lpFont = (sFont*)apFont;

	lPos.mX = apCoords->mX;
	lPos.mY = apCoords->mY;

	while( *apString )
	{
		if( ' ' == *apString )
		{
			lPos.mX = (S16)(lPos.mX + lpFont->mSpaceWidth);
		}
		else
		{
			lpSprite = Font_GetpSprite( lpFont, *apString );
			lPos.mX  = (S16)(lPos.mX- Font_GetCharX0( lpFont, *apString ));
			if( lpSprite )
			{
				apCanvas->mpFuncs->DrawSprite( apCanvas, &lPos, lpSprite );
			}
			lPos.mX = (S16)(lPos.mX + Font_GetCharX1( lpFont, *apString ) +1);
		}
		lPos.mX = (S16)(lPos.mX + lpFont->mKerning);
		apString++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_FontPrintClip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,struct sFont * apFont, const char * apString )
* ACTION   : Graphic_FontPrintClip
* CREATION : 21.01.2004 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_FontPrintClip( struct sGraphicCanvas * apCanvas,sGraphicPos * apCoords,void * apFont, const char * apString )
{
	sGraphicPos	lPos;
	sFont *		lpFont;
	sSprite *	lpSprite;

	lPos.mX = apCoords->mX;
	lPos.mY = apCoords->mY;

	lpFont = (sFont*)apFont;

	while( *apString )
	{
		if( ' ' == *apString )
		{
			lPos.mX = (S16)(lPos.mX + lpFont->mSpaceWidth);
		}
		else
		{
			lpSprite = Font_GetpSprite( lpFont, *apString );
			lPos.mX  = (S16)(lPos.mX - Font_GetCharX0( lpFont, *apString ));
			if( lpSprite )
			{
				apCanvas->mpClipFuncs->DrawSprite( apCanvas, &lPos, lpSprite );
			}
			lPos.mX = (S16)(lPos.mX + Font_GetCharX1( lpFont, *apString ) + 1);
		}
		lPos.mX = (S16)(lPos.mX +lpFont->mKerning);
		apString++;
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_FontPrintLeft( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString )
* ACTION   : Graphic_FontPrintLeft
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_FontPrintLeft( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString )
{
	sGraphicPos	lPos;

	if( apCanvas && apFont && apString )
	{
		lPos.mX = apRect->mX;
		lPos.mY = apRect->mY;

		apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, apFont, apString );
	}

}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_FontPrintRight( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString )
* ACTION   : Graphic_FontPrintRight
* CREATION : 30.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_FontPrintRight( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString )
{
	sGraphicPos	lPos;

	if( apCanvas && apFont && apString )
	{
		lPos.mX = apRect->mX + apRect->mWidth - (Font_GetStringWidth(apFont,apString));
		lPos.mY = apRect->mY;

		apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, apFont, apString );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : Graphic_FontPrintCentred( sGraphicCanvas * apCanvas, sFont * apFont, sGraphicRect * apRect, const char * apString )
* ACTION   : Graphic_FontPrintCentred
* CREATION : 16.1.2008 PNK
*-----------------------------------------------------------------------------------*/

void	Graphic_FontPrintCentred( sGraphicCanvas * apCanvas, sFont * apFont, sGraphicRect * apRect, const char * apString )
{
	sGraphicPos	lPos;

	if( apCanvas && apFont && apString )
	{
		lPos.mX = apRect->mX + ((apRect->mWidth>>1) - (Font_GetStringWidth(apFont,apString)>>1));
		lPos.mY = apRect->mY;

		apCanvas->mpFuncs->FontPrint( apCanvas, &lPos, apFont, apString );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : GraphicCanvas_CentringRender( sGraphicCanvas * apCanvas,const U16 aColour )
* ACTION   : GraphicCanvas_CentringRender
* CREATION : 03.04.2005 PNK
*-----------------------------------------------------------------------------------*/

#define	dGC_CENTRE_LINESIZE	4
#define	dGC_CENTRE_MIDSIZE	8

void	GraphicCanvas_CentringRender( sGraphicCanvas * apCanvas,const U16 aColour )
{
	sGraphicRect	lRect;
	S16				lW;
	S16				lH;

	if( apCanvas )
	{
		lW = (S16)(apCanvas->mWidth  / dGC_CENTRE_LINESIZE);
		lH = (S16)(apCanvas->mHeight / dGC_CENTRE_LINESIZE);

		lW = (S16)(( lW + lH ) >> 1 );
		lH = lW;

		apCanvas->mpFuncs->ClearScreen( apCanvas );

		lRect.mX      = 0;
		lRect.mY      = 0;
		lRect.mWidth  = lW;
		lRect.mHeight = 1;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mY      = (S16)(apCanvas->mHeight-1);
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mX = (S16)(apCanvas->mWidth-lW);
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mY = 0;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );

		lRect.mX      = 0;
		lRect.mY      = 0;
		lRect.mWidth  = 1;
		lRect.mHeight = lH;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mX = (S16)(apCanvas->mWidth-1);
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mY = (S16)(apCanvas->mHeight-lH);
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
		lRect.mX = 0;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );

		lW = (S16)(apCanvas->mWidth  / dGC_CENTRE_MIDSIZE);
		lH = (S16)(apCanvas->mHeight / dGC_CENTRE_MIDSIZE);

		lW = (S16)(( lW + lH ) >> 1 );
		lH = lW;

		lRect.mX      = (S16)( apCanvas->mWidth >> 1 );
		lRect.mY      = (S16)( (apCanvas->mHeight >> 1) - (lH >> 1) );
		lRect.mHeight = lH;
		lRect.mWidth  = 1;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );

		lRect.mX      = (S16)( (apCanvas->mWidth >> 1) - (lW >> 1) );
		lRect.mY      = (S16)(apCanvas->mHeight >> 1);
		lRect.mHeight = 1;
		lRect.mWidth  = lW;
		apCanvas->mpClipFuncs->DrawBox( apCanvas, &lRect, aColour );
	}
}

void	Graphic_ChunkList_Clear( sGraphicChunkList * apList )
{
	apList->mActiveCount = 0;
}

void	Graphic_ChunkList_Store( sGraphicChunkList * apList, sGraphicCanvas * apCanvas, const sGraphicRect * apRect )
{
	U16 lX0 = ( apRect->mX ) >> 4;
	U16 lX1 = (  apRect->mX + apRect->mWidth - 1 ) >> 4;

	U16 lY = apRect->mY;
	U16 lH = apRect->mHeight;
	U16 lOff = (U16)apCanvas->mLineOffsets[ lY ];
	S16 lCount = (lX1-lX0) + 1;

	sGraphicChunk * lpChunk = &apList->mChunks[ apList->mActiveCount ];
	lOff += (lX0<<3);

	Assert( ( apList->mActiveCount + lCount ) < apList->mChunkCount );

	apList->mActiveCount += lCount;

	while( lCount )
	{
		lpChunk->mHeight = lH;
		lpChunk->mOffset = lOff;
		lOff += 8;
		lpChunk++;
		lCount--;
	}
}


void	Graphic_ChunkList_ReStore( sGraphicChunkList * apList, sGraphicCanvas * apSrc, sGraphicCanvas * apDst )
{
	S16 lCount;
	sGraphicChunk * lpChunk;
	U8 * lpS;
	U8 * lpD;
	U16	lAddSrc;
	U16 lAddDst;

	lpS = (U8*)apSrc->mpVRAM;
	lpD = (U8*)apDst->mpVRAM;

	lAddSrc = (U16)(apSrc->mLineOffsets[ 1 ]>>1);
	lAddDst = (U16)(apSrc->mLineOffsets[ 1 ]>>1);

	lCount = apList->mActiveCount;


	lpChunk = &apList->mChunks[ 0 ];
	while( lCount )
	{
		U16 * lpSrc;
		U16 * lpDst;
		U16 lOff;
		U16 lHeight;

		lOff = lpChunk->mOffset;
		lpSrc = (U16*)( lpS + lOff );
		lpDst = (U16*)( lpD + lOff );

		lHeight = lpChunk->mHeight;
		while( lHeight )
		{
			lpDst[ 0 ] = lpSrc[ 0 ];
			lpDst[ 1 ] = lpSrc[ 1 ];
			lpDst[ 2 ] = lpSrc[ 2 ];
			lpDst[ 3 ] = lpSrc[ 3 ];
/*
			lpDst[ 0 ] = 0xFFFF;
			lpDst[ 1 ] = 0xFFFF;
			lpDst[ 2 ] = 0xFFFF;
			lpDst[ 3 ] = 0xFFFF;
*/
			lpSrc += lAddSrc;
			lpDst += lAddDst;

			lHeight--;
		}

		lpChunk++;
		lCount--;
	}

	apList->mActiveCount = 0;
}


/* ################################################################################ */

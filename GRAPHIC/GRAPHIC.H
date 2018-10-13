#ifndef	INCLUDED_GRAPHIC_H
#define	INCLUDED_GRAPHIC_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/FONT/FONT.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dGRAPHIC_HEIGHT_LIMIT	485


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eGRAPHIC_COLOURMODE_1PLANE,
	eGRAPHIC_COLOURMODE_2PLANE,
	eGRAPHIC_COLOURMODE_4PLANE,
	eGRAPHIC_COLOURMODE_8PLANE,
	eGRAPHIC_COLOURMODE_8BPP,
	eGRAPHIC_COLOURMODE_16BPP,
	eGRAPHIC_COLOURMODE_24BPP,
	eGRAPHIC_COLOURMODE_32BPP,

	eGRAPHIC_COLOURMODE_LIMIT
};


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef struct
{
	S16	mX;
	S16	mY;
} sGraphicPos;

typedef struct
{
	S16	mX;
	S16	mY;
	S16	mWidth;
	S16	mHeight;
} sGraphicRect;

typedef	struct	sGraphicBox
{
	S16	mX0;
	S16	mX1;
	S16	mY0;
	S16	mY1;
} sGraphicBox;

typedef struct sGraphicChunk
{
	U16 mOffset;
	U16 mHeight;
} sGraphicChunk;

typedef struct sGraphicChunkList
{
	U16 mActiveCount;
	U16 mChunkCount;
	sGraphicChunk			mChunks[32];
} sGraphicChunkList;

#define mGODLIB_CHUNKLIST( _aName, _aSize )	U16 _aName[ (_aSize<<1) + 2 ] = { 0, _aSize }


struct	sGraphicCanvas;
struct	sFont;


typedef struct sGraphicFuncs
{
	void (*	Blit )(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
	void (*	ClearScreen )(  struct sGraphicCanvas * apCanvas );
	void (*	CopyScreen )(   struct sGraphicCanvas * apCanvas, void * apSrc );
	void (*	ConvertBlit )(  struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
	void (* DrawBox )(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
	void (* DrawLine )(     struct sGraphicCanvas * apCanvas,  sGraphicBox * apCoords,   S16 aColour );
	void (* DrawPixel )(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
	void (* DrawSprite )(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
	void (* DrawTri  )(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
	void (* DrawQuad )(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
	void (* FontPrint )(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apFont, const char * apString );
} sGraphicFuncs;


typedef	struct sGraphicCanvas
{
	void *			mpVRAM;
	U16				mColourMode;
	U16				mWidth;
	U16				mHeight;
	U16				mpad;
	sGraphicBox		mClipBox;
	sGraphicFuncs *	mpFuncs;
	sGraphicFuncs *	mpClipFuncs;
	U32				mLineOffsets[ dGRAPHIC_HEIGHT_LIMIT ];
} sGraphicCanvas;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Graphic_Init( void );
void	Graphic_DeInit( void );

void	GraphicCanvas_Init(     sGraphicCanvas * apCanvas, const U16 aColourMode, const U16 aWidth, const U16 aHeight );
void	GraphicCanvas_SetpVRAM( sGraphicCanvas * apCanvas, void * apVRAM );
void	GraphicCanvas_CentringRender( sGraphicCanvas * apCanvas, const U16 aColour );

void	Graphic_FontPrintLeft( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString );
void	Graphic_FontPrintRight( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString );
void	Graphic_FontPrintCentred( sGraphicCanvas * apCanvas, struct sFont * apFont, sGraphicRect * apRect, const char * apString );

void	Graphic_ChunkList_Clear( sGraphicChunkList * apList );
void	Graphic_ChunkList_Store( sGraphicChunkList * apList, sGraphicCanvas * apCanvas, const sGraphicRect * apRect );
void	Graphic_ChunkList_ReStore( sGraphicChunkList * apList, sGraphicCanvas * apSrc, sGraphicCanvas * apDst );

#define GraphicCanvas_Blit( apCanvas, apCoords, apRect, apSrc )				apCanvas->mpFuncs->Blit( apCanvas, apCoords, apRect, apSrc )
#define GraphicCanvas_ClearScreen( apCanvas )								apCanvas->mpFuncs->ClearScreen( apCanvas )
#define GraphicCanvas_CopyScreen( apCanvas, apSrc )							apCanvas->mpFuncs->CopyScreen( apCanvas, apSrc )
#define GraphicCanvas_ConvertBlit( apCanvas, apCoords, apRect, apSrc )		apCanvas->mpFuncs->ConvertBlit( apCanvas, apCoords, apRect, apSrc )
#define GraphicCanvas_DrawBox( apCanvas, apCoords, aColour )				apCanvas->mpFuncs->DrawBox( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawLine( apCanvas, apCoords, aColour )				apCanvas->mpFuncs->DrawLine( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawPixel( apCanvas, apCoords, aColour )				apCanvas->mpFuncs->DrawPixel( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawSprite( apCanvas, apCoords, apSprite )			apCanvas->mpFuncs->DrawSprite( apCanvas, apCoords, apSprite )
#define GraphicCanvas_DrawTri( apCanvas, apCoords, aColour )				apCanvas->mpFuncs->DrawTri( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawQuad( apCanvas, apCoords, aColour )				apCanvas->mpFuncs->DrawQuad( apCanvas, apCoords, aColour )
#define GraphicCanvas_FontPrint( apCanvas, apCoords, apFont, apString )		apCanvas->mpFuncs->FontPrint( apCanvas, apCoords, apFont, apString )

#define GraphicCanvas_Blit_Clip( apCanvas, apCoords, apRect, apSrc )			apCanvas->mpClipFuncs->Blit( apCanvas, apCoords, apRect, apSrc )
#define GraphicCanvas_ClearScreen_Clip( apCanvas )								apCanvas->mpClipFuncs->ClearScreen( apCanvas )
#define GraphicCanvas_CopyScreen_Clip( apCanvas, apSrc )						apCanvas->mpClipFuncs->CopyScreen( apCanvas, apSrc )
#define GraphicCanvas_ConvertBlit_Clip( apCanvas, apCoords, apRect, apSrc )		apCanvas->mpClipFuncs->ConvertBlit( apCanvas, apCoords, apRect, apSrc )
#define GraphicCanvas_DrawBox_Clip( apCanvas, apCoords, aColour )				apCanvas->mpClipFuncs->DrawBox( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawLine_Clip( apCanvas, apCoords, aColour )				apCanvas->mpClipFuncs->DrawLine( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawPixel_Clip( apCanvas, apCoords, aColour )				apCanvas->mpClipFuncs->DrawPixel( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawSprite_Clip( apCanvas, apCoords, apSprite )			apCanvas->mpClipFuncs->DrawSprite( apCanvas, apCoords, apSprite )
#define GraphicCanvas_DrawTri_Clip( apCanvas, apCoords, aColour )				apCanvas->mpClipFuncs->DrawTri( apCanvas, apCoords, aColour )
#define GraphicCanvas_DrawQuad_Clip( apCanvas, apCoords, aColour )				apCanvas->mpClipFuncs->DrawQuad( apCanvas, apCoords, aColour )
#define GraphicCanvas_FontPrint_Clip( apCanvas, apCoords, apFont, apString )	apCanvas->mpClipFuncs->FontPrint( apCanvas, apCoords, apFont, apString )

/* ################################################################################ */

#endif	/* INCLUDED_GRAPHIC_H */

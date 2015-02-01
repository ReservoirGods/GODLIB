#ifndef	INCLUDED_CHUNKY_H
#define	INCLUDED_CHUNKY_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void ChunkySurface_Blit(          sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);
void ChunkySurface_ClearScreen(   sGraphicCanvas * apCanvas );
void ChunkySurface_ConvertBlit(   sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);
void ChunkySurface_CopyScreen(    sGraphicCanvas * apCanvas, void * apSrc );
void ChunkySurface_DrawBox(       sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
void ChunkySurface_DrawLine(      sGraphicCanvas * apCanvas,  sGraphicBox * apCoords,   S16 aColour );
void ChunkySurface_DrawPixel(     sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void ChunkySurface_DrawSprite(    sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
void ChunkySurface_DrawTri(       sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void ChunkySurface_DrawQuad(      sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

void ChunkySurface_Blit_Clip(          sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);
void ChunkySurface_ConvertBlit_Clip(   sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);
void ChunkySurface_DrawBox_Clip(       sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
void ChunkySurface_DrawLine_Clip(      sGraphicCanvas * apCanvas,  sGraphicBox * apCoords,   S16 aColour );
void ChunkySurface_DrawPixel_Clip(     sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void ChunkySurface_DrawSprite_Clip(    sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
void ChunkySurface_DrawTri_Clip(       sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void ChunkySurface_DrawQuad_Clip(      sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

void ChunkySurface_From4Plane(   sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);
void ChunkySurface_To4Plane(     sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect,  sGraphicCanvas * apSrc	);


/* ################################################################################ */

#endif	/* INCLUDED_CHUNKY_H */


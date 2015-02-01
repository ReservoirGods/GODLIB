#ifndef	INCLUDED_GRF_4_H
#define	INCLUDED_GRF_4_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void Graphic_4BP_Blit(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
void Graphic_4BP_ClearScreen(  struct sGraphicCanvas * apCanvas );
void Graphic_4BP_CopyScreen(   struct sGraphicCanvas * apCanvas, void * apSrc );
void Graphic_4BP_DrawBox(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
void Graphic_4BP_DrawLine(     struct sGraphicCanvas * apCanvas,  sGraphicBox * apCoords,   S16 aColour );
void Graphic_4BP_DrawPixel(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void Graphic_4BP_DrawSprite(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
void Graphic_4BP_DrawTri(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void Graphic_4BP_DrawQuad(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );

void Graphic_4BP_Blit_Clip(         struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,  sGraphicRect * apRect, struct sGraphicCanvas * apSrc	);
void Graphic_4BP_DrawBox_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicRect * apCoords,  S16 aColour );
void Graphic_4BP_DrawLine_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicBox * apCoords,   S16 aColour );
void Graphic_4BP_DrawPixel_Clip(    struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void Graphic_4BP_DrawSprite_Clip(   struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   void * apSprite );
void Graphic_4BP_DrawTri_Clip(      struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );
void Graphic_4BP_DrawQuad_Clip(     struct sGraphicCanvas * apCanvas,  sGraphicPos * apCoords,   S16 aColour );


/* ################################################################################ */

#endif	/* INCLUDED_GRF_4_H */

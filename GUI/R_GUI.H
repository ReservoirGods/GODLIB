#ifndef	INCLUDED_R_GUI_H
#define	INCLUDED_R_GUI_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/GUI/GUI.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

typedef	void	(*fRenderGui)( sGraphicCanvas * apLogic, sGuiButton * apButton, const U16 aRedrawFlag );

void	RenderGui_Init( void );
void	RenderGui_DeInit( void );

void	RenderGui_SetCustomRender( fRenderGui aCustomRender );
void	RenderGui_Update( sGraphicCanvas * apLogic );

U16		RenderGui_GetScreenIndex( void );


/* ################################################################################ */

#endif	/* INCLUDED_R_GUI_H */

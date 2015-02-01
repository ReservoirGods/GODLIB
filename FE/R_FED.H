#ifndef	INCLUDED_R_FED_H
#define	INCLUDED_R_FED_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/FE/FED.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	RenderFed_Init( void );
void	RenderFed_DeInit( void );

void	RenderFed_Update( sGraphicCanvas * apLogic, sGraphicCanvas * apBack );

void	RenderFed_IntroInit( sFedTransition * apTrans );
void	RenderFed_OutroInit( sFedTransition * apTrans );
U8		RenderFed_IsTransitionComplete( void );


/* ################################################################################ */

#endif	/* INCLUDED_R_FED_H */

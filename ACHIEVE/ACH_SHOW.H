
#ifndef	INCLUDED_ACH_SHOW_H
#define	INCLUDED_ACH_SHOW_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/PACKAGE.H>
#include	<GODLIB/CLOCK/CLOCK.H>
#include	<GODLIB/GRAPHIC/GRAPHIC.H>
#include	<GODLIB/INPUT/INPUT.H>


/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eACH_SHOW_MODE_STATS,
	eACH_SHOW_MODE_TASKS,
	eACH_SHOW_MODE_NEW,
};


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Show_AppInit( void );
void	Achieve_Show_AppDeInit( void );

void	Achieve_Show_Init( void );
void	Achieve_Show_DeInit( void );
U16		Achieve_Show_Update( sInput * apInput );

void	Achieve_Show_ButtonRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, const U16 aTaskIndex );
void	Achieve_Show_SymbolRender( sGraphicCanvas * apCanvas, sGraphicPos * apPos, uU32 * apFrame, const U16 aTaskIndex );


/* ################################################################################ */

#endif	/*	INCLUDED_ACH_SHOW_H	*/

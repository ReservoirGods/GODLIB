
#ifndef	INCLUDED_ACH_UNLK_H
#define	INCLUDED_ACH_UNLK_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/PACKAGE.H>
#include	<GODLIB/CLOCK/CLOCK.H>
#include	<GODLIB/INPUT/INPUT.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_UnLocked_AppInit( void );
void	Achieve_UnLocked_AppDeInit( void );

void	Achieve_UnLocked_Init( void );
void	Achieve_UnLocked_DeInit( void );

U16		Achieve_UnLocked_Update( sInput * apInput );


/* ################################################################################ */

#endif	/*	INCLUDED_ACH_UNLK_H	*/

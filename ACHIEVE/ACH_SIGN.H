
#ifndef	INCLUDED_ACH_SIGN_H
#define	INCLUDED_ACH_SIGN_H

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

void	Achieve_SignIn_AppInit( void );
void	Achieve_SignIn_AppDeInit( void );

void	Achieve_SignIn_Init( void );
void	Achieve_SignIn_DeInit( void );

U16		Achieve_SignIn_Update( sInput * apInput );


/* ################################################################################ */

#endif	/*	INCLUDED_SIGN_H	*/

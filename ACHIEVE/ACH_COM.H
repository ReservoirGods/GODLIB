
#ifndef	INCLUDED_ACH_COM_H
#define	INCLUDED_ACH_COM_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Achieve_Compiler_AppInit( void );
void	Achieve_Compiler_AppDeInit( void );

void	Achieve_Compiler_Compile( const char * apSrcFileName, const char * apDstFileName );
void	Achieve_Compiler_DeCompile( const char * apSrcFileName, const char * apDstFileName );


/* ################################################################################ */

#endif	/*	INCLUDED_SIGN_H	*/

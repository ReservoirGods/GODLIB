#ifndef	INCLUDED_ENCRYPT_H
#define	INCLUDED_ENCRYPT_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Encrypt_Scramble( void * apData, const U32 aSize, const U32 aKey ) ;
void	Encrypt_DeScramble( void * apData, const U32 aSize, const U32 aKey );


/* ################################################################################ */

#endif	/* INCLUDED_ENCRYPT_H */

#ifndef	INCLUDED_RANDOM_H
#define	INCLUDED_RANDOM_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

typedef struct sRandomSeed
{
	U32	mSeed;
} sRandomSeed;

void		Random_Init( void );
void		Random_DeInit( void );
void		Random_Update( void );

U16			Random_GetClamped( U16 aMax );
U32			Random_Get( void );


void		RandomSeed_Init( sRandomSeed * apSeed );
void		RandomSeed_Update( sRandomSeed * apSeed );
U16			RandomSeed_GetClamped( sRandomSeed * apSeed, U16 aMax );

/* ################################################################################ */

#endif	/*	INCLUDED_RANDOM_H	*/

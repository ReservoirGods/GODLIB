#ifndef	INCLUDED_SSD_H
#define	INCLUDED_SSD_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	Ssd_Init( void );
void	Ssd_DeInit( void );

void	Ssd_Start( void * apVoiceSet, void * apTune );
void	Ssd_Stop( void );

void	Ssd_SetSongFreq( const U16 aHz );


/* ################################################################################ */

#endif	/* INCLUDED_SSD_H */

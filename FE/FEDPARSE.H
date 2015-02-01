#ifndef	INCLUDED_FEDPARSE_H
#define	INCLUDED_FEDPARSE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/FE/FED.H>


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	FedParse_Init( void );
void	FedParse_DeInit( void );

void	FedParse_Text(const char * apText,const U32 aSize);

sFedHeader *	FedParse_GetpFed( void );
U32				FedParse_GetSize( void );
void			FedParse_ShowInfo( void );
void			FedParse_ShowIncomplete( void );
void			FedParse_ShowFedInfo( void );


/* ################################################################################ */

#endif	/* INCLUDED_FEDPARSE_H */

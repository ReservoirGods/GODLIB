#ifndef	INCLUDED_GSM_H
#define	INCLUDED_GSM_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/PICTYPES/CANVAS.H>


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sGsmHeader
{
	U32	mID;
	U16	mColourMode;
	U16	mBlockXSize;
	U16	mBlockYSize;
	U16	mBlockCount;
} sGsmHeader;

typedef	struct	sGsm
{
	sGsmHeader	mHeader;
	U16			mData[ 1 ];
} sGsm;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void		Gsm_Relocate( sGsm * apGsm );
void		Gsm_Delocate( sGsm * apGsm );

sGsm *		Gsm_FromCanvas( sCanvas * apCanvas );
sCanvas *	Gsm_ToCanvas( sGsm * apGsm );


/* ################################################################################ */

#endif	/* INCLUDED_GSM_H */

#ifndef	INCLUDED_RELOCATE_H
#define	INCLUDED_RELOCATE_H

/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	<GODLIB/BASE/BASE.H>
#include	<GODLIB/ASSET/ASSET.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

typedef	U32	(*fReloc)( void * apData, const U32 aSize, const U32 aID );


/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct sRelocater
{
	U32					mExtID;
	fReloc				IsType;
	fReloc				DoInit;
	fReloc				DoDeInit;
	fReloc				DoRelocate;
	fReloc				DoDelocate;
	struct sRelocater *	mpNext;
} sRelocater;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

sRelocater *	Relocater_Create( const char * apExt, fReloc aIsType, fReloc aDoInit, fReloc aDoDeInit, fReloc aDoRelocate, fReloc aDoDelocate );
void			Relocater_Destroy( sRelocater * apReloc );

void			Relocater_Init( sRelocater * apReloc, const char * apExt, fReloc aIsType, fReloc aDoInit, fReloc aDoDeInit, fReloc aDoRelocate, fReloc aDoDelocate );
void			Relocater_DeInit( sRelocater * apReloc );

sRelocater *	RelocaterManager_Find( sAsset * apAsset );

U32				RelocaterManager_DoInit(     sAsset * apAsset );
U32				RelocaterManager_DoDeInit(   sAsset * apAsset );
U32				RelocaterManager_DoDelocate( sAsset * apAsset );
U32				RelocaterManager_DoRelocate( sAsset * apAsset );

void			RelocaterManager_Init( void );
void			RelocaterManager_DeInit( void );


/* ################################################################################ */

#endif	/* INCLUDED_RELOCATE_H */

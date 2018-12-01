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

/*typedef	U32	(*fReloc)( void * apData, const U32 aSize, const U32 aID );*/
typedef	U32	(*fReloc)( sAssetItem * apAsset );


/* ###################################################################################
#  STRUCTS
################################################################################### */

enum
{
	eRELOC_FUNCTION_IsType,
	eRELOC_FUNCTION_DoInit,
	eRELOC_FUNCTION_DoDeInit,
	eRELOC_FUNCTION_DoRelocate,
	eRELOC_FUNCTION_DoDelocate,

	eRELOC_FUNCTION_LIMIT
};

typedef	struct sRelocater
{
	U32					mExtID;
	fReloc				IsType;
	fReloc				DoInit;
	fReloc				DoDeInit;
	fReloc				DoRelocate;
	fReloc				DoDelocate;
	fReloc				mFunctions[ eRELOC_FUNCTION_LIMIT ];
	struct sRelocater *	mpNext;
} sRelocater;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void			Relocater_Init( sRelocater * apReloc, const char * apExt, fReloc aIsType, fReloc aDoInit, fReloc aDoDeInit, fReloc aDoRelocate, fReloc aDoDelocate );
void			Relocater_DeInit( sRelocater * apReloc );

sRelocater *	RelocaterManager_Find( sAssetItem * apAsset );

U32				RelocaterManager_DoInit(     sAssetItem * apAsset );
U32				RelocaterManager_DoDeInit(   sAssetItem * apAsset );
U32				RelocaterManager_DoDelocate( sAssetItem * apAsset );
U32				RelocaterManager_DoRelocate( sAssetItem * apAsset );

void			RelocaterManager_Init( void );
void			RelocaterManager_DeInit( void );


/* ################################################################################ */

#endif	/* INCLUDED_RELOCATE_H */
